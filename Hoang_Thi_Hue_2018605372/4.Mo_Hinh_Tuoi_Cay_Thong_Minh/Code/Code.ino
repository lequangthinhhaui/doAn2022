#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>

ESP32Time rtc;

#define DHTPIN 25
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MODE 26
#define DO_AM_DAT 32
#define ANH_SANG 33

#define DEN 12
#define QUAT 14
#define BOM 13

int adc1, adc2, adc3 = 0;
float AnhSangVal, DoAmDatVal, NhietDoVal, DoAmMTVal = 0.0;

int gio, phut = 0;

#define GIO_ON1 6
#define PHUT_ON1 0
#define GIO_OFF1 8
#define PHUT_OFF1 0

#define GIO_ON2 16
#define PHUT_ON2 30
#define GIO_OFF2 16
#define PHUT_OFF2 31

bool set_time = false;
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
char* ssid = "hoanghue";
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
      <div><b>Lamp Control</b></div>
      <div>Lamp Status <b><pan id="statusLamp"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onLamp')">ON</button>
        <button class="bt_off" onclick="getdata('offLamp')">OFF</button>
      </div>
      <div><b>Pump Control</b></div>
      <div>Pump Status: <b><pan id="statusPump"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onPump')">ON</button>
        <button class="bt_off" onclick="getdata('offPump')">OFF</button>
      </div>
      <div><b>Fan Control</b></div>
      <div>Fan Status <b><pan id="statusFan"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onFan')">ON</button>
        <button class="bt_off" onclick="getdata('offFan')">OFF</button>
      </div>
      <div>Current Status: <pan id="reponsetext"><pan></div>
      <h1>DATA</h1>
      <div>Time: <b><pan id="data6"><pan></b>:<b><pan id="data7"><pan></b></div>
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
            document.getElementById("statusLamp").innerHTML = Obj.D1;
            document.getElementById("statusPump").innerHTML = Obj.D2;
            document.getElementById("statusFan").innerHTML = Obj.D3;
            document.getElementById("data1").innerHTML = Obj.D4;
            document.getElementById("data2").innerHTML = Obj.D5;
            document.getElementById("data3").innerHTML = Obj.D6;
            document.getElementById("data4").innerHTML = Obj.D7;
            document.getElementById("data5").innerHTML = Obj.D8;
            document.getElementById("data6").innerHTML = Obj.D9;
            document.getElementById("data7").innerHTML = Obj.D10;
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

void get_time(){
/*
  setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
  setTime(1609459200);  // 1st Jan 2021 00:00:00
  setTimeStruct(time);  // set with time struct
  
  getTime()          //  (String) 15:24:38
  getDate()          //  (String) Sun, Jan 17 2021
  getDate(true)      //  (String) Sunday, January 17 2021
  getDateTime()      //  (String) Sun, Jan 17 2021 15:24:38
  getDateTime(true)  //  (String) Sunday, January 17 2021 15:24:38
  getTimeDate()      //  (String) 15:24:38 Sun, Jan 17 2021
  getTimeDate(true)  //  (String) 15:24:38 Sunday, January 17 2021
  
  getMicros()        //  (long)    723546
  getMillis()        //  (long)    723
  getEpoch()         //  (long)    1609459200
  getSecond()        //  (int)     38    (0-59)
  getMinute()        //  (int)     24    (0-59)
  getHour()          //  (int)     3     (0-12)
  getHour(true)      //  (int)     15    (0-23)
  getAmPm()          //  (String)  pm
  getAmPm(true)      //  (String)  PM
  getDay()           //  (int)     17    (1-31)
  getDayofWeek()     //  (int)     0     (0-6)
  getDayofYear()     //  (int)     16    (0-365)
  getMonth()         //  (int)     0     (0-11)
  getYear()          //  (int)     2021
*/
  gio = rtc.getHour(true);
  phut = rtc.getMinute();
  //Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
  //Serial.print("Gio: ");
  //Serial.println(gio);
  //Serial.print("Phut: ");
  //Serial.println(phut);
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
    //if( (gio < GIO_OFF1 && gio >= GIO_ON1 && phut >= PHUT_ON1) || (gio < GIO_OFF2 && gio >= GIO_ON2 && phut >= PHUT_ON2) ){
    if( (set_time == false && gio == GIO_ON1 && phut == PHUT_ON1) || (set_time == false && gio == GIO_ON2 && phut == PHUT_ON2) ){
      set_time = true;
    }else if( (set_time == true && gio == GIO_OFF1 && phut == PHUT_OFF1) || (set_time == true && gio == GIO_OFF2 && phut == PHUT_OFF2) ){ 
      set_time = false;
      digitalWrite(BOM, LOW);
    }
    if(set_time == true){
      Serial.println("Controll Set Time");
      if(DoAmDatVal >= 70){
        digitalWrite(BOM, HIGH);
      }else{
        digitalWrite(BOM, LOW);
      }
    }
      
    //if(AnhSangVal <= 20){
    if(adc1 <= 500){
      digitalWrite(DEN, HIGH);
    }else{
      digitalWrite(DEN, LOW);
    }

    if(NhietDoVal >= 60){
      digitalWrite(QUAT, HIGH);
    }else{
      digitalWrite(QUAT, LOW);
    }
     
   }else if(digitalRead(MODE) == 1){

   }
}

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
  rtc.setTime(30, 20, 15, 7, 5, 2021);// giay, phut, gio, ngay, thang, nam
}

void loop() {
  server.handleClient();
  Read_Sensor();
  Set_Mode();
  get_time();
}

void mainpage(){
  String s = FPSTR(MainPage);
  server.send(200,"text/html",s);
}
void on_Lamp(){
  if(digitalRead(MODE) == 1){
    digitalWrite(DEN,HIGH);
    server.send(200,"text/html","Turn On Lamp");
  }
}
void off_Lamp(){
  if(digitalRead(MODE) == 1){
    digitalWrite(DEN,LOW);
    server.send(200,"text/html","Turn Off Lamp");
  }
}
void on_Pump(){
  if(digitalRead(MODE) == 1){
    digitalWrite(BOM,HIGH);
    server.send(200,"text/html","Turn On Pump");
  }
}
void off_Pump(){
  if(digitalRead(MODE) == 1){
    digitalWrite(BOM,LOW);
    server.send(200,"text/html","Turn Off Pump");
  }
}
void on_Fan(){
  if(digitalRead(MODE) == 1){
    digitalWrite(QUAT,HIGH);
    server.send(200,"text/html","Turn On Fan");
  }
}
void off_Fan(){
  if(digitalRead(MODE) == 1){
    digitalWrite(QUAT,LOW);
    server.send(200,"text/html","Turn Off Fan");
  }
}
void get_status(){
  String d4,d5,d6,d7,d8,d9,d10;
  String d1,d2,d3;
  if(digitalRead(DEN)==0){
    d1 = "OFF";
    //server.send(200,"text/html","Turn Off Lamp");
  }else{
    d1 = "ON";
    //server.send(200,"text/html","Turn On Lamp");
  }
  if(digitalRead(BOM)==0){
    d2 = "OFF";
    //server.send(200,"text/html","Turn Off Pump");
  }else{
    d2 = "ON";
    //server.send(200,"text/html","Turn On Pump");
  }
  if(digitalRead(QUAT)==0){
    d3 = "OFF";
    //server.send(200,"text/html","Turn Off Fan");
  }else{
    d3 = "ON";
    //server.send(200,"text/html","Turn On Fan");
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
  d9 = String(gio);
  d10 = String(phut);
  //String s1 = "{\"D4\": \""+ d4 +"\", \"D5\": \""+ d5 +"\", \"D6\": \""+ d6 +"\", \"D7\": \""+ d7 +"\", \"D8\": \""+ d8 +"\", \"D9\": \""+ d9 +"\", \"D10\": \""+ d10 +"\"}";
  String s1 = "{\"D1\": \""+ d1 +"\", \"D2\": \""+ d2 +"\", \"D3\": \""+ d3 +"\", \"D4\": \""+ d4 +"\", \"D5\": \""+ d5 +"\", \"D6\": \""+ d6 +"\", \"D7\": \""+ d7 +"\", \"D8\": \""+ d8 +"\", \"D9\": \""+ d9 +"\", \"D10\": \""+ d10 +"\"}";
  server.send(200,"application/json",s1);
}
