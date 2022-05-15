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

LiquidCrystal_I2C lcd(0x27, 16, 2);  

#define REG 0
#define REG_NUM 2
#define SLAVE_ID1 1

#define MBUS_HW_SERIAL Serial2
#define MBUS_TXD_PIN   17         
#define MBUS_RXD_PIN   16

#define ssid "thanhlong"//"WIFi bỊ nhiễm virut"
#define password "thanhlong123"//"minhdien04"


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""


//user variable
const uint16_t mqtt_port = 1883; 
int pinBuz = 18;
int pinRL = 19;


//topic mqtt
String mqtt_topic_infor_VA = "thanhlong/inforVA";
String mqtt_topic_set_nguong = "thanhlong/setnguongVA";//publish

//mqtt
WiFiClient espClient;
PubSubClient client(espClient);

ModbusRTU mb;


//function define 
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnect();
void sendDataMQTT();
void pinInit();

//rtos define 
void taskInit();
void displayDataTask();
void sendMqttTask();
void displayDataTask();
void readDataSensorTask();


void sendMqtt(void *parameter);
void readDataSensor( void * pvParameters);
void displayData(void * pvParameters);
void readDataSensor( void * pvParameters );
//handle task

//user var define
float nguongDienApCoi = 10;
float nguongDienApCat = 20;
uint16_t hregs1[REG_NUM];



xSemaphoreHandle xMutex;
Modbus::ResultCode err;
Modbus::ResultCode readSync(uint8_t address, uint16_t start, uint16_t num, uint16_t* buf) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  if (mb.slave()) {
    xSemaphoreGive(xMutex);
    return Modbus::EX_GENERAL_FAILURE;
  }
  Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.readIreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void*) {
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




void setup() {
  Serial.begin(115200);

  //modbus init
  MBUS_HW_SERIAL.begin(9600, SERIAL_8N2, MBUS_RXD_PIN, MBUS_TXD_PIN);
  mb.begin(&MBUS_HW_SERIAL);
  mb.master();
  xMutex = xSemaphoreCreateMutex();

  //wifi init
  WiFi.begin(ssid, password);


  //pininit
  pinInit();
  //task init
  taskInit();

  //mqtt init
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);

  //lcd init
  lcd.init();                   
  lcd.backlight();
}




void loop() {
  setup_wifi();
  delay(5000);
}


void callback(char* topic, byte* payload, unsigned int length)
{
  String Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }
    Serial.println(Data);
    if(payload[length-1] == 65)
    {
      nguongDienApCoi = Data.toFloat();
    }
    else if(payload[length-1] == 66)
      nguongDienApCat = Data.toFloat();
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
      client.subscribe(mqtt_topic_set_nguong.c_str());
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
  doc["dienap"]   =  hregs1[0];
  doc["dongdien"] =  hregs1[1];
  doc["nguongcoi"]   = nguongDienApCoi;
  doc["nguongcat"] = nguongDienApCat;
  serializeJson(doc, inforInverterBuff);
  client.publish(mqtt_topic_infor_VA.c_str(), inforInverterBuff);
}

void pinInit()
{
  pinMode(pinBuz, OUTPUT);
  pinMode(pinRL, OUTPUT);
}

//rtos task 
void sendMqttTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendMqtt,  // Function to be called
    "sendMqtt",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    0,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void sendMqtt(void *parameter) {
  while (1) {
    sendDataMQTT();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


void readDataSensorTask()
{
  xTaskCreatePinnedToCore(
                    readDataSensor,   /* Task function. */
                    "readDataSensor",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    10,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */
}

void readDataSensor( void * pvParameters ){
  while(true) {
      delay(1000);
      if (readSync(SLAVE_ID1, REG, REG_NUM, hregs1) == Modbus::EX_SUCCESS)
      {
        for(int i = 0; i< REG_NUM; i++)
        {
          Serial.print(REG+i);
          Serial.print("*******");
          Serial.println(hregs1[i]);
        }
        float fDienAp = (float)(hregs1[0]/100.0);
        if(fDienAp > nguongDienApCoi)
        {
          digitalWrite(pinBuz, HIGH);
          vTaskDelay(500);
          digitalWrite(pinBuz, LOW);
          vTaskDelay(500);
        }
        else
         digitalWrite(pinBuz, LOW);

        if(fDienAp > nguongDienApCat)    
        {
          digitalWrite(pinRL, HIGH);
        }
        else
         digitalWrite(pinRL, LOW);
      }
      else
        Serial.println("ko doc duoc data");
  }
}


void displayDataTask()
{
  xTaskCreatePinnedToCore(
                    displayData,   /* Task function. */
                    "displayData",     /* name of task. */
                    2048,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    0,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */
}

void displayData(void * pvParameters )
{
  while(true) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    char cDienAp[20];
    char cDongDien[20];
    float fDienAp = (float)(hregs1[0]/100.0);
    float fDongdien = (float)(hregs1[1]/100.0);
    sprintf(cDienAp, "Ap DC: %0.2f V            ",  fDienAp);
    lcd.setCursor(0, 0);
    lcd.print(cDienAp);
    lcd.setCursor(0, 1);
    sprintf(cDongDien, "Dong DC: %0.2f A            ",  fDongdien);
    lcd.print(cDongDien);
  }
}

void taskInit()
{
  readDataSensorTask();
  sendMqttTask();
  displayDataTask();
}