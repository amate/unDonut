/**
 *	@file	MtlWeb.h
 *	@brief	MTL : WEB関係
 */
// MTL Version 1.01
// Copyright (C) 2000 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Last updated: July 05, 2000

#ifndef __MTLWEB_H__
#define __MTLWEB_H__

#pragma once

#ifndef _INC_SHLWAPI
	#error mtlweb.h requires shlwapi.h to be included first
#endif


#include "resource.h"
#include "DonutDefine.h"
#include "MtlBase.h"
#include "MtlMisc.h"
#include "MtlWeb.h"
#include "MtlFile.h"



namespace MTL {


bool	MtlGetFavoritesFolder(CString &rString);
bool	MtlGetHistoryFolder(CString &rString);
bool	MtlGetFavoriteLinksFolder(CString &rString);
bool	MtlCreateInternetShortcutFile(const CString &strFileName, const CString &strUrl);

// Note. If the third parameter is "CString strPath", it rarely crash. why?
void	MtlOrganizeFavorite( HWND hWnd, bool bOldShell = false, const CString &strPath_ = _T("") );
bool	__MtlAddFavoriteOldShell(const CString &strUrl, const CString &strName_, const CString &strFavDir);
bool	_MtlAddFavoriteNewShell(const CString &strUrl, const CString &strName);
bool	_MtlAddFavoriteModalSucks(const CString &strUrl, const CString &strName_, const CString &strFavPath, HWND hWnd);
bool	MtlAddFavorite(const CString& strUrl, const CString& strName, bool bOldShell = false, const CString&	strFavDir_ = _T(""), HWND hWnd = NULL);


#pragma comment( lib, "wininet" )


// cf.MSDN "Offline Browsing"
// Bug: Even if an user is working-Offline before your application runs,
//		this function always returns FALSE.
bool	MtlIsGlobalOffline();


// cf.MSDN "Offline Browsing"
void	MtlSetGlobalOffline(bool bGoOffline);


#if (_WIN32_IE >= 0x0500)
bool	MtlAutoComplete(HWND hWnd);
#endif



void	MtlShowInternetOptions();
bool	MtlCreateShortCutFile(
			const CString&	strFile,
			const CString&	strTarget,
			const CString&	strDescription	= _T(""),
			const CString&	strArgs 		= _T(""),
			const CString&	strWorkingDir	= _T(""),
			const CString&	strIconPath 	= _T(""),
			int 			iIcon			= 0,
			int 			iShowCmd		= SW_SHOWNORMAL);

///+++ MtlCtrl.hへ移動.
// bool 	MtlParseInternetShortcutFile(CString &strFilePath);
CString 	MtlGetInternetShortcutUrl(const CString &strFile);
void		_MtlExecuteWithoutDDE( CString &strPath, const CString &strArg = CString() );
bool		MtlPreOpenFile( CString &strPath, const CString &strArg = CString() );


// Undocumented command ids
static const CLSID		CGID_IWebBrowser = { 0xED016940L, 0xBD5B, 0x11cf, 0xBA, 0x4E, 0x00, 0xC0, 0x4F, 0xD7, 0x08, 0x16 };


///////////////////////////////////////////////////////////////////
// CWebBrowserCommandHandler

template <class T>
class CWebBrowserCommandHandler 
{
	enum EHtmlId {
		HTMLID_FIND 		= 1,
		HTMLID_VIEWSOURCE	= 2,
		HTMLID_OPTIONS		= 3,
	};

public:
	// Message map and handlers
	BEGIN_MSG_MAP_EX(CWebBrowserCommandHandler)
		COMMAND_ID_HANDLER_EX( ID_FILE_SAVE_AS		, OnFileSaveAs		)
		COMMAND_ID_HANDLER_EX( ID_FILE_PAGE_SETUP	, OnFilePageSetup	)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT		, OnFilePrint		)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)			// suggested by DOGSTORE, thanks
		COMMAND_ID_HANDLER_EX( ID_FILE_PROPERTIES	, OnFileProperties	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_CUT			, OnEditCut 		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 		, OnEditCopy		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE		, OnEditPaste		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND 		, OnEditFind		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL	, OnEditSelectAll	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_BACK 		, OnViewBack		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FORWARD		, OnViewForward 	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH		, OnViewRefresh 	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP 		, OnViewStop		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_HOME 		, OnViewHome		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_LARGEST , OnViewFontLargest )
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_LARGER	, OnViewFontLarger	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_MEDIUM	, OnViewFontMedium	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_SMALLER , OnViewFontSmaller )
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_SMALLEST, OnViewFontSmallest)
		COMMAND_ID_HANDLER_EX( ID_VIEW_SOURCE		, OnViewSource		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION		, OnViewOption		)

		COMMAND_ID_HANDLER_EX( ID_VIEW_SOURCE_SELECTED , OnViewSourceSelected )
		//COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX	, OnEditFindMax 	)			// moved by minit ->CChildFrame
		COMMAND_ID_HANDLER_EX( ID_VIEW_UP			, OnViewUp			)
	END_MSG_MAP()


public:
	void OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_SAVEAS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


	void OnFilePageSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_PAGESETUP, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


	void OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


private:
	void OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


public:
	void OnFileProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_PROPERTIES, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


private:
	void OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
	//	T *pT = static_cast<T *>(this);
	//	pT->m_spBrowser->ExecWB(OLECMDID_CUT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		T *pT = static_cast<T *>(this);
		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_MSHTML, IDM_CUT, 0, NULL, NULL);
	}


	void OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_MSHTML, IDM_COPY, 0, NULL, NULL);
	}


	void OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
	//	T *pT = static_cast<T *>(this);
	//	pT->m_spBrowser->ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		T *pT = static_cast<T *>(this);
		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_MSHTML, IDM_PASTE, 0, NULL, NULL);
	}


public:
	void OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT = static_cast<T *>(this);
		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_FIND, 0, NULL, NULL);
		// this is just file search
		//	m_spBrowser->ExecWB(OLECMDID_FIND, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


private:
	void OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}


	void OnViewBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->GoBack();
	}


	void OnViewForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->GoForward();
	}


	void OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->Refresh();
	}


	void OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->Stop();
	}


	void OnViewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		pT->m_spBrowser->GoHome();
	}


	void OnViewUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT = static_cast<T *>(this);
		CString strURL;
		strURL = pT->GetLocationURL();

		if ( strURL.ReverseFind(_T('/')) == (strURL.GetLength() - 1) )
			strURL = strURL.Left( strURL.ReverseFind(_T('/')) );

		if (strURL.ReverseFind(_T('/')) != -1) {
			strURL = strURL.Left(strURL.ReverseFind(_T('/')) + 1);
		} else
			return;

		if (strURL[strURL.GetLength() - 2] == _T('/'))
			return;

		DonutOpenFile(pT->m_hWnd, strURL, D_OPENFILE_NOCREATE);
	}


public:
	void OnViewOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT		 = static_cast<T *>(this);

		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_OPTIONS, 0, NULL, NULL);
		// this is modeless
		// ::ShellExecute(0, "open", "control.exe", "inetcpl.cpl", ".", SW_SHOW);
	}


private:
	// zoom font
	void OnViewFontLargest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		CComVariant 	vaZoomFactor(4L);
		T * 			pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL);
	}


	void OnViewFontLarger(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		CComVariant 	vaZoomFactor(3L);
		T * 			pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL);
	}


	void OnViewFontMedium(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		CComVariant 	vaZoomFactor(2L);
		T * 			pT = static_cast<T *>(this);

		pT->m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL);
	}


	void OnViewFontSmaller(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		CComVariant vaZoomFactor(1L);
		T * 		pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL);
	}


	void OnViewFontSmallest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		CComVariant vaZoomFactor(0L);
		T * 		pT = static_cast<T *>(this);
		pT->m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL);
	}


	void OnViewSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 		pT		 = static_cast<T *>(this);
		CComBSTR	bstrUrl;
		CString		strUrl;
		pT->m_spBrowser->get_LocationURL(&bstrUrl);
		strUrl = bstrUrl;
		if ( strUrl.Left(8) == _T("file:///") ) {
			TCHAR	strpath[MAX_PATH];
			ULONG	pnChars = MAX_PATH;
			CRegKey	rk;
			if ( rk.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\View Source Editor\\Editor Name"), KEY_READ) == ERROR_SUCCESS ) {
				if ( rk.QueryStringValue(NULL, strpath, &pnChars) == ERROR_SUCCESS ) {
					strUrl.Replace(_T("%20"), _T(" "));
					::ShellExecute(NULL, NULL, strpath, _T("\"") + strUrl + _T("\""), NULL, SW_SHOWNORMAL);
					return;
				}
			}
		}
		CComQIPtr<IOleCommandTarget>	spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_VIEWSOURCE, 0, NULL, NULL);
	}


	void OnViewSourceSelected(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
	{
		CComBSTR/*BSTR*/											bstrHTML;			//+++ CComBSTRに変更
		
		CComPtr<IDispatch>											pDisp;
		CComPtr<IDispatch>											pDisp2;
		CComQIPtr<IHTMLSelectionObject, &IID_IHTMLSelectionObject>	spSelection;
		CComQIPtr<IHTMLTxtRange, &IID_IHTMLTxtRange>				spTextRange;
		T *		pT	= static_cast<T *>(this);
		HRESULT	hr	= pT->m_spBrowser->get_Document(&pDisp);
		if (FAILED(hr)) {
			ATLASSERT(FALSE);
			return;
		}
		CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2>				spHTML;
		spHTML = pDisp;
		ATLASSERT(spHTML);

		if (spHTML) {
			spHTML->get_selection(&spSelection);
			spSelection->createRange(&pDisp2);
			spTextRange = pDisp2;

			if (spTextRange) {	// Non Frame && GetSelection
				spTextRange->get_htmlText(&bstrHTML);

				if (bstrHTML) {
					viewSelectedSource(LPCOLESTR(bstrHTML));
				} else {
					parseFramesCollection(pDisp , wID);
				}
			} else {
				parseFramesCollection(pDisp , wID);
			}
		}
	}


	void viewSelectedSource(const CString &strHTMLText)
	{
		if ( strHTMLText.IsEmpty() ) return;

		T * 	pT					   = static_cast<T *>(this);

		// get tmp path & create tmp file
		CString strTempPath 		   = Misc::GetExeDirectory() + _T("ShortcutTmp\\");
		CreateDirectory(strTempPath, NULL);

		CString	tmpFileName;
		::GetTempFileName(strTempPath , _T("dnr") , 0 , tmpFileName.GetBuffer(MAX_PATH));
		tmpFileName.ReleaseBuffer();
		tmpFileName.Replace(_T(".tmp"), _T(".htm"));

		HANDLE	hFile	= ::CreateFile(tmpFileName ,
								GENERIC_WRITE,
								0,
								0,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_TEMPORARY,
								NULL);
		DWORD	dwAccBytes = 0;
	  #ifdef UNICODE
		//CIniFileI pr( g_szIniFileName, _T("etc") );
		//bool bSelectedSource = pr.GetValue(_T("SelectedSourceTextMode"));
		//pr.Close();
		if (Misc::IsEnableUnicodeToSJIS(strHTMLText)) {	// SJIS に変換できるなら、SJISにして出力
			std::vector<char>	sjis = Misc::wcs_to_sjis( LPCTSTR( strHTMLText ) );
			::WriteFile(hFile , &sjis[0], (DWORD)strlen(&sjis[0]), &dwAccBytes , NULL);
		} else {			// SJISにできないUnicode文字なら Utf8にして出力
			std::vector<char>	utf8 = Misc::wcs_to_utf8( LPCTSTR( strHTMLText ) );
			::WriteFile(hFile , &utf8[0], (DWORD)strlen(&utf8[0]), &dwAccBytes , NULL);
		}
	  #else
		::WriteFile(hFile , strHTMLText , strHTMLText.GetLength() , &dwAccBytes , NULL);
	  #endif
		::CloseHandle(hFile);

		// get editor path
		TCHAR	strpath[MAX_PATH];
		ULONG	pnChars = MAX_PATH;
		CRegKey	rk;
		if ( rk.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\View Source Editor\\Editor Name"), KEY_READ) == ERROR_SUCCESS ) {
			if ( rk.QueryStringValue(NULL, strpath, &pnChars) == ERROR_SUCCESS ) {
				ShellExecute(pT->m_hWnd , _T("open") , strpath , tmpFileName , NULL , SW_SHOWNORMAL);
				return;
			}
		}

		ShellExecute(pT->m_hWnd , _T("open") , _T("notepad.exe") , tmpFileName , NULL , SW_SHOWNORMAL);
		// i cannot use CreateProcess for checking Exit code, coz i cannnot check it ,in case using src Handling tool.
	}


	void parseFramesCollection(CComPtr<IDispatch> pDisp , WORD wID)
	{
		CComBSTR/*BSTR*/												bstrHTML;					//+++ CComBSTR に変更
		// BSTR bstrLocation;
		CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2>					spHTML;
		CComQIPtr<IHTMLFramesCollection2, &IID_IHTMLFramesCollection2>	spFrameCollection;

		spHTML		= pDisp;
		HRESULT hr	= spHTML->get_frames(&spFrameCollection);
		if (hr != S_OK) return;

		LONG	lFrameLength;
		hr = spFrameCollection->get_length(&lFrameLength);
		ATLASSERT(SUCCEEDED(hr));

		for (LONG i = 0; i < lFrameLength; i++) {
			CComQIPtr<IHTMLWindow2, &IID_IHTMLWindow2>				   spHTMLWindow;
			CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2>			   spNextHTML;
			CComQIPtr<IHTMLSelectionObject, &IID_IHTMLSelectionObject> spNextSelection;
			CComQIPtr<IHTMLTxtRange, &IID_IHTMLTxtRange>			   spNextTextRange;
			CComQIPtr<IHTMLLocation, &IID_IHTMLLocation>			   spHTMLLocation;
			CComPtr<IDispatch> pDispNext, pDispBack;

			VARIANT 	varResult;
			VARIANT 	varItem;
			varItem.vt	 = VT_I4;
			varItem.lVal = i;
			spFrameCollection->item(&varItem, &varResult);
			spHTMLWindow = varResult.pdispVal;

			if (spHTMLWindow) {
				spHTMLWindow->get_document(&spNextHTML);

				if (spNextHTML) {
					spNextHTML->get_selection(&spNextSelection);
					spNextSelection->createRange(&pDispNext);
					spNextTextRange = pDispNext;

					if (spNextTextRange) {
						if (wID == ID_EDIT_FIND_MAX) {
							spNextTextRange->get_text(&bstrHTML);
							T *pT = static_cast<T *>(this);

							if (bstrHTML)
								pT->searchEngines(LPCOLESTR(bstrHTML));
						} else if (wID == ID_VIEW_SOURCE_SELECTED) {
							spNextTextRange->get_htmlText(&bstrHTML);
							if (bstrHTML) {
								viewSelectedSource(LPCOLESTR(bstrHTML));
							}
						}
					} else {
						pDispBack = spNextHTML;
						parseFramesCollection(pDispBack , wID);
					}
				}
			}
		}
	}


public:
	void _OnEditFindMax(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
	{
		CComBSTR/*BSTR*/	bstrHTML;								//+++ CComBSTRに変更
		T * 				pT		= static_cast<T *>(this);

		CComPtr<IDispatch>										   pDisp;
		CComPtr<IDispatch>										   pDisp2;
		CComQIPtr<IHTMLSelectionObject, &IID_IHTMLSelectionObject> spSelection;
		CComQIPtr<IHTMLTxtRange, &IID_IHTMLTxtRange>			   spTextRange;
		HRESULT 												   hr = pT->m_spBrowser->get_Document(&pDisp);
		CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2>			   spHTML;
		spHTML = pDisp;

		if (spHTML) {
			spHTML->get_selection(&spSelection);
			spSelection->createRange(&pDisp2);
			spTextRange = pDisp2;

			if (spTextRange) { // Non Frame && GetSelection
				spTextRange->get_text(&bstrHTML);

				if (bstrHTML)
					pT->searchEngines(LPCOLESTR(bstrHTML));

				else
					parseFramesCollection(pDisp , wID);
			} else {
				parseFramesCollection(pDisp , wID);
			}
		}
	}


private:
	virtual void searchEngines(const CString &strKeyWord )
	{
		T * 		pT		  = static_cast<T *>(this);
		CString 	strURL;
		int 		nMaxCount = 15;
		TCHAR		szGoogleCode[INTERNET_MAX_PATH_LENGTH];

		memset(szGoogleCode, 0, INTERNET_MAX_PATH_LENGTH);
		DWORD		dwCount   = INTERNET_MAX_PATH_LENGTH;
		DWORD		dwFlags	  = 0;

		{
			CIniFileI pr0( g_szIniFileName, _T("AddressBar") );
			pr0.QueryString(szGoogleCode, _T("EnterCtrl"), &dwCount);
			pr0.QueryValue( dwFlags, _T("") );
		}

		CIniFileI	pr( g_szIniFileName, _T("INUYA") );
		TCHAR		szRetString[INTERNET_MAX_PATH_LENGTH + 1];
		TCHAR		szBuff[32];

		for (int nItem = 0; nItem < nMaxCount; nItem++) {
			wsprintf(szBuff, _T("Engine%i"), nItem);
			szRetString[0]= 0;
			DWORD dwCount = INTERNET_MAX_PATH_LENGTH;	//+++ * sizeof (TCHAR);

			if (pr.QueryString(szRetString, szBuff, &dwCount) == ERROR_SUCCESS) {
				strURL = szRetString + strKeyWord;
				DonutOpenFile( pT->m_hWnd, strURL, DonutGetStdOpenCreateFlag() );
			} else {
				if (nItem == 0) {
					strURL = szGoogleCode + strKeyWord;
					DonutOpenFile( pT->m_hWnd, strURL, DonutGetStdOpenCreateFlag() );
				}

				break;
			}
		}
	}
};



template <class T>
class CMDIFrameTitleUpsideDownMessageHandlerWeb {
	bool		m_bValid;
	CString 	m_strWorkOffline;
	CString 	m_strStatusBar; 		// UDT DGSTR

public:
	CMDIFrameTitleUpsideDownMessageHandlerWeb()
		: m_bValid(true)
		, m_strWorkOffline( _T(" - [オフライン作業]") )
		, m_strStatusBar( _T("") )						// UDT DGSTR
	{
	}


	void UpdateTitleBarUpsideDown()
	{
		if (m_bValid) {
			CString strApp;
			strApp.LoadString(IDR_MAINFRAME);
			T * 	pT = static_cast<T *>(this);
			pT->SetWindowText(strApp);
		}
	}


	// UDT DGSTR
	void UpdateTitleBarUpsideDown(const CString &strStatusBar)
	{
		m_strStatusBar = strStatusBar;

		if (m_bValid) {
			CString strApp;
			strApp.LoadString(IDR_MAINFRAME);
			T * 	pT = static_cast<T *>(this);
			pT->SetWindowText(strApp + " " + m_strStatusBar);
		}
	}
	// ENDE


public:
	//+++ メモ:GetText,SetTextを乗っ取って、タイトルバーの文字列の表示順を"ページ名 - APP名" にする処理
	//         ...のようだが、うまく機能していない模様...
	BEGIN_MSG_MAP(CMDIFrameTitleUpsideDownMessageHandlerWeb)
		MESSAGE_HANDLER( WM_GETTEXTLENGTH, OnGetTextLength	)
		MESSAGE_HANDLER( WM_GETTEXT 	 , OnGetText		)
		MESSAGE_HANDLER( WM_SETTEXT 	 , OnSetText		)	// never called by MDI system
	END_MSG_MAP()


private:
	LRESULT OnGetTextLength(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T *pT = static_cast<T *>(this);

		return pT->DefWindowProc(uMsg, wParam, lParam) + m_strWorkOffline.GetLength() + 1;
	}


	LRESULT OnGetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT	cchTextMax = (UINT)   wParam;	// number of characters to copy
		LPTSTR	lpszText   = (LPTSTR) lParam;

		if (!m_bValid) {
			bHandled = FALSE;
			return 0;
		}

		T * 	pT		   = static_cast<T *>(this);

		CString strApp;
		strApp.LoadString(IDR_MAINFRAME);

		CString strTitle;
		BOOL	bMaximized = FALSE;
		HWND	hWndActive = pT->MDIGetActive(&bMaximized);

		if (hWndActive == NULL || bMaximized == FALSE)
			strTitle = strApp;
		// UDT DGSTR ( Set Information on TitleBar.
		else if ( !IsWindowVisible(pT->m_hWndStatusBar) ) {
			if ( !m_strStatusBar.IsEmpty() )
				strTitle = MtlGetWindowText(hWndActive) + _T(" - ") + m_strStatusBar;
			else
				strTitle = MtlGetWindowText(hWndActive) /*+ _T(" -") + strApp*/;
		}
		// ENDE
		else
			strTitle = MtlGetWindowText(hWndActive) + _T(" - ") + strApp;
			//strTitle = _T("[") + MtlGetWindowText(hWndActive) + _T("] - ") + strApp;	//ためし...

		if ( MtlIsGlobalOffline() )
			strTitle += m_strWorkOffline;

		::lstrcpyn(lpszText, strTitle, cchTextMax);

		bHandled = TRUE;
		return std::min(strTitle.GetLength(), (int) cchTextMax);
	}


	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CString strText 	= (LPCTSTR) lParam;

		// first, find the mdi child's title
		int 	nFirstIndex = strText.Find( _T('[') );
		int 	nLastIndex	= strText.ReverseFind( _T(']') );

		if (nFirstIndex > nLastIndex || nFirstIndex == -1 || nFirstIndex == -1) {
			bHandled = FALSE;
			return FALSE;
		}

		CString strChild	= strText.Mid(nFirstIndex, nLastIndex - nFirstIndex + 1);

		// second, find the application's title
		CString strApp;
		strApp.LoadString(IDR_MAINFRAME);
		strText = strChild + _T(" - ") + strApp;

		T * 	pT			= static_cast<T *>(this);
		return pT->DefWindowProc(uMsg, wParam, (LPARAM) (LPCTSTR) strText);
	}
};



template <class TKey>
class CSimpleMapInt : public CSimpleMap<TKey, int> {
public:
	int Lookup(TKey key) const
	{
		int nIndex = FindKey(key);

		if (nIndex == -1)
			return -1;

		return GetValueAt(nIndex);
	}
};



// I thought this is the best, but it seems that
// idls which means the same path don't surely have the same bits.
//typedef CSimpleMapInt< CAdapt<CItemIDList> > CFavoritesOrder;

typedef CSimpleMapInt< CString >  CFavoritesOrder;



// Thanks to fub and namazu
typedef struct _tagCFavoritesOrderData {
	DWORD	   size;
	DWORD	   priority;
	ITEMIDLIST idl; // relative idlist
} _CFavoritesOrderData;



// Note. I've found the IE rarely fails to load his own favorites order, but don't care.
// Note. If the item not ordered, priority equals -5.
#define FAVORITESORDER_NOTFOUND 	-5

CString MtlMakeFavoriteToolTipText(const CString &strName_, const CString &strUrl_, int cchTextMax);
BOOL	MtlGetFavoritesOrder(CFavoritesOrder &order, const CString &strDirPath);




// Based on JOBBY's code
DWORD	MtlGetInternetZoneActionPolicy(DWORD dwAction);
bool	MtlIsInternetZoneActionPolicyDisallow(DWORD dwAction);
bool	MtlSetInternetZoneActionPolicy(DWORD dwAction, DWORD dwPolicy);


// not tested yet...
bool	MtlDeleteAllCookies();


template <class _Function>
_Function _MtlForEachHTMLDocument2(IHTMLDocument2 *pDocument, _Function __f)
{
	__f(pDocument);

	CComPtr<IHTMLFramesCollection2> spFrames;
	HRESULT 	hr	   = pDocument->get_frames(&spFrames);
	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) ) {
		return __f;
	}

	LONG	nCount = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return __f;

	for (LONG i = 0; i < nCount; ++i) {
		CComVariant 	varItem(i);
		CComVariant 	varResult;
		hr = spFrames->item(&varItem, &varResult);
		if ( FAILED(hr) )
			continue;

		CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
		if (!spWindow)
			continue;

		CComPtr<IHTMLDocument2> spDocument;
		hr = spWindow->get_document(&spDocument);
		if ( FAILED(hr) ) {
			CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
			ATLASSERT(spServiceProvider);
			CComPtr<IWebBrowser2>	spBrowser;
			hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
			if (!spBrowser)
				continue;
			CComPtr<IDispatch>	spDisp;
			hr = spBrowser->get_Document(&spDisp);
			if (!spDisp)
				continue;
			spDocument = spDisp;
			if (!spDocument)
				continue;
		}

		_MtlForEachHTMLDocument2(spDocument, __f);
	}

	return __f;
}



template <class _Function>
_Function MtlForEachHTMLDocument2(IWebBrowser2 *pBrowser, _Function __f)
{
	CComPtr<IDispatch>	spDisp;
	HRESULT 			hr		 = pBrowser->get_Document(&spDisp);
	if ( FAILED(hr) )
		return __f;

	CComQIPtr<IHTMLDocument2> spDocument = spDisp;
	if (!spDocument)
		return __f;

	return _MtlForEachHTMLDocument2(spDocument, __f);
}



#if 1	//+++ gae氏のunDonut_g からの移植.

// gae: 追加
template <class _Function>
_Function _MtlForEachHTMLDocument2g(IHTMLDocument2* pDocument, _Function __f)
{
	__f(pDocument);

	CComQIPtr<IOleContainer>	pOleContainer = pDocument;
	if(pOleContainer) {
		CComPtr<IEnumUnknown>	pEnum;
		if(SUCCEEDED(pOleContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnum))) {
			CComPtr<IUnknown>	pUnk;
			while(pEnum->Next(1,&pUnk,NULL)==S_OK) {
				CComQIPtr<IWebBrowser2> pSubWeb = pUnk;
				if(pSubWeb!=NULL) {
					MtlForEachHTMLDocument2g(pSubWeb, __f);
				}
				pUnk = NULL;
			}
		}
	}

	return __f;
}


template <class _Function>
_Function MtlForEachHTMLDocument2g(IWebBrowser2* pBrowser, _Function __f)
{
	CComPtr<IDispatch> spDisp;
	HRESULT hr = pBrowser->get_Document(&spDisp);
	if (FAILED(hr))
		return __f;

	CComQIPtr<IHTMLDocument2> spDocument = spDisp;
	if (!spDocument)
		return __f;

	return _MtlForEachHTMLDocument2g(spDocument, __f);
}

#endif



bool		__MtlSkipChar(const CString &str, int &nIndex, TCHAR ch);
void		_MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText);
CString 	_MtlGetDirectoryPathFixed(const CString &strPath, bool bAddBackSlash = false);
CString 	_MtlGetRootDirectoryPathFixed(const CString &strPath, bool bAddBackSlash = false);
bool		_MtlIsIllegalRootSlash(const CString &str);
bool		_MtlIsMailTo(const CString &str);
bool		_MtlIsHrefID(const CString &str);
CString 	_MtlRemoveIDFromUrl(const CString &str);
bool		_MtlIsRelativePath(const CString &str);
void		MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText, const CString &strLocationUrl);

}	//namespace MTL



#endif	// __MTLWEB_H__
