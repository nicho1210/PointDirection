#include <wiringPi.h>
#include <stdio.h>
#include <math.h>

#define Negative  544     // -60 degree = 544us
#define Positive  2400     // +60 degree = 2400us

#define Tunning   2000

#define TotalDelay  20000 	//20ms = 20000us
#define gpioPin 17
#define ToleranceDegree		5.00001	// +/-2 degree


void SetDirection (float CurDegree, float TarDegree) {
	
	unsigned int TargetTime;
	unsigned int i;
	int direction = 0;
	float moveDegree = 0.0;
	
	printf ("Servo Motor\n");
	
	if (wiringPiSetupGpio() == -1) {
		printf ("Failed\n");
		return;
	}
	pinMode (gpioPin, OUTPUT);

	// return if within tolerance value
	if ((TarDegree >= (CurDegree - ToleranceDegree)) && (TarDegree <= (CurDegree + ToleranceDegree))) {
		printf ("TT\n");
		return;
	}

	printf ("TarDeg:%f CurDeg:%f\n", TarDegree, CurDegree);
	
	if (CurDegree > 0.0 && TarDegree < 0.0) {
		moveDegree = fabs(TarDegree) + CurDegree;
		direction = Negative;
	} else if (CurDegree < 0.0 && TarDegree > 0.0) {
		moveDegree = TarDegree + fabs(CurDegree);
		direction = Positive;
	} else {
		moveDegree = TarDegree - CurDegree;
		if (moveDegree > 0) {
			direction = Negative;
		} else {
			direction = Positive;
		}
	}

	printf ("dir:%d m:%f\n", direction, moveDegree);
	
	TargetTime = micros() +  Tunning * fabs(moveDegree);
	
	for (i = 0; micros() < TargetTime; i++) {
		digitalWrite (gpioPin, HIGH);
		delayMicroseconds (direction);
		digitalWrite (gpioPin, LOW);
		delayMicroseconds (TotalDelay - direction);
	}
	
	return;
}
