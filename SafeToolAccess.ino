#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

String uuid = "";
boolean uuidGood = false;
int machineId = EEPROM.read(0);
SoftwareSerial base = SoftwareSerial(10,11);//pins for the communication to the base

int swipeTimer = 0;
int relockTimer = 0;
#define MIN_TILL_TIMEOUT 2

#define START_MACHINE_IDS 0

void setup() {//Inits and things
  Serial.begin(9600);
  base.begin(9600);
  uuid.reserve(30);
  
//  if(machineId = -1){
//    Serial.print("Please enter a Machine ID for this machine.");
//    while(!Serial.available());
//    int id = Serial.read();//replace with button to enter id number
//    if(id > 0 && id <256 && idAvailable(id)){
//      
//    }
//    
//  //first pos = this machine id
//  // rest are populated with existing on boot
//  //run checks to avoid conflicts
//  }
}

void loop() {//Main Loop
  updateTimers();
  if(swipeTimer >= 250 && uuidGood){
    if(authUser(uuid)){
        unlockMachine();
    }
    resetSwipeTimer();
  }
  
  updateTimers();
  if(relockTimer > MIN_TILL_TIMEOUT*60000){//convert to ms
    if(!isMachineBusy()){
      lockMachine();
    }
    else{
      resetRelockTimer();
    }
  }

  
  
}
//*********************************************************************************
//***Handle input******************************************************************
//*********************************************************************************
void serialEvent(){
  while (Serial.available()) {
    uuid += (char)Serial.read(); 
    }
  uuid = uuid.substring(uuid.indexOf(":"),uuid.indexOf("?"));//Shorten to just track 2
  uuid = uuid.substring(0,uuid.length()-1);//Strip lost card number
  if(uuid.length() == 9){
    uuidGood = true;
  }
  else{
    Serial.println("Bad swipe, try again.");
  }   
}

//boolean idAvailable(int id){
//  for(int i = 1; i <256; i++){
//    if(EEPROM.read(i) == id){
//      return false;
//    }
//  }
//  return true;
//}

//*********************************************************************************
//***Authentification and communication to base************************************
//*********************************************************************************
boolean authUser(String id){
  String message = "U," + id + "," + machineId + "?\n";
  sendRequest(message);
  while(!base.available());
  int userLevel = base.read();
  if(userLevel == 0){//user allowed to use
    Serial.println("You are allowed to use this machine");
    Serial.println("Please have a buddy swipe");
    while(!uuidGood);
    uuidGood = false;
    if(authBuddy(uuid, id)){
      return true;
    }    
  }
  else if(userLevel == 1){//user needs a trained person of level 0
    Serial.println("You need a supervisor");
    Serial.println("Please have a supervisor swipe");
    while(!uuidGood);
    uuidGood = false;
    if(authSuper(uuid)){
      return true;
    }
  }
  else{
    return false;
  }
}

boolean authBuddy(String buddyId, String userId){
  if(buddyId.equals(userId)){
    Serial.print("you cannot be your own buddy");
    return false;
  }
  String message = "B," + buddyId + "," + machineId + "?\n";
  sendRequest(message);
  while(!base.available());
  if(base.read() > 0){
    Serial.print("Good Buddy!");
    return true;
  }
  else{
    return false;
  }
}

boolean authSuper(String id){
  String message = "S," + id + "," + machineId + "?\n";
  sendRequest(message);
  while(!base.available());
  int userLevel = base.read();
  if(userLevel == 0){//user allowed to use
    Serial.println("You are a valid supervisor");
    return true;
  }
  return false;
}  


void sendRequest(String message){
  base.print(message);
  //fill in other code needed to send to the base station
}

//void updateMachineIds(){
//  base.print("R?")
//  while(!base.available());
//  for(int i = STARTMACHINEIDS; base.available() > 0; i++){
//    EEPROM.write(i,(int)base.read());
//  }
//}
  

//*********************************************************************************
//***Timer Functions***************************************************************
//*********************************************************************************
void updateTimers(){
  swipeTimer =+ millis() - swipeTimer;
  relockTimer =+ millis() - relockTimer;
}
void resetSwipeTimer(){
  swipeTimer = millis();
}
void resetRelockTimer(){
  relockTimer = millis();
}

//*********************************************************************************
//***Machine Control Functions*****************************************************
//*********************************************************************************
void lockMachine(){
}
void unlockMachine(){
}
boolean isMachineBusy(){
  return false;//no
}
 
 
