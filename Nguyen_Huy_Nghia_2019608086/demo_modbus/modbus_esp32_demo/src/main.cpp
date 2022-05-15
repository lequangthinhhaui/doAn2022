/*
  ModbusRTU ESP32
  Concurent thread example
  
  (c)2020 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  Tool Modbus Slave on PC for test
  https://www.modbustools.com/download.html
*/

#include <ModbusRTU.h>

#include <PubSubClient.h>

#include <WiFi.h>

#include <LiquidCrystal_I2C.h>

#include <ArduinoJson.h>


//ota update
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  

#define REG 200
#define REG_NUM 10
#define SLAVE_ID1 3

#define MBUS_HW_SERIAL Serial2
#define MBUS_TXD_PIN   17         
#define MBUS_RXD_PIN   16

#define ssid "Zoro"//"WIFi bỊ nhiễm virut"
#define password "minhdien04"//"minhdien04"


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_control_TB = "huynghia/controlTB";

String mqtt_topic_infor_TB = "huynghia/inforTB";//publish

WiFiClient espClient;
PubSubClient client(espClient);

ModbusRTU mb;

AsyncWebServer server(80);

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

uint16_t tanso = 0;


uint16_t chieuQuay = 0; //quay cung chieu kim dong ho


//button variable
int startStopPin = 19;
int quayThuanPin = 18;
int quayNguocPin = 5;

int button1PressCount = 0;
int button2PressCount = 0;
int button3PressCount = 0;

//define pin Function
void pinInit();



xSemaphoreHandle xMutex;
Modbus::ResultCode err;
Modbus::ResultCode readSync(uint8_t address, uint16_t start, uint16_t num, uint16_t* buf) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  if (mb.slave()) {
    xSemaphoreGive(xMutex);
    return Modbus::EX_GENERAL_FAILURE;
  }
  Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.readHreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void*) {
    err = event;
    return true;
  });
  while (mb.slave()) {
    vTaskDelay(1);
    mb.task();
  }
  Modbus::ResultCode res = err;
  xSemaphoreGive(xMutex);
  return res;
}

Modbus::ResultCode writeSync(uint8_t address, uint16_t start, uint16_t num, uint16_t* buf) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  if (mb.slave()) {
    xSemaphoreGive(xMutex);
    return Modbus::EX_GENERAL_FAILURE;
  }
  Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.writeHreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void*) {
    err = event;
    return true;
  });
  while (mb.slave()) {
    vTaskDelay(1);
    mb.task();
  }
  Modbus::ResultCode res = err;
  xSemaphoreGive(xMutex);
  return res;
}


void loop1( void * pvParameters );
void loop2( void * pvParameters );

//
void handleButton1(void *parameter);
void handleButton2(void *parameter);
void handleButton3(void *parameter);


void setup() {
  Serial.begin(115200);
  MBUS_HW_SERIAL.begin(9600, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
  mb.begin(&MBUS_HW_SERIAL);
  mb.master();
  WiFi.begin(ssid, password);
  taskInit();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);

  //LCD init
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  otaUpdate();
  AsyncWebServer server(80);
  xMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(
                    loop1,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    10,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */

  xTaskCreatePinnedToCore(
                    loop2,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    10,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */

  
}
uint16_t hregs1[REG_NUM];
uint16_t hregs2[2];

void loop1( void * pvParameters ){
  hregs2[1] = 0;
  while(true) {
      delay(1000);
      if(tanso > 100)
      {
        hregs2[1] = chieuQuay;
        hregs2[0] = tanso;
        writeSync(SLAVE_ID1, 13, 1, hregs2 + 0);
      }
      writeSync(SLAVE_ID1, 8, 1, hregs2 + 1);

      if (readSync(SLAVE_ID1, REG, REG_NUM, hregs1) == Modbus::EX_SUCCESS)
      {
        for(int i = 0; i< REG_NUM; i++)
        {
          Serial.print(REG+i);
          Serial.print("*******");
          Serial.println(hregs1[i]);
        }
      }
      else
        Serial.println("Error 2");
  }
}


void loop2( void * pvParameters ){

  while(true) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    char cTanso[20];
    char cChieuQuay[20];
    float tanSoHienTai = (float)(hregs1[0]/100.0);
    sprintf(cTanso, "Tan so: %0.2f Hz            ",  tanSoHienTai);
    lcd.setCursor(0, 0);
    lcd.print(cTanso);
    if(chieuQuay == 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("Dong co dung           ");
    }
    if(chieuQuay == 2)
    {
      lcd.setCursor(0, 1);
      lcd.print("Chieu: Nguoc         ");
    }
    else if(chieuQuay == 4)
    {
      lcd.setCursor(0, 1);
      lcd.print("Chieu: Thuan        ");
    }
  }
}


void loop() {
  setup_wifi();
  delay(100);
}


void callback(char* topic, byte* payload, unsigned int length)
{

  String Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }


  dataInt = Data.toInt();

  Serial.println(dataInt);
    if(payload[length-1] == 67)
      tanso = Data.toInt(); 
    else if(payload[length-1] == 66)
      chieuQuay = Data.toInt();
    else if(payload[length-1] == 65)
      chieuQuay = Data.toInt(); 
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
  doc["tanso"] = hregs1[0];
  doc["dongdien"]   =  hregs1[1];
  doc["dienap"] =  hregs1[2];
  serializeJson(doc, inforInverterBuff);
  client.publish(mqtt_topic_infor_TB.c_str(), inforInverterBuff);
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


void handleButton1(void *parameter) {
  while (1) {
    while (digitalRead(startStopPin) == HIGH);
    while (digitalRead(startStopPin) == LOW);
    button1PressCount ++;
    if (button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if (button1PressCount == 0)
    {
        chieuQuay = 0;
    }
    else if (button1PressCount == 1)
    {
        chieuQuay = 0;
    }
  }
}

//handle button 2

void handleButton2(void *parameter) {
  while (1) {
    while (digitalRead(quayThuanPin) == HIGH);
    while (digitalRead(quayThuanPin) == LOW);
    button2PressCount ++;
    if (button2PressCount == 2)
    {
      button2PressCount = 0;
    }
    if (button2PressCount == 0)
    {
        if(tanso == 0 && chieuQuay == 0)
        {
          tanso = 200;
          chieuQuay = 4;
        }
        else if (tanso > 0)
        {
          chieuQuay = 4;
        }
    }
    else if (button2PressCount == 1)
    {
        if(tanso == 0 && chieuQuay == 0)
        {
          tanso = 200;
          chieuQuay = 4;
        }
        else if (tanso > 0)
        {
          chieuQuay = 4;
        }
    }
  }
}


//handle button 3

void handleButton3(void *parameter) {
  while (1) {
    while (digitalRead(quayNguocPin) == HIGH);
    while (digitalRead(quayNguocPin) == LOW);
    button3PressCount ++;
    if (button3PressCount == 2)
    {
      button3PressCount = 0;
    }
    if (button3PressCount == 0)
    {
        if(tanso == 0 && chieuQuay == 0)
        {
          tanso = 200;
          chieuQuay = 2;
        }
        else if (tanso > 0)
        {
          chieuQuay = 2;
        }
    }
    else if (button3PressCount == 1)
    {
        if(tanso == 0 && chieuQuay == 0)
        {
          tanso = 200;
          chieuQuay = 2;
        }
        else if (tanso > 0)
        {
          chieuQuay = 2;
        }
    }
  }
}


void taskInit()
{
  sendMqttTask();
  handleButton1Task();
  handleButton2Task();
  handleButton3Task();
}



void otaUpdate()
{
  AsyncElegantOTA.begin(&server);
  server.begin();
}


//pin handle
void pinInit()
{
  pinMode(startStopPin, INPUT);
  pinMode(quayThuanPin, INPUT);
  pinMode(quayNguocPin, INPUT);
}
