/**
*	@file	DonutExplorerBar.cpp
*	@brief	エクスプローラーバー
*/

#include "stdafx.h"
#include "DonutExplorerBar.h"

///////////////////////////////////////////////////////////////////////
// CDonutExplorerBar

// Overrides


static const int aryID[] = {
	ID_VIEW_FAVEXPBAR,		  ID_VIEW_FAVEXPBAR_GROUP, ID_VIEW_FAVEXPBAR_HIST,
	ID_VIEW_FAVEXPBAR_SCRIPT, ID_VIEW_CLIPBOARDBAR,    ID_VIEW_PANELBAR,
	ID_VIEW_PLUGINBAR
};

inline int GetIndexFromID(int nID)
{
	for (int i = 0; i < _countof(aryID); ++i) {
		if (aryID[i] == nID)
			return i;
	}
	ATLASSERT( FALSE );
	return -1;
}

CDonutExplorerBar::CDonutExplorerBar(CSplitterWindow& SplitWindow) : 
	m_rSplitWindow(SplitWindow), 
	m_nShowPaneID(ID_VIEW_FAVEXPBAR),
	m_bAutoShowTimer(false), m_bAutoHideTimer(false), m_bExplicitShowBar(false)

{	}


void CDonutExplorerBar::InitPane()
{
	CIniFileI	pr( g_szIniFileName, _T("ExplorerBar") );
	int cxLeftSplitter  = pr.GetValuei(_T("cxLeftSplitter"), 200);

	m_rSplitWindow.SetSplitterPos(cxLeftSplitter);

	bool bExplorerBarVisible = pr.GetValue(_T("ExplorerBarVisible"), FALSE) != 0;
	m_nShowPaneID = pr.GetValue(_T("LastShowPaneID"), ID_VIEW_FAVEXPBAR);

	pr.Close();

	if (bExplorerBarVisible && (CExplorerBarOption::s_dwExplorerBarStyle & EXPLORERBAROPTION_AUTOSHOW) == 0) {
		OnViewBar(0, m_nShowPaneID, NULL);
	} else {
		m_funcSetSinglePaneMode();
	}
}


void CDonutExplorerBar::UpdateLayout(int cxWidth, int cyHeight)
{
	cyHeight -= kBottomHeight;
	__super::UpdateLayout(cxWidth, cyHeight);
	Invalidate(FALSE);
}

void CDonutExplorerBar::DrawPaneTitle(CDCHandle dc)
{
	__super::DrawPaneTitle(dc);

	RECT  rcClient;
	GetClientRect(&rcClient);

	rcClient.top = rcClient.bottom - kBottomHeight;
	dc.DrawEdge(&rcClient, EDGE_ETCHED, BF_TOPRIGHT | BF_ADJUST);

	dc.FillRect(&rcClient,(HBRUSH)GetStockObject(WHITE_BRUSH));

	int   nTabWidth = (int) (kBottomHeight * 1.5);
	CRect rcTab(rcClient);
	rcTab.top	-= 1;
	rcTab.left	+= 2;
	rcTab.right  = rcTab.left + nTabWidth;

	CPen  penFace, penBlack;
	penFace.CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW) );
	penBlack.CreatePen( PS_SOLID, 0, RGB(0, 0, 0) );
	dc.SelectPen(penFace);

	for (int nIndex = 0; nIndex < _countof(aryID); ++nIndex) {
		if (nIndex == m_nIndexAct) {
			dc.FillRect(&rcTab, COLOR_3DFACE);
			dc.SelectPen(penBlack);
			dc.MoveTo(rcTab.left	 , rcTab.bottom - 1);
			dc.LineTo(rcTab.right - 1, rcTab.bottom - 1);
			dc.LineTo(rcTab.right - 1, rcTab.top);
			dc.SelectPen(penFace);
		} else {
			dc.FillSolidRect( &rcTab, RGB(255, 255, 255) );

			dc.MoveTo(rcTab.right - 1, rcTab.top	+ 3);
			dc.LineTo(rcTab.right - 1, rcTab.bottom - 3);
		}

		int x = rcTab.left + (rcTab.Width() - kcyIcon) / 2;
		int y = rcTab.top  + 2;
		m_imgs.Draw(dc, nIndex, x, y, ILD_TRANSPARENT);

		rcTab.left	+= nTabWidth;
		rcTab.right += nTabWidth;
	}
}


int CDonutExplorerBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	m_nIndexAct = -1;

	CBitmap bmp;
	bmp.Attach( AtlLoadBitmapImage(static_cast<LPCTSTR>( _GetSkinDir() + _T("ExpBar.bmp") ), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL)
		bmp.LoadBitmap(IDB_EXPBAR);		//+++	スキンになければデフォルトを使う.
	m_imgs.Create(kcyIcon, kcyIcon, ILC_COLOR24 | ILC_MASK, _countof(aryID), 1);
	m_imgs.Add( bmp, RGB(255, 0, 255) );

	if (CExplorerBarOption::s_dwExplorerBarStyle & EXPLORERBAROPTION_AUTOSHOW)
		HookMouseMoveForAutoShow(true);

	return 0;
}

void CDonutExplorerBar::OnDestroy()
{
	CIniFileIO	pr( g_szIniFileName, _T("ExplorerBar") );
	pr.SetValue( m_rSplitWindow.GetSplitterPos(), _T("cxLeftSplitter") );
	bool bVisible = m_rSplitWindow.GetSinglePaneMode() == SPLIT_PANE_NONE;
	pr.SetValue(bVisible, _T("ExplorerBarVisible"));

	HookMouseMoveForAutoShow(false);
}

int		CDonutExplorerBar::_HitTestTab(const CPoint& pt)
{
	RECT  rcClient;
	GetClientRect(&rcClient);

	rcClient.top = rcClient.bottom - kBottomHeight;

	int   nTabWidth = (int) (kBottomHeight * 1.5);
	CRect rcTab(rcClient);
	rcTab.top	-= 1;
	rcTab.left	+= 2;
	rcTab.right  = rcTab.left + nTabWidth;

	for (int nIndex = 0; nIndex < _countof(aryID); ++nIndex) {
		if (rcTab.PtInRect(pt))
			return nIndex;

		rcTab.left	+= nTabWidth;
		rcTab.right += nTabWidth;
	}
	return -1;
}

// タブがクリックされたらパネルを切り替える
void CDonutExplorerBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nIndex = _HitTestTab(point);
	if (nIndex == -1 || nIndex == m_nIndexAct)
		return ;
	if (_IsBarVisible(aryID[nIndex]))
		return ;
	OnViewBar(0, aryID[nIndex], NULL);
}

void CDonutExplorerBar::OnPaneClose(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_funcSetSinglePaneMode();
	::ShowWindow(GetClient(), FALSE);
	m_bExplicitShowBar = false;
}


void CDonutExplorerBar::OnViewBar(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	bool bShow = IsWindowVisible() != 0;
	HWND hWndClient = GetClient();
	CWindow wndTarget;
	std::function<HWND ()>	funcCreateWindow;

	switch (nID) {
	case ID_VIEW_FAVEXPBAR:
		wndTarget = m_donutFavoriteTreeView;
		funcCreateWindow = [this] () -> HWND {
			m_donutFavoriteTreeView.Create(m_hWnd);
			return m_donutFavoriteTreeView.m_hWnd;
		};
		break;

	case ID_VIEW_FAVEXPBAR_HIST:
	case ID_VIEW_FAVEXPBAR_SCRIPT:
		wndTarget = m_FavBar;
		funcCreateWindow = [this]() -> HWND {
			m_FavBar.Create(m_hWnd);
			m_FavBar.SetWindowText(_T("お気に入り"));
			m_FavBar.InitToolBar( ID_FAVORITE_ADD, ID_FAVORITE_ORGANIZE, ID_FAVORITE_PLACEMENT, IDB_FAVBAR, IDB_FAVBAR_HOT,
								16, 16, RGB(255, 0, 255) );
			return m_FavBar;
		};
		break;

	case ID_VIEW_FAVEXPBAR_GROUP:
		wndTarget = m_donutFavoriteGroupTreeView;
		funcCreateWindow = [this]() -> HWND {
			m_donutFavoriteGroupTreeView.Create(m_hWnd);
			return m_donutFavoriteGroupTreeView.m_hWnd;
		};
		break;


	case ID_VIEW_CLIPBOARDBAR:
		wndTarget = m_ClipBar;
		funcCreateWindow = [this]() -> HWND {
			m_ClipBar.Create(m_hWnd);
			m_ClipBar.SetWindowText(_T("クリップボード"));
			return m_ClipBar;
		};
		break;

	default:
		return ;
	}

	if (_IsBarVisible(nID)) {
		// エクスプローラーバーを隠す
		m_rSplitWindow.SetSinglePaneMode(SPLIT_PANE_RIGHT);
		wndTarget.ShowWindow(FALSE);
	} else {
		m_nIndexAct = GetIndexFromID(nID);
		m_nShowPaneID = nID;
		Invalidate(FALSE);
		m_bExplicitShowBar = true;

		if (hWndClient)
			::ShowWindow(hWndClient, FALSE);
		
		if (wndTarget.IsWindow() == FALSE) {
			wndTarget = funcCreateWindow();
		}
		if (nID == ID_VIEW_FAVEXPBAR) {
			//m_FavBar.SendMessage(WM_COMMAND, ID_FAVTREE_BAR_STANDARD);
		} else if (nID == ID_VIEW_FAVEXPBAR_HIST) {
			m_FavBar.SendMessage(WM_COMMAND, ID_FAVTREE_BAR_HISTORY);
		} else if (nID == ID_VIEW_FAVEXPBAR_SCRIPT) {
			m_FavBar.SendMessage(WM_COMMAND, ID_FAVTREE_BAR_SCRIPT);
		}
		wndTarget.ShowWindow(TRUE);
		SetTitle( MtlGetWindowText(wndTarget) );
		SetClient(wndTarget);
		
		// スプリット表示にする
		m_rSplitWindow.SetSinglePaneMode();

		CIniFileIO	pr( g_szIniFileName, _T("ExplorerBar") );
		pr.SetValue(nID, _T("LastShowPaneID"));
	}
}

void CDonutExplorerBar::OnFavoriteExpBar(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnViewBar(0, ID_VIEW_FAVEXPBAR, NULL);
	m_FavBar.SendMessage(WM_COMMAND, nID);

	m_rSplitWindow.SetSinglePaneMode();
}


void CDonutExplorerBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kAutoShowTimerId) {
		if (m_nIndexAct == -1) {
			OnViewBar(NULL, m_nShowPaneID, NULL);
		} else {
			OnViewBar(NULL, aryID[m_nIndexAct], NULL);
		}
		
		m_bExplicitShowBar = false;
		KillTimer(kAutoShowTimerId);
		m_bAutoShowTimer = false;
	} else if (nIDEvent == kAutoHideTimerId) {
		OnPaneClose(0, 0, NULL);
		KillTimer(kAutoHideTimerId);
		m_bAutoHideTimer = false;
	}
}

// 定義
HHOOK	CDonutExplorerBar::s_hHook = NULL;
function<void (const CPoint&)>	CDonutExplorerBar::s_funcAutoShowHide;

LRESULT CALLBACK CDonutExplorerBar::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		if (wParam == WM_MOUSEMOVE) {
			 auto msllhk = (MSLLHOOKSTRUCT*)lParam;
			 s_funcAutoShowHide(msllhk->pt);
		}
	}
	return CallNextHookEx(s_hHook, nCode, wParam, lParam);
}

void	CDonutExplorerBar::HookMouseMoveForAutoShow(bool bHook)
{
	if (bHook) {
		if (s_hHook)
			return ;

		ATLVERIFY(s_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, _Module.GetModuleInstance(), 0));

		s_funcAutoShowHide = [this](const CPoint& pt) {
			HWND hWndForeground = ::GetForegroundWindow();
			if (hWndForeground != GetTopLevelWindow()) 
				return ;	// unDonutがフォアグラウンドになっていないと帰る
			
			if (IsWindowVisible() == false) {
				CWindow wndSplitter = GetParent();
				CRect rcSplitter;
				wndSplitter.GetClientRect(&rcSplitter);
				wndSplitter.ClientToScreen(&rcSplitter);
				rcSplitter.right = rcSplitter.left + kAutoShowVlidWidth;
				if (rcSplitter.PtInRect(pt)) {
					if (m_nIndexAct == -1) {
						OnViewBar(NULL, m_nShowPaneID, NULL);
					} else {
						OnViewBar(NULL, aryID[m_nIndexAct], NULL);
					}		
					m_bExplicitShowBar = false;
					//// 一定時間たったあとに表示する
					//if (m_bAutoShowTimer == false) {
					//	SetTimer(kAutoShowTimerId, kAutoShowTimerInterval);
					//	m_bAutoShowTimer = true;
					//} 
				} 
				//else {
				//	// 表示判定ポイントから外れたのでタイマーを止める
				//	if (m_bAutoShowTimer) {
				//		KillTimer(kAutoShowTimerId);
				//		m_bAutoShowTimer = false;
				//	}
				//}
			} else {
				// 自動で消す
				CRect rcThis;
				GetClientRect(&rcThis);
				ClientToScreen(&rcThis);
				rcThis.right += kAutoShowRightMargin;
				if (rcThis.PtInRect(pt) == false) {
					if (m_bExplicitShowBar)
						return ;	// 明示的に表示させたので一度クライアント領域に入るまで閉じない
					// 一定時間後隠す
					if (m_bAutoHideTimer == false) {
						SetTimer(kAutoHideTimerId, kAutoHideTimerInterval);
						m_bAutoHideTimer = true;
					}
				} else {
					m_bExplicitShowBar = false;
					// クライアント領域に入ったのでタイマーを止める
					if (m_bAutoHideTimer) {
						KillTimer(kAutoHideTimerId);
						m_bAutoHideTimer = false;
					}
				}
			}
		};
	} else {
		if (s_hHook) {
			ATLVERIFY(UnhookWindowsHookEx(s_hHook));
			s_hHook = NULL;
		}
	}
}

/// エクスプローラーバーを自動で表示
void CDonutExplorerBar::OnExplorerBarAutoShow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// トグル
	if (CExplorerBarOption::s_dwExplorerBarStyle & EXPLORERBAROPTION_AUTOSHOW) {
		CExplorerBarOption::s_dwExplorerBarStyle &= ~EXPLORERBAROPTION_AUTOSHOW;
		HookMouseMoveForAutoShow(false);
	} else {
		CExplorerBarOption::s_dwExplorerBarStyle |= EXPLORERBAROPTION_AUTOSHOW;
		HookMouseMoveForAutoShow(true);
	}
	CExplorerBarOption::WriteProfile();
}


bool	CDonutExplorerBar::_IsBarVisible(int nID)
{
	bool bShow = IsWindowVisible() != 0;
	HWND hWndClient = GetClient();
	HWND hWndBar = NULL;
	bool bFavEx = true;
	switch (nID) {
	case ID_VIEW_FAVEXPBAR:			hWndBar = m_donutFavoriteTreeView;	break;

	case ID_VIEW_FAVEXPBAR_HIST:	hWndBar = m_FavBar;	
		bFavEx = (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_HISTORY) != 0;
		break;

	case ID_VIEW_FAVEXPBAR_GROUP:	hWndBar = m_donutFavoriteGroupTreeView;		break;

	case ID_VIEW_CLIPBOARDBAR:	hWndBar = m_ClipBar;	break;

	default:
		return false;
	}

	if (bShow && hWndClient != NULL && hWndClient == hWndBar && bFavEx)
		return true;
	else
		return false;
}








