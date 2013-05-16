//=============================================================================
// Name        : Motors.cpp
// Author      : Charles Teague, Jimit Patel
// Version     : 1.1 (DEBUG VERSION)
// Description : Class; defines the Motors API
//=============================================================================

#include <Motors.h>

Motors::Motors(){
	// Enable motors
	setGPIO(63, true);
	// Stop motors	
	setMotorPWM(1, 0);
	setMotorPWM(2, 0);
	setMotorPWM(3, 0);
}

void Motors::stop(){
	// Stop motors	
	setMotorPWM(1, 0);
	setMotorPWM(2, 0);
	setMotorPWM(3, 0);
}

void Motors::disable(){
	//Disable motors
	setGPIO(63, false);
}

void Motors::enable(){
	//Enable motors
	setGPIO(63, true);
}

void Motors::setMotorHeading(float heading){
	cout << "Heading: " << heading << " deg" << endl;
	// Calculate angles
	//float v_1 = 100.0f*sin(_dtor(60.0-heading));
	//float v_2 = 100.0f*sin(_dtor(300.0-heading));
	//float v_3 = 100.0f*sin(_dtor(heading));
	float v_1 = 100.0f*sin(_dtor(240-heading));
	float v_2 = 100.0f*sin(_dtor(0-heading));
	float v_3 = 100.0f*sin(_dtor(120-heading));
	
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


void Motors::setMotorPWM (int motor, int duty){
	// Set frequency to 100Hz
	//const int freq = 100;

	// Check for out of range values
	if ((duty < 0) || (duty > 100)) {
		cerr << "Out of range" << endl;
		return;
	}
	
	cout << "Motor " << motor<< ": f=" << freq_motors<< " d=" << duty << endl;

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
	fFreq << freq_motors << endl;   // Set frequency**** (redundant??)
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

//NOTE: In Future versions, define this as static / push this in header file
void Motors::setGPIO(int gpioNumber, bool value)
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

void Motors::tiltDeg (float deg)
{
	// Check for out of range values
	if ((deg < 0.0) || (deg > 90.0)) {
		cerr << "Out of range" << endl;
		return;
	}

	// Compute duty cycle using measured parameters
	int dutyNs = (int) calDutyNs(deg);
	// frequency -> 50Hz (typical hobby servo freq.)

	// Open files and write new parameters
	ofstream fDuty, fFreq, fRun, fRequest;

	// Open necessary files
	fRequest.open("/sys/class/pwm/ecap.0/request");
	fDuty.open("/sys/class/pwm/ecap.0/duty_ns");
	fFreq.open("/sys/class/pwm/ecap.0/period_freq");
	fRun.open("/sys/class/pwm/ecap.0/run");

	fRequest << 1 << endl;		// Request control
	fDuty << dutyNs << endl;	// Set duty cycle in nanoseconds
	fFreq << freq_tilt << endl;		// Set frequency
	fRequest << 0 << endl;		// Release control
	fRun << 1 << endl;		// Start PWM if it is stopped

	// Close files
	fRequest.close();
	fDuty.close();
	fFreq.close();
	fRun.close();

	//return;
}


