#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

// SSC
#define SSC_BAS 7
#define SSC_SHL 6
#define SSC_ELB 5
#define SSC_WRI 4
#define SSC_FIN 3

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
#define Y_LEN 13500

#define X_OFFSET 300
#define Y_OFFSET 4500

#define INC_SIZE 5
#define X_REF_SIZE 6

int cycleCount = 0;

int xMotorSpeed = 100;
int xMotorAccel = 100;

int yMotorSpeed = 200;
int yMotorAccel = 200;

CwPololuSerialServo servos(&Serial1, 38000);

AccelStepper xStepper(1,X_MOT_STP,X_MOT_DIR);
AccelStepper yStepper(1,Y_MOT_STP,Y_MOT_DIR);

int incoming[INC_SIZE];

int servo_park[5] = {2930,2950,2920,2920,2950};

int xRefill[6] = {980,1850,2650,3540,4350,5250};

int paint0[INC_SIZE] = {2930,3800,2050,2930,2930};
int paint1[INC_SIZE] = {2930,3800,2050,1240,4800};
int paint2[INC_SIZE] = {2930,4200,1700,1240,4800};

int armRefill0[INC_SIZE] = {2930,2950,2900,2920,2000};
int armRefill1[INC_SIZE] = {2930,2950,2100,2920,2100};
int armRefill2[INC_SIZE] = {2930,2200,2800,2920,1200};
int armRefill3[INC_SIZE] = {2930,2000,2900,2920,1200};
int armRefill4[INC_SIZE] = {2930,1800,2900,2920,1200};
int armRefill5[INC_SIZE] = {2930,1750,2900,2920,1200};
int armRefill6[INC_SIZE] = {2930,1750,2900,2920,1200};

void servoReset() {
	digitalWrite(SSC_RST, LOW);
	delay(200);
	digitalWrite(SSC_RST, HIGH);
	delay(500);
}

void servoSpeed() {
	servos.setSpeed(SSC_FIN,3);
	servos.setSpeed(SSC_WRI,3);
	servos.setSpeed(SSC_ELB,3);
	servos.setSpeed(SSC_SHL,3);
	servos.setSpeed(SSC_BAS,3);
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
	Serial.print("zeroing X axis...");
	xStepper.moveTo(-20000);
	while(!digitalRead(X_END)) {
		xStepper.run();
	}

	xStepper.stop();
	Serial.println(" [DONE]");
	delay(500);
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
	delay(500);
	yStepper.setCurrentPosition(0);
	yStepper.moveTo(0);
}

void paint() {

	Serial.println("Now painting...");

	Serial.println("paint0");
	servoPose(paint0);
	delay(6000);
	Serial.println("paint1");
	servoPose(paint1);
	delay(2000);
	Serial.println("paint2");
	servoPose(paint2);
	delay(2000);

	randomPos();

	Serial.println("paint1");
	servoPose(paint1);
	delay(2000);
	Serial.println("paint0");
	servoPose(paint0);
	delay(6000);
	Serial.println("park");
	servoPose(servo_park);
	delay(5000);

}

void refill(int xPos) {
	servoPose(servo_park);
	Serial.print("Refill with ");
	Serial.print(xPos);
	Serial.print(", moving X axis to ");
	Serial.println(xRefill[xPos]);
	xStepper.moveTo(xRefill[xPos]);
	while(xStepper.distanceToGo() != 0) {
		xStepper.run();
	}

	Serial.println("refill0");
	servoPose(armRefill0);
	delay(3000);
	Serial.println("refill1");
	servoPose(armRefill1);
	delay(3000);
	Serial.println("refill2");
	servoPose(armRefill2);
	delay(5000);
	Serial.println("refill0");
	servoPose(armRefill0);
	delay(5000);
}

void randomPos() {
	int xNew = random(X_LEN - X_OFFSET) + X_OFFSET;
	int yNew = random(Y_LEN - Y_OFFSET) + Y_OFFSET;

	xStepper.moveTo(xNew);
	yStepper.moveTo(yNew);
	Serial.print("moving X to ");
	Serial.print(xNew);
	Serial.print(" & Y to ");
	Serial.print(yNew);
	while(xStepper.distanceToGo() != 0 && yStepper.distanceToGo() != 0) {
		xStepper.run();
		yStepper.run();
	}
	Serial.println(" ---> [DONE]");
}

void setup() {

	randomSeed(analogRead(4)); // reshuffle

	Serial.begin(115200);
	delay(500);
	Serial.println("");
	Serial.println("");
	Serial.println("**********************");
	Serial.println("**** Bincent tools ***");
	Serial.println("** refill positions **");
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

	refill(random(X_REF_SIZE));

	xStepper.run();
	yStepper.run();

	cycleCount++;

	if(cycleCount >= 5) {
		zeroX();
		zeroY();
		cycleCount = 0;
	}
}