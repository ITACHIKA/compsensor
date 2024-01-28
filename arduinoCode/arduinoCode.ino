#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include "DS1302.h"
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int datPin = 7;  // 连接到DS1302的DAT引脚
const int clkPin = 8;  // 连接到DS1302的CLK引脚
const int rstPin = 6;  // 连接到DS1302的RST引脚

DS1302 rtc(rstPin, datPin, clkPin);

SoftwareSerial btser(3,2);
String hostname;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


String twoDigits(int number) {
  if (number < 10) {
    return "0" + String(number);
  }
  return String(number);
}

void disSysInfo(String cpu,String mem,String netIn,String netOut)
{
  //Serial.print("dispsys");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  //display.println(hostname);
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btser.begin(9600);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
  /*if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }*/
  //disSysInfo("","","","");
  rtc.writeProtect(false);
    Time now = rtc.time();

  // 格式化时间和日期
  String formattedTime = String(now.hr) + ":" + twoDigits(now.minu) + ":" + twoDigits(now.sec);
  String formattedDate = String(now.yr) + "-" + twoDigits(now.mon) + "-" + twoDigits(now.day);

  // 输出到 Serial
  Serial.print("Current Time: ");
  Serial.println(formattedTime);

  Serial.print("Current Date: ");
  Serial.println(formattedDate);
}

int mapNumberToDay(int number) {
    switch (number) {
      case 6: return 1;
      case 0: return 2;
      case 1: return 3;
      case 2: return 4;
      case 3: return 5;
      case 4: return 6;
      case 5: return 7;
      default: return 1;  // 如果输入数字无效，默认为星期日
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Time timedata=rtc.time();
  //Serial.println(rtc.time().sec);
  if(btser.available())
  {
    String btdata=btser.readStringUntil('\n');
    Serial.println(btdata);
    if(btdata=="poweron")
    {
      digitalWrite(5,HIGH);
      delay(500);
      digitalWrite(5,LOW);
    }
    else if(btdata=="poweroff")
    {
      digitalWrite(5,HIGH);
      delay(5000);
      digitalWrite(5,LOW);
    }
    else
    {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc,btdata);
      String header=doc["Header"];
      if(header == "init")
      {
        hostname=doc["hostname"].as<String>();
        Serial.println(hostname);
        //disSysInfo("1","","","");
        int hr=doc["hr"];
        int minu=doc["min"];
        int sec=doc["sec"];
        int year=doc["year"];
        int month=doc["month"];
        int date=doc["date"];
        Serial.print(year);
        Serial.print(month);
        Serial.print(date);
        Time newTime(year,month,date,hr,minu,sec,mapNumberToDay(doc["weekday"]));
        rtc.time(newTime);
      }
      else if(header == "halt")
      {
        disSysInfo("","","","");
      }
      else if(header == "ddata")
      {
        String cpu=doc["cpu"];
        String mem=doc["mem"];
        String netIn=doc["netI"];
        String netOut=doc["netO"];
      
        //disSysInfo(statList[0],statList[1],statList[2],statList[3]);
        //Serial.print("dispsys");
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
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
  if(Serial.available()) 
  {
  }
}
