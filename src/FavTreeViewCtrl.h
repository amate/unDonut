/**
 *	@file	FavTreeViewCtrl.h
 *	@brief	お気に入りメニュー,お気に入りグループ,履歴,ユーザー定義,デスクトップ・スクリプト 関係.
 */

#pragma once

#include "OleDragDropExpTreeViewCtrl.h"
#include "MtlCtrls.h"
#include "ChevronHandler.h"
#include <URLHIST.H>
#include <shlguid.h>				// needed for CLSID_CUrlHistory
#include <atldef.h>
#include "comdef.h"
#include "wtypes.h"
#include "SearchHistoryDialog.h"
#include "DonutFavoritesMenu.h"
#include "option/MainOption.h"

//GetUrlCacheEntryInfo


class CFavoritesTreeViewCtrl : public COleDragDrogExplorerTreeViewCtrlImpl<CFavoritesTreeViewCtrl> 
{
public:
	DECLARE_WND_SUPERCLASS( _T("Mtl_FavTreeViewCtrl"), GetWndClassName() )
private:
	typedef COleDragDrogExplorerTreeViewCtrlImpl<CFavoritesTreeViewCtrl>  baseClass;
	typedef CFavoritesTreeViewCtrl										  thisClass;

	// Constants
	enum { s_nIDOpenAll = 10, };


	// Ctor/dtor

	// Attributes
	bool		_IsNeedFavoritesFix();

public:
	// Overrides
	void		OnExplorerTreeViewInit();
	void		OnExplorerTreeViewTerm();
	CItemIDList OnInitRootFolder();
	void		OnFolderEmpty();	// U.H
	void		OnInitialUpdateTreeItems(CSimpleArray<TV_INSERTSTRUCT> &items, LPCITEMIDLIST pidl);
	void		OnTreeItemClicked(HTREEITEM hTreeItem, UINT uFlags);
	bool		OnGetInfoToolTip(HTREEITEM hItem, CString &strTip, int cchTextMax);
	bool		OnCmdContextMenu(HTREEITEM hTreeItem, int nID);
	UINT		OnPrepareMenuForContext(HTREEITEM hTreeItem, CMenuHandle menu, int &nPos);
	void		OnPrepareMenuForPopup(HTREEITEM hTreeItem, CMenuHandle menu);

private:
	void		OnChangeImageList();
	bool		OnCheckContextItem(HTREEITEM hTreeItem);

	HTREEITEM	AddItem(CString strText);

	void		_GetProfile();
	void		_WriteProfile();
};



template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CDonutFavoritesBarImpl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public CChevronHandler<CDonutFavoritesBarImpl< T, TBase, TWinTraits > >
	, public CUpdateCmdUI<CDonutFavoritesBarImpl< T, TBase, TWinTraits > >
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

private:
	typedef CDonutFavoritesBarImpl<T, TBase, TWinTraits>  thisClass;

	// Constants
	enum {
		s_cxyBorder    = 2,
		s_cxyBtnOffset = 1,
		s_cxyViewGap   = 1,
	};

public:
	// Data members
	CToolBarCtrl		   m_wndToolBar;
	CReBarCtrl			   m_wndReBar;
	CFavoritesTreeViewCtrl m_view;
private:
	int 				   m_cxyHeader;
	CMenu				   m_menuPlacement;

	BOOL				   m_bShow;

public:
	// Ctor
	CDonutFavoritesBarImpl()
	  #if 1 //+++
		: m_wndToolBar()
		, m_wndReBar()
		, m_view()
		, m_cxyHeader(0)
		, m_menuPlacement()
		, m_bShow(0)
	  #endif
	{
		m_menuPlacement.LoadMenu(IDR_MENU_FAVTREE_BAR);
	}


private:
	// Overrides
	HMENU OnGetDropDownMenu(int nCmdID, HMENU &hMenuDestroy)
	{
		return m_menuPlacement.GetSubMenu(0);
	}


	BOOL OnToolbarReconstruct()
	{
	}


public: // Methods
	BYTE PreTranslateMessage(MSG *pMsg)
	{
		UINT msg  = pMsg->message;
		int  vKey =  (int) pMsg->wParam;

		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
			if ( m_view.m_hWnd && pMsg->hwnd == m_view.GetEditControl() )
				return _MTL_TRANSLATE_WANT;

			// pass to rename edit ctrl

			if ( pMsg->hwnd != m_view.m_hWnd || !m_view.IsWindowVisible() )
				return _MTL_TRANSLATE_PASS;

			if ( vKey == VK_LEFT || vKey == VK_RIGHT || vKey == VK_DOWN || vKey == VK_UP
				|| vKey == VK_RETURN || vKey == VK_SPACE)
			{
				return _MTL_TRANSLATE_WANT; 	// pass to control
			}
		}

		return _MTL_TRANSLATE_PASS;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutFavoritesBarImpl)
		MESSAGE_HANDLER(WM_CREATE		, OnCreate			)
		MESSAGE_HANDLER(WM_SIZE 		, OnSize			)
		MESSAGE_HANDLER(WM_SETFOCUS 	, OnSetFocus		)
		MESSAGE_HANDLER(WM_DESTROY		, OnDestroy 		)
		MESSAGE_HANDLER(WM_PAINT		, OnPaint			)
		MESSAGE_HANDLER(WM_ERASEBKGND	, OnEraseBackground )
		MESSAGE_HANDLER(WM_SHOWWINDOW	, OnShowWindow		)

		COMMAND_ID_HANDLER(ID_FAVORITE_ADD			, OnFavoriteAdd 	 )
		COMMAND_ID_HANDLER(ID_FAVORITE_ORGANIZE 	, OnFavoriteOrganize )
		COMMAND_ID_HANDLER(ID_FAVORITE_PLACEMENT	, OnFavoritePlacement)
		COMMAND_ID_HANDLER(ID_VIEW_REFRESH_FAVBAR	, OnFavoritePlacement)

		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_SCRIPT	, OnFavTreeScript	 )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_STANDARD	, OnFavTreeStandard  )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_GROUP 	, OnFavTreeGroup	 )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_USER		, OnFavTreeUser 	 )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_MYCOMPUTER, OnFavTreeMyComputer)

		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_SHELLEXECUTE	, OnFavTreeShellExecute  )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_LOADREMOTEICON, OnFavTreeLoadRemoteIcon)
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_SINGLEEXPAND	, OnFavTreeSingleExpand  )
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_HISTORY		, OnFavTreeHistory		 )	// UDT DGSTR りれき
		COMMAND_ID_HANDLER(ID_FAVTREE_BAR_HISTORY_TIME	, OnFavTreeHistoryTime	 )

		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnTbnDropDown)

		CHAIN_MSG_MAP_MEMBER(m_view);

		CHAIN_MSG_MAP(CChevronHandler<CDonutFavoritesBarImpl>)
		CHAIN_MSG_MAP(CUpdateCmdUI<CDonutFavoritesBarImpl>)
		// FORWARD_NOTIFICATIONS() // 有効にするとエラーが出る
	END_MSG_MAP()


private:
	LRESULT OnFavTreeStandard(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL	bNowThis = _check_flag( ETV_EX_FAVORITES, m_view.GetExplorerTreeViewExtendedStyle() ) ;

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_FAVORITES);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("お気に入り"), (LPARAM) 0);			// UDT DGSTR りれき

		//m_wndToolBar.UpdateWindow();
		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}


	LRESULT OnFavTreeGroup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bNowThis = _check_flag( ETV_EX_FAVORITEGROUP, m_view.GetExplorerTreeViewExtendedStyle() );

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_FAVORITEGROUP);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("お気に入りグループ"), (LPARAM) 0);	// UDT DGSTR りれき

		m_wndToolBar.UpdateWindow();

		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}


	LRESULT OnFavTreeUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bNowThis = _check_flag( ETV_EX_USERDEFINED, m_view.GetExplorerTreeViewExtendedStyle() );

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_USERDEFINED);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("ユーザー定義"), (LPARAM) 0);		// UDT DGSTR りれき

		m_wndToolBar.UpdateWindow();

		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}


	LRESULT OnFavTreeMyComputer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bNowThis = _check_flag( ETV_EX_MYCOMPUTER, m_view.GetExplorerTreeViewExtendedStyle() );

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_MYCOMPUTER);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("デスクトップ"), (LPARAM) 0);		// UDT DGSTR りれき

		m_wndToolBar.UpdateWindow();

		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}


	// UDT DGSTR りれき
	LRESULT OnFavTreeHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bNowThis = _check_flag( ETV_EX_HISTORY, m_view.GetExplorerTreeViewExtendedStyle() );

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_HISTORY);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("履歴"), (LPARAM) 0);				// UDT DGSTR

		m_wndToolBar.UpdateWindow();

		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}
	// ENDE


	// U.H
	LRESULT OnFavTreeHistoryTime(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( _check_flag( ETV_EX_HISTORY_TIME, m_view.GetExplorerTreeViewExtendedStyle() ) )
			m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_HISTORY_TIME, 0);
		else
			m_view.ModifyExplorerTreeViewExtendedStyle(0, ETV_EX_HISTORY_TIME);

		m_wndToolBar.UpdateWindow();
		m_view.RefreshRootTree();

		T *pT = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}


	LRESULT OnFavTreeScript(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		BOOL bNowThis = _check_flag( ETV_EX_SCRIPT, m_view.GetExplorerTreeViewExtendedStyle() ) ;

		m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_FOLDERS, ETV_EX_SCRIPT);

		SendMessage(GetTopLevelParent(), WM_UPDATE_EXPBAR, (WPARAM) _T("スクリプト"), (LPARAM) 0);

		m_wndToolBar.UpdateWindow();

		if (!bNowThis)
			m_view.RefreshRootTree();

		T *  pT 	  = static_cast<T *>(this);
		pT->OnToolbarReconstruct();

		return 0;
	}
	// ^^^


	LRESULT OnFavTreeShellExecute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( _check_flag( ETV_EX_SHELLEXECUTE, m_view.GetExplorerTreeViewExtendedStyle() ) )
			m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_SHELLEXECUTE, 0);
		else
			m_view.ModifyExplorerTreeViewExtendedStyle(0, ETV_EX_SHELLEXECUTE);

		return 0;
	}


	LRESULT OnFavTreeLoadRemoteIcon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( _check_flag( ETV_EX_LOADREMOTEICON, m_view.GetExplorerTreeViewExtendedStyle() ) )
			m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_LOADREMOTEICON, 0);
		else
			m_view.ModifyExplorerTreeViewExtendedStyle(0, ETV_EX_LOADREMOTEICON);

		m_wndToolBar.UpdateWindow();
		m_view.RefreshRootTree();

		return 0;
	}


	LRESULT OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		::SendMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) ID_FAVORITE_ADD, 0);
		return 0;
	}


	LRESULT OnFavoriteOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		::SendMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM) ID_FAVORITE_ORGANIZE, 0);
		return 0;
	}


	LRESULT OnFavoritePlacement(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		m_wndToolBar.UpdateWindow();
		m_view.RefreshRootTree();
		return 0;
	}


	LRESULT OnFavTreeSingleExpand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( _check_flag( ETV_EX_SINGLEEXPAND, m_view.GetExplorerTreeViewExtendedStyle() ) )
			m_view.ModifyExplorerTreeViewExtendedStyle(ETV_EX_SINGLEEXPAND, 0);
		else
			m_view.ModifyExplorerTreeViewExtendedStyle(0, ETV_EX_SINGLEEXPAND);

		m_wndToolBar.UpdateWindow();
		m_view.RefreshRootTree();

		return 0;
	}


	LRESULT OnTbnDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		CRect	  rc;
		m_wndToolBar.GetItemRect(0, &rc);

		ClientToScreen(&rc);
		UINT	  uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN |
							   (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
		TPMPARAMS TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rc;

		::TrackPopupMenuEx(m_menuPlacement.GetSubMenu(0), uMenuFlags, rc.left + 1, rc.bottom + 2, m_hWnd, &TPMParams);

		return TBDDRET_DEFAULT;
	}


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		T * 	 pT = static_cast<T *>(this);

		pT->DrawPaneTitle(dc.m_hDC);

		return 0;
	}


	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		return 1;											// no background needed
	}


	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		m_bShow = (BOOL) wParam;
		return 0;
	}


public:
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled = FALSE;
		MtlDestroyImageLists(m_hWnd);
		return 0;
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		DWORD dwStyle =   WS_CHILD		   | WS_VISIBLE 	   | WS_CLIPSIBLINGS   | WS_CLIPCHILDREN
						| TVS_INFOTIP	   | TVS_EDITLABELS    | TVS_SHOWSELALWAYS
						| TVS_SINGLEEXPAND | TVS_FULLROWSELECT | TVS_TRACKSELECT   ;

		//水平スクロールを使用するかどうか
		//if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_HSCROLL) != MAIN_EXPLORER_HSCROLL )
		//	dwStyle |= TVS_NOHSCROLL;

		////ドラッグ＆ドロップを使用するかどうか
		//if (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_NODRAGDROP)
		//	m_view.m_bDragDrop = FALSE;
		//else
			m_view.m_bDragDrop = TRUE;

		m_view.Create(m_hWnd, rcDefault, NULL, dwStyle, 0); //WS_EX_STATICEDGE);

		m_wndReBar = _CreateSimpleReBarCtrl(m_hWnd, ATL_SIMPLE_REBAR_NOBORDER_STYLE | CCS_NOPARENTALIGN);

		m_bShow    = TRUE;
		return 0;
	}


private:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
	{
		CSize size(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		UpdateLayout( size.cx, size.cy );
		
		CRect rect;
		m_wndReBar.GetRect(0, &rect);
		// ボーダー分 + タブ分(-20)だけずらす
		m_view.MoveWindow(1, rect.bottom + 3, size.cx - 3, size.cy - rect.bottom - 5/* - 20*/);
		return 0;
	}


	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		if (m_view.m_hWnd != NULL)
			m_view.SetFocus();

		return 0;
	}


	CString GetSkinFavBarPath(BOOL bHot)
	{
		CString strBmp;

		if (bHot)
			strBmp = _T("FavBarHot.bmp");
		else
			strBmp = _T("FavBar.bmp");

		return _GetSkinDir() + strBmp;
	}


	void ReloadSkin()
	{
		if ( !m_wndToolBar.IsWindow() )
			return;

		CImageList	imgs	= m_wndToolBar.GetImageList();
		CImageList	imgsHot = m_wndToolBar.GetHotImageList();

		_ReplaceImageList(GetSkinFavBarPath(FALSE), imgs);
		_ReplaceImageList(GetSkinFavBarPath(TRUE), imgsHot);
		m_wndToolBar.InvalidateRect(NULL, TRUE);
	}


public:
	void InitToolBar(
			int 		nIDAdd,
			int 		nIDOrganize,
			int 		nIDSpecial,
			UINT		nImageBmpID,
			UINT		nHotImageBmpID,
			int 		cx,
			int 		cy,
			COLORREF	clrMask,
			UINT		nFlags	= ILC_COLOR24)
	{
		CImageList imgs;

		MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmp;
		imgs.Add(bmp, clrMask);

		CImageList imgsHot;
		MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmpHot;
		imgsHot.Add(bmpHot, clrMask);

		m_wndToolBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 0);
		m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
		ATLASSERT( m_wndToolBar.IsWindow() );
		m_wndToolBar.SetButtonStructSize( sizeof (TBBUTTON) );

		m_wndToolBar.SetImageList(imgs);
		m_wndToolBar.SetHotImageList(imgsHot);

		T * 	   pT = static_cast<T *>(this);
		pT->OnToolbarReconstruct();
		_AddSimpleReBarBandCtrl(m_wndReBar, m_wndToolBar);
	}


private:
	void UpdateLayout(int cxWidth, int cyHeight)
	{
		ATLASSERT( ::IsWindow(m_hWnd) );
		CRect	  rect(s_cxyBorder, s_cxyBorder, cxWidth - s_cxyBorder * 2, m_cxyHeader);

		if (m_wndReBar.m_hWnd != NULL)
			m_wndReBar.MoveWindow(rect);

		// I can't get the reason why I have to update now without InvalidateRect.
		// InvalidateRect(&rect); can't update a rect, correctly.
		CClientDC dc(m_hWnd);
		T * 	  pT = static_cast<T *>(this);
		pT->DrawPaneTitle(dc.m_hDC);
	}


public:
	void CalcSize()
	{
		CRect rcBtn;

		m_wndToolBar.GetItemRect(0, &rcBtn);
		m_cxyHeader = rcBtn.Height() + s_cxyBorder;
	}


private:
	void DrawPaneTitle(CDCHandle dc)
	{
		CRect rect;

		GetClientRect(&rect);

		CPen  pen;
		pen.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW) );
		POINT pts[]   = { { rect.right - 3, rect.top + m_cxyHeader },
						  { rect.left	  , rect.top + m_cxyHeader },
						  { rect.left	  , rect.bottom - 2 	   } }; //, { rect.right - 3, rect.bottom - 1 } };

		HPEN  hOldPen = dc.SelectPen(pen);
		dc.Polyline( pts, _countof(pts) );
		dc.SelectPen(hOldPen);

		dc.DrawEdge(CRect(rect.left, rect.top + m_cxyHeader, rect.right, rect.bottom),
					EDGE_ETCHED, BF_RIGHT | BF_BOTTOM);

		rect.bottom = rect.top + m_cxyHeader;
		dc.DrawEdge(&rect, EDGE_ETCHED, BF_LEFT | BF_TOP | BF_RIGHT | BF_ADJUST);
		dc.FillRect( &rect, (HBRUSH) LongToPtr(COLOR_3DFACE + 1) );
	}



public:
	BEGIN_UPDATE_COMMAND_UI_MAP(CDonutFavoritesBarImpl)
		//if(!m_bShow) return FALSE;
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_SCRIPT		  , ETV_EX_SCRIPT		, m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_MYCOMPUTER	  , ETV_EX_MYCOMPUTER	, m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_STANDARD	  , ETV_EX_FAVORITES	, m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_GROUP		  , ETV_EX_FAVORITEGROUP, m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_USER		  , ETV_EX_USERDEFINED	, m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_SHELLEXECUTE  , ETV_EX_SHELLEXECUTE , m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_LOADREMOTEICON, ETV_EX_LOADREMOTEICON,m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_SINGLEEXPAND  , ETV_EX_SINGLEEXPAND , m_view.GetExplorerTreeViewExtendedStyle() )
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_HISTORY 	  , ETV_EX_HISTORY		, m_view.GetExplorerTreeViewExtendedStyle() )	// UDT DGSTR りれき
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_FAVTREE_BAR_HISTORY_TIME  , ETV_EX_HISTORY_TIME , m_view.GetExplorerTreeViewExtendedStyle() )	// U.H
	END_UPDATE_COMMAND_UI_MAP()
};



///////////////////////////////////////////////////////////////////////////////
// CDonutFavoritesBar : エクスプローラーバーのお気に入りの上にあるツールバー

class CDonutFavoritesBar : public CDonutFavoritesBarImpl<CDonutFavoritesBar> 
{
public:
	DECLARE_WND_CLASS_EX(_T("Donut_FavoritesBar"), 0, -1)

public:
	CDonutFavoritesBar();

	BEGIN_MSG_MAP(CDonutFavoriteBar)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN			, OnTbnDropDown 		)
		MESSAGE_HANDLER   ( WM_DESTROY				, OnDestroy 			)
		COMMAND_ID_HANDLER( ID_SEARCH_HISTORY		, OnSearchHistory		)
		COMMAND_ID_HANDLER( ID_SELECT_USERFOLDER	, OnSelectUserFolder	)
		COMMAND_ID_HANDLER( ID_VIEW_HISTORY_DAY 	, OnViewHistoryDay		)
		COMMAND_ID_HANDLER( ID_VIEW_HISTORY_RECENT	, OnViewHistoryRecent	)

	  #if 1 //+++ こちらでいいのかわからないが、抜けてるので追加.
		COMMAND_ID_HANDLER(ID_FAVORITE_GROUP_SAVE	, OnFavoriteGroupSave	)
		COMMAND_ID_HANDLER(ID_FAVORITE_GROUP_ADD	, OnFavoriteGroupAdd	)
		COMMAND_ID_HANDLER(ID_FAVORITE_GROUP_ORGANIZE, OnFavoriteGroupOrganize)
	  #endif

		CHAIN_MSG_MAP(CDonutFavoritesBarImpl<CDonutFavoritesBar>)
	END_MSG_MAP()


	void		ReloadSkin();
	BOOL		OnToolbarReconstruct();

private:
	void		_AddNewButton(int nID, LPTSTR lpText, int nImage, int nStyleSpecial);
	void		_InsertButton(int nID, int nStyleSpecial, int nImage);
	void		_SetButtonText(int nID, LPTSTR lpstr);

	void		SplitKeyWord(CSimpleArray<CString> &arystr, CString strWord);
	void		InsertItem(CString &strTitle, CString &strUrl);

	LRESULT 	 OnSearchHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	 OnSelectUserFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	 OnTbnDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL & bHandled);
	LRESULT 	 OnViewHistoryDay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	 OnViewHistoryRecent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

  #if 1 //+++ こちらでいいのかわからないが、抜けてるので追加.
	LRESULT 	OnFavoriteGroupSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnFavoriteGroupOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
  #endif
};


