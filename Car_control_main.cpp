#include "common.h"
#include "Car_control_main.h"

using namespace std;

int main(void){
	init_System();

	while(1){
		if(isKeyboard)get_keyboard();
		if(!isKeyboard){
			get_data();
			validate_data();
			run_car();
		}
	}
	close_System();

	return 0;
}

void init_System(){
	time(&LogTime);
	step = 0;
	car_state = 0; // wait
	isDataFull = false;
	isFirstPosValue = true;
	useIntersection = false;
	targetDataDone = false;
	dataTimingSetting = false;
	isKeyboard=true;

	setSelfID(1);

	init_Log();
	init_Serial();
	init_GPIO();
	control.setup();
	control.setLog(LogCar);
	control.setKeyboardUse(true); // KSLee to control Log data, set variable

}
void close_System(){
	close_Log();
	close_Serial();
}

void init_Log(){
	struct tm *now = localtime(&LogTime);
   	char FileNAME[200];
   	sprintf(FileNAME, "%s%0.2d%0.2d%0.2d%s", "Log",now->tm_mday,now->tm_hour,now->tm_min, ".txt");
   	LogCar.open(FileNAME);
   	printf("Done : init_Log, file open\n");
}

void init_Serial(){
	if ((fd = serialOpen(device, baud))<0) {
		cout << "Unable to open serial device" << endl;
		return;
	}
	printf("Done : init_Serial, serial open\n");
}

void init_GPIO(){
	if (wiringPiSetup() == -1) {
		cout << "Unable to start wiringPi" << endl;
		return;
	}
	printf("Done : init_GPIO, wiringPi open\n");
}

void close_Log(){
	LogCar.close();
}
void close_Serial(){
	serialClose(fd);
}
void get_keyboard(){
	cout<<"KEYBOARD MODE - press key :"<<endl;
	cin>>inputString;
	if(inputString=='W'||inputString=='w') control.moveForward();
	else if(inputString=='S'||inputString=='s') control.moveBackward();
	else if(inputString=='A'||inputString=='a') {
		control.rl=-1;
		control.drive();
	}
	else if(inputString=='D'||inputString=='d'){
		control.rl=1;
		control.drive();
	}
	else if(inputString=='R'||inputString=='r') {
		control.setStopState(0);
		control.stop();
	}
	else if(inputString=='P'||inputString=='p') {
		isKeyboard=false;
		control.rl=0;
		control.setKeyboardUse(false); // KSLee
	}
	else cout<<"Unknown Command"<<endl;
}

void get_data(){
	if (fd < 0){
		cout << "ERROR : serial closed" << endl;
		LogCar << "ERROR : serial closed" << endl;
		return;
	}
	if (serialDataAvail(fd)){
		char newChar = serialGetchar(fd);
		if (newChar != '\n') data += newChar;
		if (newChar == '\r') {
			cout << "input data : " << data << endl;
			LogCar << "input data : " << data << endl;
			splitedInput = split(data, ',');
			isDataFull = true;
			data.clear();
		}
	}
}

void validate_data(){
	if (isDataFull){
		//cout << "splited Input size is : " << splitedInput.size() << endl;
		//cout << "[0][0] value is : " << splitedInput[0][0] << endl;

		char dataType = splitedInput[0][0]; // check the data is 'char'
		//char dataType = splitedInput[0].c_str();
		bool validator = false;

		switch (dataType){
		case 't': // TODO check count of number of t id
			if (splitedInput.size() == 4){ // check size and input type
				validator
					= isPosValid(atoi(splitedInput[2].c_str()))
					& isPosValid(atoi(splitedInput[3].c_str()));
			}
			else{
				cout << "ERROR : case T : not enough target data" << endl;
				LogCar << "ERROR : case T : not enough target data" << endl;
			}

			if (validator){
				int getid = atoi(splitedInput[1].c_str());
				if (sid == getid){ // save target position of matched one
					xgoal.push_back(atoi(splitedInput[2].c_str()));
					ygoal.push_back(atoi(splitedInput[3].c_str()));
					LogCar << "T case / id : " << getid
						<< ", x : " << splitedInput[2]
						<< ", y : " << splitedInput[3]
						<< endl;
				}
				else{
					cout << "WARNING : case T : not my target data" << endl;
				}
			}
			else{ // validator false case
				cout << "ERROR : case T : false target position data" << endl;
				LogCar << "ERROR : case T : false target position data" << endl;
			}
			isDataFull = false;
			break; // T case end

		case 'p':
			if (splitedInput.size() == 4){
				validator
					= isPosValid(atoi(splitedInput[2].c_str()))
					& isPosValid(atoi(splitedInput[3].c_str()));
			}
			else{
				cout << "ERROR : case P : not enough position data" << endl;
				LogCar << "ERROR : case P : not enough position data" << endl;
			}

			if (validator){
				int id = atoi(splitedInput[1].c_str());
				if (id == sid){ // in my position, need to calculate vector
					if (isFirstPosValue){ // initial car vector setting
						car[id].x = atoi(splitedInput[2].c_str());
						car[id].y = atoi(splitedInput[3].c_str());
						if (isGoal()){
							car[id].x_v = xgoal[step] - car[id].x;
							car[id].y_v = ygoal[step] - car[id].y;
							x_before = 1;
							//y_before = 1;
							isFirstPosValue = false;
						}
						else{
							car[id].x_v = 0;
							car[id].y_v = 0;
							cout << "waiting for target data" << endl;
							LogCar << "waiting for target data" << endl;
						}

					}
					else{
						x_before = car[id].x;
						y_before = car[id].y;
						car[id].x = atoi(splitedInput[2].c_str());
						car[id].y = atoi(splitedInput[3].c_str());
						car[id].x_v = car[id].x - x_before;
						car[id].y_v = car[id].y - y_before;
					}
				}
				else{ // if not my car position, don't need to calculate vector
					car[id].x = atoi(splitedInput[2].c_str());
					car[id].y = atoi(splitedInput[3].c_str());
					car[id].x_v = 0, car[id].y_v = 0;
				}

				cout << "P case / (id,x,y,xv,yv) = (" << id << ", " << car[id].x << ", " << car[id].y << ", "
					<< car[id].x_v << ", " << car[id].y_v << ")" << endl;
				LogCar << "P case / (id,x,y,xv,yv) = (" << id << ", " << car[id].x << ", " << car[id].y << ", "
					<< car[id].x_v << ", " << car[id].y_v << ")" << endl;
			}
			else{ // validator false case
				cout << "ERROR : case P : false detected position data" << endl;
				LogCar << "ERROR : case P : false detected position data" << endl;
			} // if validator end

			isDataFull = false;
			dataTimingSetting = true;
			break; // P case end

		case 'i':
			if (splitedInput.size() == 5){ // check size and input type
				validator
					= isPosValid(atoi(splitedInput[1].c_str()))
					& isPosValid(atoi(splitedInput[2].c_str()))
					& isPosValid(atoi(splitedInput[3].c_str()))
					& isPosValid(atoi(splitedInput[4].c_str()));
			}
			else{
				cout << "ERROR : case I : not enough intersection pos data" << endl;
				LogCar << "ERROR : case I : not enough intersection pos data" << endl;
			}

			if (validator){
				lft = atoi(splitedInput[1].c_str());
				btm = atoi(splitedInput[2].c_str());
				rht = atoi(splitedInput[3].c_str());
				top = atoi(splitedInput[4].c_str());
				useIntersection = true;
				safetyline = (rht - lft) / SAFETY_LINE_DIV;
				intsec.setIntersectionData(NULL, lft, top, rht, btm, 
					safetyline, sid, MAX_NUM_CAR, LogCar);
				cout << "LEFT : " << lft << ", " << "RIGHT : " << rht << endl;
				cout << "TOP  : " << top << ", " << "BOTTOM : " << btm << endl;
				LogCar << "LEFT : " << lft << ", " << "RIGHT : " << rht << endl;
				LogCar << "TOP  : " << top << ", " << "BOTTOM : " << btm << endl;
				
			}
			else{
				cout << "ERROR : case I : false intersection pos data" << endl;
				LogCar << "ERROR : case I : false intersection pos data" << endl;
			}

			isDataFull = false;
			break;

		case 'r':
			//TODO stop or wait signal
			if (control.checkStopState()) {
				//control.stop();
				control.setStopState(0);
			}
			x_before = -1;
			//y_before = -1;
			step = 0;
			xgoal.clear();
			ygoal.clear();
			for (int i = 1; i<7; i++){
				car[i].x = -100; car[i].y = -100;
			}
			cout << "R case : controller Reset / (xsize, ysize) = ("
				<< xgoal.size() <<", " << ygoal.size() << ")" << endl;
			LogCar << "R case : controller Reset / (xsize, ysize) = ("
				<< xgoal.size() << ", " << ygoal.size() << ")" << endl;
			dataTimingSetting = false;
			targetDataDone = false;
			isDataFull = false;
			break;

		case 'e': // eot?
			//TODO target send end signal
			printGoal(sid); // TODO : for iteration for every id
			targetDataDone = true;
			isDataFull = false;
			dataTimingSetting = true;
			break;

		case 'x': // eop?
			cout << "X, EOT case" << endl;
			LogCar << "X, EOT case" << endl;
			isDataFull = false;
			break;

		default:
			isDataFull = false;
			break;
		}
	}
}

void run_car(){
	if (!dataTimingSetting){ // to make the sync between serial input & running
		dataTimingSetting = false;
		return;
	}

	if (targetDataDone){ // after T data sending ended
		if (!isGoal()){
			cout << "WARNING : no target data, remain wait state" << endl;
			LogCar << "WARNING : no target data, remain wait state" << endl;
			dataTimingSetting = false;
			return ;
		}

		if (car[sid].x > 0 && x_before>0) {	//at least one position data required
			//cout << "run_car function started" << endl;
			//cout << "get car x,y and x before" << endl;

			//let goal position would be send first

			//It would be ok even if y_v becomes 0
			//let x_v=0.0001.
			if (car[sid].x_v == 0) car[sid].x_v = 0.0001;
			double m = car[sid].y_v / car[sid].x_v;

			double xprime = (double)car[sid].x + ((double)ygoal[step] - (double)car[sid].y) / m;
			int xd = xgoal[step] - car[sid].x;
			int yd = ygoal[step] - car[sid].y;
			int dist = sqrt(xd*xd + yd*yd);
			if (dist <= DIST_TH) {
				if (step == xgoal.size() - 1) {
					cout << "End of the trip!" << endl;
					LogCar << "End of the trip!" << endl;
					if (control.checkStopState()) control.stop();
					xd = 0;	yd = 0;
					//vector<int>().swap(xgoal);	vector<int>().swap(ygoal);	//releasing memory
					xgoal.clear();
					ygoal.clear();
					delete[] car;            //serialClose(fd);
				}
				else {
					step++;
					cout << "Go to next step : " << step << endl;
					LogCar << "Go to next step : " << step << endl;
					xd = xgoal[step] - car[sid].x;
					yd = ygoal[step] - car[sid].y;
					dist=sqrt(xd*xd+yd*yd);
					xprime = (double)car[sid].x + ((double)ygoal[step] - (double)car[sid].y) / m;
				}
			}
			cout << "(sid, dist, M, X) = (" << sid << "," << dist << "," << m << "," << xprime << ")" << endl;
			LogCar << "(sid, dist, M, X) = (" << sid << "," << dist << "," << m << "," << xprime << ")" << endl;

			if (isGoal()){
				whereToGo(dist, m, xprime);
				//bool isSafe = safetyCheck(sid);
				bool isSafe = true;
				control.setStopState(1);

				if (useIntersection){ // use Intersection case	
					if (isSafe && intsec.isInIntersection(sid, car[sid].x, car[sid].y)){ // I'm in intersection
						intsec.checkCarIntersection(car, MAX_NUM_CAR);
						if (intsec.isPossibleCrossing(sid)){ // no car or my case
							LogCar << "case INTER : possible crossing" << endl;
							cout << "case INTER : possible crossing" << endl;
							control.drive();
						}
						else{ // don't move
							LogCar << "case INTER : impossible crossing" << endl;
							cout << "case INTER : impossible crossing" << endl;
							control.setStopState(0);
							control.stop();
						}							
					}
					else{ // I'm not in intersection
						control.drive();
					}
				} // not use intersection case
				else{
					if (isSafe) control.drive();
					else {
						control.setStopState(0);
						control.stop();
					}
				}
			}
			else{ // goal size 0
				targetDataDone = false;
				control.setStopState(0);
				control.stop();
			}
		}
	}
	dataTimingSetting = false;
	LogCar << endl;
	cout << endl;
}

vector<string> split(string str, char delimiter){
	vector<string> internal;
	stringstream ss(str);
	string tok;

	while (getline(ss, tok, delimiter)){
		internal.push_back(tok);
	}
	return internal;
}

void setSelfID(int i){
	sid = i;
}

bool isPosValid(int i){
	if (i > 0)
		return true;
	else
		return false;
}

void whereToGo(int dist, double m, double xprime){
	if (dist <= DIST_TURN) {
		//huge turn
		if (abs(m)>V_HIGH || abs(m) <= V_LOW) { //1
			if (abs(m) >V_HIGH) {						//2
				if (car[sid].y_v > 0) {							//3
					cout << "huge-case 1" << endl;
					LogCar << "huge-case 1" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = 2;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = -2;
					else control.rl = 0;
				}
				else {
					cout << "huge-case 2" << endl;
					LogCar << "huge-case 2" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = -2;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = 2;
					else control.rl = 0;
				}
			}
			else {
				if (car[sid].x_v > 0) {	//6
					cout << "huge-case 3" << endl;
					LogCar << "huge-case 3" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = 2;
					else if ((ygoal[step] - car[sid].y) < -P_TH)	control.rl = -2;
					else control.rl = 0;
				}
				else {
					cout << "huge-case 4" << endl;
					LogCar << "huge-case 4" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = -2;
					else if ((ygoal[step] - car[sid].y) < -P_TH) control.rl = 2;
					else control.rl = 0;
				}
			}
		}
		else {
			if (car[sid].y_v > 0) {	//9
				cout << "huge-case 5" << endl;
				LogCar << "huge-case 5" << endl;
				if ((xgoal[step] - xprime) < -P_TH)	control.rl = 2;
				else if ((xgoal[step] - xprime) > P_TH)	control.rl = -2;
				else control.rl = 0;
			}
			else {
				cout << "huge-case 6" << endl;
				LogCar << "huge-case 6" << endl;
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
					LogCar << "small-case 1" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = 1;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = -1;
					else control.rl = 0;
				}
				else {
					cout << "small-case 2" << endl;
					LogCar << "small-case 2" << endl;
					if ((xgoal[step] - car[sid].x) < -P_TH)	control.rl = -1;
					else if ((xgoal[step] - car[sid].x) > P_TH) control.rl = 1;
					else control.rl = 0;
				}
			}
			else {
				if (car[sid].x_v > 0) {	//6
					cout << "small-case 3" << endl;
					LogCar << "small-case 3" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = 1;
					else if ((ygoal[step] - car[sid].y) < -P_TH)	control.rl = -1;
					else control.rl = 0;
				}
				else {
					cout << "small-case 4" << endl;
					LogCar << "small-case 4" << endl;
					if ((ygoal[step] - car[sid].y) > P_TH)	control.rl = -1;
					else if ((ygoal[step] - car[sid].y) < -P_TH) control.rl = 1;
					else control.rl = 0;
				}
			}
		}
		else {
			if (car[sid].y_v > 0) {	//9
				cout << "small-case 5" << endl;
				LogCar << "small-case 5" << endl;
				if ((xgoal[step] - xprime) < -P_TH)	control.rl = 1;
				else if ((xgoal[step] - xprime) > P_TH)	control.rl = -1;
				else control.rl = 0;
			}
			else {
				cout << "small-case 6" << endl;
				LogCar << "small-case 6" << endl;
				if ((xgoal[step] - xprime < -P_TH)) control.rl = -1;
				else if ((xgoal[step] - xprime)>P_TH)	control.rl = 1;
				else control.rl = 0;
			}
		}
	}
}
bool safetyCheck(int sid){
	int dist;
	int counter = 0;
	cout << "SAFETYCHECK : ";
	LogCar << "SAFETYCHECK : ";
	for (int i = 1; i<7; i++){
		counter++;
		if (i == sid)
			continue;
		dist = sqrt((car[sid].x - car[i].x)*(car[sid].x - car[i].x) + (car[sid].y - car[i].y)*(car[sid].y - car[i].y));
		if (dist<DIST_TH) {
			cout << "NOT SAFE" << endl;
			LogCar << "NOT SAFE" << endl;
			return false;	//Not safe
		}
	}
	if (counter == 6){
		cout << "SAFE!" << endl;
		LogCar << "SAFE!" << endl;
		return true; //Safe!

	}
}
void printGoal(int id) {
	cout << "Car id : " << id << endl;
	cout << "x goal :";
	LogCar << "Car id : " << id << endl;
	LogCar << "x goal :";
	for (int i = 0; i<xgoal.size(); i++) {
		cout << xgoal[i] << " ";
		LogCar << xgoal[i] << " ";
	}
	cout << "// size of x : " << xgoal.size() << endl;
	cout << "y goal :";
	LogCar << "// size of x : " << xgoal.size() << endl;
	LogCar << "y goal :";
	for (int i = 0; i<ygoal.size(); i++) {
		cout << ygoal[i] << " ";
		LogCar << ygoal[i] << " ";
	}
	cout << "// size of y : " << ygoal.size() << endl;
	LogCar << "// size of y : " << ygoal.size() << endl;

}

bool isGoal(){
	if (xgoal.size() == 0 || ygoal.size() == 0){
		return false;
	}
	else{
		return true;
	}
}



