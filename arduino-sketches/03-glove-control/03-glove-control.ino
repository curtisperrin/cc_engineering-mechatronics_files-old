#include <PCF8574.h> // imports the PCF8574 library See https://github.com/RobTillaart/PCF8574

#include "solenoid.h" // imports the header files for the solenoid and gloves
#include "glove.h"

#define LEFT_OUTER_KNEE_S     5
#define LEFT_OUTER_HIP_S      4


// Each of these corresponds to one of the bits within the byte read by the PCF8574 chip. Each bit represents the state (pressed or not) of a button
#define LEFT_OUTER_KNEE_B     3
#define LEFT_OUTER_HIP_B      4


// Flex sensor resistance calibration
#define LEFT_OUTER_KNEE_C     240
#define LEFT_OUTER_HIP_C      200


// Pre-processor directive that inverts the knees (regular flex = retract, inverted flex = extend). Comment out lines to not invert. See https://www.tutorialspoint.com/cplusplus/cpp_preprocessor.htm
//#define INVERT_KNEES
//#define INVERT_HIPS 

// See https://github.com/RobTillaart/PCF8574 for documentation on this library
PCF8574 pcf(0x20); // it will always be 0x20 as it's the default address for the chip in our application

// Place to store the incoming byte from the above pcf
int incomingByte = 0;

Solenoid s_left_outer_knee(LEFT_OUTER_KNEE_S);
Solenoid s_left_outer_hip(LEFT_OUTER_HIP_S);

Glove g_left_outer_knee(3);
Glove g_left_outer_hip(4);

void setup() // runs once when the microcontroller starts to setup various things needed
{
  Serial.begin(9600); // Start the serial port running at 9600 baud

  // call the setup functions for the pcf, solenoid, and glove classes
  pcf.begin();
  Glove::begin();
  s_left_outer_knee.begin();
  s_left_outer_hip.begin();
 
}

void solenoidLoops() // solenoid loop: calls the loop functions in each instance of the solenoid class
{
  s_left_outer_knee.loop();
  s_left_outer_hip.loop();
}

void gloveLoops() // glove loop: calls the loop functions in each instance of the glove class
{
  g_left_outer_knee.loop();
  g_left_outer_hip.loop();
}

void toggle(){

  // See https://github.com/RobTillaart/PCF8574 for documentation on this library
  byte inputStates = pcf.readButton8(); // Will read each input as a single byte

//first check if button is pressed
if(!bitRead(inputStates, LEFT_OUTER_KNEE_B)) // bitRead Isolates a bit from the byte. See https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/bitread/
  s_left_outer_knee.extend();
else // Read glove
  #ifdef INVERT_KNEES // pre-processor conditional to invert the functionality of the knee https://en.cppreference.com/w/cpp/preprocessor/conditional
    if(g_left_outer_knee.isFlexed(LEFT_OUTER_KNEE_C)) 
      s_left_outer_knee.retract();
    else
      s_left_outer_knee.extend();
  #else // knees aren't inverted
    if(g_left_outer_knee.isFlexed(LEFT_OUTER_KNEE_C)) 
      s_left_outer_knee.extend();
    else
      s_left_outer_knee.retract();   
  #endif
if(!bitRead(inputStates, LEFT_OUTER_HIP_B))
  s_left_outer_hip.extend();
else
  #ifdef INVERT_HIPS //if hips are inverted compile the following code
    if(g_left_outer_hip.isFlexed(LEFT_OUTER_HIP_C))
      s_left_outer_hip.retract();
    else
      s_left_outer_hip.extend();
  #else //Hips aren't inverted
   if(g_left_outer_hip.isFlexed(LEFT_OUTER_HIP_C))
      s_left_outer_hip.extend();
   else
      s_left_outer_hip.retract();
  #endif

}

// Loop runs continuously when microcontroller is running  
void loop() 
{
  solenoidLoops();
  gloveLoops();
  toggle();
  delay(100); // We only want to update it every 100ms to prevent it from being too jumpy
}
