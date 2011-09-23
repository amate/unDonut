// DonutAddressBar.cpp

#include "stdafx.h"
#include "DonutAddressBar.h"
#include "HlinkDataObject.h"
#include "DonutSearchBar.h"
#include "Donut.h"
#include "MtlBrowser.h"
#include "option/AddressBarPropertyPage.h"
#include "FlatComboBox.h"

//////////////////////////////////////////////////////////////
// CDonutAddressBar::Impl

struct CDonutAddressBar::Impl :
	public CWindowImpl<CDonutAddressBar::Impl, CComboBoxEx>,
	public IDropTargetImpl<CDonutAddressBar::Impl>,
	public IDropSourceImpl<CDonutAddressBar::Impl>,
	protected CAddressBarOption
{
	DECLARE_WND_SUPERCLASS( _T("Donut_AddressBar"), GetWndClassName() )

	// Constants
	enum { 
		s_kcxHeaderGap = 4,
		s_kcxGap = 2 
	};
	////////////////////////////////////////////////
	// CGoBtnInfo
	struct CGoBtnInfo {
		// Ctor
		CGoBtnInfo(int cx, int cy, COLORREF clrMask, UINT nFlags)
			: _cx(cx)
			, _cy(cy)
			, _clrMask(clrMask)
			, _nFlags(nFlags)
		{ }

		//+++ UINT	_nImageBmpID;
		//+++ UINT	_nHotImageBmpID;
		int 		_cx;
		int 		_cy;
		COLORREF	_clrMask;
		UINT		_nFlags;
	};
	////////////////////////////////////////////////

	// Constructor/Destructor
	Impl();
	~Impl();

	HWND	Create(HWND	hWndParent, UINT nID, UINT nGoBtnCmdID, int cx, int cy, COLORREF clrMask, UINT nFlags = ILC_COLOR24);
	void	InitReBarBandInfo(CReBarCtrl rebar);

	// Attributes
	bool	GetDroppedStateEx() const { return ( GetDroppedState() || _AutoCompleteWindowVisible() ); }
	void	SetFont(HFONT hFont);
	void	ReloadSkin(int nCmbStyle);
	void	ShowAddresText(CReBarCtrl rebar, BOOL bShow);
	void	SetWindowText(LPCTSTR str);
	void	ShowGoButton(bool bShow);
	CString GetAddressBarText();
	CEdit	GetEditCtrl() { return m_edit; }
	void	ReplaceIcon(HICON hIcon);

	void	OnItemSelected(const CString &str);
	void	OnItemSelectedEx(const CString &str);
	void	OnGetItem(const CString &str, COMBOBOXEXITEM &item);

	HRESULT OnGetAddressBarCtrlDataObject(IDataObject **ppDataObject);

	// IDropTargetImpl
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	void		OnDragLeave();
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);

	// Overrides
	BOOL	PreTranslateMessage(MSG *pMsg);

	// Message map and handlers
	BEGIN_MSG_MAP( CDonutAddressBar::Impl )
		MSG_WM_CREATE	( OnCreate	)
		MSG_WM_DESTROY	( OnDestroy )
		MSG_WM_COMMAND	( OnCommand )
		MSG_WM_SIZE 	( OnSize	)
		MSG_WM_SETTEXT	( OnSetText )
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnEraseBackground		)
		MESSAGE_HANDLER ( WM_WINDOWPOSCHANGING, OnWindowPosChanging 	)
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO,	OnToolTipText			)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( CBEN_GETDISPINFO	, OnCbenGetDispInfo )
		REFLECTED_COMMAND_CODE_HANDLER_EX( CBN_EDITCHANGE	, OnCbnEditChange )
		DEFAULT_REFLECTION_HANDLER()
	ALT_MSG_MAP(1)	// ComboBox
		MESSAGE_HANDLER ( WM_WINDOWPOSCHANGING, OnComboWindowPosChanging)
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnComboEraseBackground	)
		MESSAGE_HANDLER ( WM_LBUTTONDOWN,		OnLButtonDown			)
		MESSAGE_HANDLER ( WM_RBUTTONDOWN,		OnRButtonDown			)
		MESSAGE_HANDLER ( WM_LBUTTONDBLCLK, 	OnLButtonDoubleClick	) // UDT DGSTR
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO,	OnToolTipText_for_Edit	)	//+++
	ALT_MSG_MAP(2)	// EditBox
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnEditEraseBackground	)
		MESSAGE_HANDLER ( WM_LBUTTONDBLCLK, 	OnEditLButtonDblClk 	)
		MSG_WM_KEYDOWN	( OnEditKeyDown )
		MSG_WM_RBUTTONUP  ( OnEditRButtonUp )
		MSG_WM_CONTEXTMENU( OnEditContextMenu )
	ALT_MSG_MAP(3)	// GoButton
		MSG_WM_RBUTTONDOWN( OnGoRButtonDown )
	END_MSG_MAP()


	LRESULT OnCreate(LPCREATESTRUCT);
	void	OnDestroy();
	void	OnSize(UINT nFlags, CSize size);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */);
	void	OnCommand(UINT, int nID, HWND hWndCtrl);
	LRESULT OnSetText(LPCTSTR lpszText);
	LRESULT OnEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled);
	LRESULT OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ );
	LRESULT OnCbenGetDispInfo(LPNMHDR pnmh);
	LRESULT OnCbnEditChange(UINT uNotifyCode, int nID, CWindow wndCtl);

	// ComboBox
	LRESULT OnComboEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled);
	LRESULT OnComboWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */ );
	LRESULT OnToolTipText_for_Edit(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ );
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnLButtonDoubleClick(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ );

	// EditBox
	LRESULT OnEditEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled);
	LRESULT OnEditLButtonDblClk(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ );
	void	OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnEditRButtonUp(UINT nFlags, CPoint point);
	void	OnEditContextMenu(CWindow wnd, CPoint point);

	// GoButton
	void	OnGoRButtonDown(UINT nFlags, CPoint point);

private:
	// Implementation
	HWND	_CreateGoButton(int cx, int cy, COLORREF clrMask, UINT nFlags = ILC_COLOR24);
	void	_MoveToTopAddressBox(const CString &strURL);
	void	_OnEnterKeyDown();
	void	_OnEnterKeyDownEx();
	bool	_AutoCompleteWindowVisible() const;
	void	_AddToAddressBoxUnique(const CString &strURL);
	
	int 	_GetGoBtnWidth();
	void	_RefreshBandInfo();
	int 	_CalcBtnWidth(int cxIcon, const CString &strText);
	int 	_CalcBtnHeight(int cyIcon);
	bool	_LoadTypedURLs();
	CString _GetSkinGoBtnPath(BOOL bHot);
	bool	_HitTest(CPoint pt);
	void	_DoDragDrop(CPoint pt, UINT nFlags, bool bLeftButton);
	void	_DrawDragEffect(bool bRemove);
	void	_ComplementURL(CString &strURL);
	bool	_MtlSaveTypedURLs(HWND hWndComboBoxEx);
	void	_SetEditIconIndex(int nIndex);

public:

	// Data members
	CFlatComboBox				m_comboFlat;
	CToolBarCtrl				m_wndGo;
	CGoBtnInfo*					m_pGoBtnInfo;
	CContainedWindow			m_wndCombo;
	CContainedWindowT<CEdit>	m_wndEdit;
	CContainedWindow			m_wndGoButton;
	CEdit						m_edit;

	CFont		m_font;
	HWND		m_hWndParent;		//+++
	UINT		m_nGoBtnCmdID;
	int 		m_cx;
	int 		m_cxGoBtn;
	CSize		m_szGoIcon;
	bool		m_bDragFromItself;
	int 		m_cxDefaultHeader;
	CItemIDList m_idlHtml;			// used to draw .url icon faster
	bool		m_bDragAccept;
	CString		m_strToolTip;
	CImageList	m_FaviconImage;
	int			m_nEditFaviconIndex;
	bool		m_bNowSetWindowText;
	CString		m_strCurrentURL;
};

///////////////////////////////////////////////////////////////////////////////////
// impl
CDonutAddressBar::Impl::Impl() :
	m_nGoBtnCmdID(0),
	m_cxGoBtn(0),
	m_wndCombo(this, 1),
	m_wndEdit(this, 2),
	m_wndGoButton(this, 3),
	m_bDragFromItself(false),
	m_cx(0),
	m_pGoBtnInfo(0),
	m_hWndParent(0),
	m_cxDefaultHeader(0),
	m_bDragAccept(false),
	m_nEditFaviconIndex(0),
	m_bNowSetWindowText(false)
{
	m_idlHtml = MtlGetHtmlFileIDList();
}

CDonutAddressBar::Impl::~Impl()
{ }


HWND CDonutAddressBar::Impl::Create(
	HWND		hWndParent,
	UINT		nID,
	UINT		nGoBtnCmdID,
	int 		cx,
	int 		cy,
	COLORREF	clrMask,
	UINT		nFlags)
{
	m_nGoBtnCmdID	= nGoBtnCmdID;
	m_pGoBtnInfo	= new CGoBtnInfo(cx, cy, clrMask, nFlags);
	m_hWndParent	= hWndParent;		//+++
	return __super::Create(hWndParent, CRect(0,0,500,250), NULL, ABR_PANE_STYLE | CBS_DROPDOWN | CBS_AUTOHSCROLL, 0, nID);
}

void	CDonutAddressBar::Impl::InitReBarBandInfo(CReBarCtrl rebar)
{
	
	int nIndex = rebar.IdToIndex(GetDlgCtrlID());
	CVersional < REBARBANDINFO > rbBand;
	rbBand.fMask	  = RBBIM_HEADERSIZE;
	MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );
	m_cxDefaultHeader = rbBand.cxHeader;

	// Calculate the header of the band
	HFONT		hFont = m_font.m_hFont;

	if (hFont == NULL)
		hFont = rebar.GetFont();

	int 	nWidth = MtlComputeWidthOfText(_T("アドレス"), hFont);			// UDT JOBBY (remove (&D))

	if (s_bTextVisible == false)
		nWidth = 0;
	// ?
	rbBand.cxHeader   = m_cxDefaultHeader + nWidth + s_kcxHeaderGap;
}

// "移動"を表示する
void	CDonutAddressBar::Impl::ShowAddresText(CReBarCtrl rebar, BOOL bShow)
{
	SIZE	size;
	m_wndGo.GetButtonSize(size);

	CString strText;
	if (bShow) {
		m_cxGoBtn = _CalcBtnWidth( m_szGoIcon.cx, _T("移動") );
		m_wndGo.SetButtonSize( CSize(m_cxGoBtn, size.cy) );
		strText = _T("移動");
	} else {
		m_cxGoBtn = _CalcBtnWidth( m_szGoIcon.cx, _T("") );
		m_wndGo.SetButtonSize( CSize(m_cxGoBtn, size.cy) );
	}
	TBBUTTONINFO	bi = { sizeof (TBBUTTONINFO) };
	bi.dwMask  = TBIF_TEXT;
	bi.pszText = strText.GetBuffer(0);
	MTLVERIFY( m_wndGo.SetButtonInfo(m_nGoBtnCmdID, &bi) );


	CRect rect;
	GetWindowRect(rect);
	CWindow( GetParent() ).ScreenToClient(rect);

	SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOREDRAW);		// | SWP_NOACTIVATE);
	SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);							// | SWP_NOACTIVATE);
}

void	CDonutAddressBar::Impl::SetWindowText(LPCTSTR str)
{
	m_bNowSetWindowText = true;
	m_strCurrentURL = str;
	CLockRedraw	lock(m_edit);
	SetImageList(m_FaviconImage);
	SetCurSel(-1);
	//__super::SetWindowText(str);
	m_edit.SetWindowText(str);
	m_edit.SetSel(0, 0);
	if (m_strCurrentURL.IsEmpty())
		_SetEditIconIndex(0);
	m_bNowSetWindowText = false;
}

void	CDonutAddressBar::Impl::ShowGoButton(bool bShow)
{
	s_bGoBtnVisible = bShow;
	if (bShow)
		m_wndGo.ShowWindow(SW_SHOWNORMAL);
	else
		m_wndGo.ShowWindow(SW_HIDE);

	// テキストの表示非表示の変更をメインフレーム通知
	GetTopLevelWindow().SendMessage(WM_USER_SHOW_TEXT_CHG, s_bTextVisible);

	// generate WM_WINDOWPOSCHANGING, no other way
	CRect rect;

	GetWindowRect(rect);
	CWindow( GetParent() ).ScreenToClient(rect);
	SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOREDRAW);	// | SWP_NOACTIVATE);
	SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left	  , rect.bottom - rect.top, SWP_NOZORDER);					// | SWP_NOACTIVATE);
}

void	CDonutAddressBar::Impl::SetFont(HFONT hFont)
{
	__super::SetFont(hFont);

	CRect	rc;
	m_wndCombo.GetWindowRect(rc);
	m_wndCombo.RedrawWindow();

	CSize	size;
	m_wndGo.GetButtonSize(size);
	m_wndGo.SetButtonSize(size.cx, rc.Height());

	_RefreshBandInfo();
}


void	CDonutAddressBar::Impl::ReloadSkin(int nCmbStyle)
{
	m_comboFlat.SetDrawStyle(nCmbStyle);

	if (m_wndGo.m_hWnd) {
		CImageList imgs, imgsHot;

		imgs	= m_wndGo.GetImageList();
		imgsHot = m_wndGo.GetHotImageList();
		_ReplaceImageList(_GetSkinGoBtnPath(FALSE), imgs);
		_ReplaceImageList(_GetSkinGoBtnPath(TRUE), imgsHot);
		m_wndGo.InvalidateRect(NULL, TRUE);
	}
	SetImageList(m_FaviconImage);

	Invalidate(TRUE);
}


// 'h'が抜けていれば補完する
void	CDonutAddressBar::Impl::_ComplementURL(CString &strURL)
{
	if (strURL.Left(6).CompareNoCase(_T("ttp://")) == 0) {
		strURL.Insert(0 ,_T("h"));
	}
}

// strを開く
void	CDonutAddressBar::Impl::OnItemSelected(const CString &str)
{
	DWORD dwOpenFlags = 0;
	if ( !s_bNewWindow )
		dwOpenFlags |= D_OPENFILE_NOCREATE;
	if ( !s_bNoActivate )
		dwOpenFlags |= D_OPENFILE_ACTIVATE;

	DonutToggleOpenFlag(dwOpenFlags);

	DonutOpenFile(str, dwOpenFlags);
}


void	CDonutAddressBar::Impl::OnItemSelectedEx(const CString &str)
{
	DWORD dwOpenFlags = 0;
	if ( !s_bNewWindow )
		dwOpenFlags |= D_OPENFILE_NOCREATE;
	if ( !s_bNoActivate )
		dwOpenFlags |= D_OPENFILE_ACTIVATE;
	if ( !s_bUseEnterCtrl ) {
		if (::GetAsyncKeyState(VK_CONTROL) < 0) {
			if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlags) )
				dwOpenFlags &= ~D_OPENFILE_NOCREATE;
			else
				dwOpenFlags |= D_OPENFILE_NOCREATE;
		}
	}

	if ( !s_bUseEnterShift ) {
		if (::GetAsyncKeyState(VK_SHIFT) < 0) {
			if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlags) )
				dwOpenFlags &= ~D_OPENFILE_NOCREATE;
			else
				dwOpenFlags |= D_OPENFILE_NOCREATE;
		}
	}

	DonutOpenFile(str, dwOpenFlags);
}


void	CDonutAddressBar::Impl::OnGetItem(const CString &str, COMBOBOXEXITEM &item)
{
	item.mask			= CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;	// | CBEIF_OVERLAY;

	int 	nImage; 	// = 2;
	nImage				= I_IMAGECALLBACK;
	item.iImage 		= nImage;
	item.iSelectedImage = nImage;
	item.iOverlay		= nImage;
}


CString CDonutAddressBar::Impl::GetAddressBarText()
{
	return MtlGetWindowText(GetEditCtrl());
}

void	CDonutAddressBar::Impl::ReplaceIcon(HICON hIcon)
{
	m_nEditFaviconIndex = 0;
	if (hIcon) {
		m_FaviconImage.ReplaceIcon(1, hIcon);
		m_nEditFaviconIndex = 1;
	}

	_SetEditIconIndex(m_nEditFaviconIndex);
}

HRESULT CDonutAddressBar::Impl::OnGetAddressBarCtrlDataObject(IDataObject **ppDataObject)
{
	ATLASSERT(ppDataObject != NULL);
	HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL, IID_IDataObject, (void **) ppDataObject);

	if ( FAILED(hr) ) {
		*ppDataObject = NULL;
		return E_NOTIMPL;
	}

	#ifdef _ATL_DEBUG_INTERFACES
	ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
	#endif
	CHlinkDataObject *pHlinkDataObject = NULL;	// this is hack, no need to release

	hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);

	if ( SUCCEEDED(hr) ) {
		HWND		 hWnd	 = DonutGetActiveWindow( GetTopLevelParent() );

		if (hWnd == NULL)
			return E_NOTIMPL;

		ATLASSERT( ::IsWindow(hWnd) );
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if ( browser.IsBrowserNull() )
			return E_NOTIMPL;

		CString 	 strName = MtlGetWindowText(hWnd);
		CString 	 strUrl  = browser.GetLocationURL();

		if ( strUrl.IsEmpty() )
			return E_NOTIMPL;

		if ( strUrl.Left(5) == _T("file:") ) {			// Donut, to be explorer or not
			strName.Empty();
			strUrl = strUrl.Right(strUrl.GetLength() - 8);
			strUrl.Replace( _T('/'), _T('\\') );
		}

		pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(strName, strUrl) );
	}

	return S_OK;
}



// IDropTargetImpl
DROPEFFECT	CDonutAddressBar::Impl::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(false);

	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}


DROPEFFECT	CDonutAddressBar::Impl::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (m_bDragFromItself || !m_bDragAccept)
		return DROPEFFECT_NONE;

	return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
}


void	CDonutAddressBar::Impl::OnDragLeave()
{
	if (m_bDragFromItself)
		return;

	_DrawDragEffect(true);
}



DROPEFFECT	CDonutAddressBar::Impl::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(true);

	CSimpleArray<CString> arrFiles;
	if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
		// one file
		if (arrFiles.GetSize() == 1) {
			OnItemSelected(arrFiles[0]);				// allow to AddressBar style
			return DROPEFFECT_COPY;
		}

		// files
		for (int i = 0; i < arrFiles.GetSize(); ++i) {
			DonutOpenFile(arrFiles[i]);
		}
		return DROPEFFECT_COPY;
	}

	CString strText;
	if (   MtlGetHGlobalText(pDataObject, strText)
		|| MtlGetHGlobalText(pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) ) 
	{
		_ComplementURL(strText);
		OnItemSelected(strText);
		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}



BOOL	CDonutAddressBar::Impl::PreTranslateMessage(MSG *pMsg)
{
	UINT msg  = pMsg->message;
	int  vKey = (int) pMsg->wParam;

	if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
		if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )				// ignore
			return _MTL_TRANSLATE_PASS;

		// left or right pressed, check shift and control key.
		if (vKey == VK_UP || vKey == VK_DOWN || vKey == VK_LEFT || vKey == VK_RIGHT || vKey == VK_HOME || vKey == VK_END
			|| vKey == (0x41 + 'C' - 'A') || vKey == (0x41 + 'V' - 'A') || vKey == (0x41 + 'X' - 'A') || vKey == VK_INSERT)
		{
			if (::GetKeyState(VK_SHIFT) < 0 || ::GetKeyState(VK_CONTROL) < 0)
				return _MTL_TRANSLATE_WANT; 			// pass to edit control
		}

		// return key have to be passed
		if (vKey == VK_RETURN) {
			return _MTL_TRANSLATE_WANT;
		}

		// other key have to be passed
		if (VK_LBUTTON <= vKey && vKey <= VK_HELP) {
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
			if (!bAlt && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)	// not pressed
				return _MTL_TRANSLATE_WANT; 			// pass to edit control
		}
	}

	return _MTL_TRANSLATE_PASS;
}


// Message map

LRESULT	CDonutAddressBar::Impl::OnCreate(LPCREATESTRUCT)
{

	_CreateGoButton(/*+++  m_pGoBtnInfo->_nImageBmpID, m_pGoBtnInfo->_nHotImageBmpID, */
					m_pGoBtnInfo->_cx,
					m_pGoBtnInfo->_cy,
					m_pGoBtnInfo->_clrMask,
					m_pGoBtnInfo->_nFlags);

	delete	m_pGoBtnInfo;
	m_pGoBtnInfo	= NULL; 	//+++	delete後は強制クリア.

	LRESULT	lRet = DefWindowProc();

	SetExtendedStyle(0, CBES_EX_NOSIZELIMIT);

	if ( s_bLoadTypedUrls )
		_LoadTypedURLs();

	ShowGoButton(s_bGoBtnVisible);

	m_edit = __super::GetEditCtrl();
	m_wndCombo.SubclassWindow( GetComboCtrl() );
	m_wndEdit.SubclassWindow( m_edit );
	m_comboFlat.FlatComboBox_Install( GetComboCtrl() );
	m_wndGoButton.SubclassWindow(m_wndGo);

	if ( s_bAutoComplete )
		MtlAutoComplete( m_edit );

	m_edit.SetLimitText(INTERNET_MAX_URL_LENGTH);
	m_comboFlat.LimitText(INTERNET_MAX_URL_LENGTH);

	#if 1	//+++ エディット部分にツールTIPを追加
	{
		CToolTipCtrl	toolTipCtrl;
		toolTipCtrl.Create(m_hWnd/*Parent*/);
		toolTipCtrl.Activate(TRUE);
		CToolInfo	toolInfo(TTF_SUBCLASS, GetEditCtrl());
		toolTipCtrl.AddTool(toolInfo);
	}
	#endif

	m_FaviconImage.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1);
	ATLASSERT(m_FaviconImage.m_hImageList);
	CIcon	icon = AtlLoadIconImage(IDI_GENERIC_DOCUMENT, LR_DEFAULTCOLOR, 16, 16);
	ATLASSERT(icon.m_hIcon);
	m_FaviconImage.AddIcon(icon);
	m_FaviconImage.AddIcon(icon);
	SetImageList(m_FaviconImage);

	_SetEditIconIndex(I_IMAGECALLBACK);

	RegisterDragDrop();

	return lRet;
}


void	CDonutAddressBar::Impl::OnDestroy()
{
	SaveProfile();

	MtlDestroyImageLists(m_wndGo);

	if (s_bLoadTypedUrls)
		_MtlSaveTypedURLs(m_hWnd);

	m_wndCombo.UnsubclassWindow();
	m_wndEdit.UnsubclassWindow();
	m_wndGoButton.UnsubclassWindow();

	RevokeDragDrop();
}

void	CDonutAddressBar::Impl::OnSize(UINT nFlags, CSize size)
{
	/* Goボタンをコンボボックスの横に移動する */
	int   cxGo = _GetGoBtnWidth();
	CRect rc;

	GetClientRect(rc);
	m_wndGo.MoveWindow( rc.right - cxGo + s_kcxGap, 0, cxGo, rc.Height() );
	InvalidateRect( CRect( rc.right - cxGo, 0, rc.right - cxGo + s_kcxGap, rc.Height() ) );
}


LRESULT	CDonutAddressBar::Impl::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */ )
{
	LPWINDOWPOS lpWP = (LPWINDOWPOS) lParam;

	m_cx = lpWP->cx;

	LRESULT 	lRet = DefWindowProc(uMsg, wParam, lParam);
	if (m_wndGo.m_hWnd) {
		CSize size;

		m_wndGo.GetButtonSize(size);
		/* Goボタンの方が大きければ高さはGoボタンに合わせる */
		if (lpWP->cy < size.cy) {
			lpWP->cy = size.cy;
		}
	}

	return lRet;
}


void	CDonutAddressBar::Impl::OnCommand(UINT, int nID, HWND hWndCtrl)
{
	if (hWndCtrl == m_wndGo.m_hWnd) {
		ATLASSERT(nID == m_nGoBtnCmdID);
		CString str  = GetAddressBarText();
		if (str.IsEmpty() == FALSE) 
			OnItemSelected(str);

	} else {
		SetMsgHandled(FALSE);
	}
}


LRESULT	CDonutAddressBar::Impl::OnSetText(LPCTSTR lpszText)
{
	CString		strText(lpszText);
	if (GetAddressBarText() == strText)
		return TRUE;

	COMBOBOXEXITEM	item = { 0 };
	// Strangely, Icon on edit control never be changed... tell me why?
	//		pT->OnGetItem(lpszText, item);
	item.mask	 = CBEIF_TEXT;
	item.iItem	 = -1;							// on edit control
	item.pszText = (LPTSTR) (LPCTSTR) strText;
	MTLVERIFY( SetItem(&item) );

	return TRUE;
}

LRESULT	CDonutAddressBar::Impl::OnCbenGetDispInfo(LPNMHDR lpnmhdr)
{
	PNMCOMBOBOXEX pDispInfo = (PNMCOMBOBOXEX) lpnmhdr;
	COMBOBOXEXITEM&	item = pDispInfo->ceItem;
	if ( !_check_flag(CBEIF_IMAGE, item.mask) && !_check_flag(CBEIF_SELECTEDIMAGE, item.mask) )
		return 0;
#if 0
	CString 	str;
	if ( GetDroppedState() && item.iItem != -1) {
		GetComboCtrl().GetLBText((int)item.iItem, str);
	} else {
		str = GetAddressBarText();
	}
#endif
	if (item.iItem == -1) {	// Edit
		item.iImage 		= m_nEditFaviconIndex;
		item.iSelectedImage = m_nEditFaviconIndex;
	} else {
		item.iImage			= 0;
		item.iSelectedImage	= 0;
	}
	return 0;
#if 0
	CItemIDList idl = str;
	if ( idl.IsNull() ) {		// invalid idl
		int iImage = MtlGetSystemIconIndex(m_idlHtml);

		item.iImage 		= iImage;
		item.iSelectedImage = iImage;
		return 0;
	}

	if (item.mask & CBEIF_IMAGE) {
		item.iImage = MtlGetNormalIconIndex(idl, m_idlHtml);
	}

	if (item.mask & CBEIF_SELECTEDIMAGE) {
		item.iSelectedImage = MtlGetSelectedIconIndex(idl, true, m_idlHtml);
	}

	return 0;
#endif
}


LRESULT CDonutAddressBar::Impl::OnCbnEditChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_bNowSetWindowText)
		return 0;
	_SetEditIconIndex(0);
	return 0;
}

// リバーから背景をコピーする
LRESULT	CDonutAddressBar::Impl::OnEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
{
	HWND	hWnd = GetParent();
	CPoint	pt;
	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
	::SetWindowOrgEx( (HDC) wParam, 0, 0, NULL );

	return lResult;
}


// ComboBox

LRESULT	CDonutAddressBar::Impl::OnComboEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
{
	return 1;			// don't do the default erase
}


LRESULT	CDonutAddressBar::Impl::OnComboWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */ )
{
	LRESULT 	lRet = m_wndCombo.DefWindowProc(uMsg, wParam, lParam);
	LPWINDOWPOS lpWP = (LPWINDOWPOS) lParam;
	lpWP->cx = m_cx - _GetGoBtnWidth();
	return lRet;
}


LRESULT	CDonutAddressBar::Impl::OnEditEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
{
	return 1;	// don't do the default erase
}


// エディットボックスにあるテキストを全選択する
LRESULT	CDonutAddressBar::Impl::OnEditLButtonDblClk(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ )
{	// fixed for Win2000 by DOGSTORE
	m_wndEdit.SetSel(0, -1);
	return 0;
}


void	CDonutAddressBar::Impl::OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// if not dropped, eat VK_DOWN
	if ( !GetDroppedState() && !_AutoCompleteWindowVisible() && (nChar == VK_DOWN || nChar == VK_UP) ) {
		SetMsgHandled(TRUE);
	} else if (nChar == VK_RETURN) {
		if (::GetKeyState(VK_CONTROL) < 0 || ::GetKeyState(VK_SHIFT) < 0) {
			_OnEnterKeyDownEx();
		} else {
			_OnEnterKeyDown();
		}

		SetMsgHandled(TRUE);
	} else {
		SetMsgHandled(FALSE);
	}
}

void	CDonutAddressBar::Impl::OnEditRButtonUp(UINT nFlags, CPoint point)
{
	if (::GetFocus() != m_edit.m_hWnd) {
		m_edit.SetFocus();
		m_edit.SetSelAll();
	}
	SetMsgHandled(FALSE);
}


void	CDonutAddressBar::Impl::OnEditContextMenu(CWindow wnd, CPoint point)
{
	enum {
		ID_UNDO			= 1,
		ID_CUT			= 2,
		ID_COPY			= 3,
		ID_PASTE		= 4,
		ID_PASTEANDMOVE	= 5,
		ID_CLEAR		= 6,
		ID_SELALL		= 7,
	};

	UINT nFlags = 0;
	CMenu menu;
	menu.CreatePopupMenu();
	{
		nFlags = m_edit.CanUndo() ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_UNDO		  , _T("元に戻す(&U)"));
	}
	menu.AppendMenu(MF_SEPARATOR);

	int nStart, nEnd;
	m_edit.GetSel(nStart, nEnd);
	{
		nFlags = (nStart == nEnd) ? MFS_DISABLED : 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_CUT		  , _T("切り取り(&T)"));
	}
	{
		nFlags = (nStart < nEnd) ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_COPY		  , _T("コピー(&C)"));
	}
	{
		nFlags = MtlGetClipboardText().IsEmpty() ? MFS_DISABLED : 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_PASTE		 , _T("貼り付け(&P)"));
		menu.AppendMenu(nFlags, (UINT_PTR)ID_PASTEANDMOVE, _T("貼り付けて移動(&M)"));
	}
	{
		nFlags = (nStart < nEnd) ? 0 : MFS_DISABLED;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_CLEAR		 , _T("削除(&D)"));
	}
	menu.AppendMenu(MF_SEPARATOR);
	{
		if (nStart == 0 && nEnd == MtlGetWindowText(m_edit).GetLength())
			nFlags = MFS_DISABLED;
		else
			nFlags = 0;
		menu.AppendMenu(nFlags, (UINT_PTR)ID_SELALL		 , _T("すべて選択(&A)"));
	}

	int nRet = menu.TrackPopupMenu(TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, m_hWnd);
	if (nRet == 0)
		return;

	switch (nRet) {
	case ID_UNDO:		
		m_edit.Undo();
		break;

	case ID_CUT:			
		m_edit.Cut();
		break;

	case ID_COPY:	
		m_edit.Copy();
		break;

	case ID_PASTE:		
		m_edit.Paste();
		break;

	case ID_PASTEANDMOVE:	
		OnGoRButtonDown(0, CPoint());
		SetMsgHandled(TRUE);
		break;

	case ID_CLEAR:		
		m_edit.Clear();
		break;

	case ID_SELALL:		
		m_edit.SetSelAll();
		break;
	}
}

// GoButton
// [Go]ボタンを右クリックでクリップボードにあるURLに移動する
void	CDonutAddressBar::Impl::OnGoRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	CString strURL = MtlGetClipboardText();
	if (strURL.IsEmpty() == FALSE) {
		_ComplementURL(strURL);
		OnItemSelected(strURL);
	}
}


LRESULT	CDonutAddressBar::Impl::OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ )
{
	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;
	if ( (idCtrl != 0) && !(pDispInfo->uFlags & TTF_IDISHWND) ) {
		CString strURL = GetAddressBarText();
		if ( strURL.IsEmpty() )
			return 0;

		m_strToolTip.Format(_T("\"%s\" へ移動"), strURL);
		pDispInfo->lpszText = m_strToolTip.GetBuffer(0);
	}

	return 0;
}


///+++ アドレス編集バーでのツールチップ表示.
LRESULT	CDonutAddressBar::Impl::OnToolTipText_for_Edit(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ )
{
	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

	m_strToolTip = GetAddressBarText();
	if ( m_strToolTip.IsEmpty() )
		return 0;
	if (m_strToolTip.Find(_T('%')) >= 0)
		m_strToolTip = Misc::urlstr_decodeJpn(m_strToolTip);

	pDispInfo->lpszText = m_strToolTip.GetBuffer(0);

	return 0;
}


LRESULT	CDonutAddressBar::Impl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (wParam & MK_CONTROL) {
		bHandled = FALSE;
		return 0;
	}

	if ( _HitTest(pt) ) {
		_DoDragDrop(pt, (UINT) wParam, true);
	} else {
		#if 0 //+++ キャプション無しモードの時に、窓を動かせるようにするための処理... やめ
		Donut_FakeCaptionLButtonDown(m_hWnd, GetTopLevelWindow(), lParam);
		#endif
		bHandled = FALSE;
	}
	return 0;
}


LRESULT	CDonutAddressBar::Impl::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (wParam & MK_CONTROL) {
		bHandled = FALSE;
		return 0;
	}

	if ( _HitTest(pt) ) {
		_DoDragDrop(pt, (UINT) wParam, true);
	} else
		bHandled = FALSE;

	return 0;
}

// コンボボックスがダブルクリックされた
LRESULT	CDonutAddressBar::Impl::OnLButtonDoubleClick(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ )
{
	CString strUrl	= GetAddressBarText();
	if ( strUrl.IsEmpty() )
		return 0;

	//+++ IEの代わりに他のexeを実行可能にした.
	CString 	strIeExePath = s_strIeExePath;
	if (strIeExePath.IsEmpty() == 0)	//+++ 何か設定されていたら、フルパス化しておく.
		strIeExePath = Misc::GetFullPath_ForExe( strIeExePath );

	if (strIeExePath.IsEmpty() || ::PathFileExists( strIeExePath ) == FALSE) {
		TCHAR	szIEPath[MAX_PATH] = _T("\0");
		DWORD	dwCount = MAX_PATH;	//+++ * sizeof (TCHAR);
		Misc::CRegKey 	rk;
		LONG	lRet	= rk.Open( HKEY_LOCAL_MACHINE
								, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE")
								, KEY_QUERY_VALUE);
		if (lRet == ERROR_SUCCESS) {
			rk.QueryStringValue(NULL, szIEPath, &dwCount);
			//ShellExecute(NULL, _T("open"), szIEPath, strUrl, NULL, SW_SHOW);
			strIeExePath = szIEPath;		//+++
			rk.Close();
		}
	}
	if (strIeExePath.IsEmpty() == FALSE)	//+++
		::ShellExecute(NULL, _T("open"), strIeExePath, strUrl, NULL, SW_SHOW);

	return 0;
}


// Implementation


void	CDonutAddressBar::Impl::_MoveToTopAddressBox(const CString &strURL)
{
	COMBOBOXEXITEM item;

	OnGetItem(strURL, item);
	item.iItem	 = 0;							// add to top
	item.pszText = (LPTSTR) (LPCTSTR) strURL;

	// search the same string
	int 		   nCount = GetCount();

	for (int n = 0; n < nCount; ++n) {
		CString str;

		MtlGetLBTextFixed(m_hWnd, n, str);

		if (strURL == str) {
			DeleteItem(n);
			InsertItem(&item);
			break;
		}
	}
}


// Enterキーが押された
void	CDonutAddressBar::Impl::_OnEnterKeyDown()
{
	CString str  = GetAddressBarText();
	if ( str.IsEmpty() == FALSE ) {
		if ( _AutoCompleteWindowVisible() ) { 	// I have to clean auto complete window, but escape means undo...
			::SendMessage(GetEditCtrl(), WM_KEYDOWN, (WPARAM) VK_ESCAPE, 0);
		}
		_ComplementURL(str);
		OnItemSelected(str);
		_AddToAddressBoxUnique(str);
	}
}



void	CDonutAddressBar::Impl::_OnEnterKeyDownEx()
{
	CEdit		edit	= GetEditCtrl();
	CString 	str 	= MtlGetWindowText(edit);									//some comboboxex control impl often not support GetWindowText

	if ( str.IsEmpty() )
		return;

	if ( s_bReplaceSpace )			//minit
		str.Replace( _T("　"), _T(" ") );

	CIniFileI	pr( g_szIniFileName, _T("AddressBar") );
	CString 	strEnterCtrl  = pr.GetStringUW(_T("EnterCtrlEngin"));
	CString 	strEnterShift = pr.GetStringUW(_T("EnterShiftEngin"));
	pr.Close();

	if (::GetKeyState(VK_CONTROL) < 0) {
		if ( !s_bUseEnterCtrl ) {
			_OnEnterKeyDown();
			return;
		}

		// Ctrl時
		::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) str, (LPARAM) (LPCTSTR) strEnterCtrl);
		return;

	} else if (::GetKeyState(VK_SHIFT) < 0) {
		if ( !s_bUseEnterShift ) {
			_OnEnterKeyDown();
			return;
		}
		// Shift時
		::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) str, (LPARAM) (LPCTSTR) strEnterShift);
		return;
	}

	if ( _AutoCompleteWindowVisible() ) 		// I have to clean auto complete window, but escape means undo...
		::SendMessage(GetEditCtrl(), WM_KEYDOWN, (WPARAM) VK_ESCAPE, 0);

	OnItemSelectedEx(str);

	_AddToAddressBoxUnique(str);
}



bool	CDonutAddressBar::Impl::_AutoCompleteWindowVisible() const
{
	CRect  rc;

	m_wndEdit.GetWindowRect(&rc);
	CPoint pt(rc.left + 1, rc.bottom + 5);
	HWND   hWndDropDown = ::WindowFromPoint(pt);

	if	( MtlIsFamily(m_hWnd, hWndDropDown) )
		return false;
	else
		return true;
}




void	CDonutAddressBar::Impl::_AddToAddressBoxUnique(const CString &strURL)
{
	COMBOBOXEXITEM item;

	OnGetItem(strURL, item);
	item.iItem	 = 0;							// ado to top
	item.pszText = (LPTSTR) (LPCTSTR) strURL;

	// search the same string
	int 		   nCount = GetCount();

	for (int n = 0; n < nCount; ++n) {
		CString str;

		MtlGetLBTextFixed(m_hWnd, n, str);

		if (strURL == str) {
			DeleteItem(n);
			break;
		}
	}

	InsertItem(&item);
}



int	CDonutAddressBar::Impl::_GetGoBtnWidth()
{
	if ( !m_wndGo.m_hWnd || !s_bGoBtnVisible ) {
		return 0;
	}

	return m_cxGoBtn + s_kcxGap + 1;
}


void	CDonutAddressBar::Impl::_RefreshBandInfo()
{
	HWND		  hWndReBar = GetParent();
	CReBarCtrl	  rebar(hWndReBar);
	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	rbBand.fMask  = RBBIM_CHILDSIZE;

	int nIndex	= rebar.IdToIndex( GetDlgCtrlID() );
	rebar.GetBandInfo(nIndex, &rbBand);

	CRect	rc;
	m_wndCombo.GetWindowRect(rc);
	if ( rbBand.cyMinChild != rc.Height() ) {
		// Calculate the size of the band
		rbBand.cxMinChild = 0;
		rbBand.cyMinChild = rc.Height();

		rebar.SetBandInfo(nIndex, &rbBand);
	}
}

//public:

/// Strangly GetButtonInfo lies...

int	CDonutAddressBar::Impl::_CalcBtnWidth(int cxIcon, const CString &strText)
{
	const int cxGap  = 3;

	if ( strText.IsEmpty() )
		return cxGap * 2 + cxIcon;

	int 	  nWidth = 0;
	nWidth += cxGap * 2;
	nWidth += cxIcon;
	nWidth += cxGap * 2 - 1;
	nWidth += MtlComputeWidthOfText( strText, m_wndGo.GetFont() );
	// nWidth += cxGap*2;

	return nWidth;
}


//private:

int	CDonutAddressBar::Impl::_CalcBtnHeight(int cyIcon)
{
	const int		cyGap = 3;
	MTL::CLogFont	lf;
	CFontHandle( m_wndGo.GetFont() ).GetLogFont(&lf);
	int 	  cy	= lf.lfHeight;
	if (cy < 0)
		cy = -cy;

	return std::max(cy, cyIcon + cyGap * 2);
}


bool	CDonutAddressBar::Impl::_LoadTypedURLs()
{
	Misc::CRegKey 	rk;
	LONG	lRet = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\TypedURLs") );

	if (lRet != ERROR_SUCCESS)
		return false;

	std::list<CString>	urls;
	if ( !MtlGetProfileString(rk, std::back_inserter(urls), _T("url"), 1, 25) ) 		// get only 25 items
		return false;

	std::list<CString>::iterator	it;

	for (it = urls.begin(); it != urls.end(); ++it) {
		CString 	   strUrl = *it;
		COMBOBOXEXITEM item;

		OnGetItem(strUrl, item);
		item.iItem		= -1;						// add to tail
		item.pszText	= (LPTSTR) (LPCTSTR)strUrl;
		InsertItem(&item);
	}

	return true;
}


CString	CDonutAddressBar::Impl::_GetSkinGoBtnPath(BOOL bHot)
{
	LPCTSTR	pTmp;
	if (bHot) {
		pTmp = _T("GoHot.bmp");
	} else {
		pTmp = _T("Go.bmp");
	}

	return _GetSkinDir() + pTmp;
}


HWND	CDonutAddressBar::Impl::_CreateGoButton(int cx, int cy, COLORREF clrMask, UINT nFlags)
{
	m_szGoIcon.cx = cx;
	m_szGoIcon.cy = cy;

	/* 移動ボタン(ツールバー)を作成 */
	m_wndGo.Create(m_hWnd, CRect(0, 0, 100, 100), _T("GoButton"), ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST | CCS_TOP, 0, 12);
	m_wndGo.SetButtonStructSize();
	// フォント
	if (m_font.m_hFont)
		m_wndGo.SetFont(m_font.m_hFont);

	// init button size
	if ( s_bTextVisible ) 
		m_cxGoBtn = _CalcBtnWidth( cx, _T("移動") );
	 else 
		m_cxGoBtn = _CalcBtnWidth( cx, _T("") );

	m_wndGo.SetButtonSize( CSize( m_cxGoBtn, _CalcBtnHeight(cy) ) );

	{
		CImageList imgs;
		MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmp;
		bmp.Attach( AtlLoadBitmapImage(static_cast<LPCTSTR>(_GetSkinGoBtnPath(FALSE)), LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL)
			bmp.LoadBitmap(IDB_GOBUTTON/*nImageBmpID*/);		//+++ skin設定がここでやってるので、この場で直接デフォルト設定.
		imgs.Add(bmp, clrMask);
		m_wndGo.SetImageList(imgs);
	}
	{
		CImageList imgsHot;
		MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmpHot;
		bmpHot.Attach( AtlLoadBitmapImage(static_cast<LPCTSTR>(_GetSkinGoBtnPath(TRUE)), LR_LOADFROMFILE) );
		if (bmpHot.m_hBitmap == NULL)
			bmpHot.LoadBitmap(IDB_GOBUTTON_HOT/*nHotImageBmpID*/);		//+++ skin設定がここでやってるので、この場で直接デフォルト設定.

		imgsHot.Add(bmpHot, clrMask);
		m_wndGo.SetHotImageList(imgsHot);
	}

	// one button
	TBBUTTON	 btn = { 0 };//, m_nGoBtnCmdID, TBSTATE_ENABLED, TBSTYLE_BUTTON /* | TBSTYLE_AUTOSIZE */ , 0, 0 };
	btn.idCommand	= m_nGoBtnCmdID;
	btn.fsState		= TBSTATE_ENABLED;
	btn.fsStyle		= TBSTYLE_BUTTON;
	MTLVERIFY( m_wndGo.AddButton(&btn) );	// ボタンを追加

	if ( s_bTextVisible ) {
		TBBUTTONINFO	bi = { sizeof (TBBUTTONINFO) };
		bi.dwMask  = TBIF_TEXT;
		bi.pszText = _T("移動");
		MTLVERIFY( m_wndGo.SetButtonInfo(m_nGoBtnCmdID, &bi) );
	}
	//m_wndGo.AddStrings( _T("NS\0") );			// for proper item height

	return m_wndGo.m_hWnd;
}


// アイコンの部分にHitするかどうか
bool	CDonutAddressBar::Impl::_HitTest(CPoint pt)
{
	CRect rc;
	m_wndCombo.GetClientRect(rc);

	CRect rcEdit;
	m_wndEdit.GetWindowRect(&rcEdit);
	m_wndCombo.ScreenToClient(&rcEdit);

	rc.right = rcEdit.left;

	#if 1	//+++
	return rc.PtInRect(pt) != 0;
	#else
	if ( rc.PtInRect(pt) )
		return true;
	else
		return false;
	#endif
}


void	CDonutAddressBar::Impl::_DoDragDrop(CPoint pt, UINT nFlags, bool bLeftButton)
{
	if ( PreDoDragDrop(m_hWnd, NULL, false) ) { 	// now dragging
		CComPtr<IDataObject> spDataObject;
		HRESULT hr = OnGetAddressBarCtrlDataObject(&spDataObject);

		if ( SUCCEEDED(hr) ) {
			m_bDragFromItself = true;
			DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
			m_bDragFromItself = false;
		}
	} else {										// canceled
		if (bLeftButton) {
			SetWindowText(m_strCurrentURL);	// 現在表示中のURLを復元する
			if (m_strCurrentURL.IsEmpty() == FALSE)
				_SetEditIconIndex(m_nEditFaviconIndex);
		}
		//} else {	//\\?
		//	m_wndGo.SendMessage( WM_RBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
		//}
	}
}


void	CDonutAddressBar::Impl::_DrawDragEffect(bool bRemove)
{
	CClientDC dc(m_wndCombo.m_hWnd);

	CRect	  rect;
	m_wndCombo.GetClientRect(rect);

	if (bRemove) {
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
	} else {
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL, CSize(2, 2), NULL, NULL);
	}
}


bool CDonutAddressBar::Impl::_MtlSaveTypedURLs(HWND hWndComboBoxEx)
{
	Misc::CRegKey 	rk;
	LONG			lRet  = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\TypedURLs") );
	if (lRet != ERROR_SUCCESS)
		return false;

	std::list<CString>	urls;

	CComboBoxEx combo(hWndComboBoxEx);
	int nCount = combo.GetCount();
	if (nCount == 0)
		return false;
		
	if (nCount > 25)
		nCount = 25;

	for (int n = 0; n < nCount; ++n) {
		CString str;
		if (MtlGetLBTextFixed(combo, n, str) != CB_ERR)
			urls.push_back(str);
	}
	if (urls.size() == 0)
		return false;

	MtlWriteProfileString(urls.begin(), urls.end(), rk, _T("url"), 1);

	return true;
}

//--------------------------------
/// 左端のアイコンを設定
void	CDonutAddressBar::Impl::_SetEditIconIndex(int nIndex)
{
	COMBOBOXEXITEM     cbi = {0};
	cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
	cbi.iItem			= -1;
	cbi.iImage			= nIndex;
	cbi.iSelectedImage	= nIndex;
	SetItem(&cbi);
}



//////////////////////////////////////////////////////////////////////////////////////////
// CDonutAddressBar

CDonutAddressBar*	CDonutAddressBar::s_pThis = NULL;

// Constructor
CDonutAddressBar::CDonutAddressBar()
	: pImpl(new Impl)
{
	s_pThis = this;
}

CDonutAddressBar::~CDonutAddressBar()
{
	delete pImpl;
}

CDonutAddressBar* CDonutAddressBar::GetInstance()
{
	return s_pThis;
}

HWND	CDonutAddressBar::Create(HWND hWndParent, UINT nID, UINT nGoBtnCmdID, int cx, int cy, COLORREF clrMask, UINT nFlags)
{
	return pImpl->Create(hWndParent, nID, nGoBtnCmdID, cx, cy, clrMask, nFlags);
}


void	CDonutAddressBar::InitReBarBandInfo(CReBarCtrl rebar)
{
	pImpl->InitReBarBandInfo(rebar);
}


////////////////////////////////////////////////////////////////
// Attribute
void	CDonutAddressBar::SetFont(HFONT hFont)
{
	pImpl->SetFont(hFont);
}

void	CDonutAddressBar::ReloadSkin(int nCmbStyle)
{
	pImpl->ReloadSkin(nCmbStyle);
}

CString CDonutAddressBar::GetAddressBarText()
{
	return s_pThis->pImpl->GetAddressBarText();
}

bool	CDonutAddressBar::GetDroppedStateEx() const
{
	return pImpl->GetDroppedStateEx();
}

CEdit	CDonutAddressBar::GetEditCtrl()
{
	return pImpl->GetEditCtrl();
}

void	CDonutAddressBar::ReplaceIcon(HICON hIcon)
{
	pImpl->ReplaceIcon(hIcon);
}

BOOL	CDonutAddressBar::IsWindow() const
{
	return pImpl->IsWindow();
}

/////////////////////////////////////////////////////////////////
// Oparation
void	CDonutAddressBar::ShowDropDown(BOOL bShow)
{
	pImpl->ShowDropDown(bShow);
}

void	CDonutAddressBar::SetWindowText(LPCTSTR str)
{
	pImpl->SetWindowText(str);
}

void	CDonutAddressBar::SetFocus()
{
	pImpl->SetFocus();
}

void	CDonutAddressBar::ShowGoButton(bool bShow)
{
	pImpl->ShowGoButton(bShow);
}

void	CDonutAddressBar::ShowAddresText(CReBarCtrl rebar, BOOL bShow)
{
	pImpl->ShowAddresText(rebar, bShow);
}


BOOL	CDonutAddressBar::PreTranslateMessage(MSG *pMsg)
{
	return pImpl->PreTranslateMessage(pMsg);
}

// Message map
BEGIN_MSG_MAP_EX_impl( CDonutAddressBar )
	CHAIN_MSG_MAP_MEMBER((*pImpl))
END_MSG_MAP()













