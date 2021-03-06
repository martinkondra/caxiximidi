

int digitalSmooth(int rawIn, int *sensSmoothArray) {
  int j, k, temp, top, bottom;
  long total;
  static int i;
  // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;
  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot
  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }
  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
        }
      }
    }
    // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
    bottom = max(((filterSamples * 15)  / 100), 1); 
    top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
    k = 0;
    total = 0;
    for ( j = bottom; j< top; j++){
      total += sorted[j];  // total remaining indices
      k++; 
    }
  return total / k;
}

void setCircularBuffer() {
  smoothAccelX = digitalSmooth(SensorRead[SENSOR_ACCEL_X], accelXSmooth);
  smoothAccelY = digitalSmooth(SensorRead[SENSOR_ACCEL_Y], accelYSmooth);
  accelXBuffer.addValue(smoothAccelX);
  accelYBuffer.addValue(smoothAccelY);
}

boolean isBufferReady() {
  if(accelXBuffer.getCount() < BUFFER_SIZE
  || accelYBuffer.getCount() < BUFFER_SIZE){
    return false;
  }else{
    bufferReady = true;
    return true;
  }
}

void setSlopeStill() {
  if(abs(currentAccelX - accelXBuffer.getPreviousElement(8)) < SLOPE_STILL_X_RANGE
  && abs(currentAccelY - accelYBuffer.getPreviousElement(8)) < SLOPE_STILL_Y_RANGE){
    slopeStill = SLOPE_STILL_STATUS; 
  }else{
    slopeStill = SLOPE_MOVING_STATUS;
  }
}

void setAccelXForce() {
  if(currentAccelX > FORCE_THRESHOLD_FORWARD || currentAccelY > NOTE_THRESHOLD_HIT){
    accelXForce = FORCE_FORWARD;
  }
  if(currentAccelX < FORCE_THRESHOLD_BACKWARD){
    accelXForce = FORCE_BACKWARD;
  }
  if(currentAccelX < FORCE_STILL_RANGE_FORWARD && currentAccelX > FORCE_STILL_RANGE_BACKWARD){
    accelXForce = FORCE_STILL;
  }
}
  
void setState() {////send NOTE HIT
  if(slopeStill == SLOPE_STILL_STATUS && accelXForce == FORCE_STILL){
    state = STATE_STILL;
  }else if(slopeStill == SLOPE_MOVING_STATUS && accelXForce == FORCE_FORWARD){
    state = STATE_FORWARD;
  }else if(slopeStill == SLOPE_MOVING_STATUS && accelXForce == FORCE_BACKWARD){
    state = STATE_BACKWARD;
  }else{
    state = prevState;
  }
}

boolean noteReleaseForward() {
  if(currentAccelX < NoteRelease[NOTE_FORWARD]){
    return true;
  }else{
    return false;
  }
}

boolean noteReleaseBackward() {
  if(currentAccelX > NoteRelease[NOTE_BACKWARD]){
    return true;
  }else{
    return false;
  }
}

boolean noteReleaseHit() {
  if(currentAccelY < NoteRelease[NOTE_HIT]){
    return true;
  }else{
    return false;
  }
}


void runCaxixi() {
  if(caxixiRight){
    ButtonRecord();
    ButtonOctaveUp();
  } else {
    ButtonClear();
    ButtonOctaveDown();
    }
  if(caxixiRight){
    currentAccelY = accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
  } else {
    currentAccelY = - accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    }
  setSlopeStill();
  setAccelXForce();
  prevState = state;
  setState();

  ////Serial Monitor Debug SET IN Caxixiconfig.h
  if (debugSerialMonitor = true){
    debug();///For Debug Caxixi /// Check debug.h para elegir info a debuguear
  }

  /////SEND HITS MESSAGES
  switch (noteOn) {
    case NOTE_FORWARD:
    if(noteReleaseForward()){
      SendNoteOff(CAXIXI_FORWARD_NOTEOFF);
      noteOn = NOTE_OFF;
    }
    break;
    case NOTE_BACKWARD:
    if(noteReleaseBackward()){
      SendNoteOff(CAXIXI_BACKWARD_NOTEOFF);
      noteOn = NOTE_OFF;
    }
    break;
    case NOTE_HIT:
    if(noteReleaseHit()){
      SendNoteOff(CAXIXI_HIT_NOTEOFF);
      noteOn = NOTE_OFF;
    }
    break;
    default:
    break;
  }
  if(noteOn == NOTE_OFF && state == STATE_FORWARD && prevState == STATE_BACKWARD){
    noteOn = NOTE_FORWARD;
    SendNoteOn(CAXIXI_FORWARD_NOTEON);
  }
  if(noteOn == NOTE_OFF && state == STATE_BACKWARD && prevState == STATE_FORWARD){
    noteOn = NOTE_BACKWARD;
    SendNoteOn(CAXIXI_BACKWARD_NOTEON);
  }
  if(noteOn == NOTE_OFF && currentAccelY > NOTE_THRESHOLD_HIT){
    noteOn = NOTE_HIT;
    SendNoteOn(CAXIXI_HIT_NOTEON);
  }
}
