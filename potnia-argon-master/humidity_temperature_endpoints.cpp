// This #include statement was automatically added by the Particle IDE.
#include <I2CSoilMoistureSensor.h>

double TEMPERATURE_MIN = 65 
double TEMPERATURE_MAX = 85
double SOILMOISTURE_MIN = 0.41 // minimum 41% moisture
double SOILMOISTURE_MAX = 0.8 // maximum 80% moisture

// plants prefer temperatures between 65 and 80 degrees fahrenheit
// plants need somewhere between 41% - 80% moisture
// CODE TEMPLATED FROM: https://github.com/VintageGeek/I2CSoilMoistureSensor

// constructor initializing library object with default address (0x20)
I2CSoilMoistureSensor i2CSoilMoistureSensorDefAddr;

void setup() {
    
    // i2c communication with wire
    Wire.setSpeed(300000);
    if (!Wire.isEnabled()) {
        Wire.begin();
    }
    
    // initializing library object to begin calls of functions
    i2CSoilMoistureSensorDefAddr.begin();
    delay(3000);
}

void loop() {
    
    // moisture value
    while (i2CSoilMoistureSensorDefAddr.isBusy()) delay(50);
        int moisture = i2CSoilMoistureSensorDefAddr.getCapacitance();
        delay(1000);
    
    // temperature value
    while (i2CSoilMoistureSensorDefAddr.isBusy()) delay(50);
        int temperature = i2CSoilMoistureSensorDefAddr.getTemperature();
        delay(1000);
        
    // temperature value is in degrees Celsius times 10, so div by 10 to get real celsius value
    float celsius = (temperature/(float)10);
    
    // fahrenheit conversion from celsius
    float fahrenheit = (celsius*(9/5)) + 32;
    
    // converting moisture value to percentage
    int moisturePercent = moisture / 100;
    
    // if moisture level is too high
    if (moisturePercent > SOILMOISTURE_MAX) {
        Particle.publish("soilMoisture", "Soil Moisture is TOO WET! Soil Moisture Level: " +String(moisturePercent*100)+"%");
        delay(10000);
    // if moisture level is too low
    } else if (moisturePercent < SOILMOISTURE_MIN) {
        Particle.publish("soilMoisture", "Soil Moisture is TOO DRY! Soil Moisture Level: " +String(moisturePercent*100)+"%");
        delay(10000);
    }
    
    // if temperature is too high
    if (temperature > TEMPERATURE_MAX) {
        Particle.publish("tempFaherenheit", "Temperature is TOO HOT! Temperature in Fahrenheit: " +String(fahrenheit));
        delay(10000);
    // if temperature is too low    
    } else if (temperature < TEMPERATURE_MIN) {
        Particle.publish("tempFaherenheit", "Temperature is TOO COLD! Temperature in Fahrenheit: " +String(fahrenheit));
        delay(10000);
    }    
    
    // resets sensors
    i2CSoilMoistureSensorDefAddr.resetSensor();

    // puts device to sleep
    i2CSoilMoistureSensorDefAddr.sleep();
}