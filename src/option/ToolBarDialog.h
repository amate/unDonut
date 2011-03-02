/**
 *	@file	ToolBarDialog.h
 *	@brief	donutのオプション : ツールバー
 */

#pragma once

#include "../resource.h"
#include "../DonutToolBar.h"



class CToolBarPropertyPage
	: public CPropertyPageImpl<CToolBarPropertyPage>
	, public CWinDataExchange<CToolBarPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_TOOLBAR };

private:
	// Data members
	CComboBox						m_cmbCategory;
	CComboBox						m_cmbCommand;
	CListViewCtrl					m_ltIcon;
	CImageList						m_imgList;
	HMENU							m_hMenu;
	CSimpleArray<STD_TBBUTTON>		m_aryStdBtn;
	CSimpleArray<STD_TBBUTTON>* 	m_pAryStdBtnBase;
	BOOL							m_bExistSkin;
	BOOL *							m_pbSkinChanged;

public:
	// DDX map
	BEGIN_DDX_MAP(CToolBarPropertyPage)
	END_DDX_MAP()


	CToolBarPropertyPage(HMENU hMenu, CSimpleArray<STD_TBBUTTON>*pAryStdBtn, BOOL *pbSkinChange);

	// Overrides

	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Message map and handlers
	BEGIN_MSG_MAP(CToolBarPropertyPage)
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE  , OnSelChangeCate	)
		COMMAND_HANDLER_EX( IDC_CMB_COMMAND , CBN_SELCHANGE  , OnSelChangeCmd	)
		NOTIFY_HANDLER_EX ( IDC_LIST_ICON	, LVN_ITEMCHANGED, OnItemChgIcon	)
		COMMAND_ID_HANDLER_EX( IDC_BTN01, OnBtnMove )
		COMMAND_ID_HANDLER_EX( IDC_BTN02, OnBtnMove )
		COMMAND_ID_HANDLER_EX( IDC_BTN03, OnBtnAdd	)
		COMMAND_ID_HANDLER_EX( IDC_BTN04, OnBtnDel	)
		COMMAND_ID_HANDLER_EX( IDC_BTN05, OnBtnIns	)
		COMMAND_ID_HANDLER( IDC_CHKBTN_TXT, OnChkBtnText )
		CHAIN_MSG_MAP( CPropertyPageImpl<CToolBarPropertyPage> )
	END_MSG_MAP()

private:
	// カテゴリ変更時
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);

	LRESULT OnChkBtnText(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnItemChgIcon(LPNMHDR pnmh);

	// コマンド変更時
	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);

	void	OnBtnIns(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	DWORD	GetToolButtonStyle(UINT nTarID);

	void	OnBtnMove(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);

private:
	// Constructor
	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();

	// 拡張コマンド
	void		PickUpCommandEx();
	CString 	GetBigFilePath();
	CString 	GetSmallFilePath();

	// コンボボックスの初期化
	void		InitialCombbox();

	// リストビューの初期化
	void		InitialListCtrl();
	void		DisableButtons();
	void		DisableControls();

	void		EnableMove(int nIndex);

	// 追加できる？？
	BOOL		CanAddCommand();
};

