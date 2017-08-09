// NetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "NetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetDialog dialog


CNetDialog::CNetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetDialog, CDialog)
	//{{AFX_MSG_MAP(CNetDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetDialog message handlers
