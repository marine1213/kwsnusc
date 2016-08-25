/*
You can use only function "pcessIntsc()" from this class. 
This function needs some arguments.
1. Car *car; => the poisition of all the car.
2. int left, right, top, bottom; => the postion of intersection.
3. int safeline => the distance between intersection & stopline.
4. int myId => my id.

This fucntion returns int.
0=ignore "pcessIntsc()" signal; 
1=stop; 
2=forward;
*/

#include "common.h"
#include "carStruct.h"

using namespace std;

class intersection{
public:
	intersection();
	~intersection();
	void setIntersectionData(Car *currcar, int lx, int ly, int rx, int ry, 
		int safeline, int sid, int currCarNum, ofstream &LogCar);
	void setIntersectionArea(int lx, int ly, int rx, int ry);
	void setCurrCarNum(int i);
	void setSafeLine(int i);
	void setSelfID(int i);
	bool isInIntersection(int selfID, int selfX, int selfY);
	bool checkCarIntersection(Car *currCar, int currCarNum);
	
	bool isPossibleCrossing(int id);
	void printIntersection(); // print Car id, num car, in intersection 
	
	
private:
	int currCarNum;
	int leftLine, topLine, rightLine, bottomLine;
	int sID;
	int safeLine;
	Car *car;
	bool UseIntersection;
	ofstream *iLog;
	vector<int> intersectionQueue;

	void printLog(string str);
	int  checkPriority(); // private? 
	void setIntersectionLog(ofstream &LogCar);
};
