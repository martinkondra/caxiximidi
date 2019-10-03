// VERSION RECEIVER MODULAR
/// SAMPLER OLD SCHOOLL -> Corte de loop en Inicio de sample
//// MIDI SELECCIONABLE -> Seleccionar librerias y funciones de envio de notas segun corresponda
///// FOR CCM OCTAVES, check SplitCCM() 


//// -MIDI- (SELECT)

////OPCION 1 MIDI USB NATIVO/////
#include "MIDIUSB.h"
////FIN OPCION 1 MIDI NATIVO/////

////OPCION 2 VIRTUAL MIDI PORT///
/*
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
struct MyMidiSettings : public midi::DefaultSettings {
  static const long DefaultSettings::BaudRate = 9600;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MyMidiSettings);
boolean midiMode = false;
*/
////FIN OPCION 2 VIRTUAL MIDI PORT///

//// -COMUNICATION- 
#include <SPI.h>
#include "RF24.h"
#include "nRF24config.h"

//// -CAXIXI
#include "receiverConfig.h"
#include "CxCircularBuffer.h"
char inData[10];
int index;
boolean started = false;
boolean ended = false;
int inInt;
int ch;
int num;
///RECEIVED MESSAGES
int cxRightForwardNote = CAXIXI_RIGHT_NOTE_FORWARD;
int cxRightBackwardNote = CAXIXI_RIGHT_NOTE_BACKWARD;
int cxRightHitNote = CAXIXI_RIGHT_NOTE_HIT;
int cxLeftForwardNote = CAXIXI_LEFT_NOTE_FORWARD;
int cxLeftBackwardNote = CAXIXI_LEFT_NOTE_BACKWARD;
int cxLeftHitNote = CAXIXI_LEFT_NOTE_HIT;
int currentOctave = 0;
#include "leds.h"
#include "Sampler.h"

void setup() {
  ///PINES, initialize the LED as an output:
  pinMode(RECORD_LED_PIN, OUTPUT);
  pinMode(OCTAVE_UP_LED_RED_PIN, OUTPUT);
  pinMode(OCTAVE_UP_LED_GREEN_PIN, OUTPUT);
  pinMode(OCTAVE_DOWN_LED_RED_PIN, OUTPUT);
  pinMode(OCTAVE_DOWN_LED_GREEN_PIN, OUTPUT);
  analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
  analogWrite(OCTAVE_DOWN_LED_GREEN_PIN, 0);
  analogWrite(OCTAVE_UP_LED_RED_PIN, 255);
  analogWrite(OCTAVE_UP_LED_GREEN_PIN, 0);
  analogWrite(RECORD_LED_PIN, 255); 
  delay(100);
  //MIDI.begin(1);  //Seleccionar en caso de MIDI Virtual
  Serial.begin(9600); 
  delay(100);
  initSamplerBuffer();
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
  }else{
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening(); 
  }
  delay(100);
}

void loop() {
  time = millis()-(t0 + t1 + reset);///time  es el momento Ti dentro del sample
  setReset();
  PlayBuffer();
  showLeds();
  if( radio.available()){// While there is data ready
    radio.read( &inInt, sizeof(int) ); 
    if (inInt>999) {  //It means that its a CCMessagge
      SplitCCM(inInt);
      sendMIDIccm(MIDI_CHANNEL, ch, num);
    } else if (inInt == CAXIXI_SAMPLER_CLEAR){
      Clear();
    } else if (inInt == CAXIXI_RECORD_START){
      RecordStart();
    } else if (inInt == CAXIXI_RECORD_STOP){
      RecordStop();
    } else if (inInt == CAXIXI_OCTAVE_UP){
      OctaveUp();
    } else if (inInt == CAXIXI_OCTAVE_DOWN){
      OctaveDown();
    } else if (inInt == CAXIXI_RIGHT_FORWARD_NOTEON){
      SendNoteOn(cxRightForwardNote);
    } else if (inInt == CAXIXI_RIGHT_FORWARD_NOTEOFF){
      SendNoteOff(cxRightForwardNote);
    } else if (inInt == CAXIXI_RIGHT_BACKWARD_NOTEON){
      SendNoteOn(cxRightBackwardNote);
    } else if (inInt == CAXIXI_RIGHT_BACKWARD_NOTEOFF){ 
      SendNoteOff(cxRightBackwardNote);
    } else if (inInt == CAXIXI_RIGHT_HIT_NOTEON){ 
      SendNoteOn(cxRightHitNote);
    } else if (inInt == CAXIXI_RIGHT_HIT_NOTEOFF){  
      SendNoteOff(cxRightHitNote);
    } else if (inInt == CAXIXI_LEFT_FORWARD_NOTEON){
      SendNoteOn(cxLeftForwardNote);
    } else if (inInt == CAXIXI_LEFT_FORWARD_NOTEOFF){
      SendNoteOff(cxLeftForwardNote);
    } else if (inInt == CAXIXI_LEFT_BACKWARD_NOTEON){ 
      SendNoteOn(cxLeftBackwardNote);
    } else if (inInt == CAXIXI_LEFT_BACKWARD_NOTEOFF){  
      SendNoteOff(cxLeftBackwardNote);
    }   else if (inInt == CAXIXI_LEFT_HIT_NOTEON){  
      SendNoteOn(cxLeftHitNote);
    } else if (inInt == CAXIXI_LEFT_HIT_NOTEOFF){ 
      SendNoteOff(cxLeftHitNote);
    }
  }
  /*
  ////TEST MIDI VIRTUAL
  SendNoteOn(cxLeftHitNote);
  delay(500);
  SendNoteOff(cxLeftHitNote);
  delay(500);
  */
}

void SplitCCM(int inInt){
    ch = inInt/1000;
    //ch = ch + currentOctave * 6;//Unncomment if want to have Octaves in CCM control numbers //SELECT
    num = inInt - ch*1000;
}

void OctaveUp()
{
  SendNoteOff(cxRightForwardNote);
  SendNoteOff(cxRightBackwardNote);
  SendNoteOff(cxRightHitNote);
  SendNoteOff(cxLeftForwardNote);
  SendNoteOff(cxLeftBackwardNote);
  SendNoteOff(cxLeftHitNote);
  currentOctave = currentOctave + 1;
  if (currentOctave>3) {
    currentOctave = -3;
  }
}

void OctaveDown()
{
  SendNoteOff(cxRightForwardNote);
  SendNoteOff(cxRightBackwardNote);
  SendNoteOff(cxRightHitNote);
  SendNoteOff(cxLeftForwardNote);
  SendNoteOff(cxLeftBackwardNote);
  SendNoteOff(cxLeftHitNote);
  currentOctave = currentOctave - 1;
  if (currentOctave<-3) {
    currentOctave = 3;
  }
}

//// -MIDI- (SELECT)

/////OPCION 1 MIDI NATIVO///////////
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void sendMIDIccm(byte channel, byte control, byte value){
      midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
    }

void SendNoteOn(int note){  //MIDIUSB Version
  note = note + (currentOctave * 12);
  if(record){
    setT0(); //For the first note only
    time = millis()-(t0 + t1 + reset);  //First Note add as Ti == 0, from then Ti as position in the loop
    Buffer sample = {note, layer, 1, time};
    samples[bufferI] = sample;
  }
  noteOn(MIDI_CHANNEL,note,127);
  if(record){
    bufferI++;  ///modificar al condicional de arriba
  }
}

void SendNoteOff(int note){ //MIDIUSB Version
  note = note + (currentOctave * 12);
  if(record){
    Buffer sample = {note, layer, 0, time};
    samples[bufferI] = sample;
  }
  noteOff(MIDI_CHANNEL,note,127);
  if(record){
    bufferI++;  ///modificar al condicional de arriba
  }
}

void PlayBuffer() { //MIDIUSB Version
  if(play){
    if(samples[bufferJ].time-time<0 && samples[bufferJ].time-time>-100){//Esto es un hardcodeo (el -100), corregirlo mirando el reloj en la V2, o dejarlo...
      if(samples[bufferJ].encendido){
       noteOn(MIDI_CHANNEL,samples[bufferJ].note,127);
      }else{
       noteOff(MIDI_CHANNEL,samples[bufferJ].note,127);
      }
      bufferJ++; //J se reinicia cuando da la vuelta, en la fc setReset 
    }
  }
} 
/////FIN OPCION 1 MIDI NATIVO///////////

/////OPCION 2 VIRTUAL MIDI PORT/////////
/*
void sendMIDIccm(byte channel, byte control, byte value){//ch=controller num=value
 MIDI.sendControlChange(ch, num, MIDI_CHANNEL);
}

void SendNoteOn(int note) { //MIDI VIRTUAL VERSION
  note = note + (currentOctave * 12);
  if(record){
    setT0(); //For the first note only
    time = millis()-(t0 + t1 + reset);  //First Note add as Ti == 0, from then Ti as position in the loop
    Buffer sample = {note, layer, 1, time};
    samples[bufferI] = sample;
  }
  MIDI.sendNoteOn(note,127,MIDI_CHANNEL);
  if(record){
    bufferI++;  ///modificar al condicional de arriba
  }
}

void SendNoteOff(int note) { //MIDI VIRTUAL VERSION
 note = note + (currentOctave * 12);
  if(record){
    Buffer sample = {note, layer, 0, time};
    samples[bufferI] = sample;
  }
  MIDI.sendNoteOff(note,127,MIDI_CHANNEL);
  if(record){
    bufferI++;  ///modificar al condicional de arriba
  }
}

void PlayBuffer() { //MIDI VIRTUAL VERSION
  if(play){
    if(samples[bufferJ].time-time<0 && samples[bufferJ].time-time>-100){//Esto es un hardcodeo (el -100), corregirlo mirando el reloj en la V2, o dejarlo...
      if(samples[bufferJ].encendido){
        MIDI.sendNoteOn(samples[bufferJ].note,127,MIDI_CHANNEL);
      }else{
        MIDI.sendNoteOff(samples[bufferJ].note,127,MIDI_CHANNEL);
      }
      bufferJ++; //J se reinicia cuando da la vuelta, en la fc setReset 
    }
  }
}
*/
////FIN OPCION 2 VIRTUAL MIDI PORT/////////
