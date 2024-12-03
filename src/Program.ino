Code:
#include <stdio.h>
#include <ServoTimer2.h>  // the servo library
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>


//#include <Servo.h>
#include"ServoTimer2.h"
#define rollPin  A4
ServoTimer2 servoRoll;

const String PHONE = "+917780336029";

#define rxPin 2
#define txPin 3

SoftwareSerial mySerial(rxPin, txPin);

AltSoftSerial neogps;
TinyGPSPlus gps;
//   alt pins uno pins
//rx    9
//tx    8
unsigned char rcv;

//ultrasonic
char value;
int distance;

//directions
int L = 0;
int R = 0;
int L1 = 0;
int R1 = 0;
#define in1 7   //Motor1  L298 Pin in1
#define in2 6   //Motor1  L298 Pin in1
#define in3 5   //Motor2  L298 Pin in1
#define in4 4   //Motor2  L298 Pin in1


//pins
#define Echo A0
#define Trig A1
#define motor 10
#define Speed 170
#define spoint 103

int Left;
int Right;
//car
void forward() {

  digitalWrite(in1, LOW);   //Left Motor backword Pin
  digitalWrite(in2, HIGH);  //Left Motor forword Pin
  digitalWrite(in3, LOW);   //Right Motor forword Pin
  digitalWrite(in4, HIGH);  //Right Motor backword Pin
}
void backward() {
  digitalWrite(in1, HIGH);  //Left Motor backword Pin
  digitalWrite(in2, LOW);   //Left Motor forword Pin
  digitalWrite(in3, HIGH);  //Right Motor forword Pin
  digitalWrite(in4, LOW);   //Right Motor backword Pin
}
void right() {
  digitalWrite(in1, LOW);  //Left Motor backword Pin
  digitalWrite(in2, HIGH);   //Left Motor forword Pin
  digitalWrite(in3, HIGH);   //Right Motor forword Pin
  digitalWrite(in4, LOW);   //Right Motor backword Pin
}
void left() {
  digitalWrite(in1, HIGH);   //Left Motor backword Pin
  digitalWrite(in2, LOW);   //Left Motor forword Pin
  digitalWrite(in3, LOW);  //Right Motor forword Pin
  digitalWrite(in4, HIGH);   //Right Motor backword Pin
}
void Stop() {
  digitalWrite(in1, LOW);  //Left Motor backword Pin
  digitalWrite(in2, LOW);  //Left Motor forword Pin
  digitalWrite(in3, LOW);  //Right Motor forword Pin
  digitalWrite(in4, LOW);  //Right Motor backword Pin
}


// Ultrasonic sensor distance reading function
int ultrasonic() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(4);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long t = pulseIn(Echo, HIGH);
  long cm = t / 29 / 2;  //time convert distance
  Serial.println(cm);
  return cm;
}






void setup_car() {
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}
//end car
void okcheck() {
  unsigned char rcr;
  do {
    rcr = mySerial.read();
  } while (rcr != 'K');
  Serial.println("at ok");
}


void gsminit() {
  mySerial.write("AT\r\n");
  okcheck();
  mySerial.write("ATE0\r\n");
  okcheck();
  mySerial.write("AT+CMGF=1\r\n");
  okcheck();
}


void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}



void sendSmsGPS(String text) {
  // Can take up to 60 seconds
  boolean newData = true;
  /*
  for (unsigned long start = millis(); millis() - start < 2000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
      }
    }
  }*/
  Serial.println("in send loc");
  if (newData)  //If newData is true
  {
    while (neogps.available() > 0) {
      Serial.println("in loop");
      if (gps.encode(neogps.read())) {
        displayInfo();
      }
    }
    Serial.println("new data");
    float flat, flon;
    unsigned long age;
    Serial.print("Latitude= ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" Longitude= ");
    Serial.println(gps.location.lng(), 6);
    newData = false;
    delay(300);
    ///*
    mySerial.print("AT+CMGF=1\r");
    delay(1000);
    mySerial.print("AT+CMGS=\"" + PHONE + "\"\r");
    delay(1000);
    mySerial.print("http://maps.google.com/maps?q=loc:");
    mySerial.print(gps.location.lat(), 6);
    mySerial.print(",");
    mySerial.print(gps.location.lng(), 6);
    delay(100);
    mySerial.write(0x1A);
    delay(1000);
    Serial.println("loc sent");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
}


void sendSms(String text) {
  mySerial.print("AT+CMGF=1\r");
  delay(1000);
  mySerial.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  mySerial.print(text);
  delay(100);
  mySerial.write(0x1A);
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}

int rightsee() {
  servoRoll.write(20);
  delay(800);
  Left = ultrasonic();
  return Left;
}
int leftsee() {
  servoRoll.write(180);
  delay(800);
  Right = ultrasonic();
  return Right;
}


void Obs() {

  distance = ultrasonic();
  if (distance <= 30 && distance >= 0) {
    Stop();
    backward();
    delay(100);
    Stop();
    L = leftsee();
    servoRoll.write(spoint);
    delay(800);
    R = rightsee();
    servoRoll.write(spoint);
    if (L < R) {
      right();
      delay(500);
      Stop();
      delay(200);
    } else if (L > R) {
      left();
      delay(500);
      Stop();
      delay(200);
    }
  } else {
    forward();
  }
}


void setup() {
  //servo
  servoRoll.attach(rollPin);
  servoRoll.write(70);
  delay(150);
  servoRoll.write(100);
  delay(500);
  servoRoll.write(90);

  delay(100);
  //servo end
  
  delay(7000);
  Serial.begin(115200);
  mySerial.begin(9600);
  neogps.begin(9600);
  setup_car();
  void gsminit();

  Serial.println("neogps Software serial initialize");
  mySerial.print("AT+CMGF=1\r");  //SMS text mode
  delay(1000);


  sendSms("reg");
  delay(5000);
  sendSmsGPS("in loc");
  delay(2000);



}


void loop() {

  //delay(100);
  Obs();
  if(distance <= 5 && distance > 0){
//  if (digitalRead(button) == LOW) {
    delay(3000);
    Serial.println("accident detect");
    sendSmsGPS("accident detected in loc");
  }
}