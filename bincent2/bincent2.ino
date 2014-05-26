#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

#define BAS_SERVO 7
#define SHL_SERVO 6
#define ELB_SERVO 5
#define WRI_SERVO 4
#define FIN_SERVO 3

#define X_END 48
#define Y_END 50

int xMotorSpeed = 200; //maximum steps per second
int xMotorMax = 500;
int xMotorAccel = 200; //steps/second/second to accelerate

int yMotorSpeed = 200;
int yMotorMax = 500;
int yMotorAccel = 200;

int xMotorDirPin = 8;
int xMotorStepPin = 9;

int yMotorDirPin = 7;
int yMotorStepPin = 6;

AccelStepper xStepper(1, xMotorStepPin, xMotorDirPin);
AccelStepper yStepper(1, yMotorStepPin, yMotorDirPin);

int thres = 5000;
int arm = 0;
long lastMove = 0;

CwPololuSerialServo servos(&Serial1, 38000);
  
void setup() {

  servoSetSpeed(FIN_SERVO,10);
  servoSetSpeed(WRI_SERVO,10);
  servoSetSpeed(ELB_SERVO,7);
  servoSetSpeed(SHL_SERVO,5);
  servoSetSpeed(BAS_SERVO,8);
  
  servo_park();

  xStepper.setMaxSpeed(xMotorSpeed);
  yStepper.setMaxSpeed(yMotorSpeed);

  xStepper.setAcceleration(xMotorAccel);
  yStepper.setAcceleration(yMotorAccel);
 
  xZero();
  yZero();

  xStepper.moveTo(3000);
  yStepper.moveTo(3000);

  pinMode(xEndPin,INPUT);
  pinMode(yEndPin,INPUT);

}

void loop() {

    if(lastMove + thres < millis()) {
      if(arm==0) {
      servos.setPositionAbsolute(BAS_SERVO,2900);
      servos.setPositionAbsolute(SHL_SERVO,3500);
      servos.setPositionAbsolute(ELB_SERVO,3700);
      servos.setPositionAbsolute(WRI_SERVO,2700);
      servos.setPositionAbsolute(FIN_SERVO,2500);
        lastMove = millis();
        arm = 1;
      } else {
        servo_park();
        lastMove = millis();
        arm = 0;
      }
    }
  
    xStepper.run();
    yStepper.run();

}

void xZero() {
  Serial.println("zeroing X axis");
  xStepper.setSpeed(500);
  xStepper.moveTo(-10000);
  while(digitalRead(X_END) == 0) {
    xStepper.run();
  }
  xStepper.stop();
  xStepper.setCurrentPosition(-3000); // do 0
  Serial.println("** STOP ** // X axis zeroed.");
  delay(500);
}

void yZero() {
  Serial.println("zeroing Y axis");
  yStepper.setSpeed(500);
  yStepper.moveTo(-10000);
  while(digitalRead(Y_END) == 0) {
    yStepper.run();
  }
  yStepper.stop();
  yStepper.setCurrentPosition(-3000);
  Serial.println("** STOP ** // Y axis zeroed.");
  delay(500);
}

void servo_park() {
  servos.setPositionAbsolute(BAS_SERVO,3000);
  servos.setPositionAbsolute(SHL_SERVO,3000);
  servos.setPositionAbsolute(ELB_SERVO,3000);
  servos.setPositionAbsolute(WRI_SERVO,3000);
  servos.setPositionAbsolute(FIN_SERVO,3000);
}

void servoPose(int bas, int shl, int elb, int wri, int fin) {
  servos.setPositionAbsolute(BAS_SERVO,bas);
  servos.setPositionAbsolute(SHL_SERVO,shl);
  servos.setPositionAbsolute(ELB_SERVO,elb);
  servos.setPositionAbsolute(WRI_SERVO,wri);
  servos.setPositionAbsolute(FIN_SERVO,fin);
}


