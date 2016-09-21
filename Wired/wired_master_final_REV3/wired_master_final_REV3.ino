#include <EEPROM.h>
#include <SPI.h>

//#define dBug

#define LMPin 11
#define LFMPin 9
#define LBMPin 10
#define RMPin 3
#define RFMPin 5
#define RBMPin 6
#define leftFlagIndicator 18
#define rightFlagIndicator 19
#define selectPin 21


/*States*/
boolean state = LOW;
boolean menu = true;
boolean debug = false;
/*States*/

/*Communication*/
char fieldMap[50];
short daughterShipPos=0; 
short motherShipPos=-1;
const char nogo=125;
/*Communication*/


/*Sensor Related Variables*/

unsigned int sensor[] = {
  0,0,0,0,0,0,0,0};
unsigned int sensorState[] = {
  0,0,0,0,0,0,0,0};  
unsigned int whiteValue[] = {
  2000,2000,2000,2000,2000,2000,2000,2000};
unsigned int greenValue[] = {
  0,0,0,0,0,0,0,0};
unsigned int greenValueMin[] = {
  2000,2000,2000,2000};
unsigned int blackValue[] = {
  0,0,0,0};
unsigned int whiteThresHold[] = {
  0,0,0,0,0,0,0,0};
unsigned int greenThresHold[] = {
  0,0,0,0};

float error = 0.00;
float lastError = 0.00;
float count = 0.00;
float sum = 0.00;
/*Sensor Related Variables*/

byte command;
unsigned short levelCount=0;

/*Flag Related*/
boolean LF = false;
boolean RF = false;
boolean LeftTurn = false;
boolean RightTurn = false;
unsigned int maxSpeed = 120;
unsigned int savedMaxSpeed;
unsigned int control = 30;
unsigned int motorDelayTime = 2000;
/*Flag Related*/

/******************************EEPROM FUNCTIONS***********************************************/
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void readEEPROMData()
{
  whiteThresHold[0] = EEPROMReadInt(0);
  delay(10);
  whiteThresHold[1] = EEPROMReadInt(4);
  delay(10);
  whiteThresHold[2] = EEPROMReadInt(8);
  delay(10);
  whiteThresHold[3] = EEPROMReadInt(12);
  delay(10);
  whiteThresHold[4] = EEPROMReadInt(16);
  delay(10);
  whiteThresHold[5] = EEPROMReadInt(20);
  delay(10);
  whiteThresHold[6] = EEPROMReadInt(24);
  delay(10);
  whiteThresHold[7] = EEPROMReadInt(28);
  delay(10);

  greenThresHold[0] = EEPROMReadInt(32);
  delay(10);
  greenThresHold[1] = EEPROMReadInt(36);
  delay(10);
  greenThresHold[2] = EEPROMReadInt(40);
  delay(10);
  greenThresHold[3] = EEPROMReadInt(44);
  delay(10);

  maxSpeed = EEPROMReadInt(50);
  delay(10);
}
/*******************End of EEPROM FUNCTIONS***************************************************/


/*******************************Show Sensor Conditons****************************************/
void showSensorState()
{
  sensorRead();
  for(int i=0;i<8;i++)
  {
    if(i>=4)
    {
      if(sensor[i] >= whiteThresHold[i])
        sensorState[i] = 1;  
      else if(sensor[i] >= greenThresHold[i-4])
        sensorState[i] = 0;  
      else
        sensorState[i] = 2;
      continue;
    }
    if(sensor[i] >= whiteThresHold[i])
      sensorState[i] = 1;  
    else 
      sensorState[i] = 0;  
  }
}

void show()
{
  for(int i=0; i<8;i++)
  {
    if(i <= 6)
    {
      Serial.print(sensor[i]);
      Serial.print(" | ");
    }
    if(i == 7)
    {
      Serial.println(sensor[i]);
    }
  }
}
/*change*/




/*********************************Sensor Input*********************************/
unsigned int sensorCalcSum;

unsigned int readAnalog(int i)
{
  sensorCalcSum=0;
  for(int j=0;j<4;j++)
    sensorCalcSum += analogRead(i+8);
  sensor[i]=sensorCalcSum>>2;
}

void sensorRead()
{  
  for(int i=0; i<8; i++)
  {
    sensor[i] = readAnalog(i);
  }

  error = 0.00;
  count = 0.00;
  sum = 0.00;

  for(int i=0;i<4;i++)
  {
    if(sensor[i] >= whiteThresHold[i])
    {
      count++; 
      if(i<2)
        sum+=i-2;
      else if(i>=2)
        sum+=i-1;  
    }
  }


  if(count == 0)
    error = lastError;
  else if(count==4)
    error=-lastError;
  else
    error=float(sum/count);


  if(levelCount%2)
  {
    if((sensor[4] < greenThresHold[0]) || (sensor[6] < greenThresHold[2]))
    {
#ifdef dBug
      Serial.println("leftflag");
      show();
#endif

      maxSpeed=105;   //Reduce Speed After Flag
      LF = true;
      digitalWrite(leftFlagIndicator,HIGH);
    }
    else if(((sensor[6] > whiteThresHold[6]) || (sensor[7] > whiteThresHold[7])) && (LF))
    {
#ifdef dBug
      Serial.println("leftturn");
      show();
#endif

      LeftTurn = true;
      levelCount++;

      //Serial.println(levelCount);
      digitalWrite(leftFlagIndicator,LOW);

      updateField('L');
      Serial.println(fieldMap);
    }
    else if((sensor[6] > whiteThresHold[6]) && !(LF))
    {
      updateField('F');
      Serial.println(fieldMap);
      while(readAnalog(6) > whiteThresHold[6]);
      delay(10);
    }


    if((sensor[5] < greenThresHold[1]) || (sensor[7] < greenThresHold[3]))
    {
#ifdef dBug
      Serial.println("rightflag");
      show();
#endif
      maxSpeed=105;
      RF = true;
      digitalWrite(rightFlagIndicator,HIGH);
    }
    else if((sensor[7] > whiteThresHold[7]) && (RF))
    {
#ifdef dBug
      Serial.println("rightturn");
      show();
#endif
      RightTurn = true;
      levelCount++;
      //Serial.println(levelCount);
      digitalWrite(rightFlagIndicator,LOW);
      updateField('R');
      Serial.println(fieldMap);
    }  
    else if(((sensor[6] > whiteThresHold[6]) || (sensor[7] > whiteThresHold[7])) && !(RF))
    {
      updateField('F');
      Serial.println(fieldMap);
      while(readAnalog(7) > whiteThresHold[7]);
      delay(10);
    }
  }

  else
  {
    if((sensor[4] < greenThresHold[0]) || (sensor[6] < greenThresHold[2]))
    {
#ifdef dBug
      Serial.println("leftflag");
      show();
#endif

      maxSpeed=105;   //Reduce Speed After Flag
      LF = true;
      digitalWrite(leftFlagIndicator,HIGH);
    }
    else if((sensor[6] > whiteThresHold[6]) && (LF))
    {
#ifdef dBug
      Serial.println("leftturn");
      show();
#endif

      LeftTurn = true;
      levelCount++;

      //Serial.println(levelCount);
      digitalWrite(leftFlagIndicator,LOW);

      updateField('L');
      Serial.println(fieldMap);
    }
    else if((sensor[6] > whiteThresHold[6]) && !(LF))
    {
      updateField('F');
      Serial.println(fieldMap);
      while(readAnalog(6) > whiteThresHold[6]);
      delay(10);
    }


    if((sensor[5] < greenThresHold[1]) || (sensor[7] < greenThresHold[3]))
    {
#ifdef dBug
      Serial.println("rightflag");
      show();
#endif
      maxSpeed=105;
      RF = true;
      digitalWrite(rightFlagIndicator,HIGH);
    }
    else if((sensor[7] > whiteThresHold[7]) && (RF))
    {
#ifdef dBug
      Serial.println("rightturn");
      show();
#endif
      RightTurn = true;
      levelCount++;
      //Serial.println(levelCount);
      digitalWrite(rightFlagIndicator,LOW);
      updateField('R');
      Serial.println(fieldMap);
    }  
    else if((sensor[7] > whiteThresHold[7]) && !(RF))
    {
      updateField('F');
      Serial.println(fieldMap);
      while(readAnalog(7) > whiteThresHold[7]);
      delay(10);
    }
  }
  lastError = error;
}




/*********************************Calibration Functions*********************************/

void calibrateWhite()
{
  for(int i=0; i<8;i++)
  {
    whiteValue[i] = min(whiteValue[i],readAnalog(i));
  }
}

void calibrateGreen()
{
  for(int i=0; i<8;i++)
  {
    greenValue[i] = max(greenValue[i],readAnalog(i));
  }
  greenValueMin[0] = min(greenValueMin[0],readAnalog(4)); //left
  greenValueMin[1] = min(greenValueMin[1],readAnalog(5)); //Right
  greenValueMin[2] = min(greenValueMin[2],readAnalog(6)); //left
  greenValueMin[3] = min(greenValueMin[3],readAnalog(7)); //Right
}

void calibrateBlack()
{
  blackValue[0] = max(blackValue[0],readAnalog(4)); //left
  blackValue[1] = max(blackValue[1],readAnalog(5)); //Right
  blackValue[2] = max(blackValue[2],readAnalog(6)); //left
  blackValue[3] = max(blackValue[3],readAnalog(7)); //Right
}

void thresHold()
{
  for(int i=0; i<8;i++)
  {
    whiteThresHold[i] = (whiteValue[i] + greenValue[i])/2;
  }


  greenThresHold[0] = (blackValue[0]+greenValueMin[0])/2;
  greenThresHold[1] = (blackValue[1]+greenValueMin[1])/2;
  greenThresHold[2] = (blackValue[2]+greenValueMin[2])/2;
  greenThresHold[3] = (blackValue[3]+greenValueMin[3])/2;


  EEPROMWriteInt(0,whiteThresHold[0]);
  delay(10);
  EEPROMWriteInt(4,whiteThresHold[1]);
  delay(10);
  EEPROMWriteInt(8,whiteThresHold[2]);
  delay(10);
  EEPROMWriteInt(12,whiteThresHold[3]);
  delay(10);
  EEPROMWriteInt(16,whiteThresHold[4]);
  delay(10);
  EEPROMWriteInt(20,whiteThresHold[5]);
  delay(10);
  EEPROMWriteInt(24,whiteThresHold[6]);
  delay(10);
  EEPROMWriteInt(28,whiteThresHold[7]);
  delay(10);

  EEPROMWriteInt(32,greenThresHold[0]);
  delay(10);
  EEPROMWriteInt(36,greenThresHold[1]);
  delay(10);
  EEPROMWriteInt(40,greenThresHold[2]);
  delay(10);
  EEPROMWriteInt(44,greenThresHold[3]);
  delay(10);
}


/****************************End of Calibration Functions*********************************/


/****************************Communication Functions**************************************/


void updateField(char x)
{
  if(x=='L'||x=='R'||x=='F'||x=='S')
    fieldMap[++motherShipPos]=x;
}

void communicate()
{
  if(Serial2.available())
  {
    daughterShipPos=Serial2.read();
    while(Serial2.available())
      Serial2.read();
    /*
    Serial.print("Querry Recieved : ");
     Serial.println(daughterShipPos);
     */
    if(daughterShipPos>motherShipPos)
      Serial2.write(nogo);
    else
      Serial2.print(fieldMap[daughterShipPos]);
    /*
    Serial.print( "Current Map:");
     Serial.println(fieldMap);
     Serial.print("Reply sent : ");
     Serial.println(fieldMap[daughterShipPos]);
     */
  }
}


/****************************Communication Functions**************************************/


/********************************Bot Setup************************************************/
void botSetup()
{
  digitalWrite(leftFlagIndicator,HIGH);
  digitalWrite(rightFlagIndicator,LOW);
  Serial.println("---------------------");
  Serial.println("    I am Alive !");
  Serial.println("---------------------");
  delay(250);
  digitalWrite(leftFlagIndicator,LOW);
  digitalWrite(rightFlagIndicator,HIGH);
  Serial.println(" ");
  Serial.println("Setup Menu y/n");
  delay(250);
  digitalWrite(leftFlagIndicator,HIGH);
  digitalWrite(rightFlagIndicator,HIGH);

  while(digitalRead(selectPin) == LOW)
  {
    if(Serial.available())
      command = Serial.read();
    else
      command = '*';
    switch(command)
    {
    case 'y':
      menu = true;
      Serial.println("Setup Modes :");
      Serial.println("             'S' - Sensor Readings");
      Serial.println("             'C' - Sensor State");
      Serial.println("             'W' - WhiteLine Calibration");
      Serial.println("             'G' - GreenLine Calibration");
      Serial.println("             'B' - BlackLine Calibration");
      Serial.println("             'D' - Final Calibration");
      Serial.println("             'M' - Motor MaxSpeed");
      Serial.println("             'T' - Motor Test");
      Serial.println("             'R' - EEEPROM Values");
      Serial.println("             'Z' - Debug During Run");
      Serial.println("             'E' - Exit");
      break;
    case 'n':
      menu = false;
      Serial.println("Now Press Menu Button");
      break;
    default:
      menu = false;
      break;
    }

    while(menu == true)
    {
      while(!Serial.available());
      if(Serial.available())
        command = Serial.read();
      else
        command = '*';
      switch(command)
      {
      case 's':
        while(!Serial.available())
        {
          sensorRead();
          for(int i=0; i<8;i++)
          {
            if(i <= 6)
            {
              Serial.print(sensor[i]);
              Serial.print(" | ");
            }
            if(i == 7)
            {
              Serial.println(sensor[i]);
            }
          }
          delay(300); 
        }
        Serial.flush();
        break;
      case 'c':
        while(!Serial.available())
        {
          showSensorState();
          for(int i=0; i<8;i++)
          {
            if(i <= 6)
            {
              Serial.print(sensorState[i]);
              Serial.print(" | ");
            }
            if(i == 7)
            {
              Serial.println(sensorState[i]);
            }
          }
          delay(300); 
        }
        Serial.flush();
        break;
      case 'w':
        calibrateWhite();
        Serial.println("White Reading : ");
        for(int i=0; i<8;i++)
        {
          if(i <= 6)
          {
            Serial.print(whiteValue[i]);
            Serial.print(" | ");
          }
          if(i == 7)
            Serial.println(whiteValue[i]);
        }
        break;
      case 'g':
        calibrateGreen();
        Serial.println("Green Reading Max: ");
        for(int i=0; i<8;i++)
        {
          if(i <= 6)
          {
            Serial.print(greenValue[i]);
            Serial.print(" | ");
          }
          if(i == 7)
            Serial.println(greenValue[i]);
        }

        Serial.println("Green Reading Min: ");

        Serial.print(greenValueMin[0]);
        Serial.print(" | ");
        Serial.print(greenValueMin[1]);
        Serial.print(" | ");
        Serial.print(greenValueMin[2]);
        Serial.print(" | ");
        Serial.println(greenValueMin[3]);

        break;
      case 'b':
        calibrateBlack();
        Serial.println("Black Reading : ");

        Serial.print(blackValue[0]);
        Serial.print(" | ");
        Serial.print(blackValue[1]);
        Serial.print(" | ");
        Serial.print(blackValue[2]);
        Serial.print(" | ");
        Serial.println(blackValue[3]);

        break;
      case 'm':
        Serial.print("Motor Max Speed = ");
        Serial.println(maxSpeed);
        Serial.println("Enter Max Speed = ");
        while(!Serial.available());
        while(Serial.available() > 0)
        {
          maxSpeed= Serial.parseInt();
          if (Serial.read() == '\n')
          {
            break;
          }
        }
        Serial.print("Motor Max Speed = ");
        Serial.println(maxSpeed);
        EEPROMWriteInt(50,maxSpeed);
        break;
      case 't':
        motorDrive(maxSpeed,maxSpeed);
        delay(2000);
        motorDrive(maxSpeed,0);
        delay(2000);
        motorDrive(0,maxSpeed);
        delay(2000);
        break;
      case 'd':
        thresHold();
        Serial.println("White Thresholds : ");
        for(int i=0; i<8;i++)
        {
          if(i <= 6)
          {
            Serial.print(whiteThresHold[i]);
            Serial.print(" | ");
          }
          if(i == 7)
            Serial.println(whiteThresHold[i]);
        }
        Serial.println("Green Thresholds : ");

        Serial.print(greenThresHold[0]);
        Serial.print(" | ");
        Serial.print(greenThresHold[1]);
        Serial.print(" | ");
        Serial.print(greenThresHold[2]);
        Serial.print(" | ");
        Serial.println(greenThresHold[3]);

        break;

      case 'r':
        readEEPROMData();
        Serial.println("From EEPROM - White Thresholds : ");
        for(int i=0; i<8;i++)
        {
          if(i <= 6)
          {
            Serial.print(whiteThresHold[i]);
            Serial.print(" | ");
          }
          if(i == 7)
            Serial.println(whiteThresHold[i]);
        }
        Serial.println("From EEPROM - Green Thresholds : ");
        Serial.println("Green Thresholds : ");

        Serial.print(greenThresHold[0]);
        Serial.print(" | ");
        Serial.print(greenThresHold[1]);
        Serial.print(" | ");
        Serial.print(greenThresHold[2]);
        Serial.print(" | ");
        Serial.println(greenThresHold[3]);

        Serial.print("From EEPROM - Motor Max Speed = ");
        Serial.println(maxSpeed);
        break;
      case 'z':
        debug = true;
        Serial.print("Debug during Run = ");
        Serial.println(debug);
        break;
      case '*':
        break;
      case 'e':
        menu = false;
        Serial.println("Exiting Setup Menu");
        digitalWrite(leftFlagIndicator,LOW);
        digitalWrite(rightFlagIndicator,LOW);
        break;
      default:
        Serial.println("Invalid Command");
        break;
      }
      Serial.flush();
    }
  }
  while(digitalRead(selectPin) == HIGH);
  delay(50);

  digitalWrite(leftFlagIndicator,HIGH);
  digitalWrite(rightFlagIndicator,HIGH);

  while(digitalRead(selectPin) == LOW)
  { 
    digitalWrite(leftFlagIndicator,state);
    digitalWrite(rightFlagIndicator,!state);
    delay(300);
    if(state == HIGH)
      state = LOW;
    else
      state = HIGH;
  } 
  while(digitalRead(selectPin) == HIGH);
  delay(50);
  readEEPROMData();
  digitalWrite(leftFlagIndicator,LOW);
  digitalWrite(rightFlagIndicator,LOW);
}


/********************************End of Bot Setup******************************************/

/************************************Bot Motion********************************************/
void motorDrive(int LFMSpeed,int RFMSpeed)
{
  if(LFMSpeed>220)LFMSpeed=220;
  if(RFMSpeed>220)RFMSpeed=220; 
  analogWrite(RMPin,RFMSpeed);
  analogWrite(LMPin,LFMSpeed);
}

void turn(boolean whichSide)
{
  if(levelCount%2==0)
    maxSpeed=savedMaxSpeed;
  /*forward thrust to get past line*/
  motorDrive(maxSpeed,maxSpeed);
  delay(20);

  /*turn to the required direction*/
  if(whichSide)			//whichSide=true -> left
    motorDrive(0,maxSpeed+30);
  else				//whichSide=false -> right
  motorDrive(maxSpeed+30,0);

  delay(50);

  while((readAnalog(0) > whiteThresHold[0]) || (readAnalog(1) > whiteThresHold[1]) || (readAnalog(2) > whiteThresHold[2]) || (readAnalog(3) > whiteThresHold[3]));

  if(whichSide)
    while(readAnalog(2)<whiteThresHold[2]);
  else
    while(readAnalog(1)<whiteThresHold[1]);

  /*reverse to find flag while tracking the connecting lines*/
  if(levelCount%2)
  {
    //Some backward thrust
    digitalWrite(LFMPin,LOW);
    digitalWrite(LBMPin,HIGH);
    digitalWrite(RFMPin,LOW);
    digitalWrite(RBMPin,HIGH);

    if(whichSide)
      motorDrive(maxSpeed-10,maxSpeed+10);
    else 
      motorDrive(maxSpeed+10,maxSpeed-10);

    delay(50);

    motorDrive(maxSpeed,maxSpeed);
    delay(150);

    RF = false;
    LF = false;				//reset flags
    LeftTurn = false;
    RightTurn = false;
    
    int cnt=0;
    while(cnt<40)
    { 
      sensorRead();
      if(LF||RF)
        break; 
      else if((readAnalog(6)>whiteThresHold[6])||(readAnalog(7)>whiteThresHold[7]))
      {
        digitalWrite(LFMPin,HIGH);
        digitalWrite(LBMPin,LOW);
        digitalWrite(RFMPin,HIGH);
        digitalWrite(RBMPin,LOW);
        motorDrive(maxSpeed,maxSpeed);
        break;
      }
      else if(error==0)
        motorDrive(maxSpeed,maxSpeed);
      else
        motorDrive((maxSpeed - (error*control)),(maxSpeed + (error*control)));
      delay(5);
      cnt++;

    }

//    while(1)
//    { 
//      sensorRead();
//      if(LF||RF)
//        break; 
//      else if((readAnalog(6)>whiteThresHold[6])&&(readAnalog(7)>whiteThresHold[7]))
//      {
//        digitalWrite(LFMPin,HIGH);
//        digitalWrite(LBMPin,LOW);
//        digitalWrite(RFMPin,HIGH);
//        digitalWrite(RBMPin,LOW);
//        motorDrive(maxSpeed,maxSpeed);
//        delay(150);
//        break;
//      } 
//      else if((readAnalog(6)>whiteThresHold[6])||(readAnalog(7)>whiteThresHold[7]))
//      {
//        if(readAnalog(6)>whiteThresHold[6])
//        {
//          digitalWrite(LFMPin,HIGH);
//          digitalWrite(LBMPin,LOW);
//          digitalWrite(RFMPin,HIGH);
//          digitalWrite(RBMPin,LOW);
//          motorDrive(maxSpeed,0);
//          delay(50);
//          motorDrive(maxSpeed,maxSpeed);
//          delay(50);
//          break;
//        }
//        else if(readAnalog(7)>whiteThresHold[7])
//        {
//          digitalWrite(LFMPin,HIGH);
//          digitalWrite(LBMPin,LOW);
//          digitalWrite(RFMPin,HIGH);
//          digitalWrite(RBMPin,LOW);
//          motorDrive(0,maxSpeed);
//          delay(50);
//          motorDrive(maxSpeed,maxSpeed);
//          delay(50);
//          break;
//        }
//      }
//
//      else if(error==0)
//        motorDrive(maxSpeed,maxSpeed);
//      else
//        motorDrive((maxSpeed - (error*control)),(maxSpeed + (error*control)));
//
//    }
  } 

  /*forward motion after flag or end of line reached*/
  digitalWrite(LFMPin,HIGH);  
  digitalWrite(LBMPin,LOW);
  digitalWrite(RFMPin,HIGH);  
  digitalWrite(RBMPin,LOW);
  motorDrive(maxSpeed,maxSpeed);

  //reset flags
  LeftTurn = false;
  RightTurn = false;
  if(!(levelCount%2))
  { 
    RF = false;
    LF = false;
  }
}

void bot_stop()
{

  digitalWrite(LFMPin,HIGH);  
  digitalWrite(LBMPin,LOW);
  digitalWrite(RFMPin,HIGH);  
  digitalWrite(RBMPin,LOW);
  motorDrive(maxSpeed,maxSpeed);
  delay(200);
  motorDrive(0,0);
  digitalWrite(RMPin,LOW);
  digitalWrite(LMPin,LOW);

  while(1)
  {
    communicate();
  }
}

/**************************end of Bot Motion Functions**************************************/


/*****************************Main Setup***************************************************/
void setup()
{
  Serial.begin(9600);
  pinMode(selectPin,INPUT);
  pinMode(LMPin,OUTPUT);
  pinMode(RMPin,OUTPUT);
  pinMode(LFMPin,OUTPUT);
  pinMode(LBMPin,OUTPUT);
  pinMode(RFMPin,OUTPUT);
  pinMode(RBMPin,OUTPUT);
  pinMode(leftFlagIndicator,OUTPUT);
  pinMode(rightFlagIndicator,OUTPUT);

  digitalWrite(LFMPin,HIGH); 
  digitalWrite(LBMPin,LOW);
  digitalWrite(RFMPin,HIGH); 
  digitalWrite(RBMPin,LOW);
  Serial2.begin(9600);
  Serial.println("Communication Initialized for MotherShip");
  while(Serial2.available())
    Serial2.read();
  while(Serial1.available())
    Serial1.read();
  botSetup();
  savedMaxSpeed=maxSpeed;

  motorDrive(maxSpeed,maxSpeed);
  while((readAnalog(6) > whiteThresHold[6])||(readAnalog(7) >whiteThresHold[7]))
    delay(5); 
}


/*****************************End of Main Setup********************************************/

/*****************************Main Loop****************************************************/
void loop()
{
  sensorRead();

  communicate();

  if((sensor[4]<greenThresHold[0])&&(sensor[5]<greenThresHold[1])&&(sensor[6]<greenThresHold[2])&&(sensor[7]<greenThresHold[3]))
  {
    bot_stop();  
  }
  else if(LeftTurn == true)
  {
    turn(true);
  }
  else if(RightTurn == true)
  {
    turn(false);
  }

  else if(error==0)
    motorDrive(maxSpeed,maxSpeed);
  else
    motorDrive((maxSpeed + (error*control)),(maxSpeed - (error*control)));
}



