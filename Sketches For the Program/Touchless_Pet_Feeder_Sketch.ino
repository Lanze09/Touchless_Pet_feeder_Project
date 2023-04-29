/*  Touchless Pet Feeder  */


// Headers to allow use of Wire and RTClib Libraries
#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <Wire.h> // used for I2C communication with the RTC module
#include <RTClib.h> // provides functions for accessing and manipulating the RTC module
#include <Ds1302.h> // for DS1302 RTC Chip
#include <LiquidCrystal.h> // allows usage of LiquidCrystal library, which is used to interface with the LCD display
#include <Servo.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <HardwareSerial_private.h>
#include <ATtinySerialOut.hpp>



// software serial #1: RX = digital pin 0, TX = digital pin 1
SoftwareSerial portOne(0,1);

// software serial #2: RX = digital pin 7, TX = digital pin 8
SoftwareSerial portTwo(7,8);


// Essential Variables
int DispCount = 0; // Food dispensing frequency counter
// int MaxDisp = 0; // Maximum dispense per feeding interval
int CTime = 0; //TENTATIVE- Current time / Last identified "Time"
int ATime = 0; // Time checker for adult / senior
int PTime = 0; // Time checker for puppy
int Timer = 0; // For dispense interval (The pause before next dispense)
int TimerDuration = 10;
int TCounter = 0;
//unsigned long DateTime = 0;

int PuppyMax = 3; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
int AdultMax = 5; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
bool Adult = true; //TENTATIVE-- Switch for dog age (false=Puppy ; true=Adult/Senior)
String DayTime = "";


// Pin Variables
const int DispensePin = 2;
const int PIN_CLK = 3;
const int AgePin = 4;
const int PIN_DAT = 5;
const int PIN_ENA = 6;
const int DogPin = 7;
const int HumanPin = 8;
const int DispMaxed = 9;
const int AdultSignalPin = 11;
const int PowerPin = 12; 
const int DispSignal = 13;

Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT); // this one actually works. noice
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const static char* WeekDays[] =
{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

void setup() {
  // This shit runs once fr fr

Serial.begin(9600);

pinMode (DispensePin, OUTPUT);
pinMode (AgePin, INPUT);
pinMode (DogPin, INPUT);
pinMode (HumanPin, INPUT);
pinMode (DispSignal, OUTPUT);
pinMode (PowerPin, OUTPUT);
pinMode (AdultSignalPin, OUTPUT);
pinMode (DispMaxed, OUTPUT);

lcd.begin(16, 2);
rtc.init();




// test if clock is halted and set a date-time (see example 2) to start it
    if (rtc.isHalted())
    {
        Serial.println("RTC is halted. Setting time...");

        Ds1302::DateTime dt = {
            .year = 23,
            .month = Ds1302::MONTH_APR,
            .day = 29,
            .hour = 20,
            .minute = 24,
            .second = 59,
            .dow = Ds1302::DOW_SAT
        };

        rtc.setDateTime(&dt);
    }


}


void loop() {
  // This shit runs over and over and over again no cap

Ds1302::DateTime now;
rtc.getDateTime(&now);
static uint8_t last_second = 0;


if (TCounter != now.second)
  {
    TCounter = now.second;
    Timer--;
  }


if (Adult == true)
{
  if (now.hour >= 7 && now.hour < 12)
    {
      ATime = 0;
      DayTime = "Morning, Doggo";
    }
  else if (now.hour >= 12 && now.hour < 19)
    {
      ATime = 1;
      DayTime = "Lunch Time, Doggo";
    }
  else if (now.hour >= 19)
    {
      ATime = 2;
      DayTime = "Evening, Doggo";
    }
  else if (now.hour >= 0 && now.hour < 7)
    {
      ATime = 3;
      DayTime = "Midnight, Doggo";
    }
}
else
{
  if (now.hour >= 7 && now.hour < 11)
    {
      PTime = 0;
      DayTime = "Early Morning, Puppy";
    }
  else if (now.hour >= 11 && now.hour < 15)
    {
      PTime = 1;
      DayTime = "Late Morning, Puppy";
    }
  else if (now.hour >= 15 && now.hour < 19)
    {
      PTime = 2;
      DayTime = "Lunch Time, Puppy";
    }
  else if (now.hour >= 19)
    {
      PTime = 3;
      DayTime = "Evening, Puppy";
    }
  else if (now.hour >= 0 && now.hour < 7)
    {
      PTime = 4;
      DayTime = "Midnight, Puppy";
    }
}



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
  Serial.println("Human Sensed");
  lcd.setCursor(0, 0);
  Serial.print(now.hour);
  Serial.print(':');
  Serial.print(now.minute);
  Serial.print(':');
  Serial.print(now.second);
  Serial.print(" - " + ATime);
  Serial.println(" - " + DayTime);
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

            Timer = TimerDuration;
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
            Timer = TimerDuration;

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
