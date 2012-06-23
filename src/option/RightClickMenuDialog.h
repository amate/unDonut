/**
 *	@file	RightClickMenuDialog.h
 *	@brief	donutのオプション : 右クリックメニュー
 */

#pragma once

#include "../resource.h"

#define CONTEXT_MENU_HOLDLEFTBUTTON		(123)
#define CONTEXT_MENU_TABITEM			(130)

///////////////////////////////////////////////////////////
// CCustomContextMenuOption

class CCustomContextMenuOption
{
public:
	static CMenu	s_menuDefault;
	static CMenu	s_menuImage;
	static CMenu	s_menuTextSelect;
	static CMenu	s_menuAnchor;
	static CMenu	s_menuHoldLeftButton;

	static CMenu	s_menuTabItem;

public:
	static void		GetProfile();
	static void		WriteProfile();

	static void		GetDefaultContextMenu(CMenu& rMenu, DWORD dwID);
	static HMENU	GetContextMenuFromID(DWORD dwID);
	static void		SetContextMenuFromID(HMENU hMenu, DWORD dwID);

	static void		AddSubMenu(CMenuHandle menu, HWND hWndTopLevel, CSimpleArray<HMENU>& arrDestroyMenu, int& nExtIndex);
	static void		RemoveSubMenu(CMenuHandle menu, CSimpleArray<HMENU>& arrDestroyMenu, int nExtIndex);
	static void		ResetMenu();	// for CRightClickPropertyPage
};



/////////////////////////////////////////////
// CAccessKeyWindow : アクセスキー設定ウィンドウ

class CAccessKeyWindow : public CDialogImpl<CAccessKeyWindow>
{
public:
	// Constants
	enum { IDD = IDD_ACCESSKEYEDIT };

public:
	CString	m_strKey;

private:
	CEdit	m_edit;

public:
	BEGIN_MSG_MAP(CAccessKeyWindow)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		m_edit = GetDlgItem(IDC_EDIT_ACCESSKEY);
		m_edit.SetLimitText(1);
		m_edit.SetWindowText(m_strKey);
		m_edit.SetSelAll(TRUE);
		m_edit.SetFocus();

        return 0;
    }

    void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		TCHAR	str[2] = _T("");
		m_edit.GetWindowText(str, 2);
		m_strKey = str;

        EndDialog(nID);
    }

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(nID);
	}

};



///////////////////////////////////////////////////////////
// CRightClickPropertyPage : [Donutのオプション] - [右クリックメニュー]

class CRightClickPropertyPage
	: public CPropertyPageImpl<CRightClickPropertyPage>
	, public CDragListNotifyImpl<CRightClickPropertyPage>
	, protected CCustomContextMenuOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MENU_RIGHTCLICK };

private:
	CMenuHandle		m_menu;
	bool			m_bInit;

	CComboBox		m_cmbTarget;	// 対象
	CComboBox		m_cmbCategory;	// カテゴリ

	CListBox		m_listCommand;	// コマンド
	CDragListBox	m_listMenu;		// メニュー

	CButton			m_btnAdd;
	CButton			m_btnRemove;
	CButton			m_btnAddSeparator;
	CButton			m_btnApplyMenu;

public:
	// Constructor
	CRightClickPropertyPage(HMENU hMenu);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	BOOL	OnBeginDrag(int nCtlID, HWND hWndDragList, POINT ptCursor);
    void	OnCancelDrag(int nCtlID, HWND hWndDragList, POINT ptCursor);
    int		OnDragging(int nCtlID, HWND hWndDragList, POINT ptCursor);
    void	OnDropped(int nCtlID, HWND hWndDragList, POINT ptCursor);


private:
	void	_InitComboBox();

	void	_AddCommandtoListFromSubMenu(CMenuHandle subMenu);
	void	_SetCommandList(int nIndex);

	void	_AddStringToMenuListFromMenu(const CMenuHandle menu);
	void	_SetMenuList(int nIndex);

	void	_CreateMenuFromMenuList(CMenuHandle &rMenu);

public:
	// Message map
	BEGIN_MSG_MAP( CRightClickPropertyPage )
		COMMAND_HANDLER_EX( IDC_CMB_TARGET 	, CBN_SELCHANGE, OnSelChangeTarget	)
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate	)
		COMMAND_HANDLER_EX( IDC_LIST_COMMAND, LBN_SELCHANGE, OnSelChangeCommandList )
		COMMAND_HANDLER_EX( IDC_LIST_COMMAND, LBN_DBLCLK   , OnSelDblclkCommandList )
		COMMAND_HANDLER_EX( IDC_LIST_MENU	, LBN_SELCHANGE, OnSelChangeMenuList )
		COMMAND_HANDLER_EX( IDC_LIST_MENU	, LBN_DBLCLK   , OnSelDblclkMenuList )
		
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD			, OnAdd				)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_REMOVE		, OnRemove			)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD_SEPARATOR	, OnAddSeparator	)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_APPLYMENU		, OnApplyMenu		)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_RESET			, OnMenuReset		)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_EXAMPLE		, OnExample			)
		CHAIN_MSG_MAP( CPropertyPageImpl<CRightClickPropertyPage> )
		CHAIN_MSG_MAP( CDragListNotifyImpl<CRightClickPropertyPage> )
	END_MSG_MAP()


	void	OnSelChangeTarget(UINT code, int id, HWND hWnd);
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);
	void	OnSelChangeCommandList(UINT code, int id, HWND hWnd);
	void	OnSelDblclkCommandList(UINT code, int id, HWND hWnd);
	void	OnSelChangeMenuList(UINT code, int id, HWND hWnd);
	void	OnSelDblclkMenuList(UINT code, int id, HWND hWnd);

	void	OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnRemove(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnAddSeparator(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnApplyMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnMenuReset(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnExample(UINT uNotifyCode, int nID, CWindow wndCtl);

};