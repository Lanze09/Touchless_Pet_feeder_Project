// defines pins numbers
const int trigPin2 = 2;
const int echoPin3 = 3;
const int buzzer4 = 4;
const int trigPin = 9;
const int echoPin = 10;
const int buzzer = 11;
const int buzzer0 = 0;
const int ledPin = 8;
const int ledPin7 = 7;
const int vmotor1 = 12;
const int vmotor2 = 13;
const int SensorPin = 6;  // IR pin
const int LED = 5; // LED pin

// defines variables
long duration;
int distance;
int safetyDistance;


void setup() {
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin3, INPUT); // Sets the echoPin as an Input
pinMode(buzzer4, OUTPUT);
pinMode(buzzer, OUTPUT);
pinMode(buzzer0, OUTPUT);
pinMode(ledPin, OUTPUT);
pinMode(ledPin7, OUTPUT);
pinMode(vmotor1, OUTPUT);
pinMode(vmotor2, OUTPUT);
pinMode(SensorPin, INPUT);
pinMode(LED, OUTPUT);
Serial.begin(9600); // Starts the serial communication
}


void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

safetyDistance = distance;
if (safetyDistance <= 6){
  digitalWrite(buzzer, HIGH);
  digitalWrite(ledPin, HIGH);
  digitalWrite(vmotor2, HIGH);
}
else{
  digitalWrite(buzzer, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(vmotor2, LOW);
}


////
// Clears the trigPin
digitalWrite(trigPin2, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin2, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin2, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin3, HIGH);

// Calculating the distance
distance= duration*0.034/2;

safetyDistance = distance;
if (safetyDistance <= 6){
  digitalWrite(buzzer4, HIGH);
  digitalWrite(ledPin7, HIGH);
  digitalWrite(vmotor1, HIGH);
}
else{
  digitalWrite(buzzer4, LOW);
  digitalWrite(ledPin7, LOW);
  digitalWrite(vmotor1, LOW);
}
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);

///////////////////////////
int SensorValue = digitalRead(SensorPin);

  if (SensorValue==LOW){ // LOW MEANS Object Detected
    digitalWrite(LED, HIGH);
      digitalWrite(buzzer, HIGH);
        digitalWrite(vmotor2, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW); 
      digitalWrite(buzzer, LOW);
        digitalWrite(vmotor2, LOW);
  }

}
