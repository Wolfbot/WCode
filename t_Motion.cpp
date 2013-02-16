//=============================================================================
// Name        : t_Motion.cpp
// Author      : Jimit Patel
// Version     : 1.2 (DEBUG VERSION), Last modified on 14th Feb, 2013
// Description : Lib; Contains functions for Motion Control. Uses Motors Class
//=============================================================================

#include <Wolfbot.h>
#include <Motors.h>

extern char av1[10], av2[10];
extern int ac,flagShutdown,flagObs;
extern void sleepms(int millisec);
extern float X,Z;
extern pthread_mutex_t mutex_cords, mutex_flag; 

extern Motors *motor;

static int BoundChk();
static bool IsObstacle();

void *chk_obs(void *ptr);
void *run_motors(void *ptr);
void *tilt_cam(void *ptr);



void *tilt_cam(void *ptr){
	
	int condition=0;
	float deg_specified=30;
	// If position specified, tilt and stop
	if (condition == 1) {
		motor->tiltDeg(deg_specified);									//Single tilt
	}
	// If no position specified, tilt through entire range
	else if (condition == 0) {
		int deg = -1;
		for (int count_tilts = 0 ; count_tilts < 2; count_tilts++){		//Continuous tilting approx 60 sec sweep
		
				for (deg = 0; deg <= 50 ; deg += 10){
					if (flagShutdown==1) return;
					motor->tiltDeg((float) deg);
					sleep(1);
				}
				
				sleep(10);
				
				for (deg = 50; deg >= 0 ; deg -= 10){
					if (flagShutdown==1) return;
					motor->tiltDeg((float) deg);
					sleep(1);
				}
				
				sleep(10);
			}
	}

}




void *run_motors(void *ptr){

	cout<<"Motors thread started"<<endl;
	
	float heading;
	int bound_result = -1;
	
	if (ac == 2) {
	
		motor->enable();
		
		// Send in random directions	
		for(int i = 0; i<60; i++) {
			
			//Task - 1 - Check for Obstacles
			pthread_mutex_lock( &mutex_flag );
			if (flagObs == 1) {
				motor->stop();
				flagObs == 0;
				continue;
			}
			pthread_mutex_unlock( &mutex_flag );
		
		
		
			//Task - 2 - Generate random heading
			srand(time(NULL));
			heading = (float) (rand() % 360 + 1);
			motor->setMotorHeading(heading);
			sleep(1);
	
			
			
			//Task - 3 -Check for overrun - separate thread for this task, like the obstacle detection????
			pthread_mutex_lock( &mutex_cords );
			bound_result = BoundChk();
			cout<<"X: "<<X<<endl;
			cout<<"Z: "<<Z<<endl;
			pthread_mutex_unlock( &mutex_cords );
			if (bound_result==1){
				cout << "WolfBot is going out of the field ... " << endl;
				//reverse the current headng ?? .... 
				if (heading >180) {
					heading-=180;
				} else {
					heading+=180;
				}
				motor->setMotorHeading(heading);
				//Allow time for the wolfbot to get sufficiently inside the field.
				sleep(4);					
			}
			
			//end of a randome iteration
		}
	}//end of random motion
	else if (ac == 4) {
		
		heading = (float) atof(av1);
		int sleep_time = (int) atoi(av2);

		pthread_mutex_lock( &mutex_flag );
		if (flagObs == 1) sleep_time = 0;
		pthread_mutex_unlock( &mutex_flag );

		cout << "Moving in phi=" << av1 << " for " << av2 << " seconds." << endl;

		// Send in specified heading for specified time
		motor->setMotorHeading(heading);
		sleep(sleep_time);
	}//end of linear motion

	motor->stop();
	motor->disable();
	
	cout<<"Exiting the motors thread..."<<endl;
}


static int BoundChk(){
	int result = 0;													//1 for moving out of the field; 0 otherwise
	float distance = sqrtf(X*X + Z*Z);
	if (distance >= 1200){
		result = 1;
	}
	return result;
}


void *chk_obs(void *ptr){
	cout <<"obstacle detection thread started"<<endl;	
	while(1){
		if (IsObstacle()){
			pthread_mutex_lock( &mutex_flag );
			flagObs = 1;
			motor->stop();
			pthread_mutex_unlock( &mutex_flag );
			cout << "OBSTACLE DETECTED "<<endl;
			break;
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
	int adcVal,ret=0;
	
	ifstream fain;
	fain.open(ain0);						// Open & read from the ain0 file
	fain >> adc;
	fain.close();

	adcVal = atof(adc);
	if (1000 < adcVal){
		ret+=1;
	}
	
	/*fain.open(ain2);						// Open & read from the ain2 file - BROKEN, check hw
	fain >> adc;
	fain.close();
	
	adcVal = atof(adc);
	if (1000 < adcVal){
		ret+=1;
	} */
	
	fain.open(ain6);						// Open & read from the ain6 file
	fain >> adc;
	fain.close();
	
	adcVal = atof(adc);
	if (1000 < adcVal){
		ret+=1;
	} 
	
	if (ret == 0) return false;
	return true;
}


