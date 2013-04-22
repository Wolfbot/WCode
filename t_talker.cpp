//=============================================================================
// Name        : t_talker.cpp
// Author      : Jimit Patel
// Version     : 2.2 - DEBUG VERSION
// Description : Connect to Optitrack server / Broadcast data via UDP / Xbee 
//=============================================================================


#include <Wolfbot.h>
#include "XBee.h"
//#include "uart.h"


extern void sleepms(int millisec);
extern void Sleep(float s);
extern float X,Z;
extern pthread_mutex_t mutex_cords, mutex_flag; 
extern char av1[10], av2[10];
extern int ac;
extern Error *error;

const char *fileN = "/dev/i2c-3";			//I2C bus for LSM303DLHC
int p1, p2;

//Thread for Xbees
void *xbee(void *ptr){
	
	error->log(1,P_Xb);

	XBee zigbee;

	if (MODE == 0) {
		char msg[MAX_MSG_SIZE];
		int i;
		
		/*cout << "This test program recives from the XBee and tests for Hello World\n";
		cout << "It diplays Detected Hello World if Hello World has been recieved\n";
		cout << "\n" << "All Recieved XBee messages will be printed below:\n:";
		*/
		zigbee.initXbee();
	
		for(i=0;i<MAX_MSG_SIZE-1;i++)
			msg[i]=NULL;
				
		while(1){
			zigbee.readXbee(msg);
			cout << msg << "\n";
			if(!strcmp(msg,"Hello World")){
				cout << "Sleeping for 2 sec\n";
				sleep(2);
				cout << "Detected Hello World\n";
			}
			for(i=0;i<MAX_MSG_SIZE-1;i++)
				msg[i]=NULL;
		}

	}else {
		zigbee.initXbee();		
              zigbee.sendXbee("Hello World");
	}

	error->logComment("Exiting the Xbee thread...");
	
	return 0;
}


//Implementation is UDP and not TCP
void *broadcast(void *ptr){


	error->log(1,P_Bro);
   	int sock, length, n;
  	socklen_t fromlen;
   	struct sockaddr_in server;
   	struct sockaddr_in from;
   	char buf[20];
   	char data[100];

	try{
		LSM303 lsm303dlhc(fileN);
       	lsm303dlhc.enable();

	
		error->logComment("LSM303 enabled");

		srand(time(NULL));

		int id=100,ax=101,ay=202,az=303,mx=101,my=202,mz=303;
   		p1=1,p2=2;
	   	int d1=1200,d2=2000,d3=3000;		

   		//if (argc < 2) {
      		//	fprintf(stderr, "ERROR, no port provided\n");
      		//	exit(0);
   		//}
  	 
   		sock=socket(AF_INET, SOCK_DGRAM, 0);
   		if (sock < 0) error->logComment("Error in opening socket");
   		length = sizeof(server);
   		bzero(&server,length);
   		server.sin_family=AF_INET;
   		server.sin_addr.s_addr=INADDR_ANY;
   		server.sin_port=htons(8888);
	   	if (bind(sock,(struct sockaddr *)&server,length)<0) 
       		error->logComment("Error in binding the broadcast socket");
   		fromlen = sizeof(struct sockaddr_in);

	   	while (1) {
			//Wait for remote connection

			p1 = rand()%3;
	         	p2 = rand()%3;
			
			//Get the accelerometer & magnetometer readings
			lsm303dlhc.readAccelerationRaw();
			lsm303dlhc.readMagnetometerRaw();
			
			sprintf(data,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d \n",id,lsm303dlhc.acc_x_raw,lsm303dlhc.acc_y_raw,lsm303dlhc.acc_z_raw,lsm303dlhc.mag_x_raw, lsm303dlhc.mag_y_raw, lsm303dlhc.mag_z_raw,p1,p2,d1,d2,d3);
      	  		n = recvfrom(sock,buf,20,0,(struct sockaddr *)&from,&fromlen);
       	  	if (n < 0) error->logComment("Error in recFrom");

			//write(1,"Received a datagram: ",21);
			//write(1,buf,n);
			
			n = sendto(sock,data,sizeof(data),0,(struct sockaddr *)&from,fromlen);
			if (n  < 0) error->logComment("Error in sendTo");
			sleepms(250);

   		}
	}
	catch(exception& e){
		error->logComment(e.what());
	}
   	return 0;
}




class Position{
public:
	Position() {}
	float data[8];

};


Position splitter(char *src,char *delimiter){
	Position *info = new Position();
	int counter=0;
	//char result[8][8];
	char *pch;
	pch = strtok(src,delimiter);
	while (pch != NULL){
		(*info).data[counter] = atof(pch);
		counter++;
		pch = strtok(NULL, " ");
	}
	return *info;

}

void *talker(void *ptr)
{
	
	error->log(1,P_Talker);

	char *args;
	args = (char *)ptr;
       char recvBuff[10000];								//--Dont change the size; its same in the server...
	char delimiter[] = " ";
	memset(recvBuff, '0',sizeof(recvBuff));				//set buffer to 0s

    // ----- SETTING UP SOCKET CONNECTION ----- //
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    portno = 27018;										//Port no for server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       // error("ERROR opening socket");
	error->log(0,P_Talker);
    server = gethostbyname(args);
    if (server == NULL) {
        cout<<"ERROR, no such host... returning from the co-ord thread"<<endl;
	 error->logComment("No host for the optitrack found");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cout<<"ERROR connecting ... returning from the co-ord thread"<<endl;
	}
	else {
		printf("... Connection Established\n");
		int n = 0;
		//int counter = 10;									//For Debugging only / limits the iterations on while loop
		char datapacket[64];
		//---Socket Connection Established-----//		

		//Get the data from the server
		while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
				//neglect data packets with length <= 1
				if (n > 1){
					recvBuff[n] = 0;						//assign null / termination character...so that fputs stops at nth character.
									
					strcpy(datapacket,recvBuff);
					//printf("%s \n",datapacket); 			//"datapacket" contains the position information
															//Format- "1 1 x y z yaw pitch roll"
															
					Position *info = new Position();		//"Info" object contains the delimited data.
					*info = splitter(datapacket,delimiter);
					pthread_mutex_lock( &mutex_cords );
					X = (*info).data[2];
					Z = (*info).data[4];
					pthread_mutex_unlock( &mutex_cords );
					cout<<"X "<<X<<" Z "<<Z<<endl;
					delete info;	
					
					memset(recvBuff, '0',sizeof(recvBuff));
					//counter--;
					//if (counter == 0) break;
				}

		} 
		

		close(sockfd);
	}
    return 0;
}




