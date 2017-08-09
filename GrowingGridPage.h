#if !defined(AFX_GROWINGGRIDPAGE_H__DD18EE40_1E3B_11D5_913F_FFEA8D5FD179__INCLUDED_)
#define AFX_GROWINGGRIDPAGE_H__DD18EE40_1E3B_11D5_913F_FFEA8D5FD179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrowingGridPage.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CGrowingGridPage dialog

class CGrowingGridPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGrowingGridPage)

// Construction
public:
	CGrowingGridPage();
	~CGrowingGridPage();

// Dialog Data
	//{{AFX_DATA(CGrowingGridPage)
	enum { IDD = IDD_GROWING_GRID_PAGE };
	int		m_GGChunkSize;
	double	m_GGFinalEps;
	double	m_GGInitEps;
	int		m_GGLambdaGrowth;
	int		m_GGNeuronNumberU;
	int		m_GGNeuronNumberV;
	double	m_GGRadiusFinal;
	double	m_GGRadiusInit;
	int		m_GGRunLength;
	double	m_GGSigma;
	BOOL	m_GGGridCanGrow;
	BOOL	m_GGCyclicUDir;
	BOOL	m_GGCyclicVDir;
	int		m_GGBoundaryCorrectionEvery;
	double	m_GGBoundaryCorrectionPointWeight;
	int		m_GGBoundaryCorrectionRunLength;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGrowingGridPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGrowingGridPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROWINGGRIDPAGE_H__DD18EE40_1E3B_11D5_913F_FFEA8D5FD179__INCLUDED_)
