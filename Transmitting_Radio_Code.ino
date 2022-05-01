#include <SPI.h>
#include <RF24.h>

// CHECK THE WIRING EXCEL TO ENSURE THAT EVERYTHING IS CORRECT IF ERROR PERSISTS

RF24 radio(9,10);

#define echoPin 7 // attach pin 7 Arduino to pin Echo of HC-SR04
#define trigPin 6 //attach pin 6 Arduino to pin Trig of HC-SR04


// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int rawDistance;
int sumDist;
int avgDist;
int TXmsg = 0;
double prevTime = 0.;
int count = 0;
double zp = 0.;

void setup() {
  radio.begin();
  radio.openWritingPipe(42);
  //radio.stopListening();
  radio.setPALevel(RF24_PA_MIN);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600);
}

void loop() {
  sumDist = 0;
  count = 0;
  
  while (millis()- prevTime <= 500) {
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculating the distance
    rawDistance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    sumDist = rawDistance + sumDist;
  }
  
  avgDist = sumDist/count;

  if (avgDist <= 91+(zp*100)) { // Because this is in cm this equates to approximately a three feet away
    avgDist *= 0.394; // Converts from cm to inches for this step
    avgDist -= 39.37*zp;
    TXmsg = avgDist;
  }

  radio.write(&TXmsg, sizeof(TXmsg));
  Serial.println(TXmsg);

  prevTime = millis();
  
}
