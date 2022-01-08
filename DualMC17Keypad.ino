//This is a midi controller buildout of an old Casio Keyboard
//Initial repair of the corroded PCB for keyboard part A took a long time
//I used pencil LED to refurb parts of the carbon contacts or even drilled holes to re-establish a dead track.
//I created a contact pad with bits of wire where one was too damaged and arrived at something that seems to work
//The PCB for keyboard part B is longer - part A was for a chord controller it seems so that had its own set of rows and columns
//I wired it to GPIO expansion as I had used in another keyboard stand-alone but in this case my goal is to make a Midi keyboard
//There are many other buttons from the old design that I hope to map if I can figure out more of the switch arrangements

/* Multikey_MC17 - 16-bit I2C port conversion of Keypad's Multikey
:: G. D. (Joe) Young Feb 12, 2013
:: MC23017 version - GDY May 19, 2014

|| #8 gap wong
*/

#include <Keypad.h>
#include <Keypad_MC17.h>    // I2C i/o library for Keypad
#include <Keypad_MC17b.h>
#include <Wire.h>           // I2C library for Keypad_MC17

#define I2CADDR_KPDA 0x20        // address of MCP23017 chip on I2C bus
#define I2CADDR_KPDB 0x22        // address of MCP23017 chip on I2C bus


const byte ROWS_A = 6; //6 ROWS_A
const byte COLS_A = 3; //3 columns on the smaller A section pcb
//define the cymbols on the buttons of the keypads
byte keys_A[ROWS_A][COLS_A] = { 
  {0,6,12},     //C1,__,C2
  {1,7,13},     //C1#,__,C#2
  {2,8,14},     //D1,__,
  {3,9,15},     //D1#,
  {4,10,16},     //E1,
  {5,11,17}      //F1,
};

byte rowPins_A[ROWS_A] = {3,4,5,6,7,8};  //connect to the row pinouts of the keypad
byte colPins_A[COLS_A] = {0,1,2}; //connect to the column pinouts of the keypad
byte rowVal_A[ROWS_A];
byte prevRowVal_A[ROWS_A];

const byte ROWS_B = 6; //6 ROWS_A
const byte COLS_B = 7; //7 columns on the B section
//define the cymbols on the buttons of the keypads
byte keys_B[ROWS_B][COLS_B] = { 
  {24,30,36,42,48,54,60},     //F2,__,C2
  {25,31,37,43,49,55,61},     //F2#,__,C#2
  {26,32,38,44,50,56,62},     //G2,__,
  {27,33,39,45,51,57,63},     //G2#,
  {28,34,40,46,52,58,64},     //A2,
  {29,35,41,47,53,59,65}      //A2#,
};

byte rowPins_B[ROWS_B] = {7,8,9,10,11,12};  //connect to the row pinouts of the keypad
byte colPins_B[COLS_B] = {0,1,2,3,4,5,6}; //connect to the column pinouts of the keypad
byte rowVal_B[ROWS_B];
byte prevRowVal_B[ROWS_B];

// modify constructor for I2C i/o
Keypad_MC17 kpdA( makeKeymap(keys_A), rowPins_A, colPins_A, ROWS_A, COLS_A, I2CADDR_KPDA );  //uses a GPIO Expander A
Keypad_MC17b kpdB( makeKeymap(keys_B), rowPins_B, colPins_B, ROWS_B, COLS_B, I2CADDR_KPDB );  ////uses a GPIO Expander B

float volumeParam = 1.0f;
float semiModifier = 0.5f;
bool noNote = 0;


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
void setup(){
  Serial.begin(9600);
  Serial.println("serial ready");
 
  setupKeyboard();
  //scan();
  

}
  
void loop(){
  byte customKeyA = kpdA.getKey();
  
  
  if (customKeyA){
    Serial.println(String(customKeyA));
  }
  delay(1);  
  byte customKeyB = kpdB.getKey();
  if (customKeyB){
    Serial.println(String(customKeyB));
  }
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
