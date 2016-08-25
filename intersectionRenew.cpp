#include "intersectionRenew.h"

intersection::intersection(){
	currCarNum =	0;
	leftLine =  	-1;
	topLine =  		-1;
	rightLine = 	-1;
	bottomLine =	-1;
	sID = 			-1;
	safeLine = 		0;
	UseIntersection = false;
	car = NULL;
}

intersection::~intersection(){
	delete [] car;
}

void intersection::setIntersectionData
	(Car *currcar, int lx, int ly, int rx, int ry, 
	int safeline, int sid, int currCarNum, ofstream &LogCar)
{
	car = currcar;
	setCurrCarNum(currCarNum);
	setIntersectionArea(lx,ly,rx,ry);
	setSelfID(sid);
	setIntersectionLog(LogCar);
	printLog("intersection class init");
	UseIntersection = true;
}

void intersection::setIntersectionArea(int lx, int ly, int rx, int ry){
	leftLine = lx;
	topLine = ly;
	rightLine = rx;
	bottomLine = ry;
	UseIntersection = true;
}

void intersection::setSafeLine(int i){
	safeLine = i;
}

void intersection::setCurrCarNum(int i){
	currCarNum = i;
}

void intersection::setSelfID(int i){
	sID = i;
}

void intersection::setIntersectionLog(ofstream &LogCar){
	iLog = &LogCar;
}

bool intersection::isInIntersection(int selfID, int selfX, int selfY){
	if (leftLine - safeLine < selfX &&
		rightLine + safeLine > selfX &&
		bottomLine - safeLine < selfY &&
		topLine + safeLine > selfY){
		printLog("I'm in the intersection");
		return true;
	}
	else{
		printLog("I'm not in the intersection");
		return false;
	}
}

bool intersection::checkCarIntersection(Car *currCar, int currCarNum){
	intersectionQueue.clear();
	car = currCar;
	for(int i = 1; i < currCarNum+1 ; i++){   // HIGH ID MOVE FIRST
	//for (int i = currCarNum; i>0; i--){	// LOW ID MOVE FIRST
		if (leftLine < car[i].x &&
			rightLine > car[i].x &&
			bottomLine < car[i].y &&
			topLine > car[i].y)
			intersectionQueue.push_back(i);
	}
	char tot[200];
	sprintf(tot, "%s%d", "intersectionQueue Size is ", intersectionQueue.size());
	printLog(tot);
	if (intersectionQueue.size() >= 2)
		return true;
	else
		return false;
}

int intersection::checkPriority(){
	if (intersectionQueue.size() >= 2){
		char tot[200];
		int tID = intersectionQueue.back();
		sprintf(tot, "%s%d", "intersectionQueue priority is ", tID);
		printLog(tot);
		return tID;
	}
	else {
		printLog("There's no car in intersection");
		return -1;
	}
}

bool intersection::isPossibleCrossing(int id){
	int priorID = checkPriority();
	if (priorID == -1){
		cout << "There's no car in intersection, you can go" << endl;
		return true;
	}
	else if (id == priorID){
		cout << "case INTER : isPossibleCrossing / (id, priority) = (" << id << ", " << checkPriority() << ")" << endl;
		return true;
	}
	else
		return false;
}

void intersection::printLog(string str){
	cout << str << endl;
	*iLog << str << endl;
}




