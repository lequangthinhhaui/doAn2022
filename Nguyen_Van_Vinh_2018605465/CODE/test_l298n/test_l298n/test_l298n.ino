// the number of the LED pin
const int ledPin = 14;  // coresponse to l2
const int ledPin2 = 27; // 17 corresponds to GPIO17

// setting PWM properties
const int freq = 10000;
const int ledChannel = 0;
const int ledChannel1 = 1;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
pinMode(14, OUTPUT);
pinMode(27, OUTPUT);
 

}
 
void loop(){
 digitalWrite(14, HIGH);
  digitalWrite(27, LOW);

  delay(2000);

   digitalWrite(14, LOW);
  digitalWrite(27, HIGH);

  delay(2000);

}
