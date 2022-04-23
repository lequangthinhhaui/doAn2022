// the number of the LED pin
const int ledPin1 = 12;  // 16 corresponds to GPIO16
const int ledPin2 = 13; // 17 corresponds to GPIO17
const int ledPin3 = 14;  // 5 corresponds to GPIO5
const int ledPin4 = 27; 
// setting PWM properties
const int freq = 5000;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(1, freq, resolution);
  ledcSetup(2, freq, resolution);
  ledcSetup(3, freq, resolution);
  ledcSetup(4, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin1, 1);
  ledcAttachPin(ledPin2, 2);
  ledcAttachPin(ledPin3, 3);
  ledcAttachPin(ledPin4, 4);
}
 
void loop(){
  // increase the LED brightness
//  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
//    // changing the LED brightness with PWM
//    ledcWrite(0, dutyCycle);
//    delay(15);
//  }
//
//  delay(2000);
//  // decrease the LED brightness
//  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
//    // changing the LED brightness with PWM
//    ledcWrite(1, dutyCycle);   
//    delay(15);
//  }
//
//   delay(2000);
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(3, dutyCycle);
    delay(200);
  }
//
//   delay(2000);
   // decrease the LED brightness
//  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
//    // changing the LED brightness with PWM
//    ledcWrite(2, dutyCycle);   
//    delay(200);
//  }

   delay(2000);

    
}
