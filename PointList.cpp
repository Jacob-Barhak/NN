
#include "stdafx.h"
#include "NeuralNet.h"
#include <time.h>
#include <algorithm>


int PointList::Reorder(unsigned int RandomSeed){
	// this function reorders the points in random order
	// if the RandomSeed is 0 the function picks one using the timer
	if (Size==0) return (0);

	if (RandomSeed)
		srand(RandomSeed);
	else 
		srand( (unsigned int) (time(NULL)) );

	SamplePoint *TempPt;

	// reorder the points by swaping pairs
	for (int i = Size -1; i>0 ;i--){
		int j= (int) ((double)rand()/RAND_MAX*(i-1));
		TempPt = Array[j];
		Array[j]=Array[i];
		Array[i]=TempPt;
	}

	return (1);
}

int PointList::CalcBoundingBox(){
	// this function calculates the bounding box of all the sample points
	// it returns the number of sample points processed.

	if (Size==0) return 0;
	

	MinVals=**Array;
	MaxVals=**Array;

	SamplePoint **P = Array;
	for(int i=0; i<Size; i++, P++){
		MinVals.MinValues(**P);
		MaxVals.MaxValues(**P);
	}

	return (Size);
}


ostream& operator << ( ostream& os, PointList &PtLst ){
	// output all the sample points to file
	SamplePoint **P=PtLst.Array;
	for (int i=0; i<PtLst.Size ; i++ , P++)
		os<<**P;
	return os;

}


istream& operator >> ( istream& is, PointList &PtLst ){
	
	//this function reads the points out of the file

	// the following local structure is used to efficiently extract the points from the file
	struct SamplePointStack {
		SamplePoint *MemberPt;
		struct SamplePointStack *Next;
	} *StackHead , *Pt;

	int NewSize=0;
	StackHead=NULL;

	is >> ws; // get to the first meanigful character
	int FirstChar=is.peek();

	if (is.fail())
		return is;

	if (FirstChar == '#'){  // this means we assume a VRML file
	
		MyPoint PtArray[MAX_ARRAY_SIZE];
		int PtIndex [MAX_ARRAY_SIZE];
//		Neuron *NeuronArray [MAX_ARRAY_SIZE];
		int FaceIndex[3][MAX_ARRAY_SIZE];


		int PtNo=0, FaceNo=0 , LastPtNo=0;

		char Buffer[MAX_BUFFER_SIZE];
		char *HelpP;
		bool FileEnded=false;
//		ifstream ifs;
//		ifs.open(FileName);

		do {
			do{

				is.getline (Buffer,MAX_BUFFER_SIZE);

				if (is.eof()) { 
					FileEnded=true; 
					if (is.fail()) // this happens when reaching EOF
						is.clear();
					break;
				}

				if (is.fail()){
//					is.close();
//					return false;
					return is;
				}

				HelpP=strstr(Buffer,"point");

			} while (HelpP==NULL || Buffer[0] == '\0');

			if (FileEnded) break;

			// found start of point list - now use it to extract points

			LastPtNo = PtNo;

			do{
				is.getline (Buffer,MAX_BUFFER_SIZE);
				if (is.fail()){
//					is.close();
//					return false;
					return is;

				}
				if (strstr(Buffer,"]")) {
					break; // check for end of the point list
				}
				
				if (sscanf (Buffer,"%lf %lf %lf", &(PtArray[PtNo].Pos[0]),&(PtArray[PtNo].Pos[1]),&(PtArray[PtNo].Pos[2])) !=3)
					continue;

				PtIndex[PtNo]=PtNo;
				PtNo++;
			} while(true);


			do{

				is.getline (Buffer,MAX_BUFFER_SIZE);
				
				if (is.fail()){
//					is.close();
//					return false;
					return is;

				}

				HelpP=strstr(Buffer,"coordIndex");

			} while (HelpP==NULL || Buffer[0] == '\0');

			// found start of point list - now use it to extract points

			do{
				is.getline (Buffer,MAX_BUFFER_SIZE);
				
				if (is.fail()){
//					is.close();
//					return false;
					return is;
				}
				if (strstr(Buffer,"]")) {
					break; // check for end of the point list
				}
				char c1,c2;
				if (sscanf (Buffer,"%d%c%d%c%d", &(FaceIndex[0][FaceNo]) ,&c1, &(FaceIndex[1][FaceNo]) ,&c2, &(FaceIndex[2][FaceNo])) !=5)
					continue;
				

				// undate indices
				FaceIndex[0][FaceNo] += LastPtNo;
				FaceIndex[1][FaceNo] += LastPtNo;
				FaceIndex[2][FaceNo] += LastPtNo;

				FaceNo++;

			} while(true);
		} while(true);
		// close the file
		//	is.close();

		// now change all indices by finding duplications

		for (int i=0; i<PtNo; i++){
			if (PtIndex[i] == i){ 
				for (int j=i+1; j<PtNo; j++)
					if (PtArray[i].Pos[0]==PtArray[j].Pos[0] &&
						PtArray[i].Pos[1]==PtArray[j].Pos[1] &&
						PtArray[i].Pos[2]==PtArray[j].Pos[2] )
							PtIndex[j]=i;

//				NeuronArray[i] = AddNeuron(PtArray[i]);
				
			}
//			else 
//				NeuronArray[i]=NULL;

		}

		// now create the faces

		for (int i=0;i<FaceNo;i++){
			MyPoint *V1, *V2, *V3;

			V1= & PtArray[PtIndex[FaceIndex[0][i]]];
			V2= & PtArray[PtIndex[FaceIndex[1][i]]];
			V3= & PtArray[PtIndex[FaceIndex[2][i]]];

			
			double d=0.25;
			for (double u=0.0; u<=1.0 ; u=u+d){
				for (double v=0.0; v<=1.0 ; v=v+d){
	
					double w = 1.0-u-v;
					if (w>=0.0){
						MyPoint InterpPt = ((*V1)*u)+((*V2)*v)+((*V3)*w);
						bool DuplicateFound = false;
						
						Pt=StackHead;
						while (Pt!=NULL){
							if (*Pt->MemberPt == InterpPt){
								DuplicateFound=true;
								break;
							}
							Pt=Pt->Next;
						}

						if (!DuplicateFound){
							// add a point to the stack
							Pt = new SamplePointStack();
							Pt->Next = StackHead;
							StackHead = Pt;
							// we don't fre this memory later on
							StackHead->MemberPt = new SamplePoint();
							*Pt->MemberPt = InterpPt;
							NewSize++; // size was initialized to zero at creation so only increment it.
						}
					}
				}
			}
		}
	}
	else{

		while (!is.eof() && !is.fail()){ // there may be a bug in here.
			// add a point to the stack
			Pt = new SamplePointStack();
			Pt->Next=StackHead;
			StackHead=Pt;
			// we don't fre this memory later on
			StackHead->MemberPt = new SamplePoint();
			is >> *(Pt->MemberPt);
			is >> ws; // this way the eof flag wil be set...
			NewSize++; // size was initialized to zero at creation so only increment it.
		}
	}

	PtLst.ReallocateMemory (NewSize);
	// now copy from the stack to the array (in correct order)
	for (int i= PtLst.Size-1;i > -1;i--){
		PtLst.Array[i]= StackHead->MemberPt;
		Pt=StackHead;
		StackHead=StackHead->Next;
		delete Pt;
	}
	
	if (!is.fail()){
		//now when the data is in we can calculate the bounding box;
		PtLst.CalcBoundingBox();
		
		PtLst.Reorder(-1); // for debug reasons reorder with known random seed
		//PtLst.Reorder(); //reorders the sample points randomly for training
	}

	return is;
}

void PointList::ReallocateMemory(int NewSize){
	// this function realloacates memory for the array and changes the pointers
	int i;
	SamplePoint **P1, **P2 , **NewArray;
	
	if (NewSize < Size) MyFatalError ("cannot allocate less memory then already taken");

	if (Array==NULL){
		Array = new ( SamplePoint *[NewSize]);
		Size=NewSize;
#ifdef MY_DEBUG
		// for debugging, purposes init all to NULL
		for (i=0, P1=Array ;i<Size;i++, P1++) P1=NULL; 
#endif
	}
	else
	{
		NewArray = new ( SamplePoint *[NewSize]);
		// copy exiting points
		for (i=0, P1=Array , P2=NewArray ;i<Size;i++, P1++, P2++) P2=P1;
		// for debugging, purposes init all to NULL
		for (i=Size, P1=Array , P2=NewArray ;i<NewSize;i++, P1++, P2++) P2=NULL;
		
		Size=NewSize;
		// free old memory (without freeing the point data itself)
		delete Array;
	}

}



PointList::PointList(){
	Array=NULL; 
	DispListNumber=0;
	Size=0;
	RecalcForDisplay=true; // do recalculate the display the first time
}


void PointList::ResetPointWeight(double NewWeight){
	for(int i = 0 ; i<Size; i++)
		Array[i]->Weight=NewWeight;
	
}


PointList::~PointList(){
	
	int i;
	SamplePoint **P=Array;
	// delete all sample points
	for (i=0; i<Size; i++ , P++) delete (*P);
	// now delete the array of pointers
	delete [] Array;
		
	//delete the display list -I'll do it later
	//***
}

