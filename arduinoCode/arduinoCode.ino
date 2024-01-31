#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include "DS1302.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

DS1302 rtc(6, 7, 8);

SoftwareSerial btser(2, 3);
String hostname;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void disSysInfo(String cpu, String mem, String netIn, String netOut) {
  //Serial.print("dispsys");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
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

  pinMode(4, INPUT);
  pinMode(5, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  disSysInfo("", "", "", "");
  rtc.writeProtect(false);
  Time now=rtc.time();
  Serial.print(now.hr);
  Serial.print(":");
  Serial.print(now.minu);
  Serial.print(":");
  Serial.println(now.sec);
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
  //Time timedata=rtc.time();
  //Serial.println(rtc.time().sec);
  if (btser.available()) {
    String btdata = btser.readStringUntil('\n');
    Serial.println(btdata);
    if (btdata == "poweron") {
      digitalWrite(5, HIGH);
      delay(500);
      digitalWrite(5, LOW);
    } 
    else if (btdata == "poweroff") {
      digitalWrite(5, HIGH);
      delay(5000);
      digitalWrite(5, LOW);
    } 
    else {
      String header=strtok(btdata.c_str(),",");
      if (header == "init") {
        hostname = String(strtok(NULL,","));
        Serial.println(hostname);
        //disSy sI   nfo("1","","","");
        char* hr = strtok(NULL, ",");
        char* minu = strtok(NULL, ",");
        char* sec = strtok(NULL, ",");
        Serial.print("h"+String(hr));
        Serial.print("m"+String(minu));
        Serial.print("s"+String(sec));
        Time newTime(2024,1,1,atoi(hr),atoi(minu),atoi(sec),1);
        rtc.time(newTime);
      }
      if (header == "halt") {
        disSysInfo("", "", "", "");
      }
      if (header == "data") {
        char* cpu = strtok(NULL,",");
        char* mem = strtok(NULL,",");
        char* netIn = strtok(NULL,",");
        char* netOut = strtok(NULL, ",");
        //disSysInfo(statList[0],statList[1],statList[2],statList[3]);
        //Serial.print("dispsys");
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
}
