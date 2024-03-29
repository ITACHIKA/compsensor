#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DS1302.h"
#include <Wire.h>
//#include "BluetoothSerial.h"
#include <SPIFFS.h>
#include "ESPAsyncWebServer.h"
#include <WiFi.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define RTC_RST 5
#define RTC_DAT 4
#define RTC_CLK 2

#define EEPROM_ADDR_WRITE 0xA0
#define EEPROM_ADDR_READ 0xA1
#define EEPROM_I2C_ADDR 0x50

int BOOT_HR = -1;
int BOOT_MIN = -1;
int BOOT_SEC = -1;

const char* ssid="ESP32_ITACHIKA";
const char* pwd="114514";

DS1302 rtc(RTC_RST,RTC_DAT,RTC_CLK);
//BluetoothSerial btser;
AsyncWebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //btser.begin("ESP32");
  pinMode(27,OUTPUT);
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(ssid,pwd);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);
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

  Time curtime=rtc.time();
  if(curtime.yr==2000)
  {
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH/2-26,SCREEN_HEIGHT/2);
    display.println("RTC Error");
    display.display();
  }

  //recover lost time data

  int addr=0;
  Wire.beginTransmission(EEPROM_I2C_ADDR); // 开始I2C传输
  Wire.write((int)(addr >> 8));   // 高位地址
  Wire.write((int)(addr & 0xFF)); // 低位地址
  Wire.endTransmission(); // 结束传输
  
  int length =6;
  int timeStr[length];
  Wire.requestFrom(EEPROM_I2C_ADDR, length); // 从EEPROM请求数据
  int i = 0;
  while (Wire.available() && i < length) { // 读取数据并保存到数组中
    timeStr[i]=Wire.read();
    i++;
  }

  if(timeStr[i]!=255)
  {
    BOOT_HR=(timeStr[0]-48)*10+(timeStr[1]-48);
    BOOT_MIN=(timeStr[2]-48)*10+(timeStr[3]-48);
    BOOT_SEC=(timeStr[4]-48)*10+(timeStr[5]-48);
  }
  rtc.writeProtect(false);
}

void web_server(){
 if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS发生错误");
    return;
  }
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}

void loop() {
  /*if (btser.available())
  {
    String btdata=btser.readStringUntil('\n');
    char btArray[btdata.length() + 1]; // +1 是为了存储字符串结尾的空字符
    btdata.toCharArray(btArray, sizeof(btArray));
    String header = strtok(btArray, ",");
    if(btdata=="poweron")
    {
      digitalWrite(27,HIGH);
      delay(500);
      digitalWrite(27,LOW);
    }
    if(btdata=="poweroff")
    {
      digitalWrite(27,HIGH);
      delay(5000);
      digitalWrite(27,LOW);
    }
    if(header=="ontime")
    {
      char* hr=strtok(NULL,",");
      BOOT_HR=atoi(hr);
      char* min=strtok(NULL,",");
      BOOT_MIN=atoi(min);
      char* sec=strtok(NULL,",");
      BOOT_SEC=atoi(sec);
      char* timedat=strcat(hr,strcat(min,sec));
      Serial.println(timedat);
      
      // 将拼接后的字符串写入 EEPROM
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
    if(header=="resettime")
    {
      BOOT_HR=-1;
      BOOT_MIN=-1;
      BOOT_SEC=-1;
      int addr = 0;  // EEPROM 的起始地址
      Wire.beginTransmission(EEPROM_I2C_ADDR);
      Wire.write((int)(addr >> 8));   // 高位地址
      Wire.write((int)(addr & 0xFF)); // 低位地址
      for (int i = 0; i < 6; i++) 
      {  // 6 是 timedat 的长度
        Wire.write(-1);  //清空
      }
      Wire.endTransmission();
    }
    if(header=="getcurset")
    {
      btser.print("Hour:");
      btser.print(BOOT_HR);
      btser.print(" Min:");
      btser.print(BOOT_MIN);
      btser.print(" Sec:");
      btser.print(BOOT_SEC);
    }
  }*/
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    char dataArray[data.length() + 1]; // +1 是为了存储字符串结尾的空字符
    data.toCharArray(dataArray, sizeof(dataArray));
    String header = strtok(dataArray, ",");
    //Serial.println(data);
    //Serial.println(header);
    //btser.println(data);
    if (header == "init") {
      char* hr = strtok(NULL, ",");
      char* minu = strtok(NULL, ",");
      char* sec = strtok(NULL, ",");
      Serial.print("h"+String(hr));
      Serial.print("m"+String(minu));
      Serial.print("s"+String(sec));
      //Time newTime(2024,1,1,atoi(hr),atoi(minu),atoi(sec),1);
      //rtc.time(newTime);
    }
    if (header == "halt") {
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
      char* hostname = strtok(NULL,",");
      char* cpu = strtok(NULL,",");
      char* mem = strtok(NULL,",");
      char* netIn = strtok(NULL,",");
      char* netOut = strtok(NULL, ",");
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
