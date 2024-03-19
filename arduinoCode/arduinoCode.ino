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

int BOOT_HR = -1;
int BOOT_MIN = -1;
int BOOT_SEC = -1;

DS1302 rtc(RTC_RST,RTC_DAT,RTC_CLK);
SoftwareSerial btser(2, 3);

#define EEPROM_ADDR_WRITE 0xA0
#define EEPROM_ADDR_READ 0xA1
#define EEPROM_I2C_ADDR 0x50

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btser.begin(9600);
  Wire.begin();

  TCNT1=0;
  TCCR1A = 0; 
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // WGM12设为1表示CTC模式，CS12和CS10设置预分频器为1024
  OCR1A = 15624;  // 计算得到的计数器初始值，用于实现1秒的间隔，16MHz的时钟频率和1024的预分频器
  TIMSK1 = (1 << OCIE1A);  // 开启Timer1的比较中断*/

  pinMode(4, INPUT);
  pinMode(5, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Time curtime=rtc.time();

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
    Serial.print(BOOT_HR);
    Serial.print(BOOT_MIN);
    Serial.println(BOOT_SEC);
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
    if(header=="Ontime")
    {
      char* hr=strtok(NULL,",");
      char* min=strtok(NULL,",");
      char* sec=strtok(NULL,",");
      char* timedat=strcat(hr,strcat(min,sec));
      Serial.println(timedat);
      BOOT_HR=atoi(hr);
      BOOT_MIN=atoi(min);
      BOOT_SEC=atoi(sec);
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
    if(header=="Resettime")
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
  }
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    String header=strtok(data.c_str(),",");
    //btser.println(data);
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

/*ISR(TIMER1_COMPA_vect) {  // Timer1的比较中断服务程序
  Serial.println("1111sads");  // 在串口上输出消息
}*/
