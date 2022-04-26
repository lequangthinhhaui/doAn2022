// the number of the LED pin
const int ledPin = 27;  // coresponse to l2
const int ledPin2 = 14; // 17 corresponds to GPIO17

// setting PWM properties
const int freq = 10000;
const int ledChannel = 0;
const int ledChannel1 = 1;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(ledPin2, ledChannel1);

  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(23, OUTPUT);

  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  digitalWrite(33, LOW);
  digitalWrite(32, LOW);
  digitalWrite(23, LOW);
}
 
void loop(){
  // increase the LED brightness
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  digitalWrite(33, LOW);
  digitalWrite(32, LOW);
  digitalWrite(23, LOW);
  ledcWrite(ledChannel, 200);
//  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
//    // changing the LED brightness with PWM
//    ledcWrite(ledChannel, dutyCycle);
//    ledcWrite(ledChannel1, 0);
//    delay(15);
//  }
  delay(1000);
  digitalWrite(26, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(32, HIGH);
  digitalWrite(23, HIGH);
  delay(1000);

}
