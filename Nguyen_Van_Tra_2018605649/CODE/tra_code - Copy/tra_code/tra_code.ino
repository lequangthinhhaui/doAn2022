
//định nghĩa chân cảm biến khoảng cách sr04 4, 5
#define echoPin 4 
#define trigPin 5 

#include <Servo.h> //hàm điều khiển servo
#include <LiquidCrystal_I2C.h> //hàm hiển thị lcd


//khai báo thư viện
#include "SoftwareSerial.h" //giao tiếp với mp3
#include "DFRobotDFPlayerMini.h" //thư viện của module mp3


//khai báo chân giao tiếp giữa arduino và module mp3
static const uint8_t PIN_MP3_TX = 2; 
static const uint8_t PIN_MP3_RX = 3; 

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX); //khai báo giao tiếp giữa module mp3 và arduino

DFRobotDFPlayerMini player; //khởi tạo player điều khiển phát nhạc



Servo servo1; //khai báo servo 1

LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd i2c 16x2

//khai báo các biến sử dụng
long duration; //biến thời gian truyền nhận

int distance = 11;
 
int servoPin = 10; //chân của servo là chân 10

int sensorPin = 8; //chân sensor là chân 8

int modePin = 7; //chân công tắc gạt

int nguongFull = 10; //đặt ngưỡng mà thùng rác đầy

void setup() {
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed

//khởi tạo giao tiếp giữa arduino với module mp3
    softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");
  }
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");

  //khởi tạo servo 1
  servo1.attach(servoPin);
  servo1.write(0);

//lcd init
  lcd.init();
  // bat led nen                     
  lcd.backlight();
}

bool flagFull = false; //cờ báo full


void loop() {


if(digitalRead(modePin) == LOW)
{
  if(digitalRead(8) == LOW && distance >= nguongFull)
  {                           
      servo1.write(180);                                  
      lcd.setCursor(0, 0);
      lcd.print("Bo rac vao thung");
      player.volume(30);
      player.play(1);
      delay(4000);
      lcd.setCursor(0, 1);
      lcd.print("Xin cam on !   ");
      delay(2000);  
      player.volume(30);
      player.play(2);
      delay(1000); 
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
      if(digitalRead(8) == LOW)
      {
        player.volume(30);
        player.play(3);
        delay(3000);           
      }
    }
    else
    {
      flagFull = false;
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

//hàm đo thùng rác đã đầy hay chưa
void readDistance(int &distance)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // đo thời gian truyền nhận
  distance = duration * 0.034 / 2;  //tính khoảng cách theo công tức s/2 = v*t
  delay(200);
}
