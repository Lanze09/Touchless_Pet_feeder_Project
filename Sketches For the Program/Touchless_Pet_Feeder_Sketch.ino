/*------------------------------- Touchless Pet Feeder -------------------------------


-GSM +Everything else -DogSwitch
Desciprtion of the last actions: 
• Changed GSM's RX and TX pin
• Moved CheckTank() Function after void loop part
• SIM900A is only connecting to network when powered by stable power supply
• Still trying to troubleshoot SIM900A's serial communication



------Other Info------
for LCD, 
VCC and GND connected to the bridge of the longer bread board
SDA - A4
SCL - A5
2 pins on the left part
lower pin - left pin of 10k potentiometer (knob facing you)
upper pin - middle pin of 10k potentiometer (knob facing you)
*/

  
  #include <stdlib.h>
  #include <stdio.h>
  #include <Wire.h>
  #include <HardwareSerial.h>
  #include <HardwareSerial_private.h>
  #include <ATtinySerialOut.hpp>
  #include <string.h>
  #include <Arduino.h>
  #include <RTClib.h>
  #include <Ds1302.h>
  #include <LiquidCrystal_I2C.h>
  #include <Servo.h>
  #include <SoftwareSerial.h>
  #include <NewPing.h>
  


  // software serial #1: RX = digital pin 0, TX = digital pin 1
  //SoftwareSerial portOne(0,1);
  // software serial #2: RX = digital pin 7, TX = digital pin 8
  //SoftwareSerial portTwo(7,8);
  SoftwareSerial SIM900A(10,11);


  // Essential Variables
  int DispCount = 0; // Food dispensing frequency counter
  int CTime = 10; //TENTATIVE- Current time / Last identified "Time"
  int ATime = 0; // Time checker for adult / senior
  int PTime = 0; // Time checker for puppy
  int Timer = 0; // For dispense interval (The pause before next dispense)
  int TCounter = 0;
  String DayLight, Hourrr, Minuteee, Seconddd, WillReset, Time;

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
 /*const int  = 8;
  const int  = 9;
  const int  = 10; // used for GSM module
  const int  = 11; // used for GSM module*/
  const int TankSensor = 12;
  const int DispSignal = 13;
  const int HumanPin = A0;
  const int DogPin = A1;

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

  LiquidCrystal_I2C lcd(0x27,16,2);

  Servo DispMotor;

  void setup() {
    // This shit runs once fr fr
  
  SIM900A.begin(115200);
  delay(80);
  Serial.begin(115200);
  
  //pinMode (DispensePin, OUTPUT);
  //pinMode (AgePin, INPUT);
  pinMode (DogPin, INPUT);
  pinMode (HumanPin, INPUT);
  pinMode (SwitchPin, INPUT);
  pinMode (DispSignal, OUTPUT);
  pinMode (ServoPin, OUTPUT);
  pinMode (TankSensor, INPUT);
  pinMode (DogAvail, OUTPUT);


  rtc.init(); //Initialize RTC
  delay(80);
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
              .day = 10,
              .hour = 13,
              .minute = 21,
              .second = 0,
              .dow = Ds1302::DOW_FRI};
              rtc.setDateTime(&dt);*/
      


  //starting screen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("---TOUCHLESS----");
  lcd.setCursor(0,1);
  lcd.print("---PET FEEDER---");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("THESIS PROTOTYPE");
  lcd.setCursor(0,1);
  lcd.print(" FOR CMPE30252");
  delay(2000);

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

  void Dispense()
  {
    digitalWrite(DispSignal, HIGH);
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

//My Fckin Everything Checker
if (Serial.available() > 0) {
  switch (Serial.read()) {
    case 'c':
    case 'C':
      CheckTank();
      break;
    case 'd':
    case 'D':
      Dispense();
      break;
    case 't':
    case 'T':
      TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
      break;
  }

  if (SIM900A.available()>0)
   Serial.write(SIM900A.read());

}




  // test if clock is halted a+nd set a date-time (see example 2) to start it
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


  //Age Button clicked or swtiched
  if (digitalRead(SwitchPin) == 1)
  {
    //Serial.println("Age Switched");    
    if (Adult == true)
    {
      Adult = false;
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
        WillReset = "RST@12pm";
      }
    else if (now.hour >= 12 && now.hour < 19)
      {
        ATime = 1;
        WillReset = "RST@7pm";
      }
    else if (now.hour >= 19)
      {
        ATime = 2;
        WillReset = "RST@12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        ATime = 3;
        WillReset = "RST@7am";
      }
  }
  else //Puppy Time Checker
  {
    //digitalWrite (AdultSignalPin, LOW);

    if (now.hour >= 7 && now.hour < 11)
      {
        PTime = 0;
        WillReset = "RST@11am";
      }
    else if (now.hour >= 11 && now.hour < 15)
      {
        PTime = 1;
        WillReset = "RST@3pm";
      }
    else if (now.hour >= 15 && now.hour < 19)
      {
        PTime = 2;
        WillReset = "RST@7pm";
      }
    else if (now.hour >= 19)
      {
        PTime = 3;
        WillReset = "RST@12am";
      }
    else if (now.hour >= 0 && now.hour < 7)
      {
        PTime = 4;
        WillReset = "RST@7am";
      } 
  } 


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
      
      for(int i = 0; i < 10; i++) {
        Serial.println();}
      Serial.println("--------PER MINUTE---------");
      TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
  }


  //  ------------------- Human Detected ------------------- fhuman
  if (digitalRead(HumanPin) != 1)
  {
    // (Add Command) Set command to open dispenser via servo motor
    DispCount++;
      // Reset if Daytime changes
      if (Adult == 1)
      {
        if (CTime != ATime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = ATime;
          DispCount = 0;
          CheckTank();
          digitalWrite (DogAvail, HIGH);
        }
      }
      else
      {
        if (CTime != PTime) //DESC-- Daytime changes. Will reset the DispCount to zero
        {
          CTime = PTime;
          DispCount = 0;
          CheckTank();
          digitalWrite (DogAvail, HIGH);
        }
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Human Detected!");
      lcd.setCursor(0,1);
      lcd.print("Dispenses Food!");                    
      //delay(800);
      Dispense();      
      for(int i = 0; i < 10; i++) 
      {
        Serial.println();}
        Serial.println("--------HUMAN DETECTED---------");
        TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
      }
    delay(300);  

  //   ------------------- Dog or Puppy Detected ------------------- fDog
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
        CheckTank();
        digitalWrite (DogAvail, HIGH);
        }      
        
        if (DispCount < AdultMax)
          {
              // (Add Command) Set command to open dispenser via servo motor
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Dog Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");                            
              digitalWrite(DispSignal, LOW);
              delay(800);
              Dispense();
              delay(1000);
              DispCount++;
              Timer = TimerDuration;

              if (DispCount >= AdultMax || Timer > 0)
                digitalWrite(DogAvail, LOW);
              else
                digitalWrite (DogAvail, HIGH);

              for(int i = 0; i < 10; i++) {
                Serial.println();}
              Serial.println("--------Doggo Detected---------");
              TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
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
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Puppy Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");              
              delay(800);
              digitalWrite(DispSignal, LOW);
              Dispense(); // Opens dispenser and light up red bulb signal
              delay(1000);
              DispCount++;  
              Timer = TimerDuration;            

              if (DispCount >= PuppyMax || Timer > 0)
                digitalWrite(DogAvail, LOW);
              else
                digitalWrite (DogAvail, HIGH);

              //For Serial Display (Puppy)
              for(int i = 0; i < 10; i++) {
                Serial.println();}
                Serial.println("---------Puppy DETECTED--------");
                TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
                delay(300);                          
            }
          }
        }
      }    
    }
  }


void CheckTank()
  {
    // Tank Sensor stops detecing object
    if (digitalRead(TankSensor) == 1)
    {
     Ds1302::DateTime now;
     rtc.getDateTime(&now);
     Serial.println("Tank is running out of food, Needs refill");
     delay(10); 
     SIM900A.println("AT+CMGF=1");
     delay(1000);
     SIM900A.println("AT+CMGS=\"+639652866745\"\r");
     delay(1000);
     SIM900A.println("Hi, Dogfood tank is running low. Please Refill");// Messsage content
     delay(1000);
     SIM900A.println((char)26);// ctrl + z
     delay(1000);
     Serial.println("Notif sent owner to refill food tank\n");
     delay(10);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("  NEEDS REFILL");
     delay(3000);
     //TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
    }
  }


  void TextDisplay(String now_year, String now_month, String now_day, String now_hour, String now_second)
  {
      Serial.print(now_month + "/" + now_day + "/" + now_year + " ");
      Serial.print(Hourrr + ":" + Minuteee + " " + DayLight);
      Serial.print(" | Interval: ");
      Serial.print(Timer);
      Serial.println(" Min/s");
      delay(80);
      lcd.clear();
      lcd.setCursor(0, 0);

      
      if (Adult == true)
      {        
        lcd.print("A INT:" + String(Timer) + "m REM:");
        if (DispCount > AdultMax)
          DispCount = AdultMax;
         lcd.print(String(AdultMax - DispCount));          
        Serial.print("Remaining for Dog: " + String(AdultMax - DispCount));
      }
      else
      {    
        lcd.print("P INT:" + String(Timer) + "m REM:");
        if (DispCount > PuppyMax)
          DispCount = PuppyMax;
        lcd.print(String(PuppyMax - DispCount));          
        Serial.print("Remaining for Puppy: " + String(PuppyMax - DispCount));
      }
      delay(100);
      Serial.println(" | " + WillReset);
      lcd.setCursor(0, 1);
      lcd.print(Time + " " + WillReset);
      Serial.println("*** DEVELOPER DATA BELOW ***");
      Serial.println("ATime: " + String(ATime) + " / PTime: " + String(PTime) + " / CTime: " + String(CTime));
      Serial.println("DispCount " + String(DispCount) + " / TankSensor: " + String(digitalRead(TankSensor)));
      Serial.println("-----------------");
  }

  
