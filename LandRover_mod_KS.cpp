#include "LandRover_mod_KS.h"

/*
FORWARD_BACKWARD_U  8
FORWARD_BACKWARD_D  9
LEFT_RIGHT_U        10
LEFT_RIGHT_D        11
SPEED               12
START_STOP          13
*/

/*
You can modify turn delay with huge_turn, small_turn!
*/


LandRover::LandRover() {
	///
	huge_turn=200;
	small_turn=100;
	///
	NEUTRAL = 0;
	FORWARD = 1;
	BACKWARD = 2;
	LEFT = 3;
	RIGHT = 4;
	STOP = 5;
	SPEED_CHANGE = 6;

	SPEED_LOW = 0;
	SPEED_MEDIUM = 1;
	SPEED_HIGH = 2;
	SPEED_INCREASE = 0;
	SPEED_DECREASE = 1;
	STOP_STATUS = 0;
	START_STATUS = 1;

	ControlState = NEUTRAL;
	SpeedChangeDir = SPEED_INCREASE;
	SpeedState = SPEED_LOW;
	StopState = START_STATUS;
	
	wState = WCENTER;
}



void LandRover::setStopState(int a) {
	if (a == 1) {
		digitalWrite(FORWARD_BACKWARD_U, HIGH);
		//StopState = START_STATUS;
		ControlState=FORWARD;
	}
	else if (a == 0) {
		//StopState = STOP_STATUS;
		digitalWrite(FORWARD_BACKWARD_U, LOW);
		ControlState=NEUTRAL;
	}
}
void LandRover::moveBackward(){
	cout<<"BACKWARD"<<endl;
	if(ControlState=FORWARD){
		digitalWrite(FORWARD_BACKWARD_U,LOW);
		digitalWrite(FORWARD_BACKWARD_D,LOW);
		delay(500);
	}
	digitalWrite(FORWARD_BACKWARD_U,LOW);
	digitalWrite(FORWARD_BACKWARD_D,HIGH);
	ControlState=BACKWARD;
}
void LandRover::moveForward(){
	cout<<"FORWARD"<<endl;
	if(ControlState==BACKWARD){
		digitalWrite(FORWARD_BACKWARD_U,LOW);
		digitalWrite(FORWARD_BACKWARD_D,LOW);
		delay(500);
	}
	digitalWrite(FORWARD_BACKWARD_U,HIGH);
	digitalWrite(FORWARD_BACKWARD_D,LOW);
	ControlState=FORWARD;

}
void LandRover::setup(){
	cout << "match pins" << endl;
	pinMode(FORWARD_BACKWARD_U, OUTPUT);
	pinMode(FORWARD_BACKWARD_D, OUTPUT);
	pinMode(LEFT_RIGHT_U, OUTPUT);
	pinMode(LEFT_RIGHT_D, OUTPUT);
	pinMode(SPEED, OUTPUT);
	pinMode(START_STOP, OUTPUT);
	cout << "pin MATCHED" << endl;

	digitalWrite(FORWARD_BACKWARD_U, LOW);
	digitalWrite(FORWARD_BACKWARD_D, LOW);
	digitalWrite(LEFT_RIGHT_U, LOW);
	digitalWrite(LEFT_RIGHT_D, LOW);

//============================== Added =================================
	if(wiringPiISR(ITR_PIN, INT_EDGE_FALLING, &centerPointItr)<0){
	    cout<< "unable to setup interrupt" <<endl;		
	}
//======================================================================
}
void LandRover::stop() {
	digitalWrite(START_STOP, HIGH);
	delay(200);
	digitalWrite(START_STOP, LOW);
	StopState++;
	StopState = StopState % 2;
	if (StopState == START_STATUS) cout << "START" << endl;
	else cout << "STOP" << endl;
	digitalWrite(LEFT_RIGHT_U,LOW);
	digitalWrite(LEFT_RIGHT_D,LOW);
}
int LandRover::checkStopState() {
	if (StopState == START_STATUS) return 1;
	else return 0;
}


/*
void LandRover::drive() {
	if (rl == 2) {
		cout << "HUGE Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(huge_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		cout << "HUGE Turn Right end " << endl;

	}
	else if (rl == 1) {
		cout << "SMALL Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		cout << "SMALL Turn Right end " << endl;

	}
	else if (rl == -1) {
		//turn left
		cout << "SMALL Turn LEFT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, HIGH);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "SMALL Turn LEFT signal end " << endl;

	}
	else if (rl == -2) {
		//turn left
		cout << "HUGE Turn LEFT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(huge_turn);
		digitalWrite(LEFT_RIGHT_U, HIGH);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "HUGE Turn LEFT signal end " << endl;

	}
	else if (rl == 0) {
		//go straight
		cout << "Go STRAIGHT" << endl;
	}

}
*/


void LandRover::drive(){
	switch (rl){
	case 1:
		cout << "SMALL Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "SMALL Turn Right end " << endl;
		break;

	case 2:
		cout << "HUGE Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		delay(huge_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "HUGE Turn Right end " << endl;
		break;

	case 0:
		//go straight
		cout << "Go STRAIGHT" << endl;
//====================== Added ==========================
		searchForCenterPoint(wState);
		wState = WCENTER;
//=======================================================
		break;

	case -1:
		//turn left
		cout << "SMALL Turn LEFT" << endl;
		digitalWrite(LEFT_RIGHT_U, HIGH);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "SMALL Turn LEFT signal end " << endl;
		break;

	case -2:
		//turn left
		cout << "HUGE Turn LEFT" << endl;
		digitalWrite(LEFT_RIGHT_U, HIGH);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(huge_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		cout << "HUGE Turn LEFT signal end " << endl;
		break;
	}
}


int LandRover::getSmallDelay(){
	return small_turn;
}
int LandRover::getHugeDelay(){
	return huge_turn;
}
void LandRover::setSmallDelay(int i){
	small_turn = i;
	cout << "set small delay to " << i << endl;
}
void LandRover::setHugeDelay(int i){
	huge_turn = i;
	cout << "set huge delay to " << i << endl;
}

//====================== Added ==========================
/*
 * Return different of time from now to last marked time
 */ 
long deltaTime(struct timespec last){
    struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	long nsec = now.tv_nsec - last.tv_nsec;
	time_t sec = now.tv_sec - last.tv_sec;
	long period = sec * ONE_THOUSAND + nsec / ONE_MILLION;
    return period;
}

/*
 * compare running time with exceeding time
 * If different <= 0 then return 0
 * If different > 0 then return different time
 */
long checkTime(struct timespec last,long exceedingTime){
    long runTime  = deltaTime(last);
    long overTime = runTime - exceedingTime;
    return overTime <= 0? 0 : overTime;
}


void searchForCenterPoint(WheelState state){
	struct timespec last;
	clock_gettime(CLOCK_REALTIME, &last);

	setSearchingCenterPoint(true);
	setCenterPointDetected(false);

	switch(wState){
	    case WLEFT:
		while(!checkTime(last,1000) && !isCenterPointDetected()){ // turn right
		    digitalWrite(LEFT_RIGHT_U, LOW);
		    digitalWrite(LEFT_RIGHT_D, HIGH);
		    delay(100);
		    digitalWrite(LEFT_RIGHT_U, LOW);
		    digitalWrite(LEFT_RIGHT_D, LOW);
		}
	    break;
	    case WRIGHT:
		while(!checkTime(last,1000) && !isCenterPointDetected()){ // turn left
		    digitalWrite(LEFT_RIGHT_U, HIGH);
		    digitalWrite(LEFT_RIGHT_D, LOW);
		    delay(100);
		    digitalWrite(LEFT_RIGHT_U, LOW);
		    digitalWrite(LEFT_RIGHT_D, LOW);
		}		
	    break;
	    case WCENTER:
		printf("The wheel has already been center");
	    break;
	}
	setSearchingCenterPoint(false);
}
void centerPointItr(){
	if(isSearchingForCenterPoint()){
		setCenterPointDetected(true);
	}
}
void setSearchingCenterPoint(bool b){
	searchingCenterPoint = b;
}
void setCenterPointDetected(bool b){
	centerPointDetected = b;
}
bool isSearchingForCenterPoint(){
	return searchingCenterPoint;
}
bool isCenterPointDetected(){
	return centerPointDetected;
}
//=======================================================