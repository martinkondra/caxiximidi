

// pins for the LEDs:
const int redREC_Pin = 3;
const int greenUP_Pin = 11;
const int redUP_Pin = 12;
const int greenDW_Pin = 10;
const int redDW_Pin = 9;


int red;
int green;


void setup() {
  // initialize serial:
  Serial.begin(9600);
  // make the pins outputs:
  pinMode(redREC_Pin, OUTPUT);
  pinMode(redUP_Pin, OUTPUT);
  pinMode(greenUP_Pin, OUTPUT);
  pinMode(redDW_Pin, OUTPUT);
  pinMode(greenDW_Pin, OUTPUT);

}

void loop() {
  red = 255;
  green = 255;
      analogWrite(redREC_Pin, red);
      analogWrite(redUP_Pin, red);
      analogWrite(greenUP_Pin, green);
      analogWrite(redDW_Pin, red);
      analogWrite(greenDW_Pin, green);

 delay (1000);

 red=0;
 green=255;
 
      analogWrite(redREC_Pin, red);
      analogWrite(redUP_Pin, red);
      analogWrite(greenUP_Pin, green);
      analogWrite(redDW_Pin, red);
      analogWrite(greenDW_Pin, green);
 delay (1000); 
 
 red=255;
 green=0;
 
      analogWrite(redREC_Pin, red);
      analogWrite(redUP_Pin, red);
      analogWrite(greenUP_Pin, green);
      analogWrite(redDW_Pin, red);
      analogWrite(greenDW_Pin, green);
 delay (1000); 
 
  red = 0;
  green = 0;
      analogWrite(redREC_Pin, red);
      analogWrite(redUP_Pin, red);
      analogWrite(greenUP_Pin, green);
      analogWrite(redDW_Pin, red);
      analogWrite(greenDW_Pin, green);

 delay (1000);
 //delay(tap/tempo);
}
