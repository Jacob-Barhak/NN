// NNView.h : interface of the CNNView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NNVIEW_H__26A63D2E_6CBB_11D4_9E62_EF8715B56461__INCLUDED_)
#define AFX_NNVIEW_H__26A63D2E_6CBB_11D4_9E62_EF8715B56461__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNet.h"


class CNNView : public CView 
{
protected: // create from serialization only
	CNNView();
	DECLARE_DYNCREATE(CNNView)

// Attributes
public:
	CNNDoc* GetDocument();

private:
	void DrawScene(CNNDoc* pDoc);
	void ReshapeFunc(int Wx, int Wy);
	HGLRC m_hrc;
	CPalette m_Pal;
	GLdouble m_ViewMat[16];
	CPoint m_MouseMoveStart;
	GLdouble *GenRotMatrix();
	GLdouble m_RotX;
	GLdouble m_RotY;
	GLdouble m_RotAngle;
	GLdouble m_MouseRotSensativity;
	GLdouble m_MouseTransSensativity;
	GLdouble m_MouseZoomSensativity;
	GLdouble m_Zoom;
	GLdouble m_TransX;
	GLdouble m_TransY;


public:
	GLdouble m_MinX, m_MinY, m_MinZ, m_MaxX, m_MaxY, m_MaxZ;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	RealType m_PointSize;
	RealType m_NeuronSize;
	RealType m_NormalSize;
	BOOL m_DrawAxes;
	BOOL m_DrawPoints;
	BOOL m_DrawNeurons;
	BOOL m_DrawEdges;
	BOOL m_DrawFaces;
	BOOL m_LightingTwoSides;
	virtual ~CNNView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	CPoint MouseMoveStart;


protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNNView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnButtonViewParam();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NNView.cpp
inline CNNDoc* CNNView::GetDocument()
   { return (CNNDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NNVIEW_H__26A63D2E_6CBB_11D4_9E62_EF8715B56461__INCLUDED_)
