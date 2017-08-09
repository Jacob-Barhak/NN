#if !defined(AFX_SELFORGANIZINGFEATUREMAPPAGE_H__CD68F3C3_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
#define AFX_SELFORGANIZINGFEATUREMAPPAGE_H__CD68F3C3_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelfOrganizingFeatureMapPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelfOrganizingFeatureMapPage dialog

class CSelfOrganizingFeatureMapPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSelfOrganizingFeatureMapPage)

// Construction
public:
	CSelfOrganizingFeatureMapPage();
	~CSelfOrganizingFeatureMapPage();

// Dialog Data
	//{{AFX_DATA(CSelfOrganizingFeatureMapPage)
	enum { IDD = IDD_SELF_ORGANIZING_FEATURE_MAP_PAGE };
	int		m_SOFMChunkSize;
	double	m_SOFMFinalEps;
	double	m_SOFMInitEps;
	double	m_SOFMRadiusFinal;
	double	m_SOFMRadiusInit;
	int		m_SOFMRunLength;
	double	m_SOFMSigmaFinal;
	double	m_SOFMSigmaInit;
	int		m_SOFMBoundaryCorrectionEvery;
	double	m_SOFMBoundaryCorrectionPointWeight;
	int		m_SOFMBoundaryCorrectionRunLength;
	int		m_SOFMLambdaGrowth;
	int		m_SOFMNeuronNumberU;
	int		m_SOFMNeuronNumberV;

	BOOL	m_SOFMGridCanGrow;
	BOOL	m_SOFMCyclicUDir;
	BOOL	m_SOFMCyclicVDir;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSelfOrganizingFeatureMapPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSelfOrganizingFeatureMapPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELFORGANIZINGFEATUREMAPPAGE_H__CD68F3C3_1DF8_11D5_913F_FFEA8D5FD179__INCLUDED_)
