#define MIDI_CHANNEL 1

#define NOTE_FORWARD 0
#define NOTE_BACKWARD 1
#define NOTE_HIT 2
#define NOTE_OFF 5

// SENSOR_BEGIN
#define SENSOR_ACCEL_X 0
#define SENSOR_ACCEL_Y 1
#define SENSOR_ACCEL_Z 2

// SmoothFilter Samples
#define filterSamples 5
#define BUFFER_SIZE 8

#define SLOPE_STILL_X_RANGE 20
#define SLOPE_STILL_Y_RANGE 50
#define SLOPE_STILL_STATUS 0
#define SLOPE_MOVING_STATUS 1

#define FORCE_STILL 0
#define FORCE_FORWARD 1
#define FORCE_BACKWARD 2
#define FORCE_THRESHOLD_FORWARD 400
#define FORCE_THRESHOLD_BACKWARD -300
#define FORCE_STILL_RANGE_FORWARD 300
#define FORCE_STILL_RANGE_BACKWARD -100

#define STATE_STILL 0
#define STATE_FORWARD 1
#define STATE_BACKWARD 2

#define RESOLUTION 1

#define SENSOR_NOTE_FORWARD 43
#define SENSOR_NOTE_BACKWARD 45
#define SENSOR_NOTE_HIT 47

#define NOTE_RELEASE_FORWARD 100
#define NOTE_RELEASE_BACKWARD -100
#define NOTE_RELEASE_HIT 600

#define NOTE_THRESHOLD_HIT 600