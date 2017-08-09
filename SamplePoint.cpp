#include "stdafx.h"
#include "NeuralNet.h"


//SamplePoint::SamplePoint(const MyPoint &Point):MyPoint(Point){
//	Weight=1.0;
//}

SamplePoint::SamplePoint(const MyPoint &Point, RealType InitWeight):MyPoint(Point){
	Weight=InitWeight;
	Classification=NULL;
	Next=NULL;
}

SamplePoint::SamplePoint(){ //default constructor
	Next=NULL;
	Classification=NULL;

}

ostream& operator << ( ostream& os,  SamplePoint &Pt ){

	//first output the weight (if not 1.0)
	if (Pt.Weight != 1.0)
		os << "W " << Pt.Weight;

	//then output the point
	MyPoint TempPt;
	TempPt=Pt;
	os << TempPt;
	return os;

}


istream& operator >> ( istream& is,  SamplePoint &Pt ){

	//first read the weight
	char c;
	RealType Weight;
	

	is>>ws;
	c=is.peek();
	if  (c=='W')
		is >> c >> Weight; // read chracter and val
	else
		Weight=1.0;
	//then read the point
	MyPoint TempPt;
	
	is >> TempPt;

	Pt= static_cast <SamplePoint> (TempPt);

	
	return is;

}
