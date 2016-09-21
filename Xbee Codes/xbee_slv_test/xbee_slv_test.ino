#include <SoftwareSerial.h>

SoftwareSerial xBee(3,2);

String text = "";
boolean xBeeMsg;

void setup()
{
  //Serial.begin(9600);
  xBee.begin(9600); 
}

void loop()
{
//  text = "";
//  while(Serial.available() > 0)
//  {
//    char x = Serial.read();
//    text += x;
//    xBeeMsg = true;
//  }
//  while(xBee.available() > 0)
//  {
//    char x = Serial.read();
//    text += x;
//    xBeeMsg = false;
//  }
//  
//
//  if(text.length() > 0)
//  {
//    if(xBeeMsg)
//    {
//      xBee.print("xBee 1 : ");
//      xBee.println(text);
//    }
//    
//    else
//    {
//      Serial.print("xBee 2 : ");
//      Serial.println(text);
//    }
//  }  
      xBee.print('1');
  delay(50);
}
  
  
