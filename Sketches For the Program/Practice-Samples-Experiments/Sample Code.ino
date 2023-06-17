#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define GSMBoard Serial2
#define GSMBaud 115200

#define GPSBoard Serial3
#define GPSBaud 9600

TinyGPSPlus gps;

const int butPin = 4;
const int vibPin = 5;
int shocksensorstate, buttonstate;

String location, phone_no, msg;

void setup() {
  Serial.begin(115200); 

  // ----- VIBRATION SENSOR SETUP ----- //
  pinMode(vibPin, INPUT); 

  // ----- BUTTON SETUP ----- //
  pinMode(butPin, INPUT_PULLUP); 

  // ----- GPS SETUP ----- //
  GPSBoard.begin(GPSBaud);

  // ----- GSM SETUP ----- //
  GSMBoard.begin(GSMBaud);
  delay(1000);
  GSMBoard.println("AT+CMGF=1"); // AT Command configuring TEXT mode
  delay(500);
  GSMBoard.println("AT+CNMI=1,2,0,0,0"); // AT Command to receive a live SMS
  delay(500);
  sendSMS("System has been started.");
  delay(300);
}

void loop() {
  // ----- WAITING TO RECEIVE MESSAGE ----- //
  while (GSMBoard.available()){
    parseData(GSMBoard.readString());//Calls the parseData function to parse SMS
  }

  buttonstate = digitalRead(butPin);
  if(buttonstate == 0){ //If button pressed
    getLocation();   
  }
  
  shocksensorstate = digitalRead(vibPin); //Activate shock sensor
  if(shocksensorstate == 0){ //If shock sensor is on
    getLocation();   
  }
}

void getLocation(){
  while (GPSBoard.available() > 0){
    gps.encode(GPSBoard.read());
    if (gps.location.isUpdated()){
      String location = "www.google.com/maps?q=" + String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);  
      sendSMS("This is my location: " + location);
      Serial.println("This is my location: " + location);
      delay(1000);
    }
  } 
}

void sendSMS(String msg){ // ----- SEND SMS FUNCTION ----- //
  GSMBoard.println("AT+CMGS=\"+639984180759\"\r"); // Replace with mobile number
  delay(300);
  GSMBoard.print(msg); // SMS Message Content
  delay(300);
  GSMBoard.println((char)26); // ASCII code of CTRL+Z
  delay(300);
}

void parseData(String buff) {
  Serial.println(buff);
  unsigned int index;

  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();

  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    //Parse necessary message from GSM Serial buffer string
    if (cmd == "+CMT") {
      //get newly arrived memory location and store it in temp
      index = buff.lastIndexOf(0x0D);//Looking for position of CR(Carriage Return)
      msg = buff.substring(index + 2, buff.length());//Writes message to variable "msg"
      msg.toLowerCase(); //Whole message gets changed to lower case
      //Serial.println(msg);
      
      //get phone number
      index = buff.indexOf(0x22);//Looking for position of first double quotes-> "
      phone_no = buff.substring(index + 1, index + 14); //Writes phone number to variable "PHONE"
      //Serial.println(phone_no);
    }

    // If message and number are match to the string received
    if(msg == "gps"){ // Replace with mobile number
      getLocation();
    }
  }
}
