#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "DS1302.h"

//#include "ESP32TimerInterrupt.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define RTC_RST 17
#define RTC_DAT 4
#define RTC_CLK 2

#define EEPROM_ADDR_WRITE 0xA0
#define EEPROM_ADDR_READ 0xA1
#define EEPROM_I2C_ADDR 0x50

//ESP32Timer ITimer(1);
//ESP32_ISR_Timer ISR_TIMER;
String APssid = "ITACHIKA_ESP32";
String APpwd = "11111111";
char *ssid = "smast";
char *pwd = "5085581232";
int wifiType=1;

int BOOT_HR = 25;
int BOOT_MIN = 61;
int BOOT_SEC = 61;

int curdate=-1;

int OFF_HR = 25;
int OFF_MIN = 61;
int OFF_SEC = 61;

bool ON_EXECUTED=false;
bool OFF_EXECUTED=false;

String curHostname = "NaN";
double curCpu = 0;
double curMem = 0;
double curNetOut = 0;
double curNetIn = 0;
int getStatFreq = -1;

DS1302 rtc(RTC_RST, RTC_DAT, RTC_CLK);

hw_timer_t *Timer0=NULL;

void powerOffTask(void *parameter)
{
  // 将针脚设置为输出模式
  Serial.println("PowerOff Task entry");
  digitalWrite(12, HIGH);
  vTaskDelay(pdMS_TO_TICKS(4000));
  digitalWrite(12, LOW);
  vTaskDelete(NULL);
}

void powerOnTask(void *parameter)
{
  // 将针脚设置为输出模式
  Serial.println("PowerOn Task entry");
  digitalWrite(12, HIGH);
  vTaskDelay(pdMS_TO_TICKS(500));
  digitalWrite(12, LOW);
  vTaskDelete(NULL);
}

void IRAM_ATTR Timer0_ISR()
{
    //Serial.println("interrupt");
    //digitalWrite(27,!digitalRead(27));
    Time curtime = rtc.time();
    int hr=curtime.hr;
    int min=curtime.minu;
    int year=curtime.yr;
    int mon=curtime.mon;
    int date=curtime.date;
    /*xTaskCreatePinnedToCore(
                  showTime, // 任务函数
                  "showTime",   // 任务名称
                  1000,         // 堆栈大小（字节）
                  NULL,         // 任务参数
                  0,            // 任务优先级
                  NULL,         // 任务句柄
                  0             // 核心编号（0 或 1）
    );*/
    if(date!=curdate)  //entered a new day
    {
      ON_EXECUTED=false;
      OFF_EXECUTED=false;
      curdate=date;
    }    
    if(BOOT_HR==hr && BOOT_MIN==min && !ON_EXECUTED)
    {
      ON_EXECUTED=true;
      //digitalWrite(27,!digitalRead(27));
      xTaskCreatePinnedToCore(
                  powerOnTask, // 任务函数
                  "PowerOn",   // 任务名称
                  1000,         // 堆栈大小（字节）
                  NULL,         // 任务参数
                  0,            // 任务优先级
                  NULL,         // 任务句柄
                  0             // 核心编号（0 或 1）
              );
    }
    if(hr==OFF_HR && min==OFF_MIN && !OFF_EXECUTED)
    {
      OFF_EXECUTED=true;
      //digitalWrite(27,!digitalRead(27));
      xTaskCreatePinnedToCore(
                  powerOffTask, // 任务函数
                  "PowerOff",   // 任务名称
                  1000,         // 堆栈大小（字节）
                  NULL,         // 任务参数
                  0,            // 任务优先级
                  NULL,         // 任务句柄
                  0             // 核心编号（0 或 1）
              );
    }
}

AsyncWebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // btser.begin("ESP32");
  pinMode(12, OUTPUT);
  pinMode(27, OUTPUT);

  Timer0=timerBegin(0,80,true);
  timerAttachInterrupt(Timer0,&Timer0_ISR,false);
  timerAlarmWrite(Timer0,5000000,true);
  timerAlarmEnable(Timer0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("I2C display failure.");
    for (;;)
      ;
  }
  Serial.println("I2C display initialized");

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
  if (curtime.yr == 2000)
  {
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH / 2 - 26, SCREEN_HEIGHT / 2);
    display.println("RTC Error");
    display.display();
  }

  Serial.println(String(curtime.yr));
  Serial.println(String(curtime.mon));
  Serial.println(String(curtime.date));
  Serial.println(String(curtime.hr));
  Serial.println(String(curtime.minu));
  Serial.println(String(curtime.sec));
  curdate=curtime.date;

  // recover boot time from eeprom

  int addr = 0;
  Wire.beginTransmission(EEPROM_I2C_ADDR); // 开始I2C传输
  Wire.write((int)(addr >> 8));            // 高位地址
  Wire.write((int)(addr & 0xFF));          // 低位地址
  if (Wire.endTransmission() != 0)         // 结束传输
  {
    Serial.println("EEPROM not installed or failed!");
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH / 2 - 28, SCREEN_HEIGHT / 2);
    display.print("EEPROM Error");
    display.display();
    for (;;)
      ;
  }

  int length = 6;
  int timeStr[length];
  Wire.requestFrom(EEPROM_I2C_ADDR, length); // 从EEPROM请求数据
  int i = 0;
  while (Wire.available() && i < length)
  { // 读取数据并保存到数组中
    timeStr[i] = Wire.read();
    i++;
  }

  if (timeStr[i] != 255)
  {
    BOOT_HR = (timeStr[0] - 48) * 10 + (timeStr[1] - 48);
    BOOT_MIN = (timeStr[2] - 48) * 10 + (timeStr[3] - 48);
    BOOT_SEC = (timeStr[4] - 48) * 10 + (timeStr[5] - 48);
  }

  Serial.println("BOOT config:");
  Serial.print(BOOT_HR);
  Serial.print(":");
  Serial.print(BOOT_MIN);
  Serial.print(":");
  Serial.println(BOOT_SEC);

  // recover poweroff time from eeprom
  addr = 10;
  Wire.beginTransmission(EEPROM_I2C_ADDR); // 开始I2C传输
  Wire.write((int)(addr >> 8));            // 高位地址
  Wire.write((int)(addr & 0xFF));          // 低位地址
  Wire.endTransmission();                  // 结束传输

  Wire.requestFrom(EEPROM_I2C_ADDR, length); // 从EEPROM请求数据
  i = 0;
  while (Wire.available() && i < length)
  { // 读取数据并保存到数组中
    timeStr[i] = Wire.read();
    i++;
  }

  if (timeStr[i] != 255)
  {
    OFF_HR = (timeStr[0] - 48) * 10 + (timeStr[1] - 48);
    OFF_MIN = (timeStr[2] - 48) * 10 + (timeStr[3] - 48);
    OFF_SEC = (timeStr[4] - 48) * 10 + (timeStr[5] - 48);
  }

  Serial.println("OFF config:");
  Serial.print(OFF_HR);
  Serial.print(":");
  Serial.print(OFF_MIN);
  Serial.print(":");
  Serial.println(OFF_SEC);

  addr = 100;                              // read type of WIFI -- 1 for AP, 0 for STA
  Wire.beginTransmission(EEPROM_I2C_ADDR); // 开始I2C传输
  Wire.write((int)(addr >> 8));            // 高位地址
  Wire.write((int)(addr & 0xFF));          // 低位地址
  Wire.endTransmission();                  // 结束传输

  Wire.requestFrom(EEPROM_I2C_ADDR, length); // 从EEPROM请求数据
  i = 0;
  while (Wire.available() && i < 1)
  { // 读取数据并保存到数组中
    wifiType = Wire.read();
    i++;
  }

  /*APssid=readFromEEPROM(20);
  APpwd=readFromEEPROM(45);
  ssid=readFromEEPROM(70);
  pwd=readFromEEPROM(95);*/

  /*Serial.println(readFromEEPROM(20));
  Serial.println(readFromEEPROM(45));
  Serial.println(readFromEEPROM(70));
  Serial.println(readFromEEPROM(95));*/

  rtc.writeProtect(false);

  if (wifiType)
  {
    initApWifi();
  }
  else
  {
    initStaWifi();
  }

  Serial.println("WIFI ready.");

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS error.");
    display.clearDisplay();
    display.setCursor(SCREEN_WIDTH / 2 - 28, SCREEN_HEIGHT / 2);
    display.println("SPIFFS Error");
    display.display();
    for (;;)
      ;
  }

  httpServer();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    char dataArray[data.length() + 1]; // +1 是为了存储字符串结尾的空字符
    data.toCharArray(dataArray, sizeof(dataArray));
    String header = strtok(dataArray, ",");
    // Serial.println(data);
    // Serial.println(header);
    if (header == "init")
    {
      char *year=strtok(NULL,",");
      char *month=strtok(NULL,",");
      char *date=strtok(NULL,",");
      char *hr = strtok(NULL, ",");
      char *minu = strtok(NULL, ",");
      char *sec = strtok(NULL, ",");
      getStatFreq = atoi(strtok(NULL, ","));
      Serial.print("y" + String(year));
      Serial.print("m" + String(month));
      Serial.print("d" + String(date));
      Serial.print("h" + String(hr));
      Serial.print("m" + String(minu));
      Serial.print("s" + String(sec));
      Serial.println();
      Serial.println(getStatFreq);
      Time newTime(atoi(year), atoi(month), atoi(date), atoi(hr), atoi(minu), atoi(sec), Time::kSunday);
      rtc.time(newTime);
    }
    if (header == "halt")
    {
      getStatFreq = -1;
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
    if (header == "data")
    {
      char *hostname = strtok(NULL, ",");
      char *cpu = strtok(NULL, ",");
      char *mem = strtok(NULL, ",");
      char *netIn = strtok(NULL, ",");
      char *netOut = strtok(NULL, ",");
      curHostname = String(hostname);
      curCpu = atof(cpu);
      curMem = atof(mem);
      curNetIn = atof(netIn);
      curNetOut = atof(netOut);
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
