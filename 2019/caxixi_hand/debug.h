// incluir la fc debug() en el loop de runCaxixi() (después de setState()) e importar debug.h antes del loop

#include "MIDIUSB.h"
int ppqn = 0;

void SendToDebug(int msg) {
  Serial.print(msg);
  Serial.print(";");
}

int readClock() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if(rx.byte1 == 0xF8){
       ++ppqn;
       if(ppqn == 24){     
          ppqn = 0;
       };
    }
    else if(rx.byte1 == 0xFA) { //Clock start byte
      ppqn = 0;
    }
    else if(rx.byte1 == 0xFC) { //Clock stop byte
      ppqn = 0;
    };
  } while (rx.header != 0);
  return ppqn;
}

void debug() {
  SendToDebug(readClock());
  SendToDebug(currentAccelX);
  SendToDebug(currentAccelY);
  //SendToDebug(abs(currentAccelX - accelXBuffer.getPreviousElement(8)));//slope x
  //SendToDebug(abs(currentAccelY - accelYBuffer.getPreviousElement(8)));//slope y
  SendToDebug(state);//Estado
  Serial.println();
}
