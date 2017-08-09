// GrowingGridPage.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "GrowingGridPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrowingGridPage property page

IMPLEMENT_DYNCREATE(CGrowingGridPage, CPropertyPage)

CGrowingGridPage::CGrowingGridPage() : CPropertyPage(CGrowingGridPage::IDD)
{
	//{{AFX_DATA_INIT(CGrowingGridPage)
	m_GGChunkSize = 0;
	m_GGFinalEps = 0.0;
	m_GGInitEps = 0.0;
	m_GGLambdaGrowth = 0;
	m_GGNeuronNumberU = 0;
	m_GGNeuronNumberV = 0;
	m_GGRadiusFinal = 0.0;
	m_GGRadiusInit = 0.0;
	m_GGRunLength = 0;
	m_GGSigma = 0.0;
	m_GGGridCanGrow = FALSE;
	m_GGCyclicUDir = FALSE;
	m_GGCyclicVDir = FALSE;
	m_GGBoundaryCorrectionEvery=0;
	m_GGBoundaryCorrectionPointWeight=0.0;
	m_GGBoundaryCorrectionRunLength=0;

	//}}AFX_DATA_INIT
}

CGrowingGridPage::~CGrowingGridPage()
{
}

void CGrowingGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrowingGridPage)
	DDX_Text(pDX, IDC_GG_CHUNK_SIZE, m_GGChunkSize);
	DDV_MinMaxInt(pDX, m_GGChunkSize, 1, 10000);
	DDX_Text(pDX, IDC_GG_FINAL_EPS, m_GGFinalEps);
	DDV_MinMaxDouble(pDX, m_GGFinalEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GG_INIT_EPS, m_GGInitEps);
	DDV_MinMaxDouble(pDX, m_GGInitEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_GG_LAMBDA_GROWTH, m_GGLambdaGrowth);
	DDV_MinMaxInt(pDX, m_GGLambdaGrowth, 1, 100000);
	DDX_Text(pDX, IDC_GG_NEURON_NUMBER_U, m_GGNeuronNumberU);
	DDV_MinMaxInt(pDX, m_GGNeuronNumberU, 3, 100);
	DDX_Text(pDX, IDC_GG_NEURON_NUMBER_V, m_GGNeuronNumberV);
	DDV_MinMaxInt(pDX, m_GGNeuronNumberV, 3, 100);
	DDX_Text(pDX, IDC_GG_RADIUS_FINAL, m_GGRadiusFinal);
	DDV_MinMaxDouble(pDX, m_GGRadiusFinal, 0., 100.);
	DDX_Text(pDX, IDC_GG_RADIUS_INIT, m_GGRadiusInit);
	DDV_MinMaxDouble(pDX, m_GGRadiusInit, 0., 100.);
	DDX_Text(pDX, IDC_GG_RUN_LENGTH, m_GGRunLength);
	DDV_MinMaxInt(pDX, m_GGRunLength, 1, 1000000);
	DDX_Text(pDX, IDC_GG_SIGMA, m_GGSigma);
	DDV_MinMaxDouble(pDX, m_GGSigma, 1.e-010, 100.);
	DDX_Check(pDX, IDC_GG_GRID_CAD_GROW, m_GGGridCanGrow);
	DDX_Check(pDX, IDC_GG_CYCLIC_U, m_GGCyclicUDir);
	DDX_Check(pDX, IDC_GG_CYCLIC_V, m_GGCyclicVDir);
	DDX_Text(pDX, IDC_GG_BOUNDARY_CORRECTION_EVERY, m_GGBoundaryCorrectionEvery);
	DDV_MinMaxInt(pDX, m_GGBoundaryCorrectionEvery, 0, 1000000);
	DDX_Text(pDX, IDC_GG_BOUNDARY_CORRECTION_POINT_WEIGHT, m_GGBoundaryCorrectionPointWeight);
	DDV_MinMaxDouble(pDX, m_GGBoundaryCorrectionPointWeight, 0., 10000000000.);
	DDX_Text(pDX, IDC_GG_BOUNDARY_CORRECTION_RUNLENGTH, m_GGBoundaryCorrectionRunLength);
	DDV_MinMaxInt(pDX, m_GGBoundaryCorrectionRunLength, 0, 1000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrowingGridPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGrowingGridPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrowingGridPage message handlers
