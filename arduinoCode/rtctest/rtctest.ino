#include "DS1302.h"

const int datPin = 7;  // 连接到DS1302的DAT引脚
const int clkPin = 8;  // 连接到DS1302的CLK引脚
const int rstPin = 6;  // 连接到DS1302的RST引脚

DS1302 rtc(rstPin, datPin, clkPin);

void setup() { 
  // put your setup code here, to run once:
  Serial.begin(9600);
  Time curtime(2024,01,01,00,00,00,1);
  rtc.writeProtect(false);
  rtc.time(curtime);
}

void loop() {
  
  // put your main code here, to run repeatedly:

  Time data=rtc.time();

  Serial.print(data.yr, DEC);
  Serial.print('/');
  Serial.print(data.mon, DEC);
  Serial.print('/');
  Serial.print(data.day, DEC);
  Serial.print(" ");
  Serial.print(data.hr, DEC);
  Serial.print(':');
  Serial.print(data.minu, DEC);
  Serial.print(':');
  Serial.println(data.sec, DEC);

  delay(1000);
}
