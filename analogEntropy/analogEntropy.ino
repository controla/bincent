
unsigned int a0,a1,a2,a3,a4,a5;
unsigned int out;

void setup() {
	Serial.begin(115200);
}

void loop() {

	chaos();
	
	if(toss()) {
		Serial.println("yey");
	} else {
		Serial.println("nay");
	}

	delay(100);
}

bool toss() {
	return random(2);
}

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