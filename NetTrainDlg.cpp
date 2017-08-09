// NetTrainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "NetTrainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetTrainDlg dialog


CNetTrainDlg::CNetTrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetTrainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetTrainDlg)
	m_ChunkSize = 0;
	m_NeuronNumber = 0;
	m_RunLength = 0;
	m_EpsFinal = 0.0;
	m_LambdaFinal = 0.0;
	m_EpsInit = 0.0;
	m_LambdaInit = 0.0;
	m_EdgeExperationAgeFinal = 0;
	m_EdgeExperationAgeInit = 0;
	//}}AFX_DATA_INIT
}


void CNetTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetTrainDlg)
	DDX_Text(pDX, IDC_CHUNK_SIZE, m_ChunkSize);
	DDV_MinMaxInt(pDX, m_ChunkSize, 1, 10000);
	DDX_Text(pDX, IDC_NEURON_NUMBER, m_NeuronNumber);
	DDV_MinMaxInt(pDX, m_NeuronNumber, 10, 100000);
	DDX_Text(pDX, IDC_RUN_LENGTH, m_RunLength);
	DDV_MinMaxInt(pDX, m_RunLength, 10, 10000000);
	DDX_Text(pDX, IDC_FINAL_EPS, m_EpsFinal);
	DDV_MinMaxDouble(pDX, m_EpsFinal, 1.e-008, 10.);
	DDX_Text(pDX, IDC_FINAL_LAMBDA, m_LambdaFinal);
	DDV_MinMaxDouble(pDX, m_LambdaFinal, 1.e-008, 100.);
	DDX_Text(pDX, IDC_INIT_EPS, m_EpsInit);
	DDV_MinMaxDouble(pDX, m_EpsInit, 1.e-008, 10.);
	DDX_Text(pDX, IDC_INIT_LAMBDA, m_LambdaInit);
	DDV_MinMaxDouble(pDX, m_LambdaInit, 1.e-008, 100.);
	DDX_Text(pDX, IDC_EDGE_EXPERATION_AGE_FINAL, m_EdgeExperationAgeFinal);
	DDV_MinMaxInt(pDX, m_EdgeExperationAgeFinal, 1, 100000);
	DDX_Text(pDX, IDC_EDGE_EXPERATION_AGE_INIT, m_EdgeExperationAgeInit);
	DDV_MinMaxInt(pDX, m_EdgeExperationAgeInit, 1, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetTrainDlg, CDialog)
	//{{AFX_MSG_MAP(CNetTrainDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetTrainDlg message handlers

void CNetTrainDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (m_LambdaInit<m_LambdaFinal){
		MessageBox("Initial Lambda must be greater the Final Lambda");
		return;
	}

	if (m_EpsInit<m_EpsFinal){
		MessageBox("Initial Epsilon must be greater the Final Epsilon");
		return;
	}

	if (m_RunLength<m_ChunkSize){
		MessageBox("Display chunk size should be smaller then the run length");
		return;
	}
	

	CDialog::OnOK();
}
