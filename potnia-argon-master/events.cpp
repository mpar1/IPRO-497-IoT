#define LIGHT_PIN A0 // Replace A0 with pin that light sensor is connected to 
#define LIGHT_LEVEL_THRESHOLD 800 // Replace with minimum amount of light needed 
#define WATER_LEVEL_PIN A1 // Replace A1 with water sensor pin  
#define WATER_LEVEL_THRESHOLD 100 // Replace with your own light level  
#define TEMP_PIN A2 // Replace A2 with pin that light sensor is connected to
#define TEMP_LEVEL_MIN 65 //
#define TEMP_LEVEL_MAX 85 //These two temperatures are prefered by most plants
#define THRESHOLD_SENSITIVITY 50 // higher number = lower sensitivity, lower number = higher sensitivity 

void setup() {
 
} 
void loop() 
{ 
    lightFunc();
    waterFunc();
    tempFunc();
}
  

void lightFunc()
{
    // Reading the light sensor to see if it is sunny enough for the plant.
    
    // value decreases as brightness increases 
    int crntLight = analogRead(LIGHT_PIN); 
    
    if (crntLight >= LIGHT_LEVEL_THRESHOLD) 
    { 
        Particle.publish("light-level-good", "bright"); 
        
        // Once dark is triggered, the threshold is temporarily shifted so that the state will not float
        // between dark and bright when it's on the threshold. This is how street lights work.
        while (analogRead(LIGHT_PIN) > LIGHT_LEVEL_THRESHOLD - THRESHOLD_SENSITIVITY); 
    } 
    else 
    { 
        Particle.publish("light-level-bad", "dark"); 
    }

} 

void waterFunc()
{
     // Reading the water sensor to see if the plant has enough water
    int waterLevel = analogRead(WATER_LEVEL_PIN);  
    
    if (waterLevel >= WATER_LEVEL_THRESHOLD) 
    {  
        Particle.publish("water-level", "high");  
    
        // This loop is to keep the reading from bouncing around
        // Once the sensor sees the water to be at a high level
        // we lower the number it senses to see if changes again
        while (analogRead(WATER_LEVEL_PIN) > WATER_LEVEL_THRESHOLD - THRESHOLD_SENSITIVITY);  
    } 
    else 
    {  
        Particle.publish("water-level", "low");  
    }  

}

void tempFunc()
{
    // Set the sensor reading into a variable
    int temperature = analogRead(TEMP_PIN);

    // If the temperature is too high
    if (temperature > TEMP_LEVEL_MAX)
    {
        Particle.publish("temperature", "high");
    }
    else if(temperature < TEMP_LEVEL_MIN) //If the temperature is too low
    {
        Particle.publish("temperature", "low");
    }
}


