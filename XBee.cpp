//=============================================================================
// Name        : XBee.cpp
// Author      : Anthony Arazo, Jimit Patel
// Version     : 1.0 (DEBUG VERSION)
// Description : Class; defines the Xbee API
//=============================================================================


#include <XBee.h>
//#include "uart.h"

FILE     *RX1_Handle = NULL, *TX1_Handle = NULL;
const char   *uart1_tx = "/sys/kernel/debug/omap_mux/uart1_txd", *uart1_rx = "/sys/kernel/debug/omap_mux/uart1_rxd";

int XBee::readXbee(char * rcv_buff){ /* Pass some string into this function to be sent out. */
	char msg[31];
	termios Xbee;
	int fd, bytes_read;
	if ((fd = open("/dev/ttyO1", O_RDWR | O_NOCTTY)) < 0){
		cout << "Could not open port." << endl;
		return -1;    
	}
	if (tcgetattr(fd, &Xbee) != 0){ /* Obtain current terminal device settings in order to modify them at a later time. */
		cout << "Unable to retrieve port attributes." << endl;
		return -1;    
	}
	if (cfsetispeed(&Xbee, B9600) < 0){ /* Sets baud rate to 9600 */
		cout << "Input baud rate not successfully set." << endl;
	}
	Xbee.c_iflag = 0;
	Xbee.c_oflag = 0;
	Xbee.c_lflag = 0;
	Xbee.c_cc[VMIN] = MAX_MSG_SIZE; /* Wait for an array of a certain size to enter the buffer before moving on. */
	Xbee.c_cc[VTIME] = 1;/* Timeout after 0.1 seconds */
	tcsetattr(fd, TCSANOW, &Xbee); /* Set newly-modified attributes. */
	bytes_read = read(fd, rcv_buff, MAX_MSG_SIZE); /* Reads ttyO port, stores data into rcv_buff. */
	close(fd);
	return 1;
}
 
int XBee::sendXbee(const char *msg){ /* Pass some string into this function to be sent out. */
	termios Xbee;
	int size = strlen(msg); /* Obtain the size of the string; for use in write() */
	int fd;
	if ((fd = open("/dev/ttyO1", O_RDWR | O_NOCTTY)) < 0){
		cout << "Could not open ttyO1." << endl;
		return -1;    
	}
	if (cfsetospeed(&Xbee, B9600) < 0){    cout << "Input baud rate not successfully set." << endl;    }
	if (tcgetattr(fd, &Xbee) != 0){ /* Obtain current terminal device settings in order to modify them at a later time. */
		cout << "Unable to retrieve port attributes." << endl;    return -1;    }
	Xbee.c_iflag = 0;
	Xbee.c_oflag = 0;
	Xbee.c_lflag = 0;
	Xbee.c_cc[VMIN] = 0;
	Xbee.c_cc[VTIME] = 1;
	tcsetattr(fd, TCSANOW, &Xbee); //Set newly-modified attributes
	write(fd, msg, size);
	close(fd);
	return 1;
}

void XBee::initXbee(void){	//Run this before using send or recv
	ofstream Xbee_RX, Xbee_TX;
	
	Xbee_RX.open(uart1_rx); //Open mode select files
	Xbee_TX.open(uart1_tx); //These are defined in uart.h
	
	Xbee_RX << 20 << endl; 	// select Mode 0 - Input
	Xbee_TX << 0x00 << endl;	// select Mode 0 - Output
	
	Xbee_RX.close();
	Xbee_TX.close();
}