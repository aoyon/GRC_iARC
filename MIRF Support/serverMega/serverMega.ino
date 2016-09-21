
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>



char fieldMap[50];
short daughterShipPos=0; 
short motherShipPos=-1;
const  char nogo=125;

void mirf_init()
{
  
  Mirf.cePin = 49;
  Mirf.csnPin = 47;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte*)"serv1");
  Mirf.payload = 1;
  Mirf.config();

}

void updateField(char x)
{
  if(x=='L'||x=='R'||x=='F'||x=='S')
      fieldMap[++motherShipPos]=x;
}

void communicate()
{
    if(!Mirf.isSending() && Mirf.dataReady())
  {
    Mirf.getData((byte*)&daughterShipPos);
    Serial.print("Querry Recieved : ");
    Serial.println(daughterShipPos);
    
    Mirf.setTADDR((byte *)"clie1");
    if(daughterShipPos>motherShipPos)
      Mirf.send((byte*)&nogo);
    else
      Mirf.send((byte*)&fieldMap[daughterShipPos]);

    Serial.print( "Current Map:");
    Serial.println(fieldMap);
    Serial.print("Reply sent : ");
    Serial.println(fieldMap[daughterShipPos]);
  }
}

void setup()
{
  Serial.begin(9600);
  mirf_init();
  Serial.println("Listening..."); 
}

void loop(){

  if(Serial.available())
  {
    char x=Serial.read();
    updateField(x);
  }
  
  communicate();
}






