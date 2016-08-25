#include "common.h"
#define FORWARD_BACKWARD_U  8
#define FORWARD_BACKWARD_D  9
#define LEFT_RIGHT_U        10
#define LEFT_RIGHT_D        11
#define SPEED               12
#define START_STOP          13

using namespace std;

class LandRover {
public:
	LandRover();
	void setup();
	void drive();
	void stop();
	void setStopState(int a);
	void moveBackward();
	void moveForward();
	int checkStopState();
	int rl;						//////////////////////////
								//2: HUGE turn right	//
								//1: SMALL turn right  //
								//0: Go Straight		//
								//-1: SMALL turn left	//
								//-2: HUGE turn left	//
								///////////////////////////
	int getSmallDelay();
	int getHugeDelay();
	void setSmallDelay(int i);
	void setHugeDelay(int i);
	void setLog(ofstream& LogCar);
	void PrintLog(string str);
	void setKeyboardUse(bool tf);
	//void fasterSpeed();
	//void slowerSpeed();
	//


private:
	int huge_turn;		//huge turn delay
	int small_turn;	//small turn delay
	//Control State
	int NEUTRAL;
	int FORWARD;
	int BACKWARD;
	int LEFT;
	int RIGHT;
	int STOP;
	int SPEED_CHANGE;
	//Stop Start State
	int STOP_STATUS;
	int START_STATUS;
	//Speed State
	int SPEED_LOW;
	int SPEED_MEDIUM;
	int SPEED_HIGH;
	//Speed Change Direction
	int SPEED_INCREASE;
	int SPEED_DECREASE;

	int ControlState;
	int SpeedChangeDir;
	int SpeedState;
	int StopState;

	//DIRECTION STATE
	int prev_rl;
	bool enlarged;

	//write Log
	ofstream* cLog;
	bool KeyboardMode; // to control Log data 
};