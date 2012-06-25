/**
 *	@file	ChildFrame.cpp
 *	@brief	タブページ１つの処理.
 */

#include "stdafx.h"
#include "ChildFrame.h"
#include <regex>
#include "SharedMemoryUtil.h"
#include "MtlBrowser.h"
#include "MtlWin.h"
#include "ExStyle.h"
#include "DonutView.h"
#include "MultiThreadManager.h"
#include "ChildFrameCommandUIUpdater.h"
#include "GlobalConfig.h"
#include "option\RightClickMenuDialog.h"
#include "option\UrlSecurityOption.h"
#include "option\KeyBoardDialog.h"
//#include "option\MainOption.h"
//#include "option\DLControlOption.h"
//#include "option\MouseDialog.h"
//#include "option\IgnoreURLsOption.h"
//#include "option\CloseTitleOption.h"
//#include "option\SearchPropertyPage.h"
//#include "option\AddressBarPropertyPage.h"
//#include "option\UserDefinedCSSOption.h"
//#include "option\UserDefinedJavascriptOption.h"
#include "FaviconManager.h"
#include "ToolTipManager.h"
#include "BingTranslatorMenu.h"
//#include "PluginManager.h"
//#include "Download\DownloadManager.h"
//#include "MainFrame.h"

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

void	OpenMultiUrl(const std::pair<std::unique_ptr<WCHAR[]>, int>& pair, CWindow wndChildFrame)
{
	COPYDATASTRUCT cds;
	cds.dwData	= kOpenMultiUrl;
	cds.lpData	= static_cast<LPVOID>(pair.first.get());
	cds.cbData	= pair.second * sizeof(WCHAR);
	wndChildFrame.GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)wndChildFrame.m_hWnd, (LPARAM)&cds);
}


};	// namespace

/////////////////////////////////////////////////////////////
// CChildFrame::Impl

class CChildFrame::Impl :
	public CDoubleBufferWindowImpl<CChildFrame::Impl>,
	public CMessageFilter,
	public IWebBrowserEvents2Impl<CChildFrame::Impl, ID_DONUTVIEW>,
	public CWebBrowserCommandHandler<CChildFrame::Impl>,
	public CWebBrowser2
{
	friend class CChildFrame;

public:
	DECLARE_WND_CLASS_EX(_T("DonutChildFrame"), 0, 0/*COLOR_APPWORKSPACE*/)

	Impl(CChildFrame* pChild);

	void	SetThreadRefCount(int* pCount) { m_pThreadRefCount = pCount; }

	DWORD	GetExStyle() const { return m_view.GetExStyle(); }
	void	SetExStyle(DWORD dwStyle);
	void	SetDLCtrl(DWORD dwDLCtrl) { m_view.PutDLControlFlags(dwDLCtrl); }
	void	SetMarshalDLCtrl(DWORD dwDLCtrl) { m_dwMarshalDLCtrlFlags = dwDLCtrl; }
	void	SetAutoRefreshStyle(DWORD dwAutoRefresh) { m_view.SetAutoRefreshStyle(dwAutoRefresh); }
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
	void	DoPaint(CDCHandle dc);

	// Message map
	BEGIN_MSG_MAP( Impl )
		MSG_WM_CREATE		( OnCreate		)
		MSG_WM_DESTROY		( OnDestroy		)
		MSG_WM_CLOSE		( OnClose		)
		MSG_WM_SIZE			( OnSize		)
		MSG_WM_COPYDATA		( OnCopyData	)
		MSG_WM_SETFOCUS		( OnSetFocus	)
		MSG_WM_GETMARSHALIWEBBROWSERPTR()
		USER_MSG_WM_CHILDFRAMEACTIVATE( OnChildFrameActivate )
		USER_MSG_WM_SET_CHILDFRAME( OnGetChildFrame )
		USER_MSG_WM_GETCHILDFRAMEDATA( OnGetChildFrameData )
		USER_MSG_WM_MENU_GOBACK 	( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD	( OnMenuGoForward	)
		USER_MSG_WM_GETSELECTEDTEXT	( OnGetSelectedText	)
		USER_MSG_WM_SETPAGEBITMAP	( OnSetPageBitmap )
		USER_MSG_WM_DRAWCHILDFRAMEPAGE( OnDrawChildFramePage )
		USER_MSG_WM_INCREMENTTHREADREFCOUNT()
		USER_MSG_WM_GETBROWSERFONTSIZE()
		USER_MSG_WM_UPDATEURLSECURITYLIST( OnUpdateUrlSecurityList )
		USER_MSG_WM_ACCELTABLECHANGE	( OnAccelTableChange	)
		MESSAGE_HANDLER_EX( WM_SETPROXYTOCHLDFRAME, OnSetProxyToChildFrame	)
		USER_MSG_WM_CLOSEHANDLEFORSHAREDMEM()
		MESSAGE_HANDLER_EX( WM_DEFAULTRBUTTONDOWN	, OnDefaultRButtonDown	)
		MESSAGE_HANDLER_EX( WM_DEFAULTRBUTTONUP		, OnDefaultRButtonUp	)

		// ファイル
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_REF, OnEditOpenSelectedRef 	)	// リンクを開く
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_TEXT,OnEditOpenSelectedText	)	// URLテキストを開く

		// 編集
		COMMAND_ID_HANDLER	 ( ID_EDIT_FIND 			, OnEditFind				)
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX 		, OnEditFindMax 			)
		COMMAND_ID_HANDLER_EX( ID_TITLE_COPY			, OnTitleCopy				)
		COMMAND_ID_HANDLER_EX( ID_URL_COPY				, OnUrlCopy 				)
		COMMAND_ID_HANDLER_EX( ID_COPY_TITLEANDURL		, OnTitleAndUrlCopy 		)

		// 表示
		COMMAND_ID_HANDLER_EX( ID_VIEW_SETFOCUS 		, OnViewSetFocus			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP				, OnViewStop				)
		COMMAND_ID_HANDLER_EX( ID_VIEW_UP				, OnViewUp					)
		COMMAND_ID_HANDLER_EX( ID_VIEW_ROOT				, OnViewRoot				)
		COMMAND_ID_HANDLER_EX( ID_PRIVACYREPORT			, OnPrivacyReport			)
		
		// ツール
		COMMAND_ID_HANDLER_EX( ID_EDIT_IGNORE			, OnAddClosePopupUrl		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_CLOSE_TITLE		, OnAddClosePopupTitle		)

		// ウィンドウ
		COMMAND_ID_HANDLER_EX( ID_FILE_CLOSE			, OnFileClose				)

		COMMAND_RANGE_HANDLER_EX( ID_VIEW_BACK1   , ID_VIEW_BACK9	, OnViewBackX	)
		COMMAND_RANGE_HANDLER_EX( ID_VIEW_FORWARD1, ID_VIEW_FORWARD9, OnViewForwardX)
		
		// 検索バーから
		USER_MSG_WM_CHILDFRAMEFINDKEYWORD	( OnFindKeyWord 	)
		// 独自ページ内検索バーから
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
		CHAIN_MSG_MAP_MEMBER( m_BingTranslatorMenu )
		CHAIN_MSG_MAP( CDoubleBufferWindowImpl<CChildFrame::Impl> )
		CHAIN_MSG_MAP( CWebBrowserCommandHandler<CChildFrame::Impl> )

		if (uMsg == WM_COMMAND)
			GetTopLevelWindow().PostMessage(WM_COMMAND_FROM_CHILDFRAME, wParam, lParam);
	END_MSG_MAP()

	int		OnCreate(LPCREATESTRUCT /*lpCreateStruct*/);
	void	OnDestroy();
	void	OnClose();
	void	OnSize(UINT nType, CSize size);
	BOOL	OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void	OnSetFocus(CWindow wndOld);
	void	OnChildFrameActivate(HWND hWndAct, HWND hWndDeact);	// タブの切り替えが通知される
	CChildFrame* OnGetChildFrame() { return m_pParentChild; }
	void	OnGetChildFrameData(bool bCreateData);

	LRESULT OnMenuGoBack(HMENU hMenu)	 { MenuChgGoBack(hMenu);	return 0; }
	LRESULT OnMenuGoForward(HMENU hMenu) { MenuChgGoForward(hMenu); return 0; }
	void	OnGetSelectedText(LPCTSTR* ppStr);
	void	OnSetPageBitmap(HBITMAP* pBmp) { m_pPageBitmap = pBmp; }
	void	OnDrawChildFramePage(CDCHandle dc);
	void	OnUpdateUrlSecurityList() { m_UrlSecurity.ReloadList(); }
	void	OnAccelTableChange() { m_AcceleratorOption.ReloadAccelerator(GetTopLevelWindow()); }
	LRESULT OnSetProxyToChildFrame(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ファイル
	void 	OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// 編集
	LRESULT OnEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void	OnEditFindMax(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { _OnEditFindMax(0, 0, NULL); }
	void	OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);	

	// 表示
	void	OnViewSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { OnSetFocus(NULL); }
	void	OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewRoot(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnPrivacyReport(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	// ツール
	void 	OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// ウィンドウ
	void 	OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// 検索バーから
	LRESULT OnHilight(const CString& strKeyWord);
	int		OnFindKeyWord(HANDLE handle);
	// 独自ページ内検索バーから
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
	void	_SearchWebWithEngine(const CString& strText, const CString& strEngine);
	void	_ExecuteUserJavascript(const CString& strScriptText);
	int		_HilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags);

	// Data members
	CChildFrame*	m_pParentChild;
	CDonutView	m_view;
	CChildFrameUIStateChange	m_UIChange;
	GlobalConfigManageData		m_GlobalConfigManageData;
	GlobalConfig*				m_pGlobalConfig;
	CUrlSecurityForChildFrame	m_UrlSecurity;
	CAcceleratorOption			m_AcceleratorOption;
	CBingTranslatorMenu			m_BingTranslatorMenu;
	int*	m_pThreadRefCount;
	bool	m_bNowActive;
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
	bool	m_bClosing;
	//CBitmap	m_bmpPage;
	HBITMAP*	m_pPageBitmap;
	HANDLE	m_hMapChildFrameData;

	DWORD	m_dwThreadIdFromNewWindow;
};

#include "ChildFrame.inl"


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
	if (data.dwDLCtrl == -1)
		data.dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
	if (data.dwExStyle == -1)
		data.dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	//pChild->pImpl->m_view.SetDefaultFlags(data.dwDLCtrl, data.dwExStyle, data.dwAutoRefresh);

	if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiThreadMode) {
		CChildFrame*	pChild = new CChildFrame;
		MultiThreadManager::ExecuteChildFrameThread(pChild, &data);

	} else if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode) {

		CSharedMemoryHandle sharedMem;
		sharedMem.Serialize(data, nullptr, true);
		CString commandline;
		commandline.Format(_T("-NewProcessSharedMemoryData=%d"), sharedMem.Handle());

		/* 子プロセス作成 */
		STARTUPINFO	startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION	processInfo = { 0 };
		ATLVERIFY(::CreateProcess(Misc::GetExeFileName(), commandline.GetBuffer(0), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo));
		::CloseHandle(processInfo.hProcess);
		::CloseHandle(processInfo.hThread);


	} else {
		ATLASSERT(FALSE);
	}
}

void	CChildFrame::SetThreadRefCount(int* pCount)
{
	pImpl->SetThreadRefCount(pCount);
}

HWND	CChildFrame::CreateEx(HWND hWndParent)
{
	RECT rc;
	::GetClientRect(hWndParent, &rc);
	return pImpl->Create(hWndParent, rc, NULL, WS_CHILD /*| WS_VISIBLE*/);
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

void	CChildFrame::SetSearchWordAutoHilight(const CString& str, bool bAutoHilight)
{
	pImpl->SetSearchWordAutoHilight(str, bAutoHilight);
}

void	CChildFrame::SetTravelLog(const vector<std::pair<CString, CString> >& fore, const vector<std::pair<CString, CString> >& back)
{
	pImpl->SetTravelLog(fore, back);
}

void	CChildFrame::SetThreadIdFromNewWindow2(DWORD dwThreadId)
{
	pImpl->m_dwThreadIdFromNewWindow = dwThreadId;
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
	//CComPtr<IWebBrowser2>	spBrowser = GetMarshalIWebBrowser();
	//if (spBrowser == nullptr)
	//	return CString();
	//CComBSTR	strtemp;
	//spBrowser->get_LocationURL(&strtemp);
	//if (strtemp)
	//	return CString(strtemp);
	return CString();
}

CString CChildFrame::GetTitle()
{
	//CComPtr<IWebBrowser2>	spBrowser = GetMarshalIWebBrowser();
	//if (spBrowser == nullptr)
	//	return CString();
	//CComBSTR	strtemp;
	//spBrowser->get_LocationName(&strtemp);
	//if (strtemp)
	//	return CString(strtemp);
	return CString();
}

CString CChildFrame::GetSelectedTextLine()
{
	return pImpl->GetSelectedTextLine();
}