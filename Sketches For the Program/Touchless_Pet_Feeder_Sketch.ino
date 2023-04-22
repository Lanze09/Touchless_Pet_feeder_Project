/*  Touchless Pet Feeder  */

#include <stdlib.h>
#include <stdio.h>
// Headers to allow use of Wire and RTClib Libraries
#include <Wire.h> // used for I2C communication with the RTC module
#include <RTClib.h> // provides functions for accessing and manipulating the RTC module
#include <Ds1302.h> // for DS1302 RTC Chip
//REMOVE THIS~~~~~~~~~  #include <Serial.h> //not working for some reason
#include <LiquidCrystal.h> // allows usage of LiquidCrystal library, which is used to interface with the LCD display
#include <Servo.h>

//*******REMOVE_ME-- RTC_DS1302 rtc; // creates an instance of the RTC_DS1302 class, which is used to access the RTC module



// Essential Variables
int DispCount = 0; // Food dispensing frequency counter
int MaxDisp = 0; // Maximum dispense per feeding interval
int CTime = 0; //TENTATIVE- Current time / Last identified "Time"
int ATime = 0; // Time checker for adult / senior
int PTime = 0; // Time checker for puppy
int Timer = 0; // For dispense interval (The pause before next dispense)
bool Adult = false; // Switch for dog age (false=Puppy ; true=Adult/Senior)



// Developer Variables
const int DispensePin = 2;
const int AgePin = 4;
const int DogPin = 7;
const int HumanPin = 8;
const int DispSignal = 13;
const int PowerPin = 12; 
const int LowbatINPPin = 11; //set up the low battery threshold
const int LowBatOUTPin = 12;
bool Detects = 0;

//REMOVE THIS~~~~~~~~~  Serial.begin(9600);


void setup() {
  // This shit runs once fr fr

pinMode (DispensePin, OUTPUT);
pinMode (AgePin, INPUT);
pinMode (DogPin, INPUT);
pinMode (HumanPin, INPUT);
pinMode (DispSignal, OUTPUT);
pinMode (PowerPin, OUTPUT);
pinMode (LowbatINPPin, INPUT);
pinMode (LowBatOUTPin, OUTPUT);
}


void loop() {
  // This shit runs over and over and over again no cap

//*******REMOVE_ME-- digitalWrite(PowerPin, HIGH);

digitalWrite(DispSignal, LOW);


if (digitalRead(HumanPin) == 1)
{
  digitalWrite(DispSignal, HIGH);
  // (Add Command) Set command to open dispenser via servo motor
  delay(1000);
  digitalWrite(DispSignal, LOW);
  DispCount++;
  // (Add Command) Set command to close dispenser via servo motor
}


if (digitalRead(DogPin) == 1)
{
 if (Timer <= 0)
  {
   if (Adult == true) //DESC -- If Adult
   {
     ATime = 0; //TENTATIVE- Set this command to change ATime to actual current time from RTC Module
     MaxDisp = 10; //TENTATIVE- Set this command to change the average Maximum dispense to meet the enough amount of food for this meal
     
     if (ATime != 3)
     {
      if (CTime != ATime)
      {
       CTime = ATime;
       DispCount = 0;
      }
      
      if (DispCount < MaxDisp)
          {
            digitalWrite(DispSignal, HIGH);
            // (Add Command) Set command to open dispenser via servo motor
            delay(1000);
            digitalWrite(DispSignal, LOW);
            // (Add Command) Set command to close dispenser via servo motor
            DispCount++;
            // (Add Command) Set new Timer via RTC Module
          }       
          
      }
    }

    else //DESC-- not adult
      {
        PTime = 0; //TENTATIVE-- Set this command to change PTime to actual current time from RTC Module
        MaxDisp = 8; //TENTATIVE-- Set this command to change the average Maximum dispense to meet the enough amount of food for this meal
        if (PTime != 4)
        {
          if (CTime != PTime) //DESC-- Daytime changes. Will reset the DispCount to zero
          {
          CTime = PTime;
          DispCount = 0;
          }
          
          if (DispCount < MaxDisp)
          {
            digitalWrite(DispSignal, HIGH);
            // (Add Command) Set command to open dispenser via servo motor
            delay(1000);
            digitalWrite(DispSignal, LOW);
            // (Add Command) Set command to close dispenser via servo motor
            DispCount++;
            // (Add Command) Set new Timer via RTC Module  
          }
        }
      }      
    }    
  }
}
