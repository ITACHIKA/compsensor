#include<SoftwareSerial.h>

SoftwareSerial btser(3,2);

void setup() {
  // put your setup code here, to run once:
  btser.begin(38400);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0)
  {
    String data=Serial.readString();
    btser.println(data);
  }
  if(btser.available()>0)
  {
    String data=btser.readString();
    Serial.println(data);
  }
}
