// Crimson Editor Main Header File

#ifndef __CEDT_HEADER_H_
#define __CEDT_HEADER_H_


#define STRING_COMPANYNAME					_T("Crimson System")
#define STRING_PROJECTFILEVER				_T("Crimson Editor 3.83")

#define STRING_CONFIGURATIONVER				_T("Configuration 3.80 x64")
#define STRING_COLORSETTINGSVER				_T("Color Settings 3.80 x64")
#define STRING_FTPACCOUNTVER				_T("FTP Account 3.80 x64")
#define STRING_USERTOOLSVER					_T("User Command 3.80 x64")
#define STRING_USERMACROVER					_T("User Macro 3.80 x64")

// The user config file stores the file-filter and syntax-type display
// strings, which are language-dependent. Give each edition its own file
// under %APPDATA%\Crimson Editor so KR and US installs don't overwrite
// each other's list when the user switches editions. CEDT_LANG_KR /
// CEDT_LANG_US are defined by cedt.vcxproj (see the language-edition
// ItemDefinitionGroup blocks).
#if defined(CEDT_LANG_KR)
#	define STRING_CONFFILENAME				_T("cedt_kr.conf")
#elif defined(CEDT_LANG_US)
#	define STRING_CONFFILENAME				_T("cedt_us.conf")
#else
#	error "Neither CEDT_LANG_KR nor CEDT_LANG_US is defined — check the cedt.vcxproj configuration."
#endif

// Homepage and sponsor pages have per-language landing URLs, so branch
// on the edition macro (same one that switches STRING_CONFIGURATIONVER).
#if defined(CEDT_LANG_KR)
#	define STRING_HOMEPAGEURL				_T("https://crimsoneditor.com/ko/")
#	define STRING_SPONSORURL				_T("https://crimsoneditor.com/ko/sponsor/")
#else
#	define STRING_HOMEPAGEURL				_T("https://crimsoneditor.com/")
#	define STRING_SPONSORURL				_T("https://crimsoneditor.com/sponsor/")
#endif
// Feedback used to be a mailto: to the developer's yahoo address —
// switched to the GitHub issue tracker so reports are public and
// searchable, and there is no per-edition variant.
#define STRING_FEEDBACKURL					_T("https://github.com/igkang00/CrimsonEditor/issues")

// Help ▸ Help Topics used to open a locally-bundled runtime/docs/*.html
// tree, which drifted out of date every release. Point it at the docs
// section of the live site instead — same per-language landing scheme
// as STRING_HOMEPAGEURL.
#if defined(CEDT_LANG_KR)
#	define STRING_HELPDOCSURL				_T("https://crimsoneditor.com/ko/docs/")
#else
#	define STRING_HELPDOCSURL				_T("https://crimsoneditor.com/docs/")
#endif

#define REGPATH_INSTALL_DIRECTORY			_T("Software\\Crimson System\\Crimson Editor")
#define REGPATH_USEININTERNETEXPLORER		_T("Software\\Microsoft\\Internet Explorer\\View Source Editor\\Editor Name")
#define REGPATH_SHELLEXTENSIONAPPROVED		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved")
#define REGPATH_ADDTORIGHTMOUSEBUTTON		_T("*\\shellex\\ContextMenuHandlers\\Crimson Editor")

#define CLSID_SHELLEXT_CRIMSONEDITOR		_T("{475A9681-F01B-11d5-BC5E-0050CE184C9B}")

#define PROGID_SHELLEXT_CRIMSONEDITOR		_T("CrimsonEditor.ShellExt")
#define PROGID_FILEASSOC_CRIMSONEDITOR		_T("CrimsonEditor")

#define MUTEX_NAME_CMDLINE					_T("CrimsonEditor.CmdLine")
#define CLIPBRD_FORMAT_PROJECT_ITEM			_T("CrimsonEditor.ProjectItem")
#define CLIPBRD_FORMAT_FILETAB_ITEM			_T("CrimsonEditor.FileTabItem")

#define REGKEY_ALLOW_MULTI_INSTANCES		_T("Allow Multi-Instances")
#define REGKEY_BROWSING_DIRECTORY			_T("Browsing Directory")
#define REGKEY_WORKING_DIRECTORY			_T("Working Directory")
#define REGKEY_LAST_WORKSPACE				_T("Last Workspace File")
#define REGKEY_WINDOW_PLACEMENT				_T("Window Placement")
#define REGKEY_BAR_STATE					_T("Bar State")


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
#include "fstream_compat.h"
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
