#include "DS1302.h"

void showTime(void *parameter)
{
  Time curtime = rtc.time();
  int hr=curtime.hr;
  int min=curtime.minu;
  Serial.println(hr);
  Serial.println(min);
  Serial.println(BOOT_HR);
  Serial.println(BOOT_MIN);
  vTaskDelete(NULL);
}

