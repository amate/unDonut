/**
 *	@file	FavoriteMenuDialog.cpp
 *	@brief	donutのオプション : "お気に入りメニュー"
 */

#include "stdafx.h"
#include "../DonutFavoritesMenu.h"
#include "FavoriteMenuDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////
// CFavoritesMenuOption

DWORD	CFavoritesMenuOption::s_dwStyle 		= EMS_DEFAULTBITS;

bool 	CFavoritesMenuOption::s_bAddOpenAll		= false;				
bool	CFavoritesMenuOption::s_bAddSaveFav		= false;				
bool 	CFavoritesMenuOption::s_bIEOrder		= false;					
bool 	CFavoritesMenuOption::s_bDrawIcon		= false;				
bool 	CFavoritesMenuOption::s_bSpaceMin		= false;				
bool	CFavoritesMenuOption::s_bDrawFavicon	= false;		

DWORD	CFavoritesMenuOption::s_dwMaxMenuItemTextLength	= 55;	
DWORD	CFavoritesMenuOption::s_dwMaxMenuBreakCount		= 5000;	

bool	CFavoritesMenuOption::s_bUserFolder		= false;				
CString CFavoritesMenuOption::s_strUserFolder;		

bool	CFavoritesMenuOption::s_bCstmOrder		= false;				
bool	CFavoritesMenuOption::s_bAddResetOrder	= false;	

std::function<void ()>	CFavoritesMenuOption::s_funcRefreshFav;


// iniから設定を読み込む
void CFavoritesMenuOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("FavoritesMenu") );

	pr.QueryValue( s_dwStyle, _T("Style") );
	s_bAddOpenAll	= _check_flag( s_dwStyle, EMS_ADDITIONAL_OPENALL	);
	s_bAddSaveFav	= _check_flag( s_dwStyle, EMS_ADDITIONAL_SAVEFAV_TO	);
	s_bIEOrder		= _check_flag( s_dwStyle, EMS_IE_ORDER				);
	s_bDrawIcon		= _check_flag( s_dwStyle, EMS_DRAW_ICON				);
	s_bSpaceMin		= _check_flag( s_dwStyle, EMS_SPACE_MIN				);
	s_bDrawFavicon	= _check_flag( s_dwStyle, EMS_DRAW_FAVICON			);
	s_bCstmOrder	= _check_flag( s_dwStyle, EMS_CUSTOMIZE_ORDER		);
	s_bAddResetOrder= _check_flag( s_dwStyle, EMS_ADDITIONAL_RESETORDER	);
	s_bUserFolder	= _check_flag( s_dwStyle, EMS_USER_DEFINED_FOLDER	);

	pr.QueryValue( s_dwMaxMenuItemTextLength, _T("Max_Text_Length") );
	pr.QueryValue( s_dwMaxMenuBreakCount	, _T("Max_Break_Count") );

	//*s_pUserDirectory	= pr.GetStringUW(_T("UserFolder"));
	s_strUserFolder		= pr.GetStringUW(_T("UserFolder"));


}

// iniへ設定を保存する
void CFavoritesMenuOption::WriteProfile()
{
	CIniFileO	pr( g_szIniFileName, _T("FavoritesMenu") );

	s_dwStyle = 0;
	if (s_bAddOpenAll	)	s_dwStyle |= EMS_ADDITIONAL_OPENALL;
	if (s_bAddSaveFav	)	s_dwStyle |= EMS_ADDITIONAL_SAVEFAV_TO;	
	if (s_bIEOrder		)	s_dwStyle |= EMS_IE_ORDER;
	if (s_bDrawIcon		)	s_dwStyle |= EMS_DRAW_ICON;
	if (s_bSpaceMin		)	s_dwStyle |= EMS_SPACE_MIN;
	if (s_bDrawFavicon	)	s_dwStyle |= EMS_DRAW_FAVICON;
	if (s_bCstmOrder	)	s_dwStyle |= EMS_CUSTOMIZE_ORDER;
	if (s_bAddResetOrder)	s_dwStyle |= EMS_ADDITIONAL_RESETORDER;
	if (s_bUserFolder	)	s_dwStyle |= EMS_USER_DEFINED_FOLDER;
	pr.SetValue( s_dwStyle, _T("Style")	);

	//pr.SetStringUW( *s_pUserDirectory		, _T("UserFolder")		);
	pr.SetStringUW( s_strUserFolder			, _T("UserFolder")		);
	pr.SetValue( s_dwMaxMenuItemTextLength	, _T("Max_Text_Length") );
	pr.SetValue( s_dwMaxMenuBreakCount		, _T("Max_Break_Count") );
}




////////////////////////////////////////////////////////////////////////////////
// CDonutFavoritesMenuPropertyPage

LRESULT CDonutFavoritesMenuPropertyPage::OnBtnFavMenuUser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strEdit = _BrowseForFolder();

	if ( !strEdit.IsEmpty() )
		m_editUserFolder.SetWindowText(strEdit);

	return 0;
}

// チェックボックスの状態を見て他のチェックボックスの有効/無効を切り替える
void	CDonutFavoritesMenuPropertyPage::OnCommandSwitch(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_SwitchEnable(nID);
}



// Constructor
CDonutFavoritesMenuPropertyPage::CDonutFavoritesMenuPropertyPage()
	: m_bInit(false)
{
}



// Overrides
BOOL CDonutFavoritesMenuPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_bInit == false) {
		m_bInit = true;

		m_nMaxMenuItemTextLength = GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = GetMaxMenuBreakCount();
		m_strFolder = CFavoritesMenuOption::GetUserDirectory();

		DoDataExchange(FALSE);

		_SwitchEnable(IDC_CHECK_DRAW_ICON		);
		_SwitchEnable(IDC_CHECK_DRAW_FAVICON	);
		_SwitchEnable(IDC_CHECK_FAVMENU_ORDER	);
		_SwitchEnable(IDC_CHECK_FAVMENU_USER	);
	}

	return TRUE;
}



BOOL CDonutFavoritesMenuPropertyPage::OnKillActive()
{
	return TRUE;
}



BOOL CDonutFavoritesMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		SetMaxMenuItemTextLength(m_nMaxMenuItemTextLength);
		SetMaxMenuBreakCount(m_nMaxMenuBreakCount);
		CFavoritesMenuOption::SetUserDirectory(m_strFolder);

		CFavoritesMenuOption::CallBack();

		CFavoritesMenuOption::WriteProfile();

		return TRUE;
	} else {
		return FALSE;
	}
}



CString CDonutFavoritesMenuPropertyPage::_BrowseForFolder()
{
	TCHAR			szDisplayName[MAX_PATH];
	::ZeroMemory(szDisplayName, sizeof szDisplayName);

	BROWSEINFO		bi = {
		m_hWnd, 			  NULL, szDisplayName, _T("ユーザー定義のお気に入りフォルダ"),
		BIF_RETURNONLYFSDIRS, NULL, 0,				0,
	};
	CItemIDList 	idl;

	idl.Attach( ::SHBrowseForFolder(&bi) );
	return idl.GetPath();
}

////////////////////////////////////////////////////////
/// チェックボックスの有効無効の切り替え
void	CDonutFavoritesMenuPropertyPage::_SwitchEnable(int nID)
{
	BOOL	bEnable = FALSE;
	return;	//\\+

	switch (nID) {
	case IDC_CHECK_DRAW_ICON:
		if (m_btnDrawIcon.GetCheck() & BST_CHECKED) {
			bEnable = FALSE;
		} else {
			bEnable = TRUE;
			m_btnSpaceMin.SetCheck(BST_UNCHECKED);
		}
		m_btnSpaceMin.EnableWindow(!bEnable);
		m_btnDrawFavicon.EnableWindow(bEnable);
		break;

	case IDC_CHECK_DRAW_FAVICON:
		if (m_btnDrawFavicon.GetCheck() & BST_CHECKED) {
			bEnable = FALSE;
			m_btnSpaceMin.SetCheck(BST_UNCHECKED);
		} else {
			bEnable = TRUE;
		}
		m_btnDrawIcon.EnableWindow(bEnable);
		break;

	case IDC_CHECK_FAVMENU_USER:
		if (m_btnUserDefinedFolder.GetCheck() & BST_CHECKED) {
			bEnable = TRUE;
		} else {
			bEnable = FALSE;
		}
		m_editUserFolder.EnableWindow(bEnable);
		// break;がないけどこれは正しい
	case IDC_CHECK_FAVMENU_ORDER:
		if (   m_btnIEOrder.GetCheck() & BST_CHECKED
			&& m_btnUserDefinedFolder.GetCheck() == BST_UNCHECKED) {
			bEnable = TRUE;
		} else {
			bEnable = FALSE;
			m_btnCstmOrder.SetCheck(BST_UNCHECKED);
		}
		m_btnCstmOrder.EnableWindow(bEnable);
		break;
	}
}