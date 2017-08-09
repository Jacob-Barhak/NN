#include "stdafx.h"
#include "NeuralNet.h"



MyPoint MyPoint::operator + (const MyPoint &P) const {

	const RealType *p1,*p2;
	RealType *p3;
	MyPoint Res;
	p1=Pos;
	p2=P.Pos;
	p3=Res.Pos;

	for (int i=0 ;i<3;i++)
		*(p3++) = *(p1++) + *(p2++);

	return Res;
}


MyPoint MyPoint::operator - () const {
	const RealType *p1;
	RealType *p2;
	MyPoint Res;
	p1=Pos;
	p2=Res.Pos;

	for (int i=0 ;i<3;i++)
		*(p2++) = - *(p1++);

	return Res;
}


MyPoint MyPoint::operator - (const MyPoint &P) const {

	MyPoint TempPt;
	TempPt = ( (*this) + (-P) );
	
	return TempPt;

}


RealType MyPoint::operator * (const MyPoint &P) const{
	// dot product
	const RealType *p1,*p2;
	RealType Res=0.0;
	p1=Pos;
	p2=P.Pos;

	for (int i=0 ;i<3;i++)
		Res += *(p1++) * *(p2++);

	return Res;

}

MyPoint MyPoint::operator * (RealType alpha) const{
	RealType *p;
	MyPoint Res = *this;
	p=Res.Pos;

	for (int i=0 ;i<3;i++,p++)
		*p = (*p) * alpha;

	return Res;
}

MyPoint MyPoint::operator ^ (RealType alpha) const{
	RealType *p;
	MyPoint Res;
	p=Res.Pos;


	for (int i=0 ;i<3;i++,p++)
		*p = pow(*p, alpha);

	return Res;
}

MyPoint MyPoint::operator ^ (const MyPoint &P) const {

	//cross operation between two vectors
	MyPoint Res;

	Res.Pos[0] = Pos[1]*P.Pos[2] - Pos[2]*P.Pos[1];
	Res.Pos[1] = Pos[2]*P.Pos[0] - Pos[0]*P.Pos[2];
	Res.Pos[2] = Pos[0]*P.Pos[1] - Pos[1]*P.Pos[0];

	return (Res);
}

bool MyPoint::operator == (const MyPoint &P) const {

	bool RetVal=true;
	const RealType *p1 = Pos;
	const RealType *p2=P.Pos;

	for (int i=0 ;i<3;i++,p1++ ,p2++)
		RetVal = RetVal && *p1 == (*p2);
		
	return RetVal;
}




RealType MyPoint::operator ~ () const {
	const RealType *p;
	RealType Res=0.0;
	p=Pos;

	for (int i=0;i<3;i++,p++)
		Res+= (*p) * (*p);
	return (sqrt(Res));
}


bool MyPoint::IsFinite(){
	const RealType *p;
	p=Pos;
	bool RetValue=false;

	for (int i=0;i<3;i++,p++)
		RetValue = RetValue || _finite(*p);

	return (RetValue);
}

MyPoint::MyPoint(const MyPoint &MinVals , const MyPoint &MaxVals){
	const RealType *p1,*p2;
	RealType *p3;
	p1=MinVals.Pos;
	p2=MaxVals.Pos;

	p3=this->Pos;

	for (int i=0;i<3;i++,p1++,p2++,p3++){ 
		RealType r=rand();
		r/=RAND_MAX;
		*p3=*p1 + r * (*p2-*p1);
	}

}


MyPoint::MyPoint(const RealType PtInit[3]){
	const RealType *p1;
	RealType *p2;
	p1=PtInit;
	p2=this->Pos;

	for (int i=0;i<3;i++,p1++,p2++) 
		*p2=*p1;
};	



MyPoint::MyPoint(){ // do nothing
}


void MyPoint::MaxValues(const MyPoint &Point){
	const RealType *p1;
	RealType *p2;
	p1=Point.Pos;
	p2=this->Pos;

	for (int i=0;i<3;i++,p1++,p2++) 
		if (*p1>*p2) *p2=*p1;
	
}




void MyPoint::MinValues(const MyPoint &Point){
	const RealType *p1;
	RealType *p2;
	p1=Point.Pos;
	p2=this->Pos;

	for (int i=0;i<3;i++,p1++,p2++) 
		if (*p1<*p2) *p2=*p1;
	
}


ostream& operator << ( ostream& os,  MyPoint &Pt ){
	
	for (int i=0;i<3;i++){
		os << Pt.Pos[i];
		if (i==2)
			os << endl;
		else
			os << " ";
	}
		
	return os;

}

istream& operator >> ( istream& is,  MyPoint &Pt ){

	for (int i=0;i<3;i++)
		is >> Pt.Pos[i];
	
	return is;
}

