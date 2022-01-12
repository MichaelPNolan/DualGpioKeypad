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
byte plexRow;
byte i;
byte j;
byte x;
bool sweepEffect, KnightRider, KR2;
byte sweepInc;



short ledRows[4]; //hold 4 binary maps of leds on/off to use per scan
//storage variable
byte dataToSend[2];
short dataToBuild;
void setupLEDplex() {
  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  timer1_counter = 65011;
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);

  interrupts();
  ledAllOff();
  sweepEffect = false;
  KnightRider = false;
  KR2 = false;
  plexRow = 0;
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
   TCNT1 = timer1_counter;   // preload timer
  
   plexRow++;
   if(plexRow > 3)
     plexRow = 0;
   if(ledRows[plexRow]){ //light all the columns set for this row
     dataToBuild =  (1 << (plexRow+12)) | (4092 & ~ledRows[plexRow]);
     dataToSend[1] = (byte)(dataToBuild & 0xff);  //convert short into two bytes
     dataToSend[0] = (byte)((dataToBuild >> 8) & 0xff);
   }
    digitalWrite(latchPin, LOW);
    // shift out the bits of dataToSend to the 74HC595
    shiftOut(dataPin, clockPin, LSBFIRST, dataToSend[1]);
   // digitalWrite(latchPin, HIGH);
   // digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, dataToSend[0]);
    //set latch pin high- this sends data to outputs so the LEDs will light up
    digitalWrite(latchPin, HIGH);
}

void effects(){
  
  if(sweepEffect)
  {
     if(sweepInc >39){
       sweepEffect = false;
       ledAllOff();
     }
     ledMatrixOn(sweepInc);
     ledMatrixOn(sweepInc+1);
     if(sweepInc>0)
       ledMatrixOff(sweepInc-1);
     sweepInc++;
  }

  if(KnightRider){

     if((sweepInc >1)&& (sweepInc<19)){
       ledMatrixOff(19+sweepInc);
       ledMatrixOff(20-sweepInc);
     }
     if(sweepInc >19){
       ledMatrixOff(68-sweepInc);
       ledMatrixOff(sweepInc-19);
     }
     sweepInc++;
     if(sweepInc <19){
       ledMatrixOn(20+sweepInc);
       ledMatrixOn(21-sweepInc);
     }
     if(sweepInc >=19){
       ledMatrixOn(68-sweepInc);
       ledMatrixOn(sweepInc-19);
     }
  
     if(sweepInc >42){
       KnightRider = false;
       ledAllOff();
    }
  }
}

void sweepEffectStart(){
  sweepEffect = true;
  sweepInc = 1;
}
void KnightRiderStart(){
  KnightRider = true;
  sweepInc = 1;
}

void ledAllOff(){  //clear all the column bits in all the rows
    for (i=0;i < 4;i++){
      ledRows[i] = (short)0;
      //printBinShort(ledRows[i]);
     //Serial.println();
  }

}
void ledMatrixOn(byte x){ //set the bit for this column j in the correct row i
  if(x < 41){
    if(x < 4)
      j = 0;
    else
      j = x / 4;
    i = x % 4;
    ledRows[i] |= (1 << j+2);
  }
 /* Serial.print(x);
  Serial.print(" on i=");
  Serial.print(i);
  Serial.print(" j=");
  Serial.print(j);
  Serial.print(" "); */
}

void ledMatrixOff(byte x){ //clear the bit for this column j in the correct row i
  if(x < 41){
    if(x < 4)
      j = 0;
    else
      j = x / 4;
    i = x % 4;
    ledRows[i] &= (0 << j+2);
  }
  //Serial.print(x);
  //Serial.println(" off ");
}
void processLEDplex() {
    //each time we are called process a different row - multiplexing
   plexRow++;
   if(plexRow > 3)
     plexRow = 0;
   
      //bit manipulation (more info at http://arduino.cc/en/Reference/Bitshift ,  http://arduino.cc/en/Reference/BitwiseXorNot , and http://arduino.cc/en/Reference/BitwiseAnd)
       //
       //dataToBuild =  (1 << (i+12)) | (4092 & ~(1 << j+2)); //there are 10 lines to fit into 12 bits of the j (cols) and 4 bits in the least bits (rows)
   if(ledRows[plexRow]){ //light all the columns set for this row
     dataToBuild =  (1 << (plexRow+12)) | (4092 & ~ledRows[plexRow]);
     dataToSend[1] = (byte)(dataToBuild & 0xff);  //convert short into two bytes
     dataToSend[0] = (byte)((dataToBuild >> 8) & 0xff);
   }
  /* else
   {
     dataToBuild =  4092;
     dataToSend[1] = (byte)(dataToBuild & 0xff);  //convert short into two bytes
     dataToSend[0] = (byte)((dataToBuild >> 8) & 0xff);
   }*/
    //Original example code that was in nested for loops for i and j in a 4 x 4 matrix
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
  

  // delay(1);
  
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
