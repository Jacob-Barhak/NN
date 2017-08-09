// NeuralNet.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "NeuralNet.h"


#define MY_DEBUG_TIME

#ifdef MY_DEBUG_TIME
#define TIMER_NO 10
	CTimeSpan TimeSpan[TIMER_NO];
	
	CTime StartTime[TIMER_NO];
	CTime EndTime;
	CTime ElapsedTime;

#define TIC(i) StartTime[i] = CTime::GetCurrentTime();
#define TOC(i) EndTime = CTime::GetCurrentTime(); TimeSpan[i] +=  (EndTime - StartTime[i]);
#define OUTPUT_TOC(k) TRACE("TIMER : timer %i returned %i seconds\n",k,TimeSpan[k].GetTotalSeconds());

#else

#define TIC(i)
#define TOC(i) 
#define OUTPUT_TOC(k) 

#endif


bool StartedTriangulation;

void NeuralNet::InitNeuralNet(int NumberOfNeuronsUDir,int NumberOfNeuronsVDir, PointList &PtLst,int NetType , LPCTSTR FileName){  
	// initialize with topology defined by net type

	MyPoint MinVals,MaxVals;
	int i,j;
	Neuron *pN , *pNu , *pNv ;

	if (!CheckNetworkTypeValidity(NetType)){
		MyFatalError ("NetWorkType given is faulty in NeuralNet constructor");
	}

	LocationSizeX=LOCATION_ARRAY_SIZE_X;
	LocationSizeY=LOCATION_ARRAY_SIZE_Y;
	LocationSizeZ=LOCATION_ARRAY_SIZE_Z;

	LocationArray = new (Neuron (*[LocationSizeX*LocationSizeY*LocationSizeZ]));
	LocationArrayCount = new (int [LocationSizeX*LocationSizeY*LocationSizeZ]);

	for (int k=0; k<LocationSizeX*LocationSizeY*LocationSizeZ; k++){
		LocationArray[k]=NULL;
		LocationArrayCount[k]=0;
	}


	// allocate memory for location array
	LocationLowX=PtLst.MinVals.Pos[0];
	LocationHighX=PtLst.MaxVals.Pos[0];
	LocationLowY=PtLst.MinVals.Pos[1];
	LocationHighY=PtLst.MaxVals.Pos[1];
	LocationLowZ=PtLst.MinVals.Pos[2];
	LocationHighZ=PtLst.MaxVals.Pos[2];

	LocationDeltaX = (LocationHighX-LocationLowX)/LocationSizeX;
	LocationDeltaY = (LocationHighY-LocationLowY)/LocationSizeY;
	LocationDeltaZ = (LocationHighZ-LocationLowZ)/LocationSizeZ;


	if (FileName!=NULL){
		if (LoadNetFromFile(FileName)){
			NetworkSizeU=Neurons.Size;
			NetworkSizeV=0;
			Tags=NetType;
			ClassifySamplePointList(PtLst);
			// more work needed here to detect the network type
		}
		else MyFatalError("could not read from file in NeuralNet constructor");
	}
	// growing neural gas with hebian learning
	else if ((NetType & NN_NO_TOPOLOGY) &&
		(NetType & NN_CAN_GROW_EDGES)){
		
		NetworkSizeU=NumberOfNeuronsUDir;
		NetworkSizeV=0;
		Tags=NetType;

		for (i=0;i<NumberOfNeuronsUDir;i++) {
			MyPoint TempPt(PtLst.MinVals,PtLst.MaxVals);
			AddNeuron (TempPt);
		}
	}
	// simple self orgenizing map (or growing neural net) with rectangular topology
	else if (NetType & NN_RECTANGULAR_TOPOLOGY){
		
		NetworkSizeU=NumberOfNeuronsUDir;
		NetworkSizeV=NumberOfNeuronsVDir;
		Tags=NetType;

		NeuronArray = new (Neuron (*[NumberOfNeuronsUDir*NumberOfNeuronsVDir]));
		// first create the neurons 
		for (i=0;i<NumberOfNeuronsUDir;i++){
			for (j=0;j<NumberOfNeuronsVDir;j++){
				MyPoint TempPt(PtLst.MinVals,PtLst.MaxVals); 
				Neuron *TempNeuron = AddNeuron(TempPt); // create a neuron instance
				TempNeuron->NeuronIdU=i;
				TempNeuron->NeuronIdV=j;
				RECTNN(i,j) = TempNeuron;
			}

		}
		// create the edges
		for (i=0;i<NumberOfNeuronsUDir;i++){
			for (j=0;j<NumberOfNeuronsVDir;j++){
				pN=RECTNN(i,j);
				if (i != NumberOfNeuronsUDir-1) {
					pNu=RECTNN(i+1,j);
					Edges.CreateOrRefreshEdge(pN,pNu,EDGE_U_DIR);
				}
				if (j != NumberOfNeuronsVDir-1) {
					pNv=RECTNN(i,j+1);
					Edges.CreateOrRefreshEdge(pN,pNv,EDGE_V_DIR);
				}
			}	
		}


		// handle cyclic conditions in U dir
		if (NetType & NN_CYCLIC_U_DIR){
			for (j=0;j<NumberOfNeuronsVDir;j++){
				pN=RECTNN(NumberOfNeuronsUDir-1,j);
				pNu=RECTNN(0,j);			
				Edges.CreateOrRefreshEdge(pN,pNu,EDGE_U_DIR);
			}
		}
		// handle cyclic conditions in V dir
		if (NetType & NN_CYCLIC_V_DIR){
			for (i=0;i<NumberOfNeuronsUDir;i++){
				pN=RECTNN(i,NumberOfNeuronsVDir-1);
				pNv=RECTNN(i,0);			
				Edges.CreateOrRefreshEdge(pN,pNv,EDGE_V_DIR);
			}
		}
	}
	// growing neural gas - only three neurons connected between themselves
	else if ((NetType & NN_HAS_TOPOLOGY) && 
		     (NetType & NN_CAN_GROW_NEURONS) && 
		     (NetType & NN_CAN_GROW_EDGES) ){
			// file support is availiable only for growing neural gas now
		NetworkSizeU=4;
		NetworkSizeV=0;
		Tags=NetType;

		MyPoint TempPt1(PtLst.MinVals , PtLst.MaxVals);
		MyPoint TempPt2(PtLst.MinVals , PtLst.MaxVals);
		MyPoint TempPt3(PtLst.MinVals , PtLst.MaxVals);
		MyPoint TempPt4 = (TempPt1 + TempPt2 + TempPt3)*(1.0/3);

		Neuron *TempNeuron1 = AddNeuron(TempPt1); // create a neuron instance
		Neuron *TempNeuron2 = AddNeuron(TempPt2); // create another neuron instance
		Neuron *TempNeuron3 = AddNeuron(TempPt3); // create another neuron instance
//		Neuron *TempNeuron4 = AddNeuron(TempPt4); // create another neuron instance

		// mark boundary neurons as boundary
//		TempNeuron1->Tags |= NEURON_ON_BOUNDARY;
//		TempNeuron2->Tags |= NEURON_ON_BOUNDARY;
//		TempNeuron3->Tags |= NEURON_ON_BOUNDARY;

		// create the edges needed
		Edge *E12=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron2);
		Edge *E13=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron3);
		Edge *E23=Edges.CreateOrRefreshEdge(TempNeuron2,TempNeuron3);
//		Edge *E14=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron4);
//		Edge *E24=Edges.CreateOrRefreshEdge(TempNeuron2,TempNeuron4);
//		Edge *E34=Edges.CreateOrRefreshEdge(TempNeuron3,TempNeuron4);

		// now create the faces
		// Faces.CreateOrRefreshFace(E12,E23,E13);
//		Faces.CreateOrRefreshFace(E12,E14,E24);
//		Faces.CreateOrRefreshFace(E23,E24,E34);
//		Faces.CreateOrRefreshFace(E13,E14,E34);

		// close manifold
//		Faces.CreateOrRefreshFace(E13,E12,E23);
		// init normals to rando vaues (intialized)

/*
		NetworkSizeU=4*10;
		NetworkSizeV=0;
		Tags=NetType;
		for (i=0;i<10;i++){


			MyPoint TempPt1((PtLst.MinVals+PtLst.MaxVals)*0.5-(PtLst.MaxVals-PtLst.MinVals)*0.4 ,(PtLst.MinVals+PtLst.MaxVals)*0.5+(PtLst.MaxVals-PtLst.MinVals)*0.4);
			MyPoint TempPt2((PtLst.MaxVals-PtLst.MinVals)*0.1 , (PtLst.MaxVals-PtLst.MinVals)*0.1);
			TempPt2=TempPt2+TempPt1;
			MyPoint TempPt3((PtLst.MaxVals-PtLst.MinVals)*0.1 , (PtLst.MaxVals-PtLst.MinVals)*0.1);
			TempPt3=TempPt3+TempPt1;
			MyPoint TempPt4 = (TempPt1 + TempPt2 + TempPt3)*(1.0/3);

			Neuron *TempNeuron1 = AddNeuron(TempPt1); // create a neuron instance
			Neuron *TempNeuron2 = AddNeuron(TempPt2); // create another neuron instance
			Neuron *TempNeuron3 = AddNeuron(TempPt3); // create another neuron instance
			Neuron *TempNeuron4 = AddNeuron(TempPt4); // create another neuron instance

			// mark boundary neurons as boundary
			TempNeuron1->Tags |= NEURON_ON_BOUNDARY;
			TempNeuron2->Tags |= NEURON_ON_BOUNDARY;
			TempNeuron3->Tags |= NEURON_ON_BOUNDARY;

			// create the edges needed
			Edge *E12=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron2);
			Edge *E13=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron3);
			Edge *E23=Edges.CreateOrRefreshEdge(TempNeuron2,TempNeuron3);
			Edge *E14=Edges.CreateOrRefreshEdge(TempNeuron1,TempNeuron4);
			Edge *E24=Edges.CreateOrRefreshEdge(TempNeuron2,TempNeuron4);
			Edge *E34=Edges.CreateOrRefreshEdge(TempNeuron3,TempNeuron4);

			// now create the faces
			// Faces.CreateOrRefreshFace(E12,E23,E13);
			Faces.CreateOrRefreshFace(E12,E14,E24);
			Faces.CreateOrRefreshFace(E23,E24,E34);
			Faces.CreateOrRefreshFace(E13,E14,E34);
			// init normals to rando vaues (intialized)
		}
*/			
	}
	else {
		Initialized=false;
		MyFatalError("unknown topology type in NeuralNet constructor");
	}

	this->Tags = NetType; 
	Initialized=true;

}

NeuralNet::NeuralNet(){
	DispListNumber=0;
	Tags=NN_NOT_INITIALIZED; // combination of NeuralNetworkTypes
	NeuronArray=NULL; // used only for rectangular topology SOM
	Initialized=false;
	NetworkSizeU=-1;
	NetworkSizeV=-1;
	NeuronIdMaxCounter=0;

	LocationSizeX=0;
	LocationSizeY=0;
	LocationSizeZ=0;
	LocationLowX=0;
	LocationLowY=0;
	LocationLowZ=0;
	LocationHighX=0;
	LocationHighY=0;
	LocationHighZ=0;
	LocationDeltaX=0;
	LocationDeltaY=0;
	LocationDeltaZ=0;

	LocationArray=NULL;
	LocationArrayCount=NULL;

}


NeuralNet::~NeuralNet(){
	// delete the display list -I'll do it later
	if (NeuronArray) delete [] NeuronArray;
	if (LocationArray) delete [] LocationArray;
	if (LocationArrayCount) delete [] LocationArrayCount;
}


int NeuralNet::TrainNeuralGasByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsInit, RealType EpsFinal, RealType LambdaInit, RealType LambdaFinal, int MaximalEdgeAgeInit,int MaximalEdgeAgeFinal){
	
	if (PtLst.Size == 0) {
		TRACE ("TrainNeuralGasByPointList: no points to train");
		return false;
	}

	if (EpsInit<0.0 || EpsFinal <0.0 || LambdaInit<0.0 || LambdaFinal<0.0 || MaximalEdgeAgeInit < 0 || MaximalEdgeAgeFinal < 0){
		TRACE ("TrainNeuralGasByPointList: function parameters are are not in possible range");
		return false;
	}

	if 	( !((Tags & NN_HAS_TOPOLOGY) || (Tags & NN_RECTANGULAR_TOPOLOGY) || (Tags & NN_NO_TOPOLOGY)) ){
		// neural gas can train all topological types.
		TRACE ("TrainNeuralGasByPointList: cannot train this type of topology");
		return false;
	}


	int MaximalEdgeAge;

	
	int Index = BaseIteration % PtLst.Size;
	for (int i=0; i<RunLength; i++){

/*
#ifdef MY_DEBUG
		TRACE("Trained Point: %lf,%lf,%lf\n",CurrentSamplePoint.Pos[0],CurrentSamplePoint.Pos[1],CurrentSamplePoint.Pos[2]);
#endif
*/
		TrainNeuralGasBySinglePoint(*PtLst.Array[Index],
		EXP_DECAY(EpsInit,EpsFinal,i,RunLength),
		EXP_DECAY(LambdaInit,LambdaFinal,i,RunLength), BaseIteration+i);

		// if neural network can grow edges, it can also delete old edges
		if (Tags & NN_CAN_GROW_EDGES){
			if (Tags & NN_RECTANGULAR_TOPOLOGY) // if we have growin grid characteristics change net type to neural gas - since edges will be deleted
				ChangeNetType(NN_NO_TOPOLOGY | NN_CAN_GROW_EDGES , PtLst);
			MaximalEdgeAge = (int) (EXP_DECAY(MaximalEdgeAgeInit,MaximalEdgeAgeFinal,i,RunLength));
			DelOldEdgesAndCascadeDelete(MaximalEdgeAge,false);
		}
		Index =(Index+1) % PtLst.Size;

	}

	return true;
}


void NeuralNet::TrainNeuralGasBySinglePoint (SamplePoint &Pt, RealType Eps, RealType Lambda ,int CurrentIteration , bool UseOnlyNeuronsWithFacesForNormalCalculation){

	if (Neurons.Size==0) return;

	//sort all neurons by increasing distance
	NeuronSortByWinners(Pt,0,true);

	Neuron  *Nwin, *Nwin2, *Nwin3;

	Nwin = Neurons.Head;
	Nwin2 = Neurons.Head->Next;
	Nwin3 = Neurons.Head->Next->Next;
	
	// the boolean parameter marks if a face exists between the three winner neurons
	bool FaceExists = false;

	// update edge (Neurons are already ordered...) if the network can grow edges
	if (Tags & NN_CAN_GROW_EDGES){
		// create or refresh the edge between the two winners
		Edge *E12 = Edges.CreateOrRefreshEdge(Nwin,Nwin2);

		// now check if the two other edges exist.
		// if so, we can create a face
		Edge *E13 = Edges.FindEdge (Nwin,Nwin3);
		Edge *E23 = Edges.FindEdge (Nwin2,Nwin3);

		if (E12!=NULL && E13!=NULL && E23!=NULL){
			Faces.CreateOrRefreshFace (E12,E13,E23);
			FaceExists=true;
		}

		// age other edges from the winner neuron
		Edges.Age(Nwin);
	}

	//modifiy each neuron
	int k=0;

	// adapt the normal
	MyPoint Pt_N1,NormalDirection;

	// normal is updated in the direction of the point
	if (!UseOnlyNeuronsWithFacesForNormalCalculation || FaceExists){
		Pt_N1 = Pt - *Nwin;

		NormalDirection = Nwin->CalcNormalUsing3Points(Nwin2,Nwin3);
		double Dir = NormalDirection * Pt_N1;
		if (Dir<=0.0) NormalDirection = - NormalDirection;
	}
	else{
		NormalDirection.Pos[0]=0.0;
		NormalDirection.Pos[1]=0.0;
		NormalDirection.Pos[2]=0.0;
	}

	Neuron *It=Neurons.Head;
	do{
		if (Pt.Weight == 1.0){
			RealType d = It->PointDist;
			It->ModifyPosBySamplePoint(Eps*exp(-k/Lambda),Pt);	
			//It->ModifyPosBySamplePoint(Eps*exp(-(Pt-*It)*(Pt-*It)/Lambda),Pt);	
		}
		else
			It->ModifyPosBySamplePoint(1.0-pow(1.0-Eps*exp(-k/Lambda),Pt.Weight),Pt);
			//It->ModifyPosBySamplePoint(1.0-pow(1.0-Eps*exp(-(Pt-*It)*(Pt-*It)/Lambda),Pt.Weight),Pt);
		//new corrected version
		MyPoint NewNormal = It->Normal + ((NormalDirection-It->Normal) * (Eps*exp(-k/Lambda)));
		//old problematic version
		//MyPoint NewNormal = It->Normal + (NormalDirection * (Eps*exp(-k/Lambda)));
		//MyPoint NewNormal = It->Normal + (NormalDirection * (Eps*exp(-(Pt-*It)*(Pt-*It)/Lambda)));

		// now normalize
		NewNormal = NewNormal * (1.0 / ~NewNormal);
		if (NewNormal.IsFinite() )
			It->Normal = NewNormal;
		//It->UpdateNormalUsingPCA (Pt,0.1);
		//It->CalcNormalUsingSemiMinimalBoundingCone();
		// decide on normal calculation method (the code above)

		It->LastUpdateAge=CurrentIteration;
		k++;

		It=It->Next;
	} while (It!=Neurons.Head);

}


ostream& operator << ( ostream& os, NeuralNet &NNet ){

	// output net characteristics

	os << "Tags " << NNet.Tags <<" " << NNet.NetworkSizeU <<" "<< NNet.NetworkSizeV << "\n";
	
	// output all neurons
	Neuron *It=NNet.Neurons.Head;
	if (NNet.Neurons.Head!=NULL) do{
		os << *It;
		It=It->Next;
	} while (It!=NNet.Neurons.Head);

	// output all edges
	Edge *TempEdge = NNet.Edges.Head;
	if (NNet.Edges.Head != NULL) do{
		os << *TempEdge;
		TempEdge=TempEdge->Next;
	} while (TempEdge!=NNet.Edges.Head);

	return os;
}


istream& operator >> ( istream& is, NeuralNet &NNet ){
	char c;

	while (!is.eof()){ // there may be a bug in here
		c=is.peek();
		switch (c){
		case 'N' :  // neuron at input
			Neuron *N = NNet.AddNeuron();
			is >> *N;
			NNet.AddOrRemoveNeuronToLocationArray(N);
		}
		is>>ws; // this way the eof flag wil be set...
	}


	return is;
}

ostream& OutputToIritFormat(ostream& os, NeuralNet &NNet ){


// this code outputs a rectangular SOM neural net as a cubic B-Spline Surface wih open hand conditions (good for initial parameterization)
	if (NNet.NeuronArray !=NULL &&
		NNet.NetworkSizeU > 0  &&
		NNet.NetworkSizeV > 0  ){

		os <<"[OBJECT Neuralnet\n\t[SURFACE BSPLINE " <<NNet.NetworkSizeU << " " <<NNet.NetworkSizeV << " 4 4 E3\n";

		//create U knot vector
		os <<"\t\t[KV 0 0 0";
		for (int i=0; i<NNet.NetworkSizeU - 2 ; i++){
			os <<" "<< ((double)i / (NNet.NetworkSizeU-3) );
		}
		os <<" 1 1 1]\n";

		//create V knot vector
		os <<"\t\t[KV 0 0 0";
		for (i=0; i<NNet.NetworkSizeV-2 ; i++){
			os <<" "<< ((double)i / (NNet.NetworkSizeV-3) );
		}
		os <<" 1 1 1]\n";

		for (int j=0; j<NNet.NetworkSizeV ; j++){
			for (i=0; i<NNet.NetworkSizeU ; i++){
				Neuron *It= NNet.NeuronArray[(i)*(NNet.NetworkSizeV)+(j)];
				os <<" [ " << (MyPoint) (*It) ;
				os.seekp( -2, ios::cur ); // eat last new line characer
				os <<" ]\n";
			}
			os << "\n";
		}



		os<<"    ]\n]\n";



	}
	else{
		// this code outputs the neurons as point list to Irit
		os << "[OBJECT NONE\n[POINTLIST " <<NNet.Neurons.Size <<"\n"; 
		// output all neurons
		Neuron *It = NNet.Neurons.Head;
		if (NNet.Neurons.Head) do{
			os <<" [ " << (MyPoint) (*It) ;
			os.seekp( -2, ios::cur ); // eat last new line characer
			os <<" ]\n";
			It=It->Next;
		} while (It != NNet.Neurons.Head);

		os<<"]\n]\n";

	}
	return os;




}

void NeuralNet::RenumberNeurons(){
// this function renumbers the neurons again to give them consecutive ID numbers
// and changes NeuronIdMaxCounter accordingly

// if the net has rectangular topology then traverse the neurons using the rectanguar data array

	NeuronIdMaxCounter=0;
	if (Tags & NN_RECTANGULAR_TOPOLOGY){
		for (int i=0; i<NetworkSizeU; i++) for (int j=0; j<NetworkSizeV; j++){
			Neuron *It= RECTNN(i,j);
			It->NeuronId = 1 + j + i*NetworkSizeV;
			NeuronIdMaxCounter++;
			It->PointDist = It->NeuronId;
		}
		// now sort the neurons using this index since we want to maintain rectangular order in the output
		Neurons.Sort(0);
	}
	else{ // other topology network
		Neuron *It = Neurons.Head;
		if (Neurons.Head) do{
			It->NeuronId = (++NeuronIdMaxCounter);
			It=It->Next;
		} while (It != Neurons.Head);
	}

}

ostream& OutputToVrmlFormat(ostream& os, NeuralNet &NNet ){

	os << "#VRML V1.0 ascii\n#date 11 October 1996\n";
	// put some comments about the mesh
	if (NNet.Tags &  NN_RECTANGULAR_TOPOLOGY){
		os<<"#@ rectangular grid of size " << NNet.NetworkSizeU << "," << NNet.NetworkSizeV << "\n";
		if (NNet.Tags & NN_CYCLIC_U_DIR)
			os<< "#@Grid is Cyclic in the U direction \n";
		if (NNet.Tags & NN_CYCLIC_V_DIR)
			os<< "#@Grid is Cyclic in the V direction \n";
	}
	else
		os<<"#@ non rectangular grid with "<< NNet.Neurons.Size << " Neurons and "  << NNet.Faces.Size << " faces \n";

	os <<"Separator {\n\n    DirectionalLight { direction 0 0 -1 }\n    Separator {                 # Object Separator \n      Material { emissiveColor 0.3 0.8 0.2 }\n      Material { emissiveColor 0.752941 0.752941 0.752941 }\n";

	// first make sure that the neurons are renumbered preperly
	NNet.RenumberNeurons();
	// output all neurons as points with additional information
	os<<"# point format is : x y z, comment_mark number_of_edges_connected number_of_faces_connected\n";
	os<<"      Coordinate3\n            {\n           point [\n\n";
	Neuron *It = NNet.Neurons.Head;
	if (NNet.Neurons.Head) do{
		os <<"                     ";
		os << (MyPoint) (*It) ;

		os.seekp( -2, ios::cur ); // eat last new line character

		// the last neuron is not terminated with a comma
		if (It->Next!= NNet.Neurons.Head){
			os << ",";  // add , seperator
		}

		os << " #@ " << It->EdgeCounter ; // add a vrml comment describing the vertex edge velance

		int FaceVelance;
		// since we do not hold faces for rectangular topology, we need to handle it differently
		if (NNet.Tags &  NN_RECTANGULAR_TOPOLOGY){
			switch (It->EdgeCounter){
			case 2:
				FaceVelance=1;
				break;
			case 3:
				FaceVelance=2;
				break;
			case 4:
				FaceVelance=4;
				break;
			default:
				MyFatalError("OutputToVrmlFormat :: bad number of eges for neuron");
			}
		}
		else{
			FaceVelance=It->FaceCounter;
		}
		os << " " << FaceVelance ; // add a vrml comment describing the vertex face velance

		os << "\n"; // terminate line

		It=It->Next;
	} while (It != NNet.Neurons.Head);
	os<<"           ]           # End of points\n               }              # End of coords \n";

	// in rectangular topology  output the boundary as a set of edges
	if (NNet.Tags &  NN_RECTANGULAR_TOPOLOGY){
		NNet.OutputRectangularTopologyBoundaryToVRML(os);
	}

		
	os<<"      IndexedFaceSet\n                {\n                coordIndex [\n\n";
	// if this is a rectangular topology network then declare its size in a remark and create traingular faces for output

	if (NNet.Tags &  NN_RECTANGULAR_TOPOLOGY){
		NNet.OutputRectangularTopologyFacesToVRML(os);
	}
	else{

		Face *F = NNet.Faces.Head;
		if (NNet.Faces.Head) do{
			os <<"                     ";
			OutputToVrmlFormat(os,(*F));
			//os.seekp( -2, ios::cur ); // eat last new line character
			//os <<",\n";
			F=F->Next;
		} while (F != NNet.Faces.Head);

	}
	os.seekp( -3, ios::cur ); // eat last new line character and comma
	os <<"\n";
	os <<"                           ]   # End of coord index\n                }              # End of indexedfaceset\n\n             }                 # End of object separator\n\n          }                    # End of main separator \n";

	return os;

}

ostream& NeuralNet::OutputRectangularTopologyFacesToVRML(ostream& os){
// the function simulates the rectangular faces of the grid and outputs them in vrml format
	int UCyclicIndexCorrection = (Tags & NN_CYCLIC_U_DIR)?0:-1;
	int VCyclicIndexCorrection = (Tags & NN_CYCLIC_V_DIR)?0:-1;

	for (int i=0; i < (NetworkSizeU + UCyclicIndexCorrection) ; i++) 
		for (int j=0; j < (NetworkSizeV + VCyclicIndexCorrection) ; j++){
		Neuron *N1, *N2, *N3, *N4;
		N1=RECTNN(i,j);
		N2=RECTNN((i+1) % NetworkSizeU,j);
		N3=RECTNN((i+1) % NetworkSizeU,(j+1) % NetworkSizeV);
		N4=RECTNN(i,(j+1) % NetworkSizeV);
		os <<"                     ";
		OutputToVrmlFormat(os,N1,N2,N3,N4);
	}

	return os;
}

ostream& NeuralNet::OutputRectangularTopologyBoundaryToVRML(ostream& os){
// the function traverses the boundaries of the mesh and outputs the cycle of vertices creating it 

	int UCyclicIndexCorrection = (Tags & NN_CYCLIC_U_DIR)?1:0;
	int VCyclicIndexCorrection = (Tags & NN_CYCLIC_V_DIR)?1:0;

	// if cyclic conditions in both directions, then do nothing (no boundary)
	if ((Tags & NN_CYCLIC_U_DIR) && (Tags & NN_CYCLIC_V_DIR))
	return os; 
		
	// output the boundary as a strip of points
	os << "           # Grid Boundary is represented as by a set of point numbers terminated by -1\n";
	os << "           #@ Boundary{ \n";

	if (!(Tags & NN_CYCLIC_U_DIR)){
		os <<"                     #@ ";
		for (int i=0; i < (NetworkSizeU + UCyclicIndexCorrection) ; i++) {
			Neuron *N;
			N=RECTNN(i% NetworkSizeU,0);
			os << N->NeuronId-1 <<", ";
		}
		os << "-1\n";  // terminate boundary
	}
	if (!(Tags & NN_CYCLIC_V_DIR)){
		os <<"                     #@ ";
		for (int j=0; j < (NetworkSizeV + VCyclicIndexCorrection) ; j++) {
			Neuron *N;
			N=RECTNN(NetworkSizeU-1,j % NetworkSizeV);
			os << N->NeuronId-1 <<", ";
		}
		os << "-1\n";  // terminate boundary
	}
	if (!(Tags & NN_CYCLIC_U_DIR)){
		os <<"                     #@ ";
		for (int i=0; i < (NetworkSizeU + UCyclicIndexCorrection) ; i++) {
			Neuron *N;
			N=RECTNN(i% NetworkSizeU,NetworkSizeV-1);
			os << N->NeuronId-1 <<", ";
		}
		os << "-1\n";  // terminate boundary
	}
	if (!(Tags & NN_CYCLIC_V_DIR)){
		os <<"                     #@ ";
		for (int j=0; j < (NetworkSizeV + VCyclicIndexCorrection) ; j++) {
			Neuron *N;
			N=RECTNN(0,j % NetworkSizeV);
			os << N->NeuronId-1 <<", ";
		}
		os << "-1\n";  // terminate boundary
	}

	os << "           #@ } # end of boundary\n";

	return os;
}



bool NeuralNet::CheckNetworkTypeValidity(int NewNetType){

	bool result=false;

	// neural gas - with or without hebian learning
	result = result || 
		(NewNetType & NN_NO_TOPOLOGY) &&
		!(NewNetType & (
		NN_RECTANGULAR_TOPOLOGY | 
		NN_CYCLIC_U_DIR |
		NN_CYCLIC_V_DIR |
		NN_CAN_GROW_NEURONS));
		
	// self orgenizing map (rect topology) or growing grid
	result = result || 
		(NewNetType & NN_RECTANGULAR_TOPOLOGY) &&
		!(NewNetType & (
		NN_NO_TOPOLOGY | 
		NN_HAS_TOPOLOGY)) && 
		~((NewNetType & (NN_CAN_GROW_NEURONS)) ^
		 (NewNetType & (NN_CAN_GROW_EDGES  )) );

	// self orgenizing feature map (arbitrary topology)
	// or growing neural gas.
	result = result || 
		(NewNetType & NN_HAS_TOPOLOGY) &&
		!(NewNetType & (
		NN_NO_TOPOLOGY| 
		NN_RECTANGULAR_TOPOLOGY|
		NN_CYCLIC_U_DIR|
		NN_CYCLIC_V_DIR)) && 
		~((NewNetType & (NN_CAN_GROW_NEURONS)) ^
		 (NewNetType & (NN_CAN_GROW_EDGES  )) );


	return(result);

}


bool NeuralNet::ChangeNetType(int NewNetType, PointList &PtLst){

	if (!CheckNetworkTypeValidity(NewNetType)){
		TRACE ("NetWorkType given is faulty in function ChangeNetType");
		return false;
	}
	
	// in case the network now is neural gas
	if ((Tags & NN_NO_TOPOLOGY) &&
		(Tags & NN_CAN_GROW_EDGES)){
		if (NewNetType & NN_RECTANGULAR_TOPOLOGY){
			TRACE("NeuralNet::ChangeNet - Typecannot change from non rectangular topology to rectangular in function ChangeNetType");
			return false;
		}
		else if (NewNetType & NN_HAS_TOPOLOGY){
			TRACE ("NeuralNet::ChangeNet - changing from no topology to other topology - deleting unconnected neurons");
			DeleteDisconnectedNeurons();
			Tags=NewNetType;
			return true;
		}
		else {
			TRACE ("NeuralNet::ChangeNet - new net type not supported");
			return false;
		}
	}
	// in case the network now  has rectangular topology
	else if (Tags & NN_RECTANGULAR_TOPOLOGY){
		// in case cyclic conditions have been introduced
		// add the appropriate edges
		if ((NewNetType & NN_CYCLIC_U_DIR) &&
			!(Tags & NN_CYCLIC_U_DIR)){
			for (int j=0;j<NetworkSizeV;j++){
				Neuron *pN,*pNu;
				pN=NeuronArray[(NetworkSizeU-1)*NetworkSizeV+j];
				pNu=NeuronArray[j];			
				Edges.CreateOrRefreshEdge(pN,pNu,EDGE_U_DIR);
			}
		}
		if ((NewNetType & NN_CYCLIC_V_DIR) &&
			!(Tags & NN_CYCLIC_V_DIR)){
			for (int i=0;i<NetworkSizeU;i++){
				Neuron *pN,*pNv;
				pN=NeuronArray[i*NetworkSizeV+NetworkSizeV-1];
				pNv=NeuronArray[i*NetworkSizeV];			
				Edges.CreateOrRefreshEdge(pN,pNv,EDGE_V_DIR);
			}
		}
		// if cyclic conditions have been removed, remove the
		// the appropriate edges
		if ((Tags & NN_CYCLIC_U_DIR) &&
			!(NewNetType & NN_CYCLIC_U_DIR)){
			for (int j=0;j<NetworkSizeV;j++){
				Neuron *pN,*pNu;
				pN=NeuronArray[(NetworkSizeU-1)*NetworkSizeV+j];
				pNu=NeuronArray[j];			
				Edges.DelEdge(pN,pNu);
			}
		}
		if ((Tags & NN_CYCLIC_V_DIR) &&
			!(NewNetType & NN_CYCLIC_V_DIR)){
			for (int i=0;i<NetworkSizeU;i++){
				Neuron *pN,*pNv;
				pN=NeuronArray[i*NetworkSizeV+NetworkSizeV-1];
				pNv=NeuronArray[i*NetworkSizeV];			
				Edges.DelEdge(pN,pNv);

			}
		}
		// in case rectangular topology is deleted
		if ( (NewNetType & NN_NO_TOPOLOGY) |
			 (NewNetType & NN_HAS_TOPOLOGY) ){
			// delete the helper array
			delete [] NeuronArray;
			NeuronArray=NULL; //we won't need it anymore
			// now delete U and V direction Tags from the edges
			Edge *It =Edges.Head;
			if (Edges.Head!=NULL) do{
				It->Tag &= ~(EDGE_U_DIR|EDGE_V_DIR);
				It=It->Next;
			} while (It != Edges.Head);

			// reset the point list weights
			PtLst.ResetPointWeight();

			//networks size counting is now not in two directions
			NetworkSizeU = NetworkSizeU*NetworkSizeV;	
			NetworkSizeV = 0;	
		} 

		Tags=NewNetType;
		TRACE ("NeuralNet::ChangeNet - modifying rectangular topology");
		return true;
	}
	// in case of growin neural gas of self orgenizing map (arbitrary topology)
	else if (Tags & NN_HAS_TOPOLOGY){
		if (NewNetType & NN_RECTANGULAR_TOPOLOGY){
			TRACE("NeuralNet::ChangeNet - cannot change from non rectangular topology to rectangular in function ChangeNetType");
			return false;
		}
		else{
			TRACE("NeuralNet::ChangeNet - changing network type from has topology to other network type");
			Tags=NewNetType;
			return true;
		}
	}
	TRACE("NeuralNet::ChangeNet - new net type not found");
	Tags=NewNetType;
	return false;
}


void NeuralNet::NeuronSortByWinners(SamplePoint &Pt,int k, bool ClassifySamplePoint , Edge *E){
 // reorders the neuron list so that the first k neurons nearest to the sampled point are first (sorted by distance)
 // if k is zero it means the whole array must be sorted

	if (Neurons.Size==0) return;
	if (k<0) MyFatalError(" NeuronSortByWinners : parameter k can't be negative");

	
#ifdef MY_DEBUG
	Neuron *DebugWinners[3]={NULL,NULL,NULL};
#endif

	// if we are looking for up to 3 neigbours - do it fast
	if (k<=3 && k<Neurons.Size && k>0 && !ClassifySamplePoint){

		TIC(8)

		int HitCounter = 0;
		int SearchRadius = 0;
		
		int IndexX = (int) ((Pt.Pos[0] - LocationLowX) / LocationDeltaX);
		int IndexY = (int) ((Pt.Pos[1] - LocationLowY) / LocationDeltaY);
		int IndexZ = (int) ((Pt.Pos[2] - LocationLowZ) / LocationDeltaZ);

		while (HitCounter <= k){
			HitCounter = 0;
			SearchRadius++;
			for (int i= max(IndexX-SearchRadius,0) ; i<= min(LocationSizeX-1,IndexX+SearchRadius) ; i++){
				for (int j= max(IndexY-SearchRadius,0) ; j<= min(LocationSizeY-1,IndexY+SearchRadius) ; j++){
					for (int k= max(IndexZ-SearchRadius,0) ; k<= min(LocationSizeZ-1,IndexZ+SearchRadius) ; k++){
						Neuron *N = NEURON_LOCATION(i,j,k);
						while (N != NULL){
							HitCounter++;
							N->CalcDistFromSamplePoint(Pt);
							Neurons.Remove(N,false);
							Neurons.PushBack(*N,false);
							N=N->NextInLoactionStuck;
						}
					}
				}
			}
		}

  
		// reset the start position of the sort to the first neuron we found
		Neuron *SortHead=Neurons.Head;
		for (int i=0; i<HitCounter; i++)
			SortHead =SortHead->Prev;

		//sort by increasing distance
			Neurons.Sort(k,SortHead,HitCounter);

#ifdef MY_DEBUG

		DebugWinners[0] = Neurons.Head;
		DebugWinners[1] = Neurons.Head->Next;
		DebugWinners[2] = Neurons.Head->Next->Next;

		ASSERT (DebugWinners[0]->PointDist <= DebugWinners[1]->PointDist);
		ASSERT (DebugWinners[1]->PointDist <= DebugWinners[2]->PointDist);
#endif


		TOC(8)


	}
  

	// debug check - look if three first neurons are the same using both methods
	else {

		TIC(9)
		
		Neuron *It = Neurons.Head;
		//update distances
		do{
			RealType d = It->CalcDistFromSamplePoint(Pt);
			It=It->Next;
		}while (It!=Neurons.Head);

		//sort by increasing distance
			Neurons.Sort(k);

#ifdef MY_DEBUG

			if (DebugWinners[0]!=NULL && DebugWinners[0] !=Neurons.Head)
				MyFatalError ("did not find winner 1 correctly");

			if (DebugWinners[1]!=NULL && DebugWinners[1] !=Neurons.Head->Next)
				MyFatalError ("did not find winner 2 correctly");

			if (DebugWinners[2]!=NULL && DebugWinners[2] !=Neurons.Head->Next->Next)
				MyFatalError ("did not find winner 3 correctly");

#endif

		TOC(9)
	}

	if (ClassifySamplePoint) // if classification is requested do it
		Pt.Classification= Neurons.Head;
}



int NeuralNet::CalcTopologicalDistance (Neuron *N,int k,bool UseOnlyConnectedEdges){
	//calculate the toplogical distance of all neurons using the new data and sort them by it, distances up to k are calculated
	// we use simple BFS algorithm on the graph
	// the function returns the number of neurons that fall under distance k
	// when UseOnlyConnectedEdges is true, only edges with one or more faces connected to them are used

	if (Neurons.Size==0) return (-1); // -1 in output means an error
	if (k<0) MyFatalError(" CalcTopologicalDistance : parameter k can't be negative");
		
	RealType MaxDist=Neurons.Size*2; // topological distance can't get so high
	//init distances
	Neuron *It=Neurons.Head;
	
	do{
		It->PointDist=MaxDist;
		It=It->Next;
	} while (It != Neurons.Head);

	//search for initial neuron from the list
	// put it at the back of the list and set the Iterator to it
	// point to the last elemnt (our first neuron

	
	N->PointDist=0; // initial neuron distance is 0
	Neurons.Remove(N,false);
	Neurons.PushBack(*N,false);
	It=N;

	int NeigbourhoodSize=1; // retains number of neigbours found.

	while ((It->PointDist < k)){ // while distance has not reached k or we want to sort all the neurons
		// now find all direct neibouring neurons and add them to the back of the list, 
		// they will be processed later on
		// for the sake of speed we do not find the neigbours in a function but do it online...
		// maybe we should add it later as a member function to the EdgeList class
		// also an optimization for rectangular SOM can be added later on since neurons are ordered in an array

		Edge *Ed=It->EdgeStuckHead;
		while (Ed != NULL ){
			if ( !UseOnlyConnectedEdges || Ed->FaceCounter>0 ){
				Neuron *N2 = Ed->OtherNeuronInEdge(It);
				if (N2->PointDist > It->PointDist){
					N2->PointDist = It->PointDist+1;

					// put it at the back of the list and set the Iterator to it
					Neurons.Remove(N2,false);
					Neurons.PushBack(*N2,false);
					NeigbourhoodSize++; // increase number of neigbours found.
				}
			}
			Ed=It->NextEdgeInEdgeStuck(Ed);
		}

		It=It->Next; //advance to the next unhandles neighbour
		if (It==Neurons.Head) break; //if no more neibours exist at end of list - exit loop
	}

	// put the located neurons in the begining of the list
	for(int i=0;i<NeigbourhoodSize;i++)	{
		Neurons.Head=Neurons.Head->Prev;
	}

	return (NeigbourhoodSize);
}




bool NeuralNet::TrainSelfOrgenizingFeatureMapsBySinglePoint (SamplePoint &Pt, RealType Eps, RealType Sigma, RealType Radius ,int CurrentIteration, bool TrainBoundaryOnly, double SamplePointIncreaseWeight ,bool UseClassification , bool UseCorrectTopologicalDistanceForRectangularTopology){

	// trains a SOFM by a single sample point
	// updating only the boundary can be done differently and efficiently, but it is not a nesesity, so we do it the dirty way


	if (Neurons.Size==0) return (false);

	//classify for boundary training 
	NeuronSortByWinners(Pt,1,true);

	Neuron *Winner = Neurons.Head;

	if (UseClassification){ // in this case find the neuron with the same patch classification
		Neuron *It=Neurons.Head;
		do{
			if (It->PatchClassification == Pt.Classification->PatchClassification)
				break;
			It=It->Next;
		} while (It!=Neurons.Head);
		// in case no neuron with similar classification is found procced as normal
		Winner=It;

	}




	// update winner counter
	Winner->LastWinningAge=CurrentIteration;

	// update win counter only if not updatin boundary
	//if (!TrainBoundaryOnly)
		Winner->WinCounter++;


	if ( (Tags & NN_RECTANGULAR_TOPOLOGY)){
		// if boundary neuron increase weight
		if (Winner->NeuronIdU==0 || Winner->NeuronIdU==NetworkSizeU-1 || Winner->NeuronIdV==0 || Winner->NeuronIdV==NetworkSizeV-1){
			if (TrainBoundaryOnly){
				Pt.Weight += SamplePointIncreaseWeight;
				//ChangeWeightOfOusideBorderPoint(Pt , SamplePointIncreaseWeight);
				//train only "outside points"
				//if (Pt.Weight==1.0) 
				//	return (false);
			}
			
		}
		else{ // if not on border reset weight anyway 
			Pt.Weight=1.0;
			//if we are training only boundary neurons and we are not on boundary exit function
			if (TrainBoundaryOnly) 
				return (false); // this means point was not trained cause it was not on boundary
		}
	}

	if ( (Tags & NN_RECTANGULAR_TOPOLOGY) && (Radius < NetworkSizeU) && (Radius < NetworkSizeV)){ 
		// we want to speed up calculation for rectangular topology)
		// if the radius is small this loop is effective and causes no double updates

		int i,j,ii,jj;

		// walk only in a diamond shape around the point
		for (i =  - (int)Radius ; i<= (int)Radius ; i++)  
			for (j = -(int)Radius+ abs(i) ; j <= (int)Radius-abs(i) ; j++){

			// compensate indices in neuron array for cyclic boundary conditions
			if (Tags & NN_CYCLIC_U_DIR)
				ii= (Winner->NeuronIdU+i) % NetworkSizeU;
			else 
				ii=max(0,min((Winner->NeuronIdU+i),NetworkSizeU-1));

			if (Tags & NN_CYCLIC_V_DIR)
				jj= (Winner->NeuronIdV+j) % NetworkSizeV;
			else 
				jj=max(0,min((Winner->NeuronIdV+j),NetworkSizeV-1));

			// if check if updating the neuron is allowed
			if (!TrainBoundaryOnly || ii==0 || ii==NetworkSizeU-1 || jj==0 || jj==NetworkSizeV-1){
				// calculate topological distance
				int d;
				if (!UseCorrectTopologicalDistanceForRectangularTopology)
					d = abs(i)+abs(j);
				else
					d = (int) (sqrt ((double)(i*i + j*j)));

				if (Pt.Weight == 1.0)
					RECTNN(ii,jj)->ModifyPosBySamplePoint(Eps*exp(-d*d/(2*Sigma*Sigma)),Pt);	
				else
					RECTNN(ii,jj)->ModifyPosBySamplePoint(1.0-pow(1.0-Eps*exp(-d*d/(2*Sigma*Sigma)),Pt.Weight),Pt);
				// update age update
				RECTNN(ii,jj)->LastUpdateAge=CurrentIteration;
			}

		}
		return (true); // point was not overlooked
	}


	// sort neurons by topological distances
	int Neighbours = CalcTopologicalDistance (Winner,(int)Radius);

	Neuron *It=Winner;

	//modifiy each neuron
	for (int i=0; i<Neighbours;i++){
		// if check if updating the neuron is allowed
		if ( !(Tags & NN_RECTANGULAR_TOPOLOGY) || !TrainBoundaryOnly || It->NeuronIdU==0 || It->NeuronIdU==NetworkSizeU-1 || It->NeuronIdV==0 || It->NeuronIdV==NetworkSizeV-1){
			RealType d = It->PointDist;
			if (Pt.Weight == 1.0){
				It->ModifyPosBySamplePoint(Eps*exp(-d*d/(2*Sigma*Sigma)),Pt);	
			}
			else
				It->ModifyPosBySamplePoint(1.0-pow(1.0-Eps*exp(-d*d/(2*Sigma*Sigma)),Pt.Weight),Pt);
				// approximate normal using PCA and neighbourhood
				It->UpdateNormalUsingPCA (Pt,Eps*exp(-d*d/(2*Sigma*Sigma)));
			It->LastUpdateAge=CurrentIteration;
		}
		It=It->Next;
	}

	return (true); // point was not 
//	} recheck JAC***
}

int NeuralNet::TrainSelfOrgenizingFeatureMapsByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsInit, RealType EpsFinal, RealType SigmaInit, RealType SigmaFinal, RealType RadiusInit, RealType RadiusFinal, double LambdaGrowth , int TrainBoundaryOnlyCycleIterations, double TrainBoundaryOnlySamplePointWeight , int TrainBoundaryOnlyIterations , bool UseClassification){
	// the function trains the neural network as a self organizing feature map
	// rectangular topology feature maps can be trained by boundary sample points training only the
	// boundary with he weiughts of outside points enhanced.
	// LambdaGrowth trains a growing grid if the topology is rectangular and neurons and edges can grow
	int BoundaryPointsTried=0;

	if (PtLst.Size==0){
		TRACE ("TrainSelfOrgenizingFeatureMapsByPointList : no points to train");
		return false;
	}
	
	if ( (EpsInit==0.0) || (SigmaInit==0.0) || (RadiusInit==0.0) || (LambdaGrowth <0)){ 
		TRACE ("TrainSelfOrgenizingFeatureMapsByPointList : init values Eps, Sigma, Radius cant be zero");
		return false;
	}

	if(TrainBoundaryOnlyCycleIterations !=0 && TrainBoundaryOnlyIterations !=0 && !(Tags & NN_RECTANGULAR_TOPOLOGY)){
		TRACE ("NeuralNet::TrainSelfOrgenizingFeatureMapsByPointList : boundary of non rectangular topology is not implemented yet");
		return false;
	}

	if ( !((Tags & NN_HAS_TOPOLOGY) || (Tags & NN_RECTANGULAR_TOPOLOGY)) ){
		TRACE ("NeuralNet::TrainSelfOrgenizingFeatureMapsByPointList - training method is incompatible with Neural Network type");
		return false;
	}


	// notice that there may be trouble here if updating the boundary.
	int Index=BaseIteration % PtLst.Size;
	
	for (int i=0; i<RunLength; i++){

		RealType Eps,Sigma,Radius;
		
		Eps = EXP_DECAY(EpsInit,EpsFinal,i,RunLength);
		Sigma = EXP_DECAY(SigmaInit,SigmaFinal,i,RunLength);
		Radius = EXP_DECAY(RadiusInit,RadiusFinal,i,RunLength);

		// if point not trained (due to boundary training) do not increase index i;
		if ( ! TrainSelfOrgenizingFeatureMapsBySinglePoint (*PtLst.Array[Index], Eps, Sigma, Radius, BaseIteration + i, TrainBoundaryOnlySamplePointWeight!=0.0 && ((BaseIteration + i) % TrainBoundaryOnlyCycleIterations > TrainBoundaryOnlyIterations) ,TrainBoundaryOnlySamplePointWeight) && (BoundaryPointsTried < PtLst.Size)) {
			i--;
			BoundaryPointsTried++; // in case no point is classified as out point - this may happen
		}


		// if growing grid and LambdaGrowth is defined grow
		if ( ((BaseIteration+i) == (NetworkSizeU*NetworkSizeV*LambdaGrowth)) && (Tags & NN_RECTANGULAR_TOPOLOGY) && (Tags & NN_CAN_GROW_NEURONS) && (Tags & NN_CAN_GROW_EDGES) && (LambdaGrowth !=0) )
			// if training uses enhanced boundary, we use it in growing
			AddGrowingGridRowColumn( (TrainBoundaryOnlyCycleIterations !=0) && (TrainBoundaryOnlyIterations !=0) ); //the grid should grow

		Index=(Index+1) % PtLst.Size;
	}



	return true;
}



void NeuralNet::AddGrowingGridRowColumn(bool BoundaryExpandsOnlyOutwards){
	// this function adds a row or a colums to the growing grid 
	// when needed by the training function :TrainGrowingGridByPointList
	// no parameters are given and there is no return value.
	// if BoundaryExpandsOnlyOutwards is set, then if the winner is on the boundary, a row /column is added not by the highest distance, but a row is added for a row border and a column for a column border

	// find the neuron with largest winning counter
	int i,j,iWin,jWin;
	iWin=0;
	jWin=0;
	for (i=0; i<NetworkSizeU; i++) for (j=0; j<NetworkSizeV; j++){
		if (RECTNN(i,j)->WinCounter > RECTNN(iWin,jWin)->WinCounter){
			iWin=i;
			jWin=j;
		}
	}

	Neuron *TN= RECTNN(iWin,jWin);
	TN->WinCounter =-100;

	// find nearest neighbour from the four
	// we will need those indices - l,r,t,b, stands for left,right,top,bottom
	int ir=iWin+1;
	int il=iWin-1;
	int jt=jWin+1;
	int jb=jWin-1;

	// compensate indices for cyclic boundaries
	if (Tags & NN_CYCLIC_U_DIR){
		ir=ir%NetworkSizeU;
		il=il%NetworkSizeU;
	}

	if (Tags & NN_CYCLIC_V_DIR){
		jt=jt%NetworkSizeV;
		jb=jb%NetworkSizeV;
	}

	// calculate distance between winner and direct neihbours
	RealType dr = (iWin == NetworkSizeU-1 ) ? -1.0 :~(*RECTNN(iWin,jWin) - *RECTNN(ir,jWin));
	RealType dl = (iWin == 0) ? -1.0 :~(*RECTNN(iWin,jWin) - *RECTNN(il,jWin));
	RealType db = (jWin == 0) ? -1.0 :~(*RECTNN(iWin,jWin) - *RECTNN(iWin,jb));
	RealType dt = (jWin == NetworkSizeV-1 ) ? -1.0 :~(*RECTNN(iWin,jWin) - *RECTNN(iWin,jt));


	RealType dMax;

	if  (BoundaryExpandsOnlyOutwards && 
		  ( ((iWin==0) || (iWin==NetworkSizeU-1)) ^
		    ((jWin==0) || (jWin==NetworkSizeV-1)) )
		){ // special case on the boundaries but not on the corners and boundary behaves differently
		if (iWin==0) dMax=dr;
		if (iWin==NetworkSizeU-1) dMax=dl;
		if (jWin==0) dMax=dt;
		if (jWin==NetworkSizeV-1) dMax=db;
	}
	else{
	// find the maximum distance between the four
		dMax=max(dr,max(dl,max(db,dt)));
	}

	// define new neuron array
	Neuron **NewNeuronArray;

	// now add the row/column in the array.

	// these macors are needed only in this stage of the program and not elsewhere
	#define RECTNNNEWU(i,j) NewNeuronArray[(i)*(NetworkSizeV)+(j)]
	#define RECTNNNEWV(i,j) NewNeuronArray[(i)*(NetworkSizeV+1)+(j)]


	if (dMax==dt || dMax==db){  // in that case you should add a column

		int jNewCol,jInterp;
		// in order to reduce if statment in the loop we check now what index 
		// should be added and what index should be interpolated
		// these two are different in case of cyclic conditions.
		// jInterp is the neuron index to interolate the winner with to calculate the new neuron value
		// jNewCol is the index of the new column to be inserted
		
		if (dMax==dt) {  
			jNewCol=jWin+1;
			jInterp=jt;
		}
		else {
			jNewCol=jWin;
			jInterp=jb;
		}

		// allocate memory for the new array.with one more colums added
		NewNeuronArray=new (Neuron (*[NetworkSizeU*(NetworkSizeV+1)]));


		// for debug purposes clear the array
		for (i=0;i<NetworkSizeU*(NetworkSizeV+1);i++) 
			NewNeuronArray[i] = NULL;


		// get last ID from rectangular array to give Id's for new neurons

#ifdef MY_DEBUG
		TRACE(" adding new column index = %i current net size is %i,%i\n - larest error neuron was %i,%i \n",jNewCol,NetworkSizeU,NetworkSizeV,iWin,jWin);
#endif
		// first create the neurons 
		for (i=0;i<NetworkSizeU;i++){
			for (j=0;j<NetworkSizeV+1;j++){
				if (j==jNewCol){  
					// in this case we should allocate memort for the neurons and add it colum array
					Neuron *TempN1= RECTNN(i,jWin);
					Neuron *TempN2= RECTNN(i,jInterp);
					MyPoint TempPt= (*TempN1+*TempN2)*0.5; 
					Neuron *TempNeuron = AddNeuron(TempPt); // create a neuron instance
					TempNeuron->NeuronIdU=i;
					TempNeuron->NeuronIdV=j;
					RECTNNNEWV(i,j) = TempNeuron;
					// delete the edge and add two others instead
					Edges.DelEdge(TempN1,TempN2);
#ifdef MY_DEBUG
					TRACE("creating col edges from Neuron %i,%i to neurons %i,%i and %i,%i\n", TempNeuron->NeuronIdU,TempNeuron->NeuronIdV,TempN1->NeuronIdU,TempN1->NeuronIdV,TempN2->NeuronIdU,TempN2->NeuronIdV);
#endif
					Edges.CreateOrRefreshEdge(TempN1,TempNeuron,EDGE_V_DIR);
					Edges.CreateOrRefreshEdge(TempN2,TempNeuron,EDGE_V_DIR);
					if (i!=0) {
#ifdef MY_DEBUG
						TRACE("creating row edge from Neuron %i,%i to neurons %i,%i\n", TempNeuron->NeuronIdU,TempNeuron->NeuronIdV,RECTNNNEWU(i-1,j)->NeuronIdU,RECTNNNEWU(i-1,j)->NeuronIdV);
#endif
						// add the row edge
						Edges.CreateOrRefreshEdge(RECTNNNEWV(i-1,j),TempNeuron,EDGE_U_DIR);
					}


				}
				else{ // we should copy pointer to existing neurons to the array
					int jj = (j>jNewCol) ? j-1 : j;  // the corrected index we will use
					RECTNNNEWV(i,j)=RECTNN(i,jj);  // copy from old to new array
					// correct indices of the neuron (it may move in index due to addition of column/row
					RECTNNNEWV(i,j)->NeuronIdU=i;
					RECTNNNEWV(i,j)->NeuronIdV=j;

				}

			}
		}
		NetworkSizeV++;   // increase number of columns by 1
		// add edge for cyclic boundary conditions 
		if (Tags & 	NN_CYCLIC_U_DIR){
			Edges.CreateOrRefreshEdge(RECTNNNEWU(NetworkSizeU-1,jNewCol),RECTNNNEWU(0,jNewCol),EDGE_U_DIR);
		}

	}

	else if (dMax==dr || dMax==dl){  // in that case you should add a column

		int iNewRow,iInterp;
		// in order to reduce if statment in the loop we check now what index 
		// should be added and what index should be interpolated
		// these two are different in case of cyclic conditions.
		// iInterp is the neuron index to interolate the winner with to calculate the new neuron value
		// iNewCol is the index of the new column to be inserted
		
		if (dMax==dr) {  
			iNewRow=iWin+1;
			iInterp=ir;
		}
		else {
			iNewRow=iWin;
			iInterp=il;
		}

		// allocate memory for the new array.with one more colums added
		NewNeuronArray=new (Neuron (*[(NetworkSizeU+1)*NetworkSizeV]));
		// for debug purposes clear the array
		for (i=0;i<(NetworkSizeU+1)*NetworkSizeV;i++) 
			NewNeuronArray[i] = NULL;
	
#ifdef MY_DEBUG
		TRACE(" adding new row index = %i current net size is %i,%i\n - larest error neuron was %i,%i \n",iNewRow,NetworkSizeU,NetworkSizeV,iWin,jWin);
#endif
		// first create the neurons 
		for (i=0;i<NetworkSizeU+1;i++){
			for (j=0;j<NetworkSizeV;j++){
				if (i==iNewRow){  
					// in this case we should allocate memort for the neurons and add a row
					Neuron *TempN1= RECTNN(iWin,j);
					Neuron *TempN2= RECTNN(iInterp,j);
					MyPoint TempPt= (*TempN1+*TempN2)*0.5; 
					Neuron *TempNeuron = AddNeuron(TempPt); // create a neuron instance
					TempNeuron->NeuronIdU=i;
					TempNeuron->NeuronIdV=j;
					RECTNNNEWU(i,j) = TempNeuron;
					// delete the edge and add two others instead
					Edges.DelEdge(TempN1,TempN2);
#ifdef MY_DEBUG
					TRACE("creating row edges from Neuron %i,%i to neurons %i,%i and %i,%i\n", TempNeuron->NeuronIdU,TempNeuron->NeuronIdV,TempN1->NeuronIdU,TempN1->NeuronIdV,TempN2->NeuronIdU,TempN2->NeuronIdV);
#endif
					Edges.CreateOrRefreshEdge(TempN1,TempNeuron,EDGE_U_DIR);
					Edges.CreateOrRefreshEdge(TempN2,TempNeuron,EDGE_U_DIR);
					if (j!=0) {
#ifdef MY_DEBUG
						TRACE("creating col edge from Neuron %i,%i to neurons %i,%i\n", TempNeuron->NeuronIdU,TempNeuron->NeuronIdV,RECTNNNEWU(i,j-1)->NeuronIdU,RECTNNNEWU(i,j-1)->NeuronIdV);
#endif
						// add the column edge
						Edges.CreateOrRefreshEdge(RECTNNNEWU(i,j-1),TempNeuron,EDGE_V_DIR);
					}

				}
				else{ // we should copy pointer to existing neurons to the array
					int ii = (i>iNewRow) ? i-1:i;  // the corrected index we will use
					RECTNNNEWU(i,j)=RECTNN(ii,j);  // copy from old to new array
					// correct indices of the neuron (it may move in index due to addition of column/row
					RECTNNNEWU(i,j)->NeuronIdU=i;
					RECTNNNEWU(i,j)->NeuronIdV=j;

				}

			}
			
		}
		// add edge for cyclic boundary conditions 
		if (Tags & 	NN_CYCLIC_V_DIR){
			Edges.CreateOrRefreshEdge(RECTNNNEWU(iNewRow,NetworkSizeV-1),RECTNNNEWU(iNewRow,0),EDGE_V_DIR);
		}

		
		NetworkSizeU++;   // increase number of rows by 1

	}

	// reset win counters
	//for (i=0;i<NetworkSizeU*NetworkSizeV;i++) 
	//		NewNeuronArray[i]->WinCounter = 0;




	// now exchange the neuron array and delete old memory
	delete [] NeuronArray;
	NeuronArray=NewNeuronArray;
	#undef RECTNNNEWU
	#undef RECTNNNEWV
	
}


void NeuralNet::TrainGrowingNeuralGasBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration , bool UseOnlyNeuronsWithFacesForNormalCalculation){

	//find 2 winner neurons
	//NeuronSortByWinners(Pt,2);
	//find 3 winner neurons
	NeuronSortByWinners(Pt,3,false);


	Neuron *Nwin,*Nwin2, *Nwin3;
	

	Nwin = Neurons.Head;
	Nwin2 = Neurons.Head->Next;
	Nwin3 = Neurons.Head->Next->Next;

	// update winner counter
	Nwin->LastWinningAge=CurrentIteration;
	Nwin->WinCounter++;

	// create o refresh the edge between the two winners
	Edge *E12 = Edges.CreateOrRefreshEdge(Nwin,Nwin2);

	// now check if the two other edges exist.
	// if so, we can create a face

	Edge *E13 = Edges.FindEdge (Nwin,Nwin3);
	Edge *E23 = Edges.FindEdge (Nwin2,Nwin3);

	bool FaceExists=false;
	if (E12!=NULL && E13!=NULL && E23!=NULL){
		Face *F=Faces.CreateOrRefreshFace (E12,E13,E23);
		FaceExists=true;
		// now look if there are any edges with bigger velance then allowed:
		DeleteNonManifoldFaces(F,E12,sqrt(3.0)/2.0);
		DeleteNonManifoldFaces(F,E13,sqrt(3.0)/2.0);
		DeleteNonManifoldFaces(F,E23,sqrt(3.0)/2.0);
		DeleteSingularNeuronFaces(F,Nwin);
		DeleteSingularNeuronFaces(F,Nwin2);
		DeleteSingularNeuronFaces(F,Nwin3);
	}

	// adapt the normal
	MyPoint NormalDirection,Pt_N1;

	// normal is updated in the direction of the point
	if (!UseOnlyNeuronsWithFacesForNormalCalculation || FaceExists){
		Pt_N1 = Pt - *Nwin;

		NormalDirection = Nwin->CalcNormalUsing3Points(Nwin2,Nwin3);
		// te noral is already normalized in this manner
		double Dir = NormalDirection * Pt_N1;
		if (Dir<=0.0) NormalDirection = - NormalDirection;
		
	}
	else{
		NormalDirection.Pos[0]=0.0;
		NormalDirection.Pos[1]=0.0;
		NormalDirection.Pos[2]=0.0;
	}
	// new corrected version
	MyPoint NewNormal = Nwin->Normal + ((NormalDirection-Nwin->Normal) * (EpsB));
	// old problematic version
	//MyPoint NewNormal = Nwin->Normal + (NormalDirection * (EpsB / ~NormalDirection));
	// now normalize
	//NewNormal = NewNormal * (1.0 / ~NewNormal);
	if (NewNormal.IsFinite() )
		Nwin->Normal = NewNormal;
	//Nwin->UpdateNormalUsingPCA (Pt,0.1);
	//Nwin->CalcNormalUsingSemiMinimalBoundingCone();
	// decide on normal calculation method (the code above)

	// accumulate error
	RealType dist = ~(Pt_N1);
	Nwin->Error += dist*dist; 

	// adapt winner

	if (Pt.Weight == 1.0)  // the if statment only saves computation time (it is negligable here, but we add it all the same
		Nwin->ModifyPosBySamplePoint(EpsB,Pt);	
	else
		Nwin->ModifyPosBySamplePoint(1.0-pow(1.0-EpsB,Pt.Weight),Pt);

	//modify position in location array if needed
	AddOrRemoveNeuronToLocationArray(Nwin);
	Nwin->LastUpdateAge = CurrentIteration;

	// adapt neibouring neurons
	int Neighbours = CalcTopologicalDistance (Nwin,1); // detect direct neibours
	Neuron *It = Neurons.Head->Next;  // skip the winner (who is now the first on the list)

	for (int i=0; i<Neighbours-1;i++){
		if (Pt.Weight == 1.0){
			It->ModifyPosBySamplePoint(EpsN,Pt);	
		}
		else
			It->ModifyPosBySamplePoint(1.0-pow(1.0-EpsN,Pt.Weight),Pt);
			// new corrected version
			NewNormal = It->Normal + ((NormalDirection-It->Normal) * (EpsN));
			// old problematic version
			//NewNormal = It->Normal + (NormalDirection * (EpsN / ~NormalDirection));
			NewNormal = NewNormal * (1.0 / ~NewNormal);
			
			if (NewNormal.IsFinite() )
			It->Normal = NewNormal;
			//It->UpdateNormalUsingPCA (Pt,0.1);
			//It->CalcNormalUsingSemiMinimalBoundingCone();
			// decide on normal calculation method (the code above)

		//modify position in location array if needed
		AddOrRemoveNeuronToLocationArray(It);

		It->LastUpdateAge=CurrentIteration;
	}

	//age all edges connected to the winner
	Edges.Age(Nwin,true); 
	DelOldEdgesAndCascadeDelete(AgeMax,true);

}



int NeuralNet::TrainGrowingNeuralGasByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LambdaGrowth ){
	if (PtLst.Size==0){
		MyFatalError ("TrainGrowingNeuralGasByPointList : no points to train");
		return false;
	}

	if ( EpsB<=0.0 || EpsN<=0.0 || Alpha<=0.0 || Betta <=0.0 || AgeMax <0 || LambdaGrowth <0 ) {
		MyFatalError ("TrainGrowingNeuralGasByPointList : init values Eps, Sigma, Radius cant be zero");
		return false;
	}
		if ( !( ((Tags & NN_HAS_TOPOLOGY) || (Tags & NN_RECTANGULAR_TOPOLOGY)) && (Tags & NN_CAN_GROW_NEURONS) && (Tags & NN_CAN_GROW_EDGES) )){
		TRACE ("NeuralNet::TrainGrowingNeuralGasByPointList  - training method is incompatible with Neural Network type");
		return false;
	}

	int Index = BaseIteration % PtLst.Size;
	for (int i=0; i<RunLength; i++){

		TrainGrowingNeuralGasBySinglePoint (*PtLst.Array[Index] , EpsB, EpsN, AgeMax , BaseIteration+i);

		if ( ! ( (BaseIteration+i+1) % LambdaGrowth) )  // every lambdagrowth iterations add a neuron
			AddGrowingNeuralGasNeuron(Alpha,PtLst);

		DecreaseGrowingNeuralGasError(Betta);
		Index = (Index +1) % PtLst.Size;
//		if (Index == 0) PtLst.Reorder();

	}

	return true;
}


void NeuralNet::DecreaseGrowingNeuralGasError(RealType Betta){
	// decrease the accumulated error of all neurons by betta
	Neuron *It=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		It->Error *= (1.0-Betta);
		It=It->Next;
	} while (It!=Neurons.Head);
}


void NeuralNet::AddGrowingNeuralGasNeuron(RealType Alpha , PointList &PtLst){
	// this function trains the neural net as 
	// neural gas with competative Hebian learning
	// for a single sample point.
	// alpha is the training magnitude.

	// determine the neuron with maximum error accumulated


	if (Neurons.Head==NULL) MyFatalError ("AddGrowingNeuralGasNeuron - empty list");

	Neuron *It=Neurons.Head; // set iterator
	Neuron *MaxErrNeuron = It;

	do{
		if (It->Error > MaxErrNeuron->Error)
			MaxErrNeuron=It;
		It=It->Next;
	} while (It!=Neurons.Head);


	//find neigbouring neurons
	int Neighbours = CalcTopologicalDistance (MaxErrNeuron,1); // detect direct neibours

#ifdef MY_DEBUG
	TRACE (" Neighbours = %d\n" , Neighbours);
#endif
	It=Neurons.Head->Next; // skip the winner (who is now the first on the list)

	//find neuron with max accumulated error
	Edge *MaxErrEdge = MaxErrNeuron->EdgeStuckHead;
	Edge *TempE=MaxErrEdge;

	while ( TempE !=NULL) {
		if (TempE->OtherNeuronInEdge(MaxErrNeuron)->Error > MaxErrEdge->OtherNeuronInEdge(MaxErrNeuron)->Error)
			MaxErrEdge = TempE;
		TempE=MaxErrNeuron->NextEdgeInEdgeStuck (TempE);
	}

	Neuron *MaxErrNeigbourNeuron = MaxErrEdge->OtherNeuronInEdge(MaxErrNeuron);

	// create a new neuron interpolating the two
	Neuron *NewNeuron = EdgeSplit(MaxErrEdge);

//	NewNeuron->W1 = (MaxErrNeigbourNeuron->W1 + MaxErrNeuron->W1)*0.5;
//	NewNeuron->W2 = (MaxErrNeigbourNeuron->W2 + MaxErrNeuron->W2)*0.5;

	NewNeuron->Normal = (MaxErrNeigbourNeuron->Normal + MaxErrNeuron->Normal);
	NewNeuron->Normal = NewNeuron->Normal * (1.0 / ~NewNeuron->Normal);

//	NewNeuron->CalcNormalUsingSemiMinimalBoundingCone();
	// decide on normal calculation method (the code above)

	// decrease accumulated error of the two edges that the edges were connedted to
	MaxErrNeuron->Error *= (1.0-Alpha);
	MaxErrNeigbourNeuron->Error *= (1.0-Alpha);
	//interpolate error for the new neuron
	NewNeuron->Error=(MaxErrNeuron->Error+MaxErrNeigbourNeuron->Error)/2;

	// if topology of network was rectangular, it has changed now.
	if (Tags & NN_RECTANGULAR_TOPOLOGY)
		ChangeNetType (( Tags & (~NN_RECTANGULAR_TOPOLOGY) ) | NN_HAS_TOPOLOGY,PtLst);
	
} 


Neuron *NeuralNet::AddNeuron(){
	// adds a neuron to the list, gives it and ID and returns its pointer	
	Neuron TempNeuron; 
	TempNeuron.NeuronId = (++NeuronIdMaxCounter);
	Neuron *N=Neurons.PushBack(TempNeuron);
	return (N);
}

void NeuralNet::AddOrRemoveNeuronToLocationArray(Neuron *N,bool Remove){

	// also add the neuron to the location array
	if (LocationSizeX!=0 && LocationSizeY!=0 && LocationSizeZ!=0){
		int IndexX = (int) ((N->Pos[0] - LocationLowX) / LocationDeltaX);
		int IndexY = (int) ((N->Pos[1] - LocationLowY) / LocationDeltaY);
		int IndexZ = (int) ((N->Pos[2] - LocationLowZ) / LocationDeltaZ);
		
		// compensate for last index problem
		if (IndexX==LocationSizeX) IndexX--;
		if (IndexY==LocationSizeY) IndexY--;
		if (IndexZ==LocationSizeZ) IndexZ--;

		ASSERT(IndexX<LocationSizeX && IndexY<LocationSizeY && IndexZ<LocationSizeZ);
		// change index only if point position had changed
		if ( Remove ||
			(IndexX != N->LocationIndexX &&
			 IndexY != N->LocationIndexY &&
			 IndexZ != N->LocationIndexZ) ){

			//first delete this neuron from its current location in the location array
			if (N->LocationIndexX >=0 ){

				ASSERT(NEURON_LOCATION_COUNT(N->LocationIndexX,N->LocationIndexY,N->LocationIndexZ));

				Neuron *StuckHead = NEURON_LOCATION(N->LocationIndexX,N->LocationIndexY,N->LocationIndexZ);
				if (StuckHead==N){
					NEURON_LOCATION(N->LocationIndexX,N->LocationIndexY,N->LocationIndexZ) = N->NextInLoactionStuck;
					NEURON_LOCATION_COUNT(N->LocationIndexX,N->LocationIndexY,N->LocationIndexZ) --;

				}
				else 
					while (StuckHead->NextInLoactionStuck != NULL){
						if (StuckHead->NextInLoactionStuck==N){
							StuckHead->NextInLoactionStuck=StuckHead->NextInLoactionStuck->NextInLoactionStuck;
							NEURON_LOCATION_COUNT(N->LocationIndexX,N->LocationIndexY,N->LocationIndexZ) --;
							break;
						}
						else
							StuckHead=StuckHead->NextInLoactionStuck;
				}
			
			}

			if(!Remove){
				Neuron *StuckHead = NEURON_LOCATION(IndexX,IndexY,IndexZ);
				N->NextInLoactionStuck = StuckHead;
				NEURON_LOCATION(IndexX,IndexY,IndexZ) = N;
				NEURON_LOCATION_COUNT(IndexX,IndexY,IndexZ)++;

				N->LocationIndexX=IndexX;
				N->LocationIndexY=IndexY;
				N->LocationIndexZ=IndexZ;
#ifdef MY_DEBUG
				// debug check 
				if (StuckHead != NULL && StuckHead->NextInLoactionStuck==StuckHead)
					MyFatalError("bug\n");
#endif
			}

		}
	}

#ifdef MY_DEBUG
	// debug check
	
		int Accm=0;
		for (int i=0;i<LocationSizeX ; i++) for (int j=0;j<LocationSizeY ; j++) for (int k=0;k<LocationSizeZ ; k++){
			int NumberInThisCell=0;
			Neuron *N = NEURON_LOCATION(i,j,k);
			while (N!=NULL){
				NumberInThisCell++;
				N=N->NextInLoactionStuck;
			}
			int LocCount = NEURON_LOCATION_COUNT(i,j,k);
			ASSERT (NumberInThisCell ==LocCount);
			Accm = Accm + LocCount;

		}
		
		ASSERT( Accm == Neurons.Size - Remove?1:0);
	
#endif

}


Neuron *NeuralNet::AddNeuron(MyPoint &InitPt){
	// adds a neuron to the list, gives it and ID and returns its pointer
	// it initializes the neuron to the initial point
	Neuron *TempNeuron = AddNeuron();
	TempNeuron->MyPoint::operator= (InitPt); // change point values in it
	AddOrRemoveNeuronToLocationArray(TempNeuron);
	return(TempNeuron);
}

RealType NeuralNet::ChangeWeightOfOusideBorderPoint (SamplePoint &Pt, double NewWeight){

	// this routine changes the new weight of border points - only if they are outside the border
	// this routine works only for rectangular domains
	// the function returns the oldweight of the sampled point
	
	if (! (Tags & NN_RECTANGULAR_TOPOLOGY) ) 
		MyFatalError("NeuralNet::ChangeWeightOfOusideBorderPoint - This function works only on rectangular topology ");

	int NeighbourU,NeighbourV;
	MyPoint d, dw, dn;
	RealType dd, OldWeight;

	int NeuronIdU=Pt.Classification->NeuronIdU;
	int NeuronIdV=Pt.Classification->NeuronIdV;

#ifdef MY_DEBUG
	// just a debug statement later to be dropped
	if (!((NeuronIdU == 0) || (NeuronIdU == NetworkSizeU-1) ||	(NeuronIdV == 0) ||	(NeuronIdV == NetworkSizeV-1)))
		MyFatalError("NeuralNet::ChangeWeightOfOusideBorderPoint - point not classified to border in U direction ");
#endif

	if (NeuronIdU == 0) NeighbourU=1;
	else if (NeuronIdU == NetworkSizeU-1) NeighbourU=NetworkSizeU-2;
	else NeighbourU=NeuronIdU;

	if (NeuronIdV == 0) NeighbourV=1;
	else if (NeuronIdV == NetworkSizeV-1) NeighbourV=NetworkSizeV-2;
	else NeighbourV=NeuronIdV;

	dw = Pt - *RECTNN(NeuronIdU,NeuronIdV);
	dn = Pt - *RECTNN(NeighbourU,NeighbourV);
	d = *RECTNN(NeuronIdU,NeuronIdV) - *RECTNN(NeighbourU,NeighbourV);
	dd=~d;

	RealType Distn = dn*d;
	RealType Distw = dw*d;
	OldWeight=Pt.Weight;
//	if ( (dn*d - dw*d) > (dd*dd) ) Pt.Weight += NewWeight;  // increase weight 
	if ( (Distn - Distw) > 0 ) Pt.Weight += NewWeight;  // increase weight 
	//else reset to 1
	else 
		Pt.Weight=1.0;
		// if the projecion of the distances is bigger then the edge size then the point is outside

	return OldWeight;
	
}


void NeuralNet::ClassifySamplePointList(PointList &PtLst){

	// this function classifies the sampled points, i.e. attaches the winner neuron pointer of the point
	Neuron *It = Neurons.Head;
	// first reset classification
	if (Neurons.Head != NULL) do {
		It->EmptyStuck();
		It=It->Next;
	} while (It != Neurons.Head);

	for(int i = 0 ; i<PtLst.Size; i++){
		NeuronSortByWinners(*PtLst.Array[i],1,true);
		(PtLst.Array[i])->Next=NULL; // reset clasification
		PtLst.Array[i]->Classification->PushClassifiedSampledPointToStuck( PtLst.Array[i]);
	}
}


void MyFatalError(char *ErrorMessage){
	TRACE ("NN fatal error: %s\n",ErrorMessage);
	ASSERT(0); // for debug value
//	exit(1);
}


int NeuralNet::DeleteDisconnectedNeurons(){
	
	Neuron *It=Neurons.Head;
	int NumberDeleted=0;
	// and for last - delete all neurons marksed for deletion
	do{
		if (It->EdgeCounter ==0){
			// this neuron has lost all of it's edges - delete it and free its memory
			// first delete from location then from list
			
			It=DelNeuron(It);
			NumberDeleted++;
			ASSERT (It); // this should never happed since it means no neurons in the list
		}
		else 
			It=It->Next;
	} while (It != Neurons.Head);

	return NumberDeleted;


}



int NeuralNet::DeleteNeuronsByEdgeMask(int EdgeTags)
{
	// this function deleted all the neurons that there are no edges connected to
	// it returns the number of neurons deleted

	Neuron *It=Neurons.Head;
	// first mark all neurons for removal,
	do{
		It->Tags |= NEURON_EDGES_REMOVED;
		It=It->Next;
	} while (It != Neurons.Head);


	// next remove tags where unneeded
	Edges.MaskConnectedNeurons(~NEURON_EDGES_REMOVED ,EdgeTags);

	int NumberDeleted=0;
	// and for last - delete all neurons marksed for deletion
	It = Neurons.Head;
	for (int i=0; i<Neurons.Size ; i++){
		if (It->Tags & NEURON_EDGES_REMOVED){
			// now delete All Edges Connected to the neuron
			Edges.DelEdges(It);
			// now you can delete the neuron itself
			// first delete from location then from list
			It=DelNeuron(It);
			i--;
			NumberDeleted++;
			ASSERT (It); // this should never happed since it means no neurons in the list
		}
		else 
			It=It->Next;
	}

	return NumberDeleted;
}


bool CheckItersection(Face *F, Neuron *N1, Neuron *N2){
	// check itersection with first Edge
	bool Intersection=false;

	MyPoint V1 = F->Edge1->Neuron2->Proj - F->Edge1->Neuron1->Proj;
	MyPoint V2 = N1->Proj - F->Edge1->Neuron1->Proj;
	MyPoint V3 = N2->Proj - F->Edge1->Neuron1->Proj;

	MyPoint Dir1=V1^V2;
	MyPoint Dir2=V1^V3;
	double Check=Dir1*Dir2;
	Intersection = Intersection || ( Check< MY_EPS);
	
	V1 = F->Edge2->Neuron2->Proj - F->Edge2->Neuron1->Proj;
	V2 = N1->Proj - F->Edge2->Neuron1->Proj;
	V3 = N2->Proj - F->Edge2->Neuron1->Proj;
	Dir1=V1^V2;
	Dir2=V1^V3;
	
	Check=Dir1*Dir2;
	Intersection = Intersection || ( Check< MY_EPS);

	V1 = F->Edge3->Neuron2->Proj - F->Edge3->Neuron1->Proj;
	V2 = N1->Proj - F->Edge3->Neuron1->Proj;
	V3 = N2->Proj - F->Edge3->Neuron1->Proj;

	Dir1=V1^V2;
	Dir2=V1^V3;

	Check=Dir1*Dir2;
	Intersection = Intersection || ( Check< MY_EPS);

	return Intersection;
}



bool NeuralNet::CheckItersectionForPruning(Neuron *NCenter, Neuron *N2 , int NeighbourNumber , Neuron **NeighboursArray){
	// check itersection with  all edges of velance more then 0
	

	Edge *E = Edges.Head;
	Edge *TheEdge = Edges.FindEdge(NCenter,N2);


	if (Edges.Head != NULL) do{
		// check that this is an edge between two neighboring neurons (without N2)
		for (int i=0; i<NeighbourNumber; i++){
			Neuron *It1=NeighboursArray[i];
			if (It1==N2) 
				continue ; // do not check edges emenating from N2
			
			for (int j=i+1; j<NeighbourNumber; j++){
				Neuron *It2=NeighboursArray[j];
				if (It2==N2)
					continue ; // do not check edges emenating from N2
				
				// checks whether there is an intersection of this edge with the edge emenatinf rom NCEnter (and that there is a triangle emenating from NCenter (this is a check that should be removed)
				if (E->FaceCounter != 0 && E->IsEdgeBetweenNeurons(It1,It2)){
					MyPoint V1 = (E->Neuron2->Proj - E->Neuron1->Proj);
					MyPoint V2 = (NCenter->Proj - E->Neuron1->Proj);
					MyPoint V3 = (N2->Proj - E->Neuron1->Proj);
				

					MyPoint Dir1=V1^V2;
					MyPoint Dir2=V1^V3;
					double Check1=Dir1*Dir2;
					
					V1 = (N2->Proj - NCenter->Proj);
					V2 = (E->Neuron1->Proj - NCenter->Proj);
					V3 = (E->Neuron2->Proj - NCenter->Proj);

					Dir1=V1^V2;
					Dir2=V1^V3;
					double Check2=Dir1*Dir2;

					//if (( Check1 < -MY_EPS ) && ( Check2 < -MY_EPS ))
					if (( Check1 < 0.0 ) && ( Check2 < 0.0 ))
						return true;
				}		
				It2=It2->Next;
			}
			It1=It1->Next;
		}
		E=E->Next;
	} while (E != Edges.Head);
	
	return false;
}




bool NeuralNet::CheckItersectionForPruning(Neuron *NCenter, Neuron *N2 , int Neighbours){
	// check itersection with  all edges of velance more then 0
	

	Edge *E = Edges.Head;
	Edge *TheEdge = Edges.FindEdge(NCenter,N2);


	if (Edges.Head != NULL) do{
		// check that this is an edge between two neighboring neurons (without N2)
		Neuron *It1=NCenter->Next;
		for (int i=1; i<Neighbours; i++){
			if (It1==N2) {
				It1=It1->Next;
				continue ; // do not check edges emenating from N2
			}
			Neuron *It2=It1->Next;;
			for (int j=i+1; j<Neighbours; j++){
				if (It2==N2){
					It2=It2->Next;				
					continue ; // do not check edges emenating from N2
				}
				// checks whether there is an intersection of this edge with the edge emenatinf rom NCEnter (and that there is a triangle emenating from NCenter (this is a check that should be removed)
				if (E->FaceCounter != 0 && E->IsEdgeBetweenNeurons(It1,It2)){
					MyPoint V1 = (E->Neuron2->Proj - E->Neuron1->Proj);
					MyPoint V2 = (NCenter->Proj - E->Neuron1->Proj);
					MyPoint V3 = (N2->Proj - E->Neuron1->Proj);
				

					MyPoint Dir1=V1^V2;
					MyPoint Dir2=V1^V3;
					double Check1=Dir1*Dir2;
					

					V1 = (N2->Proj - NCenter->Proj);
					V2 = (E->Neuron1->Proj - NCenter->Proj);
					V3 = (E->Neuron2->Proj - NCenter->Proj);

					Dir1=V1^V2;
					Dir2=V1^V3;
					double Check2=Dir1*Dir2;

					//if (( Check1 < -MY_EPS ) && ( Check2 < -MY_EPS ))
					if (( Check1 < 0.0 ) && ( Check2 < 0.0 ))
						return true;
				}		
				It2=It2->Next;
			}
			It1=It1->Next;
		}
		E=E->Next;
	} while (E != Edges.Head);
	
	return false;
}


bool NeuralNet::CheckFaceOclusionForPruning(Neuron *NCenter, Neuron *N){
	// check occlusion of N by a face belonging to the NCenter Neuron. if N2 is occluded by the face it needs pruning

	Face **FaceArray = NCenter->FacesBelongingToNeuron();

	for (int i=0; i<NCenter->FaceCounter; i++){
		Face *F = FaceArray[i];

		bool InFace = F->CheckIfProjNeuronIsInProjFace (N);

		if (InFace){ 
			// free memory of face array
			delete FaceArray;
			return true;
		}
	} 

	// free memory of face array
	delete FaceArray;
	
	return false;
}


/* this is an old version of this function
bool NeuralNet::CheckFaceOclusionForPruning(Neuron *NCenter, Neuron *N){
	// check occlusion of N by a face belonging to the NCenter Neuron. if N2 is occluded by the face it needs pruning


	Face **FaceArray = NCenter->FacesBelongingToNeuron();
	

	for (int i=0; i<NCenter->FaceCounter; i++){
		Face *F = FaceArray[i];
		
		// check that this is an edge between two neighboring neurons (without N2)
		if (F->IsFaceOfNeuron(NCenter)){
			Neuron *N2 , *N3;
			N2 = F->OtherNeuronInFace(NCenter,false);
			N3 = F->OtherNeuronInFace(NCenter,true);
			
			MyPoint V12,V13,V23,V1Pt,V2Pt,V3Pt;

			V12= N2->Proj - NCenter->Proj;
			V12 = V12 * (1.0/~V12);
			V13= N3->Proj - NCenter->Proj;
			V13 = V13 * (1.0/~V13);
			V23= N3->Proj - N2->Proj;
			V23 = V23 * (1.0/~V23);
			V1Pt= N->Proj - NCenter->Proj;
			V1Pt = V1Pt * (1.0/~V1Pt);
			V2Pt= N->Proj - N2->Proj;
			V2Pt = V2Pt * (1.0/~V2Pt);
			V3Pt= N->Proj - N3->Proj;
			V3Pt = V3Pt * (1.0/~V3Pt);

			double CAng1,CAng2,CAng3 ,CAng12Pt,CAng23Pt,CAng31Pt;

			// calc cosine of angles
			CAng1 = V12*V13;
			CAng2 = V23*(-V12);
			CAng3 = V13*V23;
			CAng12Pt = V1Pt*V12;
			CAng23Pt = V2Pt*V23;
			CAng31Pt = V3Pt*(-V13);

			if (CAng12Pt>CAng1 && CAng23Pt>CAng2){ 
				delete FaceArray;
				return true;
			}

		}
	} 

	// free memory of face array
	delete FaceArray;
	
	return false;
}

*/

/* this was an old version
void NeuralNet::DeleteNonManifoldFaces(){

	// this function goes over all the edges and 
	// for each edge with velance more than 1
	// it deletes enough faces to make it a manifold edge
	// the criterion for deletion is linked to theorem 3
	// in Amentas paper "One Pass delauney filtering for homeomorphic 3D surface reconstruction"
	// all faces of the edge are deleted except the one that
	// has the best corespondance between the face normal 
	// and the vertex normal at the largest angle vertex of the face

	

	Edge *E=Edges.Head;
	do {
		if (E->FaceCounter > 2){
			// filter criteria can't be that low
			double BestFilterCriteria = -2.0;


			// find best correspondense face
			Face *F = E->FacesStuckHead;
			Face *BestFace=F;
			Face *SecondBestFace=NULL;

			for (int i=0; i<E->FaceCounter; i++){
				F->CalcFilterCriteria();
				if ( SecondBestFace !=NULL && SecondBestFace->FilterCriteria < F->FilterCriteria){
					SecondBestFace=F;
				}
				if (BestFace->FilterCriteria < F->FilterCriteria){
					SecondBestFace=BestFace;
					BestFace=F;
				}
				F=E->NextFaceInFaceStuck(F);
			}

			// delete all other faces except from the best coresponding face
			F = E->FacesStuckHead;
			while (E->FaceCounter != 2){
				if (F != BestFace && F != SecondBestFace){
					Face *TempFace = F;
					F=E->NextFaceInFaceStuck(F);
					Faces.DelFace(TempFace);
				}
				else
					F=E->NextFaceInFaceStuck(F);
			}

		}
		E=E->Next;
	} while (E != Edges.Head);

}


*/




int NeuralNet::DeleteNonManifoldFaces(bool UseNormalProjection){   //(double MinCosAngleBetweenFaces){
	// The function goes over all neurons and faces connected to them and checks that 
	// the projection of faces in the Neuron normal direction do not overlap. (faces on the same fan of the neuron are not deleted)
	// the function returns the number of deleted faces

	// it is assumed that no edge has more then 2 faces connected to it
	
	// *** if the former does not work do the following algorithm  *** JAC
	// the projection of faces in the newest face normal direction do not overlap. (faces with a common edge are not checked).
	// older faces with a cos agle between faces smaller then MinCosAngleBetweenFaces are deleted.

	int DelCounter=0;


	// this does nothing - yet so just exit
	if (!UseNormalProjection)
		return 0;

	Neuron *N=Neurons.Head;
	do{
		// if this is a closed cape around the neuron then continue to next neuron (norma calculation may be wrong)
		if (N->EdgeCounter == N->FaceCounter ){
			N=N->Next;
			continue;
		}
		// first project all neighbour neurons in the direction of the normal
		
		Edge *E = N->EdgeStuckHead;
		MyPoint E1,E2,E3;
		N->CalcProjectionPlane ( *(E->OtherNeuronInEdge(N)) - *N ,E1,E2);
		N->Proj.Pos[0]=0.0;
		N->Proj.Pos[1]=0.0;
		N->Proj.Pos[2]=0.0;

		for (int i=0; i<N->EdgeCounter ; i++){
			E->OtherNeuronInEdge(N)->ProjectViaNormal(E1,E2,*N);
			E=N->NextEdgeInEdgeStuck(E);
		}

		// detect the faces of the neuron
		Face **FaceList;
		FaceList=N->FacesBelongingToNeuron ();
		int FaceNum=N->FaceCounter;

		//order the faces from new to old
		for (i=0; i<FaceNum-1 ; i++){
			for (int j=i+1; j<FaceNum ; j++){
				if (FaceList[i]->CreationAge > FaceList[j]->CreationAge ){
					Face *TempFace = FaceList[i];
					FaceList[i]=FaceList[j];
					FaceList[j]=TempFace;
				}
			}
		}
		// now go over the faces and check their intersection against other faces - if they intersect delete the older one - do not check faces that share an edge
		for (i=0; i<FaceNum-1 ; i++){
			if ( FaceList[i] ==NULL)
				continue;
			Neuron *N1=FaceList[i]->OtherNeuronInFace(N,false);
			Neuron *N2=FaceList[i]->OtherNeuronInFace(N,true);
			//Edge *Ed1 = Edges.FindEdge(N,N1);
			//Edge *Ed2 = Edges.FindEdge(N,N2);
			for (int j=i+1; j<FaceNum ; j++){
				if ( FaceList[j] ==NULL)
					continue;
				Neuron *N3=FaceList[j]->OtherNeuronInFace(N,false);
				Neuron *N4=FaceList[j]->OtherNeuronInFace(N,true);
				//Edge *Ed3 = Edges.FindEdge(N,N3);
				//Edge *Ed4 = Edges.FindEdge(N,N4);
		
				// if the faces share an edge then no check should be done
				if (N1==N3 || N1==N4 || N2==N3 || N2==N4)
					continue;

				// check that the edges that eminate from N in F1 do not intersect the edge in F2 that N is not a member of (the edge opposite to N)
				bool Prune = CheckTwoProjLinesIntersect(N,N1,N3,N4);
				Prune = Prune || CheckTwoProjLinesIntersect(N,N2,N3,N4);
				

				// check that the edges that eminate from N in F2 do not intersect the edge in F1 that N is not a member of (the edge opposite to N)
				Prune = Prune || CheckTwoProjLinesIntersect(N1,N2,N,N4);
				Prune = Prune || CheckTwoProjLinesIntersect(N1,N2,N,N3);


				// we should also check the case where one face is actually inside the other face (this is done by comparing angles  - it completes the check for intersections done before)
				// first we check the F2 is not in F1
				Prune = Prune || (FaceList[i]->CheckIfProjNeuronIsInProjFace(N3) &&
								  FaceList[i]->CheckIfProjNeuronIsInProjFace(N4)  );

				// first we check that F1 is not in F2
				Prune = Prune || (FaceList[j]->CheckIfProjNeuronIsInProjFace(N1) &&
								  FaceList[j]->CheckIfProjNeuronIsInProjFace(N2)  );


				if (Prune){
					// delete the older face and free it from the list we are working on
					Faces.DelFace(FaceList[j]);
					DelCounter++;
					FaceList[j]=NULL;
				}
			}
		}

		delete [] FaceList;
		N=N->Next;
	} while (N != Neurons.Head);

	return (DelCounter);
}


bool NeuralNet::DeleteSingularNeuronFaces(Face *F, Neuron *N , double CosMaxAngleWithFaceNormalBeforeFilter, bool CheckOnly){
	// this function deletes faces that are connected to N and may create a non-manifold singularities in Neuron N.
	// this is done by going over all the edges that belong to N that do not belong to F and deleting all the faces connected to them (and not to an edge shared by F ) if the edge fulfills the following conditions:
	// 1. Their projection onto F in the normal direction of F is in F. or F is in the projection to F
	// 2. The Cos of the angle between the edge and the normal of F is less then CosMaxAngleWithFaceNormalBeforeFilter (the angle between the face and the edge is small)
	// also for precaution a face will not be deleted if it on the same triangle fan of F eminating from N
	// if CheckOnly is set then no delitions are made only true is returned if face deletions are made
	// the function returns true if faces have been deleted and false otherwise

	// *** this function can be later extended not to pass edges that have a triangle fan connection to F via neuron N

	bool Retval=false;

	if (N->FaceCounter > 3){  // if there are only 3 edges then 2 edges belong to F and any other face that belongs to N must be connected to F

		// mark fans and get thier number
		int FanNumber=N->MarkDisconnectedTriangleFans();
		if (FanNumber<2) //only one triangle fan exists - no need to delete any faces
			return false; 

		Neuron *N1,*N2,*N3;
		N1=F->OtherNeuronInFace(N,false);
		N2=F->OtherNeuronInFace(N,true);

		MyPoint FNormal,E1, E2;
		
		FNormal=N->CalcNormalUsing3Points(N1, N2, &E1, &E2);
		N1->ProjectViaNormal (E1,E2,*N);
		N2->ProjectViaNormal (E1,E2,*N);
		N->Proj.Pos[0]=0.0;
		N->Proj.Pos[1]=0.0;
		N->Proj.Pos[2]=0.0;

		Edge *E = N->EdgeStuckHead;
		while (E!=NULL){
			if (!F->IsFaceOfEdge(E) && E->FaceCounter>0 ){
				N3 = E->OtherNeuronInEdge(N);
				N3->ProjectViaNormal(E1,E2,*N);
				// now check if the projection of N3 is in the angle between N1 and N2
				// notice that N1 angle is zero since it is the E1 direction , also angles are always positive
				// look at function CalcNormalUsing3Points for more information
				if ((N2->Angle < M_PI && N3->Angle <= N2->Angle) ||
					(N2->Angle >= M_PI && N3->Angle >= N2->Angle) ){
					// now check for the angle between the face and the edge 
					// first calc the angle between the face normal and the Edge
					double CosAng = fabs(FNormal* (*N3-*N));
					if (CosAng < CosMaxAngleWithFaceNormalBeforeFilter){
						// this means we want to delete faces connected to E and not to F
						Face *TempFace=E->FacesStuckHead;
						while (TempFace !=NULL){
							// delete only if not connected to F
							if (TempFace->FanNumber != F->FanNumber){
								Face *FaceToDelete=TempFace;
								TempFace=E->NextFaceInFaceStuck(TempFace);
								Retval=true;
								if(!CheckOnly)
									Faces.DelFace(FaceToDelete);
							}
							else 
								TempFace=E->NextFaceInFaceStuck(TempFace);
						}
					}
				}
			}
			E=N->NextEdgeInEdgeStuck(E);
		}
		// at this stage all the edges were checked, now check the faces (edges are already marked)
		// this may be done more efficiently  but going over the faces - but collecting the faces takes more time using the current data structure.
		E = N->EdgeStuckHead;
		while (E!=NULL){
			if (!F->IsFaceOfEdge(E) && E->FaceCounter>0 ){
				Face *TempFace = E->FacesStuckHead;
				while (TempFace!=NULL){ 
					// delete only if not connected to F through a fan
					if (TempFace->FanNumber == F->FanNumber){
						TempFace=E->NextFaceInFaceStuck(TempFace);
						continue;
					}
					Neuron *N3=TempFace->OtherNeuronInFace(N,false);
					Neuron *N4=TempFace->OtherNeuronInFace(N,true);
					if ((N4->Angle - N3->Angle) > M_PI || 
						(N3->Angle - N4->Angle) > M_PI ){  // this means that N1 is in the middle of the projected face sice its angle is 0
						
						double CosAng1 = fabs(FNormal* (*N3-*N));
						double CosAng2 = fabs(FNormal* (*N4-*N));
						if (CosAng1 < CosMaxAngleWithFaceNormalBeforeFilter ||
							CosAng2 < CosMaxAngleWithFaceNormalBeforeFilter   ){
								Face *FaceToDelete=TempFace;
								TempFace=E->NextFaceInFaceStuck(TempFace);
								Retval=true;
								if(!CheckOnly)
									Faces.DelFace(FaceToDelete);
								continue; // this is here so not to advance TempFace again.
						}
					}
					TempFace=E->NextFaceInFaceStuck(TempFace);
				}
			}
			E=N->NextEdgeInEdgeStuck(E);
		}
	}
	return Retval;
}




bool NeuralNet::DeleteNonManifoldFaces(Face *F ,Edge *E , double CosMaxAngleBetweenFacesToBeSimilar, bool CheckOnly){

	// this function deletes faces that will create non manifold surfaces with the new face F.
	// if CosMaxAngleBetweenFacesToBeSimilar is grater then 1 then if the edge E has more then 2 faces connected to it, then all the faces are deleted except from the face which has the highest angle with F.
	// if CosMaxAngleBetweenFacesToBeSimilar is less then 1 then if the edge E has more then 1 faces connected to it, all the faces are deleted except from the face which has the highest angle with F. the cos of that angle is then checked against CosMaxAngleBetweenFacesToBeSimilar is higher, then this face is also deleted
	// E must be an edge of F for the function to work properly (no check is done and it is up to the calling routine to ensure this).
	// if CheckOnly is set then no delitions are made only true is returned if face deletions are made
	// the function returns true if faces have been deleted and false otherwise

	bool Retval=false;

	if (E->FaceCounter > 2 && CosMaxAngleBetweenFacesToBeSimilar>=2.0 ||
		E->FaceCounter > 1 && CosMaxAngleBetweenFacesToBeSimilar<2.0){
		Neuron *N1,*N2,*N3,*N4;
		N1=E->Neuron1;
		N2=E->Neuron2;
		N4 = F->OtherNeuronInFace (N1,N2);
		MyPoint FNormal, F1Normal;
		
		FNormal=N1->CalcNormalUsing3Points(N2,N4);

		// filter criteria can't be that high
		double BestFilterCriterion = 2.0;

		Face *F1 = E->FacesStuckHead;
		Face *BestFace=NULL;

		// find best correspondense face
		for (int i=0; i<E->FaceCounter; i++){
			if (F1==F) {
				F1=E->NextFaceInFaceStuck(F1);
				continue;
			}
			N3= F1->OtherNeuronInFace (N1,N2);
			F1Normal=N1->CalcNormalUsing3Points(N2,N3);
			double Criterion = F1Normal*FNormal;
			if (Criterion < BestFilterCriterion){
					BestFace=F1;
					BestFilterCriterion=Criterion;
			}
			F1=E->NextFaceInFaceStuck(F1);
		}
	

		// delete all other faces except from the best coresponding face
		F1 = E->FacesStuckHead;
		while (E->FaceCounter != 2){
			if (F1 != BestFace && F1 != F){
				Face *TempFace = F1;
				F1=E->NextFaceInFaceStuck(F1);
				Retval=true;
				if (!CheckOnly) 
					Faces.DelFace(TempFace);
			}
			else
				F1=E->NextFaceInFaceStuck(F1);
		}
		// now check if the last face should be deleted
		if (CosMaxAngleBetweenFacesToBeSimilar<1.0 && 
			BestFilterCriterion > CosMaxAngleBetweenFacesToBeSimilar){
				Retval=true;
				if (!CheckOnly) 
					Faces.DelFace(BestFace);
		}


	}
	return Retval;
}


/* this was an old version
void NeuralNet::DeleteNonManifoldFaces(){

	// this function goes over all the edges and 
	// for each edge with velance more than 1
	// it deletes enough faces to make it a manifold edge
	// the criterion for deletion is linked to theorem 3
	// in Amentas paper "One Pass delauney filtering for homeomorphic 3D surface reconstruction"
	// all faces of the edge are deleted except the one that
	// has the best corespondance between the face normal 
	// and the vertex normal at the largest angle vertex of the face

	

	Edge *E=Edges.Head;
	do {
		if (E->FaceCounter > 2){
			// filter criteria can't be that low
			double BestFilterCriteria = -2.0;


			// find best correspondense face
			Face *F = E->FacesStuckHead;
			Face *BestFace=F;
			Face *SecondBestFace=NULL;

			for (int i=0; i<E->FaceCounter; i++){
				F->CalcFilterCriteria();
				if ( SecondBestFace !=NULL && SecondBestFace->FilterCriteria < F->FilterCriteria){
					SecondBestFace=F;
				}
				if (BestFace->FilterCriteria < F->FilterCriteria){
					SecondBestFace=BestFace;
					BestFace=F;
				}
				F=E->NextFaceInFaceStuck(F);
			}

			// delete all other faces except from the best coresponding face
			F = E->FacesStuckHead;
			while (E->FaceCounter != 2){
				if (F != BestFace && F != SecondBestFace){
					Face *TempFace = F;
					F=E->NextFaceInFaceStuck(F);
					Faces.DelFace(TempFace);
				}
				else
					F=E->NextFaceInFaceStuck(F);
			}

		}
		E=E->Next;
	} while (E != Edges.Head);

}


*/





 //this is the new version
void NeuralNet::DeleteNonManifoldFaces(double DeleteFaceCorrespondaceCriterion){

	// this function goes over all the edges and 
	// for each edge with velance more than 1
	// it deletes enough faces to make it a manifold edge
	// the criterion for deletion is that the two faces that are left have the maximum angle between them
	// that means that their normals are in opposit (best fit is when the dot product of the normals is -1)
	// in the best case we will get a flat surface represented by 2 triangle from the two sides of the problematic edge
	// after each edge has two faces attached to it, the dot product of the normals is compared to DeleteFaceCorrespondaceCriterion 
	// and if it is too high then this means that one of the faces should be deleted. we delete the face with the lower connectivity to other faces
	// if both faces have the same connectivity then the face with the lower agreement of the face normal and the vertex normal at its maximal angle vertex
	// the criterion for deletion is linked to theorem 3
	// in Amentas paper "One Pass delauney filtering for homeomorphic 3D surface reconstruction"

	
	Edge *E=Edges.Head;
	do {
		if (E->FaceCounter > 1){
			// filter criteria can't be that high
			double BestFilterCriterion = 2.0;

			Face *F1 = E->FacesStuckHead;
			Face *F2;
			Face *BestFace=NULL;
			Face *SecondBestFace=NULL;
			MyPoint F1Normal, F2Normal;

			Neuron *N1,*N2,*N3,*N4;
			N1=E->Neuron1;
			N2=E->Neuron2;
			// find best correspondense face
			for (int i=0; i<E->FaceCounter; i++){
				N3= F1->OtherNeuronInFace (N1,N2);
				F1Normal=N1->CalcNormalUsing3Points(N2,N3);
				F2=E->NextFaceInFaceStuck (F1);
				for (int j=i+1; j<E->FaceCounter; j++){
					N4= F2->OtherNeuronInFace (N1,N2);
					F2Normal=N1->CalcNormalUsing3Points(N2,N4);
					double Criterion = F1Normal*F2Normal;
					// debug check
					//ASSERT (fabs(Criterion)<=1.0);
					if (Criterion < BestFilterCriterion){
						BestFace=F1;
						SecondBestFace=F2;
						BestFilterCriterion=Criterion;
					}
					F2=E->NextFaceInFaceStuck(F2);
				}
				F1=E->NextFaceInFaceStuck(F1);
			}

			// delete all other faces except from the best coresponding faces
			Face *F = E->FacesStuckHead;
			while (E->FaceCounter != 2){
				if (F != BestFace && F != SecondBestFace){
					Face *TempFace = F;
					F=E->NextFaceInFaceStuck(F);
					Faces.DelFace(TempFace);
				}
				else
					F=E->NextFaceInFaceStuck(F);
			
			}

			// now see if the correspondence between the faces is above the threshold
			if (BestFilterCriterion > DeleteFaceCorrespondaceCriterion){
				// find the face with greater connectivity to other faces
				int Connectivity1 = 
					BestFace->Edge1->FaceCounter + 
					BestFace->Edge2->FaceCounter + 
					BestFace->Edge3->FaceCounter;

				int Connectivity2 = 
					SecondBestFace->Edge1->FaceCounter + 
					SecondBestFace->Edge2->FaceCounter + 
					SecondBestFace->Edge3->FaceCounter;

				if (Connectivity1>Connectivity2)
					Faces.DelFace(SecondBestFace);
				else if (Connectivity2>Connectivity1)
					Faces.DelFace(BestFace);
				else{ // in this case the connectivity is the same -delete the face with the worst correspondance of the normal to the vertex normal at its largest angle vertex
					double Criterion1 = BestFace->CalcFilterCriteria();
					double Criterion2 = SecondBestFace->CalcFilterCriteria();
					if (Criterion1>Criterion2)
						Faces.DelFace(SecondBestFace);
					else 
						Faces.DelFace(BestFace);
				}
				
			}

		}

		E=E->Next;
	} while (E != Edges.Head);


}


//******** rewrite triangulation routine  *********
void NeuralNet::TriangulateNet(PointList &PtLst, double NormalAgreementThreshold){

	//bool EdgesAddedFlag;
	
	bool Finished=false;
	
	static int CurrDist=1;
	static Neuron *N=Neurons.Head; // start with some neuron//
	int NumberOfNeuronsPassed=0;
	static NumberOfViolations=0;

	if (!StartedTriangulation){
//		ClassifySamplePointList(PtLst);
		
	int NeuronsToComplete=0;
		N=Neurons.Head; // start with some neuron//
		if (Neurons.Head!=NULL) do {
			//N->CalcNormal();
			//N->TriDist=0;
			if (N->FaceCounter!=N->EdgeCounter){
				NeuronsToComplete++;
				N->TriDist = 10.0; // a number the confidence can't reach
			}
			else
				N->TriDist = N->CalcTriangulationConfidence(true,2);
	//			N->TriDist = N->CalcTriangulationConfidence(true,1);
			N=N->Next;
		} while (N!=Neurons.Head);
		Neurons.Sort(NeuronsToComplete+1); // sort all Neurons needed complitio (the +1 is only for confidece reasons
		StartedTriangulation=true;
		N=Neurons.Head; // start with best results neuron
		//N->TriDist=1;  // this is the first neuron that should be passed

	}

//	do{
//	Neuron *NextNeuron = N->Next; // remember the next neuron to pass

	if (~N->Normal != 0.0){  // skip if no normal information exists for the neuron
		if (N->FaceCounter != N->EdgeCounter && N->FaceCounter>0 ){ // if the number of faces is the same as the number of edges it means that the Neuron has a cape over it

			// first find the neighbours and put them in an array
			Neuron **NeighbourArray;
			int NeighbourNumber = N->EdgeCounter;

			NeighbourArray = new (Neuron (*[N->EdgeCounter]));

			Edge *E = N->EdgeStuckHead;
			for (int i=0; i<NeighbourNumber ; i++){
				if (E->FaceCounter>0){
					NeighbourArray[i] = E->OtherNeuronInEdge(N);
				}
				else {
					i--;
					NeighbourNumber--;
				}
					E=N->NextEdgeInEdgeStuck(E);
			}

			MyPoint E1,E2;
			N->CalcProjectionPlane ( *NeighbourArray[0] - *N ,E1,E2);
			N->Proj.Pos[0]=0.0;
			N->Proj.Pos[1]=0.0;
			N->Proj.Pos[2]=0.0;
			for (i=0; i<NeighbourNumber;i++){
				NeighbourArray[i]->ProjectViaNormal(E1,E2,*N);
				ASSERT(NeighbourArray[i]->Angle >=0.0);
			}
			

			// order by projection angle*** - simple bubble sort
			for (i=1; i<NeighbourNumber; i++){
				for (int j=i+1; j<NeighbourNumber;j++){
					if (NeighbourArray[i]->Angle > NeighbourArray[j]->Angle){
						// swap positions
						Neuron *Temp=NeighbourArray[i];
						NeighbourArray[i]=NeighbourArray[j];
						NeighbourArray[j]=Temp;
					}
				}
			}


			//prune unneeded neurons after visibility check
			for (i=0; i<NeighbourNumber; i++){
				Neuron *It2 = NeighbourArray[i];
				bool Prune =CheckItersectionForPruning (N,It2,NeighbourNumber,NeighbourArray);
				Prune = Prune || CheckFaceOclusionForPruning (N,It2);

				if (Prune){ // delete the neuron from the array and compact the array
					for (int j=i+1; j<NeighbourNumber ; j++){
						NeighbourArray[j-1]=NeighbourArray[j];
					}
					// fill with nulls for debug
					NeighbourArray[NeighbourNumber-1]=NULL;

					NeighbourNumber--;
					i--;
				}
			}

			
			int Counter=0;
			// triangulate
			for (i=0; i<NeighbourNumber; i++){
				int j = (i+1) % NeighbourNumber; 

				// check for normal agreement and a triangle with less then 180 degrees
				bool AngleCriterion = (j>i) ? NeighbourArray[j]->Angle - NeighbourArray[i]->Angle < M_PI :
											NeighbourArray[i]->Angle - NeighbourArray[j]->Angle > M_PI ;

				/*
				MyPoint FaceNormal = N->CalcNormalUsing3Points(NeighbourArray[i],NeighbourArray[j]);
				bool NormalCriterion =  fabs(FaceNormal * NeighbourArray[i]->Normal) >= NormalAgreementThreshold &&
					 fabs(FaceNormal * NeighbourArray[j]->Normal ) >= NormalAgreementThreshold &&
					 fabs(FaceNormal * N->Normal ) >= NormalAgreementThreshold ;
				*/
				
				
				double NeuronNormalConfidence = N->CalcTriangulationConfidenceFor3Pt(NeighbourArray[i],NeighbourArray[j]);
				bool NormalCriterion =  NeuronNormalConfidence <= NormalAgreementThreshold ;
				bool FoldCriterion = false;

				if (  AngleCriterion && NormalCriterion ){
					//  find the edges
					Edge *E23=Edges.FindEdge(NeighbourArray[i],NeighbourArray[j]);
					Edge *E12=Edges.FindEdge(N,NeighbourArray[i]);
					Edge *E13=Edges.FindEdge(N,NeighbourArray[j]);

					ASSERT ( E23 !=NULL  && E12 !=NULL ||
							 E13 !=NULL  && E12 !=NULL ||
							 E23 !=NULL  && E13 !=NULL );

					Face *F= Faces.IsFace(E12,E13,E23);
					if (F==NULL){ // don't create already exiting faces
						//  don't create non manifold faces in the process
						if (E23 != NULL && E23->FaceCounter>=2 || 
							E12 != NULL && E12->FaceCounter>=2 || 
							E13 != NULL && E13->FaceCounter>=2){
							//ASSERT(0);
							TRACE (" number of non manifold violations is now: %d" , NumberOfViolations++);
						}
						else{
							// I believe that the first two checks are not needed since the projection takes care of this part
							FoldCriterion = FoldCriterion || E12->CheckFoldOverEdge(NeighbourArray[j]);
//							ASSERT(!FoldCriterion && E12->FaceCounter <2);  // this should not normally happen since projection takes care of this
							FoldCriterion = FoldCriterion || E13->CheckFoldOverEdge(NeighbourArray[i]);
//							ASSERT(!FoldCriterion && E13->FaceCounter <2);  // this should not normally happen since projection takes care of this
							FoldCriterion = FoldCriterion || E23->CheckFoldOverEdge(N);

							if (!FoldCriterion){
								Edge *OldE23=E23;
								Edge *OldE12=E12;
								Edge *OldE13=E13;

								E23=Edges.CreateOrRefreshEdge(NeighbourArray[i],NeighbourArray[j]);
								E12=Edges.CreateOrRefreshEdge(N,NeighbourArray[i]);
								E13=Edges.CreateOrRefreshEdge(N,NeighbourArray[j]);


								F=Faces.CreateOrRefreshFace(E12,E13,E23);
								//debug 
								F->Tag = Counter++;

								// now check if the Face does not cause delitions of other faces, if it does - delete the face
								bool DeleteFace=false;
								DeleteFace = DeleteFace || DeleteNonManifoldFaces(F,E12,sqrt(3.0)/2.0,true);
								DeleteFace = DeleteFace ||DeleteNonManifoldFaces(F,E13,sqrt(3.0)/2.0,true);
								DeleteFace = DeleteFace ||DeleteNonManifoldFaces(F,E23,sqrt(3.0)/2.0,true);
								Neuron *N1,*N2,*N3;
								F->GetFaceNeurons(&N1,&N2,&N3);
								DeleteFace = DeleteFace ||DeleteSingularNeuronFaces(F,N1,0.5,true);
								DeleteFace = DeleteFace ||DeleteSingularNeuronFaces(F,N2,0.5,true);
								DeleteFace = DeleteFace ||DeleteSingularNeuronFaces(F,N3,0.5,true);
								
								// now check if the oerientation of the face agrees with all patches it is connected to - otherwise delete it
								if (!DeleteFace){
									Face *F12 = E12->OtherFaceOfEdge(F);
									Face *F13 = E13->OtherFaceOfEdge(F);
									Face *F23 = E23->OtherFaceOfEdge(F);
									
									if (F12==NULL){ // in this case consider only F13 and F23
										if (F13==NULL){	// in this case just make consistant with one edge
											if (F23==NULL){ // in this case we do nothing
												TRACE ("face created with no neigbouring faces\n");
											}
											// otherwise the face is valid and may only needs flipping
											else if (E23->CheckFaceDirectionConsistency() == 1)
												F->FlipDirection = ! F->FlipDirection;
											//F->Marked = true; // do not repeat this again	
										}
										else{ // here we know that F12 is connected we need to check if F23 is also consistant
											// first we make F consistant with E13
											if (E13->CheckFaceDirectionConsistency() == 1)
												F->FlipDirection = ! F->FlipDirection;
											//F->Marked = true; // do not repeat this again		

											if (F23!=NULL){ // if F23 exists we need to establish consistency
												// check if F23 is on the same patch as F
												if (F23->PatchNumber == F->PatchNumber){
													// in that case just check for consistency and if inconsistant with at least one face- do not add to this patch
													if ( E23->CheckFaceDirectionConsistency() == 1 )
														DeleteFace=true;
												}
												else{ // in case F23 and F are on different patches merge the patches together
													MergeSurfacePatches(E23);
												}
											}
										}
									}
									else {  //we know that F12 exists
										// make compatible and consistant with one face
										F->PatchNumber = F12->PatchNumber;
										if (E12->CheckFaceDirectionConsistency() == 1)
											F->FlipDirection = ! F->FlipDirection;
										//F->Marked = true; // do not repeat this again
	                                    
										//check that F13 exists
										if (F13==NULL){ 
											// if F13 does not exist, check if F23 exists
											if (F23!=NULL){
												if (F23->PatchNumber == F->PatchNumber){
													// in that case just check for consistency and if inconsistant with at least one face- do not add to this patch
													if ( E23->CheckFaceDirectionConsistency() == 1 )
														DeleteFace=true;
												}
												else{ // in case F23 and F are on different patches merge the patches together
													MergeSurfacePatches(E23);
												}
											}
										}
										else if (F23==NULL){ // check if F23 exists
											// we know that E13 exist otherwise we have not reached here so just establish consisency
											if (F13->PatchNumber == F->PatchNumber){
												// in that case just check for consistency and if inconsistant with at least one face- do not add to this patch
												if ( E13->CheckFaceDirectionConsistency() == 1 )
													DeleteFace=true;
											}
											else{ // in case F13 and F are on different patches merge the patches together
												MergeSurfacePatches(E13);
											}
										}
										// otherwise we know that all faces exist
										// check if the other faces are on the same patch as this one
										else if (F13->PatchNumber == F->PatchNumber && 
											     F23->PatchNumber == F->PatchNumber){
											// in that case just check for consistency and if inconsistant with at least one face- do not add to this patch
											if (E13->CheckFaceDirectionConsistency() == 1  ||
												E23->CheckFaceDirectionConsistency() == 1 )
												DeleteFace=true;
										}
										// check if connecting to another patch if both edges connect to the same patch
										else if (F13->PatchNumber == F23->PatchNumber){
											// in that case just check for consistency if not consistant - do not add to this patch
											if (E13->CheckFaceDirectionConsistency() !=  E23->CheckFaceDirectionConsistency()   )
												DeleteFace=true;
											else{ // this means we can merge the patches to F (since F13 and F23 are on the same patch - just remerge through E13 and E23 will be remerged as well)
												MergeSurfacePatches(E13);
											}
										}
										// now check if F13 is on the same patch as F
										else if (F13->PatchNumber == F->PatchNumber){
											// in that case just check for consistency if not consistant - do not add to this patch
											if (E13->CheckFaceDirectionConsistency() == 1 )
												DeleteFace=true;
											else {  // F13 is on the same patch as F, so merge only F23  (we know it is not on the same patch as F13)
												MergeSurfacePatches(E23);	
											}
										}
										else if (F23->PatchNumber == F->PatchNumber){
											// in that case just check for consistency if not consistant - do not add to this patch
											if (E23->CheckFaceDirectionConsistency() == 1 )
												DeleteFace=true;
											else {  // F23 is on the same patch as F, so merge only F13  (we know it is not on the same patch as F23)
												MergeSurfacePatches(E13);	
											}
										}
										// this is the last case were we know that F and F13 and F23 are not on the same patch
										else if (F13->PatchNumber != F->PatchNumber && F23->PatchNumber != F->PatchNumber){ 
											// in this case just merge the 2 patches to F	
												MergeSurfacePatches(E13);	
												MergeSurfacePatches(E23);	
										}
										else{
											TRACE ("Problem in patch merging descision mechanism - check out if statements");
											ASSERT(0);
										}
									}
								}

								if (DeleteFace){
									Faces.DelFace(F);
									if (OldE23==NULL)
										Edges.DelEdge(E23);
									if (OldE12==NULL)
										Edges.DelEdge(E12);
									if (OldE13==NULL)
										Edges.DelEdge(E13);
								}
							
							}
						}
					}
				}
			}
			// free the memory taken
			delete [] NeighbourArray;
		} // end if if (FaceCounter=EdgeCounter)
	}
//	else 
//		TRACE (" normal calculation problem - probably number of classified points less then 3 - point skipped\n");


	N->TriDist = - N->TriDist;
//	N = NextNeuron; // set the next neuron to be passed

	// find next neuron to be passed
	double MinPositiveDist = MAX_DOUBLE;
	Neuron *NextNeuron = NULL;
	Neuron *It5=Neurons.Head;
	do{
		if (It5->TriDist >=0.0 && It5->TriDist < MinPositiveDist){
			// the point must have a boundary edge
			NextNeuron = It5;
			MinPositiveDist = It5->TriDist;
		}
		It5=It5->Next;
	}while (It5 != Neurons.Head);

	N=NextNeuron;
	return;


}





/* older funcion - still may be in use 
void NeuralNet::TriangulateNet(PointList &PtLst, double NormalAgreementThreshold){

	//bool EdgesAddedFlag;
	
	bool Finished=false;
	
	static int CurrDist=1;
	static Neuron *N=Neurons.Head; // start with some neuron//
	int NumberOfNeuronsPassed=0;
	static NumberOfViolations=0;

	if (!StartedTriangulation){
//		ClassifySamplePointList(PtLst);
		
		N=Neurons.Head; // start with some neuron//
		if (Neurons.Head!=NULL) do {
			//N->CalcNormal();
			//N->TriDist=0;
			N->TriDist = N->CalcTriangulationConfidence(true,2);
//			N->TriDist = N->CalcTriangulationConfidence(true,1);
			N=N->Next;
		} while (N!=Neurons.Head);
		Neurons.Sort(0); // sort all neurons
		StartedTriangulation=true;
		N=Neurons.Head; // start with best results neuron
		//N->TriDist=1;  // this is the first neuron that should be passed

	}

//	do{
//	Neuron *NextNeuron = N->Next; // remember the next neuron to pass

	if (~N->Normal != 0.0){  // skip if no normal information exists for the neuron
		if (N->FaceCounter != N->EdgeCounter ){ // if the number of faces is the same as the number of edges it means that the Neuron has a cape over it

			// first find the neighbours and put them in an array
			Neuron **NeighbourArray;
			int NeighbourNumber = N->EdgeCounter;

			NeighbourArray = new (Neuron (*[N->EdgeCounter]));

			Edge *E = N->EdgeStuckHead;
			for (int i=0; i<NeighbourNumber ; i++){
				NeighbourArray[i] = E->OtherNeuronInEdge(N);
				E=N->NextEdgeInEdgeStuck(E);
			}

			MyPoint E1,E2;
			N->CalcProjectionPlane ( *NeighbourArray[0] - *N ,E1,E2);
			N->Proj.Pos[0]=0.0;
			N->Proj.Pos[1]=0.0;
			N->Proj.Pos[2]=0.0;
			for (i=0; i<NeighbourNumber;i++){
				NeighbourArray[i]->ProjectViaNormal(E1,E2,*N);
				ASSERT(NeighbourArray[i]->Angle >=0.0);
			}
			

			// order by projection angle*** - simple bubble sort
			for (i=1; i<NeighbourNumber; i++){
				for (int j=i+1; j<NeighbourNumber;j++){
					if (NeighbourArray[i]->Angle > NeighbourArray[j]->Angle){
						// swap positions
						Neuron *Temp=NeighbourArray[i];
						NeighbourArray[i]=NeighbourArray[j];
						NeighbourArray[j]=Temp;
					}
				}
			}


			//prune unneeded neurons after visibility check
			for (i=0; i<NeighbourNumber; i++){
				Neuron *It2 = NeighbourArray[i];
				bool Prune =CheckItersectionForPruning (N,It2,NeighbourNumber,NeighbourArray);
				Prune = Prune || CheckFaceOclusionForPruning (N,It2);

				if (Prune){ // delete the neuron from the array and compact the array
					for (int j=i+1; j<NeighbourNumber ; j++){
						NeighbourArray[j-1]=NeighbourArray[j];
					}
					// fill with nulls for debug
					NeighbourArray[NeighbourNumber-1]=NULL;

					NeighbourNumber--;
					i--;
				}
			}

			
			int Counter=0;
			// triangulate
			for (i=0; i<NeighbourNumber; i++){
				int j = (i+1) % NeighbourNumber; 

				// check for normal agreement and a triangle with less then 180 degrees
				bool AngleCriterion = (j>i) ? NeighbourArray[j]->Angle - NeighbourArray[i]->Angle < M_PI :
											NeighbourArray[i]->Angle - NeighbourArray[j]->Angle > M_PI ;

				/*
				MyPoint FaceNormal = N->CalcNormalUsing3Points(NeighbourArray[i],NeighbourArray[j]);
				bool NormalCriterion =  fabs(FaceNormal * NeighbourArray[i]->Normal) >= NormalAgreementThreshold &&
					 fabs(FaceNormal * NeighbourArray[j]->Normal ) >= NormalAgreementThreshold &&
					 fabs(FaceNormal * N->Normal ) >= NormalAgreementThreshold ;
				* /
				
				
				double NeuronNormalConfidence = N->CalcTriangulationConfidenceFor3Pt(NeighbourArray[i],NeighbourArray[j]);
				bool NormalCriterion =  NeuronNormalConfidence <= NormalAgreementThreshold ;
				bool FoldCriterion = false;

				if (  AngleCriterion && NormalCriterion ){
					//  find the edges
					Edge *E23=Edges.FindEdge(NeighbourArray[i],NeighbourArray[j]);
					Edge *E12=Edges.FindEdge(N,NeighbourArray[i]);
					Edge *E13=Edges.FindEdge(N,NeighbourArray[j]);

					ASSERT ( E23 !=NULL  && E12 !=NULL ||
							 E13 !=NULL  && E12 !=NULL ||
							 E23 !=NULL  && E13 !=NULL );

					Face *F= Faces.IsFace(E12,E13,E23);
					if (F==NULL){ // don't create already exiting faces
						//  don't create non manifold faces in the process
						if (E23 != NULL && E23->FaceCounter>=2 || 
							E12 != NULL && E12->FaceCounter>=2 || 
							E13 != NULL && E13->FaceCounter>=2){
							//ASSERT(0);
							TRACE (" number of non manifold violations is now: %d" , NumberOfViolations++);
						}
						else{
							// I believe that the first two checks are not needed since the projection takes care of this part
							FoldCriterion = FoldCriterion || E12->CheckFoldOverEdge(NeighbourArray[j]);
//							ASSERT(!FoldCriterion && E12->FaceCounter <2);  // this should not normally happen since projection takes care of this
							FoldCriterion = FoldCriterion || E13->CheckFoldOverEdge(NeighbourArray[i]);
//							ASSERT(!FoldCriterion && E13->FaceCounter <2);  // this should not normally happen since projection takes care of this
							FoldCriterion = FoldCriterion || E23->CheckFoldOverEdge(N);

							if (!FoldCriterion){
								E23=Edges.CreateOrRefreshEdge(NeighbourArray[i],NeighbourArray[j]);
								E12=Edges.CreateOrRefreshEdge(N,NeighbourArray[i]);
								E13=Edges.CreateOrRefreshEdge(N,NeighbourArray[j]);


								F=Faces.CreateOrRefreshFace(E12,E13,E23);
								//debug 
								F->Tag = Counter++;
							}
						}
					}
				}
			}
			// free the memory taken
			delete [] NeighbourArray;
		} // end if if (FaceCounter=EdgeCounter)
	}
//	else 
//		TRACE (" normal calculation problem - probably number of classified points less then 3 - point skipped\n");


	N->TriDist = - N->TriDist;
//	N = NextNeuron; // set the next neuron to be passed

	// find next neuron to be passed
	double MinPositiveDist = MAX_DOUBLE;
	Neuron *NextNeuron = NULL;
	Neuron *It5=Neurons.Head;
	do{
		if (It5->TriDist >=0.0 && It5->TriDist < MinPositiveDist){
			// the point must have a boundary edge
			NextNeuron = It5;
			MinPositiveDist = It5->TriDist;
		}
		It5=It5->Next;
	}while (It5 != Neurons.Head);

	N=NextNeuron;
	return;


}
*/

bool NeuralNet::LoadNetFromFile(LPCTSTR FileName)
{
	// this function loads a Net from a WRL file or a JAC file

	char FilePrefixJAC[5]=".JAC";
	char FilePrefixWRL[5]=".WRL";
	char FilePrefix[5];
	strcpy(FilePrefix , &FileName [strlen(FileName) -4] );

	// check for file type
	if (!strnicmp(FilePrefix,FilePrefixJAC,4)){
		ifstream ifs;
		ifs.open(FileName);
		int PatchNo, NumberOfPatches;
		ifs >> NumberOfPatches;
		if (ifs.fail()){
			ifs.close();
			return false;
		}

		// we assume that there are no gaps between ID numberings
		struct PatchInfo{
			int PatchId;
			int NumberOfIsoLines;
			int *IsoLineNumbers;
			int MaxNumberOfVrticesInIsoLine;
		} *Patches;


		struct IsoLineInfo{
			int PatchId;
			int BranchId;
			int IsoLineId;
			int PrevIsoLine;
			int NextIsoLine;
			int IsoLineVertexNumber;
			int FirstVertexIndexDisplacement;
			MyPoint *Vertices;
		} *IsoLines;


		struct BranchInfo{
			int BranchId;
			int BranchIsoLine;
			int NextIsoLinesNumber;
			int PrevIsoLinesNumber;
			int *NextIsoLines;
			int *PrevIsoLines;
		} *Branches;


		Patches = new struct PatchInfo[NumberOfPatches];


		for (PatchNo=0; PatchNo<NumberOfPatches;PatchNo++){
			int PatchId, NumberOfIsoLines;
			ifs >> PatchId >> NumberOfIsoLines;
			if (ifs.fail()){
				ifs.close();
				return false;
			}

			Patches[PatchNo].PatchId=PatchId;
			Patches[PatchNo].NumberOfIsoLines = NumberOfIsoLines;
			Patches[PatchNo].IsoLineNumbers= new int[NumberOfIsoLines];

			for (int i=0;i<NumberOfIsoLines;i++){
				int IsoLineId;
				char SeparatorChar;
				ifs >> IsoLineId >> SeparatorChar;
				if (ifs.fail()){
					ifs.close();
					return false;
				}
				
				Patches[PatchNo].IsoLineNumbers[i] = IsoLineId;

			}

			
		}

		// add code for reading the branch node connection information

		int BranchNo, NumberOfBranches;
		ifs >> NumberOfBranches;

		if (ifs.fail()){
			ifs.close();
			return false;
		}

		Branches = new struct BranchInfo[NumberOfBranches];
		

		for (BranchNo = 0; BranchNo < NumberOfBranches ; BranchNo++ ){
			int BranchIsoLine, NextIsoLinesNumber, PrevIsoLinesNumber;
			ifs >> BranchIsoLine >> NextIsoLinesNumber >> PrevIsoLinesNumber;

			if (ifs.fail()){
				ifs.close();
				return false;
			}

			Branches[BranchNo].BranchId = BranchNo;
			Branches[BranchNo].BranchIsoLine = BranchIsoLine;
			Branches[BranchNo].NextIsoLinesNumber=NextIsoLinesNumber;
			Branches[BranchNo].PrevIsoLinesNumber=PrevIsoLinesNumber;
			Branches[BranchNo].NextIsoLines= new int [NextIsoLinesNumber];
			Branches[BranchNo].PrevIsoLines= new int [PrevIsoLinesNumber];
			//read next iso-lines
			for (int i=0; i<NextIsoLinesNumber;i++){

				int IsoLinesNo;
				ifs >> IsoLinesNo;
				Branches[BranchNo].NextIsoLines[i] = IsoLinesNo;
				if (ifs.fail()){
					ifs.close();
					return false;
				}
				// add handling code in here
			}

			//read prev iso-lines
			for (i=0; i<PrevIsoLinesNumber;i++){
			
				int IsoLinesNo;
				ifs >> IsoLinesNo;
				Branches[BranchNo].PrevIsoLines[i] = IsoLinesNo;
				if (ifs.fail()){
					ifs.close();
					return false;
				}
				// add handling code in here
			}

		}

		//read isolines
		int IsoLineNumber;
		ifs >> IsoLineNumber;

		if (ifs.fail()){
			ifs.close();
			return false;
		}

			
		IsoLines = new struct IsoLineInfo[IsoLineNumber];

		for (int i=0; i<IsoLineNumber; i++){
			int IsoLineId,IsoLineVertexNumber;
			ifs >> IsoLineId >> IsoLineVertexNumber;
			if (ifs.fail()){
				ifs.close();
				return false;
			}

			bool PatchFound = false;
			// search for that iso line in the patch structure
			for (int PatchNo=0;PatchNo<NumberOfPatches;PatchNo++){
				for (int k=0;k<Patches[PatchNo].NumberOfIsoLines; k++){
					if (Patches[PatchNo].IsoLineNumbers[k] == IsoLineId){
						IsoLines[i].PatchId = Patches[PatchNo].PatchId;
						IsoLines[i].BranchId = -1;
						IsoLines[i].IsoLineId= IsoLineId;
						if (k==0)
							IsoLines[i].PrevIsoLine = -1; // no such isoline
						else
							IsoLines[i].PrevIsoLine = Patches[PatchNo].IsoLineNumbers[k-1];
						if (k == Patches[PatchNo].NumberOfIsoLines-1)
							IsoLines[i].NextIsoLine = -1; // no such isoline
						else
							IsoLines[i].NextIsoLine = Patches[PatchNo].IsoLineNumbers[k+1];

						IsoLines[i].IsoLineVertexNumber=IsoLineVertexNumber;
						IsoLines[i].Vertices = new MyPoint[IsoLineVertexNumber];

						PatchFound=true;
						break;
					}
				}
				if (PatchFound) break;
			}
			// this means that this is a branching iso curve
			if (!PatchFound){

				bool BranchFound = false;
				// search for that iso line in the branch structure
				for (int BranchNo=0;BranchNo<NumberOfBranches;BranchNo++){
					if (Branches[BranchNo].BranchIsoLine == IsoLineId){
						IsoLines[i].PatchId = -1;
						IsoLines[i].BranchId = BranchNo;
						IsoLines[i].IsoLineId = IsoLineId;

						IsoLines[i].PrevIsoLine = -1; // no such isoline
						IsoLines[i].NextIsoLine = -1; // no such isoline

						IsoLines[i].IsoLineVertexNumber=IsoLineVertexNumber;
						IsoLines[i].Vertices = new MyPoint[IsoLineVertexNumber];

						BranchFound=true;
						break;
					}
				}

				if (!BranchFound) {
					ifs.close();
					return false;
				}

			}

			for (int j=0; j<IsoLineVertexNumber;j++){
				MyPoint Pt;
				ifs >> Pt.Pos[0] >> Pt.Pos[1] >> Pt.Pos[2];
					
				if (ifs.fail()){
					ifs.close();
					return false;
				}

				// now add specialzed code on what to do with the points
				IsoLines[i].Vertices[j]=Pt;															
			}
		}
		
		ifs.close();


		// constants
		int NumberOfPointsPerLoop=5;
		int StartPatch=1;
		int EndPatch=NumberOfPatches-1;
		int StartBranch=0;
		int EndBranch=NumberOfBranches;



		// at this point the file has been loaded, now reform the data into a neural network.

		// first find the roation index to reflect the two closest points on neigbouring arrays and find the longest isoline (vertex counting)
		// for each patch the first point in the first iso curve is the anchor




		for (PatchNo=StartPatch; PatchNo<EndPatch; PatchNo++){
			if (Patches[PatchNo].NumberOfIsoLines==0) continue;
			MyPoint LastPt = IsoLines[Patches[PatchNo].IsoLineNumbers[0]].Vertices[0];
			int LastVertexIndex=0;
			Patches[PatchNo].MaxNumberOfVrticesInIsoLine=IsoLines[Patches[PatchNo].IsoLineNumbers[0]].IsoLineVertexNumber;
			
			IsoLines[Patches[PatchNo].IsoLineNumbers[0]].FirstVertexIndexDisplacement=0;
			
			for (int IsoLine = 1; IsoLine < Patches[PatchNo].NumberOfIsoLines; IsoLine++){
				if (Patches[PatchNo].MaxNumberOfVrticesInIsoLine < IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine]].IsoLineVertexNumber){
					Patches[PatchNo].MaxNumberOfVrticesInIsoLine = IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine]].IsoLineVertexNumber;
				}

				double MinDistance=MAX_DOUBLE;
				int CurrentMinIndex=-1;
				for (int Vertex=0; Vertex < IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine]].IsoLineVertexNumber; Vertex++){
					
					double Distance = ~(IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine]].Vertices[Vertex] -
						                IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine-1]].Vertices[LastVertexIndex]) ;
					if (Distance < MinDistance){
						CurrentMinIndex=Vertex;
						MinDistance=Distance;
					}
				}
				IsoLines[Patches[PatchNo].IsoLineNumbers[IsoLine]].FirstVertexIndexDisplacement=CurrentMinIndex;
				LastVertexIndex = CurrentMinIndex;
			}
		}

		// now reinterpolate points on the isolines to get the neurons

		
		Neuron **NewNeuronArray;

		// these macors are needed only in this stage of the program and not elsewhere
		#define RECTNNNEW(i,j) NewNeuronArray[(i)*(NumberOfPointsPerLoop)+(j)]

		NewNeuronArray = new (Neuron (*[IsoLineNumber*NumberOfPointsPerLoop]));

		for (PatchNo=StartPatch; PatchNo<EndPatch; PatchNo++){

			
			for (int i = 0; i < Patches[PatchNo].NumberOfIsoLines; i++){
				int IsoLineNo = Patches[PatchNo].IsoLineNumbers[i];
				double U = 0.0;
				int IndexDisplacement=IsoLines[IsoLineNo].FirstVertexIndexDisplacement;
				int NumberOfVerticesInIsoLine = IsoLines[IsoLineNo].IsoLineVertexNumber;
				double DeltaU = (double) NumberOfVerticesInIsoLine / NumberOfPointsPerLoop;
				MyPoint *Vertices =IsoLines[IsoLineNo].Vertices;
				for(int j=0;j<NumberOfPointsPerLoop;j++){
					int FirstIndex = (int) floor(U);
					double InterpolationValue = U - FirstIndex;
					FirstIndex = (FirstIndex + IndexDisplacement) % NumberOfVerticesInIsoLine;
					int SecondIndex = (FirstIndex+1) % NumberOfVerticesInIsoLine;
					MyPoint InterpolationPoint = Vertices [FirstIndex]*(1.0-InterpolationValue) + Vertices [SecondIndex] * InterpolationValue;


					Neuron *N=AddNeuron(InterpolationPoint);
					N->PatchClassification = PatchNo;
					N->NeuronIdU=i;
					N->NeuronIdV=j;
					RECTNNNEW(IsoLineNo,j)=N;
					U=U+DeltaU;
					if (j>0)
						// connect the neurons with edges
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,j-1), EDGE_V_DIR);
					if (i>0)
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLines[IsoLineNo].PrevIsoLine,j), EDGE_U_DIR);
					// connect last point to close the loop
					if (j==NumberOfPointsPerLoop-1)
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,0), EDGE_V_DIR);
				}

				// we don't need the vertices anymore
//				delete [] IsoLines[IsoLineNo].Vertices;
			}
 
		}
		//for each branch
		// create vertices, for each next iso line 
		// find nearest vertex to the first vertex and connect it and all others to the branching isoline
		// do the same for prev isolines

/*		old version of connecting branches

		for (BranchNo=StartBranch;BranchNo<EndBranch; BranchNo++){
			int IsoLineNo = Branches[BranchNo].BranchIsoLine;
			double U = 0.0;
			int NumberOfVerticesInIsoLine = IsoLines[IsoLineNo].IsoLineVertexNumber;
			double DeltaU = (double) NumberOfVerticesInIsoLine / NumberOfPointsPerLoop;
			MyPoint *Vertices = IsoLines[IsoLineNo].Vertices;
			for(int j=0; j<NumberOfPointsPerLoop;j++){
				int FirstIndex = floor(U);
				double InterpolationValue = U - FirstIndex;
				FirstIndex = FirstIndex % NumberOfVerticesInIsoLine;
				int SecondIndex = (FirstIndex+1) % NumberOfVerticesInIsoLine;
				MyPoint InterpolationPoint = Vertices [FirstIndex]*(1.0-InterpolationValue) + Vertices [SecondIndex] * InterpolationValue;

				Neuron *N=AddNeuron(InterpolationPoint);
				N->PatchClassification = -BranchNo; // marks a branch
					N->NeuronIdU=0;
					N->NeuronIdV=j;
					RECTNNNEW(IsoLineNo,j)=N;
					U=U+DeltaU;
					if (j>0)
					// connect the neurons with edges
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,j-1), EDGE_V_DIR);
					// connect last point to close the loop
					if (j==NumberOfPointsPerLoop-1)
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,0), EDGE_V_DIR);

			}
		}

		for (BranchNo=StartBranch;BranchNo<EndBranch; BranchNo++){
			int IsoLineNo = Branches[BranchNo].BranchIsoLine;
			// now connect to next iso lines
			for (int m=0;m<Branches[BranchNo].NextIsoLinesNumber;m++){
				int NextIsoLineNo=Branches[BranchNo].NextIsoLines[m];
				if ((IsoLines[NextIsoLineNo].PatchId < StartPatch ||
					 IsoLines[NextIsoLineNo].PatchId >= EndPatch ) &&
					 IsoLines[NextIsoLineNo].PatchId >=0)
					continue;
				// find closest vertex to first in the patchisoline
				double MinDistance=MAX_DOUBLE;
				int MinIndex=-1;
				for (int j=0; j < NumberOfPointsPerLoop; j++){
					
					double Distance = ~(RECTNNNEW(IsoLineNo,0) -
										RECTNNNEW(NextIsoLineNo,j)) ;
					if (Distance < MinDistance){
						MinIndex=j;
						MinDistance=Distance;
					}
				}
						
				for (j=0; j < NumberOfPointsPerLoop; j++){
					Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(NextIsoLineNo,(j+MinIndex)%NumberOfPointsPerLoop), EDGE_U_DIR);	
				}
			}
				
			// now connect to prev iso lines
			for (m=0;m<Branches[BranchNo].PrevIsoLinesNumber;m++){
				int PrevIsoLineNo=Branches[BranchNo].PrevIsoLines[m];
				if ((IsoLines[PrevIsoLineNo].PatchId < StartPatch ||
					 IsoLines[PrevIsoLineNo].PatchId >= EndPatch ) &&
					 IsoLines[PrevIsoLineNo].PatchId >=0)
					continue;
				// find closest vertex to first in the patchisoline
				double MinDistance=MAX_DOUBLE;
				int MinIndex=-1;
				for (int j=0; j < NumberOfPointsPerLoop; j++){
					
					double Distance = ~(RECTNNNEW(IsoLineNo,0) -
										RECTNNNEW(PrevIsoLineNo,j)) ;
					if (Distance < MinDistance){
						MinIndex=j;
						MinDistance=Distance;
					}
				}
						
				for (j=0; j < NumberOfPointsPerLoop; j++){
					Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(PrevIsoLineNo,(j+MinIndex)%NumberOfPointsPerLoop), EDGE_U_DIR);	
				}
			}

			delete [] IsoLines[IsoLineNo].Vertices;
 
			delete [] Branches[BranchNo].NextIsoLines;
			delete [] Branches[BranchNo].PrevIsoLines;
		}

*/


//		new version of connecting branches

		for (BranchNo=StartBranch;BranchNo<EndBranch; BranchNo++){
			int IsoLineNo = Branches[BranchNo].BranchIsoLine;
			double U = 0.0;
			int NumberOfVerticesInIsoLine = IsoLines[IsoLineNo].IsoLineVertexNumber;
			double DeltaU = (double) NumberOfVerticesInIsoLine / NumberOfPointsPerLoop;
			MyPoint *Vertices = IsoLines[IsoLineNo].Vertices;
			for(int j=0; j<NumberOfPointsPerLoop;j++){
				int FirstIndex = (int) floor(U);
				double InterpolationValue = U - FirstIndex;
				FirstIndex = FirstIndex % NumberOfVerticesInIsoLine;
				int SecondIndex = (FirstIndex+1) % NumberOfVerticesInIsoLine;
				MyPoint InterpolationPoint = Vertices [FirstIndex]*(1.0-InterpolationValue) + Vertices [SecondIndex] * InterpolationValue;

				Neuron *N=AddNeuron(InterpolationPoint);
				N->PatchClassification = -BranchNo; // marks a branch
					N->NeuronIdU=0;
					N->NeuronIdV=j;
					// mark neuron for deletion unless connected later
					RECTNNNEW(IsoLineNo,j)=N;
					U=U+DeltaU;
					if (j>0)
					// connect the neurons with edges
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,j-1), EDGE_V_DIR);
					// connect last point to close the loop
					if (j==NumberOfPointsPerLoop-1)
						Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(IsoLineNo,0), EDGE_V_DIR);

			}
		}

		for (BranchNo=StartBranch;BranchNo<EndBranch; BranchNo++){

			int IsoLineNo,NextIsoLineNo;
			// now loop through prev iso lines
			for (int p=0;p<Branches[BranchNo].PrevIsoLinesNumber + Branches[BranchNo].NextIsoLinesNumber;p++){
				if ( p<Branches[BranchNo].PrevIsoLinesNumber )
					IsoLineNo = Branches[BranchNo].PrevIsoLines[p];
				else
					IsoLineNo = Branches[BranchNo].NextIsoLines[p - Branches[BranchNo].PrevIsoLinesNumber ];
					
					if ((IsoLines[IsoLineNo].PatchId < StartPatch ||
						 IsoLines[IsoLineNo].PatchId >= EndPatch ) &&
						 IsoLines[IsoLineNo].PatchId >=0)
						 continue;	


				// now connect to next iso lines
				for (int m=p;m<Branches[BranchNo].PrevIsoLinesNumber + Branches[BranchNo].NextIsoLinesNumber;m++){
					if ( m<Branches[BranchNo].PrevIsoLinesNumber )
						NextIsoLineNo = Branches[BranchNo].PrevIsoLines[m];
					else
						NextIsoLineNo = Branches[BranchNo].NextIsoLines[m - Branches[BranchNo].PrevIsoLinesNumber ];

					if ((IsoLines[NextIsoLineNo].PatchId < StartPatch ||
						 IsoLines[NextIsoLineNo].PatchId >= EndPatch ) &&
						 IsoLines[NextIsoLineNo].PatchId >=0)
						continue;
					
					// find closest vertex to first in the patch isoline
					double MinDistance=MAX_DOUBLE;
					int MinIndex=-1;
					bool ClockWiseMinimum;

					for (int DispIndex=0; DispIndex<NumberOfPointsPerLoop; DispIndex++){
					// now check if the sum distance is smaller in clockwise or counterclockwise counting
					// clockwise
						double CWDistanceSum=0.0;
						for (int j=0; j < NumberOfPointsPerLoop; j++){
							MyPoint InterpPt = *RECTNNNEW(IsoLineNo,j) - *RECTNNNEW(NextIsoLineNo,(j+DispIndex)%NumberOfPointsPerLoop);
							CWDistanceSum += ~InterpPt;	
						}
						if (MinDistance > CWDistanceSum){
							ClockWiseMinimum=true;
							MinIndex=DispIndex;
							MinDistance=CWDistanceSum;
						}
						// counter clockwise
						double CCWDistanceSum=0.0;
						for (j=0; j < NumberOfPointsPerLoop; j++){
							MyPoint InterpPt= *RECTNNNEW(IsoLineNo,j) - *RECTNNNEW(NextIsoLineNo,(DispIndex-j+NumberOfPointsPerLoop)%NumberOfPointsPerLoop);
							CCWDistanceSum += ~InterpPt;	
						}
						if (MinDistance > CCWDistanceSum){
							ClockWiseMinimum=false;
							MinIndex=DispIndex;
							MinDistance=CCWDistanceSum;
						}
					}

					// now connect CW or CCW (smallest distance criterion)
					if (ClockWiseMinimum)
						for (int j=0; j < NumberOfPointsPerLoop; j++){
							Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(NextIsoLineNo,(j+MinIndex)%NumberOfPointsPerLoop), EDGE_U_DIR);	
						}
					else
						for (int j=0; j < NumberOfPointsPerLoop; j++){
							Edges.CreateOrRefreshEdge(RECTNNNEW(IsoLineNo,j) , RECTNNNEW(NextIsoLineNo,(MinIndex-j+NumberOfPointsPerLoop)%NumberOfPointsPerLoop), EDGE_U_DIR);	
						}
				}

			}

	//		delete [] IsoLines[IsoLineNo].Vertices;
 
			delete [] Branches[BranchNo].NextIsoLines;
			delete [] Branches[BranchNo].PrevIsoLines;
		}

		// delete all neurons that don't have a V edge (deletes the Branch loops created and not used)
		int NeuronsRemoved = DeleteNeuronsByEdgeMask(EDGE_U_DIR);

#ifdef MY_DEBUG
		TRACE ("Neurons Removed after deleting edges");
#endif

		delete [] NewNeuronArray;
		#undef RECTNNNEW
		
		for (PatchNo=0;PatchNo<NumberOfPatches;PatchNo++){
			delete [] Patches[PatchNo].IsoLineNumbers;
		}


		// now use branch information to connect the isolines appropiatly.


		delete [] Patches;
		delete [] IsoLines;
		delete [] Branches;

	}
	else if (!strnicmp(FilePrefix,FilePrefixWRL,4)){

		
		MyPoint PtArray[MAX_ARRAY_SIZE];
		int PtIndex [MAX_ARRAY_SIZE];
		Neuron *NeuronArray [MAX_ARRAY_SIZE];
		int FaceIndex[3][MAX_ARRAY_SIZE];
		int PtNo=0, FaceNo=0 , LastPtNo=0;

		char Buffer[MAX_BUFFER_SIZE];
		char *HelpP;
		bool FileEnded=false;
		ifstream ifs;
		ifs.open(FileName);

		do {
			do{

				ifs.getline (Buffer,MAX_BUFFER_SIZE);

				if (ifs.eof()) { 
					FileEnded=true; 
					break;
				}

				if (ifs.fail()){
					ifs.close();
					return false;
				}

				HelpP=strstr(Buffer,"point");

			} while (HelpP==NULL || Buffer[0] == '\0');

			if (FileEnded) break;

			// found start of point list - now use it to extract points

			LastPtNo = PtNo;

			do{
				ifs.getline (Buffer,MAX_BUFFER_SIZE);
				if (ifs.fail()){
					ifs.close();
					return false;
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

				ifs.getline (Buffer,MAX_BUFFER_SIZE);
				
				if (ifs.fail()){
					ifs.close();
					return false;
				}

				HelpP=strstr(Buffer,"coordIndex");

			} while (HelpP==NULL || Buffer[0] == '\0');

			// found start of point list - now use it to extract points

			do{
				ifs.getline (Buffer,MAX_BUFFER_SIZE);
				if (ifs.fail()){
					ifs.close();
					return false;
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
		ifs.close();

		// now change all indices by finding duplications

		for (int i=0; i<PtNo; i++){
			if (PtIndex[i] == i){ 
				for (int j=i+1; j<PtNo; j++)
					if (PtArray[i].Pos[0]==PtArray[j].Pos[0] &&
						PtArray[i].Pos[1]==PtArray[j].Pos[1] &&
						PtArray[i].Pos[2]==PtArray[j].Pos[2] )
							PtIndex[j]=i;
					

				NeuronArray[i] = AddNeuron(PtArray[i]);
				
			}
			else 
				NeuronArray[i]=NULL;

		}

		// now create the faces

		for (i=0;i<FaceNo;i++){
			Neuron *N1, *N2, *N3;

			N1=NeuronArray[PtIndex[FaceIndex[0][i]]];
			N2=NeuronArray[PtIndex[FaceIndex[1][i]]];
			N3=NeuronArray[PtIndex[FaceIndex[2][i]]];

			Edge *E12,*E13,*E23;

			E12=Edges.CreateOrRefreshEdge(N1,N2);
			E13=Edges.CreateOrRefreshEdge(N1,N3);
			E23=Edges.CreateOrRefreshEdge(N2,N3);

			Faces.CreateOrRefreshFace(E12,E13,E23);

		}
	}
	return true;
}



bool NeuralNet::IsBoundaryNeuron(Neuron *N){
// a boundary neuron has at least one boundary edge (an edge with only one face connected to it)
	Edge *TempEdge=Edges.Head;

	int BoundaryEdgeCounter=0;
	if (Edges.Head != NULL) do{
		if ( TempEdge->IsEdgeOfNeuron(N) && TempEdge->FaceCounter==1){
			BoundaryEdgeCounter++;
			// edge found - exit
		}
		TempEdge=TempEdge->Next;
		
	} while (TempEdge!=Edges.Head);
	
	return ((BoundaryEdgeCounter % 2) ==0); // if number of boundary edges is even - this is a boundary edge

}




// new version of close boundary
void NeuralNet::CloseBoundary(){
	// loop through edges, util you find a boundary edge

	Edge *E=Edges.Head;

	// create an heper array for neurons in the size of the net
	
	Neuron **NeuronLoop= new (Neuron *[Neurons.Size+1]);
	for(int i=0;i<Neurons.Size+1;i++) NeuronLoop[i]=NULL;
	Edge **EdgeLoop= new (Edge *[Edges.Size]);
	for(i=0;i<Edges.Size;i++) EdgeLoop[i]=NULL;

	int LoopStart=0;
	int Itter=0;
	bool BoundaryEdgesStillExist=false;
	bool Flip=true;
	if (Edges.Head!=NULL) do{
		Itter++;
#ifdef MY_DEBUG
		TRACE("Triangulation Itteration %d\n",Itter);
#endif
		if (E==Edges.Head){
			BoundaryEdgesStillExist=false;
			Flip=!Flip;
		}
		if (E->FaceCounter ==1){ // this is an edge neuron
			BoundaryEdgesStillExist=true;
		
			int LoopSize=DetectLoop(NeuronLoop,EdgeLoop,Flip ? NeuronLoop[0]=E->Neuron1 : NeuronLoop[0]=E->Neuron2);
			//int LoopSize=DetectLoop(NeuronLoop,EdgeLoop,NeuronLoop[0]=E->Neuron1);
						// check if triangulation exists for this loop by checking one triangle

			if (LoopSize ==-1){
				TRACE("There was a problem finding a loop - check for manifold conditions - no loop was closed\n");
				break;
			}
				

			ASSERT(LoopSize);

			// Create a new neuron in the center of mass of the loop
			MyPoint InterpP = *NeuronLoop[0];
				for (int i=1; i<LoopSize; i++){
				InterpP = InterpP + *NeuronLoop[i];	
			}

			InterpP = InterpP * ( 1.0/(LoopSize) );


			Neuron *InterpN=AddNeuron(InterpP);

			//now coonect all points in the loop to the interpolation point
			// we do it the dirty way

			Edge *LastEdge=Edges.CreateOrRefreshEdge(InterpN,NeuronLoop[0],EDGE_CREATED_TO_CLOSE_LOOP);
			for (i=0; i<LoopSize; i++){
				Edge *NewEdge = Edges.CreateOrRefreshEdge(InterpN,NeuronLoop[i+1],EDGE_CREATED_TO_CLOSE_LOOP);			
				Faces.CreateOrRefreshFace(EdgeLoop[i], NewEdge, LastEdge, FACE_CREATED_TO_CLOSE_LOOP);
				LastEdge=NewEdge;
			}
			break;
		}

		E=E->Next;

	} while (E!=Edges.Head || BoundaryEdgesStillExist);
	
	delete [] NeuronLoop;
	delete [] EdgeLoop;

	// now count only manifold edges and neurons connected by faces to establish number of holes:

	int EdgeNum =0;
	E=Edges.Head;
	if (Edges.Head!=NULL) do{
		if (E->FaceCounter==2)
			EdgeNum++;
		E=E->Next;
	} while(E!=Edges.Head);

	int NeuronNum =0;
	Neuron *N=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		if (N->FaceCounter>0)
			NeuronNum++;
		N=N->Next;
	} while (N!=Neurons.Head);

	int FaceNum = Faces.Size;
	double HoleNum = (double)(EdgeNum - NeuronNum - FaceNum + 2.0)/2;

//#ifdef MY_DEBUG
	TRACE("Finished Triangulating loops number of holes is : %lf , E=%d , V=%d , F=%d \n", HoleNum , EdgeNum , NeuronNum , FaceNum );
	TRACE("E=%d , V=%d , F=%d \n", Edges.Size , Neurons.Size , Faces.Size );
//#endif
}






int NeuralNet::DetectLoop (Neuron **NeuronList, Edge **EdgeList, Neuron *NeuronInLoop){
	
	// we use simple BFS algorithm on the graph ang o back later
	// the function returns the number of neurons that fall under distance k
	

	if (Neurons.Size==0) return (-1); // -1 in output means an error
	if (Edges.Size==0) return (-1); // -1 in output means an error
#ifdef MY_DEBUG
	TRACE("Loooking for loop\n");
#endif
	// first check if finding a loop is possible (no vertex should have an odd number of edges eminating from it.
	
	Neuron *It=Neurons.Head;
	
	do{
		Edge *E = Edges.Head;
		int EminatingEdgeCounter=0;
		do{
			if (E->FaceCounter == 1 && E->IsEdgeOfNeuron(It))
				EminatingEdgeCounter++;
				
			E=E->Next;
		} while (E != Edges.Head);

		if (EminatingEdgeCounter %2 ==1 ) {
			TRACE ("NeuralNet::DetectLoop does not support odd number of edges eminating from Neuron ");
			return(-1);
		}

		It=It->Next;
	} while (It!=Neurons.Head);


	
	bool DuplicatesExist = false;
	Neuron *DuplicateNeuron=NeuronInLoop;

	int LoopSize;
	Neuron *NEnd , *NRight, *NLeft;
	bool LoopFound;

//	do{  // was here for duplicate detection

		int NeigbourhoodSize=1; // retains number of neigbours found.
		RealType MaxDist=Neurons.Size*2; // topological distance can't get so high
		//init distances
		do{
			It->PointDist=MaxDist;
			It=It->Next;
		} while (It != Neurons.Head);

		//search for initial neuron from the list
		// put it at the back of the list and set the Iterator to it
		// point to the last elemnt (our first neuron
		LoopFound=false;
		DuplicateNeuron->PointDist=0; // initial neuron distance is 0
		Neurons.Remove(DuplicateNeuron,false);
		Neurons.PushBack(*DuplicateNeuron,false);
		It=DuplicateNeuron;
		


		while (!LoopFound){ // while distance has not reached k or we want to sort all the neurons
			// now find all direct neibouring neurons and add them to the back of the list, 
			// they will be processed later on
			// for the sake of speed we do not find the neigbours in a function but do it online...
			// maybe we should add it later as a member function to the EdgeList class
			// also an optimization for rectangular SOM can be added later on since neurons are ordered in an array


			Edge *Ed=Edges.Head;
			if (Edges.Head!=NULL) do{
				if (Ed->FaceCounter ==1){
					Neuron *N2 = Ed->OtherNeuronInEdge(It);
					if (N2 !=NULL) // if N is a neuron of this edge we will get N2 not NULL
						if (N2->PointDist > It->PointDist){
						N2->PointDist = It->PointDist+1;

						// put it at the back of the list and set the Iterator to it
						Neurons.Remove(N2,false);
						Neurons.PushBack(*N2,false);
						NeigbourhoodSize++; // increase number of neigbours found.

						// check if a loop is closed
						Neuron *It2=DuplicateNeuron;
						// mark edge not to use it again (therwise it is not a loop
						Ed->FaceCounter = -2;
						do{
							Edge *ConnectingEdge;
							ConnectingEdge=Edges.FindEdge(N2,It2);
							if (ConnectingEdge !=NULL &&  // an edge exits
								It2!=It){  // that is not the edge already used
								if (ConnectingEdge->FaceCounter == 1){
									LoopFound=true;	
									NEnd=N2;
									NRight=It2;
									NLeft=It;
									//ConnectingEdge->FaceCounter = -2;
									break;
								}
							}
							It2=It2->Next;
						} while (It2 != N2 && ! LoopFound);

					}
				}
				Ed=Ed->Next;
			} while (Ed!=Edges.Head && !LoopFound);

			if (LoopFound) break; //if loop found exit BFS
			It=It->Next; //advance to the next unhandles neighbour
			if (It==Neurons.Head) break; //if no more neibours exist at end of list - exit loop
		}
#ifdef MY_DEBUG
		TRACE("still Loooking for loop\n");
#endif

		if (!LoopFound) 
			return (0);

		// now look of a path back using the RightSide;
		LoopSize=1;
		NeuronList[0]=NEnd;
		NeuronList[1]=NRight;
		//mark the edge so not to repeat it again
		EdgeList[0]=Edges.FindEdge(NEnd,NRight);
		EdgeList[0]->FaceCounter=-3;
		Neuron *TempN=NRight;
		while (TempN!=DuplicateNeuron){
			Edge *Ed=Edges.Head;
			if (Edges.Head!=NULL) do{
				//if (Ed->FaceCounter == 1 && Ed->IsEdgeOfNeuron(TempN)){
				if (Ed->FaceCounter == -2 && Ed->IsEdgeOfNeuron(TempN)){
					Neuron *OtherN = Ed->OtherNeuronInEdge(TempN);
					if (OtherN != NeuronList[LoopSize-1] && OtherN->PointDist < TempN->PointDist ){
						Neuron *TempN1=DuplicateNeuron;
						while (TempN1 != OtherN  &&
							   TempN1 != Neurons.Head  ){
							TempN1=TempN1->Next;
						}
						if (TempN1 ==OtherN){ // this means we found the neuron in the BFS search that is part of the loop
							LoopSize++;
							TempN=TempN1;
							ASSERT (LoopSize != Neurons.Size);
							NeuronList[LoopSize]=TempN;
							EdgeList[LoopSize-1]=Ed;
							//mark the edge so not to repeat it again
							Ed->FaceCounter=-3; // mark edge as passed

							break; // exit loop continue with to next loop
						}
					}
				} 
				Ed=Ed->Next;
			} while (Ed!=Edges.Head);
		}
		// now close the loop from the left side

		int LeftLoopSize = Neurons.Size; // put the loop at the back of the array
			//LoopSize + (int) (NEnd->PointDist);
		NeuronList[LeftLoopSize]=NEnd;
		LeftLoopSize--;
		NeuronList[LeftLoopSize]=NLeft;
		EdgeList[LeftLoopSize]=Edges.FindEdge(NEnd,NLeft);
		//mark the edge so not to repeat it again
		//EdgeList[LeftLoopSize]->FaceCounter=-2;
		TempN=NLeft;
		int ReconnectionIndex=-1;
		while (ReconnectionIndex<0){
			for (int i=0;i<LoopSize+1;i++)
				if (TempN == NeuronList[i]){
					ReconnectionIndex = i;
					break;
				}

			Edge *Ed=Edges.Head;
			if (Edges.Head!=NULL) do{
				//if (Ed->FaceCounter == 1 && Ed->IsEdgeOfNeuron(TempN)){
				if (Ed->FaceCounter == -2 && Ed->IsEdgeOfNeuron(TempN)){
					Neuron *OtherN = Ed->OtherNeuronInEdge(TempN);
					if (OtherN != NeuronList[LeftLoopSize+1] && OtherN->PointDist < TempN->PointDist){
						Neuron *TempN1=DuplicateNeuron;
						while (TempN1 != OtherN && 
							TempN1 != Neurons.Head){
							TempN1=TempN1->Next;
						}
						if (TempN1 ==OtherN){ // this means we found the neuron in the BFS search that is part of the loop
							LeftLoopSize--;
							TempN=TempN1;
							NeuronList[LeftLoopSize]=TempN;
							EdgeList[LeftLoopSize]=Ed;
							//mark the edge so not to repeat it again
							Ed->FaceCounter = -3; // mark edge as passed
							break; // exit loop continue with to next loop
						}
					}
				} 
				Ed=Ed->Next;
			} while (Ed!=Edges.Head);
		}

		// now rearrange the whole loop arrays 
#ifdef MY_DEBUG
		//debug check
		ASSERT ( NeuronList[LeftLoopSize] == NeuronList[ReconnectionIndex] );
#endif

		//first we move all the begining of the loop to the start of the array
		//for (int i=ReconnectionIndex;i<=LoopSize;i++){
		//	if (i<LoopSize) // last edge does not exist in loop yet
		//		EdgeList[i-ReconnectionIndex] = EdgeList[i];
		//	NeuronList[i-ReconnectionIndex] = NeuronList[i];
		//}


		for ( int i=0; i <= (Neurons.Size-LeftLoopSize); i++){
			NeuronList[ReconnectionIndex+i] = NeuronList[LeftLoopSize+i];
			if (i < Neurons.Size-LeftLoopSize)
				EdgeList[ReconnectionIndex+i] = EdgeList[LeftLoopSize+i];
		}


	//	ASSERT (LeftLoopSize==LoopSize) ;
#ifdef MY_DEBUG
		TRACE("found loop - cutting it down to no duplicates\n");
#endif
		LoopSize = ReconnectionIndex + Neurons.Size - LeftLoopSize ;
	
		int DupStartIndex=0;
		int DupEndIndex=0;
		//now make sure that this is the minimal loop by scanning the loop
		// see if there is a duplicate neuron in the list
		for (i=0;i<LoopSize;i++) for (int j=i+1;j<LoopSize;j++) if (NeuronList[i]==NeuronList[j]){
			DuplicatesExist = true;
			DuplicateNeuron = NeuronList[j];
			DupStartIndex = i;
			DupEndIndex = j;
			ASSERT (j!=i+1); // this means an edge and not a loop
		}
		
		if (DuplicatesExist){
			ASSERT(0); // this is old code that is not needed anymore since our algorithm produces no invalid loops
		}
		/*
			//mark all the edges before and after the loop with -1 in FaceCounter - this will be changed later on
			for (i=1; i<DupStartIndex; i++)
				EdgeList[i-1]->FaceCounter=-1;
			for (i=DupStartIndex-1; i<DupEndIndex; i++)
				EdgeList[i]->FaceCounter=1;
			for (i=DupEndIndex; i<LoopSize; i++)
				EdgeList[i]->FaceCounter=-1;

		}
		
		Edge *E = Edges.Head;
		do{
			if (E->FaceCounter <= -2)
				E->FaceCounter = 1;
			E=E->Next;
		} while (E != Edges.Head);
		*/

//	} while (DuplicatesExist);

    // restore all edges marked in thier original face count
	Edge *E = Edges.Head;
	do{
		if (E->FaceCounter < 0)
			E->FaceCounter = 1;
		E=E->Next;
	} while (E != Edges.Head);


	return (LoopSize);
}


/*
void NeuralNet::NewTriangulateNet(PointList &PtLst){

	//bool EdgesAddedFlag;
	
	bool Finished=false;
	
	static int CurrDist=1;
	static Neuron *N=Neurons.Head; // start with some neuron//
	int NumberOfNeuronsPassed=0;
	static NumberOfViolations=0;

	if (!StartedTriangulation){
		ClassifySamplePointList(PtLst);
		N=Neurons.Head; // start with some neuron//
		if (Neurons.Head!=NULL) do {
			N->CalcNormal();
			N=N->Next;
		} while (N!=Neurons.Head);
		StartedTriangulation=true;
		N->TriDist=1;  // this is the first neuron that should be passed
	}

//	do{

	if (~ N->Normal != 0.0){

		// first find the neighbours
		int Neighbours = CalcTopologicalDistance (N,1);
		MyPoint E1,E2;
		N->CalcProjectionPlane ( *(N->Next) - *N ,E1,E2);
		N->Proj.Pos[0]=0.0;
		N->Proj.Pos[1]=0.0;
		N->Proj.Pos[2]=0.0;
		Neuron *It=Neurons.Head->Next;
		for (int i=1; i<Neighbours;i++){
			It->ProjectViaNormal(E1,E2,*N);
			ASSERT(It->Angle >=0.0);
			if (It->TriDist==0)
				It->TriNew=true; // marking that triangulation has not reached this node yet
			if (It->TriDist==0 || It->TriDist > N->TriDist)
				It->TriDist = N->TriDist + 1; // update BFS distance
			It=It->Next;
		}
		

		// order by projection angle*** - simple bubble sort
		for (i=2; i<Neighbours; i++){
			Neuron *It2=N->Next->Next;
			for (int j=i; j<Neighbours-1;j++){
				if (It2->Angle > It2->Next->Angle)
					Neurons.Swap(It2,It2->Next);
				else
					It2=It2->Next;
			}
		}


		N=N->Next;		
//	}while (!Finished);
		

}

*/



void NeuralNet::AddGrowingNeuralFacesNeuron(RealType Alpha , PointList &PtLst){


			// this function trains the neural net as 
	// neural gas with competative Hebian learning
	// for a single sample point.
	// alpha is the training magnitude.

	// determine the neuron with maximum error accumulated

	if (Neurons.Head==NULL) MyFatalError ("AddGrowingNeuralGasNeuron - empty list");

	Neuron *It=Neurons.Head; // set iterator
	Neuron *N1 = It;

	do{
		if (It->Error > N1->Error)
			N1=It;
		It=It->Next;
	} while (It!=Neurons.Head);


	//find neigbouring neurons
	int Neighbours = CalcTopologicalDistance (N1,1); // detect direct neibours

#ifdef MY_DEBUG
	TRACE (" Neighbours = %d\n" , Neighbours);
#endif
	It=Neurons.Head->Next; // skip the winner (who is now the first on the list)

	//find neuron with max accumulated error
	Neuron *N2 = It;

	for (int i=0; i<Neighbours-1;i++){
		if (It->Error > N2->Error)
			N2=It;
		It=It->Next;
	}

	// put this neuron at the begining of the list just after the biggest error neuron- 
	// so it won't disturb us finding the third
	Neurons.Remove(N2,false);
	Neurons.Add(*N2,Neurons.Head->Next,false);
	// there must be at least one face connecting the two neurons found. 
	// between all the faces connecting the two, find the 
	// third neuron with the biggest accumulated error
	
	RealType MaxErr=-1.0;
	It=Neurons.Head->Next->Next;
	Neuron *N3 = It;
	for ( i=0; i<Neighbours-2;i++){
		if (It->Error > MaxErr && Faces.IsFace(N1,N2,It)!=NULL ){
			N3=It;
			MaxErr=It->Error;
		}
		It=It->Next;
	}

#ifdef MY_DEBUG
	//debug check
	if (MaxErr <0.0){
		MyFatalError ("no third neighbour found while trying to create a new neuron  \n");
	}
#endif

	// delete old face
	Faces.DelFace(N1,N2,N3);
//		MyFatalError ("trying to remove a non existing face ");

	Edge *E12,*E13,*E23,*E14,*E24,*E34;
	E12=Edges.FindEdge(N1,N2);
	E13=Edges.FindEdge(N1,N3);
	E23=Edges.FindEdge(N2,N3);
	
	// create a new neuron interpolating the three
	MyPoint InterpolationPoint = (*N2+ *N3 + *N1) * (1.0/3); 
	Neuron *NewNeuron = AddNeuron(InterpolationPoint);

	NewNeuron->Normal = (N1->Normal + N2->Normal + N3->Normal) * (1.0/3); 
	NewNeuron->Normal = NewNeuron->Normal * (1.0 / ~NewNeuron->Normal);

	// add the edges missing (for topological calculation of distance) the same
	E14=Edges.CreateOrRefreshEdge(NewNeuron,N1);
	E24=Edges.CreateOrRefreshEdge(NewNeuron,N2);
	E34=Edges.CreateOrRefreshEdge(NewNeuron,N3);

	// create three new ones instead, connected to the new neuron
	Faces.CreateOrRefreshFace(E12,E14,E24);
	Faces.CreateOrRefreshFace(E13,E14,E34);
	Faces.CreateOrRefreshFace(E23,E24,E34);

	// decrease accumulated error of the two edges that the edges were connedted to
	N1->Error *= (1.0-Alpha);
	N2->Error *= (1.0-Alpha);
	N3->Error *= (1.0-Alpha);
	//interpolate error for the new neuron
	NewNeuron->Error=(N1->Error+N2->Error+N3->Error)/3;

	// if topology of network was rectangular, it has changed now.
	if (Tags & NN_RECTANGULAR_TOPOLOGY)
		ChangeNetType (( Tags & (~NN_RECTANGULAR_TOPOLOGY) ) | NN_HAS_TOPOLOGY,PtLst);

} 


void NeuralNet::TrainGrowingNeuralFacesBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration){


	TIC(1)

	//find 2 winner neurons
	NeuronSortByWinners(Pt,3,false);

	Neuron *Nwin1,*Nwin2 ,*Nwin3;

	Nwin1 = Neurons.Head;
	Nwin2 = Neurons.Head->Next;

//	Edge *ReferenceEdge= Edges.FindEdge
//	NeuronSortByWinners(Pt,1,false,);

	TOC(1)

	TIC(2)

	

	Nwin3 = Neurons.Head->Next->Next;

	// update winner counter
	Nwin1->LastWinningAge=CurrentIteration;
	Nwin1->WinCounter++;


	// adapt the normal
	MyPoint N2_N1, N3_N1, Pt_N1;

	Pt_N1 = Pt - *Nwin1;
	N2_N1 = *Nwin2 - *Nwin1;
	N3_N1 = *Nwin3 - *Nwin1;
	N2_N1 = N2_N1 * (1.0 / ~N2_N1);
	N3_N1 = N3_N1 * (1.0 / ~N3_N1);

	
	//MyPoint NormalDirection = N2_N1 - (Pt_N1 * (N2_N1 * Pt_N1));  
	MyPoint NormalDirection = N2_N1 ^ N3_N1;  
	double Dir = NormalDirection * Pt_N1;
	if (Dir<=0.0) NormalDirection = - NormalDirection;

	/*
	MyPoint AverageNormal = (Nwin1->Normal + Nwin2->Normal +Nwin3->Normal) *(1.0/3);
	AverageNormal = AverageNormal*(1.0/~AverageNormal);
	double NormalComatibility=0.0;
	if (AverageNormal.IsFinite())
		 NormalComatibility = AverageNormal*NormalDirection; 

	if ( NormalComatibility > NormalComaptibilityThreshold){
	*/

		
		MyPoint NewNormal = Nwin1->Normal + (NormalDirection * (EpsB / ~NormalDirection));
		// now normalize
		NewNormal = NewNormal * (1.0 / ~NewNormal);
		if (NewNormal.IsFinite() )
			Nwin1->Normal = NewNormal;

/*		
		// new version with face normal checking for creation and replacement

			// now check if the new face is compatible with the average of normals in the 3 neighbours - if not - don't train

		Edge *E12, *E13 , *E23;

		E12=Edges.FindEdge (Nwin1,Nwin2);
		E13=Edges.FindEdge (Nwin1,Nwin3);
		E23=Edges.FindEdge (Nwin2,Nwin3);
		
		// delete faces from far to new
		bool CreateFace = true;
		Face *DelFace1,*DelFace2,*DelFace3;

		CreateFace = CreateFace && DelSimilarFacesForEdgeAndNeuron(E23 , Nwin1, &DelFace1);
		CreateFace = CreateFace && DelSimilarFacesForEdgeAndNeuron(E13 , Nwin2, &DelFace2);
		CreateFace = CreateFace && DelSimilarFacesForEdgeAndNeuron(E12 , Nwin3, &DelFace3);

		// count the numer of deleted faces
		int DeletedFaceNumber=0;
		if (DelFace1 != NULL) 
			DeletedFaceNumber++;
		if (DelFace2 != NULL && DelFace2 !=DelFace1) 
			DeletedFaceNumber++;
		if (DelFace3 != NULL && DelFace3 !=DelFace1 && DelFace3 !=DelFace2) 
			DeletedFaceNumber++;

		
		
		if (CreateFace){   //&& DeletedFaceNumber<2
			if (DelFace1 != NULL) 
				Faces.DelFace(DelFace1);
			if (DelFace2 != NULL && DelFace2 !=DelFace1) 
				Faces.DelFace(DelFace2);
			if (DelFace3 != NULL && DelFace3 !=DelFace1 && DelFace3 !=DelFace2) 
				Faces.DelFace(DelFace3);

			// create the new face
			E12=Edges.CreateOrRefreshEdge(Nwin1,Nwin2);
			E13=Edges.CreateOrRefreshEdge(Nwin1,Nwin3);
			E23=Edges.CreateOrRefreshEdge(Nwin2,Nwin3);
			Faces.CreateOrRefreshFace(E12,E13,E23);
			DelOrphanEdgesAndNeurons();	// some deletions were made - delete all orphans now

		}
*/
			// old version of algorithm
			Face *NewFace=Faces.IsFace(Nwin1,Nwin2,Nwin3);
			if (NewFace !=NULL){
				Faces.CreateOrRefreshFace(NewFace->Edge1,NewFace->Edge2,NewFace->Edge3);
			}
			else{
				Edge *E12, *E13 , *E23;

				E12=Edges.FindEdge (Nwin1,Nwin2);
				E13=Edges.FindEdge (Nwin1,Nwin3);
				E23=Edges.FindEdge (Nwin2,Nwin3);

				// check if edges belong to manifold
				bool CreateFace=true;
				if (E12 !=NULL){
					CreateFace = CreateFace && (E12->FaceCounter <2);
				}

				if (E13 !=NULL){
					CreateFace = CreateFace && (E13->FaceCounter <2);
				}

				if (E23 !=NULL){
					CreateFace = CreateFace && (E23->FaceCounter <2);
				}


				if (CreateFace){
					if (E12==NULL)
						E12=Edges.CreateOrRefreshEdge(Nwin1,Nwin2);

					if (E13==NULL)
						E13=Edges.CreateOrRefreshEdge(Nwin1,Nwin3);

					if (E23==NULL)
						E23=Edges.CreateOrRefreshEdge(Nwin2,Nwin3);

					Faces.CreateOrRefreshFace(E12,E13,E23);
				}

			}

//*/			

		// accumulate error
		RealType dist = ~(Pt - *Nwin1);
		Nwin1->Error += dist*dist; 

		// adapt winner

		if (Pt.Weight == 1.0)  // the if statment only saves computation time (it is negligable here, but we add it all the same
			Nwin1->ModifyPosBySamplePoint(EpsB,Pt);	
		else
			Nwin1->ModifyPosBySamplePoint(1.0-pow(1.0-EpsB,Pt.Weight),Pt);

		Nwin1->LastUpdateAge = CurrentIteration;
		
		//modify position in location array if needed
		AddOrRemoveNeuronToLocationArray(Nwin1);

		// now adapt all the neurons 
		// a point of thought- this is done before the deletion of the new edge in case of bad triangulation...
		//int Neighbours = CalcTopologicalDistance (Nwin,1); // detect direct neibours
		
		int Neighbours = CalcTopologicalDistance (Nwin1,1); // detect direct neibours
		Neuron *It = Neurons.Head->Next;  // skip the winner (who is now the first on the list)
		for (int i=0; i<Neighbours-1;i++){
			// first adapt position
			if (Pt.Weight == 1.0){
				It->ModifyPosBySamplePoint(EpsN,Pt);	
			}
			else
				It->ModifyPosBySamplePoint(1.0-pow(1.0-EpsN,Pt.Weight),Pt);
		
				NewNormal = It->Normal + (NormalDirection * (EpsN / ~NormalDirection));
				NewNormal = NewNormal * (1.0 / ~NewNormal);
				
				if (NewNormal.IsFinite() )
					It->Normal = NewNormal;

			//modify position in location array if needed
			AddOrRemoveNeuronToLocationArray(It);
			It->LastUpdateAge=CurrentIteration;
			It=It->Next;
		}
//	}

		//age all edges connected to the winner
		Faces.Age (Nwin1); 

		TOC(2)


		TIC(3)

		TOC(3)

		TIC(4)

		// we should now delete all faces that overlapp and have a common edge
		DeleteOverlappingFaces(Nwin1,2);

		TOC(4)


		//Faces.DeleteOld(AgeMax,true);
		// now do garbage collection on the edge list

		//DelOrphanEdgesAndNeurons();
	
}


int NeuralNet::TrainGrowingNeuralFacesByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LambdaGrowth){
	if (PtLst.Size==0){
		MyFatalError ("TrainGrowingNeuralFacesByPointList : no points to train");
		return false;
	}

	if ( EpsB<=0.0 || EpsN<=0.0 || Alpha<=0.0 || Betta <=0.0 || AgeMax <0 || LambdaGrowth <0 ) {
		MyFatalError ("TrainGrowingNeuralFacesByPointList : init values Eps, Sigma, Radius cant be zero");
		return false;
	}
		if ( !( ((Tags & NN_HAS_TOPOLOGY) || (Tags & NN_RECTANGULAR_TOPOLOGY)) && (Tags & NN_CAN_GROW_NEURONS) && (Tags & NN_CAN_GROW_EDGES) )){
		TRACE ("NeuralNet::TrainGrowingNeuralFacesByPointList  - training method is incompatible with Neural Network type");
		return false;
	}

	int Index = BaseIteration % PtLst.Size;
	for (int i=0; i<RunLength; i++){

		TrainGrowingNeuralFacesBySinglePoint (*PtLst.Array[Index] , EpsB, EpsN, AgeMax , BaseIteration+i);

		if ( ! ( (BaseIteration+i+1) % LambdaGrowth) ){  // every lambdagrowth iterations add a neuron
			AddGrowingNeuralFacesNeuron(Alpha,PtLst);

			OUTPUT_TOC(1)
			OUTPUT_TOC(2)
			OUTPUT_TOC(3)
			OUTPUT_TOC(4)
			OUTPUT_TOC(5)
			OUTPUT_TOC(6)
			OUTPUT_TOC(7)
			OUTPUT_TOC(8)
			OUTPUT_TOC(9)


		}

		DecreaseGrowingNeuralFacesError(Betta);
		Index = (Index +1) % PtLst.Size;
//		if (Index == 0) PtLst.Reorder();

	}

	return true;
}


void NeuralNet::DecreaseGrowingNeuralFacesError(RealType Betta){
	// decrease the accumulated error of all neurons by betta
	Neuron *It=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		It->Error *= (1.0-Betta);
		It=It->Next;
	} while (It!=Neurons.Head);
}


void NeuralNet::DelOrphanEdgesAndNeurons(){

	Edge *E=Edges.Head;
	for( int i=0;i<Edges.Size;i++){
		if (E->FaceCounter == 0){
			E=Edges.DelEdge(E,true);	// mark neurons for deletions wile deleting edges
			i--;
		}
		else 
			E=E->Next;
	}

	// now do garbage collection on the neuron list
	Neuron *It=Neurons.Head;
	for(i=0;i<Neurons.Size;i++){
		if (It->EdgeCounter == 0){
			// this neuron has lost all of it's faces - delete its edges and then it and free memory
			// first delete from location then from list
			It=DelNeuron(It);
			ASSERT (It); // this should never happed since it means no neurons in the list
			i--;
		}
		else 
			It=It->Next;
	}

}


Face *NeuralNet::FindBestFaceForManifold(Edge *E , Neuron *N){

		// the function goes over all the faces connted to the edge and returns the only face that does not need deletion
		// (the one with the Normal value farthest from the face Normal

		if (E==NULL) return (NULL); // this means that no edge exits and you may exit
		if (E->FaceCounter <2) return (NULL);// if only one face is connected, delete nothing
		// we know that two faces share this edge, find them both

	    Face *RetVal=NULL;
		const double CampatibilityThreshold = 0.7;

		MyPoint Normal1 , Normal;
		
		double Cos, BestCos;

		
		MyPoint Reference = (*(E->Neuron2) - *(E->Neuron1));
		//Reference = Reference * ( 1.0 / ~Reference);

		// calc Nrmal for candidate
		Normal= Reference^(*(N) - *(E->Neuron1));
		Normal = Normal * ( 1.0 / ~Normal);


		// now check which normal is closer to the new neuron normal

		Face *F=E->FacesStuckHead;
		BestCos=2.0;
		while (F!=NULL){

			Neuron *OtherNeuron1=F->OtherNeuronInFace(E->Neuron1,E->Neuron2);
			// calc normal for neuron
			Normal= Reference^(*(OtherNeuron1) - *(E->Neuron1));
			Normal = Normal * ( 1.0 / ~Normal);

			// now decide which face should be deleted
			Cos = Normal*Normal1;

			if (Cos<=BestCos){ // check if this face is the worst there is
				RetVal=F;
				BestCos=Cos;
			}
			F=E->NextFaceInFaceStuck(F);
		}

		return (RetVal);
}


int NeuralNet::DeleteOverlappingFaces(Neuron *Nwin,int FaceSeedNumbers){


	
	int ActualCandidateFacesNumber = 0;
	/* slow version - replaced
	Face **NeuronFaces;

	TIC(5)
	Neuron *N = Neurons.Head;
	for (int i=0; i<FaceSeedNumbers; i++){
		CandidateFacesNumber += (CandidateFacesNumber + N->FaceCounter);
		N=N->Next;
	}

	Face **FaceArray = new Face *[CandidateFacesNumber];

	for (i=0;i<CandidateFacesNumber;i++)
		FaceArray[i]=NULL;

	int ActualCandidateFacesNumber =0;
	
	N = Neurons.Head;
	for (i=0; i<FaceSeedNumbers; i++){
		NeuronFaces=N->FacesBelongingToNeuron();
		for (int j=0; j<N->FaceCounter ;j++){
			bool DuplicateFound=false;
			for (int k=0 ; k<ActualCandidateFacesNumber; k++){
				if (NeuronFaces[j] == FaceArray[k]){
					DuplicateFound=true;
					break;
				}	
			}
			if (!DuplicateFound){
				FaceArray[ActualCandidateFacesNumber]=NeuronFaces[j];
				ActualCandidateFacesNumber++;
			}

		}

		//AfxCheckMemory( );
		delete [] NeuronFaces;

		N=N->Next;
	}

	TOC(5)
  */

	TIC(5)

	// find all the neurons that faces eminating from them are candidates for an occlusion check
	CalcTopologicalDistance (Nwin,FaceSeedNumbers);

	Face **FaceArray = new Face *[Faces.Size];
	for (int i=0;i<Faces.Size;i++)
		FaceArray[i]=NULL;


	Face *F=Faces.Head;
	if (F!=NULL) do{
		Neuron *N1,*N2,*N3;
		F->GetFaceNeurons(&N1,&N2,&N3);
		if (N1->PointDist<=FaceSeedNumbers || 
			N2->PointDist<=FaceSeedNumbers || 
			N3->PointDist<=FaceSeedNumbers ) {
				FaceArray[ActualCandidateFacesNumber]=F;
				ActualCandidateFacesNumber++;
		}
		F=F->Next;
	}while (F!=Faces.Head);

	TOC(5)

	int NumberOfDeletions=0;
	//do  the following loop through all candidate faces found and eliminate problematic faces
	TIC(6)
	for (i=0;i<ActualCandidateFacesNumber;i++){
		Face *F1 = FaceArray[i];
		if (F1==NULL) continue; // skip face is it was deleted
		Neuron *F1N1,*F1N2, *F1N3;
		F1->GetFaceNeurons(&F1N1 , &F1N2 , &F1N3);
		bool DeleteF1=false;
		for(int j=i+1;j<ActualCandidateFacesNumber;j++){
			Face *F2 = FaceArray[j];
			if (F2==NULL) continue; // skip face is it was deleted	
			Neuron *CommonNeuron=NULL;
			Edge *CommonEdge=NULL;
			if (F2->IsFaceOfNeuron (F1N1))
				CommonNeuron=F1N1;
			if (F2->IsFaceOfNeuron (F1N2)){
				if (CommonNeuron==NULL)
					CommonNeuron=F1N2;
				else
					CommonEdge=Edges.FindEdge(F1N2,CommonNeuron);
			}
			if (F2->IsFaceOfNeuron (F1N3)){
#ifdef MY_DEBUG
				// debug check - this means we have the same face checked against itself (impossible under loop conditions)
				ASSERT (CommonEdge == NULL);
#endif
				if (CommonNeuron==NULL)
					CommonNeuron=F1N3;
				else
					CommonEdge=Edges.FindEdge(F1N3,CommonNeuron);
			}

			if (CommonEdge != NULL){
				// check if the fold is over and edge 
				// meanwhile do nothing.
				Neuron *Fa2N1 , *Fa2N2, *Fa2N3;
				Neuron *Fa1N1 , *Fa1N2, *Fa1N3;
				
				Fa1N1=CommonEdge->Neuron1;
				Fa1N2=CommonEdge->OtherNeuronInEdge(Fa1N1);
				Fa1N3=F1->OtherNeuronInFace(Fa1N1,Fa1N2);

				Fa2N1=Fa1N1;
				Fa2N2=Fa1N2;
				Fa2N3=F2->OtherNeuronInFace(Fa2N1,Fa2N2);

				// project via normal to detect the fold
				MyPoint E1,E2;
				Fa1N1->CalcProjectionPlane ( *Fa1N2 - *Fa1N1 ,E1,E2);

				Fa1N1->Proj.Pos[0]=0.0;
				Fa1N1->Proj.Pos[1]=0.0;
				Fa1N1->Proj.Pos[2]=0.0;

				Fa1N2->ProjectViaNormal(E1,E2,*Fa1N1);
				Fa1N3->ProjectViaNormal(E1,E2,*Fa1N1);
				Fa2N3->ProjectViaNormal(E1,E2,*Fa1N1);

				if ( F1->IsNeuronInProjFace(*Fa2N3) ||   // if the projection of one face in completly in the other or if at least one point of the other face is in the first this means the face should be deleted
					 F2->IsNeuronInProjFace(*Fa1N3) ||
					 CheckTwoProjLinesIntersect(Fa1N1,Fa1N3,Fa2N2,Fa2N3) ||
					 CheckTwoProjLinesIntersect(Fa1N2,Fa1N3,Fa2N1,Fa2N3) ){
					// delete the oldest face
					if (F1->CreationAge > F2->CreationAge){
						DeleteF1 = true;
						break; // get out of this loop to delete F1
					}
					else{
						// delete face from face list and from helper array
						FaceArray[j]=NULL;
						Faces.DelFace(F2,true);
						NumberOfDeletions++;
						continue; // to next face					
					}

				}


			}
			else if(CommonNeuron!=NULL){
				// if we have only one common Neuron (no common edges)
				// check whether the faces intersect under projection
				Neuron *Fa2N1 , *Fa2N2;
				Neuron *Fa1N1 , *Fa1N2;

				Fa1N1=F1->OtherNeuronInFace(CommonNeuron,false);
				Fa1N2=F1->OtherNeuronInFace(CommonNeuron,true);

				Fa2N1=F2->OtherNeuronInFace(CommonNeuron,false);
				Fa2N2=F2->OtherNeuronInFace(CommonNeuron,true);

				MyPoint E1,E2;
				CommonNeuron->CalcProjectionPlane ( *(Fa1N1) - *CommonNeuron ,E1,E2);

				CommonNeuron->Proj.Pos[0]=0.0;
				CommonNeuron->Proj.Pos[1]=0.0;
				CommonNeuron->Proj.Pos[2]=0.0;

				Fa1N1->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa1N2->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa2N1->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa2N2->ProjectViaNormal(E1,E2,*CommonNeuron);

				if ( F1->IsNeuronInProjFace(*Fa2N1) ||   // if the projection of one face in completly in the other or if at least one point of the other face is in the first this means the face should be deleted
					 F1->IsNeuronInProjFace(*Fa2N2) ||
					 F2->IsNeuronInProjFace(*Fa1N1) ||
					 F2->IsNeuronInProjFace(*Fa1N2) ||
					 CheckTwoProjLinesIntersect(Fa1N1,Fa1N2,CommonNeuron,Fa2N1) ||
					 CheckTwoProjLinesIntersect(Fa2N1,Fa2N2,CommonNeuron,Fa1N1) ){
					// delete the oldest face
					if (F1->CreationAge > F2->CreationAge){
						DeleteF1 = true;
						break; // get out of this loop to delete F1
					}
					else{
						// delete face from face list and from helper array
						FaceArray[j]=NULL;
						F2=Faces.DelFace(F2,true);
						NumberOfDeletions++;
						continue; // to next face					
					}

				}
			}

		}

		if (DeleteF1){
			FaceArray[i]=NULL;
			F1=Faces.DelFace(F1,true);
			NumberOfDeletions++;
		}

	}

	TOC(6)

	TIC(7)
	DelOrphanEdgesAndNeurons();
	TOC(7)

	delete [] FaceArray;
#ifdef MY_DEBUG
	TRACE ("DeleteOverlappingFaces:: deleted %d faces\n", NumberOfDeletions);
#endif
	return NumberOfDeletions;
}




/* old version of function - not used any more
int NeuralNet::DeleteOverlappingFaces(){

	int NumberOfDeletions=0;
	Face *F1=Faces.Head;
	if (Faces.Size>1) do{
		Neuron *F1N1,*F1N2, *F1N3;
		F1->GetFaceNeurons(&F1N1 , &F1N2 , &F1N3);
		bool DeleteF1=false;
		Face *F2=F1->Next;
		do{
			Neuron *CommonNeuron=NULL;
			Edge *CommonEdge=NULL;
			if (F2->IsFaceOfNeuron (F1N1))
				CommonNeuron=F1N1;
			if (F2->IsFaceOfNeuron (F1N2)){
				if (CommonNeuron==NULL)
					CommonNeuron=F1N2;
				else
					CommonEdge=Edges.FindEdge(F1N2,CommonNeuron);
			}
			if (F2->IsFaceOfNeuron (F1N3)){
#ifdef MY_DEBUG
				// debug check - this means we have the same face checked agains itself (impossible under loop conditions)
				ASSERT (CommonEdge == NULL);
#endif
				if (CommonNeuron==NULL)
					CommonNeuron=F1N3;
				else
					CommonEdge=Edges.FindEdge(F1N3,CommonNeuron);
			}

			if (CommonEdge != NULL){
				// check if the fold is over and edge 
				// meanwhile do nothing.
				Neuron *Fa2N1 , *Fa2N2, *Fa2N3;
				Neuron *Fa1N1 , *Fa1N2, *Fa1N3;
				
				Fa1N1=CommonEdge->Neuron1;
				Fa1N2=CommonEdge->OtherNeuronInEdge(Fa1N1);
				Fa1N3=F1->OtherNeuronInFace(Fa1N1,Fa1N2);

				Fa2N1=Fa1N1;
				Fa2N2=Fa1N2;
				Fa2N3=F2->OtherNeuronInFace(Fa2N1,Fa2N2);

				// project via normal to detect the fold
				MyPoint E1,E2;
				Fa1N1->CalcProjectionPlane ( *Fa1N2 - *Fa1N1 ,E1,E2);

				Fa1N1->Proj.Pos[0]=0.0;
				Fa1N1->Proj.Pos[1]=0.0;
				Fa1N1->Proj.Pos[2]=0.0;

				Fa1N2->ProjectViaNormal(E1,E2,*Fa1N1);
				Fa1N3->ProjectViaNormal(E1,E2,*Fa1N1);
				Fa2N3->ProjectViaNormal(E1,E2,*Fa1N1);

				if ( F1->IsNeuronInProjFace(*Fa2N3) ||   // if the projection of one face in completly in the other or if at least one point of the other face is in the first this means the face should be deleted
					 F2->IsNeuronInProjFace(*Fa1N3) ||
					 CheckTwoProjLinesIntersect(Fa1N1,Fa1N3,Fa2N2,Fa2N3) ||
					 CheckTwoProjLinesIntersect(Fa1N2,Fa1N3,Fa2N1,Fa2N3) ){
					// delete the oldest face
					if (F1->CreationAge > F2->CreationAge){
						DeleteF1 = true;
						break; // get out of this loop to delete F1
					}
					else{
						F2=Faces.DelFace(F2,true);
						NumberOfDeletions++;
						continue; // to next face					
					}

				}


			}
			else if(CommonNeuron!=NULL){
				// if we have only one common Neuron (no common edges)
				// check whether the faces intersect under projection
				Neuron *Fa2N1 , *Fa2N2;
				Neuron *Fa1N1 , *Fa1N2;

				Fa1N1=F1->OtherNeuronInFace(CommonNeuron,false);
				Fa1N2=F1->OtherNeuronInFace(CommonNeuron,true);

				Fa2N1=F2->OtherNeuronInFace(CommonNeuron,false);
				Fa2N2=F2->OtherNeuronInFace(CommonNeuron,true);

				MyPoint E1,E2;
				CommonNeuron->CalcProjectionPlane ( *(Fa1N1) - *CommonNeuron ,E1,E2);

				CommonNeuron->Proj.Pos[0]=0.0;
				CommonNeuron->Proj.Pos[1]=0.0;
				CommonNeuron->Proj.Pos[2]=0.0;

				Fa1N1->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa1N2->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa2N1->ProjectViaNormal(E1,E2,*CommonNeuron);
				Fa2N2->ProjectViaNormal(E1,E2,*CommonNeuron);

				if ( F1->IsNeuronInProjFace(*Fa2N1) ||   // if the projection of one face in completly in the other or if at least one point of the other face is in the first this means the face should be deleted
					 F1->IsNeuronInProjFace(*Fa2N2) ||
					 F2->IsNeuronInProjFace(*Fa1N1) ||
					 F2->IsNeuronInProjFace(*Fa1N2) ||
					 CheckTwoProjLinesIntersect(Fa1N1,Fa1N2,CommonNeuron,Fa2N1) ||
					 CheckTwoProjLinesIntersect(Fa2N1,Fa2N2,CommonNeuron,Fa1N1) ){
					// delete the oldest face
					if (F1->CreationAge > F2->CreationAge){
						DeleteF1 = true;
						break; // get out of this loop to delete F1
					}
					else{
						F2=Faces.DelFace(F2,true);
						NumberOfDeletions++;
						continue; // to next face					
					}

				}

			}

			F2=F2->Next;

		} while (F2!=Faces.Head);

		if (DeleteF1){
			F1=Faces.DelFace(F1,true);
			NumberOfDeletions++;
		}
		else
			F1=F1->Next;

	} while (F1->Next !=Faces.Head);

	DelOrphanEdgesAndNeurons();
#ifdef MY_DEBUG
	TRACE ("DeleteOverlappingFaces:: deleted %d faces\n", NumberOfDeletions);
#endif
	return NumberOfDeletions;
}

*/


bool NeuralNet::CheckTwoProjLinesIntersect( Neuron *L1N1 , Neuron *L1N2 , Neuron *L2N1 ,Neuron *L2N2){

	MyPoint V1 = (L1N2->Proj - L1N1->Proj);
	MyPoint V2 = (L2N1->Proj - L1N1->Proj);
	MyPoint V3 = (L2N2->Proj - L1N1->Proj);
				
	MyPoint Dir1=V1^V2;
	MyPoint Dir2=V1^V3;
	double Check1=Dir1*Dir2;
					

	V1 = (L2N2->Proj - L2N1->Proj);
	V2 = (L1N1->Proj - L2N1->Proj);
	V3 = (L1N2->Proj - L2N1->Proj);

	Dir1=V1^V2;
	Dir2=V1^V3;
	double Check2=Dir1*Dir2;


	return (( Check1 < 0.0 ) && ( Check2 < 0.0 ));

}


int NeuralNet::DeleteOldNeuronsInGrowingCellStructure (int CurrentIteration,int AgeMax ){
	
	Neuron *It=Neurons.Head;
	int OldSize=Neurons.Size;;
	int MaxAge=CurrentIteration;
	if (Neurons.Head!=NULL) do {
		if ( (CurrentIteration - It->LastWinningAge) > AgeMax) {
			It = DelNeuronAndCascadeDelete( It , true);
		}	
		else
			It = It->Next;
	}while (It != Neurons.Head);


	return (Neurons.Size-OldSize);
}




// growing cell structures


Neuron* NeuralNet::AddGrowingCellStructureNeuron(RealType Alpha, PointList &PtLst){


	// this function trains the neural net as 
	// neural gas with competative Hebian learning
	// for a single sample point.
	// alpha is the training magnitude.

	// determine the neuron with maximum error accumulated

	if (Neurons.Head==NULL) MyFatalError ("AddGrowingCellStructureNeuron - empty list");

	Neuron *It=Neurons.Head; // set iterator
	Neuron *N1 = It;

	do{
		if (It->Error > N1->Error)
			N1=It;
		It=It->Next;
	} while (It!=Neurons.Head);


	//find neigbouring neurons
	int Neighbours = CalcTopologicalDistance (N1,1,true); // detect direct neibours

	It=Neurons.Head->Next; // skip the winner (who is now the first on the list)

	//find neuron farthest from N1 from the neighbours
	Neuron *N2 = NULL;
	for (int i=1; i<Neighbours;i++){
		It->PointDist = ~((*It) - (*N1));
		Edge *E=Edges.FindEdge(It,N1,true);
		if ( E->FaceCounter>0 && N2==NULL || E->FaceCounter>0 && It->PointDist > N2->PointDist)
			N2=It;
		It=It->Next;
	}

	ASSERT(N2);
	Edge *E = Edges.FindEdge(N1,N2,true);
	Neuron *NewNeuron=EdgeSplit(E);

	// now decrease the error of the neighbouring neurons
	//find neigbouring neurons
	Neighbours = CalcTopologicalDistance (NewNeuron,1); // detect direct neibours
	Neighbours--; // only the neighbours without the N1

	double NeighbourRelativeErrorDecrease = (1.0-Alpha/Neighbours);
	double AccumulateError=0.0;
	int AccumulateLastWinAge=0;
	It=Neurons.Head->Next; // skip the winner (who is now the first on the list)
	for (i=0; i<Neighbours;i++){
		It->Error *= NeighbourRelativeErrorDecrease;
		AccumulateError += It->Error;
		AccumulateLastWinAge += It->LastWinningAge;
		It=It->Next;
	}

	// the new error of N1 is the average of all other neighbours
	Neurons.Head->Error = AccumulateError/Neighbours;
	Neurons.Head->LastWinningAge = AccumulateLastWinAge/Neighbours;


	// if topology of network was rectangular, it has changed now.
	if (Tags & NN_RECTANGULAR_TOPOLOGY)
		ChangeNetType (( Tags & (~NN_RECTANGULAR_TOPOLOGY) ) | NN_HAS_TOPOLOGY,PtLst);

	return NewNeuron;

} 


void NeuralNet::TrainGrowingCellStructureBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration){

	//*************** THIS FUNCTION NEEDS RETOUCHING ****
	TIC(1)

	//find 3 winner neurons
	NeuronSortByWinners(Pt,3,false);

	Neuron *Nwin1,*Nwin2 ,*Nwin3;

	Nwin1 = Neurons.Head;
	Nwin2 = Neurons.Head->Next;
	Nwin3 = Neurons.Head->Next->Next;
/*
	// check if a new face should be added at the boundary
	if ((Nwin1->Tags & NEURON_ON_BOUNDARY) &&
		(Nwin2->Tags & NEURON_ON_BOUNDARY) &&
		(Nwin3->Tags & NEURON_ON_BOUNDARY)
		&& CurrentIteration > AgeMax	){
		Edge *E12,*E13,*E23;

		E12=Edges.FindEdge(Nwin1,Nwin2,true);
		E13=Edges.FindEdge(Nwin1,Nwin3,true);
		E23=Edges.FindEdge(Nwin2,Nwin3,true);

		bool CreateBoundaryFace	= true;
		// if appropriate - create a face and thus shorten the boundary length
		if (E13!=NULL && E23!=NULL){
			E12= Edges.CreateOrRefreshEdge(Nwin1,Nwin2);
			Nwin3->Tags &= (~NEURON_ON_BOUNDARY);
		}
		else if (E12!=NULL && E23!=NULL){
			E13= Edges.CreateOrRefreshEdge(Nwin1,Nwin3);
			Nwin2->Tags &= (~NEURON_ON_BOUNDARY);
		}
		else if (E12!=NULL && E13!=NULL){
			E23= Edges.CreateOrRefreshEdge(Nwin2,Nwin3);
			Nwin1->Tags &= (~NEURON_ON_BOUNDARY);
		}
		else
			CreateBoundaryFace=false;

		if (CreateBoundaryFace)
			Faces.CreateOrRefreshFace(E12,E13,E23);

	}
*/
	TOC(1)

	TIC(2)


	Edges.CreateOrRefreshEdge(Nwin1,Nwin2);

	
	// update winner counter
	Nwin1->LastWinningAge=CurrentIteration;
	Nwin1->WinCounter++;



	// adapt the normal
	MyPoint N2_N1, N3_N1, Pt_N1;

	Pt_N1 = Pt - *Nwin1;
	N2_N1 = *Nwin2 - *Nwin1;
	N3_N1 = *Nwin3 - *Nwin1;
	N2_N1 = N2_N1 * (1.0 / ~N2_N1);
	N3_N1 = N3_N1 * (1.0 / ~N3_N1);

	//MyPoint NormalDirection = N2_N1 - (Pt_N1 * (N2_N1 * Pt_N1));  
	MyPoint NormalDirection = N2_N1 ^ N3_N1;  
	double Dir = NormalDirection * Pt_N1;
	if (Dir<=0.0) NormalDirection = - NormalDirection;

	// redo this calculation as in growing neural gas
	MyPoint NewNormal = Nwin1->Normal + (NormalDirection * (EpsB / ~NormalDirection));
	// now normalize
	NewNormal = NewNormal * (1.0 / ~NewNormal);
	if (NewNormal.IsFinite() )
		Nwin1->Normal = NewNormal;

		// accumulate error
	RealType dist = ~(Pt - *Nwin1);
	Nwin1->Error += dist*dist; 

	// adapt winner

	if (Pt.Weight == 1.0)  // the if statment only saves computation time (it is negligable here, but we add it all the same
		Nwin1->ModifyPosBySamplePoint(EpsB,Pt);	
	else
		Nwin1->ModifyPosBySamplePoint(1.0-pow(1.0-EpsB,Pt.Weight),Pt);

	Nwin1->LastUpdateAge = CurrentIteration;
	
	//modify position in location array if needed
	AddOrRemoveNeuronToLocationArray(Nwin1);

	// now adapt all the neurons 
	// a point of thought- this is done before the deletion of the new edge in case of bad triangulation...
	//int Neighbours = CalcTopologicalDistance (Nwin,1); // detect direct neibours
	
	int Neighbours = CalcTopologicalDistance (Nwin1,1,true); // detect direct neibours
	Neuron *It = Neurons.Head->Next;  // skip the winner (who is now the first on the list)
	for (int i=0; i<Neighbours-1;i++){
		// first adapt position
		if (Pt.Weight == 1.0){
			It->ModifyPosBySamplePoint(EpsN,Pt);	
		}
		else
			It->ModifyPosBySamplePoint(1.0-pow(1.0-EpsN,Pt.Weight),Pt);

			// redo this calculation as in growing neural gas
			NewNormal = It->Normal + (NormalDirection * (EpsN / ~NormalDirection));
			NewNormal = NewNormal * (1.0 / ~NewNormal);
			
			if (NewNormal.IsFinite() )
				It->Normal = NewNormal;

		//modify position in location array if needed
		AddOrRemoveNeuronToLocationArray(It);
		It->LastUpdateAge=CurrentIteration;
		It=It->Next;
	}


//	DeleteOldNeuronsInGrowingCellStructure(CurrentIteration,AgeMax);
	Edges.Age (Nwin1); 
	Edges.DeleteOld(AgeMax,true,true);

	TOC(2)
	
}


int NeuralNet::TrainGrowingCellStructureByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LambdaGrowth){
	if (PtLst.Size==0){
		MyFatalError ("TrainGrowingCellStructureByPointList : no points to train");
		return false;
	}

	if ( EpsB<=0.0 || EpsN<=0.0 || Alpha<=0.0 || Betta <=0.0 || AgeMax <0 || LambdaGrowth <0 ) {
		MyFatalError ("TrainGrowingCellStructureByPointList : init values Eps, Sigma, Radius cant be zero");
		return false;
	}
		if ( !( ((Tags & NN_HAS_TOPOLOGY) || (Tags & NN_RECTANGULAR_TOPOLOGY)) && (Tags & NN_CAN_GROW_NEURONS) && (Tags & NN_CAN_GROW_EDGES) )){
		TRACE ("NeuralNet::TrainGrowingCellStructureByPointList  - training method is incompatible with Neural Network type");
		return false;
	}

	int Index = BaseIteration % PtLst.Size;
	for (int i=0; i<RunLength; i++){

		TrainGrowingCellStructureBySinglePoint (*PtLst.Array[Index] , EpsB, EpsN, AgeMax , BaseIteration+i);

		if ( ! ( (BaseIteration+i+1) % LambdaGrowth) ){  // every lambdagrowth iterations add a neuron
			Neuron *N=AddGrowingCellStructureNeuron(Alpha,PtLst);
			N->LastUpdateAge = BaseIteration+i;

			OUTPUT_TOC(1)
			OUTPUT_TOC(2)
			OUTPUT_TOC(3)
			OUTPUT_TOC(4)
			OUTPUT_TOC(5)
			OUTPUT_TOC(6)
			OUTPUT_TOC(7)
			OUTPUT_TOC(8)
			OUTPUT_TOC(9)

		}

		DecreaseGrowingCellStructureError(Betta);
		Index = (Index +1) % PtLst.Size;
//		if (Index == 0) PtLst.Reorder();

	}

	return true;
}


void NeuralNet::DecreaseGrowingCellStructureError(RealType Betta){
	// decrease the accumulated error of all neurons by betta
	Neuron *It=Neurons.Head;
	double RelativeErrorDecrease=(1.0-Betta);
	if (Neurons.Head!=NULL) do{
		It->Error *= RelativeErrorDecrease;
		It=It->Next;
	} while (It!=Neurons.Head);
}

Neuron *NeuralNet::EdgeSplit(Edge *E){
	// the function inserts a new neuron at the middle of edge E
	// the faces that E belongs to are split accordingly
	// the function returns a pointer to the new neuron created
	

	Face *F;
	Neuron *N4=AddNeuron((*(E->Neuron1) + *(E->Neuron2))*0.5);
	Edge *E13,*E23,*E14,*E24,*E34;

	if (E->FaceCounter ==1 )
		N4->Tags  |= NEURON_ON_BOUNDARY;

	E14=Edges.CreateOrRefreshEdge(E->Neuron1,N4);
	E24=Edges.CreateOrRefreshEdge(E->Neuron2,N4);

	F=E->FacesStuckHead;
	while (F!=NULL){
		Neuron *N3 = F->OtherNeuronInFace(E->Neuron1 , E->Neuron2);
		E13 = Edges.FindEdge (E->Neuron1,N3,true);				
		E23 = Edges.FindEdge (E->Neuron2,N3,true);
		E34=Edges.CreateOrRefreshEdge(N3,N4);

		// delete old face and create two others instead
		Face *NewF=E->NextFaceInFaceStuck(F);
		Faces.DelFace(F);

		// notice that the new faces will not be pases - since they are inthe begining of the FaceStuck
		Faces.CreateOrRefreshFace(E13,E34,E14);		
		Faces.CreateOrRefreshFace(E23,E34,E24);		
	
		F=NewF;
	}

	Edges.DelEdge(E);
		
	return (N4);
}




Edge *NeuralNet::EdgeSwap(Edge *E){
	// the function swaps the edge with another edge creating the same two faces
	// the function returns a pointer to the new edge created
	// the function assumes that the surface is edge manifold

	//  check first
	if (E->FaceCounter!=2)
		return NULL;

	Neuron *N1,*N2,*N3,*N4;
	Edge *E34,*E13, *E14, *E23 ,*E24;

	Face *F1 = E->FacesStuckHead;
	Face *F2 = E->NextFaceInFaceStuck(F1);

	N1=E->Neuron1;
	N2=E->Neuron2;
	N3=F1->OtherNeuronInFace(N1,N2);
	N4=F2->OtherNeuronInFace(N1,N2);
	
	E13=Edges.FindEdge(N1,N3,true);
	E14=Edges.FindEdge(N1,N4,true);
	E23=Edges.FindEdge(N2,N3,true);
	E24=Edges.FindEdge(N2,N4,true);

	// delete old edge and faces
	Faces.DelFace(F1);
	Faces.DelFace(F2);
	Edges.DelEdge(E);

	// creaing swapping edge
	E34=Edges.CreateOrRefreshEdge(N3,N4);

#ifdef MY_DEBUG
	//debug check
	if (E34->FaceCounter>0)
		TRACE("EdgeSwap : Warning new edge already exits - faces will probably be deleted\n");
#endif

	// now create the new faces
	Faces.CreateOrRefreshFace(E13,E14,E34);
	Faces.CreateOrRefreshFace(E23,E24,E34);
		
	return (E34);
}





Neuron *NeuralNet::EdgeCollapse(Edge *E){
	// the function icollapses an edge by merging the two neurons defining it
	// all the edges and faces connected to these neurons are modified acordingly
	// the function returns a pointer to the new neuron that was merged
	// the function assumes that the surface is edge manifold



	Face **FaceList;
	Neuron *N1,*N2,*N3,*N4,*NewNeuron,*TempN ;
	Edge *E34,*E13, *E14, *E23 ,*E24;

	N1=E->Neuron1;
	N2=E->Neuron2;
	
	// interpolate the new neuron
	NewNeuron=AddNeuron((*N1+*N2)*0.5);

	// transfer all the edges from neuron1 to the new neuron
	Edge *TempEdge=N1->EdgeStuckHead;

	while(TempEdge!=NULL){
		TempN=TempEdge->OtherNeuronInEdge(N1);
		Edges.CreateOrRefreshEdge(NewNeuron,TempN);
		TempEdge=N1->NextEdgeInEdgeStuck(TempEdge);
	}

	// transfer all the edges from neuron2 to the new neuron
	TempEdge=N2->EdgeStuckHead;

	while(TempEdge!=NULL){
		TempN=TempEdge->OtherNeuronInEdge(N2);
		Edges.CreateOrRefreshEdge(NewNeuron,TempN);
		TempEdge=N2->NextEdgeInEdgeStuck(TempEdge);
	}

	// delete old faces from neuron1 and create new ones instead from the new neuron

	FaceList=N1->FacesBelongingToNeuron();
	for (int i=0;i<N1->FaceCounter;i++){
		Face *F=FaceList[i];
		N3=F->OtherNeuronInFace(N1,false);
		N4=F->OtherNeuronInFace(N1,true);

		Faces.DelFace(F); // this face is no longer of any intrest - delete it and then replce it

		if (!(N3==N2 || N4==N2)){ // this face should be deleted in the new scheme
			E13=Edges.FindEdge(NewNeuron,N3);
			E14=Edges.FindEdge(NewNeuron,N4);
			E34=Edges.FindEdge(N3,N4);
			Faces.CreateOrRefreshFace(E13,E14,E34);
		}
		
	}
	delete [] FaceList; // return memory

	// delete old faces from neuron2 and create new ones instead from the new neuron

	FaceList=N2->FacesBelongingToNeuron();
	for (i=0;i<N2->FaceCounter;i++){
		Face *F=FaceList[i];
		N3=F->OtherNeuronInFace(N2,false);
		N4=F->OtherNeuronInFace(N2,true);

		Faces.DelFace(F); // this face is no longer of any intrest - delete it and then replce it

#ifdef MY_DEBUG
		ASSERT (!(N3==N1 || N4==N1)); // this should not happen since we already deleted that face earlier in N1
#endif
		E23=Edges.FindEdge(NewNeuron,N3);
		E24=Edges.FindEdge(NewNeuron,N4);
		E34=Edges.FindEdge(N2,N4);
		Faces.CreateOrRefreshFace(E23,E24,E34);

	}
	delete [] FaceList; // return memory

	// delete all the edges from neuron1
	while(N1->EdgeStuckHead != NULL){
		Edges.DelEdge(N1->EdgeStuckHead);
	}

	// delete all the edges from neuron2
	while(N2->EdgeStuckHead != NULL){
		Edges.DelEdge(N2->EdgeStuckHead);
	}


	//delete the neurons N1 and N2
	DelNeuron(N1);
	DelNeuron(N2);

	return (NewNeuron);
}



Neuron *NeuralNet::DelNeuron(Neuron *N){

	AddOrRemoveNeuronToLocationArray(N,true);
	Neuron *RetVal=Neurons.Remove(N);
	return (RetVal);

}


bool NeuralNet::MergeCollision(Neuron *N1, Neuron *N2){
	// the funtion returns true if a merge has been successful and false otherwise
	
	// we check that both neurons are not boundary neurons and the distance between them is greater then 3 (otherwise there may be problems in creating the connections
	
	if ((N1->Tags & NEURON_ON_BOUNDARY) || (N2->Tags & NEURON_ON_BOUNDARY)){
		TRACE ("NeuralNet::MergeCollision - warning N1 or N2 are boundary neurons");
		return false;
	}

	CalcTopologicalDistance(N1,3,true);
	if (N2->PointDist<4){
		TRACE ("NeuralNet::MergeCollision - N1 and N2 are of topological distance of 3 or less");
		return false;
	}



	if (CheckManifoldType() == NON_MANIFOLD) 
		return false;

	int N1LoopSize = N1->FaceCounter;
	int N2LoopSize = N2->FaceCounter;

	int LoopSize = max(N1LoopSize,N2LoopSize);

	Neuron **NeuronLoop1 = new (Neuron *[LoopSize]);
	Neuron **NeuronLoop2 = new (Neuron *[LoopSize]);

	for (int i=0; i<LoopSize ; i++){
		NeuronLoop1[i]=NULL;
		NeuronLoop2[i]=NULL;
	}

	//construct Neuron Loop for N1
	Face *LastFace=N1->EdgeStuckHead->FacesStuckHead;
	Neuron *OtherNeuron = LastFace->OtherNeuronInFace(N1);

	for (i=0; i<N1LoopSize ; i++){
		NeuronLoop1[i]=OtherNeuron;
		OtherNeuron = LastFace->OtherNeuronInFace(N1,OtherNeuron);
		Edge *E = Edges.FindEdge(N1,OtherNeuron,true);
		ASSERT (E->FaceCounter==2);
		LastFace = E->NextFaceInFaceStuck (LastFace);
	}


	//construct Neuron Loop for N2

	LastFace=N2->EdgeStuckHead->FacesStuckHead;
	OtherNeuron = LastFace->OtherNeuronInFace(N2);

	for (i=0; i<N2LoopSize ; i++){
		NeuronLoop2[i]=OtherNeuron;
		OtherNeuron = LastFace->OtherNeuronInFace(N2,OtherNeuron);
		Edge *E = Edges.FindEdge(N2,OtherNeuron,true);
		ASSERT (E->FaceCounter==2);
		LastFace = E->NextFaceInFaceStuck (LastFace);
	}



	// check that the loops do not contain a boundary neuron
	bool InvalidMerge=false;
	for (i=0; i<N1LoopSize ; i++){
		if (NeuronLoop1[i]->Tags & NEURON_ON_BOUNDARY)
			InvalidMerge = true;
	}

	for (i=0; i<N2LoopSize ; i++){
		if (NeuronLoop2[i]->Tags & NEURON_ON_BOUNDARY)
			InvalidMerge = true;
	}


	if (InvalidMerge){
		delete [] NeuronLoop1;
		delete [] NeuronLoop2;
		TRACE ("NeuralNet::MergeCollision - warning boundary neurons detected");
		return (false);
	}

	//add neurons to smaller loop to equalize number of neurons in both loops

	Neuron **SmallerLoop ;
	int *SmallerLoopSize , *LargerLoopSize;

	if (N1LoopSize > N2LoopSize){
		SmallerLoop = NeuronLoop2;
		SmallerLoopSize = &N2LoopSize;
		LargerLoopSize = &N1LoopSize;
	} else{
		SmallerLoop = NeuronLoop1;
		SmallerLoopSize = &N1LoopSize;
		LargerLoopSize = &N2LoopSize;
	}

	// do until number of neurons equalized
	while ( *SmallerLoopSize < *LargerLoopSize){

		int LargestEdgeStartIndex=-1;
		double LargestEdgeSize=0.0;
		// look for largest edge 
		for (i=0; i<*SmallerLoopSize; i++){
			double EdgeSize = ~(*SmallerLoop[i] - *SmallerLoop[(i+1) % *SmallerLoopSize]);
			if (EdgeSize>LargestEdgeSize){
				LargestEdgeStartIndex=i;
				LargestEdgeSize=EdgeSize;
			}
		}

#ifdef MY_DEBUG
		ASSERT(LargestEdgeStartIndex > -1);
#endif

		// locate largest edge
		Edge *E = Edges.FindEdge(SmallerLoop[LargestEdgeStartIndex] , SmallerLoop[(LargestEdgeStartIndex+1) % *SmallerLoopSize],true);

#ifdef MY_DEBUG
		ASSERT(E);
#endif

		// free a place for the new neuron when edge split will occour.
		
		for (i = *SmallerLoopSize; i>LargestEdgeStartIndex+1; i--)
			SmallerLoop[i] = SmallerLoop[i-1];
#ifdef MY_DEBUG
		ASSERT (SmallerLoop[LargestEdgeStartIndex] == SmallerLoop[LargestEdgeStartIndex+1]);
#endif
		(*SmallerLoopSize)++; // increase the loop size
		// insert the new neuron
		Neuron *NewNeuron=EdgeSplit(E);
		SmallerLoop[LargestEdgeStartIndex+1]=NewNeuron;
	}

	// now find the best correspondace (minimal sum of squared distances) between the neuron loops

	int LeftCorrespondance=-1;
	double BestCorrespondanceSum = MAX_DOUBLE;

	for (int LeftOffset=0;LeftOffset<LoopSize;LeftOffset++){
		double SumOfSquaredDistances=0.0;
		for (i=0; i<LoopSize; i++){
			double Dist = ~(*NeuronLoop1[i] - *NeuronLoop2[(i+LeftOffset) % LoopSize]);
			SumOfSquaredDistances += Dist*Dist;
		}

		if (BestCorrespondanceSum > SumOfSquaredDistances){
			BestCorrespondanceSum = SumOfSquaredDistances;
			LeftCorrespondance=LeftOffset;			
		}
	
	}

	// find corrspondance for the same loops in the opposite direction
	
	int RightCorrespondance=-1;

	for (int RightOffset=0;RightOffset<LoopSize;RightOffset++){
		double SumOfSquaredDistances=0.0;
		for (i=0; i<LoopSize; i++){
			double Dist = ~(*NeuronLoop1[LoopSize-i-1] - *NeuronLoop2[(i+RightOffset) % LoopSize]);
			SumOfSquaredDistances += Dist*Dist;
		}

		if (BestCorrespondanceSum > SumOfSquaredDistances){
			BestCorrespondanceSum = SumOfSquaredDistances;
			RightCorrespondance=RightOffset;			
		}

	}

	// delete N1 and all edges and faces eminating from it
	DelNeuronAndCascadeDelete( N1,false);

	// delete N2 and all edges and faces eminating from it
	DelNeuronAndCascadeDelete( N2,false);

	for (i=0; i<LoopSize; i++){
		Neuron *N1,*N2,*N3 ,*N4;
		Edge *E12, *E34 , *E13 ,*E24 ,*E14,*E23;
		// decide which direction is best for correspondance and create faces between it
		if (RightCorrespondance >=0){ // RightCorrespondance is -1 if the best correspondance is going left
			 N1 = NeuronLoop1[LoopSize-i-1] ;
			 N2 = NeuronLoop1[(LoopSize-i-2)% LoopSize] ;
			 N3 = NeuronLoop2[(i+RightCorrespondance) % LoopSize];
			 N4 = NeuronLoop2[(i+1+RightCorrespondance) % LoopSize];
		}
		else{ // this means that correspondance is done when going left
			 N1 = NeuronLoop1[i] ;
			 N2 = NeuronLoop1[(i+1)% LoopSize] ;
			 N3 = NeuronLoop2[(i+LeftCorrespondance) % LoopSize];
			 N4 = NeuronLoop2[(i+1+LeftCorrespondance) % LoopSize];
		}

		E12=Edges.FindEdge(N1,N2,true);
		E34=Edges.FindEdge(N3,N4,true);

#ifdef MY_DEBUG
		ASSERT(E12);
		ASSERT(E34);
#endif

		E13=Edges.CreateOrRefreshEdge(N1,N3);
		E24=Edges.CreateOrRefreshEdge(N2,N4);

		// check for minimal diagonal distance between the quadrilaterals
		// create faces with minimal edge size on diagonal
		double Dist14 = ~(*N1 - *N4);
		double Dist23 = ~(*N2 - *N3);
		
		if (Dist14<Dist23){
			E14=Edges.CreateOrRefreshEdge(N1,N4);
			Faces.CreateOrRefreshFace(E12,E24,E14);
			Faces.CreateOrRefreshFace(E34,E13,E14);
		}
		else{
			E23=Edges.CreateOrRefreshEdge(N2,N3);
			Faces.CreateOrRefreshFace(E12,E13,E23);
			Faces.CreateOrRefreshFace(E34,E24,E23);
		}

	}


	delete [] NeuronLoop1;
	delete [] NeuronLoop2;
	return true;
}

int NeuralNet :: DelOldEdgesAndCascadeDelete(int MaxAge, bool DelNeurons){
	// this function deletes the old edges and cascade deletes faces eminating from them
	// if neurons are left with no edges eminating from then due to this delete they are also deleted in a cascading manner
	// the function returns the number of edges deleted in that manner

	int Deletions=0;
	// del the old edges and neurons to get disconnected
	Edge *E = Edges.Head;
	for (int i=0; i<Edges.Size;i++){
		if (E->CreationAge > MaxAge){
			// delete faces connected to old edge
			while( E->FacesStuckHead !=NULL){
				Faces.DelFace(E->FacesStuckHead);
			}
			Neuron *N1=E->Neuron1;
			Neuron *N2=E->Neuron2;
			E=Edges.DelEdge(E);
			Deletions++;
			// delete orphan neurons if requested
			if (DelNeurons){
				if (N1->EdgeCounter==0) 
					DelNeuron(N1);
				if (N2->EdgeCounter==0)
					DelNeuron(N2);
			}
		} 
		else
			E=E->Next;
	} 

	return Deletions;
}


Neuron *NeuralNet :: DelNeuronAndCascadeDelete( Neuron *N , bool MarkBoundary){
	// this function deletes the neuron N and all edges and faces eminating from it
	// if neurons are left with no edges eminating from then due to this delete they are also deleted in a cascading manner
	// the function returns the neuronafter  the deleted neuron in the list

	// now delete all the faces connected to that neuron 
	// if the face is connected to two boundary neurons - delete the third edge as well

	int Deletions=0;

	Face **FaceList = N->FacesBelongingToNeuron();
	int FaceNumber=N->FaceCounter;
	for (int i=0;i<FaceNumber ;i++){
		Face *F=FaceList[i];
		Neuron *N1=F->OtherNeuronInFace(N,false);
		Neuron *N2=F->OtherNeuronInFace(N,true);
		Edge *E;
		E=Edges.FindEdge(N1,N2,true);
		
		Faces.DelFace(F);
		if (E->FaceCounter==0)
			Edges.DelEdge(E);
	}

	delete [] FaceList; // return memory

	// now delete all edges from it and mark boundary
	while (N->EdgeStuckHead != NULL){
		// mark neuron as boundary
		Neuron *It=N->EdgeStuckHead->OtherNeuronInEdge(N);
		if (MarkBoundary)
			It->Tags |= NEURON_ON_BOUNDARY;

		Edges.DelEdge(N->EdgeStuckHead);
		if (It->EdgeCounter==0){
			DelNeuron(It);
			Deletions++;
		}
	}
	
	// finally delete the neuron
	Neuron *NextN=DelNeuron(N);
	Deletions++;

	//return (Deletions);

	return (NextN);
}


int NeuralNet::CheckManifoldType(){
	// the function returns the manifold type
	// 0 means non manifold - otherwise another value is returned

	// loop through the edges:
	Edge *E = Edges.Head;

	bool BoundaryFound=false;

	if (E!=NULL) do{
		if (E->FaceCounter > 2 || E->FaceCounter==0)
			return (NON_MANIFOLD);
		else if (E->FaceCounter==1)
			BoundaryFound=true;
		E=E->Next;
	} while (E!=Edges.Head);

	if (BoundaryFound)
		return MANIFOLD_WITH_BOUNDARY;
	else 
		return EDGE_MANIFOLD;

}


Neuron **NeuralNet::FindCandidatesForEdgeTriangulation (Edge *E, int MaxConnectivityOfEdge , bool FilterExistingFaces){
	// This function returns an array of neurons that are connected by an edge to both ends of edge E
	// Neurons are added to the list only if the connection to the edge E is by edges with MaxConnectivityOfEdge. 
	// if FilterExistingFaces is true then neurons that are connected by a face to the edge are ignored and will not be in the output list
	// the output consists of a NULL terminated array of pointers to Neurons
	// the size allocated for the output array is the max EdgeConnectivity of any neuron belonging to edge E + 1  , this is the maximal size +1 possible for neurons
	
	Neuron *N1=E->Neuron1;
	Neuron *N2=E->Neuron2;

	int MaxSize = min (N1->EdgeCounter , N2->EdgeCounter) +1;
	Neuron **OutputNeurons = new (Neuron (*[MaxSize]));

	for (int i=0; i<MaxSize; i++)
		OutputNeurons[i]=NULL;

	int Index=0;
	Edge *E1 = N1->EdgeStuckHead ;
	while (E1!=NULL){
		if (E1!=E && E1->FaceCounter <= MaxConnectivityOfEdge){
			Neuron *N3 = E1->OtherNeuronInEdge(N1);
			Edge *E2 = Edges.FindEdge(N2,N3);
			if (E2!=NULL && E2->FaceCounter <= MaxConnectivityOfEdge){
				// this means that the neuron is connected to both sides of E
				if ( !FilterExistingFaces || Faces.IsFace(E,E1,E2)==NULL ){
					OutputNeurons[Index++] = N3;
				}
			}
		}
		E1=N1->NextEdgeInEdgeStuck(E1);
	}

	return (OutputNeurons);
}



Edge * Face::FindCommonEdge(Face *F){
	// the function returns the common edge between two faace : this and F
	// it returns NULL if no common edge exists

	if (Edge1 == F->Edge1 ||
		Edge1 == F->Edge2 ||
		Edge1 == F->Edge3 )
		return Edge1;

	if (Edge2 == F->Edge1 ||
		Edge2 == F->Edge2 ||
		Edge2 == F->Edge3 )
		return Edge2;

	if (Edge3 == F->Edge1 ||
		Edge3 == F->Edge2 ||
		Edge3 == F->Edge3 )
		return Edge3;
		
	return NULL;

}


int NeuralNet::MarkNonManifoldMeshNeurons(){
// the fucntion traverses all the neurons and marks the non manifold neurons
// the function returns the number of non manifold neurons

	Neuron *N=Neurons.Head;
	int NonManifoldCounter=0;
	if (Neurons.Head !=NULL) do{
		int IsNotManifold=N->CheckIfNeuronIsManifoldNeuron();
		if (IsNotManifold!=0)
			NonManifoldCounter++;
		N=N->Next;
	} while (N!=Neurons.Head);

	TRACE("%i neurons are non manifold\n",NonManifoldCounter);

	return NonManifoldCounter;
}

int NeuralNet::DeleteNonManifoldNeurons(){
// the function deletes all non manifold neurons and edges and faces connected to them
// it returns the number of deleted neurons
// the fanction assumes that neurons had already been marked

	Neuron *N=Neurons.Head;
	int OldSize=Neurons.Size;
	bool LastIteration=false;
	if (Neurons.Head !=NULL) do{
		int IsNotManifold=N->CheckIfNeuronIsManifoldNeuron();
		// since we are deleting neurons - and dont know howmany we delete we need to mark the last iteration
		LastIteration = N==Neurons.Head->Prev;
//		if (IsNotManifold!=0)
		if (N->NonManifoldNeuron!=0)
			N=DelNeuronAndCascadeDelete(N);
		else
			N=N->Next;
	} while (!LastIteration);

	TRACE("%i neurons were deleted due to non-manifold violations\n",OldSize - Neurons.Size);

	return (OldSize - Neurons.Size);
	
}


int NeuralNet::DeleteNonManifoldEdges(){
// the function deletes all edges that do not have 2 faces connected to them
// if 3 or more faces are connected to an edge, the faces are also deleted
// the function returns the number of edges deleted in that manner.

	Edge *E=Edges.Head;
	int OldSize=Edges.Size;
	bool LastIteration=false;
	do {
		// since we are deleting edges - and dont know howmany we delete we need to mark the last iteration
		LastIteration = E == Edges.Head->Prev;
		if (E->FaceCounter>2 || E->FaceCounter==0){
			// delete faces connected to old edge
			while( E->FacesStuckHead != NULL){
				Face *F=E->FacesStuckHead;
				Edge *E1=F->Edge1;
				Edge *E2=F->Edge2;
				Edge *E3=F->Edge3;
				Faces.DelFace(F);
				if (E1 !=E && E1->FaceCounter==0)
					Edges.DelEdge(E1);
				if (E2 !=E && E2->FaceCounter==0)
					Edges.DelEdge(E2);
				if (E3 !=E && E3->FaceCounter==0)
					Edges.DelEdge(E3);
			}
			// now delete the edge
			E=Edges.DelEdge(E);
		}
		else
			E=E->Next;
	} while (!LastIteration);

	TRACE("Number of non manifold edges deleted is: %i \n",OldSize-Edges.Size);
	return (OldSize-Edges.Size);
}


void NeuralNet::CalcNetNormalsUsingLSQ(int MinNumberOfPoints, PointList &PtLst){
// This function calculates the normals of the neurons using LSQ method
// the function first classifies the points to the neural net
// MinNumberOfPoints Determines the minimum number of sample points to be taken into account when calculating the normal (see CalcNormalUsingLSQ)
// if MinNumberOfPoints=0 then the normal is calculated using the neighbouring eurons

	Neuron *N=Neurons.Head;
	ClassifySamplePointList(PtLst);
	if (Neurons.Head!=NULL) do{
		N->CalcNormalUsingLSQ(MinNumberOfPoints);
		N=N->Next;
	}while (N!=Neurons.Head);
	

}
/* old version of this function
void NeuralNet::FlipFaceAndVertexNormalsForAgreement(void){
	// The Net Vertices are traverset in BFS order from the first neuron in the list for each neuron normals of face and neighboring neurons are set to agree in the direction
	//warning - the results of the function may be ambigous if the mesh in not connected with faces

	// first order the neurons by topological order from the head using BFS using only edges that belong to faces
	CalcTopologicalDistance(Neurons.Head,0,true);

	// now travese the neurons and reset all their faces
	Neuron *N=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		Face **FaceList;
		FaceList = N->FacesBelongingToNeuron();
		for (int i=0; i<N->FaceCounter ; i++){
			Face *F=FaceList[i];
			F->SetNormalDirectionsToAgreeWithNormalAtVertex(N);
		}
		if (N->FaceCounter != 0)
			delete [] FaceList;
		N = N->Next;
	}while (N!=Neurons.Head);
	
}

*/

void NeuralNet::FlipFaceAndVertexNormalsForAgreement(bool FlipNormalDirections){
	// The faces of the net are traverset so that agreement of their orientations is established
	// if FlipNormalDirections is true then the normals will be flipped even of all the faces are consistant in their orientation this way the normals will be flipped since back and front will be flipped in each face

	// first delete markings
	Face *F=Faces.Head;
	for (int i=0; i<Faces.Size; i++){
		F->Marked=false;
		F=F->Next ;
	}


	if (FlipNormalDirections)
		F->FlipDirection=!F->FlipDirection;

	// now traverse the faces and for each unmarked face recursivly direct the faces.
	// mark patch numbers.
	int PatchCounter=0;
	F=Faces.Head;
	for (i=0; i<Faces.Size; i++){
		if (F->Marked == false){
			PatchCounter++;
			TraverseFacesRecursivlyToEstablishOrientation(F,PatchCounter);
		}
		F=F->Next ;
	}

	// now check if digagreements have been formed and mark incosistencies
	Edge *E=Edges.Head;
	int InconsistantEdgeCounter=0;
	for (i=0; i<Edges.Size; i++){
		E->NotConsistantInDirection = (E->CheckFaceDirectionConsistency()==1);
		if (E->NotConsistantInDirection)
			InconsistantEdgeCounter++;
		E=E->Next ;
	}	

	// now traverse the vertices and flip their normals to agree with the face orientation
	int InconsistantVertexCounter=0;
	Neuron *N=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		N->TriDist = 0.0;
		Face **FaceList;
		FaceList = N->FacesBelongingToNeuron();
		for (int i=0; i<N->FaceCounter ; i++){
			Face *F=FaceList[i];
			bool Flipped=F->SetVertexNormalDirectionsToAgreeWithFaceOrientation(N);
			if (i!=0 && Flipped){
				InconsistantVertexCounter++;
				N->TriDist = -1.0;
				break;
			}
		}
		if (N->FaceCounter != 0)
			delete [] FaceList;
		N = N->Next;
	}while (N!=Neurons.Head);
	
	TRACE ("Flip Face normals reports %i inconsistant edges and %i inconsistant vertex normals",InconsistantEdgeCounter,InconsistantVertexCounter);
}

void NeuralNet::TraverseFacesRecursivlyToEstablishOrientation (Face *SeedFace , int PatchNumber){
	SeedFace->Marked = true;
	SeedFace->PatchNumber = PatchNumber;
	Face *F = SeedFace->Edge1->OtherFaceOfEdge(SeedFace);
	if (F!=NULL && F->Marked!=true){
		int Consistency=SeedFace->Edge1->CheckFaceDirectionConsistency();
		if (Consistency!=0)
			F->FlipDirection = !F->FlipDirection;
		TraverseFacesRecursivlyToEstablishOrientation(F,PatchNumber);
        
	}
	
	F = SeedFace->Edge2->OtherFaceOfEdge(SeedFace);
	if (F!=NULL && F->Marked!=true){
		int Consistency=SeedFace->Edge2->CheckFaceDirectionConsistency();
		if (Consistency!=0)
			F->FlipDirection = !F->FlipDirection;
		TraverseFacesRecursivlyToEstablishOrientation(F,PatchNumber);
        
	}

        	
	F = SeedFace->Edge3->OtherFaceOfEdge(SeedFace);
	if (F!=NULL && F->Marked!=true){
		int Consistency=SeedFace->Edge3->CheckFaceDirectionConsistency();
		if (Consistency!=0)
			F->FlipDirection = !F->FlipDirection;
		TraverseFacesRecursivlyToEstablishOrientation(F,PatchNumber);
        
	}


	
	return;
}

void NeuralNet::MergeSurfacePatches(Edge *ConnectionEdge){
// change the orienation of surface patches to agree on face directions, the connecting edge between surface patches is given as an input, the patch number on the newest patch will be the minimum patch number of the two patches connected this way

	Face *F1 = ConnectionEdge->FacesStuckHead;
	Face *F2 = ConnectionEdge->NextFaceInFaceStuck(F1);

	if (F1==NULL || F2==NULL)
		return ; // this means there are no two faces to the edge
	bool PatchNeedsFlip = (ConnectionEdge->CheckFaceDirectionConsistency() == 1);
	int ChangeFrom;
	int ChangeTo;

	if (F1->PatchNumber > F2->PatchNumber){
		ChangeFrom=F1->PatchNumber;
		ChangeTo=F2->PatchNumber;
	} else if (F2->PatchNumber > F1->PatchNumber){
		ChangeFrom=F2->PatchNumber;
		ChangeTo=F1->PatchNumber;
	}
	else{ // normally we should not get here with unflipped faces
		ASSERT (!PatchNeedsFlip);
	}

	Face *F=Faces.Head;
	for (int i=0; i<Faces.Size; i++){
		if (F->PatchNumber == ChangeFrom){
			if (PatchNeedsFlip)
				F->FlipDirection = !F->FlipDirection;
			F->PatchNumber=ChangeTo;	
		}
		F=F->Next;
	}

}

int NeuralNet::ResetNeuronPositions(int ResetMethod, PointList &PtLst){
	//the function resets the neuron positions using the method dictated
	// methods are of typed declared in NeuronResetMethodsTypes


	int RetVal=0;
	switch (ResetMethod){

	case RESET_METHOD_RANDOM_IN_BOUNDING_BOX:{
			Neuron *N=Neurons.Head;
			for (int i=0;i<Neurons.Size;i++) {
				MyPoint TempPt(PtLst.MinVals,PtLst.MaxVals); 
				N->MyPoint::operator= (TempPt);	
				N=N->Next;
			}

			return RetVal;
		}
		break;
	case RESET_METHOD_RANDOM_FROM_SAMPLED_POINTS:{
			Neuron *N=Neurons.Head;
			for (int i=0;i<Neurons.Size;i++) {
				MyPoint TempPt = *(PtLst.Array [i%PtLst.Size]);
				N->MyPoint::operator= (TempPt);	
				N=N->Next;
			}
			return RetVal;
		}
		break;
	case RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_NO_DELETIONS:
	case RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_WITH_DELETIONS:
		{
			ClassifySamplePointList (PtLst);
			Neuron *N=Neurons.Head;
			for (int i=0;i<Neurons.Size;i++) {
				SamplePoint *Pt=N->FindNearestClassifiedPointToNeuron(true);
				if (ResetMethod == RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_WITH_DELETIONS &&
					Pt==NULL){
						N=DelNeuron(N);
						RetVal++;
				}
				else
					N=N->Next;
			}
			TRACE ("ResetNeuronPositions reports that %i neurons were deleted\n", RetVal);
		}
		return RetVal;
		break;
	default:
		return -1;
	} 

	return 0;
}
