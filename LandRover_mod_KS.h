#include "common.h"
#define FORWARD_BACKWARD_U  8
#define FORWARD_BACKWARD_D  9
#define LEFT_RIGHT_U        10
#define LEFT_RIGHT_D        11
#define SPEED               12
#define START_STOP          13

//=================== Added ====================
#define ITR_PIN		    0

#define  ONE_THOUSAND	1000
#define  ONE_MILLION	1000000
//==============================================

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
};

//================================= Added Parameters =====================================
	enum WheelState {
		WLEFT,
		WRIGHT,
		WCENTER,
	};
	bool searchingCenterPoint;
	bool centerPointDetected;
	WheelState wState;
//========================================================================================
//================================= Added Functions ======================================
	void centerPointItr();
	void searchForCenterPoint(WheelState state);
	void setSearchingCenterPoint(bool b);
	void setCenterPointDetected(bool b);
	bool isSearchingForCenterPoint();
	bool isCenterPointDetected();
//========================================================================================