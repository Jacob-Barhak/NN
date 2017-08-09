// ViewParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "ViewParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewParamDlg dialog


CViewParamDlg::CViewParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewParamDlg)
	m_NeuronSize = 0.0;
	m_NormalSize = 0.0;
	m_PointSize = 0.0;
	m_DrawAxes = FALSE;
	m_DrawEdges = FALSE;
	m_DrawNeurons = FALSE;
	m_DrawPoints = FALSE;
	m_DrawFaces = FALSE;
	//}}AFX_DATA_INIT
}


void CViewParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewParamDlg)
	DDX_Text(pDX, IDC_NEURON_SIZE, m_NeuronSize);
	DDV_MinMaxDouble(pDX, m_NeuronSize, 1.e-005, 100000.);
	DDX_Text(pDX, IDC_NORMAL_SIZE, m_NormalSize);
	DDV_MinMaxDouble(pDX, m_NormalSize, 0., 100000.);
	DDX_Text(pDX, IDC_POINT_SIZE, m_PointSize);
	DDV_MinMaxDouble(pDX, m_PointSize, 1.e-005, 100000.);
	DDX_Check(pDX, IDC_DRAW_AXES, m_DrawAxes);
	DDX_Check(pDX, IDC_DRAW_EDGES, m_DrawEdges);
	DDX_Check(pDX, IDC_DRAW_NEURONS, m_DrawNeurons);
	DDX_Check(pDX, IDC_DRAW_POINTS, m_DrawPoints);
	DDX_Check(pDX, IDC_DRAW_FACES, m_DrawFaces);
	DDX_Check(pDX, IDC_LIGHTING_TWO_SIDES, m_LightingTwoSides);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewParamDlg, CDialog)
	//{{AFX_MSG_MAP(CViewParamDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DRAW_FACES2, OnBnClickedDrawFaces2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewParamDlg message handlers

void CViewParamDlg::OnBnClickedDrawFaces2()
{
	// TODO: Add your control notification handler code here
}
