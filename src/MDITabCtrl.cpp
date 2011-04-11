/**
 *	@file	MDITabCtrl.cpp
 */

#include "stdafx.h"
#include "MDITabCtrl.h"
#include "MtlProfile.h"
#include "MtlBrowser.h"
#include "ParseInternetShortcutFile.h"	//+++ for ParseInternetShortcutFile()
#include "Donut.h"
#include "option/MenuDialog.h"
#include "option/RightClickMenuDialog.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace MTL {



// Constructor
CMDITabCtrl::CMDITabCtrl()
	: m_dwExtendedStyle( MTB_EX_DEFAULT_BITS )		//+++ デフォルトを0以外に設定.
	, m_strInitial( _T("Loading...") )
	, m_bInsertHere(false)
	, m_nInsertIndex(-1)
	, m_nMaxTabItemTextLength(30)
	, m_bRedrawLocked(false)
	, m_nLinkState(LINKSTATE_OFF)
	, m_nRClickCommand(0)
	, m_nXClickCommand(0)
	, m_nDClickCommand(0)
{
}



BOOL CMDITabCtrl::LoadMenu(_U_STRINGorID menu)
{
	return m_menuPopup.LoadMenu(menu);
}



int CMDITabCtrl::AddTabItem(HWND hWndMDIChild, LPCTSTR lpszText)
{
	if (hWndMDIChild == NULL)
		return -1;

	// add tab item
	int  nPos;
	int  nCurSel = GetCurSel();

	if (nCurSel == -1)
		nPos = 0;
	else if ( (m_dwExtendedStyle & MTB_EX_ADDLINKACTIVERIGHT) && GetLinkState() )
		nPos = nCurSel + 1;
	else if (m_dwExtendedStyle & MTB_EX_ADDLEFT)
		nPos = 0;
	else if (m_dwExtendedStyle & MTB_EX_ADDLEFTACTIVE)
		nPos = nCurSel;
	else if (m_dwExtendedStyle & MTB_EX_ADDRIGHTACTIVE)
		nPos = nCurSel + 1;
	else
		nPos = GetItemCount();

	BYTE fsState = TCISTATE_ENABLED;

	if ( m_bRedrawLocked || hWndMDIChild != m_wndMDIChildPopuping.MDIGetActive() )
		fsState |= TCISTATE_INACTIVE;

	return InsertItem( nPos, CTabCtrlItem(lpszText, -1, (DWORD_PTR) hWndMDIChild, fsState) );
}



// Attributes
int CMDITabCtrl::GetTabIndex(HWND hWndMDIChild)
{
	if (hWndMDIChild == NULL)
		return -1;

	int nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		DWORD_PTR	dwData = 0;
		GetItemUserData(i, dwData);

		if ( (HWND) dwData == hWndMDIChild )
			return i;
	}

	return -1;
}



HWND CMDITabCtrl::GetTabHwnd(int nIndex)
{
	if (nIndex < 0)
		return NULL;

	DWORD_PTR dwData = 0;

	if ( false == GetItemUserData(nIndex, dwData) )
		dwData = 0 /*NULL*/;

	return (HWND) dwData;
}



void CMDITabCtrl::RightTab()
{
	int nIndex = GetCurSel();
	int nCount = GetItemCount();

	if (nCount < 2)
		return;

	int nNext  = (nIndex + 1 < nCount) ? nIndex + 1 : 0;
	SetCurSelEx(nNext);
}



void CMDITabCtrl::SetCurSelEx(int nIndex, bool bActivate)
{
	SetCurSel(nIndex);

	if (bActivate) {
		HWND	hWnd = GetTabHwnd(nIndex);
		ATLASSERT( ::IsWindow(hWnd) );
#if 1 //+++ メモ:unDonut+
		CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
		wndMDI.SetRedraw(FALSE);
		m_wndMDIChildPopuping.MDIActivate(hWnd);
		wndMDI.SetRedraw(TRUE);
		wndMDI.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
#else	//+++ メモ:r13test	//*お試し
		m_wndMDIChildPopuping.MDIActivate(hWnd);
#endif
	}
}



void CMDITabCtrl::LeftTab()
{
	int nIndex = GetCurSel();
	int nCount = GetItemCount();

	if (nCount < 2)
		return;

	int nNext  = nIndex - 1 < 0 ? nCount - 1 : nIndex - 1;
	SetCurSelEx(nNext);
}



void CMDITabCtrl::_SendSelChange(int nIndex)
{
	int   idCtrl = ::GetDlgCtrlID(m_hWnd);
	NMHDR nmh	 = { m_hWnd, idCtrl, TCN_SELCHANGE };

	SendMessage(WM_NOTIFY, (WPARAM) idCtrl, (LPARAM) &nmh);
}



DWORD CMDITabCtrl::GetMDITabExtendedStyle() const
{
	return m_dwExtendedStyle;
}



DWORD CMDITabCtrl::SetMDITabExtendedStyle(DWORD dwExtendedStyle)
{
	DWORD dwPrevStyle = m_dwExtendedStyle;

	m_dwExtendedStyle = dwExtendedStyle;

	if (dwExtendedStyle & MTB_EX_MULTILINE)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_MULTILINE);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_MULTILINE, 0);

	if (dwExtendedStyle & MTB_EX_FIXEDSIZE)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_FIXEDSIZE);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_FIXEDSIZE, 0);

	if (dwExtendedStyle & MTB_EX_ANCHORCOLOR)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_ANCHORCOLOR);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_ANCHORCOLOR, 0);

	if (dwExtendedStyle & MTB_EX_MOUSEDOWNSELECT)
		ModifyTabCtrl2ExtendedStyle(0, TAB2_EX_MOUSEDOWNSELECT);
	else
		ModifyTabCtrl2ExtendedStyle(TAB2_EX_MOUSEDOWNSELECT, 0);

	return dwPrevStyle;
}



void CMDITabCtrl::SetInitialText(LPCTSTR lpszInitialText)
{
	ATLASSERT(FALSE);
	m_strInitial = lpszInitialText;
}



void CMDITabCtrl::SetMaxTabItemTextLength(int nLength)
{
	if (m_nMaxTabItemTextLength == nLength)
		return;

	m_nMaxTabItemTextLength = nLength;

	UINT nCount = GetItemCount();
	for (UINT i = 0; i < nCount; ++i) {
		HWND  hWnd = GetTabHwnd(i);
		ATLASSERT( ::IsWindow(hWnd) );
		TCHAR szText[MAX_PATH];
		szText[0] = 0;
		::GetWindowText(hWnd, szText, MAX_PATH);
		_SetTabText(i, szText);
	}
}



int CMDITabCtrl::GetMaxTabItemTextLength()
{
	return m_nMaxTabItemTextLength;
}



// Operations
void CMDITabCtrl::SetMDIClient(HWND hWndMDIClient)
{
	ATLASSERT( ::IsWindow(hWndMDIClient) );
	ATLASSERT(m_wndMDIChildPopuping.m_hWndMDIClient == NULL);
	m_wndMDIChildPopuping.m_hWndMDIClient = hWndMDIClient;
}



// UDT JOBBY
// タブバーを透明にする (メニューバーのコピー)
// ダブルバッファの使用で不要に(_DoPaintに移動)
LRESULT CMDITabCtrl::OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
{
	return 1;
}
// ENDE



//*+++ 実際のボタンにあわせてXButtonからMButtonに関数名を改名しとく...
LRESULT CMDITabCtrl::OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
{
	SetMsgHandled(FALSE);

	POINT	point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	CSimpleArray<int> arrCurMultiSel;
	HWND			  hWndChild;
	int	nIndex = HitTest(point);
	GetCurMultiSelEx(arrCurMultiSel, nIndex);

	if (arrCurMultiSel.GetSize() > 1) {
		for (int i = 0; i < arrCurMultiSel.GetSize(); ++i) {
			hWndChild = GetTabHwnd(arrCurMultiSel[i]);

			if (m_dwExtendedStyle & MTB_EX_XCLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (m_dwExtendedStyle & MTB_EX_XCLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (m_dwExtendedStyle & MTB_EX_XCLICKNLOCK)									// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (m_dwExtendedStyle & MTB_EX_XCLICKCOMMAND) && m_nXClickCommand )			//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) m_nXClickCommand, 0);
		}
	} else {
		int nIndex = HitTest(point);

		if (nIndex != -1) {
			hWndChild = GetTabHwnd(nIndex);

			if (m_dwExtendedStyle & MTB_EX_XCLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (m_dwExtendedStyle & MTB_EX_XCLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (m_dwExtendedStyle & MTB_EX_XCLICKNLOCK)									// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (m_dwExtendedStyle & MTB_EX_XCLICKCOMMAND) && m_nXClickCommand )			//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) m_nXClickCommand, 0);
		}
	}

	return 0;
}



void CMDITabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	CSimpleArray<int> arrCurMultiSel;
	HWND			  hWndChild;
	int	nIndex = HitTest(point);
	GetCurMultiSelEx(arrCurMultiSel, nIndex);

	if (arrCurMultiSel.GetSize() > 1) {
		for (int i = 0; i < arrCurMultiSel.GetSize(); ++i) {
			hWndChild = GetTabHwnd(arrCurMultiSel[i]);

			if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKNLOCK)								// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (m_dwExtendedStyle & MTB_EX_DOUBLECLICKCOMMAND) && m_nDClickCommand ) 	//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) m_nDClickCommand, 0);
		}
	} else {
		int nIndex = HitTest(point);

		if (nIndex != -1) {
			hWndChild = GetTabHwnd(nIndex);

			if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKCLOSE)
				::PostMessage(hWndChild, WM_CLOSE, 0, 0);
			else if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKREFRESH)
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
			else if (m_dwExtendedStyle & MTB_EX_DOUBLECLICKNLOCK)								// UDT DGSTR
				::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_DOCHOSTUI_OPENNEWWIN, 0);
			else if ( (m_dwExtendedStyle & MTB_EX_DOUBLECLICKCOMMAND) && m_nDClickCommand ) 	//minit
				::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) m_nDClickCommand, 0);
		}
	}
}


// 右クリック
void CMDITabCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{	// Overrides
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnRButtonUp\n") );
	int nIndex = HitTest(point);

	if (nIndex != -1) {
		HWND hWndChild = GetTabHwnd(nIndex);
		ATLASSERT(hWndChild != NULL);

		if (m_dwExtendedStyle & MTB_EX_RIGHTCLICKCLOSE) {
			::PostMessage(hWndChild, WM_CLOSE, 0, 0);
		} else if (m_dwExtendedStyle & MTB_EX_RIGHTCLICKREFRESH) {
			::PostMessage(hWndChild, WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
		} else if ( (m_dwExtendedStyle & MTB_EX_RIGHTCLICKCOMMAND) && m_nRClickCommand ) {		//minit
			::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) m_nRClickCommand, 0);
		} else if (m_menuPopup.m_hMenu) {
			ClientToScreen(&point);
			//CMenuHandle menu	 = m_menuPopup.GetSubMenu(0);

			DWORD	dwFlag = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON;
			if (CMenuOption::s_bR_Equal_L) dwFlag |= TPM_RIGHTBUTTON;

			CMenuHandle menu = CCustomContextMenuOption::s_menuTabItem;
			CSimpleArray<HMENU>	arrDestroyMenu;
			CCustomContextMenuOption::AddSubMenu(menu, GetTopLevelWindow(), arrDestroyMenu);
			// ポップアップメニューを表示する
			menu.TrackPopupMenu(dwFlag, point.x, point.y, hWndChild);

			CCustomContextMenuOption::RemoveSubMenu(menu, arrDestroyMenu);

		} else {																				// system menu (default)
			CMenuHandle menuSys = ::GetSystemMenu(hWndChild, FALSE);
			ClientToScreen(&point);
			m_wndMDIChildPopuping = hWndChild;
			_UpdateMenu(menuSys);
			menuSys.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								   point.x, point.y, m_hWnd);									// owner is me!!
		}
	} else {
		::SendMessage(GetTopLevelParent(), WM_SHOW_TOOLBARMENU, 0, 0);
	}

	SetMsgHandled(FALSE);
}


// タブが切り替わるときに呼ばれる
LRESULT CMDITabCtrl::OnTcnSelChange(LPNMHDR lpnhmdr)
{
	// Watch OnLButtonDown, this handler will not be called by windows.
	// The flat style tab has a wastefull animation.

	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnTcnSelChange : %d\n"), GetCurSel() );
	int 	nIndex	   = GetCurSel();
	ATLASSERT(nIndex != -1);
#if 1	//+++ メモ:unDonut+
	CWindow wndMDI(m_wndMDIChildPopuping.m_hWndMDIClient);
	wndMDI.SetRedraw(FALSE);

	HWND	hWndActive = GetTabHwnd(nIndex);
	m_wndMDIChildPopuping.MDIActivate(hWndActive);

	wndMDI.SetRedraw(TRUE);
	wndMDI.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
#else	//+++ メモ:r13test	//*お試し
	HWND	hWndActive = GetTabHwnd(nIndex);
	m_wndMDIChildPopuping.MDIActivate(hWndActive);
#endif
	return 0;
}



LRESULT CMDITabCtrl::OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
{
	ATLASSERT( ::IsWindow(m_wndMDIChildPopuping.m_hWndMDIClient) );
	// for help message line
	return ::SendMessage(::GetParent(m_wndMDIChildPopuping.m_hWndMDIClient), WM_MENUSELECT, wParam, lParam);
}



void CMDITabCtrl::OnCommand(UINT wNotifyCode, int wID, HWND hwndCtl)
{
	ATLTRACE2(atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnCommand - MDI child window:%x\n"), m_wndMDIChildPopuping.m_hWnd);

	if (m_wndMDIChildPopuping.m_hWnd == NULL) { 		// this is not my command message
		SetMsgHandled(FALSE);
		return;
	}

	ATLASSERT( m_wndMDIChildPopuping.IsWindow() );
	CMDIWindow		wnd   = m_wndMDIChildPopuping;
	m_wndMDIChildPopuping = NULL;		// It must be reset cause SendMessage(WM_SYSCOMMAND) make one more WM_COMMAND.

	if (wID != SC_CLOSE && wID != SC_NEXTWINDOW)
		wnd.BringWindowToTop();

	// Note: If you send SC_NEXTWINDOW to inactive child window,
	//		 order is broken. Tell me why.
	if (wID == SC_NEXTWINDOW)
		wnd.MDINext(NULL);					// I think NULL is natural.
	else if (wID == SC_MAXIMIZE)
		wnd.ShowWindow(SW_MAXIMIZE);		// without this, frame window maximized. why?
	//else if (wID == SC_CLOSE)
	//	wnd.PostMessage(WM_CLOSE);			// without this, debug assertion occurs?
	else
		wnd.SendMessage(WM_SYSCOMMAND, (WPARAM) wID);
}



void CMDITabCtrl::OnMDIChildCreate(HWND hWnd)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CChildFrame::OnMDIChildCreate\n") );

	if (m_bInsertHere) {
		BYTE fsState = TCISTATE_ENABLED;

		if ( m_bRedrawLocked || hWnd != m_wndMDIChildPopuping.MDIGetActive() )
			fsState |= TCISTATE_INACTIVE;

		InsertItem( m_nInsertIndex, CTabCtrlItem(m_strInitial, -1, (DWORD_PTR) hWnd, fsState) );
		return;
	}

	ATLASSERT(GetTabIndex(hWnd) == -1);

	AddTabItem(hWnd, m_strInitial);
	ATLTRACE2( atlTraceGeneral, 4, _T(" new child window found, add tab\n") );
	return;
}



void CMDITabCtrl::OnMDIActivate(HWND hWndChildDeact, HWND hWndChildAct)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnMDIActivate\n") );
	SetMsgHandled(FALSE);

	if (hWndChildAct == NULL)
		return;

	int nIndex = GetTabIndex(hWndChildAct);
	ATLASSERT(nIndex != -1);

	if (!m_bRedrawLocked) {
		SetItemActive(nIndex);
		UpdateWindow();
	}

	SetCurSelEx(nIndex, false);
}



LRESULT CMDITabCtrl::OnSetText(LPCTSTR lpszText)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnSetText\n") );
	SetMsgHandled(FALSE);
	// for new text
	int nIndex = GetTabIndex(m_wndMDIChildProcessing);

	if (nIndex != -1)
		_SetTabText(nIndex, lpszText);

	return FALSE;
}



void CMDITabCtrl::OnMDIChildDestroy(HWND hWnd)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::OnDestroy\n") );
	int nIndex = GetTabIndex(hWnd);

	if (nIndex != -1) {
		DeleteItem(nIndex);
	}
}



// Implementation
bool CMDITabCtrl::_SetTabText(int nIndex, LPCTSTR lpszTab)
{
	CString strTab(lpszTab);

	strTab = MtlCompactString(strTab, m_nMaxTabItemTextLength);
	return SetItemText(nIndex, strTab);
}



CString CMDITabCtrl::_GetTabText(int nIndex)
{
	CString str;

	GetItemText(nIndex, str);
	return str;
}



void CMDITabCtrl::_UpdateMenu(CMenuHandle &menuSys)
{	// Emulation is needed cause MDI child window won't update its menu information until clicked.
	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrlImpl::_UpdateMenu\n") );
	ATLASSERT( ::IsMenu(menuSys.m_hMenu) );

	// no effect
	// m_wndMDIChildPopuping.SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE |
	//	SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

	menuSys.EnableMenuItem(SC_RESTORE , MF_BYCOMMAND | MF_ENABLED);
	menuSys.EnableMenuItem(SC_MOVE	  , MF_BYCOMMAND | MF_ENABLED);
	menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_ENABLED);
	menuSys.EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);
	menuSys.EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | MF_ENABLED);

	CWindowPlacement wndpl;
	m_wndMDIChildPopuping.GetWindowPlacement(&wndpl);

	switch (wndpl.showCmd) {
	case SW_SHOWNORMAL:
		ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWNORMAL\n") );
		menuSys.EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
		::SetMenuDefaultItem(menuSys, SC_CLOSE, FALSE);
		break;

	case SW_SHOWMAXIMIZED:
		ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWMAXIMIZED\n") );
		menuSys.EnableMenuItem(SC_MOVE	  , MF_BYCOMMAND | MF_GRAYED);
		menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_GRAYED);
		menuSys.EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
		::SetMenuDefaultItem(menuSys, SC_CLOSE, FALSE);
		break;

	case SW_SHOWMINIMIZED:
		ATLTRACE2( atlTraceGeneral, 4, _T(" SW_SHOWMINIMIZED\n") );
		menuSys.EnableMenuItem(SC_SIZE	  , MF_BYCOMMAND | MF_GRAYED);
		menuSys.EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
		::SetMenuDefaultItem(menuSys, SC_RESTORE, FALSE);
		break;

	default:
		ATLASSERT(FALSE);
		break;
	}

	if ( m_wndMDIChildPopuping.m_hWnd != m_wndMDIChildPopuping.MDIGetActive() ) {	// it's not active
		menuSys.EnableMenuItem(SC_MOVE, MF_BYCOMMAND | MF_GRAYED);
		menuSys.EnableMenuItem(SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
	}
}



BOOL CMDITabCtrl::LoadConnectingAndDownloadingImageList(UINT nImageBmpID, int cx, int cy, COLORREF clrMask, UINT nFlags)
{
	CString strTabBmp = _GetSkinDir() + _T("Tab.bmp");

	if (m_imgs.m_hImageList == NULL) {
		m_imgs.Create(cx, cy, nFlags | ILC_MASK, 2, 1);
		ATLASSERT(m_imgs.m_hImageList != NULL);

		if (m_imgs.m_hImageList == NULL)
			return FALSE;
	}

	CBitmap bmp;
	bmp.Attach( AtlLoadBitmapImage(strTabBmp.GetBuffer(0), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL)
		bmp.LoadBitmap(nImageBmpID);

	ATLASSERT(bmp.m_hBitmap != NULL);

	if (bmp.m_hBitmap == NULL)
		return FALSE;

	if (m_imgs.Add(bmp, clrMask) == -1)
		return FALSE;

	return TRUE;
}



void CMDITabCtrl::SetConnecting(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return;

	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrl::SetConnecting\n") );
	_SetImageListIndex(nItem, 0);
}



void CMDITabCtrl::SetDownloading(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return;

	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrl::SetDowloding\n") );
	_SetImageListIndex(nItem, 1);
}



void CMDITabCtrl::SetComplete(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return;

	ATLTRACE2( atlTraceGeneral, 4, _T("CMDITabCtrl::SetComplete\n") );
	_SetImageListIndex(nItem, -1);
}



void CMDITabCtrl::_SetImageListIndex(int nItem, int nIndex)
{
	SetItemImageIndex(nItem, nIndex);
}



bool CMDITabCtrl::IsConnecting(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == 0;
}



bool CMDITabCtrl::IsDownloading(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == 1;
}



bool CMDITabCtrl::IsCompleted(HWND hWnd)
{
	int nItem = GetTabIndex(hWnd);

	if (nItem == -1)
		return false;

	return _GetImageListIndex(nItem) == -1;
}



int CMDITabCtrl::_GetImageListIndex(int nItem)
{
	int nImgIndex = -1;

	GetItemImageIndex(nItem, nImgIndex);
	return nImgIndex;
}



int CMDITabCtrl::ManageClose(HWND m_hWnd)
{
	int  nIndex 	= GetCurSel();
	HWND hWndActive = GetTabHwnd(nIndex);

	ATLASSERT(hWndActive == m_hWnd);

	int  nCount 	= GetItemCount();

	if (m_dwExtendedStyle & MTB_EX_RIGHTACTIVEONCLOSE) {
		int nNext = nIndex + 1;

		if (nNext < nCount) {
			//SetCurSelEx(nNext);
			return nNext;
		} else {
			nNext = nIndex - 1;

			if (nNext >= 0) {
				//SetCurSelEx(nNext);
				return nNext;
			}
		}
	} else if (m_dwExtendedStyle & MTB_EX_LEFTACTIVEONCLOSE) {
		int nNext = nIndex - 1;

		if (nNext >= 0) {
			//SetCurSelEx(nNext);
			return nNext;
		} else {
			nNext = nIndex + 1;

			if (nNext < nCount) {
				//SetCurSelEx(nNext);
				return nNext;
			}
		}
	}

	return -1;
}



void CMDITabCtrl::NavigateLockTab(HWND hWnd, bool bOn)
{
	int nIndex = GetTabIndex(hWnd);
	//ATLASSERT(nIndex != -1);

	if (nIndex >= 0) {
		if (bOn)
			SetItemDisabled(nIndex);
		else
			SetItemEnabled(nIndex);
	}
}



int CMDITabCtrl::ShowTabListMenuDefault(int nX, int nY)
{
	int 	nCount = GetItemCount();
	CMenu	menu;

	menu.CreatePopupMenu();

	for (int i = 0; i < nCount; i++) {
		menu.AppendMenu( 0, i + 1, _GetTabText(i) );
	}

	int 	nRet	 = menu.TrackPopupMenu(
						TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						nX,
						nY + 1,
						m_hWnd);
	return	nRet - 1;
}



int CMDITabCtrl::ShowTabListMenuVisible(int nX, int nY)
{
	int 		idxFirst = GetFirstVisibleIndex();
	int 		idxLast  =	GetLastVisibleIndex();

	if (idxFirst == -1 || idxLast == -1)
		return -1;

	// no item exist

	//表示されていない左側のタブ
	int 		i		 = 0;
	int 		nCount	 = GetItemCount();
	CMenu		menu;
	menu.CreatePopupMenu();

	for (i = 0; i < idxFirst; i++) {
		menu.AppendMenu( 0, i + 1, _GetTabText(i) );
	}

	//表示中のタブ
	if (i >= nCount)
		return -1;

	if (idxFirst != 0) {
		menu.AppendMenu(MF_SEPARATOR, 0);
	}

	CMenuHandle menuSub;
	menuSub.CreatePopupMenu();

	for (i = idxFirst; i <= idxLast; i++) {
		menuSub.AppendMenu( 0, i + 1, _GetTabText(i) );
	}

	menu.AppendMenu( MF_POPUP, (UINT_PTR) menuSub.m_hMenu, _T("表示中のタブ") );

	//表示されていない右側のタブ
	if (i > nCount)
		return -1;

	if (idxLast != nCount - 1) {
		menu.AppendMenu(MF_SEPARATOR, 0);
	}

	for (i = idxLast + 1; i < nCount; i++) {
		menu.AppendMenu( 0, i + 1, _GetTabText(i) );
	}

	int 	nRet = menu.TrackPopupMenu(
						TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
						nX,
						nY + 1,
						m_hWnd);
	return nRet - 1;
}



struct CMDITabCtrl::_Object_TabSorting {
	CString strTitle;
	int 	nIndex;
};

struct CMDITabCtrl::_Function_CompareTitle {
	bool operator ()(const _Object_TabSorting &tab1, const _Object_TabSorting &tab2)
	{
		return ::lstrcmp(tab1.strTitle, tab2.strTitle) < 0;
	}
};

int CMDITabCtrl::ShowTabListMenuAlphabet(int nX, int nY)
{
	std::vector<_Object_TabSorting> aryTab;

	int 	nCount = GetItemCount();
	int 	i;
	for (i = 0; i < nCount; i++) {
		_Object_TabSorting tab;
		tab.strTitle = _GetTabText(i);
		tab.nIndex	 = i;
		aryTab.push_back(tab);
	}

	std::sort( aryTab.begin(), aryTab.end(), _Function_CompareTitle() );

	CMenu	menu;
	menu.CreatePopupMenu();

	for (i = 0; i < nCount; i++) {
		menu.AppendMenu(0, i + 1, aryTab[i].strTitle);
	}

	int 	nRet   = menu.TrackPopupMenu(
							TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
							nX,
							nY + 1,
							m_hWnd);

	if (nRet == 0)
		return -1;

	return aryTab[nRet - 1].nIndex;
}



void CMDITabCtrl::ShowTabMenu(int nIndex)
{
	HWND		hWndChild = GetTabHwnd(nIndex);

	if (hWndChild == NULL) return;

	CPoint			point;
	::GetCursorPos(&point);
	CMenuHandle 	menu	  = m_menuPopup.GetSubMenu(0);

	DWORD dwFlags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON;
	if (CMenuOption::s_bR_Equal_L) dwFlags |= TPM_RIGHTBUTTON;

	menu.TrackPopupMenu( dwFlags,
						 point.x,
						 point.y,
						 hWndChild
						);
}



// Overrides
void CMDITabCtrl::OnDropDownButton()
{
	CRect rc;

	m_wndDropBtn.GetClientRect(&rc);
	m_wndDropBtn.ClientToScreen(&rc);
	int   nIndex = ShowTabListMenuVisible(rc.left, rc.top);

	if (nIndex == -1)
		return;

	HWND  hChild = GetTabHwnd(nIndex);

	if ( !::IsWindow(hChild) )
		return;

	::SendMessage(m_wndMDIChildPopuping.m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM) hChild, 0);
}



CString CMDITabCtrl::OnGetToolTipText(int nIndex)
{
	CString strTip;
	HWND	hWnd = GetTabHwnd(nIndex);

	if (hWnd) {
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if ( !browser.IsBrowserNull() ) {
			//const int TIPBUF  = 79;
			CString   strName = MtlGetWindowText(hWnd);
			CString   strUrl  = browser.GetLocationURL();
			strTip = strName + _T("\n") + strUrl;//MtlMakeFavoriteToolTipText(strName, strUrl, TIPBUF);
		}
	}

	return strTip;
}



void CMDITabCtrl::OnDeleteItemDrag(int nIndex)
{
	HWND hWnd;

	if ( ( hWnd = GetTabHwnd(nIndex) ) != 0 )
		::PostMessage( (HWND) hWnd, WM_CLOSE, 0, 0 );
}


// なんでこんなところにあるんだ
//\\bool	m_bDragAccept;



DROPEFFECT CMDITabCtrl::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}



DROPEFFECT CMDITabCtrl::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (!m_bDragAccept)
		return DROPEFFECT_NONE;
	return COleDragDropTabCtrl<CMDITabCtrl>::OnDragOver(pDataObject, dwKeyState, point, dropOkEffect);
}



bool CMDITabCtrl::OnNewTabCtrlItems(
		int 							nInsertIndex,
		CSimpleArray<CTabCtrlItem> &	/*items*/,
		IDataObject *					pDataObject,
		DROPEFFECT &					dropEffect)
{
	CSimpleArray<CString> arrUrls;

	if ( MtlGetDropFileName(pDataObject, arrUrls) ) {
		ATLTRACE(" HDROP found\n");
		m_bInsertHere  = true;
		m_nInsertIndex = nInsertIndex;
		UINT	size   = arrUrls.GetSize();
		for (UINT i = 0; i < size; ++i) {
			HWND hWnd  = DonutOpenFile(m_hWnd, arrUrls[i]);
			if (hWnd)
				++m_nInsertIndex;
		}

		m_nInsertIndex = -1;
		m_bInsertHere  = false;
		dropEffect	   = DROPEFFECT_COPY;
		return true;
	}

	CString 	strText;

	if ( MtlGetHGlobalText(pDataObject, strText)
	   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		m_bInsertHere  = true;
		m_nInsertIndex = nInsertIndex;
		DonutOpenFile(m_hWnd, strText);
		m_nInsertIndex = -1;
		m_bInsertHere  = false;
		dropEffect	   = DROPEFFECT_COPY;
		return true;
	}

	dropEffect = DROPEFFECT_NONE;
	return false;
}



bool CMDITabCtrl::OnDropTabCtrlItem(int nIndex, IDataObject *pDataObject, DROPEFFECT &dropEffect)
{
	HWND	hWnd = GetTabHwnd(nIndex);
	ATLASSERT( ::IsWindow(hWnd) );

	// first, get drop filename
	CSimpleArray<CString> arrFiles;

	if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if (browser.m_spBrowser != NULL) {
			CString strFile = arrFiles[0];

			if ( !MtlIsProtocol( strFile, _T("http") )
			   && !MtlIsProtocol( strFile, _T("https") ) )
			{
				if ( MtlPreOpenFile(strFile) )
					return false;

				// handled
			}

			MTL::ParseInternetShortcutFile(strFile);

			browser.Navigate2(strFile);
			dropEffect = DROPEFFECT_COPY;
			return false;
		}
	}

	// last, get text
	CString 	strText;

	if ( MtlGetHGlobalText(pDataObject, strText)
	   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		CWebBrowser2 browser = DonutGetIWebBrowser2(hWnd);

		if (browser.m_spBrowser != NULL) {
			browser.Navigate2(strText);
			dropEffect = DROPEFFECT_COPY;
			return false;
		}
	}

	dropEffect = (DROPEFFECT) -1;
	return false;
}



HRESULT CMDITabCtrl::OnGetTabCtrlDataObject(CSimpleArray<int> &arrIndex, IDataObject **ppDataObject)
{
	ATLASSERT(ppDataObject != NULL);
	ATLASSERT(arrIndex.GetSize() > 0);

	HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL,IID_IDataObject, (void **) ppDataObject);
	if ( FAILED(hr) ) {
		*ppDataObject = NULL;
		return E_NOTIMPL;
	}

  #ifdef _ATL_DEBUG_INTERFACES
	ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
  #endif

	CHlinkDataObject *pHlinkDataObject = NULL;		// this is hack, no need to release

	hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);
	if ( SUCCEEDED(hr) ) {
		for (int i = 0; i < arrIndex.GetSize(); ++i) {
			int 		 nIndex  = arrIndex[i];
			DWORD_PTR	 dwData;
			GetItemUserData(nIndex, dwData);
			HWND		 hWnd	 = (HWND) dwData;
			ATLASSERT( ::IsWindow(hWnd) );
			CWebBrowser2 browser = (IWebBrowser2 *) ::SendMessage(hWnd, WM_USER_GET_IWEBBROWSER, 0, 0);
			CString 	 strName = MtlGetWindowText(hWnd);
			CString 	 strUrl  = browser.GetLocationURL();

			if ( strUrl.Left(5) == _T("file:") ) {	// Donut, to be explorer or not
				strName.Empty();
				strUrl = strUrl.Right(strUrl.GetLength() - 8);
				strUrl.Replace( _T('/'), _T('\\') );
			}

			pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(strName, strUrl) );
		}
	}

	return S_OK;
}



void CMDITabCtrl::_LockRedraw(bool bLock)
{
	if (bLock) {
		m_bRedrawLocked = true;
	} else {
		m_bRedrawLocked = false;
		int nIndex = GetCurSel();

		if ( m_wndMDIChildPopuping.MDIGetActive() == GetTabHwnd(nIndex) ) {
			SetItemActive(nIndex);
		}
	}
}


void CMDITabCtrl::SetLinkState(int nState)
{
	m_nLinkState = nState;
}


BOOL CMDITabCtrl::GetLinkState()
{
	BOOL ret = FALSE;

	if (m_nLinkState == LINKSTATE_A_ON || m_nLinkState == LINKSTATE_B_ON)
		ret = TRUE;

	if (m_nLinkState == LINKSTATE_A_ON)
		m_nLinkState = LINKSTATE_OFF;

	return ret;
}







} //namespace MTL
