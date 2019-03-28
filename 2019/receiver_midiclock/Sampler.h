int len_sample = 96; // 1 compas
int grid = 12; //24= negra, 12=corchea
int ppqn = 0;

Buffer samples[SAMPLER_BUFFER_SIZE]; //puede almacenar hasta 50 notas (50 on, 50 off)
int bufferI=0; //indice del buffer para record
int bufferJ=0; //indice del buffer para play
int layer = 0;
bool record = false;
bool play = false;
int time = 0;

void bubbleSort(Buffer a[], int bufferI) {
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

int fixTime(int time, int grid) {
  div_t result;
  result = div(time,grid);
  if(result.rem<=grid/2) {
    return result.quot*grid;
  } else {
    return (result.quot+1)*grid;
  }
}

void initSamplerBuffer() {
  for(int k=0; k<SAMPLER_BUFFER_SIZE; k++){
    samples[k] = Buffer_default;
  }
}

void makeRecord() { //cargo las notas en el buffer, for test purposes
  int fixed;
  Buffer sample0 = {45, 1, 1, 2};
  samples[0] = sample0;
  fixed = fixTime(samples[0].time, grid);
  samples[0].time = fixed;
  
  Buffer sample1 = {45, 1, 0, 25};
  samples[1] = sample1;
  fixed = fixTime(samples[1].time, grid);
  samples[1].time = fixed;
  
  Buffer sample2 = {43, 1, 1, 32};
  samples[2] = sample2;
  fixed = fixTime(samples[2].time, grid);
  samples[2].time = fixed;
  
  Buffer sample3 = {43, 1, 0, 34};
  samples[3] = sample3;
  fixed = fixTime(samples[3].time, grid);
  samples[3].time = fixed;
  
  Buffer sample4 = {45, 1, 1, 50};
  samples[4] = sample4;
  fixed = fixTime(samples[4].time, grid);
  samples[4].time = fixed;
  
  Buffer sample5 = {45, 1, 0, 66};
  samples[5] = sample5;
  fixed = fixTime(samples[5].time, grid);
  samples[5].time = fixed;
  
  Buffer sample6 = {43, 1, 1, 78};
  samples[6] = sample6;
  fixed = fixTime(samples[6].time, grid);
  samples[6].time = fixed;
  
  Buffer sample7 = {43, 1, 0, 88};
  samples[7] = sample7;
  fixed = fixTime(samples[7].time, grid);
  samples[7].time = fixed;

}

void RecordStart() {
  digitalWrite(RECORD_LED_PIN, LOW);
  record=true;
  layer = layer + 1;
}

void RecordStop() {
  analogWrite(RECORD_LED_PIN, HIGH);
  record=false;
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
  
  Clear_Buffer(samples,bufferI);
  int bufferI=0; //indice del buffer para record
  int bufferJ=0; //indice del buffer para play
}

int readClock() {
  // agregar una variables len_sample que indique cada cuánto dar la vuelta
  // comienza siendo 24
  // cuando pongo a grabar es "infinito" (decidir cuántos compaces es el máximo, *24)
  // cuando pongo play es el valor en el que se quedo ppqn fixeado con grid en 24
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if(rx.byte1 == 0xF8){
       ++ppqn;
       if(ppqn == len_sample){    
          ppqn = 0;
          bufferJ=0;
       };
    }
    //Clock start byte
    else if(rx.byte1 == 0xFA){
      ppqn = 0;
      //bufferJ = 0; //posiblemente no sea necesario ya que vengo de un stop
    }
    //Clock stop byte
    else if(rx.byte1 == 0xFC){
      //ppqn = 0;
      bufferJ = 0;
    };
  } while (rx.header != 0);
  return ppqn; 
}
