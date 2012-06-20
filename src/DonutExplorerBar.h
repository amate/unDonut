/**
 *	@file	DonutExplorerBar.h
 *	@brief	エクスプローラーバー
 */
#pragma once


#define CHAIN_COMMANDS_TO_EXPLORERBAR(x)		CHAIN_COMMANDS_ALT_MEMBER(x, 1)

#include "option/ExplorerBarDialog.h"
#include "FavTreeViewCtrl.h"
#include "DonutClipboardBar.h"
#include "DonutPanelBar.h"
#include "DonutPFunc.h"
#include "PluginBar.h"


class CDonutExplorerBar
	: public CPaneContainerImpl<CDonutExplorerBar>
	, public CUpdateCmdUI<CDonutExplorerBar>
{
public:
	DECLARE_WND_CLASS_EX(_T("Donut_ExplorerBar"), 0, -1)

private:
	typedef CPaneContainerImpl<CDonutExplorerBar>  baseClass;

	enum EDeb_Ex {
		DEB_EX_NONE 		= 0x00000000L,	//+++ 追加.
		DEB_EX_FAVBAR		= 0x00000001L,
		DEB_EX_CLPBAR		= 0x00000002L,
		DEB_EX_PANELBAR 	= 0x00000004L,
		DEB_EX_PLUGINBAR	= 0x00000008L,
	};

	// Data members
	CSplitterWindow &	m_wndSplit;

public:
//	CDonutFavoritesBar	m_FavBar;
	CDonutClipboardBar	m_ClipBar;
	CDonutPanelBar		m_PanelBar;
	CPluginBar			m_PluginBar;
	DWORD				m_dwExStyle;

private:
	std::vector<int>	m_aryID;
	CImageList			m_imgs;
	int 				m_nBottomAria;
	int 				m_nIndexAct;

	BYTE/*BOOL*/		m_bHideTab;
	BYTE/*bool*/		m_bRemakeFavBarList;

	static CDonutExplorerBar*	s_pThis_;

public:
	// Ctor
	CDonutExplorerBar(CSplitterWindow &wndSplit)
		: m_wndSplit(wndSplit)
		, m_dwExStyle(DEB_EX_FAVBAR)
		, m_bRemakeFavBarList(0)
	{
		s_pThis_		= this;
		m_nBottomAria	= 16;
		m_nIndexAct 	= -1;				//+++ 初回はまだ何も設定されていない状態、とする. //x 0だと、エクスプローラメニューのお気に入りを出したまま起動すると項目が表示されないエラーに化ける.
		m_bHideTab		= FALSE;

		static const int aryID[] = {
			ID_VIEW_FAVEXPBAR,		  ID_VIEW_FAVEXPBAR_GROUP, ID_VIEW_FAVEXPBAR_HIST,
			ID_VIEW_FAVEXPBAR_SCRIPT, ID_VIEW_CLIPBOARDBAR,    ID_VIEW_PANELBAR,
			ID_VIEW_PLUGINBAR
		};
		enum { aryID_NUM = ( sizeof (aryID) / sizeof (aryID[0]) ) };
		for (int ii = 0; ii < aryID_NUM; ii++)
			m_aryID.push_back(aryID[ii]);

		CBitmap 		 bmp;
		bmp.Attach( AtlLoadBitmapImage(LPCTSTR( GetSkinPath() )/*.GetBuffer(0)*/, LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL)
			bmp.LoadBitmap(IDB_EXPBAR);		//+++	スキンになければデフォルトを使う.
		m_imgs.Create(m_nBottomAria, m_nBottomAria, ILC_COLOR24 | ILC_MASK, aryID_NUM, 1);
		m_imgs.Add( bmp, RGB(255, 0, 255) );

	}

	~CDonutExplorerBar() { s_pThis_ = NULL; }

	static CDonutExplorerBar* GetInstance() { return s_pThis_; }

	void Init(HWND hWndMDIClient)
	{
		_CreateOnDemandBar(DEB_EX_CLPBAR);	//+++ クリップボードの窓を用意.(これがないと、クリップボード監視onがクリップボードバーを開けるまで機能しなかった)
		_GetProfile(hWndMDIClient);
	}

	void	RefreshFavBar() { //if (m_FavBar.IsWindow()) m_FavBar.SendMessage(ID_VIEW_REFRESH_FAVBAR); 
	}

	void UpdateLayout()
	{
		RECT rcClient;

		GetClientRect(&rcClient);
	  #if 1	//+++
		int w = rcClient.right	- rcClient.left;
		int h = rcClient.bottom - rcClient.top;
		if (w > 0 && h > 0)
		{
			UpdateLayout(w, h);
		}
	  #else
		UpdateLayout(rcClient.right, rcClient.bottom);
	  #endif
	}


	void UpdateLayout(int cxWidth, int cyHeight)
	{
		if (!m_bHideTab) {
			cyHeight -= m_nBottomAria + 1;
		  #if 1		//+++ WTLに任せる.
		  	CClientDC	dc( m_hWnd );
			DrawBottomTab(HDC(dc));
		  #elif 1	//+++ きっと、ReleaseDCが必要...
			HDC hDc = ::GetDC(m_hWnd);
			DrawBottomTab(hDc);
			::ReleaseDC(m_hWnd, hDc);
		  #else
			CDCHandle dc = ::GetDC(m_hWnd);
			DrawBottomTab(dc);
		  #endif
		}

		//お気に入りツリーのサイズ変更
		//if (m_FavBar) {
		//	CRect rect;
		//	m_FavBar.m_wndReBar.GetRect(0, &rect);
		//	//ボーダー分 + タブ分(-20)だけずらす
		//	m_FavBar.m_view.MoveWindow(1, rect.bottom + 3, cxWidth - 3, cyHeight - rect.bottom - 5 - 20);
		//}

		baseClass::UpdateLayout(cxWidth, cyHeight);
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP( CDonutExplorerBar )
		//MESSAGE_HANDLER( WM_CREATE , OnCreate 	)
		MESSAGE_HANDLER( WM_DESTROY  , OnDestroy	)
		MESSAGE_HANDLER( WM_PAINT	 , OnPaint		)
		MESSAGE_HANDLER( WM_LBUTTONUP, OnLButtonUp	)
		//CHAIN_MSG_MAP( CUpdateCmdUI<CDonutExplorerBar> )
		CHAIN_MSG_MAP( baseClass )

	ALT_MSG_MAP(1) // As CPaneContainer has FORWARD_NOTIFICATIONS, you have to separate a map.
		COMMAND_ID_HANDLER( ID_VIEW_FAVEXPBAR		, OnViewFavExpBar		)
		COMMAND_ID_HANDLER( ID_VIEW_FAVEXPBAR_HIST	, OnViewFavExpBarHist	)
		COMMAND_ID_HANDLER( ID_VIEW_FAVEXPBAR_GROUP , OnViewFavExpBarGroup	)
		COMMAND_ID_HANDLER( ID_VIEW_FAVEXPBAR_USER	, OnViewFavExpBarUser	)
		COMMAND_ID_HANDLER( ID_VIEW_EXPBAR_SETFOCUS , OnViewExpBarSetFocus	)
		COMMAND_ID_HANDLER( ID_VIEW_FAVEXPBAR_SCRIPT, OnViewFavExpBarScript )
		COMMAND_ID_HANDLER( ID_VIEW_CLIPBOARDBAR	, OnViewClipboardBar	)
		COMMAND_ID_HANDLER( ID_VIEW_PANELBAR		, OnViewPanelBar		)
		COMMAND_ID_HANDLER( ID_VIEW_PLUGINBAR		, OnViewPluginBar		)
		COMMAND_ID_HANDLER( ID_PANE_CLOSE			, OnIDPaneClose 		)
		COMMAND_ID_HANDLER( ID_VIEW_EXPLORERBAR_TAB , OnViewExpBarTabs		)

		USER_MSG_WM_OPEN_EXPFAVMENU( OnOpenFavExpMenu )

//		if ( m_FavBar.IsWindow() ) {
//			CHAIN_MSG_MAP_MEMBER(m_FavBar)
//		}
		//CDockingChildBase hasn't FORWARD_NOTIFICATIONS, then I removed ALT_MSG_MAP. (minit)
		//ALT_MSG_MAP(1) // As CPaneContainer has FORWARD_NOTIFICATIONS, you have to separate a map.
	END_MSG_MAP()


	void ReloadSkin()
	{
		_ReplaceImageList(GetSkinPath(), m_imgs);
//		m_FavBar.ReloadSkin();
//		m_FavBar.m_view.ReloadSkin();

		InvalidateRect(NULL, TRUE);
	}


private:
	CString GetSkinPath()
	{
		return _GetSkinDir() + _T("ExpBar.bmp");
	}


	//void DrawBottomTab(CDCHandle dc)
	void DrawBottomTab(HDC hDc)		//+++ 引数をちょっと修正.
	{
		CDCHandle dc(hDc);			//+++ 引数をちょっと修正.
		RECT  rcClient;

		GetClientRect(&rcClient);

		rcClient.top = rcClient.bottom - m_nBottomAria - 1;
		dc.FillSolidRect( &rcClient, RGB(255, 255, 255) );

		int   nTabWidth = (int) (m_nBottomAria * 1.5);
		CRect rcTab(rcClient);
		rcTab.top	-= 2;
		rcTab.left	+= 2;
		rcTab.right  = rcTab.left + nTabWidth;

		CPen  penFace, penBlack;
		penFace.CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW) );
		penBlack.CreatePen( PS_SOLID, 0, RGB(0, 0, 0) );
		dc.SelectPen(penFace);

		int aryID_size = (int) m_aryID.size();
		for (int nIndex = 0; nIndex < aryID_size; nIndex++) {
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

			int x = rcTab.left + (rcTab.Width() - m_nBottomAria) / 2;
			int y = rcTab.top  + 2;
			::ImageList_Draw(m_imgs, nIndex, dc, x, y, ILD_TRANSPARENT);

			rcTab.left	+= nTabWidth;
			rcTab.right += nTabWidth;
		}
	}


	LRESULT OnOpenFavExpMenu(int x, int y)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_FAVTREE_BAR);

		UINT  uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN;
		::TrackPopupMenu(menu.GetSubMenu(0), uMenuFlags, x , y , 0, GetTopLevelParent(), NULL);
		return 0;
	}


	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (m_bHideTab)
			return 0;

		POINT	pt		= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		RECT  rcClient;
		GetClientRect(&rcClient);

		int   nTabWidth = (int) (m_nBottomAria * 1.5);
		CRect rcTab(rcClient);
		//rcTab.top -= 2;
		rcTab.top	= rcClient.bottom - m_nBottomAria;
		rcTab.left += 2;
		rcTab.right = rcTab.left + nTabWidth;

		for (int nIndex = 0; nIndex < (int) m_aryID.size(); nIndex++) {
			if ( rcTab.PtInRect(pt) ) {
				if (m_nIndexAct != nIndex) {
					m_nIndexAct = nIndex;

					Invalidate(FALSE);
					PostMessage(WM_COMMAND, m_aryID[nIndex], 0);
				}

				return 0;
			}

			rcTab.left	+= nTabWidth;
			rcTab.right += nTabWidth;
		}

		return 0;
	}


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CPaintDC dc(m_hWnd);

		DrawPaneTitle(dc.m_hDC);

		if (!m_bHideTab) {
			DrawBottomTab(dc.m_hDC);
		}

		bHandled = FALSE;
		return 0;
	}


	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		_WriteProfile();
		return 0;
	}


	LRESULT OnViewFavExpBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//if ( !IsWindowVisible() ) {
		//	UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_STANDARD);
		//	ShowBar(DEB_EX_FAVBAR);
		//} else if ( !IsFavBarVisible() ) {
		//	if ( (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITES) != ETV_EX_FAVORITES )
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_STANDARD);

		//	ShowBar(DEB_EX_FAVBAR);
		//} else {
		//	if (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITES)
		//		HideBar();
		//	else
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_STANDARD);
		//}

		return 0;
	}


	LRESULT OnViewFavExpBarHist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//if ( !IsWindowVisible() ) {
		//	UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_HISTORY);
		//	ShowBar(DEB_EX_FAVBAR);
		//} else if ( !IsFavBarVisible() ) {
		//	if ( (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_HISTORY) != ETV_EX_HISTORY )
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_HISTORY);

		//	ShowBar(DEB_EX_FAVBAR);
		//} else {
		//	if (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_HISTORY)
		//		HideBar();
		//	else
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_HISTORY);
		//}

		return 0;
	}


	LRESULT OnViewFavExpBarGroup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//if ( !IsWindowVisible() ) {
		//	UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_GROUP);
		//	ShowBar(DEB_EX_FAVBAR);
		//} else if ( !IsFavBarVisible() ) {
		//	if ( (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITEGROUP) != ETV_EX_FAVORITEGROUP )
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_GROUP);

		//	ShowBar(DEB_EX_FAVBAR);
		//} else {
		//	if (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITEGROUP)
		//		HideBar();
		//	else
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_GROUP);
		//}

		return 0;
	}


	LRESULT OnViewFavExpBarUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//if ( !IsWindowVisible() ) {
		//	UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_USER);
		//	ShowBar(DEB_EX_FAVBAR);
		//} else if ( !IsFavBarVisible() ) {
		//	if ( (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_USERDEFINED) != ETV_EX_USERDEFINED )
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_USER);

		//	ShowBar(DEB_EX_FAVBAR);
		//} else {
		//	if (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_USERDEFINED)
		//		HideBar();
		//	else
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_USER);
		//}

		return 0;
	}


	LRESULT OnViewFavExpBarScript(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//if ( !IsWindowVisible() ) {
		//	UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_SCRIPT);
		//	ShowBar(DEB_EX_FAVBAR);
		//} else if ( !IsFavBarVisible() ) {
		//	if ( (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_SCRIPT) != ETV_EX_SCRIPT )
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_SCRIPT);

		//	ShowBar(DEB_EX_FAVBAR);
		//} else {
		//	if (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_SCRIPT)
		//		HideBar();
		//	else
		//		UpSendMessage(WM_COMMAND, ID_FAVTREE_BAR_SCRIPT);
		//}

		return 0;
	}


	LRESULT OnViewClipboardBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( IsExplorerBarHidden() ) {
			if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW) != MAIN_EXPLORER_AUTOSHOW )
				ShowBar(DEB_EX_CLPBAR, true);
			else {
				if (m_dwExStyle == DEB_EX_CLPBAR)
					m_dwExStyle = DEB_EX_NONE;
				else
					ShowBar(DEB_EX_CLPBAR, true);
			}
		} else if ( !IsClipboardBarVisible() )
			ShowBar(DEB_EX_CLPBAR);
		else
			HideBar();

		return 0;
	}


	LRESULT OnViewPanelBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( IsExplorerBarHidden() ) {
			if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW) != MAIN_EXPLORER_AUTOSHOW ) {
				ShowBar(DEB_EX_PANELBAR, true);
			} else {
				if (m_dwExStyle == DEB_EX_PANELBAR)
					m_dwExStyle = DEB_EX_NONE;
				else
					ShowBar(DEB_EX_PANELBAR, true);
			}
		} else if ( !IsPanelBarVisible() ) {
			ShowBar(DEB_EX_PANELBAR);
		} else {
			HideBar();
		}
		return 0;
	}


	LRESULT OnViewPluginBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( IsExplorerBarHidden() ) {
			if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW) != MAIN_EXPLORER_AUTOSHOW ) {
				ShowBar(DEB_EX_PLUGINBAR, true);
			} else {
				if (m_dwExStyle == DEB_EX_PLUGINBAR)
					m_dwExStyle = DEB_EX_NONE;
				else
					ShowBar(DEB_EX_PLUGINBAR, true);
			}
		} else if ( !IsPluginBarVisible() ) {
			ShowBar(DEB_EX_PLUGINBAR);
		} else {
			HideBar();
		}
		return 0;
	}


	LRESULT OnIDPaneClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		HideBar();
		return 0;
	}


	LRESULT OnViewExpBarTabs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bShow = m_bHideTab != 0;
		m_bHideTab = !m_bHideTab;
		ShowTab(bShow);
		return 0;
	}


	LRESULT OnViewExpBarSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//m_FavBar.SetFocus();
		::SetFocus( GetClient() );
		return 0;
	}


	// Methods

public:
	BYTE PreTranslateMessage(MSG *pMsg)
	{
/*		if		( IsFavBarVisible() 	  ) return m_FavBar.PreTranslateMessage(pMsg);
		else if ( IsClipboardBarVisible() ) return m_ClipBar.PreTranslateMessage(pMsg);
		else if ( IsPanelBarVisible()	  ) return m_PanelBar.PreTranslateMessage(pMsg);
		else if ( IsPluginBarVisible()	  ) return m_PluginBar.PreTranslateMessage(pMsg);
		else*/								return _MTL_TRANSLATE_PASS;
	}


	// Attributes
	bool IsFavBarVisible()
	{
		return _check_flag(DEB_EX_FAVBAR, m_dwExStyle);
	}


	bool IsClipboardBarVisible()
	{
		return _check_flag(DEB_EX_CLPBAR, m_dwExStyle);
	}


	bool IsPanelBarVisible()
	{
		return _check_flag(DEB_EX_PANELBAR, m_dwExStyle);
	}


	bool IsPluginBarVisible()
	{
		return _check_flag(DEB_EX_PLUGINBAR, m_dwExStyle);
	}


	bool IsExplorerBarHidden()
	{
		return (m_wndSplit.GetSinglePaneMode() == SPLIT_PANE_RIGHT);
	}


	void ShowBar(DWORD dwExStyle, bool bShowSplitter = false)
	{
		_CreateOnDemandBar(dwExStyle);
		_SwitchClient(dwExStyle);
		m_dwExStyle = dwExStyle;

		//if (dwExStyle & DEB_EX_FAVBAR) {						// && !bNotSetTitle)
		//	DWORD dwViewExStyle = m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle;
		//	if		(dwViewExStyle & ETV_EX_FAVORITES	 )	SetTitle( _T("お気に入り") );
		//	else if (dwViewExStyle & ETV_EX_HISTORY 	 )	SetTitle( _T("履歴") );
		//	else if (dwViewExStyle & ETV_EX_FAVORITEGROUP)	SetTitle( _T("お気に入りグループ") );
		//	else if (dwViewExStyle & ETV_EX_USERDEFINED  )	SetTitle( _T("ユーザー定義") );
		//	else if (dwViewExStyle & ETV_EX_MYCOMPUTER	 )	SetTitle( _T("デスクトップ") );
		//	else if (dwViewExStyle & ETV_EX_SCRIPT		 )	SetTitle( _T("スクリプト") );
		//}

		//if(!IsWindowVisible())
		//	baseClass::Show();
		//UpdateLayout();
		if (bShowSplitter)
			m_wndSplit.SetSinglePaneMode();
	}


	void HideBar()
	{
		m_dwExStyle = DEB_EX_NONE;
		//Hide();
		m_wndSplit.SetSinglePaneMode(SPLIT_PANE_RIGHT);
	}


	void ShowTab(BOOL bShow)
	{
		m_bHideTab = !bShow;
		UpdateLayout();
	}


  #if 1 //+++ mainfrm.hより移動& 隠れている場合に、次回表示時にRefreshされるようにフラグon
	void RefreshExpBar(int nType)
	{
		//if ( ::IsWindow(m_FavBar.m_hWnd) ) {
		//	if ( nType == 0) {
		//		if ( IsFavBarVisibleNormal() )
		//			m_FavBar.m_view.RefreshRootTree();
		//		else
		//			m_bRemakeFavBarList = true; 	//+++ 隠れている場合なら、次回表示時にRefresh
		//	} else if ( nType == 1 ) {
		//		if ( IsFavBarVisibleGroup() )
		//			m_FavBar.m_view.RefreshRootTree();
		//		//else
		//		//	m_bRemakeFavBarList = true;
		//	}
		//}
	}
  #endif

private:
	void _SwitchClient(DWORD dwStyle)
	{
		//_HideOtherBars();
		//if		  ( _check_flag(DEB_EX_FAVBAR  , dwStyle) ) {	// お気に入りバー
		//	if (m_bRemakeFavBarList) {							//+++ リクエストがあったら再生成する.
		//		m_FavBar.m_view.RefreshRootTree();
		//		m_bRemakeFavBarList = false;
		//	}
		//	m_FavBar.ShowWindow(SW_SHOW);
		//	SetClient(m_FavBar.m_hWnd);
		//	SetTitle( MtlGetWindowText(m_FavBar.m_hWnd) );

		//} else if ( _check_flag(DEB_EX_CLPBAR  , dwStyle) ) {	// クリップバー
		//	m_ClipBar.ShowWindow(SW_SHOW);
		//	SetClient(m_ClipBar.m_hWnd);
		//	SetTitle( MtlGetWindowText(m_ClipBar.m_hWnd) );

		//} else if ( _check_flag(DEB_EX_PANELBAR, dwStyle) ) {	// パネルバー
		//	m_PanelBar.ShowWindow(SW_SHOW);
		//	SetClient(m_PanelBar.m_hWnd);
		//	SetTitle( MtlGetWindowText(m_PanelBar.m_hWnd) );

		//} else if ( _check_flag(DEB_EX_PLUGINBAR, dwStyle) ) {	// プラグインバー
		//	m_PluginBar.ShowWindow(SW_SHOW);
		//	SetClient(m_PluginBar.m_hWnd);
		//	SetTitle( MtlGetWindowText(m_PluginBar.m_hWnd) );
		//}
	}


	void _HideOtherBars()
	{
		//if (m_FavBar.m_hWnd    != NULL) 	m_FavBar.ShowWindow(SW_HIDE);
		//if (m_ClipBar.m_hWnd   != NULL) 	m_ClipBar.ShowWindow(SW_HIDE);
		//if (m_PanelBar.m_hWnd  != NULL) 	m_PanelBar.ShowWindow(SW_HIDE);
		//if (m_PluginBar.m_hWnd != NULL) 	m_PluginBar.ShowWindow(SW_HIDE);
	}


	void _CreateOnDemandBar(DWORD dwExStyle)
	{
		// お気に入りツリー, (履歴の場合もあり)
		if ( _check_flag(DEB_EX_FAVBAR, dwExStyle) ) {			// on demand
			//if (!m_FavBar.m_hWnd) {
			//	m_FavBar.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
			//	m_FavBar.SetWindowText( _T("お気に入り") );
			//	m_FavBar.InitToolBar( ID_FAVORITE_ADD, ID_FAVORITE_ORGANIZE, ID_FAVORITE_PLACEMENT, IDB_FAVBAR, IDB_FAVBAR_HOT,
			//						 16, 16, RGB(255, 0, 255) );
			//}
		}

		// クリップボード
		else if ( _check_flag(DEB_EX_CLPBAR, dwExStyle) ) {
			if (!m_ClipBar.m_hWnd) {
				m_ClipBar.Create(m_hWnd);
				m_ClipBar.SetWindowText( _T("クリップボード") );
			}
		}

		// パネルバー
		else if ( _check_flag(DEB_EX_PANELBAR, dwExStyle) ) {
			if (!m_PanelBar.m_hWnd) {
				m_PanelBar.CreatePanelBar(m_hWnd, TRUE);
				m_PanelBar.SetWindowText( _T("パネルバー") );
			}
		}

		// プラグインバー
		else if ( _check_flag(DEB_EX_PLUGINBAR, dwExStyle) ) {
			if (!m_PluginBar.m_hWnd) {
			  #if _ATL_VER >= 0x700
				m_PluginBar.Create(m_hWnd);
			  #else //+++
				static RECT dmy = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };
				m_PluginBar.Create(m_hWnd, dmy);
			  #endif
				m_PluginBar.SetWindowText( _T("プラグインバー") );
			}
		}
	}


public:
	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP( CDonutExplorerBar )
		for (int ii = 0; ii < (int) m_aryID.size(); ii++) {
			int nIndexAct = m_nIndexAct;

			switch (m_aryID[ii]) {
			case ID_VIEW_FAVEXPBAR: 		if ( IsFavBarVisibleNormal() )	m_nIndexAct = ii;	break;
			case ID_VIEW_FAVEXPBAR_HIST:	if ( IsFavBarVisibleHist()	 )	m_nIndexAct = ii;	break;
			case ID_VIEW_FAVEXPBAR_GROUP:	if ( IsFavBarVisibleGroup()  )	m_nIndexAct = ii;	break;
			case ID_VIEW_FAVEXPBAR_USER:	if ( IsFavBarVisibleUser()	 )	m_nIndexAct = ii;	break;
			case ID_VIEW_FAVEXPBAR_SCRIPT:	if ( IsFavBarVisibleScript() )	m_nIndexAct = ii;	break;
			case ID_VIEW_CLIPBOARDBAR:		if ( IsClipboardBarVisible() )	m_nIndexAct = ii;	break;
			case ID_VIEW_PANELBAR:			if ( IsPanelBarVisible()	 )	m_nIndexAct = ii;	break;
			case ID_VIEW_PLUGINBAR: 		if ( IsPluginBarVisible()	 )	m_nIndexAct = ii;	break;
			}
			if (nIndexAct != m_nIndexAct) {
				Invalidate(FALSE);
				break;
			}
		}
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_FAVEXPBAR		, IsFavBarVisibleNormal()	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_FAVEXPBAR_HIST	, IsFavBarVisibleHist() 	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_FAVEXPBAR_GROUP	, IsFavBarVisibleGroup()	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_FAVEXPBAR_USER	, IsFavBarVisibleUser() 	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_FAVEXPBAR_SCRIPT , IsFavBarVisibleScript()	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_CLIPBOARDBAR 	, IsClipboardBarVisible()	)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_PANELBAR 		, IsPanelBarVisible()		)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_VIEW_PLUGINBAR		, IsPluginBarVisible()		)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_EXPLORERBAR			, IsWindowVisible() 		)	// IsFavBarVisible())
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_VIEW_EXPLORERBAR_TAB, !m_bHideTab )
//		CHAIN_UPDATE_COMMAND_UI_MEMBER(m_FavBar)
	END_UPDATE_COMMAND_UI_MAP()


private:
	void _WriteProfile()
	{
		CIniFileO	pr( g_szIniFileName, _T("Explorer_Bar") );

		pr.SetValue( m_dwExStyle, _T("Extended_Style") );
		pr.SetValue( m_wndSplit.GetSplitterPos(), _T("cxLeftSplitter") );

		DWORD		dwSplitterVisible = (m_wndSplit.GetSinglePaneMode() != SPLIT_PANE_RIGHT);
		pr.SetValue( dwSplitterVisible, _T("bLeftVisible") );
		pr.SetValue( (int) m_bHideTab, _T("TabState") );
	}


	void _GetProfile(HWND hWndMDIClient)
	{
		CIniFileI	pr( g_szIniFileName, _T("Explorer_Bar") );
		DWORD		dwcxLeftSplitter  = pr.GetValue( _T("cxLeftSplitter"), 160 );
		DWORD		dwSplitterVisible = pr.GetValue( _T("bLeftVisible"), FALSE );
		DWORD		dwExStyle		  = pr.GetValue( _T("Extended_Style"), DEB_EX_FAVBAR );
		DWORD		dwHideTab		  = pr.GetValue( _T("TabState"), 0 );
		pr.Close();
		m_bHideTab				   = dwHideTab != 0/*? TRUE : FALSE*/;

		//ShowBar(dwExStyle);

		// ここでスプリットウィンドウの設定
		m_wndSplit.SetSplitterPanes(m_hWnd, hWndMDIClient);
		//:::m_wndSplit.SetSplitterPaneParents( NULL, GetTopLevelParent() );
		m_wndSplit.m_xySplitterPos = dwcxLeftSplitter;

		if (dwSplitterVisible == FALSE) {
			HideBar();

			if (dwExStyle/* != 0*/)
				m_dwExStyle = dwExStyle;
		} else {
			ShowBar(dwExStyle);
			// SetFocus();
		}

		//minit P5.0β4の機能の模倣
		if (m_bHideTab) {
			ShowTab(FALSE);
		} else {
			ShowTab(TRUE);
		}

		SetWindowText( _T("エクスプローラバー") );
	}


public:
	bool IsFavBarVisibleNormal()
	{
		if (!IsWindowVisible() || IsFavBarVisible() == false)
			return false;
		_CreateOnDemandBar(m_dwExStyle);
//		return (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITES) != 0;
		return true;
	}


private:
	bool IsFavBarVisibleScript()
	{
		if (!IsWindowVisible() || IsFavBarVisible() == false)
			return false;
		_CreateOnDemandBar(m_dwExStyle);
//		return (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_SCRIPT) != 0;
		return true;
	}


	bool IsFavBarVisibleHist()
	{
		if (!IsWindowVisible() || IsFavBarVisible() == false)
			return false;
		_CreateOnDemandBar(m_dwExStyle);
//		return (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_HISTORY) != 0;
		return true;
	}


public:
	bool IsFavBarVisibleGroup()
	{
		if (!IsWindowVisible() || IsFavBarVisible() == false)
			return false;
		_CreateOnDemandBar(m_dwExStyle);
//		return (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_FAVORITEGROUP) != 0;
		return true;
	}


private:
	bool IsFavBarVisibleUser()
	{
		if (!IsWindowVisible() || IsFavBarVisible() == false)
			return false;
		_CreateOnDemandBar(m_dwExStyle);
//		return (m_FavBar.m_view.m_dwExplorerTreeViewExtendedStyle & ETV_EX_USERDEFINED) != 0;
		return true;
	}


	void UpSendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
	  #if _ATL_VER >= 0x700
		GetTopLevelParent().SendMessage(message, wParam, lParam);
	  #else //+++
		CWindow( GetTopLevelParent() ).SendMessage(message, wParam, lParam);
	  #endif
	}


	bool Show()
	{
		ShowBar(m_dwExStyle);
		return true;
	}
};


__declspec(selectany) CDonutExplorerBar*	CDonutExplorerBar::s_pThis_;



