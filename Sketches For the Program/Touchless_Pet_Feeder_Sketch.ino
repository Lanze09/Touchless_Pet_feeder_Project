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

int PuppyMax = 3; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
int AdultMax = 5; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
bool Adult = false; //TENTATIVE-- Switch for dog age (false=Puppy ; true=Adult/Senior)


// Pin Variables
const int DispensePin = 2;
const int AgePin = 4;
const int DogPin = 7;
const int HumanPin = 8;
const int DispMaxed = 9;
const int AdultSignalPin = 11;
const int PowerPin = 12; 
const int DispSignal = 13;



//REMOVE THIS~~~~~~~~~  Serial.begin(9600);


void setup() {
  // This shit runs once fr fr

pinMode (DispensePin, OUTPUT);
pinMode (AgePin, INPUT);
pinMode (DogPin, INPUT);
pinMode (HumanPin, INPUT);
pinMode (DispSignal, OUTPUT);
pinMode (PowerPin, OUTPUT);
pinMode (AdultSignalPin, OUTPUT);
pinMode (DispMaxed, OUTPUT);
}


void loop() {
  // This shit runs over and over and over again no cap

//*******REMOVE_ME-- digitalWrite(PowerPin, HIGH);

digitalWrite(DispSignal, LOW);


if (Adult == true)
  digitalWrite (AdultSignalPin, HIGH);
else
  digitalWrite (AdultSignalPin, LOW);



if (digitalRead(HumanPin) == 1)
{
  digitalWrite(DispSignal, HIGH);
  // (Add Command) Set command to open dispenser via servo motor
  delay(1000);
  digitalWrite(DispSignal, LOW);
  delay(300);
  DispCount++;
  // (Add Command) Set command to close dispenser via servo motor


  if (Adult == true)
  {
    if (DispCount >= AdultMax)
      digitalWrite (DispMaxed, HIGH);
    else
      digitalWrite (DispMaxed, LOW);
  }
  else
  {
    if (DispCount >= PuppyMax)
      digitalWrite (DispMaxed, HIGH);
    else
      digitalWrite (DispMaxed, LOW);
  } 
}


if (digitalRead(DogPin) == 1)
{
 if (Timer <= 0)
  {
   if (Adult == true) //DESC -- If Adult   
   {     
     ATime = 0; //TENTATIVE- Set this command to change ATime to actual current time from RTC Module
     if (ATime != 3)
     {
      if (CTime != ATime) //DESC-- Daytime changes. Will reset the DispCount to zero
      {
       CTime = ATime;
       DispCount = 0;
       digitalWrite (DispMaxed, LOW);
      }      
      
      if (DispCount < AdultMax)
        {
            digitalWrite(DispSignal, HIGH);
            // (Add Command) Set command to open dispenser via servo motor
            delay(1000);
            digitalWrite(DispSignal, LOW);
            // (Add Command) Set command to close dispenser via servo motor
            DispCount++;
            // (Add Command) Set new Timer via RTC Module  

            if (DispCount >= AdultMax)
              digitalWrite (DispMaxed, HIGH);
            else
              digitalWrite (DispMaxed, LOW);
            
            delay(300);
        }
      }
    }

    else //DESC-- not adult
      {
        PTime = 0; //TENTATIVE-- Set this command to change PTime to actual current time from RTC Module
        if (PTime != 4)
        {
          if (CTime != PTime) //DESC-- Daytime changes. Will reset the DispCount to zero
          {
          CTime = PTime;
          DispCount = 0;
          digitalWrite (DispMaxed, LOW);
          }
          if (DispCount < PuppyMax)
          {
            digitalWrite(DispSignal, HIGH);
            // (Add Command) Set command to open dispenser via servo motor
            delay(1000);
            digitalWrite(DispSignal, LOW);
            // (Add Command) Set command to close dispenser via servo motor
            DispCount++;
            // (Add Command) Set new Timer via RTC Module  

            if (DispCount >= PuppyMax)
              digitalWrite(DispMaxed, HIGH);
            else
              digitalWrite (DispMaxed, LOW);

            delay(300);
          }
        }
      }      
    }    
  }
}
