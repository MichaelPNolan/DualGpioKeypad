//LED TEST w/ 74HC595
//by Amanda Ghassaei 2012
//https://www.instructables.com/id/Multiplexing-with-Arduino-and-the-74HC595/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

//this code will light up each led in the 4x4 matrix one at a time


//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin A1
#define clockPin A0
#define dataPin A2

//looping variables
static byte i;
byte j;
byte x;
short ledRows[4]; //hold 4 binary maps of leds on/off to use per scan
//storage variable
byte dataToSend[2];
short dataToBuild;
void setupLEDplex() {
  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  ledAllOff();
}
void ledAllOff(){
    for (i=0;i < 4;i++){
      ledRows[i] = 0;
  }
}
void ledMatrixOn(byte x){
      j = x / 4;
      i = x % 4;
      ledRows[i] |= (1 << j+2);
}

void ledMatrixOff(byte x){
      j = x / 4;
      i = x % 4;
      ledRows[i] &= (0 << j+2);
}
void processLEDplex() {
   i++;
   if(i > 3)
     i = 0;
  
      //bit manipulation (more info at http://arduino.cc/en/Reference/Bitshift ,  http://arduino.cc/en/Reference/BitwiseXorNot , and http://arduino.cc/en/Reference/BitwiseAnd)
       //
       //dataToBuild =  (1 << (i+12)) | (4092 & ~(1 << j+2)); //there are 10 lines to fit into 12 bits of the j (cols) and 4 bits in the least bits (rows)
   if(ledRows[i]){
     dataToBuild =  (1 << (i+12)) | (4092 & ~ledRows[i]);
     dataToSend[1] = (byte)(dataToBuild & 0xff);
     dataToSend[0] = (byte)((dataToBuild >> 8) & 0xff);
     //convert short into two bytes
     //ret[0] = (byte)(x & 0xff);
     //ret[1] = (byte)((x >> 8) & 0xff);
    //dataToSend[0] = (1 << (i+4)) | (15 & ~(1 << j));//preprare byte (series of 8 bits) to send to 74HC595
  
    //for example when i =2, j = 3,
    //dataToSend = (1 << 6) | (15 & ~(1 << 3));
    //dataToSend = 01000000 | (15 & ~(00001000));
    //dataToSend = 01000000 | (15 & 11110111);
    //dataToSend = 01000000 | (15 & 11110111);
    //dataToSend = 01000000 | 00000111;
    //dataToSend = 01000111;
    //the first four bits of dataToSend go to the four rows (anodes) of the LED matrix- only one is set high and the rest are set to ground
    //the last four bits of dataToSend go to the four columns (cathodes) of the LED matrix- only one is set to ground and the rest are high
    //this means that going through i = 0 to 3 and j = 0 to three with light up each led once
    #ifndef MIDI_SERIAL_MODE //for testing purposes to confirm the binary math worked
/*    Serial.print("Row: "+String(i));
    printBinShort(ledRows[i]);
    Serial.println();
    Serial.print(" First: ");
    printBinByte(dataToSend[1]);
    Serial.print(" 2nd: ");
    printBinByte(dataToSend[0]);
    Serial.println(); */
    #endif 
    // setlatch pin low so the LEDs don't change while sending in bits
    digitalWrite(latchPin, LOW);
    // shift out the bits of dataToSend to the 74HC595
    shiftOut(dataPin, clockPin, LSBFIRST, dataToSend[1]);
   // digitalWrite(latchPin, HIGH);
   // digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, dataToSend[0]);
    //set latch pin high- this sends data to outputs so the LEDs will light up
    digitalWrite(latchPin, HIGH);
  }
  //delay(500);
}

void printBinByte(byte n){
 for(int i=0; i<8;i++) {
    if (n & 1)
        Serial.print("1");
    else
        Serial.print("0");

    n >>= 1;
  } 
}

void printBinShort(short  n){
  for(int i=0; i<16;i++) {
    if (n & 1)
        Serial.print("1");
    else
        Serial.print("0");

    n >>= 1;
  } 
}
