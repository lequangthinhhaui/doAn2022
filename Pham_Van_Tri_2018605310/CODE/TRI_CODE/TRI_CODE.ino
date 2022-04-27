
#define BLYNK_TEMPLATE_ID "TMPLkc0eU8vo"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "T6wIAD-bAbAVR5pOYo9A_Tj6Gnc5Rao4"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "phamtri";
char pass[] = "12345678";

#define DHTPIN 15         
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;


//user variable

LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd i2c 16x2

float temper = 0.0;
float humi = 0.0;
//humi = dht.readHumidity();
//temper = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
void sendSensor()
{
  humi = dht.readHumidity();
  temper = dht.readTemperature();

  if (isnan(humi) || isnan(temper)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, humi);
  Blynk.virtualWrite(V6, temper);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  sendSensor();
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
  
  lcd.init();
  // bat led nen                     
  lcd.backlight();
}

void loop()
{
  Blynk.run();
  timer.run();
  lcd.setCursor(0, 0);
  //in ra man hinh
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(temper);
  //set con tro hang thu hai, cot thu nhat
  lcd.setCursor(0,1);
  lcd.print("Humi: ");
  lcd.setCursor(6, 1);
  lcd.print(humi);
}
