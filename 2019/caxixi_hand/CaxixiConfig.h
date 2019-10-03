bool debugSerialMonitor = false; ///For Debug (via SerialMonitor in debugCaxixi.h corriendo Caixi.h)

// Para programar la mano izquierda, poner CAXIXI_RIGHT en false, comentar el primer bloque y descomentar el segundo
bool CAXIXI_RIGHT = false;

/*
///CAXIXI RIGHT NOTES
const int CAXIXI_FORWARD_NOTEON = 111;
const int CAXIXI_FORWARD_NOTEOFF = 110;
const int CAXIXI_BACKWARD_NOTEON = 121;
const int CAXIXI_BACKWARD_NOTEOFF = 120;
const int CAXIXI_HIT_NOTEON = 131;
const int CAXIXI_HIT_NOTEOFF = 130;
///*/

///CAXIXI LEFT NOTES
const int CAXIXI_FORWARD_NOTEON = 211;
const int CAXIXI_FORWARD_NOTEOFF = 210;
const int CAXIXI_BACKWARD_NOTEON = 221;
const int CAXIXI_BACKWARD_NOTEOFF = 220;
const int CAXIXI_HIT_NOTEON = 231;
const int CAXIXI_HIT_NOTEOFF = 230;
////////

const int CAXIXI_OCTAVE_UP = 401;
const int CAXIXI_OCTAVE_DOWN = 400;
const int CAXIXI_RECORD_STOP = 500;
const int CAXIXI_RECORD_START = 501;
const int CAXIXI_SAMPLER_CLEAR = 502;

const int MIDI_CHANNEL = 1;

//PINES 
//Caxixi Left
#define OCTAVE_DOWN_BUTTON_PIN 12 // OCTAVE DOWN
#define SAMPLER_BUTTON_CLEAR_PIN 11//Sampler Clear
//Caxixi Right
#define OCTAVE_UP_BUTTON_PIN 11 // OCTAVE UP
#define SAMPLER_BUTTON_RECORD_PIN 12 //Sampler Rec
//Caxixi Receiver 
#define OCTAVE_DOWN_LED_GREEN_PIN 10
#define OCTAVE_DOWN_LED_RED_PIN 9
#define OCTAVE_UP_LED_GREEN_PIN 11 
#define OCTAVE_UP_LED_RED_PIN 12 
#define RECORD_LED_PIN 3

// SAMPLER (Variables)
#define SAMPLER_BUFFER_SIZE 100

// SENSOR_BEGIN
const int SENSOR_ACCEL_X = 0;
const int SENSOR_ACCEL_Y = 1;
const int SENSOR_ACCEL_Z = 2;
//////Los siguientes son para usar los valores de acelerometro para el ccm
const int SENSOR_GYRO_Y = 3;
const int SENSOR_GYRO_X = 4;
const int SENSOR_GYRO_Z = 5;

// SmoothFilter Samples
#define filterSamples 5
#define BUFFER_SIZE 8

///VARIALES SLOPE STATUS
const int SLOPE_STILL_X_RANGE = 500;
const int SLOPE_STILL_Y_RANGE = 500;

//VARIALBES FORCE STATUS
const int FORCE_THRESHOLD_FORWARD = 3000;
const int FORCE_THRESHOLD_BACKWARD = -2000;
const int FORCE_STILL_RANGE_FORWARD = 2000;
const int FORCE_STILL_RANGE_BACKWARD = -1500;

//// THRESHOLD_HIT EJE Y
const int NOTE_THRESHOLD_HIT = 2000;

///RELEASE THRESHOLD
const int NOTE_RELEASE_FORWARD = 1000;
const int NOTE_RELEASE_BACKWARD = -1000;
const int NOTE_RELEASE_HIT = 1000;

//VARIABLES CAXIXI STATE MACHINE
//STATE
const int STATE_STILL = 0;
const int STATE_FORWARD = 1;
const int STATE_BACKWARD = 2;
//FORCE
const int FORCE_STILL = 0;
const int FORCE_FORWARD = 1;
const int FORCE_BACKWARD = 2;
//SLOPE
const int SLOPE_STILL_STATUS = 0;
const int SLOPE_MOVING_STATUS = 1;

const int NOTE_FORWARD = 0;
const int NOTE_BACKWARD = 1;
const int NOTE_HIT = 2;
const int NOTE_OFF = 5;
const int RESOLUTION = 1;

//MIDI CENTRAL NOTES
#define CAXIXI_RIGHT_NOTE_FORWARD 43
#define CAXIXI_RIGHT_NOTE_BACKWARD 45
#define CAXIXI_RIGHT_NOTE_HIT 47
#define CAXIXI_LEFT_NOTE_FORWARD 36
#define CAXIXI_LEFT_NOTE_BACKWARD 38
#define CAXIXI_LEFT_NOTE_HIT 40
///MIDI CCM CONTROL NUMBERS
const int CAXIXI_CCM_X = 13; //19  to have full 7 octaves
const int CAXIXI_CCM_Y = 14; //20  to have full 7 octaves
const int CAXIXI_CCM_Z = 15; //21  to have full 7 octaves


//Buffer en Receiver
struct Buffer_s {
  int note;
  int layer;  //capa de sobregrabación. cada vez que se prende "record", suma 1.
  bool encendido;// on/off
  long time;
} Buffer_default = {0,0,0,9999999};

typedef struct Buffer_s Buffer;
