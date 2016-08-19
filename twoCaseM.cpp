#include "carStruct.h"
#include "LandRover2.h"
#include "intersection.h"
#include <stdint.h> 
#include <stdlib.h>
//#include <string.h>
#include <math.h>
//wiring pi

#include <wiringSerial.h>

//using cpp
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

#define V_HIGH 100.0
#define V_LOW 0.3333
#define DIST_TH 70	//dist
#define V_TH 10		//vector
#define P_TH 50 	//position thr
#define DIST_TURN 200 //decide magnitude of turn. MUST be bigger than DIST_TH
//setting
char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;

vector<int> xgoal;
vector<int> ygoal;
vector<string> splitedInput;
string data;		//save received data into string named 'data'
Car* car = new Car[7];			//save information of each car ID, list[0]=blank
LandRover control;
intersection intsec;

int id;			//car id
int x_before;		//for calculating x_v
int y_before;		//for calculating y_v
int sid;		//self id
int step = 0;		//goal
int count=0;		//check if input has received
int lft;		//left line
int btm;		//bottom line
int rht;		//right line
int top;		//top line
int safetyline;		//(right-left)/3
//

void setup(void);
void loop(void);
void printGoal(int id);
void tokenizer(void);	//tokenize string with ','
vector<string> split(string str, char delimiter);
int dataConverter(string tmp);
bool safetyCheck(int sid);
void whereToGo(int dist, double m, double xprime);

int main(void) {
	setup();
	while (1) loop();
	serialClose(fd);
	return 0;
}

void setup() {
	cout << "Rpi StartUp" << endl;
	if ((fd = serialOpen(device, baud))<0) {
		cout << "Unable to open serial device" << endl;
		return;
	}
	if (wiringPiSetup() == -1) {
		cout << "Unable to start wiringPi" << endl;
		return;
	}
        data = "";
	control.setup();
}

void loop() {
	
	//read signal
	if (serialDataAvail(fd)) {
		//cout<<"serialDataAvail"<<endl;
		//update data by tokenizing
		char newChar = serialGetchar(fd);
		if (newChar != '\n') data += newChar;
		if (newChar == '\r') { // KSLee, modified, 
			cout << "input data : " << data << endl;
			//tokenizer(); // KSLee Split function 
			splitedInput = split(data,',');
			
			//cout << "splite[0]: "<<splitedInput[0] << endl;
			if(splitedInput[0] == "p"&&splitedInput[2]!="0"){//if position x is 0, ignore the data
				id = atoi(splitedInput[1].c_str());
				//id = 1;			//for test
				if (count == 0) {
					car[id].x = atoi(splitedInput[2].c_str());
					car[id].y = atoi(splitedInput[3].c_str());
					car[id].x_v = xgoal[step]-car[id].x;
					car[id].y_v = ygoal[step]-car[id].y;
					x_before=1;
				}
				else {
					if(car[id].x!=atoi(splitedInput[2].c_str()) || car[id].y!=atoi(splitedInput[3].c_str())){
						//ignore if there is no difference
						x_before = car[id].x;
						y_before = car[id].y;
						car[id].x = atoi(splitedInput[2].c_str());
						car[id].y = atoi(splitedInput[3].c_str());
						car[id].x_v = car[id].x - x_before;
						car[id].y_v = car[id].y - y_before;
					}
				}
				count++;
				cout << "Car ID : " << id << "	Position : (" << car[id].x << "," << car[id].y << ")" << endl;
				cout << "		      Headvector : (" << car[id].x_v << "," << car[id].y_v<<")" << endl;
				//cout<<"p case done"<<endl;
			}

			if(splitedInput[0] == "t"){
				
                //                cout<<"case t"<<endl;
                                //check how to save ID
				//id = atoi(splitedInput[1].c_str());
				id = 1;			//for test
				sid=1;
				xgoal.push_back(atoi(splitedInput[2].c_str()));
				ygoal.push_back(atoi(splitedInput[3].c_str()));
				printGoal(id);
			}		

			if(splitedInput[0]=="i"){
				lft=atoi(splitedInput[1].c_str());
				btm=atoi(splitedInput[2].c_str());
				rht=atoi(splitedInput[3].c_str());
				top=atoi(splitedInput[4].c_str());
				safetyline=(rht-lft)/3;
				cout<<"LEFT : "<<lft<<", "<<"RIGHT : "<<rht<<endl;
				cout<<"TOP  : "<<top<<", "<<"BOTTOM : "<<btm<<endl;

			}
		
		//start analyzing
			data="";
			
			//cout << "start pos, vector analysis"<<endl;
			if (car[sid].x > 0 && x_before>0) {	//at least one position data required
									//let goal position would be send first
				
				//It would be ok even if y_v becomes 0
				//let x_v=0.0001.
				if (car[sid].x_v == 0) car[sid].x_v = 0.0001;
				double m = car[sid].y_v / car[sid].x_v;
				
				double xprime = (double)car[sid].x + ((double)ygoal[step] - (double)car[sid].y) / m;
				int xd = xgoal[step] - car[sid].x;
				int yd = ygoal[step] - car[sid].y;
				int dist = sqrt(xd*xd + yd*yd);
				cout << "dist is : " << dist << endl;
				cout<<"M : "<<m<<",  X' :"<<xprime<<endl;
				
				if (dist <= DIST_TH) {
					if (step == xgoal.size() - 1) {
						cout << "End of the trip!" << endl;
						if (control.checkStopState()) control.stop();
						xd = 0;	yd = 0;
						vector<int>().swap(xgoal);	vector<int>().swap(ygoal);	//releasing memory
			            delete [] car;            //serialClose(fd);
					}
					else {
						step++;
						cout << "Go to next step : " << step << endl;
						xd = xgoal[step] - car[sid].x;
						yd = ygoal[step] - car[sid].y;
						xprime = (double)car[sid].x + ((double)ygoal[step] - (double)car[sid].y) / m;
					}
				}
				if(xgoal.size()!=0){
					whereToGo(dist, m, xprime);
					int mode=intsec.pcessIntsc(car,lft,rht,top,btm,safetyline,id);
					control.setStopState(1);
					//bool isSafe=safetyCheck(sid);
					cout<<"MODE : "<<mode<<endl;
					if(1) {
						switch(mode){
						case 0: //cout<<"mode =0"<<endl;
							control.drive();
							break;
						case 1: //cout<<"mode =1"<<endl;
							control.setStopState(0);
							break;
						case 2: //cout<<"mode =2"<<endl;
							control.rl=0;
							break;
						}
					}
					else {//	cout<<"NOT SAFE"<<endl;
						control.setStopState(0);
						control.stop();
					}
				
				}
				else{	//cout<<"goal size =0"<<endl;
					 control.setStopState(0);
					 control.stop();
				}
			}
		}
	}
}
void whereToGo(int dist, double m, double xprime){
	if (dist<=DIST_TURN) {
		//huge turn
		if (abs(m)>V_HIGH || abs(m) <= V_LOW) { //1
			if (abs(m) >V_HIGH) {						//2
				if (car[sid].y_v > 0) {							//3
					cout << "huge-case 1" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = 2;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = -2;
					else control.rl = 0;
				}
				else {
					cout << "huge-case 2" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = -2;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = 2;
					else control.rl = 0;
				}
			}
			else {
				if (car[sid].x_v > 0) {	//6
					cout << "huge-case 3" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = 2;
					else if ((ygoal[step] - car[sid].y) < -P_TH)	control.rl = -2;
					else control.rl = 0;
				}
				else {
					cout << "huge-case 4" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = -2;
					else if ((ygoal[step] - car[sid].y) < -P_TH) control.rl = 2;
					else control.rl = 0;
				}
			}
		}
		else {
			if (car[sid].y_v > 0) {	//9
				cout << "huge-case 5" << endl;
				if ((xgoal[step] - xprime) < -P_TH)	control.rl = 2;
				else if ((xgoal[step] - xprime) > P_TH)	control.rl = -2;
				else control.rl = 0;
			}
			else {
				cout << "huge-case 6" << endl;
				if ((xgoal[step] - xprime < -P_TH)) control.rl = -2;
				else if ((xgoal[step] - xprime)>P_TH)	control.rl = 2;
				else control.rl = 0;
			}
		}			
	}
	else{
		//small turn
			if (abs(m)>V_HIGH || abs(m) <= V_LOW) { //1
			if (abs(m) >V_HIGH) {						//2
				if (car[sid].y_v > 0) {							//3
					cout << "small-case 1" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = 1;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = -1;
					else control.rl = 0;
				}
				else {
					cout << "small-case 2" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = -1;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = 1;
					else control.rl = 0;
				}
			}
			else {
				if (car[sid].x_v > 0) {	//6
					cout << "small-case 3" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = 1;
					else if ((ygoal[step] - car[sid].y) < -P_TH)	control.rl = -1;
					else control.rl = 0;
				}
				else {
					cout << "small-case 4" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = -1;
					else if ((ygoal[step] - car[sid].y) < -P_TH) control.rl = 1;
					else control.rl = 0;
				}
			}
		}
		else {
			if (car[sid].y_v > 0) {	//9
				cout << "small-case 5" << endl;
				if ((xgoal[step] - xprime) < -P_TH)	control.rl = 1;
				else if ((xgoal[step] - xprime) > P_TH)	control.rl = -1;
				else control.rl = 0;
			}
			else {
				cout << "small-case 6" << endl;
				if ((xgoal[step] - xprime < -P_TH)) control.rl = -1;
				else if ((xgoal[step] - xprime)>P_TH)	control.rl = 1;
				else control.rl = 0;
			}
		}			
	
	
	
	}



}


void printGoal(int id) {
	cout << "Car id : " << id << endl;
	cout << "x goal :";
	for (int i = 0; i<xgoal.size(); i++) {
		cout << xgoal[i] << " ";
	}
	cout << "		size of x : " << xgoal.size() << endl;
	cout << "y goal :";
	for (int i = 0; i<ygoal.size(); i++) {
		cout << ygoal[i] << " ";
	}
	cout << "		size of y : " << ygoal.size() << endl;

}


//KSLee
vector<string> split(string str, char delimiter){
	vector<string> internal;
	stringstream ss(str);
	string tok;

	while(getline(ss,tok,delimiter)){
		internal.push_back(tok);
	}
	return internal;
}
bool safetyCheck(int sid){
	int dist;
	int counter=0;
//	cout<<"SAFETYCHECK START"<<endl;
	for(int i=1; i<7; i++){
		counter++;
		if(i==sid)
			continue;
		dist =sqrt((car[sid].x-car[i].x)*(car[sid].x-car[i].x)+(car[sid].y-car[i].y)*(car[sid].y-car[i].y));
		if(dist<DIST_TH) {
			cout<<"NOT SAFE"<<endl;
			return false;	//Not safe			
		}
	}
	if(counter==6){
		cout<<"SAFE!"<<endl;
		return true; //Safe!
		
	}
}
