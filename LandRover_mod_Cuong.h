#include "common.h"
#define FORWARD_BACKWARD_U  8
#define FORWARD_BACKWARD_D  9
#define LEFT_RIGHT_U        10
#define LEFT_RIGHT_D        11
#define SPEED               12
#define START_STOP          13

#define ITR_PIN				0

using namespace std;

class LandRover {
public:
	LandRover();
	void setup();
	void drive();
	void stop();
	void setStopState(int a);
	int checkStopState();
	int rl;						//////////////////////////
								//2: HUGE turn right	//
								//1: SMALL turn right  //
								//0: Go Straight		//
								//-1: LITTLE turn left	//
								//-2: HUGE turn left	//
								///////////////////////////
	int getSmallDelay();
	int getHugeDelay();
	void setSmallDelay(int i);
	void setHugeDelay(int i);
	//void fasterSpeed();
	//void slowerSpeed();

	//================================= Added Functions ==============================================
	void searchForCenterPoint(WheelwState wState);
	void setSearchingCenterPoint(bool b);
	void setCenterPointDetected(bool b);
	void isSearchingCenterPoint();
	void isCenterPointDetected();
	//=================================================================================================
	
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

	int SpeedChangeDir;
	int SpeedState;
	int StopState;
	
	//================================= Added Parameters ===========================================
	bool searchingCenterPoint;
	bool centerPointDetected;
	//=================================================================================================
};