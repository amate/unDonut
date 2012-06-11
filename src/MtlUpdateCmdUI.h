/**
 *	@file	MtlUpdateCmdUI.h
 *	@brief	CmdUI の更新関係.
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlUpdateCmdUI.h: Last updated: March 17, 2001
/////////////////////////////////////////////////////////////////////////////

#ifndef __MTLUPDATECMDUI_H__
#define __MTLUPDATECMDUI_H__

#pragma once

#include "MtlWin.h"



namespace MTL {


// better not use
#define CN_UPDATE_COMMAND_UI	( (UINT) (-1) )    // void (CCmdUI*)


#define BEGIN_UPDATE_COMMAND_UI_MAP(theClass)				   \
public: 													   \
	BOOL m_bCmdUIHandled;									   \
	/* "handled" management for cracked handlers */ 		   \
	BOOL IsCmdUIHandled() const 							   \
	{														   \
		return m_bCmdUIHandled; 							   \
	}														   \
	void SetCmdUIHandled(BOOL bHandled) 					   \
	{														   \
		m_bCmdUIHandled = bHandled; 						   \
	}														   \
	BOOL ProcessCmdUIMap(UINT nID, CCmdUI * pCmdUI) 		   \
	{														   \
		BOOL bOldCmdUIHandled = m_bCmdUIHandled;			   \
		BOOL bRet			  = _ProcessCmdUIMap(nID, pCmdUI); \
		m_bCmdUIHandled = bOldCmdUIHandled; 				   \
		return bRet;										   \
	}														   \
	BOOL _ProcessCmdUIMap(UINT nID, CCmdUI * pCmdUI)		   \
	{


#if 1 //+++ ヘッダ＆ルーチン分離用
#define BEGIN_UPDATE_COMMAND_UI_MAP_decl(theClass)			   \
public: 													   \
	BOOL m_bCmdUIHandled;									   \
	/* "handled" management for cracked handlers */ 		   \
	BOOL IsCmdUIHandled() const 							   \
	{														   \
		return m_bCmdUIHandled; 							   \
	}														   \
	void SetCmdUIHandled(BOOL bHandled) 					   \
	{														   \
		m_bCmdUIHandled = bHandled; 						   \
	}														   \
	BOOL ProcessCmdUIMap(UINT nID, CCmdUI * pCmdUI) 		   \
	{														   \
		BOOL bOldCmdUIHandled = m_bCmdUIHandled;			   \
		BOOL bRet			  = _ProcessCmdUIMap(nID, pCmdUI); \
		m_bCmdUIHandled = bOldCmdUIHandled; 				   \
		return bRet;										   \
	}														   \
	BOOL _ProcessCmdUIMap(UINT nID, CCmdUI * pCmdUI);		   \

#define BEGIN_UPDATE_COMMAND_UI_MAP_impl(theClass)			   \
	BOOL theClass::_ProcessCmdUIMap(UINT nID, CCmdUI * pCmdUI) \
	{

#endif



#define UPDATE_COMMAND_UI(id, func) \
	if (nID == id) {				\
		SetCmdUIHandled(TRUE);		\
		func(pCmdUI);				\
		if ( IsCmdUIHandled() ) 	\
			return TRUE;			\
	}


// set text
#define UPDATE_COMMAND_UI_SETTEXT(id, text) \
	if (nID == id) {						\
		pCmdUI->SetText(text);				\
		return TRUE;						\
	}


// set check
#define UPDATE_COMMAND_UI_SETCHECK_IF(id, condition)	\
	if (nID == id) {									\
		pCmdUI->SetCheck(condition != 0/*? 1 : 0*/);	\
		return TRUE;									\
	}


#define UPDATE_COMMAND_UI_SETCHECK_IF_PASS(id, condition) \
	if (nID == id) {									  \
		pCmdUI->SetCheck(condition != 0/*? 1 : 0*/);	  \
	}


#define UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF(id, condition) \
	if (nID == id) {										\
		pCmdUI->Enable();									\
		pCmdUI->SetCheck(condition != 0/*? 1 : 0*/);		\
		return TRUE;										\
	}


#define UPDATE_COMMAND_UI_SETCHECK_FLAG(id, flag, flags)	\
	if (nID == id) {										\
		pCmdUI->SetCheck((flags & flag) != 0/*? 1 : 0*/);	\
		return TRUE;										\
	}


#define UPDATE_COMMAND_UI_SETCHECK_FLAG_PASS(id, flag, flags) \
	if (nID == id) {										  \
		pCmdUI->SetCheck((flags & flag) != 0/*? 1 : 0*/);	  \
	}


#define UPDATE_COMMAND_UI_SETCHECK_FLAG_REV(id, flag, flags) \
	if (nID == id) {										 \
		pCmdUI->SetCheck((flags & flag) ? 0 : 1);			 \
		return TRUE;										 \
	}


#define UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(id, flag, flags) \
	if (nID == id) {											\
		pCmdUI->Enable();										\
		pCmdUI->SetCheck((flags & flag) != 0/*? 1 : 0*/);		\
		return TRUE;											\
	}


#define UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV(id, flag, flags) \
	if (nID == id) {												\
		pCmdUI->Enable();											\
		pCmdUI->SetCheck((flags & flag) == 0 /*? 0 : 1*/);			\
		return TRUE;												\
	}


// enable
#define UPDATE_COMMAND_UI_ENABLE_IF(id, condition)				\
	if (nID == id) {											\
		pCmdUI->Enable((condition) != 0 /*? true : false*/);	\
		return TRUE;											\
	}


#define UPDATE_COMMAND_UI_ENABLE_IF_PASS(id, condition) 	\
	if (nID == id) {										\
		pCmdUI->Enable(condition != 0/*? true : false*/);	\
	}


#define UPDATE_COMMAND_UI_ENABLE_FLAG(id, flag, flags)			\
	if (nID == id) {											\
		pCmdUI->Enable((flags & flag) != 0 /*? true : false*/); \
		return TRUE;											\
	}


#define UPDATE_COMMAND_UI_ENABLE_FLAG_REV(id, flag, flags) \
	if (nID == id) {									   \
		pCmdUI->Enable((flags & flag) ? 0 : 1); 		   \
		return TRUE;									   \
	}


#define UPDATE_COMMAND_UI_POPUP_ENABLE_IF(id, condition)											\
	if (nID == id) {																				\
		if (pCmdUI->m_menuSub.m_hMenu) {															\
			pCmdUI->m_menu.EnableMenuItem( pCmdUI->m_nIndex, MF_BYPOSITION |						\
										  ( condition ? MF_ENABLED : (MF_DISABLED | MF_GRAYED) ) ); \
		}																							\
		return TRUE;																				\
	}


#define UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP(id, condition, popup_condition)							  \
	if (nID == id) {																					  \
		if (pCmdUI->m_menuSub.m_hMenu) {																  \
			pCmdUI->m_menu.EnableMenuItem( pCmdUI->m_nIndex, MF_BYPOSITION |							  \
										  ( popup_condition ? MF_ENABLED : (MF_DISABLED | MF_GRAYED) ) ); \
		} else {																						  \
			pCmdUI->Enable(condition != 0 /*? true : false*/);											  \
		}																								  \
		return TRUE;																					  \
	}


// set default
#define UPDATE_COMMAND_UI_SETDEFAULT(id) \
	if (nID == id) {					 \
		pCmdUI->SetDefault();			 \
		return TRUE;					 \
	}


#define UPDATE_COMMAND_UI_SETDEFAULT_PASS(id) \
	if (nID == id) {						  \
		pCmdUI->SetDefault();				  \
	}


#define UPDATE_COMMAND_UI_SETDEFAULT_IF(id, condition) \
	if (nID == id) {								   \
		pCmdUI->SetDefault(condition);				   \
		return TRUE;								   \
	}


#define UPDATE_COMMAND_UI_SETDEFAULT_IF_PASS(id, condition) \
	if (nID == id) {										\
		pCmdUI->SetDefault(condition);						\
	}


#define UPDATE_COMMAND_UI_SETDEFAULT_FLAG(id, flag, flags)			\
	if (nID == id) {												\
		pCmdUI->SetDefault((flags & flag) != 0/*? true : false*/);	\
		return TRUE;												\
	}


#define UPDATE_COMMAND_UI_SETDEFAULT_FLAG_PASS(id, flag, flags) 	\
	if (nID == id) {												\
		pCmdUI->SetDefault((flags & flag) != 0/*? true : false*/);	\
	}


#define END_UPDATE_COMMAND_UI_MAP() \
		return FALSE;				\
	}



/// simple helper class
class CCmdUI {
public:
	// Attributes
	UINT			m_nID;
	UINT			m_nIndex;				// menu item or other index

	// if a menu item
	CMenuHandle 	m_menu; 				// NULL if not a menu
	CMenuHandle 	m_menuSub;				// sub containing menu item
											// if a popup sub menu - ID is for first in popup
	// if from some other window
	CWindow 		m_wndOther; 			// NULL if a menu or not a CWnd

	// bool			m_bEnableChanged;
	// bool			m_bContinueRouting;
	UINT			m_nIndexMax;			// last + 1 for iterating m_nIndex


public:
	// Operations to do in ON_UPDATE_COMMAND_UI
	virtual void Enable(bool bOn = true)
	{
		if (m_menu.m_hMenu != NULL) {
			if (m_menuSub.m_hMenu != NULL)
				return; 					// don't change popup menus indirectly

			ATLASSERT(m_nIndex < m_nIndexMax);
			m_menu.EnableMenuItem( m_nIndex, MF_BYPOSITION | ( bOn ? MF_ENABLED : (MF_DISABLED | MF_GRAYED) ) );

		} else {
			// enable/disable a control (i.e. child window)
			ATLASSERT(m_wndOther.m_hWnd != NULL);
			// if control has the focus, move the focus before disabling
			//	if (!bOn && (::GetFocus() == m_wndOther.m_hWnd)) {
			//		m_pOther->GetParent()->GetNextDlgTabItem(m_pOther)->SetFocus();
			//	}
			m_wndOther.EnableWindow(bOn);
		}

		//		m_bEnableChanged = true;
	}


	virtual void SetCheck(int nCheck = 1)			// 0, 1 or 2 (indeterminate)
	{
		if (m_menu.m_hMenu != NULL) {
			if (m_menuSub.m_hMenu != NULL)
				return; 							// don't change popup menus indirectly
			// place checkmark next to menu item
			ATLASSERT(m_nIndex < m_nIndexMax);
			m_menu.CheckMenuItem( m_nIndex, MF_BYPOSITION | (nCheck ? MF_CHECKED : MF_UNCHECKED) );
		}
	}


	virtual void SetRadio(bool bOn = true)
	{
		// not supported
		SetCheck(bOn /*? 1 : 0*/);
	}


	virtual void SetText(LPCTSTR lpszText)
	{
		if (lpszText == NULL)
			lpszText = _T("");

		//		ATLASSERT(lpszText != NULL);

		if (m_menu.m_hMenu != NULL) {
			if (m_menuSub.m_hMenu != NULL)
				return; 													// don't change popup menus indirectly

			// get current menu state so it doesn't change
			UINT nState = m_menu.GetMenuState(m_nIndex, MF_BYPOSITION);
			nState &= ~(MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR);

			// set menu text
			ATLASSERT(m_nIndex < m_nIndexMax);
			m_menu.ModifyMenu(m_nIndex, MF_BYPOSITION | MF_STRING | nState, m_nID, lpszText);
		} else {
			ATLASSERT(m_wndOther.m_hWnd != NULL);
			MtlSetWindowText(m_wndOther.m_hWnd, lpszText);
		}
	}


	virtual void SetDefault(bool bOn = true)
	{
		if (m_menu.m_hMenu != NULL) {
			if (m_menuSub.m_hMenu != NULL)
				return; 													// don't change popup menus indirectly

			// place checkmark next to menu item
			ATLASSERT(m_nIndex < m_nIndexMax);

			if (bOn)
				m_menu.SetMenuDefaultItem(m_nIndex, TRUE);
		}
	}


private:
	// Advanced operation
	void ContinueRouting() { /*m_bContinueRouting = true;*/ }


public:
	// Implementation
	CCmdUI()
	{
		// zero out everything
		m_nID = m_nIndex = m_nIndexMax = 0;
	}

};



// class private to this file !
class CToolCmdUI : public CCmdUI {
public: 	// re-implementations only

private:
	virtual void Enable(bool bOn = true)
	{
		//		ATLTRACE2(atlTraceGeneral, 4, _T("CToolCmdUI::Enable (%d:%d)\n"), m_wndOther.m_hWnd, m_nIndex);
		//		m_bEnableChanged = true;
		CToolBarCtrl toolbar   = m_wndOther.m_hWnd;

		ATLASSERT(m_nIndex < m_nIndexMax);

		UINT		 nOldState = toolbar.GetState(m_nID);
		UINT		 nNewState = nOldState;
		ATLASSERT(nNewState != -1);

		if (!bOn) {
			nNewState &= ~TBSTATE_ENABLED;
			// WINBUG: If a button is currently pressed and then is disabled
			// COMCTL32.DLL does not unpress the button, even after the mouse
			// button goes up!	We work around this bug by forcing TBBS_PRESSED
			// off when a button is disabled.
			nNewState &= ~TBSTATE_CHECKED;
		} else {
			nNewState |= TBSTATE_ENABLED;
		}

		if (nNewState != nOldState) {
			MTLVERIFY( toolbar.SetState(m_nID, nNewState) );
		}
	}


	virtual void SetCheck(int nCheck = 1)
	{
		ATLASSERT(nCheck >= 0 && nCheck <= 2);		// 0=>off, 1=>on, 2=>indeterminate
		CToolBarCtrl	toolbar = m_wndOther.m_hWnd;
		ATLASSERT(m_nIndex < m_nIndexMax);

		CVersional<TBBUTTONINFO> tbb;
		tbb.dwMask = TBIF_STATE | TBIF_STYLE;
		MTLVERIFY( toolbar.GetButtonInfo(m_nID, &tbb) != -1 );

		BYTE	fsNewState = tbb.fsState & ~(TBSTATE_CHECKED | TBSTATE_INDETERMINATE);
		if (nCheck == 1)
			fsNewState |= TBSTATE_CHECKED;
		else if (nCheck == 2)
			fsNewState |= TBSTATE_INDETERMINATE;

		BYTE	fsNewStyle	= tbb.fsStyle | TBSTYLE_CHECK;	// add a check style
		bool	bUpdate 	= false;
		if (tbb.fsState != fsNewState) {
			bUpdate 	= true;
			tbb.fsState = fsNewState;
		}

		if (tbb.fsStyle != fsNewStyle) {
			bUpdate 	= true;
			tbb.fsStyle = fsNewStyle;
		} else {
			tbb.dwMask = TBIF_STATE;						// update only state
		}

		if (bUpdate) {
			MTLVERIFY( toolbar.SetButtonInfo(m_nID, &tbb /* | TBBS_CHECKBOX*/) );
		}
	}


	virtual void SetText(LPCTSTR lpszText)
	{
		// ignored it.
	}


	virtual void SetDefault(bool bOn = true)
	{
		// ignored it.
	}
};



class CUpdateCmdUIBase {
public:
	static bool s_bAutoMenuEnable;		// not supported yet

	DECLARE_REGISTERED_MESSAGE(Mtl_Update_CmdUI_Message)

	static BOOL ProcessCmdUIToWindow(HWND hWnd, UINT nID, CCmdUI *pCmdUI)
	{
		return ::SendMessage(hWnd, GET_REGISTERED_MESSAGE(Mtl_Update_CmdUI_Message), (WPARAM) nID, (LPARAM) pCmdUI) != 0;
	}
};



__declspec(selectany) bool CUpdateCmdUIBase::s_bAutoMenuEnable = true;



template <class T>
class CUpdateCmdUIHandler : public CUpdateCmdUIBase {
public:
	BEGIN_MSG_MAP(CUpdateCmdUIHandler)
		MESSAGE_HANDLER(GET_REGISTERED_MESSAGE(Mtl_Update_CmdUI_Message), OnUpdateCommandUI)
	END_MSG_MAP()

private:
	LRESULT OnUpdateCommandUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ATLASSERT( uMsg == GET_REGISTERED_MESSAGE(Mtl_Update_CmdUI_Message) );
		UINT	nID    = (UINT) wParam;
		CCmdUI *pCmdUI = (CCmdUI *) lParam;

		T * 	pT	   = static_cast<T *>(this);
		return pT->ProcessCmdUIMap(nID, pCmdUI);
	}
};



template <class T>
class CUpdateCmdUI : public CUpdateCmdUIBase {
public:
	// Data members
	CSimpleArray<HWND> m_arrToolBar;

	// Constructor
	CUpdateCmdUI()
	{
	}


	// Methods
	void CmdUIAddToolBar(HWND hWndToolBar)
	{
		m_arrToolBar.Add(hWndToolBar);
	}


	// Message map and handlers
	BEGIN_MSG_MAP(CUpdateCmdUI)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MESSAGE_HANDLER(GET_REGISTERED_MESSAGE(Mtl_Update_CmdUI_Message), OnUpdateCommandUI)
	END_MSG_MAP()

private:
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &bHandled)
	{	// a service to avoid a tool bar button's flicker
		bHandled = FALSE;

		HWND hWndCtl = (HWND) lParam;

		if (hWndCtl != NULL && m_arrToolBar.Find(hWndCtl) != -1) {
			::UpdateWindow(hWndCtl);
		}

		return 0;
	}


	void OnInitMenuPopup(HMENU hmenuPopup, UINT uPos, BOOL fSystemMenu)
	{
		SetMsgHandled(FALSE);
		CmdUIUpdateMenuBar(hmenuPopup);
	}


	LRESULT OnUpdateCommandUI(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		UINT	nID    = (UINT) wParam;
		CCmdUI* pCmdUI = (CCmdUI *) lParam;
		T * 	pT	   = static_cast<T *>(this);
		return pT->ProcessCmdUIMap(nID, pCmdUI);
	}


	BOOL _DoUpdate(CCmdUI *pCmdUI, bool bDisableIfNoHndler)
	{
		if ( /*pCmdUI->m_nID == 0 || */ LOWORD(pCmdUI->m_nID) == 0xFFFF)
			return TRUE;

		// ignore invalid IDs
		// pCmdUI->m_bEnableChanged = false;
		T *  pT 	 = static_cast<T *>(this);
		BOOL bResult = pT->ProcessCmdUIMap(pCmdUI->m_nID, pCmdUI);

		//if (!bResult)
		//	ATLASSERT(!pCmdUI->m_bEnableChanged); // not routed

		//if (bDisableIfNoHndler && !pCmdUI->m_bEnableChanged && !bResult)
		//{
		//	Enable or Disable based on whether there is a handler there
		//}
		return bResult;
	}


	void CmdUIUpdateMenuBar(CMenuHandle menuPopup)
	{
		CCmdUI	state;
		state.m_menu	  = menuPopup.m_hMenu;
		ATLASSERT(state.m_wndOther.m_hWnd == NULL);

		state.m_nIndexMax = menuPopup.GetMenuItemCount();

		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
			state.m_nID = menuPopup.GetMenuItemID(state.m_nIndex);

			if (state.m_nID == 0)
				continue;
			// menu separator or invalid cmd - ignore it

			ATLASSERT(state.m_wndOther.m_hWnd == NULL);
			ATLASSERT(state.m_menu.m_hMenu != NULL);

			if (state.m_nID == (UINT) -1) {
				// possibly a popup menu, route to first item of that popup
				state.m_menuSub = menuPopup.GetSubMenu(state.m_nIndex);

				if (state.m_menuSub.m_hMenu == NULL
				   || ( state.m_nID = state.m_menuSub.GetMenuItemID(0) ) == 0
				   || state.m_nID == (UINT) -1)
				{
					continue;					// first item of popup can't be routed to
				}

				_DoUpdate(&state, false);		// popups are never auto disabled
			} else {
				// normal menu item
				// Auto enable/disable if frame window has 's_bAutoMenuEnable'
				//	  set and command is _not_ a system command.
				//state.m_menuSub = NULL;
				state.m_menuSub.Detach();
				_DoUpdate(&state, s_bAutoMenuEnable && state.m_nID < 0xF000);
			}

			// adjust for menu deletions and additions
			UINT nCount = menuPopup.GetMenuItemCount();

			if (nCount < state.m_nIndexMax) {
				state.m_nIndex -= (state.m_nIndexMax - nCount);

				while (state.m_nIndex < nCount
					  && menuPopup.GetMenuItemID(state.m_nIndex) == state.m_nID)
				{
					state.m_nIndex++;
				}
			}

			state.m_nIndexMax = nCount;
		}	// for
	}


public:
	void CmdUIUpdateToolBars()
	{
		for (int i = 0; i < m_arrToolBar.GetSize(); ++i) {
			_CmdUIUpdateToolBar(m_arrToolBar[i]);
		}
	}


private:
	void _CmdUIUpdateToolBar(HWND hWndToolBar)
	{
		// ATLTRACE2(atlTraceGeneral, 4, _T("CUpdateCmdUI::_CmdUIUpdateToolBar\n"));
		if ( !::IsWindowVisible(hWndToolBar) )
			return;

		CToolBarCtrl toolbar(hWndToolBar);
		CToolCmdUI	 state;
		state.m_wndOther  = hWndToolBar;

		state.m_nIndexMax = toolbar.GetButtonCount();

		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
			// get buttons state
			TBBUTTON button;
			MTLVERIFY( toolbar.GetButton(state.m_nIndex, &button) );
			state.m_nID = button.idCommand;

			// ignore separators
			if ( !(button.fsStyle & TBSTYLE_SEP) ) {
				// allow reflections
				//	if (CWnd::OnCmdMsg(0,
				//		MAKELONG((int)CN_UPDATE_COMMAND_UI, WM_COMMAND+WM_REFLECT_BASE),
				//		&state, NULL))
				//		continue;

				// allow the toolbar itself to have update handlers
				//	if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				//		continue;

				// allow the owner to process the update
				_DoUpdate(&state, false);
			}
		}
		// update the dialog controls added to the toolbar
		//UpdateDialogControls(pTarget, bDisableIfNoHndler);
	}


public:
	void CmdUIUpdateChildWindow(HWND hWndContainer, int nID)
	{
		HWND   hWndChild = ::GetDlgItem(hWndContainer, nID);

		ATLASSERT( ::IsWindow(hWndChild) );
		CCmdUI state;
		state.m_wndOther  = hWndChild;
		state.m_nID 	  = nID;

		state.m_nIndexMax = 1;
		_DoUpdate(&state, false);
	}


	void CmdUIUpdateStatusBar(HWND hWndStatusBar, int nPaneID)
	{
		CCmdUI state;

		state.m_wndOther  = hWndStatusBar;
		state.m_nIndexMax = 1;
		state.m_nID 	  = nPaneID;
		_DoUpdate(&state, false);
	}
};



// Update Command UI Chaining Macros
#define CHAIN_UPDATE_COMMAND_UI_MEMBER(theChainMember)	\
	if ( theChainMember.ProcessCmdUIMap(nID, pCmdUI) )	\
		return TRUE;



#define CHAIN_CLIENT_UPDATE_COMMAND_UI()					\
	if ( ProcessCmdUIToWindow(m_hWndClient, nID, pCmdUI) )	\
		return TRUE;



#define CHAIN_MDI_CHILD_UPDATE_COMMAND_UI() 								\
	{																		\
		HWND hWndActive = MDIGetActive();									\
		if ( hWndActive && ProcessCmdUIToWindow(hWndActive, nID, pCmdUI) )	\
			return TRUE;													\
	}



////////////////////////////////////////////////////////////////////////////


}		//namespace MTL



#endif	// __MTLUPDATECMDUI_H__
