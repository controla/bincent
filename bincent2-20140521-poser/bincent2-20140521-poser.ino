#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

// SSC (servo pins on SSC)
#define SSC_BAS 7
#define SSC_SHL 6
#define SSC_ELB 5
#define SSC_WRI 3
#define SSC_FIN 2

#define SSC_RST 20 // reset from arduino

// X - Y motors (pwm and dir from arduino)
#define X_MOT_DIR 8
#define X_MOT_STP 9
#define Y_MOT_DIR 7
#define Y_MOT_STP 6

// endstops (digital on arduino)
#define X_END 48
#define Y_END 50

#define X_LEN 10000
#define Y_LEN 13300 // full range is 13300

#define X_OFFSET 400 // safe start
#define Y_OFFSET 100

#define X_MOT_SPD 1500
#define X_MOT_ACC 3000
#define Y_MOT_SPD 1000
#define Y_MOT_ACC 1000

#define INC_SIZE 5 // number of elements for the position arrays?
#define X_REF_SIZE 7

unsigned int a0,a1,a2,a3,a4,a5; // analog values for chaos init
unsigned int out; // sum of all fears.

int lastX = 200; // initial position
int lastY = 200;
int last_rPos = 0; // ???

boolean rToss = 0; // the coin
int cycleCount = 0; // for stats

CwPololuSerialServo servos(&Serial1, 38000);

AccelStepper xStepper(1,X_MOT_STP,X_MOT_DIR);
AccelStepper yStepper(1,Y_MOT_STP,Y_MOT_DIR);


int xRefill[7] = {700,1450,2170,2870,3560,4280,5000}; // refill location

// poses

int servo_park[INC_SIZE] = {2930,2950,2920,2920,2950};

int paint0[INC_SIZE] = {2930,1800,2920,2920,2950};
int paint1[INC_SIZE] = {2930,1500,4860,2930,4200};
int paint2[INC_SIZE] = {2930,1690,4860,2930,4000};

int armRefill0[INC_SIZE] = {2930,2950,2900,2920,2000};
int armRefill1[INC_SIZE] = {2930,2950,2100,2920,2100};
int armRefill2[INC_SIZE] = {2930,2900,2200,2920,1100};
int armRefill3[INC_SIZE] = {2930,2600,2300,2920,1200};
int armRefill4[INC_SIZE] = {2930,2400,2400,2920,1300};
int armRefillClean[INC_SIZE] = {2930,2600,2400,2920,1400};

int cleanClean0[INC_SIZE] = {3050,2700,2700,2920,1800};
int cleanClean1[INC_SIZE] = {3050,2700,2700,2920,1500};


// all functions

void chaos() { // generate a good random seed
	delay(20);
	a0 = analogRead(0);
	delay(20);
	a1 = analogRead(1);
	delay(20);
	a2 = analogRead(2);
	delay(20);
	a3 = analogRead(3);
	delay(20);
	a4 = analogRead(4);
	delay(20);
	a5 = analogRead(5);

	out = a0 + a1 + a2 + a3 + a4 + a5;
	
	randomSeed(out/6);
	Serial.print("Chaos was: ");
	Serial.println(out/6);
}

bool toss() { // a coin, output is a random binary.
	return random(2);
}

void servoReset() { // reset SSC
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

void xMove(int dest) {
	Serial.print("moving X to: ");
	Serial.print(dest);

	xStepper.moveTo(dest);
	while(xStepper.distanceToGo() != 0) {
		xStepper.run();
	}
	Serial.println(" [DONE]");
	lastX = dest;
}

void yMove(int dest) {
	Serial.print("moving Y to: ");
	Serial.print(dest);

	yStepper.moveTo(dest);
	while(yStepper.distanceToGo() != 0) {
		yStepper.run();
	}
	Serial.println(" [DONE]");
	lastY = dest;
}

void paintIn() {
	Serial.println("I'm painting...");
	Serial.print("p0,");
	servoPose(paint0);
	delay(6000);
	Serial.println("p1,");
	servoPose(paint1);
	delay(4000);
	Serial.println("p2,");
	servoPose(paint2);
	delay(5000);
}

void paintOut() {
	Serial.println(" ... I'm off.");
	Serial.print("p1");
	servoPose(paint1);
	delay(2000);
	Serial.print("p2");
	servoPose(paint0);
	delay(6000);
	Serial.println("p");
	servoPose(servo_park);
	delay(5000);
}

void clean() {
	Serial.println("I'm cleaning...");
	zeroX();
	Serial.print("r0,");
	servoPose(armRefill0);
	delay(3000);
	Serial.print("r1,");
	servoPose(armRefill1);
	delay(3000);
	Serial.print("r2,");
	servoPose(armRefill2);
	delay(2000);

	// good ol' in-out-in-out...
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("rC,");
	servoPose(armRefillClean);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("rC,");
	servoPose(armRefillClean);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("rC,");
	servoPose(armRefillClean);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("rC,");
	servoPose(armRefillClean);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("rC,");
	servoPose(armRefillClean);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r2,");
	servoPose(armRefill2);
	delay(10000); // drip
	
	Serial.print("r1,");
	servoPose(armRefill1);
	delay(3000);
	
	// pañito
	Serial.print("c0,");
	servoPose(cleanClean0);
	delay(2000);
	Serial.print("c1,");
	servoPose(cleanClean1);
	delay(2000);
	Serial.print("c0,");
	servoPose(cleanClean0);
	delay(2000);
	
	// and up.
	Serial.print("r1,");
	servoPose(armRefill1);
	delay(3000);
	Serial.print("r0,");
	servoPose(armRefill0);
	delay(3000);
	Serial.println("done cleaning.");
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

	Serial.print("r0,");
	servoPose(armRefill0);
	delay(3000);
	Serial.print("r1,");
	servoPose(armRefill1);
	delay(3000);
	Serial.print("r2,");
	servoPose(armRefill2);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r4,");
	servoPose(armRefill4);
	delay(2000);
	Serial.print("r3,");
	servoPose(armRefill3);
	delay(2000);
	Serial.print("r2,");
	servoPose(armRefill2);
	delay(3000);
	Serial.print("r1,");
	servoPose(armRefill1);
	delay(3000);
	Serial.print("r0,");
	servoPose(armRefill0);
	delay(3000);
	Serial.println("done refilling.");
}

void newTrace(boolean dir) {
	
	paintIn();
	if(dir) {
		xMove(random(X_LEN));
	} else {
		yMove(random(Y_LEN));
	}
	paintOut();
}

void setup() {
	Serial.begin(115200);

	delay(500);
	Serial.println("");
	Serial.println("*************************");
	Serial.println("**** Bin¢ent II v0.8 ****");
	Serial.println("*************************");
	
	pinMode(SSC_RST, OUTPUT);

	servoReset();
	servoSpeed();
	servoPose(servo_park);

	xStepper.setMaxSpeed(X_MOT_SPD);
	xStepper.setAcceleration(X_MOT_ACC);
	yStepper.setMaxSpeed(Y_MOT_SPD);
	yStepper.setAcceleration(Y_MOT_ACC);

	chaos();
	zeroX();
	zeroY();
//	clean();

	// but you are already in this world
	lastX = X_LEN/2;
	lastY = Y_LEN/2;
	rToss = 1;
}

void loop() {




/*
	xMove(X_LEN);
	delay(1500);
	xMove(X_OFFSET);
	delay(1500);

	yMove(Y_LEN);
	delay(1500);
	yMove(Y_OFFSET);
	delay(1500);
*/

/*
	if(rToss) {
		Serial.println("** refilling");
		refill(random(X_REF_SIZE));
		Serial.print("moving back to ");
		Serial.print(lastX);
		Serial.print(",");
		Serial.println(lastY);
		xMove(lastX);
		yMove(lastY);
	}
	
	newTrace(toss());

	cycleCount++;
	while(cycleCount > 666) {
		Serial.println("the art is done.");
		delay(10000);
	}

	rToss = toss();
*/
}
