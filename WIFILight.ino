#include <FS.h>
#include <ESP8266WiFi.h>  
#include <ESP8266WebServer.h>   

// 设置WiFi名字和密码
const char *ssid = "LiMing"; 
const char *password = "12345678"; 

ESP8266WebServer esp8266_server(80);// 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）


void setup() {
  Serial.begin(9600);   // 开启串口监视器
  Serial.println("");
  pinMode(D6, OUTPUT);   // 初始化NodeMCU控制板载LED引脚为OUTPUT
  digitalWrite(D6, LOW);// 初始化LED引脚状态  

  WiFi.softAP(ssid, password);   // 使用接入点模式（AP）
  Serial.print("Access Point: ");    // 通过串口监视器输出信息
  Serial.println(ssid);              // WiFi名
  Serial.print("IP address: ");      
  Serial.println(WiFi.softAPIP());   // 得到NodeMCU的IP地址
  
  if(SPIFFS.begin()){ // 启动闪存文件
    Serial.println("SPIFFS Started.");
  } else {
    Serial.println("SPIFFS Failed to Start.");
  }
  
  esp8266_server.on("/setLight", handleLight); // 设置局部更新页面

  esp8266_server.onNotFound(handleUserRequet);      // 告知系统如何处理用户请求
  esp8266_server.begin();                           // 启动网站服务
  Serial.println("HTTP server started");
}


void loop() {
  // put your main code here, to run repeatedly:
   esp8266_server.handleClient();                    // 处理用户请求
}


void handleLight() {
 String lightState = "OFF";
 String state = esp8266_server.arg("LightState"); //对应Ajax的xhttp.open("GET", "setLight?LightState="+light, true);
 Serial.println(state);
 
 if(state == "1"){
  digitalWrite(D6,HIGH); //灯点亮
  lightState = "ON"; //反馈参数
 } else {
  digitalWrite(D6,LOW); //LED 熄灭
  lightState = "OFF"; //反馈参数
 }
 
 esp8266_server.send(200, "text/plain", lightState); //发送网页
}


// 处理用户浏览器的HTTP访问
void handleUserRequet() {         
  // 获取用户请求网址信息
  String webAddress = esp8266_server.uri();
  // 通过handleFileRead函数处处理用户访问
  bool fileReadOK = handleFileRead(webAddress);
  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!fileReadOK){                                                 
    esp8266_server.send(404, "text/plain", "404 Not Found"); 
  }
}

bool handleFileRead(String path) {            //处理浏览器HTTP访问
  if (path.endsWith("/")) {                   // 如果访问地址以"/"为结尾
    path = "/index.html";                     // 则将访问地址修改为/index.html便于SPIFFS访问
  } 
  String contentType = getContentType(path);  // 获取文件类型
  if (SPIFFS.exists(path)) {                     // 如果访问的文件可以在SPIFFS中找到
    File file = SPIFFS.open(path, "r");          // 则尝试打开该文件
    esp8266_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  return false;                                  // 如果文件未找到，则返回false
}

// 获取文件类型
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
