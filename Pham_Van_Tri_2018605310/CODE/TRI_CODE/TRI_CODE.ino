
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
//
char ssid[] = "phamtri";
char pass[] = "12345678";


//char ssid[] = "Zoro";
//char pass[] = "minhdien04";

#define DHTPIN 15         
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;


LiquidCrystal_I2C lcd(0x27, 16, 2);
float temper = 0.0;
float humi = 0.0;

void sendSensor()
{
  Blynk.virtualWrite(V5, humi);
  Blynk.virtualWrite(V6, temper);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  dht.begin();
  sendSensor();
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
  lcd.init();
  // bat led nen                     
  lcd.backlight();
  sendDataToLCDTask();
  Blynk.begin(auth, ssid, pass);

}

void loop()
{
  timer.run(); 
  Blynk.run();
}


void sendDataToLCDTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendDataToLCD,  // Function to be called
    "handleMQTT",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    5,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void sendDataToLCD(void *parameter) {
  while (1) {
    humi = dht.readHumidity();
    temper = dht.readTemperature();
    lcd.setCursor(0, 0);
    //in ra man hinh
    lcd.print("Temp: ");
    lcd.setCursor(6, 0);
    lcd.print(temper);
    lcd.setCursor(13, 0);
    lcd.write(0xDF);
    lcd.print("C");
    //set con tro hang thu hai, cot thu nhat
    lcd.setCursor(0,1);
    lcd.print("Humi: ");
    lcd.setCursor(6, 1);
    lcd.print(humi);
    lcd.setCursor(13, 1);
    lcd.print("%");
    vTaskDelay(2000/ portTICK_PERIOD_MS);
  }
}
