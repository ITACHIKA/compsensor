#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include "DS1302.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define RTC_RST 6
#define RTC_DAT 7
#define RTC_CLK 8

DS1302 rtc(RTC_RST,RTC_DAT,RTC_CLK);

SoftwareSerial btser(2, 3);

#define EEPROM_ADDR_WRITE 0xA0;
#define EEPROM_ADDR_READ 0xA1;
#define EEPROM_I2C_ADDR 0x50;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void disSysInfo(String cpu, String mem, String netIn, String netOut) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
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
  Wire.begin();

  pinMode(4, INPUT);
  pinMode(5, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  disSysInfo("", "", "", "");
  Time curtime=rtc.time();
  if(curtime.yr==2000)
  {
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH/2-26,SCREEN_HEIGHT/2);
    display.println("RTC Error");
    display.display();
  }
  rtc.writeProtect(false);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (btser.available())
  {
    String btdata=btser.readStringUntil('\n');
    String header=strtok(btdata.c_str(),",");
    if(btdata=="poweron")
    {
      digitalWrite(5,HIGH);
      delay(500);
      digitalWrite(5,LOW);
    }
    if(btdata=="poweroff")
    {
      digitalWrite(5,HIGH);
      delay(5000);
      digitalWrite(5,LOW);
    }
    if(header=="Settime")
    {
      char* hr=strtok(NULL,",");
      char* min=strtok(NULL,",");
      char* sec=strtok(NULL,",");
      char* timedat=strcat(hr,strcat(min,sec));
      Serial.println(timedat);
    }
  }
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    String header=strtok(data.c_str(),",");
    if (header == "init") {
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
      char* hostname = strtok(NULL,",");
      Serial.println(hostname);
      char* cpu = strtok(NULL,",");
      char* mem = strtok(NULL,",");
      char* netIn = strtok(NULL,",");
      char* netOut = strtok(NULL, ",");
      //disSysInfo(cpu,mem,netIn,netOut);
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
