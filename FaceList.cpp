
#include "stdafx.h"
#include "NeuralNet.h"

 
int FaceList::DeleteOld(int Age, bool MarkNeurons){ 
	// the function returns the number of deleted edges
	// use deletion age to delete by edge age, 
	// if MarkNeurons is set then neurons with no edges
	// are marked for further processing later
	int OldSize=Size;
	Face *TempFace=Head;

	for (int i=0; i<OldSize ; i++){
		if ( TempFace->CreationAge > Age  ){
			TempFace=DelFace(TempFace,MarkNeurons);
		}
		else TempFace=TempFace->Next;
	}
	return (OldSize - Size);
}

FaceList::FaceList(){
	DispListNumber=0;
	Size=0;
	Head=NULL;
}

FaceList::~FaceList(){
	// delete the list - notice that the lsit has to be deleted before the neuron list, 
	// otherwise the delete statement may conclude at an access violation
	while (Head!=NULL){
		DelFace(Head);
	}
#ifdef MY_DEBUG
	//debug check
	if (Size!=0) MyFatalError("FaceList::~FaceList - inconsistency detected while destructing face list");
#endif

}

int FaceList::Age(Neuron *N = NULL){

	// all the faces connected to the Neuron are made older !!
	// if no input is spesified, all faces are aged

	int FacesAged=0;
	Face *TempFace=Head;
	if (Head != NULL) do{
		if (TempFace->IsFaceOfNeuron(N) || N==NULL){
			TempFace->CreationAge++;
			FacesAged++;
		}
		TempFace=TempFace->Next;
	} while (TempFace!=Head);

	return FacesAged;
}




Face *FaceList::IsFace(Neuron *N1,Neuron *N2,Neuron *N3){

	Edge *E12;
	Edge *E13;
	Edge *E23;

	// first convert to edges and if edges don't exist exit with NULL (no face can be defined with no edges )

	Edge *TempEdge=N1->EdgeStuckHead;
	while (TempEdge != NULL) {
		if ( TempEdge->IsEdgeOfNeuron(N2) )
			break;
		
		TempEdge=N1->NextEdgeInEdgeStuck(TempEdge);
	}

	E12=TempEdge;
	if (E12==NULL) return NULL;


	TempEdge=N1->EdgeStuckHead;
	while (TempEdge!=NULL) {
		if ( TempEdge->IsEdgeOfNeuron(N3) )
			break;
		
		TempEdge=N1->NextEdgeInEdgeStuck(TempEdge);
	}

	E13=TempEdge;
	if (E13==NULL) return NULL;


	TempEdge=N2->EdgeStuckHead;
	while (TempEdge!=NULL){
		if ( TempEdge->IsEdgeOfNeuron(N3) ){
			break;
		}
		TempEdge=N2->NextEdgeInEdgeStuck(TempEdge);
	}

	E23=TempEdge;
	if (E23==NULL) return NULL;

	// now look if such a face really exist
	return IsFace(E12,E13,E23);

}


Face *FaceList::IsFace(Edge *E1,Edge *E2,Edge *E3){

	Face *TempFace=E1->FacesStuckHead;
	while (TempFace!=NULL){
		if ( TempFace->IsFaceBetweenEdges(E1,E2,E3))
			return TempFace;
		TempFace=E1->NextFaceInFaceStuck (TempFace);
	}
	
	return NULL;

}



Face *FaceList::DelFace(Face *F, bool MarkNeurons){
	// this function deltes a Face  and frees it's memory
	// it returns a pointer to the Next face after the deleted one
	// it changes the head if the face is the first face it also changes
	// the neuron counters.
	// if MarkNeurons is set he neurons are marked for later deletion


	Face *TempFace=F->Next;

	if (TempFace==F){ // this happens when there is only one edge in the list
#ifdef MY_DEBUG
		// this is a debug check
		if (Size!=1 || F->Prev!=F) MyFatalError("FaceList::DelFace inconsistency detected in the face list");
#endif
		
		// now reset the Head to NULL and also TempFace to be returned
		Head=NULL;
		TempFace=NULL;
		
	}
	else{
		TempFace->Prev=F->Prev;
		F->Prev->Next=TempFace;
		// if this is the face in the list - modify the head
		if (F==Head) Head=TempFace;
	}

	Neuron *Neuron1,*Neuron2,*Neuron3;
	F->GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
	Neuron1->FaceCounter--;
	Neuron2->FaceCounter--;
	Neuron3->FaceCounter--;

	Edge *E;
	for (int i=0;i<3;i++){
		bool DebugFaceDeleted = false;
		switch (i){
			case 0: E=F->Edge1;
				break;
			case 1: E=F->Edge2;
				break;
			case 2: E=F->Edge3;
				break;
		}

		Neuron *N2=E->Neuron2;

		if (E->FacesStuckHead==F){
			E->FacesStuckHead = E->NextFaceInFaceStuck(E->FacesStuckHead);
#ifdef MY_DEBUG
			DebugFaceDeleted=true;
#endif
		}
		else {
			Face *TempStuckFace = E->FacesStuckHead;
			Face *NextStuckFace = E->NextFaceInFaceStuck(TempStuckFace);

			while (NextStuckFace !=NULL){
				if (NextStuckFace == F){
					if (E==TempStuckFace->Edge1)
						TempStuckFace->NextFaceInE1 = E->NextFaceInFaceStuck(NextStuckFace);
					else if (E==TempStuckFace->Edge2)
						TempStuckFace->NextFaceInE2 = E->NextFaceInFaceStuck(NextStuckFace);
					else
						TempStuckFace->NextFaceInE3 = E->NextFaceInFaceStuck(NextStuckFace);

#ifdef MY_DEBUG
					DebugFaceDeleted =true;
#endif
					break;
				}
				TempStuckFace = NextStuckFace;
				NextStuckFace = E->NextFaceInFaceStuck(NextStuckFace);
			}
		}

#ifdef MY_DEBUG
		if (!DebugFaceDeleted)
			MyFatalError("Face not deleted from stuck");
#endif

	}


	F->Edge1->FaceCounter--;
	F->Edge2->FaceCounter--;
	F->Edge3->FaceCounter--;
	// if the MarkNeurons flag is set mark neurons
	if (MarkNeurons && F->Edge3->FaceCounter==0) F->Edge3->Tag |= EDGE_FACES_REMOVED;
	if (MarkNeurons && F->Edge2->FaceCounter==0) F->Edge2->Tag |= EDGE_FACES_REMOVED;
	if (MarkNeurons && F->Edge1->FaceCounter==0) F->Edge1->Tag |= EDGE_FACES_REMOVED;
#ifdef MY_DEBUG
	// now free the memory of the face
	TRACE ("FaceList::DelFace: Deleteing face between neurons %d, %d, %d \n",F->Edge1->EdgeId,F->Edge2->EdgeId,F->Edge3->EdgeId);
#endif
	delete(F);
	Size--;
	return TempFace;

}


Face *FaceList::DelFace(Edge *E1, Edge *E2 , Edge *E3){
	// the function deletes a Face between three edges (if exists)
	// it returns a pointer to the member after the deleted face
	// it retuns NULL if no faces were deleted
	Face *TempFace=Head;
	if (Head!=NULL) while (1){
		if ( TempFace->IsFaceBetweenEdges(E1,E2,E3)){
			// free memory and remove from list
			TempFace=DelFace(TempFace);
			//we can get out of the loop now - we assume that no two faces are alike!
			return TempFace;
		}
		else 
			TempFace=TempFace->Next;
		// when covered all the faces you can get out of the loop
		if (TempFace==Head) return NULL;
	}
	return NULL;
}


Face *FaceList::DelFace(Neuron *N1, Neuron *N2 , Neuron *N3){
	// the function deletes a Face between three neurons (if exists)
	// it returns a pointer to the member after the deleted face
	// it retuns NULL if no faces were deleted
	Face *TempFace=Head;
	if (Head!=NULL) while (1){
		if ( TempFace->IsFaceBetweenNeurons(N1,N2,N3)){
			// free memory and remove from list
			TempFace=DelFace(TempFace);
			//we can get out of the loop now - we assume that no two faces are alike!
			return TempFace;
		}
		else 
			TempFace=TempFace->Next;
		// when covered all the faces you can get out of the loop
		if (TempFace==Head) return NULL;
	}
	return NULL;
}


Face *FaceList::CreateOrRefreshFace(Edge *E1, Edge *E2, Edge *E3 , int FaceType){
	// this function scans for existing faces beween the same Edgess, if found it refreshed the face
	// if non are found a new face is created at the tail of the list (older faces will be closer to the head this way)
	// the function returns a pointer to the created or refreshed face
	Face *TempFace=Head;
	

	if (Head != NULL) do{
		if ( TempFace->IsFaceBetweenEdges(E1,E2,E3) ){
			// face found refresh it and exit
			TempFace->CreationAge = 0;
			return TempFace;
		}
		TempFace=TempFace->Next;
		
	} while (TempFace!=Head);
	// if we got to this point it means that you need to create a new face - put it at the back of the list
	
	Face *NewFace = new Face(E1,E2,E3);
	NewFace->Tag=FaceType;
	if (Head==NULL){ // empty list - fill it
		NewFace->Prev=NewFace;
		NewFace->Next=NewFace;
		Head=NewFace;
	}
	else{
		NewFace->Next=Head;
		NewFace->Prev=Head->Prev;
		Head->Prev->Next=NewFace;
		Head->Prev=NewFace;
	}
	Size++;
	return NewFace;
}


void FaceList::MaskConnectedNeurons(int Mask)
{


	Face *TempFace = Head;
	if (Head != NULL) do{
		Neuron *Neuron1,*Neuron2,*Neuron3;
		TempFace->GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
		Neuron1->Tags &= Mask;
		Neuron2->Tags &= Mask;
		Neuron3->Tags &= Mask;
		TempFace=TempFace->Next;
	} while (TempFace!=Head);

}

int FaceList::CountFacesForEdge(Edge *E){
	int Counter=0;
	Face *TempFace = Head;
	
	if (Head != NULL) do{
		if (TempFace->IsFaceOfEdge(E)) 
			Counter++;
		TempFace=TempFace->Next;
	} while (TempFace!=Head);
	
	return Counter;

}

/* old stuff not used any more
Face **FaceList::FindFacesByEdge( Face **ListOfFaces, Edge *E){
	Face **ReturnList;
	if (ListOfFaces==NULL) 
		ReturnList= new (Face (*[E->FaceCounter]));
	else 
		ReturnList=ListOfFaces;


	Face *F=Head;
	for (int i=0; i<E->FaceCounter;){
		
		if (F->IsFaceOfEdge (E))
			ReturnList[i++]=F;
		F=F->Next;
		if (F==Head)
			break;
	}

	ASSERT(i==E->FaceCounter);
	
	return ReturnList;

}
*/