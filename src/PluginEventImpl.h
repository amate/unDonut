/**
 *	@file	PluginEventImpl.h
 *	@brief	プラグイン・イベント
 */

#pragma once


enum EPcom {
	//IAPI
	PCOM_GETTABINDEX		=	 1,
	PCOM_SETTABINDEX		=	 2,
	PCOM_GETDOCUMENT		=	 3,
	PCOM_GETBROWSER 		=	 4,
	PCOM_GETWINDOW			=	 5,
	PCOM_GETTABCOUNT		=	 6,
	PCOM_GETPANELDOCUMENT	=	 7,
	PCOM_GETPANELBROWSER	=	 8,
	PCOM_GETPANELWINDOW 	=	 9,
	PCOM_NEWWINDOW			=	10,
	PCOM_CLOSEWINDOW		=	11,
	PCOM_MOVE				=	12,
	//PCOM_MESSAGEBOX		=	13,
	PCOM_GETTABSTATE		=	14,
	PCOM_SHOWPANEL			=	15,

	//IAPI2
	PCOM_EXECUTECOMMAND 	=	16,
	PCOM_GETSEARCHTEXT		=	17,
	PCOM_SETSEARCHTEXT		=	18,
	PCOM_GETADDRESSTEXT 	=	19,
	PCOM_SETADDRESSTEXT 	=	20,
	PCOM_GETEXTENDEDTABSTATE=	21,
	PCOM_SETEXTENDEDTABSTATE=	22,
	PCOM_GETKEYSTATE		=	23,
	//PCOM_GETPROFILEINT	=	24,
	//PCOM_WRITEPROFILEINT	=	25,
	//PCOM_GETPROFILESTRING =	26,
	//PCOM_WRITEPROFILESTRING=	27,
	PCOM_GETSCRIPTFOLDER	=	28,
	PCOM_GETCSSFOLDER		=	29,
	PCOM_GETBASEFOLDER		=	30,
	PCOM_GETEXEPATH 		=	31,
	PCOM_SETSTYLESHEET		=	32,

	//IAPI3
	PCOM_SAVEGROUP			=	33,
	PCOM_LOADGROUP			=	34,
	PCOM_NEWWINDOW3 		=	35,
	PCOM_ADDGROUPITEM		=	36,
	PCOM_DELETEGROUPITEM	=	37,
};



class CPluginEvent {
public:
	friend LRESULT CMainFrame::OnPluginCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
	typedef struct {
		LPCTSTR lpszUrl;
		BOOL	bActive;
	} PCOM_NEWWINDOWSTRUCT;


	typedef struct {
		int nBefore;
		int nAfter;
	} PCOM_MOVEWINDOWSTRUCT;


	typedef struct {
		int    nBufSize;
		LPTSTR lpszText;
	} PCOM_STRINGSTRUCT;


	typedef struct {
		int   nIndex;
		DWORD dwState;
	} PCOM_EXSTATESTRUCT;


	typedef struct {
		int 	nIndex;
		LPCTSTR lpszCSS;
		BOOL	bOff;
	} PCOM_STYLESHEETSTRUCT;


	typedef struct {
		LPCTSTR lpszFileName;
		BOOL	bClose;
	} PCOM_LOADGROUPSTRUCT;


	typedef struct {
		LPCTSTR lpszUrl;
		BOOL	bActive;
		DWORD	dwExStyle;
	} PCOM_NEWWINDOW3STRUCT;


	typedef struct {
		LPCTSTR lpszFileName;
		int 	nIndex;
	} PCOM_GROUPITEMSTRUCT;


	static BOOL SetTextToBuffer(CComBSTR &bstr, PCOM_STRINGSTRUCT *lpss)
	{
		if (!lpss)
			return FALSE;

		CString str(bstr);
		LPTSTR	lpstr = ::lstrcpyn(lpss->lpszText, str, lpss->nBufSize);
		return lpstr != 0;	//+++ ? TRUE : FALSE;
	}


	static BOOL SetTextToBuffer(CComBSTR &bstr, LPARAM lParam)
	{
		PCOM_STRINGSTRUCT *lpss = (PCOM_STRINGSTRUCT *) lParam;

		return SetTextToBuffer(bstr, lpss);
	}

};



LRESULT CMainFrame::OnPluginCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	UINT	uID  = (UINT) wParam;
	LRESULT lRet = 0;

	//switch文を書くたびにbreakを忘れそうで怖いんですが
	switch (uID) {
		//IAPI
	case PCOM_GETTABINDEX:
		lRet = ApiGetTabIndex();
		break;

	case PCOM_SETTABINDEX:
		ApiSetTabIndex( (int) lParam );
		break;

	case PCOM_GETDOCUMENT:
		lRet = (LRESULT) ApiGetDocumentObject( (int) lParam );
		break;

	case PCOM_GETBROWSER:
		lRet = (LRESULT) ApiGetWebBrowserObject( (int) lParam );
		break;

	case PCOM_GETWINDOW:
		lRet = (LRESULT) ApiGetWindowObject( (int) lParam );
		break;

	case PCOM_GETTABCOUNT:
		lRet = ApiGetTabCount();
		break;

	case PCOM_GETPANELDOCUMENT:
		lRet = (LRESULT) ApiGetPanelDocumentObject();
		break;

	case PCOM_GETPANELBROWSER:
		lRet = (LRESULT) ApiGetPanelWebBrowserObject();
		break;

	case PCOM_GETPANELWINDOW:
		lRet = (LRESULT) ApiGetPanelWindowObject();
		break;

	case PCOM_NEWWINDOW:
		{
			CPluginEvent::PCOM_NEWWINDOWSTRUCT *lpnws = (CPluginEvent::PCOM_NEWWINDOWSTRUCT *) lParam;

			if (!lpnws)
				break;

			lRet = ApiNewWindow(CComBSTR(lpnws->lpszUrl), lpnws->bActive);
		}
		break;

	case PCOM_CLOSEWINDOW:
		ApiClose( (int) lParam );
		break;

	case PCOM_MOVE:
		{
			CPluginEvent::PCOM_MOVEWINDOWSTRUCT *lpmws = (CPluginEvent::PCOM_MOVEWINDOWSTRUCT *) lParam;

			if (!lpmws)
				break;

			ApiMoveToTab(lpmws->nBefore, lpmws->nAfter);
		}
		break;

	case PCOM_GETTABSTATE:
		lRet = ApiGetTabState( (int) lParam );
		break;

	case PCOM_SHOWPANEL:
		ApiShowPanelBar();
		break;

		//IAPI2
	case PCOM_EXECUTECOMMAND:
		ApiExecuteCommand( (int) lParam );
		break;

	case PCOM_GETADDRESSTEXT: //BSTRを介するので面倒
		{
			CComBSTR bstr;
			ApiGetAddressText(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_SETADDRESSTEXT:
		{
			CComBSTR bstr( (LPCTSTR) lParam );
			ApiSetAddressText(bstr);
		}
		break;

	case PCOM_GETSEARCHTEXT:
		{
			CComBSTR bstr;
			ApiGetSearchText(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_SETSEARCHTEXT:
		{
			CComBSTR bstr( (LPCTSTR) lParam );
			ApiSetSearchText(bstr);
		}
		break;

	case PCOM_GETEXTENDEDTABSTATE:
		lRet = ApiGetExtendedTabState( (int) lParam );
		break;

	case PCOM_SETEXTENDEDTABSTATE:
		{
			CPluginEvent::PCOM_EXSTATESTRUCT *lpes = (CPluginEvent::PCOM_EXSTATESTRUCT *) lParam;

			if (!lpes)
				break;

			ApiSetExtendedTabState(lpes->nIndex, lpes->dwState);
			break;
		}
		break;

	case PCOM_GETKEYSTATE:
		lRet = ApiGetKeyState( (int) lParam );
		break;		//+++ どう考えても GetScriptFolderに流れるのはマズイのでbreak追加.

	case PCOM_GETSCRIPTFOLDER:
		{
			CComBSTR bstr;
			ApiGetScriptFolder(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_GETCSSFOLDER:
		{
			CComBSTR bstr;
			ApiGetCSSFolder(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_GETBASEFOLDER:
		{
			CComBSTR bstr;
			ApiGetBaseFolder(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_GETEXEPATH:
		{
			CComBSTR bstr;
			ApiGetExePath(&bstr);
			BOOL	 bRet = CPluginEvent::SetTextToBuffer(bstr, lParam);
			lRet = (LRESULT) bRet;
		}
		break;

	case PCOM_SETSTYLESHEET:
		{
			CPluginEvent::PCOM_STYLESHEETSTRUCT *lpsss = (CPluginEvent::PCOM_STYLESHEETSTRUCT *) lParam;

			if (!lpsss)
				break;

			CComBSTR	bstr(lpsss->lpszCSS);
			ApiSetStyleSheet(lpsss->nIndex, bstr, lpsss->bOff);
		}
		break;

		//IAPI3
	case PCOM_SAVEGROUP:
		{
			CComBSTR bstr( (LPCTSTR) lParam );
			ApiSaveGroup(bstr);
		}
		break;

	case PCOM_LOADGROUP:
		{
			CPluginEvent::PCOM_LOADGROUPSTRUCT *lpls = (CPluginEvent::PCOM_LOADGROUPSTRUCT *) lParam;

			if (!lpls)
				break;

			CComBSTR	bstr(lpls->lpszFileName);
			ApiLoadGroup(bstr, lpls->bClose);
		}
		break;

	case PCOM_NEWWINDOW3:
		{
			CPluginEvent::PCOM_NEWWINDOW3STRUCT *lpns = (CPluginEvent::PCOM_NEWWINDOW3STRUCT *) lParam;

			if (!lpns) {
				lRet = -1;
				break;
			}

			CComBSTR	bstr(lpns->lpszUrl);
			lRet = ApiNewWindow3(bstr, lpns->bActive, lpns->dwExStyle, NULL);
		}
		break;

	case PCOM_ADDGROUPITEM:
		{
			CPluginEvent::PCOM_GROUPITEMSTRUCT *lpgs = (CPluginEvent::PCOM_GROUPITEMSTRUCT *) lParam;

			if (!lpgs) {
				lRet = -1;
				break;
			}

			CComBSTR	bstr(lpgs->lpszFileName);
			lRet = ApiAddGroupItem(bstr, lpgs->nIndex);
		}
		break;

	case PCOM_DELETEGROUPITEM:
		{
			CPluginEvent::PCOM_GROUPITEMSTRUCT *lpgs = (CPluginEvent::PCOM_GROUPITEMSTRUCT *) lParam;

			if (!lpgs) {
				lRet = 0;
				break;
			}

			CComBSTR	bstr(lpgs->lpszFileName);
			lRet = ApiDeleteGroupItem(bstr, lpgs->nIndex);
		}
		break;
	};


	return lRet;
}


