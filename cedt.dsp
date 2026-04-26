# Microsoft Developer Studio Project File - Name="cedt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cedt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cedt.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cedt - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imm32.lib htmlhelp.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "cedt - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imm32.lib htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "cedt - Win32 Release"
# Name "cedt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cedtapp.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppConf.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppFile.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppHndr.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppProject.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtAppView.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocAnal.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocDictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocEditAdv.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocFile.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocHndr.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocMap.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocSyntax.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtDocView.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtElement.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtView.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewAction.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewCaret.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewEditAdv.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewEditCompose.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewFont.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewHighlight.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewHndrEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewHndrMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMacro.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMap.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMapAdv.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMetric.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewMove.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\cedtViewUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTab.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTabDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWndDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWndDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWndProject.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWndRemote.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrmDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrmHndr.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterWndEx.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cedtApp.h
# End Source File
# Begin Source File

SOURCE=.\cedtColors.h
# End Source File
# Begin Source File

SOURCE=.\cedtDoc.h
# End Source File
# Begin Source File

SOURCE=.\cedtElement.h
# End Source File
# Begin Source File

SOURCE=.\cedtHeader.h
# End Source File
# Begin Source File

SOURCE=.\cedtView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\FileTab.h
# End Source File
# Begin Source File

SOURCE=.\FileTabDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\FileWnd.h
# End Source File
# Begin Source File

SOURCE=.\FileWndDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrmDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\OutputWindow.h
# End Source File
# Begin Source File

SOURCE=.\RemoteFile.h
# End Source File
# Begin Source File

SOURCE=.\SplitterWndEx.h
# End Source File
# Begin Source File

SOURCE=.\StatusBarEx.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arrowmacro.cur
# End Source File
# Begin Source File

SOURCE=.\res\cedt.manifest
# End Source File
# Begin Source File

SOURCE=.\res\cedt.rc2
# End Source File
# Begin Source File

SOURCE=.\res\cedticon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cedtlogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cedttype.ico
# End Source File
# Begin Source File

SOURCE=.\res\fwincategory.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fwintooldir.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fwintoolprj.bmp
# End Source File
# Begin Source File

SOURCE=.\res\genbuttons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ibeammacro.cur
# End Source File
# Begin Source File

SOURCE=.\res\mainfrm.ico
# End Source File
# Begin Source File

SOURCE=.\res\mdifiletab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\projectitem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\remotehost.bmp
# End Source File
# Begin Source File

SOURCE=.\res\remoteitem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\remoteopen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rightarrow.cur
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\winbuttons.bmp
# End Source File
# End Group
# Begin Group "Utility Files"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\CmdLine.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdLine.h
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.h
# End Source File
# Begin Source File

SOURCE=.\date.cpp
# End Source File
# Begin Source File

SOURCE=.\date.h
# End Source File
# Begin Source File

SOURCE=.\encode.cpp
# End Source File
# Begin Source File

SOURCE=.\encode.h
# End Source File
# Begin Source File

SOURCE=.\evaluate.cpp
# End Source File
# Begin Source File

SOURCE=.\evaluate.h
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\FtpClnt.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpClnt.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\PathName.cpp
# End Source File
# Begin Source File

SOURCE=.\PathName.h
# End Source File
# Begin Source File

SOURCE=.\RegExp.cpp
# End Source File
# Begin Source File

SOURCE=.\RegExp.h
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\Separator.cpp
# End Source File
# Begin Source File

SOURCE=.\Separator.h
# End Source File
# Begin Source File

SOURCE=.\SizeCBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SizeCBar.h
# End Source File
# Begin Source File

SOURCE=.\SortStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\SortStringArray.h
# End Source File
# Begin Source File

SOURCE=.\Utility.cpp
# End Source File
# Begin Source File

SOURCE=.\Utility.h
# End Source File
# Begin Source File

SOURCE=.\VerticalStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\VerticalStatic.h
# End Source File
# Begin Source File

SOURCE=.\XPTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XPTabCtrl.h
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\AskReplaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AskReplaceDialog.h
# End Source File
# Begin Source File

SOURCE=.\DocumentSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentSummary.h
# End Source File
# Begin Source File

SOURCE=.\DummyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DummyDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindInFilesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindInFilesDialog.h
# End Source File
# Begin Source File

SOURCE=.\FtpAdvancedDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpAdvancedDialog.h
# End Source File
# Begin Source File

SOURCE=.\FtpPasswordDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpPasswordDialog.h
# End Source File
# Begin Source File

SOURCE=.\FtpSettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpSettingsDialog.h
# End Source File
# Begin Source File

SOURCE=.\FtpTransferDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpTransferDialog.h
# End Source File
# Begin Source File

SOURCE=.\GoToDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToDialog.h
# End Source File
# Begin Source File

SOURCE=.\MacroDefineDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroDefineDialog.h
# End Source File
# Begin Source File

SOURCE=.\OpenRemoteDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenRemoteDialog.h
# End Source File
# Begin Source File

SOURCE=.\ReloadAsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ReloadAsDialog.h
# End Source File
# Begin Source File

SOURCE=.\ReplaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplaceDialog.h
# End Source File
# Begin Source File

SOURCE=.\UserInputDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInputDialog.h
# End Source File
# End Group
# Begin Group "Languages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cedt_kr.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Preferences"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PrefDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialog.h
# End Source File
# Begin Source File

SOURCE=.\PrefDialogAssoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogColors.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogFonts.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogMacros.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogSyntax.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogTools.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefDialogVisual.cpp
# End Source File
# End Group
# End Target
# End Project
