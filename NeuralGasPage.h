#if !defined(AFX_NEURALGASPAGE_H__CD68F3C0_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
#define AFX_NEURALGASPAGE_H__CD68F3C0_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NeuralGasPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNeuralGasPage dialog

class CNeuralGasPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNeuralGasPage)

// Construction
public:
	CNeuralGasPage();
	~CNeuralGasPage();

// Dialog Data
	//{{AFX_DATA(CNeuralGasPage)
	enum { IDD = IDD_NEURAL_GAS_PAGE };
	int		m_NGChunkSize;
	int		m_NGEdgeExperationAgeFinal;
	double	m_NGFinalEps;
	double	m_NGFinalLambda;
	double	m_NGInitEps;
	double	m_NGInitLambda;
	int		m_NGEdgeExperationAgeInit;
	int		m_NGNeuronNumber;
	int		m_NGRunLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNeuralGasPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNeuralGasPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEURALGASPAGE_H__CD68F3C0_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
