#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// Cập nhật thông tin
// Thông tin về wifi
#define ssid "WIFi bỊ nhiễm virut"//"WIFi bỊ nhiễm virut"
#define password "minhdien04"//"minhdien04"
// Thông tin về MQTT Broker
#define mqtt_server "18.213.193.98" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_pub = "hungthinhhaui/maylanh";   
String mqtt_topic_sub = "hungthinhhaui/quat";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

String Data = "";
String ChuoiSendWebJson = "";
float nhietdo = 0;

unsigned long last = 0, bien = 0;

int trangthairelay1 = 0;
int trangthairelay2 = 0;
int trangthairelay3 = 0;
int trangthairelay4 = 0;


void setup() {
  Serial.begin(9600);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
}
// Hàm kết nối wifi
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
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length)
{

  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }
  Serial.print("Data nhận = ");
  Serial.println(Data);

  if (Data.indexOf("A0B") >= 0)
  {
    Serial.println("OFF thiết bị 1");
    digitalWrite(D4,LOW);
    trangthairelay1 = 0;
  }
  else if (Data.indexOf("A1B") >= 0)
  {
    Serial.println("ON thiết bị 1");
    digitalWrite(D4,HIGH);
    trangthairelay1 = 1;
  }

  if (Data.indexOf("C0D") >= 0)
  {
    Serial.println("OFF thiết bị 2");
    trangthairelay2 = 0;
    digitalWrite(D5,LOW);
  }
  else if (Data.indexOf("C1D") >= 0)
  {
    Serial.println("ON thiết bị 2");
    trangthairelay2 = 1;
    digitalWrite(D5,HIGH);
  }
  else if (Data.indexOf("E0F") >= 0)
  {
    Serial.println("OFF thiết bị 3");
    trangthairelay3 = 0;
    digitalWrite(D6,LOW);
  }
  else if (Data.indexOf("E1F") >= 0)
  {
    Serial.println("ON thiết bị 3");
    trangthairelay3 = 1;
    digitalWrite(D6,HIGH);
  }
  else if (Data.indexOf("G0H") >= 0)
  {
    Serial.println("OFF thiết bị 4");
    trangthairelay4 = 0;
    digitalWrite(D7,LOW);
  }
  else if (Data.indexOf("G1H") >= 0)
  {
    Serial.println("ON thiết bị 4");
    trangthairelay4 = 1;
    digitalWrite(D7,HIGH);
  }
  Data = "";
  
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
void loop() {
  // Kiểm tra kết nối
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Sau mỗi 2s sẽ thực hiện publish dòng hello world lên MQTT broker
  long now = millis();
  if (now - lastMsg > 500) {
    SendDataMQTT(String(trangthairelay1), String(trangthairelay2), String(trangthairelay3), String(trangthairelay4) );
    lastMsg = now;
  }
}
void SendDataMQTT(String relay1 ,  String relay2 , String relay3 ,  String relay4 )
{
  // JSON
  ChuoiSendWebJson = "";
  ChuoiSendWebJson = "{\"relay1\":\"" + String(relay1) + "\"," +
                     "\"relay2\":\"" + String(relay2) + "\"," +
                     "\"relay3\":\"" + String(relay3) + "\"," +
                     "\"relay4\":\"" + String(relay4) + "\"}";

  Serial.print("ChuoiSendWebJson:");
  Serial.println(ChuoiSendWebJson);

  // gửi về server
  client.publish(mqtt_topic_pub.c_str(), ChuoiSendWebJson.c_str()); // => .c_str()
}
