/**
 *	@file	DonutToolBar.h
 *	@brief	ツールバー
 */
#pragma once

#include "StdToolBarCtrl.h"
#include "ChevronHandler.h"


///////////////////////////////////////////////////////////////////
// CDonutToolBar

class CDonutToolBar
	: public CStdToolBarCtrlImpl<CDonutToolBar>
	, public CChevronHandler<CDonutToolBar>
{
public:
	DECLARE_WND_SUPERCLASS( _T("Donut_ToolBar"), GetWndClassName() )

	// Ctor
	CDonutToolBar();
	void DonutToolBar_SetFavoritesMenu(HMENU hMenu, HMENU hMenuUser, HMENU hMenuCSS);

	// Message map and handelrs
	BEGIN_MSG_MAP(CDonutToolBar)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnToolTipText)
		REFLECTED_NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		CHAIN_MSG_MAP(CStdToolBarCtrlImpl<CDonutToolBar>)
	END_MSG_MAP()

	// Overrides
	HMENU			ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy);
	void			Chevronhandler_OnCleanupChevronMenu();

	LRESULT 		StdToolBar_OnDropDown(int nCmdID);
	void			StdToolBar_WriteProfile();
	void			StdToolBar_GetProfile();

	// Methods
	void			ReloadSkin();
	HWND			DonutToolBar_Create(HWND hWndParent);
	static CString	GetToolBarFilePath();

private:
	LRESULT 		OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL & bHandled);
	LRESULT 		OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT 		OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	// Implemantation
	HMENU			_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu);

	int 			GetToolBarStatus(STD_TBBUTTON * &pBtns);
	int 			GetToolBarStatusStd(STD_TBBUTTON * &pBtns);

private:
	// Data members;
	CMenuHandle 	m_menuFavorites;
	CMenuHandle 	m_menuFavoritesUser;
	CMenuHandle 	m_menuCSS;
	CString 		m_strToolbarIniPath;
};
