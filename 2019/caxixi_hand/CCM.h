//Variables Threshold
int NoteThresholdCCM = 2000; //Umbral para mandar midiOn/Off
int NoteReleaseCCM = 1000;
int rollMovingThreshold = 5; //DIFERENCIA ABSOLUTA GYROBUFFER Threshold XYZ 
////////////////////////////
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
int msg; //msg to send
bool NoteState = true;
bool Bloqueo = false;
 
boolean isRollingX;
String debugRollingX;
//int isRollingXVariation = 5;  //Threshold X
boolean isRollingY;
String debugRollingY;
//int isRollingYVariation = 5;  //Threshold Y
boolean isRollingZ;
String debugRollingZ;
//int isRollingZVariation = 5;  //Threshold Z

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

/////CAXIXI CCM HITS
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
/////END OF CCM HITS

/////Check Movement GYRO
void setIsRollingX(){
  int currentValue = GyroXBuffer.getPreviousElement(1);
    /*Debug
  Serial.print(currentValue);
  Serial.print(",");
  */
  int firstValue = GyroXBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);//DIFERENCIA ABSOLUTA GYROBUFFER
  if(firstVariation > rollMovingThreshold){//UMBRAL DE MOVIMIENTO 
        isRollingX = true;
       return;
  } else {
    isRollingX = false; }
}

void setIsRollingY(){
  int currentValue = GyroYBuffer.getPreviousElement(1);
     /*Debug
  Serial.print(currentValue);
  Serial.print(",");*/
  int firstValue = GyroYBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);
  if(firstVariation > rollMovingThreshold){
    isRollingY = true;
    return;
  }else{
    isRollingY = false; }
}

void setIsRollingZ(){
  int currentValue = GyroZBuffer.getPreviousElement(1);
     /*Debug
  Serial.println(currentValue);*/
  int firstValue = GyroZBuffer.getPreviousElement(3);
  int firstVariation = abs(firstValue - currentValue);
  if(firstVariation > rollMovingThreshold){
    isRollingZ = true;
    return;
  }else{
    isRollingZ = false; }
}

int formatCCM(int NUM, int CH) {
  msg = CH*1000 + NUM;
  return msg;
}




void areRolling() {
  setIsRollingX();
  setIsRollingY();
  setIsRollingZ();  
}
///END OF Check Movement GYRO


///ProcessCCM  (send ccm if moving)
//class Solution maybe?
//Wrong accel "name" x->z, y->x, z->y (YawPitchRoll)
void processX() {
  int x, controlvalueX;
  controlvalueX = abs(GyroXBuffer.getPreviousElement(1));
  if(controlvalueX>180){
     controlvalueX = 180;}      
  x = map(controlvalueX, 0, 180, 0, 127); // scale to midi range
  int ccmProcessed = formatCCM(x,15); ///Yaw
  SendCCM(ccmProcessed); //CHECK 
  //SendToReceiver(formatCCM(x,15));
  delay(2);
}

void processY() {
  int y, controlvalueY;
  controlvalueY = GyroYBuffer.getPreviousElement(1);      
  y = map(controlvalueY, -90, 90, 0, 127); 
  int ccmProcessed = formatCCM(y,16);//Pitch
  SendCCM(ccmProcessed);
  //SendToReceiver(formatCCM(y,16));//CHECK 
  delay(2);
}

void processZ() {
  int z, controlvalueZ;
  controlvalueZ = GyroZBuffer.getPreviousElement(1);
  z = map(controlvalueZ, -90, 90, 0, 127); 
  int ccmProcessed = formatCCM(z,17);//Roll
  SendCCM(ccmProcessed);
  //SendToReceiver(formatCCM(z,17));//CHECK 
  delay(2);
}

void ProcessCCM() {
  if(isRollingX){
    processX();///EMITE CCM MAPEADO
    }
  if(isRollingY){
    processY();///EMITE CCM MAPEADO
    }
  if(isRollingZ){
    processZ();///EMITE CCM MAPEADO
    }
}
/////END CCM Messagge (send ccm if moving)


////CAXIXI CCM PROGRAM
void runCCM() {
  
  if(caxixiRight){
    currentAccelY = accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    ButtonOctaveUp();
  } else {
    currentAccelY = - accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    ButtonOctaveDown();
  }

  ccmNotes();//SEND Caxixi CCM Notes 
  areRolling(); //check if Gyro Moving
  ProcessCCM(); //send ccm if moving

  //Debug
  //Serial.println();
  }
