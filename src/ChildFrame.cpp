/**
 *	@file	ChildFrame.cpp
 *	@brief	�^�u�y�[�W�P�̏���.
 */

#include "stdafx.h"
#include "ChildFrame.h"
#include <regex>
#include <chrono>
#include <fstream>
#include <boost\serialization\string.hpp>
#include <boost\serialization\vector.hpp>
#include <boost\serialization\utility.hpp>
#include <boost\archive\text_wiarchive.hpp>
#include <boost\archive\text_woarchive.hpp>
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
#include "option\SupressPopupOption.h"
//#include "option\MainOption.h"
//#include "option\DLControlOption.h"
//#include "option\MouseDialog.h"
//#include "option\SearchPropertyPage.h"
//#include "option\AddressBarPropertyPage.h"
//#include "option\UserDefinedCSSOption.h"
//#include "option\UserDefinedJavascriptOption.h"
#include "FaviconManager.h"
#include "ToolTipManager.h"
#include "BingTranslatorMenu.h"
#include "AutoLogin.h"
#include "SearchBarHelper.h"
//#include "PluginManager.h"
//#include "Download\DownloadManager.h"
//#include "MainFrame.h"


#define WM_GETCHILDFRAMENOWACTIVE	(WM_APP + 1)
#define WM_EXECUTE_JAVASCRIPT		(WM_APP + 2)

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
	bool operator ()(IHTMLDocument2 *pDocument)
	{
		CComPtr<IHTMLSelectionObject> spSelection;
		HRESULT 	hr = pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
		return true;
	}
};

struct _Function_Hilight2 {
	LPCTSTR 	m_lpszKeyWord;
	BOOL		m_bHilight;
	function<bool ()>	m_funcProcessMessage;

	_Function_Hilight2(LPCTSTR lpszKeyWord, BOOL bHilight, function<bool ()> func)
		: m_lpszKeyWord(lpszKeyWord), m_bHilight(bHilight), m_funcProcessMessage(func)
	{	}

	bool operator ()(IHTMLDocument2* pDocument)
	{
		if (m_bHilight) {
			if ( !FindHilight(pDocument) ) {
				if (MakeHilight(pDocument) == false)
					return false;
			}
		} else {
			RemoveHilight(pDocument);
		}
		return true;
	}

	// �n�C���C�g�쐬
	bool MakeHilight(IHTMLDocument2* pDocument)
	{
		try {
			// �L�[���[�h�̍ŏ��̈����擾
			CString		strKeyWord = m_lpszKeyWord;

			//+++ �P���؂�𒲐�
			LPCTSTR		strExcept	= _T(" \t\"\r\n�@");
			strKeyWord = _tcstok( strKeyWord.GetBuffer(0), strExcept );
			strKeyWord.TrimLeft(strExcept);
			strKeyWord.TrimRight(strExcept);

			int 	nLightIndex = 0;
			HRESULT hr;

			// �L�[���[�h����ɂȂ�܂ŌJ��Ԃ�
			while ( !strKeyWord.IsEmpty() ) {
				CComPtr<IHTMLElement>		spHTMLElement;
				// <body>���擾
				hr = pDocument->get_body(&spHTMLElement);
				if (spHTMLElement == NULL) 
					break;

				CComQIPtr<IHTMLBodyElement>	spHTMLBody = spHTMLElement;
				if (spHTMLBody == NULL) 
					break;

				// �e�L�X�g�����W���擾
				CComPtr<IHTMLTxtRange>	  spHTMLTxtRange;
				hr = spHTMLBody->createTextRange(&spHTMLTxtRange);
				if (!spHTMLTxtRange)
					AtlThrow(hr);			

				//+++ �ő�L�[���[�h��(�������[�v�΍�)
				static unsigned maxKeyword	= Misc::getIEMejourVersion() <= 6 ? 1000 : 10000;
				//+++ �������[�v��Ԃ������I�������邽�߁A���[�v���J�E���g����
				unsigned num = 0;

				// �L�[���[�h������
				CComBSTR		bstrText= strKeyWord;
				VARIANT_BOOL	vBool	= VARIANT_FALSE;
				while (spHTMLTxtRange->findText(bstrText, 1, 0, &vBool), vBool == VARIANT_TRUE) {
					// ���ݑI�����Ă���HTML�e�L�X�g���擾
					CComBSTR	bstrTextNow;
					hr = spHTMLTxtRange->get_text(&bstrTextNow);
					if (FAILED(hr))
						AtlThrow(hr);

					// <span>��t��
					CComBSTR	bstrTextNew;
					bstrTextNew.Append(g_lpszLight[nLightIndex]);	// <span �`
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
						hr = spHTMLTxtRange->pasteHTML(bstrTextNew);	// �n�C���C�g����
						if (FAILED(hr))
							AtlThrow(hr);

						//+++ �ʏ�̃y�[�W�Ńn�C���C�g�u��������Ȃɂ����邱�Ƃ͂Ȃ����낤�ŁA�������[�v�����ł����ǂ߂��Ƃ�
						if (++num > maxKeyword)		
							break;
					}
					spHTMLTxtRange->collapse(VARIANT_FALSE);	// Caret�̈ʒu��I�������e�L�X�g�̈�ԉ���

					// ���܂��Ă郁�b�Z�[�W����������
					//if (m_funcProcessMessage() == false)
					//	return false;
				}

				++nLightIndex;
				if (nLightIndex >= g_LIGHTMAX)
					nLightIndex = 0;

				// ���̃L�[���[�h��
				strKeyWord = _tcstok(NULL, strExcept);
				strKeyWord.TrimLeft(strExcept);
				strKeyWord.TrimRight(strExcept);
			}

		} catch (const CAtlException& e) {
				e;	// ��O������Ԃ�
		}	// try
		return true;
	}

	// �n�C���C�g����������
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

	// �n�C���C�g�����łɂ���Ă��邩�m�F����
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


CRect GetIFrameAbsolutePosition(CComQIPtr<IHTMLElement>	spIFrame)
{
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

CPoint GetScrollPosition(CComQIPtr<IHTMLDocument2> spDoc2)
{
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


};	// namespace

/////////////////////////////////////////////////////////////
// CChildFrame::Impl

class CChildFrame::Impl :
	public CWindowImpl<CChildFrame::Impl>,
	public CMessageFilter,
	public IWebBrowserEvents2Impl<CChildFrame::Impl, ID_DONUTVIEW>,
	public CWebBrowserCommandHandler<CChildFrame::Impl>,
	public CWebBrowser2
{
	friend class CChildFrame;

public:
	DECLARE_WND_CLASS_EX(_T("DonutChildFrame"), 0, 0/*COLOR_APPWORKSPACE*/)

	// Constants
	enum { 
		kReturnTitleTimerId = 1, 
		kReturnTitleInterval = 2000,

		WM_DELAYDOCUMENTCOMPLETE = WM_APP + 300,
		kDelayHilightTimerId = 2,
		kDelayHilightInterval = 500,

		WM_DELAYHILIGHT	= WM_APP + 301,
	};

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

	// PreTranslateMessage�p
	BOOL	OnRButtonHook(MSG* pMsg);
	BOOL	OnMButtonHook(MSG* pMsg);
	BOOL	OnXButtonUp(WORD wKeys, WORD wButton);

	// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalMessage(HWND /*hWnd*/) { delete m_pParentChild;/*this;*/ }
	void	searchEngines(const CString &strKeyWord);

	// Message map
	BEGIN_MSG_MAP( Impl )
		MSG_WM_CREATE		( OnCreate		)
		MSG_WM_DESTROY		( OnDestroy		)
		MSG_WM_CLOSE		( OnClose		)
		MSG_WM_SIZE			( OnSize		)
		MSG_WM_COPYDATA		( OnCopyData	)
		MSG_WM_SETFOCUS		( OnSetFocus	)
		MSG_WM_TIMER		( OnTimer		)
		MESSAGE_HANDLER_EX( WM_FORWARDMSG, OnForwardMsg )

		MESSAGE_HANDLER_EX( WM_DELAYDOCUMENTCOMPLETE	, OnDelayDocumentComplete	)
		MESSAGE_HANDLER_EX( WM_DELAYHILIGHT				, OnDelayHilight )
		USER_MSG_WM_GETMARSHALIWEBBROWSERPTR()
		MESSAGE_HANDLER_EX( WM_GETCHILDFRAMENOWACTIVE, OnGetChildFrameActive	)
		USER_MSG_WM_CHILDFRAMEACTIVATE( OnChildFrameActivate )
		USER_MSG_WM_SET_CHILDFRAME( OnGetChildFrame )
		USER_MSG_WM_GETCHILDFRAMEDATA( OnGetChildFrameData )
		USER_MSG_WM_CREATETRAVELLOGMENU( OnCreateTravelLogMenu	)
		USER_MSG_WM_GETSELECTEDTEXT	( OnGetSelectedText	)
		USER_MSG_WM_SETPAGEBITMAP	( OnSetPageBitmap )
		USER_MSG_WM_DRAWCHILDFRAMEPAGE( OnDrawChildFramePage )
		USER_MSG_WM_INCREMENTTHREADREFCOUNT()
		USER_MSG_WM_GETBROWSERFONTSIZE()
		MESSAGE_HANDLER_EX( WM_SETPROXYTOCHLDFRAME, OnSetProxyToChildFrame	)
		USER_MSG_WM_CLOSEHANDLEFORSHAREDMEM()
		MESSAGE_HANDLER_EX( WM_DEFAULTRBUTTONDOWN	, OnDefaultRButtonDown	)
		MESSAGE_HANDLER_EX( WM_DEFAULTRBUTTONUP		, OnDefaultRButtonUp	)

		MESSAGE_HANDLER_EX( WM_EXECUTE_JAVASCRIPT	, OnExecuteJavascript	)
		USER_MSG_WM_GETLOGININFOMATION		( OnGetLoginInfomation	)
		USER_MSG_WM_SETLASTSCRIPTERRORMESSAGE( OnSetLastScriptErrorMessage	)
		USER_MSG_WM_CHANGECHILDFRAMEFLAGS	( OnChangeChildFrameFlags )

		// �t�@�C��
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_REF, OnEditOpenSelectedRef 	)	// �����N���J��
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_TEXT,OnEditOpenSelectedText	)	// URL�e�L�X�g���J��

		// �ҏW
		COMMAND_ID_HANDLER	 ( ID_EDIT_FIND 			, OnEditFind				)
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX 		, OnEditFindMax 			)
		COMMAND_ID_HANDLER_EX( ID_TITLE_COPY			, OnTitleCopy				)
		COMMAND_ID_HANDLER_EX( ID_URL_COPY				, OnUrlCopy 				)
		COMMAND_ID_HANDLER_EX( ID_COPY_TITLEANDURL		, OnTitleAndUrlCopy 		)

		// �\��
		COMMAND_ID_HANDLER_EX( ID_VIEW_SETFOCUS 		, OnViewSetFocus			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP				, OnViewStop				)
		COMMAND_ID_HANDLER_EX( ID_VIEW_UP				, OnViewUp					)
		COMMAND_ID_HANDLER_EX( ID_VIEW_ROOT				, OnViewRoot				)
		COMMAND_ID_HANDLER_EX( ID_PRIVACYREPORT			, OnPrivacyReport			)
		
		// �c�[��
		COMMAND_ID_HANDLER_EX( ID_EDIT_IGNORE			, OnAddClosePopupUrl		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_CLOSE_TITLE		, OnAddClosePopupTitle		)
		COMMAND_ID_HANDLER_EX( ID_STYLESHEET_BASE		, OnChangeCSS				)
		COMMAND_RANGE_HANDLER_EX( ID_INSERTPOINT_CSSMENU, ID_INSERTPOINT_CSSMENU_END, OnChangeCSS )
		COMMAND_ID_HANDLER_EX( ID_STYLESHEET_OFF		, OnChangeCSS				)

		// �E�B���h�E
		COMMAND_ID_HANDLER_EX( ID_FILE_CLOSE			, OnFileClose				)

		COMMAND_RANGE_HANDLER_EX( ID_VIEW_BACK1   , ID_VIEW_BACK9	, OnViewBackX	)
		COMMAND_RANGE_HANDLER_EX( ID_VIEW_FORWARD1, ID_VIEW_FORWARD9, OnViewForwardX)

		// �^�u�o�[����
		COMMAND_ID_HANDLER_EX( ID_TAB_CLONE	, OnTabClone )
		
		// �����o�[����
		USER_MSG_WM_CHILDFRAMEFINDKEYWORD	( OnFindKeyWord 	)
		// �Ǝ��y�[�W�������o�[����
		USER_MSG_WM_REMOVEHILIGHT( OnRemoveHilight )

		COMMAND_ID_HANDLER_EX( ID_STATUSBAR_DEFAULTPANE	, OnStatusBarDefaultPaneDblClk )

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
		COMMAND_ID_HANDLER_EX( ID_DOAUTOLOGIN, OnDoAutoLogin )

		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH		, OnViewRefresh 	)

		CHAIN_COMMANDS_MEMBER( m_view )
		CHAIN_MSG_MAP_MEMBER( m_BingTranslatorMenu )
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
	void	OnTimer(UINT_PTR nIDEvent);
	LRESULT OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDelayDocumentComplete(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDelayHilight(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetChildFrameActive(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void	OnChildFrameActivate(HWND hWndAct, HWND hWndDeact);	// �^�u�̐؂�ւ����ʒm�����
	CChildFrame* OnGetChildFrame() { return m_pParentChild; }
	void	OnGetChildFrameData(bool bCreateData);
	void	OnCreateTravelLogMenu(bool bFore);
	void	OnGetSelectedText(LPCTSTR* ppStr);
	void	OnSetPageBitmap(HBITMAP* pBmp) { m_pPageBitmap = pBmp; }
	void	OnDrawChildFramePage(CDCHandle dc);
	LRESULT OnSetProxyToChildFrame(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDefaultRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnExecuteJavascript(UINT uMsg, WPARAM wParam, LPARAM lParam);
	HANDLE	OnGetLoginInfomation(HANDLE hMapForClose);
	void	OnSetLastScriptErrorMessage(LPCTSTR strErrorMessage) { 
		m_UIChange.SetStatusText(_T("�X�N���v�g�G���[���������܂���"));
		m_strLastScriptErrorMessage = strErrorMessage; 
	}
	DWORD	OnChangeChildFrameFlags(ChildFrameChangeFlag change, DWORD flags);

	// �t�@�C��
	void 	OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// �ҏW
	LRESULT OnEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void	OnEditFindMax(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { _OnEditFindMax(0, 0, NULL); }
	void	OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);	

	// �\��
	void	OnViewSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { OnSetFocus(NULL); }
	void	OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewRoot(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnPrivacyReport(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void	OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	// �c�[��
	void 	OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnChangeCSS(UINT uNotifyCode, int nID, CWindow wndCtl);

	// �E�B���h�E
	void 	OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// �^�u�o�[����
	void	OnTabClone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// �����o�[����
	LRESULT OnHilight(const CString& strKeyWord);
	int		OnFindKeyWord(HANDLE handle);
	// �Ǝ��y�[�W�������o�[����
	void	OnRemoveHilight();

	void	OnStatusBarDefaultPaneDblClk(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnHtmlZoomMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	// Specla command
	void	OnHtmlZoom(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSpecialKeys(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSaveImage(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnDoAutoLogin(UINT uNotifyCode, int nID, CWindow wndCtl);

	void	OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

private:
	DWORD	_GetInheritedDLCtrlFlags();
	DWORD	_GetInheritedExStyleFlags();
	bool	_CursorOnSelectedText();
	void	_InitTravelLog();
	void	_CollectDataOnClose(ChildFrameDataOnClose& data);
	void	_AutoImageResize(bool bFirst);
	void	_SetFavicon(const CString& strURL);
	BOOL 	_FindKeyWordOne(IHTMLDocument2* pDocument, const CString& strKeyword, BOOL bFindDown, long Flags = 0);
	void	_SearchWebWithEngine(const CString& strText, const CString& strEngine);
	void	_ExecuteUserJavascript(const CString& strScriptText);
	int		_HilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags);
	void	_SetFocusToHTML();
	bool	_ShowLinkTextSelectWindow(MSG* pMsg);
	CString	_SearchBarHelper(const CString& URL);

	void	_HilightText(LPCTSTR lpszKeyWord, bool bHilight);
	void	_DeleteMinimumHilightTextLengthWord(CString& strWord);

	// Data members
	CChildFrame*	m_pParentChild;
	CDonutView	m_view;
	CChildFrameUIStateChange	m_UIChange;
	GlobalConfigManageData		m_GlobalConfigManageData;
	GlobalConfig*				m_pGlobalConfig;
	CBingTranslatorMenu			m_BingTranslatorMenu;
	int*	m_pThreadRefCount;
	bool	m_bNowActive;
	CString	m_strSearchWord;
	bool	m_bAutoHilight;
	CString m_strStatusText;
	bool	m_bExecutedNewWindow;	// for OnMButtonHook
	bool	m_bCancelRButtonUp;		// for PreTranslateMessage
	DWORD	m_dwMarshalDLCtrlFlags;
	bool	m_bInitTravelLog;
	vector<std::pair<CString, CString> >	m_TravelLogFore;
	vector<std::pair<CString, CString> >	m_TravelLogBack;

	int		m_nImgScl;					//+++ zoom,img�T�C�Y�����ݒ�ł̐ݒ�l.
	int		m_nImgSclSav;				//+++ zoom,img�T�C�Y��100%�Ƃ̃g�O���؂�ւ��p
	int		m_nImgSclSw;				//+++ zoom,img�T�C�Y��100%�Ƃ̃g�O���؂�ւ��p
	bool	m_bImagePage;	// �摜�t�@�C�����J�������ǂ���
	CSize	m_ImageSize;

	bool	m_bReload;
	bool	m_bNowNavigate;
	bool	m_bClosing;
	//CBitmap	m_bmpPage;
	HBITMAP*	m_pPageBitmap;
	HANDLE	m_hMapChildFrameData;

	DWORD	m_dwThreadIdFromNewWindow;
	CString m_strNewWindowURL;
	bool	m_bFirstNavigate;
	bool	m_bWaitNavigateLock;	// for NavigateLock
	int		m_nAutoLoginPrevIndex;
	CString m_strDelayLoadURL;

	bool	m_bMClickFail;
	CSharedMemory m_sharedTravelLogMenu;
	CString m_strLastScriptErrorMessage;

	struct FindHilightData {
		CString		strLastHilightText;
		CString		strLastSearchText;
		CComBSTR	strBookmark;
		int			nPaneBookmark;
		int			nLastHilightCount;
		long		flags;
		int			nHitPos;

		FindHilightData() : nPaneBookmark(0), nLastHilightCount(0), flags(0), nHitPos(0) { }

		void Clear() {
			strLastHilightText.Empty();
			strLastSearchText.Empty();
			strBookmark.Empty();
			nPaneBookmark = 0;
			nLastHilightCount = 0;
			flags = 0;
			nHitPos = 0;
		}
	};
	FindHilightData	m_SearchBarHilightData;
	FindHilightData m_FindBarHilightData;

	CSharedMemory	m_sharedMemKeyMessage;

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


/// �X���b�h�𗧂Ă�CChildFrame�̃C���X�^���X�����
/// MainFrame�������Ă���v���Z�X���炵���Ă�ł͂����Ȃ��I
void	CChildFrame::AsyncCreate(NewChildFrameData& data)
{
	if (data.dwDLCtrl == -1)
		data.dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
	if (data.dwExStyle == -1)
		data.dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	//pChild->pImpl->m_view.SetDefaultFlags(data.dwDLCtrl, data.dwExStyle, data.dwAutoRefresh);

	if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiThreadMode) {
		MultiThreadManager::CreateChildFrameThread(data, false);

	} else if (CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode) {
		MultiThreadManager::CreateChildProcess(data);

	} else {
		ATLASSERT(FALSE);
	}
}

HWND	CChildFrame::CreateChildFrame(const NewChildFrameData& data, int* pThreadRefCount)
{
	pImpl->SetThreadRefCount(pThreadRefCount);

	pImpl->m_dwThreadIdFromNewWindow = data.dwThreadIdFromNewWindow;
	pImpl->m_strNewWindowURL	= data.strNewWindowURL;
		
	// ChildFrame�E�B���h�E�쐬
	RECT rc;
	::GetClientRect(data.hWndParent, &rc);
	HWND hWndChildFrame = pImpl->Create(data.hWndParent, rc, NULL, WS_CHILD /*| WS_VISIBLE*/ | WS_CLIPSIBLINGS);
	return hWndChildFrame;
}

void	CChildFrame::InitChildFrame(const NewChildFrameData& data)
{
	pImpl->SetDLCtrl(data.dwDLCtrl);
	pImpl->SetExStyle(data.dwExStyle);

	pImpl->SetAutoRefreshStyle(data.dwAutoRefresh);
	pImpl->SetSearchWordAutoHilight(data.searchWord, data.bAutoHilight);
	pImpl->SetTravelLog(data.TravelLogFore, data.TravelLogBack);
	if (data.strURL.GetLength() > 0 && data.bDelayLoad == false)
		pImpl->Navigate2(data.strURL);
	else
		pImpl->m_strDelayLoadURL = data.strURL;
	pImpl->OnTitleChange(data.strTitle);
}

HWND	CChildFrame::GetHwnd() const
{
	return pImpl->m_hWnd;
}


CString CChildFrame::GetSelectedTextLine()
{
	return pImpl->GetSelectedTextLine();
}