//=============================================================================
// Name        : Error.cpp
// Author      : Jimit Patel
// Version     : 1.0 (DEBUG VERSION)
// Description : Class; exposes the Error handling & logging functions API
//=============================================================================


#include <Error.h>


//Constructor for Error Class
Error::Error(){

	//Update the system time from the web??? ---
	
	cur_time = time(NULL);
	
	//Open the log file
	ferror.open("log.txt");
	
	//Input the first log:
	ferror<<endl<<"====================================================================="<<endl<<endl<<endl;
	ferror<<ctime(&cur_time)<<"\t \t \tProgram launched & log file opened. "<<endl;
}

//Edits the log file - log.txt. Adds a line pointed by "comment".
int Error::logComment(char *comment){
	//get the current time
	cur_time = time(NULL);
	//log the comment
	ferror <<ctime(&cur_time)<<"\t \t \t"<<comment<<endl;
	return 1;
}


//Logs comment based on the "returnedVal" arg
void Error::log (int returnedVal, int condition_num){

	if (1 == returnedVal) 
		logComment((char *)(conditions[condition_num].success));
	else 
		logComment((char *)(conditions[condition_num].failure));
}

//Destructor for Error Class
Error::~Error(){
	
		
	logComment((char *)(conditions[P_END].success));

	ferror<<endl<<"====================================================================="<<endl<<endl<<endl;
	//Close the log file
	ferror.close();
}
