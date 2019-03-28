#include "MIDIUSB.h"
#include <SPI.h>
#include "RF24.h"
#include <stdlib.h>

int roleSET = 0;
bool radioNumber = roleSET;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(5,6);
byte addresses[][6] = {"1Node","2Node"};
// Used to control whether this node is sending or receiving
bool role = roleSET;
int inInt;

#include "receiverConfig.h"
#include "CxCircularBuffer.h"

char inData[10];
int index;
boolean started = false;
boolean ended = false;
int ch;
int num;

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
  pinMode(RECORD_LED_PIN, OUTPUT);
  pinMode(OCTAVE_UP_LED_RED_PIN, OUTPUT);
  pinMode(OCTAVE_UP_LED_GREEN_PIN, OUTPUT);
  pinMode(OCTAVE_DOWN_LED_RED_PIN, OUTPUT);
  pinMode(OCTAVE_DOWN_LED_GREEN_PIN, OUTPUT);
  analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
  analogWrite(OCTAVE_DOWN_LED_GREEN_PIN, 255);
  analogWrite(OCTAVE_UP_LED_RED_PIN, 255);
  analogWrite(OCTAVE_UP_LED_GREEN_PIN, 255);
  analogWrite(RECORD_LED_PIN, 255);
  delay(500);
  Serial.begin(9600); 
  delay(100);
  initSamplerBuffer();
  delay(1000);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
  }else{
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening(); 
  }
  delay(100);
  //makeRecord();
}

void loop() {
  time = readClock();
  PlayBuffer();
  showLeds();
  receiveRadio();
  debug();
}

void debug() { //crear una string con todo el debug y mandarlo de una
  Serial.print(time);
  Serial.print('\t');
  Serial.println(samples[bufferPlay].time);
}

void receiveRadio() {
  if(radio.available()) {
    radio.read( &inInt, sizeof(int) ); 
    if (inInt>999) {  //CCMessagge
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
    } else if (inInt == CAXIXI_LEFT_HIT_NOTEON){  
      SendNoteOn(cxLeftHitNote);
    } else if (inInt == CAXIXI_LEFT_HIT_NOTEOFF){ 
      SendNoteOff(cxLeftHitNote);
    }
  }
}

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

//Probar estos cambios para que deje de grabar cuando completa el buffer!
void SendNoteOn(int note) {
  note = note + (currentOctave * 12);
  if(record && bufferRec<SAMPLER_BUFFER_SIZE/2) {
    time = fixTime(ppqn, grid);
    Buffer sample = {note, layer, 1, time};
    samples[bufferRec] = sample;
  }
  noteOn(MIDI_CHANNEL,note,127);
  if(record && bufferRec<SAMPLER_BUFFER_SIZE/2){
    bufferRec++;  
  }
}

void SendNoteOff(int note) {
  note = note + (currentOctave * 12);
  if(record && bufferRec<=SAMPLER_BUFFER_SIZE/2){
    Buffer sample = {note, layer, 0, time};
    samples[bufferRec] = sample;
  }
  noteOff(MIDI_CHANNEL,note,127);
  if(record && bufferRec<=SAMPLER_BUFFER_SIZE/2){
    bufferRec++;  
  }
}

void sendMIDIccm(byte channel, byte control, byte value) { //ch=controller num=value
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void SplitCCM(int inInt) {
  ch = inInt/1000;
  num = inInt - ch*1000;
}

void PlayBuffer() {
  if(play){
    if(samples[bufferPlay].time==time){
      if(samples[bufferPlay].encendido){
       noteOn(MIDI_CHANNEL,samples[bufferPlay].note,127);
      }else{
       noteOff(MIDI_CHANNEL,samples[bufferPlay].note,127);
      }
      bufferPlay++; //J se reinicia cuando da la vuelta readClock() 
    }
  }
}

void OctaveUp() {
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

void OctaveDown() {
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
