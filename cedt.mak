# Microsoft Developer Studio Generated NMAKE File, Based on cedt.dsp
!IF "$(CFG)" == ""
CFG=cedt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cedt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cedt - Win32 Release" && "$(CFG)" != "cedt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cedt.mak" CFG="cedt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cedt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cedt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cedt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\cedt.exe"


CLEAN :
	-@erase "$(INTDIR)\AboutDialog.obj"
	-@erase "$(INTDIR)\AskReplaceDialog.obj"
	-@erase "$(INTDIR)\AuxWindow.obj"
	-@erase "$(INTDIR)\AuxWindowAction.obj"
	-@erase "$(INTDIR)\AuxWindowHndr.obj"
	-@erase "$(INTDIR)\cedt.pch"
	-@erase "$(INTDIR)\cedt_us.res"
	-@erase "$(INTDIR)\cedtApp.obj"
	-@erase "$(INTDIR)\cedtAppConf.obj"
	-@erase "$(INTDIR)\cedtAppFile.obj"
	-@erase "$(INTDIR)\cedtAppHndr.obj"
	-@erase "$(INTDIR)\cedtDoc.obj"
	-@erase "$(INTDIR)\cedtDocAnal.obj"
	-@erase "$(INTDIR)\cedtDocDictionary.obj"
	-@erase "$(INTDIR)\cedtDocEdit.obj"
	-@erase "$(INTDIR)\cedtDocEditAdv.obj"
	-@erase "$(INTDIR)\cedtDocFile.obj"
	-@erase "$(INTDIR)\cedtDocHndr.obj"
	-@erase "$(INTDIR)\cedtDocMap.obj"
	-@erase "$(INTDIR)\cedtDocSearch.obj"
	-@erase "$(INTDIR)\cedtDocSyntax.obj"
	-@erase "$(INTDIR)\cedtDocUndo.obj"
	-@erase "$(INTDIR)\cedtElement.obj"
	-@erase "$(INTDIR)\cedtView.obj"
	-@erase "$(INTDIR)\cedtViewAll.obj"
	-@erase "$(INTDIR)\cedtViewCaret.obj"
	-@erase "$(INTDIR)\cedtViewCommand.obj"
	-@erase "$(INTDIR)\cedtViewConf.obj"
	-@erase "$(INTDIR)\cedtViewDraw.obj"
	-@erase "$(INTDIR)\cedtViewEdit.obj"
	-@erase "$(INTDIR)\cedtViewEditAdv.obj"
	-@erase "$(INTDIR)\cedtViewEvnt.obj"
	-@erase "$(INTDIR)\cedtViewFormat.obj"
	-@erase "$(INTDIR)\cedtViewHighlight.obj"
	-@erase "$(INTDIR)\cedtViewHndr.obj"
	-@erase "$(INTDIR)\cedtViewMacro.obj"
	-@erase "$(INTDIR)\cedtViewMap.obj"
	-@erase "$(INTDIR)\cedtViewMisc.obj"
	-@erase "$(INTDIR)\cedtViewMove.obj"
	-@erase "$(INTDIR)\cedtViewPrint.obj"
	-@erase "$(INTDIR)\cedtViewSearch.obj"
	-@erase "$(INTDIR)\cedtViewSelect.obj"
	-@erase "$(INTDIR)\cedtViewUndo.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\date.obj"
	-@erase "$(INTDIR)\DocumentSummary.obj"
	-@erase "$(INTDIR)\DummyDialog.obj"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\evaluate.obj"
	-@erase "$(INTDIR)\FileTab.obj"
	-@erase "$(INTDIR)\FindDialog.obj"
	-@erase "$(INTDIR)\FindInFilesDialog.obj"
	-@erase "$(INTDIR)\FolderDialog.obj"
	-@erase "$(INTDIR)\FtpAdvancedDialog.obj"
	-@erase "$(INTDIR)\FtpClient.obj"
	-@erase "$(INTDIR)\FtpPasswordDialog.obj"
	-@erase "$(INTDIR)\FtpSettingsDialog.obj"
	-@erase "$(INTDIR)\GoToDialog.obj"
	-@erase "$(INTDIR)\HyperLink.obj"
	-@erase "$(INTDIR)\MacroDefineDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrmHndr.obj"
	-@erase "$(INTDIR)\MainFrmStatus.obj"
	-@erase "$(INTDIR)\OpenRemoteDialog.obj"
	-@erase "$(INTDIR)\OutputWindow.obj"
	-@erase "$(INTDIR)\PathName.obj"
	-@erase "$(INTDIR)\PrefDialog.obj"
	-@erase "$(INTDIR)\PrefDialogAssociation.obj"
	-@erase "$(INTDIR)\PrefDialogBackup.obj"
	-@erase "$(INTDIR)\PrefDialogColors.obj"
	-@erase "$(INTDIR)\PrefDialogCommands.obj"
	-@erase "$(INTDIR)\PrefDialogFile.obj"
	-@erase "$(INTDIR)\PrefDialogFilters.obj"
	-@erase "$(INTDIR)\PrefDialogFonts.obj"
	-@erase "$(INTDIR)\PrefDialogGeneral.obj"
	-@erase "$(INTDIR)\PrefDialogMacros.obj"
	-@erase "$(INTDIR)\PrefDialogPrint.obj"
	-@erase "$(INTDIR)\PrefDialogSyntax.obj"
	-@erase "$(INTDIR)\PrefDialogTools.obj"
	-@erase "$(INTDIR)\PrefDialogWordWrap.obj"
	-@erase "$(INTDIR)\RegExp.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\RemoteFile.obj"
	-@erase "$(INTDIR)\ReplaceDialog.obj"
	-@erase "$(INTDIR)\Separator.obj"
	-@erase "$(INTDIR)\SizeCBar.obj"
	-@erase "$(INTDIR)\SortStringArray.obj"
	-@erase "$(INTDIR)\StatusBarEx.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UserInputDialog.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VerticalStatic.obj"
	-@erase "$(OUTDIR)\cedt.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\cedt.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cedt_us.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cedt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=imm32.lib htmlhelp.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\cedt.pdb" /machine:I386 /out:"$(OUTDIR)\cedt.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AuxWindow.obj" \
	"$(INTDIR)\AuxWindowAction.obj" \
	"$(INTDIR)\AuxWindowHndr.obj" \
	"$(INTDIR)\cedtApp.obj" \
	"$(INTDIR)\cedtAppConf.obj" \
	"$(INTDIR)\cedtAppFile.obj" \
	"$(INTDIR)\cedtAppHndr.obj" \
	"$(INTDIR)\cedtDoc.obj" \
	"$(INTDIR)\cedtDocAnal.obj" \
	"$(INTDIR)\cedtDocDictionary.obj" \
	"$(INTDIR)\cedtDocEdit.obj" \
	"$(INTDIR)\cedtDocEditAdv.obj" \
	"$(INTDIR)\cedtDocFile.obj" \
	"$(INTDIR)\cedtDocHndr.obj" \
	"$(INTDIR)\cedtDocMap.obj" \
	"$(INTDIR)\cedtDocSearch.obj" \
	"$(INTDIR)\cedtDocSyntax.obj" \
	"$(INTDIR)\cedtDocUndo.obj" \
	"$(INTDIR)\cedtElement.obj" \
	"$(INTDIR)\cedtView.obj" \
	"$(INTDIR)\cedtViewAll.obj" \
	"$(INTDIR)\cedtViewCaret.obj" \
	"$(INTDIR)\cedtViewCommand.obj" \
	"$(INTDIR)\cedtViewConf.obj" \
	"$(INTDIR)\cedtViewDraw.obj" \
	"$(INTDIR)\cedtViewEdit.obj" \
	"$(INTDIR)\cedtViewEditAdv.obj" \
	"$(INTDIR)\cedtViewEvnt.obj" \
	"$(INTDIR)\cedtViewFormat.obj" \
	"$(INTDIR)\cedtViewHighlight.obj" \
	"$(INTDIR)\cedtViewHndr.obj" \
	"$(INTDIR)\cedtViewMacro.obj" \
	"$(INTDIR)\cedtViewMap.obj" \
	"$(INTDIR)\cedtViewMisc.obj" \
	"$(INTDIR)\cedtViewMove.obj" \
	"$(INTDIR)\cedtViewPrint.obj" \
	"$(INTDIR)\cedtViewSearch.obj" \
	"$(INTDIR)\cedtViewSelect.obj" \
	"$(INTDIR)\cedtViewUndo.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\FileTab.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MainFrmHndr.obj" \
	"$(INTDIR)\MainFrmStatus.obj" \
	"$(INTDIR)\OutputWindow.obj" \
	"$(INTDIR)\StatusBarEx.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\date.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\evaluate.obj" \
	"$(INTDIR)\FolderDialog.obj" \
	"$(INTDIR)\FtpClient.obj" \
	"$(INTDIR)\HyperLink.obj" \
	"$(INTDIR)\PathName.obj" \
	"$(INTDIR)\RegExp.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\RemoteFile.obj" \
	"$(INTDIR)\Separator.obj" \
	"$(INTDIR)\SizeCBar.obj" \
	"$(INTDIR)\SortStringArray.obj" \
	"$(INTDIR)\Utility.obj" \
	"$(INTDIR)\VerticalStatic.obj" \
	"$(INTDIR)\AboutDialog.obj" \
	"$(INTDIR)\AskReplaceDialog.obj" \
	"$(INTDIR)\DocumentSummary.obj" \
	"$(INTDIR)\DummyDialog.obj" \
	"$(INTDIR)\FindDialog.obj" \
	"$(INTDIR)\FindInFilesDialog.obj" \
	"$(INTDIR)\FtpAdvancedDialog.obj" \
	"$(INTDIR)\FtpPasswordDialog.obj" \
	"$(INTDIR)\FtpSettingsDialog.obj" \
	"$(INTDIR)\GoToDialog.obj" \
	"$(INTDIR)\MacroDefineDialog.obj" \
	"$(INTDIR)\OpenRemoteDialog.obj" \
	"$(INTDIR)\ReplaceDialog.obj" \
	"$(INTDIR)\UserInputDialog.obj" \
	"$(INTDIR)\PrefDialog.obj" \
	"$(INTDIR)\PrefDialogAssociation.obj" \
	"$(INTDIR)\PrefDialogBackup.obj" \
	"$(INTDIR)\PrefDialogColors.obj" \
	"$(INTDIR)\PrefDialogCommands.obj" \
	"$(INTDIR)\PrefDialogFile.obj" \
	"$(INTDIR)\PrefDialogFilters.obj" \
	"$(INTDIR)\PrefDialogFonts.obj" \
	"$(INTDIR)\PrefDialogGeneral.obj" \
	"$(INTDIR)\PrefDialogMacros.obj" \
	"$(INTDIR)\PrefDialogPrint.obj" \
	"$(INTDIR)\PrefDialogSyntax.obj" \
	"$(INTDIR)\PrefDialogTools.obj" \
	"$(INTDIR)\PrefDialogWordWrap.obj" \
	"$(INTDIR)\cedt_us.res"

"$(OUTDIR)\cedt.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cedt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\cedt.exe"


CLEAN :
	-@erase "$(INTDIR)\AboutDialog.obj"
	-@erase "$(INTDIR)\AskReplaceDialog.obj"
	-@erase "$(INTDIR)\AuxWindow.obj"
	-@erase "$(INTDIR)\AuxWindowAction.obj"
	-@erase "$(INTDIR)\AuxWindowHndr.obj"
	-@erase "$(INTDIR)\cedt.pch"
	-@erase "$(INTDIR)\cedt_us.res"
	-@erase "$(INTDIR)\cedtApp.obj"
	-@erase "$(INTDIR)\cedtAppConf.obj"
	-@erase "$(INTDIR)\cedtAppFile.obj"
	-@erase "$(INTDIR)\cedtAppHndr.obj"
	-@erase "$(INTDIR)\cedtDoc.obj"
	-@erase "$(INTDIR)\cedtDocAnal.obj"
	-@erase "$(INTDIR)\cedtDocDictionary.obj"
	-@erase "$(INTDIR)\cedtDocEdit.obj"
	-@erase "$(INTDIR)\cedtDocEditAdv.obj"
	-@erase "$(INTDIR)\cedtDocFile.obj"
	-@erase "$(INTDIR)\cedtDocHndr.obj"
	-@erase "$(INTDIR)\cedtDocMap.obj"
	-@erase "$(INTDIR)\cedtDocSearch.obj"
	-@erase "$(INTDIR)\cedtDocSyntax.obj"
	-@erase "$(INTDIR)\cedtDocUndo.obj"
	-@erase "$(INTDIR)\cedtElement.obj"
	-@erase "$(INTDIR)\cedtView.obj"
	-@erase "$(INTDIR)\cedtViewAll.obj"
	-@erase "$(INTDIR)\cedtViewCaret.obj"
	-@erase "$(INTDIR)\cedtViewCommand.obj"
	-@erase "$(INTDIR)\cedtViewConf.obj"
	-@erase "$(INTDIR)\cedtViewDraw.obj"
	-@erase "$(INTDIR)\cedtViewEdit.obj"
	-@erase "$(INTDIR)\cedtViewEditAdv.obj"
	-@erase "$(INTDIR)\cedtViewEvnt.obj"
	-@erase "$(INTDIR)\cedtViewFormat.obj"
	-@erase "$(INTDIR)\cedtViewHighlight.obj"
	-@erase "$(INTDIR)\cedtViewHndr.obj"
	-@erase "$(INTDIR)\cedtViewMacro.obj"
	-@erase "$(INTDIR)\cedtViewMap.obj"
	-@erase "$(INTDIR)\cedtViewMisc.obj"
	-@erase "$(INTDIR)\cedtViewMove.obj"
	-@erase "$(INTDIR)\cedtViewPrint.obj"
	-@erase "$(INTDIR)\cedtViewSearch.obj"
	-@erase "$(INTDIR)\cedtViewSelect.obj"
	-@erase "$(INTDIR)\cedtViewUndo.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\date.obj"
	-@erase "$(INTDIR)\DocumentSummary.obj"
	-@erase "$(INTDIR)\DummyDialog.obj"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\evaluate.obj"
	-@erase "$(INTDIR)\FileTab.obj"
	-@erase "$(INTDIR)\FindDialog.obj"
	-@erase "$(INTDIR)\FindInFilesDialog.obj"
	-@erase "$(INTDIR)\FolderDialog.obj"
	-@erase "$(INTDIR)\FtpAdvancedDialog.obj"
	-@erase "$(INTDIR)\FtpClient.obj"
	-@erase "$(INTDIR)\FtpPasswordDialog.obj"
	-@erase "$(INTDIR)\FtpSettingsDialog.obj"
	-@erase "$(INTDIR)\GoToDialog.obj"
	-@erase "$(INTDIR)\HyperLink.obj"
	-@erase "$(INTDIR)\MacroDefineDialog.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrmHndr.obj"
	-@erase "$(INTDIR)\MainFrmStatus.obj"
	-@erase "$(INTDIR)\OpenRemoteDialog.obj"
	-@erase "$(INTDIR)\OutputWindow.obj"
	-@erase "$(INTDIR)\PathName.obj"
	-@erase "$(INTDIR)\PrefDialog.obj"
	-@erase "$(INTDIR)\PrefDialogAssociation.obj"
	-@erase "$(INTDIR)\PrefDialogBackup.obj"
	-@erase "$(INTDIR)\PrefDialogColors.obj"
	-@erase "$(INTDIR)\PrefDialogCommands.obj"
	-@erase "$(INTDIR)\PrefDialogFile.obj"
	-@erase "$(INTDIR)\PrefDialogFilters.obj"
	-@erase "$(INTDIR)\PrefDialogFonts.obj"
	-@erase "$(INTDIR)\PrefDialogGeneral.obj"
	-@erase "$(INTDIR)\PrefDialogMacros.obj"
	-@erase "$(INTDIR)\PrefDialogPrint.obj"
	-@erase "$(INTDIR)\PrefDialogSyntax.obj"
	-@erase "$(INTDIR)\PrefDialogTools.obj"
	-@erase "$(INTDIR)\PrefDialogWordWrap.obj"
	-@erase "$(INTDIR)\RegExp.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\RemoteFile.obj"
	-@erase "$(INTDIR)\ReplaceDialog.obj"
	-@erase "$(INTDIR)\Separator.obj"
	-@erase "$(INTDIR)\SizeCBar.obj"
	-@erase "$(INTDIR)\SortStringArray.obj"
	-@erase "$(INTDIR)\StatusBarEx.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UserInputDialog.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\VerticalStatic.obj"
	-@erase "$(OUTDIR)\cedt.exe"
	-@erase "$(OUTDIR)\cedt.ilk"
	-@erase "$(OUTDIR)\cedt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\cedt.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cedt_us.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cedt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=imm32.lib htmlhelp.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\cedt.pdb" /debug /machine:I386 /out:"$(OUTDIR)\cedt.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AuxWindow.obj" \
	"$(INTDIR)\AuxWindowAction.obj" \
	"$(INTDIR)\AuxWindowHndr.obj" \
	"$(INTDIR)\cedtApp.obj" \
	"$(INTDIR)\cedtAppConf.obj" \
	"$(INTDIR)\cedtAppFile.obj" \
	"$(INTDIR)\cedtAppHndr.obj" \
	"$(INTDIR)\cedtDoc.obj" \
	"$(INTDIR)\cedtDocAnal.obj" \
	"$(INTDIR)\cedtDocDictionary.obj" \
	"$(INTDIR)\cedtDocEdit.obj" \
	"$(INTDIR)\cedtDocEditAdv.obj" \
	"$(INTDIR)\cedtDocFile.obj" \
	"$(INTDIR)\cedtDocHndr.obj" \
	"$(INTDIR)\cedtDocMap.obj" \
	"$(INTDIR)\cedtDocSearch.obj" \
	"$(INTDIR)\cedtDocSyntax.obj" \
	"$(INTDIR)\cedtDocUndo.obj" \
	"$(INTDIR)\cedtElement.obj" \
	"$(INTDIR)\cedtView.obj" \
	"$(INTDIR)\cedtViewAll.obj" \
	"$(INTDIR)\cedtViewCaret.obj" \
	"$(INTDIR)\cedtViewCommand.obj" \
	"$(INTDIR)\cedtViewConf.obj" \
	"$(INTDIR)\cedtViewDraw.obj" \
	"$(INTDIR)\cedtViewEdit.obj" \
	"$(INTDIR)\cedtViewEditAdv.obj" \
	"$(INTDIR)\cedtViewEvnt.obj" \
	"$(INTDIR)\cedtViewFormat.obj" \
	"$(INTDIR)\cedtViewHighlight.obj" \
	"$(INTDIR)\cedtViewHndr.obj" \
	"$(INTDIR)\cedtViewMacro.obj" \
	"$(INTDIR)\cedtViewMap.obj" \
	"$(INTDIR)\cedtViewMisc.obj" \
	"$(INTDIR)\cedtViewMove.obj" \
	"$(INTDIR)\cedtViewPrint.obj" \
	"$(INTDIR)\cedtViewSearch.obj" \
	"$(INTDIR)\cedtViewSelect.obj" \
	"$(INTDIR)\cedtViewUndo.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\FileTab.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MainFrmHndr.obj" \
	"$(INTDIR)\MainFrmStatus.obj" \
	"$(INTDIR)\OutputWindow.obj" \
	"$(INTDIR)\StatusBarEx.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\date.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\evaluate.obj" \
	"$(INTDIR)\FolderDialog.obj" \
	"$(INTDIR)\FtpClient.obj" \
	"$(INTDIR)\HyperLink.obj" \
	"$(INTDIR)\PathName.obj" \
	"$(INTDIR)\RegExp.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\RemoteFile.obj" \
	"$(INTDIR)\Separator.obj" \
	"$(INTDIR)\SizeCBar.obj" \
	"$(INTDIR)\SortStringArray.obj" \
	"$(INTDIR)\Utility.obj" \
	"$(INTDIR)\VerticalStatic.obj" \
	"$(INTDIR)\AboutDialog.obj" \
	"$(INTDIR)\AskReplaceDialog.obj" \
	"$(INTDIR)\DocumentSummary.obj" \
	"$(INTDIR)\DummyDialog.obj" \
	"$(INTDIR)\FindDialog.obj" \
	"$(INTDIR)\FindInFilesDialog.obj" \
	"$(INTDIR)\FtpAdvancedDialog.obj" \
	"$(INTDIR)\FtpPasswordDialog.obj" \
	"$(INTDIR)\FtpSettingsDialog.obj" \
	"$(INTDIR)\GoToDialog.obj" \
	"$(INTDIR)\MacroDefineDialog.obj" \
	"$(INTDIR)\OpenRemoteDialog.obj" \
	"$(INTDIR)\ReplaceDialog.obj" \
	"$(INTDIR)\UserInputDialog.obj" \
	"$(INTDIR)\PrefDialog.obj" \
	"$(INTDIR)\PrefDialogAssociation.obj" \
	"$(INTDIR)\PrefDialogBackup.obj" \
	"$(INTDIR)\PrefDialogColors.obj" \
	"$(INTDIR)\PrefDialogCommands.obj" \
	"$(INTDIR)\PrefDialogFile.obj" \
	"$(INTDIR)\PrefDialogFilters.obj" \
	"$(INTDIR)\PrefDialogFonts.obj" \
	"$(INTDIR)\PrefDialogGeneral.obj" \
	"$(INTDIR)\PrefDialogMacros.obj" \
	"$(INTDIR)\PrefDialogPrint.obj" \
	"$(INTDIR)\PrefDialogSyntax.obj" \
	"$(INTDIR)\PrefDialogTools.obj" \
	"$(INTDIR)\PrefDialogWordWrap.obj" \
	"$(INTDIR)\cedt_us.res"

"$(OUTDIR)\cedt.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cedt.dep")
!INCLUDE "cedt.dep"
!ELSE 
!MESSAGE Warning: cannot find "cedt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cedt - Win32 Release" || "$(CFG)" == "cedt - Win32 Debug"
SOURCE=.\AuxWindow.cpp

"$(INTDIR)\AuxWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\AuxWindowAction.cpp

"$(INTDIR)\AuxWindowAction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\AuxWindowHndr.cpp

"$(INTDIR)\AuxWindowHndr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtApp.cpp

"$(INTDIR)\cedtApp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtAppConf.cpp

"$(INTDIR)\cedtAppConf.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtAppFile.cpp

"$(INTDIR)\cedtAppFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtAppHndr.cpp

"$(INTDIR)\cedtAppHndr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDoc.cpp

"$(INTDIR)\cedtDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocAnal.cpp

"$(INTDIR)\cedtDocAnal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocDictionary.cpp

"$(INTDIR)\cedtDocDictionary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocEdit.cpp

"$(INTDIR)\cedtDocEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocEditAdv.cpp

"$(INTDIR)\cedtDocEditAdv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocFile.cpp

"$(INTDIR)\cedtDocFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocHndr.cpp

"$(INTDIR)\cedtDocHndr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocMap.cpp

"$(INTDIR)\cedtDocMap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocSearch.cpp

"$(INTDIR)\cedtDocSearch.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocSyntax.cpp

"$(INTDIR)\cedtDocSyntax.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtDocUndo.cpp

"$(INTDIR)\cedtDocUndo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtElement.cpp

"$(INTDIR)\cedtElement.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtView.cpp

"$(INTDIR)\cedtView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewAll.cpp

"$(INTDIR)\cedtViewAll.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewCaret.cpp

"$(INTDIR)\cedtViewCaret.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewCommand.cpp

"$(INTDIR)\cedtViewCommand.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewConf.cpp

"$(INTDIR)\cedtViewConf.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewDraw.cpp

"$(INTDIR)\cedtViewDraw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewEdit.cpp

"$(INTDIR)\cedtViewEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewEditAdv.cpp

"$(INTDIR)\cedtViewEditAdv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewEvnt.cpp

"$(INTDIR)\cedtViewEvnt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewFormat.cpp

"$(INTDIR)\cedtViewFormat.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewHighlight.cpp

"$(INTDIR)\cedtViewHighlight.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewHndr.cpp

"$(INTDIR)\cedtViewHndr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewMacro.cpp

"$(INTDIR)\cedtViewMacro.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewMap.cpp

"$(INTDIR)\cedtViewMap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewMisc.cpp

"$(INTDIR)\cedtViewMisc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewMove.cpp

"$(INTDIR)\cedtViewMove.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewPrint.cpp

"$(INTDIR)\cedtViewPrint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewSearch.cpp

"$(INTDIR)\cedtViewSearch.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewSelect.cpp

"$(INTDIR)\cedtViewSelect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedtViewUndo.cpp

"$(INTDIR)\cedtViewUndo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\ChildFrm.cpp

"$(INTDIR)\ChildFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FileTab.cpp

"$(INTDIR)\FileTab.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\MainFrmHndr.cpp

"$(INTDIR)\MainFrmHndr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\MainFrmStatus.cpp

"$(INTDIR)\MainFrmStatus.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\OutputWindow.cpp

"$(INTDIR)\OutputWindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\StatusBarEx.cpp

"$(INTDIR)\StatusBarEx.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "cedt - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\cedt.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\cedt.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "cedt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\cedt.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\cedt.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\CmdLine.cpp

"$(INTDIR)\CmdLine.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\date.cpp

"$(INTDIR)\date.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\encode.cpp

"$(INTDIR)\encode.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\evaluate.cpp

"$(INTDIR)\evaluate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FolderDialog.cpp

"$(INTDIR)\FolderDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FtpClient.cpp

"$(INTDIR)\FtpClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\HyperLink.cpp

"$(INTDIR)\HyperLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PathName.cpp

"$(INTDIR)\PathName.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\RegExp.cpp

"$(INTDIR)\RegExp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\registry.cpp

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\RemoteFile.cpp

"$(INTDIR)\RemoteFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\Separator.cpp

"$(INTDIR)\Separator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\SizeCBar.cpp

"$(INTDIR)\SizeCBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\SortStringArray.cpp

"$(INTDIR)\SortStringArray.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\Utility.cpp

"$(INTDIR)\Utility.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\VerticalStatic.cpp

"$(INTDIR)\VerticalStatic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\AboutDialog.cpp

"$(INTDIR)\AboutDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\AskReplaceDialog.cpp

"$(INTDIR)\AskReplaceDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\DocumentSummary.cpp

"$(INTDIR)\DocumentSummary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\DummyDialog.cpp

"$(INTDIR)\DummyDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FindDialog.cpp

"$(INTDIR)\FindDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FindInFilesDialog.cpp

"$(INTDIR)\FindInFilesDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FtpAdvancedDialog.cpp

"$(INTDIR)\FtpAdvancedDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FtpPasswordDialog.cpp

"$(INTDIR)\FtpPasswordDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\FtpSettingsDialog.cpp

"$(INTDIR)\FtpSettingsDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\GoToDialog.cpp

"$(INTDIR)\GoToDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\MacroDefineDialog.cpp

"$(INTDIR)\MacroDefineDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\OpenRemoteDialog.cpp

"$(INTDIR)\OpenRemoteDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\ReplaceDialog.cpp

"$(INTDIR)\ReplaceDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\UserInputDialog.cpp

"$(INTDIR)\UserInputDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\cedt_us.rc

"$(INTDIR)\cedt_us.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\PrefDialog.cpp

"$(INTDIR)\PrefDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogAssociation.cpp

"$(INTDIR)\PrefDialogAssociation.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogBackup.cpp

"$(INTDIR)\PrefDialogBackup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogColors.cpp

"$(INTDIR)\PrefDialogColors.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogCommands.cpp

"$(INTDIR)\PrefDialogCommands.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogFile.cpp

"$(INTDIR)\PrefDialogFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogFilters.cpp

"$(INTDIR)\PrefDialogFilters.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogFonts.cpp

"$(INTDIR)\PrefDialogFonts.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogGeneral.cpp

"$(INTDIR)\PrefDialogGeneral.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogMacros.cpp

"$(INTDIR)\PrefDialogMacros.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogPrint.cpp

"$(INTDIR)\PrefDialogPrint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogSyntax.cpp

"$(INTDIR)\PrefDialogSyntax.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogTools.cpp

"$(INTDIR)\PrefDialogTools.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"


SOURCE=.\PrefDialogWordWrap.cpp

"$(INTDIR)\PrefDialogWordWrap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cedt.pch"



!ENDIF 

