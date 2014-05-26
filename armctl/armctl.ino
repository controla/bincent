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

int xMotorSpeed = 100;
int xMotorAccel = 100;

int yMotorSpeed = 200;
int yMotorAccel = 200;

AccelStepper xStepper(1, X_MOT_STP, X_MOT_DIR);
AccelStepper yStepper(1, Y_MOT_STP, Y_MOT_DIR);

CwPololuSerialServo servos(&Serial1, 38000);

int pose_park[5] = {3000,3000,3000,3000,3000};

int refill_pre0[5] = {1200,3500,2000,3000,3000};
int refill_pre1[5] = {1200,4500,4000,3000,2900};

int refill_s0[5] = {1800,4500,4000,3000,2900};
int refill_s1[5] = {1330,4400,4000,3000,3400};
int refill_s2[5] = {1180,4400,4000,3000,3400};
int refill_s3[5] = {1050,4400,4000,2700,3250};
int refill_s4[5] = {1320,4400,4000,3000,3800};
int refill_s5[5] = {1100,4200,3500,3000,3200};
int refill_s6[5] = {1000,4200,3500,2700,3200};
int refill_s7[5] = {1000,4400,4600,2500,3100};
int refill_s8[5] = {1000,3000,3000,3000,2900};

int paint_clean1[] = {1500,3500,3000,3000,3000};
int paint_clean2[] = {1500,3500,3000,3000,3000};

int paint_0[5] = {2780,3830,2500,3500,3400};
int paint_1[5] = {2800,3150,3300,3000,3200};
int paint_2[5] = {2930,3060,3470,2800,2923};
int paint_3[5] = {3100,3130,3610,3000,3000};
int paint_4[5] = {2900,3130,3776,3200,3350};
int paint_5[5] = {3200,3500,3100,2500,3100};
int paint_6[5] = {3000,2900,3000,3000,3000};
int paint_7[5] = {3000,2000,4000,3000,3000};

void servoReset() {
  digitalWrite(SSC_RST, LOW);
  delay(200);
  digitalWrite(SSC_RST, HIGH);
  delay(500);
}

void servo_speed() {
  servos.setSpeed(SSC_FIN,5);
  servos.setSpeed(SSC_WRI,5);
  servos.setSpeed(SSC_ELB,5);
  servos.setSpeed(SSC_SHL,5);
  servos.setSpeed(SSC_BAS,5);
  delay(500);
}

void servoPose(int* angles) {
  servos.setPositionAbsolute(SSC_BAS,angles[0]);
  servos.setPositionAbsolute(SSC_SHL,angles[1]);
  servos.setPositionAbsolute(SSC_ELB,angles[2]);
  servos.setPositionAbsolute(SSC_WRI,angles[3]);
  servos.setPositionAbsolute(SSC_FIN,angles[4]);
}

void zeroX() {
  xStepper.setSpeed(500);
  xStepper.moveTo(-20000);
  while(digitalRead(X_END) == 0) {
    xStepper.run();
  }
  xStepper.stop();
  delay(500);
  xStepper.setCurrentPosition(0);
}

void zeroY() {
  yStepper.setSpeed(500);
  yStepper.moveTo(-20000);
  while(digitalRead(Y_END) == 0) {
    yStepper.run();
  }
  yStepper.stop();
  delay(500);
  yStepper.setCurrentPosition(0);
}

void service_park() {
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(1000);
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(2000);
}

void service_rinse() {
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(2000);
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(2000);
}

void service_dry() {
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(2000);
  servos.setPositionAbsolute(SSC_SHL,3000);
  delay(2000);
}

void clean() {
  service_rinse();
  service_park();
  service_dry();
  service_park();
  delay(2000);
}

void colorPick() {}

void doRefill() {
  servoPose(pose_park);
  delay(3000);
  zeroX();

  clean();
  servoPose(refill_pre0);
  delay(3000);
  servoPose(pose_park);
  delay(3000);
  colorPick();
}

void doPaint() {

}

void setup() {

  randomSeed(analogRead(4));
  Serial.begin(115200);

  pinMode(SSC_RST, OUTPUT);

  servoReset();
  servo_speed();
  servoPose(pose_park);
  delay(2000);

  zeroX();
  zeroY();
}

void loop() {

  doRefill();
  doPaint();

}
