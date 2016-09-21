

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);  
}
 
void loop()
{
  while(Serial.available() == 0);
  
  int data = Serial.read();
  
  Serial.println(data);
  Serial3.println(data);
  Serial.flush();
}
  
  
