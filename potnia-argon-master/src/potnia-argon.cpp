#include "application.h"
#include "Adafruit_seesaw.h"

// **** Pin definitions ****
int lightSensor3V3 = A4;
int lightSensor = A5;


// **** structs ****
struct rawData {
  float tempC;
  uint16_t capacitance;
  int lightLevel;
  int batteryIn;
};

// this is a struct instead of #defs so users can eventually change it
struct calibration { 
  uint16_t dryAirHumidity; // capacitance reading reported in dry air
  uint16_t waterHumidity; // capacitance reading reported when submerged in water
};

struct readyData {
  float tempF;
  int normLightLevel;
  int moistureLevel;
  float batteryVoltage;
};

// **** Globals ****
Adafruit_seesaw stemma;
char* error;
struct calibration usercal;

// **** Functions ****
void setup();
void loop();
void blinkOnOff(int, int, int);
void takeMeasurements(struct rawData *);
void normalize(struct rawData *, struct calibration *, struct readyData *);
void processing(struct rawData *, struct calibration *, struct readyData *);
void upload(struct readyData *);


void setup() {
  usercal = { // experimentally determined
    .dryAirHumidity = 330,
    .waterHumidity = 615
  };

  pinMode(D7, OUTPUT); // status LED

  pinMode(lightSensor3V3, OUTPUT);
  digitalWrite(lightSensor3V3, HIGH);

  Serial.begin(11200);

  if (!stemma.begin(0x36)) {
    error = "Couldn't initialize stemma!";
  }
}

void loop() {
  if (error) {
    blinkOnOff(D7, 50, 20);
    Serial.println(error);
    // TODO: report this error to the cloud
    return;
  }

  blinkOnOff(D7, 500, 1); // "alive and well" blink

  struct rawData readings;
  takeMeasurements(&readings);

  struct readyData output;
  normalize(&readings, &usercal, &output);

  // passing rawData, usercalibration (dry/water humidity), and readyData to processing function
  processing(&readings, &usercal, &output);

  upload(&output);

  System.sleep(D7, RISING, 900); // sleep for 15m. Restart from setup on resume
}

/*
 * Blink a pin on and off `count` times, taking `duration` ms per transition.
 * TODO: consider using the RGB LED for error states instead:
 * https://docs.particle.io/reference/device-os/firmware/argon/#led-signaling
 */
void blinkOnOff(int pin, int duration, int count) {
  for (int x=0; x<count; x++) {
    digitalWrite(pin, HIGH);
    delay(duration);
    digitalWrite(pin, LOW);
    delay(duration);
  }
}

void takeMeasurements(struct rawData * data) {
  data->tempC = stemma.getTemp();
  data->capacitance = stemma.touchRead(0);
  data->lightLevel = analogRead(lightSensor);
  data->batteryIn = analogRead(BATT);
}

void normalize(struct rawData* data, struct calibration* calib, struct readyData* output) {
  // how do we normalize light level? should be linear with respect to some measurable quantity
  output->normLightLevel = data->lightLevel;

  // this will probably need some experimental calibration with a thermometer since the sensor sucks
  output->tempF = data->tempC*1.8 + 32;

  // again, not sure what should be done here. Linearly mapped to a % humidity.
  output->moistureLevel = map(data->capacitance, calib->dryAirHumidity, calib->waterHumidity, 0, 100);

  // Battery ADC is between 806k and 2M resistors.  Computed constant. Source:
  // https://github.com/kennethlimcp/particle-examples/blob/master/vbatt-argon-boron/vbatt-argon-boron.ino
  output->batteryVoltage = (float) data->batteryIn * 0.0011224;
}

void processing(struct rawData* data, struct calibration* calib, struct readyData* output) {

  // if temperature is less than 65, publish "Temperature is too cold!"
  // else if temperature is greater than 85, publish "Temperature is too hot!"
  char tempStr[6];
  snprintf(tempStr, 6, "%.2f", output->tempF);
  if (output->tempF < 65) {
    Particle.publish("tempMin", tempStr, PRIVATE);
  } else if (output->tempF > 85) {
    Particle.publish("tempMax", tempStr, PRIVATE);
  }

  // if moisture level is less than 330, publish "Soil Moisture is too dry!"
  // else if moisture level is greater than 615, publish "Soil Moisture is too wet!"
  char moistureStr[5];
  snprintf(moistureStr, 5, "%.4d", output->moistureLevel);
  if (output->moistureLevel < calib->dryAirHumidity) {
    Particle.publish("moistureMin", moistureStr, PRIVATE);
  } else if (output->moistureLevel >  calib->waterHumidity) {
    Particle.publish("moistureMax", moistureStr, PRIVATE);
  }

  // if light level is less than 400, publish "Plant needs more light!"
  char lightStr[5];
  snprintf(lightStr, 5, "%.4d", output->normLightLevel);
  if (output->normLightLevel < 400) {
    Particle.publish("lightMin", lightStr, PRIVATE);
  }

  // if battery level is less than 1, publish "Battery level is very low!"
  char battVoltageStr[5];
  snprintf(battVoltageStr, 5, "%.2f", output->batteryVoltage);
  if (output->batteryVoltage < 1) {
    Particle.publish("batteryMin", battVoltageStr, PRIVATE);
  }
}

// changed readyData* data -> readyData* output to be consistent with the rest of code
void upload(struct readyData* output) {
  Serial.print("Temperature: ");
  Serial.print(output->tempF);
  Serial.println("F");
  Serial.print("Light level: ");
  Serial.print(output->normLightLevel);
  Serial.println("/4096");
  Serial.print("Humidity: ");
  Serial.print(output->moistureLevel);
  Serial.println("/100");
  Serial.print("Battery: ");
  Serial.print(output->batteryVoltage);
  Serial.println("V");

  Serial.println();

  char battVoltageStr[5];
  snprintf(battVoltageStr, 5, "%.2f", output->batteryVoltage);
  Particle.publish("battery", battVoltageStr, PRIVATE); // TODO: handle errors when publishing

  char tempStr[6];
  snprintf(tempStr, 6, "%.2f", output->tempF);
  Particle.publish("temperature", tempStr, PRIVATE);

  char lightStr[5];
  snprintf(lightStr, 5, "%.4d", output->normLightLevel);
  Particle.publish("light", lightStr, PRIVATE);

  char moistureStr[5];
  snprintf(moistureStr, 5, "%.4d", output->moistureLevel);
  Particle.publish("moisture", moistureStr, PRIVATE);

  delay(5000); // wait a bit to make sure these actually get published before sleeping
  Serial.println("Published Particle events.");
}