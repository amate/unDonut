/**
 *	@file	ExplorerBarDialog.cpp
 *	@brief	donutのオプション : エクスプローラバー
 */

#pragma once

#include "../resource.h"


enum EMain_Explorer {
	MAIN_EXPLORER_HSCROLL		= 0x00000001L,
	MAIN_EXPLORER_NOSPACE		= 0x00000002L,
	MAIN_EXPLORER_FAV_ORGIMG	= 0x00000004L,
	MAIN_EXPLORER_NODRAGDROP	= 0x00000008L,
	MAIN_EXPLORER_AUTOSHOW		= 0x00000010L,
};


class CExplorerPropertyPage
	: public CPropertyPageImpl<CExplorerPropertyPage>
	, public CWinDataExchange<CExplorerPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_EXPLORERBAR };

private:
	// Data members
	int 		m_nExplorerHScroll;
	int 		m_nExplorerNoSpace;
	int 		m_nExplorerOrgImage;
	int 		m_nExplorerDragDrop;
	int 		m_nExplorerAutoShow;
	CString 	m_strUserFolder;

public:
	// DDX map
	BEGIN_DDX_MAP( CExplorerPropertyPage )
		DDX_CHECK( IDC_CHECK_EXP_HSCROLL , m_nExplorerHScroll  )
		DDX_CHECK( IDC_CHECK_EXP_SPACE	 , m_nExplorerNoSpace  )
		DDX_CHECK( IDC_CHECK_EXP_ORGIMAGE, m_nExplorerOrgImage )
		DDX_CHECK( IDC_CHECK_EXP_DRAGDROP, m_nExplorerDragDrop )
		DDX_CHECK( IDC_CHECK_EXP_AUTOSHOW, m_nExplorerAutoShow )

		DDX_TEXT ( IDC_EDIT_USER		 , m_strUserFolder	   )
	END_DDX_MAP()

	// Constructor
	CExplorerPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Message map and handlers
	BEGIN_MSG_MAP( CExplorerPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_FOLDER, OnUserFolder )
		CHAIN_MSG_MAP( CPropertyPageImpl<CExplorerPropertyPage> )
	END_MSG_MAP()

private:
	void	OnUserFolder(UINT wNotifyCode, int wID, HWND hWndCtl);

	CString _BrowseForFolder();

	// Implementation

	void	_GetData();
	void	_SetData();
};
