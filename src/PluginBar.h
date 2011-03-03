/**
 *	@file	PluginBar.h
 *	@brief	プラグインバー
 */

#pragma once

#include "./include/PluginInfo.h"
#include "ChevronHandler.h"

#include "PluginManager.h"
#include "MtlWin.h"
#include "MtlCtrls.h"
#include "IniFile.h"
#include "DonutPFunc.h"
#include "resource.h"


class CPluginBar
	: public CWindowImpl<CPluginBar>
	, public CChevronHandler<CPluginBar>
{
private:
	//typedef CWindowImpl<CPluginBar> baseClass;

	//メンバ変数
	HWND			m_hWndCur;		//今表示しているプラグインのウィンドウハンドル
	int 			m_nCurIndex;	//そのプラグインのインデックス番号

	CReBarCtrl		m_wndReBar; 	//リバー(ツールバー一つしかないから不要かも)
	CToolBarCtrl	m_wndToolBar;	//ツールバー


public:
	DECLARE_WND_CLASS_EX(_T("Donut_PluginBar"), CS_DBLCLKS, COLOR_WINDOW)

	//コンストラクタ
	CPluginBar()
		: m_hWndCur(NULL)
		, m_nCurIndex(-1)
	{
	}

private:
	//メソッド

	/// プラグインを表示(なければ作る)
	///
	void ShowBar(int nIndex)
	{
		if (CPluginManager::GetCount(PLT_EXPLORERBAR) == 0)
			return;

		if (m_nCurIndex == nIndex)
			return;

		if ( !IsWindowVisible() )
			ShowWindow(SW_SHOWNORMAL);

		HWND hWndNew	= CPluginManager::GetHWND(PLT_EXPLORERBAR, nIndex);

		if (!hWndNew) {
			//ないので作る
			if ( !CPluginManager::LoadPlugin(PLT_EXPLORERBAR, nIndex, m_hWnd) )
				return;

			hWndNew = CPluginManager::GetHWND(PLT_EXPLORERBAR, nIndex);

			if (!hWndNew)
				return;
		}

		HWND hWndBefore = m_hWndCur;

		//状態変数を変更
		m_nCurIndex = nIndex;
		m_hWndCur	= hWndNew;

		//リサイズして表示
		UpdateLayout();
		::ShowWindow(m_hWndCur, SW_SHOWNORMAL);

		//今表示しているプラグインを隠す
		if ( ::IsWindow(hWndBefore) ) {
			::ShowWindow(hWndBefore, SW_HIDE);
		}

		//フォーカスを移す
		::SetFocus(hWndNew);
	}


	void HideBar()
	{
		ShowWindow(SW_HIDE);
	}


public:
	BYTE PreTranslateMessage(MSG *pMsg)
	{
		//キー関連のメッセージを転送
		if ( (WM_KEYFIRST <= pMsg->message) && (pMsg->message <= WM_KEYLAST) ) {
			int nCount = CPluginManager::GetCount(PLT_EXPLORERBAR);

			for (int i = 0; i < nCount; i++) {
				if ( CPluginManager::Call_PreTranslateMessage(PLT_EXPLORERBAR, i, pMsg) )
					return _MTL_TRANSLATE_HANDLE;
			}
		}

		return _MTL_TRANSLATE_PASS;
	}


	void ReloadSkin()
	{
		if ( !m_wndToolBar.IsWindow() )
			return;

		CImageList imgs, imgsHot;
		imgs	= m_wndToolBar.GetImageList();
		imgsHot = m_wndToolBar.GetHotImageList();

		_ReplaceImageList(GetSkinFavBarPath(FALSE), imgs);
		_ReplaceImageList(GetSkinFavBarPath(TRUE) , imgsHot);
		m_wndToolBar.InvalidateRect(NULL		  , TRUE);
	}


public:
	//メッセージマップ
	BEGIN_MSG_MAP(CPluginBar)
		MESSAGE_HANDLER(WM_CREATE		, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY		, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE 		, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND	, OnEraseBackGround)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnTbnDropDown)
		//CHAIN_MSG_MAP(baseclass)
		CHAIN_MSG_MAP(CChevronHandler<CPluginBar>)
	END_MSG_MAP()


private:
	//メッセージハンドラ
	LRESULT CPluginBar::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled = FALSE;
		CString 	strSection;
		strSection.Format(_T("Plugin%02d"), PLT_EXPLORERBAR);
		DWORD		dwIndex = 0;
		{
			CIniFileI pr(g_szIniFileName, strSection);
			pr.QueryValue( dwIndex, _T("SelIndex") );
			//x pr.Close(); //++++
		}

		InitToolBar();
		CPluginManager::ReadPluginData(PLT_EXPLORERBAR, m_hWnd); //プラグイン情報読み込み
	  #if 1	//+++
		CPluginManager::ReadPluginData(PLT_DOCKINGBAR , m_hWnd); //プラグイン情報読み込み
	  #endif
		ShowBar(dwIndex);

		return 0;
	}


	LRESULT CPluginBar::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled = FALSE;
		//プラグインの後片付けはCMainFrameで一括して行う
		//CPluginManager::DeleteAllPlugin(PLT_EXPLORERBAR); //プラグイン解放

		CString 	strKey;
		strKey.Format(_T("Plugin%02d"), PLT_EXPLORERBAR);
		CIniFileO	pr( g_szIniFileName, strKey);
		pr.SetValue( m_nCurIndex, _T("SelIndex") );
		//x pr.Close(); 	//+++

		return 0;
	}


	LRESULT CPluginBar::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
	{
		UpdateLayout();
		return 0;
	}


	LRESULT CPluginBar::OnEraseBackGround(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		if ( !::IsWindow(m_hWndCur) )
			return DefWindowProc(uMsg, wParam, lParam);

		return 1;
	}


	//内部関数
	void CPluginBar::UpdateLayout()
	{
		CRect rc;

		GetClientRect(&rc);
		UpdateLayout( rc.Width(), rc.Height() );
	}


	void CPluginBar::UpdateLayout(int nWidth, int nHeight)
	{
		CRect rcToolBar;

		m_wndReBar.GetClientRect(&rcToolBar);
		m_wndReBar.MoveWindow( 0, 0, nWidth, rcToolBar.Height() );

		int   nOffsetY = rcToolBar.Height();
		::MoveWindow(m_hWndCur, 0, nOffsetY, nWidth, nHeight - nOffsetY, TRUE);	
	}


	void CPluginBar::DelIniKey(CString strKey)
	{
		CString 	strSection;

		strSection.Format( _T("Plugin%02d"), PLT_EXPLORERBAR );

		CIniFileO	pr( g_szIniFileName, strSection );
		pr.DeleteValue( strKey );
		//x pr.Close(); 	//+++
	}


	void CPluginBar::InitToolBar()
	{
		int 	cx , cy;
		int 	nFlags	= ILC_COLOR24 | ILC_MASK;
		cx	 = cy = 16;

		m_wndReBar = _CreateSimpleReBarCtrl(m_hWnd, ATL_SIMPLE_REBAR_NOBORDER_STYLE | CCS_NOPARENTALIGN);

		CImageList	imgs;
		CImageList	imgsHot;
		imgs.Create   (cx, cy, nFlags , 1, 1);
		imgsHot.Create(cx, cy, nFlags , 1, 1);

		m_wndToolBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 0);
		m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
		ATLASSERT( m_wndToolBar.IsWindow() );
		m_wndToolBar.SetButtonStructSize( sizeof (TBBUTTON) );

		m_wndToolBar.SetImageList(imgs);
		m_wndToolBar.SetHotImageList(imgsHot);

		TBBUTTON	btnPlg	= { 4		   , ID_PLUGIN_PLACEMENT , TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN, 0, 0 };
		TBBUTTON	btnMenu = { I_IMAGENONE, ID_PLUGIN_MENU 	 , TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN, 0, 0 };
		m_wndToolBar.InsertButton(-1, &btnPlg);
		m_wndToolBar.InsertButton(-1, &btnMenu);

		CVersional<TBBUTTONINFO> bi;
		bi.dwMask  = TBIF_TEXT;
		bi.pszText = _T("リスト");
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_PLUGIN_PLACEMENT, &bi) );
		bi.pszText = _T("メニュー");
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_PLUGIN_MENU, &bi) );

		_AddSimpleReBarBandCtrl(m_wndReBar, m_wndToolBar);

		ReloadSkin();
	}


	CString CPluginBar::GetSkinFavBarPath(BOOL bHot)
	{
		CString strBmp;

		if (bHot)
			strBmp = _T("FavBarHot.bmp");
		else
			strBmp = _T("FavBar.bmp");

		return _GetSkinDir() + strBmp;
	}


	LRESULT CPluginBar::OnTbnDropDown(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;

		int 		nID    = lpnmtb->iItem;
		int 		index  = -1;
		int 		nCount = m_wndToolBar.GetButtonCount();

		for (int i = 0; i < nCount; i++) {
			TBBUTTON tb;
			m_wndToolBar.GetButton(i, &tb);

			if (nID == tb.idCommand) {
				index = i;
				break;
			}
		}

		if (index == -1)
			return 0;

		CRect		rc;
		m_wndToolBar.GetItemRect(index, &rc);
		ClientToScreen(&rc);

		if (nID == ID_PLUGIN_PLACEMENT) {
			CMenu menu		   = GetPluginListMenu();
			int   nNewSelIndex = menu.TrackPopupMenu(
										TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
										rc.left   + 1,
										rc.bottom + 2,
										m_hWnd );

			if (nNewSelIndex > 0)
				ShowBar(nNewSelIndex - 1);
		} else if (nID == ID_PLUGIN_MENU) {
			ShowExplorerMenu(rc.left + 1, rc.bottom + 2);
		}

		return TBDDRET_DEFAULT;
	}


	CMenuHandle CPluginBar::GetPluginListMenu()
	{
		CMenuHandle 	menu;
		menu.CreatePopupMenu();

		CString 	strCap;
		int 		nCount = CPluginManager::GetCount(PLT_EXPLORERBAR);
		for (int i = 0; i < nCount; i++) {
			strCap = CPluginManager::GetCaption(PLT_EXPLORERBAR, i);
			menu.AppendMenu(MF_ENABLED | MF_STRING, i + 1, strCap);
		}
		menu.CheckMenuItem(m_nCurIndex, MF_BYPOSITION | MF_CHECKED);

		return menu;
	}


	void CPluginBar::ShowExplorerMenu(int x, int y)
	{
		if (m_nCurIndex == -1)
			return;
		CPluginManager::Call_ShowExplorerMenu(m_nCurIndex, x, y);
	}

};
