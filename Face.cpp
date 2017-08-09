
#include "stdafx.h"
#include "NeuralNet.h"

Face::Face(Edge *E1, Edge *E2, Edge *E3,  int CurrentIteration){
	// use only three parameters the forth parameter is for future use
	Edge1=E1;
	Edge2=E2;
	Edge3=E3;

	NextFaceInE1 = E1->FacesStuckHead;
	NextFaceInE2 = E2->FacesStuckHead;
	NextFaceInE3 = E3->FacesStuckHead;

	E1->FacesStuckHead=this;
	E2->FacesStuckHead=this;
	E3->FacesStuckHead=this;

	/*
	// note the face at the edges (save up to two edges (manifold)
	if (E1->F1==NULL)
		E1->F1=this;
	else
		E1->F2=this;

	if (E2->F1==NULL)
		E2->F1=this;
	else
		E2->F2=this;

	if (E3->F1==NULL)
		E3->F1=this;
	else
		E3->F2=this;

   */

	E1->FaceCounter++;
	E2->FaceCounter++;
	E3->FaceCounter++;

	CreationAge=CurrentIteration;
	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
	Neuron1->FaceCounter++;
	Neuron2->FaceCounter++;
	Neuron3->FaceCounter++;

	FlipDirection=false;
	Marked=false;

	FanNumber=0;
	PatchNumber=0;

}


bool Face::IsFaceOfNeuron(Neuron *N){

	if (N==NULL)
		return false;
	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
	return ( Neuron1==N || Neuron2==N || Neuron3==N);

}


bool Face::IsFaceBetweenNeurons(Neuron *N1,Neuron *N2,Neuron *N3){

	return ( IsFaceOfNeuron(N1) &&
		     IsFaceOfNeuron(N2) &&
			 IsFaceOfNeuron(N3) );
}


bool Face::IsFaceOfEdge(Edge *E){


	if (E==NULL) 
		return false;
	Face *F=E->FacesStuckHead;
	while (F !=NULL){
		if (F==this)
			return true;
		F=E->NextFaceInFaceStuck(F);
	}

	return false;
}


bool Face::IsFaceBetweenEdges(Edge *E1,Edge *E2,Edge *E3){

	return ( IsFaceOfEdge(E1) &&
		     IsFaceOfEdge(E2) &&
			 IsFaceOfEdge(E3));
}



Neuron *Face::OtherNeuronInFace(Neuron *N, bool Other){
// returns the pointer to the Other neuron in the Face which is not N
// if Other is set (default not) it will reurn the other neuron than
// if Other was not set (there are two other neurons).
// returns NULL if N is not a neuron of the face

	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);

	if (Other){
		if (Neuron1==N) return (Neuron2);
		if (Neuron2==N) return (Neuron3);
		if (Neuron3==N) return (Neuron1);
	}
	else{
		if (Neuron1==N) return (Neuron3);
		if (Neuron2==N) return (Neuron1);
		if (Neuron3==N) return (Neuron2);
	}

	return (NULL);

}


Edge *Face::OtherEdgeInFace(Edge *E, bool Other){
// returns the pointer to the Other Edge in the Face which is not E
// if Other is set (default not) it will reurn the other Edge than
// if Other was not set (there are two other Edges).
// returns NULL if E is not an Edge of the Face

	if (Other){
		if (Edge1==E) return (Edge2);
		if (Edge2==E) return (Edge3);
		if (Edge3==E) return (Edge1);
	}
	else{
		if (Edge1==E) return (Edge3);
		if (Edge2==E) return (Edge1);
		if (Edge3==E) return (Edge2);
	}

	return (NULL);

}


Neuron *Face::OtherNeuronInFace(Neuron *N1, Neuron *N2){
// returns the pointer to the Other neuron in the face which is not N1 or N2
// returns NULL if N is not a neuron of the edge

	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);

	if (N1==Neuron1 && N2==Neuron2) return (Neuron3);
	if (N1==Neuron1 && N2==Neuron3) return (Neuron2);
	
	if (N1==Neuron2 && N2==Neuron1) return (Neuron3);
	if (N1==Neuron2 && N2==Neuron3) return (Neuron1);

	if (N1==Neuron3 && N2==Neuron1) return (Neuron2);
	if (N1==Neuron3 && N2==Neuron2) return (Neuron1);

	return (NULL);

}


Edge *Face::OtherEdgeInFace(Edge *E1, Edge *E2){
// returns the pointer to the Other Edge in the face which is not E1 or E2
// the function returns NULL if E1 and E2 are not edges of the Face
	if (E1==Edge1 && E2==Edge2) return (Edge3);
	if (E1==Edge1 && E2==Edge3) return (Edge2);
	
	if (E1==Edge2 && E2==Edge1) return (Edge3);
	if (E1==Edge2 && E2==Edge3) return (Edge1);

	if (E1==Edge3 && E2==Edge1) return (Edge2);
	if (E1==Edge3 && E2==Edge2) return (Edge1);

	return (NULL);

}


Edge *Face::OtherEdgeInFace(Edge *E, Neuron *N){
// returns the pointer to the Other Edge in the face which is not E1 and is connected to N
// returns NULL if N is not a neuron of the edge E
// no check is made that E is part of the face.
// this function s useful for finding fans around neuron N
	if (E->IsEdgeOfNeuron(N)){
		if (E != Edge1  && Edge1->IsEdgeOfNeuron(N)) return (Edge1);
		if (E != Edge2  && Edge2->IsEdgeOfNeuron(N)) return (Edge2);
		if (E != Edge3  && Edge3->IsEdgeOfNeuron(N)) return (Edge3);
	}
	return (NULL);
}


void Face::GetFaceNeurons(Neuron **N1,Neuron **N2,Neuron **N3){
	// we assume that everyy face is created by exactly 3 neurons
	// no check is made for that.
	*N1=Edge1->Neuron1;
	*N2=Edge1->Neuron2;
	if(Edge2->Neuron1 == *N1 || Edge2->Neuron1 == *N2)
		*N3=Edge2->Neuron2;
	else
		*N3=Edge2->Neuron1;
}


ostream& operator << ( ostream& os, Face &F ){

	os << "F " << F.FaceId;
	os << " " << F.Edge1->EdgeId;
	os << " " << F.Edge2->EdgeId;
	os << " " << F.Edge3->EdgeId;
	os << " " << F.Tag;   // combination of EdgeTypes
	os << " " << F.CreationAge;
	os << "\n";
	return os;
}


ostream& OutputToVrmlFormat ( ostream& os ,Face &F){
	Neuron *Neuron1,*Neuron2,*Neuron3;
	F.GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
	if (!F.FlipDirection){
		os << " " << (Neuron1->NeuronId -1) << ",";
		os << " " << (Neuron2->NeuronId -1) << ",";
		os << " " << (Neuron3->NeuronId -1) << ",";
	}
	else{
		os << " " << (Neuron3->NeuronId -1) << ",";
		os << " " << (Neuron2->NeuronId -1) << ",";
		os << " " << (Neuron1->NeuronId -1) << ",";
	}

	os << " " <<  -1 << ",";
	os << "\n";

	return os;
}

ostream& OutputToVrmlFormat ( ostream& os , Neuron *N1 ,Neuron *N2 ,Neuron *N3 ,Neuron *N4){

	// this function outputs a face to vrml format (the face can be wither triangular or quadrilateral
	os << " " << (N1->NeuronId -1) << ",";
	os << " " << (N2->NeuronId -1) << ",";
	os << " " << (N3->NeuronId -1) << ",";
	if (N4 != NULL)
		os << " " << (N4->NeuronId -1) << ",";

	os << " " <<  -1 << ",";
	os << "\n";

	return os;
}



istream& operator >> ( istream& is, Face &F ){
 // not implemented (maybe should be changed
	return is;
}


bool Face::IsNeuronInProjFace(Neuron &N){

	// this is similar to calculating barycentric coordinates,
	// but without the coordinates themselves
	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);

	MyPoint V1 = N.Proj - Neuron1->Proj;
	MyPoint V2 = N.Proj - Neuron2->Proj;
	MyPoint V3 = N.Proj - Neuron3->Proj;

	MyPoint X12 = V1^V2;
	MyPoint X23 = V2^V3;
	MyPoint X31 = V3^V1;

	double DotProduct1 = X12*X23;
	double DotProduct2 = X23*X31;
	double DotProduct3 = X12*X23;

	return ( DotProduct1 > 0.0 &&
			 DotProduct2 > 0.0 &&
			 DotProduct3 > 0.0 );

}


double Face::CalcFilterCriteria(){

	// calculates the filer criteria :
	// the angle between the Normal at the largerst angle vertex 
	// and the Normal of the face
	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);


	MyPoint V12 = *Neuron2 - *Neuron1;
	MyPoint V23 = *Neuron3 - *Neuron2;
	MyPoint V13 = *Neuron3 - *Neuron1;

	V12 = V12 *(1.0 / ~V12);
	V23 = V23 *(1.0 / ~V23);
	V13 = V13 *(1.0 / ~V13);


	MyPoint FaceNormal = V12 ^ V13;
	FaceNormal= FaceNormal * (1.0 /~ FaceNormal);

	Neuron *MaxNeuron = Neuron1;
	double CosMaxAngle = V12 * V13;

	double CosTempAngle = - V12 * V23;
	if (CosTempAngle < CosMaxAngle){
		MaxNeuron = Neuron2;
		CosMaxAngle = CosTempAngle;
	}

	CosTempAngle = V13 * V23;
	if (CosTempAngle < CosMaxAngle){
		MaxNeuron = Neuron3;
		CosMaxAngle = CosTempAngle;
	}


	FilterCriteria = fabs(MaxNeuron->Normal * FaceNormal);

	return FilterCriteria;

}

/* older - bugged version of this function

bool Face::CheckIfProjNeuronIsInProjFace(Neuron *N){
	// checks if the 2D projection of the neuron resides inside the projection of face F
	// the function assumes that projection for each neuron of the face and N is already calculated

	Neuron *N1 ,*N2 , *N3;
	GetFaceNeurons(&N1,&N2,&N3);
			
	MyPoint V12,V13,V23,V1Pt,V2Pt,V3Pt;

	V12= N2->Proj - N1->Proj;
	V12 = V12 * (1.0/~V12);
	V13= N3->Proj - N1->Proj;
	V13 = V13 * (1.0/~V13);
	V23= N3->Proj - N2->Proj;
	V23 = V23 * (1.0/~V23);
	V1Pt= N->Proj - N1->Proj;
	V1Pt = V1Pt * (1.0/~V1Pt);
	V2Pt= N->Proj - N2->Proj;
	V2Pt = V2Pt * (1.0/~V2Pt);
	V3Pt= N->Proj - N3->Proj;
	V3Pt = V3Pt * (1.0/~V3Pt);

	double CAng1,CAng2,CAng3 ,CAng12Pt,CAng23Pt,CAng31Pt;
	double A.A12Pt,A23Pt,A31Pt;

	// calc cosine of angles
	CAng1 = V12*V13;
	CAng2 = V23*(-V12);
	CAng3 = V13*V23;
	CAng12Pt = V1Pt*V12;
	CAng23Pt = V2Pt*(-V12);
	CAng31Pt = V3Pt*(-V13);

	if (CAng12Pt>CAng1 && CAng23Pt>CAng2)
		return true;
	
	return false;
}
*/

// new fixed version of this function
bool Face::CheckIfProjNeuronIsInProjFace(Neuron *N){
	// checks if the 2D projection of the neuron resides inside the projection of face F (without boundary)
	// the function assumes that projection for each neuron of the face and N is already calculated
	// the method is calculating barycentric coordinates and returns true if u,v,w are above 0.0 and below 1.0 
	// notice that neurons on the triangle boundary returns false

	Neuron *N1 ,*N2 , *N3;
	GetFaceNeurons(&N1,&N2,&N3);

			
	MyPoint V12,V13,V23,V1Pt,V2Pt,V3Pt;

	V12= N2->Proj - N1->Proj;
	V13= N3->Proj - N1->Proj;
	V23= N3->Proj - N2->Proj;
	V1Pt= N->Proj - N1->Proj;
	V2Pt= N->Proj - N2->Proj;
	V3Pt= N->Proj - N3->Proj;

	MyPoint A123,A12Pt,A23Pt,A31Pt;
	double U,V,W;

	// calc areas for barycntric coordinates
	A123 = V12^V13;
	A12Pt = V1Pt^V2Pt;
	A23Pt = V2Pt^V3Pt;
	A31Pt = V3Pt^V1Pt;

	U=A23Pt.Pos[2]/A123.Pos[2];
	V=A31Pt.Pos[2]/A123.Pos[2];
	W=A12Pt.Pos[2]/A123.Pos[2];


	if (U>0.0 && U<1.0 && V>0.0 && V<1.0 && W>0.0 && W<1.0)
		return true;
	
	return false;
}


Face *Face::NextManifoldFace(Edge *E){
	// the function returns the next face connected to the edge that is not the current face
	// the function return NULL if:
	// 1. E is not an edge of 'this' 
	// 2. the number of faces connected to E is not 2
	if (E->FaceCounter != 2) return NULL;
	if (!IsFaceOfEdge(E)) return NULL;

	Face *F=E->FacesStuckHead;
	if (F!=this) 
		return (F);
	else 
		return (E->NextFaceInFaceStuck(F));

}

bool Face::SetVertexNormalDirectionsToAgreeWithFaceOrientation(Neuron *N){
// this function sets the direction of the vertex normal to agree with the face, the function returns true if the normal was fliiped and false otherwise

	Neuron *N1 ,*N2 , *N3;
	GetFaceNeurons(&N1,&N2,&N3);

    MyPoint FaceNormal = N1->CalcNormalUsing3Points(N2,N3);
	double DotProduct = FaceNormal*N->Normal;
	//set the direction of the face
	if (FlipDirection != (DotProduct < 0.0)){
		N->Normal = - N->Normal;
		return true;
	}
	return false;
}

/* old version that is not used anymore
void Face::SetNormalDirectionsToAgreeWithNormalAtVertex(Neuron * N){
// This functions determines the direction of the face to agree with the direction of the normal in neuron N, afterwards the normals at the other two Neurons is set to agree with the neuron in the face 

	Neuron *N1 ,*N2 , *N3;
	GetFaceNeurons(&N1,&N2,&N3);

    MyPoint FaceNormal = N1->CalcNormalUsing3Points(N2,N3);
	double DotProduct = FaceNormal*N->Normal;
	//set the direction of the face
	FlipDirection = (DotProduct < 0.0);

	// now check if the other normals agree on the direction
	double DotProduct1 = FaceNormal*N1->Normal;
	if (( (DotProduct1 < 0.0) && !FlipDirection ) ||
	    ( (DotProduct1 > 0.0) &&  FlipDirection )   )
		N1->Normal= -N1->Normal;

	double DotProduct2 = FaceNormal*N2->Normal;
	if (( (DotProduct2 < 0.0) && !FlipDirection ) ||
	    ( (DotProduct2 > 0.0) &&  FlipDirection )   )
		N2->Normal= -N2->Normal;

	double DotProduct3 = FaceNormal*N3->Normal;
	if (( (DotProduct3 < 0.0) && !FlipDirection ) ||
	    ( (DotProduct3 > 0.0) &&  FlipDirection )   )
		N3->Normal= -N3->Normal;

}
*/