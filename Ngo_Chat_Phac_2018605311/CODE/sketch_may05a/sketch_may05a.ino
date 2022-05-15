#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <AsyncElegantOTA.h>
#include <LiquidCrystal_I2C.h>


//dht define 
#define DHTPIN 13    
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE); 

LiquidCrystal_I2C lcd(0x27, 20, 4); //lcd i2c 20x04

void setup() {


//dht begin
  dht.begin();

  lcd.init();
  // bat led nen                     
  lcd.backlight();

}

void loop() {
   lcd.setCursor(0, 0);

  float temper = dht.readTemperature();
  float humi = dht.readHumidity();
  //in ra man hinh
  lcd.print(temper);

  lcd.setCursor(0, 1);
  lcd.print(humi);

  lcd.setCursor(0, 2);  
  lcd.print("Do am dat");

  lcd.setCursor(0, 3);  
  lcd.print("Anh sang");
  delay(3000);


}
