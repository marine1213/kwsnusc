#include "common.h"
#include "LandRover_mod_KS.h"
#include "intersectionRenew.h"
#include "carStruct.h"

// Variables for main (twocaseX.cpp)
#define V_HIGH				100.0
#define V_LOW				0.3333
#define DIST_TH				50		//dist
#define V_TH				10		//vector
#define P_TH				60 		//position thr
#define DIST_TURN			200		//decide magnitude of turn. MUST be bigger than DIST_TH
#define SAFETY_LINE_DIV		3		//safetyline = (rht - lft) / SAFETY_LINE_Div;

vector<int> xgoal;
vector<int> ygoal;
vector<string> splitedInput;
string data;								//save received data into string named 'data'
Car* car = new Car[MAX_NUM_CAR+1];			//save information of each car ID, list[0]=blank
intersection intsec;
char inputString;

#if RANGEROVER
LandRover control;
#elif BENZ
Benz control;
#endif

int car_state; 				// 0 wait, 1 intersection, 2 move
bool isDataFull;			// full data line from serial
bool isFirstPosValue;		// first pos data get
bool useIntersection;		// intersection 
bool targetDataDone;		// if 't' value end 
bool dataTimingSetting;		// do _run everytime we get data
bool isKeyboard;			// use keyboard

int id;				//car id
int x_before;		//for calculating x_v
int y_before;		//for calculating y_v
int sid;			//self id
int step = 0;		//goal
int count = 0;		//check if input has received
int lft;			//left line
int btm;			//bottom line
int rht;			//right line
int top;			//top line
int safetyline;		//(right-left)/3

// Variables for Serial
char device[] = "/dev/ttyUSB0";
//char device[] = "/dev/ttyAMA0";
unsigned long baud = 9600;
int fd;

// Variables for fileIO and System
ofstream LogCar;
time_t LogTime;

// System Setting // 
void init_System();
void init_Log();
void init_Serial();
void init_GPIO();
void get_keyboard();

void close_System();
void close_Log();
void close_Serial();

// main functions // 
void get_data();
void validate_data();
void run_car();

// auxiliaries for driving // 
void whereToGo(int dist, double m, double xprime);
bool safetyCheck(int sid);
bool isGoal();
void setSelfID(int i);

// Data getter and checker  //
vector<string> split(string str, char delimiter);
bool isPosValid(int i);
void printGoal(int id);

