
#include "stdafx.h"
#include "NeuralNet.h"

 
int EdgeList::DeleteOld(int Age, bool MarkNeurons,bool DeleteOnlyDisconnectedEdges){ 
	// the function returns the number of deleted edges
	// use deletion age to delete by edge age, 
	// if MarkNeurons is set then neurons with no edges
	// are marked for further processing later
	int OldSize=Size;
	Edge *TempEdge=Head;

	for (int i=0; i<OldSize; i++){
		if ( TempEdge->CreationAge > Age   && (!DeleteOnlyDisconnectedEdges || TempEdge->FaceCounter==0)){
				TempEdge=DelEdge(TempEdge,MarkNeurons);
		}
		else TempEdge=TempEdge->Next;
			// advance pointer
	}

	return (OldSize - Size);
}

EdgeList::EdgeList(){
	DispListNumber=0;
	Size=0;
	Head=NULL;
}

EdgeList::~EdgeList(){
	// delete the list - notice that the lsit has to be deleted before the neuron list, 
	// otherwise the delete statement may conclude at an access violation
	while (Head!=NULL){
		DelEdge(Head);
	}
#ifdef MY_DEBUG
	//debug check
	if (Size!=0) MyFatalError("EdgeList::~EdgeList - inconsistency detected while destructing edge list");
#endif

}

int EdgeList::Age(Neuron *N , bool AgeOnlyDisconnectedEdges){

	// all the edges connected to the Neuron are made older !!
	// if no input is spesified, all edges are aged

	int EdgesAged=0;
	Edge *TempEdge=Head;
	if (Head != NULL) do{
		if (TempEdge->IsEdgeOfNeuron(N) || N==NULL){
			if (!AgeOnlyDisconnectedEdges || TempEdge->FaceCounter <2){
				TempEdge->CreationAge++;
				EdgesAged++;
			}
		}
		TempEdge=TempEdge->Next;
	} while (TempEdge!=Head);

	return EdgesAged;
}


Edge *EdgeList::DelEdge(Edge *E, bool MarkNeurons){
	// this function deltes an edge and frees it's memory
	// it returns a pointer to the Next edge after the deleted one
	// it changes the head if the edge is the first edge it also changes
	// the neuron counters.
	// if MarkNeurons is set he neurons are marked ofr later deletion

	Edge *TempEdge=E->Next;

	if (TempEdge==E){ // this happens when there is only one edge in the list
#ifdef MY_DEBUG
		// this is a debug check
		if (Size!=1 || E->Prev!=E) MyFatalError("EdgeList::DelEdge inconsistency detected in the edge list");
#endif		
		// now reset the Head to NULL and also TempEdge to be returned
		Head=NULL;
		TempEdge=NULL;
		
	}
	else{
		TempEdge->Prev=E->Prev;
		E->Prev->Next=TempEdge;
		// if this is the edge in the list - modify the head
		if (E==Head) Head=TempEdge;
	}

	// now delete this edge from the neuron stuck
#ifdef MY_DEBUG
	bool DebugEdgeDeleted = false;
#endif

	Neuron *N1=E->Neuron1;
	Neuron *N2=E->Neuron2;

	if (N1->EdgeStuckHead==E){
		N1->EdgeStuckHead = N1->NextEdgeInEdgeStuck(N1->EdgeStuckHead);
#ifdef MY_DEBUG
		DebugEdgeDeleted=true;
#endif
	}
	else{
		Edge *TempStuckEdge = N1->EdgeStuckHead;
		Edge *NextStuckEdge = N1->NextEdgeInEdgeStuck(TempStuckEdge);

		while (NextStuckEdge !=NULL){
			if (NextStuckEdge == E){
				if (N1==TempStuckEdge->Neuron1)
					TempStuckEdge->NextInN1 = N1->NextEdgeInEdgeStuck(NextStuckEdge);
				else
					TempStuckEdge->NextInN2 = N1->NextEdgeInEdgeStuck(NextStuckEdge);
#ifdef MY_DEBUG
				DebugEdgeDeleted =true;
#endif
				break;
			}
			TempStuckEdge = NextStuckEdge;
			NextStuckEdge = N1->NextEdgeInEdgeStuck(NextStuckEdge);
		}
	}

#ifdef MY_DEBUG
	if (!DebugEdgeDeleted)
		MyFatalError("Edge not deleted from stuck of Neuron 1");

	DebugEdgeDeleted = false;
#endif

	if (N2->EdgeStuckHead==E){
		N2->EdgeStuckHead = N2->NextEdgeInEdgeStuck(N2->EdgeStuckHead);
#ifdef MY_DEBUG
		DebugEdgeDeleted=true;
#endif
	}
	else{
		Edge *TempStuckEdge = N2->EdgeStuckHead;
		Edge *NextStuckEdge = N2->NextEdgeInEdgeStuck(TempStuckEdge);

		while (NextStuckEdge !=NULL){
			if (NextStuckEdge == E){
				if (N2==TempStuckEdge->Neuron1)
					TempStuckEdge->NextInN1 = N2->NextEdgeInEdgeStuck(NextStuckEdge);
				else
					TempStuckEdge->NextInN2 = N2->NextEdgeInEdgeStuck(NextStuckEdge);
#ifdef MY_DEBUG
				DebugEdgeDeleted =true;
#endif
				break;
			}
			TempStuckEdge = NextStuckEdge;
			NextStuckEdge = N2->NextEdgeInEdgeStuck(NextStuckEdge);
		}
	}
#ifdef MY_DEBUG
	if (!DebugEdgeDeleted)
		MyFatalError("Edge not deleted from stuck of Neuron 2");
#endif


	E->Neuron1->EdgeCounter--;
	E->Neuron2->EdgeCounter--;

	// if the MarkNeurons flag is set mark neurons
	if (MarkNeurons && E->Neuron2->EdgeCounter==0) E->Neuron2->Tags |= NEURON_EDGES_REMOVED;
	if (MarkNeurons && E->Neuron1->EdgeCounter==0) E->Neuron1->Tags |= NEURON_EDGES_REMOVED;
	// now free the memory of th edge

#ifdef MY_DEBUG
	TRACE ("EdgeList::DelEdge: Deleteing Edge between neurons %d, %d \n",E->Neuron1->NeuronId,E->Neuron2->NeuronId);
#endif

	delete(E);
	
	Size--;
	return TempEdge;

}


Edge *EdgeList::DelEdge(Neuron *N1, Neuron *N2){
	// the function deletes an edge between two neurons (if exists)
	// it returns a pointer to the member after the deleted edge
	// it retuns NULL if no edges were deleted
	Edge *TempEdge=Head;
	if (Head!=NULL) while (1){
		if ( TempEdge->IsEdgeBetweenNeurons(N1,N2)){
			// free memory and remove from list
			TempEdge=DelEdge(TempEdge);
			//we can get out of the loop now - we assume that no two edges are alike!
			return TempEdge;
		}
		else 
			TempEdge=TempEdge->Next;
		// when covered all the edges you can get out of the loop
		if (TempEdge==Head) return NULL;
	}
	return NULL;
}


int EdgeList::DelEdges(Neuron *N){
	// the function deletes all the edges ementating from the Neuron (if exists)
	// it returns the number of edges deleted
	// if N is NULL then all edges are deleted
	int DelCounter=0;
	Edge *TempEdge=Head;
	if (Head!=NULL) do{
		if ( TempEdge->IsEdgeOfNeuron(N) ){
			// free memory and remove from list
			TempEdge=DelEdge(TempEdge);
			DelCounter++;
			//we can get out of the loop now - we assume that no two edges are alike!
		}
		else 
			TempEdge=TempEdge->Next;
		// when covered all the edges you can get out of the loop
	} while (TempEdge!=Head );

	return DelCounter;
}


Edge *EdgeList::CreateOrRefreshEdge(Neuron *N1, Neuron *N2 , int EdgeType){
	// this function scans for existing edges beween the same neurons, if found it refreshed the edge
	// if non are found a new edge is created at the tail of the list (older edges will be closer to the head this way)
	// the function returns a pointer to the created or refreshed edge
	Edge *TempEdge=Head;
	

	if (Head != NULL) do{
		if ( TempEdge->IsEdgeOfNeuron(N1) && TempEdge->IsEdgeOfNeuron(N2)){
			// edge found refresh it and exit
			TempEdge->CreationAge = 0;
			return TempEdge;
		}
		TempEdge=TempEdge->Next;
		
	} while (TempEdge!=Head);
	// if we got to this point it means that you need to create a new edge - put it at the back of the list
	
	Edge *NewEdge = new Edge(N1,N2);
	NewEdge->Tag=EdgeType;
	if (Head==NULL){ // empty list - fill it
		NewEdge->Prev=NewEdge;
		NewEdge->Next=NewEdge;
		Head=NewEdge;
	}
	else{
		NewEdge->Next=Head;
		NewEdge->Prev=Head->Prev;
		Head->Prev->Next=NewEdge;
		Head->Prev=NewEdge;
	}
	Size++;
	return NewEdge;
}


void EdgeList::MaskConnectedNeurons(int NeuronMask , int EdgeMask){

	Edge *TempEdge = Head;
	int count=0;
	if (Head != NULL) do{
		if (TempEdge->Tag & EdgeMask){
			count++;
			TempEdge->Neuron1->Tags &= NeuronMask;
			TempEdge->Neuron2->Tags &= NeuronMask;
		}
		TempEdge=TempEdge->Next;
	} while (TempEdge!=Head);

}





Edge *EdgeList::FindEdge(Neuron *N1, Neuron *N2 , bool UseOnlyConnectedEdges){
// this function returns the edge betweem N1 and N2 o
// returns NULL if no edge like this exists in the list
	Edge *TempEdge=N1->EdgeStuckHead;

	if (TempEdge != NULL) do{
		if ( TempEdge->IsEdgeOfNeuron(N2) && (!UseOnlyConnectedEdges || TempEdge->FaceCounter>0)){
#ifdef MY_DEBUG
			// debug check
			if (! TempEdge->IsEdgeOfNeuron(N1))
				MyFatalError ("Edge reported in N1 list but is not a member of N1");
#endif
			// edge found - exit
			return TempEdge;
		}
		TempEdge=N1->NextEdgeInEdgeStuck(TempEdge);
		
	} while (TempEdge!=NULL);
	
	return (NULL);
}
