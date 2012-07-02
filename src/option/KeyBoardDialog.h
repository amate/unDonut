/**
 *	@file	KeyBoardDialog.h
 *	@brief	donutのオプション : キー
 */
#pragma once

#include "../resource.h"
#include "../AccelManager.h"
#include "../SharedMemoryUtil.h"


class CAcceleratorOption
{
public:
	CAcceleratorOption() : m_hAccel(NULL) { }
	~CAcceleratorOption()
	{
		if (m_hAccel)
			::DestroyAcceleratorTable(m_hAccel);
	}

	// for MainFrame
	static HACCEL CreateOriginAccelerator(HWND hWndMainFrame, HACCEL hAccel);
	static void	  DestroyOriginAccelerator(HWND hWndMainFrame, HACCEL hAccel);

	// for ChildFrame
	void	ReloadAccelerator(HWND hWndMainFrame);

	bool	TranslateAccelerator(HWND hWndChildFrame, LPMSG lpMsg);

private:
	// for ChildFrame
	HACCEL	m_hAccel;

	// for MainFrame
	static CSharedMemory s_sharedMem;
};

class CKeyBoardPropertyPage
	: public CPropertyPageImpl<CKeyBoardPropertyPage>
	, public CWinDataExchange<CKeyBoardPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_KEYBOARD };

	//生成・破棄
	CKeyBoardPropertyPage(HACCEL& hAccel, HMENU hMenu, HWND hWndMainFrame, function<void (function<void (HWND)>) > foreach);
	~CKeyBoardPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CKeyBoardPropertyPage )
		DDX_CHECK( IDC_CHK_CMDUPDATE	, m_nCmdUpdate 	)
		DDX_CHECK( IDC_CHK_CMDMOVE		, m_nCmdMove	)
		//DDX_CHECK( IDC_CHK_MINBTN2TRAY, m_nMinBtn2Tray)		//+++
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP(CKeyBoardPropertyPage)
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate )
		COMMAND_HANDLER_EX( IDC_CMB_COMMAND , CBN_SELCHANGE, OnSelChangeCmd  )

		COMMAND_ID_HANDLER_EX( IDC_BTN_DEL, OnBtnDel )
		COMMAND_ID_HANDLER_EX( IDC_BTN_SET, OnBtnSel )
		COMMAND_ID_HANDLER( IDC_CHK_CMDUPDATE, OnChkBtn )
		COMMAND_ID_HANDLER( IDC_CHK_CMDMOVE  , OnChkBtn )

		CHAIN_MSG_MAP(CPropertyPageImpl<CKeyBoardPropertyPage>)
	END_MSG_MAP()


	LRESULT OnChkBtn(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	void	OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);

	void	OnBtnSel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);

	// コンボボックスの初期化
	void	InitialCombbox();
	void	InitialListbox();
	void	SetAccelList();

	// カテゴリ変更時
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);

	// コマンド変更時
	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);


	BOOL	OnTranslateAccelerator(LPMSG lpMsg);

private:

	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();

	// 設定ボタンのEnable調節
	void	EnableSetBtn();


	// Data members
	HWND		  m_hWndMainFrame;
	function<void (function<void (HWND)>) > m_TabBarForEachWindow;
	CEdit		  m_editNow;
	CEditAccel	  m_editNew;
	CComboBox	  m_cmbCategory;
	CComboBox	  m_cmbCommand;
	CListViewCtrl m_ltAccel;

	HACCEL&		  m_hAccel;
	HMENU		  m_hMenu;
	int 		  m_nCmdUpdate;
	int 		  m_nCmdMove;

	//int		  m_nMinBtn2Tray;	//+++ 最小化ボタンでトレイに入れる設定
};
