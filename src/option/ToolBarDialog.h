/**
 *	@file	ToolBarDialog.h
 *	@brief	donutのオプション : ツールバー
 */

#pragma once

#include "../resource.h"
#include "../DonutToolBar.h"


// Standard ToolBar Button:
typedef struct _STD_TBBUTTON {
	int 	idCommand;
	DWORD	fsStyle;		// BTNS_
} STD_TBBUTTON, *PSTD_TBBUTTON, *LPSTD_TBBUTTON;
typedef const STD_TBBUTTON	 *LPCSTD_TBBUTTON;

// Standard ToolBar Styles:
enum EStd_TBStyle {
	STD_TBSTYLE_SMALLICON		= 0x00000001L,
	STD_TBSTYLE_LARGEICON		= 0x00000002L,
	STD_TBSTYLE_ICONS			= (STD_TBSTYLE_SMALLICON | STD_TBSTYLE_LARGEICON),

	STD_TBSTYLE_HIDELABEL		= 0x00000004L,
	STD_TBSTYLE_SHOWLABEL		= 0x00000008L,
	STD_TBSTYLE_SHOWLABELRIGHT	= 0x00000010L,
	STD_TBSTYLE_LABELS			= (STD_TBSTYLE_HIDELABEL | STD_TBSTYLE_SHOWLABEL | STD_TBSTYLE_SHOWLABELRIGHT),

	STD_TBSTYLE_DEFAULT 		= (STD_TBSTYLE_SMALLICON | STD_TBSTYLE_HIDELABEL),	//+++ 好みで変更. (STD_TBSTYLE_LARGEICON | STD_TBSTYLE_SHOWLABEL),
	STD_TBSTYLE_ALL 			= (STD_TBSTYLE_ICONS | STD_TBSTYLE_LABELS),
};

enum { BTNS_STD_LIST			= 0x00010000L };


/////////////////////////////////////////////////
// CToolBarOption

class CToolBarOption
{
public:
	static void GetProfile();
	static void WriteProfileToolbar();
	static void WriteProfileToolbarShowButton();


	static vector<STD_TBBUTTON>	s_vecTBbtns;	/// アイコンと関連付けられたコマンドIDと状態
	static vector<int>			s_vecShowBtn;	/// 表示するボタンのインデックス(bitmapからの)
	static DWORD				s_dwToolbarStyle;
};

/////////////////////////////////////////////////
// CToolBarPropertyPage

class CToolBarPropertyPage
	: public CPropertyPageImpl<CToolBarPropertyPage>
	, public CWinDataExchange<CToolBarPropertyPage>
	, public COwnerDraw<CToolBarPropertyPage>
	, protected CToolBarOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_TOOLBAR };

	enum { 
		cyMargin = 2,
		cxMargin = 1,
		IconTextMargin = 3,
	};

	struct IconListData {
		CString strText;
		int		nIndex;
	};

	// Constructor
	CToolBarPropertyPage(HMENU hMenu, BOOL *pbSkinChange, function<void ()> funcReloadSkin);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	// DDX map
	BEGIN_DDX_MAP(CToolBarPropertyPage)
		DDX_CONTROL_HANDLE(IDC_CMB_CATEGORY	, m_cmbCategory	)
		DDX_CONTROL_HANDLE(IDC_LIST_COMMAND	, m_listCommand	)
		DDX_CONTROL_HANDLE(IDC_LIST_ICON	, m_ltIcon	)
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP(CToolBarPropertyPage)
		MSG_WM_DESTROY( OnDestroy )
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE , OnSelChangeCate	)
		COMMAND_HANDLER_EX( IDC_LIST_ICON	, LBN_SELCHANGE	, OnSelChangeCommandIcon )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CHANGE_COMMAND, OnChangeCommand )
		COMMAND_ID_HANDLER( IDC_CHKBTN_TXT, OnChkBtnText )
		CHAIN_MSG_MAP( CPropertyPageImpl<CToolBarPropertyPage> )
		CHAIN_MSG_MAP( COwnerDraw<CToolBarPropertyPage> )
	END_MSG_MAP()

	void	OnDestroy();
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);
	void	OnSelChangeCommandIcon(UINT code, int id, HWND hWnd);
	void	OnChangeCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnChkBtnText(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);


	// コマンド変更時
	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);

	DWORD	GetToolButtonStyle(UINT nTarID);


private:
	
	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();
	

private:
	// コンボボックスの初期化
	void		InitialCombbox();

	// リストビューの初期化
	void		InitialListCtrl();

	void	_SetCommandList(int nIndex);
	void	_AddCommandtoListFromSubMenu(CMenuHandle subMenu);
	void	_InitImageList();

	// Data members
	CComboBox		m_cmbCategory;	// カテゴリ
	CListBox		m_listCommand;	// コマンド
	CListBox		m_ltIcon;
	CImageList						m_imgList;
	CSize							m_iconSize;
	HMENU							m_hMenu;
	BOOL							m_bExistSkin;
	BOOL*							m_pbSkinChanged;

	bool	m_bInit;
	bool	m_bChanged;
	function<void ()>	m_funcReloadSkin;
};

