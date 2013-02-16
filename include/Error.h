
#pragma once

#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <math.h>

using namespace std;

//Define all conditions to be evaluated, assign codes

/*-------KEY------------------------------------

Condition Word Format - %1_%2, where

$1 Stands for::
M_ -> Motors 
P_ -> Threads
S_ -> Sensors
C_ -> Communications
P_ -> Programming
I_ -> OpenCV, image processing

$2 Stands for single word descriptor

-----------------------------------------------*/

#define P_START 0
#define P_END	1


struct condition {
		const char *success;
		const char *failure;
};

const struct condition conditions[] = {
{"Program has started successfully","Program failed to start"},						//P_START
{"Terminating the program with success","Failed to terminate the program"}			//P_END
};


class Error {
	public:
	Error();
	void log (int returnedVal, int successVal, int condition_num);
	void logComment (char *comment);
	
	private:
	

};

#endif
