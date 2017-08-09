#if !defined(AFX_TRAINNETDIALOGPROPSHEET_H__42115D21_207E_11D5_913F_9200F8453E78__INCLUDED_)
#define AFX_TRAINNETDIALOGPROPSHEET_H__42115D21_207E_11D5_913F_9200F8453E78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainNetDialogPropSheet.h : header file
//

#include "GrowingGridPage.h" 
#include "GrowinNeuralGasPage.h" 
#include "NeuralGasPage.h" 
#include "SelfOrganizingFeatureMapPage.h" 


enum NetTrainingMethods {
	NN_TRAINING_METHOD_NOT_SELECTED=-1,
	NN_TRAINING_METHOD_GROWING_GRID,
	NN_TRAINING_METHOD_NEURAL_GAS,
	NN_TRAINING_METHOD_GROWING_NEURAL_GAS,
	NN_TRAINING_METHOD_SELF_ORGANIZING_FEATURE_MAP
};

/////////////////////////////////////////////////////////////////////////////
// CTrainNetDialogPropSheet

class CTrainNetDialogPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTrainNetDialogPropSheet)

// Construction
public:
	CTrainNetDialogPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTrainNetDialogPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CGrowingGridPage GrowingGridPage;
	CGrowinNeuralGasPage GrowinNeuralGasPage;
	CNeuralGasPage NeuralGasPage;
	CSelfOrganizingFeatureMapPage FeatureMapPage;
	int m_NetTrainingMethod;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainNetDialogPropSheet)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTrainNetDialogPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrainNetDialogPropSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINNETDIALOGPROPSHEET_H__42115D21_207E_11D5_913F_9200F8453E78__INCLUDED_)
