/**
 *	@file	MouseGestureDialog.h
 *	@brief	donutのオプション : マウスジェスチャ
 */

#pragma once

#include "../resource.h"

//////////////////////////////////////////////////////////////////
// CMouseGesturePropertyPage

class CMouseGesturePropertyPage
	: public CPropertyPageImpl<CMouseGesturePropertyPage>
	, public CWinDataExchange<CMouseGesturePropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MOUSEGESTURE };

	// Constructor
	CMouseGesturePropertyPage(HMENU hMenu);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CKeyBoardPropertyPage )
		DDX_CONTROL_HANDLE( IDC_CMB_CATEGORY, m_cmbCategory )
		DDX_CONTROL_HANDLE( IDC_LIST_COMMAND, m_listCommand )
		DDX_CONTROL_HANDLE( IDC_LISTBOX	, m_ltMoveCmd )
		DDX_CONTROL_HANDLE( IDC_STC01	, m_stcMoveCmd )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMouseGesturePropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMouseGesturePropertyPage> )

		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate )
		COMMAND_HANDLER_EX( IDC_LIST_COMMAND, LBN_SELCHANGE, OnSelChangeCommandList )
		NOTIFY_HANDLER_EX( IDC_LISTBOX, NM_DBLCLK, OnGestureListDblClk )
		NOTIFY_HANDLER_EX( IDC_LISTBOX, LVN_KEYDOWN, OnGestureListKeyDown )

		COMMAND_ID_HANDLER_EX( IDC_BTN_UP	, OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_DOWN , OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_LEFT , OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_RIGHT, OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CLEAR, OnBtn )

		COMMAND_ID_HANDLER_EX( IDC_BTN_ADD, OnBtnAdd )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CHANGE, OnBtnChange )
		COMMAND_ID_HANDLER_EX( IDC_BTN_DEL, OnBtnDel )
		NOTIFY_HANDLER_EX( IDC_LISTBOX, LVN_ITEMCHANGED, OnItemChg )
	END_MSG_MAP()


	// カテゴリ変更時
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);
	// コマンド変更時
	void	OnSelChangeCommandList(UINT code, int id, HWND hWnd);
	LRESULT OnGestureListDblClk(LPNMHDR pnmh);
	LRESULT OnGestureListKeyDown(LPNMHDR pnmh);

	void	OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnChange(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtn	(UINT /*wNotifyCode*/, int	 wID  , HWND /*hWndCtl*/);
	LRESULT OnItemChg(LPNMHDR pnmh);

private:
	void	OnInitCmb();
	void	OnInitList();

	// データを保存
	void	_GetData();

	void	EnableAddBtn();


	// Data members
	bool	m_bInit;
	CSimpleMap<CString, DWORD>	m_mapMouseCmd;
	CSimpleArray<CString>		m_aryDelJst;

	CComboBox		m_cmbCategory;
	CListBox		m_listCommand;
	CListViewCtrl	m_ltMoveCmd;

	HMENU			m_hMenu;

	CStatic 		m_stcMoveCmd;
	CString 		m_strPath;

};
