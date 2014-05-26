#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

// SSC
#define SSC_BAS 7
#define SSC_SHL 6
#define SSC_ELB 5
#define SSC_WRI 4
#define SSC_FIN 3

#define SSC_RST 20
#define X_MOT_DIR 8
#define X_MOT_STP 9

#define Y_MOT_DIR 7
#define Y_MOT_STP 6

// end stops
#define X_END 48
#define Y_END 50

#define X_SIZE 5000

int xLast = 0;
int xWait = 5000;

int pose_park[] = {3000,3500,2000,3000,3000};
int pose_rise[] = {3000,3000,3000,3000,3000};
int pose_0[] = {2900,3200,3500,2700,3200};

int paint_0[] = {0,0,0,0,0};
int paint_1[] = {0,0,0,0,0};
int paint_2[] = {0,0,0,0,0};
int paint_3[] = {0,0,0,0,0};
int paint_4[] = {0,0,0,0,0};
int paint_5[] = {0,0,0,0,0};

int paint_clean1[] = {0,0,0,0,0};
int paint_clean2[] = {0,0,0,0,0};

int xMotorSpeed = 800; //maximum steps per second
int xMotorAccel = xMotorSpeed; //steps/second/second to accelerate

int yMotorSpeed = 800;
int yMotorAccel = 400;

AccelStepper xStepper(1, X_MOT_STP, X_MOT_DIR);
AccelStepper yStepper(1, Y_MOT_STP, Y_MOT_DIR);

int thres = 2000;
int pose_count = 0;
long lastMove = 0;

CwPololuSerialServo servos(&Serial1, 38000);

void xZero() {
  
  xStepper.setSpeed(500);
  xStepper.moveTo(-10000);
  while(digitalRead(X_END) == 0) {
    xStepper.run();
  }
  xStepper.stop();
  delay(500);
  xStepper.setCurrentPosition(-5000); // do 0  
}

void yZero() {
  yStepper.setSpeed(500);
  yStepper.moveTo(-10000);
  while(digitalRead(Y_END) == 0) {
    yStepper.run();
  }
  yStepper.stop();
  delay(500);
  yStepper.setCurrentPosition(-5000);
}

void servo_speed() {
  servos.setSpeed(SSC_FIN,10);
  servos.setSpeed(SSC_WRI,10);
  servos.setSpeed(SSC_ELB,7);
  servos.setSpeed(SSC_SHL,5);
  servos.setSpeed(SSC_BAS,8);  
}

void servo_park() {
  servoPose(2900,3200,3500,2700,3200);
}

void servoPose(int bas, int shl, int elb, int wri, int fin) {
  servos.setPositionAbsolute(SSC_BAS,bas);
  servos.setPositionAbsolute(SSC_SHL,shl);
  servos.setPositionAbsolute(SSC_ELB,elb);
  servos.setPositionAbsolute(SSC_WRI,wri);
  servos.setPositionAbsolute(SSC_FIN,fin);
}

void setup() {

  pinMode(X_END,INPUT);
  pinMode(Y_END,INPUT);
  pinMode(SSC_RST, OUTPUT);

  digitalWrite(SSC_RST, LOW);
  delay(100);
  digitalWrite(SSC_RST, HIGH);
  delay(100);

  servo_speed();
  delay(1000);
  servo_park();
  delay(3000);

  xStepper.setMaxSpeed(xMotorSpeed);
  yStepper.setMaxSpeed(yMotorSpeed);

  xStepper.setAcceleration(xMotorAccel);
  yStepper.setAcceleration(yMotorAccel);

  xZero();
  yZero();

  xStepper.moveTo(5000);
  yStepper.moveTo(5000);

}

void loop() {

  if(lastMove + thres < millis()) {

    if(pose_count==0) {
      servoPose(2900,3200,3500,2700,3200);
      pose_count=1;
    } else if (pose_count==1){
      servo_park();
      pose_count=0;
    } 
    lastMove = millis();
  }

  if(xStepper.distanceToGo() == 0) {
    xStepper.moveTo(-xStepper.currentPosition());
    
  }

  if(yStepper.distanceToGo() == 0) {
    yStepper.moveTo(-yStepper.currentPosition());
  }

  xStepper.run();
  yStepper.run();

}

