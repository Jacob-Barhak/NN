#if !defined(AFX_NETTRAINDLG_H__CE5303E2_6DA7_11D4_9E62_F79F642E583A__INCLUDED_)
#define AFX_NETTRAINDLG_H__CE5303E2_6DA7_11D4_9E62_F79F642E583A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetTrainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetTrainDlg dialog

class CNetTrainDlg : public CDialog
{
// Construction
public:
	CNetTrainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetTrainDlg)
	enum { IDD = IDD_DIALOG_NET_TRAIN };
	int		m_ChunkSize;
	int		m_NeuronNumber;
	int		m_RunLength;
	double	m_EpsFinal;
	double	m_LambdaFinal;
	double	m_EpsInit;
	double	m_LambdaInit;
	int		m_EdgeExperationAgeFinal;
	int		m_EdgeExperationAgeInit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetTrainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetTrainDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETTRAINDLG_H__CE5303E2_6DA7_11D4_9E62_F79F642E583A__INCLUDED_)
