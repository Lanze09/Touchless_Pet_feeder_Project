/* ------------------------------- Touchless Pet Feeder ------------------------------- */

   
/*

for LCD, 

VCC and GND connected to the bridge of the longer bread board
SDA - A4
SCL - A5

2 pins on the left part
lower pin - left pin of 10k potentiometer (knob facing you)
upper pin - middle pin of 10k potentiometer (knob facing you)

*/

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
  #include <NewPing.h>
  


  // software serial #1: RX = digital pin 0, TX = digital pin 1
  SoftwareSerial portOne(0,1);
  // software serial #2: RX = digital pin 7, TX = digital pin 8
  SoftwareSerial portTwo(7,8);

  // SoftwareSerial SIM900A(A2,A3);


  // Essential Variables
  int DispCount = 0; // Food dispensing frequency counter
  // int MaxDisp = 0; // Maximum dispense per feeding interval
  int CTime = 10; //TENTATIVE- Current time / Last identified "Time"
  int ATime = 0; // Time checker for adult / senior
  int PTime = 0; // Time checker for puppy
  int Timer = 0; // For dispense interval (The pause before next dispense)
  int TCounter = 0;
  int i;
  String DayTime = "";
  String DOW, DayLight, Hourrr, Minuteee, Seconddd, WillReset, Time;

  //TENTATIVE VALUES
  int TimerDuration = 3; // timer Duration per minute
  int PuppyMax = 5; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  int AdultMax = 10; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  bool Adult = true; // Switch for dog age (false=Puppy ; true=Adult/Senior)

  // Pin Variables
  const int SwitchPin = 2;
  const int PIN_CLK = 3;
  const int DogAvail = 4;
  const int PIN_DAT = 5;
  const int PIN_ENA = 6;
  const int ServoPin = 7;
 /* const int  = 8;
  const int  = 9;
  const int  = 10;
  const int  = 11;*/
  const int TankSensor = 12;
  const int DispSignal = 13;
  const int HumanPin = A0;
  const int DogPin = A1;
  //const int GSMrx = A2;
  //const int GSMtx = A3;

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

  Servo DispMotor;

  void setup() {
    // This shit runs once fr fr
  
  //SIM900A.begin(9600);
  Serial.begin(9600);

  

  //pinMode (DispensePin, OUTPUT);
  //pinMode (AgePin, INPUT);
  pinMode (DogPin, INPUT);
  pinMode (HumanPin, INPUT);
  pinMode (SwitchPin, INPUT);
  pinMode (DispSignal, OUTPUT);
  pinMode (ServoPin, OUTPUT);
  //pinMode (PowerPin, OUTPUT);
  pinMode (TankSensor, INPUT);
  //pinMode (AdultSignalPin, OUTPUT);
  pinMode (DogAvail, OUTPUT);


  rtc.init(); //Initialize RTC
  lcd.init(); // initialize the lcd 
  // left to right - yellow green orange brown
  // Print a message to the LCD.
  lcd.setContrast(255); // maximum contrast level
  lcd.backlight();

  DispMotor.attach(ServoPin);
  DispMotor.write(0);

         
          // ------------------------------ Use this to reprogram RTC Module ------------------------------
    /*          Ds1302::DateTime dt = {
              .year = 23,
              .month = Ds1302::MONTH_JUN,
              .day = 5,
              .hour = 15,
              .minute = 28,
              .second = 0,
              .dow = Ds1302::DOW_MON};
              rtc.setDateTime(&dt);
      */ 

  // test if clock is halted and set a date-time (see example 2) to start it
      if (rtc.isHalted())
      {
          Serial.println("RTC is halted. Setting time...");

          Ds1302::DateTime dt = {
              .year = 23,
              .month = Ds1302::MONTH_JUN,
              .day = 9,
              .hour = 16,
              .minute = 20,
              .second = 0,
              .dow = Ds1302::DOW_FRI
          };
          rtc.setDateTime(&dt);
      }


  //starting screen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("---TOUCHLESS----");
  lcd.setCursor(0,1);
  lcd.print("---PET FEEDER---");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Thesis Prototype");
  lcd.setCursor(0,1);
  lcd.print("for CMPE30252");
  delay(3000);

  digitalWrite(DispSignal, LOW);
  delay(200);
  digitalWrite(DispSignal, HIGH);
  delay(100);
  digitalWrite(DispSignal, LOW);
  delay(200);
  digitalWrite(DispSignal, HIGH);
  delay(100);
  digitalWrite(DispSignal, LOW);
  delay(200);
  digitalWrite(DispSignal, HIGH);
  delay(100);


  //CheckTank();

  }


  


  void CheckTank()
  {
    // Tank Sensor stops detecing object
    if (digitalRead(TankSensor) == 1)
    {
     Serial.println("Tank is running out of food, Please refill");
     delay(10);
     Serial.println("TankSensor: " + String((digitalRead(TankSensor))));    
    }
  }

  void Dispense()
  {
    digitalWrite(DispSignal, HIGH);
    Serial.println("Dispenses Food");
    DispMotor.write(180);
    delay(1000);
    DispMotor.write(0);
    digitalWrite(DispSignal, LOW);
  }


  void loop() {
    // This shit runs over and over and over again no cap

  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  digitalWrite(DispSignal, LOW);
  
  DispMotor.write(0);

/*

  if (Serial.available()>0)
   switch(Serial.read())
  {
    case 's':
      SendMessage();
      break;
  }

 if (SIM900A.available()>0)
   Serial.write(SIM900A.read());
*/

  //Tank Sensor Value checker (TESTER)
  //Serial.println("Tank: " + String((digitalRead(TankSensor))));
  //delay("500");  


//My Fckin Everything Checker
if (Serial.available() > 0) {
  switch (Serial.read()) {
    case 'T':
    case 't':
      CheckTank();
      break;
    case 's':
    case 'S':
      Dispense();
      break;
  }
}


  //Age Button clicked or swtiched
  if (digitalRead(SwitchPin) == 1)
  {
    Serial.println("Age Switched");    
    if (Adult == true)
    {
      Adult = false;
     // digitalWrite(AdultSignalPin, LOW);
      DispCount = 0;
      Timer = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Changed to PUPPY");
      lcd.setCursor(0, 1);
      lcd.print("Resetting now!");
    }
    else
    {
      Adult = true;
      //digitalWrite(AdultSignalPin, HIGH);
      DispCount = 0;
      Timer = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Changed to Adult");
      lcd.setCursor(0, 1);
      lcd.print("Resetting now!");
    }
    delay(3000);
    TCounter++;
  }

  //Time Checker
  if (Adult == true) //ADT Time Checker
  {
    //digitalWrite (AdultSignalPin, HIGH);

    if (now.hour >= 7 && now.hour < 12)
      {
        ATime = 0;
        DayTime = "Morning, Doggo";
        WillReset = "RST@12pm";
      }
    else if (now.hour >= 12 && now.hour < 19)
      {
        ATime = 1;
        DayTime = "Lunch Time, Doggo";
        WillReset = "RST@7pm";
      }
    else if (now.hour >= 19)
      {
        ATime = 2;
        DayTime = "Evening, Doggo";
        WillReset = "RST@12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        ATime = 3;
        DayTime = "Midnight, Doggo";
        WillReset = "RST@7am";
      }
  }
  else //Puppy Time Checker
  {
    //digitalWrite (AdultSignalPin, LOW);

    if (now.hour >= 7 && now.hour < 11)
      {
        PTime = 0;
        DayTime = "Early Morning, Puppy";
        WillReset = "RST@11am";
      }
    else if (now.hour >= 11 && now.hour < 15)
      {
        PTime = 1;
        DayTime = "Lunch Time, Puppy";
        WillReset = "RST@3pm";
      }
    else if (now.hour >= 15 && now.hour < 19)
      {
        PTime = 2;
        DayTime = "Afternoon, Puppy";
        WillReset = "RST@7pm";
      }
    else if (now.hour >= 19)
      {
        PTime = 3;
        DayTime = "Evening, Puppy";
        WillReset = "RST@12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        PTime = 4;
        DayTime = "Midnight, Puppy";
        WillReset = "RST@7am";
      } 
  } 

/*    //DayTime Change Tester
    if (Tester <= 0)
      {
        ATime++;
        Tester = 3;
      }
*/

    //Day of the week naming
    if (now.dow == 7)
      DOW = "SUN";
    else if (now.dow == 1)
      DOW = "MON";
    else if (now.dow == 2)
      DOW = "TUE";
    else if (now.dow == 3)
      DOW = "WED";
    else if (now.dow == 4)
      DOW = "THU";
    else if (now.dow == 5)
      DOW = "FRI";
    else if (now.dow == 6)
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
  if (Hourrr.toInt() < 10)
    Hourrr = ("0" + Hourrr);
  else
    Hourrr = String(Hourrr);
  if (now.minute < 10)
    Minuteee = ("0" + String(now.minute));
  else
    Minuteee = String(now.minute);
  if (now.second < 10)
    Seconddd = ("0" + String(now.second));
  else
    Seconddd = String(now.second);


    // To ping if maximum dispense has been reached
    if (Adult == true)
    {
      if (DispCount >= AdultMax)
        {
          digitalWrite (DogAvail, LOW);
        }
      else
        digitalWrite (DogAvail, HIGH);
    }
    else
    {
      if (DispCount >= PuppyMax)
        {
          digitalWrite (DogAvail, LOW);
        }
      else
        digitalWrite (DogAvail, HIGH);
    }

      // Reset if Daytime changes
      if (Adult == true)
      {
        if (CTime != ATime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = ATime;
          DispCount = 0;
          digitalWrite (DogAvail, HIGH);
          CheckTank();
        }
      }
      else
      {
        if (CTime != PTime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = PTime;
          DispCount = 0;
          digitalWrite (DogAvail, LOW);
          CheckTank();

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("---TOUCHLESS----");
            lcd.setCursor(0,1);
            lcd.print("---PET FEEDER---");
            delay(3000);

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Thesis Prototype");
            lcd.setCursor(0,1);
            lcd.print("for CMPE30252");
            delay(3000);

            digitalWrite(DispSignal, HIGH);
            delay(200);
            digitalWrite(DispSignal, LOW);
            delay(100);
            digitalWrite(DispSignal, HIGH);
            delay(200);
            digitalWrite(DispSignal, LOW);
            delay(100);
            digitalWrite(DispSignal, HIGH);
            delay(200);
            digitalWrite(DispSignal, LOW);
            delay(100);

        }
      }


    // setting of the DogAvail LED
    if (Timer <= 0)
    {
      if (Adult == true)
      {
        if (DispCount < AdultMax)
        {
          if (ATime != 3)
            digitalWrite(DogAvail, HIGH);
          else
          {
            DispCount = 30;
            digitalWrite(DogAvail, LOW);
          }
        }
      }
      else
      {
        if (DispCount < PuppyMax)
        {
          if (PTime != 4)
            digitalWrite(DogAvail, HIGH);
          else
            {
              DispCount = 30;
              digitalWrite(DogAvail, LOW);
            }
        }
      }
    }
    else
    {
      digitalWrite(DogAvail, LOW);
    }


  Time = (Hourrr + ":" + Minuteee + DayLight);

  ////  ------------------- Per minute actions ------------------- fminute fperminute
  if (TCounter != now.minute)
    {
      TCounter = now.minute;
      Timer--;
      if (Timer <= 0)
      {
        digitalWrite(DogAvail, LOW);
        if (Timer < 0)
          Timer = 0;
      }
      

      //For Serial Display per minute
      for(i = 0; i < 50; i++) {
        Serial.println();}
      Serial.println("--------PER MINUTE---------");
      Serial.println(DayTime);
      Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
      Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
      Serial.print(" | Interval: ");
      Serial.print(Timer);
      Serial.println(" Min/s");
      lcd.clear();
      lcd.setCursor(0, 0);
      
      if (Adult == true)
      {
        lcd.print("A INT:" + String(Timer) + "m REM:");

        if ((AdultMax - DispCount) > 0)
        {
          lcd.print(String(AdultMax - DispCount));          
          Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
        }
        else
          {
            lcd.print("0 ");
            Serial.print("Remaining for Dog: 0");
            digitalWrite(DogAvail, LOW);
          }
      }
      else
      {    
          lcd.print("P INT:" + String(Timer) + "m REM:");

        if ((PuppyMax - DispCount) > 0)
        {
          Serial.print("Remaining for Puppy: " + String(PuppyMax - DispCount));
          lcd.print(String(PuppyMax - DispCount));
        }
        else
          {
            Serial.print("Remaining for Puppy: 0");
            lcd.print("0 ");
            digitalWrite(DogAvail, LOW);
          }     
      }
      Serial.println(" | " + WillReset);
      lcd.setCursor(0, 1);
      lcd.print("                  ");
      lcd.setCursor(0, 1);
      lcd.print(Time + " " + WillReset);
      Serial.println("*** DEVELOPER DATA BELOW ***");
      Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
      Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax) + " / now.hour: " + String(now.hour));
      Serial.println("Hourrr: " + String(Hourrr) + " / DOW: " + String(now.dow));
      Serial.println("-----------------"); 
  }





  //  ------------------- Human Detected ------------------- fhuman
  if (digitalRead(HumanPin) != 1)
  {
    digitalWrite(DispSignal, HIGH);
    // (Add Command) Set command to open dispenser via servo motor
    DispCount++;
    delay(1000);
    digitalWrite(DispSignal, LOW);

      // Reset if Daytime changes
      if (Adult == 1)
      {
        if (CTime != ATime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = ATime;
          DispCount = 0;
          digitalWrite (DogAvail, HIGH);
        }
      }
      else
      {
        if (CTime != PTime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = PTime;
          DispCount = 0;
          digitalWrite (DogAvail, HIGH);
        }
      }


      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Human Detected!");
      lcd.setCursor(0,1);
      lcd.print("Dispenses Food!");                    
      delay(1000);


      //fServo Rotation
      /*DispMotor.write(180);
      delay(1000);
      DispMotor.write(0);*/

      //For Serial Display 
      for(int i = 0; i < 50; i++) {
        Serial.println();}
      Serial.println("--------HUMAN DETECTED---------");
      Serial.println(DayTime);
      Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
      Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
      Serial.print(" | Interval: ");
      Serial.print(Timer);
      Serial.println(" Min/s");
      lcd.clear();
      lcd.setCursor(0, 0);
      if (Adult == true)
      {
        lcd.print("A INT:" + String(Timer) + "m REM: ");
        if ((AdultMax - DispCount) > 0)
          {
            Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
            lcd.print(String(AdultMax - DispCount));
    //        digitalWrite(DogAvail, HIGH);
          }
        else
          {
            Serial.print("Remaining for Dog: 0");
            lcd.print("0");
            digitalWrite(DogAvail, LOW);
          }
      }
      else
      {
        lcd.print("P INT:" + String(Timer) + "m REM:");

        if ((PuppyMax - DispCount) > 0)
          {
            Serial.print("Remaining for Puppy: " + String(PuppyMax - DispCount));
            lcd.print(String(PuppyMax - DispCount));            
   //         digitalWrite(DogAvail, HIGH);
          }
          
        else
          {
            Serial.print("Remaining for Puppy: 0");
            lcd.print("0");            
            digitalWrite(DogAvail, LOW);
          }     
      }
      Serial.println(" | " + WillReset);
      lcd.setCursor(0, 1);
      lcd.print(Time + " " + WillReset);
      Serial.println("*** DEVELOPER DATA BELOW ***");
      Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
      Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax) + " / HumanPin: " + String(HumanPin));
      Serial.println("Dog pin: " + String(DogPin));
      Serial.println("-----------------");
    }
    // (Add Command) Set command to close dispenser via servo motor
    delay(300);
  





  //   ------------------- Dog or Puppy Detected -------------------
 if (digitalRead(DogPin) != 1)
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
        digitalWrite (DogAvail, HIGH);
        }      
        
        if (DispCount < AdultMax)
          {
              digitalWrite(DispSignal, HIGH);
              // (Add Command) Set command to open dispenser via servo motor
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Dog Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");                            
              delay(2000);
              digitalWrite(DispSignal, LOW);
              // (Add Command) Set command to close dispenser via servo motor
              DispCount++;
              // (Add Command) Set new Timer via RTC Module  
              Timer = TimerDuration;

              if (DispCount >= AdultMax || Timer > 0)
                digitalWrite(DogAvail, LOW);
              else
                digitalWrite (DogAvail, HIGH);

            //fServo Rotation
            /*DispMotor.write(180);
            delay(1000);
            DispMotor.write(0);*/

              //For Serial Display
              for(int i = 0; i < 50; i++) {
                Serial.println();}
              Serial.println("--------Doggo Detected---------");
              Serial.println(DayTime);
              Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
              Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
              Serial.print(" | Interval: ");
              Serial.print(Timer);
              Serial.println(" Min/s");
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("A INT:" + String(Timer) + "m REM:");
              if ((AdultMax - DispCount) > 0)
              {
                Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
                lcd.print(String(AdultMax - DispCount));
              }
              else
              {
                Serial.print("Remaining for Dog: 0");
                lcd.print("0");
              }
              Serial.println(" | " + WillReset);
              lcd.setCursor(0, 1);
              lcd.print(Time + " " + WillReset);
              Serial.println("*** DEVELOPER DATA BELOW ***");
              Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
              Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax) + " / HumanPin: " + String(HumanPin));
              Serial.println("Dog pin: " + String(DogPin));
              Serial.println("-----------------");
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
            digitalWrite (DogAvail, HIGH);
            }
            if (DispCount < PuppyMax)
            {
              digitalWrite(DispSignal, HIGH);
              // (Add Command) Set command to open dispenser via servo motor
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Puppy Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");              
              delay(2000);
              digitalWrite(DispSignal, LOW);
              // (Add Command) Set command to close dispenser via servo motor
              DispCount++;  
              Timer = TimerDuration;            

              if (DispCount >= PuppyMax || Timer > 0)
                digitalWrite(DogAvail, LOW);
              else
                digitalWrite (DogAvail, HIGH);


            //fServo Rotation
            /*DispMotor.write(180);
            delay(1000);
            DispMotor.write(0);*/

              //For Serial Display (Puppy)
              for(int i = 0; i < 50; i++) {
                Serial.println();}
              Serial.println("---------Puppy DETECTED--------");
              Serial.println(DayTime);
              Serial.print(String(now.month) + "/" + String(now.day) + "/" + String(now.year) + " " + DOW + " ");
              Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
              Serial.print(" | Interval: ");
              Serial.print(Timer);
              Serial.println(" Min/s");
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("P INT:" + String(Timer) + "m REM:");
              if ((PuppyMax - DispCount) > 0)
              {
                Serial.print("Remaining for Puppy: " + String(PuppyMax - DispCount));
                lcd.print(String(PuppyMax - DispCount));
              }
              else
              {
                Serial.print("Remaining for Puppy: 0");
                lcd.print("0");
              }
              Serial.println(" | " + WillReset);
              lcd.setCursor(0, 1);
              lcd.print(Time + " " + WillReset);
              Serial.println("*** DEVELOPER DATA BELOW ***");
              Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / Counter: " + String(DispCount));
              Serial.println("Adult: " + String(Adult) + " / Max for Pup: " + String(PuppyMax) + " / Max for Adt: " + String(AdultMax));
              Serial.println("-----------------");

              delay(300);                          
            }
          }
        }
      }    
    }
  }

/*

void SendMessage()
{
  Serial.println ("Sending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+639652866745\"\r"); //Mobile phone number to send message
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println("This is the Arduino speaking. Good Evening.");// Messsage content
  delay(100);
  Serial.println ("Finish");
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent");
}

*/
