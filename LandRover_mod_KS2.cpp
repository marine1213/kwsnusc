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
	prev_rl=0;
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

void LandRover::drive(){
	switch (rl){
	case 1:
		cout << "SMALL Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		prev_rl=1;
		cout << "SMALL Turn Right end " << endl;
		break;

	case 2:
		cout << "HUGE Turn RIGHT" << endl;
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, HIGH);
		delay(huge_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		prev_rl=2;
		cout << "HUGE Turn Right end " << endl;
		break;

	case 0:
		//go straight
		cout << "Go STRAIGHT" << endl;
		switch(prev_rl){
			case 1:
				cout<<"prev state was small turn right"<<endl;
				digitalWrite(LEFT_RIGHT_U,HIGH);
				digitalWrite(LEFT_RIGHT_D,LOW);
				delay(small_turn);
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,LOW);
				prev_rl=0;
				break;
			case 2:
				cout<<"prev state was huge turn right"<<endl;
				digitalWrite(LEFT_RIGHT_U,HIGH);
				digitalWrite(LEFT_RIGHT_D,LOW);
				delay(huge_turn);
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,LOW);
				prev_rl=0;
				break;
			case 0:
				cout<<"prev state was go straight"<<endl;
				break;
			case -1:
				cout<<"prev state was small turn left"<<endl;
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,HIGH);
				delay(small_turn);
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,LOW);
				prev_rl=0;
				break;
			case -2:
				cout<<"prev state was small turn left"<<endl;
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,HIGH);
				delay(huge_turn);
				digitalWrite(LEFT_RIGHT_U,LOW);
				digitalWrite(LEFT_RIGHT_D,LOW);
				prev_rl=0;
				break;
		}
		break;

	case -1:
		//turn left
		cout << "SMALL Turn LEFT" << endl;
		digitalWrite(LEFT_RIGHT_U, HIGH);
		digitalWrite(LEFT_RIGHT_D, LOW);
		delay(small_turn);
		digitalWrite(LEFT_RIGHT_U, LOW);
		digitalWrite(LEFT_RIGHT_D, LOW);
		prev_rl=-1;
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
		prev_rl=-2;
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
