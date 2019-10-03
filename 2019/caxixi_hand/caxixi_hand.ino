//SENSOR LIBRERIAS
#include <I2Cdev.h>
#include <MPU60X0.h>  //No se necesita declarar aparte porque viene incluida en FreeIMU.h adaptado para MPU6050
#include <EEPROM.h>
#include "DebugUtils.h"
#include "FreeIMU.h" ///Usando aun libreria FREIMU, actualmente con AccScale en 16. Ver MPU60X0.cpp para cambiar la Scale +- 16,8,4 o 2g
#include <Wire.h>
#include "CommunicationUtils.h"//FREEIMU- CHECKEAR para que se utiliza
///CHEQUEAR (1-Wire.h Repetido 2-Si se puede sacar en Feather23u4 y otras )
#include <Wire.h> // For proMini Boards //Solucion para inicializacion de sensor en promini + battery + switch on/off
#include <avr/power.h> // For proMini Boards //Solucion para inicializacion de sensor en promini + battery + switch on/off
int powerpin = 5; // For proMini Boards //Solucion para inicializacion de sensor en promini + battery + switch on/off

//LIBRERIAS COMUNICACION
//CAXIXI NRF24 CONFIG
#include <SPI.h>
#include "RF24.h"
RF24 radio(9,10); //Hardware configuration: Set up nRF24L01 (CE- ; CSN+)radio on SPI bus plus pins 9 & 10 (for Hand), 5 & 6 (for CX Receiver) 
int roleSET = 1; //Set the Role 0 receiver or 1 sender right or #CHECK ?¿ 2 sender Second Pair- Caxixi x4 Hand and feet?¿
int radioNumber = roleSET;
byte addresses[][6] = {"1Node","2Node"};
bool role = roleSET; // Used to control whether this node is sending or receiving
////END OF CAXIXI NRF24 CONFIG

//CAXIXI LIBRERIAS Y OBJETOS PROPIOS
#include "CxCircularBuffer.h"
#include "messageOut.h" ///Mensaje emitido por NRF o Serial Monitor (For Debug porpouse)
#include "CaxixiConfig.h"
///Atributos usados 
int state = STATE_STILL;
bool wantCCM = false;
bool caxixiRight = CAXIXI_RIGHT;
CxCircularBuffer accelXBuffer(BUFFER_SIZE);
CxCircularBuffer accelYBuffer(BUFFER_SIZE);
boolean bufferReady = false;

int currentAccelX, currentAccelY;
int slopeStill;
int accelXForce;
int prevState;
int accelXSmooth[filterSamples];
int accelYSmooth[filterSamples];
int smoothAccelX;
int smoothAccelY;
int initialMillis;

int NoteRelease[3] = {
  NOTE_RELEASE_FORWARD,
  NOTE_RELEASE_BACKWARD,
  NOTE_RELEASE_HIT
};
int noteOn = NOTE_OFF;

///CCM Control numbers
int xccm, yccm, zccm;

//Data from MPU 6050 BY FREEIMU
int SensorRead[6] = {0, 0, 0, 0, 0, 0};
float v[6];
float angles[3];
FreeIMU my3IMU = FreeIMU();


//Logicas hit antiguas
//1
//boolean isUpThreshold, isDownThreshold, isUpThresholdRotated, isDownThresholdRotated;//YA NO SE UTILIZA
//boolean canHit, canHitRotated;//YA NO SE UTILIZA
//2
//int noteThresholdHit = NOTE_THRESHOLD_HIT;///CHECKEAR QUE YA NO SE UTILIZA
//int canHitDefinition = 1;///CHECKEAR QUE YA NO SE UTILIZA
//3
////Nueva logica de hit basada en evitar el ghost hit
//bool canHitUp = true;
//bool canHitDown = true;
#include "debugCaxixi.h" //Usado en Caxixi.h aun
#include "buttons.h"
#include "Caxixi.h"//QUITAR Incluye al "debug.h"
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
  //FOR DEBUG
  //Serial.begin(9600); ////Debug (no need in 32u4)
  //COMMUNICATION NRF24 
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
    ////SENSOR CONNECTION
    Wire.begin(); //Fix to powerOn battery ArduinoProMini
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true); 
    ///BUFFERS
    accelXBuffer.clear();
    accelYBuffer.clear();
    delay(500);
    GyroXBuffer.clear();
    GyroYBuffer.clear();
    GyroZBuffer.clear();
    delay(5);
    //INCIO LIBRERIA DEL IMU
    my3IMU.init();
    defCMMnumbers();///Definir los Control changes usados segun right or left
    delay(50);
}

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
