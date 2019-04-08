int len_sample = 96; // 1 compas
int grid = 6; //24=negra, 12=corchea
int ppqn = 0;

Buffer samples[SAMPLER_BUFFER_SIZE];
int bufferRec=0;
int bufferPlay=0;
int layer = 0;
bool record = false;
bool play = false;
bool firstNote = false;
bool setT1 = false;
bool need_bubble = false;

// Performance: Hacer un flag para ver cuando es necesario hacer el bubble sort
// bool need_bubble = false;
// pasa a true cuando se agregan notas al buffer
void bubbleSort(Buffer a[], int bufferRec) {
  if (need_bubble) {
    for(int k=0; k<=bufferRec; k++) {
      for(int o=0; o<(bufferRec-(k+1)); o++) {
        if(a[o].time > a[o+1].time) {
          Buffer t = a[o];
          a[o] = a[o+1];
          a[o+1] = t;
        }
      }
    }
  need_bubble = false;
  }
}

int fixTime(int time, int grid) {
  div_t result;
  int fixed;
  result = div(time,grid);
  if(result.rem<=grid/2) {
    fixed = result.quot*grid;
  } else {
    fixed = (result.quot+1)*grid;
  }
  if((fixed==96) && (len_sample==-1)){ //BUG: provoca que no suene la nota cuando el len_sample es mayor a 96
    ppqn =0;
    fixed=0;
  }
  return fixed;
}

void initSamplerBuffer() {
  for(int k=0; k<SAMPLER_BUFFER_SIZE; k++){
    samples[k] = Buffer_default;
  }
}

void RecordStart() {
  digitalWrite(RECORD_LED_PIN, LOW);
  record=true;
  layer = layer + 1;
}

void RecordStop() {
  if((!setT1) && (firstNote)) { // para no armar un bucle vacio
    len_sample = ((ppqn/24)+1)*24; // len_sample es el pulso siguiente
    setT1 = true;
  }
  digitalWrite(RECORD_LED_PIN, HIGH);
  record=false;
  play=true;
}

void Clear_Buffer(Buffer a[], int bufferRec) {
  for(int k=0; k<=bufferRec; k++) {
    samples[k] = Buffer_default;
  }
}

// Work in progress
// Pensar consecuencias para la reproducción si borro una capa
// bufferPlay debería también estar organizado en layers
// o quizás hacerlo cuando termina el loop
void Clear_Layer(Buffer a[], int bufferRec) {
  for(int k=0; k<=bufferRec; k++) {
    if (samples[k].layer==layer) {
      samples[k] = Buffer_default;
    }
  }
}

void Clear() {
  record = false;
  digitalWrite(RECORD_LED_PIN, HIGH);
  play = false;
  setT1 = false;
  firstNote = false;
  len_sample = 96;
  Clear_Buffer(samples,bufferRec);
  bufferRec=0;
  bufferPlay=0;
  ppqn = ppqn%96; // para quedar en el mismo lugar del compas, hardcodeado para 4/4
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

void playFirstNotes() {
  if(play){
    for(int k=0; k<=bufferRec; k++) {
      if(samples[k].time==0){
        if(samples[bufferPlay].encendido){
          noteOn(MIDI_CHANNEL,samples[k].note,127);
        }else{
          noteOff(MIDI_CHANNEL,samples[k].note,127);
        }
        bufferPlay++;
      }
      else {
        break;
      }
    }
  }
}

int readClock() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if(rx.byte1 == 0xF8){ //0xF8
       ++ppqn;
       if(ppqn == len_sample){    
          ppqn = 0;
          bufferPlay=0;
          bubbleSort(samples,bufferRec);
       };
    }
    //Clock start byte
    else if(rx.byte1 == 0xFA){
      play = true;
      ppqn = 0;
      playFirstNotes();
    }
    //Clock stop byte
    else if(rx.byte1 == 0xFC){
      play = false;
      bufferPlay = 0;
      bubbleSort(samples,bufferRec);
    };
  } while (rx.header != 0);
  return ppqn; 
}
