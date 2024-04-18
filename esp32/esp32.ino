#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DS1302.h"
#include <Wire.h>
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include <WiFi.h>
#include <Arduino.h>
#include <esp_timer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define RTC_RST 17
#define RTC_DAT 4
#define RTC_CLK 2

#define EEPROM_ADDR_WRITE 0xA0
#define EEPROM_ADDR_READ 0xA1
#define EEPROM_I2C_ADDR 0x50

int BOOT_HR = 25;
int BOOT_MIN = 61;
int BOOT_SEC = 61;

int OFF_HR=25;
int OFF_MIN=61;
int OFF_SEC=61;

String curHostname="NaN";
double curCpu=0;
double curMem=0;
double curNetOut=0;
double curNetIn=0;
int getStatFreq=-1;

String APssid = "ITACHIKA_ESP32";
String APpwd = "11111111";
char* ssid = "smast";
char* pwd = "5085581232";

int wifiType=1;

DS1302 rtc(RTC_RST, RTC_DAT, RTC_CLK);

AsyncWebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void powerOffTask(void * parameter) {
  // 将针脚设置为输出模式
  Serial.println("PowerOff Task entry");
  digitalWrite(12, HIGH);
  vTaskDelay(pdMS_TO_TICKS(4000));
  digitalWrite(12, LOW);
  vTaskDelete(NULL);
}

void initApWifi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpwd);
  WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
  IPAddress ip = WiFi.softAPIP();
  Serial.print("Initiating in AP mode.\nAP IP address: ");
  Serial.println(ip);
}

void initStaWifi() {
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting to designated AP");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Connected");
  IPAddress ip = WiFi.localIP();
  Serial.print("Sta IP address: ");
  Serial.println(ip);
}

void handlePostRequest(AsyncWebServerRequest *request) {
  // 检查请求类型是否为 POST
  if (request->method() == HTTP_POST) {
    Serial.println("POST request received");

    // 检查请求是否有正文
    if (request->hasParam("data", true)) {
      AsyncWebParameter* param = request->getParam("data", true);
      String data = param->value();

      // 在这里对接收到的数据进行处理
      Serial.println("Received data: " + data);
      char dataArray[data.length() + 1];  // +1 是为了存储字符串结尾的空字符
      data.toCharArray(dataArray, sizeof(dataArray));
      String header = strtok(dataArray, ",");
      if(header=="onTime")
      {
        char* hr=strtok(NULL,",");
        BOOT_HR=atoi(hr);
        char* min=strtok(NULL,",");
        BOOT_MIN=atoi(min);
        char* sec=strtok(NULL,",");
        BOOT_SEC=atoi(sec);
        char* timedat=strcat(hr,strcat(min,sec));
        int addr = 0;  // EEPROM 的起始地址
        Wire.beginTransmission(EEPROM_I2C_ADDR);
        Wire.write((int)(addr >> 8));   // 高位地址
        Wire.write((int)(addr & 0xFF)); // 低位地址
        for (int i = 0; i < 6; i++) 
        {  // 6 是 timedat 的长度
          Wire.write(timedat[i]);  // 逐个字节写入拼接后的字符串
        }
        Wire.endTransmission();
      }
      if(header=="offTime")
      {
        char* hr=strtok(NULL,",");
        OFF_HR=atoi(hr);
        char* min=strtok(NULL,",");
        OFF_MIN=atoi(min);
        char* sec=strtok(NULL,",");
        OFF_SEC=atoi(sec);
        char* timedat=strcat(hr,strcat(min,sec));
        int addr = 10;  // EEPROM 的起始地址
        Wire.beginTransmission(EEPROM_I2C_ADDR);
        Wire.write((int)(addr >> 8));   // 高位地址
        Wire.write((int)(addr & 0xFF)); // 低位地址
        for (int i = 0; i < 6; i++) 
        {  // 6 是 timedat 的长度
          Wire.write(timedat[i]);  // 逐个字节写入拼接后的字符串
        }
        Wire.endTransmission();
      }
      if(header=="netSet")
      {
        //char* 
      }

      // 发送响应
      request->send(200, "text/plain", "Setting received");
    } else {
      request->send(400, "text/plain", "No data received");
    }
  } else {
    request->send(405, "text/plain", "Method Not Allowed");
  }
}

void httpServer()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.on("/", HTTP_POST, handlePostRequest);
  server.on("/init", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(getStatFreq)+","+String(APssid)+","+String(APpwd)+","+String(ssid)+","+String(pwd));
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(getStatFreq)+","+String(curHostname)+","+String(curCpu)+","+String(curMem)+","+String(curNetIn)+","+String(curNetOut));
  });
  server.on("/poweroff", HTTP_GET, [](AsyncWebServerRequest *request){
  xTaskCreatePinnedToCore(
    powerOffTask,   // 任务函数
    "PowerOff", // 任务名称
    1000,          // 堆栈大小（字节）
    NULL,           // 任务参数
    0,              // 任务优先级
    NULL,           // 任务句柄
    0               // 核心编号（0 或 1）
  );
  request->send(200, "text/plain", "Device powered off"); // 发送响应
  });
  server.on("/poweron", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(12, HIGH);
  delay(500);
  digitalWrite(12,LOW);
  request->send(200, "text/plain", "Device powered on"); // 发送响应
});

  server.begin();
  Serial.println("HTTP server started");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //btser.begin("ESP32");
  pinMode(12, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("CPU : ");
  display.println("%");
  display.print("MEM : ");
  display.println("%");
  display.print("Net in: ");
  display.println("Mbps");
  display.print("Net out: ");
  display.println("Mbps");
  display.display();

  Time curtime = rtc.time();
  if (curtime.yr == 2000) {
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH / 2 - 26, SCREEN_HEIGHT / 2);
    display.println("RTC Error");
    display.display();
  }

  //recover boot time from eeprom

  int addr = 0;
  Wire.beginTransmission(EEPROM_I2C_ADDR);  // 开始I2C传输
  Wire.write((int)(addr >> 8));             // 高位地址
  Wire.write((int)(addr & 0xFF));           // 低位地址
  Wire.endTransmission();                   // 结束传输

  int length = 6;
  int timeStr[length];
  Wire.requestFrom(EEPROM_I2C_ADDR, length);  // 从EEPROM请求数据
  int i = 0;
  while (Wire.available() && i < length) {  // 读取数据并保存到数组中
    timeStr[i] = Wire.read();
    i++;
  }

  if (timeStr[i] != 255) {
    BOOT_HR = (timeStr[0] - 48) * 10 + (timeStr[1] - 48);
    BOOT_MIN = (timeStr[2] - 48) * 10 + (timeStr[3] - 48);
    BOOT_SEC = (timeStr[4] - 48) * 10 + (timeStr[5] - 48);
  }

  Serial.print(BOOT_HR);
  Serial.print(BOOT_MIN);
  Serial.println(BOOT_SEC);

  //recover poweroff time from eeprom
  addr = 10;
  Wire.beginTransmission(EEPROM_I2C_ADDR);  // 开始I2C传输
  Wire.write((int)(addr >> 8));             // 高位地址
  Wire.write((int)(addr & 0xFF));           // 低位地址
  Wire.endTransmission();                   // 结束传输
  
  Wire.requestFrom(EEPROM_I2C_ADDR, length);  // 从EEPROM请求数据
  i = 0;
  while (Wire.available() && i < length) {  // 读取数据并保存到数组中
    timeStr[i] = Wire.read();
    i++;
  }

  if (timeStr[i] != 255) {
    OFF_HR = (timeStr[0] - 48) * 10 + (timeStr[1] - 48);
    OFF_MIN = (timeStr[2] - 48) * 10 + (timeStr[3] - 48);
    OFF_SEC = (timeStr[4] - 48) * 10 + (timeStr[5] - 48);
  }

  Serial.print(OFF_HR);
  Serial.print(OFF_MIN);
  Serial.println(OFF_SEC);
  
  rtc.writeProtect(false);

  if(wifiType)
  {
    initApWifi();
  }
  else
  { 
    initStaWifi();
  }

  SPIFFS.begin();

  httpServer();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    char dataArray[data.length() + 1];  // +1 是为了存储字符串结尾的空字符
    data.toCharArray(dataArray, sizeof(dataArray));
    String header = strtok(dataArray, ",");
    //Serial.println(data);
    //Serial.println(header);
    if (header == "init") {
      char* hr = strtok(NULL, ",");
      char* minu = strtok(NULL, ",");
      char* sec = strtok(NULL, ",");
      getStatFreq=atoi(strtok(NULL,","));
      Serial.print("h" + String(hr));
      Serial.print("m" + String(minu));
      Serial.print("s" + String(sec));
      Serial.println();
      Serial.println(getStatFreq);
      Time newTime(2024,1,1,atoi(hr),atoi(minu),atoi(sec),Time::kSunday);
      rtc.time(newTime);
    }
    if (header == "halt") {
      getStatFreq=-1;
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("CPU : ");
      display.println("%");
      display.print("MEM : ");
      display.println("%");
      display.print("Net in: ");
      display.println("Mbps");
      display.print("Net out: ");
      display.println("Mbps");
      display.display();
    }
    if (header == "data") {
      char* hostname = strtok(NULL, ",");
      char* cpu = strtok(NULL, ",");
      char* mem = strtok(NULL, ",");
      char* netIn = strtok(NULL, ",");
      char* netOut = strtok(NULL, ",");
      curHostname=String(hostname);
      curCpu=atof(cpu);
      curMem=atof(mem);
      curNetIn=atof(netIn);
      curNetOut=atof(netOut);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(hostname);
      display.print("CPU : ");
      display.print(cpu);
      display.println("%");
      display.print("MEM : ");
      display.print(mem);
      display.println("%");
      display.print("Net in: ");
      display.print(netIn);
      display.println("Mbps");
      display.print("Net out: ");
      display.print(netOut);
      display.println("Mbps");
      display.display();
    }
  }
}
