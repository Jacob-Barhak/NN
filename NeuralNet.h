#ifndef NEURAL_NET_H
#define NEURAL_NET_H
#define MY_EPS  1e-5
#define M_PI (4*atan(1.0))
//spesific classes
#include "stdafx.h"

//#include "irit_sm.h"

using namespace std ;

typedef double RealType;


#define MAX_ARRAY_SIZE 20000
#define MAX_BUFFER_SIZE 1024


// for debug checks of memory leaks
#define new DEBUG_NEW
// used for trace output
//#define MY_DEBUG





// these STL types are no longer used in the program - remove them after rewriting the project *** JAC
//typedef list< class SamplePoint> POINT_LIST;
//typedef list< class Neuron> NEURON_LIST;
//typedef list< class Edge> EDGE_LIST;

class SamplePoint;
class Neuron;
class Edge;


// calculate exponential decay between init and final values 
// when i is the iterator whose maximal value is n
#define EXP_DECAY(init,final,i,n)  init*((double)pow((double)final / (double)init,(double)i/n))


// this is a helper macro to find position in rectangular topology networks
#define RECTNN(i,j) (NeuronArray[(i)*(NetworkSizeV)+(j)])

#define MAX_DOUBLE 1.7e308

typedef enum NeuronTypesTag{ 
	NEURON_NO_TYPE = 0x0,
	NEURON_CAN_NOT_WIN = 0x1,
	NEURON_CAN_NOT_MOVE = 0x2,
	NEURON_EDGES_REMOVED = 0x4,
	NEURON_ON_BOUNDARY = 0x8
} NeuronTypes;


typedef enum EdgeTypesTag{ 
	EDGE_NO_TYPE = 0x0,
	EDGE_U_DIR = 0x1,
	EDGE_V_DIR = 0x2,
	EDGE_FACES_REMOVED = 0x4,
	EDGE_ADDED_FOR_TRIANGULATION = 0x08,
	EDGE_ON_BOUNDARY = 0x10,
	EDGE_ON_NON_MANIFOLD =0x20,
	EDGE_ON_INSIDE =0x40,
	EDGE_CREATED_TO_CLOSE_LOOP =0x80
} EdgeTypes;


typedef enum FaceTypesTag{ 
	FACE_NO_TYPE = 0x0,
	FACE_CREATED_TO_CLOSE_LOOP = 0x1
} FaceTypes;

typedef enum NeuralNetworkTypesTag{
	NN_NOT_INITIALIZED=0x8000,
	NN_NO_TOPOLOGY=0x01,
	NN_HAS_TOPOLOGY=0x2,
	NN_RECTANGULAR_TOPOLOGY=0x4,
	NN_CYCLIC_U_DIR=0x8,
	NN_CYCLIC_V_DIR=0x10,
	NN_CAN_GROW_NEURONS=0x80,
	NN_CAN_GROW_EDGES=0x100
} NeuralNetworkTypes;



typedef enum SamplePointTypeTag{ 
	POINT_NO_TYPE = 0x0,
	POINT_OUTSIDE = 0x1,
	POINT_INSIDE = 0x2
} SamplePointType;


typedef enum MeshTypeTag{ 
	NON_MANIFOLD = 0x0,
	MANIFOLD_WITH_BOUNDARY = 0x1,
	EDGE_MANIFOLD = 0x2
} MeshType;



typedef enum NeuronResetMethodsTypesTag{ 
	RESET_METHOD_RANDOM_IN_BOUNDING_BOX = 0x0,
	RESET_METHOD_RANDOM_FROM_SAMPLED_POINTS,
	RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_NO_DELETIONS,
	RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_WITH_DELETIONS
} NeuronResetMethodsTypes;


class MyPoint {
public:
	RealType Pos[3]; //3 initial values are x,y,z
	MyPoint operator + (const MyPoint &P) const;
	MyPoint operator - (const MyPoint &P) const;
	MyPoint operator - () const;
	MyPoint operator * (RealType alpha) const;
	RealType operator * (const MyPoint &P) const;
	MyPoint operator ^ (RealType alpha) const;
	MyPoint operator ^ (const MyPoint &P) const;
	bool operator == (const MyPoint &P) const;
	//MyPoint operator = (const MyPoint Src);
	RealType operator ~ () const; //size operator
	MyPoint();	
	MyPoint(const RealType PtInit[3] );	
	MyPoint(const MyPoint &MinVals , const MyPoint &MaxVals); //constructor by random point
	bool IsFinite();
	void MaxValues(const MyPoint &Point);
	void MinValues(const MyPoint &Point);
	
	friend ostream& operator<< ( ostream& os,  MyPoint &Pt );
	friend istream& operator>> ( istream& is,  MyPoint &Pt );

	void Draw(RealType PointSize);

//	MyPoint (const &MyPoint);
//	MyPoint();
}; 

class SamplePoint: public MyPoint{
public:
	int Tag;  // combination of SamplePointType
	RealType Weight;
	Neuron *Classification; // nearset neuron  is stored here after classification
	SamplePoint *Next; // used for classifications stack

	SamplePoint();
	SamplePoint(const MyPoint &Point, RealType InitWeight=1.0);


	friend ostream& operator << ( ostream& os,  SamplePoint &Pt );
	friend istream& operator >> ( istream& is,  SamplePoint &Pt );
};

class Edge{

public:
	int EdgeId;
	int Tag;   // combination of EdgeTypes
	Neuron *Neuron1, *Neuron2;
	int CreationAge;
	int FaceCounter;
	int FanNumber;
	bool NotConsistantInDirection;

	// double connected list
	class Edge *Prev;
	class Edge *Next;

	// list of edges in neuron list
	class Edge *NextInN1;

	class Edge *NextInN2;

	// pointers to both faces connected to this edge
	//class Face *F1;
	//class Face *F2;

	class Face *FacesStuckHead;

	class Face *NextFaceInFaceStuck(Face *F);

//	Edge();
	Edge(Neuron *N1 , Neuron *N2, int CurrentIteration=0);
//	~Edge();
	bool IsEdgeOfNeuron(Neuron *N);
	bool IsEdgeBetweenNeurons(Neuron *N1,Neuron *N2);
	Neuron *OtherNeuronInEdge(Neuron *N);
	bool CheckFoldOverEdge (Neuron *N , double FoldCosAngle = 0.0);

	void Draw();
	friend ostream& operator << ( ostream& os, Edge &E );
	friend istream& operator >> ( istream& is, Edge &E );

	int CheckFaceDirectionConsistency(void);
	// returns a pointer to the other face of the edge. if more then one face is connected to the edge or if less than two faces are connected to the face the function returns null
	Face * OtherFaceOfEdge(Face *F);
};


class Face{

public:
	Edge * FindCommonEdge(Face *F);
	double CalcFilterCriteria();
	bool IsNeuronInProjFace(Neuron &N);
	int FaceId;
	int Tag;   // combination of FaceTypes
	Edge *Edge1, *Edge2, *Edge3;
	int CreationAge;
	int FanNumber;
	bool FlipDirection;
	bool Marked;
	int PatchNumber;


	// double connected list
	class Face *Prev;
	class Face *Next;

	class Face *NextFaceInE1;
	class Face *NextFaceInE2;
	class Face *NextFaceInE3;


	Face(Edge *E1 , Edge *E2, Edge *E3, int CurrentIteration=0);
	bool IsFaceOfNeuron(Neuron *N);
	bool IsFaceOfEdge(Edge *E);
	void GetFaceNeurons(Neuron **N1,Neuron **N2,Neuron **N3);
	bool IsFaceBetweenNeurons(Neuron *N1,Neuron *N2,Neuron *N3);
	bool IsFaceBetweenEdges(Edge *E1,Edge *E2,Edge *E3);
	Neuron *OtherNeuronInFace(Neuron *N, bool Other=false);
	Neuron *OtherNeuronInFace(Neuron *N1,Neuron *N2);
	Edge *OtherEdgeInFace(Edge *E, bool Other=false);
	Edge *OtherEdgeInFace(Edge *E1,Edge *E2);
	Edge *OtherEdgeInFace(Edge *E, Neuron *N);
	bool CheckIfProjNeuronIsInProjFace(Neuron *N);
	Face *NextManifoldFace(Edge *E);

	double FilterCriteria;


	void Draw();
	friend ostream& operator << ( ostream& os, Face &E );
	friend istream& operator >> ( istream& is, Face &E );
	
	friend ostream& OutputToVrmlFormat ( ostream& os , Face &F);

	bool SetVertexNormalDirectionsToAgreeWithFaceOrientation(Neuron * N);
};



extern bool StartedTriangulation;

class Neuron : public MyPoint {

	public:
	MyPoint Normal; // the normal to the surface
	MyPoint W1,W2; // the eigenvectors used by PCA
	MyPoint Proj; // the projection along the normal
	double Angle; // projection angle
	double TriDist; // saves the triangulation distace - negative number 
	bool TriNew;
	int NeuronId;
	int NeuronIdU;  // used only on rectangular arrays
	int NeuronIdV;	// used only on rectangular arrays
	int Tags;
	int WinCounter;
	int LastWinningAge;
	int LastUpdateAge;
	int EdgeCounter;
	int FaceCounter;
	int PatchClassification;
	int LocationIndexX;
	int LocationIndexY;
	int LocationIndexZ;
	SamplePoint *StuckHead;
	int NonManifoldNeuron;

	// the head of the doubly connected edge list to this neuron
	class Edge *EdgeStuckHead; 

	RealType Error;
	//EDGE_LIST Links;
	RealType PointDist;
	// double connected list
	class Neuron *Prev;
	class Neuron *Next;
	
	MyPoint CalcNormalUsing3Points(Neuron *Neuron2, Neuron *Neuron3 , MyPoint *E1=NULL, MyPoint *E2=NULL);
	MyPoint CalcNormalUsingLSQ(int MinNumberOfPoints);
	void UpdateNormalUsingPCA(MyPoint &Pt, double Eta);
	void CalcNormalUsingSemiMinimalBoundingCone();

	Neuron(); 

	void Draw (RealType PointSize, RealType NormalSize);
	Neuron(const MyPoint &Point) ;
	void PushClassifiedSampledPointToStuck(SamplePoint *Pt);
	void EmptyStuck();

	Neuron *NextInLoactionStuck;
	Edge *NextEdgeInEdgeStuck(Edge *E);
	
	void ProjectViaNormal(MyPoint &E1, MyPoint &E2 , MyPoint &Zero);
	void CalcProjectionPlane(MyPoint &V1 , MyPoint &E1 , MyPoint &E2);
	Face **FacesBelongingToNeuron();
	
	double CalcTriangulationConfidence(bool UpdatePointDist=true, int MaxConnectivityOfEdge=2);
	double CalcTriangulationConfidenceFor3Pt(Neuron *N1 , Neuron *N2);

	int MarkDisconnectedTriangleFans();
	int CheckIfNeuronIsManifoldNeuron();
	

	bool operator <	(Neuron &N);
	RealType CalcDistFromSamplePoint( SamplePoint Pt, Edge *E=NULL);
	void ModifyPosBySamplePoint(RealType alpha, SamplePoint Pt);
	friend ostream& operator << ( ostream& os, Neuron &N );
	friend istream& operator >> ( istream& is, Neuron &N );
	SamplePoint *FindNearestClassifiedPointToNeuron(bool ChangeNeuronPositionToNearestPoint = false);
};


class NeuronList{
public:
	Neuron *Head;
	int Size;
	NeuronList();
	~NeuronList();
	Neuron *PushFront( Neuron &N, bool AllocateMemory=true);
	Neuron *PopFront(bool DeleteMemory=true);
	Neuron *PushBack( Neuron &N, bool AllocateMemory=true);
	Neuron *PopBack(bool DeleteMemory=true);
	Neuron *Remove(Neuron *N, bool DeleteMemory=true);
	Neuron *Add(Neuron &N, Neuron *BeforeN, bool AllocateMemory=true);

	void Swap ( Neuron *N1 , Neuron *N2);
	void Sort(int k , Neuron *SortHead = NULL, int SizeToEnd=0);

	friend ostream& operator << ( ostream& os, NeuronList &Nl);
	friend istream& operator >> ( istream& is, NeuronList &Nl);
};



class PointList{
public:
	
	SamplePoint **Array; // the array in which the list resides in

	int Size;
	MyPoint MinVals;
	MyPoint MaxVals;
	int Reorder(unsigned int RandomSeed=0);
	int CalcBoundingBox();
	friend ostream& operator << ( ostream& os, PointList &PtLst );
	friend istream& operator >> ( istream& is, PointList &PtLst );

	bool RecalcForDisplay;
	
	void PointList::ReallocateMemory(int NewSize);
	void ResetPointWeight(double NewWeight=1.0);

	PointList();
	~PointList();
	void InitDispList();

	void DrawList(RealType PointSize, int ChunkSize, int BaseIteration);
	void Draw();

private:
	GLuint DispListNumber;

};


class EdgeList{

public:
	EdgeList();
	~EdgeList();
	
	Edge *Head;
	int Size;

	void InitDispList();
	void DrawList(int DrawEdgeNew = 0);
	void Draw();
	int DeleteOld(int Age, bool MarkNeurons=false ,bool DeleteOnlyDisconnectedEdges=false);
	int Age( Neuron *N=NULL , bool AgeOnlyDisconnectedEdges=false);
	Edge *CreateOrRefreshEdge(Neuron *N1, Neuron *N2 , int EdgeType=EDGE_NO_TYPE);
	Edge *FindEdge(Neuron *N1, Neuron *N2 , bool UseOnlyConnectedEdges=false);
	//bool DelEdge(Edge *E);  // delete the edge - withouth deleting the neurons 
	Edge *DelEdge(Neuron *N1, Neuron *N2); // delete the edge - without deleting the neurons 
	Edge *DelEdge(Edge *E , bool MarkNeurons=false);
	int DelEdges(Neuron *N);
	void MaskConnectedNeurons(int NeuronMask , int EdgeMask);

	friend ostream& operator << ( ostream& os, EdgeList &El);
	friend istream& operator >> ( istream& is, EdgeList &El);


private:
	int DispListNumber;

};


class FaceList{

public:
	FaceList();
	~FaceList();
	
	Face *Head;
	int Size;

	void InitDispList();
	void DrawList();
	void Draw();
	int DeleteOld(int Age, bool MarkNeurons=false);
	int Age( Neuron *N);
	Face *CreateOrRefreshFace(Edge *E1, Edge *E2 ,Edge *E3 , int FaceType=FACE_NO_TYPE);
	Face *DelFace(Neuron *N1, Neuron *N2, Neuron *N3); // delete the edge - without deleting the neurons 
	Face *DelFace(Edge *E1, Edge *E2, Edge *E3); // delete the edge - without deleting the neurons 
	Face *DelFace(Face *F , bool MarkNeurons=false);
	Face *IsFace(Neuron *N1,Neuron *N2,Neuron *N3);
	Face *IsFace(Edge *E1,Edge *E2,Edge *E3);
	int CountFacesForEdge(Edge *E);
	void MaskConnectedNeurons(int Mask);
	// old stuff not used anymore 
	// Face **FindFacesByEdge( Face **ListOfFaces, Edge *E);
	
	
	friend ostream& operator << ( ostream& os, FaceList &El);
	friend istream& operator >> ( istream& is, FaceList &El);


private:
	int DispListNumber;

};



#define NEURON_LOCATION(i,j,k)  LocationArray[((i)*LocationSizeY+(j))*LocationSizeZ+(k)]
#define NEURON_LOCATION_COUNT(i,j,k)  LocationArrayCount[((i)*LocationSizeY+(j))*LocationSizeZ+(k)]
#define LOCATION_ARRAY_SIZE_X 20
#define LOCATION_ARRAY_SIZE_Y 20
#define LOCATION_ARRAY_SIZE_Z 20

class NeuralNet{
public:
	bool LoadNetFromFile (LPCTSTR FileName);
	int DeleteDisconnectedNeurons();
	int DeleteNeuronsByEdgeMask(int EdgeTags);
	// properties
	
	int Tags; // combination of NeuralNetworkTypes
	NeuronList Neurons;
	Neuron **NeuronArray; // used only for rectangular topology SOM

	// used to locate array
	int LocationSizeX,LocationSizeY,LocationSizeZ;
	double LocationLowX,LocationLowY,LocationLowZ,LocationHighX,LocationHighY,LocationHighZ ,LocationDeltaX,LocationDeltaY,LocationDeltaZ;
	Neuron **LocationArray;
	int *LocationArrayCount;

	EdgeList Edges;
	FaceList Faces;
	bool Initialized;
	int NetworkSizeU; // holds the size in the number of neurons in U direction for rect topology or max id for any other topology
	int NetworkSizeV; // holds the size in the number of neurons in U direction for rect topology or 0 for any other topology
	int NeuronIdMaxCounter;

	// methods
	NeuralNet();
	~NeuralNet();

	void InitNeuralNet(int NumberOfNeuronsUDir,int NumberOfNeuronsVDir, PointList &PtLst,int NetType, LPCTSTR FileName = NULL); // initialize with topology defined by net type
	bool CheckNetworkTypeValidity(int NewNetType);
	bool ChangeNetType(int NewNetType, PointList &PtLst);
	void InitDispList();
	
	//	NEURON_LIST::iterator LocateIterator(Neuron *N);
	
	void NeuronSortByWinners(SamplePoint &Pt,int k, bool ClassifySamplePoint=true , Edge *E=NULL);

	int CalcTopologicalDistance (Neuron *N,int k,bool UseOnlyConnectedEdges=false); //calculate the toplogical distance of all neurons using the new data and sort them by it, distances up to k are calculated
	void RenumberNeurons();
	
	void TrainNeuralGasBySinglePoint (SamplePoint &Pt, RealType Eps, RealType Lambda ,int CurrentIteration , bool UseOnlyNeuronsWithFacesForNormalCalculation=true);
	int TrainNeuralGasByPointList (PointList &PtLst, int BaseIteration,int RunLength, RealType EpsInit, RealType EpsFinal, RealType LambdaInit, RealType LambdaFinal, int MaximalEdgeAgeInit,int MaximalEdgeAgeFinal);

	bool TrainSelfOrgenizingFeatureMapsBySinglePoint (SamplePoint &Pt, RealType Eps, RealType Sigma, RealType Radius,int CurrentIteration, bool TrainBoundaryOnly = false , double SamplePointIncreaseWeight =0.0 , bool UseClassification = false , bool UseCorrectTopologicalDistanceForRectangularTopology = false);
	int TrainSelfOrgenizingFeatureMapsByPointList (PointList &PtLst, int BaseIteration,int RunLength, RealType EpsInit, RealType EpsFinal, RealType SigmaInit, RealType SigmaFinal, RealType RadiusInit, RealType RadiusFinal, double LambdaGrowth,  int TrainBoundaryOnlyCycleIterations=0, double TrainBoundaryOnlySamplePointWeight=0.0 , int TrainBoundaryOnlyIterations=0, bool UseClassification = false);

	void AddGrowingGridRowColumn(bool BoundaryExpandsOnlyOutwards=false);

	void TrainGrowingNeuralGasBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration,bool UseOnlyNeuronsWithFacesForNormalCalculation=true);
	int TrainGrowingNeuralGasByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LamdgaGrowth);
	void AddGrowingNeuralGasNeuron(RealType Alpha, PointList &PtLst);
	void DecreaseGrowingNeuralGasError(RealType Betta);

	void TrainGrowingNeuralFacesBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration);
	int TrainGrowingNeuralFacesByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LamdgaGrowth);
	void AddGrowingNeuralFacesNeuron(RealType Alpha, PointList &PtLst);
	void DecreaseGrowingNeuralFacesError(RealType Betta);


	void TrainGrowingCellStructureBySinglePoint (SamplePoint &Pt, RealType EpsB, RealType EpsN, int AgeMax, int CurrentIteration);
	int TrainGrowingCellStructureByPointList (PointList &PtLst, int BaseIteration, int RunLength, RealType EpsB, RealType EpsN, RealType Alpha, RealType Betta, int AgeMax , int LamdgaGrowth);
	Neuron *AddGrowingCellStructureNeuron(RealType Alpha, PointList &PtLst);
	void DecreaseGrowingCellStructureError(RealType Betta);

	int DeleteOldNeuronsInGrowingCellStructure (int CurrentIteration,int AgeMax );
	int DelOldEdgesAndCascadeDelete( int MaxAge, bool DelNeurons);

	int MarkNonManifoldMeshNeurons();
	int DeleteNonManifoldNeurons();
	void CalcNetNormalsUsingLSQ(int MinNumberOfPoints,PointList &PtLst);


	// edge topological operation
	Neuron *EdgeSplit(Edge *E);
	Edge *EdgeSwap(Edge *E);
	Neuron *EdgeCollapse(Edge *E);

	bool MergeCollision(Neuron *N1, Neuron *N2);

	int CheckManifoldType();

	bool CheckTwoProjLinesIntersect( Neuron *L1N1 , Neuron *L1N2 , Neuron *L2N1 ,Neuron *L2N2);

	Neuron * DelNeuronAndCascadeDelete( Neuron *N , bool MarkBoundary = true);
	int DeleteOverlappingFaces(Neuron *Nwin,int FaceSeedNumbers);

	bool CheckItersectionForPruning(Neuron *NCenter, Neuron *N2 , int NeighbourNumber , Neuron **NeighboursArray);
	bool CheckItersectionForPruning(Neuron *NCenter, Neuron *N2 , int Neighbours);
	bool CheckFaceOclusionForPruning(Neuron *NCenter, Neuron *N);
	bool CheckIfNeuronIsInFace(Neuron *N , Face *F);

	Neuron *AddNeuron(); // adds a neuron to the list, gives it and ID and returns its pointer
	Neuron *AddNeuron(MyPoint &InitPt); // adds a neuron with a spesified position to the list, gives it and ID and returns and returns it's pointer
	void AddOrRemoveNeuronToLocationArray(Neuron *N,bool Remove=false);
	Neuron *DelNeuron(Neuron *N);

	void ClassifySamplePointList(PointList &PtLst);
	RealType ChangeWeightOfOusideBorderPoint (SamplePoint &Pt, double NewWeight);

	bool IsBoundaryNeuron(Neuron *N);


	void CloseBoundary();
	int DetectLoop (Neuron **NeuronList, Edge **EdgeList, Neuron *NeuronInLoop);
	void DelOrphanEdgesAndNeurons();
//	bool DelSimilarFacesForEdgeAndNeuron(Edge *E , Neuron *N , Face **FaceToDelete);
	Face *FindBestFaceForManifold(Edge *E , Neuron *N);


	void TriangulateNet(PointList &PtLst, double NormalAgreementThreshold=-1.0);
	void DeleteNonManifoldFaces(double DeleteFaceCorrespondaceCriterion = 2.0);
	int DeleteNonManifoldFaces(bool UseNormalProjection=true);   //(double MinCosAngleBetweenFaces){
	bool DeleteNonManifoldFaces(Face *F ,Edge *E , double CosMaxAngleBetweenFacesToBeSimilar=2.0, bool CheckOnly=false);
	bool DeleteSingularNeuronFaces(Face *F, Neuron *N , double CosMaxAngleWithFaceNormalBeforeFilter = 0.5, bool CheckOnly=false);
	int DeleteNonManifoldEdges();

//	void ClassifyEdges(); old function not needed anymore
	Neuron **FindCandidatesForEdgeTriangulation (Edge *E, int MaxConnectivityOfEdge , bool FilterExistingFaces);

	


	void DrawList(RealType PointSize, RealType NormalSize);
	void Draw();

	ostream& OutputRectangularTopologyFacesToVRML(ostream& os);
	ostream& OutputRectangularTopologyBoundaryToVRML(ostream& os);
	friend ostream& operator << ( ostream& os, NeuralNet &NNet );
	friend istream& operator >> ( istream& is, NeuralNet &NNet );
	friend ostream& OutputToIritFormat(ostream& os, NeuralNet &NNet );
	friend ostream& OutputToVrmlFormat(ostream& os, NeuralNet &NNet );

private:
	GLuint DispListNumber;


public:
	void FlipFaceAndVertexNormalsForAgreement(bool FlipNormalDirections = true);
	void TraverseFacesRecursivlyToEstablishOrientation (Face *SeedFace, int PatchNumber);

	void MergeSurfacePatches(Edge *ConnectionEdge);
	int ResetNeuronPositions(int ResetMethod , PointList &PtLst);
};


ostream& operator << ( ostream& os,  MyPoint &Pt );
istream& operator >> ( istream& is,  MyPoint &Pt );

ostream& operator << ( ostream& os,  SamplePoint &Pt );
istream& operator >> ( istream& is,  SamplePoint &Pt );

ostream& operator << ( ostream& os, Neuron &N );
istream& operator >> ( istream& is, Neuron &N );

ostream& operator << ( ostream& os, PointList &PtLst );
istream& operator >> ( istream& is, PointList &PtLst );


ostream& operator << ( ostream& os, NeuralNet &NNet );
istream& operator >> ( istream& is, NeuralNet &NNet );

ostream& OutputToVrmlFormat( ostream& os , Face &F);

ostream& OutputToIritFormat(ostream& os, NeuralNet &NNet );
ostream& OutputToVrmlFormat(ostream& os, NeuralNet &NNet );
ostream& OutputToVrmlFormat ( ostream& os , Neuron *N1 ,Neuron *N2 ,Neuron *N3 ,Neuron *N4=NULL);

ostream& operator << ( ostream& os, Edge &E );
istream& operator >> ( istream& is, Edge &E );

int MyGLErrorMessage(char *ErrorMessage);
void MyFatalError(char *ErrorMessage);

#endif //NEURAL_NET_H

