/**
 *	@file	MouseGestureDialog.h
 *	@brief	donutのオプション : マウスジェスチャ
 */

#pragma once

#include "../resource.h"

class CMouseGesturePropertyPage
	: public CPropertyPageImpl<CMouseGesturePropertyPage>
	, public CWinDataExchange<CMouseGesturePropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MOUSEGESTURE };

private:
	// Data members
	CSimpleMap<CString, DWORD>	m_mapMouseCmd;
	CSimpleArray<CString>		m_aryDelJst;

	CComboBox					m_cmbCategory;
	CComboBox					m_cmbCommand;
	CListViewCtrl				m_ltMoveCmd;
	HMENU						m_hMenu;

	CStatic 					m_stcMoveCmd;
	CString 					m_strPath;

public:
	// Constructor
	CMouseGesturePropertyPage(HMENU hMenu);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	void	OnInitCmb();
	void	OnInitList();

public:
	// Message map and handlers
	BEGIN_MSG_MAP( CMouseGesturePropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMouseGesturePropertyPage> )

		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate )
		COMMAND_HANDLER_EX( IDC_CMB_COMMAND , CBN_SELCHANGE, OnSelChangeCmd )

		COMMAND_ID_HANDLER_EX( IDC_BTN_UP	, OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_DOWN , OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_LEFT , OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_RIGHT, OnBtn )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CLEAR, OnBtn )

		COMMAND_ID_HANDLER_EX( IDC_BTN_ADD, OnBtnAdd )
		COMMAND_ID_HANDLER_EX( IDC_BTN_DEL, OnBtnDel )
		NOTIFY_HANDLER_EX( IDC_LISTBOX, LVN_ITEMCHANGED, OnItemChg )
	END_MSG_MAP()

	BEGIN_DDX_MAP( CMouseGesturePropertyPage )
	END_DDX_MAP()


private:
	void	OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtn	(UINT /*wNotifyCode*/, int	 wID  , HWND /*hWndCtl*/);
	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);
	LRESULT OnItemChg(LPNMHDR pnmh);

	// カテゴリ変更時
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);

private:
	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();

	void	EnableAddBtn();
};
