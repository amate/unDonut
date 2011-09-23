/**
 *	@file	DonutClipbordBar.cpp
 *	@brief	クリップボード・バー
 */

#include "stdafx.h"
#include "DonutClipboardBar.h"
#include "Donut.h"
#include "DonutPFunc.h"
#include "option/MainOption.h"



// Ctor
CDonutClipboardBar::CDonutClipboardBar()
	: m_dwExStyle(0)
  #if 1 //+++
	, m_strExts()
	, m_nOn(0)
	, m_nDirect(0)
	, m_nFlush(0)
	, m_edit()
  #endif
	, m_box(this, 1)
{
}



// Methods

CString CDonutClipboardBar::GetExtsList()
{
	return MtlGetWindowText(m_edit);
}



void CDonutClipboardBar::OpenClipboardUrl()
{
	CString 	strText = MtlGetClipboardText();
	if ( strText.IsEmpty() )
		return;

	CSimpleArray<CString>	arrExt;
	MtlBuildExtArray( arrExt, GetExtsList() );

	CSimpleArray<CString>	arrUrl;
	MtlBuildUrlArray(arrUrl, arrExt, strText);

	for (int i = 0; i < arrUrl.GetSize(); ++i) {
		DonutOpenFile(arrUrl[i], 0);
	}
}



// Overrides

void CDonutClipboardBar::OnUpdateClipboard()
{
	if (CMainOption::s_bIgnoreUpdateClipboard)
		return;

	clbTRACE( _T("OnUpdateClipboard\n") );
	CString 	strText = MtlGetClipboardText();
	if ( strText.IsEmpty() )
		return;

	CWebBrowser2	browser = DonutGetIWebBrowser2( GetTopLevelParent() );
	if ( !browser.IsBrowserNull() ) {
		CString strUrl = browser.GetLocationURL();

		if (strUrl == strText)
			return;
	}

	if ( _check_flag(CLPV_EX_FLUSH, m_dwExStyle) )
		m_box.ResetContent();

	CSimpleArray<CString>	arrExt;
	MtlBuildExtArray( arrExt, MtlGetWindowText(m_edit) );

	CSimpleArray<CString>	arrUrl;
	MtlBuildUrlArray(arrUrl, arrExt, strText);

	if (arrUrl.GetSize() == 0) {
		return;
	} else {
		if ( _check_flag(CLPV_EX_FLUSH, m_dwExStyle) )
			m_box.ResetContent();
	}

	for (int i = 0; i < arrUrl.GetSize(); ++i) {
		if (m_box.GetCount() == 0)
			m_box.AddString(arrUrl[i]);
		else
			m_box.InsertString(0, arrUrl[i]);
	}

	if ( _check_flag(CLPV_EX_DIRECT, m_dwExStyle) ) {
		for (int i = 0; i < arrUrl.GetSize(); ++i) {
			DonutOpenFile(arrUrl[i], 0);
		}
	}
}



// Methods
BYTE CDonutClipboardBar::PreTranslateMessage(MSG *pMsg)
{
	UINT	msg  = pMsg->message;
	//x int vKey =	pMsg->wParam;
	HWND	hWnd = pMsg->hwnd;

	if (m_hWnd == NULL)
		return _MTL_TRANSLATE_PASS;

  #if 1 //+++ 起動直後で、無項で、クリップボードバーを表示しているとき、
		//	  F4押してもオプションメニューが起動できない件の暫定対処.
		//	  よくわからずなんとなくの修正なんで、問題があるかも...
		//	  ※ 何かWeb頁表示してたり、一旦、お気に入りバー等を表示したあとだとokになる状態で、
		//	  駄目なときは、下のIsDialogMessage(pMsg)が真を返していた。
		//	  ここでなく他の箇所でのバグな気もするし、そもそも現状のundonutで、
		//	  ここでIsDialogMessage()をする必要があるのかもわかってないけれど。
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		if (hWnd != m_edit.m_hWnd)
			return _MTL_TRANSLATE_PASS;
	}
  #else
	if (msg == WM_KEYDOWN) {
		if (hWnd != m_edit.m_hWnd && ::GetKeyState(VK_CONTROL) < 0) 	// may be accelerator
			return _MTL_TRANSLATE_PASS;
	} else if (msg == WM_SYSKEYDOWN) {
		return _MTL_TRANSLATE_PASS;
	}
  #endif
	if ( IsDialogMessage(pMsg) )										// is very smart.
		return _MTL_TRANSLATE_HANDLE;

	return _MTL_TRANSLATE_PASS;
}



LRESULT CDonutClipboardBar::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	DlgResize_Init(false, true, WS_CLIPCHILDREN);

	m_edit.Attach( GetDlgItem(IDC_EDIT_CB_EXT) );
	m_box.SubclassWindow( GetDlgItem(IDC_LIST_CB_CONT) );

	_GetProfile();
	_SetData();
	DoDataExchange(FALSE);

	return TRUE;
}



LRESULT CDonutClipboardBar::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	_GetData();
	_WriteProfile();
	return 0;
}



LRESULT CDonutClipboardBar::OnCheckCommand(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (wNotifyCode == BN_CLICKED) {
		DoDataExchange(TRUE);
		_GetData();
	}

	return 0;
}



LRESULT CDonutClipboardBar::OnButtonDelete(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	_MtlForEachListBoxSelectedItem( m_box.m_hWnd, _Function_DeleteString() );

	return 0;
}



LRESULT CDonutClipboardBar::OnButtonOpen(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	_MtlForEachListBoxSelectedItem( m_box.m_hWnd, _Function_Open() );

	return 0;
}



LRESULT CDonutClipboardBar::OnButtonPaste(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	OnUpdateClipboard();

	return 0;
}



LRESULT CDonutClipboardBar::OnButtonPasteDonut(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	OpenClipboardUrl();

	return 0;
}



LRESULT CDonutClipboardBar::OnButtonDeleteAll(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_box.ResetContent();

	return 0;
}



LRESULT CDonutClipboardBar::OnIdOk(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (::GetFocus() == m_box.m_hWnd)
		_MtlForEachListBoxSelectedItem( m_box.m_hWnd, _Function_Open() );

	return 0;
}



LRESULT CDonutClipboardBar::OnLBLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
{
	CPoint pt( LOWORD(lParam), HIWORD(lParam) );

	BOOL   bOutside;
	UINT   nIndex = m_box.ItemFromPoint(pt, bOutside);

	if (!bOutside)
		_OnItemOpen(nIndex);

	return 0;
}



// Implementation
void CDonutClipboardBar::_GetData()
{
	// update style
	m_dwExStyle = 0;

	if (m_nOn /*== 1*/)
		m_dwExStyle |= CLPV_EX_ON;

	if (m_nDirect /*== 1*/)
		m_dwExStyle |= CLPV_EX_DIRECT;

	if (m_nFlush /*== 1*/)
		m_dwExStyle |= CLPV_EX_FLUSH;

	if ( _check_flag(CLPV_EX_ON, m_dwExStyle) )
		InstallClipboardViewer();
	else
		UninstallClipboardViewer();
}



void CDonutClipboardBar::_SetData()
{
	m_nOn	  = _check_flag(CLPV_EX_ON	  , m_dwExStyle);	//+++ ? 1 : 0;
	m_nDirect = _check_flag(CLPV_EX_DIRECT, m_dwExStyle);	//+++ ? 1 : 0;
	m_nFlush  = _check_flag(CLPV_EX_FLUSH , m_dwExStyle);	//+++ ? 1 : 0;
}



void CDonutClipboardBar::_GetProfile()
{
	CIniFileI	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("ClipboardBar") );
	pr.QueryValue( m_dwExStyle, _T("Extended_Style") );

	m_strExts = pr.GetString( _T("Ext_List"), _T("/;html;htm;shtml;cgi;asp;com;net;jp;lnk;url;") ); //+++ "Ext_List"があればその中身を、なければ第２引数を値とする.

	if ( _check_flag(CLPV_EX_ON, m_dwExStyle) )
		InstallClipboardViewer();
}



void CDonutClipboardBar::_WriteProfile()
{
	CIniFileO	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("ClipboardBar") );
	pr.SetValue ( m_dwExStyle, _T("Extended_Style") );
	pr.SetString( m_strExts	 , _T("Ext_List")	   );
	pr.Close();

	UninstallClipboardViewer();
}



void CDonutClipboardBar::_OnItemOpen(int nIndex)
{
	clbTRACE( _T("_OnItemOpen\n") );
	CString 	str;
	MtlListBoxGetText(m_box.m_hWnd, nIndex, str);

	if ( str.IsEmpty() )
		return;

	DonutOpenFile(str, 0);
}

