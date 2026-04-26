// Crimson Editor Main Header File

#ifndef __CEDT_HEADER_H_
#define __CEDT_HEADER_H_


#define STRING_COMPANYNAME					"Crimson System"
#define STRING_PROJECTFILEVER				"Crimson Editor 3.60"
#define STRING_CONFIGURATIONVER				"Configuration 3.71"
#define STRING_COLORSETTINGSVER				"Color Settings 3.71"
#define STRING_FTPACCOUNTVER				"FTP Account 3.71"
#define STRING_USERTOOLSVER					"User Command 3.40"
#define STRING_USERMACROVER					"User Macro 3.50 Beta"

#define STRING_COPYRIGHT1					"\r\n  Crimson Editor 3.71 Korean (Freeware)"
#define STRING_COPYRIGHT2					"\r\n"
#define STRING_COPYRIGHT3					"\r\n  Copyright (C) 1999-2005 Ingyu Kang, All rights reserved."
#define STRING_COPYRIGHT4					"\r\n"

#define STRING_HOMEPAGEURL					"http://www.crimsoneditor.com/"
#define STRING_SPONSORURL					"http://www.crimsoneditor.com/english/sponsor.html"
#define STRING_EMAILADDRESS					"mailto:crimsonware@yahoo.com"

#define REGPATH_INSTALL_DIRECTORY			"Software\\Crimson System\\Crimson Editor"
#define REGPATH_USEININTERNETEXPLORER		"Software\\Microsoft\\Internet Explorer\\View Source Editor\\Editor Name"
#define REGPATH_SHELLEXTENSIONAPPROVED		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"
#define REGPATH_ADDTORIGHTMOUSEBUTTON		"*\\shellex\\ContextMenuHandlers\\Crimson Editor"

#define CLSID_SHELLEXT_CRIMSONEDITOR		"{475A9681-F01B-11d5-BC5E-0050CE184C9B}"

#define PROGID_SHELLEXT_CRIMSONEDITOR		"CrimsonEditor.ShellExt"
#define PROGID_FILEASSOC_CRIMSONEDITOR		"CrimsonEditor"

#define MUTEX_NAME_CMDLINE					"CrimsonEditor.CmdLine"
#define CLIPBRD_FORMAT_PROJECT_ITEM			"CrimsonEditor.ProjectItem"
#define CLIPBRD_FORMAT_FILETAB_ITEM			"CrimsonEditor.FileTabItem"

#define REGKEY_INSTALL_DIRECTORY			"Install Directory"
#define REGKEY_ALLOW_MULTI_INSTANCES		"Allow Multi-Instances"
#define REGKEY_BROWSING_DIRECTORY			"Browsing Directory"
#define REGKEY_WORKING_DIRECTORY			"Working Directory"
#define REGKEY_LAST_WORKSPACE				"Last Workspace File"
#define REGKEY_WINDOW_PLACEMENT				"Window Placement"
#define REGKEY_BAR_STATE					"Bar State"


// USER MESSSAGE
#define WM_ANOTHER_INSTANCE					(WM_USER+1)
#define WM_SIZE_MAIN_WINDOW					(WM_USER+2)
#define WM_BEGINDRAG_TAB_CTRL				(WM_USER+3)


// DRAG_OBJECT_TYPE
#define DRAG_OBJECT_UNKNOWN					0
#define DRAG_OBJECT_TEXT					1
#define DRAG_OBJECT_HDROP					2
#define DRAG_OBJECT_PROJECT_ITEM			3
#define DRAG_OBJECT_FILETAB_ITEM			4


// MAXIMUM CONSTANTS
#define MAX_FTP_ACCOUNT						16
#define MAX_FILE_FILTER						32
#define MAX_SYNTAX_TYPE						32


// MACROS
#define _IS_BET(min, x, max)				((x) >= (min) && (x) <= (max))
#define _MY_MAX(x, y)						((x) > (y) ? (x) : (y))
#define _MY_MIN(x, y)						((x) < (y) ? (x) : (y))

#define ASSURE_BOUND_VALUE(x, min, max)	{	\
	x = (x < (min)) ? (min) : x;			\
	x = (x > (max)) ? (max) : x;			\
}


// HEADERS
#include <fstream.h>
#include "resource.h"
#include "PathName.h"
#include "RegExp.h"
#include "SortStringArray.h"
#include "XPTabCtrl.h"
#include "ColorListBox.h"
#include "Utility.h"

#include "cedtElement.h"
#include "StatusBarEx.h"
#include "SplitterWndEx.h"
#include "FileTabDropTarget.h"
#include "FileTab.h"
#include "SizeCBar.h"
#include "FileWndDropTarget.h"
#include "FileWnd.h"
#include "VerticalStatic.h"
#include "OutputWindow.h"

#include "RemoteFile.h"
#include "CmdLine.h"
#include "cedtApp.h"
#include "MainFrmDropTarget.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "cedtDoc.h"
#include "cedtView.h"


#endif // __CEDT_HEADER_H_
