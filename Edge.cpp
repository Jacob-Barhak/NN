
#include "stdafx.h"
#include "NeuralNet.h"

Edge::Edge(Neuron *N1, Neuron *N2,  int CurrentIteration){
	// use only two parameters the third parameter is for future use
	Neuron1=N1;
	Neuron2=N2;
	FaceCounter=0;
	CreationAge=CurrentIteration;
	//F1=NULL;
	//F2=NULL;

	FacesStuckHead=NULL;

	// add edge to neuron inner list
	NextInN1=N1->EdgeStuckHead; 
	N1->EdgeStuckHead=this; 
	NextInN2=N2->EdgeStuckHead; 
	N2->EdgeStuckHead=this; 

	// modify edge counter
	N1->EdgeCounter++;
	N2->EdgeCounter++;

	FanNumber=0;
	NotConsistantInDirection=false;

}

bool Edge::IsEdgeOfNeuron(Neuron *N){
	
	return ( Neuron1==N || Neuron2==N);

}


bool Edge::IsEdgeBetweenNeurons(Neuron *N1,Neuron *N2){

	return ( IsEdgeOfNeuron(N1) &&
		     IsEdgeOfNeuron(N2) );
}


Neuron *Edge::OtherNeuronInEdge(Neuron *N){
// returns the pointer to the Other neuron in the Edge which is not N
// returns NULL if N is not a neuron of the edge

	if (Neuron1==N) return (Neuron2);
	if (Neuron2==N) return (Neuron1);
	return (NULL);
}


Face *Edge::NextFaceInFaceStuck(Face *F){
// returns the next face connected to the edge
	if (this == F->Edge1)
		return F->NextFaceInE1;
	else if (this == F->Edge2)
		return F->NextFaceInE2;
	else if (this == F->Edge3)
		return F->NextFaceInE3;
	else
		MyFatalError ("Face mix up in edge face stuck");
	
	return NULL;
}

bool Edge::CheckFoldOverEdge (Neuron *N , double FoldCosAngle ){
// this function checks if the face that will be created using the neuron N and the edge E will not cause a fold
// a fold means that the angle between the new face and the other face of the edge is less then acos(FoldCosAngle) degrees 
// (the scalar product of the normals is less then FoldCosAngle that is set to 0 (90 degrees) by default
// the fuction returns true if no fold will be created.
// the function returns true if the edge has already two faces or a fold exists
// the function also returns false if the edge is not exstant (E==NULL) or if the face has no faces attached to it

	if (this==NULL) // edge is not existant yet
		return false;
	if (FaceCounter >1) // more then one face attached another face will create a fold
		return true;
	if (FaceCounter ==0) // if no face is connected to the edge there cannot be a fold
		return false;

	Neuron *N1=Neuron1;
	Neuron *N2=Neuron2;
	Face *F = FacesStuckHead;
	Neuron *FaceN = F->OtherNeuronInFace(N1,N2);
	if (FaceN==N) // this means the neuron is already a part of the face to be created - so no fold will occour if the face is refreshed
		return false;
	MyPoint Normal1 = N1->CalcNormalUsing3Points (N2,N);
	// now take the last neuron belonging to the other face and calc the normal using it
	MyPoint Normal2 = N1->CalcNormalUsing3Points (N2,FaceN);

	double CosAngle=Normal1*Normal2;

	return CosAngle>FoldCosAngle;

}


ostream& operator << ( ostream& os, Edge &E ){

	os << "E " << E.EdgeId;
	os << " " << E.Neuron1->NeuronId;
	os << " " << E.Neuron2->NeuronId;
	os << " " << E.Tag;   // combination of EdgeTypes
	os << " " << E.CreationAge;
	os << "\n";
	return os;
}


istream& operator >> ( istream& is, Edge &E ){
 // not implemented (maybe should be changed
	return is;
}


int Edge::CheckFaceDirectionConsistency(void){
// this function checks the consistency of face directions
// the retrun values of the function are:
// 0 - faces are consistant
// 1 - faces are inconsistant
// -1 - only one face is connected to the edge
// -2 - no faces are connected to the edge
// -3 - more then 2 faces connected to the edge
	if (FaceCounter==0) return -2;
	if (FaceCounter==1) return -1;
	if (FaceCounter>2) return -3;

	Face *F1,*F2;
	F1=FacesStuckHead;
	F2=NextFaceInFaceStuck(F1);
	Neuron *F1N1,*F1N2,*F1N3;
	F1->GetFaceNeurons(&F1N1,&F1N2,&F1N3);
	Neuron *F2N1,*F2N2,*F2N3;
	F2->GetFaceNeurons(&F2N1,&F2N2,&F2N3);

	// check direction of first face - we call the face being in the correct direction if
	// the neurons order at the edge are in the cyclic order  1 2 3 
	bool EdgeAgreesWithF1=(Neuron1==F1N1 && Neuron2==F1N2 ||
		Neuron1==F1N2 && Neuron2==F1N3 ||
		Neuron1==F1N3 && Neuron2==F1N1);
	// if the face is marked as flipped, flip it
	if (F1->FlipDirection) EdgeAgreesWithF1 = !EdgeAgreesWithF1; 

	bool EdgeAgreesWithF2=(Neuron1==F2N1 && Neuron2==F2N2 ||
		Neuron1==F2N2 && Neuron2==F2N3 ||
		Neuron1==F2N3 && Neuron2==F2N1);
	// if the face is marked as flipped, flip it
	if (F2->FlipDirection) EdgeAgreesWithF2 = !EdgeAgreesWithF2; 

	//if both faces are in the same direction of the edge - return inconsitency
	if (EdgeAgreesWithF1 == EdgeAgreesWithF2) 
		return 1;
	// otherwise return consistency
	
	return 0;
}

Face * Edge::OtherFaceOfEdge(Face *F){
// returns a pointer to the other face of the edge. if more then one face is connected to the edge or if less than two faces are connected to the face the function returns null
// if F is not a face of E, the function returns NULL
	if (FaceCounter<2 || FaceCounter>2 || !F->IsFaceOfEdge(this))
	return NULL;

	if (FacesStuckHead==F) 
		return NextFaceInFaceStuck(F);
	else
		return FacesStuckHead;
}

