
#include "stdafx.h"
#include "NeuralNet.h"


NeuronList::NeuronList(){
	// init the list to nothing
	Head=NULL;
	Size=0;
}


NeuronList::~NeuronList(){
	// destroy all neurons and free their memory
	Neuron *N,*TempN;
	N=Head;
	for (N=Head; N!=NULL; ){
		TempN=N->Next;
		delete N;
		N=(TempN==Head) ? NULL : TempN;
		Size --;
#ifdef MY_DEBUG
		// debug check
		if (Size<0) MyFatalError ("NeuronList::~NeuronList - neuron list is inconsistent");
#endif
	}
#ifdef MY_DEBUG
	// another debug check
	if (Size>0) MyFatalError ("NeuronList::~NeuronList - neuron list is inconsistent");
#endif
	Head=NULL;
}


Neuron *NeuronList::PushFront( Neuron &N, bool AllocateMemory){
	// pushes a new neuron to the begining of the list
	// if AllocateMemory is set, new memory is allocated
	// and a new instance of the Neuron is created

	return Add(N, NULL, AllocateMemory);
}

Neuron *NeuronList::PopFront(bool DeleteMemory){
	// this function removes a neuron from the head of the list,
	// if DeleteMemory is set, is deletes the memory allocated for it and the function returns NULL
	// otherwise the function returns a pointer to the removed neuron.

	return Remove(Head,DeleteMemory);
}


Neuron *NeuronList::PushBack( Neuron &N, bool AllocateMemory){
	// pushes a new neuron to the tail of the list
	// if AllocateMemory is set, new memory is allocated
	// and a new instance of the Neuron is created

	
	return Add(N, Head, AllocateMemory);
}

Neuron *NeuronList::PopBack(bool DeleteMemory){
	// this function removes a neuron from the tail of the list,
	// if DeleteMemory is set, is deletes the memory allocated for it and the function returns NULL
	// otherwise the function returns a pointer to the removed neuron.

	return Remove((Head==NULL)? NULL:Head->Prev,DeleteMemory);
}


Neuron *NeuronList::Remove(Neuron *N, bool DeleteMemory){
	// this function removes a neuron from the list,
	// if DeleteMemory is set, is deletes the memory allocated for it 
	// the function returns a pointer to the neuron just after the removed neuron.
	// if the list is empty it returns NULL
	Neuron *TempN, *JustAfterN;

	// first remove classification information of the sampled points
	if(DeleteMemory)
		N->EmptyStuck();

	TempN=N;


	if (TempN->Next==TempN){ // in case this is the only neuron in the list

#ifdef MY_DEBUG
		// this is a debug check
		if (Head != TempN || Size !=1 || TempN->Prev != TempN) MyFatalError("NeuronList::Remove : inconsistency detected in the list");
#endif
		Head=NULL;
		JustAfterN=NULL;
	}
	else{ // just reset the pointers of the neigbours in the list
		TempN->Next->Prev=TempN->Prev;		
		TempN->Prev->Next=TempN->Next;
		if (Head == TempN) Head=TempN->Next; // reset the head if needed
		JustAfterN=TempN->Next;
	}
	Size --;

	if (DeleteMemory){
		delete TempN;
	}
	return JustAfterN;
}


Neuron *NeuronList::Add(Neuron &N, Neuron *BeforeN, bool AllocateMemory){
	// this function adds a neuron before BEforN in the list. if BeforeN is NULL and the list is empty , insert the first member of the list
	// the function allocates memory for the new neuron if Allocate memory is true.
	// the function returns a pointer to the newly inseted member in the list
	
	Neuron *NewNeuron;

	// this is a debug check for precaution, this should never happen
#ifdef MY_DEBUG
	if (&N == BeforeN) TRACE("*NeuronLisst::Add - warning - duplication of a neuron"); 
#endif

	if (AllocateMemory){
		NewNeuron = new Neuron;
		*NewNeuron = N;
	}
	else NewNeuron=&N;

	if  (BeforeN==NULL){ // in this case check the head, if it is also NULL , insert at the begining of the list
		if (Head==NULL){ // in case the list was empty
#ifdef MY_DEBUG
			// add a debug check
		 	if (Size!=0) MyFatalError ("NeuronList::Add - inconsistency detected in the PointList");
#endif
			NewNeuron->Next=NewNeuron;
			NewNeuron->Prev=NewNeuron;
		}
		else{ // in case thes is something is the list
			NewNeuron->Next=Head;
			NewNeuron->Prev=Head->Prev;
			Head->Prev->Next=NewNeuron;
			Head->Prev=NewNeuron;
		}
		// the head now is new
		Head=NewNeuron;
	}
	else{ // put not at the head
		
		NewNeuron->Next=BeforeN;
		NewNeuron->Prev=BeforeN->Prev;
		BeforeN->Prev->Next=NewNeuron;
		BeforeN->Prev=NewNeuron;
	}

	Size++;
	return NewNeuron;
}



void NeuronList::Swap ( Neuron *N1 , Neuron *N2){
	// this function swaps two neurons in the list

	Neuron *TempN;
	if (N1==N2) return; // dont swap the same neuron with itself

	if (N1->Next==N2){  // in case both neurons are near each other
		Remove (N2,false);
		Add(*N2,N1,false);
	} else{ //if (N2->Next==N1->Next){
		TempN=N1->Next;
		Remove (N1,false);
		Add(*N1,N2,false);
		Remove (N2,false);
		Add(*N2,TempN,false);
	}

}


void NeuronList::Sort(int k , Neuron *SortHead ,int SizeToEnd){
	//this function sorts the first k members of the neuron List by decreasing order
	// if k is 0 (default) or k>=Size all the list is sorted. otherwize only the first k members are sorted
	// sorting method is simple - every iteration the smallest value is put at the end.
	// SizeToEnd defines the number of neurons in the list that are of interest to us

	// the SortHead should follow the head of the list unless it was given as a parameter
	int RelevantListSize = Size;
	if (SortHead == NULL){
		SortHead=Head;
	}
	else
		RelevantListSize=SizeToEnd;


	if (k==0 || k>=RelevantListSize) k=RelevantListSize;  // sort all k members in this case

	int IterationsDone=0;
	 // only the closest at the end by bubble sort
	for (int i=0;i<k;i++){
		Neuron *It = SortHead;
		Neuron *MinIt = SortHead;

		
		int LastCount = RelevantListSize-i-1;


		for ( int j=0; j<LastCount; j++){
			It=It->Next;
			if (It->PointDist < MinIt->PointDist){
				MinIt=It;
			}
			
		}

		// if removing the first member of the list, update the position of SortHead to point to the next member
		if (MinIt==SortHead) 
			SortHead=SortHead->Next;

		Remove(MinIt,false);
		PushBack(*MinIt,false);

		IterationsDone++; // increase the actual number of iteration that were preformed (this is important if we exit due to the fact the CompleteFlag is true)
	}

	// move head to the back k steps so the minimal value will be at the start
	for (i=0;i<IterationsDone;i++){
		Head=Head->Prev;
		
	}
	
}


/*
void NeuronList::Sort(int k , Neuron *SortHead ,int SizeToEnd){
	//this function sorts the first k members of the neuron List by decreasing order
	// if k is 0 (default) or k>=Size all the list is sorted. otherwize only the first k members are sorted
	// sorting method is simple - every iteration the smallest value is put at the end.

	if (k==0 || k>=Size) k=Size;  // sort all k


	if ( SortHead !=NULL && SizeToEnd !=NULL)
		TRACE ("new version used");
	 // only the closest at the end by bubble sort
	for (int i=0;i<k;i++){
		Neuron *It = Head;
		Neuron *MinIt = Head;
		
		RealType MinDist = (It->PointDist);
		int LastCount = Size-i-1;

		for ( int j=0; j<LastCount; j++){
			It=It->Next;
			if (It->PointDist < MinIt->PointDist)
				MinIt=It;	
		}

		Remove(MinIt,false);
		PushBack(*MinIt,false);
	}

	// move head to the back k steps so the minimal value will be at the start
	for (i=0;i<k;i++){
		Head=Head->Prev;
		
	}
	
}

*/