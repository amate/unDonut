/**
 *	@file	LinkBarPropertyPage.h
 *	@brief	リンクバーのオプション設定.
 *	@note
 *		nogui.txt に書かれている Linkバーに関する設定をguiのdonutオプションで設定できるようにしたもの(+mod)
 */
#pragma once

#include "../resource.h"

class CLinkBarOption
{
public:
	static bool	s_bShowIconOnly;
	static int	s_nMaxTextWidth;
	static bool	s_bNoShowDragImage;

	static void GetProfile();
	static void WriteProfile();
};

// 前方宣言
class CDonutLinkBarCtrl;

/////////////////////////////////////////////////////////
// CLinkBarPropertyPage

class CLinkBarPropertyPage
		: public CPropertyPageImpl< CLinkBarPropertyPage >
		, public CWinDataExchange< CLinkBarPropertyPage >
		, protected CLinkBarOption
{
public:
	enum { IDD = IDD_PROPPAGE_LINKBAR };

	CLinkBarPropertyPage(CDonutLinkBarCtrl& LinkBar);

	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CLinkBarPropertyPage )
		DDX_CHECK( IDC_CHECK_SHOWICONONLY, s_bShowIconOnly	)
		DDX_CHECK( IDC_CHECK_NOSHOWDRAGIMAGE, s_bNoShowDragImage	)
		DDX_INT_RANGE( IDC_EDIT_MAXTEXTWIDTH, s_nMaxTextWidth, 10, 500 )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CExplorerPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BTN_LINKIMPORTFROMFOLDER, OnLinkImportFromFolder )
		COMMAND_ID_HANDLER_EX( IDC_BTN_LINKEXPORTTOFOLDER, OnLinkExportToFolder )
		CHAIN_MSG_MAP( CPropertyPageImpl<CLinkBarPropertyPage> )
	END_MSG_MAP()

	void OnLinkImportFromFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnLinkExportToFolder(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	// Data members
	bool	m_bInit;
	CDonutLinkBarCtrl&	m_rLinkBarCtrl;
};

