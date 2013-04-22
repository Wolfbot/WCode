#pragma once


#ifndef XBEE_H_
#define XBEE_H_
 
#define ARRAY_SIZE 5

#include <Wolfbot.h>


class XBee{
	public:
	int sendXbee(const char*);
	int readXbee(char *);
	void initXbee(void); 
};


#endif /* XBEE_H_ */