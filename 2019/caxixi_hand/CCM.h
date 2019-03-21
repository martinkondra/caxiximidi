bool CCMbufferReady = false;
CxCircularBuffer GyroXBuffer(BUFFER_SIZE);
CxCircularBuffer GyroYBuffer(BUFFER_SIZE);
CxCircularBuffer GyroZBuffer(BUFFER_SIZE);
int GyroXSmooth[filterSamples];
int smoothGyroX;
int GyroYSmooth[filterSamples];
int smoothGyroY;
int GyroZSmooth[filterSamples];
int smoothGyroZ;
bool NoteState = true;
int NoteThresholdCCM = 2000; //Umbral para mandar midiOn/Off
int NoteReleaseCCM = 1000;
bool Bloqueo = false;
int msg; //msg to send

boolean isRollingX;
String debugRollingX;
int isRollingXVariation = 1;
boolean isRollingY;
String debugRollingY;
int isRollingYVariation = 1;
boolean isRollingZ;
String debugRollingZ;
int isRollingZVariation = 1;

void setCircularBufferCCM(){
  smoothAccelY = digitalSmooth(SensorRead[SENSOR_ACCEL_Y], accelYSmooth);
  accelYBuffer.addValue(smoothAccelY);
  smoothGyroX = digitalSmooth(SensorRead[SENSOR_GYRO_X], GyroXSmooth);
  GyroXBuffer.addValue(smoothGyroX);
  smoothGyroY = digitalSmooth(SensorRead[SENSOR_GYRO_Y], GyroYSmooth);
  GyroYBuffer.addValue(smoothGyroY);
  smoothGyroZ = digitalSmooth(SensorRead[SENSOR_GYRO_Z], GyroZSmooth);
  GyroZBuffer.addValue(smoothGyroZ);
  }

boolean isCCMBufferReady(){
  if(GyroXBuffer.getCount() < BUFFER_SIZE
  || GyroYBuffer.getCount() < BUFFER_SIZE
  || GyroZBuffer.getCount() < BUFFER_SIZE
  || accelYBuffer.getCount() < BUFFER_SIZE){
    return false;
  } else {
    CCMbufferReady = true;
    return true;
  }
}

void setIsRollingX(){
  int currentValue = GyroXBuffer.getPreviousElement(1);
  /*para debug
  Serial.print(currentValue);
  Serial.print(",");
  */
  int firstValue = GyroXBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);
  if(firstVariation > isRollingXVariation){
        isRollingX = true;
       return;
  } else {
    isRollingX = false; }
}

void setIsRollingY(){
  int currentValue = GyroYBuffer.getPreviousElement(1);
     /* //para debug
  Serial.print(currentValue);
  Serial.print(",");*/
  int firstValue = GyroYBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);
  if(firstVariation > isRollingYVariation){
    isRollingY = true;
    return;
  }else{
    isRollingY = false; }
}

void setIsRollingZ(){
  int currentValue = GyroZBuffer.getPreviousElement(1);
     /* //para debug
  Serial.println(currentValue);*/
  int firstValue = GyroZBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);
  if(firstVariation > isRollingZVariation){
    isRollingZ = true;
    return;
  }else{
    isRollingZ = false; }
}

int formatCCM(int NUM, int CH) {
  msg = CH*1000 + NUM;
  return msg;
}

void processX()
{
  int x, controlvalueX;
  controlvalueX = abs(GyroXBuffer.getPreviousElement(1));
  if(controlvalueX>180){
     controlvalueX = 180;}      
  x = map(controlvalueX, 0, 180, 0, 127); // scale to midi range
  SendToReceiver(formatCCM(x,15));
  delay(2);
}

void processY()
{
  int y, controlvalueY;
  controlvalueY = GyroYBuffer.getPreviousElement(1);      
  y = map(controlvalueY, -90, 90, 0, 127); 
  SendToReceiver(formatCCM(y,16));
  delay(2);
}

void processZ()
{
  int z, controlvalueZ;
  controlvalueZ = GyroZBuffer.getPreviousElement(1);
  z = map(controlvalueZ, -90, 90, 0, 127); 
  SendToReceiver(formatCCM(z,17));
  delay(2);
}


void ccmNotes() {
  if (currentAccelY > NoteThresholdCCM & (!Bloqueo)){
    if (NoteState){
       SendToReceiver(CAXIXI_HIT_NOTEON);
       NoteState = false;
       Bloqueo = true;
       }
    else{
      SendToReceiver(CAXIXI_HIT_NOTEOFF);
      NoteState = true;
      Bloqueo = true;
      }
    }
  if (currentAccelY < NoteReleaseCCM){
    Bloqueo = false;
    }
} 

void areRolling() {
  setIsRollingX();
  setIsRollingY();
  setIsRollingZ();  
}

void ProcessCCM() {
  if(isRollingX){
    processX();
    }
  if(isRollingY){
    processY();
    }
  if(isRollingZ){
    processZ();
    }
}

void runCCM() {
   if(caxixiRight){
      ButtonOctaveUp();
     } else {
      ButtonOctaveDown();
      }
  if(caxixiRight){
    currentAccelY = accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    } else {
    currentAccelY = - accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    }
  ccmNotes();
  areRolling();
  ProcessCCM();
  }
