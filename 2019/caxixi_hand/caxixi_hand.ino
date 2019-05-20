#include <MS561101BA.h>
#include <I2Cdev.h>
#include <MPU60X0.h>
#include <EEPROM.h>
#include "DebugUtils.h"
#include "FreeIMU.h"
#include <Wire.h>
#include <SPI.h>
#include "RF24.h"
#include "CommunicationUtils.h"
#include <Wire.h>
#include "CxCircularBuffer.h"
#include <avr/power.h> // For proMini Boards
int powerpin = 5;

//CAXIXI NRF24 CONFIG
int roleSET = 1; //Set the Role 0 receiver or 1 sender right or 2 sender left
int radioNumber = roleSET;
RF24 radio(9,10); //Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */

byte addresses[][6] = {"1Node","2Node"};
bool role = roleSET; // Used to control whether this node is sending or receiving

#include "messageOut.h" ///Mensaje emitido por NRF o Serial Monitor (For Debug porpouse)

boolean isUpThreshold, isDownThreshold, isUpThresholdRotated, isDownThresholdRotated;
boolean canHit, canHitRotated;
int currentAccelX, currentAccelY;
int slopeStill;
int accelXForce;
int prevState;

#include "CaxixiConfig.h"

int state = STATE_STILL;
bool wantCCM = false;
bool caxixiRight = CAXIXI_RIGHT;
CxCircularBuffer accelXBuffer(BUFFER_SIZE);
CxCircularBuffer accelYBuffer(BUFFER_SIZE);
boolean bufferReady = false;

int accelXSmooth[filterSamples];
int accelYSmooth[filterSamples];
int smoothAccelX;
int smoothAccelY;

int SensorRead[6] = {0, 0, 0, 0, 0, 0};

int NoteRelease[3] = {
  NOTE_RELEASE_FORWARD,
  NOTE_RELEASE_BACKWARD,
  NOTE_RELEASE_HIT
};

int noteThresholdHit = NOTE_THRESHOLD_HIT;
int canHitDefinition = 1;

int noteOn = NOTE_OFF;

float v[6];
float angles[3];
FreeIMU my3IMU = FreeIMU();

#include "buttons.h"
#include "Caxixi.h"
#include "CCM.h"

void setup() {
  analogWrite(13,150);
  if(caxixiRight){
    pinMode(OCTAVE_UP_BUTTON_PIN, INPUT);// initialize the button pin as a input:
    pinMode(SAMPLER_BUTTON_RECORD_PIN, INPUT);
  } else {
    pinMode(OCTAVE_DOWN_BUTTON_PIN, INPUT);
    pinMode(SAMPLER_BUTTON_CLEAR_PIN, INPUT);
  }
  //Serial.begin(9600); ////Debug (no need in 32u4)
   radio.begin();
  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber==0){
    //radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
   // radio.openReadingPipe(2,addresses[2]);
    radio.startListening(); 
      }
    else if(radioNumber==1){
    radio.openWritingPipe(addresses[1]);
    //radio.openReadingPipe(1,addresses[0]);
    }
    else{
    radio.openWritingPipe(addresses[2]);
    }
    delay(10);
    Wire.begin(); //Fix to powerOn battery ArduinoProMini
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true); 
    accelXBuffer.clear();
    accelYBuffer.clear();
    delay(500);
    GyroXBuffer.clear();
    GyroYBuffer.clear();
    GyroZBuffer.clear();
    delay(5);
    my3IMU.init();
    //Serial.begin(9600);//PARA DEBUG, NO NECESARIO EN FEATHER 32U4
    delay(50);
}

int initialMillis;

void loop() {
  //initialMillis = millis(); //Debug
  my3IMU.getValues(v);
  SensorRead[SENSOR_ACCEL_X] = (int)v[0];
  SensorRead[SENSOR_ACCEL_Y] = (int)v[1];
  SensorRead[SENSOR_ACCEL_Z] = (int)v[2];

  if(wantCCM){      
    my3IMU.getYawPitchRoll(angles);
    SensorRead[SENSOR_GYRO_X] = (int)angles[0];
    SensorRead[SENSOR_GYRO_Y] = (int)angles[1];
    SensorRead[SENSOR_GYRO_Z] = (int)angles[2];
    setCircularBufferCCM();
    if(CCMbufferReady || isCCMBufferReady()){
      runCCM();
    }
  }
  else {
    setCircularBuffer();
    if(bufferReady || isBufferReady()){
      runCaxixi();
    }
  }
  delay(2);
  //int diffMillis = millis() - initialMillis;  //Debug
  //Serial.println(diffMillis); //Debug
}



