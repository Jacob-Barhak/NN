#include "stdafx.h"
#include "NeuralNet.h"

RealType Neuron::CalcDistFromSamplePoint(SamplePoint Pt, Edge *E){
	// calculates the distance of the Neuron from the given sample point
	// stores the distance in the neuron object and returns it
	if (E==NULL)
		PointDist = ~((*this) - Pt);
	else{
		MyPoint Dir = *E->Neuron2 - *E->Neuron1;
		MyPoint Norm1 = (Pt - *E->Neuron1)^Dir;
		MyPoint Norm2 = (*this - *E->Neuron1)^Dir;

		if ((Norm1*Norm2)>0.0) // this means that the normals are in the same direction of the sampled point - calc distance
			PointDist = ~((*this) - Pt);
		else 
			PointDist = MAX_DOUBLE;

	}
	return(PointDist);
}



void Neuron::ModifyPosBySamplePoint(RealType Alpha, SamplePoint Pt){
	//updates the position of the neuron by the sample point (weight is not checked)
	// changes are made only if the result is a finite number

	MyPoint TempPt;
	TempPt = Pt - (*this);
	TempPt = (TempPt*Alpha)+(*this) ;
	if ( TempPt.IsFinite() )  // only if the result is finite then changes should be made
		this->MyPoint::operator = (TempPt);
}



Neuron::Neuron(){
	Error=0.0;
	TriDist=0;
	TriNew=false;
	PointDist=0.0;
	//mark neuron ID's as -1 (un initialized)
	NeuronId=-1;
	NeuronIdU=-1;
	NeuronIdV=-1;
	Tags = NEURON_NO_TYPE;
	WinCounter=0;
	LastUpdateAge=0;
	LastWinningAge=0;
	EdgeCounter=0;
	FaceCounter=0;
	// init normal randomy
	
	RealType Zero[3] = {0.0,0.0,0.0};
	RealType One[3] = {1.0,1.0,1.0};
	MyPoint PZero(Zero);
	MyPoint POne(One);
	MyPoint NormalInit(PZero,POne);
	Normal = NormalInit;
	MyPoint W1Init(PZero,POne);
	MyPoint W2Init(PZero,POne);
	W1=W1Init;
	W2=W2Init;
	Next=NULL;
	Prev=NULL;
	StuckHead=NULL;
	EdgeStuckHead=NULL;
	NextInLoactionStuck=NULL;
	LocationIndexX=-1;
	LocationIndexY=-1;
	LocationIndexZ=-1;
	NonManifoldNeuron=0;
}


Neuron::Neuron(const MyPoint &Point): MyPoint (Point){
	Neuron();
}


bool Neuron::operator < (Neuron &N) {
	return (this->PointDist < N.PointDist);

}


ostream& operator << ( ostream& os, Neuron &N ){
	
	os << "N " << N.NeuronId;
	os << " " << (MyPoint) (N);
	os << " " << N.NeuronIdU;  
	os << " " << N.NeuronIdV;	// used only on rectangular arrays
	os << " " << N.Tags;
	os << " " << N.WinCounter;
	os << " " << N.LastWinningAge;
	os << " " << N.LastUpdateAge;
	os << " " << N.EdgeCounter;
	os << " " << N.Error; 
	os << " " << N.PointDist; // not really needed - but it is for here anyway
	os <<"\n";

	return os;
}

istream& operator >> ( istream& is, Neuron &N ){

		
	//first read the character

	char c;
	MyPoint Pt;
	is>>ws;
	c=is.peek();
	if  (c == 'N'){ 
		is >> c >> Pt; // read chracter and point
		N=Pt;
	}
	//else error message or setting is to bad should be here...
		

	return is;
}


void Neuron::ProjectViaNormal(MyPoint &E1, MyPoint &E2, MyPoint &Zero){

	// this function chages the projection, using the Zero direction ( the place of the neuron to which we are refering as the initial position.
	Proj.Pos[0]= E1 * ((*this)-Zero);
	Proj.Pos[1]= E2 * ((*this)-Zero);
	Proj.Pos[2]= 0.0;

	if (fabs(Proj.Pos[0]) < MY_EPS) 
		Proj.Pos[0]=0.0;
	if (fabs(Proj.Pos[1]) < MY_EPS) 
		Proj.Pos[1]=0.0;

	
	Angle= atan2(Proj.Pos[1] , Proj.Pos[0]) ;
	if (Angle < 0.0) 
		Angle += M_PI*2;
	
	//Proj.Pos[2]= Normal * (*this);

}



void Neuron::CalcNormalUsingSemiMinimalBoundingCone(){
// this function calculates the vertex normal using a semi minimal bounding cone 
//	of the edges eminating from the neuron
// the method is taken from "a fast and efficient projection-based approach for surface reconstruction"
// by Gopi Meenakshisundaram and Shankar Krishnan

	if (EdgeCounter<2) // do not calculate for underconnected points
		return;
	Edge *E= EdgeStuckHead;
	double CosTheta=1.0; // cos of half the angle of the bounding cones apex
	MyPoint A = *E->OtherNeuronInEdge(this) - (*this); // the axis of the bounding cone
	A=A*(1.0/~A);
	E = this->NextEdgeInEdgeStuck(E);

	while (E!=NULL){
		MyPoint V = *E->OtherNeuronInEdge(this) - (*this);
		V=V*(1.0/~V);
		double CosAlpha = A*V;
		if (CosTheta > CosAlpha){
			MyPoint Vt;
			if (CosTheta==1.0){ // in case this is the second vector
				Vt=A;
			}
			else{
				double Theta = acos(CosTheta);
				double Alpha = acos(CosAlpha);
				double q = sin(Theta+Alpha)/sin(Theta);
				Vt = (A*q)-V;
				Vt = Vt*(1.0/~Vt);
			}

			A=Vt+V;
			A=A*(1.0/~A);
			CosTheta = A*V;
		}

		E = this->NextEdgeInEdgeStuck(E);
	}

	Normal=A;
}



MyPoint Neuron::CalcNormalUsing3Points(Neuron *Neuron2, Neuron *Neuron3 , MyPoint *E1, MyPoint *E2){
// this function calculates a face normal using two other points as input
// it returns the normalized normal (but does not register it in the 
// the function returns E1 and E2 which are perpendicular one to the other and define a plane
// that is perpendicular to E3
// the function does not take care of numerical problems if they occour


	MyPoint V12 = *Neuron2 - *this;
	//MyPoint V23 = *Neuron3 - *Neuron2;
	MyPoint V13 = *Neuron3 - *this;

	V12 = V12 *(1.0 / ~V12);
	//V23 = V23 *(1.0 / ~V23);
	V13 = V13 *(1.0 / ~V13);

	MyPoint FaceNormal = V12 ^ V13;
	FaceNormal= FaceNormal * (1.0 /~ FaceNormal);


	if ( E1 != NULL && E2 != NULL){
		*E1	= V12;
		*E2 = FaceNormal^V12;  // no need to normalize since E112 and V12 are normalized
	}
	

	return FaceNormal;


}



void Neuron::CalcProjectionPlane(MyPoint &V1 , MyPoint &E1 , MyPoint &E2){

	// this function calculates the projection E1 and E2, using the Zero direction as V1 and the neuron normal
	E2 = Normal ^ V1;
	E2 = E2 *(1.0 / ~E2);
	E1 = E2 ^ Normal;
	E1 = E1 *(1.0 / ~E1);
	
}


void Neuron::PushClassifiedSampledPointToStuck(SamplePoint *Pt){
	Pt->Next = StuckHead;
	StuckHead=Pt;	
}


void Neuron::EmptyStuck(){

	SamplePoint *It = StuckHead;
	// just for debug
	int Counter=0;
	do {
		if (It==NULL) 
			break;
		It->Classification=NULL;
		Counter++;
		It = It->Next;

	} while (true);

	StuckHead=NULL;

}

/* old version of calc normal with PCA computation

void Neuron::CalcNormal(){
	// calculate the normal of the  neuron using PCA on the classified points
	// inplementaion of PCA is by neural networks (see Haykin secion 9.6)
	// we assume classification was already done.

	
	RealType Zero[3] = {0.0,0.0,0.0};
	RealType Small[3] = {0.5,0.5,0.5};
	MyPoint PZero(Zero);
	MyPoint PSmall(Small);
	// first find the middle of the sampled points
	SamplePoint *It = StuckHead;
	MyPoint AvgPt(Zero);
	int NumberOfPoints=0;
	do{
		AvgPt = AvgPt + *It ;
		NumberOfPoints++;
		It=It->Next;
	} while (It->Next !=NULL);

	AvgPt = AvgPt * (1.0 / NumberOfPoints);

	// now find how to normalize
	It = StuckHead;
	double MaxNorm=0.0;
	do{
		double Norm = ~(*It - AvgPt);
		if (Norm > MaxNorm) 
			MaxNorm = Norm;
		It=It->Next;
	} while (It->Next !=NULL);

	double DivideNorm=1.0/MaxNorm;

	double Eta=0.1; // this should be a constant
	double Y1,Y2;
	MyPoint W1(PZero,PSmall),W2(PZero,PSmall),Delta1,Delta2;		
	double Error = 0.0;
	int Epocs=0;
	double LastError=2*MY_EPS;
	It = StuckHead;
	do {
		//MyPoint X = *It - *this;
		MyPoint X = (*It - AvgPt) * DivideNorm;
		Y1=W1*X;
		Y2=W2*X;
		Delta1=(X-W1*Y1)*Eta*Y1;
		Delta2=(X-W1*Y1-W2*Y2)*Eta*Y2;
		W1=W1+Delta1;
		W2=W2+Delta2;
		Error=max(~Delta1,~Delta2);

		It = It->Next;
		if (It==NULL) {	
			Epocs++;
			//if (fabs(Error-LastError < 1e-8)) 
			//	break;
			if ( (W1*(1.0/~W1)) * (W2*(1.0/~W2)) < MY_EPS)
				break;

			LastError=Error;
			
			if (Epocs*NumberOfPoints >100000) {
				TRACE ("did not converge, trying to retrain\n");
				MyPoint NewW1(PZero,PSmall),NewW2(PZero,PSmall);
				Epocs=0;
				int RandPosition = (int) ((double)rand()/RAND_MAX*(NumberOfPoints-2))+1;
				SamplePoint *It2=StuckHead->Next;
				SamplePoint *OldStuckHead=StuckHead;;
				for (int i=0; i<RandPosition ; i++){
					It2=It2->Next;
				}
				StuckHead=StuckHead->Next;
				OldStuckHead->Next=It2->Next;
				It2->Next=OldStuckHead;
				W1=NewW1;
				W2=NewW2;
			}

			It=StuckHead;
			
		}
	} while (true);

	TRACE ("Epocs %d ,  number of points in epoc is %d \n",Epocs,NumberOfPoints);
	W1 = W1*(1.0/~W1);
	W2 = W2*(1.0/~W2);
	Normal = W1 ^ W2;  
	Normal = Normal * (1.0/~Normal);
}

*/



// new version of calc normal with LSQ computation


double SolveEqSetOfSize3(double A[3][3], double B[3], double Res[3]){
	// the function returns 1 / euclidian condition number
	

//	double rres[3];
	double a11=A[0][0];
	double a12=A[0][1];
	double a13=A[0][2];
	double a21=A[1][0];
	double a22=A[1][1];
	double a23=A[1][2];
	double a31=A[2][0];
	double a32=A[2][1];
	double a33=A[2][2];
	
	double det =
		a11*a22*a33-a11*a23*a32-a21*a12*a33+a21*a13*a32+a31*a12*a23-a31*a13*a22;

	double InvMat[3][3];
	

	InvMat[0][0] = (a22*a33-a23*a32);
	InvMat[0][1] =  -(a12*a33-a13*a32);
	InvMat[0][2] = (a12*a23-a13*a22);
	InvMat[1][0] = -(a21*a33-a23*a31);
	InvMat[1][1] =   (a11*a33-a13*a31);
	InvMat[1][2] = -(a11*a23-a13*a21); 
	InvMat[2][0] = (a21*a32-a22*a31);
	InvMat[2][1] = -(a11*a32-a12*a31);
	InvMat[2][2] = (a11*a22-a12*a21);

	// calc euclidian norm of inverse matrix for condition number calculation
	double NormInvA =0.0 , NormA =0.0;
	for (int i=0; i<3;i++){
		Res[i]=0.0;
		for (int j=0;j<3;j++){
			Res[i] += InvMat[i][j]	* B[j] / det;
			NormA += A[i][j] * A[i][j];
			NormInvA += InvMat[i][j]*InvMat[i][j];
		}

	}

	double OneDivCond = det / sqrt(NormA) / sqrt(NormInvA);
	if (_finite(OneDivCond))
		return OneDivCond;
	else
		return (0.0);
}


void Neuron::UpdateNormalUsingPCA(MyPoint &Pt , double Eta){
	// update the normal of the  neuron using PCA on the point Pt
	// inplementaion of PCA is by neural networks (see Haykin secion 9.6)

	// normalize so the point will reside on the unit sphere around the neuron

	MyPoint X = (Pt - *this);
	// normalize X to the size of 0.5
	X=X*(0.5/~X);

	// now check that W1 and W2 are finite if not - initialize them again to random values
	if (!W1.IsFinite() || !W2.IsFinite()){
		RealType Zero[3] = {0.0,0.0,0.0};
		RealType One[3] = {1.0,1.0,1.0};
		MyPoint PZero(Zero);
		MyPoint POne(One);
		MyPoint W1Init(PZero,POne);
		MyPoint W2Init(PZero,POne);
		W1=W1Init;
		W2=W2Init;
	}

	double Y1,Y2;
	Y1=W1*X;
	Y2=W2*X;
	MyPoint Delta1=(X-W1*Y1)*Eta*Y1;
	MyPoint Delta2=(X-W1*Y1-W2*Y2)*Eta*Y2;
	W1=W1+Delta1;
	W2=W2+Delta2;

	//W1 = W1*(1.0/~W1);
	//W2 = W2*(1.0/~W2);
	MyPoint NewNormal = W1 ^ W2 ;  
	NewNormal = NewNormal * (1.0/~NewNormal);
	if (NewNormal.IsFinite ()){
		TriDist = 0;
		Normal=NewNormal;
	}
	else{ // mark visually that pca is not working
		TriDist = -1;
	}

}

MyPoint Neuron::CalcNormalUsingLSQ(int MinNumberOfPoints){
	// calculate the normal of the  neuron using LSQ 
	// if MinNumberOfPoints is zero then the LSQ is calculated over the the neurons neighbours
	// otherwise all the classified points belonging to the neuron are used - if their number is smaller then MinNumberOfPoints then the points classified by the neighbours are also picked
	// we assume classification was already done.

	
	double Axy[3][3],Axz[3][3],Ayz[3][3];
	double Bxy[3], Bxz[3] , Byz[3];
	double Resxy[3] ,Resxz[3] ,Resyz[3];


	for (int i=0; i<3;i++){
		Bxy[i]=0.0;
		Bxz[i]=0.0;
		Byz[i]=0.0;
		for(int j=0;j<3;j++){
			Axy[i][j]=0.0;
			Axz[i][j]=0.0;
			Ayz[i][j]=0.0;
		}

	}

	Edge *E;
	Neuron *N;
	SamplePoint *It;
	int Counter=0;

	if (MinNumberOfPoints != 0){ //this means we want to use classified sample points
		E = NULL;
		N = this;
		It = N->StuckHead;
		while (It==NULL){ // check that we do not start with an empty neuron
			MinNumberOfPoints = -1; // this marks that we are not looking for minnumberofpoints anymore - instead we are traversing neighbours
			if (E==NULL)
				E = EdgeStuckHead;
			else
				E = NextEdgeInEdgeStuck(E);

			if (E==NULL) // this means we have traversed all edges of the neuron
				break;
			
			N = E->OtherNeuronInEdge(this);
			It=N->StuckHead;
		}
		if (MinNumberOfPoints == -1 && E == NULL) // this means that we did not find any classified point at the neighbours - and we want to use neurons instead
			MinNumberOfPoints=0;
		
	}
	if (MinNumberOfPoints == 0){ // this means that we want to use neighouring neurons
		E = EdgeStuckHead;
		if (E==NULL){ // this means no neighbours
			Normal.Pos[0]=0.0;  
			Normal.Pos[1]=0.0;
			Normal.Pos[2]=0.0;
			return Normal; //exit and set normal to zero to mark problem
		}
		N = E->OtherNeuronInEdge(this);
	}

#ifdef MY_DEBUG
	TRACE (" start of point list for neuron \n Pt=[");
#endif 

	while ((MinNumberOfPoints<=0 && E!=NULL) ||
		(MinNumberOfPoints>0 && Counter < MinNumberOfPoints) ){// && E!=NULL) ){

		MyPoint X;

		if (MinNumberOfPoints == 0)
			X = *N;
		else
			X = *It;

#ifdef MY_DEBUG
		TRACE ( " %lf, %lf, %lf \n" , X.Pos[0] , X.Pos[1] , X.Pos[2] );
#endif
		Axy[0][0] += X.Pos[0]*X.Pos[0];
		Axy[0][1] += X.Pos[0]*X.Pos[1];
		Axy[0][2] += X.Pos[0];

		Axy[1][1] += X.Pos[1]*X.Pos[1];
		Axy[1][2] += X.Pos[1];
		
		Axy[2][2] += 1.0;
		

		Bxy[0] -= X.Pos[0]*X.Pos[2];
		Bxy[1] -= X.Pos[1]*X.Pos[2];
		Bxy[2] -= X.Pos[2];


		
		Axz[0][0] += X.Pos[0]*X.Pos[0];
		Axz[0][1] += X.Pos[0]*X.Pos[2];
		Axz[0][2] += X.Pos[0];

		Axz[1][1] += X.Pos[2]*X.Pos[2];
		Axz[1][2] += X.Pos[2];
		
		Axz[2][2] += 1.0;
		

		Bxz[0] -= X.Pos[0]*X.Pos[1];
		Bxz[1] -= X.Pos[2]*X.Pos[1];
		Bxz[2] -= X.Pos[1];

		

		Ayz[0][0] += X.Pos[1]*X.Pos[1];
		Ayz[0][1] += X.Pos[1]*X.Pos[2];
		Ayz[0][2] += X.Pos[1];

		Ayz[1][1] += X.Pos[2]*X.Pos[2];
		Ayz[1][2] += X.Pos[2];
		
		Ayz[2][2] += 1.0;
		

		Byz[0] -= X.Pos[1]*X.Pos[0];
		Byz[1] -= X.Pos[2]*X.Pos[0];
		Byz[2] -= X.Pos[0];

		Counter++;

		if (MinNumberOfPoints == 0){
			E = NextEdgeInEdgeStuck(E);
			if (E==NULL) // this means all edges have been traversed
				break;
			N = E->OtherNeuronInEdge(this);
		}
		else{
			It = It->Next;
			while (It==NULL){
				MinNumberOfPoints=-1; // this marks that we are not looking for minnumberofpoints anymore - instead we are traversing neighbours
				if (E==NULL)
					E = EdgeStuckHead;
				else
					E = NextEdgeInEdgeStuck(E);
				
				if (E==NULL) // this means we have traversed all edges of the neuron
					break;
				
				N = E->OtherNeuronInEdge(this);
				It=N->StuckHead;
			}
		}
	} 
#ifdef MY_DEBUG
	TRACE ("];\n");
#endif

	// this is a symetric mztrix - finish it
	Axy[1][0] = Axy[0][1];
	Axy[2][0] = Axy[0][2];
	Axy[2][1] = Axy[1][2];

	Axz[1][0] = Axz[0][1];
	Axz[2][0] = Axz[0][2];
	Axz[2][1] = Axz[1][2];

	Ayz[1][0] = Ayz[0][1];
	Ayz[2][0] = Ayz[0][2];
	Ayz[2][1] = Ayz[1][2];
/*
#ifdef MY_DEBUG
	TRACE ("AMatrix = [ %lf, %lf, %lf ; %lf ,%lf ,%lf ; %lf ,%lf, %lf]\n" , A[0][0],  A[0][1] ,A[0][2], A[1][0],A[1][1], A[1][2],A[2][0],A[2][1],A[2][2]);
#endif
*/
	// a check is needed for the determinant - if it too low - this means the matrix is near singularity (it that case try setting B=1 istead of C=1)
	double Condxy= SolveEqSetOfSize3(Axy, Bxy, Resxy);
	double Condxz= SolveEqSetOfSize3(Axz, Bxz, Resxz);
	double Condyz= SolveEqSetOfSize3(Ayz, Byz, Resyz);

#ifdef MY_DEBUG
	TRACE(" condition numbers are XY %lg, XZ %lg , YZ %lg \n", Condxy,Condxz,Condyz);
#endif
	double MaxCond = max(Condxy,max(Condxz,Condyz));

	//save old normal for later comparison
	MyPoint OldNormal = Normal;

	// now search for highest condition - it probably holds the best results

	if (Condxy==MaxCond){

		Normal.Pos[0]=Resxy[0];  
		Normal.Pos[1]=Resxy[1];  
		Normal.Pos[2]=1.0;  
	}
	else if(Condxz==MaxCond){

		Normal.Pos[0]=Resxz[0];  
		Normal.Pos[1]=1.0;  
		Normal.Pos[2]=Resxz[1];  
	}
	else if(Condyz==MaxCond){

		Normal.Pos[0]=1.0;  
		Normal.Pos[1]=Resyz[0];  
		Normal.Pos[2]=Resyz[1];  
	}


	Normal = Normal * (1.0/~Normal); 

#ifdef MY_DEBUG
	TRACE (" Vec=[%lf, %lf, %lf ]';\n" , Normal.Pos[0],Normal.Pos[1],Normal.Pos[2]);
	TRACE (" calculated normal is \n");
#endif
//	ASSERT (Counter>3);
	
	// do not believe this point... skip to another
	if (Counter<3){
		Normal.Pos[0]=0.0;  
		Normal.Pos[1]=0.0;  
		Normal.Pos[2]=0.0;  
	}

	if (Normal*OldNormal < 0.0) // calculated normal is not in the same direction as the older one - corret it
		Normal = -Normal;

	return Normal;
#ifdef MY_DEBUG
	TRACE (" number of points listed is %d \n",Counter);
#endif


}


Edge *Neuron::NextEdgeInEdgeStuck(Edge *E){

	if (this==E->Neuron1){
		return E->NextInN1;
	}
	else if (this==E->Neuron2){
		return E->NextInN2;
	}
	else
		MyFatalError ("Edge mix up in Neuron edge stuck");
	
	return NULL;

}



Face **Neuron::FacesBelongingToNeuron(){
	// the function returns a list of faces that belongs to the neuron
	// the function works only for manifold surfaces

	if (this->FaceCounter == 0) 
		return NULL;

	Face **FaceArray = new Face *[this->FaceCounter];
	
	// go through the edge list of a neuron and find the faces there
	int CurrentFaceCounter=0;

	Edge *E=this->EdgeStuckHead;

	while (E!=NULL){
		Face *F = E->FacesStuckHead;
		while (F!=NULL){
			
			bool DuplicateFound=false;

			for (int i=0; i<CurrentFaceCounter ; i++){
				if (F==FaceArray[i]){
					DuplicateFound=true;
					break;
				}
			}
			if (!DuplicateFound){
				FaceArray[CurrentFaceCounter]=F;
				CurrentFaceCounter++;
			}
			F=E->NextFaceInFaceStuck(F);
		}
		E = this->NextEdgeInEdgeStuck(E);
	}

#ifdef MY_DEBUG
	//debug check
	if ( CurrentFaceCounter != this->FaceCounter )
		MyFatalError ("number of faces found does not correspond to number of faces listed in neuron");
#endif

	return (FaceArray);
}

/* version 1 of confidense Normal * edge
double Neuron::CalcTriangulationConfidenceFor3Pt(Neuron *N1 , Neuron *N2){
		MyPoint Dir1 = (*N1 - *this);
		Dir1=Dir1 *(1.0/~Dir1);
		double Mult1= Normal * Dir1;

		MyPoint Dir2 = (*N2 - *this);
		Dir2=Dir2 *(1.0/~Dir2);
		double Mult2= Normal * Dir2;

		return (Mult1*Mult1 + Mult2*Mult2)/2; 
}
double Neuron::CalcTriangulationConfidence(bool UpdatePointDist, int MaxConnectivityOfEdge){
	// this function calculates the confidence of a Neuron for triangulation
	// the function ruturns the squared sum of the neuron normal multiplied scalarly with the edge direction divided by the number of Edges
	// only edges with fcae connectivity of MaxConnectivityOfEdge or lower are added in the furmula
	// if UpdatePointDist is true the value is stored in the PointDist member variable of the Neuron

	Edge *E= EdgeStuckHead;
	double SquaredSum=0.0;
	int NumberOfEdges=0;
	while (E!=NULL){
		if (E->FaceCounter <= MaxConnectivityOfEdge ){
			Neuron *N =E->OtherNeuronInEdge(this);
			MyPoint Dir = (*N - *this);
			Dir=Dir *(1.0/~Dir);
			double Mult= Normal * Dir;
			SquaredSum += Mult*Mult; 
			NumberOfEdges++;
		}

		E = this->NextEdgeInEdgeStuck(E);
	}

	if (NumberOfEdges) // dont devide by zero
		SquaredSum /= NumberOfEdges;

	if (UpdatePointDist){
		PointDist=SquaredSum;
	}

	return SquaredSum;

}

*/


//version 2 of confidense Normal * Normal
double Neuron::CalcTriangulationConfidenceFor3Pt(Neuron *N1 , Neuron *N2){
		double Mult1= Normal * N1->Normal;
		double Mult2= Normal * N2->Normal;
		return 1.0 - (Mult1*Mult1 + Mult2*Mult2)/2; 
}

double Neuron::CalcTriangulationConfidence(bool UpdatePointDist, int MaxConnectivityOfEdge){
	// this function calculates the confidence of a Neuron for triangulation
	// the function ruturns the squared sum of the neuron normals of neurons with an edge direction divided by the number of Edges
	// only edges with fcae connectivity of MaxConnectivityOfEdge or lower are added in the furmula
	// if UpdatePointDist is true the value is stored in the PointDist member variable of the Neuron

	Edge *E= EdgeStuckHead;
	double SquaredSum=0.0;
	int NumberOfEdges=0;
	while (E!=NULL){
		if (E->FaceCounter <= MaxConnectivityOfEdge ){
			Neuron *N =E->OtherNeuronInEdge(this);
			MyPoint Dir = (*N - *this);
			double Mult= Normal * N->Normal;
			SquaredSum += Mult*Mult; 
			NumberOfEdges++;
		}

		E = this->NextEdgeInEdgeStuck(E);
	}

	if (NumberOfEdges) // dont devide by zero
		SquaredSum /= NumberOfEdges;

	if (UpdatePointDist){
		PointDist=SquaredSum;
	}

	return (1.0 - SquaredSum);

}



int Neuron::MarkDisconnectedTriangleFans(){
// This function gives each disconnected triangle fan connected to the current Neuron a different number
// Each face and each edge is marked with this number
// the function works properly only for manifold edges - the function returns -1 (non manifold) if there are edges with 3 or more faces 
	int NumberOfFans=0;
	// first deleted all previous tags from edges
	Edge *E = EdgeStuckHead;
	while (E!=NULL){
		if (E->FaceCounter>2)
			return (-1);
		E->FanNumber=0;
		E=NextEdgeInEdgeStuck (E);
	}

	// now go over the edges not marked and have faces connected to them
	E = EdgeStuckHead;
	while (E!=NULL){
		// loop through the edges an look for unmarked edges with faces connected to them
		if (E->FaceCounter!=0 && E->FanNumber==0){
			NumberOfFans++;
			Face *F=E->FacesStuckHead;
			while (F!=NULL){
				Edge *E1=E;
				Face *F1=F;
				// now continue walking around the vertex through face and edge
				do {
					E1->FanNumber=NumberOfFans;
					E1=F1->OtherEdgeInFace(E1,this);
					F1->FanNumber=NumberOfFans;
					F1 = F1->NextManifoldFace (E1);
				} while (F1!=NULL && E1!=E);
				if (E1==E) // this means we have found a whole triangle fan and there is no need to go and search the same fan in the opposite direction
					break;
				else	// this means that the last edge needs to be marked as well
					E1->FanNumber = NumberOfFans;
				// search the fan in the opposite direction
				F=E->NextFaceInFaceStuck(F);
			}
		}
		E=NextEdgeInEdgeStuck(E);
	}

	return (NumberOfFans);

}


int Neuron::CheckIfNeuronIsManifoldNeuron(){

// this function calculates if the neron is a manifold neuron it marks the result in the NonManifoldNeuron member variable
// if no edges are connected to the neuron return -3
// if the neuron has edges with more then 2 faces connected to them - return (-2)  
// If the number of faces connected to the neuron is no the the same as the number of edges connected to it then the triangle fan around the neuron is not complete and the function returns -1 
// Otherwise the neuron is either a manifold if there is only one fan connected to it or a singular point in the manifold if more ten one fan is connected to it.  the function returns the number of different triangle fans minus 1 (that is manifold neurons get the value of zero (false) 
// the function also marks the triangle fans of the neuron.

	int FanNumber = MarkDisconnectedTriangleFans();

	if (EdgeCounter==0)
		NonManifoldNeuron=-3;
	else if (FanNumber==-1)
		NonManifoldNeuron=-2;
//	else if (FaceCounter != EdgeCounter ) ***** change this line of code to check only edges with velance of 2
//		NonManifoldNeuron=-1;
	else
		NonManifoldNeuron=FanNumber-1;

	return (NonManifoldNeuron);

}

SamplePoint *Neuron::FindNearestClassifiedPointToNeuron(bool ChangeNeuronPositionToNearestPoint){
// the function returns the nearest sampled point classified to the neuron

	SamplePoint *Pt=this->StuckHead ;
	double MinDist=MAX_DOUBLE;
	SamplePoint *BestPt=Pt;

	while (Pt!=NULL){
		double Dist = ~(*Pt - *this);
		if (Dist < MinDist){
			MinDist=Dist;
			BestPt=Pt;
		}
		Pt = Pt->Next;
	}

	if (ChangeNeuronPositionToNearestPoint && BestPt!=NULL)
		this->MyPoint::operator = ((MyPoint) *BestPt);
		
	return BestPt;
}


