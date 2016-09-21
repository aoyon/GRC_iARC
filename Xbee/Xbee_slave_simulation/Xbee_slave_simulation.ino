
char fieldMap[50];
short daughterShipPos=0;

void updateField(char x)
{
  if(x=='L'||x=='R'||x=='F'||x=='S')
    fieldMap[daughterShipPos]=x;
  fieldMap[daughterShipPos+1]=0;  

}

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(300);
  while(Serial2.available())
    Serial2.read();
  while(Serial2.available())
    Serial2.read();



}
void communicate()
{ 



  if(Serial.available())
  {
    unsigned char x=Serial.read();
    if(x=='n')
    {
      daughterShipPos++;

    }
    else
    {
      Serial.print("Sending -> ");
      Serial.println(daughterShipPos);

      while(Serial2.available())
        Serial2.read();

      Serial2.write(daughterShipPos);
      while(!Serial2.available());

      unsigned char currentDecision=Serial2.read();
      while(Serial2.available())
        Serial2.read();
      Serial.print("Recieved -> ");
      Serial.println(currentDecision);
      updateField(currentDecision);

      if(currentDecision == 125)
      {
        Serial.println("Received Value : Wait");
        delay(1000);

      }
      else 
      {
        Serial.print("Received Value : ");
        Serial.println((char)currentDecision);
        Serial.println(fieldMap);
      }
    }
  }

}



void loop()
{

  communicate();



}







