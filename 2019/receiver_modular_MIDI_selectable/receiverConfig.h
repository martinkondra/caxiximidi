const int MIDI_CHANNEL = 1;

//PINES Caxixi Receiver 
#define OCTAVE_DOWN_LED_GREEN_PIN 10
#define OCTAVE_DOWN_LED_RED_PIN 9
#define OCTAVE_UP_LED_GREEN_PIN 11 
#define OCTAVE_UP_LED_RED_PIN 12 
#define RECORD_LED_PIN 3
// SAMPLER (Variables)
#define SAMPLER_BUFFER_SIZE 100

///MENSAJES INT via nRF24L01+
const int CAXIXI_RIGHT_FORWARD_NOTEON = 111;
const int CAXIXI_RIGHT_FORWARD_NOTEOFF = 110;
const int CAXIXI_RIGHT_BACKWARD_NOTEON = 121;
const int CAXIXI_RIGHT_BACKWARD_NOTEOFF = 120;
const int CAXIXI_RIGHT_HIT_NOTEON = 131;
const int CAXIXI_RIGHT_HIT_NOTEOFF = 130;
const int CAXIXI_LEFT_FORWARD_NOTEON = 211;
const int CAXIXI_LEFT_FORWARD_NOTEOFF = 210;
const int CAXIXI_LEFT_BACKWARD_NOTEON = 221;
const int CAXIXI_LEFT_BACKWARD_NOTEOFF = 220;
const int CAXIXI_LEFT_HIT_NOTEON = 231;
const int CAXIXI_LEFT_HIT_NOTEOFF = 230;
const int CAXIXI_OCTAVE_UP = 401;
const int CAXIXI_OCTAVE_DOWN = 400;
const int CAXIXI_RECORD_STOP = 500;
const int CAXIXI_RECORD_START = 501;
const int CAXIXI_SAMPLER_CLEAR = 502;

////MIDI CENTRAL NOTES
#define CAXIXI_RIGHT_NOTE_FORWARD 43
#define CAXIXI_RIGHT_NOTE_BACKWARD 45
#define CAXIXI_RIGHT_NOTE_HIT 47
#define CAXIXI_LEFT_NOTE_FORWARD 36
#define CAXIXI_LEFT_NOTE_BACKWARD 38
#define CAXIXI_LEFT_NOTE_HIT 40

struct Buffer_s {
  int note;
  int layer;  //capa de sobregrabación. cada vez que se prende "record", suma 1.
  bool encendido;// ???on/off
  long time;
} Buffer_default = {0,0,0,9999999};
typedef struct Buffer_s Buffer;
