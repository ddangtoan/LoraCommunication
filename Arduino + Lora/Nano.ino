//  MASTER:
//  Command:  M: Master
//            A1: Address 1 -> 5
//            E: End
//
//  SLAVE:
//  Command:  S:  Slave
//            A1: Address 1-> 5
//            C:  C - Close; O - Open; S - SOS; P - Motion
//            E: End

#include <SoftwareSerial.h>

#define TX 3
#define RX 2

SoftwareSerial ClientSerial(TX, RX);

//Command
const char MASTER = 'M';
const char ENDLINE = 'E';
const char SLAVE = 'S';
const char CMD_CLOSE = 'C';
const char CMD_OPEN = 'O';
const char CMD_SOS = 'S';
const char CMD_MOTION = 'P';

int counter = 1000;
int nSensorInput = 0;
String ADDRESS = "A4";
int nSendData = 0;

const int RED_PIN = 4;    // led đỏ
const int BLUE_PIN = 5;  // led xanh dương
const int GREEN_PIN = 6;  // led xanh dương
const int PinSensor = 7; // pin sensor LJ
const int LED_MOTION = 8; // led sensor motion
const int PinSensorMotion = 9; // pin sensor motion

//const int M0_PIN = 10;  
//const int M1_PIN = 11;  

// int PARA_COUNT_ERROR = 9;
// int PARA_SEND_DELAY = 1;
// int PARA_COUNT_NOISE = 2;
// int PARA_COUNT_SOS = 3;

int SEND_DELAY = 100;
int COUNT_NOISE = 0;
int COUNT_SOS = 200;

char STATE = 'E';
char CurrentState = 'E';
const char STATE_ERROR = 'R';
const char STATE_CLOSE = 'C';
const char STATE_OPEN = 'O';
const char STATE_SOS = 'N';
const char STATE_MOTION = 'P';

int statusmotion = 0;
int StatusSensor =0;
int nCount;

char address = 0;

void setup() {
  
  Serial.begin(9600);
  ClientSerial.begin(9600);
  Serial.println("Door Start");
  
  pinMode(PinSensorMotion, INPUT);
  pinMode(PinSensor, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);  
  pinMode(LED_MOTION, OUTPUT);
  
//    pinMode(M0_PIN, OUTPUT); 
//  pinMode(M1_PIN, OUTPUT); 
//  digitalWrite (M0_PIN, LOW);
//  digitalWrite (M1_PIN, LOW);   
}

void loop() {
  //Sensor Motion
//  
//  if(statusmotion != digitalRead (PinSensorMotion))
//   {
//    statusmotion = digitalRead (PinSensorMotion);
//   }
//// statusmotion = digitalRead (PinSensorMotion);
//  if(statusmotion == 1){
//    LED_Display_Motion();
//  }else
//  {
//    LED_Display_Motion_Off();
//  }

  //Sensor LJ12A3-4Z/BX
   if(StatusSensor != digitalRead (PinSensor))
   {
    StatusSensor = digitalRead (PinSensor);
    nCount = 0;
   }
  if(StatusSensor == 1)
  {
    nCount++;
    if (nCount <= COUNT_SOS)
    {
        STATE = STATE_OPEN;
    } 
    else if(nCount > COUNT_SOS)
    {
        STATE = STATE_SOS;
    }
  }else if (StatusSensor == 0)
  {
    STATE = STATE_CLOSE; 
  }else
  {
    STATE = STATE_ERROR; 
  }


String strSendData = SLAVE + ADDRESS + STATE + ENDLINE;
//Serial.println(strSendData);
  char buff[20];
  strSendData.toCharArray(buff, 10);  
  
        
  if (STATE != CurrentState)
  {
    //Serial.println(STATE);
    CurrentState = STATE;
    ClientSerial.write(buff);
    ClientSerial.flush();    
    switch (STATE)
    {
      case STATE_CLOSE:
        LED_Display_DoorClose();
      break;
      case STATE_OPEN:
        LED_Display_DoorOpen();
      break;
      case STATE_SOS:
        LED_Display_SOS();
      break;
      case STATE_ERROR:
        LED_Display_ERROR();
      break;    
    //   case STATE_MOTION:
    //     LED_Display_Motion();
    //   break;    
    }   
  }

  
  if (ClientSerial.available() > 0){                  
    char readChar = ClientSerial.read();
    delay(5);
    String address = "";

    if (readChar == MASTER)
    {
      while (ClientSerial.available())
      {
        readChar = ClientSerial.read();
        delay(5);         
        if (readChar == ENDLINE) break;
        address += readChar;
      }
      Serial.print("Read: ");
      Serial.println(address);       
    }
 
    if (address == ADDRESS)
    {
      Serial.print("Send: ");
      //Serial.write(nSendData);

      
//  statusmotion = digitalRead (PinSensorMotion);
//    if(statusmotion == 1){
//      String strSendData1 = SLAVE + ADDRESS + STATE_MOTION + ENDLINE;
//      Serial.println(strSendData1);
//    }else
//    {
//      Serial.println(strSendData);
//    }
             
        Serial.println(strSendData);
      ClientSerial.write(buff);
      ClientSerial.flush();
    }
  }   
  delay(SEND_DELAY); 
}




void LED_Display_SOS()
{
    LED_Display_OFF();
    digitalWrite(RED_PIN, HIGH);
}

 void LED_Display_DoorOpen()
{
  LED_Display_OFF();
  digitalWrite(GREEN_PIN, HIGH);
}

 void LED_Display_DoorClose()
{
  LED_Display_OFF();
  digitalWrite(BLUE_PIN, HIGH);
}

void LED_Display_OFF()
{
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void LED_Display_ERROR()
{
  LED_Display_OFF();
  digitalWrite(RED_PIN, HIGH);
}

 void LED_Display_Motion()
{
  digitalWrite(LED_MOTION, HIGH);

  // LED_Display_OFF();
  // digitalWrite(RED_PIN, HIGH);
  // digitalWrite(GREEN_PIN, HIGH);
  // digitalWrite(BLUE_PIN, HIGH);
}

void LED_Display_Motion_Off()
{
  digitalWrite(LED_MOTION, LOW);
}
