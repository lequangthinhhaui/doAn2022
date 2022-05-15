#include <Servo.h> // Khai báo thư viện Servo  
#include <Wire.h> //
#include <LiquidCrystal_I2C.h>// Khai báo thư viện LCD



Servo myservo1; //
Servo myservo2; //
Servo myservo3; // Khai báo biến Servo

int Bangchuyen = 8; // Khai báo chân băng chuyền

int sensorCapPhoi = 10;
int sensorThap = 13;
int sensorTrungBinh = 12;
int sensorCao = 11;

int thapCounter = 0;
int trungBinhCounter = 0;
int caoCounter = 0;

int preThap = 0;
int preTrungBinh = 0;
int preCao = 0;

LiquidCrystal_I2C lcd(0x27,16,2); // Chạy biến của LCD


void setup() {
  pinMode(Bangchuyen,OUTPUT); // Khai báo đầu ra
  myservo1.attach(9); // 
  myservo2.attach(6); //
  myservo3.attach(5); // Chọn chân kết nối servo ( chân 5,6,9 đều là chân có thể PWM )
  myservo1.write(150); //
  myservo2.write(150); // 
  myservo3.write(150); // Vị trí số 150 là vị trí gốc
  lcd.init();
  lcd.backlight(); 

  
  pinMode(sensorCapPhoi, INPUT);
  pinMode(sensorThap, INPUT);
  pinMode(sensorTrungBinh, INPUT);
  pinMode(sensorCao, INPUT);


  Serial.begin(9600);
  updateValueLCD();
}

int preTime = 0;
int lastTime = 0;
int timeDelay = 0;
void loop() {


  while(1)
  {
    preTime = millis();
    digitalWrite(Bangchuyen,HIGH);
    for(int i = 0; i<30000; i++)
    {
      if(digitalRead(sensorThap)== 0)
      {
        preThap++;
      }
      if(digitalRead(sensorTrungBinh)== 0)
      {
        preTrungBinh++;
      }
      if(digitalRead(sensorCao)== 0)
      {
        preCao++;
      }
    }
    timeDelay = millis()-preTime;
    Serial.println("*********************");
    Serial.println(timeDelay);
    Serial.println(preThap);
    Serial.println(preTrungBinh);
    Serial.println(preCao);
    if(preThap>0 && preTrungBinh == 0 && preCao ==0)
    {
      Serial.println("thap");
      myservo2.write(80); 
      myservo3.write(150);
      thapCounter++;
      updateValueLCD();
      Serial.println(thapCounter);
      delay(10000);
      myservo2.write(150);
      myservo3.write(150);   
      preThap = 0; 
      preTrungBinh = 0;
      preCao = 0;   
    }
    if(preThap>0 && preTrungBinh>0 && preCao ==0)
    {
      Serial.println("TB");
      myservo3.write(70); 
      myservo2.write(150);
      trungBinhCounter++;
      updateValueLCD();
      Serial.println(trungBinhCounter);  
      delay(10000);
      myservo2.write(150);
      myservo3.write(150);  
      preThap = 0; 
      preTrungBinh = 0;
      preCao = 0;   
    }
    if(preThap>0 && preTrungBinh > 0 && preCao>0)
    {
      Serial.println("cao");
      myservo2.write(150);
      myservo3.write(150);
      caoCounter++;
      Serial.println(caoCounter); 
      updateValueLCD();
      delay(10000);
      myservo2.write(150);
      myservo3.write(150);  
      preThap = 0; 
      preTrungBinh = 0;
      preCao = 0; 
    }
  }
}
void updateValueLCD()
{
  int tongSP = caoCounter +thapCounter + trungBinhCounter;
  lcd.setCursor(0,0);
  lcd.print("Tong so SP: ");
  lcd.setCursor(13,0);
  lcd.print(tongSP);

  char buffer[100];
  sprintf(buffer, "C:%d, T:%d, TB:%d", caoCounter, thapCounter, trungBinhCounter);
  lcd.setCursor(0,1);
  lcd.print(buffer);
}
