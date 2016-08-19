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
	intersection():car(NULL),left(0),right(0),top(0),bottom(0),safeline(0),myId(0),cState(0),nState(0){};
	~intersection(){
		delete [] car;
	};
	int pcessIntsc(Car * car, int left, int right,int top,int bottom, int safeline, int myId);


private:
	int left,right,top,bottom;
	int myId;
	int safeline;
	Car *car;
	int cState, nState;

	bool isIntscSafe();
	bool isCarInIntsc();
	bool isFirstCarInSafe();
};
