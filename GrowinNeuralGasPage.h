#if !defined(AFX_GROWINNEURALGASPAGE_H__CD68F3C2_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
#define AFX_GROWINNEURALGASPAGE_H__CD68F3C2_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrowinNeuralGasPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrowinNeuralGasPage dialog

class CGrowinNeuralGasPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGrowinNeuralGasPage)

// Construction
public:
	CGrowinNeuralGasPage();
	~CGrowinNeuralGasPage();

// Dialog Data
	//{{AFX_DATA(CGrowinNeuralGasPage)
	enum { IDD = IDD_GROWING_NEURAL_GAS_PAGE };
	int		m_GNGAgeMax;
	double	m_GNGAlpha;
	double	m_GNGBetta;
	int		m_GNGChunkSize;
	double	m_GNGEpsB;
	double	m_GNGEpsN;
	int		m_GNGRunLength;
	int		m_GNGNeuronNumber;
	int		m_GNGLambdaGrowth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGrowinNeuralGasPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGrowinNeuralGasPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROWINNEURALGASPAGE_H__CD68F3C2_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
