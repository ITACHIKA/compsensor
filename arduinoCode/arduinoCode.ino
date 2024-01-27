#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

SoftwareSerial btser(3,2);
String hostname;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void disSysInfo(String cpu,String mem,String netIn,String netOut)
{
  //Serial.print("dispsys");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  //display.println(hostname);
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btser.begin(9600);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  disSysInfo("","","","");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(btser.available())
  {
    String btdata=btser.readString();
    Serial.println(btdata);
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
  }
  if(Serial.available()) 
  { // 如果串口有数据
    String data = Serial.readString(); // 读取串口数据
    if(data == "a114514")
    {
      Serial.println("good");
      hostname=Serial.readString();
    }
    else if(data == "upstreamStop")
    {
      disSysInfo("","","","");
    }
    else
    {
      btser.println(data);
      delay(10);
      if(btser.available())
      {
        Serial.println(btser.readString());
      }
      String statList[4];
      int cnt=0;
      char *token = strtok(const_cast<char*>(data.c_str()), ",");
      while (token != NULL && cnt < 4) 
      {
        statList[cnt] = token;
        token = strtok(NULL, ",");
        cnt++;
      }
      for(int i=0;i<4;i++)
      {
        Serial.println(statList[i]);
      }
      
      //disSysInfo(statList[0],statList[1],statList[2],statList[3]);
      //Serial.print("dispsys");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.println(hostname);
      display.print("CPU : ");
      display.print(statList[0]);
      display.println("%");
      display.print("MEM : ");
      display.print(statList[1]);
      display.println("%");
      display.print("Net in: ");
      display.print(statList[2]);
      display.println("Mbps");
      display.print("Net out: ");
      display.print(statList[3]);
      display.println("Mbps");
      display.display();
      
    }
  }
}
