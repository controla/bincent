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

#define X_LEN 10000
#define Y_LEN 13500

int pose_park[] = {3000,3500,2000,3000,3000};
int pose_rise[] = {3000,3000,3000,3000,3000};
int pose_0[] = {2900,3200,3500,2700,3200};

int paint_0[] = {2780,3830,2500,3500,3400}; // ok
int paint_1[] = {2800,3150,3300,3000,3200}; // mas atras
int paint_2[] = {2930,3060,3470,2800,2923}; // ok
int paint_3[] = {3100,3130,3610,3000,3000}; // ok
int paint_4[] = {2900,3130,3776,3200,3350}; // ok bit atras
int paint_5[] = {3200,3500,3100,2500,3100}; // bit less
int paint_6[] = {3000,2900,3000,3000,3000}; // rise up
int paint_7[] = {3000,2000,4000,3000,3000}; // stand back in awe.

int refill_0[] = {1200,4500,4000,3000,2900};
int refill_1[] = {1330,4400,4000,3000,3400};
int refill_2[] = {1180,4400,4000,3000,3400};
int refill_3[] = {1050,4400,4000,2700,3250};
int refill_4[] = {1320,4400,4000,3000,3800};
int refill_5[] = {1100,4200,3500,3000,3200};
int refill_6[] = {1000,4200,3500,2700,3200};
int refill_7[] = {1000,4400,4600,2500,3100};
int refill_8[] = {1000,3000,3000,3000,2900};

int paint_clean1[] = {1500,3500,3000,3000,3000};
int paint_clean2[] = {1500,3500,3000,3000,3000};

int xMotorSpeed = 800; //maximum steps per second
int xMotorAccel = xMotorSpeed; //steps/second/second to accelerate

int yMotorSpeed = 800;
int yMotorAccel = 400;

AccelStepper xStepper(1, X_MOT_STP, X_MOT_DIR);
AccelStepper yStepper(1, Y_MOT_STP, Y_MOT_DIR);

CwPololuSerialServo servos(&Serial1, 38000);

void xZero() {
  xStepper.setSpeed(500);
  xStepper.moveTo(-20000);
  while(digitalRead(X_END) == 0) {
    xStepper.run();
  }
  xStepper.stop();
  delay(500);
  xStepper.setCurrentPosition(0);
}

void yZero() {
  yStepper.setSpeed(500);
  yStepper.moveTo(-20000);
  while(digitalRead(Y_END) == 0) {
    yStepper.run();
  }
  yStepper.stop();
  delay(500);
  yStepper.setCurrentPosition(0);
}

void servo_speed() {
  servos.setSpeed(SSC_FIN,10);
  servos.setSpeed(SSC_WRI,10);
  servos.setSpeed(SSC_ELB,5);
  servos.setSpeed(SSC_SHL,10);
  servos.setSpeed(SSC_BAS,5);  
}

void servo_park() {
  servoPose(3000,2000,4000,3000,3200);
}

void servoPose(int bas, int shl, int elb, int wri, int fin) {
  servos.setPositionAbsolute(SSC_BAS,bas);
  servos.setPositionAbsolute(SSC_SHL,shl);
  servos.setPositionAbsolute(SSC_ELB,elb);
  servos.setPositionAbsolute(SSC_WRI,wri);
  servos.setPositionAbsolute(SSC_FIN,fin);
}

void setup() {
  Serial.begin(115200);

  randomSeed(analogRead(4));

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

  xNew();
  yNew();
}

void xNew() {
  int salt = random(1000);
  int newX = constrain(random(xLength), 1000, xLength);
  Serial.print("newX: ");
  Serial.println(newX);
  xStepper.moveTo(newX);
}

void yNew() {
  int salt = random(1000);
  int newY = constrain(random(yLength), 1000, yLength);
  Serial.print("newY: ");
  Serial.println(newY);
  yStepper.moveTo(newY);
}

void loop() {

  if(justPainted + strokeTimer < millis()) {
    if(doPaint) {
      paintCount = random(8);
      if(paintCount == 0) {
        Serial.println("stroke 0");
        servoPose(paint_0[0],paint_0[1],paint_0[2],paint_0[3],paint_0[4]);
      }
       if(paintCount == 1) {
        Serial.println("stroke 1");
        servoPose(paint_1[0],paint_1[1],paint_1[2],paint_1[3],paint_1[4]);
      }
       if(paintCount == 2) {
        Serial.println("stroke 2");
        servoPose(paint_2[0],paint_2[1],paint_2[2],paint_2[3],paint_2[4]);
      }
       if(paintCount == 3) {
        Serial.println("stroke 3");
        servoPose(paint_3[0],paint_3[1],paint_3[2],paint_3[3],paint_3[4]);
      }
       if(paintCount == 4) {
        Serial.println("stroke 4");
        servoPose(paint_4[0],paint_4[1],paint_4[2],paint_4[3],paint_4[4]);
      }
       if(paintCount == 5) {
        Serial.println("stroke 5");
        servoPose(paint_5[0],paint_5[1],paint_5[2],paint_5[3],paint_5[4]);
      }
       if(paintCount == 6) {
        Serial.println("stroke 6");
        servoPose(paint_6[0],paint_6[1],paint_6[2],paint_6[3],paint_6[4]);
      }
       if(paintCount == 7) {
        Serial.println("stroke 7");
        servoPose(paint_7[0],paint_7[1],paint_7[2],paint_7[3],paint_7[4]);
      }
    }

    if(doRefill) {
      Serial.println("arriba");
      servoPose(refill_8[0],refill_8[1],refill_8[2],refill_8[3],refill_8[4]);
      delay(5000);
      Serial.println("aguarraz");
      servoPose(refill_7[0],refill_7[1],refill_7[2],refill_7[3],refill_7[4]);
      delay(5000);
      Serial.println("arriba");
      servoPose(refill_8[0],refill_8[1],refill_8[2],refill_8[3],refill_8[4]);
      delay(5000);
      Serial.println("paño");
      servoPose(refill_0[0],refill_0[1],refill_0[2],refill_0[3],refill_0[4]);
      delay(5000);
      Serial.println("arriba");
      servoPose(refill_8[0],refill_8[1],refill_8[2],refill_8[3],refill_8[4]);
      delay(5000);

      paintCount = random(3)+1;

      if(paintCount == 1) {
        servoPose(refill_1[0],refill_1[1],refill_1[2],refill_1[3],refill_1[4]);
      }
      if(paintCount == 2) {
        servoPose(refill_2[0],refill_2[1],refill_2[2],refill_2[3],refill_2[4]);
      }
      if(paintCount == 3) {
        servoPose(refill_3[0],refill_3[1],refill_3[2],refill_3[3],refill_3[4]);
      }
      if(paintCount == 4) {
        servoPose(refill_4[0],refill_4[1],refill_4[2],refill_4[3],refill_4[4]);
      }
      if(paintCount == 5) {
        servoPose(refill_5[0],refill_5[1],refill_5[2],refill_5[3],refill_5[4]); 
      }
      if(paintCount == 6) {
        servoPose(refill_6[0],refill_6[1],refill_6[2],refill_6[3],refill_6[4]); 
      }
      if(paintCount == 7) {
        servoPose(refill_7[0],refill_7[1],refill_7[2],refill_7[3],refill_7[4]); 
      }
      delay(5000);
      Serial.println("arriba");
      servoPose(refill_8[0],refill_8[1],refill_8[2],refill_8[3],refill_8[4]);
      delay(2000);

      doRefill = 0;
      doPaint = 1;
    }
  }

  if(xStepper.distanceToGo() == 0 && xLast + xInterval < millis()) {
    xNew();
    xTrip++;
    if(xTrip >=2) {
      Serial.println("REFILL");
      doPaint = 0;
      doRefill = 1;
      xStepper.moveTo(0);
      while(xStepper.distanceToGo()!=0) {
        xStepper.run();
      }
    }
  }

  if(yStepper.distanceToGo() == 0 && yLast + yInterval < millis()) {
    yNew();
  }

  if(!doRefill) {
    xStepper.run();
    yStepper.run();
  }
}
