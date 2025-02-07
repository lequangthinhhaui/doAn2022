

#define echoPin 4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D3 Arduino to pin Trig of HC-SR04

#include <Servo.h>
#include <LiquidCrystal_I2C.h>


//khai bao cac servo su dung
Servo servo1;
LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd i2c 16x2

// defines variables
long duration; // variable for the duration of sound wave travel
int distance = 11; 
int servoPin = 10;
int sensorPin = 8;
int modePin = 7;

int nguongFull = 10;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  servo1.attach(servoPin);
  servo1.write(0);

//lcd init
  lcd.init();
  // bat led nen                     
  lcd.backlight();
}
void loop() {

if(digitalRead(modePin) == LOW)
{
  if(digitalRead(8) == LOW && distance >= nguongFull)
  {
    for(int i = 0; i < 180; i++)  
    {                                  
      servo1.write(i);               
      delay(2);                   
    }
      lcd.setCursor(0, 0);
      lcd.print("Bo rac vao thung");
      delay(5000);
      lcd.setCursor(0, 1);
      lcd.print("Xin cam on !   ");
      delay(3000);  
  }
  else
  {
    
    readDistance(distance);
    if(distance < nguongFull)
    {
      lcd.setCursor(0, 0);
      servo1.write(0);
      lcd.print("Thung rac da day");
      lcd.setCursor(0, 1);
      lcd.print("Vui long do rac");       
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Bo rac vao thung");
      lcd.setCursor(0, 1);
      lcd.print("Xanh-Sach-Dep     ");     
      servo1.write(0);
      readDistance(distance);
      Serial.println(distance);          
    } 
  }  
}
else
{
  lcd.setCursor(0, 0);
  //in ra man hinh
  lcd.print("Thung rac da day");
  lcd.setCursor(0, 1);
  //in ra man hinh
  lcd.print("Vui long do rac");     
  servo1.write(180);   
}   
}

void readDistance(int &distance)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; 
  delay(200);
}
