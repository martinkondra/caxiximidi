int roleSET = 0; // role =0 receiver, =1 sender
bool radioNumber = roleSET;
// Hardware configuration:  nRF24L01 SPI bus pins 
RF24 radio(5,6); //RF24 radio(CE -NRF, CSN +NRF)
byte addresses[][6] = {"1Node","2Node"};
bool role = roleSET;
