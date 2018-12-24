; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFavOrgView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "favorg.h"
LastPage=0

ClassCount=13
Class1=CByStateDlg
Class2=CEditUrlDlg
Class3=CFavOrgApp
Class4=CAboutDlg
Class5=CFavOrgDoc
Class6=CFavOrgTreeCtl
Class7=CFavOrgView
Class8=CLegendDlg
Class9=CMainFrame
Class10=CMultiselTreeCtrl
Class11=COptionsDlg
Class12=CTreeCtrlEdit

ResourceCount=7
Resource1=IDD_ABOUTBOX (English (U.S.))
Resource2=IDD_OPTIONS
Resource3=IDD_EDITURL
Resource4=IDD_LEGEND (English (U.S.))
Resource5=IDD_BYSTATE (English (U.S.))
Resource6=IDD_FAVORG_FORM (English (U.S.))
Class13=CProxyAuthDlg
Resource7=IDR_MAINFRAME (English (U.S.))

[CLS:CByStateDlg]
Type=0
BaseClass=CDialog
HeaderFile=bystatedlg.h
ImplementationFile=bystatedlg.cpp
LastObject=CByStateDlg
Filter=D
VirtualFilter=dWC

[CLS:CEditUrlDlg]
Type=0
BaseClass=CDialog
HeaderFile=EditUrlDlg.h
ImplementationFile=EditUrlDlg.cpp
LastObject=CEditUrlDlg

[CLS:CFavOrgApp]
Type=0
BaseClass=CWinApp
HeaderFile=FavOrg.h
ImplementationFile=FavOrg.cpp
Filter=N
VirtualFilter=AC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=FavOrg.cpp
ImplementationFile=FavOrg.cpp
LastObject=CAboutDlg

[CLS:CFavOrgDoc]
Type=0
BaseClass=CDocument
HeaderFile=FavOrgDoc.h
ImplementationFile=FavOrgDoc.cpp

[CLS:CFavOrgTreeCtl]
Type=0
BaseClass=CMultiselTreeCtrl
HeaderFile=FavOrgTreeCtl.h
ImplementationFile=FavOrgTreeCtl.cpp
Filter=W
VirtualFilter=GWC
LastObject=CFavOrgTreeCtl

[CLS:CFavOrgView]
Type=0
BaseClass=CFormView
HeaderFile=FavOrgView.h
ImplementationFile=FavOrgView.cpp
Filter=D
VirtualFilter=VWC
LastObject=IDC_NOOVERWRITE

[CLS:CLegendDlg]
Type=0
BaseClass=CDialog
HeaderFile=LegendDlg.h
ImplementationFile=LegendDlg.cpp
LastObject=CLegendDlg

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CMultiselTreeCtrl]
Type=0
BaseClass=CTreeCtrl
HeaderFile=MultiselTreeCtrl.h
ImplementationFile=MultiselTreeCtrl.cpp

[CLS:COptionsDlg]
Type=0
BaseClass=CDialog
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp
LastObject=IDHELP
Filter=D
VirtualFilter=dWC

[CLS:CTreeCtrlEdit]
Type=0
BaseClass=CEdit
HeaderFile=TreeCtrlEdit.h
ImplementationFile=TreeCtrlEdit.cpp

[DLG:IDD_BYSTATE]
Type=1
Class=CByStateDlg

[DLG:IDD_EDITURL]
Type=1
Class=CEditUrlDlg
ControlCount=4
Control1=IDC_REFNAME,static,1342308352
Control2=IDC_URLED,edit,1350631552
Control3=IDOK,button,1342373889
Control4=IDCANCEL,button,1342242816

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_FAVORG_FORM]
Type=1
Class=CFavOrgView

[DLG:IDD_LEGEND]
Type=1
Class=CLegendDlg

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=8
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342177280
Control3=IDC_EDTIMEOUT,edit,1350631552
Control4=IDC_NOCONNECTIONTEST,button,1342242819
Control5=IDOK,button,1342373889
Control6=IDCANCEL,button,1342242816
Control7=IDHELP,button,1342242816
Control8=IDC_SELFOLDER,button,1342373888

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=IDC_GENERATELOG
Command2=IDC_VIEWREPORT
Command3=ID_APP_EXIT
Command4=IDC_PICKICON
Command5=IDC_ASSIGNTHIS
Command6=IDC_ASSIGNCUST
Command7=IDC_EDITURL
Command8=IDC_MNU_UNCHECKED
Command9=IDC_MNU_CHECKED
Command10=IDC_MNU_MOVED
Command11=IDC_MNU_MOVED_UPDATED
Command12=IDC_MNU_PAGENONE
Command13=IDC_MNU_SITENONE
Command14=IDC_MNU_TIMEOUT
Command15=IDC_MNU_ACCESSDENIED
Command16=IDC_MNU_QUESTION
Command17=IDC_MNU_RESET
Command18=IDC_DISPINFO
Command19=IDC_REMOVE
Command20=IDC_SHOWLEGEND
Command21=IDC_DELETEBYSTATE
Command22=IDC_OPTIONS
Command23=IDC_EXPANDALL
Command24=IDC_COLLAPSEALL
Command25=IDC_EXPANDCURRENT
Command26=IDC_COLLAPSECURRENT
Command27=IDC_SELECTNONE
Command28=IDC_FAVORGHELP
Command29=ID_APP_ABOUT
CommandCount=29

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=3
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889

[DLG:IDD_FAVORG_FORM (English (U.S.))]
Type=1
Class=CFavOrgView
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_FAVNUMBER,static,1342177280
Control3=IDC_FAVTREE,SysTreeView32,1350764571
Control4=IDC_PROGRESS,msctls_progress32,1350565889
Control5=IDC_REFRESH,button,1342242816
Control6=IDC_RESETTOROOT,button,1342242816
Control7=IDC_GO,button,1342373888
Control8=IDC_CANCELACTION,button,1342242816
Control9=IDC_OPTIONGROUP,button,1342308359
Control10=IDC_FINDFAVICONS,button,1342242819
Control11=IDC_ALLOWUPDATE,button,1342242819
Control12=IDC_NOOVERWRITE,button,1342242819

[DLG:IDD_LEGEND (English (U.S.))]
Type=1
Class=CLegendDlg
ControlCount=23
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342177280
Control3=IDC_STATIC,static,1342177283
Control4=IDC_STATIC,static,1342177280
Control5=IDC_STATIC,static,1342177283
Control6=IDC_STATIC,static,1342177280
Control7=IDC_STATIC,static,1342177283
Control8=IDC_STATIC,static,1342177280
Control9=IDC_STATIC,static,1342177283
Control10=IDC_STATIC,static,1342177280
Control11=IDC_STATIC,static,1342177283
Control12=IDC_STATIC,static,1342177280
Control13=IDC_STATIC,static,1342177283
Control14=IDC_STATIC,static,1342177280
Control15=IDC_STATIC,static,1342177283
Control16=IDC_STATIC,static,1342177280
Control17=IDC_STATIC,static,1342177283
Control18=IDC_STATIC,static,1342177280
Control19=IDC_STATIC,static,1342177283
Control20=IDC_STATIC,static,1342177280
Control21=IDC_STATIC,button,1342308359
Control22=IDC_STATIC,static,1342177283
Control23=IDC_STATIC,static,1342177280

[DLG:IDD_BYSTATE (English (U.S.))]
Type=1
Class=CByStateDlg
ControlCount=18
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342177283
Control3=IDC_CHECK_SITENONE,button,1342242819
Control4=IDC_SITENOTFOUND_COUNT,static,1342177280
Control5=IDC_STATIC,static,1342177283
Control6=IDC_CHECK_PAGENONE,button,1342242819
Control7=IDC_PAGENOTFOUND_COUNT,static,1342177280
Control8=IDC_STATIC,static,1342177283
Control9=IDC_CHECK_TIMEOUT,button,1342242819
Control10=IDC_TIMEOUT_COUNT,static,1342177280
Control11=IDC_STATIC,static,1342177283
Control12=IDC_CHECK_ACCESSDENIED,button,1342242819
Control13=IDC_ACCESSDENIED_COUNT,static,1342177280
Control14=IDOK,button,1342373889
Control15=IDCANCEL,button,1342242816
Control16=IDHELP,button,1342242816
Control17=IDC_STATIC,static,1342308352
Control18=IDC_COUNT,static,1342308352

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_HELP
CommandCount=1

[CLS:CProxyAuthDlg]
Type=0
HeaderFile=ProxyAuthDlg.h
ImplementationFile=ProxyAuthDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CProxyAuthDlg

