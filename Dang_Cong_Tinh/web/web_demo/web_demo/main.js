var client = new Paho.MQTT.Client("broker.hivemq.com", 8000,"web_" + parseInt(Math.random() * 100, 10));  
client.onConnectionLost = onConnectionLost;  
client.onMessageArrived = onMessageArrived;

var options = { useSSL: false,userName: "",password: " ",onSuccess: onConnect , onFailure: doFail } 


var checkbox_lamp_1 = document.getElementById('btn1');
var lamp1 = document.getElementById('lamp1');
var checkbox_lamp_2 = document.getElementById('btn2');
var lamp2 = document.getElementById('lamp2');
var checkbox_lamp_3 = document.getElementById('btn3');
var lamp3 = document.getElementById('lamp3');
var checkbox_lamp_4 = document.getElementById('btn4');
var lamp4 = document.getElementById('lamp4');

client.connect(options);  
    
function onConnect()
{
    console.log("Connect tới Server");
    client.subscribe("dangcongtinh/inforTBRelay");
    client.subscribe("dangcongtinh/inforTBALL");	 		
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


var flagMessArrive = false;

function onMessageArrived(message) 
{
    console.log("Data ESP:" + message.payloadString);
    var topicSub = message.destinationName;

    if(topicSub == "dangcongtinh/inforTBALL")
    {
        var dataInforTBAll = message.payloadString;

        var dataInforTBAllJson = JSON.parse(dataInforTBAll);
        var nhietDo = dataInforTBAllJson.nhietdo;
        var doAm = dataInforTBAllJson.doam;
        if(nhietDo != null)
        {
            nhietDo = nhietDo.toFixed(2);
        }

        if (doAm != null)
        {
            doAm = doAm.toFixed(2);
        }
        // console.log(dataInforTBAllJson.nhietdo.toFixed(2));
        // console.log(dataInforTBAllJson.doam.toFixed(2));
        document.getElementById("nhiet_do").innerHTML = nhietDo;
        document.getElementById("do_am").innerHTML = doAm;

        if(flagMessArrive == false)
        {
            if(dataInforTBAllJson.den1 == 1)
            {
                document.getElementById("status1").innerHTML = "Bật";
    
                //change status
                checkbox_lamp_1.checked = true;
                lamp1.classList.add('yellow');
            }
            else
            {
                document.getElementById("status1").innerHTML = "Tắt";
                checkbox_lamp_1.checked = false;
                lamp1.classList.remove('yellow');
            }

            if(dataInforTBAllJson.den2 == 1)
            {
                document.getElementById("status2").innerHTML = "Bật";
                checkbox_lamp_2.checked = true;
                lamp2.classList.add('yellow');
            }
            else
            {
                document.getElementById("status2").innerHTML = "Tắt";
                checkbox_lamp_2.checked = false;
                lamp2.classList.remove('yellow');
            }

    
            if(dataInforTBAllJson.den3 == 1)
            {
                document.getElementById("status3").innerHTML = "Bật";
                checkbox_lamp_3.checked = true;
                lamp3.classList.add('yellow');
            }
            else
            {
                document.getElementById("status3").innerHTML = "Tắt";
                checkbox_lamp_3.checked = false;
                lamp3.classList.remove('yellow');
            }

    
            if(dataInforTBAllJson.quat == 1)
            {
                document.getElementById("status4").innerHTML = "Bật";
                checkbox_lamp_4.checked = true;
                lamp4.classList.add('yellow');
            }
            else
            {
                document.getElementById("status4").innerHTML = "Tắt";
                checkbox_lamp_4.checked = false;
                lamp4.classList.remove('yellow');
            }

            if(dataInforTBAllJson.coi == 1)
            {
                document.getElementById("bao_dong").innerHTML = "Bật";
            }
            else
                document.getElementById("bao_dong").innerHTML = "Tắt";

            flagMessArrive = true;
        } 

    }

    if(topicSub == "dangcongtinh/inforTBRelay")
    {
        var dataInforTBARelay = message.payloadString;
        if(dataInforTBARelay == 11)
        {
            document.getElementById("status1").innerHTML = "Bật";
            checkbox_lamp_1.checked = true;
            lamp1.classList.add('yellow');
        }
        else if(dataInforTBARelay == 10)
        {
            document.getElementById("status1").innerHTML = "Tắt";
            checkbox_lamp_1.checked = false;
            lamp1.classList.remove('yellow');
        }

        if(dataInforTBARelay == 21)
        {
            document.getElementById("status2").innerHTML = "Bật";
            checkbox_lamp_2.checked = true;
            lamp2.classList.add('yellow');
        }
        else if(dataInforTBARelay == 20)
        {
            document.getElementById("status2").innerHTML = "Tắt";
            checkbox_lamp_2.checked = false;
            lamp2.classList.remove('yellow');
        }


        if(dataInforTBARelay == 31)
        {
            document.getElementById("status3").innerHTML = "Bật";
            checkbox_lamp_3.checked = true;
            lamp3.classList.add('yellow');
        }
        else if(dataInforTBARelay == 30)
        {
            document.getElementById("status3").innerHTML = "Tắt";
            checkbox_lamp_3.checked = false;
            lamp3.classList.remove('yellow');
        }


        if(dataInforTBARelay == 41)
        {
            document.getElementById("status4").innerHTML = "Bật";
            checkbox_lamp_4.checked = true;
            lamp4.classList.add('yellow');
        }
        else if(dataInforTBARelay == 40)
        {
            document.getElementById("status4").innerHTML = "Tắt";
            checkbox_lamp_4.checked = false;
            lamp4.classList.remove('yellow');
        }

        if(dataInforTBARelay == 51)
        {
            document.getElementById("bao_dong").innerHTML = "Bật";
        }
        else if(dataInforTBARelay == 50)
            document.getElementById("bao_dong").innerHTML = "Tắt";
    }  

}

