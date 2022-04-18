var client = new Paho.MQTT.Client("broker.hivemq.com", 8000,"web_" + parseInt(Math.random() * 100, 10));  
client.onConnectionLost = onConnectionLost;  
client.onMessageArrived = onMessageArrived;

var options = { useSSL: false,userName: "hungthinhhaui",password: "CNxCDXpJ",onSuccess: onConnect , onFailure: doFail } 

client.connect(options);  
    
function onConnect()
{
    console.log("Connect tới Server");
    client.subscribe("hungthinhhaui/maylanh");	 // nhận dữ liệu		
}  

function doFail(e) {
    console.log("Không kết nối được tới server");
    console.log(e);
}

function onConnectionLost(responseObject) 
{
    if (responseObject.errorCode !== 0) 
    {
    console.log("error");
    console.log("onConnectionLost:" + responseObject.errorMessage);
    }
} 

function onMessageArrived(message) 
{
    console.log("Data ESP:" + message.payloadString);
    var DataServer = message.payloadString;
    //document.getElementById("status-relay-1").innerHTML = DataServer;
    var DataJson = JSON.parse(DataServer);
    //console.log(DataServer)


// hiển thị trạng thái relay 1

    if(DataJson.relay1 == 0)
    {
    // hiển thị OFF relay
    document.getElementById("status-relay-1").innerHTML = "Đang Tắt";
    document.getElementById("light-bulb1").classList.add("light-bulb");
    document.getElementById("light-bulb1").classList.remove("light-bulb2");
    }
    else if(DataJson.relay1 == 1)
    {
    // hiển thị On relay
    document.getElementById("status-relay-1").innerHTML = "Đang Bật";
    document.getElementById("light-bulb1").classList.add("light-bulb2");
    document.getElementById("light-bulb1").classList.remove("light-bulb");
    }
    
    // hiển thị trạng thái relay 2
    if(DataJson.relay2 == 0)
    {
    // hiển thị OFF relay
    document.getElementById("status-relay-2").innerHTML = "Đang Tắt";
    document.getElementById("light-bulb2").classList.add("light-bulb");
    document.getElementById("light-bulb2").classList.remove("light-bulb2");
    }
    else if(DataJson.relay2 == 1)
    {
    // hiển thị On relay
    document.getElementById("status-relay-2").innerHTML = "Đang Bật";
    document.getElementById("light-bulb2").classList.add("light-bulb2");
    document.getElementById("light-bulb2").classList.remove("light-bulb");
    }
    // hiển thị trạng thái relay 3
    if(DataJson.relay3 == 0)
    {
    // hiển thị OFF relay
    document.getElementById("status-relay-3").innerHTML = "Đang Tắt";
    document.getElementById("light-bulb3").classList.add("light-bulb");
    document.getElementById("light-bulb3").classList.remove("light-bulb2");
    }
    else if(DataJson.relay3 == 1)
    {
    // hiển thị On relay
    document.getElementById("status-relay-3").innerHTML = "Đang Bật";
    document.getElementById("light-bulb3").classList.add("light-bulb2");
    document.getElementById("light-bulb3").classList.remove("light-bulb");
    }
    // hiển thị trạng thái relay 4
    if(DataJson.relay4 == 0)
    {
    // hiển thị OFF relay
    document.getElementById("status-relay-4").innerHTML = "Đang Tắt";
    document.getElementById("light-bulb4").classList.add("light-bulb");
    document.getElementById("light-bulb4").classList.remove("light-bulb2");
    
    }
    else if(DataJson.relay4 == 1)
    {
    // hiển thị On relay
    document.getElementById("status-relay-4").innerHTML = "Đang Bật";
    document.getElementById("light-bulb4").classList.add("light-bulb2");
    document.getElementById("light-bulb4").classList.remove("light-bulb");
    }

}


//DOM HTML
var onButton1 = document.getElementById("on-button-1");
var offButton1 = document.getElementById("off-button-1");
var onButton2 = document.getElementById("on-button-2");
var offButton2 = document.getElementById("off-button-2");
var onButton3 = document.getElementById("on-button-3");
var offButton3 = document.getElementById("off-button-3");
var onButton4 = document.getElementById("on-button-4");
var offButton4 = document.getElementById("off-button-4");

//add event listener
onButton1.addEventListener("click", onClick1);
offButton1.addEventListener("click", offClick1);
onButton2.addEventListener("click", onClick2);
offButton2.addEventListener("click", offClick2);
onButton3.addEventListener("click", onClick3);
offButton3.addEventListener("click", offClick3);
onButton4.addEventListener("click", onClick4);
offButton4.addEventListener("click", offClick4);

function onClick1()
{
	var DataSend = "A1B";
	client.send("hungthinhhaui/quat", DataSend, 2, false);
	console.log("hungthinhhaui/quat:" + DataSend);
	 
}
function offClick1()
{
	var DataSend = "A0B";
	client.send("hungthinhhaui/quat", DataSend, 2, false);
	console.log("hungthinhhaui/quat" + DataSend);
}

function onClick2()
{
    // ON tb 2
    var DataSend = "C1D";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);
}
function offClick2()
{

    // OFF
    var DataSend = "C0D";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);
 
}

function onClick3()
{

    // ON tb 3
    var DataSend = "E1F";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);
}
 
function offClick3()
{

    // OFF
    var DataSend = "E0F";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);

}
function onClick4()
{
    // ON tb 1
    var DataSend = "G1H";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);
}
function offClick4()
{

    // OFF
    var DataSend = "G0H";
    client.send("hungthinhhaui/quat", DataSend, 2, false);
    console.log("hungthinhhaui/quat:" + DataSend);
 
}