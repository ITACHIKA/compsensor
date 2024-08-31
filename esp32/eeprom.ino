#include <Wire.h>

char *readFromEEPROM(int addr) {
  Wire.beginTransmission(EEPROM_I2C_ADDR);  // 开始I2C传输
  Wire.write((int)(addr >> 8));             // 高位地址
  Wire.write((int)(addr & 0xFF));           // 低位地址
  Wire.endTransmission();                   // 结束传输

  Wire.requestFrom(EEPROM_I2C_ADDR, 1);  // 从EEPROM请求数据
  int strlen = Wire.read();
  char str[strlen];

  Wire.beginTransmission(EEPROM_I2C_ADDR);  // 开始I2C传输
  Wire.write((int)((addr + 1) >> 8));       // 高位地址
  Wire.write((int)((addr + 1) & 0xFF));     // 低位地址
  Wire.endTransmission();                   // 结束传输

  Wire.requestFrom(EEPROM_I2C_ADDR, strlen);  // 从EEPROM请求数据
  int i = 0;
  while (Wire.available() && i < strlen) {  // 读取数据并保存到数组中
    str[i] = (char)Wire.read();
    i++;
  }
  return str;
}


struct paramEEPROM {
  char *radioTypE;
  char *apSSId;
  char *apPWd;
  char *staSSId;
  char *staPWd;
};



/*void writeNetToEEPROM(void *pvParameters) {
  paramEEPROM *params = (paramEEPROM *)pvParameters;

  char* radioType=params->radioTypE;
  char* apSSID= params->apSSId;
  char* apPWD=params->apPWd;
  char* staSSID=params->staSSId;
  char* staPWD=params->staPWd;

  Serial.println(radioType);
  Serial.println(apSSID);
  Serial.println(apPWD);
  Serial.println(staSSID);
  Serial.println(staPWD);

  int apSSIDlen = strlen(apSSID);
  int apPWDlen = strlen(apPWD);
  int staSSIDlen = strlen(staSSID);
  int staPWDlen = strlen(staPWD);

  // Write ApSSID string to EEPROM at addr 20, the following codes are similar
  int addr = 20;  // EEPROM 的起始地址
  Wire.beginTransmission(EEPROM_I2C_ADDR);
  Wire.write((int)(addr >> 8));    // 高位地址
  Wire.write((int)(addr & 0xFF));  // 低位地址
  Wire.write(apSSIDlen);
  for (int i = 0; i < apSSIDlen; i++) {
    Wire.write(apSSID[i]);
  }
  Wire.endTransmission();

  addr = 45;  // EEPROM 的起始地址
  Wire.beginTransmission(EEPROM_I2C_ADDR);
  Wire.write((int)(addr >> 8));    // 高位地址
  Wire.write((int)(addr & 0xFF));  // 低位地址
  Wire.write(apPWDlen);
  for (int i = 0; i < apPWDlen; i++) {
    Wire.write(apSSID[i]);
  }
  Wire.endTransmission();

  addr = 70;  // EEPROM 的起始地址
  Wire.beginTransmission(EEPROM_I2C_ADDR);
  Wire.write((int)(addr >> 8));    // 高位地址
  Wire.write((int)(addr & 0xFF));  // 低位地址
  Wire.write(staSSIDlen);
  for (int i = 0; i < staSSIDlen; i++) {
    Wire.write(staSSID[i]);
  }
  Wire.endTransmission();

  addr = 95;  // EEPROM 的起始地址
  Wire.beginTransmission(EEPROM_I2C_ADDR);
  Wire.write((int)(addr >> 8));    // 高位地址
  Wire.write((int)(addr & 0xFF));  // 低位地址
  Wire.write(apSSIDlen);
  for (int i = 0; i < staPWDlen; i++) {
    Wire.write(staPWD[i]);
  }
  Wire.endTransmission();

  addr = 100;  // EEPROM 的起始地址
  Wire.beginTransmission(EEPROM_I2C_ADDR);
  Wire.write((int)(addr >> 8));    // 高位地址
  Wire.write((int)(addr & 0xFF));  // 低位地址
  Wire.write(radioType[0]);
  Wire.endTransmission();

  vTaskDelete(NULL);

  }*/
