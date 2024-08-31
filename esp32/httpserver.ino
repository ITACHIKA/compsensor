#include "ESPAsyncWebServer.h"

void handlePostRequest(AsyncWebServerRequest *request)
{
  // 检查请求类型是否为 POST
  if (request->method() == HTTP_POST)
  {
    Serial.println("POST request received");

    // 检查请求是否有正文
    if (request->hasParam("data", true))
    {
      AsyncWebParameter *param = request->getParam("data", true);
      String data = param->value();

      // 在这里对接收到的数据进行处理
      Serial.print("Received data: ");
      Serial.println(data.c_str());
      char dataArray[data.length() + 1]; // +1 是为了存储字符串结尾的空字符
      data.toCharArray(dataArray, sizeof(dataArray));
      String header = strtok(dataArray, ",");
      if (header == "onTime")
      {
        ON_EXECUTED=false;
        char *hr = strtok(NULL, ",");
        BOOT_HR = atoi(hr);
        char *min = strtok(NULL, ",");
        BOOT_MIN = atoi(min);
        char *sec = strtok(NULL, ",");
        BOOT_SEC = atoi(sec);
        char *timedat = strcat(hr, strcat(min, sec));

        Serial.println("Receive BOOT config:");
        Serial.print(BOOT_HR);
        Serial.print(BOOT_MIN);
        Serial.println(BOOT_SEC);

        int addr = 0; // EEPROM 的起始地址
        Wire.beginTransmission(EEPROM_I2C_ADDR);
        Wire.write((int)(addr >> 8));   // 高位地址
        Wire.write((int)(addr & 0xFF)); // 低位地址
        for (int i = 0; i < 6; i++)
        {                         // 6 是 timedat 的长度
          Wire.write(timedat[i]); // 逐个字节写入拼接后的字符串
        }
        Wire.endTransmission();
      }
      else if (header == "offTime")
      {
        OFF_EXECUTED=false;
        char *hr = strtok(NULL, ",");
        OFF_HR = atoi(hr);
        char *min = strtok(NULL, ",");
        OFF_MIN = atoi(min);
        char *sec = strtok(NULL, ",");
        OFF_SEC = atoi(sec);
        char *timedat = strcat(hr, strcat(min, sec));

        Serial.println("Receive OFF config:");
        Serial.print(OFF_HR);
        Serial.print(OFF_MIN);
        Serial.println(OFF_SEC);

        int addr = 10; // EEPROM 的起始地址
        Wire.beginTransmission(EEPROM_I2C_ADDR);
        Wire.write((int)(addr >> 8));   // 高位地址
        Wire.write((int)(addr & 0xFF)); // 低位地址
        for (int i = 0; i < 6; i++)
        {                         // 6 是 timedat 的长度
          Wire.write(timedat[i]); // 逐个字节写入拼接后的字符串
        }
        Wire.endTransmission();
      }
      else if (header == "netSet")
      {
        /*char *radioType = strtok(NULL, ",");
        char *apSSID = strtok(NULL, ",");
        char *apPWD = strtok(NULL, ",");
        char *staSSID = strtok(NULL, ",");
        char *staPWD = strtok(NULL, ",");

        paramEEPROM params = { radioType,apSSID, apPWD, staSSID, staPWD };

        vTaskDelay(pdMS_To_MS(10))

        xTaskCreatePinnedToCore(
          writeNetToEEPROM,  // 任务函数
          "writeNetToEEPROM",    // 任务名称
          2000,          // 堆栈大小（字节）
          &params,          // 任务参数
          0,             // 任务优先级
          NULL,          // 任务句柄
          0              // 核心编号（0 或 1）
        );
      }*/

        // 发送响应
        //request->send(200, "text/plain", "Setting received");
      }
      else
      {
        request->send(400, "text/plain", "No data received");
      }
    }
    else
    {
      request->send(405, "text/plain", "Method Not Allowed");
    }
  }
}

void httpServer()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.on("/", HTTP_POST, handlePostRequest);
  server.on("/init", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(getStatFreq) + "," + String(wifiType) + "," + String(APssid) + "," + String(APpwd) + "," + String(ssid) + "," + String(pwd)); });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(getStatFreq) + "," + String(curHostname) + "," + String(curCpu) + "," + String(curMem) + "," + String(curNetIn) + "," + String(curNetOut)); });
  server.on("/poweroff", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              xTaskCreatePinnedToCore(
                  powerOffTask, // 任务函数
                  "PowerOff",   // 任务名称
                  1000,         // 堆栈大小（字节）
                  NULL,         // 任务参数
                  0,            // 任务优先级
                  NULL,         // 任务句柄
                  0             // 核心编号（0 或 1）
              );
              request->send(200, "text/plain", "Device powered off"); // 发送响应
            });
  server.on("/poweron", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              xTaskCreatePinnedToCore(
                  powerOnTask, // 任务函数
                  "PowerOn",   // 任务名称
                  1000,        // 堆栈大小（字节）
                  NULL,        // 任务参数
                  0,           // 任务优先级
                  NULL,        // 任务句柄
                  0            // 核心编号（0 或 1）
              );
              request->send(200, "text/plain", "Device powered on"); // 发送响应
            });

  server.begin();
  Serial.println("HTTP server started");
}
