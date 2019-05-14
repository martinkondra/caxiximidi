void showLeds(){
switch(currentOctave){
    case 0: analogWrite(OCTAVE_UP_LED_RED_PIN,255);
            analogWrite(OCTAVE_UP_LED_GREEN_PIN,0);
            analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
            analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,0);
    break;
    case 1: analogWrite(OCTAVE_UP_LED_RED_PIN,0);
            analogWrite(OCTAVE_UP_LED_GREEN_PIN,0);
            analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
            analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,255);
    break;
    case 2: analogWrite(OCTAVE_UP_LED_RED_PIN,0);
            analogWrite(OCTAVE_UP_LED_GREEN_PIN,75);
            analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
            analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,255);  
    break;
    case 3: analogWrite(OCTAVE_UP_LED_RED_PIN,0);
            analogWrite(OCTAVE_UP_LED_GREEN_PIN,255);
            analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
            analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,255);
    break;
    
    case -1: analogWrite(OCTAVE_UP_LED_RED_PIN,255);
             analogWrite(OCTAVE_UP_LED_GREEN_PIN,255);
             analogWrite(OCTAVE_DOWN_LED_RED_PIN,0);
             analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,0); 
    break;
    case -2: analogWrite(OCTAVE_UP_LED_RED_PIN,255);
             analogWrite(OCTAVE_UP_LED_GREEN_PIN,255);
             analogWrite(OCTAVE_DOWN_LED_RED_PIN,0);
             analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,75);
    break;
    case -3: analogWrite(OCTAVE_UP_LED_RED_PIN,255);
             analogWrite(OCTAVE_UP_LED_GREEN_PIN,255);
             analogWrite(OCTAVE_DOWN_LED_RED_PIN,0);
             analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,255);
    break;
    }  
}

void TurnOffAll(){
  analogWrite(OCTAVE_UP_LED_RED_PIN,255);
  analogWrite(OCTAVE_UP_LED_GREEN_PIN,255);
  analogWrite(OCTAVE_DOWN_LED_RED_PIN,255);
  analogWrite(OCTAVE_DOWN_LED_GREEN_PIN,255);
}
