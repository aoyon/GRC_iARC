void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(300);
  Serial2.flush();
  Serial.flush();



}


void loop()
{

  if(Serial2.available())
  {
    char x=Serial2.read();
    while(Serial2.available())
      Serial2.read();
    x+=3;
    Serial2.print(x);
  }



}


