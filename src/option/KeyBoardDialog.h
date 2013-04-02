/**
 *	@file	KeyBoardDialog.h
 *	@brief	donutのオプション : キー
 */
#pragma once

#include "../resource.h"
#include "../AccelManager.h"
#include "../SharedMemoryUtil.h"

///////////////////////////////////////////////////////////////////////
// CAcceleratorOption

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
	static void	CreateOriginAccelerator(HWND hWndMainFrame, HACCEL& hAccel);
	static void	DestroyOriginAccelerator(HWND hWndMainFrame, HACCEL hAccel);

	// for ChildFrame
	void	ReloadAccelerator(HWND hWndMainFrame);

	bool	TranslateAccelerator(HWND hWndChildFrame, LPMSG lpMsg);

private:
	// for ChildFrame
	HACCEL	m_hAccel;

	// for MainFrame
	static CSharedMemory s_sharedMem;
};


//////////////////////////////////////////////////////////////////////////
// CKeyBoardPropertyPage

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
		DDX_CONTROL_HANDLE( IDC_CMB_CATEGORY, m_cmbCategory )
		DDX_CONTROL_HANDLE( IDC_LIST_COMMAND, m_listCommand )
		DDX_CONTROL_HANDLE( IDC_EDIT_NOW_KEY , m_editNow )
		DDX_CONTROL_HANDLE( IDC_LISTBOX1	, m_ltAccel )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP(CKeyBoardPropertyPage)
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate )
		COMMAND_HANDLER_EX( IDC_LIST_COMMAND, LBN_SELCHANGE, OnSelChangeCommandList )
		NOTIFY_HANDLER_EX( IDC_LISTBOX1, NM_DBLCLK, OnAccelListDblClk )

		COMMAND_ID_HANDLER_EX( IDC_BTN_DEL, OnBtnDel )
		COMMAND_ID_HANDLER_EX( IDC_BTN_SET, OnBtnSet )

		CHAIN_MSG_MAP(CPropertyPageImpl<CKeyBoardPropertyPage>)
	END_MSG_MAP()


	void	OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnBtnSet(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);

	void	SetAccelList();

	// カテゴリ変更時
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);
	// コマンド変更時
	void	OnSelChangeCommandList(UINT code, int id, HWND hWnd);
	LRESULT OnAccelListDblClk(LPNMHDR pnmh);

	BOOL	OnTranslateAccelerator(LPMSG lpMsg);

private:

	// コンボボックスの初期化
	void	InitialCombbox();
	void	InitialListbox();

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
	CListBox	  m_listCommand;
	CListViewCtrl m_ltAccel;

	HACCEL&		  m_hAccel;
	HMENU		  m_hMenu;

	bool	m_bInit;
};
