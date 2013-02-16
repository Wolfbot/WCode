//=============================================================================
// Name        : main.cpp
// Author      : Jimit Patel
// Version     : 1.2 (DEBUG VERSION)
// Description : Execute as :
//				 ./ex ipaddr [heading] [time in sec] 
//=============================================================================



#include <Wolfbot.h>
#include <Motors.h>


//----------FUNCTIONS---------------------------------//

//POSIX Function calls
extern void *talker(void *ptr); 		
extern void *chk_obs(void *ptr);
extern void *run_motors(void *ptr);
extern void *tilt_cam(void *ptr);

//Native Function Prototype
void sleepms(int millisec);
//----------------------------------------------------//



//----------GLOBALS-----------------------------------//
//variables
int ac=-1,flagShutdown=-1, flagObs=-1;
char av1[10], av2[10];
float X=-1.0,Z=-1.0;															//Co-ordinates of the wolfbot in X-Z plane

//Mutexes
pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cords = PTHREAD_MUTEX_INITIALIZER;

//Objects
Motors *motor;

//----------------------------------------------------//



int main (int argc, char* argv[])
{

	pthread_t p_Obstacle_chk;
	pthread_t p_Motors;
	pthread_t p_Coord_receive;
	pthread_t p_Tilt_Cam;
	
	/*//--------THREAD KEY------------------------------------------//
			1 -> p_Coord_receive		// 1 server
			2 -> p_Obstacle_chk			// 2 DMS working 
			3 -> p_Motors				// 3 motors
			4 -> p_Tilt_Cam				// 4th motor is for tilt
	//--------------------------------------------------------------//*/
	
	
	
	int iret1=-2, iret2=-2, iret3=-2,iret4=-2;
	
	X=0.0f;
	Z=0.0f;
	ac = argc;
	motor = new Motors();
	
	if (ac==4){
		strcpy(av1,argv[2]);
		strcpy(av2,argv[3]);
	}
	flagShutdown = 0;
	cout<<"About to start the co-ordinate receive thread..."<<endl;
	iret1 = pthread_create(&p_Coord_receive,NULL,&talker,(void*)argv[1]);
	cout<<"About to start the obstacle detection thread..."<<endl;	
	iret2 = pthread_create(&p_Obstacle_chk,NULL,&chk_obs,NULL);
	cout<<"About to start the motors thread..."<<endl;
	iret3 = pthread_create(&p_Motors,NULL,&run_motors,NULL);
	cout<<"About to start the camera tilting thread..."<<endl;
	iret4 = pthread_create(&p_Tilt_Cam,NULL,&tilt_cam,NULL);
	

	if ( (iret1+iret2+iret3+iret4)>0){
		cout <<"Error in thread formations..."<<endl;
		cout <<"iret for co-ords: "<<iret1<<endl;
		cout <<"iret for obstacles: "<<iret2<<endl;
		cout <<"iret for motors: "<<iret3<<endl;
		cout <<"iret for tilt :"<<iret4<<endl;
	}
	 
	//pthread_join(Obstacle_chk,NULL);
	pthread_join(p_Motors,NULL);	//High Priority thread for completion task
	
	/*all code pertaining to movement is complete and so it safe to 
	kill the obstacle detection and co-ord reception threads... */
	
	//Send Cancellation request to all other threads
	iret1 = pthread_cancel(p_Coord_receive);
	//log(iret#,noerror_value,condition#)
	iret2 = pthread_cancel(p_Obstacle_chk);
	//log(iret#,noerror_value,condition#)
	iret3 = pthread_cancel(p_Tilt_Cam);
	//log(iret#,noerror_value,condition#)
	
	
	//Wait for thread completion - log ALL
	iret1 = pthread_join(p_Coord_receive,NULL);
	iret2 = pthread_join(p_Obstacle_chk,NULL);
	iret4 = pthread_join(p_Tilt_Cam,NULL);
	
	delete motor;
	
	return(EXIT_SUCCESS);
}


void sleepms(int millisec){
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = millisec * 1000000L;
	nanosleep(&req, (struct timespec *)NULL);
}


