#include <CwPololuSerialServo.h>

#define BAS_SERVO 7
#define SHL_SERVO 6
#define ELB_SERVO 5
#define WRI_SERVO 4
#define FIN_SERVO 3

#define X_END 48
#define Y_END 50
#define SSC_RST 20
#define SSC_ 20

CwPololuSerialServo servos(&Serial1, 40000);

int thres = 5000;
int arm = 0;
long lastMove = 0;

void setup() {

  Serial.begin(115200);
  
  pinMode(SSC_RST,OUTPUT);
  
  digitalWrite(SSC_RST,LOW);
  delay(500);
  digitalWrite(SSC_RST,HIGH);
  delay(500);
  
  servos.setSpeed(BAS_SERVO,8);
  servos.setSpeed(SHL_SERVO,8);
  servos.setSpeed(ELB_SERVO,8);
  servos.setSpeed(WRI_SERVO,8);
  servos.setSpeed(FIN_SERVO,8);

  servo_park();

}

void loop() {

    if(digitalRead(Y_END) == 1) && lastMove + thres < millis()) {
      if(arm==0) {
        servos.setPositionAbsolute(BAS_SERVO, 2500 );
        servos.setPositionAbsolute(SHL_SERVO, 4000 );
        servos.setPositionAbsolute(ELB_SERVO, 3500 );
        servos.setPositionAbsolute(WRI_SERVO, 2000 );
        servos.setPositionAbsolute(FIN_SERVO, 2000 );
        lastMove = millis();
        arm = 1;
      } else {
        servo_park();
        lastMove = millis();
        arm = 0;
      }
    }
  
}

void servo_park() {
  servos.setPositionAbsolute(BAS_SERVO, 3000 );
  servos.setPositionAbsolute(SHL_SERVO, 3000 );
  servos.setPositionAbsolute(ELB_SERVO, 3000 );
  servos.setPositionAbsolute(WRI_SERVO, 3000 );
  servos.setPositionAbsolute(FIN_SERVO, 3000 );
  return;
}

