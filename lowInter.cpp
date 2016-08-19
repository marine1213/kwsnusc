#include "intersection.h"

//1== stop, 2==forward
int intersection::pcessIntsc(Car * car, int left, int right,int top,int bottom, int safeline, int myId){

	this->left = left; this->right = right;this->top = top;this->bottom= bottom;
	this->safeline = safeline; 
	this->myId = myId;
	this->car = car;

	
	//state
	// 0 = check whether you are in intersection;
	// 1 = you stopped in front of intersection. check you can cross the intersection.
	// 2 = you are crossing the intersection. 

	int mode; // 0=ignore pcessIntsc signal; 1=stop; 2=forward


	switch(cState){
		case 0:
			if(isIntscSafe()){
				cout<<"Car is in the intersection - STOP"<<endl;
				cout<<endl;
				mode = 1;
				nState = 1;
			}else{
				cout<<"Car is not in the intersection-Ignore signal"<<endl;
				cout<<endl;
				mode = 0;
				nState = 0;
			}
			break;

		case 1:
			if(isCarInIntsc()){
				cout<<"There is other cars in the intersection-STOP"<<endl;
				cout<<endl;
				mode =1;
				nState = 1;
			}else{
				if(isFirstCarInSafe()){
					cout<<"The car has the highest ID-MOVE FORWARD"<<endl;
					cout<<endl;
					mode = 2;
					nState = 2;
				}else{
					cout<<"my car is not the highest ID-STOP"<<endl;
					cout<<endl;
					mode =1;
					nState = 1;
				}
			}
			break;

		case 2:
			if(isIntscSafe()){
				cout<<"still crossing...-Ignore signal"<<endl;
				cout<<endl;
				mode = 0;
				nState = 2;
			}else{
				cout<<"Got out of intersection-Ignore signal"<<endl;
				cout<<endl;
				mode =0;
				nState =0;
			}
			break;

	}
			cState = nState;
			return mode;
}
			


	
bool intersection::isIntscSafe(){
	if((left-safeline<car[myId].x && car[myId].x<right+safeline) &&(bottom-safeline<car[myId].y && car[myId].y < top+safeline)){return true;}
	else{return false;}
}

bool intersection::isCarInIntsc(){
	for(int i =1; i<7; i++){
		if(i==myId){continue;}
		if(left<car[i].x && car[i].x< right && bottom<car[i].y && car[i].y<top){return true;}
	}
	return false;
}


bool intersection::isFirstCarInSafe(){
	if(myId == 2) return true; //if my car id is 2, which means that my car has highest id, my car will go first.
	else return false;
}
