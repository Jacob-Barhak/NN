#if !defined(AFX_INITNEURALNETWORKDIALOG_H__5EB382C0_40A2_11D5_913F_E076C6FCC179__INCLUDED_)
#define AFX_INITNEURALNETWORKDIALOG_H__5EB382C0_40A2_11D5_913F_E076C6FCC179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitNeuralNetworkDialog.h : header file
//

typedef enum {
	INIT_NN_DLG_TOPOLOGY_RECTANGULAR=0,
	INIT_NN_DLG_TOPOLOGY_NONE,
	INIT_NN_DLG_TOPOLOGY_OTHER
} NNInitTopologyTypes;

typedef enum {
	INIT_NN_DLG_FAST_INIT_SELF_ORGANIZING_FEATURE_MAP=0,
	INIT_NN_DLG_FAST_INIT_NEURAL_GAS,
	INIT_NN_DLG_FAST_INIT_GROWING_NEURAL_GAS,
	INIT_NN_DLG_FAST_INIT_OTHER
} NNFastInitNetTypes;


#define MAX_CEDIT_TEXT_LENGTH 255
/////////////////////////////////////////////////////////////////////////////
// CInitNeuralNetworkDialog dialog

class CInitNeuralNetworkDialog : public CDialog
{
// Construction
public:
	CInitNeuralNetworkDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInitNeuralNetworkDialog)
	enum { IDD = IDD_DIALOG_INIT_NN };
	CEdit	m_NNInitCtlNeuronNumberV;
	CEdit	m_NNInitCtlNeuronNumberU;
	CButton	m_NNInitCtlNetCanGrowNeurons;
	CButton	m_NNInitCtlNetCanGrowEdges;
	CButton	m_NNInitCtlCyclicV;
	CButton	m_NNInitCtlCyclicU;
	CComboBox	m_NNInitCtlTopologyCombo;
	CComboBox	m_NNInitCtlFastInitTypeCombo;
	//}}AFX_DATA

	int	m_NNInitNeuronNumberU;
	int	m_NNInitNeuronNumberV;
	int m_NNInitNetworkType;
	bool m_AllowNetworkChange;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitNeuralNetworkDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInitNeuralNetworkDialog)
	afx_msg void OnSelchangeNnInitTopology();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeNnInitFastInitType();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITNEURALNETWORKDIALOG_H__5EB382C0_40A2_11D5_913F_E076C6FCC179__INCLUDED_)
