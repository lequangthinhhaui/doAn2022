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

LiquidCrystal_I2C lcd(0x27, 16, 2);  

#define REG 202
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

String mqtt_topic_pub = "huynghia/dataPub";   
String mqtt_topic_sub = "huynghia/dataSub";

WiFiClient espClient;
PubSubClient client(espClient);

ModbusRTU mb;


//function define 
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void SendDataMQTT();
void reconnect();

//user var define
String Data = "";

uint16_t dataInt = 0;
float dataFloat = 0;
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

void setup() {
  Serial.begin(115200);
  MBUS_HW_SERIAL.begin(9600, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
  mb.begin(&MBUS_HW_SERIAL);
  mb.master();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);

  //LCD init
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
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
bool flagRun = false;
void loop1( void * pvParameters ){

  while(true) {
      delay(1000);

      hregs2[1] = 2;
      if(dataInt > 100)
      {
        hregs2[0] = dataInt;
        writeSync(SLAVE_ID1, 13, 1, hregs2+0);
      }

        writeSync(SLAVE_ID1, 8, 1, hregs2 + 1);
      if (readSync(SLAVE_ID1, REG, REG_NUM, hregs1) == Modbus::EX_SUCCESS)
      {
        for(int i = REG; i< (REG+REG_NUM); i++)
        {
          Serial.print(i);
          Serial.print(hregs2[0]);
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
      lcd.setCursor(0, 0);
    lcd.print(dataInt);
  }
}


void loop() {
  delay(100);
}


void callback(char* topic, byte* payload, unsigned int length)
{

  // byte* p = (byte*)malloc(length);
  // // Copy the payload to the new buffer
  // memcpy(p,payload,length);
  // client.publish(mqtt_topic_pub.c_str(), p, length);
  // // Free the memory
  // free(p);
  Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }
  dataInt = Data.toInt();
  Serial.println(dataInt);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
      Serial.println("Connected MQTT ngoinhaiot.com");
      client.subscribe(mqtt_topic_sub.c_str());
      digitalWrite(16, HIGH);
    }
    else
    {
      Serial.println("Không thể kết nối MQTT ngoinhaiot.com");
      digitalWrite(16, LOW);
      delay(5000);
    }
  }
}

void SendDataMQTT()
{
  client.publish(mqtt_topic_pub.c_str(), "hello"); // => .c_str()
}