Buffer samples[SAMPLER_BUFFER_SIZE]; //BUFFER SIZE
int bufferI=0; //indice del buffer para record
int bufferJ=0; //indice del buffer para play
int buttonPushCounter = 0;     // counter for the number of button presses
int layer = 0;
bool record = false;
bool play = false;
bool isSetT0 = false;
bool isSetT1 = false;
bool FirstNote = false;
long time = 0;  //Ti, Actual time inside loop 
long t0 = 0;    //To, First note time, Records begins
long t1 = 0;    //Tf, First Layer record ends, Loop duration
long reset = 0; //Actual time (milis) when loop begins, Updated when sample restart


void bubbleSort(Buffer a[], int bufferI) {  // ???Optimizamos esto reemplazando size por i?
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

void initSamplerBuffer(){
  for(int k=0; k<SAMPLER_BUFFER_SIZE; k++){
    samples[k] = Buffer_default;
  }
}

void Clear_Buffer(Buffer a[], int bufferI) {
    for(int k=0; k<=bufferI; k++) {
        for(int o=0; o<(bufferI-(k+1)); o++) {
                    samples[k] = Buffer_default;
        }
    }
}

void setReset(){ //ENDING LOOP
  int a = t1 - time;  // a = remaing loop time
  if(t0 != 0 && t1!=0 && a<=0){  
    bubbleSort(samples,bufferI);
    reset = millis()-t0-t1;  // Updates var reset to play next loop 
    bufferJ = 0;
  }
}

void setT0() { //Called by NoteON if record
  if (!isSetT0 && !FirstNote){ //SET IF FIRST NOTE
    t0 = millis(); 
    FirstNote = true;
    isSetT0 = true; 
  }
}

void setT1(){ //Called by RecordStop() 
  if(!isSetT1){  // t1 = Sample duration, set by first layer
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

void Clear(){
  record = false;
  play = false;
  isSetT0 = false;
  isSetT1 = false;
  FirstNote = false;
  t0 = 0;
  t1 = 0;
  reset = 0;
  layer = 0;
  Clear_Buffer(samples,bufferI);
  bufferI=0;
  bufferJ=0;
}
