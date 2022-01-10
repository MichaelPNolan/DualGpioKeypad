/* Multikey_MC17 - 16-bit I2C port conversion of Keypad's Multikey
:: G. D. (Joe) Young Feb 12, 2013
:: MC23017 version - GDY May 19, 2014
::
|| @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/

#include <Keypad.h>
#include <Key.h>
#include <Keypad_MC17.h>    // I2C i/o library for Keypad
#include <Wire.h>           // I2C library for Keypad_MC17
#define I2CADDR_KPDA 0x20        // address of MCP23017 chip on I2C bus
#define I2CADDR_KPDB 0x22        // address of MCP23017 chip on I2C bus

//typedef enum{ IDLE, PRESSED, HOLD, RELEASED } KeyState;
const byte ROWS_A = 6; //6 ROWS_A
const byte COLS_A = 3; //3 columns on the smaller A section pcb
const int LIST_MAXA = ROWS_A*COLS_A;
byte rowPins_A[ROWS_A] = {3,4,5,6,7,8};  //connect to the row pinouts of the keypad
byte colPins_A[COLS_A] = {0,1,2}; //connect to the column pinouts of the keypad
//define the cymbols on the buttons of the keypads
uint8_t keys_A[ROWS_A][COLS_A] = { 
  {12,18,24},     //C1,__,C2
  {13,19,25},     //C1#,__,C#2
  {14,20,26},     //D1,__,
  {15,21,27},     //D1#,
  {16,22,28},     //E1,
  {17,23,29}      //F1,
};

const byte ROWS_B = 6; //6 ROWS_A
const byte COLS_B = 7; //7 columns on the B section
const int LIST_MAXB = ROWS_B*COLS_B;
byte rowPins_B[ROWS_B] = {7,8,9,10,11,12};  //connect to the row pinouts of the keypad
byte colPins_B[COLS_B] = {0,1,2,3,4,5,6}; //connect to the column pinouts of the keypad
//define the cymbols on the buttons of the keypads
uint8_t keys_B[ROWS_B][COLS_B] = { 
  {24,30,36,42,48,54,60},     //F2,__,C2
  {25,31,37,43,49,55,61},     //F2#,__,C#2
  {26,32,38,44,50,56,62},     //G2,__,
  {27,33,39,45,51,57,63},     //G2#,
  {28,34,40,46,52,58,64},     //A2,
  {29,35,41,47,53,59,65}      //A2#,
};
float semiModifier = 0.6f; 
byte volumeParam = 64;
bool noNote = false;

// modify constructor for I2C i/o
Keypad_MC17 kpdA( makeKeymap(keys_A), rowPins_A, colPins_A, ROWS_A, COLS_A, I2CADDR_KPDA );  //uses a GPIO Expander A
Keypad_MC17 kpdB( makeKeymap(keys_B), rowPins_B, colPins_B, ROWS_B, COLS_B, I2CADDR_KPDB );  ////uses a GPIO Expander B

unsigned long loopCount = 0;
unsigned long startTime = millis();
String msg = "";
uint8_t  keyMod = 40; //the range of value that can be added to the input note number to determine played note


void setupKeyboard() {
    //for USB serial switching boards
  Wire.begin( );
  // Set the address 0,0,0 using pins 6,7,8 on gpioExpander 23017
 /* pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW); */
  kpdA.begin( );                // now does not starts wire library
  kpdB.begin( );
  kpdA.setDebounceTime(1);
  kpdB.setDebounceTime(1);
}

// to receive changes from controller adc input see adc_module
void keyboardSetVolume(byte value)
{
  volumeParam = value;
}

byte getKeyboardVolume(){
  return volumeParam;
}

void keyboardSetSemiModifier(float value)
{
  semiModifier = value;
}
// for more functionality - we have all the keyboard keys to use - so we check the modifier bank button
void serviceKeyboardMatrix() {
  if (kpdA.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.  LIST_MAX
    {
      if ( kpdA.key[i].stateChanged )   // Only find keys that have changed state.
      {
        if (uint8_t(kpdA.key[i].kchar) > 0){ //some possible/unused keys were mapped to 0 in the keyboard char array to prevent output
         keyToNote(byte(kpdA.key[i].kchar),kpdA.key[i].kstate); // this may fix my bad logic - move arpeggiator test into keytoNote
        }
      }
    }
  }

  if (kpdB.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.  LIST_MAX
    {
      if ( kpdB.key[i].stateChanged )   // Only find keys that have changed state.
      {
        if (uint8_t(kpdB.key[i].kchar) > 0){ //some possible/unused keys were mapped to 0 in the keyboard char array to prevent output
         keyToNote(byte(kpdB.key[i].kchar),kpdB.key[i].kstate); // this may fix my bad logic - move arpeggiator test into keytoNote
        }
      }
    }
  }
  
}




void keyToNote(byte keyUS,byte kStatus){  //kStatus is type KeyStatus but this doesn't seem to be accessible outside the library so i passed as a byte
  keyUS += keyMod*semiModifier; //semiModifier is set in ADC from one of the pots to a 1.0f value and keyMod is whatever you want as adjust range
  switch (kStatus) {  // Report active keystate based on typedef enum{ IDLE, PRESSED, HOLD, RELEASED } KeyState;
      case PRESSED:
          playMIDINote(1, keyUS, volumeParam);
          msg = " PRESSED.";
          break;
      case HOLD:
          msg = " HOLD.";

          break;
      case RELEASED:
          msg = " RELEASED.";
          playMIDINoteOff(1, keyUS);
          break;
      case IDLE:    // there are times when idle needs to be calling noteOff because you had notes on hold in arpeggiator
          msg = " IDLE.";

  }
  #ifndef MIDI_SERIAL_MODE   
  Serial.print("Key :/");//+String(LIST_MAX));
  Serial.print(keyUS);
  Serial.println(" "+msg);
  #endif
}


//----------------------------------
void scan() { //spi bus scan for devices
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
