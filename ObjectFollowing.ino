
#include <Wire.h>
#include <Servo.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <AFMotor.h>

AF_DCMotor leftFront(1, MOTOR12_64KHZ);                          
AF_DCMotor rightFront(2, MOTOR12_64KHZ);
AF_DCMotor rightBack(3, MOTOR12_64KHZ);
AF_DCMotor leftBack(4, MOTOR12_64KHZ);

int ID1 = 1;
HUSKYLENS huskylens;
Servo servo_9, servo_10;

int posPan = 90;
int oldPan = 90;
int posTilt = 90;
int oldTilt = 90;
float p = 0.05;
float d = 0.07;

int flip_i = 0;

void printResult(HUSKYLENSResult result);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  servo_9.attach(9);
  servo_10.attach(10);   
  while (!huskylens.begin(Wire)){
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
    
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);        

  motorStop();
  motorForward(); 
}

void loop(){
  if (!huskylens.request())
  Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned())
  {
  Serial.println(F("Object not learned!"));
  }
  else if(!huskylens.available())
  {
  Serial.println(F("Object disappeared!"));
  }
  else
  {
    
    while (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();
   
      posPan -= int(float(result.xCenter-160)*p);
      posPan += int((posPan - oldPan)*d);
      
      if(posPan <= 40)        posPan = 40;
      else if(posPan >= 140) posPan = 140;
      servo_9.write(abs(posPan));
      Serial.print("posPan:");
      Serial.print(posPan);
      Serial.print(" center:");
      Serial.print((result.xCenter - 160));
      delay(50);

      posTilt += int(float(result.yCenter-160)*p);
      posTilt -= int((posTilt - oldTilt)*d);
      
      if(posTilt <= 40)        posTilt = 40;
      else if(posTilt >= 120) posTilt = 120;
      servo_10.write(abs(posTilt));
      Serial.print("posTilt:");
      Serial.print(posTilt);
      Serial.print(" center:");
      Serial.print((result.yCenter - 160));
      delay(50);      
      
      if(abs(result.xCenter -160 ) < 40 && abs(posPan-90) > 40 && flip_i){
        if(posPan >= 140){
          motorRight(); 
          }else if(posPan <= 40){
            motorLeft(); 
            }
            flip_i = 0;
            }
            else{
              motorStop();           
              flip_i = 1;
            }
            oldPan = posPan;
    }
  }
}

void printResult(HUSKYLENSResult result){
  if (result.command == COMMAND_RETURN_BLOCK){
    Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
  }

  else if (result.command == COMMAND_RETURN_ARROW){
    Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
  }
  else{
    Serial.println("Object unknown!");
  }   
}

void motorForward(){
  leftFront.run(FORWARD);
  rightFront.run(FORWARD);
  rightBack.run(FORWARD);
  leftBack.run(FORWARD);
}

void motorStop(){
  leftFront.setSpeed(0);
  rightFront.setSpeed(0);
  rightBack.setSpeed(0);
  leftBack.setSpeed(0);
}

void motorRight(){
  leftFront.setSpeed(0);
  rightFront.setSpeed(160);
  rightBack.setSpeed(160);
  leftBack.setSpeed(0);
}

void motorLeft(){
  leftFront.setSpeed(160);
  rightFront.setSpeed(0);
  rightBack.setSpeed(0);
  leftBack.setSpeed(160);
}
