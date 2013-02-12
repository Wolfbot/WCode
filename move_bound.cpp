//=============================================================================
// Name        : move.cpp
// Author      : 
// Version     : 3/Bound Check - DEBUG VERSION
// Description : Execute as :
//				 ./ex ipaddr [heading] [time in sec] 
//=============================================================================



#include <wolfbot.h>


//void setMotorHeading(float heading);
//void setMotorPWM(int motor, int duty);
//void setGPIO(int gpioNumber, bool value);
bool IsObstacle();
void sleepms(int millisec);
void *chk_obs(void *ptr);
void *run_motors(void *ptr);
extern void *talker(void *ptr);

int ac;
char av1[10], av2[10];
float X,Z;													//Co-ordinates of the wolfbot in X-Z plane


pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cords = PTHREAD_MUTEX_INITIALIZER;
int flag;


int BoundChk(){
	int result = 0;		//1 for moving out of the field; 0 otherwise
	float distance = sqrtf(X*X + Z*Z);
	if (distance >= 1200){
		result = 1;
	}
	return result;
}

int main (int argc, char* argv[])
{

	pthread_t p_Obstacle_chk;
	pthread_t p_Motors;
	pthread_t p_Coord_receive;
	
	
	int iret1=0, iret2=0, iret3=0;
	
	X=0.0f;
	Z=0.0f;
	ac = argc;
	if (ac==4){
		strcpy(av1,argv[2]);
		strcpy(av2,argv[3]);
	}
	flag = 0;
	cout<<"About to start the co-ordinate receive thread..."<<endl;
	iret2 = pthread_create(&p_Coord_receive,NULL,&talker,(void*)argv[1]);
	cout<<"About to start the obstacle detection thread..."<<endl;	
	iret1 = pthread_create(&p_Obstacle_chk,NULL,&chk_obs,NULL);
	cout<<"About to start the motors thread..."<<endl;
	iret3 = pthread_create(&p_Motors,NULL,&run_motors,NULL);

	if ( (iret1+iret2+iret3)>0){
		cout <<"Error in thread formations..."<<endl;
		cout <<"iret for co-ords: "<<iret1<<endl;
		cout <<"iret for obstacles: "<<iret2<<endl;
		cout <<"iret for motors: "<<iret3<<endl;
	}
	 
	//pthread_join(Obstacle_chk,NULL);
	pthread_join(p_Motors,NULL);
	
	//all code pertaining to movement is complete and so it safe to 
	//kill the obstacle detection and co-ord reception threads...
	
 	if (!pthread_cancel(p_Obstacle_chk)){
		if (!pthread_cancel(p_Coord_receive)){
			cout<<"All threads exited without any errors"<<endl;
		}
		else {
			cout<<"Error in shutting down the co-ordinate reception thread"<<endl;
		}
	}else {
		cout<<"Error in shutting down the obstacle detection thread"<<endl;
	}
	
	
	return(EXIT_SUCCESS);
}


void *chk_obs(void *ptr){
	cout <<"obstacle detection thread started"<<endl;	
	while(1){
		if (IsObstacle()){
			pthread_mutex_lock( &mutex_flag );
			// Disable motors
			setGPIO(63, false);
			// Stop motors	
			setMotorPWM(1, 0);
			setMotorPWM(2, 0);
			setMotorPWM(3, 0);
			flag = 1;
			pthread_mutex_unlock( &mutex_flag );
			cout << "OBSTACLE DETECTED "<<endl;
			break;
		}
		sleepms(250);
	}

	cout<<"Exiting the obstacle thread..."<<endl;

}

void *run_motors(void *ptr){
	cout<<"Motors thread started"<<endl;
	float heading;
	int bound_result = -1;
	
	// Stop motors	
	setMotorPWM(1, 0);
	setMotorPWM(2, 0);
	setMotorPWM(3, 0);
	// Enable motors
	setGPIO(63, true);
	
	if (ac == 2) {
		// Send in random directions	
		for(int i = 0; i<60; i++) {
			srand(time(NULL));
			
			// Generate random heading
			heading = (float) (rand() % 360 + 1);
			setMotorHeading(heading);
		
			sleep(1);
			
			pthread_mutex_lock( &mutex_flag );
			if (flag == 1) break;
			pthread_mutex_unlock( &mutex_flag );
			
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
				setMotorHeading(heading);
				//Allow time for the wolfbot to get sufficiently inside the field.
				sleep(4);					
			}
		}
	}
	else if (ac == 4) {
		
		heading = (float) atof(av1);
		int sleep_time = (int) atoi(av2);

		pthread_mutex_lock( &mutex_flag );
		if (flag == 1) sleep_time = 0;
		pthread_mutex_unlock( &mutex_flag );

		cout << "Moving in phi=" << av1
			<< " for " << av2 << " seconds." << endl;

		// Send in specified heading for specified time
		setMotorHeading(heading);
		sleep(sleep_time);
	}

	// Disable motors
	setGPIO(63, false);
	// Stop motors	
	setMotorPWM(1, 0);
	setMotorPWM(2, 0);
	setMotorPWM(3, 0);

	cout<<"Exiting the motors thread..."<<endl;
}




void setMotorHeading(float heading) {
	cout << "Heading: " << heading << " deg" << endl;
	// Calculate angles
	float v_1 = 100.0f*sin(_dtor(60.0-heading));
	float v_2 = 100.0f*sin(_dtor(300.0-heading));
	float v_3 = 100.0f*sin(_dtor(heading));
	
	// Set motor directions
	cout << "Reverse: ";
	if (v_1 < 0) {	// Motor 1
		cout << 1 << " ";
		setGPIO(26, 1);	// Reverse
		v_1 += 100;	// Duty cycle = 100-abs(duty)
	}
	else {
		setGPIO(26, 0);	// Forward
	}
	if (v_2 < 0) {	// Motor 2
		cout << 2 << " ";
		setGPIO(47, 1);	// Reverse
		v_2 += 100;	// Duty cycle = 100-abs(duty)
	}
	else {
		setGPIO(47, 0);	// Forward
	}
	if (v_3 < 0) {	// Motor 3
		cout << 3 << " ";
		setGPIO(46, 1);	// Reverse
		v_3 += 100;	// Duty cycle = 100-abs(duty)
	}
	else {
		setGPIO(46, 0);	// Forward
	}
	cout << endl;

	// Set motor PWMs
	setMotorPWM(1, (int) v_1);
	setMotorPWM(2, (int) v_2);
	setMotorPWM(3, (int) v_3);
} 

void setGPIO(int gpioNumber, bool value)
{
	fstream fValue;	
	fstream fExport;

	// Create filename
	ostringstream conv;
	conv << gpioNumber;
	string filename = string("/sys/class/gpio/gpio") + conv.str() + string("/value");

	// Export GPIO (Make available via sysfs)
	fExport.open("/sys/class/gpio/export");
	fExport << gpioNumber << endl;
	fExport.close();

	// Write value
	fValue.open(filename.c_str());

	if (value) {
		fValue << "1" << endl;
	}	
	else {
		fValue << "0" << endl;
	}

	fValue.close();
	
	return;
}

void setMotorPWM (int motor, int duty)
{
	// Set frequency to 100Hz
	const int freq = 100;

	// Check for out of range values
	if ((duty < 0) || (duty > 100)) {
			cerr << "Out of range" << endl;
			return;
	}
	
	cout << "Motor " << motor
	  << ": f=" << freq
	  << " d=" << duty << endl;

	// Determine where the motor control files are
	string dir;
	switch (motor)
	{
	case 1:
		dir = "/sys/class/pwm/ehrpwm.2:1";
		break;
	case 2:
		dir = "/sys/class/pwm/ehrpwm.1:0";
		break;
	case 3:
		dir = "/sys/class/pwm/ehrpwm.0:1";
		break;
	default:
		cerr << "Invalid motor number." << endl;
		return;
	}
	
        // Open files and write new parameters
        ofstream fDuty, fFreq, fRun, fRequest;

	// Create filenames
	string requestFile = dir + string("/request");
	string dutyFile = dir + string("/duty_percent");
	string freqFile = dir + string("/period_freq");
	string runFile = dir + string("/run");

        // Open necessary files
        fRequest.open(requestFile.c_str());
        fDuty.open(dutyFile.c_str());
        fFreq.open(freqFile.c_str());
        fRun.open(runFile.c_str());

        fRequest << 1 << endl;          // Request control
        fFreq << freq << endl;          // Set frequency
        fDuty << duty << endl;          // Set duty cycle in nanoseconds
		fRequest << 0 << endl;          // Release control
        fRun << 1 << endl;              // Start PWM if it is stopped

        // Close files
        fRequest.close();
        fDuty.close();
        fFreq.close();
        fRun.close();

        return;
}


bool IsObstacle() {
	
	char ain0[40]="/sys/devices/platform/omap/tsc/ain1";
	char ain2[40]="/sys/devices/platform/omap/tsc/ain3";
	char ain6[40]="/sys/devices/platform/omap/tsc/ain7";
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
	
void sleepms(int millisec){
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = millisec * 1000000L;
	nanosleep(&req, (struct timespec *)NULL);
}


