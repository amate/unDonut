/**
 *	@file	ChildFrame.cpp
 *	@brief	タブページ１つの処理.
 */

#include "stdafx.h"
#include "ChildFrame.h"
#include "MtlBrowser.h"
#include "MtlWin.h"
#include "DonutView.h"
#include "MultiThreadManager.h"
#include "ChildFrameCommandUIUpdater.h"
#include "option\MainOption.h"
#include "option\DLControlOption.h"
#include "option\MouseDialog.h"
#include "option\IgnoreURLsOption.h"
#include "option\CloseTitleOption.h"
#include "option\UrlSecurityOption.h"
#include "option\SearchPropertyPage.h"
#include "option\AddressBarPropertyPage.h"
#include "FaviconManager.h"
#include "ToolTipManager.h"
#include "PluginManager.h"
#include "Download\DownloadManager.h"

DECLARE_REGISTERED_MESSAGE(GetMarshalIWebBrowserPtr)

#define MSG_WM_GETMARSHALIWEBBROWSERPTR()							   \
{															   \
	static UINT WM_GETMARSHALIWEBBROWSERPTR = GET_REGISTERED_MESSAGE(GetMarshalIWebBrowserPtr);	\
	if ( uMsg == WM_GETMARSHALIWEBBROWSERPTR ) { \
		SetMsgHandled(TRUE);								   \
		IStream*	pStream = nullptr;							\
		CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_spBrowser, &pStream);	\
		lResult = (LRESULT)pStream;									   \
		if ( IsMsgHandled() )								   \
			return TRUE;									   \
	}														   \
}

namespace {

	
static const LPCTSTR	g_lpszLight[] = {
	_T("<span id='unDonutHilight' style='color:black;background:#FFFF00'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#00FFFF'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#FF00FF'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#7FFF00'>"),
	_T("<span id='unDonutHilight' style='color:black;background:#1F8FFF'>"),
};

static const int	g_LIGHTMAX	= _countof(g_lpszLight);


struct _Function_SelectEmpt {
	void operator ()(IHTMLDocument2 *pDocument)
	{
		CComPtr<IHTMLSelectionObject> spSelection;
		HRESULT 	hr = pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
	}
};

struct _Function_Hilight2 {
	LPCTSTR 	m_lpszKeyWord;
	BOOL		m_bHilight;

	_Function_Hilight2(LPCTSTR lpszKeyWord, BOOL bHilight)
		: m_lpszKeyWord(lpszKeyWord), m_bHilight(bHilight)
	{	}

	void operator ()(IHTMLDocument2* pDocument)
	{
		if (m_bHilight) {
			if ( !FindHilight(pDocument) ) {
				MakeHilight(pDocument);
			}
		} else {
			RemoveHilight(pDocument);
		}
	}

	// ハイライト作成
	void MakeHilight(IHTMLDocument2* pDocument)
	{
	try {
		// キーワードの最初の一語を取得
		CString		strKeyWord = m_lpszKeyWord;

		//+++ 単語区切りを調整
		LPCTSTR		strExcept	= _T(" \t\"\r\n　");
		strKeyWord = _tcstok( strKeyWord.GetBuffer(0), strExcept );
		strKeyWord.TrimLeft(strExcept);
		strKeyWord.TrimRight(strExcept);

		int 	nLightIndex = 0;
		HRESULT hr;

		// キーワードが空になるまで繰り返し
		while ( !strKeyWord.IsEmpty() ) {
			CComPtr<IHTMLElement>		spHTMLElement;
			// <body>を取得
			hr = pDocument->get_body(&spHTMLElement);
			if (spHTMLElement == NULL) 
				break;

			CComQIPtr<IHTMLBodyElement>	spHTMLBody = spHTMLElement;
			if (spHTMLBody == NULL) 
				break;

			// テキストレンジを取得
			CComPtr<IHTMLTxtRange>	  spHTMLTxtRange;
			hr = spHTMLBody->createTextRange(&spHTMLTxtRange);
			if (!spHTMLTxtRange)
				AtlThrow(hr);			

			//+++ 最大キーワード数(無限ループ対策)
			static unsigned maxKeyword	= Misc::getIEMejourVersion() <= 6 ? 1000 : 10000;
			//+++ 無限ループ状態を強制終了させるため、ループをカウントする
			unsigned num = 0;

			// キーワードを検索
			CComBSTR		bstrText= strKeyWord;
			VARIANT_BOOL	vBool	= VARIANT_FALSE;
			while (spHTMLTxtRange->findText(bstrText, 1, 0, &vBool), vBool == VARIANT_TRUE) {
				// 現在選択しているHTMLテキストを取得
				CComBSTR	bstrTextNow;
				hr = spHTMLTxtRange->get_text(&bstrTextNow);
				if (FAILED(hr))
					AtlThrow(hr);

				// <span>を付加
				CComBSTR	bstrTextNew;
				bstrTextNew.Append(g_lpszLight[nLightIndex]);	// <span ～
				bstrTextNew.Append(bstrTextNow);
				bstrTextNew.Append(_T("</span>"));


				CComPtr<IHTMLElement> spParentElement;
				hr = spHTMLTxtRange->parentElement(&spParentElement);
				if (FAILED(hr))
					AtlThrow(hr);

				CComBSTR	bstrParentTag;
				hr = spParentElement->get_tagName(&bstrParentTag);
				if (FAILED(hr))
					AtlThrow(hr);

				if (   bstrParentTag != _T("SCRIPT")
					&& bstrParentTag != _T("NOSCRIPT")
					&& bstrParentTag != _T("TEXTAREA")
					&& bstrParentTag != _T("STYLE"))
				{
					hr = spHTMLTxtRange->pasteHTML(bstrTextNew);	// ハイライトする
					if (FAILED(hr))
						AtlThrow(hr);

					//+++ 通常のページでハイライト置換がこんなにもあることはないだろうで、無限ループ扱いでうちどめしとく
					if (++num > maxKeyword)		
						break;
				}
				spHTMLTxtRange->collapse(VARIANT_FALSE);	// Caretの位置を選択したテキストの一番下に
			}

			++nLightIndex;
			if (nLightIndex >= g_LIGHTMAX)
				nLightIndex = 0;

			// 次のキーワードに
			strKeyWord = _tcstok(NULL, strExcept);
			strKeyWord.TrimLeft(strExcept);
			strKeyWord.TrimRight(strExcept);
		}

	} catch (const CAtlException& e) {
			e;	// 例外を握りつぶす
	}	// try
	}

	// ハイライトを解除する
	void RemoveHilight(IHTMLDocument2* pDocument)
	{
		CComBSTR	hilightID(L"unDonutHilight");
		CComBSTR	hilightTag(L"SPAN");

		CComPtr<IHTMLElementCollection> pAll;

		if (SUCCEEDED( pDocument->get_all(&pAll) ) && pAll != NULL) {
			CComVariant 		id(L"unDonutHilight");
			CComPtr<IDispatch>	pDisp;
			CComVariant 		vIndex(0);
			pAll->item(id, vIndex, &pDisp);
			if (pDisp == NULL)
				return;

			CComPtr<IUnknown>	pUnk;

			if (SUCCEEDED( pAll->get__newEnum(&pUnk) ) && pUnk != NULL) {
				CComQIPtr<IEnumVARIANT> pEnumVariant = pUnk;

				if (pEnumVariant != NULL) {
					VARIANT  v;
					ULONG	 ul;
					CComBSTR bstrTagName;
					CComBSTR bstrID;
					CComBSTR bstrTmp;

					while (pEnumVariant->Next(1, &v, &ul) == S_OK) {
						CComQIPtr<IHTMLElement> pElement = v.pdispVal;
						VariantClear(&v);

						if (pElement != NULL) {
							bstrTagName.Empty();
							bstrID.Empty();
							pElement->get_tagName(&bstrTagName);
							pElement->get_id(&bstrID);

							if (bstrTagName == hilightTag && bstrID == hilightID) {
								bstrTmp.Empty();
								pElement->get_innerHTML(&bstrTmp);
								pElement->put_outerHTML(bstrTmp);
							}
						}
					}
				}
			}
		}
	}

	// ハイライトがすでにされているか確認する
	BOOL FindHilight(IHTMLDocument2* pDocument)
	{
		CComPtr<IHTMLElementCollection> 	pAll;

		if (SUCCEEDED(pDocument->get_all(&pAll)) && pAll != NULL) {
			CComVariant 		id(L"unDonutHilight");
			CComPtr<IDispatch>	pDisp;
			CComVariant 		vIndex(0);
			pAll->item(id, vIndex, &pDisp);
			if (pDisp != NULL) {
				return TRUE;
			}
		}
		return FALSE;
	}
};

};	// namespace

/////////////////////////////////////////////////////////////
// CChildFrame::Impl

class CChildFrame::Impl :
	public CWindowImpl<Impl>,
	public CMessageFilter,
	public IWebBrowserEvents2Impl<Impl, ID_DONUTVIEW>,
	public CWebBrowserCommandHandler<Impl>,
	public CWebBrowser2
{
	friend class CChildFrame;

public:
	DECLARE_WND_CLASS_EX(_T("DonutChildFrame"), 0, COLOR_APPWORKSPACE)

	Impl(CChildFrame* pChild);

	void	SetThreadRefCount(int* pCount) { m_pThreadRefCount = pCount; }

	DWORD	GetExStyle() const { return m_view.GetExStyle(); }
	void	SetExStyle(DWORD dwStyle);
	void	SetDLCtrl(DWORD dwDLCtrl) { m_view.PutDLControlFlags(dwDLCtrl); }
	void	SetMarshalDLCtrl(DWORD dwDLCtrl) { m_dwMarshalDLCtrlFlags = dwDLCtrl; }
	void	SetAutoRefreshStyle(DWORD dwAutoRefresh) { m_view.SetAutoRefreshStyle(dwAutoRefresh); }
	void	SaveSearchWordflg(bool bSave) { m_bSaveSearchWordflg = bSave; }
	void 	SetSearchWordAutoHilight(const CString& str, bool bAutoHilight);
	void	SetTravelLog(const vector<std::pair<CString, CString> >& fore, const vector<std::pair<CString, CString> >& back) {
		m_TravelLogFore = fore; m_TravelLogBack = back;
	}

	CString	GetSelectedText();
	CString GetSelectedTextLine();

	// Event handlers
	void	OnBeforeNavigate2(IDispatch*		pDisp,
						   const CString&		strURL,
						   DWORD				nFlags,
						   const CString&		strTargetFrameName,
						   CSimpleArray<BYTE>&	baPostedData,
						   const CString&		strHeaders,
						   bool&				bCancel );
	void	OnDownloadBegin() { }
	void	OnDownloadComplete();
	void	OnTitleChange(const CString& strTitle);
	void	OnProgressChange(long progress, long progressMax);
	void	OnStatusTextChange(const CString& strText);
	void	OnSetSecureLockIcon(long nSecureLockIcon);
	void	OnPrivacyImpactedStateChange(bool bPrivacyImpacted);
	void	OnStateConnecting();
	void	OnStateDownloading();
	void	OnStateCompleted();
	void	OnDocumentComplete(IDispatch *pDisp, const CString& strURL);
	void	OnCommandStateChange(long Command, bool bEnable);
	void	OnNewWindow2(IDispatch **ppDisp, bool& bCancel);
	void	OnNewWindow3(IDispatch **ppDisp, bool& bCancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl);
	void	OnWindowClosing(bool IsChildWindow, bool& bCancel);

	// PreTranslateMessage用
	BOOL	OnRButtonHook(MSG* pMsg);
	BOOL	OnMButtonHook(MSG* pMsg);
	BOOL	OnXButtonUp(WORD wKeys, WORD wButton);

	// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalMessage(HWND /*hWnd*/) { delete m_pParentChild;/*this;*/ }
	void	searchEngines(const CString &strKeyWord);

	// Message map
	BEGIN_MSG_MAP( Impl )
		MSG_WM_CREATE		( OnCreate )
		MSG_WM_DESTROY		( OnDestroy )
		MSG_WM_CLOSE		( OnClose )
		MSG_WM_SIZE			( OnSize )
		MSG_WM_GETMARSHALIWEBBROWSERPTR()
		USER_MSG_WM_CHILDFRAMEACTIVATE( OnChildFrameActivate )
		USER_MSG_WM_SET_CHILDFRAME( OnGetChildFrame )
		USER_MSG_WM_GETCHILDFRAMEDATA( OnGetChildFrameData )
		USER_MSG_WM_MENU_GOBACK 	( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD	( OnMenuGoForward	)

		// ファイル
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_REF, OnEditOpenSelectedRef 	)	// リンクを開く
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_TEXT,OnEditOpenSelectedText	)	// URLテキストを開く

		// 編集
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND 			, OnEditFind				)
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX 		, OnEditFindMax 			)
		COMMAND_ID_HANDLER_EX( ID_TITLE_COPY			, OnTitleCopy				)
		COMMAND_ID_HANDLER_EX( ID_URL_COPY				, OnUrlCopy 				)
		COMMAND_ID_HANDLER_EX( ID_COPY_TITLEANDURL		, OnTitleAndUrlCopy 		)

		// 表示
		COMMAND_ID_HANDLER_EX( ID_VIEW_SETFOCUS 		, OnViewSetFocus			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP				, OnViewStop				)
		
		// ツール
		COMMAND_ID_HANDLER_EX( ID_EDIT_IGNORE			, OnAddClosePopupUrl		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_CLOSE_TITLE		, OnAddClosePopupTitle		)

		// ウィンドウ
		COMMAND_ID_HANDLER_EX( ID_FILE_CLOSE			, OnFileClose				)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnWindowCloseAll			)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_REFRESH_EXCEPT	, OnWindowRefreshExcept 	)

		COMMAND_RANGE_HANDLER_EX( ID_VIEW_BACK1   , ID_VIEW_BACK9	, OnViewBackX	)
		COMMAND_RANGE_HANDLER_EX( ID_VIEW_FORWARD1, ID_VIEW_FORWARD9, OnViewForwardX)
		
		// 検索バーから
		MSG_WM_USER_HILIGHT 		( OnHilight 		)
		MSG_WM_USER_FIND_KEYWORD	( OnFindKeyWord 	)
		// 独自ページ内検索バーから
		USER_MSG_WM_HILIGHTFROMFINDBAR( OnHilightFromFindBar )
		USER_MSG_WM_REMOVEHILIGHT( OnRemoveHilight )

		COMMAND_ID_HANDLER_EX( ID_HTMLZOOM_MENU			, OnHtmlZoomMenu			)
		// Special command
		COMMAND_ID_HANDLER_EX( ID_HTMLZOOM_ADD			, OnHtmlZoom				)
		COMMAND_ID_HANDLER_EX( ID_HTMLZOOM_SUB			, OnHtmlZoom				)
		COMMAND_ID_HANDLER_EX( ID_HTMLZOOM_100TOGLE		, OnHtmlZoom				)
		COMMAND_RANGE_HANDLER_EX( ID_HTMLZOOM_400 , ID_HTMLZOOM_050 , OnHtmlZoom    )
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_HOME 		, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_END		, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_PAGEUP	, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_PAGEDOWN	, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_UP		, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SPECIAL_DOWN 		, OnSpecialKeys		)
		COMMAND_ID_HANDLER_EX( ID_SAVEIMAGE			, OnSaveImage		)

		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH		, OnViewRefresh 	)

		CHAIN_COMMANDS_MEMBER( m_view )
		CHAIN_MSG_MAP( CWebBrowserCommandHandler<Impl> )

		if (uMsg == WM_COMMAND)
			GetTopLevelWindow().PostMessage(WM_COMMAND_FROM_CHILDFRAME, wParam, lParam);
	END_MSG_MAP()

	int		OnCreate(LPCREATESTRUCT /*lpCreateStruct*/);
	void	OnDestroy();
	void	OnClose();
	void	OnSize(UINT nType, CSize size);
	void	OnChildFrameActivate(HWND hWndAct, HWND hWndDeact);	// タブの切り替えが通知される
	CChildFrame* OnGetChildFrame() { return m_pParentChild; }
	void	OnGetChildFrameData(ChildFrameDataOnClose* pData) { _CollectDataOnClose(*pData); }

	LRESULT OnMenuGoBack(HMENU hMenu)	 { MenuChgGoBack(hMenu);	return 0; }
	LRESULT OnMenuGoForward(HMENU hMenu) { MenuChgGoForward(hMenu); return 0; }

	// ファイル
	void 	OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// 編集
	void	OnEditFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnEditFindMax(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { _OnEditFindMax(0, 0, NULL); }
	void	OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);	

	// 表示
	void	OnViewSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { m_view.SetFocus(); }
	void	OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	// ツール
	void 	OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// ウィンドウ
	void 	OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// 検索バーから
	LRESULT OnHilight(CString strKeyWord);
	LRESULT OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown, long Flags = 0);
	// 独自ページ内検索バーから
	int		OnHilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags);
	void	OnRemoveHilight();

	void	OnHtmlZoomMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	// Specla command
	void	OnHtmlZoom(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSpecialKeys(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSaveImage(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

private:
	DWORD	_GetInheritedDLCtrlFlags();
	DWORD	_GetInheritedExStyleFlags();
	bool	_CursorOnSelectedText();
	void	_InitTravelLog();
	void	_CollectDataOnClose(ChildFrameDataOnClose& data);
	void	_AutoImageResize(bool bFirst);
	void	_SetFavicon(const CString& strURL);
	void	_HilightOnce(IDispatch *pDisp, LPCTSTR lpszKeyWord);
	BOOL 	_FindKeyWordOne(IHTMLDocument2* pDocument, const CString& strKeyword, BOOL bFindDown, long Flags = 0);

	// Data members
	CChildFrame*	m_pParentChild;
	CDonutView	m_view;
	CChildFrameUIStateChange	m_UIChange;
	int*	m_pThreadRefCount;
	bool	m_bNowActive;
	bool	m_bSaveSearchWordflg;
	CString	m_strSearchWord;
	CComBSTR	m_strBookmark;
	int 		m_nPainBookmark;
	CString		m_strOldKeyword;
	bool	m_bNowHilight;
	bool	m_bAutoHilight;
	CString m_strStatusText;
	bool	m_bExecutedNewWindow;	// for OnMButtonHook
	bool	m_bCancelRButtonUp;		// for PreTranslateMessage
	DWORD	m_dwMarshalDLCtrlFlags;
	bool	m_bInitTravelLog;
	vector<std::pair<CString, CString> >	m_TravelLogFore;
	vector<std::pair<CString, CString> >	m_TravelLogBack;

	int		m_nImgScl;					//+++ zoom,imgサイズ自動設定での設定値.
	int		m_nImgSclSav;				//+++ zoom,imgサイズの100%とのトグル切り替え用
	int		m_nImgSclSw;				//+++ zoom,imgサイズの100%とのトグル切り替え用
	bool	m_bImagePage;	// 画像ファイルを開いたかどうか
	CSize	m_ImageSize;

	bool	m_bReload;
	bool	m_bNowNavigate;

};


CChildFrame::Impl::Impl(CChildFrame* pChild) : 
	m_pParentChild(pChild), 
	m_pThreadRefCount(nullptr), 
	m_view(m_UIChange),
	m_bNowActive(false),
	m_bSaveSearchWordflg(false),
	m_nPainBookmark(0),
	m_bNowHilight(false),
	m_bAutoHilight(false),
	m_bExecutedNewWindow(false),
	m_bCancelRButtonUp(false),
	m_dwMarshalDLCtrlFlags(0),
	m_bInitTravelLog(false),
	m_nImgScl(100),
	m_nImgSclSav(100),
	m_nImgSclSw(0),
	m_bImagePage(false),
	m_bReload(false),
	m_bNowNavigate(false)
{	}


void	CChildFrame::Impl::SetExStyle(DWORD dwStyle)
{
	m_view.SetExStyle(dwStyle);

	bool bNavigateLock = (dwStyle & DVS_EX_OPENNEWWIN) != 0;
	//m_MDITab.NavigateLockTab(m_hWnd, bNavigateLock);
	//m_bExPropLock					= (bNavigateLock && bExProp) /*? true : false*/;

}


void 	CChildFrame::Impl::SetSearchWordAutoHilight(const CString& str, bool bAutoHilight)
{
	m_strSearchWord = str;
	m_bAutoHilight	= bAutoHilight;
}

/// 選択範囲の文字列を取得(フレーム対応版)
CString CChildFrame::Impl::GetSelectedText()
{
	CString 	strSelText;
	MtlForEachHTMLDocument2( m_spBrowser, [&strSelText](IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>		spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( SUCCEEDED( hr ) ) {
			CComPtr<IDispatch>				spDisp;
			hr	   = spSelection->createRange(&spDisp);
			if ( SUCCEEDED( hr ) ) {
				CComQIPtr<IHTMLTxtRange>	spTxtRange = spDisp;
				if (spTxtRange != NULL) {
					CComBSTR				bstrText;
					hr	   = spTxtRange->get_text(&bstrText);
					if (SUCCEEDED(hr) && !!bstrText)
						strSelText = bstrText;
				}
			}
		}
	});
#if 0	//:::
	if (strSelText.IsEmpty()) {
		//+++ 選択範囲がない場合、右クリックメニュー経由のことがあるので、それ対策を強引に...
		CCustomContextMenu* pMenu = CCustomContextMenu::GetInstance();
		if (pMenu && pMenu->GetContextMenuMode() == CONTEXT_MENU_ANCHOR && pMenu->GetAnchorUrl().IsEmpty() == 0) {
			//bstrText		= pMenu->GetAnchorUrl();
			//bstrLocationUrl = pMenu->GetAnchorUrl();
			strSelText = CString(pMenu->GetAnchorUrl());
		}
	}
#endif
	return strSelText;
}



//+++ _OpenSelectedText()より分離.
CString CChildFrame::Impl::GetSelectedTextLine()
{
	CString str = GetSelectedText();
	if (str.IsEmpty() == 0) {
		int 	n	= str.Find(_T("\r"));
		int 	n2	= str.Find(_T("\n"));
		if ((n < 0 || n > n2) && n2 >= 0)
			n = n2;
		if (n >= 0) {
			str = str.Left(n);
		}
	}
	return str;
}


// Event handlers
void	CChildFrame::Impl::OnBeforeNavigate2(IDispatch*		pDisp,
										const CString&		strURL,
										DWORD				nFlags,
										const CString&		strTargetFrameName,
										CSimpleArray<BYTE>&	baPostedData,
										const CString&		strHeaders,
										bool&				bCancel )
{
	bool bTopWindow = IsPageIWebBrowser(pDisp);

	// Navigate中かつjavescriptから始まるウィンドウはナビゲートしない
	if (m_bNowNavigate && 
		strURL.Left(15).CompareNoCase(_T("javascript:void")) == 0 || strURL.CompareNoCase(_T("javascript:;")) == 0) 
	{
		bCancel = true;
		return;
	}

	if (bTopWindow) {
		if (m_dwMarshalDLCtrlFlags) {
			m_view.PutDLControlFlags(m_dwMarshalDLCtrlFlags);
			m_dwMarshalDLCtrlFlags = 0;
		}

		// 自動リサイズの設定を初期化
		m_bImagePage	= false;
		m_nImgSclSw		= (CMainOption::s_nAutoImageResizeType == AUTO_IMAGE_RESIZE_FIRSTON);
		m_ImageSize.SetSize(0, 0);

		/* ページ内検索の情報をリセット */
		m_nPainBookmark = 0;
		m_strBookmark	= LPCOLESTR(NULL);

		/* Faviconを白紙に設定 */
		//if (strURL.Left(11).CompareNoCase(_T("javascript:")) != 0) {
			//m_UIChange.SetLocationURL(strURL);
		//	_SetFavicon(strURL);
		//}

	}
	m_bNowNavigate = true;	// Navigate中である

	{
		//イベント発生 - ナビゲート
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_NAVIGATE, stn.nIndex, (SPARAM) &stn);
	}
}

void	CChildFrame::Impl::OnDownloadComplete()
{
	m_bNowNavigate = false;	// Navigate終了
}

void	CChildFrame::Impl::OnTitleChange(const CString& strTitle)
{
	SetWindowText(strTitle);
	m_UIChange.SetTitle(strTitle);

	m_UIChange.SetLocationURL(GetLocationURL());
}

void	CChildFrame::Impl::OnProgressChange(long progress, long progressMax)
{
	m_UIChange.SetProgress(progress, progressMax);
}

void	CChildFrame::Impl::OnStatusTextChange(const CString& strText)
{
	m_strStatusText = strText;
	m_UIChange.SetStatusText(strText);
}

void	CChildFrame::Impl::OnSetSecureLockIcon(long nSecureLockIcon)
{
	m_UIChange.SetSecureLockIcon((int)nSecureLockIcon);
}

void	CChildFrame::Impl::OnPrivacyImpactedStateChange(bool bPrivacyImpacted)
{
	m_UIChange.SetPrivacyImpacted(bPrivacyImpacted);
}

void	CChildFrame::Impl::OnStateConnecting()
{
	READYSTATE	state;
	HRESULT hr = m_spBrowser->get_ReadyState(&state);
	if (hr == S_OK && state == READYSTATE_COMPLETE)
		return;

	//m_MDITab.SetConnecting(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMECONNECTING, (WPARAM)m_hWnd);
	//m_bPrivacyImpacted = TRUE;	// ページ読み込み後もOnStateConnectingが呼ばれるページがあるので
								// クッキー制限アイコンが表示されないページがある
	//m_nSecureLockIcon = secureLockIconUnsecure;
}

void	CChildFrame::Impl::OnStateDownloading()
{
	READYSTATE	state;
	HRESULT hr = m_spBrowser->get_ReadyState(&state);
	if (hr == S_OK && state == READYSTATE_COMPLETE)
		return;

	//m_MDITab.SetDownloading(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMEDOWNLOADING, (WPARAM)m_hWnd);
}

void	CChildFrame::Impl::OnStateCompleted()
{
	//m_MDITab.SetComplete(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMECOMPLETE, (WPARAM)m_hWnd);

	if (m_bReload) {
		CString strUrl = GetLocationURL();
		CComQIPtr<IDispatch>	spDisp = m_spBrowser;
		OnDocumentComplete(spDisp, strUrl);
		m_bReload = false;
	}
}

/// documentが操作できるようになった
void	CChildFrame::Impl::OnDocumentComplete(IDispatch *pDisp, const CString& strURL)
{
	if ( IsPageIWebBrowser(pDisp) ) {
		_InitTravelLog();	// トラベルログを設定

		_AutoImageResize(true);

		_SetFavicon(strURL);

		bool bHilight = m_bAutoHilight || CDonutSearchBar::GetInstance()->GetHilightSw();
		if (bHilight && m_strSearchWord.IsEmpty() == FALSE) {
			if (m_bNowActive)
				GetTopLevelWindow().SendMessage(WM_SETSEARCHTEXT, (WPARAM)(LPCTSTR)m_strSearchWord, true);
			CString strWords = m_strSearchWord;
			DeleteMinimumLengthWord(strWords);
			m_bNowHilight = true;
			_HilightOnce(pDisp, strWords);
		}
	}

	{
		//プラグインイベント - ロード完了
		CComQIPtr<IWebBrowser2> 	pWB2 = pDisp;

		if (pWB2) {
			DEVTS_TAB_DOCUMENTCOMPLETE	dc;
			dc.lpstrURL   = (LPCTSTR) strURL;

			CComBSTR	bstrTitle;
			pWB2->get_LocationName(&bstrTitle);

			CString 	strTitle = bstrTitle;
			dc.lpstrTitle = (LPCTSTR) strTitle;
			dc.nIndex	  = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
			dc.bMainDoc   = IsPageIWebBrowser(pDisp);
			CPluginManager::BroadCast_PluginEvent(DEVT_TAB_DOCUMENTCOMPLETE, dc.nIndex, (DWORD_PTR) &dc);
		}
	}
}

/// ブラウザのコマンドの状態が変化した(戻る、進むなどの)
void	CChildFrame::Impl::OnCommandStateChange(long Command, bool bEnable)
{
	if (Command == CSC_NAVIGATEBACK)
		m_UIChange.SetNavigateBack(bEnable);
	else if (Command == CSC_NAVIGATEFORWARD)
		m_UIChange.SetNavigateForward(bEnable);
}

void	CChildFrame::Impl::OnNewWindow2(IDispatch **ppDisp, bool& bCancel)
{
#if 1
	m_bExecutedNewWindow = true;

	CChildFrame*	pChild = new CChildFrame;
	pChild->pImpl->SetThreadRefCount(m_pThreadRefCount);
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
		dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	}
	pChild->pImpl->m_view.SetDefaultFlags(dwDLCtrl, dwExStyle, 0);
	HWND hWnd = pChild->CreateEx(GetParent());
	ATLASSERT( ::IsWindow(hWnd) );

	pChild->pImpl->m_spBrowser->get_Application(ppDisp);
	ATLASSERT( ppDisp && *ppDisp );

	pChild->pImpl->SetSearchWordAutoHilight(m_strSearchWord, m_bNowHilight);

	GetTopLevelWindow().PostMessage(WM_TABCREATE, (WPARAM)pChild->pImpl->m_hWnd, TAB_LINK);
#endif
}

void	CChildFrame::Impl::OnNewWindow3(IDispatch **ppDisp, bool& bCancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl)
{
#if 0
	bCancel	= true;
	m_bExecutedNewWindow = true;

	NewChildFrameData	data(GetParent());
	data.strURL	= bstrUrl;
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
		dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	}
	data.dwDLCtrl	= dwDLCtrl;
	data.dwExStyle	= dwExStyle;
	data.bLink	= true;
	CString strSearchWord = m_strSearchWord;
	bool	bNowHilight	= m_bNowHilight;
	data.funcCallAfterCreated	= [strSearchWord, bNowHilight](CChildFrame* pChild) {
		pChild->SetSearchWordAutoHilight(strSearchWord, bNowHilight);
	};
	CChildFrame::AsyncCreate(data);
#endif
}

void	CChildFrame::Impl::OnWindowClosing(bool IsChildWindow, bool& bCancel)
{
	PostMessage(WM_CLOSE);
}


BOOL	CChildFrame::Impl::OnMButtonHook(MSG* pMsg)
{
	CIniFileI	 pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD	dwLinkOpenBtnM = pr.GetValue(_T("LinkOpenBtnM"), 0);
	if (dwLinkOpenBtnM == 0)	// ボタンが設定されていなかったら、終わり.
		return FALSE;

	//+++ メモ：デフォルトでは dwLinkOpenBtmM = IDM_FOLLOWLINKN
	// ATLASSERT(dwLinkOpenBtnM == IDM_FOLLOWLINKN);

	//+++ カーソルがリンクをさしていたらstatusBar用のメッセージがあることを利用.
	bool	bLink = false;
	if (   m_strStatusText.IsEmpty() == FALSE
		&& m_strStatusText != _T("ページが表示されました") 
		&& m_strStatusText != _T("完了") )			//+++ リンクのないページでは"ページが表示されました"というメッセージが設定されているので除外.
		bLink = true;

	::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 				//+++ おまじない.
	//int 	nTabCnt = m_MDITab.GetItemCount();									//+++ リンクをクリックしたかどうかのチェック用.
	//pChild->m_bAllowNewWindow = true;
	m_bExecutedNewWindow = false;
	::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);					//+++ リンクを新しい窓にひらく
	if (m_bExecutedNewWindow)
		return TRUE;
	//pChild->m_bAllowNewWindow = false;
	//int 	nTabCnt2 = m_MDITab.GetItemCount();
	//if (nTabCnt != nTabCnt2 || bLink)											//+++ リンクメッセージがあるか、タブが増えていたら、リンクをクリックしたとする.
	///	return TRUE;															//+++ trueを返して中ボタンクリックの処理をやったことにする.

	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ リンク以外をクリックした場合おまじないの左クリック押しを終了しておく.
	return FALSE;																//+++ falseを返すことで、IEコンポーネントにウィールクリックの処理を任せる.
}

static DWORD GetMouseButtonCommand(const MSG& msg)
{
	CString 	strKey;
	switch (msg.message) {
	case WM_LBUTTONUP:	strKey = _T("LButtonUp");					break;
	case WM_MBUTTONUP:	strKey = _T("MButtonUp");					break;
	case WM_XBUTTONUP:	strKey.Format(_T("XButtonUp%d"), GET_XBUTTON_WPARAM(msg.wParam)); break;
	case WM_MOUSEWHEEL:
		if (HIWORD(msg.wParam) > 0)
			strKey = _T("WHEEL_UP");
		else
			strKey = _T("WHEEL_DOWN");
		break;
	}

	CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	return pr.GetValue(strKey, 0);;
}

static int PointDistance(const CPoint& pt1, const CPoint& pt2)
{
	return (int)sqrt( pow(float (pt1.x - pt2.x), 2.0f) + pow(float (pt1.y - pt2.y), 2.0f) );
}

BOOL CChildFrame::Impl::OnRButtonHook(MSG* pMsg)
{
	if ( !(m_view.GetExStyle() & DVS_EX_MOUSE_GESTURE) )
		return FALSE;

	SetCapture();

	CPoint	ptDown(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
	::ClientToScreen(pMsg->hwnd, &ptDown);
	CPoint	ptLast = ptDown;

	bool bCursorOnSelectedText = CMouseOption::s_bUseRightDragSearch && _CursorOnSelectedText();
	HMODULE	hModule	= ::LoadLibrary(_T("ole32.dll"));
	CCursor cursor	= ::LoadCursor(hModule, MAKEINTRESOURCE(3));
	CString	strSearchEngine;
	CString strLastMark;
	CString strMove;
	DWORD	dwTime = 0;
	int		nDistance = 0;
	bool	bNoting = true;	// 何もしなかった
	MSG msg = { 0 };
	do {
		BOOL nRet = GetMessage(&msg, NULL, 0, 0);
		if (nRet == 0 || nRet == -1 || GetCapture() != m_hWnd)
			break;

		DWORD dwCommand = 0;
		switch (msg.message) {
		case WM_MOUSEWHEEL:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			dwCommand = GetMouseButtonCommand(msg);
			break;

		case WM_MOUSEMOVE: {
			if (bNoting == false)	// 他のコマンドを実行済み
				break;
			CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			::ClientToScreen(msg.hwnd, &ptNow);
			if (bCursorOnSelectedText) {
				if (nDistance < 10) {
					nDistance = PointDistance(ptDown, ptNow);	// 距離を求める
					if (nDistance < 10)
						break;
				}
				SetCursor(cursor);	// カーソルを変更する

				CString strMark;
				if (CMouseOption::s_bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// 角度を求める
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine = CMouseOption::s_strREngine;	
						strMark = _T("[→] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = CMouseOption::s_strTEngine;	
						strMark = _T("[↑] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = CMouseOption::s_strLEngine;	
						strMark = _T("[←] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = CMouseOption::s_strBEngine;
						strMark = _T("[↓] ");
					}
				} else {
					strSearchEngine = CMouseOption::s_strCEngine;
				}
				if (strSearchEngine.IsEmpty() == FALSE) {
					CString strMsg;
					strMsg.Format(_T("検索 %s: %s"), strMark, strSearchEngine);
					m_UIChange.SetStatusText(strMsg);
				} else {
					m_UIChange.SetStatusText(_T(""));
				}
			} else {
				nDistance = PointDistance(ptLast, ptNow);	// 距離を求める
				if (nDistance < 10)
					break;
				
				CString strMark1;
				int nAng1  = (int) _GetAngle(ptLast, ptNow);	// 角度を求める
				if		(nAng1 <  45 || nAng1 >  315)
					strMark1 = _T("→");
				else if (nAng1 >= 45 && nAng1 <= 135)
					strMark1 = _T("↑");
				else if (nAng1 > 135 && nAng1 <  225)
					strMark1 = _T("←");
				else if (nAng1 >= 225 && nAng1 <= 315)
					strMark1 = _T("↓");

				if (strMark1 == strLastMark) {					// 同じ方向に動かして、かつ300ms以上経ったなら有効		
					DWORD dwTimeNow = ::GetTickCount();
					if ( (dwTimeNow - dwTime) > 300 ) {
						strLastMark = _T("");
						dwTime	 = dwTimeNow;
					}
				}
				if (strMark1 != strLastMark) {
					strMove	+= strMark1;	// 方向を追加
					strLastMark = strMark1;	

					CString strCmdName;
					CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
					DWORD	dwCommand = pr.GetValue(strMove);
					if (dwCommand) {
						// 合致するコマンドがあれば表示
						CString strTemp;
						CToolTipManager::LoadToolTipText(dwCommand, strTemp);
						strCmdName.Format(_T("[ %s ]"), strTemp);
					}

					// ステータスバーに表示
					CString 	strMsg;
					strMsg.Format(_T("ジェスチャー : %s %s"), strMove, strCmdName);
					m_UIChange.SetStatusText(strMsg);
				}
				dwTime = ::GetTickCount();
			}
			ptLast = ptNow;
			break;
						   }

		case WM_LBUTTONDOWN:
			if (bCursorOnSelectedText && nDistance >= 10) {	// 右ボタンドラッグをキャンセルする
				m_bCancelRButtonUp = true;
				msg.message = WM_RBUTTONUP;
				m_UIChange.SetStatusText(_T(""));
			}
			break;
			
		default:
			::DispatchMessage(&msg);
			break;
		}	// switch

		switch (dwCommand) {
		case 0:	
			break;
			
		case ID_FILE_CLOSE:
			PostMessage(WM_CLOSE);	// これだけじゃたぶんダメ
			msg.message = WM_RBUTTONUP;
			//::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
			//::PostMessage(hWnd, WM_CLOSE, 0, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;

		case ID_GET_OUT:				// 退避
		case ID_VIEW_FULLSCREEN:		// 全体表示
		case ID_VIEW_UP:				// 上へ
		case ID_VIEW_BACK:				// 前に戻る
		case ID_VIEW_FORWARD:			// 次に進む
		case ID_VIEW_STOP_ALL:			// すべて中止
		case ID_VIEW_REFRESH_ALL:		// すべて更新
		case ID_WINDOW_CLOSE_ALL:		// すべて閉じる
		case ID_WINDOW_CLOSE_EXCEPT:	// これ以外閉じる
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;

		default:
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;
		}

	} while (msg.message != WM_RBUTTONUP);

	ReleaseCapture();
	::FreeLibrary(hModule);
	if (bCursorOnSelectedText) {
		SetCursor(::LoadCursor(NULL, IDC_ARROW));	// カーソルを元に戻す
		if (m_bCancelRButtonUp)
			return TRUE;
		if (strSearchEngine.IsEmpty() == FALSE) {	// 右ボタンドラッグ実行
			m_UIChange.SetStatusText(_T(""));
			CString str = GetSelectedTextLine();
			CDonutSearchBar::GetInstance()->SearchWebWithEngine(str, strSearchEngine);
			bNoting = false;
		}
	}

	/* マウスジェスチャーコマンド実行 */
	if (bNoting) {
		ptLast.SetPoint(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		::ClientToScreen(msg.hwnd, &ptLast);

		m_UIChange.SetStatusText(_T(""));

		CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		DWORD dwCommand = pr.GetValue(strMove);
		if (dwCommand) {
			::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting = false;
		} else if (dwCommand == -1)
			return TRUE;
	}

	if ( bNoting && strMove.IsEmpty() ) {	// 右クリックメニューを出す
		::ScreenToClient(pMsg->hwnd, &ptLast);
		pMsg->lParam = MAKELONG(ptLast.x, ptLast.y);

		::PostMessage(pMsg->hwnd, WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
	}
	return !bNoting;
}

BOOL CChildFrame::Impl::OnXButtonUp(WORD wKeys, WORD wButton)
{
	CString 	strKey;
	switch (wButton) {
	case XBUTTON1: strKey = _T("Side1"); break;
	case XBUTTON2: strKey = _T("Side2"); break;
	}

	CIniFileI pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD	dwCommand = pr.GetValue(strKey);
	if (dwCommand == 0)
		return FALSE;

	switch (dwCommand) {
	case ID_VIEW_BACK:		GoBack();		break;
	case ID_VIEW_FORWARD:	GoForward();	break;
	default:
		GetTopLevelWindow().SendMessage(WM_COMMAND, dwCommand);
	}
	return TRUE;
}

// Overrides

BOOL CChildFrame::Impl::PreTranslateMessage(MSG* pMsg)
{
	if (IsWindowVisible() == FALSE/*m_bNowActive == false*/)
		return FALSE;

	// 自動リサイズのトグル
	if (   m_bImagePage 
		&& pMsg->message == WM_LBUTTONDOWN 
		&& CMainOption::s_nAutoImageResizeType != AUTO_IMAGE_RESIZE_NONE)
	{
		CPoint	pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		CRect	rc;
		GetClientRect(&rc);
		rc.right -= GetSystemMetrics(SM_CXVSCROLL);
		rc.bottom-= GetSystemMetrics(SM_CYHSCROLL);
		if (rc.PtInRect(pt))
			OnHtmlZoom(0, ID_HTMLZOOM_100TOGLE, 0);
		return FALSE;
	}

	// ミドルクリック
	if ( pMsg->message == WM_MBUTTONDOWN && IsChild(pMsg->hwnd) && OnMButtonHook(pMsg) )
		return TRUE;

	// 右ドラッグキャンセル用
	if ( pMsg->message == WM_RBUTTONUP && m_bCancelRButtonUp ) {
		m_bCancelRButtonUp = false;
		return TRUE;
	}
	// マウスジェスチャーへ
	if ( pMsg->message == WM_RBUTTONDOWN && OnRButtonHook(pMsg) )
		return TRUE;

	// サイドボタン
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam)) )
			return TRUE;
	}
	return m_view.PreTranslateMessage(pMsg);
}

/// 選択中の文字列を検索(アドレスバーでCtrl+Enterしたときの検索エンジンが使われる)
void CChildFrame::Impl::searchEngines(const CString& strKeyWord)
{
	CString 	strSearchWord = strKeyWord;

	if (CAddressBarOption::s_bReplaceSpace)
		strSearchWord.Replace( _T("　"), _T(" ") );

	//_ReplaceCRLF(strSearchWord,CString(_T(" ")));
	//strSearchWord.Replace('\n',' ');
	//strSearchWord.Remove('\r');
	strSearchWord.Replace( _T("\r\n"), _T("") );

	CIniFileI	pr( g_szIniFileName, _T("AddressBar") );
	CString 		strEngin = pr.GetStringUW( _T("EnterCtrlEngin"), NULL, 256 );

	::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) strSearchWord, (LPARAM) (LPCTSTR) strEngin);
}


// Message map

int		CChildFrame::Impl::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	DefWindowProc();

	++(*m_pThreadRefCount);

	CChildFrameCommandUIUpdater::AddCommandUIMap(m_hWnd);
	m_UIChange.SetChildFrame(m_hWnd);

	m_view.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ATLASSERT( m_view.IsWindow() );
	m_view.SetDlgCtrlID(ID_DONUTVIEW);

	m_view.QueryControl(IID_IWebBrowser2, (void **)&m_spBrowser);

	WebBrowserEvents2Advise();

	SetRegisterAsBrowser(true);
	SetVisible(true);

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

void	CChildFrame::Impl::OnDestroy()
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);

	SetRegisterAsBrowser(false);

	WebBrowserEvents2Unadvise();

	m_spBrowser.Release();

	m_view.DestroyWindow();

	--(*m_pThreadRefCount);
	if (*m_pThreadRefCount == 0) {
		TRACEIN(_T("ChildFreameスレッドの破棄"));
		PostQuitMessage(0);
	}

	CChildFrameCommandUIUpdater::RemoveCommandUIMap(m_hWnd);
}

void	CChildFrame::Impl::OnClose()
{
	SetMsgHandled(FALSE);

	CWindow	wndMain = GetTopLevelWindow();

	ChildFrameDataOnClose*	pClosedTabData = new ChildFrameDataOnClose;
	_CollectDataOnClose(*pClosedTabData);
	wndMain.PostMessage(WM_ADDRECENTCLOSEDTAB, (WPARAM)pClosedTabData);

	{
		int nIndex = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
		//プラグインイベント - クローズ
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CLOSE, nIndex, 0);
	}

	wndMain.SendMessage(WM_TABDESTROY, (WPARAM)m_hWnd);
}

void	CChildFrame::Impl::OnSize(UINT nType, CSize size)
{
	DefWindowProc();
	if (m_view.IsWindow())
		m_view.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER);

	_AutoImageResize(false);
}

void	CChildFrame::Impl::OnChildFrameActivate(HWND hWndAct, HWND hWndDeact)
{
	if (hWndAct == m_hWnd) {
		m_bNowActive = true;
		if (MtlIsApplicationActive(m_hWnd) && m_view.IsWindow())
			m_view.SetFocus();

		if (CSearchBarOption::s_bSaveSearchWord) {
			CDonutSearchBar::GetInstance()->SetSearchStr(m_strSearchWord); //\\ 保存しておいた文字列を検索バーに戻す
			CDonutSearchBar::GetInstance()->ForceSetHilightBtnOn(m_bNowHilight);
		}

	} else if (hWndDeact == m_hWnd) {
		m_bNowActive = false;
		// _KillFocusToHTML
		HRESULT	hr = E_FAIL;
		CComQIPtr<IOleInPlaceObject> spIOleInPlaceObject = m_spBrowser;
		if(spIOleInPlaceObject) {
			hr = spIOleInPlaceObject->UIDeactivate(); // IEのUIを無効化
		}

		if( m_bSaveSearchWordflg ){	//\\ 現在、検索バーにある文字列を取っておく
			CDonutSearchBar::GetInstance()->GetEditCtrl().GetWindowText(m_strSearchWord.GetBuffer(1024), 1024);
			m_strSearchWord.ReleaseBuffer();
		} else {
			m_bSaveSearchWordflg = true;
		}
	}
}

/// ファイルを閉じる
void 	CChildFrame::Impl::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//+++ タブロック対策... OnClose側のだけで大丈夫のようだけれど、とりあえず.
	if (   //::: s_bMainframeClose == false											//+++ unDonut終了時以外で
		/*&&*/ _check_flag(m_view.GetExStyle(), DVS_EX_OPENNEWWIN)					//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK) )	//+++ ナビゲートロックのページを閉じない、の指定があれば
	{
		return; 																//+++ 閉じずに帰る
	}

	PostMessage(WM_CLOSE);
}

/// 全てのタブを閉じる
void 	CChildFrame::Impl::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	GetTopLevelWindow().SendMessage(WM_COMMAND, ID_WINDOW_CLOSE_ALL);
}

/// 自分以外のタブを閉じる
void 	CChildFrame::Impl::OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
}

void 	CChildFrame::Impl::OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
}


// 検索バーから

/// ハイライト
LRESULT CChildFrame::Impl::OnHilight(CString strKeyWord)
{
	bool bHilightSw = CDonutSearchBar::GetInstance()->GetHilightSw();
	//if (m_bNowHilight != bHilightSw)
	{
		m_bNowHilight	= bHilightSw;
		if (bHilightSw) {
			m_strSearchWord = strKeyWord;
			DeleteMinimumLengthWord(strKeyWord);
		} else {
			m_strSearchWord.Empty();
		}
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(strKeyWord, bHilightSw));
	}
	return TRUE;
}

/// ページ内検索
LRESULT CChildFrame::Impl::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown, long Flags /*= 0*/)
{
	if (!m_spBrowser)
		return 0;

	CComPtr<IDispatch>	spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDocument = spDisp;	// htmlの取得
	if (!spDocument)
		return 0;

	CString strKeyword = lpszKeyWord;
	strKeyword.Replace(_T('ﾞ'), _T('゛'));

	// 検索
	BOOL	bSts = _FindKeyWordOne(spDocument, strKeyword, bFindDown, Flags);
	if (bSts)
		return TRUE;

	// フレームウィンドウの取得
	CComPtr<IHTMLFramesCollection2> 	spFrames;
	hr = spDocument->get_frames(&spFrames);
	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) )
		return 0;

	// フレーム内ウィンドウの数を取得
	LONG	nCount	   = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return 0;

	BOOL	bFindIt    = FALSE;
	if (bFindDown) {	// ページ内検索 - 下
		for (LONG ii = m_nPainBookmark; ii < nCount; ii++) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// フレーム内のウィンドウを取得
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
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
				spDocumentFr = spDisp;
				if (!spDocument)
					continue;
			}

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, bFindDown, Flags);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}

	} else {			// ページ内検索 - 上
		if (m_nPainBookmark == 0 && !m_strBookmark)
			m_nPainBookmark = nCount - 1;

		for (LONG ii = m_nPainBookmark; ii >= 0; ii--) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// ウィンドウの取得
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
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
				spDocumentFr = spDisp;
				if (!spDocument)
					continue;
			}

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, bFindDown, Flags);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}
	}

	return bFindIt;
}

static void _RemoveHighlight(IHTMLDocument3* pDoc3)
{
	CComQIPtr<IHTMLDocument2> spDoc2 = pDoc3;
	_MtlForEachHTMLDocument2(spDoc2, [&](IHTMLDocument2* pDoc) {
		CComPtr<IHTMLSelectionObject> spSelection;	/* テキスト選択を空にする */
		pDoc->get_selection(&spSelection);
		if (spSelection.p)
			spSelection->empty();

		vector<CComPtr<IHTMLElement> > vecElm;
		CComPtr<IHTMLElementCollection>	spCol;
		CComQIPtr<IHTMLDocument3> spDoc3 = pDoc;
		spDoc3->getElementsByTagName(CComBSTR(L"span"), &spCol);
		ForEachHtmlElement(spCol, [&vecElm](IDispatch* pDisp) -> bool {
			CComQIPtr<IHTMLElement>	spElm = pDisp;
			if (spElm.p) {
				CComBSTR strID;
				spElm->get_id(&strID);
				if (strID && strID == L"udfbh")
					vecElm.push_back(spElm);
			}
			return true;
		});
		for (auto it = vecElm.rbegin(); it != vecElm.rend(); ++it) {
			CComBSTR str;
			(*it)->get_innerText(&str);
			(*it)->put_outerHTML(str);
		}
	});
}

int		CChildFrame::Impl::OnHilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags)
{
	CString strKeyword = strText;
	CComBSTR	strChar(L"Character");
	CComBSTR	strTextedit(L"Textedit");
	CComBSTR	strBackColor(L"BackColor");
	CComBSTR	strColor(L"greenyellow");

	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return 0;

	/* 前のハイライト表示を消す */
	CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc;
	if (bEraseOld)
		_RemoveHighlight(spDoc3);

	/* 単語をハイライトする */
	int nMatchCount = 0;
	if (strKeyword.GetLength() > 0) {
		_MtlForEachHTMLDocument2(spDoc, [&](IHTMLDocument2* pDoc) {
			CComPtr<IHTMLSelectionObject> spSelection;	/* テキスト選択を空にする */
			pDoc->get_selection(&spSelection);
			if (spSelection.p)
				spSelection->empty();

			CComPtr<IHTMLElement>	spElm;
			pDoc->get_body(&spElm);
			CComQIPtr<IHTMLBodyElement>	spBody = spElm;
			if (spBody.p == nullptr)
				return;

			CComPtr<IHTMLTxtRange>	spTxtRange;
			spBody->createTextRange(&spTxtRange);
			if (spTxtRange.p == nullptr)
				return;

			//long nMove;
			//spTxtRange->moveStart(strTextedit, -1, &nMove);
			//spTxtRange->moveEnd(strTextedit, 1, &nMove);
			VARIANT_BOOL	vResult;
			CComBSTR	strWord = strKeyword;
			while (spTxtRange->findText(strWord, 1, Flags, &vResult), vResult == VARIANT_TRUE) {
				CComPtr<IHTMLElement> spParentElement;
				spTxtRange->parentElement(&spParentElement);
				CComBSTR	bstrParentTag;
				spParentElement->get_tagName(&bstrParentTag);
				if (   bstrParentTag != _T("SCRIPT")
					&& bstrParentTag != _T("NOSCRIPT")
					&& bstrParentTag != _T("TEXTAREA")
					&& bstrParentTag != _T("STYLE")) 
				{
					if (bNoHighlight == false) {
						CComBSTR strInnerText;
						spTxtRange->get_text(&strInnerText);
						//VARIANT_BOOL	vRet;
						//spTxtRange->execCommand(strBackColor, VARIANT_FALSE, CComVariant(strColor), &vRet);
						CComBSTR strValue(L"<span id=\"udfbh\" style=\"color:black;background:greenyellow\">");//#00FFFF
						strValue += strInnerText;
						strValue += _T("</span>");
						spTxtRange->pasteHTML(strValue);
					}
					++nMatchCount;
				}
				spTxtRange->collapse(VARIANT_FALSE);
			}
		});
	}
	return nMatchCount;
}

void	CChildFrame::Impl::OnRemoveHilight()
{
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3> spDoc3 = spDisp;
	if (spDoc3)
		_RemoveHighlight(spDoc3);
}


/// ポップアップズームメニューを開く
void	CChildFrame::Impl::OnHtmlZoomMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::SetForegroundWindow(m_hWnd);

	CMenu	menu0;
	menu0.LoadMenu(IDR_ZOOM_MENU);
	ATLASSERT(menu0.IsMenu());
	CMenuHandle menu = menu0.GetSubMenu(0);
	ATLASSERT(menu.IsMenu());

	// ポップアップメニューを開く.
	POINT 	pt;
	::GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
}

void	CChildFrame::Impl::OnSpecialKeys(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCode = 0;
	switch (nID) {
	case ID_SPECIAL_HOME:		nCode = VK_HOME;	break;
	case ID_SPECIAL_END:		nCode = VK_END; 	break;
	case ID_SPECIAL_PAGEUP: 	nCode = VK_PRIOR;	break;
	case ID_SPECIAL_PAGEDOWN:	nCode = VK_NEXT;	break;
	case ID_SPECIAL_UP: 		nCode = VK_UP;		break;
	case ID_SPECIAL_DOWN:		nCode = VK_DOWN;	break;
	default:	
		ATLASSERT(FALSE);
		return ;
	}

	if (nID == ID_SPECIAL_HOME || ID_SPECIAL_END) {
		auto funcGetHTMLWindowOnCursorPos = [](CPoint& pt, IHTMLDocument3* pDoc) -> CComPtr<IHTMLWindow2> {
			auto funcGetIFrameAbsolutePosition = [](CComQIPtr<IHTMLElement>	spIFrame) -> CRect {
				CRect rc;
				spIFrame->get_offsetHeight(&rc.bottom);
				spIFrame->get_offsetWidth(&rc.right);
				CComPtr<IHTMLElement>	spCurElement = spIFrame;
				do {
					CPoint temp;
					spCurElement->get_offsetTop(&temp.y);
					spCurElement->get_offsetLeft(&temp.x);
					rc += temp;
					CComPtr<IHTMLElement>	spTemp;
					spCurElement->get_offsetParent(&spTemp);
					spCurElement.Release();
					spCurElement = spTemp;
				} while (spCurElement.p);
				
				return rc;
			};
			auto funcGetScrollPosition = [](CComQIPtr<IHTMLDocument2> spDoc2) -> CPoint {
				CPoint ptScroll;
				CComPtr<IHTMLElement>	spBody;
				spDoc2->get_body(&spBody);
				CComQIPtr<IHTMLElement2>	spBody2 = spBody;
				spBody2->get_scrollTop(&ptScroll.y);
				spBody2->get_scrollLeft(&ptScroll.x);
				if (ptScroll == CPoint(0, 0)) {
					CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc2;
					CComPtr<IHTMLElement>	spDocumentElement;
					spDoc3->get_documentElement(&spDocumentElement);
					CComQIPtr<IHTMLElement2>	spDocumentElement2 = spDocumentElement;
					spDocumentElement2->get_scrollTop(&ptScroll.y);
					spDocumentElement2->get_scrollLeft(&ptScroll.x);
				}
				return ptScroll;
			};

			HRESULT hr = S_OK;
			CComQIPtr<IHTMLDocument2>	spDoc2 = pDoc;

			CComPtr<IHTMLFramesCollection2>	spFrames;
			spDoc2->get_frames(&spFrames);
			CComPtr<IHTMLElementCollection>	spIFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"iframe"), &spIFrameCol);
			CComPtr<IHTMLElementCollection>	spFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"frame"), &spFrameCol);
			
			long frameslength = 0, iframelength = 0, framelength = 0;
			spFrames->get_length(&frameslength);
			spIFrameCol->get_length(&iframelength);
			spFrameCol->get_length(&framelength);
			ATLASSERT(frameslength == iframelength || frameslength == framelength);

			if (frameslength == iframelength && spIFrameCol.p && spFrames.p) {
				for (long i = 0; i < iframelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spIFrameCol->item(vIndex, vIndex, &spDisp2);
					CRect rcAbsolute = funcGetIFrameAbsolutePosition(spDisp2.p);
					CPoint ptScroll = funcGetScrollPosition(spDoc2);
					CRect rc = rcAbsolute - ptScroll;
					if (rc.PtInRect(pt)) {
						CComVariant vResult;
						spFrames->item(&vIndex, &vResult);
						CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
						return spWindow;
					}
				}
			}

			if (frameslength == framelength && spFrameCol.p && spFrames.p) {
				for (long i = 0; i < framelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spFrameCol->item(vIndex, vIndex, &spDisp2);
					CComQIPtr<IHTMLElement>	spFrame = spDisp2;
					if (spFrame.p) {
						CRect rc;
						spFrame->get_offsetLeft(&rc.left);
						spFrame->get_offsetTop(&rc.top);
						long temp;
						spFrame->get_offsetWidth(&temp);
						rc.right += rc.left + temp;
						spFrame->get_offsetHeight(&temp);
						rc.bottom+= rc.top + temp;
						if (rc.PtInRect(pt)) {
							CComVariant vResult;
							spFrames->item(&vIndex, &vResult);
							CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
							return spWindow;
						}
					}
				}
			}
			return nullptr;
		};

		CComPtr<IDispatch>	spDisp;
		HRESULT hr = m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument3>	spDocument = spDisp;
		if (spDocument == nullptr)
			return ;
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		CComPtr<IHTMLWindow2> spWindow = funcGetHTMLWindowOnCursorPos(pt, spDocument);
		if (spWindow) {
			CComQIPtr<IHTMLDocument2>	spFrameDocument;
			hr = spWindow->get_document(&spFrameDocument);
			if ( FAILED(hr) ) {
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					return ;
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					return ;
				spFrameDocument = spDisp;
				if (!spFrameDocument)
					return ;
				spWindow.Release();
				spFrameDocument->get_parentWindow(&spWindow);
				if (!spWindow)
					return ;
			}

			if (nID == ID_SPECIAL_HOME)
				spWindow->scrollBy(0, -300000);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +300000);
		} else {
			CComQIPtr<IHTMLDocument2>	spDocument2 = spDocument;
			spDocument2->get_parentWindow(&spWindow);
			ATLASSERT(spWindow);
			if (nID == ID_SPECIAL_HOME)
				spWindow->scrollBy(0, -300000);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +300000);
		}
	} else {
		m_view.SetFocus();

		INPUT	inputs[2] = { 0 };
		inputs[0].type		= INPUT_KEYBOARD;
		inputs[0].ki.wVk	= nCode;
		inputs[0].ki.wScan	= ::MapVirtualKey(nCode, 0);
		inputs[0].ki.dwFlags= 0;
		inputs[1].type		= INPUT_KEYBOARD;
		inputs[1].ki.wVk	= nCode;
		inputs[1].ki.wScan	= ::MapVirtualKey(nCode, 0);
		inputs[1].ki.dwFlags= KEYEVENTF_KEYUP;
		SendInput(2, inputs, sizeof(INPUT));

		//PostMessage(WM_KEYDOWN, nCode, 0);
	}
}

/// 画像を保存する
void	CChildFrame::Impl::OnSaveImage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto funcGetUrl = [](CComQIPtr<IDispatch> spDisp) -> LPCTSTR {
		CComBSTR strUrl;
		CComQIPtr<IHTMLImgElement>	spImage =  spDisp;
		if (spImage != NULL) {
			spImage->get_href(&strUrl);
		} else {
			CComQIPtr<IHTMLInputElement>	spInput = spDisp;
			if (spInput != NULL) {
				spInput->get_src(&strUrl);
			} else {
				CComQIPtr<IHTMLAreaElement>	spArea = spDisp;
				if (spArea != NULL) {
					spArea->get_href(&strUrl);
				}
			}
		}
		return strUrl;
	};

	CString strUrl = m_view.GetAnchorURL();
	if (strUrl.IsEmpty() == FALSE) {
		CDownloadManager::GetInstance()->DownloadStart(strUrl, NULL, NULL, DLO_SAVEIMAGE);
	} else {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		ATLASSERT(spDocument);
		CPoint	pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		CComPtr<IHTMLElement>	spHitElement;
		spDocument->elementFromPoint(pt.x, pt.y, &spHitElement);
		// Hit先が画像の場合
		CComQIPtr<IHTMLImgElement>	spImg = spHitElement;
		if (spImg == NULL) {
			// フレームの可能性
			CComQIPtr<IHTMLFrameElement3>	spFrame = spHitElement;
			if (spFrame) {
				LONG x, y;
				spHitElement->get_offsetTop(&y);
				spHitElement->get_offsetLeft(&x);
				pt.Offset(-x, -y);
				CComPtr<IDispatch>	spFrameDisp;
				spFrame->get_contentDocument(&spFrameDisp);
				CComQIPtr<IHTMLDocument2>	spFrameDocument = spFrameDisp;
				if (spFrameDocument == NULL) 
					return;	// 別ドメイン
				ATLASSERT(spFrameDocument);
				CComPtr<IHTMLElement>	spHitFrameElement;
				spFrameDocument->elementFromPoint(pt.x, pt.y, &spHitFrameElement);
				spImg = spHitFrameElement;
			}
		}
		if (spImg) {
			CString strUrl = funcGetUrl(spImg.p);
			if (strUrl.IsEmpty() == FALSE)
				CDownloadManager::GetInstance()->DownloadStart(strUrl, NULL, NULL, DLO_SAVEIMAGE);
		}
	}
}


void	CChildFrame::Impl::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_bReload = true;
	if (::GetAsyncKeyState(VK_CONTROL) < 0) 	// Inspired by DOGSTORE, Thanks
		Refresh2(REFRESH_COMPLETELY);
	else
		Refresh();
}

void	CChildFrame::Impl::OnHtmlZoom(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto SetBodyStyleZoom	= [this](int addSub, int scl, bool bWheel) {
		if (Misc::getIEMejourVersion() >= 7 && bWheel) {
			CComVariant vZoom;
			if (addSub) {
				m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &vZoom);
				scl =  static_cast<int>(vZoom.lVal);
				scl += addSub;
			}
			if (scl < 5)
				scl = 5;
			else if (scl > 500)
				scl = 500;

			vZoom.lVal = scl;
			m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoom, NULL); 

		} else {
			CComPtr<IDispatch>	spDisp;
			m_spBrowser->get_Document(&spDisp);
			CComQIPtr<IHTMLDocument2> 	pDoc = spDisp;
			if ( pDoc.p == nullptr )
				return;

			CComPtr<IHTMLElement>	spHTMLElement;
			if ( FAILED( pDoc->get_body(&spHTMLElement) ) && !spHTMLElement )
				return;
			CComQIPtr<IHTMLElement2>  spHTMLElement2 = spHTMLElement;
			if (!spHTMLElement2)
				return;

			CComPtr<IHTMLStyle>		pHtmlStyle;
			if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) && !pHtmlStyle )
				return;

			CComVariant variantVal;
			CComBSTR	bstrZoom( L"zoom" );
			if (addSub) {
				if ( FAILED(pHtmlStyle->getAttribute(bstrZoom, 1, &variantVal)) )
					return;
				CComBSTR	bstrTmp(variantVal.bstrVal);
				scl =  !bstrTmp.m_str ? 100 : ::wcstol((wchar_t*)LPCWSTR(bstrTmp.m_str), 0, 10);
				scl += addSub;
			}
			if (scl < 5)
				scl = 5;
			else if (scl > 500)
				scl = 500;

			m_nImgScl = scl;

			wchar_t	wbuf[128] = L"\0";
			swprintf_s(wbuf, L"%d%%", scl);
			variantVal = CComVariant(wbuf);
			pHtmlStyle->setAttribute(bstrZoom, variantVal, 1);
			if (scl != 100 && m_ImageSize != CSize(0, 0))		// スクロールバーを隠す
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));
			else 
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"auto"));
		}
	};	// lamda

	switch (nID) {
	case ID_HTMLZOOM_ADD:	SetBodyStyleZoom(+10, 0, wndCtl == 0);	break;
	case ID_HTMLZOOM_SUB:	SetBodyStyleZoom(-10, 0, wndCtl == 0);	break;

	case ID_HTMLZOOM_100TOGLE:
		if (m_nImgScl == 100) {	// 100% なら元の拡大率に
			m_nImgScl	 = m_nImgSclSav;
			m_nImgSclSw	 = 1;
		} else {				// それ以外なら 100% に
			m_nImgSclSav = m_nImgScl;
			m_nImgScl    = 100;
			m_nImgSclSw	 = 0;
		}
		SetBodyStyleZoom(0, m_nImgScl, false);
		break;

	default: {
		ATLASSERT(ID_HTMLZOOM_400 <= nID && nID <= ID_HTMLZOOM_050);
		static const int scls[] = { 400, 200, 150, 125, 100, 75, 50 };
		int  n = nID - ID_HTMLZOOM_400;
		if (n < 0 || n > ID_HTMLZOOM_050 - ID_HTMLZOOM_400)
			return;
		SetBodyStyleZoom(0, scls[n], false);
		}
		break;
	}
}


/// 選択範囲のリンクを開く
void 	CChildFrame::Impl::OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CSimpleArray<CString> arrUrls;
	bool bNoAddFromMenu = false;
	MtlForEachHTMLDocument2( m_spBrowser, [&arrUrls, &bNoAddFromMenu, this] (IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>	spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>				spDisp;
		hr = spSelection->createRange(&spDisp);
		if ( FAILED(hr) )
			return;

		CComQIPtr<IHTMLTxtRange>		spTxtRange = spDisp;
		if (!spTxtRange)
			return;

		CComBSTR						bstrLocationUrl;
		CComBSTR						bstrText;
		hr = spTxtRange->get_htmlText(&bstrText);
		if (FAILED(hr) || !bstrText) {	//+++
			if (bNoAddFromMenu == false) {	// 右クリックメニューから取ってくる
				bNoAddFromMenu = true;
				CString strURL = m_view.GetAnchorURL();
				if (strURL.IsEmpty() == FALSE)
					arrUrls.Add(strURL);
			}
		} else {
			hr = pDocument->get_URL(&bstrLocationUrl);
			if ( FAILED(hr) )
				return;

			//BASEタグに対処する minit
			CComPtr<IHTMLElementCollection> spAllClct;
			hr = pDocument->get_all(&spAllClct);
			if ( SUCCEEDED(hr) ) {
				CComQIPtr<IHTMLElementCollection> spBaseClct;
				CComVariant 		val = _T("BASE");
				CComPtr<IDispatch>	spDisp;
				hr = spAllClct->tags(val, &spDisp);
				spBaseClct	= spDisp;
				if ( SUCCEEDED(hr) && spBaseClct ) {
					long	length;
					hr = spBaseClct->get_length(&length);
					if (length > 0) {
						CComPtr<IHTMLElement> spElem;
						CComVariant 		  val1( (int) 0 ), val2( (int) 0 );
						hr = spBaseClct->item(val1, val2, (IDispatch **) &spElem);
						if ( SUCCEEDED(hr) ) {
							CComPtr<IHTMLBaseElement> spBase;
							hr = spElem->QueryInterface(&spBase);
							if ( SUCCEEDED(hr) ) {
								CComBSTR bstrBaseUrl;
								hr = spBase->get_href(&bstrBaseUrl);
								if ( SUCCEEDED(hr) && !(!bstrBaseUrl) )
									bstrLocationUrl = bstrBaseUrl;
							}
						}
					}
				}
			}

			MtlCreateHrefUrlArray( arrUrls, WTL::CString(bstrText), WTL::CString(bstrLocationUrl) );
			if (arrUrls.GetSize() > 0)
				bNoAddFromMenu = true;	// 選択範囲からリンクが見つかったので
		}
	});
#if 1	//:::
	//m_MDITab.SetLinkState(LINKSTATE_B_ON);
	for (int i = 0; i < arrUrls.GetSize(); ++i) {
		DonutOpenFile(arrUrls[i], 0);
	}
	//m_MDITab.SetLinkState(LINKSTATE_OFF);
#endif
}

/// URLテキストを開く
void 	CChildFrame::Impl::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = GetSelectedText();
	if (strText.IsEmpty())
		return;

	std::vector<CString> lines;
	lines.reserve(20);
	Misc::SeptTextToLines(lines, strText);
	size_t	size = lines.size();
	//+++ 選択文字列中にurlぽいものがなかったら、文字列検索にしてみる.
	bool	f	= 0;
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		f |= (strUrl.Find(_T(':')) >= 0) || (strUrl.Find(_T('/')) >= 0) || (strUrl.Find(_T('.')) >= 0) ||  (strUrl.Find(_T('\\')) >= 0);
		if (f)
			break;
	}
	if (f == 0) {	// urlぽい文字列がなかった...
		CDonutSearchBar*	pSearchBar = CDonutSearchBar::GetInstance();
		if (pSearchBar) {
			CString str	= lines[0];
			LPCTSTR	strExcept  = _T(" \t\"\r\n　");
			str.TrimLeft(strExcept);
			str.TrimRight(strExcept);

			pSearchBar->SearchWeb(str);
			return;
		}
	}
#if 0	//:::
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		Misc::StrToNormalUrl(strUrl);		//+++ 関数化
		DonutOpenFile(strUrl);
	}
#endif
}

/// ポップアップ抑止に追加して閉じます。
void 	CChildFrame::Impl::OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::Add( GetLocationURL() );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// タイトル抑止に追加
void 	CChildFrame::Impl::OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// nページ戻る
void	CChildFrame::Impl::OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_BACK1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoBack();
}

/// nページ進む
void	CChildFrame::Impl::OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_FORWARD1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoForward();
}

// 編集

void	CChildFrame::Impl::OnEditFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	if (CMainOption::s_bUseCustomFindBar) {
		CString strText = GetSelectedText();
		GetTopLevelWindow().SendMessage(WM_OPENFINDBARWITHTEXT, (WPARAM)(LPCTSTR)strText);
	} else {
		__super::OnEditFind(0, 0, NULL);
	}
}

/// タイトルをクリップボードにコピーします。
void	CChildFrame::Impl::OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(MtlGetWindowText(m_hWnd), m_hWnd);
}

/// アドレスをクリップボードにコピーします。
void	CChildFrame::Impl::OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(GetLocationURL(), m_hWnd);
}

/// タイトルとアドレスをクリップボードにコピーします。
void	CChildFrame::Impl::OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strText;
	strText.Format(_T("%s\r\n%s"), MtlGetWindowText(m_hWnd), GetLocationURL());
	MtlSetClipboardText(strText, m_hWnd);
}

// 表示

void	CChildFrame::Impl::OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	Stop();

	m_nDownloadCounter = 0;
	OnStateCompleted();
}


/// 継承するDLコントロールを得る
DWORD	CChildFrame::Impl::_GetInheritedDLCtrlFlags()
{
	DWORD dwDLFlags = CDLControlOption::s_dwDLControlFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) )
		dwDLFlags = m_view.GetDLControlFlags();
	return dwDLFlags;
}

/// 継承するExStyleを得る
DWORD	CChildFrame::Impl::_GetInheritedExStyleFlags()
{
	DWORD dwExFlags = CDLControlOption::s_dwExtendedStyleFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) ) {
		dwExFlags = m_view.GetExStyle();
	  #if 1	//+++ ナビゲートロックに関しては継承しない....
		dwExFlags &= ~DVS_EX_OPENNEWWIN;											//+++ off
		dwExFlags |= CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_OPENNEWWIN;	//+++ けど、デフォルト設定があれば、それを反映.
	  #endif
	}
	return dwExFlags;
}

bool	CChildFrame::Impl::_CursorOnSelectedText()
{
	try {
		HRESULT hr = S_OK;

		CPoint	pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		CComPtr<IDispatch>	spDisp;
		hr = m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		if (spDocument == NULL)
			AtlThrow(hr);


		auto funcGetHTMLWindowOnCursorPos = [](CPoint& pt, IHTMLDocument3* pDoc) -> CComPtr<IHTMLWindow2> {
			auto funcGetIFrameAbsolutePosition = [](CComQIPtr<IHTMLElement>	spIFrame) -> CRect {
				CRect rc;
				spIFrame->get_offsetHeight(&rc.bottom);
				spIFrame->get_offsetWidth(&rc.right);
				CComPtr<IHTMLElement>	spCurElement = spIFrame;
				do {
					CPoint temp;
					spCurElement->get_offsetTop(&temp.y);
					spCurElement->get_offsetLeft(&temp.x);
					rc += temp;
					CComPtr<IHTMLElement>	spTemp;
					spCurElement->get_offsetParent(&spTemp);
					spCurElement.Release();
					spCurElement = spTemp;
				} while (spCurElement.p);
				
				return rc;
			};
			auto funcGetScrollPosition = [](CComQIPtr<IHTMLDocument2> spDoc2) -> CPoint {
				CPoint ptScroll;
				CComPtr<IHTMLElement>	spBody;
				spDoc2->get_body(&spBody);
				CComQIPtr<IHTMLElement2>	spBody2 = spBody;
				spBody2->get_scrollTop(&ptScroll.y);
				spBody2->get_scrollLeft(&ptScroll.x);
				if (ptScroll == CPoint(0, 0)) {
					CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc2;
					CComPtr<IHTMLElement>	spDocumentElement;
					spDoc3->get_documentElement(&spDocumentElement);
					CComQIPtr<IHTMLElement2>	spDocumentElement2 = spDocumentElement;
					spDocumentElement2->get_scrollTop(&ptScroll.y);
					spDocumentElement2->get_scrollLeft(&ptScroll.x);
				}
				return ptScroll;
			};

			HRESULT hr = S_OK;
			CComQIPtr<IHTMLDocument2>	spDoc2 = pDoc;

			CComPtr<IHTMLFramesCollection2>	spFrames;
			spDoc2->get_frames(&spFrames);
			CComPtr<IHTMLElementCollection>	spIFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"iframe"), &spIFrameCol);
			CComPtr<IHTMLElementCollection>	spFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"frame"), &spFrameCol);
			
			long frameslength = 0, iframelength = 0, framelength = 0;
			spFrames->get_length(&frameslength);
			spIFrameCol->get_length(&iframelength);
			spFrameCol->get_length(&framelength);
			ATLASSERT(frameslength == iframelength || frameslength == framelength);

			if (frameslength == iframelength && spIFrameCol.p && spFrames.p) {	// インラインフレーム
				for (long i = 0; i < iframelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spIFrameCol->item(vIndex, vIndex, &spDisp2);
					CRect rcAbsolute = funcGetIFrameAbsolutePosition(spDisp2.p);
					CPoint ptScroll = funcGetScrollPosition(spDoc2);
					CRect rc = rcAbsolute - ptScroll;
					if (rc.PtInRect(pt)) {
						pt.x	-= rc.left;
						pt.y	-= rc.top;
						CComVariant vResult;
						spFrames->item(&vIndex, &vResult);
						CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
						return spWindow;
					}
				}
			}

			if (frameslength == framelength && spFrameCol.p && spFrames.p) {	// 普通のフレーム
				for (long i = 0; i < framelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spFrameCol->item(vIndex, vIndex, &spDisp2);
					CComQIPtr<IHTMLElement>	spFrame = spDisp2;
					if (spFrame.p) {
						CRect rc = funcGetIFrameAbsolutePosition(spFrame);
						//spFrame->get_offsetLeft(&rc.left);
						//spFrame->get_offsetTop(&rc.top);
						//long temp;
						//spFrame->get_offsetWidth(&temp);
						//rc.right += rc.left + temp;
						//spFrame->get_offsetHeight(&temp);
						//rc.bottom+= rc.top + temp;
						if (rc.PtInRect(pt)) {
							pt.x	-= rc.left;
							pt.y	-= rc.top;
							CComVariant vResult;
							spFrames->item(&vIndex, &vResult);
							CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
							return spWindow;
						}
					}
				}
			}
			return nullptr;
		};	// funcGetHTMLWindowOnCursorPos

		CComQIPtr<IHTMLDocument3>	spDocument3 = spDocument;
		CComPtr<IHTMLWindow2> spWindow = funcGetHTMLWindowOnCursorPos(pt, spDocument3);
		if (spWindow) {
			spDocument.Release();
			CComQIPtr<IHTMLDocument2>	spFrameDocument;
			hr = spWindow->get_document(&spFrameDocument);
			if ( FAILED(hr) ) {	// 別ドメイン
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					AtlThrow(hr);
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					AtlThrow(hr);
				spDocument = spDisp;
			} else {
				spDocument = spFrameDocument;
			}
		}

		CComPtr<IDispatch>	spTargetDisp;
		auto funcGetRangeDisp = [&spTargetDisp](CPoint pt, IHTMLDocument2 *pDocument) {
			CComPtr<IHTMLSelectionObject>		spSelection;
			HRESULT 	hr	= pDocument->get_selection(&spSelection);
			if ( SUCCEEDED(hr) ) {
				CComPtr<IDispatch>				spDisp;
				hr	   = spSelection->createRange(&spDisp);
				if ( SUCCEEDED(hr) ) {
					CComQIPtr<IHTMLTxtRange>	spTxtRange = spDisp;
					if (spTxtRange != NULL) {
						CComBSTR				bstrText;
						hr	   = spTxtRange->get_text(&bstrText);
						if (SUCCEEDED(hr) && !!bstrText) {
							spTargetDisp = spDisp;
						}
					}
				}
			}
		};

		funcGetRangeDisp(pt, spDocument);
		if (spTargetDisp == NULL)
			return false;

		CComQIPtr<IHTMLTextRangeMetrics2>	spMetrics = spTargetDisp;
		ATLASSERT(spMetrics);
		CComPtr<IHTMLRect>	spRect;
		hr = spMetrics->getBoundingClientRect(&spRect);
		if (FAILED(hr))
			AtlThrow(hr);

		CRect rc;
		spRect->get_top(&rc.top);
		spRect->get_left(&rc.left);
		spRect->get_right(&rc.right);
		spRect->get_bottom(&rc.bottom);
		if (rc.PtInRect(pt)) {
			return true;
		}


#if 0
		CComPtr<IHTMLRectCollection>	spRcCollection;
		hr = spMetrics->getClientRects(&spRcCollection);
		if (FAILED(hr))
			AtlThrow(hr);

		long l;
		spRcCollection->get_length(&l);
		for (long i = 0; i < l; ++i) {
			VARIANT vIndex;
			vIndex.vt	= VT_I4;
			vIndex.lVal	= i;
			VARIANT vResult;
			if (spRcCollection->item(&vIndex, &vResult) == S_OK) {
				CComQIPtr<IHTMLRect>	spRect = vResult.pdispVal;
				ATLASSERT(spRect);
				CRect rc;
				spRect->get_top(&rc.top);
				spRect->get_left(&rc.left);
				spRect->get_right(&rc.right);
				spRect->get_bottom(&rc.bottom);
				if (rc.PtInRect(pt)) {
					return true;
				}
			}
		}
#endif

	}
	catch (const CAtlException& e) {
		e;
	}
	return false;
}

/// トラベルログを初期化する
void	CChildFrame::Impl::_InitTravelLog()
{
	if (m_bInitTravelLog == false) {
		m_bInitTravelLog = true;
		if (m_TravelLogFore.empty() && m_TravelLogBack.empty())
			return ;
		auto LoadTravelLog = [this](vector<std::pair<CString, CString> >& arrLog, bool bFore) -> BOOL {
			HRESULT 					 hr;
			CComPtr<IEnumTravelLogEntry> pTLEnum;
			CComPtr<ITravelLogEntry>	 pTLEntryBase;
			int		nDir = bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK;

			CComQIPtr<IServiceProvider>	 pISP = m_spBrowser;
			if (pISP == NULL)
				return FALSE;

			CComPtr<ITravelLogStg>		 pTLStg;
			hr	   = pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
			if (FAILED(hr) || pTLStg == NULL)
				return FALSE;

			hr	   = pTLStg->EnumEntries(nDir, &pTLEnum);
			if (FAILED(hr) || pTLEnum == NULL)
				return FALSE;

			int		nLast	= ( 10 > arrLog.size() ) ? (int) arrLog.size() : 10;
			for (int i = 0; i < nLast; i++) {
				CComPtr<ITravelLogEntry> pTLEntry;

			  #if 1	//+++ UNICODE 対応.
				std::vector<wchar_t>	title = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].first ) );
				std::vector<wchar_t>	url   = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].second) );
				// CreateEntry の第四引数がTRUEだと前に追加される
				hr			 = pTLStg->CreateEntry(&url[0], &title[0], pTLEntryBase, !bFore, &pTLEntry);
			  #else
				CString 	strTitle	= arrLog[i].first;
				CString 	strURL		= arrLog[i].second;

				LPOLESTR				 pszwURL   = _ConvertString(strURL, strURL.GetLength() + 1);		//new
				LPOLESTR				 pszwTitle = _ConvertString(strTitle, strTitle.GetLength() + 1);	//new

				hr			 = pTLStg->CreateEntry(pszwURL, pszwTitle, pTLEntryBase, !bFore, &pTLEntry);
				delete[] pszwURL;																			//Don't forget!
				delete[] pszwTitle;
			  #endif

				if (FAILED(hr) || pTLEntry == NULL)
					return FALSE;

				if (pTLEntryBase.p)
					pTLEntryBase.Release();

				pTLEntryBase = pTLEntry;
			}

			return TRUE;
		};	// lamda

		LoadTravelLog(m_TravelLogFore, true);
		LoadTravelLog(m_TravelLogBack, false);
		m_TravelLogFore.clear();
		m_TravelLogBack.clear();
	}
}

/// 最近閉じたタブ用のデータを集める
void	CChildFrame::Impl::_CollectDataOnClose(ChildFrameDataOnClose& data)
{	
	data.strTitle	= GetLocationName();
	data.strURL		= GetLocationURL();
	data.dwDLCtrl	= m_view.GetDLControlFlags();
	data.dwExStyle	= m_view.GetExStyle();
	data.dwAutoRefreshStyle	= m_view.GetAutoRefreshStyle();

	HRESULT hr;
	CComQIPtr<IServiceProvider>	 pISP = m_spBrowser;
	if (pISP == NULL)
		return ;

	CComPtr<ITravelLogStg>		 pTLStg;
	hr	= pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
	if (FAILED(hr) || pTLStg == NULL)
		return ;

	auto GetTravelLog	= [&](vector<std::pair<CString, CString> >& vecLog, bool bFore) {
		DWORD	dwCount = 0;
		hr	= pTLStg->GetCount(bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK, &dwCount);
		if (FAILED(hr) || dwCount == 0)	// 単に履歴がないだけの状態ならtrue.
			return ;

		CComPtr<IEnumTravelLogEntry> pTLEnum;
		hr	= pTLStg->EnumEntries(bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK, &pTLEnum);
		if (FAILED(hr) || pTLEnum == NULL)
			return ;

		vecLog.reserve(10);

		int 	count = 0;
		for (int j = 0; j < (int)dwCount; ++j) {
			CComPtr<ITravelLogEntry>  pTLEntry	= NULL;
			LPOLESTR				  szURL 	= NULL;
			LPOLESTR				  szTitle	= NULL;
			DWORD	dummy = 0;
			hr = pTLEnum->Next(1, &pTLEntry, &dummy);
			if (pTLEntry == NULL || FAILED(hr))
				break;

			if (   SUCCEEDED( pTLEntry->GetTitle(&szTitle) ) && szTitle
				&& SUCCEEDED( pTLEntry->GetURL  (&szURL  ) ) && szURL  )
			{
				vecLog.push_back(std::make_pair<CString, CString>(szTitle, szURL));
				++count;
			}
			if (szTitle) ::CoTaskMemFree( szTitle );
			if (szURL)	 ::CoTaskMemFree( szURL );

			if (count >= 10)
				break;
		}
	};	// lamda

	GetTravelLog(data.TravelLogBack, false);
	GetTravelLog(data.TravelLogFore, true);
}

/// 自動画像リサイズ
void	CChildFrame::Impl::_AutoImageResize(bool bFirst)
{
	if (CMainOption::s_nAutoImageResizeType == AUTO_IMAGE_RESIZE_NONE)
		return ;

	if (bFirst) {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
		if (spDoc == nullptr)
			return ;
		CComBSTR	strmineType;
		spDoc->get_mimeType(&strmineType);
		if (strmineType == nullptr)
			return ;

		CString strExt = CString(strmineType).Left(3);
		if (strExt != _T("JPG") && strExt != _T("PNG") && strExt != _T("GIF"))
			return ;
	
		m_bImagePage = true;
	}
	if (m_bImagePage == false)
		return ;

	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return ;

	if (bFirst) {	// 画像のサイズを得る
		CComPtr<IHTMLElementCollection> 	pElemClct;
		if ( FAILED( spDoc->get_images(&pElemClct) ) )
			return;
		long	length = 0;
		pElemClct->get_length(&length);

		CComPtr<IDispatch>			pDisp;
		CComVariant 				varName ( 0L );
		CComVariant 				varIndex( 0L );
		if ( FAILED( pElemClct->item(varName, varIndex, &pDisp) ) )
			return;
		CComQIPtr<IHTMLImgElement>	pImg = pDisp;
		if ( pImg == nullptr )
			return;
		
		long	Width; 
		long	Height;
		if ( FAILED( pImg->get_width(&Width) ) )
			return;
		if ( FAILED( pImg->get_height(&Height) ) )
			return;
		m_ImageSize.SetSize((int)Width, (int)Height);
	}

	CComPtr<IHTMLElement>	spHTMLElement;
	spDoc->get_body(&spHTMLElement);
	CComQIPtr<IHTMLElement2>  spHTMLElement2 = spHTMLElement;
	if (!spHTMLElement2)
		return;

	RECT	rcClient;
	GetClientRect(&rcClient);

	enum { kMargen = 10/*32*/ };
	long 	scWidth  = (rcClient.right  - kMargen < 0) ? 1 : rcClient.right  - kMargen;
	long 	scHeight = (rcClient.bottom - kMargen < 0) ? 1 : rcClient.bottom - kMargen;

	double  sclW = scWidth  * 100.0 / m_ImageSize.cx;
	double  sclH = scHeight * 100.0 / m_ImageSize.cy;
	double  scl  = sclW < sclH ? sclW : sclH;
	if (scl < 1) {
		scl = 1;
	} else if (scl >= 100) {
		scl			= 100;
		m_nImgSclSav = 100;
	}
	if (m_nImgSclSw == 0) {
		m_nImgSclSav = int(scl);
		scl			 = 100;
	}
	m_nImgScl = int( scl );
	wchar_t		wbuf[128];
	swprintf_s(wbuf, L"%d%%", int(scl));
	CComVariant variantVal(wbuf);
	CComBSTR	bstrZoom( L"zoom" );

	CComPtr<IHTMLStyle>		pHtmlStyle;
	if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) || !pHtmlStyle )
		return;
	pHtmlStyle->setAttribute(bstrZoom, variantVal);
	if (m_nImgSclSw)
		pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));

}

/// タブなどにFaviconを設定
void	CChildFrame::Impl::_SetFavicon(const CString& strURL)
{
	CString strFaviconURL;
	HRESULT hr = S_OK;
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3>	spDocument = spDisp;
	if (spDocument) {
		CComPtr<IHTMLElementCollection>	spCol;
		spDocument->getElementsByTagName(CComBSTR(L"link"), &spCol);
		if (spCol) {
			ForEachHtmlElement(spCol, [&](IDispatch* pDisp) -> bool {
				CComQIPtr<IHTMLLinkElement>	spLink = pDisp;
				if (spLink.p) {
					CComBSTR strrel;
					spLink->get_rel(&strrel);
					CComBSTR strhref;
					spLink->get_href(&strhref);
					strrel.ToLower();
					if (strrel == _T("shortcut icon") || strrel == _T("icon")) {
						DWORD	dwSize = INTERNET_MAX_URL_LENGTH;
						hr = ::UrlCombine(strURL, strhref, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), &dwSize, 0);
						strFaviconURL.ReleaseBuffer();
						if (SUCCEEDED(hr))
							return false;
					}
				}
				return true;
			});
		}
	}
	if (strFaviconURL.IsEmpty()) {	// ルートにあるFaviconのアドレスを得る
		DWORD cchResult = INTERNET_MAX_URL_LENGTH;
		if (::CoInternetParseUrl(strURL, PARSE_ROOTDOCUMENT, 0, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH, &cchResult, 0) == S_OK) {
			strFaviconURL.ReleaseBuffer();
			strFaviconURL += _T("/favicon.ico");
		}
	}

	m_UIChange.SetFaviconURL(strFaviconURL);
	CFaviconManager::SetFavicon(m_hWnd, strFaviconURL);
}

void	CChildFrame::Impl::_HilightOnce(IDispatch *pDisp, LPCTSTR lpszKeyWord)
{
	CComQIPtr<IWebBrowser2> 	pWebBrowser = pDisp;
	if (pWebBrowser) {
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_Hilight2(lpszKeyWord, TRUE));
	}
}


	
BOOL CChildFrame::Impl::_FindKeyWordOne(IHTMLDocument2* pDocument, const CString& strKeyword, BOOL bFindDown, long Flags /*= 0*/)
{
	// ドキュメントがNULLなら終了
	if (!pDocument)
		return FALSE;

	HRESULT	hr = S_OK;
	// キーワードを一語取得
	//x strKeyWord = strtok( (LPSTR) strKeyWord.GetBuffer(0), " " );
	CString 	strKeyWord = strKeyword;//\\Misc::GetStrWord( strKeyword );
	LPCTSTR		strExcept  = _T(" \t\"\r\n　");
	strKeyWord.TrimLeft(strExcept);
	strKeyWord.TrimRight(strExcept);

	// <body>を取得
	CComPtr<IHTMLElement>		spHTMLElement;
	pDocument->get_body(&spHTMLElement);
	CComQIPtr<IHTMLBodyElement> spHTMLBody = spHTMLElement;
	if (!spHTMLBody)
		return FALSE;

	// テキストレンジを取得
	CComPtr<IHTMLTxtRange>		spTxtRange;
	spHTMLBody->createTextRange(&spTxtRange);
	if (!spTxtRange)
		return FALSE;

	if (m_strBookmark && m_strOldKeyword == strKeyword) {
		VARIANT_BOOL vMoveBookmark = VARIANT_FALSE;
		spTxtRange->moveToBookmark(m_strBookmark, &vMoveBookmark);
		if (vMoveBookmark == TRUE) {
			long lActual;
			if (bFindDown) {
				spTxtRange->collapse(false);	// Caretの位置を選択したテキストの一番下に
				if (Misc::getIEMejourVersion() < 9)
					spTxtRange->moveStart(CComBSTR(L"character"), 1, &lActual);
				//spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
			} else {
				spTxtRange->collapse(true);	// Caretの位置を選択したテキストの一番上に
				//if (Misc::getIEMejourVersion() < 9) 
				//	spTxtRange->moveEnd  (CComBSTR(L"character"), -10, &lActual);			
			}
		}
	} else {	// 検索範囲を全体にする
		long lActual;
		spTxtRange->moveStart(CComBSTR("Textedit"), -1, &lActual);
		spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
	}
	m_strOldKeyword = strKeyword;

	CComBSTR		bstrText(strKeyWord);
	BOOL			bSts  = FALSE;
	VARIANT_BOOL	vBool = VARIANT_FALSE;
	int	nSearchCount = 0;
	while (spTxtRange->findText(bstrText, (bFindDown) ? 1 : -1, Flags, &vBool), vBool == VARIANT_TRUE) {

		auto funcMove = [&spTxtRange, bFindDown] () {	// 検索範囲を変更する関数
			long lActual = 0;
			if (bFindDown)
				spTxtRange->collapse(false);
				//spTxtRange->moveStart(CComBSTR(L"character"), 1, &lActual);
			else
				spTxtRange->collapse(true);
				//spTxtRange->moveEnd  (CComBSTR(L"character"), -10, &lActual);
		};
		
		CComPtr<IHTMLElement>	spFirstParentElement;

		bool	bVisible = true;
		CComQIPtr<IHTMLElement> spParentElement;
		spTxtRange->parentElement(&spParentElement);
		spFirstParentElement = spParentElement;
		while (spParentElement) {
			CComQIPtr<IHTMLElement2>	spParentElm2 = spParentElement;
			CComPtr<IHTMLCurrentStyle>	spStyle;
			spParentElm2->get_currentStyle(&spStyle);
			if (spStyle) {
				CComBSTR	strdisplay;
				spStyle->get_display(&strdisplay);
				if (strdisplay && strdisplay == _T("none")) {	// 表示されていない場合はスキップ
					funcMove();
					bVisible = false;
					break;
				}
			}
			CComPtr<IHTMLElement>	spPPElm;
			spParentElement->get_parentElement(&spPPElm);
			spParentElement = spPPElm;
		}
		if (bVisible == false)
			continue;

		CComBSTR	bstrParentTag;
		spFirstParentElement->get_tagName(&bstrParentTag);
		if (   bstrParentTag != _T("SCRIPT")
			&& bstrParentTag != _T("NOSCRIPT")
			&& bstrParentTag != _T("TEXTAREA")) 
			break;	/* 終わり */

		//++nSearchCount;
		//if (nSearchCount > 5)	// 5以上で打ち止め
		//	break;

		funcMove();
	}

	auto funcScrollBy = [](IHTMLDocument2 *pDoc2) {
		CComPtr<IHTMLDocument3> 	   pDoc3;
		HRESULT 	hr = pDoc2->QueryInterface(&pDoc3);
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLElement>		   pElem;
		hr	= pDoc3->get_documentElement(&pElem);
		if ( FAILED(hr) )
			return;

		long		height = 0;
		hr	= pElem->get_offsetHeight(&height); 	// HTML表示領域の高さ
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLSelectionObject>  pSel;
		hr	= pDoc2->get_selection(&pSel);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>			   pDisp;
		hr	= pSel->createRange(&pDisp);

		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLTextRangeMetrics> pTxtRM;
		hr	= pDisp->QueryInterface(&pTxtRM);
		if ( FAILED(hr) )
			return;

		long		y = 0;
		hr	= pTxtRM->get_offsetTop(&y);		// 選択部分の画面上からのy座標
		if ( FAILED(hr) )
			return;

		long scy = y - height / 2;				// 画面中央までの距離

		// 距離が表示部分の1/4より大きければスクロールさせる
		if ( (scy > height / 4) || (scy < -height / 4) ) {
			CComPtr<IHTMLWindow2> pWnd;
			hr = pDoc2->get_parentWindow(&pWnd);

			if ( FAILED(hr) )
				return;

			pWnd->scrollBy(0, scy);
		}
	};

	if (vBool == VARIANT_FALSE) {
		CComPtr<IHTMLSelectionObject> spSelection;
		pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
	} else {
		if (spTxtRange->getBookmark(&m_strBookmark) != S_OK)
			m_strBookmark = LPCOLESTR(NULL);

		spTxtRange->select();
		spTxtRange->scrollIntoView(VARIANT_TRUE);

		bSts = TRUE;

		if (CSearchBarOption::s_bScrollCenter)
			funcScrollBy(pDocument);
	}

	return bSts;
}



/////////////////////////////////////////////////////////////
// CChildFrame

CChildFrame::CChildFrame() : pImpl(new Impl(this))
{
}

CChildFrame::~CChildFrame()
{
	delete pImpl;
}

/// スレッドを立ててCChildFrameのインスタンスを作る
void	CChildFrame::AsyncCreate(NewChildFrameData& data)
{
	CChildFrame*	pChild = new CChildFrame;
	if (data.dwDLCtrl == -1)
		data.dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
	if (data.dwExStyle == -1)
		data.dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	pChild->pImpl->m_view.SetDefaultFlags(data.dwDLCtrl, data.dwExStyle, data.dwAutoRefresh);

	MultiThreadManager::ExecuteChildFrameThread(pChild, &data);
}

void	CChildFrame::SetThreadRefCount(int* pCount)
{
	pImpl->SetThreadRefCount(pCount);
}

HWND	CChildFrame::CreateEx(HWND hWndParent)
{
	return pImpl->Create(hWndParent, 0, NULL, WS_CHILD | WS_VISIBLE);
}

void	CChildFrame::Navigate2(LPCTSTR lpszURL)
{
	pImpl->Navigate2(lpszURL);
}

HWND	CChildFrame::GetHwnd() const
{
	return pImpl->m_hWnd;
}

DWORD	CChildFrame::GetExStyle() const
{
	return pImpl->GetExStyle();
}

void	CChildFrame::SetExStyle(DWORD dwStyle)
{
	pImpl->SetExStyle(dwStyle);
}

void	CChildFrame::SetDLCtrl(DWORD dwDLCtrl)
{
	pImpl->SetDLCtrl(dwDLCtrl);
}

void	CChildFrame::SetMarshalDLCtrl(DWORD dwDLCtrl)
{
	pImpl->SetMarshalDLCtrl(dwDLCtrl);
}

void	CChildFrame::SetAutoRefreshStyle(DWORD dwAutoRefresh)
{
	pImpl->SetAutoRefreshStyle(dwAutoRefresh);
}

void	CChildFrame::SaveSearchWordflg(bool bSave)
{
	pImpl->SaveSearchWordflg(bSave);
}

void	CChildFrame::SetSearchWordAutoHilight(const CString& str, bool bAutoHilight)
{
	pImpl->SetSearchWordAutoHilight(str, bAutoHilight);
}

void	CChildFrame::SetTravelLog(const vector<std::pair<CString, CString> >& fore, const vector<std::pair<CString, CString> >& back)
{
	pImpl->SetTravelLog(fore, back);
}


CComPtr<IWebBrowser2>	CChildFrame::GetIWebBrowser()
{
	return pImpl->m_spBrowser;
}


CComPtr<IWebBrowser2>	CChildFrame::GetMarshalIWebBrowser()
{
	IStream* pStream = (IStream*)pImpl->SendMessage(GET_REGISTERED_MESSAGE(GetMarshalIWebBrowserPtr));
	if (pStream == nullptr)
		return nullptr;
	CComPtr<IWebBrowser2>	spBrowser;
	CoGetInterfaceAndReleaseStream(pStream, IID_IWebBrowser2, (void**)&spBrowser);
	return spBrowser;
}

CString	CChildFrame::GetLocationURL()
{
	CComPtr<IWebBrowser2>	spBrowser = GetMarshalIWebBrowser();
	if (spBrowser == nullptr)
		return CString();
	CComBSTR	strtemp;
	spBrowser->get_LocationURL(&strtemp);
	if (strtemp)
		return CString(strtemp);
	return CString();
}

