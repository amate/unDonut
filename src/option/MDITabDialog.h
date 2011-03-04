/**
 *	@file	MDITabDialog.h
 *	@brief	donutのオプション : タブバー
 */

#pragma once

#include "../MDITabCtrl.h"


class CMDITabPropertyPage
	: public CInitPropertyPageImpl< CMDITabPropertyPage >
	, public CWinDataExchange< CMDITabPropertyPage >
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MDITAB };

private:
	// Data members
	int 			m_nAddPos;
	int 			m_nMultiLine;
	int 			m_nWheel;
	int 			m_nFixedSize;
	int 			m_nAnchorColor;
	int 			m_nRadioRightClick;
	int 			m_nRadioDoubleClick;
	int 			m_nRadioXClick;
	int 			m_nRadioOnClose;
	int 			m_nAddLinkRight;
	int 			m_nFixedSizeX;
	int 			m_nFixedSizeY;
	int 			m_nMaxTextLength;
	int				m_nTabSeparatorWidth;
	bool			m_bFirefoxLike;
	CMDITabCtrl*	m_pMDITab;
	BOOL			m_bInit;
	HMENU			m_hMenu;

	int 			m_nLastCurSel;

	int 			m_nXCmd;
	int 			m_nDCmd;
	int 			m_nRCmd;
	int 			m_nMouseDownSelect;
	int				m_nCtrlTabMDI;			//+++ CTRL(+SHIFT)+TABでの移動をMDIの順番にする場合on

public:
	// DDX map
	BEGIN_DDX_MAP( CMDITabPropertyPage )
	DDX_CHECK		( IDC_CHECK_WHEEL				, m_nWheel				)
	DDX_CHECK		( IDC_CHECK_MDITAB_MULTILINE	, m_nMultiLine			)
	DDX_CHECK		( IDC_CHECK_MDITAB_FIXEDSIZE	, m_nFixedSize			)
	DDX_CHECK		( IDC_CHECK_MDITAB_ANCHORCOLOR	, m_nAnchorColor		)
	DDX_CHECK		( IDC_CHECK_ADDLINKACTIVERIGHT	, m_nAddLinkRight		)
	DDX_CHECK		( IDC_CHECK_CTRLTAB_MDI         , m_nCtrlTabMDI         )		//+++
	DDX_INT_RANGE	( IDC_EDIT_MDITAB_FIXEDSIZEX	, m_nFixedSizeX, 1, 500 )
	DDX_INT_RANGE	( IDC_EDIT_MDITAB_FIXEDSIZEY	, m_nFixedSizeY, 1, 500 )
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_ADDPOS		, m_nAddPos 			)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_RIGHTCLICK	, m_nRadioRightClick	)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_DOUBLECLICK	, m_nRadioDoubleClick	)
	DDX_CBINDEX 	( IDC_COMBO_MDITAB_MCLICK		, m_nRadioXClick		)
	DDX_CBINDEX 	( IDC_COMBO_ONCLOSE 			, m_nRadioOnClose		)
	DDX_INT_RANGE	( IDC_EDIT_TABTEXTMAX			, m_nMaxTextLength, 1, 255 )
	DDX_CHECK		( IDC_CHECK_MOUSEDOWNSELECT 	, m_nMouseDownSelect	)
	DDX_INT_RANGE	( IDC_EDIT_TABSEPARATORWIDTH	, m_nTabSeparatorWidth, 1, 125 )
	DDX_CHECK		( IDC_CHECK_FIREFOXLIKE			, m_bFirefoxLike )
	END_DDX_MAP()

public:
	// Constructor
	CMDITabPropertyPage(CMDITabCtrl *pMDITab, HMENU hMenu)
		: m_pMDITab(pMDITab)
		, m_bInit(FALSE)
		, m_hMenu(hMenu)
	{
		DWORD	dwExStyle  = m_pMDITab->GetMDITabExtendedStyle();

		m_nWheel		   = (dwExStyle & MTB_EX_WHEEL				) != 0;
		m_nFixedSize	   = (dwExStyle & MTB_EX_FIXEDSIZE			) != 0;
		m_nMultiLine	   = (dwExStyle & MTB_EX_MULTILINE			) != 0;
		m_nAnchorColor	   = (dwExStyle & MTB_EX_ANCHORCOLOR		) != 0;
		m_nAddLinkRight    = (dwExStyle & MTB_EX_ADDLINKACTIVERIGHT ) != 0;
		m_nMouseDownSelect = (dwExStyle & MTB_EX_MOUSEDOWNSELECT	) != 0;
		m_nCtrlTabMDI	   = (dwExStyle & MTB_EX_CTRLTAB_MDI		) != 0;		//+++

		CSize	sizeItem   = m_pMDITab->GetItemSize();
		m_nFixedSizeX	   = sizeItem.cx;
		m_nFixedSizeY	   = sizeItem.cy;

		m_nMaxTextLength   = m_pMDITab->GetMaxTabItemTextLength();
		m_nLastCurSel	   = 1;

		m_nTabSeparatorWidth = m_pMDITab->GetTabSeparatorWidth();
		m_bFirefoxLike		 = m_pMDITab->GetFirefoxLike();

		if		(dwExStyle & MTB_EX_RIGHTCLICKCLOSE  )	m_nRadioRightClick = 1;
		else if (dwExStyle & MTB_EX_RIGHTCLICKREFRESH)	m_nRadioRightClick = 2;
		else if (dwExStyle & MTB_EX_RIGHTCLICKCOMMAND)	m_nRadioRightClick = 4; 	// minit
		else											m_nRadioRightClick = 0;

		if		(dwExStyle & MTB_EX_DOUBLECLICKCLOSE  ) m_nRadioDoubleClick = 1;
		else if (dwExStyle & MTB_EX_DOUBLECLICKREFRESH) m_nRadioDoubleClick = 2;
		else if (dwExStyle & MTB_EX_DOUBLECLICKNLOCK  ) m_nRadioDoubleClick = 3;	// UDT DGSTR
		else if (dwExStyle & MTB_EX_DOUBLECLICKCOMMAND) m_nRadioDoubleClick = 5;	// minit
		else											m_nRadioDoubleClick = 0;

		if		(dwExStyle & MTB_EX_XCLICKCLOSE 	)	m_nRadioXClick = 1;
		else if (dwExStyle & MTB_EX_XCLICKREFRESH	)	m_nRadioXClick = 2;
		else if (dwExStyle & MTB_EX_XCLICKNLOCK 	)	m_nRadioXClick = 3; 		// UDT DGSTR
		else if (dwExStyle & MTB_EX_XCLICKCOMMAND	)	m_nRadioXClick = 5; 		// minit
		else											m_nRadioXClick = 0;

		if		(dwExStyle & MTB_EX_RIGHTACTIVEONCLOSE) m_nRadioOnClose = 1;
		else if (dwExStyle & MTB_EX_LEFTACTIVEONCLOSE ) m_nRadioOnClose = 2;
		else											m_nRadioOnClose = 0;

		if		(dwExStyle & MTB_EX_ADDLEFT 		)	m_nAddPos = 1;
		else if (dwExStyle & MTB_EX_ADDRIGHTACTIVE	)	m_nAddPos = 2;
		else if (dwExStyle & MTB_EX_ADDLEFTACTIVE	)	m_nAddPos = 3;
		else											m_nAddPos = 0;

		m_nXCmd = m_pMDITab->m_nXClickCommand;
		m_nDCmd = m_pMDITab->m_nDClickCommand;
		m_nRCmd = m_pMDITab->m_nRClickCommand;
	}


	// Overrides
	BOOL OnSetActive()
	{
		if (!m_bInit) {
			CmbUpdate();
			m_bInit = TRUE;
		}
		SetModified(TRUE);
		return DoDataExchange(FALSE);
	}


	BOOL OnKillActive()
	{
		return DoDataExchange(TRUE);
	}


	BOOL OnApply()
	{
		if ( DoDataExchange(TRUE) ) {
			DWORD	dwExtendedStyle = 0;
			if (m_nWheel			/*== 1*/)	dwExtendedStyle |= MTB_EX_WHEEL;
			if (m_nMultiLine		/*== 1*/)	dwExtendedStyle |= MTB_EX_MULTILINE;
			if (m_nFixedSize		/*== 1*/)	dwExtendedStyle |= MTB_EX_FIXEDSIZE;
			if (m_nAnchorColor		/*== 1*/)	dwExtendedStyle |= MTB_EX_ANCHORCOLOR;
			if (m_nAddLinkRight 	/*== 1*/)	dwExtendedStyle |= MTB_EX_ADDLINKACTIVERIGHT;
			if (m_nMouseDownSelect	/*== 1*/)	dwExtendedStyle |= MTB_EX_MOUSEDOWNSELECT;
			if (m_nCtrlTabMDI				)	dwExtendedStyle |= MTB_EX_CTRLTAB_MDI;

			switch (m_nRadioRightClick) {
			case 1: dwExtendedStyle |= MTB_EX_RIGHTCLICKCLOSE;		break;
			case 2: dwExtendedStyle |= MTB_EX_RIGHTCLICKREFRESH;	break;
			case 4: dwExtendedStyle |= MTB_EX_RIGHTCLICKCOMMAND;	break;	// minit
			}

			switch (m_nRadioDoubleClick) {
			case 1: dwExtendedStyle |= MTB_EX_DOUBLECLICKCLOSE; 	break;
			case 2: dwExtendedStyle |= MTB_EX_DOUBLECLICKREFRESH;	break;
			case 3: dwExtendedStyle |= MTB_EX_DOUBLECLICKNLOCK; 	break;	// UDT DGSTR
			case 5: dwExtendedStyle |= MTB_EX_DOUBLECLICKCOMMAND;	break;	// minit
			}

			switch (m_nRadioXClick) {
			case 1: dwExtendedStyle |= MTB_EX_XCLICKCLOSE;			break;
			case 2: dwExtendedStyle |= MTB_EX_XCLICKREFRESH;		break;
			case 3: dwExtendedStyle |= MTB_EX_XCLICKNLOCK;			break;	// UDT DGSTR
			case 5: dwExtendedStyle |= MTB_EX_XCLICKCOMMAND;		break;	// minit
			}

			switch (m_nRadioOnClose) {
			case 1: dwExtendedStyle |= MTB_EX_RIGHTACTIVEONCLOSE;	break;
			case 2: dwExtendedStyle |= MTB_EX_LEFTACTIVEONCLOSE;	break;
			}

			switch (m_nAddPos) {
			case 1: dwExtendedStyle |= MTB_EX_ADDLEFT;				break;
			case 2: dwExtendedStyle |= MTB_EX_ADDRIGHTACTIVE;		break;
			case 3: dwExtendedStyle |= MTB_EX_ADDLEFTACTIVE;		break;
			}

			m_pMDITab->SetItemSize( CSize(m_nFixedSizeX, m_nFixedSizeY) );
			m_pMDITab->SetMaxTabItemTextLength(m_nMaxTextLength);
			m_pMDITab->SetMDITabExtendedStyle(dwExtendedStyle);

			m_pMDITab->SetTabSeparatorWidth(m_nTabSeparatorWidth);
			m_pMDITab->SetFirefoxLike(m_bFirefoxLike);

			m_pMDITab->m_nXClickCommand = m_nXCmd;
			m_pMDITab->m_nDClickCommand = m_nDCmd;
			m_pMDITab->m_nRClickCommand = m_nRCmd;

			CIniFileO	pr( g_szIniFileName, _T("MDITab") );
			MtlWriteProfileMDITab(pr, *m_pMDITab);
			pr.Close();

			return TRUE;
		} else {
			return FALSE;
		}
	}


private:
	void CmbUpdate()
	{
		CComboBox cmbR	 = GetDlgItem(IDC_COMBO_MDITAB_RIGHTCLICK);
		CComboBox cmbD	 = GetDlgItem(IDC_COMBO_MDITAB_DOUBLECLICK);
		CComboBox cmbX	 = GetDlgItem(IDC_COMBO_MDITAB_MCLICK);

		CString   strCmd = _T("コマンド ...");

		cmbR.AddString(strCmd);
		cmbD.AddString(strCmd);
		cmbX.AddString(strCmd);

		CString   strDesc;

		if (m_pMDITab->m_nRClickCommand) {
			CToolTipManager::LoadToolTipText(m_pMDITab->m_nRClickCommand, strDesc);
			cmbR.AddString(strDesc);
		}

		if (m_pMDITab->m_nXClickCommand) {
			CToolTipManager::LoadToolTipText(m_pMDITab->m_nXClickCommand, strDesc);
			cmbX.AddString(strDesc);
		}

		if (m_pMDITab->m_nDClickCommand) {
			CToolTipManager::LoadToolTipText(m_pMDITab->m_nDClickCommand, strDesc);
			cmbD.AddString(strDesc);
		}
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP( CMDITabPropertyPage )
		CHAIN_MSG_MAP ( CInitPropertyPageImpl<CMDITabPropertyPage> )
		//COMMAND_ID_HANDLER_EX ( IDC_BUTTON_MDITAB_FONT, OnFont )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_RIGHTCLICK , CBN_SELCHANGE, OnCmbSelChange )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_DOUBLECLICK, CBN_SELCHANGE, OnCmbSelChange )
		COMMAND_HANDLER_EX ( IDC_COMBO_MDITAB_MCLICK	 , CBN_SELCHANGE, OnCmbSelChange )
	END_MSG_MAP()


	void OnCmbSelChange(UINT /*code*/, int id, HWND /*hWnd*/)
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
					cmb.SetCurSel(m_nRadioXClick);

				return;
			}

			DWORD				 dwCommand = (DWORD) dlg.GetCommandID();

			if (dwCommand) {
				if (index == count - 2)
					cmb.DeleteString(count - 1);

				CString strDesc;
				CToolTipManager::LoadToolTipText(dwCommand, strDesc);
				cmb.SetCurSel( cmb.AddString(strDesc) );

				if (id == IDC_COMBO_MDITAB_RIGHTCLICK)
					m_nRCmd = dwCommand;
				else if (id == IDC_COMBO_MDITAB_DOUBLECLICK)
					m_nDCmd = dwCommand;
				else if (id == IDC_COMBO_MDITAB_MCLICK)
					m_nXCmd = dwCommand;
			}
		}
	}

};
