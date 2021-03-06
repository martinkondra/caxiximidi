// Incluida en el loop de runCaxixi() (después de setState()) e importar debug.h antes del loop
int prevMillis = 0;
int currentMillis = 0;

#include "MIDIUSB.h"
int ppqn = 0;


////debug on serial monitor
void SendToDebug(int msg) {
  Serial.print(msg);
  Serial.print(";");
}

void debug() {
  //SendToDebug(readClock());
  currentMillis = millis();
  int difMillis = currentMillis- prevMillis;
  SendToDebug(difMillis);
  prevMillis = currentMillis;//Save for next loop
  SendToDebug(currentAccelX);
  SendToDebug(currentAccelY);
  //SendToDebug(abs(currentAccelX - accelXBuffer.getPreviousElement(8)));//slope x
  //SendToDebug(abs(currentAccelY - accelYBuffer.getPreviousElement(8)));//slope y
  SendToDebug(state);//Debug Cambio de estado para send Note HIT
  //SendToDebug(GYROZ);//DEBUG ANGLES
  //SendToDebug(GYROX);//DEBUG ANGLES
  //SendToDebug(GYROY);//DEBUG ANGLES
  Serial.println();
}
