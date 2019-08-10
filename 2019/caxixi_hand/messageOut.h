

//

void SendToReceiver(int msg) {///Send msge NRF24
  radio.write( &msg, sizeof(int));
}


////MENSAJES NOTES EMITIDOS
///MENSAJES HIT/NOTES
void SendNoteOn(int note) {
  SendToReceiver(note);
}
void SendNoteOff(int note) {
  SendToReceiver(note);
}

///MENSAJES CCM
void SendCCM(int ccmProcessed){
	SendToReceiver(ccmProcessed);	
}

//MENSAJES BOTONES
