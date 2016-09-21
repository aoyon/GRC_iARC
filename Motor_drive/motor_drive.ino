

#define LMPin 11
#define LFMPin 9
#define LBMPin 10
#define RMPin 3
#define RFMPin 5
#define RBMPin 6

void motorDrive(int LFMSpeed,int RFMSpeed)
{
  if(LFMSpeed>220)LFMSpeed=220;
  if(RFMSpeed>220)RFMSpeed=220; 
  analogWrite(RMPin,RFMSpeed);
  analogWrite(LMPin,LFMSpeed);
}


void setup()
{
  Serial.begin(9600);
  pinMode(LMPin,OUTPUT);
  pinMode(RMPin,OUTPUT);
  pinMode(LFMPin,OUTPUT);
  pinMode(LBMPin,OUTPUT);
  pinMode(RFMPin,OUTPUT);
  pinMode(RBMPin,OUTPUT);
  
  
  

}


/*****************************End of Main Setup********************************************/

/*****************************Main Loop****************************************************/
void loop()
{
  Serial.println("BOTH FF");
  digitalWrite(LFMPin,HIGH); 
  digitalWrite(LBMPin,LOW);
  digitalWrite(RFMPin,HIGH); 
  digitalWrite(RBMPin,LOW);
  motorDrive(200,200);
  delay(2000);
  Serial.println("LEFT F");
  motorDrive(200,0);
  delay(2000);
  Serial.println("RIGHT F");
  motorDrive(0,200);
  delay(2000);
  Serial.println("LEFT CC");
  digitalWrite(LFMPin,LOW); 
  digitalWrite(LBMPin,HIGH);
  digitalWrite(RFMPin,HIGH); 
  digitalWrite(RBMPin,LOW);
  motorDrive(200,200);
  delay(3000);
  
  Serial.println("RIGHT CC");
  digitalWrite(LFMPin,HIGH); 
  digitalWrite(LBMPin,LOW);
  digitalWrite(RFMPin,LOW); 
  digitalWrite(RBMPin,HIGH);
  motorDrive(200,200);
  delay(3000);
  
  
}


