// NeuralGasPage.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "NeuralGasPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNeuralGasPage property page

IMPLEMENT_DYNCREATE(CNeuralGasPage, CPropertyPage)

CNeuralGasPage::CNeuralGasPage() : CPropertyPage(CNeuralGasPage::IDD)
{
	//{{AFX_DATA_INIT(CNeuralGasPage)
	m_NGChunkSize = 0;
	m_NGEdgeExperationAgeFinal = 0;
	m_NGFinalEps = 0.0;
	m_NGFinalLambda = 0.0;
	m_NGInitEps = 0.0;
	m_NGInitLambda = 0.0;
	m_NGEdgeExperationAgeInit = 0;
	m_NGNeuronNumber = 0;
	m_NGRunLength = 0;
	//}}AFX_DATA_INIT
}

CNeuralGasPage::~CNeuralGasPage()
{
}

void CNeuralGasPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNeuralGasPage)
	DDX_Text(pDX, IDC_NG_CHUNK_SIZE, m_NGChunkSize);
	DDV_MinMaxInt(pDX, m_NGChunkSize, 1, 100000);
	DDX_Text(pDX, IDC_NG_EDGE_EXPERATION_AGE_FINAL, m_NGEdgeExperationAgeFinal);
	DDV_MinMaxInt(pDX, m_NGEdgeExperationAgeFinal, 1, 1000000);
	DDX_Text(pDX, IDC_NG_FINAL_EPS, m_NGFinalEps);
	DDV_MinMaxDouble(pDX, m_NGFinalEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_NG_FINAL_LAMBDA, m_NGFinalLambda);
	DDV_MinMaxDouble(pDX, m_NGFinalLambda, 1.e-010, 100.);
	DDX_Text(pDX, IDC_NG_INIT_EPS, m_NGInitEps);
	DDV_MinMaxDouble(pDX, m_NGInitEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_NG_INIT_LAMBDA, m_NGInitLambda);
	DDV_MinMaxDouble(pDX, m_NGInitLambda, 1.e-010, 100.);
	DDX_Text(pDX, IDC_NG_EDGE_EXPERATION_AGE_INIT, m_NGEdgeExperationAgeInit);
	DDV_MinMaxInt(pDX, m_NGEdgeExperationAgeInit, 1, 1000000);
	DDX_Text(pDX, IDC_NG_NEURON_NUMBER, m_NGNeuronNumber);
	DDV_MinMaxInt(pDX, m_NGNeuronNumber, 10, 100000);
	DDX_Text(pDX, IDC_NG_RUN_LENGTH, m_NGRunLength);
	DDV_MinMaxInt(pDX, m_NGRunLength, 1, 10000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNeuralGasPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNeuralGasPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNeuralGasPage message handlers
