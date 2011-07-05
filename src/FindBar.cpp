/**
*	@ file	FindBar.cpp
*	@brief	Firefoxライクなページ内検索バー
*/

#include "stdafx.h"
#include "FindBar.h"
#include "MtlWin.h"
#include "MainFrame.h"

//////////////////////////////////////////////
// CFindBar::Impl

class CFindBar::Impl : 
	public CDoubleBufferWindowImpl<Impl>,
	public CThemeImpl<Impl>,
	public CTrackMouseLeave<Impl>,
	public CMessageFilter
{
public:
	DECLARE_WND_CLASS(_T("DonutFindBar"))

	Impl();

	HWND	Create(HWND hWndParent);
	void	SetUpdateLayoutFunc(function<void (BOOL)> func) { m_funcUpdateLayout = func; }
	void	ShowFindBar(const CString& strKeyword);
	void	CloseFindBar();

	// Overrides
	void	DoPaint(CDCHandle dc);
	void	OnTrackMouseLeave();
	BOOL	PreTranslateMessage(MSG* pMsg);

	// Message map
	BEGIN_MSG_MAP_EX( Impl )
		CHAIN_MSG_MAP( CThemeImpl<Impl> )
		MSG_WM_DESTROY	 ( OnDestroy )
		MSG_WM_SIZE		 ( OnSize )
		MSG_WM_MOUSEMOVE ( OnMouseMove )
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_LBUTTONUP  ( OnLButtonUp	)
		MESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnMouseWheel )
		NOTIFY_CODE_HANDLER_EX( TBN_DROPDOWN, OnDropDownOption )
		MSG_WM_CTLCOLOREDIT	 ( OnCtlColorEdit )
		MSG_WM_CTLCOLORSTATIC( OnCtlColorStatic )
		COMMAND_HANDLER_EX( IDC_EDIT, EN_CHANGE, OnEditChanged )
		COMMAND_ID_HANDLER_EX( ID_FIND_PAGEDOWN	, OnFindPageDown )
		COMMAND_ID_HANDLER_EX( ID_FIND_PAGEUP	, OnFindPageUp )
		COMMAND_ID_HANDLER_EX( ID_FIND_HIGHLIGHT, OnFindHighlight )
		CHAIN_MSG_MAP( CDoubleBufferWindowImpl<Impl> )
		CHAIN_MSG_MAP( CTrackMouseLeave<Impl> )
	ALT_MSG_MAP(1)	// Edit
		MSG_WM_KEYDOWN		( OnEditKeyDown )
		MSG_WM_LBUTTONDOWN	( OnEditLButtonDown )
	END_MSG_MAP()

	void	OnDestroy();
	void	OnSize(UINT nType, CSize size);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDropDownOption(LPNMHDR pnmh);
	HBRUSH	OnCtlColorEdit(CDCHandle dc, CEdit edit);
	HBRUSH	OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
	void	OnEditChanged(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFindPageUp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFindPageDown(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFindHighlight(UINT uNotifyCode, int nID, CWindow wndCtl);

	// Edit
	void	OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnEditLButtonDown(UINT nFlags, CPoint point);

private:
	void	_HighlightKeyword(bool bNoHighlight = false, bool bEraseOld = true);
	void	_FindKeyword(bool bFindDown);

	// Constants
	enum {
		BarSize = 25,

		cxBarClose	= 5,
		cyBarClose	= 4,
		CloseWidth	= 17,
		CloseHeight = 16,

		cyOffset = 1,

		cxBarEdit = cxBarClose + CloseWidth + cxBarClose,
		EditWidth = 200,
		EditHeight = 22,
		
		cxToolBarOffset = cxBarEdit + EditWidth + 3,
		ToolBarHeight = 22,

		StaticWidth = 200,
		StaticHeight = 22,
	};
	// Data members
	CContainedWindowT<CEdit>	m_Edit;
	CToolBarCtrl	m_ToolBar;
	CStatic			m_static;
	
	CRect	m_rcClose;
	CLOSESTATES m_CloseState;
	int	m_nToolbarWidth;
	bool	m_bWordUnit;
	bool	m_bDistinguish;
	bool	m_bAutoHighlight;

	bool	m_bNoHit;
	int		m_nMatchCount;
	bool	m_bPageEnd;

	function<void (BOOL)>	m_funcUpdateLayout;
};

//=============================


CFindBar::Impl::Impl()
	: m_Edit(this, 1)
	, m_rcClose(cxBarClose, cyBarClose, cxBarClose + CloseWidth, cyBarClose + CloseHeight)
	, m_CloseState(TTCS_NORMAL)
	, m_bWordUnit(false)
	, m_bDistinguish(false)
	, m_bAutoHighlight(false)
	, m_bNoHit(false)
	, m_nMatchCount(0)
	, m_bPageEnd(false)
{
	SetThemeClassList(VSCLASS_TOOLTIP);
}

HWND	CFindBar::Impl::Create(HWND hWndParent)
{
	HWND hWnd = __super::Create(hWndParent, CRect(0, 0, 300, BarSize));
	ATLASSERT( ::IsWindow(hWnd) );
	ShowWindow(FALSE);

	m_Edit.Create(hWnd, CRect(cxBarEdit, cyOffset, EditWidth, EditHeight), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
	m_Edit.SetDlgCtrlID(IDC_EDIT);
	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_Edit.SetFont(lf.CreateFontIndirect());

	m_ToolBar.Create(hWnd, CRect(cxToolBarOffset, cyOffset, 0, ToolBarHeight), NULL, 
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST | CCS_NOPARENTALIGN | CCS_NODIVIDER | CCS_NORESIZE);
	m_ToolBar.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DOUBLEBUFFER);
	m_ToolBar.SetButtonStructSize();

	CImageList	imgList;
	ATLVERIFY(imgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 3, 1));
	CBitmap bmp;
	bmp.LoadBitmap(IDB_FIND);
	imgList.Add(bmp, RGB(255, 0, 255));
	m_ToolBar.SetImageList(imgList);
	
	static TBBUTTON	btns[] = {
		{ 0, ID_FIND_PAGEDOWN	, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, {0}, 0, (INT_PTR)_T("次を検索") },
		{ 1, ID_FIND_PAGEUP		, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, {0}, 0, (INT_PTR)_T("前を検索") },
		{ 2, ID_FIND_HIGHLIGHT	, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT, {0}, 0, (INT_PTR)_T("すべて強調表示") },
		{ I_IMAGENONE, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{ I_IMAGENONE, ID_FIND_OPTION	, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE | BTNS_SHOWTEXT, {0}, 0, (INT_PTR)_T("オプション") },
		{ I_IMAGENONE, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0}
	};
	m_ToolBar.AddButtons(_countof(btns), btns);
	m_ToolBar.SetButtonSize(0, ToolBarHeight);
	RECT rcleftButton;
	m_ToolBar.GetItemRect(_countof(btns) - 1, &rcleftButton);
	m_nToolbarWidth = rcleftButton.right;

	m_static.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE);
	m_static.SetFont(lf.CreateFontIndirect());

	CIniFileI	pr(g_szIniFileName, _T("FindBar"));
	m_bWordUnit = pr.GetValue(_T("WordUnit")) != 0;
	m_bDistinguish	= pr.GetValue(_T("Distinguish")) != 0;
	m_bAutoHighlight	= pr.GetValue(_T("AutoHighlight")) != 0;

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);
	return hWnd;
}

//--------------------------------
/// バーを表示する
void	CFindBar::Impl::ShowFindBar(const CString& strKeyword)
{
	if (strKeyword.IsEmpty() == FALSE)
		m_Edit.SetWindowText(strKeyword);
	ShowWindow(TRUE);
	m_funcUpdateLayout(FALSE);
	m_Edit.SetFocus();
	m_Edit.SetSelAll();
	_HighlightKeyword(!m_bAutoHighlight, false);	// 更新
}

//--------------------------------
/// バーを閉じる
void	CFindBar::Impl::CloseFindBar()
{
	m_CloseState = TTCS_NORMAL;
	ShowWindow(FALSE);
	m_funcUpdateLayout(FALSE);

	CChildFrame* pChild = g_pMainWnd->GetActiveChildFrame();
	if (pChild) 
		pChild->SendMessage(WM_COMMAND, ID_VIEW_SETFOCUS);
}


// Overrides

void	CFindBar::Impl::DoPaint(CDCHandle dc)
{
	RECT rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));

	// 下にラインを引く
	static COLORREF BorderColor = ::GetSysColor(COLOR_BTNSHADOW);
	HPEN hPen = ::CreatePen(PS_SOLID, 1, BorderColor);
	HPEN hOldPen = dc.SelectPen(hPen);
	--rc.bottom;
	dc.MoveTo(CPoint(rc.left, rc.bottom));
	dc.LineTo(rc.right, rc.bottom);
	dc.SelectPen(hOldPen);
	::DeleteObject(hPen);


	if (IsThemeNull() == false) {
		DrawThemeBackground(dc, TTP_CLOSE, m_CloseState, &m_rcClose);
	} else {
		WTL::CLogFont	lf;
		lf.SetMenuFont();
		CFont font = lf.CreateFontIndirect();
		HFONT hFontPrev = dc.SelectFont(font);

		dc.SetBkMode(TRANSPARENT);
		if (m_CloseState == TTCS_HOT)
			dc.SetTextColor(RGB(255, 0, 0));
		dc.DrawText(_T("Ｘ"), 1, &m_rcClose, DT_SINGLELINE | DT_VCENTER);

		dc.SelectFont(hFontPrev);
	}
}


void	CFindBar::Impl::OnTrackMouseLeave()
{
	CLOSESTATES oldState = m_CloseState;
	m_CloseState = TTCS_NORMAL;
	if (oldState != m_CloseState)
		InvalidateRect(&m_rcClose);
}

BOOL	CFindBar::Impl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_Edit.m_hWnd) {
		UINT msg = pMsg->message;
		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
			UINT nChar = (int) pMsg->wParam;
			if (nChar == VK_RETURN || nChar == VK_ESCAPE) {
				OnEditKeyDown(nChar, 0, 0);
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Message map

void CFindBar::Impl::OnDestroy()
{
	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
}

void CFindBar::Impl::OnSize(UINT nType, CSize size)
{
	if (m_Edit.IsWindow())
		m_Edit.MoveWindow(cxBarEdit, cyOffset, EditWidth, EditHeight);
	if (m_ToolBar.IsWindow())
		m_ToolBar.MoveWindow(cxToolBarOffset, cyOffset, m_nToolbarWidth, ToolBarHeight);
	if (m_static.IsWindow() && size.cx >= cxToolBarOffset + m_nToolbarWidth + 2)
		m_static.MoveWindow(cxToolBarOffset + m_nToolbarWidth + 2, cyOffset, size.cx/*StaticWidth*/, StaticHeight);
}


//------------------------------------
/// 閉じるボタンの状態を変える
void	CFindBar::Impl::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	CLOSESTATES oldState = m_CloseState;
	if (m_rcClose.PtInRect(point)) {
		if (GetCapture() == m_hWnd)
			m_CloseState = TTCS_PRESSED;
		else
			m_CloseState = TTCS_HOT;
	} else {
		m_CloseState = TTCS_NORMAL;
	}

	if (oldState != m_CloseState)
		InvalidateRect(&m_rcClose);
}

//-----------------------------------
/// TTCS_PRESSED用
void	CFindBar::Impl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rcClose.PtInRect(point)) {
		SetCapture();
		m_CloseState = TTCS_PRESSED;
		InvalidateRect(&m_rcClose);
	}
}

//----------------------------------
/// 閉じるボタン上で左ボタンアップしたときページ内検索バーを隠す
void	CFindBar::Impl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == m_hWnd) {
		ReleaseCapture();
		if (m_rcClose.PtInRect(point)) 
			CloseFindBar();
	}
}

//---------------------------------
/// エディットコントロールにフォーカスがある場合でもスクロールできるようにする
LRESULT CFindBar::Impl::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CChildFrame* pChild = g_pMainWnd->GetActiveChildFrame();
	if (pChild) 
		pChild->SendMessageToDescendants(uMsg, wParam, lParam);
	
	return 0;
}

LRESULT CFindBar::Impl::OnDropDownOption(LPNMHDR pnmh)
{
	enum {
		ID_WORDUNIT = 1,
		ID_DISTINC	= 2,
		ID_AUTOHIGHLIGHT = 3,
	};
	CMenu	menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(m_bWordUnit		 ? MFS_CHECKED : 0, (UINT_PTR)ID_WORDUNIT		, _T("単語単位で探す(&W)"));
	menu.AppendMenu(m_bDistinguish	 ? MFS_CHECKED : 0, (UINT_PTR)ID_DISTINC		, _T("大文字と小文字を区別する(&C)"));
	menu.AppendMenu(m_bAutoHighlight ? MFS_CHECKED : 0, (UINT_PTR)ID_AUTOHIGHLIGHT	, _T("見つかったすべての項目を強調表示する(&H)"));

	RECT rc;
	m_ToolBar.GetRect(ID_FIND_OPTION, &rc);
	m_ToolBar.ClientToScreen(&rc);
	int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, rc.left, rc.bottom, m_hWnd);


	CIniFileO	pr(g_szIniFileName, _T("FindBar"));
	switch (nRet) {
	case ID_WORDUNIT:		m_bWordUnit		 = !m_bWordUnit;		
		pr.SetValue(m_bWordUnit, _T("WordUnit"));
		break;
	case ID_DISTINC:		m_bDistinguish	 = !m_bDistinguish;		
		pr.SetValue(m_bDistinguish, _T("Distinguish"));
		break;
	case ID_AUTOHIGHLIGHT:	m_bAutoHighlight = !m_bAutoHighlight;	
		pr.SetValue(m_bAutoHighlight, _T("AutoHighlight"));
		break;
	};
	if (nRet != 0) {
		if (m_bAutoHighlight)
			_HighlightKeyword();
		else {
			_HighlightKeyword(true);	// 更新
			_FindKeyword(true);
		}
	}
	return 0;
}


HBRUSH	CFindBar::Impl::OnCtlColorEdit(CDCHandle dc, CEdit edit)
{
	if (m_bNoHit) { 
		CBrushHandle	brush;
		brush.CreateSolidBrush(RGB(255,102,102));
		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(RGB(255,102,102));
		dc.SetTextColor(RGB(255, 255, 255));
		return brush;
	} else {
		SetMsgHandled(FALSE);
		return 0;
	}
}

HBRUSH	CFindBar::Impl::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
{
	if (m_bPageEnd && wndStatic.m_hWnd == m_static.m_hWnd) {
		CBrushHandle	brush;
		brush.CreateSolidBrush(RGB(0,153,255));
		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(RGB(0,153,255));
		dc.SetTextColor(RGB(255, 255, 255));
		return brush;
	} else {
		SetMsgHandled(FALSE);
		return 0;
	}
}


void	CFindBar::Impl::OnEditChanged(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_HighlightKeyword(!m_bAutoHighlight);
	_FindKeyword(true);
}

//-----------------------------
/// 次を検索
void	CFindBar::Impl::OnFindPageDown(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_FindKeyword(true);
}

//----------------------------
/// 前を検索
void	CFindBar::Impl::OnFindPageUp(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_FindKeyword(false);
}

//----------------------------
/// ハイライト表示する
void	CFindBar::Impl::OnFindHighlight(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_HighlightKeyword();
}

// Edit

void	CFindBar::Impl::OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) 
		_FindKeyword(!(::GetKeyState(VK_SHIFT) < 0));
	else if (nChar == VK_ESCAPE)
		CloseFindBar();
	else
		m_Edit.DefWindowProc();
}

//---------------------------------
/// エディットコントロールがフォーカスを得たとき、テキストを全選択する
void	CFindBar::Impl::OnEditLButtonDown(UINT nFlags, CPoint point)
{
	if (GetFocus() != m_Edit.m_hWnd) {
		m_Edit.SetFocus();
		m_Edit.SetSelAll();
		_HighlightKeyword(!m_bAutoHighlight, false);	// 更新
	} else {
		SetMsgHandled(FALSE);
	}
}


void	CFindBar::Impl::_HighlightKeyword(bool bNoHighlight /*= false*/, bool bEraseOld /*= true*/)
{
	if (m_bPageEnd) {
		m_bPageEnd = false;
		m_static.Invalidate();
		m_static.UpdateWindow();
	}

	CString	strKeyword = MtlGetWindowText(m_Edit);

	CChildFrame* pChild = g_pMainWnd->GetActiveChildFrame();
	if (pChild == nullptr)
		return;

	CComPtr<IDispatch>	spDisp;
	pChild->m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return;

	/* テキスト選択を空にする */
	CComPtr<IHTMLSelectionObject> spSelection;
	spDoc->get_selection(&spSelection);
	if (spSelection)
		spSelection->empty();

	long Flags = 0;
	if (m_bWordUnit)
		Flags |= 2;
	if (m_bDistinguish)
		Flags |= 4;

	CComBSTR	strChar(L"Character");
	CComBSTR	strTextedit(L"Textedit");
	CComBSTR	strBackColor(L"BackColor");
	CComBSTR	strColor(L"greenyellow");

	/* 前のハイライト表示を消す */
	CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc;
	CComPtr<IHTMLElement>	spElmID;
	spDoc3->getElementById(CComBSTR(L"udfbh"), &spElmID);
	if (bEraseOld && spElmID) {
		vector<CComPtr<IHTMLElement> > vecElm;
		CComPtr<IHTMLElementCollection>	spCol;
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
	}

	/* 単語をハイライトする */
	m_nMatchCount = 0;
	if (strKeyword.GetLength() > 0) {
		CComPtr<IHTMLElement>	spElm;
		spDoc->get_body(&spElm);
		CComQIPtr<IHTMLBodyElement>	spBody = spElm;
		if (spBody == nullptr)
			return;

		CComPtr<IHTMLTxtRange>	spTxtRange;
		spBody->createTextRange(&spTxtRange);
		if (spTxtRange == nullptr)
			return;

		long nMove;
		spTxtRange->moveStart(strTextedit, -1, &nMove);
		spTxtRange->moveEnd(strTextedit, 1, &nMove);
		VARIANT_BOOL	vResult;
		CComBSTR	strWord = strKeyword;
		while (spTxtRange->findText(strWord, 0, Flags, &vResult), vResult == VARIANT_TRUE) {
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
				++m_nMatchCount;
			}
			long t;
			spTxtRange->moveStart(strChar, 1, &t);
			spTxtRange->moveEnd(strTextedit, 1, &t);
		}
	}

	/* 一致件数を表示 */
	if (strKeyword.GetLength() > 0) {
		if (m_nMatchCount > 0) {
			CString strMsg;
			strMsg.Format(_T(" %d 件の一致"), m_nMatchCount);
			m_static.SetWindowText(strMsg);
			m_bNoHit = false;
		} else {
			m_static.SetWindowText(_T(" 見つかりませんでした"));
			m_bNoHit = true;
		}
	} else {	// からっぽに
		m_static.SetWindowText(_T("\0"));
		m_bNoHit = false;
	}
	m_Edit.Invalidate();
	m_Edit.UpdateWindow();
}

//-----------------------------------
/// ページ内検索
void	CFindBar::Impl::_FindKeyword(bool bFindDown)
{
	CString	strKeyword = MtlGetWindowText(m_Edit);
#if 0
	CString strtemp2;
	int nLength = strKeyword.GetLength() + 1;
	::LCMapString(LOCALE_SYSTEM_DEFAULT, LCMAP_HALFWIDTH, strKeyword, -1, strtemp2.GetBuffer(nLength), nLength);
	strtemp2.ReleaseBuffer();
	strKeyword = strtemp2;
#endif
	if (strKeyword.IsEmpty())
		return;

	long Flags = 0;
	if (m_bWordUnit)
		Flags |= 2;
	if (m_bDistinguish)
		Flags |= 4;

	CChildFrame* pChild = g_pMainWnd->GetActiveChildFrame();
	if (pChild) {
		BOOL bFind = (BOOL)pChild->OnFindKeyWord(strKeyword, bFindDown, Flags);
		if (m_nMatchCount == 0)
			return;	// 一致0ならなにもしない

		if (bFind == FALSE) {
			CString strMsg  = bFindDown ? _T(" ページの最後まで検索しました") : _T(" ページの先頭まで検索しました");
			m_static.SetWindowText(strMsg);
			m_bPageEnd = true;
			m_static.Invalidate();
			m_static.UpdateWindow();
		} else {
			if (m_bPageEnd) {
				m_bPageEnd = false;
				m_static.Invalidate();
				m_static.UpdateWindow();
			}

			if (m_nMatchCount > 0) {
				CString strMsg;
				strMsg.Format(_T(" %d 件の一致"), m_nMatchCount);
				m_static.SetWindowText(strMsg);
			} else {
				m_static.SetWindowText(_T(" 見つかりませんでした"));
			}
		}
	}
}



//==========================================

//CFindBar


CFindBar::CFindBar()
	: pImpl(new Impl)
{	}

CFindBar::~CFindBar()
{
	delete pImpl;
}


HWND	CFindBar::Create(HWND hWndParent)
{
	return pImpl->Create(hWndParent);
}

void	CFindBar::SetUpdateLayoutFunc(function<void (BOOL)> func)
{
	pImpl->SetUpdateLayoutFunc(func);
}

HWND	CFindBar::GetHWND()
{
	return pImpl->m_hWnd;
}

void	CFindBar::ShowFindBar(const CString& strKeyword)
{
	pImpl->ShowFindBar(strKeyword);
}

