// NNDoc.cpp : implementation of the CNNDoc class
//

#include "stdafx.h"
#include "NN.h"

#include "NNDoc.h"
#include "TrainNetDialogPropSheet.h"
#include "InitNeuralNetworkDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNNDoc

IMPLEMENT_DYNCREATE(CNNDoc, CDocument)

BEGIN_MESSAGE_MAP(CNNDoc, CDocument)
	//{{AFX_MSG_MAP(CNNDoc)
	ON_COMMAND(ID_BUTTON_PAUSE_NET_TRAINING, OnButtonPauseNetTraining)
	ON_COMMAND(ID_BUTTON_TRAIN_NET_CONTINOUS, OnButtonTrainNetContinous)
	ON_COMMAND(ID_BUTTON_TRAIN_NET_ONE_STEP, OnButtonTrainNetOneStep)
	ON_COMMAND(ID_BUTTON_CHANGE_TRAINING_PARAMETERS, OnButtonChangeTrainingParameters)
	ON_COMMAND(ID_BUTTON_CHANGE_NETWORK_TYPE, OnButtonChangeNetworkType)
	ON_COMMAND(ID_BUTTON_TRIANGULATE_NET, OnButtonTriangulateNet)
	ON_COMMAND(ID_BUTTON_CLOSE_LOOPS, OnButtonCloseLoops)
	ON_COMMAND(ID_BUTTON_DELETE_NON_MANIFOLD_NEURONS, OnButtonDeleteNonManifoldNeurons)
	ON_COMMAND(ID_BUTTON_DETECT_NON_MANIFOLD_NEURONS, OnButtonDetectNonManifoldNeurons)
	ON_COMMAND(ID_BUTTON_CALC_NORMALS_USING_LSQ, OnButtonCalcNormalsUsingLsq)
	ON_COMMAND(ID_BUTTON_DELETE_NON_MANIFOLD_FACES, OnButtonDeleteNonManifoldFaces)
	ON_COMMAND(ID_BUTTON_INITIATE_ENHANCED_HEBBIAN_LEARNING, OnButtonInitiateEnhancedHebbianLearning)
	ON_COMMAND(ID_BUTTON_RESET_NORMAL_DIRECTIONS, OnButtonResetNormalDirections)
	ON_COMMAND(ID_BUTTON_RESET_NEURON_POSITIONS_TO_RANDOM_SAMPLE_POINTS, OnButtonResetNeuronPositionsToRandomSamplePoint)
	ON_COMMAND(ID_BUTTON_RESET_NEURON_POSITIONS_TO_NEAREST_SAMPLE_POINT, OnButtonResetNeuronPositionsToNearestSamplePoint)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNNDoc construction/destruction

CNNDoc::CNNDoc()
{
	// TODO: add one-time construction code here
	// Neural Gas
	m_NGInitEps=0.3;
	m_NGFinalEps=0.05;
	m_NGInitLambda=30.0;
	m_NGFinalLambda=0.05;
	m_NGRunLength=40000;
	m_NGEdgeExperationAgeInit=20;
	m_NGEdgeExperationAgeFinal=200;
	m_NGNeuronNumber=100;
	m_NGChunkSize=100;

	m_NGEdgeExperationAgeNextChunk = m_NGEdgeExperationAgeInit;
	m_NGEpsNextChunk = m_NGInitEps;
	m_NGLambdaNextChunk = m_NGInitLambda;

	m_CurrentIteration=0;

	//growing neural gas
	m_GNGAgeMax=100;
	m_GNGAlpha=0.5;
	m_GNGBetta=0.0005;
	m_GNGChunkSize=100;
	m_GNGEpsB=0.05;
	m_GNGEpsN=0.006;
	m_GNGRunLength=100000;
	m_GNGNeuronNumber=3; // is not used
	m_GNGLambdaGrowth=1000;


	// self organizing feature map
	m_SOFMChunkSize=1000;
	m_SOFMFinalEps=0.01;
	m_SOFMInitEps=0.1;
	m_SOFMRadiusFinal=100.0; // do not use radius
	m_SOFMRadiusInit=100.0; // do not use radius
	m_SOFMRunLength=500000;
	m_SOFMSigmaFinal=0.1;
	m_SOFMSigmaInit=3;

	m_SOFMNeuronNumberU=3;
	m_SOFMNeuronNumberV=3;

	m_SOFMGridCanGrow=TRUE;
	m_SOFMCyclicUDir=FALSE;
	m_SOFMCyclicVDir=FALSE;


	m_SOFMEpsNextChunk = m_SOFMInitEps;
	m_SOFMRadiusNextChunk = m_SOFMRadiusInit;
	m_SOFMSigmaNextChunk = m_SOFMSigmaInit;

	m_TrainingNetNow=false;
	m_TrainingContinously=false;

	m_SOFMLambdaGrowth=1000;


	m_SOFMBoundaryCorrectionEvery=5000;
	m_SOFMBoundaryCorrectionPointWeight=1.0;
	m_SOFMBoundaryCorrectionRunLength=4000;


	m_NetTrainingMethod=NN_TRAINING_METHOD_NOT_SELECTED; //this means that the training method is not selected YET

	m_TerminateThread=false;
}

CNNDoc::~CNNDoc()
{

}

BOOL CNNDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CNNDoc diagnostics

#ifdef _DEBUG
void CNNDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNNDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNNDoc commands





void CNNDoc::OnButtonPauseNetTraining() 
{
	// TODO: Add your command handler code here
	m_TrainingContinously=false;
	
}

void CNNDoc::OnButtonTrainNetContinous() 
{
	// TODO: Add your command handler code here
	if (m_TrainingContinously) return;

	if ((m_NetTrainingMethod == NN_TRAINING_METHOD_NEURAL_GAS //Neural Gas
		&& m_CurrentIteration < m_NGRunLength )	||		
		(m_NetTrainingMethod == NN_TRAINING_METHOD_GROWING_NEURAL_GAS //Growing Neural Gas
		&& m_CurrentIteration < m_GNGRunLength ) ||
		(m_NetTrainingMethod == NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP //Self Organizing Feature Map
		&& m_CurrentIteration < m_SOFMRunLength )){

		// mark that you are training continously
		m_TrainingContinously=true;

		AfxBeginThread(TrainInThread, this );
#ifdef MY_DEBUG
		TRACE ("NNdoc: thread created\n");
#endif
	}
	else{
		MessageBox(NULL,"Already finished training the Neural Net",NULL,MB_OK);
		m_TrainingContinously=false;
		return;
	}

}

void CNNDoc::OnButtonTrainNetOneStep() 
{
	// TODO: Add your command handler code here
 	if (m_TrainingNetNow) return; //acts as a semaphore

	if (m_CurrentIteration >= m_NGRunLength){
		MessageBox(NULL,"Already finished training the Neural Net",NULL,MB_OK);
		m_TrainingNetNow=false;
		return;
		
	}

	AfxBeginThread(TrainInThread, this );
}

void CNNDoc::TrainNetOneChunk() 
{
	// mark that we are training now
	m_TrainingNetNow=true;
	int NumberOfIterations;

	double NGEpsInit;
	double NGLambdaInit;
	int NGEdgeExperationAgeInit;

	double SOFMEpsInit;
	double SOFMRadiusInit;
	double SOFMSigmaInit;

	int TrainingSuccess;

	switch (m_NetTrainingMethod){

	case NN_TRAINING_METHOD_NEURAL_GAS: //Neural Gas
		NumberOfIterations=min(m_NGChunkSize,m_NGRunLength-m_CurrentIteration);
		 
		NGEpsInit=m_NGEpsNextChunk;
		NGLambdaInit=m_NGLambdaNextChunk;
		NGEdgeExperationAgeInit=m_NGEdgeExperationAgeNextChunk;

		m_NGEpsNextChunk=EXP_DECAY(m_NGInitEps,m_NGFinalEps,(m_CurrentIteration+NumberOfIterations),m_NGRunLength);
		m_NGLambdaNextChunk=EXP_DECAY(m_NGInitLambda,m_NGFinalLambda,(m_CurrentIteration+NumberOfIterations),m_NGRunLength);
		m_NGEdgeExperationAgeNextChunk = (int) (EXP_DECAY(m_NGEdgeExperationAgeInit,m_NGEdgeExperationAgeFinal,(m_CurrentIteration+NumberOfIterations),m_NGRunLength));

		m_UpdatingListsNow.Lock(); //*** locking
		TrainingSuccess=m_NNet.TrainNeuralGasByPointList(m_PtLst , m_CurrentIteration , NumberOfIterations, NGEpsInit, m_NGEpsNextChunk, NGLambdaInit, m_NGLambdaNextChunk, NGEdgeExperationAgeInit,m_NGEdgeExperationAgeNextChunk);
		m_UpdatingListsNow.Unlock(); // *** unlocking


		break;
	case NN_TRAINING_METHOD_GROWING_NEURAL_GAS: //Growing Neural Gas
		NumberOfIterations=min(m_GNGChunkSize,m_GNGRunLength-m_CurrentIteration);

		m_UpdatingListsNow.Lock(); //*** locking
		//TrainingSuccess=m_NNet.TrainGrowingCellStructureByPointList(m_PtLst , m_CurrentIteration , NumberOfIterations, m_GNGEpsB, m_GNGEpsN, m_GNGAlpha, m_GNGBetta, m_GNGAgeMax, m_GNGLambdaGrowth);
		//TrainingSuccess=m_NNet.TrainGrowingNeuralFacesByPointList(m_PtLst , m_CurrentIteration , NumberOfIterations, m_GNGEpsB, m_GNGEpsN, m_GNGAlpha, m_GNGBetta, m_GNGAgeMax, m_GNGLambdaGrowth);
		TrainingSuccess=m_NNet.TrainGrowingNeuralGasByPointList(m_PtLst , m_CurrentIteration , NumberOfIterations, m_GNGEpsB, m_GNGEpsN, m_GNGAlpha, m_GNGBetta, m_GNGAgeMax, m_GNGLambdaGrowth);
		m_UpdatingListsNow.Unlock(); // *** unlocking

		break;
	case NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP: //Self Organizing Feature Map
		NumberOfIterations=min(m_SOFMChunkSize,m_SOFMRunLength-m_CurrentIteration);

		SOFMEpsInit=m_SOFMEpsNextChunk;
		SOFMRadiusInit=m_SOFMRadiusNextChunk;
		SOFMSigmaInit=m_SOFMSigmaNextChunk;

		m_SOFMEpsNextChunk=EXP_DECAY(m_SOFMInitEps,m_SOFMFinalEps,(m_CurrentIteration+NumberOfIterations),m_SOFMRunLength);
		m_SOFMRadiusNextChunk=EXP_DECAY(m_SOFMRadiusInit,m_SOFMRadiusFinal,(m_CurrentIteration+NumberOfIterations),m_SOFMRunLength);
		m_SOFMSigmaNextChunk=EXP_DECAY(m_SOFMSigmaInit,m_SOFMSigmaFinal,(m_CurrentIteration+NumberOfIterations),m_SOFMRunLength);

		m_UpdatingListsNow.Lock(); //*** locking
		TrainingSuccess=m_NNet.TrainSelfOrgenizingFeatureMapsByPointList(m_PtLst , m_CurrentIteration , NumberOfIterations, SOFMEpsInit, m_SOFMEpsNextChunk, SOFMSigmaInit,m_SOFMSigmaNextChunk, SOFMRadiusInit, m_SOFMRadiusNextChunk, m_SOFMLambdaGrowth,m_SOFMBoundaryCorrectionEvery,m_SOFMBoundaryCorrectionPointWeight, m_SOFMBoundaryCorrectionRunLength ,true);
		m_UpdatingListsNow.Unlock(); // *** unlocking

		break;
	}

	// this is true for all methods
	if (TrainingSuccess)
		m_CurrentIteration+=NumberOfIterations;


	//	this->UpdateAllViews(NULL);
	//CWnd* pWndNotifyProgress = CWnd::FromHandle(AfxGetMainWnd()->m_hWnd);
	//pWndNotifyProgress->SendMessage( WM_USER_NN_CHUNK_CALC_FINISHED);

#ifdef MY_DEBUG
	TRACE ("doc: Sending message to mainframe - iteration %i\n", m_CurrentIteration);
#endif
	if (m_TerminateThread) {
		m_TrainingNetNow=false;
		AfxEndThread(0,true);
	}
	AfxGetMainWnd()->SendMessage(WM_USER_NN_CHUNK_CALC_FINISHED,0,(LPARAM) (LPVOID) this);
#ifdef MY_DEBUG
	TRACE ("doc: already Sent message to mainframe\n");
#endif
	::Sleep(0); // go to sleep
	
	m_TrainingNetNow=false;

}


void CNNDoc::TrainNetContinously() 
{

	switch (m_NetTrainingMethod){

	case NN_TRAINING_METHOD_NEURAL_GAS: //Neural Gas
		while (m_TrainingContinously && m_CurrentIteration < m_NGRunLength && !m_TerminateThread)			
			TrainNetOneChunk();
		break;
	case NN_TRAINING_METHOD_GROWING_NEURAL_GAS: //Growing Neural Gas
		while (m_TrainingContinously && m_CurrentIteration < m_GNGRunLength && !m_TerminateThread)
			TrainNetOneChunk();
		break;
	case NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP: //Self Organizing Feature Map
		while (m_TrainingContinously && m_CurrentIteration < m_SOFMRunLength && !m_TerminateThread)
			TrainNetOneChunk();
		break;
	}
	
		
	
	m_TrainingContinously=false;
}


UINT TrainInThread(LPVOID pParam)
{

	CNNDoc *pObject = (CNNDoc *) pParam;

#ifdef MY_DEBUG
	TRACE("thread started\n");
#endif
	if (pObject == NULL || !pObject->IsKindOf(RUNTIME_CLASS(CNNDoc))){
		return 1;   // if pObject is not valid
	}
	if (pObject->m_TrainingContinously){
		pObject->TrainNetContinously();
	}
	else{
		pObject->TrainNetOneChunk();
	}

	return 0;

}

LRESULT CNNDoc::OnFinishedTrainingChunk(WPARAM wParam, LPARAM lParam)
{

	this->UpdateAllViews(NULL);
#ifdef MY_DEBUG
	TRACE ("doc - called the view after chunk training - update should be complete now\n");
#endif

	return 0;
}

BOOL CNNDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	//if (!CDocument::OnOpenDocument(lpszPathName))
	//	return FALSE;
	
	// TODO: Add your specialized creation code here


	CString FileName;
	FileName = lpszPathName ;
	CString Suffix = FileName.Right(4);
	Suffix.MakeLower();

	if (Suffix == ".wrl"){ // wrl file to convert to sampled

		ifstream ifs;
		ifs.open(lpszPathName);
		if (!ifs.fail()) ifs >> m_PtLst;
		if (ifs.fail()){
			MessageBox(NULL,"Error reading input file",NULL,MB_OK);
			ifs.close();
			ifs.clear();
			return FALSE;
		}

		ifs.close();

		FileName = FileName + ".jac";

		ifs.open(FileName);
		if (!ifs.fail()){
			ifs.close();
			ifs.clear();
			ChangeNetworkType(FileName);
		}
		else{
			ifs.close();
			ifs.clear();
			ChangeNetworkType(NULL);
		}
		// debug memory check
		AfxCheckMemory();

	}
	else{ // text file

		ifstream ifs;
		ifs.open(lpszPathName);
		if (!ifs.fail()) ifs >> m_PtLst;
		if (ifs.fail()){
			MessageBox(NULL,"Error reading input file",NULL,MB_OK);
			ifs.close();
			ifs.clear();
			return FALSE;
		}

		ifs.close();

		FileName = FileName + ".jac";

		ifs.open(FileName);
		if (!ifs.fail()){
			ifs.close();
			ifs.clear();
			ChangeNetworkType(FileName);
		}
		else{
				ifs.close();
				ifs.clear();
				FileName = lpszPathName ;
				FileName = FileName + ".wrl";

				ifs.open(FileName);
				if (!ifs.fail()){
					ifs.close();
					ifs.clear();
					ChangeNetworkType(FileName);
				}
				else{
					ifs.close();
					ifs.clear();
					ChangeNetworkType(NULL);
				}
		}
		// debug memory check
		AfxCheckMemory( );

	}


	if (!ChangeNetTrainingParameters()){
	   MessageBox(NULL,"load operation canceled",NULL,MB_OK);
	   return FALSE;
	}

	this->UpdateAllViews(NULL,1L);

	return TRUE;

}

BOOL CNNDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	ofstream ofs;
	CString FileName (lpszPathName);

	
	// find the point at the end of the file name s we can change the extension
	
	ofs.open(FileName.Left(FileName.ReverseFind('.')) + ".irt");
	OutputToIritFormat(ofs,m_NNet);
	if (ofs.good()){
		SetModifiedFlag(TRUE);
		ofs.close();
	}
	else {
		MessageBox(NULL,"Error writing irit output file",NULL,MB_OK);
		ofs.close(); // not sure you really need it, but it is here jst in case - hope it does not create and error
		return FALSE;
	}

	ofs.clear();	
	//ofs << m_NNet;

	ofs.open(FileName.Left(FileName.ReverseFind('.')) + ".wrl");

	OutputToVrmlFormat(ofs,m_NNet);
	
	if (ofs.good()){
		SetModifiedFlag(TRUE);
		ofs.close();
		return TRUE;
	}
	else {
		MessageBox(NULL,"Error writing vrml output file",NULL,MB_OK);
		ofs.close(); // not sure you really need it, but it is here jst in case - hope it does not create and error
		return FALSE;
	}

	//return CDocument::OnSaveDocument(lpszPathName);
}

BOOL CNNDoc::ChangeNetTrainingParameters()
{


	CTrainNetDialogPropSheet InputDlg("Train Neural Network Dialog");

	InputDlg.NeuralGasPage.m_NGChunkSize=m_NGChunkSize;
	InputDlg.NeuralGasPage.m_NGFinalEps=m_NGFinalEps;
	InputDlg.NeuralGasPage.m_NGInitEps=m_NGInitEps;
	InputDlg.NeuralGasPage.m_NGFinalLambda=m_NGFinalLambda;
	InputDlg.NeuralGasPage.m_NGInitLambda=m_NGInitLambda;
	InputDlg.NeuralGasPage.m_NGNeuronNumber=m_NGNeuronNumber;
	InputDlg.NeuralGasPage.m_NGRunLength=m_NGRunLength;
	InputDlg.NeuralGasPage.m_NGEdgeExperationAgeInit=m_NGEdgeExperationAgeInit;
	InputDlg.NeuralGasPage.m_NGEdgeExperationAgeFinal=m_NGEdgeExperationAgeFinal;

	//growing neural gas
	
	InputDlg.GrowinNeuralGasPage.m_GNGAgeMax=m_GNGAgeMax;
	InputDlg.GrowinNeuralGasPage.m_GNGAlpha=m_GNGAlpha;
	InputDlg.GrowinNeuralGasPage.m_GNGBetta=m_GNGBetta;
	InputDlg.GrowinNeuralGasPage.m_GNGChunkSize=m_GNGChunkSize;
	InputDlg.GrowinNeuralGasPage.m_GNGEpsB=m_GNGEpsB;
	InputDlg.GrowinNeuralGasPage.m_GNGEpsN=m_GNGEpsN;
	InputDlg.GrowinNeuralGasPage.m_GNGRunLength=m_GNGRunLength;
	InputDlg.GrowinNeuralGasPage.m_GNGNeuronNumber=m_GNGNeuronNumber; // is not used
	InputDlg.GrowinNeuralGasPage.m_GNGLambdaGrowth=m_GNGLambdaGrowth;


	// self organizing feature map
	
	InputDlg.FeatureMapPage.m_SOFMChunkSize=m_SOFMChunkSize;
	InputDlg.FeatureMapPage.m_SOFMFinalEps=m_SOFMFinalEps;
	InputDlg.FeatureMapPage.m_SOFMInitEps=m_SOFMInitEps;
	InputDlg.FeatureMapPage.m_SOFMRadiusFinal=m_SOFMRadiusFinal; // do not use radius
	InputDlg.FeatureMapPage.m_SOFMRadiusInit=m_SOFMRadiusInit; // do not use radius
	InputDlg.FeatureMapPage.m_SOFMRunLength=m_SOFMRunLength;
	InputDlg.FeatureMapPage.m_SOFMSigmaFinal=m_SOFMSigmaFinal;
	InputDlg.FeatureMapPage.m_SOFMSigmaInit=m_SOFMSigmaInit;

	InputDlg.FeatureMapPage.m_SOFMLambdaGrowth=m_SOFMLambdaGrowth;

	InputDlg.FeatureMapPage.m_SOFMNeuronNumberU=m_SOFMNeuronNumberU;
	InputDlg.FeatureMapPage.m_SOFMNeuronNumberV=m_SOFMNeuronNumberV;
	InputDlg.FeatureMapPage.m_SOFMGridCanGrow=m_SOFMGridCanGrow;
	InputDlg.FeatureMapPage.m_SOFMCyclicUDir=m_SOFMCyclicUDir;
	InputDlg.FeatureMapPage.m_SOFMCyclicVDir=m_SOFMCyclicVDir;

	InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionEvery=m_SOFMBoundaryCorrectionEvery;
	InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionPointWeight=m_SOFMBoundaryCorrectionPointWeight;
	InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionRunLength=m_SOFMBoundaryCorrectionRunLength;



	if (InputDlg.DoModal()==IDOK){
		// get last page number returned by property sheet
		m_NetTrainingMethod = InputDlg.m_NetTrainingMethod;
		m_CurrentIteration=0; // iterations start from nothing

		//Neural Gas
		m_NGChunkSize=InputDlg.NeuralGasPage.m_NGChunkSize;
		m_NGFinalEps=InputDlg.NeuralGasPage.m_NGFinalEps;
		m_NGInitEps=InputDlg.NeuralGasPage.m_NGInitEps;
		m_NGFinalLambda=InputDlg.NeuralGasPage.m_NGFinalLambda;
		m_NGInitLambda=InputDlg.NeuralGasPage.m_NGInitLambda;
		m_NGNeuronNumber=InputDlg.NeuralGasPage.m_NGNeuronNumber;
		m_NGRunLength=InputDlg.NeuralGasPage.m_NGRunLength;
		m_NGEdgeExperationAgeInit=InputDlg.NeuralGasPage.m_NGEdgeExperationAgeInit;
		m_NGEdgeExperationAgeFinal=InputDlg.NeuralGasPage.m_NGEdgeExperationAgeFinal;

		m_NGEdgeExperationAgeNextChunk = m_NGEdgeExperationAgeInit;
		m_NGEpsNextChunk = m_NGInitEps;
		m_NGLambdaNextChunk = m_NGInitLambda;

		//growing neural gas
		
		m_GNGAgeMax=InputDlg.GrowinNeuralGasPage.m_GNGAgeMax;
		m_GNGAlpha=InputDlg.GrowinNeuralGasPage.m_GNGAlpha;
		m_GNGBetta=InputDlg.GrowinNeuralGasPage.m_GNGBetta;
		m_GNGChunkSize=InputDlg.GrowinNeuralGasPage.m_GNGChunkSize;
		m_GNGEpsB=InputDlg.GrowinNeuralGasPage.m_GNGEpsB;
		m_GNGEpsN=InputDlg.GrowinNeuralGasPage.m_GNGEpsN;
		m_GNGRunLength=InputDlg.GrowinNeuralGasPage.m_GNGRunLength;
		m_GNGNeuronNumber=InputDlg.GrowinNeuralGasPage.m_GNGNeuronNumber; // is not used
		m_GNGLambdaGrowth=InputDlg.GrowinNeuralGasPage.m_GNGLambdaGrowth;


		// self organizing feature map
		
		m_SOFMChunkSize=InputDlg.FeatureMapPage.m_SOFMChunkSize;
		m_SOFMFinalEps=InputDlg.FeatureMapPage.m_SOFMFinalEps;
		m_SOFMInitEps=InputDlg.FeatureMapPage.m_SOFMInitEps;
		m_SOFMRadiusFinal=InputDlg.FeatureMapPage.m_SOFMRadiusFinal; // do not use radius
		m_SOFMRadiusInit=InputDlg.FeatureMapPage.m_SOFMRadiusInit; // do not use radius
		m_SOFMRunLength=InputDlg.FeatureMapPage.m_SOFMRunLength;
		m_SOFMSigmaFinal=InputDlg.FeatureMapPage.m_SOFMSigmaFinal;
		m_SOFMSigmaInit=InputDlg.FeatureMapPage.m_SOFMSigmaInit;

		m_SOFMLambdaGrowth=InputDlg.FeatureMapPage.m_SOFMLambdaGrowth;

		m_SOFMNeuronNumberU=InputDlg.FeatureMapPage.m_SOFMNeuronNumberU;
		m_SOFMNeuronNumberV=InputDlg.FeatureMapPage.m_SOFMNeuronNumberV;
		m_SOFMGridCanGrow=InputDlg.FeatureMapPage.m_SOFMGridCanGrow;
		m_SOFMCyclicUDir=InputDlg.FeatureMapPage.m_SOFMCyclicUDir;
		m_SOFMCyclicVDir=InputDlg.FeatureMapPage.m_SOFMCyclicVDir;

		m_SOFMBoundaryCorrectionEvery=InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionEvery;
		m_SOFMBoundaryCorrectionPointWeight=InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionPointWeight;
		m_SOFMBoundaryCorrectionRunLength=InputDlg.FeatureMapPage.m_SOFMBoundaryCorrectionRunLength;

		m_SOFMEpsNextChunk = m_SOFMInitEps;
		m_SOFMRadiusNextChunk = m_SOFMRadiusInit;
		m_SOFMSigmaNextChunk = m_SOFMSigmaInit;

		// debug memory check
		AfxCheckMemory( );

		return TRUE;
	}
	else
		return FALSE;
	

}

void CNNDoc::OnButtonChangeTrainingParameters() 
{
	// TODO: Add your command handler code here
	ChangeNetTrainingParameters();
	
}




void CNNDoc::OnButtonChangeNetworkType() 
{
	// TODO: Add your command handler code here
	ChangeNetworkType(NULL);

}

void CNNDoc::OnButtonTriangulateNet() 
{
	// TODO: Add your command handler code here

	//for (double NormalCompatibility=0.9;NormalCompatibility>=0.0; NormalCompatibility-=0.1){
	for (double NormalCompatibility=0.0;NormalCompatibility<=1.0; NormalCompatibility+=0.1){
		StartedTriangulation=false;
		for (int i=0; i<m_NNet.Neurons.Size;i++){
			m_NNet.TriangulateNet(m_PtLst,NormalCompatibility);
			this->UpdateAllViews(NULL);
		}
	}
	// finish the work by allowing bad projections
	StartedTriangulation=false;
}

void CNNDoc::ChangeNetworkType(LPCTSTR FileName)
{
	bool NetworkValid;
	CInitNeuralNetworkDialog InitDlg;
	// call the dialog box until the user enters valid values

	//**** cont here :
	//1. disable fields that are not allowed to change
	//2. call the appropriate functions needed from one change to another (reset point weight and deleted unconnected neurons)
	//or 
	//3. just do not implement forbidden changes
	
	// init network type by current network.for further processing
	InitDlg.m_NNInitNetworkType = m_NNet.Tags ;
	InitDlg.m_NNInitNeuronNumberU = m_NNet.NetworkSizeU;
	InitDlg.m_NNInitNeuronNumberV = m_NNet.NetworkSizeV;
	
	do{
		if (InitDlg.DoModal() != IDOK) 
			return; //modification canceled
		NetworkValid = m_NNet.CheckNetworkTypeValidity(InitDlg.m_NNInitNetworkType);
		if (!NetworkValid) {
			MessageBox(NULL,"This Type of Network entered is invalid or unrecognized - reneter network parameters",NULL,MB_OK);
			continue;
		}

		if  (m_NNet.Tags & NN_NOT_INITIALIZED){
			// initialize the network.
			m_NNet.InitNeuralNet(InitDlg.m_NNInitNeuronNumberU,InitDlg.m_NNInitNeuronNumberV, m_PtLst, InitDlg.m_NNInitNetworkType,FileName);
			NetworkValid=true; // exit loop
			continue;
		}
		else{
			NetworkValid = m_NNet.ChangeNetType(InitDlg.m_NNInitNetworkType,m_PtLst);
			if (!NetworkValid){
				MessageBox(NULL,"This Type of Network entered is invalid, unrecognized or impossible for change - reneter network parameters",NULL,MB_OK);
				continue;
			}
		}
	} while (!NetworkValid);

}

void CNNDoc::OnButtonCloseLoops() 
{
	// TODO: Add your command handler code here
	m_NNet.CloseBoundary();
	this->UpdateAllViews(NULL);

}

void CNNDoc::OnButtonDeleteNonManifoldNeurons() 
{
	// TODO: Add your command handler code here
	m_NNet.DeleteNonManifoldEdges();
	m_NNet.DeleteNonManifoldNeurons();
	this->UpdateAllViews(NULL);
}

void CNNDoc::OnButtonDetectNonManifoldNeurons() 
{
	// TODO: Add your command handler code here
	m_NNet.MarkNonManifoldMeshNeurons();
	this->UpdateAllViews(NULL);
}

void CNNDoc::OnButtonCalcNormalsUsingLsq() 
{
	// TODO: Add your command handler code here

	m_NNet.CalcNetNormalsUsingLSQ(10,m_PtLst);
	this->UpdateAllViews(NULL);

}

void CNNDoc::OnButtonDeleteNonManifoldFaces(){
	// the use another threshold to delete non manifolds
	m_NNet.DeleteNonManifoldFaces(0.8);
	// first delete problemaic faces
	m_NNet.DeleteNonManifoldFaces(true);
	//m_NNet.ClassifySamplePointList(m_PtLst);
	this->UpdateAllViews(NULL);

}

void CNNDoc::OnButtonInitiateEnhancedHebbianLearning(){

	// delete all edges and face
	m_NNet.Faces.DeleteOld(-1);
	m_NNet.Edges.DeleteOld(-1);

	// set needed parameters and call hebbian learning
	m_NetTrainingMethod=NN_TRAINING_METHOD_NEURAL_GAS;
	m_NGRunLength=m_PtLst.Size * 2;
	m_CurrentIteration=0;
	m_NGFinalEps=0.0;  // this is enough to cause eps to be zero
	m_NGEdgeExperationAgeInit = m_NGRunLength +1;
	m_NGEdgeExperationAgeFinal = m_NGRunLength +1;
	m_NGEdgeExperationAgeNextChunk = m_NGRunLength +1;
	

	// now train continously

	OnButtonTrainNetContinous();


}

void CNNDoc::OnButtonResetNormalDirections(){
	m_NNet.FlipFaceAndVertexNormalsForAgreement();
	this->UpdateAllViews(NULL);
}

void CNNDoc::WaitUntilThreadTerminates(void){
	m_TerminateThread = true;
	//wait until no more training is performed by locking critical section - 
	//this way we will end until the end of the current cunk training in the thread and the thread will exit
	m_UpdatingListsNow.Lock();
	// this will give the thread enough time to exit
	::Sleep(0);
	m_UpdatingListsNow.Unlock();

}


void CNNDoc::OnButtonResetNeuronPositionsToRandomSamplePoint(){
	m_NNet.ResetNeuronPositions(RESET_METHOD_RANDOM_FROM_SAMPLED_POINTS,m_PtLst);
	this->UpdateAllViews(NULL);
}

void CNNDoc::OnButtonResetNeuronPositionsToNearestSamplePoint(){
	m_NNet.ResetNeuronPositions(RESET_METHOD_BY_NEAREST_CLASSIFIED_POINT_WITH_DELETIONS,m_PtLst);
	this->UpdateAllViews(NULL);
}
