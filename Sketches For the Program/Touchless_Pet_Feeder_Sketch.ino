/* ------------------------------- Touchless Pet Feeder ------------------------------- */

  
  #include <Arduino.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <Wire.h>
  #include <RTClib.h>
  #include <Ds1302.h>
  #include <LiquidCrystal_I2C.h>
  #include <Servo.h>
  #include <SoftwareSerial.h>
  #include <HardwareSerial.h>
  #include <HardwareSerial_private.h>
  #include <ATtinySerialOut.hpp>
  #include <string.h>


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
  int TCounter = 0;
  String DOW, DayLight, Hourrr, Minuteee, Seconddd, WillReset;
  //unsigned long DateTime = 0;

  //TENTATIVE VALUES
  int TimerDuration = 1;
  int PuppyMax = 2; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  int AdultMax = 3; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  bool Adult = false; // Switch for dog age (false=Puppy ; true=Adult/Senior)
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

  LiquidCrystal_I2C lcd(0x27,20,4);

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


  rtc.init(); //Initialize RTC


  lcd.init(); // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("I'm Lanze!");
  lcd.setCursor(0,2);
  lcd.print("Arduino UNO Project");
  lcd.setCursor(2,3);
  lcd.print("Power By 5V!");


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
  digitalWrite(DispSignal, LOW);

  //Time Checker
  if (Adult == true)
  {
    digitalWrite (AdultSignalPin, HIGH);

    if (now.hour >= 7 && now.hour < 12)
      {
        ATime = 0;
        DayTime = "Morning, Doggo";
        WillReset = "Will reset at 12pm";
      }
    else if (now.hour >= 12 && now.hour < 19)
      {
        ATime = 1;
        DayTime = "Lunch Time, Doggo";
        WillReset = "Will reset at 7pm";
      }
    else if (now.hour >= 19)
      {
        ATime = 2;
        DayTime = "Evening, Doggo";
        WillReset = "Will reset at 12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        ATime = 3;
        DayTime = "Midnight, Doggo";
        WillReset = "Will reset at 7am";
      }
  }
  else
  {
    digitalWrite (AdultSignalPin, LOW);

    if (now.hour >= 7 && now.hour < 11)
      {
        PTime = 0;
        DayTime = "Early Morning, Puppy";
        WillReset = "Will reset at 11am";
      }
    else if (now.hour >= 11 && now.hour < 15)
      {
        PTime = 1;
        DayTime = "Late Morning, Puppy";
        WillReset = "Will reset at 3pm";
      }
    else if (now.hour >= 15 && now.hour < 19)
      {
        PTime = 2;
        DayTime = "Lunch Time, Puppy";
        WillReset = "Will reset at 7pm";
      }
    else if (now.hour >= 19)
      {
        PTime = 3;
        DayTime = "Evening, Puppy";
        WillReset = "Will reset at 12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        PTime = 4;
        DayTime = "Midnight, Puppy";
        WillReset = "Will reset at 7am";
      }
  }

    if (now.dow == 1)
      DOW = "SUN";
    else if (now.dow == 2)
      DOW = "MON";
    else if (now.dow == 3)
      DOW = "TUE";
    else if (now.dow == 4)
      DOW = "WED";
    else if (now.dow == 5)
      DOW = "THU";
    else if (now.dow == 6)
      DOW = "FRI";
    else if (now.dow == 7)
      DOW = "SAT";

  // Turn time into 12-Hour format
  if (now.hour <= 12)
    {
      Hourrr = String(now.hour);
      DayLight = "AM";
      if (now.hour == 0)
        Hourrr = "12";
      if (now.hour == 12) 
        DayLight = "PM";
    }
  else
    {
     Hourrr = String(now.hour - 12);
     DayLight = "PM";
    }

  //Make the time 2 digits if below 10
  if (now.hour < 10)
    Hourrr = ("0" + String(now.hour));
  else
    Hourrr = String(now.hour);
  if (now.minute < 10)
    Minuteee = ("0" + String(now.minute));
  else
    Minuteee = String(now.minute);
  if (now.second < 10)
    Seconddd = ("0" + String(now.second));
  else
    Seconddd = String(now.second);


  if (ATime == 3 || PTime == 4)
    DispCount = 10;


  if (TCounter != now.minute) // change all now.second to minute
    {
      TCounter = now.minute;
      Timer--;
      if (Timer < 0)
        Timer = 0;

    // To ping if maximum dispense has been reached
    if (Adult == true)
    {
      if (DispCount >= AdultMax)
        {
          digitalWrite (DispMaxed, HIGH);
          //Serial.println("NOTE: Maximum reached. Wait til next DayTime!");
        }
      else
        digitalWrite (DispMaxed, LOW);
    }
    else
    {
      if (DispCount >= PuppyMax)
        {
          digitalWrite (DispMaxed, HIGH);
          //Serial.println("NOTE: Maximum reached. Wait til next DayTime!");
        }
      else
        digitalWrite (DispMaxed, LOW);
    }

      //For Serial Display
      for(int i = 0; i < 50; i++) {
        Serial.println();}
      Serial.println("-----------------");
      Serial.println(DayTime);
      Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
      Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
      Serial.print(" | Interval: ");
      Serial.print(Timer);
      Serial.println(" Min/s");
      if ((AdultMax - DispCount) >= 0)
        Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
      else
        Serial.print("Remaining for Dog: 0");
      Serial.println(" | " + WillReset);
      Serial.println("*** DEVELOPER DATA BELOW ***");
      Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
      Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax));
      Serial.println("-----------------");
    }
  
  // Human Detected
  if (digitalRead(HumanPin) == 1)
  {
    digitalWrite(DispSignal, HIGH);
    // (Add Command) Set command to open dispenser via servo motor
    DispCount++;
    delay(1000);
    digitalWrite(DispSignal, LOW);

    //For Serial Display
    for(int i = 0; i < 50; i++) {
      Serial.println();}
    Serial.println("-----------------");
    Serial.print("-Human Detected-");
    Serial.println(" " + DayTime);
    Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
    Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
    Serial.print(" | Interval: ");
    Serial.print(Timer);
    Serial.println(" Min/s");
    if ((AdultMax - DispCount) >= 0)
      Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
    else
      Serial.print("Remaining for Dog: 0");
    Serial.println(" | " + WillReset);
    Serial.println("*** DEVELOPER DATA BELOW ***");
    Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
    Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax));
    Serial.println("-----------------");
    
    // (Add Command) Set command to close dispenser via servo motor
    delay(300);
  }


  // Dog Detected
  if (digitalRead(DogPin) == 1)
  {
  if (Timer <= 0)
    {
    if (Adult == true) //DESC -- If Adult   
    {     
      if (ATime != 3) // Making sure it's not midnight
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

              //For Serial Display
              for(int i = 0; i < 50; i++) {
                Serial.println();}
              Serial.println("-----------------");
              Serial.print("-Doggo Detected-");
              Serial.println(" " + DayTime);
              Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
              Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
              Serial.print(" | Interval: ");
              Serial.print(Timer);
              Serial.println(" Min/s");
              if ((AdultMax - DispCount) >= 0)
                Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
              else
                Serial.print("Remaining for Dog: 0");
              Serial.println(" | " + WillReset);
              Serial.println("*** DEVELOPER DATA BELOW ***");
              Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
              Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax));
              Serial.println("-----------------");

              Timer = TimerDuration;
              delay(300);
          }
        }
      }

      else //DESC-- not adult
        {
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

              if (DispCount >= PuppyMax)
                digitalWrite(DispMaxed, HIGH);
              else
                digitalWrite (DispMaxed, LOW);

              //For Serial Display
              for(int i = 0; i < 50; i++) {
                Serial.println();}
              Serial.println("-----------------");
              Serial.print("-Doggo Detected-");
              Serial.println(" " + DayTime);
              Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
              Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
              Serial.print(" | Interval: ");
              Serial.print(Timer);
              Serial.println(" Min/s");
              if ((PuppyMax - DispCount) >= 0)
                Serial.print("Remaining for Dog: " + String(PuppyMax - DispCount));
              else
                Serial.print("Remaining for Dog: 0");
              Serial.println(" | " + WillReset);
              Serial.println("*** DEVELOPER DATA BELOW ***");
              Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
              Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax));
              Serial.println("-----------------");

              Timer = TimerDuration;
              delay(300);
            }
          }
        }
      }    
    }
  }
