/**
 *	@file	MenuEncode.h
 *	@brief	エンコード選択メニュー.
 */
#ifndef __MENUENCODE_H_
#define __MENUENCODE_H_

#include <mshtmcid.h>

// 前方宣言
class CMainFrame;

///////////////////////////////////////////////////
// CMenuEncode

class ATL_NO_VTABLE CMenuEncode 
{
public:
	CMenuEncode(CMainFrame* pFrame);
	~CMenuEncode();

	void Init(HMENU hMenu, int insertPoint);

	// Message map and handlers
	BEGIN_MSG_MAP( CMenuEncode )
		MSG_WM_INITMENUPOPUP( OnInitMenuPopup )
		USER_MSG_WM_RELEASE_CONTEXTMENU( OnReleaseContextMenu )
		MESSAGE_HANDLER( WM_EXITMENULOOP, OnExitMenuLoop )
		COMMAND_RANGE_HANDLER_EX( ID_START, ID_END, OnRangeCommand )
	END_MSG_MAP()


	void OnInitMenuPopup(HMENU hmenuPopup, UINT uPos, BOOL fSystemMenu);
	LRESULT OnReleaseContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnExitMenuLoop(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void OnRangeCommand(UINT, int nID, HWND hWndCtrl);



	HMENU _SearchSubMenu(int nID, HMENU hParentMenu);
	BOOL CreateMenu();


	HMENU CopySubMenu(HMENU hCopy , HMENU hOriginal, int &id);

private:
	void _ReleaseMenu();
	void _ClearMenu();


	// Data members
	CMainFrame*		m_pMainFrame;
	CMenuHandle 	m_Menu;
	CMenuHandle 	m_MenuSub;

	CMenuHandle 	m_CtxMenu;
	CMenuHandle 	m_CtxMenuSub;
	CMenuHandle 	m_EncodeMenu;

	enum { ID_START = 0x6000, ID_END   = 0x6d00 };

	int 						m_posStart;
	int 						m_insertPoint;
	HWND						m_hWndTarget;

	HINSTANCE					m_hInstDLL;

	CComPtr<IOleCommandTarget>	spCT;
	CComPtr<IOleWindow> 		spWnd;

	BOOL						m_bFlag;
};


#endif		//__MENUENCODE_H_
