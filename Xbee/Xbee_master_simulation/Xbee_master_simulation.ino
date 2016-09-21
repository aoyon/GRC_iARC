
#include <SPI.h>

/*Communication*/
char fieldMap[50];
short daughterShipPos=0; 
short motherShipPos=-1;
const char nogo=125;
/*Communication*/

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting Master bot");
  Serial2.begin(9600);
  Serial.println("Communication Initialized for MotherShip");
  while(Serial2.available())
    Serial2.read();
  while(Serial1.available())
    Serial1.read();

}

void communicate()
{
  if(Serial2.available())
  {
    daughterShipPos=Serial2.read();
    while(Serial2.available())
      Serial2.read();
    
    Serial.print("Querry Recieved : ");
     Serial.println(daughterShipPos);
     
    if(daughterShipPos>motherShipPos)
      Serial2.write(nogo);
    else
      Serial2.print(fieldMap[daughterShipPos]);
    
    Serial.print( "Current Map:");
     Serial.println(fieldMap);
     Serial.print("Reply sent : ");
     Serial.println(fieldMap[daughterShipPos]);
     
  }
}
void updateField(char x)
{
  if(x=='L'||x=='R'||x=='F'||x=='S')
    fieldMap[++motherShipPos]=x;
}

void loop()
{

  communicate();
  if(Serial.available())
  {
    char x=Serial.read();
    updateField(x);
    Serial.print( "Current Map:");
     Serial.println(fieldMap);
    
    
  }



}
















