#include <TimerOne.h>
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
//Command

#include <SoftwareSerial.h>

#define TX 11
#define RX 10
SoftwareSerial Serialmaster(TX, RX);

const char MASTER = 'M';
const char ENDLINE = 'E';
const char SLAVE = 'S';
const char CMD_CLOSE = 'C';
const char CMD_OPEN = 'O';
const char CMD_SOS = 'S';
const char CMD_MOTION = 'P';

char STATE = 'E';
const char STATE_ERROR = 'R';
const char STATE_CLOSE = 'C';
const char STATE_OPEN = 'O';
const char STATE_SOS = 'N';
const char STATE_MOTION = 'P';

int const DOORADDRESSFROM = 51;

struct DoorClient {
  String Address;
  char DoorStatus;
  int pinR;
  int pinG;
  int pinB; 
};
int pinLED[] = {52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24};
String SlaveAddress[] = {"A1", "A2", "A3", "A4", "A5"};
int slaveindex = 0;
int pirState = LOW; 

DoorClient DoorClientArray[5];


void setup() {
  Serial.begin(9600);
  Serialmaster.begin(9600);
  int i, j;
  
  for (i = 0; i < 15; i++)
    pinMode(pinLED[i], OUTPUT);    
  
  for(int i = 0; i < 5; i++)
  {
    DoorClientArray[i].Address = SlaveAddress[i];
    DoorClientArray[i].DoorStatus = STATE_OPEN;
    DoorClientArray[i].pinR = pinLED[3*i + 0];
    DoorClientArray[i].pinG = pinLED[3*i + 1];
    DoorClientArray[i].pinB = pinLED[3*i + 2];
  }

  Timer1.initialize(1000000);
//  Timer1.initialize(30000000);//30s
  Timer1.attachInterrupt(timer1);
}
void timer1()
{
  //Sent to Salve
  delay(1000);
  String strSendData = "";
  strSendData = MASTER + SlaveAddress[slaveindex] + ENDLINE;
  char buff[100];
  strSendData.toCharArray(buff, 10);
  //Serial.println(buff);
  Serialmaster.write(buff);
  Serialmaster.flush();
  slaveindex++;
  if (slaveindex >= 4)slaveindex = 0; 
}
void loop() 
{
  //Read Data From Slave
  if(Serialmaster.available() > 0)
  {
//    Serial.print("Read ");
    char readChar = Serialmaster.read();
    //Serial.println(readChar);
    delayMicroseconds(500); 
//    Timer1.initialize(5000000);
    String slaveData = "";
    if (readChar == SLAVE)
    {
      while (Serialmaster.available())
      {
        readChar = Serialmaster.read();
        delay(10);
        if (readChar == ENDLINE) break;
        slaveData += readChar;
      }
      
      Serial.println(slaveData);
      ReceiveData(slaveData);      
    } 
  }  
}

int index = 0;
void ReceiveData(String slaveData)
{
  DoorClient door;
  door.Address= slaveData.substring(0,2);
  door.DoorStatus = slaveData.charAt(2);
  
  for (index = 0; index < sizeof(SlaveAddress); index++)
    if (door.Address == SlaveAddress[index]) break;

  if (door.DoorStatus != DoorClientArray[index].DoorStatus)
  {
    DoorClientArray[index].DoorStatus = door.DoorStatus;
    Serial.println("======== DOOR INFO =======");
    Serial.println("Address: " + String(door.Address));    
    Serial.println("DoorStatus: " + String(door.DoorStatus));    
    switch (door.DoorStatus)
    {
      case STATE_ERROR:
        Serial.println("Door Status: ERROR");
      break;        
      case STATE_CLOSE:
        digitalWrite(DoorClientArray[index].pinR, LOW);
        digitalWrite(DoorClientArray[index].pinG, HIGH);
        digitalWrite(DoorClientArray[index].pinB, LOW);
        Serial.println("Door Status: CLOSE");
      break;
      case STATE_OPEN:
        digitalWrite(DoorClientArray[index].pinR, LOW);
        digitalWrite(DoorClientArray[index].pinG, LOW);
        digitalWrite(DoorClientArray[index].pinB, HIGH);         
        Serial.println("Door Status: OPEN");
      break;
      case STATE_SOS:
        digitalWrite(DoorClientArray[index].pinR, HIGH);
        digitalWrite(DoorClientArray[index].pinG, LOW);
        digitalWrite(DoorClientArray[index].pinB, LOW);          
        Serial.println("Door Status: SOS");
      break;
      case STATE_MOTION:
        digitalWrite(DoorClientArray[index].pinR, HIGH);
        digitalWrite(DoorClientArray[index].pinG, HIGH);
        digitalWrite(DoorClientArray[index].pinB, HIGH);          
        Serial.println("Door Status: SOS");
      break;          
    }
  }  
}
