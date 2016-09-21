void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(300);
  while(Serial2.available())
    Serial2.read();
  Serial.flush();



}



void loop()
{
  char x;
  if(Serial.available())
  {
    x=Serial.read();
    Serial.flush();

    Serial.print("Sending -> ");
    while(Serial2.available())
      Serial2.read();
      
    Serial2.print(x);
    Serial.println(x);
    while(!Serial2.available());

    x=Serial2.read();
    Serial.print("Recieved -> ");
    Serial.println(x);    


  }




}




