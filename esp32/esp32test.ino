#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DS1302.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define RTC_RST 14
#define RTC_DAT 12
#define RTC_CLK 13

int BOOT_HR = -1;
int BOOT_MIN = -1;
int BOOT_SEC = -1;

DS1302 rtc(RTC_RST,RTC_DAT,RTC_CLK);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Time curtime=rtc.time();
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

  Serial.print(curtime.yr);
  Serial.print(" ");
  Serial.print(curtime.hr);
  Serial.print(" ");
  Serial.print(curtime.minu);
  Serial.print(" ");
  Serial.println(curtime.sec);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    char dataArray[data.length() + 1]; // +1 是为了存储字符串结尾的空字符
    data.toCharArray(dataArray, sizeof(dataArray));
    String header = strtok(dataArray, ",");
    Serial.println(data);
    Serial.println(header);
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
