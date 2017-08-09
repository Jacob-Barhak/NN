// NNDoc.h : interface of the CNNDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NNDOC_H__26A63D2C_6CBB_11D4_9E62_EF8715B56461__INCLUDED_)
#define AFX_NNDOC_H__26A63D2C_6CBB_11D4_9E62_EF8715B56461__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNet.h"

#define WM_USER_NN_CHUNK_CALC_FINISHED (WM_USER + 1)


class CNNDoc : public CDocument 
{
protected: // create from serialization only
	CNNDoc();
	DECLARE_DYNCREATE(CNNDoc)

// Attributes
public:
	PointList m_PtLst;
	NeuralNet m_NNet;


	int m_CurrentIteration;
	int m_NetTrainingMethod;

	//growing neural gas
	int		m_GNGAgeMax;
	double	m_GNGAlpha;
	double	m_GNGBetta;
	int		m_GNGChunkSize;
	double	m_GNGEpsB;
	double	m_GNGEpsN;
	int		m_GNGRunLength;
	int		m_GNGNeuronNumber;
	int		m_GNGLambdaGrowth;

	// Neural Gas
	int		m_NGChunkSize;
	int		m_NGEdgeExperationAgeFinal;
	double	m_NGFinalEps;
	double	m_NGFinalLambda;
	double	m_NGInitEps;
	double	m_NGInitLambda;
	int		m_NGEdgeExperationAgeInit;
	int		m_NGNeuronNumber;
	int		m_NGRunLength;

	int		m_NGEdgeExperationAgeNextChunk;
	double	m_NGEpsNextChunk;
	double	m_NGLambdaNextChunk;


	// self organizing feature map
	int		m_SOFMChunkSize;
	double	m_SOFMFinalEps;
	double	m_SOFMInitEps;
	double	m_SOFMRadiusFinal;
	double	m_SOFMRadiusInit;
	int		m_SOFMRunLength;
	double	m_SOFMSigmaFinal;
	double	m_SOFMSigmaInit;

	int m_SOFMNeuronNumberU;
	int m_SOFMNeuronNumberV;

	BOOL m_SOFMGridCanGrow;
	BOOL m_SOFMCyclicUDir;
	BOOL m_SOFMCyclicVDir;

	double	m_SOFMEpsNextChunk;
	double	m_SOFMRadiusNextChunk;
	double	m_SOFMSigmaNextChunk;

	int		m_SOFMBoundaryCorrectionEvery;
	double	m_SOFMBoundaryCorrectionPointWeight;
	int		m_SOFMBoundaryCorrectionRunLength;

	int		m_SOFMLambdaGrowth;


	CCriticalSection m_UpdatingListsNow;
	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ChangeNetTrainingParameters();
	LRESULT OnFinishedTrainingChunk(WPARAM wParam, LPARAM lParam);
	void TrainNetOneChunk();
	void TrainNetContinously();
	virtual ~CNNDoc();
	
	friend UINT TrainInThread(LPVOID pParam);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNNDoc)
	afx_msg void OnButtonPauseNetTraining();
	afx_msg void OnButtonTrainNetContinous();
	afx_msg void OnButtonTrainNetOneStep();
	afx_msg void OnButtonChangeTrainingParameters();
	afx_msg void OnButtonChangeNetworkType();
	afx_msg void OnButtonTriangulateNet();
	afx_msg void OnButtonCloseLoops();
	afx_msg void OnButtonDeleteNonManifoldNeurons();
	afx_msg void OnButtonDetectNonManifoldNeurons();
	afx_msg void OnButtonCalcNormalsUsingLsq();
	afx_msg void OnButtonDeleteNonManifoldFaces();
	afx_msg void OnButtonInitiateEnhancedHebbianLearning();
	afx_msg void OnButtonResetNormalDirections();
	afx_msg void OnButtonResetNeuronPositionsToRandomSamplePoint();
	afx_msg void OnButtonResetNeuronPositionsToNearestSamplePoint();


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL m_TrainingNetNow;
	BOOL m_TrainingContinously;
	BOOL m_TerminateThread;
private:
	void ChangeNetworkType(LPCTSTR FileName);
public:
	void WaitUntilThreadTerminates(void);
};

/////////////////////////////////////////////////////////////////////////////



// global decleration
UINT TrainInThread(LPVOID pParam);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NNDOC_H__26A63D2C_6CBB_11D4_9E62_EF8715B56461__INCLUDED_)
