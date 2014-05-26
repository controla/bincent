#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

// SSC
#define SSC_BAS 7
#define SSC_SHL 6
#define SSC_ELB 5
#define SSC_WRI 3
#define SSC_FIN 2

#define SSC_RST 20

// X - Y motors
#define X_MOT_DIR 8
#define X_MOT_STP 9
#define Y_MOT_DIR 7
#define Y_MOT_STP 6

// endstops
#define X_END 48
#define Y_END 50

#define X_LEN 5300
#define Y_LEN 13000

#define X_OFFSET 300
#define Y_OFFSET 4000

#define INC_SIZE 5
#define X_REF_SIZE 6

int cycleCount = 0;

int xMotorSpeed = 1500;
int xMotorAccel = 3000;

int yMotorSpeed = 1000;
int yMotorAccel = 2000;

long xLast = 0;
long yLast = 0;
long xWait = 30000;
long yWait = 30000;

String errorMsg;
bool error = 0;

CwPololuSerialServo servos(&Serial1, 38000);

AccelStepper xStepper(1,X_MOT_STP,X_MOT_DIR);
AccelStepper yStepper(1,Y_MOT_STP,Y_MOT_DIR);

int incoming[INC_SIZE];

int servo_park[INC_SIZE] = {2930,2950,2920,2920,2950};

void servoReset() {
	digitalWrite(SSC_RST, LOW);
	delay(200);
	digitalWrite(SSC_RST, HIGH);
	delay(500);
}

void servoSpeed() {
	servos.setSpeed(SSC_FIN,5);
	servos.setSpeed(SSC_WRI,5);
	servos.setSpeed(SSC_ELB,5);
	servos.setSpeed(SSC_SHL,5);
	servos.setSpeed(SSC_BAS,5);
	delay(500);
}

void servoPose(int* pose) {
	servos.setSpeed(SSC_BAS,5);
	servos.setPositionAbsolute(SSC_BAS,pose[0]);
	servos.setSpeed(SSC_SHL,5);
	servos.setPositionAbsolute(SSC_SHL,pose[1]);
	servos.setSpeed(SSC_ELB,10);
	servos.setPositionAbsolute(SSC_ELB,pose[2]);
	servos.setSpeed(SSC_WRI,10);
	servos.setPositionAbsolute(SSC_WRI,pose[3]);
	servos.setSpeed(SSC_FIN,10);
	servos.setPositionAbsolute(SSC_FIN,pose[4]);
}


void zeroX() {
	Serial.print("zeroing X axis...");
	xStepper.moveTo(-20000);
	while(!digitalRead(X_END)) {
		xStepper.run();
	}
	xStepper.stop();
	Serial.println(" [DONE]");
	delay(100);
	xStepper.setCurrentPosition(0);
	xStepper.moveTo(0);
}

void zeroY() {
	Serial.print("zeroing Y axis...");
	yStepper.moveTo(-20000);
	while(!digitalRead(Y_END)) {
		yStepper.run();
	}
	yStepper.stop();
	Serial.println(" [DONE]");
	delay(100);
	yStepper.setCurrentPosition(0);
	yStepper.moveTo(0);
}

void setup() {

	randomSeed(analogRead(4)); // reshuffle

	Serial.begin(115200);
	delay(500);
	Serial.println("");
	Serial.println("");
	Serial.println("**********************");
	Serial.println("**** Bincent tools ***");
	Serial.println("*****  ARMACOORD *****");
	Serial.println("**********************");
	
	pinMode(SSC_RST, OUTPUT);

	servoReset();
	servoSpeed();
	servoPose(servo_park);

	xStepper.setMaxSpeed(xMotorSpeed);
	xStepper.setAcceleration(xMotorAccel);
	yStepper.setMaxSpeed(yMotorSpeed);
	yStepper.setAcceleration(yMotorAccel);

	zeroX();
	zeroY();

}

void loop() {

	if(Serial.available()) {

		int xDest = Serial.parseInt();
		int yDest = Serial.parseInt();
		incoming[0] = Serial.parseInt();	
		incoming[1] = Serial.parseInt();
		incoming[2] = Serial.parseInt();
		incoming[3] = Serial.parseInt();
		incoming[4] = Serial.parseInt();

		servos.setSpeed(SSC_BAS,5);
		servos.setPositionAbsolute(SSC_BAS,pose[0]);

		xStepper.moveTo(xDest);
		yStepper.moveTo(yDest);

		servoPose(incoming);
	}

	xStepper.run();
	yStepper.run();

}
