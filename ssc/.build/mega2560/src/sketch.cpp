#include <Arduino.h>
void setup();
void loop();
void set_arm( float x, float y, float z, float grip_angle_d );
void servo_park();
#line 1 "src/sketch.ino"
#include <CwPololuSerialServo.h>

/* Arm dimensions( mm ) */
#define BASE_HGT 45.00 // base hight 2.65"
#define HUMERUS 46.85   // shoulder-to-elbow "bone" 5.75"
#define ULNA 40.20      // elbow-to-wrist "bone" 7.375"
#define GRIPPER 120.20   // effector length 3.94"
 
#define ftl(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))  //float to long conversion

#define BAS_SERVO 7
#define SHL_SERVO 6
#define ELB_SERVO 5
#define WRI_SERVO 4
#define GRI_SERVO 3

int moveX, moveY, moveZ;

/* pre-calculations */
float hum_sq = HUMERUS*HUMERUS;
float uln_sq = ULNA*ULNA;

CwPololuSerialServo servos(&Serial1, 40000);

void setup() {
  servos.setSpeed(0,10);
  servos.setSpeed(1,10);
  servos.setSpeed(2,10);
  servos.setSpeed(3,10);
  servos.setSpeed(4,10);
  servos.setSpeed(5,8);
  servos.setSpeed(6,5);
  servos.setSpeed(7,8);
  
  servo_park();
  Serial.begin( 115200 );
  delay( 2000 );
  Serial.println("Waiting for commands...");
}

void loop() {
	while(Serial.available() > 0) {
	    int moveX = Serial.parseInt();
	    int moveY = Serial.parseInt();
	    int moveZ = Serial.parseInt();
	    if (Serial.read() == '\n') {
	    	Serial.println("************");
		    Serial.print("new coords: ");
		    Serial.print(moveX);
		    Serial.print(",");
		    Serial.print(moveY);
		    Serial.print(",");
			Serial.println(moveZ);
		    
		    set_arm(moveX,moveY,moveZ,0);
		}
	}
}

/* z is height, y is distance from base center out, x is side to side. y,z can only be positive */
void set_arm( float x, float y, float z, float grip_angle_d ) {

	float grip_angle_r = radians( grip_angle_d );    //grip angle in radians for use in calculations
	/* Base angle and radial distance from x,y coordinates */
	float bas_angle_r = atan2( x, y );
	float rdist = sqrt(( x * x ) + ( y * y ));
	/* rdist is y coordinate for the arm */
	y = rdist;
	/* Grip offsets calculated based on grip angle */
	float grip_off_z = ( sin( grip_angle_r )) * GRIPPER;
	float grip_off_y = ( cos( grip_angle_r )) * GRIPPER;
	/* Wrist position */
	float wrist_z = ( z - grip_off_z ) - BASE_HGT;
	float wrist_y = y - grip_off_y;
	/* Shoulder to wrist distance ( AKA sw ) */
	float s_w = ( wrist_z * wrist_z ) + ( wrist_y * wrist_y );
	float s_w_sqrt = sqrt( s_w );
	/* s_w angle to ground */
	//float a1 = atan2( wrist_y, wrist_z );
	float a1 = atan2( wrist_z, wrist_y );
	/* s_w angle to humerus */
	float a2 = acos((( hum_sq - uln_sq ) + s_w ) / ( 2 * HUMERUS * s_w_sqrt ));
	/* shoulder angle */
	float shl_angle_r = a1 + a2;
	float shl_angle_d = degrees( shl_angle_r );
	/* elbow angle */
	float elb_angle_r = acos(( hum_sq + uln_sq - s_w ) / ( 2 * HUMERUS * ULNA ));
	float elb_angle_d = degrees( elb_angle_r );
	float elb_angle_dn = -( 180.0 - elb_angle_d );
	/* wrist angle */
	float wri_angle_d = ( grip_angle_d - elb_angle_dn ) - shl_angle_d;

	/* Servo pulses // map it instead. */
	
	float bas_servopulse = map(degrees(bas_angle_r),-200,200,1100,4900); //1500.0 - (( degrees( bas_angle_r )) * 11.11 );
	float shl_servopulse = map(-shl_angle_d,-200,200,1100,4900); //1500.0 + (( shl_angle_d - 90.0 ) * 6.6 );
	float elb_servopulse = map(elb_angle_d,-200,200,1100,4900); //1500.0 -  (( elb_angle_d - 90.0 ) * 6.6 );
	float wri_servopulse = map(wri_angle_d,-200,200,1100,4900); //1500 + ( wri_angle_d  * 11.1 );

	Serial.print("base angle: ");
	Serial.println(bas_angle_r);
	Serial.println(bas_servopulse);
  	servos.setPositionAbsolute(BAS_SERVO, constrain(bas_servopulse,1100,4900));

	Serial.print("shoulder angle: ");
	Serial.println(shl_angle_d);
	Serial.println(shl_servopulse);
	servos.setPositionAbsolute(SHL_SERVO, constrain(shl_servopulse,1100,4900));

	Serial.print("elbow angle: ");
	Serial.println(elb_angle_d);
	Serial.println(elb_servopulse);
	servos.setPositionAbsolute(ELB_SERVO, constrain(elb_servopulse,1100,4900));

	Serial.print("wrist angle: ");
	Serial.println(wri_angle_d);
	Serial.println(wri_servopulse);
	servos.setPositionAbsolute(WRI_SERVO, constrain(wri_servopulse,1100,4900));

}

void servo_park()
{
  servos.setPositionAbsolute(BAS_SERVO, 3000 );
  servos.setPositionAbsolute(SHL_SERVO, 3000 );
  servos.setPositionAbsolute(ELB_SERVO, 3000 );
  servos.setPositionAbsolute(WRI_SERVO, 3000 );
  servos.setPositionAbsolute(GRI_SERVO, 3000 );
  return;
}
