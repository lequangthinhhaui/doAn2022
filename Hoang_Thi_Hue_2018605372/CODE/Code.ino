#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 25
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MODE 26
#define DO_AM_DAT 32
#define ANH_SANG 33

#define DEN 13
#define QUAT 12
#define BOM 14

int adc1, adc2, adc3 = 0;
float AnhSangVal, DoAmDatVal, NhietDoVal, DoAmMTVal = 0.0;
/*
 * Require: Yêu cầu: 2 chế độ: 
 * - Chế độ tự động: Nhiệt độ MT, độ ẩm đất, ánh sáng MT (bỏ hiển thị LCD) 
 * - Chế độ bằng tay: Điều khiển Bật tắt quạt, máy bơm, đèn qua Webserver và hiển thị.
 */
/*
 * Sensor Temperature: DHT22
 *          VCC  ---- 5V 
 *          GND  ---- GND 
 *          DATA ----  25
 * Sensor Do am dat:
 *          DATA ---- 32 
 * Sensor Anh Sang:
 *          DATA ---- 33
 * Output       
 *          Den   ---- 13
 *          Quat  ---- 12
 *          Bom   ---- 14
 * Button Mode: 26
 */
 
// Wifi
char* ssid = "TP-LINK_D64E64";
char* pass = "88888888";
WebServer server(80);

// Web Interface
//=========Biến chứa mã HTLM Website===========//
const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
          .bt_on {height:50px; width:100px; margin:10px 0;background-color:#FF6600;border-radius:5px;}
          .bt_off {height:50px; width:100px; margin:10px 0;background-color:#00FF00;border-radius:5px;}
       </style>
       <meta charset="UTF-8">
   </head>
   <body> 
      <h1>CONTROL SYSTEM</h1> 
      <div>Lamp Status: <b><pan id="trangthaiLamp"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onLamp')">ON</button>
        <button class="bt_off" onclick="getdata('offLamp')">OFF</button>
      </div>
      <div>Pump Status: <b><pan id="trangthaiPump"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onPump')">ON</button>
        <button class="bt_off" onclick="getdata('offPump')">OFF</button>
      </div>
      <div>Fan Status: <b><pan id="trangthaiFan"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onFan')">ON</button>
        <button class="bt_off" onclick="getdata('offFan')">OFF</button>
      </div>
      <h1>DATA</h1> 
      <div>Mode: <b><pan id="data4"><pan></b></div>
      <div>Brightness (%): <b><pan id="data1"><pan></b></div>
      <div>Soil Moisturet(%): <b><pan id="data2"><pan></b></div>
      <div>Environment Temperature(độ C): <b><pan id="data3"><pan></b></div>
      <div>Environmental Humidity(%): <b><pan id="data5"><pan></b></div>
      <script>
        //-----------Tạo đối tượng request----------------
        function create_obj(){
          td = navigator.appName;
          if(td == "Microsoft Internet Explorer"){
            obj = new ActiveXObject("Microsoft.XMLHTTP");
          }else{
            obj = new XMLHttpRequest();
          }
          return obj;
        }
        var xhttp = create_obj();
        //-----------Thiết lập dữ liệu và gửi request-------
        function getdata(url){
          xhttp.open("GET","/"+url,true);
          xhttp.onreadystatechange = process;//nhận reponse 
          xhttp.send();
        }
        //-----------Kiểm tra response--------------------
        function process(){
          if(xhttp.readyState == 4 && xhttp.status == 200){
            //------Updat data sử dụng javascript----------
            ketqua = xhttp.responseText; 
            document.getElementById("reponsetext").innerHTML=ketqua;       
          }
        }
        //------------Kiểm tra trạng thái chân D1 D2-------------
        function getstatus(){
          xhttp.open("GET","getstatus",true);
          xhttp.onreadystatechange = process_json;
          xhttp.send();
        }
       
        //-----------Kiểm tra response-------------------------------
        function process_json(){
          if(xhttp.readyState == 4 && xhttp.status == 200){
            //------Update data sử dụng javascript-------------------
            var trangthai_JSON = xhttp.responseText;
            var Obj = JSON.parse(trangthai_JSON);
            document.getElementById("trangthaiLamp").innerHTML = Obj.D1;
            document.getElementById("trangthaiPump").innerHTML = Obj.D2;
            document.getElementById("trangthaiFan").innerHTML = Obj.D3;
            document.getElementById("data1").innerHTML = Obj.D4;
            document.getElementById("data2").innerHTML = Obj.D5;
            document.getElementById("data3").innerHTML = Obj.D6;
            document.getElementById("data4").innerHTML = Obj.D7;
            document.getElementById("data5").innerHTML = Obj.D8;
          }
        }
        //---Ham update duu lieu tu dong---
        setInterval(function() {
          getstatus();
        }, 1000);
      </script>
   </body> 
  </html>
)=====";
//=========================================//

void setup() {
  WiFi.begin(ssid,pass);
  Serial.begin(115200);
  Serial.print("\nConnecting ");
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }
  Serial.println("ESP Address is : ");
  Serial.print(WiFi.localIP());

  server.on("/",mainpage);
  server.on("/onLamp",on_Lamp);
  server.on("/offLamp",off_Lamp);
  server.on("/onPump",on_Pump);
  server.on("/offPump",off_Pump);
  server.on("/onFan",on_Fan);
  server.on("/offFan",off_Fan);
  server.on("/getstatus",get_status);
  server.begin();
  
  pinMode(MODE, INPUT_PULLUP);
  
  pinMode(DEN, OUTPUT);
  pinMode(BOM, OUTPUT);
  pinMode(QUAT, OUTPUT);

  digitalWrite(DEN, LOW);
  digitalWrite(BOM, LOW);
  digitalWrite(QUAT, LOW);
  
  dht.begin();
}

void Read_Sensor(){
  DoAmMTVal = dht.readHumidity();
  NhietDoVal = dht.readTemperature();
  /*
  Serial.print(F("Humidity: "));
  Serial.println(DoAmMTVal);
  Serial.print(F("Temperature: "));
  Serial.println(NhietDoVal);

  if(isnan(DoAmMTVal) || isnan(NhietDoVal)){
    Serial.println(F("Failed to read form DHT sensor"));
    return;
  }
  */
  adc1 = analogRead(ANH_SANG);
  adc2 = analogRead(DO_AM_DAT); 

  AnhSangVal = (float(adc1) / 4096) *100;
  DoAmDatVal = (float(adc2) / 4096) *100;
  /*
  Serial.print("Ánh Sáng: ");
  Serial.print(AnhSangVal);
  Serial.println(" %");
  
  Serial.print("Độ Ẩm Đất: ");
  Serial.print(DoAmDatVal);
  Serial.println(" %");
  */
}
void Set_Mode(){
  if(digitalRead(MODE) == 0){
    Serial.println("MODE Auto");
     if(AnhSangVal <= 40){
        digitalWrite(DEN, HIGH);
     }else{
        digitalWrite(DEN, LOW);
     }

     if(DoAmDatVal >= 60){
        digitalWrite(BOM, HIGH);
     }else{
        digitalWrite(BOM, LOW);
     }

     if(NhietDoVal >= 60){
        digitalWrite(QUAT, HIGH);
     }else{
        digitalWrite(QUAT, LOW);
     }
   }else if(digitalRead(MODE) == 1){

   }
}
void loop() {
  server.handleClient();
  Read_Sensor();
  Set_Mode();
}

void mainpage(){
  String s = FPSTR(MainPage);
  server.send(200,"text/html",s);
}
void on_Lamp(){
  if(digitalRead(MODE) == 1){
    digitalWrite(DEN,HIGH);
  }
}
void off_Lamp(){
  if(digitalRead(MODE) == 1){
    digitalWrite(DEN,LOW);
  }
}
void on_Pump(){
  if(digitalRead(MODE) == 1){
    digitalWrite(BOM,HIGH);
  }
}
void off_Pump(){
  if(digitalRead(MODE) == 1){
    digitalWrite(BOM,LOW);
  }
}
void on_Fan(){
  if(digitalRead(MODE) == 1){
    digitalWrite(QUAT,HIGH);
  }
}
void off_Fan(){
  if(digitalRead(MODE) == 1){
    digitalWrite(QUAT,LOW);
  }
}
void get_status(){
  String d1,d2,d3,d4,d5,d6,d7,d8;

  if(digitalRead(DEN)==0){
    d1 = "OFF";
  }else{
    d1 = "ON";
  }
  if(digitalRead(BOM)==0){
    d2 = "OFF";
  }else{
    d2 = "ON";
  }
  if(digitalRead(QUAT)==0){
    d3 = "OFF";
  }else{
    d3 = "ON";
  }
 
  if(digitalRead(MODE)==0){
      d7 = "Auto Mode";
  }else{
      d7 = "Menu Mode";
  }
  d4 = String(AnhSangVal);
  d5 = String(DoAmDatVal);
  d6 = String(NhietDoVal);
  d8 = String(DoAmMTVal);
  String s1 = "{\"D1\": \""+ d1 +"\", \"D2\": \""+ d2 +"\", \"D3\": \""+ d3 +"\", \"D4\": \""+ d4 +"\", \"D5\": \""+ d5 +"\", \"D6\": \""+ d6 +"\", \"D7\": \""+ d7 +"\", \"D8\": \""+ d8 +"\"}";
  server.send(200,"application/json",s1);
}
