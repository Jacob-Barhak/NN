// TrainNetDialogPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "TrainNetDialogPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainNetDialogPropSheet

IMPLEMENT_DYNAMIC(CTrainNetDialogPropSheet, CPropertySheet)

CTrainNetDialogPropSheet::CTrainNetDialogPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTrainNetDialogPropSheet::CTrainNetDialogPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&FeatureMapPage);
	AddPage(&NeuralGasPage);
	AddPage(&GrowinNeuralGasPage);
//	AddPage(&GrowingGridPage); - transfred to SOFM
	// init to first page
	m_NetTrainingMethod = NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP;
}

CTrainNetDialogPropSheet::~CTrainNetDialogPropSheet()
{

	RemovePage(&FeatureMapPage);
	RemovePage(&NeuralGasPage);
	RemovePage(&GrowinNeuralGasPage);

}


BEGIN_MESSAGE_MAP(CTrainNetDialogPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTrainNetDialogPropSheet)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainNetDialogPropSheet message handlers



BOOL CTrainNetDialogPropSheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class

	NMHDR* pnmh = (NMHDR*)lParam;
	// tab has been changed
	if (TCN_SELCHANGE == pnmh->code){
		// get the current page index
		int NetTrainingMethod = GetActiveIndex();
		switch (NetTrainingMethod){
		case 0:
			m_NetTrainingMethod=NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP;
		break;
		case 1:
			m_NetTrainingMethod=NN_TRAINING_METHOD_NEURAL_GAS;
		break;
		case 2:
			m_NetTrainingMethod=NN_TRAINING_METHOD_GROWING_NEURAL_GAS;
		break;
		default:
			m_NetTrainingMethod=NN_TRAINING_METHOD_NOT_SELECTED;
		}
	}
      	
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}
