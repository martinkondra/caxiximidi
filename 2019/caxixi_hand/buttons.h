// Sampler
bool record = false;
int recordButtonState = 0;    // current state of the button
int lastRecordButtonState = 0;  // previous state of the button
int clearButtonState = 0;
int lastClearButtonState = 0;

// Octavator
int octaveUpButtonLastState  = 0;
int octaveDownButtonLastState  = 0;
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = LOW;
unsigned long firstTime; // how long since the button was first pressed
int longPressThreshold = 2000; //when a button press is considered long in millis

int lastDownState = 0;
int octaveUpButtonState = 0;  // variable for reading the pushbutton status
int octaveDownButtonState = 0;
int currentOctave = 0;



///MENSAJES EMITIDOS POR BOTONES

void SendRecordStart() {
  SendToReceiver(CAXIXI_RECORD_START);
}

void SendRecordStop() {
  SendToReceiver(CAXIXI_RECORD_STOP);
}

void SendOctaveUp() {
  SendToReceiver(CAXIXI_OCTAVE_UP);
}

void SendOctaveDown(){
  SendToReceiver(CAXIXI_OCTAVE_DOWN);
}

void SendClear() {
  SendToReceiver(CAXIXI_SAMPLER_CLEAR);
}





void ButtonClear() {
  clearButtonState = digitalRead(SAMPLER_BUTTON_CLEAR_PIN);
  if (clearButtonState != lastClearButtonState) {// if the state has changed, increment the counter
    if (clearButtonState == HIGH) {// if the current state is HIGH then the button wend from off to on:
    SendClear();
    }
    lastClearButtonState = clearButtonState; // save the current state as the last state, for next time through the loop
  }
}

void ButtonOctaveDown() {
  octaveDownButtonState = digitalRead(OCTAVE_DOWN_BUTTON_PIN);
  if (octaveDownButtonState != octaveDownButtonLastState) {
    if (octaveDownButtonState == HIGH){
      firstTime = millis();
    }else{//Se decidira que hacer cuando se despresione el boton
      millis_held = (millis() - firstTime);//calculate time held
      if(millis_held > longPressThreshold){//Boton presion larga
        wantCCM = !wantCCM;
      }else{//boton presion corta
        SendOctaveDown(); 
      }
    }
    octaveDownButtonLastState = octaveDownButtonState;
  }
}

void ButtonRecord() {
  recordButtonState = digitalRead(SAMPLER_BUTTON_RECORD_PIN);
  if (recordButtonState != lastRecordButtonState) {// if the state has changed, increment the counter
    if (recordButtonState == HIGH) {// if the current state is HIGH then the button wend from off to on:
      if (record==true){
        record=false;
        SendRecordStop();
      } else {
        record=true;
        SendRecordStart();
      }
    }
    lastRecordButtonState = recordButtonState; // save the current state as the last state, for next time through the loop
  }
}

void ButtonOctaveUp() {
  octaveUpButtonState = digitalRead(OCTAVE_UP_BUTTON_PIN);

  if (octaveUpButtonState != octaveUpButtonLastState) {
    if (octaveUpButtonState == HIGH){
      firstTime = millis();
    }else{//Se decidira que hacer cuando se despresione el boton
      millis_held = (millis() - firstTime);//calculate time held
      if(millis_held > longPressThreshold){//Boton presion larga
        wantCCM = !wantCCM;
      }else{//boton presion corta
        SendOctaveUp(); 
      }
    }
    octaveUpButtonLastState = octaveUpButtonState;
  }
}
