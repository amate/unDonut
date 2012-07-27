/**
*	@file MDITabDialog.cpp
*/

#include "stdafx.h"
#include "MDITabDialog.h"
#include "../IniFile.h"
#include "../dialog/CommandSelectDialog.h"
#include "../ToolTipManager.h"
#include "../DonutTabBar.h"

/////////////////////////////////////////////////////

// 定義
DWORD	CTabBarOption::s_dwExStyle		= MTB_EX_DEFAULT_BITS;
bool	CTabBarOption::s_bMultiLine		= true;
bool	CTabBarOption::s_bWheel			= false;
bool	CTabBarOption::s_bUseFixedSize	= false;
bool	CTabBarOption::s_bAnchorColor	= true;
bool	CTabBarOption::s_bAddLinkRight	= true;
CSize	CTabBarOption::s_FixedSize( 150, 23 );
int		CTabBarOption::s_nMaxTextLength		= 30;
int		CTabBarOption::s_nTabSeparatorWidth	= 10;
bool	CTabBarOption::s_bFirefoxLike		= false;
bool	CTabBarOption::s_bMouseDownSelect	= false;
bool	CTabBarOption::s_bCtrlTabMDI		= false;
bool	CTabBarOption::s_bShowFavicon		= false;
bool	CTabBarOption::s_bCenterAlign		= false;
int		CTabBarOption::s_MClickCommand = 0;
int		CTabBarOption::s_RClickCommand = 0;
int		CTabBarOption::s_DClickCommand = 0;
CPoint	CTabBarOption::s_ptOffsetActive;
CPoint	CTabBarOption::s_ptOffsetNormal;
CPoint	CTabBarOption::s_ptOffsetMSelect;

int		CTabBarOption::m_nRadioRightClick;
int		CTabBarOption::m_nRadioDoubleClick;
int		CTabBarOption::m_nRadioMClick;
int		CTabBarOption::m_nRadioOnClose;
int		CTabBarOption::m_nAddPos;


//-------------------------
/// 設定を復元する
void CTabBarOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("MDITab") );
	s_dwExStyle  = pr.GetValue( _T("Extended_Style"), MTB_EX_DEFAULT_BITS );

	s_bWheel		   = (s_dwExStyle & MTB_EX_WHEEL				) != 0;
	s_bUseFixedSize	   = (s_dwExStyle & MTB_EX_FIXEDSIZE			) != 0;
	s_bMultiLine	   = (s_dwExStyle & MTB_EX_MULTILINE			) != 0;
	s_bAnchorColor	   = (s_dwExStyle & MTB_EX_ANCHORCOLOR			) != 0;
	s_bAddLinkRight    = (s_dwExStyle & MTB_EX_ADDLINKACTIVERIGHT	) != 0;
	s_bMouseDownSelect = (s_dwExStyle & MTB_EX_MOUSEDOWNSELECT		) != 0;
	s_bCtrlTabMDI	   = (s_dwExStyle & MTB_EX_CTRLTAB_MDI			) != 0;		//+++
	s_bShowFavicon	   = (s_dwExStyle & MTB_EX_SHOWFAVICON			) != 0;
	s_bCenterAlign	   = (s_dwExStyle & MTB_EX_CENTERALIGN			) != 0;

	if		(s_dwExStyle & MTB_EX_RIGHTCLICKCLOSE  )	m_nRadioRightClick = 1;
	else if (s_dwExStyle & MTB_EX_RIGHTCLICKREFRESH)	m_nRadioRightClick = 2;
	else if (s_dwExStyle & MTB_EX_RIGHTCLICKCOMMAND)	m_nRadioRightClick = 4; 	// minit
	else												m_nRadioRightClick = 0;

	if		(s_dwExStyle & MTB_EX_DOUBLECLICKCLOSE  ) m_nRadioDoubleClick = 1;
	else if (s_dwExStyle & MTB_EX_DOUBLECLICKREFRESH) m_nRadioDoubleClick = 2;
	else if (s_dwExStyle & MTB_EX_DOUBLECLICKNLOCK  ) m_nRadioDoubleClick = 3;	// UDT DGSTR
	else if (s_dwExStyle & MTB_EX_DOUBLECLICKCOMMAND) m_nRadioDoubleClick = 5;	// minit
	else											  m_nRadioDoubleClick = 0;

	if		(s_dwExStyle & MTB_EX_XCLICKCLOSE 	)	m_nRadioMClick = 1;
	else if (s_dwExStyle & MTB_EX_XCLICKREFRESH	)	m_nRadioMClick = 2;
	else if (s_dwExStyle & MTB_EX_XCLICKNLOCK 	)	m_nRadioMClick = 3; 		// UDT DGSTR
	else if (s_dwExStyle & MTB_EX_XCLICKCOMMAND	)	m_nRadioMClick = 5; 		// minit
	else											m_nRadioMClick = 0;

	if		(s_dwExStyle & MTB_EX_RIGHTACTIVEONCLOSE) m_nRadioOnClose = 0;
	else if (s_dwExStyle & MTB_EX_LEFTACTIVEONCLOSE ) m_nRadioOnClose = 1;
	else											  m_nRadioOnClose = 1;

	if		(s_dwExStyle & MTB_EX_ADDLEFT 		)	m_nAddPos = 1;
	else if (s_dwExStyle & MTB_EX_ADDRIGHTACTIVE)	m_nAddPos = 2;
	else if (s_dwExStyle & MTB_EX_ADDLEFTACTIVE	)	m_nAddPos = 3;
	else											m_nAddPos = 0;


	pr.QueryValue(s_nMaxTextLength, _T("Max_Text_Length"));

	pr.QueryValue((int&)s_FixedSize.cx, _T("Fixed_Size_X"));
	pr.QueryValue((int&)s_FixedSize.cy, _T("Fixed_Size_Y"));

	s_MClickCommand = pr.GetValuei(_T("XClickCommand"));
	s_RClickCommand = pr.GetValuei(_T("RClickCommand"));
	s_DClickCommand = pr.GetValuei(_T("DClickCommand"));

	pr.QueryValue((int&)s_nTabSeparatorWidth, _T("SeparatorWidth"));
	s_bFirefoxLike = pr.GetValue(_T("FirefoxLike"), s_bFirefoxLike) != 0;

	s_ptOffsetActive.x	= pr.GetValuei(_T("OffsetActiveX"));
	s_ptOffsetActive.y	= pr.GetValuei(_T("OffsetActiveY"));
	s_ptOffsetNormal.x	= pr.GetValuei(_T("OffsetNormalX"));
	s_ptOffsetNormal.y	= pr.GetValuei(_T("OffsetNormalY"));
	s_ptOffsetMSelect.x = pr.GetValuei(_T("OffsetMSelectX"));
	s_ptOffsetMSelect.y = pr.GetValuei(_T("OffsetMSelectY"));
}

//-------------------------
/// 設定を保存する
void CTabBarOption::WriteProfile()
{
	s_dwExStyle = 0;
	if (s_bWheel			)	s_dwExStyle |= MTB_EX_WHEEL;
	if (s_bMultiLine		)	s_dwExStyle |= MTB_EX_MULTILINE;
	if (s_bUseFixedSize		)	s_dwExStyle |= MTB_EX_FIXEDSIZE;
	if (s_bAnchorColor		)	s_dwExStyle |= MTB_EX_ANCHORCOLOR;
	if (s_bAddLinkRight 	)	s_dwExStyle |= MTB_EX_ADDLINKACTIVERIGHT;
	if (s_bMouseDownSelect	)	s_dwExStyle |= MTB_EX_MOUSEDOWNSELECT;
	if (s_bCtrlTabMDI		)	s_dwExStyle |= MTB_EX_CTRLTAB_MDI;
	if (s_bShowFavicon		)	s_dwExStyle |= MTB_EX_SHOWFAVICON;
	if (s_bCenterAlign		)	s_dwExStyle |= MTB_EX_CENTERALIGN;

	bool bCommand = false;
	switch (m_nRadioRightClick) {
	case 1: s_dwExStyle |= MTB_EX_RIGHTCLICKCLOSE;		break;
	case 2: s_dwExStyle |= MTB_EX_RIGHTCLICKREFRESH;	break;
	case 4: s_dwExStyle |= MTB_EX_RIGHTCLICKCOMMAND;	bCommand = true;	break;	// minit
	}
	if (bCommand == false)
		s_RClickCommand = 0;

	bCommand = false;
	switch (m_nRadioDoubleClick) {
	case 1: s_dwExStyle |= MTB_EX_DOUBLECLICKCLOSE; 	break;
	case 2: s_dwExStyle |= MTB_EX_DOUBLECLICKREFRESH;	break;
	case 3: s_dwExStyle |= MTB_EX_DOUBLECLICKNLOCK; 	break;	// UDT DGSTR
	case 5: s_dwExStyle |= MTB_EX_DOUBLECLICKCOMMAND;	bCommand = true;	break;	// minit
	}
	if (bCommand == false)
		s_DClickCommand = 0;

	bCommand = false;
	switch (m_nRadioMClick) {
	case 1: s_dwExStyle |= MTB_EX_XCLICKCLOSE;			break;
	case 2: s_dwExStyle |= MTB_EX_XCLICKREFRESH;		break;
	case 3: s_dwExStyle |= MTB_EX_XCLICKNLOCK;			break;	// UDT DGSTR
	case 5: s_dwExStyle |= MTB_EX_XCLICKCOMMAND;	bCommand = true;	break;	// minit
	}
	if (bCommand == false)
		s_MClickCommand = 0;

	switch (m_nRadioOnClose) {
	case 0: s_dwExStyle |= MTB_EX_RIGHTACTIVEONCLOSE;	break;
	case 1: s_dwExStyle |= MTB_EX_LEFTACTIVEONCLOSE;	break;
	}

	switch (m_nAddPos) {
	case 1: s_dwExStyle |= MTB_EX_ADDLEFT;				break;
	case 2: s_dwExStyle |= MTB_EX_ADDRIGHTACTIVE;		break;
	case 3: s_dwExStyle |= MTB_EX_ADDLEFTACTIVE;		break;
	}


	CIniFileO	pr( g_szIniFileName, _T("MDITab") );
	pr.SetValue( s_dwExStyle	 , _T("Extended_Style") );
	pr.SetValue( s_nMaxTextLength, _T("Max_Text_Length") );

	pr.SetValue( s_MClickCommand, _T("XClickCommand") ); //minit
	pr.SetValue( s_RClickCommand, _T("RClickCommand") );
	pr.SetValue( s_DClickCommand, _T("DClickCommand") );

	pr.SetValue( s_FixedSize.cx, _T("Fixed_Size_X") );
	pr.SetValue( s_FixedSize.cy, _T("Fixed_Size_Y") );

	pr.SetValue( s_nTabSeparatorWidth, _T("SeparatorWidth") );
	pr.SetValue( s_bFirefoxLike		 , _T("FirefoxLike") );

	pr.SetValue( s_ptOffsetActive.x		, _T("OffsetActiveX"));
	pr.SetValue( s_ptOffsetActive.y		, _T("OffsetActiveY"));
	pr.SetValue( s_ptOffsetNormal.x		, _T("OffsetNormalX"));
	pr.SetValue( s_ptOffsetNormal.y		, _T("OffsetNormalY"));
	pr.SetValue( s_ptOffsetMSelect.x	, _T("OffsetMSelectX"));
	pr.SetValue( s_ptOffsetMSelect.y	, _T("OffsetMSelectY"));
}




///////////////////////////////////////////////////////////
// CMDITabPropertyPage

// Constructor
CMDITabPropertyPage::CMDITabPropertyPage(CDonutTabBar* pTabBar, HMENU hMenu)
	: m_bInit(false)
	, m_pTabBar(pTabBar)
	, m_hMenu(hMenu)
{	}


// Overrides

BOOL CMDITabPropertyPage::OnSetActive()
{
	if (m_bInit == false) {
		DoDataExchange(DDX_LOAD);
		CmbUpdate();
		m_bInit = true;
	}
	SetModified(TRUE);
	return TRUE;
}


BOOL CMDITabPropertyPage::OnKillActive()
{
	return TRUE;
}


BOOL CMDITabPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		WriteProfile();
		m_pTabBar->ReloadSkin();
		return TRUE;
	} else {
		return FALSE;
	}
}


void CMDITabPropertyPage::OnDataValidateError(UINT nCtrlID, BOOL bSave, _XData& data)
{
    CString strMsg; 
    strMsg.Format(_T("%d から %d までの値を入力してください。"),
        data.intData.nMin, data.intData.nMax);
    MessageBox(strMsg, _T("エラー"), MB_ICONEXCLAMATION);

}


void CMDITabPropertyPage::OnCmbSelChange(UINT /*code*/, int id, HWND /*hWnd*/)
{
	if (  (id != IDC_COMBO_MDITAB_RIGHTCLICK )
		&& (id != IDC_COMBO_MDITAB_DOUBLECLICK)
		&& (id != IDC_COMBO_MDITAB_MCLICK	 ) )
		return;

	CComboBox cmb	= GetDlgItem(id);

	if ( !::IsWindow(cmb.m_hWnd) )
		return;

	int 	  index = cmb.GetCurSel();
	int 	  count = cmb.GetCount();

	if (index == CB_ERR)
		return;

	CString   strSel;
	cmb.GetLBText(index, strSel);

	if ( strSel == _T("コマンド ...") ) {
		CCommandSelectDialog dlg(m_hMenu);

		if (dlg.DoModal() != IDOK || dlg.GetCommandID() == 0) {
			if (id == IDC_COMBO_MDITAB_RIGHTCLICK)
				cmb.SetCurSel(m_nRadioRightClick);

			if (id == IDC_COMBO_MDITAB_DOUBLECLICK)
				cmb.SetCurSel(m_nRadioDoubleClick);

			if (id == IDC_COMBO_MDITAB_MCLICK)
				cmb.SetCurSel(m_nRadioMClick);

			return;
		}

		DWORD	dwCommand = (DWORD) dlg.GetCommandID();

		if (dwCommand) {
			if (index == count - 2)
				cmb.DeleteString(count - 1);

			CString strDesc;
			CToolTipManager::LoadToolTipText(dwCommand, strDesc);
			cmb.SetCurSel( cmb.AddString(strDesc) );

			if (id == IDC_COMBO_MDITAB_RIGHTCLICK)
				s_RClickCommand = dwCommand;
			else if (id == IDC_COMBO_MDITAB_DOUBLECLICK)
				s_DClickCommand = dwCommand;
			else if (id == IDC_COMBO_MDITAB_MCLICK)
				s_MClickCommand = dwCommand;
		}
	}
}



void CMDITabPropertyPage::CmbUpdate()
{
	CComboBox cmbR	 = GetDlgItem(IDC_COMBO_MDITAB_RIGHTCLICK);
	CComboBox cmbD	 = GetDlgItem(IDC_COMBO_MDITAB_DOUBLECLICK);
	CComboBox cmbM	 = GetDlgItem(IDC_COMBO_MDITAB_MCLICK);

	CString   strCmd = _T("コマンド ...");

	cmbR.AddString(strCmd);
	cmbD.AddString(strCmd);
	cmbM.AddString(strCmd);

	CString   strDesc;

	if (s_RClickCommand) {
		CToolTipManager::LoadToolTipText(s_RClickCommand, strDesc);
		cmbR.SetCurSel(cmbR.AddString(strDesc));
	}

	if (s_DClickCommand) {
		CToolTipManager::LoadToolTipText(s_DClickCommand, strDesc);
		cmbD.SetCurSel(cmbD.AddString(strDesc));
	}

	if (s_MClickCommand) {
		CToolTipManager::LoadToolTipText(s_MClickCommand, strDesc);
		cmbM.SetCurSel(cmbM.AddString(strDesc));
	}
}
















