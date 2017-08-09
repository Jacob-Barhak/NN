#include "stdafx.h"
#include "NeuralNet.h"

// define colors
double NeuronColor[4]={0.0,0.0,0.0,1.0};
double NormalColorTriangulated[4]={0.8,0.6,0.6,1.0};
double NormalColorNonManifold[4]={1.0,0.0,1.0,1.0};
double PointColorUsed[4]={0.0,0.0,0.6,1.0};
double PointColorNotUsed[4]={0.0,0.0,1.0,1.0};

// added for patch classification
double NeuronBaseColor[4]={0.3,0.3,0.3,0.99};
double NeuronPatchAdditionColor[4]={0.2,0.35,0.2,0.0};
double EdgeColorDiscrimination[4] = {0.225,0.325,0.425,0.0};

double EdgeColorUDir[4]={1.0,0.0,0.0,1.0};
double EdgeColorVDir[4]={0.0,0.8,0.0,1.0};
double EdgeColor[4]={0.5,1.0,0.0,1.0};
double EdgeColorNew[4]={1.0,1.0,0.0,1.0};
double EdgeColorInside[4]={1.0,0.0,0.0,1.0};
double EdgeColorBoundary[4]={0.0,1.0,1.0,1.0};
double EdgeColorNonManifold[4]={1.0,0.0,1.0,1.0};
double EdgeColorNotConsistantInDirection[4]={1.0,1.0,1.0,1.0};

double NormalColor[4]={0.0,0.0,0.0,1.0};

double FaceColorTransperant[4]={0.0,1.0,0.0,0.5};
double FaceColorNew[4]={0.0,1.0,0.0,1.0};
float FaceMaterialColorFront[4]={0.0f,1.0f,0.0f,1.0f};
float FaceMaterialColorBack[4]={1.0f,0.6f,0.0f,1.0f};
float MaterialSpecularColor[4]={1.0f,1.0f,1.0f,1.0f};
float MaterialShininess[1]={8.0f};





void MyPoint::Draw (RealType PointSize){

	glBegin(GL_LINES);
	  glVertex3d(Pos[0]-PointSize,Pos[1],Pos[2]);
	  glVertex3d(Pos[0]+PointSize,Pos[1],Pos[2]);

	  glVertex3d(Pos[0],Pos[1]-PointSize,Pos[2]);
	  glVertex3d(Pos[0],Pos[1]+PointSize,Pos[2]);

	  glVertex3d(Pos[0],Pos[1],Pos[2]-PointSize);
	  glVertex3d(Pos[0],Pos[1],Pos[2]+PointSize);
	glEnd();

}




void Neuron::Draw (RealType PointSize, RealType NormalSize){

	glLineWidth(2.0);
	glColor4dv(NeuronColor);
	this->MyPoint::Draw(PointSize);
	if (NormalSize>0.0){
		glLineWidth(4.0);
		glPushName ( (GLint) this);
		glBegin(GL_LINES);
		if (NonManifoldNeuron>0)
			glColor4dv(NormalColorNonManifold);
		else if (TriDist<0 || Tags & NEURON_ON_BOUNDARY)
			glColor4dv(NormalColorTriangulated);
		else
			glColor4dv(NormalColor);

		glVertex3d(Pos[0],Pos[1],Pos[2]);
		glVertex3d(Normal.Pos[0]*NormalSize+Pos[0],Normal.Pos[1]*NormalSize+Pos[1],Normal.Pos[2]*NormalSize+Pos[2]);
		glEnd();
	}
	glPopName();
	glLineWidth(1.0);

}


/* edges with different colors by patches version
void Edge::Draw(){
	double ThisNeuronColor[4];

	if (Tag & EDGE_V_DIR)
		glLineWidth(4.0);
	else
		glLineWidth(2.0);
	glBegin(GL_LINES);
		for (int i=0;i<4;i++){
			ThisNeuronColor[i]=Neuron1->PatchClassification * NeuronPatchAdditionColor[i];
			if (Tag & EDGE_U_DIR) ThisNeuronColor[i] += EdgeColorDiscrimination[i];
			if (Tag & EDGE_V_DIR) ThisNeuronColor[i] += EdgeColorDiscrimination[i]+EdgeColorDiscrimination[i];
			
			ThisNeuronColor[i] = NeuronBaseColor[i] + fabs( ThisNeuronColor[i] - (int) ThisNeuronColor[i]) *(1.0 - NeuronBaseColor[i]);

		}
		//if (Tag & EDGE_U_DIR)
		//	glColor4dv(EdgeColorUDir);
		//else
			glColor4dv(ThisNeuronColor);
		glVertex3d(Neuron1->Pos[0],Neuron1->Pos[1],Neuron1->Pos[2]);
  		
		for (i=0;i<4;i++){
			ThisNeuronColor[i]=Neuron2->PatchClassification * NeuronPatchAdditionColor[i];
			if (Tag & EDGE_U_DIR) ThisNeuronColor[i] += EdgeColorDiscrimination[i];
			if (Tag & EDGE_V_DIR) ThisNeuronColor[i] += EdgeColorDiscrimination[i]+EdgeColorDiscrimination[i];
			
			ThisNeuronColor[i] = NeuronBaseColor[i] + fabs( ThisNeuronColor[i] - (int) ThisNeuronColor[i]) *(1.0 - NeuronBaseColor[i]);
		}
		//if (Tag & EDGE_U_DIR)
		//	glColor4dv(EdgeColorUDir);
		//else
			glColor4dv(ThisNeuronColor);
		glVertex3d(Neuron2->Pos[0],Neuron2->Pos[1],Neuron2->Pos[2]);
	glEnd();

	glLineWidth(1.0);


}
*/


void Edge::Draw(){
	glBegin(GL_LINES);
		glVertex3d(Neuron1->Pos[0],Neuron1->Pos[1],Neuron1->Pos[2]);
		glVertex3d(Neuron2->Pos[0],Neuron2->Pos[1],Neuron2->Pos[2]);
	glEnd();

}





void PointList::DrawList (RealType PointSize, int ChunkSize, int BaseIteration){

	if (!DispListNumber) return; // list not initialized

	glNewList(DispListNumber,GL_COMPILE);

	for(int i=0; i<Size; i++){
		// first points after index (cyclic) are purple all others are blue
		/*
		if ( ((i-(BaseIteration % Size)) % Size) < ChunkSize) 
			glColor4dv(PointColorUsed);
		else
			glColor4dv(PointColorNotUsed);
		
		Array[i]->Draw(PointSize*Array[i]->Weight);
		*/
		/*
		if ( Array[i]->Weight == 1.0) 
			glColor4dv(PointColorUsed);
		else
			glColor4dv(PointColorNotUsed);
		
		Array[i]->Draw(PointSize);//*Array[i]->Weight);
		*/
		
		
		double ThisNeuronColor[4];
		if (Array[i]->Classification != NULL){
			for (int j=0;j<4;j++){
				ThisNeuronColor[j] = Array[i]->Classification->NeuronId * NeuronPatchAdditionColor[j];			
				ThisNeuronColor[j] = NeuronBaseColor[j] + fabs( ThisNeuronColor[j] - (int) ThisNeuronColor[j]) *(1.0 - NeuronBaseColor[j]);
			}
			glColor4dv(ThisNeuronColor);
		}
		else
			glColor4dv(PointColorUsed);
		Array[i]->Draw(PointSize);//*Array[i]->Weight);

	}

	glEndList();
	RecalcForDisplay=false;

	MyGLErrorMessage("building point list");

}


void PointList::Draw(){
	if (!DispListNumber) return; // list not initialized
	glCallList(DispListNumber);
}


void NeuralNet::DrawList(RealType PointSize, RealType NormalSize ){

	if (!DispListNumber) return; // list not initialized

	int i=0, 
	NetSize=Neurons.Size;	

	glNewList(DispListNumber,GL_COMPILE);
	Neuron *It=Neurons.Head;
	if (Neurons.Head!=NULL) do{
		//glColor4dv(NeuronColor);
		It->Draw(PointSize, NormalSize);
		It=It->Next;
		i++; //just count to see that there is no missing point at the end
	} while (It!=Neurons.Head);

	if (i != Neurons.Size) MyFatalError ("NeuralNet::DrawList - inconsistency detected in size of list");

		glEndList();


	MyGLErrorMessage("building NN list");

}


void NeuralNet::Draw(){
	if (!DispListNumber) return; // list not initialized
	glCallList(DispListNumber);
}


int MyGLErrorMessage(char *ErrorMessage){

	int err=glGetError();
	if (err){
		const unsigned char *ErrStr = gluErrorString(err);	
		TRACE ("GL error: %s - %s\n",ErrorMessage,ErrStr);
		ASSERT(0);
		exit(0);
	}
	
	return err;
}


void NeuralNet::InitDispList(){
	DispListNumber = glGenLists(1);
	MyGLErrorMessage("NN init value");
	// we assume no glerror is created... add warning later in here
	// we create the an empty list...
	glNewList(DispListNumber,GL_COMPILE);
		glRectf(0.0,0.0,1.0,1.0);
	glEndList();

	MyGLErrorMessage("NN init list");

}


void PointList::InitDispList(){

	
	DispListNumber = glGenLists(1);

	MyGLErrorMessage("Ptlist init value");
	// we assume no glerror is created... add warning later in here
	// we create the an empty list...
	glNewList(DispListNumber,GL_COMPILE);
		glRectf(0.0,0.0,1.0,1.0);
	glEndList();

	MyGLErrorMessage("Ptlist init list");

}

void EdgeList::Draw(){
	if (!DispListNumber) return; // list not initialized
	glCallList(DispListNumber);
}

void EdgeList::DrawList(int DrawEdgeNew){
	if (!DispListNumber) return; // list not initialized

	int i=0;	

	glNewList(DispListNumber,GL_COMPILE);


	Edge *TempEdge=Head;
	if (Head!=NULL) do {

//		TempEdge->Draw();


		if (TempEdge->Tag & EDGE_U_DIR){
			glLineWidth(2);
			glColor4dv(EdgeColorUDir);
		}
		else if (TempEdge->Tag & EDGE_V_DIR){
			glLineWidth(2);
			glColor4dv(EdgeColorVDir);
		}
		else if (TempEdge->NotConsistantInDirection){
			glLineWidth(8);
			glColor4dv(EdgeColorNotConsistantInDirection);
		}
		else if (TempEdge->FaceCounter ==1 )
			glColor4dv(EdgeColorBoundary);
		else if (TempEdge->FaceCounter ==2 )
			glColor4dv(EdgeColorInside);
		else if (TempEdge->FaceCounter >2 ){
			glLineWidth(8);
			glColor4dv(EdgeColorNonManifold);
		}
		else
			glColor4dv(EdgeColor);


		if (TempEdge->Tag & EDGE_ADDED_FOR_TRIANGULATION){			
			glEnable (GL_LINE_STIPPLE);
			glLineStipple(1,0x0f0f);
			TempEdge->Draw();
			glDisable (GL_LINE_STIPPLE);
		}
		else
			TempEdge->Draw();

		// return line width to original
		glLineWidth(1);
		TempEdge=TempEdge->Next;
		i++; //just count to see that there is no missing point at the end
	} while (TempEdge!=Head);

#ifdef MY_DEBUG
	// debug check
	if (i != Size) MyFatalError("EdgeList::DrawList - inconsistency in list size was detected");
#endif

	glEndList();

	MyGLErrorMessage("building Edge list");

}

void EdgeList::InitDispList(){
	DispListNumber = glGenLists(1);

	MyGLErrorMessage("Edgelist init value");
	// we assume no glerror is created... add warning later in here
	// we create the an empty list...
	glNewList(DispListNumber,GL_COMPILE);
		glRectf(0.0,0.0,1.0,1.0);
	glEndList();

	MyGLErrorMessage("Edgelist init list");


}




void FaceList::Draw(){
	if (!DispListNumber) return; // list not initialized
	glCallList(DispListNumber);
}


void FaceList::DrawList(){
	// this function draws all the edges, the first DrawEdgeNew edges are drawn in a different color.
	// for rectangular topology grids, the U direction edges are drawn with a different color from V direction edges
	

	if (!DispListNumber) return; // list not initialized

	int i=0;	

	glNewList(DispListNumber,GL_COMPILE);


	Face *TempFace=Head;
	if (Head!=NULL) do {
		//glColor4dv(FaceColorTransperant);
		//glColor4d(0.0,1.0-(double)TempFace->PatchNumber/5.0,(double)TempFace->PatchNumber/5.0,1.0);//0.5);
		//glColor4d(0.0,1.0-(double)TempFace->Tag/5.0,(double)TempFace->Tag/5.0,1.0);//0.5);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,MaterialSpecularColor);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,MaterialShininess);

		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE , FaceMaterialColorFront);
		glMaterialfv(GL_BACK,GL_AMBIENT_AND_DIFFUSE , FaceMaterialColorBack);
		//glColor4fv(FaceMaterialColor);

		TempFace->Draw();
		TempFace=TempFace->Next;
		i++; //just count to see that there is no missing point at the end
	} while (TempFace!=Head);

#ifdef MY_DEBUG
	// debug check
	if (i != Size) MyFatalError("FaceList::DrawList - inconsistency in list size was detected");
#endif

	glEndList();

	MyGLErrorMessage("building Face list");

}

void FaceList::InitDispList(){
	DispListNumber = glGenLists(1);

	MyGLErrorMessage("Edgelist init value");
	// we assume no glerror is created... add warning later in here
	// we create the an empty list...
	glNewList(DispListNumber,GL_COMPILE);
		glRectf(0.0,0.0,1.0,1.0);
	glEndList();

	MyGLErrorMessage("Facelist init list");


}

void Face::Draw(){
	Neuron *Neuron1,*Neuron2,*Neuron3;
	GetFaceNeurons(&Neuron1,&Neuron2,&Neuron3);
	glPushName ( (GLint) this);
	if (!FlipDirection){
		glBegin(GL_POLYGON);
		glNormal3d(Neuron1->Normal.Pos[0],Neuron1->Normal.Pos[1],Neuron1->Normal.Pos[2]);
		glVertex3d(Neuron1->Pos[0],Neuron1->Pos[1],Neuron1->Pos[2]);
		glNormal3d(Neuron2->Normal.Pos[0],Neuron2->Normal.Pos[1],Neuron2->Normal.Pos[2]);
		glVertex3d(Neuron2->Pos[0],Neuron2->Pos[1],Neuron2->Pos[2]);
		glNormal3d(Neuron3->Normal.Pos[0],Neuron3->Normal.Pos[1],Neuron3->Normal.Pos[2]);
		glVertex3d(Neuron3->Pos[0],Neuron3->Pos[1],Neuron3->Pos[2]);
		glEnd();
	}
	else{
		glBegin(GL_POLYGON);
		glNormal3d(Neuron3->Normal.Pos[0],Neuron3->Normal.Pos[1],Neuron3->Normal.Pos[2]);
		glVertex3d(Neuron3->Pos[0],Neuron3->Pos[1],Neuron3->Pos[2]);
		glNormal3d(Neuron2->Normal.Pos[0],Neuron2->Normal.Pos[1],Neuron2->Normal.Pos[2]);
		glVertex3d(Neuron2->Pos[0],Neuron2->Pos[1],Neuron2->Pos[2]);
		glNormal3d(Neuron1->Normal.Pos[0],Neuron1->Normal.Pos[1],Neuron1->Normal.Pos[2]);
		glVertex3d(Neuron1->Pos[0],Neuron1->Pos[1],Neuron1->Pos[2]);
		glEnd();
	}
	glPopName ();

	
}
