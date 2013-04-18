
#pragma once

#ifndef ERROR_H
#define ERROR_H

#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>

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

For the project, for any funcrion, 0 means failure, 
1 means success
-----------------------------------------------*/

#define P_START	0
#define P_Threads	1
#define P_END		2
#define P_ConsoleIO	3
#define M_Start	4
#define M_Stop	5
#define S_Acc		6
#define S_Light	7
#define S_Dms		8
#define C_Zigbee	9
#define C_IR		10
#define C_WLAN	11
#define G_Error	12
#define G_Log		13
#define P_Motors	14
#define P_Talker	15
#define P_Tilt	16
#define S_Obs		17
#define P_Com		18
#define S_Bound	19
#define P_Obs		20
#define P_Bro		21
#define P_Sen		22
#define P_Xb		23
#define P_IR		24

struct condition {
		const char *success;
		const char *failure;
};

const struct condition conditions[] = {
{"Program has started successfully","Program failed to start"},				//P_START
{"All threads successfully initialized","Failed to initialize threads"},			//P_Threads
{"Terminating the program with success","Failed to terminate the program"},			//P_END
{"Capture the input from console", "Failed to capture the input from the console"},	//P_ConsoleIO
{"Motors started","Failed to start the motors"}, 						//M_Start
{"Motors stopped","Failed to stop the motors"},							//M_Stop
{"Accelerometer intialized","Failed to initialize accelerometer"},				//S_Acc
{"Light sensors working","Light sensors not working"},						//S_Light
{"DMS working","DMS not working"},									//S_Dms
{"Zigbee initialized","Failed to initialize zigbee"},						//C_Zigbee
{"IR Comm initialized,","Failed to initialize IR Comm"},					//C_IR
{"WLAN working","WLAN failure"},									//C_WLAN
{"NO ERROR or ISSUES","Generic Error"},								//G_Error
{"No error for logging","Error while logging"},							//G_Log
{"Inside the motors thread","Error: Motors thread"},						//P_Motors
{"Inside the talker","Error: Talker thread"},							//P_Talker
{"Inside Tilt Thread","Error: Tilt thread"}, 							//P_Tilt
{"Obstacle Detected"," "},								 		//S_Obs
{"Inside the com","Error: Talker com"},								//P_Com	
{"Boundary exceeded","Error: Bound check"},							//S_Bound
{"Inside Obstacle thread","Error: Obstacle thread"},						//P_Obs	
{"Inside Broadcast thread","Error: Broadcast thread"},						//P_Bro
{"Inside Sensor thread","Error: Sensor thread"},						//P_Sen
{"Inside Xbee thread","Error: Xbee thread"},							//P_Xb
{"Inside localization thread","Error: localization thread"}					//P_IR
};


class Error {
	public:
	Error();																		//Should have the code for creating/opening the log file
	void log (int returnedVal, int condition_num);									//calls log comment - msg selection based on the args
	int logComment (char *comment);													//File handling, writes to the file - modify
	~Error();																		//Responsible for the closing the log file		
	private:
	ofstream ferror;
	time_t cur_time;

};

#endif
