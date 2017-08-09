// SelfOrganizingFeatureMapPage.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "SelfOrganizingFeatureMapPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelfOrganizingFeatureMapPage property page

IMPLEMENT_DYNCREATE(CSelfOrganizingFeatureMapPage, CPropertyPage)

CSelfOrganizingFeatureMapPage::CSelfOrganizingFeatureMapPage() : CPropertyPage(CSelfOrganizingFeatureMapPage::IDD)
{
	//{{AFX_DATA_INIT(CSelfOrganizingFeatureMapPage)
	m_SOFMChunkSize = 0;
	m_SOFMFinalEps = 0.0;
	m_SOFMInitEps = 0.0;
	m_SOFMRadiusFinal = 0.0;
	m_SOFMRadiusInit = 0.0;
	m_SOFMRunLength = 0;
	m_SOFMSigmaFinal = 0.0;
	m_SOFMSigmaInit = 0.0;
	m_SOFMBoundaryCorrectionEvery = 0;
	m_SOFMBoundaryCorrectionPointWeight = 0.0;
	m_SOFMBoundaryCorrectionRunLength = 0;
	m_SOFMLambdaGrowth = 0;


	m_SOFMNeuronNumberU=0;
	m_SOFMNeuronNumberV=0;

	m_SOFMGridCanGrow=FALSE;
	m_SOFMCyclicUDir=FALSE;
	m_SOFMCyclicVDir=FALSE;

	//}}AFX_DATA_INIT
}

CSelfOrganizingFeatureMapPage::~CSelfOrganizingFeatureMapPage()
{
}

void CSelfOrganizingFeatureMapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelfOrganizingFeatureMapPage)
	DDX_Text(pDX, IDC_SOFM_CHUNK_SIZE, m_SOFMChunkSize);
	DDV_MinMaxInt(pDX, m_SOFMChunkSize, 1, 10000);
	DDX_Text(pDX, IDC_SOFM_FINAL_EPS, m_SOFMFinalEps);
	DDV_MinMaxDouble(pDX, m_SOFMFinalEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_SOFM_INIT_EPS, m_SOFMInitEps);
	DDV_MinMaxDouble(pDX, m_SOFMInitEps, 1.e-010, 1.);
	DDX_Text(pDX, IDC_SOFM_RADIUS_FINAL, m_SOFMRadiusFinal);
	DDV_MinMaxDouble(pDX, m_SOFMRadiusFinal, 0., 1000.);
	DDX_Text(pDX, IDC_SOFM_RADIUS_INIT, m_SOFMRadiusInit);
	DDV_MinMaxDouble(pDX, m_SOFMRadiusInit, 0., 1000.);
	DDX_Text(pDX, IDC_SOFM_RUN_LENGTH, m_SOFMRunLength);
	DDV_MinMaxInt(pDX, m_SOFMRunLength, 1, 1000000);
	DDX_Text(pDX, IDC_SOFM_SIGMA_FINAL, m_SOFMSigmaFinal);
	DDV_MinMaxDouble(pDX, m_SOFMSigmaFinal, 1.e-010, 100.);
	DDX_Text(pDX, IDC_SOFM_SIGMA_INIT, m_SOFMSigmaInit);
	DDV_MinMaxDouble(pDX, m_SOFMSigmaInit, 1.e-010, 100.);
	DDX_Text(pDX, IDC_SOFM_BOUNDARY_CORRECTION_EVERY, m_SOFMBoundaryCorrectionEvery);
	DDV_MinMaxInt(pDX, m_SOFMBoundaryCorrectionEvery, 0, 1000000);
	DDX_Text(pDX, IDC_SOFM_BOUNDARY_CORRECTION_POINT_WEIGHT, m_SOFMBoundaryCorrectionPointWeight);
	DDV_MinMaxDouble(pDX, m_SOFMBoundaryCorrectionPointWeight, 0., 10000000000.);
	DDX_Text(pDX, IDC_SOFM_BOUNDARY_CORRECTION_RUNLENGTH, m_SOFMBoundaryCorrectionRunLength);
	DDV_MinMaxInt(pDX, m_SOFMBoundaryCorrectionRunLength, 0, 100000);
	DDX_Text(pDX, IDC_SOFM_LAMBDA_GROWTH, m_SOFMLambdaGrowth);
	DDV_MinMaxInt(pDX, m_SOFMLambdaGrowth, 0, 100000);
	DDX_Text(pDX, IDC_SOFM_NEURON_NUMBER_U, m_SOFMNeuronNumberU);
	DDV_MinMaxInt(pDX, m_SOFMNeuronNumberU, 3, 100);
	DDX_Text(pDX, IDC_SOFM_NEURON_NUMBER_V, m_SOFMNeuronNumberV);
	DDV_MinMaxInt(pDX, m_SOFMNeuronNumberV, 3, 100);
	DDX_Check(pDX, IDC_SOFM_GRID_CAN_GROW, m_SOFMGridCanGrow);
	DDX_Check(pDX, IDC_SOFM_CYCLIC_U, m_SOFMCyclicUDir);
	DDX_Check(pDX, IDC_SOFM_CYCLIC_V, m_SOFMCyclicVDir);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelfOrganizingFeatureMapPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSelfOrganizingFeatureMapPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelfOrganizingFeatureMapPage message handlers
