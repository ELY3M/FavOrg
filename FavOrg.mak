# Microsoft Developer Studio Generated NMAKE File, Based on FavOrg.dsp
!IF "$(CFG)" == ""
CFG=FavOrg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FavOrg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FavOrg - Win32 Release" && "$(CFG)" != "FavOrg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FavOrg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\FavOrg.exe"


CLEAN :
	-@erase "$(INTDIR)\ByStateDlg.obj"
	-@erase "$(INTDIR)\EditUrlDlg.obj"
	-@erase "$(INTDIR)\FavOrg.obj"
	-@erase "$(INTDIR)\FavOrg.pch"
	-@erase "$(INTDIR)\FavOrg.res"
	-@erase "$(INTDIR)\FavOrgDoc.obj"
	-@erase "$(INTDIR)\FavOrgTreeCtl.obj"
	-@erase "$(INTDIR)\FavOrgView.obj"
	-@erase "$(INTDIR)\LegendDlg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MultiselTreeCtrl.obj"
	-@erase "$(INTDIR)\OptionsDlg.obj"
	-@erase "$(INTDIR)\SelIconDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StringExt.obj"
	-@erase "$(INTDIR)\TreeCtrlEdit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\FavOrg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\FavOrg.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\FavOrg.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FavOrg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\FavOrg.pdb" /machine:I386 /out:"$(OUTDIR)\FavOrg.exe" /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 /IGNORE:4089 
LINK32_OBJS= \
	"$(INTDIR)\ByStateDlg.obj" \
	"$(INTDIR)\EditUrlDlg.obj" \
	"$(INTDIR)\FavOrg.obj" \
	"$(INTDIR)\FavOrgDoc.obj" \
	"$(INTDIR)\FavOrgTreeCtl.obj" \
	"$(INTDIR)\FavOrgView.obj" \
	"$(INTDIR)\LegendDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MultiselTreeCtrl.obj" \
	"$(INTDIR)\OptionsDlg.obj" \
	"$(INTDIR)\SelIconDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StringExt.obj" \
	"$(INTDIR)\TreeCtrlEdit.obj" \
	"$(INTDIR)\FavOrg.res"

"$(OUTDIR)\FavOrg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\FavOrg.exe" "$(OUTDIR)\FavOrg.bsc"


CLEAN :
	-@erase "$(INTDIR)\ByStateDlg.obj"
	-@erase "$(INTDIR)\ByStateDlg.sbr"
	-@erase "$(INTDIR)\EditUrlDlg.obj"
	-@erase "$(INTDIR)\EditUrlDlg.sbr"
	-@erase "$(INTDIR)\FavOrg.obj"
	-@erase "$(INTDIR)\FavOrg.pch"
	-@erase "$(INTDIR)\FavOrg.res"
	-@erase "$(INTDIR)\FavOrg.sbr"
	-@erase "$(INTDIR)\FavOrgDoc.obj"
	-@erase "$(INTDIR)\FavOrgDoc.sbr"
	-@erase "$(INTDIR)\FavOrgTreeCtl.obj"
	-@erase "$(INTDIR)\FavOrgTreeCtl.sbr"
	-@erase "$(INTDIR)\FavOrgView.obj"
	-@erase "$(INTDIR)\FavOrgView.sbr"
	-@erase "$(INTDIR)\LegendDlg.obj"
	-@erase "$(INTDIR)\LegendDlg.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MultiselTreeCtrl.obj"
	-@erase "$(INTDIR)\MultiselTreeCtrl.sbr"
	-@erase "$(INTDIR)\OptionsDlg.obj"
	-@erase "$(INTDIR)\OptionsDlg.sbr"
	-@erase "$(INTDIR)\SelIconDlg.obj"
	-@erase "$(INTDIR)\SelIconDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\StringExt.obj"
	-@erase "$(INTDIR)\StringExt.sbr"
	-@erase "$(INTDIR)\TreeCtrlEdit.obj"
	-@erase "$(INTDIR)\TreeCtrlEdit.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FavOrg.bsc"
	-@erase "$(OUTDIR)\FavOrg.exe"
	-@erase "$(OUTDIR)\FavOrg.ilk"
	-@erase "$(OUTDIR)\FavOrg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\FavOrg.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\FavOrg.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FavOrg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ByStateDlg.sbr" \
	"$(INTDIR)\EditUrlDlg.sbr" \
	"$(INTDIR)\FavOrg.sbr" \
	"$(INTDIR)\FavOrgDoc.sbr" \
	"$(INTDIR)\FavOrgTreeCtl.sbr" \
	"$(INTDIR)\FavOrgView.sbr" \
	"$(INTDIR)\LegendDlg.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\MultiselTreeCtrl.sbr" \
	"$(INTDIR)\OptionsDlg.sbr" \
	"$(INTDIR)\SelIconDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\StringExt.sbr" \
	"$(INTDIR)\TreeCtrlEdit.sbr"

"$(OUTDIR)\FavOrg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\FavOrg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\FavOrg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ByStateDlg.obj" \
	"$(INTDIR)\EditUrlDlg.obj" \
	"$(INTDIR)\FavOrg.obj" \
	"$(INTDIR)\FavOrgDoc.obj" \
	"$(INTDIR)\FavOrgTreeCtl.obj" \
	"$(INTDIR)\FavOrgView.obj" \
	"$(INTDIR)\LegendDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MultiselTreeCtrl.obj" \
	"$(INTDIR)\OptionsDlg.obj" \
	"$(INTDIR)\SelIconDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StringExt.obj" \
	"$(INTDIR)\TreeCtrlEdit.obj" \
	"$(INTDIR)\FavOrg.res"

"$(OUTDIR)\FavOrg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("FavOrg.dep")
!INCLUDE "FavOrg.dep"
!ELSE 
!MESSAGE Warning: cannot find "FavOrg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FavOrg - Win32 Release" || "$(CFG)" == "FavOrg - Win32 Debug"
SOURCE=.\ByStateDlg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\ByStateDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\ByStateDlg.obj"	"$(INTDIR)\ByStateDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\EditUrlDlg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\EditUrlDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\EditUrlDlg.obj"	"$(INTDIR)\EditUrlDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\FavOrg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\FavOrg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\FavOrg.obj"	"$(INTDIR)\FavOrg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\FavOrg.rc

"$(INTDIR)\FavOrg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\FavOrgDoc.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\FavOrgDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\FavOrgDoc.obj"	"$(INTDIR)\FavOrgDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\FavOrgTreeCtl.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\FavOrgTreeCtl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\FavOrgTreeCtl.obj"	"$(INTDIR)\FavOrgTreeCtl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\FavOrgView.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\FavOrgView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\FavOrgView.obj"	"$(INTDIR)\FavOrgView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\LegendDlg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\LegendDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\LegendDlg.obj"	"$(INTDIR)\LegendDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\MultiselTreeCtrl.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\MultiselTreeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\MultiselTreeCtrl.obj"	"$(INTDIR)\MultiselTreeCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\OptionsDlg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\OptionsDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\OptionsDlg.obj"	"$(INTDIR)\OptionsDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\SelIconDlg.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\SelIconDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\SelIconDlg.obj"	"$(INTDIR)\SelIconDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\FavOrg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FavOrg.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\FavOrg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\FavOrg.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\StringExt.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\StringExt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\StringExt.obj"	"$(INTDIR)\StringExt.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 

SOURCE=.\TreeCtrlEdit.cpp

!IF  "$(CFG)" == "FavOrg - Win32 Release"


"$(INTDIR)\TreeCtrlEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ELSEIF  "$(CFG)" == "FavOrg - Win32 Debug"


"$(INTDIR)\TreeCtrlEdit.obj"	"$(INTDIR)\TreeCtrlEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FavOrg.pch"


!ENDIF 


!ENDIF 

