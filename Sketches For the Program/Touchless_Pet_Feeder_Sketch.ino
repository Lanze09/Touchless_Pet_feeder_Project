/*  Touchless Pet Feeder  */

// Headers to allow use of Wire and RTClib Libraries
#include <Wire.h> // used for I2C communication with the RTC module
#include <RTClib.h> // provides functions for accessing and manipulating the RTC module
#include <Ds1302.h> // for DS1302 RTC Chip
// #include <Serial.h> //not working for some reason
#include <LiquidCrystal.h> // allows usage of LiquidCrystal library, which is used to interface with the LCD display

//*******REMOVE_ME RTC_DS1302 rtc; // creates an instance of the RTC_DS1302 class, which is used to access the RTC module

// Essential Variables
int DispCount = 0; // Food dispensing frequency counter
int MaxDisp = 0; // Maximum dispense per feeding interval
int CTime = 0; // Current time / Last identified "Time"
int ATime = 0; // Time checked for adult / senior
int PTime = 0; // Time checked for puppy
int Timer = 0; // For dispense interval (The pause before next dispense)
int Age = 0; // Switch for dog age (0=Puppy ; 1=Adult/Senior)

// Developer Variables
const int DispensePin = 2;
const int AgePin = 4;
const int DogPin = 7;
const int HumanPin = 8;
const int PowerPin = 13;
const int LowbatINPPin = 11; //set up the low battery threshold
const int LowBatOUTPin = 12;
int HumanSensor = 0;

Serial.begin(9600);


void setup() {
  // This shit runs once fr fr

pinMode (DispensePin, OUTPUT);
pinMode (AgePin, INPUT);
pinMode (DogPin, INPUT);
pinMode (HumanPin, INPUT);
pinMode (PowerPin, OUTPUT);
pinMode (LowbatINPPin, INPUT);
pinMode (LowBatOUTPin, OUTPUT);

}

void loop() {
  // This shit runs over and over and over again no cap

digitalWrite(PowerPin, HIGH);


HumanSensor = digitalRead(HumanPin);
if (HumanPin == HIGH){
;
delay(5000);

}

}
