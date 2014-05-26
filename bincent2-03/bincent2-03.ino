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

#define X_LEN 5350
#define Y_LEN 11000 // full range is 13200

#define X_OFFSET 400
#define Y_OFFSET 200

#define INC_SIZE 5
#define X_REF_SIZE 7

unsigned int a0,a1,a2,a3,a4,a5;
unsigned int out;

int last_rPos = 0;
int cycleCount = 0;

bool moveCanvas = 0;

int xMotorSpeed = 100;
int xMotorAccel = 100;

int yMotorSpeed = 200;
int yMotorAccel = 200;

CwPololuSerialServo servos(&Serial1, 38000);

AccelStepper xStepper(1,X_MOT_STP,X_MOT_DIR);
AccelStepper yStepper(1,Y_MOT_STP,Y_MOT_DIR);

int incoming[INC_SIZE];

int servo_park[5] = {2930,2950,2920,2920,2950};

int xClean = 0;
int xRefill[7] = {700,1450,2170,2870,3560,4280,5000};

int paint0[INC_SIZE] = {2930,3800,2050,2930,2930};
int paint1[INC_SIZE] = {2930,3800,2050,1240,4800};
int paint2[INC_SIZE] = {2930,4200,1700,1240,4800};

int armRefill0[INC_SIZE] = {2930,2950,2900,2920,2000};
int armRefill1[INC_SIZE] = {2930,2950,2100,2920,2100};
int armRefill2[INC_SIZE] = {2930,2200,2800,2920,1100};
int armRefill3[INC_SIZE] = {2930,2000,2900,2920,1100};
int armRefill4[INC_SIZE] = {2930,1600,2800,2920,1100};

int cleanClean0[INC_SIZE] = {2500,2950,2100,2920,2100};
int cleanClean1[INC_SIZE] = {2500,2950,2100,2920,2100};

void chaos() {
	a0 = analogRead(0);
	a1 = analogRead(1);
	a2 = analogRead(2);
	a3 = analogRead(3);
	a4 = analogRead(4);
	a5 = analogRead(5);

	out = a0 + a1 + a2 + a3 + a4 + a5;
	
	randomSeed(out/6);
}

bool toss() {
	return random(2);
}


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
	Serial.println("I'm painting...");

	Serial.println("paint0");
	servoPose(paint0);
	delay(6000);
	Serial.println("paint1");
	servoPose(paint1);
	delay(2000);
	Serial.println("paint2");
	servoPose(paint2);
	delay(3000);

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

void clean() {
	Serial.println("I'm cleaning...");
	zeroX();
	Serial.println("refill0");
	servoPose(armRefill0);
	delay(3000);
	Serial.println("refill1");
	servoPose(armRefill1);
	delay(3000);
	Serial.println("refill2");
	servoPose(armRefill2);
	delay(2000);

	// good ol' in-out-in-out...
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill2");
	servoPose(armRefill2);
	delay(10000);
	
	// pañito
	Serial.println("clean0");
	servoPose(cleanClean0);
	delay(5000);
	Serial.println("clean1");
	servoPose(cleanClean1);
	delay(5000);
	Serial.println("clean0");
	servoPose(cleanClean0);
	delay(5000);
	Serial.println("clean1");
	servoPose(cleanClean1);
	delay(5000);
	Serial.println("clean0");
	servoPose(cleanClean0);
	delay(5000);

	// and up.
	Serial.println("refill1");
	servoPose(armRefill1);
	delay(3000);
	Serial.println("refill0");
	servoPose(armRefill0);
	delay(3000);
}

void refill(int rPos) {
	servoPose(servo_park);
	if(rPos != last_rPos) {
		clean();
	}
	last_rPos = rPos;
	Serial.print("Refill with ");
	Serial.print(rPos);
	Serial.print(", moving X axis to ");
	Serial.println(xRefill[rPos]);
	xStepper.moveTo(xRefill[rPos]);
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
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill4");
	servoPose(armRefill4);
	delay(2000);
	Serial.println("refill3");
	servoPose(armRefill3);
	delay(2000);
	Serial.println("refill2");
	servoPose(armRefill2);
	delay(3000);
	Serial.println("refill1");
	servoPose(armRefill1);
	delay(3000);
	Serial.println("refill0");
	servoPose(armRefill0);
	delay(3000);
}

void randomPos() {

	moveCanvas = 1;

	int xNew = random(X_LEN - X_OFFSET) + X_OFFSET;
	int yNew = random(Y_LEN - Y_OFFSET) + Y_OFFSET;

	xStepper.moveTo(xNew);
	yStepper.moveTo(yNew);
	Serial.print("moving X to ");
	Serial.print(xNew);
	Serial.print(" & Y to ");
	Serial.print(yNew);
	while(moveCanvas == 1) {
		if(xStepper.distanceToGo() == 0 && yStepper.distanceToGo() == 0) {
			moveCanvas = 0;
		} else {
			xStepper.run();
			yStepper.run();
		}
	}
	Serial.println(" ---> [DONE]");

	// while(xStepper.distanceToGo() != 0 && yStepper.distanceToGo() != 0) {}
	
}

void setup() {
	Serial.begin(115200);

	delay(500);
	Serial.println("");
	Serial.println("*************************");
	Serial.println("**** Bin¢ent II v0.3 ****");
	Serial.println("*************************");
	
	pinMode(SSC_RST, OUTPUT);

	servoReset();
	servoSpeed();
	servoPose(servo_park);

	xStepper.setMaxSpeed(xMotorSpeed);
	xStepper.setAcceleration(xMotorAccel);
	yStepper.setMaxSpeed(yMotorSpeed);
	yStepper.setAcceleration(yMotorAccel);

	chaos();
	zeroX();
	zeroY();
	clean();

}

void loop() {

	refill(random(X_REF_SIZE));

	randomPos();

	paint();

	randomPos();

	paint();

	xStepper.run();
	yStepper.run();

	cycleCount++;
	if(cycleCount >= 10) {
		zeroX();
		zeroY();
		cycleCount = 0;
	}
}
