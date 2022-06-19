/*
  ModbusRTU ESP32
  Concurent thread example
  
  (c)2020 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  Tool Modbus Slave on PC for test
  https://www.modbustools.com/download.html
*/


#include <PubSubClient.h>

#include <WiFi.h>

#include <LiquidCrystal_I2C.h>

#include <ArduinoJson.h>

#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>


//ota update
// #include <ESPAsyncWebServer.h>
// #include <AsyncElegantOTA.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  



#define ssid "dangcongtinh"
#define password "dangcongtinh123"


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_control_TB = "dangcongtinh/controlTB"; //subscribe

String mqtt_topic_infor_TB_ALL = "dangcongtinh/inforTBALL";//publish

String mqtt_topic_infor_TB_Relay = "dangcongtinh/inforTBRelay"; //publish

WiFiClient espClient;
PubSubClient client(espClient);



// AsyncWebServer server(80);

//function define 
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void SendDataMQTT();
void reconnect();

//rtos define 
void taskInit();
void sendMqttTask();
void sendMqtt(void *parameter);

//handle task
void sendDataMQTT();

//ota update
void  otaUpdate();



//user var define
String Data = "";
uint16_t dataInt = 0;
float dataFloat = 0;



//button variable
int den1Pin = 15;
int den2Pin = 4;
int den3Pin = 5;
int quatPin = 18;
int coiPin = 19;

//output pin
int den1OutPin = 14;
int den2OutPin = 27;
int den3OutPin = 26;
int quatOutPin = 25;
int coiOutPin = 33;


int button1PressCount = 0;
int button2PressCount = 0;
int button3PressCount = 0;
int button4PressCount = 0;
int button5PressCount = 0;


float nhietDo = 0.0;
float doAm = 0.0;
bool canhBao = 0;


void pinInit();

///handle button task
void handleButton1Task();
void handleButton2Task();
void handleButton3Task();
void handleButton4Task();
void handleButton5Task();



void handleButton1(void *parameter);
void handleButton2(void *parameter);
void handleButton3(void *parameter);
void handleButton4(void *parameter);
void handleButton5(void *parameter);



//data send
void sendDataTask();
void sendData(void *parameter);

void handleAutoTask();
void handleAuto(void *parameter);


#define DHTPIN 13     // what pin we're connected to
#define DHTTYPE DHT22  // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinInit();
  dht.begin();
  setup_wifi() ;

  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);

  //LCD init
  lcd.init();                    
  lcd.backlight();
  taskInit();

  
  // otaUpdate();
  // AsyncWebServer server(80);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length)
{

  String Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }


  dataInt = Data.toInt();

  if(strcmp(topic, mqtt_topic_control_TB.c_str()) == 0)
  {
    String Data = "";
    for (int i = 0; i < length; i++)
    {
      Data += (char)payload[i]; // abcde
    }
    dataInt = Data.toInt();
    Serial.println(dataInt);
    ///button 1
    if (dataInt == 10)
    {
       button1PressCount = 0;
       digitalWrite(den1OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
    }
    else if (dataInt == 11)
    {
       button1PressCount = 1;
       digitalWrite(den1OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");
    }
        ///button 2
    else if (dataInt == 20)
    {
       button2PressCount = 0;
       digitalWrite(den2OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
    }
    else if (dataInt == 21)
    {
       button2PressCount = 1;
       digitalWrite(den2OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
    }
        ///button 3
    else if (dataInt == 30)
    {
       button3PressCount = 0;
       digitalWrite(den3OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
    }
    else if (dataInt == 31)
    {
       button3PressCount = 1;
       digitalWrite(den3OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
    }
        ///button 4
    else if (dataInt == 40)
    {
       digitalWrite(quatOutPin, LOW);
       button4PressCount = 0;
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
    }
    else if (dataInt == 41)
    {
       digitalWrite(quatOutPin, HIGH);
       button4PressCount = 1;
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect()
{

  while (!client.connected())
  {
    String clientId = String(random(0xffff), HEX); 
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd))
    {
      Serial.println("Connected MQTT");
      client.subscribe(mqtt_topic_control_TB.c_str());
    }
    else
    {
      Serial.println("Not Connnected MQTT");
      delay(3000);
    }
  }
}

void sendDataMQTT()
{
  DynamicJsonDocument doc(1024);
  char inforInverterBuff[256];
  doc["nhietdo"] = nhietDo;
  doc["doam"]   =  doAm;
  doc["baochay"] =  canhBao;
  doc["den1"] = button1PressCount;
  doc["den2"]   =  button2PressCount;
  doc["den3"] =  button3PressCount;
  doc["quat"]   =  button4PressCount;
  doc["coi"] =  button5PressCount;
  serializeJson(doc, inforInverterBuff);
  client.publish(mqtt_topic_infor_TB_ALL.c_str(), inforInverterBuff);
}

//rtos task 
void sendMqttTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendMqtt,  // Function to be called
    "sendMqtt",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void sendMqtt(void *parameter) {
  while (1) {
  sendDataMQTT();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


void handleButton1Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton1,  // Function to be called
    "handleButton1",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton2Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton2,  // Function to be called
    "handleButton2",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton3Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton3,  // Function to be called
    "handleButton3",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButton4Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton4,  // Function to be called
    "handleButton4",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton5Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton5,  // Function to be called
    "handleButton5",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void sendDataTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendData,  // Function to be called
    "sendData",   // Name of task
    10000,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    5,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleAutoTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleAuto,  // Function to be called
    "sendData",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    5,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton1(void *parameter) {
  while (1) {
    while (digitalRead(den1Pin) == HIGH);
    while (digitalRead(den1Pin) == LOW);
    button1PressCount ++;
    if (button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if (button1PressCount == 0)
    {
        digitalWrite(den1OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
    }
    else if (button1PressCount == 1)
    {
        digitalWrite(den1OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");

    }
  }
}

//handle button 2

void handleButton2(void *parameter) {
  while (1) {
    while (digitalRead(den2Pin) == HIGH);
    while (digitalRead(den2Pin) == LOW);
    button2PressCount ++;
    if (button2PressCount == 2)
    {
      button2PressCount = 0;
    }
    if (button2PressCount == 0)
    {
        digitalWrite(den2OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
    }
    else if (button2PressCount == 1)
    {
        digitalWrite(den2OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
    }
  }
}


//handle button 3

void handleButton3(void *parameter) {
  while (1) {
    while (digitalRead(den3Pin) == HIGH);
    while (digitalRead(den3Pin) == LOW);
    button3PressCount ++;
    if (button3PressCount == 2)
    {
      button3PressCount = 0;
    }
    if (button3PressCount == 0)
    {
        digitalWrite(den3OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
    }
    else if (button3PressCount == 1)
    {
        digitalWrite(den3OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
    }
  }
}


void handleButton4(void *parameter) {
  while (1) {
    while (digitalRead(quatPin) == HIGH);
    while (digitalRead(quatPin) == LOW);
    button4PressCount ++;
    if (button4PressCount == 2)
    {
      button4PressCount = 0;
    }
    if (button4PressCount == 0)
    {
      digitalWrite(quatOutPin, LOW);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
    }
    else if (button4PressCount == 1)
    {
      digitalWrite(quatOutPin, HIGH);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");

    }
  }
}


void handleButton5(void *parameter) {
  while (1) {
    while (digitalRead(coiPin) == HIGH);
    while (digitalRead(coiPin) == LOW);
    button5PressCount ++;
    if (button5PressCount == 2)
    {
      button5PressCount = 0;
    }
    if (button5PressCount == 0)
    {
      digitalWrite(coiOutPin, LOW);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "50");
    }
    else if (button5PressCount == 1)
    {
      digitalWrite(coiOutPin, HIGH);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "51");
    }
  }
}


void sendData(void *parameter) {
  while (1) {
  nhietDo = dht.readTemperature();
  doAm = dht.readHumidity();
  //set con tro hang thu nhat, cot thu nhat
  lcd.setCursor(0, 0);
  //in ra man hinh
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(nhietDo);
  lcd.setCursor(13, 0);
  lcd.write(0xDF);
  lcd.print("C");
  //set con tro hang thu hai, cot thu nhat
  lcd.setCursor(0,1);
  lcd.print("Humi: ");
  lcd.setCursor(6, 1);
  lcd.print(doAm);
  lcd.setCursor(13, 1);
  lcd.print("%");

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

bool flagFanAuto = false;
bool flagCoiAuto = false;

void handleAuto(void *parameter) {
  while (1) {
  uint16_t dataFlame = analogRead(36);
  Serial.println(dataFlame);


  if(dataFlame <= 10 && flagCoiAuto == false)
  {
      button5PressCount = 1;
      digitalWrite(coiOutPin, HIGH);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "51");
      flagCoiAuto = true;
      canhBao = 1;
  }
  else if(dataFlame > 10 && flagCoiAuto == true)
  {
      button5PressCount = 0;
      digitalWrite(coiOutPin, LOW);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "50");
      flagCoiAuto = false;
      canhBao = 0;    
  }


  if(doAm >= 80 && flagFanAuto == false)
  {
      button4PressCount = 1;
      digitalWrite(quatOutPin, HIGH);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");
      flagFanAuto = true;
  }
  else if(doAm < 80 && flagFanAuto == true)
  {
      button4PressCount = 0;
      digitalWrite(quatOutPin, LOW);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
      flagFanAuto = false;    
  }

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}




void taskInit()
{
  sendDataTask();
  sendMqttTask();
  handleAutoTask();
  handleButton1Task();
  handleButton2Task();
  handleButton3Task();
  handleButton4Task();
  handleButton5Task();
  
}



// void otaUpdate()
// {
//   AsyncElegantOTA.begin(&server);
//   server.begin();
// }




//pin handle
void pinInit()
{
  pinMode(den1Pin, INPUT);
  pinMode(den2Pin, INPUT);
  pinMode(den3Pin, INPUT);
  pinMode(quatPin, INPUT);
  pinMode(coiPin, INPUT);

  pinMode(den1OutPin, OUTPUT);
  pinMode(den2OutPin, OUTPUT);
  pinMode(den3OutPin, OUTPUT);
  pinMode(quatOutPin, OUTPUT);
  pinMode(coiOutPin, OUTPUT);
}
