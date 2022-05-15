#include <Servo.h>


//khai bao cac servo su dung
Servo servo1;
Servo servo2;
String readString;

//khai bao bien

const int ledPin = 27;  // coresponse to l2
const int ledPin2 = 14; // 17 corresponds to GPIO17


//servo goc
int sv1AngleFirst = 0;
int sv1AngleLast = 150;

int sv2AngleFirst = 0;
int sv2AngleLast = 150;

//servo chan
int servo1Pin = 10;
int servo2Pin = 6;


//define function
void servoInit();

void setup() {
  servoInit();
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
}

bool flagsr1 = false;
void loop() 
{
  while(!Serial.available()) {}
  // serial read section
  while (Serial.available())
  {
    if (Serial.available() >0)
    {
      String c = Serial.readStringUntil('@');
      Serial.println(c);
      if(c == "Sản phẩm 01")
      {
        digitalWrite(13, LOW);                          
        servo2.write(50);               
      }
      else if(c == "Sản phẩm 02")
        digitalWrite(13, HIGH);
        digitalWrite(13, LOW);                            
        servo2.write(0);           
        delay(1000);
    }
  }

//  analogWrite(3, 0);
//  analogWrite(5, 200);
//  if(digitalRead(8) == LOW)
//  {
////      analogWrite(3, 0);
////      analogWrite(5, 0);
////      Serial.println("stop");
//    for(sv1AngleFirst = 0; sv1AngleFirst < 180; sv1AngleFirst++)  
//    {                                  
//      servo2.write(sv1AngleFirst);               
//      delay(2);                   
//    }
//    delay(5000); 
//  }
//  else
//      servo2.write(0); 
  
//   delay(1000);
// // scan from 0 to 180 degrees

//  for(sv1AngleFirst = 0; sv1AngleFirst < 180; sv1AngleFirst++)  
//  {                                  
//    servo2.write(0);               
//    delay(15);                   
//  } 
//      servo2.write(0);
//  // now scan back from 180 to 0 degrees

//
//  for(sv1AngleFirst = 10; sv1AngleFirst < 180; sv1AngleFirst++)  
//  {                                  
//    servo1.write(sv1AngleFirst);               
//    delay(15);                   
//  } 
//  // now scan back from 180 to 0 degrees
//  for(sv1AngleFirst = 180; sv1AngleFirst > 10; sv1AngleFirst--)    
//  {                                
//    servo1.write(sv1AngleFirst);           
//    delay(15);       
//  } 
//   analogWrite(3, 200);
//   analogWrite(5, 0);
//   delay(1000);

  
}

//function
//servo init
void servoInit()
{
  //servo 1 init
  servo1.attach(servo1Pin);
  servo1.write(sv1AngleFirst); 
  //servo 2 init
  servo2.attach(servo2Pin);
  servo2.write(sv2AngleFirst); 
}
