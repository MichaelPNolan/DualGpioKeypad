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


//#include "MIDIUSB.h"   if i want to use a USB enabled controller - future use

//#define MIDI_SERIAL_MODE
long counter = 0;

void setup(){
  #ifdef MIDI_SERIAL_MODE    
  Serial.begin(31250);
  #else
  Serial.begin(9600);
  Serial.println("serial ready");
  #endif

  setupLEDplex();
  setupKeyboard();
  //scan();
  

}
  
void loop(){
 //serviceKeyboardMatrix();
 if(counter > 12000){
   
   ledAllOff();
   for(int i=0;i<10;i++)
     ledMatrixOn(random(40));

   //ledMatrixOff(random(40));
   counter = 0;
 } else {
   counter++; 
   //delay(1);
 }
 if(counter % 20)
   processLEDplex();
// if(counter%2000)
  //  ledMatrixOff(random(40));

 
}

void playMIDINote(byte channel, byte note, byte velocity)
{
    //MIDI channels 1-16 are really 0-15
    if(channel == 0)
      channel = 1;
    byte noteOnStatus = 0x90 + (channel-1);

    //Transmit a Note-On message
    #ifdef MIDI_SERIAL_MODE
    Serial.write(noteOnStatus);
    Serial.write(note);
    Serial.write(velocity);
    #endif
} 

void playMIDINoteOff(byte channel, byte note){
    //MIDI channels 1-16 are really 0-15
    // Option 1 write velocity 0 noteOn
    if(channel == 0)
      channel = 1;
    byte noteOnStatus = 0x90 + (channel-1);
    
    #ifdef MIDI_SERIAL_MODE
    Serial.write(noteOnStatus);
    Serial.write(note);
    Serial.write(0); //basically write 0 velocity = off
    #endif
    /*  
    byte noteOffStatus = 0x80 + (channel-1);  //option 2 do a note off command
    #ifdef MIDI_SERIAL_MODE
    Serial.write(noteOffStatus);
    Serial.write(note);
    #endif */
}
