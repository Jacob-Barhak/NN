// InitNeuralNetworkDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NN.h"
#include "NeuralNet.h"
#include "InitNeuralNetworkDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInitNeuralNetworkDialog dialog


CInitNeuralNetworkDialog::CInitNeuralNetworkDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInitNeuralNetworkDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInitNeuralNetworkDialog)
	//}}AFX_DATA_INIT

	m_NNInitNetworkType = NN_NOT_INITIALIZED;
	m_NNInitNeuronNumberU=0;
	m_NNInitNeuronNumberV=0;
	m_AllowNetworkChange=true;
}


void CInitNeuralNetworkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitNeuralNetworkDialog)
	DDX_Control(pDX, IDC_NN_INIT_NEURON_NUMBER_V, m_NNInitCtlNeuronNumberV);
	DDX_Control(pDX, IDC_NN_INIT_NEURON_NUMBER_U, m_NNInitCtlNeuronNumberU);
	DDX_Control(pDX, IDC_NN_INIT_NET_CAN_GROW_NEURONS, m_NNInitCtlNetCanGrowNeurons);
	DDX_Control(pDX, IDC_NN_INIT_NET_CAN_GROW_EDGES, m_NNInitCtlNetCanGrowEdges);
	DDX_Control(pDX, IDC_NN_INIT_CYCLIC_V, m_NNInitCtlCyclicV);
	DDX_Control(pDX, IDC_NN_INIT_CYCLIC_U, m_NNInitCtlCyclicU);
	DDX_Control(pDX, IDC_NN_INIT_TOPOLOGY, m_NNInitCtlTopologyCombo);
	DDX_Control(pDX, IDC_NN_INIT_FAST_INIT_TYPE, m_NNInitCtlFastInitTypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitNeuralNetworkDialog, CDialog)
	//{{AFX_MSG_MAP(CInitNeuralNetworkDialog)
	ON_CBN_SELCHANGE(IDC_NN_INIT_TOPOLOGY, OnSelchangeNnInitTopology)
	ON_CBN_SELCHANGE(IDC_NN_INIT_FAST_INIT_TYPE, OnSelchangeNnInitFastInitType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitNeuralNetworkDialog message handlers

void CInitNeuralNetworkDialog::OnSelchangeNnInitTopology() 
{
	// TODO: Add your control notification handler code here
	
		
	int SelectionNumber=m_NNInitCtlTopologyCombo.GetCurSel();	
	switch (SelectionNumber){
	case INIT_NN_DLG_TOPOLOGY_RECTANGULAR:

		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.EnableWindow(TRUE);
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.EnableWindow(TRUE);		
		m_NNInitCtlNetCanGrowNeurons.EnableWindow(TRUE);
		m_NNInitCtlNetCanGrowEdges.EnableWindow(TRUE);
		m_NNInitCtlCyclicU.EnableWindow(TRUE) ;
		m_NNInitCtlCyclicV.EnableWindow(TRUE) ;

		break;
	case INIT_NN_DLG_TOPOLOGY_NONE:

		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.EnableWindow(TRUE);
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.EnableWindow(FALSE);		
		m_NNInitCtlNetCanGrowNeurons.EnableWindow(FALSE);
		m_NNInitCtlNetCanGrowEdges.EnableWindow(TRUE);
		m_NNInitCtlCyclicU.EnableWindow(FALSE) ;
		m_NNInitCtlCyclicV.EnableWindow(FALSE) ;

		//if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.SetWindowText("100");
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.SetWindowText("0");
		if (m_AllowNetworkChange) m_NNInitCtlNetCanGrowNeurons.SetCheck(FALSE);
		//m_NNInitCtlNetCanGrowEdges.SetCheck(TRUE);
		if (m_AllowNetworkChange) m_NNInitCtlCyclicU.SetCheck(FALSE) ;
		if (m_AllowNetworkChange) m_NNInitCtlCyclicV.SetCheck(FALSE) ;
		

		break;
	case INIT_NN_DLG_TOPOLOGY_OTHER:
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.EnableWindow(TRUE);
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.EnableWindow(FALSE);		
		m_NNInitCtlNetCanGrowNeurons.EnableWindow(TRUE);
		m_NNInitCtlNetCanGrowEdges.EnableWindow(TRUE);
		m_NNInitCtlCyclicU.EnableWindow(FALSE) ;
		m_NNInitCtlCyclicV.EnableWindow(FALSE) ;

		//if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.SetWindowText("3");
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.SetWindowText("0");
		//if (m_AllowNetworkChange) m_NNInitCtlNetCanGrowNeurons.SetCheck(TRUE);
		//if (m_AllowNetworkChange) m_NNInitCtlNetCanGrowEdges.SetCheck(TRUE);
		if (m_AllowNetworkChange) m_NNInitCtlCyclicU.SetCheck(FALSE) ;
		if (m_AllowNetworkChange) m_NNInitCtlCyclicV.SetCheck(FALSE) ;
		break;
	default:
		MessageBox("CInitNeuralNetworkDialog::OnSelchangeNnInitTopology() - invalid selection");
	}
		

}

BOOL CInitNeuralNetworkDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_NNInitCtlTopologyCombo.InsertString(INIT_NN_DLG_TOPOLOGY_RECTANGULAR,"Rectangular Topology");
	m_NNInitCtlTopologyCombo.InsertString(INIT_NN_DLG_TOPOLOGY_NONE,"No Topology");
	m_NNInitCtlTopologyCombo.InsertString(INIT_NN_DLG_TOPOLOGY_OTHER,"Other Topology");

	m_NNInitCtlFastInitTypeCombo.InsertString(INIT_NN_DLG_FAST_INIT_SELF_ORGANIZING_FEATURE_MAP,"Self Organizing Map (rectangular topology)");
	m_NNInitCtlFastInitTypeCombo.InsertString(INIT_NN_DLG_FAST_INIT_NEURAL_GAS,"Neural Gas");
	m_NNInitCtlFastInitTypeCombo.InsertString(INIT_NN_DLG_FAST_INIT_GROWING_NEURAL_GAS,"Growing Neural Gas");


	if (m_NNInitNetworkType & NN_NOT_INITIALIZED){
		m_NNInitCtlFastInitTypeCombo.SetCurSel(INIT_NN_DLG_FAST_INIT_SELF_ORGANIZING_FEATURE_MAP); // init as self organizing feature map
	}
	else{ // in case the network was initialized se the values to the network type
	m_NNInitCtlFastInitTypeCombo.InsertString(INIT_NN_DLG_FAST_INIT_OTHER,"Other");
		m_NNInitCtlFastInitTypeCombo.SetCurSel(INIT_NN_DLG_FAST_INIT_OTHER); // init as other
	}

	OnSelchangeNnInitFastInitType();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInitNeuralNetworkDialog::OnSelchangeNnInitFastInitType() 
{
	// TODO: Add your control notification handler code here
	
	int SelectionNumber=m_NNInitCtlFastInitTypeCombo.GetCurSel();

	switch (SelectionNumber){

	case INIT_NN_DLG_FAST_INIT_SELF_ORGANIZING_FEATURE_MAP:
		m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_RECTANGULAR);
		OnSelchangeNnInitTopology();

		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.SetWindowText("3");
		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberV.SetWindowText("3");
		m_NNInitCtlNetCanGrowNeurons.SetCheck(TRUE);
		m_NNInitCtlNetCanGrowEdges.SetCheck(TRUE);
		m_NNInitCtlCyclicU.SetCheck(FALSE) ;
		m_NNInitCtlCyclicV.SetCheck(FALSE) ;

		break;
	case INIT_NN_DLG_FAST_INIT_NEURAL_GAS:

		m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_NONE);
		OnSelchangeNnInitTopology();

		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.SetWindowText("100");
		m_NNInitCtlNetCanGrowNeurons.SetCheck(FALSE);
		m_NNInitCtlNetCanGrowEdges.SetCheck(TRUE);
				
		break;
	case INIT_NN_DLG_FAST_INIT_GROWING_NEURAL_GAS:
		m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_OTHER);
		OnSelchangeNnInitTopology();

		if (m_AllowNetworkChange) m_NNInitCtlNeuronNumberU.SetWindowText("3");
		m_NNInitCtlNetCanGrowNeurons.SetCheck(TRUE);
		m_NNInitCtlNetCanGrowEdges.SetCheck(TRUE);
		           
		break;

	case INIT_NN_DLG_FAST_INIT_OTHER: // this happens when user gives some input
		m_AllowNetworkChange=false;

		if (m_NNInitNetworkType & NN_HAS_TOPOLOGY){
			m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_OTHER);
		}
		else if (m_NNInitNetworkType & NN_NO_TOPOLOGY){
			m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_NONE);
		}
		else if (m_NNInitNetworkType & NN_RECTANGULAR_TOPOLOGY){
			m_NNInitCtlTopologyCombo.SetCurSel(INIT_NN_DLG_TOPOLOGY_RECTANGULAR);
		}
		else MyFatalError ("CInitNeuralNetworkDialog::OnSelchangeNnInitFastInitType - improper initialization detected - network not initialized");
		
		OnSelchangeNnInitTopology();

		m_NNInitCtlCyclicU.SetCheck (m_NNInitNetworkType & NN_CYCLIC_U_DIR) ;
		m_NNInitCtlCyclicV.SetCheck (m_NNInitNetworkType & NN_CYCLIC_V_DIR) ;
		
		m_NNInitCtlNetCanGrowEdges.SetCheck(m_NNInitNetworkType & NN_CAN_GROW_EDGES);
		m_NNInitCtlNetCanGrowNeurons.SetCheck(m_NNInitNetworkType & NN_CAN_GROW_NEURONS);

		// init network size - it will not be touched afterwards
		char Str[255];
		sprintf(Str,"%d",m_NNInitNeuronNumberU);
		m_NNInitCtlNeuronNumberU.SetWindowText(Str);
		sprintf(Str,"%d",m_NNInitNeuronNumberV);
		m_NNInitCtlNeuronNumberV.SetWindowText(Str);
		m_NNInitCtlNeuronNumberU.EnableWindow(FALSE);
		m_NNInitCtlNeuronNumberV.EnableWindow(FALSE);		

		break;

	default:
		MessageBox("CInitNeuralNetworkDialog::OnSelchangeNnInitFastInitType() - invalid selection");

	}
	
}


void CInitNeuralNetworkDialog::OnOK() 
{
	// TODO: Add extra validation here
	int NumU, NumV, RetVal;
	char WinText[MAX_CEDIT_TEXT_LENGTH];

	m_NNInitCtlNeuronNumberU.GetWindowText (WinText,MAX_CEDIT_TEXT_LENGTH);
	RetVal=sscanf(WinText,"%d",&NumU);
	if (RetVal !=1 || NumU<0 || NumU>100000){
		MessageBox("Invalid Value Entered in Number of Neurons in U direction - th input must be a number between 0 and 1000");
		m_NNInitCtlNeuronNumberU.SetFocus();
		return;
	}

	m_NNInitCtlNeuronNumberV.GetWindowText (WinText,MAX_CEDIT_TEXT_LENGTH);
	RetVal=sscanf(WinText,"%d",&NumV);
	if (RetVal !=1 || NumV<0 || NumV>100000){
		MessageBox("Invalid Value Entered in Number of Neurons in U direction - th input must be a number between 0 and 1000");
		// must be valid if wrog input in enetered - since we initialize it correctly
		m_NNInitCtlNeuronNumberV.SetFocus();
		return;
	}

	if (NumU==0 && NumV==0){
		MessageBox("Network Size canot be zero");
		m_NNInitCtlNeuronNumberU.SetFocus();
		return;
	}

	// determine network Type and sizes in these variables for simple exit values
	m_NNInitNeuronNumberU=NumU;
	m_NNInitNeuronNumberV=NumV;
	

	int SelectionNumber=m_NNInitCtlTopologyCombo.GetCurSel();	

	switch (SelectionNumber){
		
	case INIT_NN_DLG_TOPOLOGY_RECTANGULAR:
		m_NNInitNetworkType = NN_RECTANGULAR_TOPOLOGY;
		break;
	case INIT_NN_DLG_TOPOLOGY_NONE:
		m_NNInitNetworkType	= NN_NO_TOPOLOGY;
		break;
	case INIT_NN_DLG_TOPOLOGY_OTHER:
		m_NNInitNetworkType = NN_HAS_TOPOLOGY;
		break;
	default:
		TRACE("CInitNeuralNetworkDialog::OnOk - invalid selection in topology combo box");
	
	}

	if (m_NNInitCtlCyclicU.GetCheck()) m_NNInitNetworkType |= NN_CYCLIC_U_DIR;
	if (m_NNInitCtlCyclicV.GetCheck()) m_NNInitNetworkType |= NN_CYCLIC_V_DIR;
	
	if (m_NNInitCtlNetCanGrowEdges.GetCheck()) m_NNInitNetworkType |= NN_CAN_GROW_EDGES;
	if (m_NNInitCtlNetCanGrowNeurons.GetCheck()) m_NNInitNetworkType |= NN_CAN_GROW_NEURONS;
	
	CDialog::OnOK();
}
