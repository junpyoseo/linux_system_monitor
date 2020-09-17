#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include <sys/types.h>



using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
public:
    static string getCmd(string pid); // complete
    static vector<string> getPidList(); // complete
    static std::string getVmSize(string pid); // complete
    static std::string getCpuPercent(string pid); // complete
    static long int getSysUpTime(); // complete
    static std::string getProcUpTime(string pid); // complete
    static string getProcUser(string pid); // complete
    static vector<string> getSysCpuPercent(string coreNumber = ""); // complete
    static float getSysRamPercent(); // complete
    static std::string getSysKernelVersion(); // complete
    static int getNumberOfCores(); // complete
    static int getTotalThreads(); // complete
    static int getTotalNumberOfProcesses(); // complete
    static int getNumberOfRunningProcesses(); // complete
    static std::string getOSName(); // complete
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2); // complete
    static bool isPidExisting(string pid); // complete
  
   // active cpu and idle cpu time helper functions
  static float getSysActiveCpuTime(std::vector<std::string> values);
  static float getSysIdleCpuTime(std::vector<std::string> values);
};
// TODO: Define all of the above functions below:

std::string ProcessParser::getVmSize(string pid){
	
	// local variables
	std::string line;
	// target attribute
	std::string name = "VmData";
  	std::string value;
	float result;
	ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
	while(std::getline(stream, line)){
 	// searching line by line
      if(line.compare(0, name.size(), name) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	// KB -> GB conversion
        	result = (stof(values[1])/float(1024*1024));
        	break;
      }// end of if braces
    
    }// end of while braces
  return std::to_string(result);
} // end of getVmSize()

std::string ProcessParser::getCpuPercent(string pid){
	
	// local variables
	std::string line;
	// target attribute
	std::string value;
	float result;
	ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
	getline(stream, line);
  	std::string str = line;
   	istringstream buf(str);
    istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> values(beg, end);
  	float utime = stof(ProcessParser::getProcUpTime(pid));
	float stime = stof(values[14]);
	float cutime = stof(values[15]);
	float cstime = stof(values[16]);
	float starttime = stof(values[21]);
	float uptime = ProcessParser::getSysUpTime();
	float freq = sysconf(_SC_CLK_TCK);
	float total_time = utime + stime + cutime + cstime;
	float seconds = uptime - (starttime / freq);
    result = 100.0 * ((total_time/freq) / seconds);
    return std::to_string(result);
                                  
} // end of getCpuPercent()

std::string ProcessParser::getProcUpTime(string pid){

	// local variables
	std::string line;
	// target attribute
	std::string value;
	float result;
	ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
 	getline(stream, line);
  	std::string str = line;
   	istringstream buf(str);
    istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> values(beg, end);
  	result = stof(values[13])  / sysconf(_SC_CLK_TCK);
  	return std::to_string(result);
}// end of getProcUpTime()
                               
long int ProcessParser::getSysUpTime(){
	std::string line;
	ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
	getline(stream, line);
	istringstream buf(line);
	istream_iterator<std::string> beg(buf), end;
	std::vector <std::string> values(beg, end);
	return stoi(values[0]);
	
}// end of getSysUpTime()
                                  
std::string ProcessParser::getProcUser(string pid){
	// local variables
	std::string line;
	// target attribute
	std::string name = "Uid";
	std::string result = "";
	ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
	while(std::getline(stream, line)){
 	// searching line by line
      if(line.compare(0, name.size(), name) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	result = values[1];
        	break;
      }// end of if braces
    
    }// end of while braces
	stream = Util::getStream("/etc/passwd");
	name = ("x:" + result);
	while(std::getline(stream, line)){
    if(line.find(name) != std::string::npos){
      result  = line.substr(0, line.find(":"));
      return result;
    }// end if 2nd if brace   
 } // end of 2nd while brace
 return ""; // return an empty string if not found
} // end of getProcUser()
                                  
std::vector<string> ProcessParser::getPidList(){
	DIR* dir;
	std::vector<std::string> container;

	
	if(!(dir = opendir("/proc")))
       throw (std::runtime_error(std::strerror(errno)));
	while(dirent* dirp = readdir(dir)){
    	// Directory check
			
      	if(dirp -> d_type != DT_DIR)
          continue;
      	// Check whether every letter is a digit
   		if(all_of(dirp -> d_name, dirp -> d_name + std::strlen(dirp -> d_name), [](char c) {return std::isdigit(c);})) {
        container.push_back(dirp -> d_name);
        } // end of if brace
     
    }// end of while brace
	

	if(closedir(dir)){
		throw std::runtime_error(std::strerror(errno));
	}
       
	return container;
}// end of getPidList()

std::string ProcessParser::getCmd(string pid){
std::string line;
ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));

std::getline(stream, line);
return line;
} // end of getCmd()
       
       
int ProcessParser::getNumberOfCores(){
  // local variables
	std::string line;
	// target attribute
	std::string name = "cpu cores";
  	std::string value;
	int result = 0;
   // base path is /proc/
	ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
	while(std::getline(stream, line)){
 	// searching line by line
      if(line.compare(0, name.size(), name) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	result = (stoi(values[3]));
        	break;
      }// end of if braces
    
    }// end of while braces
  return result;
}// end of getNumberOfCores()

std::vector<string> ProcessParser::getSysCpuPercent(string coreNumber){

  // local variables
	std::string line;
	// target attribute
	std::string name = "cpu" + coreNumber;
   // base path is /proc/
	ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
	while(std::getline(stream, line)){
 	// searching line by line
      if(line.compare(0, name.size(), name) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
      // return a vector of strings 
      return values; 
      } // end if if brace
    }// end of while loop
  return std::vector<std::string>();
} // end of getSysCpuPercent()

float ProcessParser::getSysActiveCpuTime(std::vector<std::string> values) {
  return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
} // end of getSysActiveCpuTime()
float ProcessParser::getSysIdleCpuTime(std::vector<std::string> values){
  return (stof(values[S_IDLE]) + stof(values[S_IOWAIT])); 
} // end of getSysIdleCpuTime()
       
std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2){

  float  result;
  float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
  float totalTime = activeTime + idleTime;
  result = 100.0 * (activeTime / totalTime) ; 
  return std::to_string(result);
} // end of printCpuStatus()

float ProcessParser::getSysRamPercent() {

  // local variables
	std::string line;
	// target attribute
	std::string name1 = "MemAvailable:";
	std::string name2 = "MemFree:";
	std::string name3 = "Buffers:";
  
	//memUsage variables
	float availableMemory = 0;
	float freeMemory = 0;
	float buffers = 0;
   // base path is /proc/
	ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
	while(std::getline(stream, line)){  
 	// searching line by line
    	if(availableMemory != 0 && freeMemory !=0)
          break;
		if(line.compare(0, name1.size(), name1) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	availableMemory = stof(values[1]);
		} // end of an if statement
		if(line.compare(0, name2.size(), name2) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	freeMemory = stof(values[1]);
		} // end of an if statement
		if(line.compare(0, name3.size(), name3) == 0) {
        	istringstream buf(line);
        	istream_iterator<std::string> beg(buf), end;
        	std::vector<std::string> values(beg, end);
        	buffers = stof(values[1]);
		} // end of an if statement
    } // end of a while statement

return float(100.0 * (1 - ( freeMemory / (availableMemory - buffers ))));
  
}// end of getSysRamPercent()
       
       
std::string ProcessParser::getSysKernelVersion() {

std::string line;
std::string name;
std::string result = "";

ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
  while(getline(stream, name)){
    if(line.compare(0, name.size(), name) == 0){
      istringstream buf(line);
      istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      result = values[2];
      break;
    } // end of an if statement
  }// end of an while statement
	return result;
}// end of getSysKernelVersion()

        
std::string ProcessParser::getOSName() {
  std::string line;
  std::string name = "PRETTY_NAME=";
  std::string result = "";
  
  ifstream stream = Util::getStream(("/etc/os-release"));
  
  while(getline(stream, line)){
	if(line.compare(0, name.size(), name) == 0){
      std::size_t pos = line.find("=");
      pos += 1;
      result = line.substr(pos);
      result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
     break;
    }// end of a if statement
  }// end of a while loop
  
  return result;
}  // end of getOsName()


int ProcessParser::getTotalThreads() {
// objective: get each pid and look up the status of each pid 
// and add every thread count
// local variable declaration
std::string line; // string parser 
std::string name = "Threads:"; // targetstring
int result = 0; // total thredcount
std::vector <std::string> pid_list = ProcessParser::getPidList();
for ( int i = 0 ; i < pid_list.size() ; i++){
std::string eachPid = pid_list[i];
	ifstream stream = Util::getStream((Path::basePath() + eachPid + Path::statusPath()));
  	while( std::getline(stream, line)){
      if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      result += stoi(values[1]);
      break;
      		} // end of an if statement
    	} // end of an while statement
 
 	} // end of for loop
 return result;
} // end of getTotalThreads 
int ProcessParser::getTotalNumberOfProcesses(){

// string parser
std::string line;
std::string name = "processes";
int result = 0;
ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(std::getline(stream, line)){
    if(line.compare(0, name.size(), name) == 0){
    istringstream buf(line);
    istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> values(beg, end);
    result += stoi(values[1]);
    break;
    } // end of an if statement
  } // end of a while loop
return result;
} // end of getTotalNumberofProcesses()

  
int ProcessParser::getNumberOfRunningProcesses() {
// string parser
std::string line;
// search target
std::string name = "procs_running";
int result = 0;
ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(std::getline(stream, line)){
    if(line.compare(0, name.size(), name) == 0){
    istringstream buf(line);
    istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> values(beg, end);
    result += stoi(values[1]);
    break;
    } // end of an if statement
  } // end of a while loop
return result;
} // end of getNumberOfRunningProcesses()
  
bool ProcessParser::isPidExisting(string pid) {

 // local variable
bool pidExisting = false;
  
std::vector <std::string> pid_list = ProcessParser::getPidList();
for ( int i = 0 ; i < pid_list.size() ; i++){
std::string eachPid = pid_list[i];
  if (eachPid == pid){
  pidExisting = true;
  break;
  }// end of an if statement
} // end of a while statement
  
return pidExisting;
  
}  // end of isPidExisting()
  