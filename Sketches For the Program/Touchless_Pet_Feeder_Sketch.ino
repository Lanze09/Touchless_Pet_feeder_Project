 /*------------------------------- Touchless Pet Feeder -------------------------------

+Updated Dispense() +Updated_Refill_Display +1Initial_Dispense_Per_MT_Change +All_Components_works

Desciprtion of the last actions: 
• Added command to perform 1 default initial dispense every meal time changes
• Changed LCD Display "NEEDS REFILL" to "REFILL NOTIF SENT TO OWNER"
• Reduced the motor open duration to 500 millisecond
• Updated Dispense() Function to have delay between actions and also increment DispCount automatically
• Removed unnecessary parts of the code
• Cleaned the codes into presentable form
• All components are working
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

  #define GSM Serial1

  //TENTATIVE VALUES
  int TimerDuration = 3; // timer Duration per minute
  int PuppyMax = 5; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  int AdultMax = 10; // Change the value to average number of dispense to complete the enough amount of meal for the daytime
  bool Adult, CAge; // Switch for dog age (false=Puppy ; true=Adult/Senior)

  // Essential Variables
  int DispCount = 0; // Food dispensing frequency counter
  int CTime = 10; //TENTATIVE- Current time / Last identified "Time"
  int ATime = 0; // Time checker for adult / senior
  int PTime = 0; // Time checker for puppy
  int Timer = 0; // For dispense interval (The pause before next dispense)
  int TCounter = 0;
  int rem, max;
  long SensorDur, SensorDist; 
  char FL;  
  String DayLight, Hourrr, Minuteee, Seconddd, WillReset, Time, readSMS;

  // Pin Variables
  const int PIN_CLK = 3;
  const int PIN_DAT = 4;
  const int PIN_ENA = 5;
  const int DogAvail = 6;
  const int ServoPin = 9;  
  const int SwitchPin = 10;
  const int TankSensor = 12;
  const int DispSignal = 13;
  const int HumanPin = A0;
  const int DogTrig = A1;
  const int DogEcho = A2;

  Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT); 
  LiquidCrystal_I2C lcd(0x27,16,2);
  Servo DispMotor;

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
    
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(80);
  
  pinMode (SwitchPin, INPUT_PULLUP);
  pinMode (DogTrig, OUTPUT);
  pinMode (DogEcho, INPUT);
  pinMode (HumanPin, INPUT);
  pinMode (DispSignal, OUTPUT);
  pinMode (ServoPin, OUTPUT);
  pinMode (TankSensor, INPUT);
  pinMode (DogAvail, OUTPUT);

  rtc.init(); 
  lcd.init(); 
  delay(80);
  lcd.setContrast(255); 
  lcd.backlight();
  DispMotor.attach(ServoPin);
  DispMotor.write(0);
         
          // ------------------------------ Use this to reprogram RTC Module ------------------------------
   /*           Ds1302::DateTime dt = {
              .year = 23,
              .month = Ds1302::MONTH_JUN,
              .day = 22,
              .hour = 17,
              .minute = 25,
              .second = 0,
              .dow = Ds1302::DOW_MON};
              rtc.setDateTime(&dt);*/
      
//To check Feeder mode switch: (Adult or Puppy)
  if (digitalRead(SwitchPin) == 1)
    {
      Adult = true;
      CAge = true;
    }
  else
    {
      Adult = false;
      CAge = false;
    }



  GSM.println("AT+CMGF=1");
  delay(500);
  GSM.println("AT+CNMI=1,2,0,0,0");
  delay(500);  

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
  }

  void Dispense()
  {
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    digitalWrite(DispSignal, HIGH);
    delay(500);
    DispMotor.write(180);
    delay(500);
    DispMotor.write(0);
    delay(800);
    digitalWrite(DispSignal, LOW);
    DispCount++;
    TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
  }

  void loop() {

  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  digitalWrite(DispSignal, LOW);

//Ultimate Checker
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
    case 'x':
    Serial.println("SMS = " + readSMS);
      break;
  }
}

if (GSM.available()>0)
  {
    readSMS = FilterSMS();
    delayMicroseconds(10);
    readSMS = readSMS.substring(2);
    readSMS = readSMS.substring(0, readSMS.length() -2);
    readSMS.toLowerCase();
    Serial.println("readSMS: " + readSMS);
    if (readSMS == "c")
    {
     if (Adult == true)
      {
        rem = AdultMax - DispCount;
        max = AdultMax;
      }
     else
      {
        rem = PuppyMax - DispCount;
        max = PuppyMax;
      }
     if (rem < 0)
      rem = 0;
     GSM.println("AT+CMGS=\"+639654337341\"\r");
     delay(500);
     GSM.println("Hi, feeder's running timer is " + String(Timer) + "m | Remaining dispense for this Mealtime: " + String(rem) + " out of " + String(max) + " | " + WillReset);
     delay(500);
     GSM.println((char)26);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(" SMS Command!");
     lcd.setCursor(0,1);
     lcd.print("Checked Details");
     delay(2000);
     TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
    }
  else if (readSMS == "d")
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" SMS Command!");
      lcd.setCursor(0,1);
      lcd.print("Dispenses Food!");
      delay(1000);
      Dispense();
      TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
    }
    else
    {      
      for (size_t i = 0; i < readSMS.length() - 1; ++i) 
      {
        if (readSMS.substring(i, i + 2) == "ii") 
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("GSM Connection");
          lcd.setCursor(0,1);
          lcd.print(" is HALTED!");
          delay(2000);
          TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
        }
      }
    }
  }

  // test if clock is halted a+nd set a date-time to my birthday at 4:20pm
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






Adult = digitalRead(SwitchPin);
if (Adult != CAge) //When feeder mode has been switched
{    
    if (Adult == true)
    {
      CAge = true;
      DispCount = 0;
      Timer = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Changed to Adult");
      lcd.setCursor(0, 1);
      lcd.print("Resetting now!");
    }
    else
    {
      CAge = false;
      DispCount = 0;
      Timer = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Changed to Puppy");
      lcd.setCursor(0, 1);
      lcd.print("Resetting now!");
    }
    delay(3000);
    TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
}

  //Time Checker
  if (Adult == true) //ADT Time Checker
  {
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
  if (now.hour <= 12) //for AM
    {
      Hourrr = String(now.hour);
      DayLight = "AM";
      if (now.hour == 0)
        Hourrr = "12";
      if (now.hour == 12) 
        DayLight = "PM";
    }
  else //for PM
    {
     Hourrr = String(now.hour - 12);
     DayLight = "PM";
    }

  //Make hour 2 digits if below 10
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

  Time = (Hourrr + ":" + Minuteee + DayLight);

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

      // Reset if Meal Time changes
      if (Adult == true)
      {
        if (CTime != ATime) //Meal time changes. Will reset the DispCount to zero
        {
          CTime = ATime;
          DispCount = 0;
          digitalWrite (DogAvail, HIGH);
          CheckTank();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Initial Time!");
          lcd.setCursor(0,1);
          lcd.print("Dispenses Food!");
          Dispense();
        }
      }
      else
      {
        if (CTime != PTime) //Meal time changes. Will reset the DispCount to zero
        {
          CTime = PTime;
          DispCount = 0;
          digitalWrite (DogAvail, LOW);
          CheckTank();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Initial Time!");
          lcd.setCursor(0,1);
          lcd.print("Dispenses Food!");
          Dispense();
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
      Dispense();  
      for(int i = 0; i < 10; i++) 
      {
        Serial.println();}
        Serial.println("--------HUMAN DETECTED---------");
        TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
      }
    delay(300);  


//Sends ultrasonic pulse
 digitalWrite(DogTrig, LOW);
 delayMicroseconds(2);
 digitalWrite(DogTrig, HIGH);
 delayMicroseconds(10);
 digitalWrite(DogTrig, LOW);
 SensorDur = pulseIn(DogEcho, HIGH);
 SensorDist = (SensorDur * 0.034 / 2);

  //   ------------------- Dog or Puppy Detected ------------------- fDog
 if (SensorDist < 12.5)
  {
  if (Timer <= 0)
    {
    if (Adult == true) 
    {     
      if (ATime != 3) 
      {
        if (CTime != ATime) //Meal time changes. Will reset the DispCount to zero
        {
        CTime = ATime;
        DispCount = 0;
        CheckTank();
        digitalWrite (DogAvail, HIGH);
        }      
        
        if (DispCount < AdultMax)
          {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Dog Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");                            
              digitalWrite(DispSignal, LOW);
              Dispense();
              Timer = TimerDuration;

              //Setting DogAvail LED
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

      else //Not adult
        {
          if (PTime != 4)
          {
            if (CTime != PTime) //Meal time changes. Will reset the DispCount to zero
            {
            CTime = PTime;
            DispCount = 0;
            digitalWrite (DogAvail, HIGH);
            }
            if (DispCount < PuppyMax)
            {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Puppy Detected!");
              lcd.setCursor(0,1);
              lcd.print("Dispenses Food!");              
              digitalWrite(DispSignal, LOW);
              Dispense();
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
     GSM.println("AT+CMGS=\"+639654337341\"\r");
     delay(500);
     GSM.println("Hi, Dogfood tank is running low. Please Refill");// Messsage content
     delay(500);
     GSM.println((char)26);
     delay(500);
     Serial.println("Notif sent owner to refill food tank\n");
     delay(10);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(" REFILL NOTIF");
     lcd.setCursor(0,1);
     lcd.print("SENT TO OWNER");              
     delay(3000);
     TextDisplay(String(now.year), String(now.month), String(now.day), String(now.hour), String(now.second));
    }
  }

String FilterSMS()
{
  int x=0;
  String sms = ""; 
  unsigned long int prevM = millis(); 
  while(1)
  {
    if(GSM.available() >0)
    {
      char incomingText=GSM.read();
      sms += incomingText; 
      prevM = millis();
      x++; 
    }
    unsigned long int newM = millis();
    if((x>0)&&((newM - prevM)>200)) 
      break;     
  }  
  int m = 1 + sms.lastIndexOf('"');
  int n = sms.length(); 
  return(sms.substring(m,n)); 
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
      //Serial.println("readSMS: " + readSMS);
      Serial.println("SensorDist: " + String(SensorDist) + " / DogEcho: " + String(DogEcho) + " / SwitchPin: " + String(SwitchPin));
      Serial.println("-----------------");
  }
