#pragma once

#ifndef MOTION_H
#define MOTION_H

#include <Wolfbot.h>


#define freq_motors 100
#define freq_tilt 50
#define calDutyNs(deg) (1890000.0 - 5750.0 * deg)

class Motors{

	public:
	Motors(void);
	void setMotorHeading(float heading);
	void stop();
	void disable();
	void enable();
//	void moveRandom(int iterations);
//	void moveOnHeading(float heading, int time_ms);
	void tiltDeg (float deg);
	private:
	void setMotorPWM(int motor, int duty);
	void setGPIO(int gpioNumber, bool value);
};



#endif

