// GrowinNeuralGasPage.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "GrowinNeuralGasPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrowinNeuralGasPage property page

IMPLEMENT_DYNCREATE(CGrowinNeuralGasPage, CPropertyPage)

CGrowinNeuralGasPage::CGrowinNeuralGasPage() : CPropertyPage(CGrowinNeuralGasPage::IDD)
{
	//{{AFX_DATA_INIT(CGrowinNeuralGasPage)
	m_GNGAgeMax = 0;
	m_GNGAlpha = 0.0;
	m_GNGBetta = 0.0;
	m_GNGChunkSize = 0;
	m_GNGEpsB = 0.0;
	m_GNGEpsN = 0.0;
	m_GNGRunLength = 0;
	m_GNGNeuronNumber = 0;
	m_GNGLambdaGrowth = 0;
	//}}AFX_DATA_INIT
}

CGrowinNeuralGasPage::~CGrowinNeuralGasPage()
{
}

void CGrowinNeuralGasPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrowinNeuralGasPage)
	DDX_Text(pDX, IDC_GNG_AGE_MAX, m_GNGAgeMax);
	DDV_MinMaxInt(pDX, m_GNGAgeMax, 1, 100000);
	DDX_Text(pDX, IDC_GNG_ALPHA, m_GNGAlpha);
	DDV_MinMaxDouble(pDX, m_GNGAlpha, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GNG_BETTA, m_GNGBetta);
	DDV_MinMaxDouble(pDX, m_GNGBetta, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GNG_CHUNK_SIZE, m_GNGChunkSize);
	DDV_MinMaxInt(pDX, m_GNGChunkSize, 1, 10000);
	DDX_Text(pDX, IDC_GNG_EPS_B, m_GNGEpsB);
	DDV_MinMaxDouble(pDX, m_GNGEpsB, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GNG_EPS_N, m_GNGEpsN);
	DDV_MinMaxDouble(pDX, m_GNGEpsN, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GNG_RUN_LENGTH, m_GNGRunLength);
	DDV_MinMaxInt(pDX, m_GNGRunLength, 1, 100000000);
	DDX_Text(pDX, IDC_GNG_NEURON_NUMBER, m_GNGNeuronNumber);
	DDV_MinMaxInt(pDX, m_GNGNeuronNumber, 2, 10000);
	DDX_Text(pDX, IDC_GNG_LAMBDA_GROWTH, m_GNGLambdaGrowth);
	DDV_MinMaxInt(pDX, m_GNGLambdaGrowth, 1, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrowinNeuralGasPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGrowinNeuralGasPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrowinNeuralGasPage message handlers
