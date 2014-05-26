#include <AccelStepper.h>
#include <CwPololuSerialServo.h>

// SSC channel-servo mappings
#define SSC_BAS 7
#define SSC_SHL 6
#define SSC_ELB 5
#define SSC_WRI 4
#define SSC_FIN 3

// SSC reset pin
#define SSC_RST 20

// servo speed
#define SSPEED 5 

// X - Y motors dir / step pins
#define X_MOT_DIR 8
#define X_MOT_STP 9
#define Y_MOT_DIR 7
#define Y_MOT_STP 6

// endstops pins
#define X_END 48
#define Y_END 50

#define X_LEN 5300
#define Y_LEN 13300 // full range is 13300

#define X_OFFSET 300
#define Y_OFFSET 4000

#define INC_SIZE 5 // array size (arm segments)
#define X_REF_SIZE 7 // array size (paint buckets)

unsigned int a0,a1,a2,a3,a4,a5;
unsigned int out;

int last_rPos = 0;
int cycleCount = 0;
	
bool moveCanvas = 0;

int xMotorSpeed = 200;
int xMotorAccel = 200;

int yMotorSpeed = 200;
int yMotorAccel = 100;

CwPololuSerialServo servos(&Serial1, 38000);

AccelStepper xStepper(1,X_MOT_STP,X_MOT_DIR);
AccelStepper yStepper(1,Y_MOT_STP,Y_MOT_DIR);

int incoming[INC_SIZE];

int xTotal = 0;
int thisX = 0;
int xClean = 0;
int xRefill[7] = {700,1450,2170,2870,3560,4280,5000};

int servo_park[5] = {2930,2950,2920,2920,2950};

int paint0[INC_SIZE] = {2930,1800,2920,2920,2950};
int paint1[INC_SIZE] = {2930,1500,4860,2930,4200};
int paint2[INC_SIZE] = {2930,1690,4860,2930,4000};

int armRefill0[INC_SIZE] = {2930,2950,2900,2920,2000}; // ok
int armRefill1[INC_SIZE] = {2930,2950,2100,2920,2100}; // ok
int armRefill2[INC_SIZE] = {2930,2900,2200,2920,1100};
int armRefill3[INC_SIZE] = {2930,2600,2300,2920,1200}; // not
int armRefill4[INC_SIZE] = {2930,2400,2400,2920,1300};
int armRefillClean[INC_SIZE] = {2930,2600,2400,2920,1400};

int cleanClean0[INC_SIZE] = {3050,2700,2700,2920,1800};
int cleanClean1[INC_SIZE] = {3050,2700,2700,2920,1500};
void chaos() {
	a0 = analogRead(0);
	a1 = analogRead(1);
	a2 = analogRead(2);
	a3 = analogRead(3);
	a4 = analogRead(4);
	a5 = analogRead(5);

	out = a0 + a1 + a2 + a3 + a4 + a5;
	
	randomSeed(out/6);
	Serial.print("Chaos was: ");
	Serial.println(out/6);
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
	servos.setSpeed(SSC_FIN,SSPEED);
	servos.setSpeed(SSC_WRI,SSPEED);
	servos.setSpeed(SSC_ELB,SSPEED);
	servos.setSpeed(SSC_SHL,SSPEED);
	servos.setSpeed(SSC_BAS,SSPEED);
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
}

void setup() {
	Serial.begin(115200);

	delay(500);
	Serial.println("");
	Serial.println("*************************");
	Serial.println("**** Bin¢ent II v0.9 ****");
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

	// LTOR

	for(int y = Y_OFFSET; y < Y_LEN; y+=random(150)+50) { // advance Y axis in 50~199 increments for each row
		yStepper.moveTo(y);
		Serial.print("move Y to: ");
		Serial.print(y);

		while(yStepper.distanceToGo() != 0) {
			yStepper.run();
		}
		Serial.println(" [DONE]");

		while(xTotal < X_LEN) { // LTOR
			refill(random(X_REF_SIZE));
			Serial.print("continue from ");
			Serial.print(xTotal);
			xStepper.moveTo(xTotal);
			while(xStepper.distanceToGo() != 0) {
				xStepper.run();
			}
			Serial.println(" [DONE]");

			paintIn();
			Serial.println("brush on");

			int thisX = X_LEN; // XLEN - 100...
//			int thisX = random(X_LEN - xTotal - 400) + xTotal + 400; // minimum 400 steps X stroke
			xStepper.moveTo(thisX);
			Serial.print("move X to: ");
			Serial.print(thisX);
			while(xStepper.distanceToGo() != 0) {
				xStepper.run();
			}
			Serial.println(" [DONE]");

			paintOut();
			Serial.println("brush off");

			xTotal = xTotal + thisX;
		}
		Serial.println("end X cycle.");
		Serial.println("reset X");
		xTotal = X_OFFSET;
	}


	// RTOL

/*
	xTotal = X_LEN;
	for(int y = Y_OFFSET+150; y < Y_LEN; y+=300) { // start with 150+offset (do the inbetweens) advance Y axis in 200 increments for each row
		yStepper.moveTo(y);
		Serial.print("*** move Y to: ");
		Serial.print(y);

		while(yStepper.distanceToGo() != 0) {
			yStepper.run();
		}
		Serial.println(" [DONE]");

		while(xTotal > X_OFFSET) { // RTOL
			Serial.print("xTotal is: ");
			Serial.println(xTotal);

			thisX = random(xTotal - 400) + 400; // minimum 400 steps X stroke
			Serial.print("random thisX: ");
			Serial.println(thisX);

			thisX = xTotal - thisX;

			Serial.print("X will move to: ");
			Serial.println(thisX);

			if (thisX < X_OFFSET) {
				Serial.println("***** breaking out...");
				break;
			}

			refill(random(X_REF_SIZE));
			Serial.print("continue from ");
			Serial.print(xTotal);
			xStepper.moveTo(xTotal);
			while(xStepper.distanceToGo() != 0) {
				xStepper.run();
			}
			Serial.println(" [DONE]");

			Serial.print("xTotal: ");
			Serial.println(xTotal);
			
			paintIn();
			Serial.println("brush on");

			xStepper.moveTo(thisX);
			Serial.print("* move X to: ");
			Serial.print(thisX);
			while(xStepper.distanceToGo() != 0) {
				xStepper.run();
			}
			Serial.println(" [DONE]");

			paintOut();
			Serial.println("brush off");

			xTotal = thisX - random(X_OFFSET);
			delay(4000); // cut this off
			Serial.println("-------");
		}
		Serial.println("********* finish X stroke");
		xTotal = X_LEN; // back to the start
	}

	while(true) {
		Serial.println("the art is done.");
		delay(5000);
	}
*/
}
