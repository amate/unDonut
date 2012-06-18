/**
 *	@file	DonutDefine.h
 *	@brief	Donut固有のWindowメッセージの定義.
 */

#ifndef __DONUTDEFINE_H__
#define __DONUTDEFINE_H__

#pragma once

#define MYWM_NOTIFYICON 	(WM_APP + 100)		// UDT DGSTR
#define TM_TRAY 			0					// UDT DGSTR

// Don't change for saving rebar state
#define IDC_ADDRESSBAR					101		// アドレスバー
#define IDC_GOBUTTON					102 	//+++ 未使用ぽい?
#define IDC_MDITAB						103		// タブバー
#define IDC_LINKBAR 					104		// リンクバー
#define IDC_SEARCHBAR					105		// 検索バー
#define IDC_PLUGIN_TOOLBAR				151		// プラグインツールバー
#define IDC_PLUGIN_IETOOLBAR			180		//+++ テスト.
#define IDC_PLUGIN_EXPLORERBAR			181 	//minit

#define IDC_PROGRESS					201
#define IDC_COMBBOX 					202

// #define DONUT_HAS_CMDBAR

enum DonutOpenFileFlags {
	D_OPENFILE_CREATETAB	= 0x00000000,	// Normal
	D_OPENFILE_ACTIVATE 	= 0x00000001,	// 作成したタブをアクティブにする
	D_OPENFILE_NOCREATE 	= 0x00000002,	// 現在表示中のタブでナビゲートする
	D_OPENFILE_NOSETFOCUS	= 0x00000004,
};

// Devstudio doesn't know BEGIN_MSG_MAP_EX
#undef	BEGIN_MSG_MAP
#define BEGIN_MSG_MAP			BEGIN_MSG_MAP_EX


#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL			0x020A
#endif

#define DONUT_THREADWAIT		5000					//+++ スレッドの終了に5秒待つ
#define DONUT_BACKUP_THREADWAIT 90000					//+++ 自動更新スレッドの終了に90秒待つ


enum WmCopyDataUseage {
	kNewDonutInstance	= 1,
	kNewDonutLink,
	kSetFaviconURL,
	kSearchTextWithEngine,
	kSetSearchText,
	kHilightText,
	kNavigateChildFrame,
	kOpenMultiUrl,
	kFileDownload,
	kExecuteUserJavascript,
	kHilightFromFindBar,
	kOpenFindBarWithText,
	kDebugTrace = 0xFFFF,

};

// UH
__declspec(selectany) BOOL	   	g_bSwapProxy 		 = FALSE;
__declspec(selectany) DWORD    	g_dwProgressPainWidth = 0;			//*+++ 手抜きでグローバル変数。あとで、なんとかする...
extern	class CMainFrame *		g_pMainWnd;


// to emulate DDE.
#define WM_NEWINSTANCE			(WM_USER + 16)
#define USER_MSG_WM_NEWINSTANCE(func) \
	if (uMsg == WM_NEWINSTANCE) {	  \
		SetMsgHandled(TRUE);		  \
		func( (ATOM) wParam );		  \
		lResult = 0;				  \
		if ( IsMsgHandled() )		  \
			return TRUE;			  \
	}

// to save child window options
#define WM_SAVE_OPTION	(WM_USER + 20)
#define USER_MSG_WM_SAVE_OPTION(func)			\
	if (uMsg == WM_SAVE_OPTION) {				\
		SetMsgHandled(TRUE);					\
		func( (LPCTSTR) wParam, (int) lParam ); \
		lResult = 0;							\
		if ( IsMsgHandled() )					\
			return TRUE;						\
	}

// UDT DGSTR
#define WM_UPDATE_TITLEBAR	(WM_USER + 24)
#define USER_MSG_WM_UPDATE_TITLEBAR(func)							  \
	if (uMsg == WM_UPDATE_TITLEBAR) {								  \
		SetMsgHandled(TRUE);										  \
		lResult = (LRESULT) func( (LPCTSTR) wParam, (DWORD) lParam ); \
		if ( IsMsgHandled() )										  \
			return TRUE;											  \
	}
// ENDE

// UDT DGSTR
#define WM_UPDATE_EXPBAR	(WM_USER + 28)
#define USER_MSG_WM_UPDATE_EXPBAR(func) 							  \
	if (uMsg == WM_UPDATE_EXPBAR) { 								  \
		SetMsgHandled(TRUE);										  \
		lResult = (LRESULT) func( (LPCTSTR) wParam, (DWORD) lParam ); \
		if ( IsMsgHandled() )										  \
			return TRUE;											  \
	}
// ENDE

// UH
#define WM_MENU_GOBACK	(WM_USER + 50)
#define USER_MSG_WM_MENU_GOBACK(func)				\
	if (uMsg == WM_MENU_GOBACK) {					\
		SetMsgHandled(TRUE);						\
		lResult = (LRESULT) func( (HMENU) wParam ); \
		if ( IsMsgHandled() )						\
			return TRUE;							\
	}

#define WM_MENU_GOFORWARD	(WM_USER + 51)
#define USER_MSG_WM_MENU_GOFORWARD(func)			\
	if (uMsg == WM_MENU_GOFORWARD) {				\
		SetMsgHandled(TRUE);						\
		lResult = (LRESULT) func( (HMENU) wParam ); \
		if ( IsMsgHandled() )						\
			return TRUE;							\
	}

#define WM_MENU_GET_FAV 	(WM_USER + 52)
#define USER_MEG_WM_MENU_GET_FAV(func) \
	if (uMsg == WM_MENU_GET_FAV) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = (LRESULT) func();    \
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_MENU_GET_FAV_GROUP	(WM_USER + 53)
#define USER_MEG_WM_MENU_GET_FAV_GROUP(func) \
	if (uMsg == WM_MENU_GET_FAV_GROUP) {	 \
		SetMsgHandled(TRUE);				 \
		lResult = (LRESULT) func(); 		 \
		if ( IsMsgHandled() )				 \
			return TRUE; 				 \
	}

#define WM_USER_HILIGHT 	(WM_USER + 54)
#define MSG_WM_USER_HILIGHT(func)					  \
	if (uMsg == WM_USER_HILIGHT) {					  \
		SetMsgHandled(TRUE);						  \
		lResult = (LRESULT) func( (LPCTSTR) wParam ); \
		if ( IsMsgHandled() )						  \
			return TRUE;							  \
	}

#define WM_USER_FIND_KEYWORD	(WM_USER + 55)
#define MSG_WM_USER_FIND_KEYWORD(func)								 \
	if (uMsg == WM_USER_FIND_KEYWORD) { 							 \
		SetMsgHandled(TRUE);										 \
		lResult = (LRESULT) func( (LPCTSTR) wParam, (BOOL) LOWORD(lParam), (long) HIWORD(lParam) ); \
		if ( IsMsgHandled() )										 \
			return TRUE;											 \
	}

#define WM_USER_USED_MOUSE_GESTURE		(WM_USER + 56)
#define MSG_WM_USER_USED_MOUSE_GESTURE(func)  \
	if (uMsg == WM_USER_USED_MOUSE_GESTURE) { \
		SetMsgHandled(TRUE);				  \
		lResult = func(); 					  \
		if ( IsMsgHandled() )				  \
			return TRUE; 				  \
	}

#define WM_USER_CHANGE_CSS		(WM_USER + 57)
#define MSG_WM_USER_CHANGE_CSS(func)				  \
	if (uMsg == WM_USER_CHANGE_CSS) {				  \
		SetMsgHandled(TRUE);						  \
		lResult = (LRESULT) func( (LPCTSTR) wParam ); \
		if ( IsMsgHandled() )						  \
			return TRUE; 						  \
	}

#if 0	//+++ 未使用?
#define WM_MENU_GET_CSS 		(WM_USER + 58)
#endif

#define WM_MENU_GET_SCRIPT		(WM_USER + 59)
#define USER_MEG_WM_MENU_GET_SCRIPT(func) \
	if (uMsg == WM_MENU_GET_SCRIPT) {	  \
		SetMsgHandled(TRUE);			  \
		lResult = (LRESULT) func(); 	  \
		if ( IsMsgHandled() )			  \
			return TRUE; 			  \
	}

#define WM_STATUS_SETICON		(WM_USER + 60)
#define WM_STATUS_SETTIPTEXT	(WM_USER + 61)		// 20061119追加

//End UH


//minit
#define WM_MENU_REFRESH_FAV 	(WM_USER + 80)
#define USER_MEG_WM_MENU_REFRESH_FAV(func)		   \
	if (uMsg == WM_MENU_REFRESH_FAV) {			   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (BOOL) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE; 					   \
	}

#define WM_MENU_REFRESH_FAV_GROUP		(WM_USER + 81)
#define USER_MEG_WM_MENU_REFRESH_FAV_GROUP(func)   \
	if (uMsg == WM_MENU_REFRESH_FAV_GROUP) {	   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (BOOL) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE; 					   \
	}

#define WM_MENU_REFRESH_SCRIPT		(WM_USER + 82)
#define USER_MEG_WM_MENU_REFRESH_SCRIPT(func)	   \
	if (uMsg == WM_MENU_REFRESH_SCRIPT) {		   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (BOOL) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE; 					   \
	}

#define WM_MENU_RESTRICT_MESSAGE	(WM_USER + 83)
#define USER_MSG_WM_MENU_RESTRICT_MESSAGE(func)    \
	if (uMsg == WM_MENU_RESTRICT_MESSAGE) { 	   \
		SetMsgHandled(TRUE);					   \
		lResult = (LRESULT) func( (BOOL) wParam ); \
		if ( IsMsgHandled() )					   \
			return TRUE; 					   \
	}

#define WM_RELEASE_CONTEXTMENU		(WM_USER + 84)
#define USER_MSG_WM_RELEASE_CONTEXTMENU(func)					  \
	if (uMsg == WM_RELEASE_CONTEXTMENU) {						  \
		SetMsgHandled(TRUE);									  \
		lResult = (LRESULT) func(uMsg, wParam, lParam, bHandled); \
		if ( IsMsgHandled() )									  \
			return TRUE; 									  \
	}

#define WM_MENU_RECENTDOCUMENT		(WM_USER + 85)
#define USER_MSG_WM_MENU_RECENTDOCUMENT(func)		\
	if (uMsg == WM_MENU_RECENTDOCUMENT) {			\
		SetMsgHandled(TRUE);						\
		lResult = (LRESULT) func( (HMENU) wParam ); \
		if ( IsMsgHandled() )						\
			return TRUE; 						\
	}

#define WM_GET_EXTENDED_TABSTYLE		(WM_USER + 86)
#define USER_MSG_WM_GET_EXTENDED_TABSTYLE(func) \
	if (uMsg == WM_GET_EXTENDED_TABSTYLE) { 	\
		SetMsgHandled(TRUE);					\
		lResult = (LRESULT) func(); 			\
		if ( IsMsgHandled() )					\
			return TRUE; 					\
	}

#define WM_SET_EXTENDED_TABSTYLE		(WM_USER + 87)
#define USER_MSG_WM_SET_EXTENDED_TABSTYLE(func) \
	if (uMsg == WM_SET_EXTENDED_TABSTYLE) { 	\
		SetMsgHandled(TRUE);					\
		func( (DWORD) wParam ); 				\
		if ( IsMsgHandled() )					\
			return TRUE;						\
	}

#define WM_GET_CHILDFRAME		(WM_USER + 88)
#define USER_MSG_WM_SET_CHILDFRAME(func) \
	if (uMsg == WM_GET_CHILDFRAME) {	 \
		SetMsgHandled(TRUE);			 \
		lResult = (LRESULT) func(); 	 \
		if ( IsMsgHandled() )			 \
			return TRUE; 			 \
	}

#define WM_CHANGE_SKIN		(WM_USER + 89)
#define USER_MSG_WM_CHANGE_SKIN(func) \
	if (uMsg == WM_CHANGE_SKIN) {	  \
		SetMsgHandled(TRUE);		  \
		lResult = (LRESULT) func();   \
		if ( IsMsgHandled() )		  \
			return TRUE; 		  \
	}

#define WM_OPEN_EXPFAVMENU		(WM_USER + 90)
#define USER_MSG_WM_OPEN_EXPFAVMENU(func)						\
	if (uMsg == WM_OPEN_EXPFAVMENU) {							\
		SetMsgHandled(TRUE);									\
		lResult = func( (int) wParam, (int) lParam );			\
		if ( IsMsgHandled() )									\
			return TRUE; 									\
	}

#define WM_COMMAND_DIRECT		(WM_USER + 91)
#define USER_MSG_WM_COMMAND_DIRECT(func)							\
	if (uMsg == WM_COMMAND_DIRECT) {								\
		SetMsgHandled(TRUE);										\
		lResult = (LRESULT) func( (int) wParam, (LPCTSTR) lParam ); \
		if ( IsMsgHandled() )										\
			return TRUE; 										\
	}

#define WM_SEARCH_WEB_SELTEXT		(WM_USER + 92)
#define USER_MSG_WM_SEARCH_WEB_SELTEXT(func)							\
	if (uMsg == WM_SEARCH_WEB_SELTEXT) {								\
		SetMsgHandled(TRUE);											\
		lResult = (LRESULT) func( (LPCTSTR) wParam, (LPCTSTR) lParam ); \
		if ( IsMsgHandled() )											\
			return TRUE; 											\
	}

#define WM_REFRESH_EXPBAR		(WM_USER + 93)
#define USER_MSG_WM_REFRESH_EXPBAR(func)		  \
	if (uMsg == WM_REFRESH_EXPBAR) {			  \
		SetMsgHandled(TRUE);					  \
		lResult = (LRESULT) func( (int) wParam ); \
		if ( IsMsgHandled() )					  \
			return TRUE;						  \
	}

#define WM_SET_EXPROPERTY		(WM_USER + 94)
#define USER_MSG_WM_SET_EXPROPERTY(func)			  \
	if (uMsg == WM_SET_EXPROPERTY) {				  \
		SetMsgHandled(TRUE);						  \
		lResult = (LRESULT) func( (LPCTSTR) wParam ); \
		if ( IsMsgHandled() )						  \
			return TRUE;							  \
	}

#define WM_GET_OWNERDRAWMODE		(WM_USER + 95)
#define USER_MSG_WM_GET_OWNERDRAWMODE(func) \
	if (uMsg ==  WM_GET_OWNERDRAWMODE) {	\
		SetMsgHandled(TRUE);				\
		lResult = (LRESULT) func(); 		\
		if ( IsMsgHandled() )				\
			return TRUE;					\
	}




/// 拡張プロパティの引数用
struct _EXPROP_ARGS {
	CString 	strUrl;
	DWORD		dwOpenFlag;
	CString 	strIniFile;
	CString 	strSection;
	CString 	strSearchWord;
};

#define WM_OPEN_WITHEXPROP		(WM_USER + 96)
#define USER_MSG_WM_OPEN_WITHEXPROP(func)					 \
	if (uMsg == WM_OPEN_WITHEXPROP) {						 \
		SetMsgHandled(TRUE);								 \
		lResult = (LRESULT) func( (_EXPROP_ARGS *) wParam ); \
		if ( IsMsgHandled() )								 \
			return TRUE;									 \
	}

#define WM_GET_SEARCHBAR		(WM_USER + 97)
#define USER_MSG_WM_GET_SEARCHBAR(func) \
	if (uMsg == WM_GET_SEARCHBAR) { 	\
		SetMsgHandled(TRUE);			\
		lResult = (LRESULT) func(); 	\
		if ( IsMsgHandled() )			\
			return TRUE;				\
	}

#define WM_SHOW_TOOLBARMENU 	(WM_USER + 98)
#define USER_MSG_WM_SHOW_TOOLBARMENU(func) \
	if (uMsg == WM_SHOW_TOOLBARMENU) {	   \
		SetMsgHandled(TRUE);			   \
		lResult = (LRESULT) func(); 	   \
		if ( IsMsgHandled() )			   \
			return TRUE;				   \
	}

#define WM_GET_FAVORITEFILEPATH	(WM_USER + 99)
#define USER_MSG_WM_GET_FAVORITEFILEPATH(func) \
	if (uMsg == WM_GET_FAVORITEFILEPATH) {	   \
		SetMsgHandled(TRUE);			   \
		lResult = (LRESULT) func((int)wParam); 	   \
		if ( IsMsgHandled() )			   \
			return TRUE;				   \
	}

#define WM_MENU_GET_BINGTRANSLATE 	(WM_USER + 100)
#define USER_MEG_WM_MENU_GET_BINGTRANSLATE(func) \
	if (uMsg == WM_MENU_GET_BINGTRANSLATE) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = (LRESULT) func();    \
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

enum TabCreateOption {
	TAB_ACTIVE	= 0x01,
	TAB_LINK	= 0x02,
};
#define WM_TABCREATE	(WM_USER + 101)
#define USER_MSG_WM_TABCREATE(func)	\
	if (uMsg == WM_TABCREATE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam, (DWORD)lParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_TABDESTROY	(WM_USER + 102)
#define USER_MSG_WM_TABDESTROY(func)	\
	if (uMsg == WM_TABDESTROY) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_UIUPDATE		(WM_USER + 103)
#define USER_MSG_WM_UIUPDATE()	\
	if (uMsg == WM_UIUPDATE) {	   \
		SetMsgHandled(TRUE);		   \
		OnIdle();    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_CHILDFRAMEACTIVATE	(WM_USER + 104)
#define USER_MSG_WM_CHILDFRAMEACTIVATE(func)	\
	if (uMsg == WM_CHILDFRAMEACTIVATE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam, (HWND)lParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnBrowserTitleChange(HWND hWndChildFrame, LPCTSTR strTitle);
#define WM_BROWSERTITLECHANGE	(WM_USER + 105)
#define USER_MSG_WM_BROWSERTITLECHANGE(func)	\
	if (uMsg == WM_BROWSERTITLECHANGE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam, (LPCTSTR)lParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnAddCommandUIMap(HWND hWndChildFrame);
#define WM_ADDCOMMANDUIMAP	(WM_USER + 106)
#define USER_MSG_WM_ADDCOMMANDUIMAP(func)	\
	if (uMsg == WM_ADDCOMMANDUIMAP) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// void	OnAddRecentClosedTab(HWND hWndChildFrame)
#define WM_ADDRECENTCLOSEDTAB	(WM_USER + 107)
#define USER_MSG_WM_ADDRECENTCLOSEDTAB(func)	\
	if (uMsg == WM_ADDRECENTCLOSEDTAB) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnBrowserLocationChange(LPCTSTR strURL, HICON hFavicon)
#define WM_BROWSERLOCATIONCHANGE	(WM_USER + 108)
#define USER_MSG_WM_BROWSERLOCATIONCHANGE(func)	\
	if (uMsg == WM_BROWSERLOCATIONCHANGE) {	   \
		SetMsgHandled(TRUE);		   \
		func((LPCTSTR)wParam, (HICON)lParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnSetSearchText(LPCTSTR strText, bool bHilightOn)
#define WM_SETSEARCHTEXT	(WM_USER + 109)
#define USER_MSG_WM_SETSEARCHTEXT(func)	\
	if (uMsg == WM_SETSEARCHTEXT) {	   \
		SetMsgHandled(TRUE);		   \
		func((LPCTSTR)wParam, lParam != 0);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_COMMAND_FROM_CHILDFRAME	(WM_USER + 110)

// void	OnGetChildFrameData(bool bCreateData)
#define WM_GETCHILDFRAMEDATA	(WM_USER + 111)
#define USER_MSG_WM_GETCHILDFRAMEDATA(func)	\
	if (uMsg == WM_GETCHILDFRAMEDATA) {	   \
		SetMsgHandled(TRUE);		   \
		func(wParam != 0);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnOpenFindBarWithText(LPCTSTR strText)
#define WM_OPENFINDBARWITHTEXT	(WM_USER + 111)
#define USER_MSG_WM_OPENFINDBARWITHTEXT(func)	\
	if (uMsg == WM_OPENFINDBARWITHTEXT) {	   \
		SetMsgHandled(TRUE);		   \
		func((LPCTSTR)wParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// int	OnHilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags)
#define WM_HILIGHTFROMFINDBAR	(WM_USER + 112)
#define USER_MSG_WM_HILIGHTFROMFINDBAR(func)	\
	if (uMsg == WM_HILIGHTFROMFINDBAR) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = func((LPCTSTR)wParam, (LOWORD(lParam) & 0x1) != 0, (LOWORD(lParam) & 0x2) != 0, (long)HIWORD(lParam));;		\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnRemoveHilight()
#define WM_REMOVEHILIGHT	(WM_USER + 113)
#define USER_MSG_WM_REMOVEHILIGHT(func)	\
	if (uMsg == WM_REMOVEHILIGHT) {	   \
		SetMsgHandled(TRUE);		   \
		func();    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// int	OnGetTabIndex(HWND hWndChildFrame)
#define WM_GETTABINDEX	(WM_USER + 114)
#define USER_MSG_WM_GETTABINDEX(func)	\
	if (uMsg == WM_GETTABINDEX) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = func((HWND)wParam);				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnChangeChildFrameUIMap(HWND hWndChildFrame)
#define WM_CHANGECHILDFRAMEUIMAP	(WM_USER + 115)
#define USER_MSG_WM_CHANGECHILDFRAMEUIMAP(func)	\
	if (uMsg == WM_CHANGECHILDFRAMEUIMAP) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);				\
		lResult = 0;					\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnChildFrameConnecting(HWND hWndChildFrame)
#define WM_CHILDFRAMECONNECTING		(WM_USER + 116)
#define USER_MSG_WM_CHILDFRAMECONNECTING(func)	\
	if (uMsg == WM_CHILDFRAMECONNECTING) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);				\
		lResult = 0;					\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnChildFrameDownloading(HWND hWndChildFrame)
#define WM_CHILDFRAMEDOWNLOADING	(WM_USER + 117)
#define USER_MSG_WM_CHILDFRAMEDOWNLOADING(func)	\
	if (uMsg == WM_CHILDFRAMEDOWNLOADING) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);				\
		lResult = 0;					\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnChildFrameComplete(HWND hWndChildFrame)
#define WM_CHILDFRAMECOMPLETE		(WM_USER + 118)
#define USER_MSG_WM_CHILDFRAMECOMPLETE(func)	\
	if (uMsg == WM_CHILDFRAMECOMPLETE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);				\
		lResult = 0;					\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

// void	OnInitProcessFinished()
#define WM_INITPROCESSFINISHED	(WM_USER + 119)
#define USER_MSG_WM_INITPROCESSFINISHED(func)	\
	if (uMsg == WM_INITPROCESSFINISHED) {	   \
		SetMsgHandled(TRUE);		   \
		func();				\
		lResult = 0;					\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// void	OnRemoveCommandUIMap(HWND hWndChildFrame);
#define WM_REMOVECOMMANDUIMAP	(WM_USER + 120)
#define USER_MSG_WM_REMOVECOMMANDUIMAP(func)	\
	if (uMsg == WM_REMOVECOMMANDUIMAP) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam);    \
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// ChildFrameが実装する。返された文字列はちゃんとdeleteすること!
// void	OnGetSelectedText(LPCTSTR* ppStr);
#define WM_GETSELECTEDTEXT	(WM_USER + 121)
#define USER_MSG_WM_GETSELECTEDTEXT(func)	\
	if (uMsg == WM_GETSELECTEDTEXT) {	   \
		SetMsgHandled(TRUE);		   \
		func((LPCTSTR*)wParam);						\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// void	OnExecuteUserJavascript(LPCTSTR strScriptText)
#define WM_EXECUTEUSERJAVASCRIPT	(WM_USER + 122)
#define USER_MSG_WM_EXECUTEUSERJAVASCRIPT(func)	\
	if (uMsg == WM_EXECUTEUSERJAVASCRIPT) {	   \
		SetMsgHandled(TRUE);		   \
		func((CString*)wParam);						\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// void	OnSetPageBitmap(HBITMAP* pBmp)
#define WM_SETPAGEBITMAP	(WM_USER + 123)
#define USER_MSG_WM_SETPAGEBITMAP(func)	\
	if (uMsg == WM_SETPAGEBITMAP) {	   \
		SetMsgHandled(TRUE);		   \
		func((HBITMAP*)wParam);						\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


// void	OnDrawChildFramePage(CDCHandle dc)
#define WM_DRAWCHILDFRAMEPAGE	(WM_USER + 124)
#define USER_MSG_WM_DRAWCHILDFRAMEPAGE(func)	\
	if (uMsg == WM_DRAWCHILDFRAMEPAGE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HDC)wParam);						\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_BROWSERTITLECHANGEFORUIUPDATER		(WM_USER + 125)
#define WM_BROWSERLOCATIONCHANGEFORUIUPDATER	(WM_USER + 126)

#define WM_SETFAVICONFROMCHILDFRAME		(WM_USER + 127)

// void	OnHilightSwitchChange(bool bOn)
#define WM_HILIGHTSWITCHCHANGE			(WM_USER + 128)
#define USER_MSG_WM_HILIGHTSWITCHCHANGE(func)	\
	if (uMsg == WM_HILIGHTSWITCHCHANGE) {	   \
		SetMsgHandled(TRUE);		   \
		func(wParam != 0);						\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_GETBROWSERFONTSIZE		(WM_USER + 129)
#define USER_MSG_WM_GETBROWSERFONTSIZE()	\
	if (uMsg == WM_GETBROWSERFONTSIZE) {	   \
		SetMsgHandled(TRUE);		   \
		CComVariant 	var;			\
		CComVariant		varIn;			\
		m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);	\
		lResult = var.lVal;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

//	int OnChildFrameFindKeyword(HANDLE handle)
#define WM_CHILDFRAMEFINDKEYWORD	(WM_USER + 130)
#define USER_MSG_WM_CHILDFRAMEFINDKEYWORD(func)	\
	if (uMsg == WM_CHILDFRAMEFINDKEYWORD) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = func((HANDLE)wParam);			\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

//	void OnCleanUpNewProcessSharedMemHandle(HANDLE hDel)
#define WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE	(WM_USER + 131)
#define USER_MSG_WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE(func)	\
	if (uMsg == WM_CLEANUPNEWPROCESSSHAREDMEMHANDLE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HANDLE)wParam);			\
		lResult = 0;								\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

//	void OnUpdateUrlSecurityList()
#define WM_UPDATEURLSECURITYLIST	(WM_USER + 132)
#define USER_MSG_WM_UPDATEURLSECURITYLIST(func)	\
	if (uMsg == WM_UPDATEURLSECURITYLIST) {	   \
		SetMsgHandled(TRUE);		   \
		func();			\
		lResult = 0;								\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


//	void OnMouseGesture(HWND hWndChildFrame, HANDLE hMapForClose)
#define WM_MOUSEGESTURE	(WM_USER + 133)
#define USER_MSG_WM_MOUSEGESTURE(func)	\
	if (uMsg == WM_MOUSEGESTURE) {	   \
		SetMsgHandled(TRUE);		   \
		func((HWND)wParam, (HANDLE)lParam);			\
		lResult = 0;								\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

#define WM_CLOSEHANDLEFORSHAREDMEM	(WM_USER + 134)
#define USER_MSG_WM_CLOSEHANDLEFORSHAREDMEM()	\
	if (uMsg == WM_CLOSEHANDLEFORSHAREDMEM) {	   \
		SetMsgHandled(TRUE);		   \
		::CloseHandle((HWND)wParam);			\
		lResult = 0;								\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


//	HWND OnGetDownloadingViewHWND()
#define WM_GETDOWNLOADINGVIEWHWND	(WM_USER + 135)
#define USER_MSG_WM_GETDOWNLOADINGVIEWHWND(func)	\
	if (uMsg == WM_GETDOWNLOADINGVIEWHWND) {	   \
		SetMsgHandled(TRUE);		   \
		lResult = (LRESULT)func();				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

//	void OnSetDLConfigToGlobalConfig()
#define WM_SETDLCONFIGTOGLOBALCONFIG	(WM_USER + 136)
#define USER_MSG_WM_SETDLCONFIGTOGLOBALCONFIG(func)	\
	if (uMsg == WM_SETDLCONFIGTOGLOBALCONFIG) {	   \
		SetMsgHandled(TRUE);		   \
		func();				\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}

//	void OnSetProxyToChildFrame()
#define WM_SETPROXYTOCHLDFRAME	(WM_USER + 137)
#define USER_MSG_WM_SETPROXYTOCHLDFRAME(func)	\
	if (uMsg == WM_SETPROXYTOCHLDFRAME) {	   \
		SetMsgHandled(TRUE);		   \
		func();				\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}



#define WM_DECREMENTTHREADREFCOUNT	(WM_APP + 200)
#define WM_INCREMENTTHREADREFCOUNT	(WM_APP + 201)
#define USER_MSG_WM_INCREMENTTHREADREFCOUNT()	\
	if (uMsg == WM_INCREMENTTHREADREFCOUNT) {	   \
		SetMsgHandled(TRUE);		   \
		++(*m_pThreadRefCount);			\
		lResult = 0;				\
		if ( IsMsgHandled() )		   \
			return TRUE; 		   \
	}


#endif // __DONUTDEFINE_H__
