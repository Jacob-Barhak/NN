// NNView.cpp : implementation of the CNNView class
//

#include "stdafx.h"
#include "NN.h"

#include "NNDoc.h"
#include "NNView.h"
#include "ViewParamDlg.h"
#include "NeuralNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNNView

IMPLEMENT_DYNCREATE(CNNView, CView)

BEGIN_MESSAGE_MAP(CNNView, CView)
	//{{AFX_MSG_MAP(CNNView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_BUTTON_VIEW_PARAM, OnButtonViewParam)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNNView construction/destruction

CNNView::CNNView()
{
	// TODO: add construction code here
	//JAC insert
	m_MaxX=1.0;
	m_MinX=-1.0;
	m_MaxY=1.0;
	m_MinY=-1.0;
	m_MaxZ=1.0;
	m_MinZ=-1.0;
	//RotX=0.0;
	//RotY=0.0;
	m_RotX=0.0;
	m_RotY=0.0;
	m_MouseRotSensativity=10.0;
	m_MouseTransSensativity=0.01;
	m_MouseZoomSensativity=0.01;
	m_PointSize=0.1;
	m_NeuronSize=0.1;
	m_NormalSize=0.1;
	m_DrawAxes=false;
	m_DrawNeurons=true;
	m_DrawPoints=true;
	m_DrawEdges=true;
	m_DrawFaces=true;
	m_LightingTwoSides=false;

	m_RotAngle=0.0;
	m_TransX=0.0;
	m_TransY=0.0;
	m_Zoom=1.0;
	// init rotation matrix
	for (int i=0;i<4;i++) 
		for (int j=0;j<4;j++) 
			m_ViewMat[i*4+j]= (i==j)? 1.0:0.0;



}

CNNView::~CNNView()
{
	//when trying to close the view make sure that no thread is running
	//mark that training continously is not needed anymore.
	CNNDoc* pDoc = GetDocument();

	pDoc->WaitUntilThreadTerminates ();
}

BOOL CNNView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;


	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNNView drawing

void CNNView::OnDraw(CDC* pDC)
{
	CNNDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
		CPalette* ppalOld = pDC->SelectPalette(&m_Pal, 0);
	pDC->RealizePalette();
	
	// Make the HGLRC current.
	BOOL bResult = wglMakeCurrent (pDC->m_hDC, m_hrc);
#ifdef MY_DEBUG
	if (!bResult)   {
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
	}
#endif
	
	// Draw.
	DrawScene(pDoc) ;

	glFinish(); // wait untill al GL commands have been processed
	// Swap buffers.
	SwapBuffers(pDC->m_hDC) ;
	
	// Select old palette.
    if (ppalOld) pDC->SelectPalette(ppalOld, 0); 
	wglMakeCurrent(NULL, NULL) ;

}

/////////////////////////////////////////////////////////////////////////////
// CNNView printing

BOOL CNNView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNNView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNNView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CNNView diagnostics

#ifdef _DEBUG
void CNNView::AssertValid() const
{
	CView::AssertValid();
}

void CNNView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNNDoc* CNNView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNNDoc)));
	return (CNNDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNNView message handlers

int CNNView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
		CClientDC dc(this) ;
#ifdef MY_DEBUG
	TRACE("window created") ;
#endif
	//
	// Fill in the pixel format descriptor.
	//
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)) ;
#ifdef MY_DEBUG
	TRACE("mem set") ;
#endif

	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion   = 1 ; 
	pfd.dwFlags    = PFD_DOUBLEBUFFER |
					 PFD_SUPPORT_OPENGL |
					 PFD_DRAW_TO_WINDOW ;
	pfd.iPixelType = PFD_TYPE_RGBA ;
	pfd.cColorBits = 24 ;
	pfd.cDepthBits = 32 ;
	pfd.iLayerType = PFD_MAIN_PLANE ;

#ifdef MY_DEBUG
	TRACE("pfd set") ;
#endif

	int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
#ifdef MY_DEBUG
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
#endif
		return -1;
	}
#ifdef MY_DEBUG
	TRACE("Pixel Format %d\r\n",nPixelFormat) ;
#endif
	BOOL bResult = SetPixelFormat (dc.m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
#ifdef MY_DEBUG
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
#endif
		return -1 ;
	}

   //   // Create a rendering context.   //
	m_hrc = wglCreateContext(dc.m_hDC);   
	if (!m_hrc)   {
#ifdef MY_DEBUG
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
#endif
		return -1;
	}

	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CNNView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
    CClientDC dc(this) ;

	BOOL bResult = wglMakeCurrent (dc.m_hDC, m_hrc);
	if (!bResult)
	{
#ifdef MY_DEBUG
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
#endif
		return ;
	}

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	ReshapeFunc(cx, cy);

	wglMakeCurrent(NULL, NULL);

	
}

void CNNView::ReshapeFunc(int Wx, int Wy)
{

	GLdouble gldAspect = (GLdouble) Wx/ (GLdouble) Wy;


	glViewport(0, 0 ,Wx, Wy);

	//double MaxSize = m_MaxX-m_MinX;
	//double MeanX=(m_MaxX+m_MinX)/2;
	//double MeanY=(m_MaxY+m_MinY)/2;
	double MaxSizeX=m_MaxX-m_MinX;
	double MaxSizeY=m_MaxY-m_MinY;
	double MaxSizeZ=m_MaxZ-m_MinZ;

	double MaxSize=max(MaxSizeX,MaxSizeY);
	MaxSize=max(MaxSize,MaxSizeZ);

	double ar;
	if (Wy==0) //it is possible in the initial creation of the window...
		ar=1.0;
	else
		ar=(double)Wx/Wy;

	if (ar>1.0){
		glOrtho(-MaxSize*ar, MaxSize*ar,-MaxSize, MaxSize,-2*MaxSize, 2*MaxSize);
	}
	else{
		glOrtho(-MaxSize, MaxSize,-MaxSize/ar, MaxSize/ar,-2*MaxSize, 2*MaxSize);
	}

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();


}

BOOL CNNView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return true;
	//return CView::OnEraseBkgnd(pDC);
}

void CNNView::DrawScene(CNNDoc* pDoc)
{

	
	float DefaultAmbientColor[4]={0.0f,0.0f,0.0f,1.0f};

	float Light1AmbientColor[4]={0.1f,0.1f,0.1f,1.0f};
	float Light1DiffuseColor[4]={0.5f,0.5f,0.5f,1.0f};
	float Light1SpecularColor[4]={1.0f,1.0f,1.0f,1.0f};
	float Light1Position[4]={1.0f,1.0f,1.0f,0.0f};

	

	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable (GL_BLEND);

	glEnable (GL_DEPTH_TEST);

	glEnable (GL_LIGHTING);

	glEnable (GL_NORMALIZE);

	//glEnable (GL_COLOR_MATERIAL);

	glDisable (GL_LIGHT0);
	glEnable (GL_LIGHT1);

	glShadeModel(GL_SMOOTH);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, m_LightingTwoSides ? 1:0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT ,DefaultAmbientColor);

	glLightfv(GL_LIGHT1,GL_AMBIENT,Light1AmbientColor);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,Light1DiffuseColor);
	glLightfv(GL_LIGHT1,GL_SPECULAR,Light1SpecularColor);
	glLightfv(GL_LIGHT1,GL_POSITION,Light1Position);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	 
//	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	GenRotMatrix();

	glMultMatrixd(m_ViewMat);

	glTranslated(-(m_MaxX+m_MinX)/2,-(m_MaxY+m_MinY)/2,-(m_MaxZ+m_MinZ)/2);

	// axes are red
	glColor3f(1.0,0.0,0.0);

	double ArrowSize;

	glInitNames();
//	glPushName(0);

	glDisable(GL_LIGHTING);
	ArrowSize=m_MaxX-m_MinX;
	ArrowSize=min(ArrowSize,m_MaxY-m_MinY);
	ArrowSize=min(ArrowSize,m_MaxZ-m_MinZ);

	if(m_DrawAxes){
		glBegin(GL_LINES);

			glVertex3d((m_MaxX+m_MinX)/2,(m_MaxY+m_MinY)/2,(m_MaxZ+m_MinZ)/2);
			glVertex3d((m_MaxX+m_MinX)/2+ArrowSize/4,(m_MaxY+m_MinY)/2,(m_MaxZ+m_MinZ)/2);

			glVertex3d((m_MaxX+m_MinX)/2,(m_MaxY+m_MinY)/2,(m_MaxZ+m_MinZ)/2);
			glVertex3d((m_MaxX+m_MinX)/2,(m_MaxY+m_MinY)/2+ArrowSize/4,(m_MaxZ+m_MinZ)/2);

			glVertex3d((m_MaxX+m_MinX)/2,(m_MaxY+m_MinY)/2,(m_MaxZ+m_MinZ)/2);
			glVertex3d((m_MaxX+m_MinX)/2,(m_MaxY+m_MinY)/2,(m_MaxZ+m_MinZ)/2+ArrowSize/4);
		
		glEnd();
	}
	// points 
	if (m_DrawPoints){
		pDoc->m_PtLst.Draw();
	}
	// Neurons 
	if (m_DrawNeurons){
		pDoc->m_NNet.Draw();
	}
	// Edges
	if (m_DrawEdges){
		pDoc->m_NNet.Edges.Draw();
	}

	glEnable(GL_LIGHTING);

	//faces
	if (m_DrawFaces){
		pDoc->m_NNet.Faces.Draw();
	}

	glDisable(GL_LIGHTING);


#ifdef MY_DEBUG
	MyGLErrorMessage("drawing scene");
#endif

	glPopMatrix();

//	glPopName();

	glDisable (GL_BLEND);

	glDisable (GL_LIGHT1);

	glDisable (GL_DEPTH_TEST);

	glDisable (GL_NORMALIZE);

	//glDisable (GL_COLOR_MATERIAL);

#ifdef MY_DEBUG
	TRACE ("view : finished Drawing picture\n");
#endif
}

void CNNView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CNNDoc *pDoc;

	
	pDoc=GetDocument();



	//
	// Build a display lists
	//    


	if (lHint==1){ // this occours when we first set the bounding box of the point list

		RECT MyRect;
		GetClientRect( &MyRect ); //get the area of the current window
		m_MinX=pDoc->m_PtLst.MinVals.Pos[0];
		m_MinY=pDoc->m_PtLst.MinVals.Pos[1];
		m_MinZ=pDoc->m_PtLst.MinVals.Pos[2];
		m_MaxX=pDoc->m_PtLst.MaxVals.Pos[0];
		m_MaxY=pDoc->m_PtLst.MaxVals.Pos[1];
		m_MaxZ=pDoc->m_PtLst.MaxVals.Pos[2];

		double ArrowSize;

		ArrowSize=m_MaxX-m_MinX;
		ArrowSize=max(ArrowSize,m_MaxY-m_MinY);
		ArrowSize=max(ArrowSize,m_MaxZ-m_MinZ);

		ArrowSize /= 4;

		m_PointSize=(double)ArrowSize/100;
		m_NeuronSize=(double)ArrowSize/50;
		m_NormalSize=(double)ArrowSize/5;


		CClientDC dc(this) ;
		BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 
#ifdef MY_DEBUG
		if (!bResult)
		{
			TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		}
#endif

		glMatrixMode(GL_PROJECTION);
		MyGLErrorMessage("reshape problem7");

		glLoadIdentity();
		MyGLErrorMessage("reshape problem6");

		ReshapeFunc(MyRect.right , MyRect.bottom);
		// also, we can now initialize the lists...
	
		// release the dc
		wglMakeCurrent(NULL, NULL) ;

	}


	//
	// Attach the window dc to OpenGL.
	//
	CClientDC dc(this) ;
	BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 
#ifdef MY_DEBUG
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
	}
#endif

	pDoc->m_UpdatingListsNow.Lock(); //*** locking

	if (lHint==1){ // this occours when we first set the bounding box of the point list

		pDoc->m_PtLst.InitDispList();
		pDoc->m_NNet.InitDispList();	
		pDoc->m_NNet.Edges.InitDispList();
		pDoc->m_NNet.Faces.InitDispList();
		
	}

#ifdef MY_DEBUG
	TRACE("view - locking in update view\n");
#endif
	
	if(pDoc->m_PtLst.RecalcForDisplay)
		pDoc->m_PtLst.DrawList(m_PointSize,pDoc->m_NGChunkSize,pDoc->m_CurrentIteration);
	pDoc->m_NNet.DrawList(m_NeuronSize,m_NormalSize);
	pDoc->m_NNet.Edges.DrawList(0);
	pDoc->m_NNet.Faces.DrawList();
	
	glFinish(); //wait untill all gl commands have been processed.
	
	pDoc->m_UpdatingListsNow.Unlock(); // *** unlocking list
#ifdef MY_DEBUG
	TRACE("view - unlocking in update view\n");
#endif

	// release the dc
	wglMakeCurrent(NULL, NULL) ;

	Invalidate(FALSE) ; 

#ifdef MY_DEBUG
	TRACE ("View - finshed updating the window\n");
#endif
	UpdateWindow() ;
}

void CNNView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	m_MouseMoveStart=point;
	CView::OnLButtonDown(nFlags, point);
}

void CNNView::OnLButtonUp(UINT nFlags, CPoint point) 
{

	if (nFlags && MK_SHIFT) { //picking


#define BUFSIZE 512
 
		CClientDC dc(this) ;
		BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 
#ifdef MY_DEBUG
		if (!bResult)
		{
			TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		}
#endif

		GLuint SelectBuf[BUFSIZE];

		GLint Hits;

		glSelectBuffer (BUFSIZE, SelectBuf);
		glRenderMode (GL_SELECT);

		GLint ViewPort[4];

		glMatrixMode(GL_PROJECTION);  
		glPushMatrix();
		glLoadIdentity();  
		glGetIntegerv (GL_VIEWPORT, ViewPort);
		
		gluPickMatrix(point.x, ViewPort[3] - point.y , 1.0 , 1.0 , ViewPort);  

		ReshapeFunc(ViewPort[2],ViewPort[3]);

		CNNDoc* pDoc = GetDocument();

		DrawScene(pDoc);

		Hits = glRenderMode (GL_RENDER);

		// release the dc

		glMatrixMode(GL_PROJECTION);  
		glPopMatrix();

		
		wglMakeCurrent(NULL, NULL) ;


		GLint i;
		GLuint names, ptr, j=0;
#ifdef MY_DEBUG
		TRACE ("hits = %d\n", Hits);
#endif
		ptr = 0;
		for ( i = 0; i < Hits; i++) { /* for each hit */
			names = SelectBuf[ptr];
			TRACE (" number of names for hit %d = %d\n",i+1,names);
			ptr+=3;
			TRACE (" names are: ");
			for (j = 0; j < names; j++){ /* for each name */
				TRACE ("%X ", SelectBuf[ptr]);
				ptr++;
			}
			TRACE ("\n");
		}

	}
	else{
		m_RotX = point.x-m_MouseMoveStart.x;
		m_RotY = point.y-m_MouseMoveStart.y;
		m_RotAngle = sqrt(m_RotX*m_RotX+m_RotY*m_RotY) / m_MouseRotSensativity;

		Invalidate(FALSE) ; // maybe true???
		UpdateWindow() ;
	}

	//GetDocument()->UpdateAllViews(NULL);
	CView::OnLButtonUp(nFlags, point);
}

void CNNView::OnButtonViewParam() 
{
	// TODO: Add your command handler code here

	CViewParamDlg ViewDlg;

	ViewDlg.m_NeuronSize=m_NeuronSize;
	ViewDlg.m_NormalSize=m_NormalSize;
	ViewDlg.m_PointSize=m_PointSize;
	ViewDlg.m_DrawAxes=m_DrawAxes;
	ViewDlg.m_DrawPoints=m_DrawPoints;
	ViewDlg.m_DrawNeurons=m_DrawNeurons;
	ViewDlg.m_DrawEdges=m_DrawEdges;
	ViewDlg.m_DrawFaces=m_DrawFaces;
	ViewDlg.m_LightingTwoSides=m_LightingTwoSides;


	if (ViewDlg.DoModal()==IDOK){
		m_NeuronSize=ViewDlg.m_NeuronSize;
		m_NormalSize=ViewDlg.m_NormalSize;
		if (m_PointSize!=ViewDlg.m_PointSize){
			// update point list parameters and force update of display 
			m_PointSize=ViewDlg.m_PointSize;
			CNNDoc *pDoc=GetDocument();
			pDoc->m_PtLst.RecalcForDisplay=true;	
		}
		m_DrawAxes=ViewDlg.m_DrawAxes;
		m_DrawPoints=ViewDlg.m_DrawPoints;
		m_DrawNeurons=ViewDlg.m_DrawNeurons;
		m_DrawEdges=ViewDlg.m_DrawEdges;
		m_DrawFaces=ViewDlg.m_DrawFaces;
		m_LightingTwoSides=ViewDlg.m_LightingTwoSides;
	}

	GetDocument()->UpdateAllViews(NULL);

}



void CNNView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_MouseMoveStart=point;
	CView::OnRButtonDown(nFlags, point);
}

void CNNView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (nFlags && MK_SHIFT) {
		double Scx=point.x-m_MouseMoveStart.x;
		double Scy=point.y-m_MouseMoveStart.y;

		if (Scy < 0.0)  
			m_Zoom = 1.0 / (1.0 + sqrt(Scx*Scx+Scy*Scy) * (m_MouseZoomSensativity));
		else
			m_Zoom = 1.0 + sqrt(Scx*Scx+Scy*Scy) * (m_MouseZoomSensativity);

		// this happens when mouse has not been moved
		//if (m_Zoom==0.0) m_Zoom=1.0;
	}
	else{
		m_TransX = (point.x-m_MouseMoveStart.x) * m_MouseTransSensativity *(m_MaxX-m_MinX);
		m_TransY = (point.y-m_MouseMoveStart.y) * m_MouseTransSensativity *(m_MaxY-m_MinY);
	}
	Invalidate(FALSE) ; // maybe true???
	UpdateWindow() ;

	//GetDocument()->UpdateAllViews(NULL);
	CView::OnRButtonUp(nFlags, point);
}

GLdouble *CNNView::GenRotMatrix()
{

	// JAC insert - use opengl's commands to create a new modeling matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(m_TransX,-m_TransY,0.0);
	if (m_RotAngle!=0.0) glRotated(m_RotAngle,m_RotY,m_RotX,0.0);
	glScaled(m_Zoom,m_Zoom,m_Zoom);
	glMultMatrixd(m_ViewMat);
	glGetDoublev(GL_MODELVIEW_MATRIX,m_ViewMat);
	glPopMatrix();

	m_RotAngle=0.0;
	m_Zoom=1.0;
	m_TransX=0.0;
	m_TransY=0.0;


	MyGLErrorMessage("roatation");

	return m_ViewMat;
}

void CNNView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// retinitialize
	m_RotAngle=0.0;
	m_TransX=0.0;
	m_TransY=0.0;
	m_Zoom=1.0;
	// init rotation matrix
	for (int i=0;i<4;i++) 
		for (int j=0;j<4;j++) 
			m_ViewMat[i*4+j]= (i==j)? 1.0:0.0;
	
	Invalidate(FALSE) ; // maybe true???
	UpdateWindow() ;

	CView::OnRButtonDblClk(nFlags, point);
}
