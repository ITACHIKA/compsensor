#include <WiFi.h>

void initApWifi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpwd);
  WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
  IPAddress ip = WiFi.softAPIP();
  Serial.print("Initiating in AP mode.\nAP IP address: ");
  Serial.println(ip);
}

void initStaWifi()
{
  WiFi.begin(ssid, pwd);
  Serial.print("Connecting to designated AP");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Connected");
  IPAddress ip = WiFi.localIP();
  Serial.print("Sta IP address: ");
  Serial.println(ip);
}
