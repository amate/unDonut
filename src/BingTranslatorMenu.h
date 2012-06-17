// BingTranslatorMenu.h

#pragma once

#include "resource.h"

// ëOï˚êÈåæ
class CDictionaryWindow;
class CChildFrame;

/////////////////////////////////////////////////////////////////////
// CBingTranslatorMenu

class CBingTranslatorMenu : public CWindowImpl<CBingTranslatorMenu>
{
public:
	CBingTranslatorMenu(CChildFrame* p);
	~CBingTranslatorMenu();

	CMenuHandle GetMenu() { return m_menu.m_hMenu; }

	BEGIN_MSG_MAP( x )
		MSG_WM_INITMENUPOPUP( OnInitMenuPopup )
		MSG_WM_MENUSELECT( OnMenuSelect )
		COMMAND_ID_HANDLER_EX(ID_BINGTRANSLATE		, OnBingTranslate)
		COMMAND_ID_HANDLER_EX(ID_COPYTRANSLATERESULT, OnCopyTranslateResult )
		COMMAND_ID_HANDLER_EX(ID_A_DICTIONARY		, OnShowDicWindow )
	END_MSG_MAP()


	void OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu);
	void OnMenuSelect(UINT nItemID, UINT nFlags, CMenuHandle menu);
	void OnBingTranslate(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCopyTranslateResult(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnShowDicWindow(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	CChildFrame*	m_pChildFrame;
	CMenu	m_menu;
	CString	m_strSelectedText;
	CString m_strTranslated;
	CDictionaryWindow*	m_pDicWindow;
	CMenuHandle m_RootMenu;
};