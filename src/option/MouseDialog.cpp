/**
 *	@file	MouseDialog.cpp
 *	@brief	donutのオプション : マウス
 */

#include "stdafx.h"
#include "MouseDialog.h"
#include <boost/thread.hpp>
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"
#include "../dialog/CommandSelectDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////
// CMenuOption

bool	CMouseOption::s_bUseRightDragSearch = false;
bool	CMouseOption::s_bUseRect = false;
CString CMouseOption::s_strTEngine;
CString CMouseOption::s_strLEngine;
CString CMouseOption::s_strREngine;
CString CMouseOption::s_strBEngine;
CString CMouseOption::s_strCEngine;


void	CMouseOption::GetProfile()
{
	CString strPath = _GetFilePath( _T("MouseEdit.ini") );
	CIniFileI	pr(strPath, _T("RightDrag"));
	s_bUseRightDragSearch	= pr.GetValue(_T("UseRightDragSearch"), s_bUseRightDragSearch) != 0;
	s_bUseRect				= pr.GetValue(_T("UseRect"), s_bUseRect) != 0;

	s_strTEngine = pr.GetString(_T("TEngine"));
	s_strLEngine = pr.GetString(_T("LEngine"));
	s_strREngine = pr.GetString(_T("REngine"));
	s_strBEngine = pr.GetString(_T("BEngine"));
	s_strCEngine = pr.GetString(_T("CEngine"));
	
}

void	CMouseOption::WriteProfile()
{
	CString strPath = _GetFilePath( _T("MouseEdit.ini") );
	CIniFileO	pr(strPath, _T("RightDrag"));

	pr.SetValue(s_bUseRightDragSearch, _T("UseRightDragSearch"));
	pr.SetValue(s_bUseRect			 , _T("UseRect"));

	pr.SetString(s_strTEngine, _T("TEngine"));
	pr.SetString(s_strLEngine, _T("LEngine"));
	pr.SetString(s_strREngine, _T("REngine"));
	pr.SetString(s_strBEngine, _T("BEngine"));
	pr.SetString(s_strCEngine, _T("CEngine"));
}



////////////////////////////////////////////////////////
// CMousePropertyPage

CMousePropertyPage::CMousePropertyPage(HMENU hMenu, HMENU hSearchEngineMenu)
{
	m_hMenu   = hMenu;
	m_strPath = _GetFilePath( _T("MouseEdit.ini") );

	m_menuSearchEngine = hSearchEngineMenu;
}



// Overrides
BOOL CMousePropertyPage::OnSetActive()
{
	SetModified(TRUE);
	BOOL b = DoDataExchange(FALSE);

	if (m_cmbLinkM.m_hWnd == NULL) {
		OnInitSetting();
		OnInitCmb();
		SetCmdString();
		boost::thread thrd(boost::bind(&CMousePropertyPage::_InitRightDragComboBox, this));
		_InitRightDragOption();
	}

	_SetData();
	return b;
}



BOOL CMousePropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CMousePropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else
		return FALSE;
}



// Constructor
// データを得る
void CMousePropertyPage::_SetData()
{
}



// データを保存
void CMousePropertyPage::_GetData()
{
	CIniFileO	pr( m_strPath, _T("MouseCtrl") );

	for (int ii = 0; ii < m_mapMouseCmd.GetSize(); ii++) {
		CString strKey	  = m_mapMouseCmd.GetKeyAt(ii);
		DWORD	dwCommand = m_mapMouseCmd.GetValueAt(ii);

		pr.SetValue(dwCommand, strKey);
	}

	DWORD		dwCommandD = DWORD( m_cmbDragDrop.GetItemData( m_cmbDragDrop.GetCurSel() ) );
	pr.SetValue( dwCommandD, _T("DragDrop") );

	//x pr.Close(); 	//+++
	_SetRightDragProfile();
	WriteProfile();
}



void CMousePropertyPage::OnInitSetting()
{
	static LPCTSTR lpKeyStr[] = {
		_T("WHEEL_UP"),		_T("WHEEL_DOWN"),
		_T("LButtonUp"),	_T("MButtonUp"),
		_T("XButtonUp1"),	_T("XButtonUp2"),
		_T("LinkOpenBtnM"),
		_T("Side1"),		_T("Side2")
	};
	int 		nKeyCnt   = 9;

	CIniFileI	pr( m_strPath, _T("MouseCtrl") );

	for (int ii = 0; ii < nKeyCnt; ii++) {
		CString		strKey;
		strKey.Format(_T("%s"), lpKeyStr[ii]);

		DWORD	dwCommand = 0;
		pr.QueryValue(dwCommand, strKey);

		m_mapMouseCmd.Add(strKey, dwCommand);
	}
}



void CMousePropertyPage::SetCmdString()
{
	static const DWORD dwWndID[] = {
		IDC_SETBTN_RL,	  IDC_SETBTN_RM,
		IDC_SETBTN_RX1,   IDC_SETBTN_RX2,
		IDC_SETBTN_RW_UP, IDC_SETBTN_RW_DOWN,
		IDC_SETBTN_SIDE1, IDC_SETBTN_SIDE2
	};

	int 			   ii;

	for (ii = 0; ii < ( sizeof (dwWndID) / sizeof (DWORD) ); ii++) {
		CString strTar	  = GetTarString(dwWndID[ii]);
		DWORD	dwCommand = m_mapMouseCmd.Lookup(strTar);

		CString strCmdName;

		if (dwCommand != 0)
			CToolTipManager::LoadToolTipText(dwCommand, strCmdName);
		else
			strCmdName = _T("デフォルト");

		DWORD	dwStatic  = GetTarStaticID(dwWndID[ii]);
		::SetWindowText(::GetDlgItem(m_hWnd, dwStatic), strCmdName);
	}

	CString 		   strTar	 = GetTarString(IDC_CMB_LINK_M);
	DWORD			   dwCommand = m_mapMouseCmd.Lookup(strTar);

	for (ii = 0; ii < m_cmbLinkM.GetCount(); ii++) {
		DWORD_PTR dwVal = m_cmbLinkM.GetItemData(ii);

		if (dwVal != dwCommand)
			continue;

		m_cmbLinkM.SetCurSel(ii);
		break;
	}
}



void CMousePropertyPage::OnInitCmb()
{
	CMenu menuSel;

	menuSel.LoadMenu(IDR_MOUSE_EX_SEL);

	m_cmbLinkM.Attach( GetDlgItem(IDC_CMB_LINK_M) );
	AddMenuItem(menuSel, m_cmbLinkM);

	m_cmbDragDrop.Attach( GetDlgItem(IDC_COMBO_DRAGDROP) );
	AddDragDropItem(m_cmbDragDrop);
}



void CMousePropertyPage::AddMenuItem(CMenu &menu, CComboBox &cmb)
{
	cmb.SetDroppedWidth(250);
	int nIndex = cmb.AddString( _T("デフォルト") );
	cmb.SetItemData(nIndex, 0);

	for (int ii = 0; ii < menu.GetMenuItemCount(); ii++) {
		DWORD	dwCommand = menu.GetMenuItemID(ii);

		if (dwCommand == 0)
			continue;

		CString strMenu;
		menu.GetMenuString(ii, strMenu, MF_BYPOSITION);

		nIndex = cmb.AddString(strMenu);
		cmb.SetItemData(nIndex, dwCommand);
	}
}



void CMousePropertyPage::AddDragDropItem(CComboBox &cmb)
{
	struct _CmbItemData {
		int 		nCommand;
		LPCTSTR 	strDesc;
	};
	static const _CmbItemData	ccd[]	= {
		{ 0 				, _T("デフォルト")	  },
		{ ID_SEARCH_DIRECT	, _T("即検索")		  },
		{ ID_OPENLINK_DIRECT, _T("リンクを開く")  },
	};

	CIniFileI	pr( m_strPath, _T("MouseCtrl") );
	DWORD		dwCommand = pr.GetValue( _T("DragDrop"), ID_SEARCH_DIRECT /*0*/ );		//+++ デフォルト変更.
	pr.Close();

	int 		nCommand	= dwCommand;
	int 		nIndex		= 0;
	int 		nCount		= 3;

	for (int i = 0; i < nCount; i++) {
		nIndex = cmb.AddString(ccd[i].strDesc);
		cmb.SetItemData(nIndex, ccd[i].nCommand);

		if (ccd[i].nCommand == nCommand)
			cmb.SetCurSel(nIndex);
	}
}



LRESULT CMousePropertyPage::OnSetStatic(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/) // UDT DGSTR
{
	switch (wID) {
	case IDC_STC_RL:		OnSetBtn(0, IDC_SETBTN_RL		, 0);	break;
	case IDC_STC_RM:		OnSetBtn(0, IDC_SETBTN_RM		, 0);	break;
	case IDC_STC_RX1:		OnSetBtn(0, IDC_SETBTN_RX1		, 0);	break;
	case IDC_STC_RX2:		OnSetBtn(0, IDC_SETBTN_RX2		, 0);	break;
	case IDC_STC_RW_UP: 	OnSetBtn(0, IDC_SETBTN_RW_UP	, 0);	break;
	case IDC_STC_RW_DOWN:	OnSetBtn(0, IDC_SETBTN_RW_DOWN	, 0);	break;
	case IDC_STC_SIDE1: 	OnSetBtn(0, IDC_SETBTN_SIDE1	, 0);	break;
	case IDC_STC_SIDE2: 	OnSetBtn(0, IDC_SETBTN_SIDE2	, 0);	break;
	}
	return 0;
}

// 右ボタンドラッグ関連の有効無効を切り替える
void	CMousePropertyPage::OnCommandSwitch(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	_InitRightDragOption();
}



void CMousePropertyPage::OnSetBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CCommandSelectDialog dlg(m_hMenu);

	if (dlg.DoModal() != IDOK)
		return;

	UINT				 nCommand  = (UINT) dlg.GetCommandID();
	CString 			 strTar    = GetTarString(wID);

	m_mapMouseCmd.SetAt(strTar, nCommand);

	CString 			 strCmdName;

	if (nCommand != 0)
		CToolTipManager::LoadToolTipText(nCommand, strCmdName);
	else
		strCmdName = _T("デフォルト");

	DWORD				 dwStatic  = GetTarStaticID(wID);
	::SetWindowText(::GetDlgItem(m_hWnd, dwStatic), strCmdName);
}



CString CMousePropertyPage::GetTarString(int id)
{
	CString strTar;

	switch (id) {
	case IDC_SETBTN_RL: 		strTar = _T("LButtonUp");		break;
	case IDC_SETBTN_RM: 		strTar = _T("MButtonUp");		break;
	case IDC_SETBTN_RW_UP:		strTar = _T("WHEEL_UP");		break;
	case IDC_SETBTN_RW_DOWN:	strTar = _T("WHEEL_DOWN");		break;
	case IDC_SETBTN_RX1:		strTar = _T("XButtonUp1");		break;
	case IDC_SETBTN_RX2:		strTar = _T("XButtonUp2");		break;
	case IDC_SETBTN_SIDE1:		strTar = _T("Side1");			break;
	case IDC_SETBTN_SIDE2:		strTar = _T("Side2");			break;
	case IDC_CMB_LINK_M:		strTar = _T("LinkOpenBtnM");	break;
	}

	return strTar;
}



DWORD CMousePropertyPage::GetTarStaticID(int id)
{
	DWORD dwWndID = 0; //value should initialized : minit

	switch (id) {
	case IDC_SETBTN_RL: 		dwWndID = IDC_STC_RL;		break;
	case IDC_SETBTN_RM: 		dwWndID = IDC_STC_RM;		break;
	case IDC_SETBTN_RX1:		dwWndID = IDC_STC_RX1;		break;
	case IDC_SETBTN_RX2:		dwWndID = IDC_STC_RX2;		break;
	case IDC_SETBTN_RW_UP:		dwWndID = IDC_STC_RW_UP;	break;
	case IDC_SETBTN_RW_DOWN:	dwWndID = IDC_STC_RW_DOWN;	break;
	case IDC_SETBTN_SIDE1:		dwWndID = IDC_STC_SIDE1;	break;
	case IDC_SETBTN_SIDE2:		dwWndID = IDC_STC_SIDE2;	break;
	}

	return dwWndID;
}



void CMousePropertyPage::OnSelChange(UINT code, int id, HWND hWnd)
{
	CString 	strTar		= GetTarString(id);

	CComboBox	cmb(hWnd);
	int 		nIndex		= cmb.GetCurSel();
	DWORD		dwCommand	= DWORD( cmb.GetItemData(nIndex) );
	m_mapMouseCmd.SetAt(strTar, dwCommand);
}

// SearchEngineMenuからコンボボックスに登録する
void	CMousePropertyPage::_InitRightDragComboBox()
{
	m_cmbT.SetDroppedWidth(200);
	m_cmbL.SetDroppedWidth(200);
	m_cmbR.SetDroppedWidth(200);
	m_cmbB.SetDroppedWidth(200);
	m_cmbC.SetDroppedWidth(200);

	m_cmbT.AddString(_T("- 未登録 -"));
	m_cmbL.AddString(_T("- 未登録 -"));
	m_cmbR.AddString(_T("- 未登録 -"));
	m_cmbB.AddString(_T("- 未登録 -"));
	m_cmbC.AddString(_T("- 未登録 -"));

	int nCount = m_menuSearchEngine.GetMenuItemCount();
	for (int i = 0; i < nCount; ++i) {
		CString strText;
		m_menuSearchEngine.GetMenuString(i, strText, MF_BYPOSITION);
		m_cmbT.AddString(strText);
		m_cmbL.AddString(strText);
		m_cmbR.AddString(strText);
		m_cmbB.AddString(strText);
		m_cmbC.AddString(strText);
	}

	// 文字列を選択する
	int nIndex;
	nIndex = m_cmbT.FindStringExact(-1, s_strTEngine);
	m_cmbT.SetCurSel(nIndex != CB_ERR ? nIndex : 0);
	nIndex = m_cmbL.FindStringExact(-1, s_strLEngine);
	m_cmbL.SetCurSel(nIndex != CB_ERR ? nIndex : 0);
	nIndex = m_cmbR.FindStringExact(-1, s_strREngine);
	m_cmbR.SetCurSel(nIndex != CB_ERR ? nIndex : 0);
	nIndex = m_cmbB.FindStringExact(-1, s_strBEngine);
	m_cmbB.SetCurSel(nIndex != CB_ERR ? nIndex : 0);
	nIndex = m_cmbC.FindStringExact(-1, s_strCEngine);
	m_cmbC.SetCurSel(nIndex != CB_ERR ? nIndex : 0);

}

// コンボボックスからs_str?Engineに代入
void	CMousePropertyPage::_SetRightDragProfile()
{
	CString strText;
	m_cmbT.GetLBText(m_cmbT.GetCurSel(), strText);
	if (strText != _T("- 未登録 -")) {
		s_strTEngine = strText;
	} else {
		s_strTEngine.Empty();
	}
	m_cmbL.GetLBText(m_cmbL.GetCurSel(), strText);
	if (strText != _T("- 未登録 -")) {
		s_strLEngine = strText;
	} else {
		s_strLEngine.Empty();
	}
	m_cmbR.GetLBText(m_cmbR.GetCurSel(), strText);
	if (strText != _T("- 未登録 -")) {
		s_strREngine = strText;
	} else {
		s_strREngine.Empty();
	}
	m_cmbB.GetLBText(m_cmbB.GetCurSel(), strText);
	if (strText != _T("- 未登録 -")) {
		s_strBEngine = strText;
	} else {
		s_strBEngine.Empty();
	}
	m_cmbC.GetLBText(m_cmbC.GetCurSel(), strText);
	if (strText != _T("- 未登録 -")) {
		s_strCEngine = strText;
	} else {
		s_strCEngine.Empty();
	}
}

void	CMousePropertyPage::_InitRightDragOption()
{
	_SwitchCommand(IDC_CHECK_RIGHT_DRAGDROP);
	_SwitchCommand(IDC_CHECK_USERECT);
}


void	CMousePropertyPage::_SwitchCommand(int nID)
{
	BOOL bEnable;

	switch (nID) {
	case IDC_CHECK_RIGHT_DRAGDROP:	
		if (m_btnUseRightDrag.GetCheck() != 0) {
			bEnable = TRUE;
		} else {
			bEnable = FALSE;
		}
		m_btnUseRect.EnableWindow(bEnable);
		m_cmbT.EnableWindow(bEnable);
		m_cmbL.EnableWindow(bEnable);
		m_cmbR.EnableWindow(bEnable);
		m_cmbB.EnableWindow(bEnable);
		m_cmbC.EnableWindow(bEnable);
		break;
	case IDC_CHECK_USERECT:
		if (m_btnUseRightDrag.GetCheck() == 0)
			return;
		if (m_btnUseRect.GetCheck() != 0) {
			bEnable = TRUE;
		} else {
			bEnable = FALSE;
		}
		m_cmbT.EnableWindow(bEnable);
		m_cmbL.EnableWindow(bEnable);
		m_cmbR.EnableWindow(bEnable);
		m_cmbB.EnableWindow(bEnable);
		m_cmbC.EnableWindow(!bEnable);
	}
}

