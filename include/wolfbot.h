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
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#define _dtor(d) ((d) * (3.14159265359 / 180))
#define debug 1

using namespace std;


//Function Prototypes
extern void sleepms(int millisec);


#endif
