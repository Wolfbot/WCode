#pragma once

#ifndef WOLFBOT_H
#define WOLFBOT_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <termios.h>    /* Enables us to set baud rate for RX/TX separately */
#include <fcntl.h>      /* Enables use of flags to modify open(), read(), write() functions */
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <Error.h>
#include "LSM303DLHC.h"
#include <exception>

#define _dtor(d) ((d) * (3.14159265359 / 180))
#define debug 1

#define MAX_MSG_SIZE 32 				//FOR XBEE : Max message size (characters) that the XBEE will recieve
#define MODE 0 					//FOR XBEE : Sets program to send or recieve
							// 0 = recieve test
							// !0 = send test


#define USING_OPTITRACK 1
#define NO_OPTITRACK 2

using namespace std;


//Function Prototypes
extern void sleepms(int millisec);


#endif
