/**
 *	@file	MDITabDialog.h
 *	@brief	donutのオプション : タブバー
 */

#pragma once

#include "../MtlUser.h"




// Extended styles
enum EMtb_Ex {
	MTB_EX_ADDLEFT				= 0x00000001L,
	MTB_EX_RIGHTCLICKCLOSE		= 0x00000002L,
	MTB_EX_DELAYED				= 0x00000004L,
	MTB_EX_MULTILINE			= 0x00000008L,
	MTB_EX_DOUBLECLICKCLOSE 	= 0x00000010L,
	MTB_EX_XCLICKCLOSE			= 0x00000020L,
	MTB_EX_RIGHTCLICKREFRESH	= 0x00000040L,
	MTB_EX_DOUBLECLICKREFRESH	= 0x00000080L,
	MTB_EX_XCLICKREFRESH		= 0x00000100L,
	MTB_EX_RIGHTACTIVEONCLOSE	= 0x00000200L,
	MTB_EX_LEFTACTIVEONCLOSE	= 0x00000400L,
	MTB_EX_ADDRIGHTACTIVE		= 0x00000800L,
	MTB_EX_ADDLEFTACTIVE		= 0x00001000L,
	MTB_EX_WHEEL				= 0x00002000L,
	MTB_EX_FIXEDSIZE			= 0x00004000L,
	MTB_EX_ANCHORCOLOR			= 0x00008000L,
	// UDT DGSTR
	MTB_EX_DOUBLECLICKNLOCK 	= 0x00010000L,
	MTB_EX_XCLICKNLOCK			= 0x00020000L,
	MTB_EX_ADDLINKACTIVERIGHT	= 0x00040000L,
	// minit
	MTB_EX_DOUBLECLICKCOMMAND	= 0x00080000L,
	MTB_EX_XCLICKCOMMAND		= 0x00100000L,
	MTB_EX_RIGHTCLICKCOMMAND	= 0x00200000L,
	MTB_EX_MOUSEDOWNSELECT		= 0x00400000L,
	// +mod
	MTB_EX_CTRLTAB_MDI			= 0x00800000L,	//+++
	MTB_EX_SHOWFAVICON			= 0x01000000L,
	MTB_EX_CENTERALIGN			= 0x02000000L,

	MTB_EX_DEFAULT_BITS	= MTB_EX_MULTILINE
						| MTB_EX_WHEEL 
						| MTB_EX_ANCHORCOLOR 
						| MTB_EX_XCLICKCLOSE 
						| MTB_EX_DOUBLECLICKNLOCK 
						| MTB_EX_RIGHTACTIVEONCLOSE,	//+++
		
};



/////////////////////////////////////////////
// CTabBarOption

class CTabBarOption
{
public:
	static DWORD	s_dwExStyle;
	static bool		s_bMultiLine;
	static bool		s_bWheel;
	static bool		s_bUseFixedSize;
	static bool		s_bAnchorColor;
	static bool		s_bAddLinkRight;
	static CSize	s_FixedSize;
	static int		s_nMaxTextLength;
	static int		s_nTabSeparatorWidth;
	static bool		s_bFirefoxLike;
	static bool		s_bMouseDownSelect;
	static bool		s_bCtrlTabMDI;
	static bool		s_bShowFavicon;
	static bool		s_bCenterAlign;
	static int		s_MClickCommand;
	static int		s_RClickCommand;
	static int		s_DClickCommand;
	static CPoint	s_ptOffsetActive;
	static CPoint	s_ptOffsetNormal;
	static CPoint	s_ptOffsetMSelect;

	// CMDITabPropertyPage用
	static int		m_nRadioRightClick;
	static int		m_nRadioDoubleClick;
	static int		m_nRadioMClick;
	static int		m_nRadioOnClose;
	static int		m_nAddPos;

	static void	GetProfile();
	static void WriteProfile();
};


class CDonutTabBar;

//////////////////////////////////////////////
// [Donutのオプション] - [タブバー]

class CMDITabPropertyPage
	: public CInitPropertyPageImpl< CMDITabPropertyPage >
	, public CWinDataExchange< CMDITabPropertyPage >
	, protected CTabBarOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MDITAB };


	// Constructor
	CMDITabPropertyPage(CDonutTabBar* pTabBar, HMENU hMenu);

	// Overrides
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnApply();
	void OnDataValidateError(UINT nCtrlID, BOOL bSave, _XData& data);

	// DDX map
	BEGIN_DDX_MAP( CMDITabPropertyPage )
	DDX_CHECK		( IDC_CHECK_WHEEL				, s_bWheel				)
	DDX_CHECK		( IDC_CHECK_MDITAB_MULTILINE	, s_bMultiLine			)
	DDX_CHECK		( IDC_CHECK_MDITAB_FIXEDSIZE	, s_bUseFixedSize		)
	DDX_CHECK		( IDC_CHECK_MDITAB_ANCHORCOLOR	, s_bAnchorColor		)
	DDX_CHECK		( IDC_CHECK_ADDLINKACTIVERIGHT	, s_bAddLinkRight		)
	DDX_CHECK		( IDC_CHECK_CTRLTAB_MDI         , s_bCtrlTabMDI         )		//+++
	DDX_INT_RANGE	( IDC_EDIT_MDITAB_FIXEDSIZEX	, (int&)s_FixedSize.cx, 10, 500 )
	DDX_INT_RANGE	( IDC_EDIT_MDITAB_FIXEDSIZEY	, (int&)s_FixedSize.cy, 10, 500 )

	DDX_CBINDEX 	( IDC_COMBO_MDITAB_ADDPOS		, m_nAddPos 			)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_RIGHTCLICK	, m_nRadioRightClick	)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_DOUBLECLICK	, m_nRadioDoubleClick	)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_MCLICK		, m_nRadioMClick		)
	DDX_CBINDEX 	( IDC_COMBO_ONCLOSE 			, m_nRadioOnClose		)

	DDX_INT_RANGE	( IDC_EDIT_TABTEXTMAX			, s_nMaxTextLength, 10, 255 )
	DDX_CHECK		( IDC_CHECK_MOUSEDOWNSELECT 	, s_bMouseDownSelect	)
	DDX_INT_RANGE	( IDC_EDIT_TABSEPARATORWIDTH	, s_nTabSeparatorWidth, 5, 125 )
	DDX_CHECK		( IDC_CHECK_FIREFOXLIKE			, s_bFirefoxLike )
	DDX_CHECK		( IDC_SHOWFAVICON				, s_bShowFavicon )
	DDX_CHECK		( IDC_CHECK_CENTERALIGN			, s_bCenterAlign )

	DDX_INT_RANGE	( IDC_EDIT_OFFSETACTIVEX		, (int&)s_ptOffsetActive.x  , -5, 5 )
	DDX_INT_RANGE	( IDC_EDIT_OFFSETACTIVEY		, (int&)s_ptOffsetActive.y  , -5, 5 )
	DDX_INT_RANGE	( IDC_EDIT_OFFSETNORMALX		, (int&)s_ptOffsetNormal.x  , -5, 5 )
	DDX_INT_RANGE	( IDC_EDIT_OFFSETNORMALY		, (int&)s_ptOffsetNormal.y  , -5, 5 )
	DDX_INT_RANGE	( IDC_EDIT_OFFSETMSELECTX		, (int&)s_ptOffsetMSelect.x , -5, 5 )
	DDX_INT_RANGE	( IDC_EDIT_OFFSETMSELECTY		, (int&)s_ptOffsetMSelect.y , -5, 5 )
	END_DDX_MAP()


	// Message map and handlers
	BEGIN_MSG_MAP( CMDITabPropertyPage )
		CHAIN_MSG_MAP ( CInitPropertyPageImpl<CMDITabPropertyPage> )
		//COMMAND_ID_HANDLER_EX ( IDC_BUTTON_MDITAB_FONT, OnFont )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_RIGHTCLICK , CBN_SELCHANGE, OnCmbSelChange )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_DOUBLECLICK, CBN_SELCHANGE, OnCmbSelChange )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_MCLICK	 , CBN_SELCHANGE, OnCmbSelChange )
		COMMAND_ID_HANDLER_EX( IDC_CHECK_MDITAB_FIXEDSIZE, OnBtnFixedWidth	)
	END_MSG_MAP()


	void OnCmbSelChange(UINT /*code*/, int id, HWND /*hWnd*/);
	void OnBtnFixedWidth(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	void CmbUpdate();


	// Data members
	bool			m_bInit;
	CDonutTabBar*	m_pTabBar;
	HMENU			m_hMenu;

};
