#include <Servo.h>


//khai bao cac servo su dung
Servo servo1;
Servo servo2;

//khai bao bien

//servo goc
int sv1AngleFirst = 0;
int sv1AngleLast = 150;

int sv2AngleFirst = 0;
int sv2AngleLast = 150;

//servo chan
int servo1Pin = 10;
int servo2Pin = 8;


//define function
void servoInit();

void setup() {
  servoInit();
}


void loop() 
{ 
 // scan from 0 to 180 degrees
  for(sv1AngleFirst = 10; sv1AngleFirst < 180; sv1AngleFirst++)  
  {                                  
    servo2.write(sv1AngleFirst);               
    delay(15);                   
  } 
  // now scan back from 180 to 0 degrees
  for(sv1AngleFirst = 180; sv1AngleFirst > 10; sv1AngleFirst--)    
  {                                
    servo2.write(sv1AngleFirst);           
    delay(15);       
  } 
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
