///CCM Config . Configurar tipo de funcionamiento. (Ej:Nota hold si o no)
bool NotaHOLD = true; ///true cuando se quiere prender o apagar la nota con cada hit
//Variables Threshold
int CCM_NOTE_THRESHOLD_HIT = 2000; //Umbral para mandar midiOn/Off
int CCM_HOLDING_NOTE_THRESHOLD_HIT = 4000;
int CCM_NOTE_RELEASE = 1000;
int rollMovingThreshold = 5; //DIFERENCIA ABSOLUTA GYROBUFFER Threshold XYZ 
////////////////////////////


//Format CCM Message
int msg; //msg to send 
int formatCCM(int NUM, int CH) {
  msg = CH*1000 + NUM;
  return msg;
}
///End of Format CCM Message

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

////STATES 
bool Hit_State = false; // (Begin out of hit, expecting Hit) Usar para saber cuando se puede enviar notas. On en Hit Lapsus hasta ReleaseHit
bool HitCCM_NoteOn = false; // NoteOn State
 
boolean isRollingX;
String debugRollingX;
boolean isRollingY;
String debugRollingY;
boolean isRollingZ;
String debugRollingZ;

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
//NEW ccmNotes()
void ccmNotes() {
  if(!NotaHOLD){//MODO CLASIC HIT EN EJE Y
    //Hit_State, if inside Hit Lapsus before HitReleased
   
    ////SEND NOTE ON
    if (currentAccelY > CCM_NOTE_THRESHOLD_HIT & HitCCM_NoteOn == false){ //Encima del umbral CCM & Out of Hit Lapsus
       SendNoteOn(CAXIXI_HIT_NOTEON);
       HitCCM_NoteOn = true;
    }
    ///SEND NOTE OFF
    if (currentAccelY < CCM_NOTE_RELEASE & HitCCM_NoteOn == true){ //Cada situacion ReleaseHit habilita el envio de notas via NoteState
      SendNoteOff(CAXIXI_HIT_NOTEOFF);
      HitCCM_NoteOn = false;
    }
  
  }else{//NotaHOLD True //THRESHOLD diferente (mas alto)
    //Hit_State = true inside Hit Lapsus before relaesed. Se usa para saber cuando se puede enviar una nota sea On u Off
    //HitCCM_NoteOn para cuando esta la nota prendida
    if (currentAccelY > CCM_HOLDING_NOTE_THRESHOLD_HIT & Hit_State == false){
      if (!HitCCM_NoteOn){ //If its OFF send NoteOn
        SendNoteOn(CAXIXI_HIT_NOTEON);
        Hit_State = true;
        HitCCM_NoteOn = true;
      }else{
        SendNoteOff(CAXIXI_HIT_NOTEOFF);
        Hit_State = true;
        HitCCM_NoteOn = false;
      }
    }
    if (currentAccelY < CCM_NOTE_RELEASE & Hit_State == true){//Cada situacion ReleaseHit habilita el envio de notas via NoteState (Hit Lapsus)
      Hit_State = false;
    }
  }
}  
/////END OF ccmNotes()

//CAXIXI CCM Control Changes
//Define CCM numbers 
void defCMMnumbers(){
  if (!CAXIXI_RIGHT){
    xccm = CAXIXI_CCM_X;
    yccm = CAXIXI_CCM_Y;
    zccm = CAXIXI_CCM_Z;
  }else{
    xccm = CAXIXI_CCM_X + 3;
    yccm = CAXIXI_CCM_Y + 3;
    zccm = CAXIXI_CCM_Z + 3;
    }
}

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

void areRolling() {
  setIsRollingX();
  setIsRollingY();
  setIsRollingZ();  
}
///END OF Check Movement GYRO


/////ProcessCCM  (send ccm if moving)
//Wrong accel "name" x->z, y->x, z->y (YawPitchRoll)
void processX() {
  int x, controlvalueX;
  controlvalueX = abs(GyroXBuffer.getPreviousElement(1));
  if(controlvalueX>180){
     controlvalueX = 180;}      
  x = map(controlvalueX, 0, 180, 0, 127); // scale to midi range
  int ccmProcessed = formatCCM(x,xccm); ///Yaw
  SendCCM(ccmProcessed); //CHECK 
  //SendToReceiver(formatCCM(x,15));
  delay(2);
}
void processY() {
  int y, controlvalueY;
  controlvalueY = GyroYBuffer.getPreviousElement(1);      
  y = map(controlvalueY, -90, 90, 0, 127); 
  int ccmProcessed = formatCCM(y,yccm);//Pitch
  SendCCM(ccmProcessed);
  //SendToReceiver(formatCCM(y,16));//CHECK 
  delay(2);
}
void processZ() {
  int z, controlvalueZ;
  controlvalueZ = GyroZBuffer.getPreviousElement(1);
  z = map(controlvalueZ, -90, 90, 0, 127); 
  int ccmProcessed = formatCCM(z,zccm);//Roll
  SendCCM(ccmProcessed);
  //SendToReceiver(formatCCM(z,17));//CHECK 
  delay(2);
}

void ProcessCCM() {
  if(isRollingX){
    processX();///EMITE CCM MAPEADO 360º (0-127)
    }
  if(isRollingY){
    processY();///EMITE CCM MAPEADO 360º (0-127)
    }
  if(isRollingZ){
    processZ();///EMITE CCM MAPEADO 360º (0-127)
    }
}
/////END CCM Messagge (send ccm if moving)


////CAXIXI CCM runCCM()
void runCCM() { 
  if(caxixiRight){
    currentAccelY = accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    ButtonOctaveUp();
    ButtonRecord(); //CHEQUEAR SI FUNCIONA
  } else {
    currentAccelY = - accelYBuffer.getPreviousElement(1);
    currentAccelX = accelXBuffer.getPreviousElement(1);
    ButtonOctaveDown();
    ButtonClear(); //CHEQUEAR SI FUNCIONA
  }

  ccmNotes();//SEND Caxixi CCM Notes 
  areRolling(); //check if Gyro Moving
  ProcessCCM(); //send ccm if moving maped 360º (0-127)

}
///
