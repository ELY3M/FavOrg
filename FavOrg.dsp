# Microsoft Developer Studio Project File - Name="FavOrg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FavOrg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FavOrg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FavOrg.mak" CFG="FavOrg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FavOrg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FavOrg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FavOrg - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089

!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "FavOrg - Win32 Release"
# Name "FavOrg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ByStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditUrlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FavOrg.cpp
# End Source File
# Begin Source File

SOURCE=.\FavOrg.rc
# End Source File
# Begin Source File

SOURCE=.\FavOrgDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FavOrgTreeCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\FavOrgView.cpp
# End Source File
# Begin Source File

SOURCE=.\LegendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiselTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelIconDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StringExt.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEdit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ByStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditUrlDlg.h
# End Source File
# Begin Source File

SOURCE=.\FavOrg.h
# End Source File
# Begin Source File

SOURCE=.\FavOrgDoc.h
# End Source File
# Begin Source File

SOURCE=.\FavorgHlp.h
# End Source File
# Begin Source File

SOURCE=.\FavOrgTreeCtl.h
# End Source File
# Begin Source File

SOURCE=.\FavOrgView.h
# End Source File
# Begin Source File

SOURCE=.\LegendDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MultiselTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelIconDlg.h
# End Source File
# Begin Source File

SOURCE=.\shelldef.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StringExt.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEdit.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Checkmrkb.ico
# End Source File
# Begin Source File

SOURCE=.\res\Checkmrkg.ico
# End Source File
# Begin Source File

SOURCE=.\res\Checkmrkr.ico
# End Source File
# Begin Source File

SOURCE=.\res\FavOrg.ico
# End Source File
# Begin Source File

SOURCE=.\res\FavOrg.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FavOrgDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Folder.ico
# End Source File
# Begin Source File

SOURCE=.\res\FolderSel.ico
# End Source File
# Begin Source File

SOURCE=.\res\hourglass.ico
# End Source File
# Begin Source File

SOURCE=.\res\inproc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Locked.ico
# End Source File
# Begin Source File

SOURCE=.\res\NoPage.ico
# End Source File
# Begin Source File

SOURCE=.\res\NoSite.ico
# End Source File
# Begin Source File

SOURCE=.\res\question.ico
# End Source File
# Begin Source File

SOURCE=.\res\reset.ico
# End Source File
# Begin Source File

SOURCE=.\res\Timer.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unchecked.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
