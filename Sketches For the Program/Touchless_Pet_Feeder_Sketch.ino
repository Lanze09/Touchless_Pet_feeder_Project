#include <Ds1302.h>

/*  Touchless Pet Feeder  */


// Headers to allow use of Wire and RTClib Libraries
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
//#include <Serial.h> //not working for some reason


// Essential Variables
int DispCount = 0; // Food dispensing frequency counter
// int MaxDisp = 0; // Maximum dispense per feeding interval
int CTime = 0; //TENTATIVE- Current time / Last identified "Time"
int ATime = 0; // Time checker for adult / senior
int PTime = 0; // Time checker for puppy
int Timer = 0; // For dispense interval (The pause before next dispense)
//unsigned long DateTime = 0;

int PuppyMax = 3; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
int AdultMax = 5; //TENTATIVE-- Change the value to average number of dispense to complete the enough amount of meal for the daytime
bool Adult = true; //TENTATIVE-- Switch for dog age (false=Puppy ; true=Adult/Senior)


// Pin Variables
const int DispensePin = 2;
const int DS1302_CLK = 3;
const int AgePin = 4;
const int DS1302_DAT = 5;
const int DS1302_RST = 6;
const int DogPin = 7;
const int HumanPin = 8;
const int DispMaxed = 9;
const int AdultSignalPin = 11;
const int PowerPin = 12; 
const int DispSignal = 13;

Ds1302 rtc(DS1302_RST, DS1302_DAT, DS1302_CLK); // this one actually works. noice

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


// test if clock is halted and set a date-time (see example 2) to start it
    if (rtc.isHalted())
    {
        //Serial.println("RTC is halted. Setting time...");
        Ds1302::DateTime dt = {
            .year = 23,
            .month = Ds1302::MONTH_APR,
            .day = 29,
            .hour = 3,
            .minute = 50,
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
//static uint8_t last_second = 0;

if (Adult == true)
{
  if (6 < now.hour < 12)
    digitalWrite(ATime, 0);
  else if (11 < now.hour < 19)
    digitalWrite(ATime, 1);
  else if (18 < now.hour < 0)
    digitalWrite(ATime, 2);
  else if (11 < now.hour < 7)
    digitalWrite(ATime, 3);
}
else
{
  if (6 < now.hour < 11)
    digitalWrite(PTime, 0);
  else if (10 < now.hour < 14)
    digitalWrite(PTime, 1);
  else if (14 < now.hour < 19)
    digitalWrite(PTime, 2);
  else if (18 < now.hour < 0)
    digitalWrite(PTime, 3);
  else if (11 < now.hour < 7)
    digitalWrite(PTime, 4);
}


// get the current time
    /*
    serial.begin(9600);
    Ds1302::DateTime now;
    rtc.getDateTime(&now);

    static uint8_t last_second = 0;
    if (last_second != now.second)
    {
        last_second = now.second;

        Serial.print("20");
        Serial.print(now.year);    // 00-99
        Serial.print('-');
        if (now.month < 10) Serial.print('0');
        Serial.print(now.month);   // 01-12
        Serial.print('-');
        if (now.day < 10) Serial.print('0');
        Serial.print(now.day);     // 01-31
        Serial.print(' ');
        Serial.print(WeekDays[now.dow - 1]); // 1-7
        Serial.print(' ');
        if (now.hour < 10) Serial.print('0');
        Serial.print(now.hour);    // 00-23
        Serial.print(':');
        if (now.minute < 10) Serial.print('0');
        Serial.print(now.minute);  // 00-59
        Serial.print(':');
        if (now.second < 10) Serial.print('0');
        Serial.print(now.second);  // 00-59
        Serial.println();
    }

    delay(100);
*/




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
