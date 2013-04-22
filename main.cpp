//=============================================================================
// Name        : main.cpp
// Author      : Jimit Patel
// Version     : 2.1 (DEBUG VERSION)
// Description : Execute as :
//				 ./ex [ipaddr] [heading] [time in sec] 
//=============================================================================



#include <Wolfbot.h>
#include <Motors.h>

#define EXIT_THREADS 1
#define RUN_MOTORS 1

//----------FUNCTIONS---------------------------------//

//POSIX Function calls
extern void *talker(void *ptr); 		
extern void *chk_obs(void *ptr);
extern void *run_motors(void *ptr);
extern void *tilt_cam(void *ptr);
extern void *broadcast(void *ptr);
extern void *xbee(void *ptr);

//Native Function Prototype
void sleepms(int millisec);
//----------------------------------------------------//



//----------GLOBALS-----------------------------------//
//variables
int ac=-1,flagShutdown=-1, flagObs=-1;
char av1[10], av2[10],av3[10];
float X=-1.0,Z=-1.0;			
int demo_type = -1;												//Co-ordinates of the wolfbot in X-Z plane

//Mutexes
pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cords = PTHREAD_MUTEX_INITIALIZER;

//Objects
Motors *motor;
Error *error;
//----------------------------------------------------//



int main (int argc, char* argv[])
{

	demo_type = NO_OPTITRACK;
	
	pthread_t p_Obstacle_chk;
	pthread_t p_Motors;
	pthread_t p_Coord_receive;
	pthread_t p_Tilt_Cam;
	pthread_t p_Broadcast;
	pthread_t p_Xbee;
	
	/*//--------THREAD KEY------------------------------------------//
			1 -> p_Coord_receive		// 1 server
			2 -> p_Obstacle_chk			// 2 DMS working 
			3 -> p_Motors				// 3 motors
			4 -> p_Tilt_Cam				// 4th motor is for tilt
			5 -> p_Broadcast			
			6 -> p_Xbee
	//--------------------------------------------------------------//*/
	
	
	
	int iret1=-2, iret2=-2, iret3=-2,iret4=-2, iret5=-2,iret6=-2;
	
	X=0.0f;
	Z=0.0f;
	ac = argc;
	motor = new Motors();
	error = new Error();		

	error->log(1,P_START);
	
	switch(demo_type){
	
		case USING_OPTITRACK:
					
					
					if (ac>=4){
						strcpy(av1,argv[2]);
						strcpy(av2,argv[3]);
					}

					if (ac>4) strcpy(av3,argv[4]);

					flagShutdown = 0;
					cout<<"About to start the co-ordinate receive thread..."<<endl;
					iret1 = pthread_create(&p_Coord_receive,NULL,&talker,(void*)argv[1]);
					cout<<"About to start the obstacle detection thread..."<<endl;	
					iret2 = pthread_create(&p_Obstacle_chk,NULL,&chk_obs,NULL);
					cout<<"About to start the camera tilting thread..."<<endl;
					iret4 = pthread_create(&p_Tilt_Cam,NULL,&tilt_cam,NULL);
					cout<<"About to start the broadcast thread..."<<endl;
					iret5 = pthread_create(&p_Broadcast,NULL,&broadcast,NULL);
					cout<<"About to start the Xbee thread..."<<endl;
					iret6 = pthread_create(&p_Xbee,NULL,&xbee,NULL);
					sleep(1);
					cout<<"About to start the motors thread..."<<endl;
					iret3 = pthread_create(&p_Motors,NULL,&run_motors,NULL);

					if ( (iret1+iret2+iret3+iret4+iret5)>0){
						error->log(0,P_Threads);	
						cout <<"Error in thread formations..."<<endl;
						cout <<"iret for co-ords: "<<iret1<<endl;
						cout <<"iret for obstacles: "<<iret2<<endl;
						cout <<"iret for motors: "<<iret3<<endl;
						cout <<"iret for tilt :"<<iret4<<endl;
						cout <<"iret for Broadcast:"<<iret5<<endl;
						cout <<"iret for Xbee: "<<iret6<<endl;
					}
					 
					//pthread_join(Obstacle_chk,NULL);
					pthread_join(p_Motors,NULL);	//High Priority thread for completion task
					
					/*all code pertaining to movement is complete and so it safe to 
					kill the obstacle detection and co-ord reception threads... */
					
					error->logComment("Cencellation signalled to all threads");
					//Send Cancellation request to all other threads
					iret1 = pthread_cancel(p_Coord_receive);
					iret2 = pthread_cancel(p_Obstacle_chk);
					iret4 = pthread_cancel(p_Tilt_Cam);
					iret5 = pthread_cancel(p_Broadcast);
					iret6 = pthread_cancel(p_Xbee);
					
					//Wait for thread completion - log ALL
					iret1 = pthread_join(p_Coord_receive,NULL);
					iret2 = pthread_join(p_Obstacle_chk,NULL);
					iret4 = pthread_join(p_Tilt_Cam,NULL);
					iret5 = pthread_join(p_Broadcast,NULL);
					iret6 = pthread_join(p_Xbee,NULL);

					error->log(1,G_Error);
					//error->log(1,P_END);
		
		
		case NO_OPTITRACK:
		default:
					
			
					if (ac>=3){
						strcpy(av1,argv[1]);
						strcpy(av2,argv[2]);
					}

					if (ac>3) strcpy(av3,argv[4]);
					if (ac==2) strcpy(av3,argv[1]);
					
					flagShutdown = 0;
					cout<<"About to start the obstacle detection thread..."<<endl;	
				//	iret2 = pthread_create(&p_Obstacle_chk,NULL,&chk_obs,NULL);
					iret2 = 0;
					cout<<"About to start the camera tilting thread..."<<endl;
					iret4 = pthread_create(&p_Tilt_Cam,NULL,&tilt_cam,NULL);
					cout<<"About to start the broadcast thread..."<<endl;
					iret5 = pthread_create(&p_Broadcast,NULL,&broadcast,NULL);
					cout<<"About to start the Xbee thread..."<<endl;

					iret6 = pthread_create(&p_Xbee,NULL,&xbee,NULL);

					if (RUN_MOTORS == 1){			
					//	sleep(1);
						cout<<"About to start the motors thread..."<<endl;
						iret3 = pthread_create(&p_Motors,NULL,&run_motors,NULL);
					}
					else {
						iret3 = 0;
						if (ac>3 || ac == 2) sleep(atoi(av3));
					}

					
					if ( (iret2+iret3+iret4+iret5+iret6)>0){
						error->log(0,P_Threads);	
						cout <<"Error in thread formations..."<<endl;
						cout <<"iret for obstacles: "<<iret2<<endl;
						cout <<"iret for motors: "<<iret3<<endl;
						cout <<"iret for tilt :"<<iret4<<endl;
						cout <<"iret for Broadcast:"<<iret5<<endl;
						cout <<"iret for Xbee: "<<iret6<<endl;
					}
					
					 
					//pthread_join(Obstacle_chk,NULL);
					
					if (RUN_MOTORS == 1)	pthread_join(p_Motors,NULL);	//High Priority thread for completion task
					
					/*all code pertaining to movement is complete and so it safe to 
					kill the obstacle detection and co-ord reception threads... */
			
					if (EXIT_THREADS == 1){
						//Send Cancellation request to all other threads
//						iret2 = pthread_cancel(p_Obstacle_chk);
						iret4 = pthread_cancel(p_Tilt_Cam);
						iret5 = pthread_cancel(p_Broadcast);
						iret6 = pthread_cancel(p_Xbee);
					}
					error->logComment("Cencellation signalled to all threads");
					
					//Wait for thread completion - log ALL
//					iret2 = pthread_join(p_Obstacle_chk,NULL);
					iret4 = pthread_join(p_Tilt_Cam,NULL);
					iret5 = pthread_join(p_Broadcast,NULL);
					iret6 = pthread_join(p_Xbee,NULL);
					
					error->logComment("All threads joined");
					
					error->log(1,G_Error);
					//error->log(1,P_END);
	
	}
	
	delete motor;
	delete error;	
	return(EXIT_SUCCESS);
}


void sleepms(int millisec){
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = millisec * 1000000L;
	nanosleep(&req, (struct timespec *)NULL);
}


void Sleep(float s) 
{ 
    int sec = int(s*1000000); 
    usleep(sec); 
} 

