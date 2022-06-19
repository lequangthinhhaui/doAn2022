#include <Servo.h>  //khai báo thư viện servo


//Khai báo 2 servo sử dụng là servo 1 và servo 2
Servo servo1;
Servo servo2;


String readString;

//khai bao bien

const int PWM1 = 5;  
const int PWM2 = 3; 



#define denPin A0
#define camBienPin 8
//servo goc
int sv1AngleFirst = 0;
int sv2AngleFirst = 0;


//khai báo chân servo
int servo1Pin = 10;
int servo2Pin = 6;


//định nghĩa function servo init
void servoInit();

void setup() {
  servoInit();
  pinMode(denPin, OUTPUT); //khai báo đầu ra cho đèn
  pinMode(camBienPin, INPUT);//khai báo đầu vào cho cảm biến
  Serial.begin(9600);


//cho băng tải chạy khi bật nguồn
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 200);
}

void loop() 
{
  if(digitalRead(camBienPin) == LOW)//gặp cảm biến
  {
      //cho dừng băng tải
      analogWrite(PWM1, 0);
      analogWrite(PWM2, 0);

      //chờ nhận đọc được dữ liệu từ camera
      while(!Serial.available()) {}
      while (Serial.available())
      {
        if (Serial.available() >0) //nhận được
        {
          //cho băng tải chạy khi nhận được dữ liệu qr code
          analogWrite(PWM1, 0);
          analogWrite(PWM2, 200);
          
          String c = Serial.readStringUntil('@');
          Serial.println(c);
          
          if(c == "Sản phẩm 01")
          {
            digitalWrite(13, LOW);
            servo1.write(50);   //gat servo 1                       
            servo2.write(0);
            delay(10000);  //đợi phân loại xong
            servo1.write(0);   //thu tay gạt về                       
            servo2.write(0);        
          }
          
          else if(c == "Sản phẩm 02")
          {
            digitalWrite(A0, HIGH);                           
            servo1.write(0);                          
            servo2.write(50);    //gat servo 2         
            delay(10000); //đợi phân loại xong
            servo1.write(0);   //thu tay gạt về                       
            servo2.write(0);                 
          }
          else
          {
            //ko gạt servo
            servo1.write(0);                          
            servo2.write(0); 
            delay(10000);//đợi phân loại xong           
          }
        }
      }
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
