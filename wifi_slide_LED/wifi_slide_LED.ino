#include "html510.h"
HTML510Server h(80);
#define LEDPIN 4

const char* ssid     = "xiwei"; //TP-Link_05AF
const char* password = "123456789"; //47543454

const int ledChannel = 0;      // Choose an available channel //6 channels for ledc in esp32c3, we choose channel 0
const int ledFrequency = 30;   // Frequency in 30hz
const int ledResolution = 14;  // Resolution in bits 0-16383

const char body[] PROGMEM = R"===(
 <!DOCTYPE html>
<html>
  <body>
    <input type="range" id="myRange" min="0" max="100" oninput="updateValue(this.value)"> Value: <span id="demo"></span>
    <script>
      function updateValue(value) {
        document.getElementById("demo").innerHTML = value;
        sendData(value); // 调用发送数据的函数
      }

      function sendData(value) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?val=" + value, true); // 发送GET请求, 将value作为参数
        xhr.send();
      }
    </script>
  </body>
</html>
  
)===";

void handleRoot(){
  h.sendhtml(body);
}

void handleH(){
  int x = h.getVal();
  int duty = map(x, 0, 100, 0, 16383);
  ledcWrite(ledChannel, duty);  // Set the duty cycle // Resolution in 14 bits 0-16383
  h.sendhtml(body);
}

void setup() {
  Serial.begin(115200);

  // Set up LEDC for PWM control
  pinMode(LEDPIN, OUTPUT);
  ledcSetup(ledChannel, ledFrequency, ledResolution);
  ledcAttachPin(LEDPIN, ledChannel);  // attach PWM to GPIO7

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address");
  Serial.println(IP);

  
  h.begin(); //attachHandler是用于将不同的URL路径和处理函数关联起来//找到URL发过来的是什么指令, （handleH/L/Root）并且进行对应的操作，最后返回一个html给客户端
  h.attachHandler("/slider?val=",handleH); //when user ask /H, call handleH
  // h.attachHandler("/L",handleL); //when user ask /L, call handleL
  h.attachHandler("/ ",handleRoot); //call handleRoot. It is main default interface for users
}

void loop() {
  h.serve();
  delay(10);
}
