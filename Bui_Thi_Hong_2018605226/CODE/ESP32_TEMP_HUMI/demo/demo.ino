#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <DHT.h>
#include <AsyncElegantOTA.h>
#include <LiquidCrystal_I2C.h>

#define ssid "Hong"
#define password "hongdt4"

//khai bao bien luu nhiet do va do am
float temper = 0.0;
float humi = 0.0;
float temperPre = 0.0;
float humiPre = 0.0;

String temperStr = " ";
String humiStr = " ";

//init lcd
LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd i2c 16x2

#define DHTPIN 15     // what pin we're connected to
#define DHTTYPE DHT21  // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTTemp() 
{
  // Read temperature as Celsius (the default)
  temper = dht.readTemperature();
  if(temper > 0)
  {
    temperPre = temper;
  }

  if (isnan(temper)) {    
    Serial.println("khong doc duoc nhiet do");
    temper = temperPre;
    return String(temper);
  }
  else {
    Serial.println(temper);
    return String(temper);
  }
}

String readDHTHum() 
{
  humi = dht.readHumidity();
  if(humi > 0)
  {
    humiPre = humi;
  }
  if (isnan(humi)) {
    Serial.println("khong doc duoc do am");
    humi = humiPre;
    return String(humi);
  }
  else {
    Serial.println(humi);
    return String(humi);
  }
}

void setup()
{

  Serial.begin(115200);
  dht.begin();

  //khoi tao lcd
  lcd.init();
  // bat led nen                     
  lcd.backlight();

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperStr.c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", humiStr.c_str());
  });
  AsyncElegantOTA.begin(&server);
  server.begin();
}

void loop(){
  temperStr = readDHTTemp();
  humiStr = readDHTHum();
  //set con tro hang thu nhat, cot thu nhat
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
  delay(5000);
}
