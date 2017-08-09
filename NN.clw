; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNNDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "NN.h"
LastPage=0

ClassCount=15
Class1=CNNApp
Class2=CNNDoc
Class3=CNNView
Class4=CMainFrame

ResourceCount=12
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_NNTYPE
Resource4=IDD_DIALOG_VIEW_PARAM
Resource5=IDR_MAINFRAME (English (U.S.))
Resource6=IDD_SELF_ORGANIZING_FEATURE_MAP_PAGE (English (U.S.))
Class5=CChildFrame
Class6=CAboutDlg
Class7=CNetDialog
Class8=CNetTrainDlg
Class9=CViewParamDlg
Resource7=IDD_GROWING_NEURAL_GAS_PAGE (English (U.S.))
Resource8=IDD_GROWING_GRID_PAGE (English (U.S.))
Class10=CNeuralGasPage
Resource9=IDR_NNTYPE (English (U.S.))
Resource10=IDD_ABOUTBOX (English (U.S.))
Class11=CSelfOrganizingFeatureMapPage
Class12=CGrowinNeuralGasPage
Class13=CGrowingGridPage
Class14=CTrainNetDialogPropSheet
Resource11=IDD_NEURAL_GAS_PAGE (English (U.S.))
Class15=CInitNeuralNetworkDialog
Resource12=IDD_DIALOG_INIT_NN

[CLS:CNNApp]
Type=0
HeaderFile=NN.h
ImplementationFile=NN.cpp
Filter=N

[CLS:CNNDoc]
Type=0
HeaderFile=NNDoc.h
ImplementationFile=NNDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=ID_BUTTON_CALC_NORMALS_USING_LSQ

[CLS:CNNView]
Type=0
HeaderFile=NNView.h
ImplementationFile=NNView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CNNView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_BUTTON32777


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=ID_BUTTON_CHANGE_TRAINING_PARAMETERS


[CLS:CAboutDlg]
Type=0
HeaderFile=NN.cpp
ImplementationFile=NN.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
CommandCount=8
Command8=ID_APP_ABOUT

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
CommandCount=8
Command8=ID_APP_ABOUT

[MNU:IDR_NNTYPE]
Type=1
Class=CNNView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
CommandCount=21
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
CommandCount=14
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE


[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_BUTTON_VIEW_PARAM
Command10=ID_BUTTON_TRAIN_NET_ONE_STEP
Command11=ID_BUTTON_TRAIN_NET_CONTINOUS
Command12=ID_BUTTON_PAUSE_NET_TRAINING
Command13=ID_BUTTON_CHANGE_TRAINING_PARAMETERS
Command14=ID_BUTTON_CHANGE_NETWORK_TYPE
Command15=ID_BUTTON_TRIANGULATE_NET
Command16=ID_BUTTON_CLOSE_LOOPS
Command17=ID_BUTTON_DETECT_NON_MANIFOLD_NEURONS
Command18=ID_BUTTON_DELETE_NON_MANIFOLD_NEURONS
Command19=ID_BUTTON_CALC_NORMALS_USING_LSQ
CommandCount=19

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_NNTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CNetDialog]
Type=0
HeaderFile=NetDialog.h
ImplementationFile=NetDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CNetDialog

[CLS:CNetTrainDlg]
Type=0
HeaderFile=NetTrainDlg.h
ImplementationFile=NetTrainDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CNetTrainDlg

[CLS:CViewParamDlg]
Type=0
HeaderFile=ViewParamDlg.h
ImplementationFile=ViewParamDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_DRAW_FACES
VirtualFilter=dWC

[DLG:IDD_DIALOG_VIEW_PARAM]
Type=1
Class=CViewParamDlg
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_POINT_SIZE,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_NEURON_SIZE,edit,1350631552
Control7=IDC_DRAW_AXES,button,1342242819
Control8=IDC_DRAW_NEURONS,button,1342242819
Control9=IDC_DRAW_POINTS,button,1342242819
Control10=IDC_DRAW_EDGES,button,1342242819
Control11=IDC_DRAW_FACES,button,1342242819

[DLG:IDD_NEURAL_GAS_PAGE (English (U.S.))]
Type=1
Class=CNeuralGasPage
ControlCount=18
Control1=IDC_STATIC,static,1342308352
Control2=IDC_NG_NEURON_NUMBER,edit,1484849280
Control3=IDC_STATIC,static,1342308352
Control4=IDC_NG_RUN_LENGTH,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_NG_INIT_EPS,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_NG_CHUNK_SIZE,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_NG_FINAL_EPS,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_NG_INIT_LAMBDA,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_NG_FINAL_LAMBDA,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_NG_EDGE_EXPERATION_AGE_FINAL,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_NG_EDGE_EXPERATION_AGE_INIT,edit,1350631552

[CLS:CNeuralGasPage]
Type=0
HeaderFile=NeuralGasPage.h
ImplementationFile=NeuralGasPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CNeuralGasPage
VirtualFilter=idWC

[DLG:IDD_GROWING_NEURAL_GAS_PAGE (English (U.S.))]
Type=1
Class=CGrowinNeuralGasPage
ControlCount=18
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GNG_NEURON_NUMBER,edit,1350633600
Control3=IDC_STATIC,static,1342308352
Control4=IDC_GNG_RUN_LENGTH,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GNG_EPS_B,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_GNG_CHUNK_SIZE,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_GNG_EPS_N,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_GNG_ALPHA,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_GNG_BETTA,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_GNG_LAMBDA_GROWTH,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_GNG_AGE_MAX,edit,1350631552

[DLG:IDD_GROWING_GRID_PAGE (English (U.S.))]
Type=1
Class=CGrowingGridPage
ControlCount=29
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GG_NEURON_NUMBER_U,edit,1484849280
Control3=IDC_STATIC,static,1342308352
Control4=IDC_GG_RUN_LENGTH,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_GG_INIT_EPS,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_GG_CHUNK_SIZE,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_GG_FINAL_EPS,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_GG_SIGMA,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_GG_LAMBDA_GROWTH,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_GG_RADIUS_FINAL,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_GG_RADIUS_INIT,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_GG_NEURON_NUMBER_V,edit,1484849280
Control21=IDC_GG_GRID_CAD_GROW,button,1342242819
Control22=IDC_GG_CYCLIC_U,button,1342242819
Control23=IDC_GG_CYCLIC_V,button,1342242819
Control24=IDC_STATIC,static,1342308352
Control25=IDC_GG_BOUNDARY_CORRECTION_EVERY,edit,1350631552
Control26=IDC_STATIC,static,1342308352
Control27=IDC_GG_BOUNDARY_CORRECTION_RUNLENGTH,edit,1350631552
Control28=IDC_STATIC,static,1342308352
Control29=IDC_GG_BOUNDARY_CORRECTION_POINT_WEIGHT,edit,1350631552

[DLG:IDD_SELF_ORGANIZING_FEATURE_MAP_PAGE (English (U.S.))]
Type=1
Class=CSelfOrganizingFeatureMapPage
ControlCount=31
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SOFM_RUN_LENGTH,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SOFM_INIT_EPS,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_SOFM_CHUNK_SIZE,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_SOFM_FINAL_EPS,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SOFM_SIGMA_INIT,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_SOFM_SIGMA_FINAL,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_SOFM_RADIUS_FINAL,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_SOFM_RADIUS_INIT,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_SOFM_BOUNDARY_CORRECTION_EVERY,edit,1350631552
Control19=IDC_STATIC,static,1342308352
Control20=IDC_SOFM_BOUNDARY_CORRECTION_RUNLENGTH,edit,1350631552
Control21=IDC_STATIC,static,1342308352
Control22=IDC_SOFM_BOUNDARY_CORRECTION_POINT_WEIGHT,edit,1350631552
Control23=IDC_STATIC,static,1342308352
Control24=IDC_SOFM_LAMBDA_GROWTH,edit,1350631552
Control25=IDC_SOFM_GRID_CAN_GROW,button,1342242819
Control26=IDC_SOFM_CYCLIC_U,button,1342242819
Control27=IDC_SOFM_CYCLIC_V,button,1342242819
Control28=IDC_STATIC,static,1342308352
Control29=IDC_SOFM_NEURON_NUMBER_U,edit,1484849280
Control30=IDC_STATIC,static,1342308352
Control31=IDC_SOFM_NEURON_NUMBER_V,edit,1484849280

[CLS:CGrowinNeuralGasPage]
Type=0
HeaderFile=GrowinNeuralGasPage.h
ImplementationFile=GrowinNeuralGasPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=IDC_GNG_AGE_MAX
VirtualFilter=idWC

[CLS:CSelfOrganizingFeatureMapPage]
Type=0
HeaderFile=SelfOrganizingFeatureMapPage.h
ImplementationFile=SelfOrganizingFeatureMapPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CSelfOrganizingFeatureMapPage
VirtualFilter=idWC

[CLS:CGrowingGridPage]
Type=0
HeaderFile=GrowingGridPage.h
ImplementationFile=GrowingGridPage.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CGrowingGridPage
VirtualFilter=idWC

[CLS:CTrainNetDialogPropSheet]
Type=0
HeaderFile=TrainNetDialogPropSheet.h
ImplementationFile=TrainNetDialogPropSheet.cpp
BaseClass=CPropertySheet
Filter=W
LastObject=CTrainNetDialogPropSheet
VirtualFilter=hWC

[DLG:IDD_DIALOG_INIT_NN]
Type=1
Class=CInitNeuralNetworkDialog
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_NN_INIT_TOPOLOGY,combobox,1344339971
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_NN_INIT_NEURON_NUMBER_U,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_NN_INIT_NEURON_NUMBER_V,edit,1350631552
Control9=IDC_NN_INIT_NET_CAN_GROW_NEURONS,button,1342242819
Control10=IDC_NN_INIT_CYCLIC_U,button,1342242819
Control11=IDC_NN_INIT_CYCLIC_V,button,1342242819
Control12=IDC_NN_INIT_NET_CAN_GROW_EDGES,button,1342242819
Control13=IDC_NN_INIT_FAST_INIT_TYPE,combobox,1344339971
Control14=IDC_STATIC,static,1342308352

[CLS:CInitNeuralNetworkDialog]
Type=0
HeaderFile=InitNeuralNetworkDialog.h
ImplementationFile=InitNeuralNetworkDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NN_INIT_NEURON_NUMBER_U
VirtualFilter=dWC

