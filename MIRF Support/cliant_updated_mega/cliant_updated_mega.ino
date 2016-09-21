
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


short daughterShipPos=0; 

boolean doCommunicate = true;

void mirf_init()
{
  Mirf.cePin = 49;
  Mirf.csnPin = 47;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 1;
  Mirf.config();

}

void setup()
{
  Serial.begin(9600);
  mirf_init();
  Serial.println("Starting daughter bot"); 
}

void loop()
{
  if(Serial.available())
  {
    mirf_init();
    char x=Serial.read();
    if(x=='n')
    {
      daughterShipPos++;
      doCommunicate = true;
    }
    else if(!Mirf.isSending())
    {
      while(doCommunicate == true)
      {
        Serial.print("Sending Querry -> ");
        Serial.println(daughterShipPos);
        Mirf.setTADDR((byte *)"serv1");
        Mirf.send((byte*)&daughterShipPos);

        while(Mirf.isSending());

        Serial.println("Finished sending");
        delay(10);
        while(!Mirf.dataReady());
        char currentDecision;

        Mirf.getData((byte *) &currentDecision);
        if(currentDecision==125)
        {
          Serial.print("Received Value : ");
          Serial.println("Wait");
          doCommunicate = true;
        }
        else
        {
          Serial.print("Received Value : ");
          Serial.println(currentDecision);
          doCommunicate = false;
        }
        delay(1000);
      }
    }    
  }



}














