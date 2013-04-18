//=============================================================================
// Name        : Talker.cpp
// Author      : Jimit Patel
// Version     : 1.0 - DEBUG VERSION
// Description : Connect to Optitrack server, get the raw data, delimit for " " 
//=============================================================================


#include <Wolfbot.h>

extern float X,Z;
extern pthread_mutex_t mutex_cords, mutex_flag; 
extern char av1[10], av2[10];
extern int ac;

extern Error *error;


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
        fprintf(stderr,"ERROR, no such host... returning from the co-ord thread\n");
        
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
