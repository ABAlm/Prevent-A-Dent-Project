#include <SPI.h>
#include <RF24.h> // Needed for radio to work
#include <SevSeg.h>// Needed for the displays to work

RF24 radio(9, 10);
SevSeg sevseg; // initate 7 segment display

int RXmsg = 0;
int cm;
int interval;
int prevTime = 0;
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;

void setup() {

  // Radio setup
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("Radio failed");
  }
  radio.openReadingPipe(0, 42);
  radio.startListening();
    
  // Display setup
  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);
  
  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);
  sevseg.setBrightness(99);
  
}

void loop() {

  // Radio connection
  // Checks if the radio is available and then sets the RXmsg
  if (millis() - prevTime >= 500) {
    if (radio.available()) {
      radio.read(&RXmsg, sizeof(RXmsg)); // Reads the TXmsg
      showNumber(RXmsg);
    }
    else {
      digitalWrite(segmentClock, LOW);
      digitalWrite(segmentData, LOW);
      digitalWrite(segmentLatch, LOW);
    }
    prevTime = millis();
  }
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(int num) // Setting equal to the RXmsg, because that's the number we need to print
{
  int number = abs(num); //Remove negative signs and any decimals which shouldn't be an issue (for thoroughness)

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
    
  }
  
}
