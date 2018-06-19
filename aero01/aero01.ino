// This is a simple working-example to quick measuring:
// 1. the x, y and z-values read from a GY-61-acceleration-module
// 2. an angle in eg. the x-direction

// You can put the module in 4 directions to determine the min-max-values:
// –>   0deg   x = 349  y = 341  z = 425
// /\    90deg  x = 281  y = 345  z = 357
// <–  180deg  x = 350  y = 345  z = 288
//  V   270deg  x = 419  y = 341  z = 355
// This can be used to measure degrees

// you can use all the analog inputs to control the GY-61
// picture: https://www.fabtolab.com/ADXL335-GY61?search=gy-61

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

const int VCCPin = A0;
const int xPin   = A1;
const int yPin   = A2;
const int buttonPin = 2;     // the number of the pushbutton pin
const int zPin   = A3;
const int GNDPin = A4;
 const byte pinDfpBusy = 13;  


// variables
int buttonState = 0;   
int x = 0;
int y = 0;
int z = 0;
int hit = 0;
int count = 0;
int maxCount = 10;
int prevX = x;
int speedThreshold = 3;
int angleThreshold = 40;
int xAngle = 0;
int yAngle = 0;
int currentTune = 1;

void setup() {
  // pin A0 (pin14) is VCC and pin A4 (pin18) in GND to activate the GY-61-module
  pinMode(A0, OUTPUT);
  pinMode(A4, OUTPUT);
  digitalWrite(14, HIGH);
  digitalWrite(18, LOW);
  pinMode(buttonPin, INPUT);
  
  // activating debugging for arduino UNO
  Serial.begin(9600);
  
  mySoftwareSerial.begin(9600);
  pinMode(pinDfpBusy, INPUT);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(23);  //Set volume value. From 0 to 30
  myDFPlayer.enableLoopAll(); //loop all mp3 files.
  myDFPlayer.play(1);  //Play the first mp3
  myDFPlayer.pause();  //pause the mp3
  

} // end setup

void loop() {
  x = analogRead(xPin);
  y = analogRead(yPin);
  z = analogRead(zPin);
  buttonState = digitalRead(buttonPin);
  
  // show x, y and z-values
//  Serial.print("x = ");
//  Serial.print(x);
//  Serial.print("  y = ");
//  Serial.print(y);
//  Serial.print("  z = ");
//  Serial.print(z);
//  // show angle
//  Serial.print("  angle = ");
  xAngle = constrain(map(x,433,281,90,-90),-90,90);
  yAngle = constrain(map(y,424,281,90,-90),-90,90);
//  Serial.print(xAngle);
//  Serial.print(" ");
  
//  Serial.print(hit);
//  Serial.print(" ");
//  Serial.print(count);
  if (buttonState == HIGH) {
    Serial.print("button Pressed");
    myDFPlayer.advertise(3);
    delay(1000);
  }
  
  else if (abs(yAngle) > angleThreshold) {
    if (digitalRead(pinDfpBusy == HIGH)) {
      Serial.print(currentTune);
      Serial.println("turning");
      myDFPlayer.start();
      delay(100);
    hit ++;
    }
    
  } 
  
  else if (abs(x - prevX) > speedThreshold) {
    if (digitalRead(pinDfpBusy) == HIGH) {
      Serial.print(currentTune);
      Serial.println("fly straight");
      
      myDFPlayer.start();
      delay(100);
    }
  }
//  Serial.print("count ");
//  Serial.print(count);
  if (count == maxCount) {
    Serial.print("Tune: ");
    Serial.println(currentTune);
    if (hit < 5 && currentTune == 2) {
    // change tune
      myDFPlayer.play(2);
      currentTune = 1;
    } else if (hit >= 5 && currentTune == 1) {
      myDFPlayer.play(1);
      currentTune = 2;

    }
    myDFPlayer.pause();
    hit = 0;
    count = 0;
  }
 

  prevX = x;
  count ++;
} // end loop

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
