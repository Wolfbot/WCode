//=============================================================================
// Name        : t_Motion.cpp
// Author      : Jimit Patel
// Version     : 2.5 (DEBUG VERSION), Last modified on 23rd April, 2013
// Description : Contains functions for Motion Control. Uses Motors Class
//=============================================================================

#include <Wolfbot.h>
#include <Motors.h>



extern char av1[10], av2[10], av3[10];
extern int ac,flagShutdown,flagObs;
extern void sleepms(int millisec);
extern float X,Z;
extern pthread_mutex_t mutex_cords, mutex_flag; 

extern Motors *motor;
extern Error *error;
extern int demo_type;
extern int d1,d2,d3;

static int BoundChk();
static bool IsObstacle();

void *chk_obs(void *ptr);
void *run_motors(void *ptr);
void *tilt_cam(void *ptr);



void *tilt_cam(void *ptr){
	
	error->log(1,P_Tilt);	

	int condition=1;
	float deg_specified=30;
	// If position specified, tilt and stop
	if (condition == 1) {
		motor->tiltDeg(deg_specified);									//Single tilt
		error->logComment("Cam Tilted to a specific angle - once");
	}
	// If no position specified, tilt through entire range
	else if (condition == 0) {
		error->logComment("Cam sweeping a range of angular positions");
		int deg = -1;
		for (int count_tilts = 0 ; count_tilts < 2; count_tilts++){					//Continuous tilting approx 60 sec sweep
		
				for (deg = 0; deg <= 50 ; deg += 10){
					if (flagShutdown==1) break;
					motor->tiltDeg(deg);
					sleep(1);
				}
				
				if (flagShutdown==1) break;
				sleep(5);
				
				
				for (deg = 50; deg >= 0 ; deg -= 10){
					if (flagShutdown==1) break;
					motor->tiltDeg((float) deg);
					sleep(1);
				}
				
				sleep(5);
			}
	}

}




void *run_motors(void *ptr){

	cout<<"Motors thread started"<<endl;
	
	float heading;
	int bound_result = -1;
	
	error->log(1,P_Motors);

	if (ac <= 2) {
	
		motor->enable();
		error->log(1,M_Start);
		
		// Send in random directions	
		for(int i = 0; i<30; i++) {
			
			//Task - 1 - Check for Obstacles
			
			if (flagObs == 1) {
				pthread_mutex_lock( &mutex_flag );
				motor->stop();
				pthread_mutex_unlock( &mutex_flag );			
				error->log(1,S_Obs);
				sleepms(500);
				cout<<"here"<<endl;
				continue;
			}
			
		
		
		
			//Task - 2 - Generate random heading
			srand(time(NULL));
			heading = (float) (rand() % 360 + 1);
			pthread_mutex_lock( &mutex_flag );			
			motor->setMotorHeading(heading);
			pthread_mutex_unlock( &mutex_flag );
			sleep(1);
	
			
			if (1 == demo_type){
				//Task - 3 -Check for overrun - separate thread for this task, like the obstacle detection????
				pthread_mutex_lock( &mutex_cords );
				bound_result = BoundChk();
				cout<<"X: "<<X<<endl;
				cout<<"Z: "<<Z<<endl;
				pthread_mutex_unlock( &mutex_cords );
				if (bound_result==1){
					cout << "WolfBot is going out of the field ... " << endl;
					error->log(1,S_Bound);

					//reverse the current headng ?? .... 
					if (heading >180) {
						heading-=180;
					} else {
						heading+=180;
					}
					pthread_mutex_lock( &mutex_flag );
					motor->setMotorHeading(heading);
					pthread_mutex_unlock( &mutex_flag );
					//Allow time for the wolfbot to get sufficiently inside the field.
					sleep(4);					
				}
			}
			
			//end of a randome iteration
		}
	}//end of random motion
	else if (ac >= 3) {
	
		motor->enable();
		error->log(1,M_Start);

		heading = (float) atof(av1);
		int sleep_time = (int) atoi(av2);

	//	pthread_mutex_lock( &mutex_flag );
		if (flagObs == 1) {
			//sleep_time = 0;
			error->log(1,S_Obs);
		}
	//	pthread_mutex_unlock( &mutex_flag );
	
		// Send in specified heading for specified time
		motor->setMotorHeading(heading);
		cout << "Moving in phi=" << heading  << " for " << sleep_time << " seconds." << endl;
		sleep(sleep_time);
		
		
	}//end of linear motion

	motor->stop();
	motor->disable();
	
	error->log(1,M_Stop);
	
	cout<<"Exiting the motors thread..."<<endl;
}


static int BoundChk(){
	int result = 0;								//1 for moving out of the field; 0 otherwise
	float distance = sqrtf(X*X + Z*Z);
	if (distance >= 1200){
		result = 1;
	}
	return result;
}


void *chk_obs(void *ptr){
	cout <<"obstacle detection thread started"<<endl;	
	error->log(1,P_Obs);
	while(1){
		if (IsObstacle()){
			pthread_mutex_lock( &mutex_flag );
			flagObs = 1;
			motor->stop();
			pthread_mutex_unlock( &mutex_flag );
			cout << "OBSTACLE DETECTED "<<endl;
		}
		else {
			pthread_mutex_lock( &mutex_flag );
			flagObs = 0;
			pthread_mutex_unlock( &mutex_flag );
		}
		sleepms(250);
	}

	cout<<"Exiting the obstacle thread..."<<endl;

}


static bool IsObstacle() {
	
	char ain0[40]="/sys/devices/platform/omap/tsc/ain1";	//IR DMS#0
	char ain2[40]="/sys/devices/platform/omap/tsc/ain3";	//IR DMS#2	//not working, below the cam, 0 to left of it
	char ain6[40]="/sys/devices/platform/omap/tsc/ain7";	//IR DMS#6
	char adc[6];
	int ret=0;
	
	ifstream fain;
	fain.open(ain0);						// Open & read from the ain0 file
	fain >> adc;
	fain.close();

	d1 = atof(adc);
	if (d1 > 1500){
		ret+=1;
	}
	
	fain.open(ain2);						// Open & read from the ain2 file
	fain >> adc;
	fain.close();
	
	d2 = atof(adc);
	if (d2 > 1500){
		ret+=1;
	} 
	
	fain.open(ain6);						// Open & read from the ain6 file
	fain >> adc;
	fain.close();
	
	d3 = atof(adc);
	if (d3 > 1500){
		ret+=1;
	} 
	
	if (ret == 0) return false;
	return true;
}


