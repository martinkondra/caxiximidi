Buffer samples[SAMPLER_BUFFER_SIZE]; //puede almacenar hasta 50 notas (50 on, 50 off)
int bufferI=0; //indice del buffer para record
int bufferJ=0; //indice del buffer para play
int buttonPushCounter = 0;        // counter for the number of button presses
int layer = 0;
bool record = false;
bool play = false;
bool isSetT0 = false;
bool isSetT1 = false;
bool FirstNote = false;
long time = 0;    //time = millis() cuenta los milisegundos desde q arranca
long t0 = 0;    //cuando record==True, ajustamos una variable "t0" a esos msec y t = millis()-t0
long t1 = 0;    //cuando record == False, ajustamos una variable "t1", t1 = millis-t0
long reset = 0;   //y ajustamos la variable reset = millis - t0 - t1, que en principio va a ser cero, pero en cada vuelta va a ir incrementándose en el tamaño del loop

void bubbleSort(Buffer a[], int bufferI) {//Optimizamos esto reemplazando size por i?
  for(int k=0; k<=bufferI; k++) {
    for(int o=0; o<(bufferI-(k+1)); o++) {
      if(a[o].time > a[o+1].time) {
        Buffer t = a[o];
        a[o] = a[o+1];
        a[o+1] = t;
      }
    }
  }
}

void initSamplerBuffer() {
  for(int k=0; k<SAMPLER_BUFFER_SIZE; k++){
    samples[k] = Buffer_default;
  }
}

void setReset() {
  int a = t1 - time;
  if(t0 != 0 && t1!=0 && a<=0){
    bubbleSort(samples,bufferI);
    reset = millis()-t0-t1;
    bufferJ = 0;
  }
}

void setT0() {//A esta fc la llama ButtonRecord()
  if (record && !isSetT0 && FirstNote){
    t0 = millis();
  }
  if (record && !isSetT0 && FirstNote){
    isSetT0 = true;
  }
}

void setT1() {
  if(!record && !isSetT1 && t0 != 0){
    t1 = millis() - t0;
    isSetT1 = true;
    play = true;
  }
}

void RecordStart() {
  digitalWrite(RECORD_LED_PIN, LOW);
  // This will trigger setT0 on first Note
  record=true;
  layer = layer + 1;
}

void RecordStop() {
  analogWrite(RECORD_LED_PIN, LED_ON);
  record=false;
  setT1();
}

void Clear_Buffer(Buffer a[], int bufferI) {
    for(int k=0; k<=bufferI; k++) {
        for(int o=0; o<(bufferI-(k+1)); o++) {
                    samples[k] = Buffer_default;
        }
    }
}

void Clear() {
  bool record = false;
  bool play = false;
  bool isSetT0 = false;
  bool isSetT1 = false;
  bool FirstNote = false;
  
  long t0 = 0;           //cuando record==True, ajustamos una variable "t0" a esos msec y t = millis()-t0
  long t1 = 0;           //cuando record == False, ajustamos una variable "t1", t1 = millis-t0
  long reset = 0;        //y ajustamos la variable reset = millis - t0 - t1, que en principio va a ser cero, pero en cada vuelta va
  
  Clear_Buffer(samples,bufferI);
  int bufferI=0; //indice del buffer para record
  int bufferJ=0; //indice del buffer para play
}

#include <stdlib.h>
int fixTime2(int time, int grid) {
  div_t result;
  result = div(time,grid);
  if(result.rem<=grid/2) {
    return result.quot*grid;
  } else {
    return (result.quot+1)*grid;
  }
}

int fixTime(int time, int grid) {
  // buscar la forma más eficiente de calcular division y resto
  int div = time/grid;
  int resto = time%grid;
  if(resto<=grid/2) {
    return div*grid;
  } else {
    return (div+1)*grid;
  }
}
