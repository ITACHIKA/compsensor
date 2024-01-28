#include<SoftwareSerial.h>

SoftwareSerial btser(2,3);

void setup() {
  // put your setup code here, to run once:
  btser.begin(38400);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    String data=Serial.readString();
    btser.println(data);
    Serial.println(data);
  }
  if(btser.available())
  {
    Serial.print(btser.readString());
  }
}
