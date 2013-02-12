
#ifndef MOTION_H
#define MOTION_H

#include <wolfbot.h>

class motors{

	public:
	motors(void);
//	void moveRandom(int iterations);
//	void moveOnHeading(float heading, int time_ms);
	void setMotorHeading(float heading);
	private:
	void setMotorPWM(int motor, int duty);
	void setGPIO(int gpioNumber, bool value);
}

#endif

