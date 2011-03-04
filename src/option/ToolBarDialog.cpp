/**
 *	@file	ToolBarDialog.cpp
 *	@brief	donutのオプション : ツールバー
 */

#include "stdafx.h"
#include "ToolBarDialog.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace MTL;



extern const UINT	g_uDropDownCommandID[];
extern const UINT	g_uDropDownWholeCommandID[];
extern const int	g_uDropDownCommandCount;
extern const int	g_uDropDownWholeCommandCount;



CToolBarPropertyPage::CToolBarPropertyPage(HMENU hMenu, CSimpleArray<STD_TBBUTTON>*	pAryStdBtn, BOOL *pbSkinChange)
{
	m_hMenu 		 = hMenu;
	m_pAryStdBtnBase = pAryStdBtn;
	m_bExistSkin	 = FALSE;
	m_pbSkinChanged  = pbSkinChange;

	// コピーが無い(;^_^A ｱｾｱｾ･･･
	//m_arrStdBtn.Copy(*pAryStdBtn);
	for (int ii = 0; ii < pAryStdBtn->GetSize(); ii++)
		m_aryStdBtn.Add( (*pAryStdBtn)[ii] );
}


// Overrides


BOOL CToolBarPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_cmbCategory.m_hWnd == NULL)
		m_cmbCategory.Attach( GetDlgItem(IDC_CMB_CATEGORY) );

	if (m_cmbCommand.m_hWnd == NULL)
		m_cmbCommand.Attach( GetDlgItem(IDC_CMB_COMMAND) );

	if (m_ltIcon.m_hWnd == NULL)
		m_ltIcon.Attach( GetDlgItem(IDC_LIST_ICON) );

	_SetData();
	return DoDataExchange(FALSE);
}



BOOL CToolBarPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CToolBarPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// Constructor
// データを得る
void CToolBarPropertyPage::_SetData()
{
	// コンボボックの初期化
	InitialCombbox();
	// リストビューの初期化
	InitialListCtrl();
}



// データを保存
void CToolBarPropertyPage::_GetData()
{
	if (m_bExistSkin && m_pbSkinChanged) {
		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// パス
		CString 	strFile;
		strFile = CDonutToolBar::GetToolBarFilePath();

		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// ツールバー
		CIniFileO	pr( strFile, _T("TOOLBAR") );

		DWORD		dwToolbarCnt = m_aryStdBtn.GetSize();
		pr.SetValue( dwToolbarCnt, _T("TOOLBAR_CNT") );

		int 		ii;

		for (ii = 0; ii < m_aryStdBtn.GetSize(); ii++) {
			CString strKeyID, strKeyStyle;
			strKeyID.Format(_T("ID_%d"), ii);
			strKeyStyle.Format(_T("STYLE_%d"), ii);

			pr.SetValue( (DWORD) m_aryStdBtn[ii].idCommand, strKeyID );
			pr.SetValue( (DWORD) m_aryStdBtn[ii].fsStyle, strKeyStyle );
		}

		pr.Close();
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		m_pAryStdBtnBase->RemoveAll();

		for (ii = 0; ii < m_aryStdBtn.GetSize(); ii++)
			m_pAryStdBtnBase->Add(m_aryStdBtn[ii]);
	}
}



// コンボボックスの初期化
void CToolBarPropertyPage::InitialCombbox()
{
	if (_SetCombboxCategory(m_cmbCategory, m_hMenu) == FALSE)
		return;

	m_cmbCategory.AddString( _T("ドロップダウン・アイコン") );

	// 仮想的に、選択変更
	OnSelChangeCate(0, 0, 0);
}



// カテゴリ変更時
void CToolBarPropertyPage::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	int nIndex = m_cmbCategory.GetCurSel();

	// コマンド選択
	if ( (nIndex + 1) != m_cmbCategory.GetCount() )
		_PickUpCommand(m_hMenu, nIndex, m_cmbCommand);
	else
		PickUpCommandEx();
}



// 拡張コマンド
void CToolBarPropertyPage::PickUpCommandEx()
{
	m_cmbCommand.ResetContent();

	int ii;

	for (ii = 0; ii < g_uDropDownCommandCount; ii++) {
		CString strMenu;
		strMenu.LoadString(g_uDropDownCommandID[ii]);

		if (strMenu.Find(_T("\n")) != -1)
			strMenu = strMenu.Left( strMenu.Find(_T("\n")) );

		int 	nIndex = m_cmbCommand.AddString(strMenu);
		m_cmbCommand.SetItemData(nIndex, g_uDropDownCommandID[ii]);
	}

	for (ii = 0; ii < g_uDropDownWholeCommandCount; ii++) {
		CString strMenu;
		strMenu.LoadString(g_uDropDownWholeCommandID[ii]);

		if (strMenu.Find(_T("\n")) != -1)
			strMenu = strMenu.Left( strMenu.Find(_T("\n")) );

		int 	nIndex = m_cmbCommand.AddString(strMenu);
		m_cmbCommand.SetItemData(nIndex, g_uDropDownWholeCommandID[ii]);
	}
}



CString CToolBarPropertyPage::GetBigFilePath()
{
	CString strPath = _GetSkinDir();

	strPath += _T("BigHot.bmp");
	return strPath;
}



CString CToolBarPropertyPage::GetSmallFilePath()
{
	CString strPath = _GetSkinDir();

	strPath += _T("SmallHot.bmp");
	return strPath;
}



// リストビューの初期化
void CToolBarPropertyPage::InitialListCtrl()
{
	BOOL	bBig = TRUE;

	if (m_pbSkinChanged && *m_pbSkinChanged || !m_pbSkinChanged) {
		::MessageBox(m_hWnd, _T("スキンが変更されたためツールバーの設定ができない状態です。\n")
							 _T("一旦このウィンドウを閉じることで設定できるようになります。"), _T("information"), MB_OK);
		DisableControls();
		m_pbSkinChanged = NULL;
		return;
	}

	if (m_ltIcon.m_hWnd == NULL)
		return;

	if (m_imgList.m_hImageList != NULL)
		return;

	CBitmap bmp;
	bmp.Attach( AtlLoadBitmapImage(GetBigFilePath().GetBuffer(0), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL) {
		//bBig = FALSE; 								//+++ small用の表示がバグってるぽいので、とりあえずbig扱いで対処.
		bmp.Attach( AtlLoadBitmapImage(GetSmallFilePath().GetBuffer(0), LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL) {					//+++ 内蔵のbmpを使うようにする...
			bmp.LoadBitmap(IDB_MAINFRAME_TOOLBAR_HOT);	//+++ 内蔵のbmpを使うようにする...
			if (bmp.m_hBitmap == NULL) {
				::MessageBox(m_hWnd, _T("ツールバースキンファイルが見つかりませんでした。\n")
									 _T("カスタマイズに支障が出るので操作ができないようになっています。\n")
									 _T("スキンフォルダにBigHot.bmpファイルを準備してください。")			, _T("information"), MB_OK);
				DisableControls();
				m_bExistSkin = FALSE;
				return;
			}
		}
	}

	CSize	szImg;
	bmp.GetSize(szImg);

	int 	nCount	= szImg.cx / szImg.cy;
	szImg.cx		= szImg.cy;

	MTLVERIFY( m_imgList.Create(szImg.cx, szImg.cy, ILC_COLOR24 | ILC_MASK, nCount, 1) );
	MTLVERIFY( m_imgList.Add( bmp, RGB(255, 0, 255) ) != -1 );

	int 	nFlag  = (bBig) ? LVSIL_NORMAL : LVSIL_SMALL;
	m_ltIcon.SetImageList(m_imgList, nFlag);

	for (int ii = 0; ii < m_aryStdBtn.GetSize(); ii++) {
		UINT	nID = m_aryStdBtn[ii].idCommand;

		CString strCmd;
		CToolTipManager::LoadToolTipText(nID, strCmd);

		m_ltIcon.InsertItem(ii, strCmd, ii);
	}

	DisableButtons();
	m_bExistSkin = TRUE;
}



void CToolBarPropertyPage::DisableButtons()
{
	::EnableWindow(GetDlgItem(IDC_BTN01), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN02), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN03), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN04), FALSE);
	::EnableWindow(GetDlgItem(IDC_BTN05), FALSE);
}



void CToolBarPropertyPage::DisableControls()
{
	DisableButtons();
	m_cmbCategory.EnableWindow(FALSE);
	m_cmbCommand.EnableWindow(FALSE);
	m_ltIcon.EnableWindow(FALSE);
	::EnableWindow(GetDlgItem(IDC_CHKBTN_TXT), FALSE);
}



LRESULT CToolBarPropertyPage::OnChkBtnText(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);

	int nIndexLt = m_ltIcon.GetSelectedIndex();

	if (nIndexLt == -1)
		return 0;

	int nChk	 = (int) ::SendMessage(GetDlgItem(IDC_CHKBTN_TXT), BM_GETCHECK, 0L, 0L);

	if (nChk == BST_CHECKED)
		m_aryStdBtn[nIndexLt].fsStyle |= BTNS_STD_LIST;
	else
		m_aryStdBtn[nIndexLt].fsStyle &= ~BTNS_STD_LIST;

	return 0;
}



LRESULT CToolBarPropertyPage::OnItemChgIcon(LPNMHDR pnmh)
{
	LPNMLISTVIEW pnmv		= (LPNMLISTVIEW) pnmh;

	if (pnmv->uNewState != LBN_SELCANCEL)
		return 0;

	BOOL		 bEnableAdd = FALSE, bEnableDel = FALSE, bEnableIns = FALSE;

	int 		 nIndexCmd	= m_cmbCommand.GetCurSel();
	bEnableAdd = bEnableIns = CanAddCommand();

	long		 nIndex 	= pnmv->iItem;
	bEnableDel = TRUE;

	::EnableWindow(GetDlgItem(IDC_BTN03), bEnableAdd);
	::EnableWindow(GetDlgItem(IDC_BTN04), bEnableDel);
	::EnableWindow(GetDlgItem(IDC_BTN05), bEnableIns);

	//x UINT nID = m_aryStdBtn[nIndex].idCommand;
	//x UINT nStyle = m_aryStdBtn[nIndex].fsStyle;

	if (m_aryStdBtn[nIndex].fsStyle & BTNS_STD_LIST)
		::SendMessage(GetDlgItem(IDC_CHKBTN_TXT), BM_SETCHECK, BST_CHECKED, 0L);
	else
		::SendMessage(GetDlgItem(IDC_CHKBTN_TXT), BM_SETCHECK, BST_UNCHECKED, 0L);

	return 0;
}



void CToolBarPropertyPage::EnableMove(int nIndex)
{
	BOOL bEnableL = FALSE, bEnableR = FALSE;

	if (m_aryStdBtn.GetSize() < 2) {
		bEnableL = FALSE;
		bEnableR = FALSE;
	} else if (nIndex == 0) {
		bEnableL = FALSE;
		bEnableR = TRUE;
	} else if ( (nIndex + 1) == m_aryStdBtn.GetSize() ) {
		bEnableL = TRUE;
		bEnableR = FALSE;
	} else {
		bEnableL = TRUE;
		bEnableR = TRUE;
	}

	::EnableWindow(GetDlgItem(IDC_BTN01), bEnableL);
	::EnableWindow(GetDlgItem(IDC_BTN02), bEnableR);
}



// コマンド変更時
void CToolBarPropertyPage::OnSelChangeCmd(UINT code, int id, HWND hWnd)
{
	::EnableWindow( GetDlgItem(IDC_BTN03), CanAddCommand() );

	int nIndexLt = m_ltIcon.GetSelectedIndex();

	if (nIndexLt != -1)
		::EnableWindow( GetDlgItem(IDC_BTN05), CanAddCommand() );
	else
		::EnableWindow(GetDlgItem(IDC_BTN05), FALSE);
}



// 追加できる？？
BOOL CToolBarPropertyPage::CanAddCommand()
{
	int 	  nIndex = m_cmbCommand.GetCurSel();

	if (nIndex == -1)
		return FALSE;

	DWORD_PTR nTarID = m_cmbCommand.GetItemData(nIndex);

	if (nTarID == 0)
		return FALSE;

	for (int ii = 0; ii < m_aryStdBtn.GetSize(); ii++) {
		DWORD_PTR nID = m_aryStdBtn[ii].idCommand;

		if (nID == nTarID)
			return FALSE;
	}

	return TRUE;
}



void CToolBarPropertyPage::OnBtnIns(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int 	nIndexLt  = m_ltIcon.GetSelectedIndex();
	if (nIndexLt == -1)
		return;

	int 	nIndexCmb = m_cmbCommand.GetCurSel();
	if (nIndexCmb == -1)
		return;

	UINT	nTarID	  = (UINT) m_cmbCommand.GetItemData(nIndexCmb);
	if (nTarID == 0)
		return;

	CString 	 strCmd;
	CToolTipManager::LoadToolTipText(nTarID, strCmd);

	int 		 nLtCnt    = m_ltIcon.GetItemCount();
	m_ltIcon.InsertItem(nLtCnt, strCmd, nLtCnt);

	int 		 ii;
	for (ii = nLtCnt; ii > nIndexLt; ii--) {
		TCHAR	cBuff[MAX_PATH];
		memset( cBuff, 0, sizeof (cBuff) );
		m_ltIcon.GetItemText(ii - 1, 0, cBuff, MAX_PATH);
		CString strText(cBuff);

		m_ltIcon.SetItemText(ii, 0, strText);
	}

	m_ltIcon.SetItemText(nIndexLt, 0, strCmd);

	STD_TBBUTTON stdBtn;
	stdBtn.idCommand	  = nTarID;
	stdBtn.fsStyle		  = GetToolButtonStyle(nTarID);

	int 		 nAryCnt   = m_aryStdBtn.GetSize();
	m_aryStdBtn.Add(stdBtn);

	for (ii = nLtCnt; ii > nIndexLt; ii--)
		m_aryStdBtn[ii] = m_aryStdBtn[ii - 1];

	m_aryStdBtn[nIndexLt] = stdBtn;

	m_cmbCommand.SetCurSel(nIndexCmb + 1);
	::EnableWindow( GetDlgItem(IDC_BTN03), CanAddCommand() );
	::EnableWindow( GetDlgItem(IDC_BTN05), CanAddCommand() );
	EnableMove(nIndexLt);
}



void CToolBarPropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int nIndex = m_ltIcon.GetSelectedIndex();

	if (nIndex == -1)
		return;

	m_aryStdBtn.RemoveAt(nIndex);

	int ii;
	for (ii = nIndex; ii < (m_ltIcon.GetItemCount() - 1); ii++) {
		TCHAR	cBuff[MAX_PATH];
		memset( cBuff, 0, sizeof (cBuff) );
		m_ltIcon.GetItemText(ii + 1, 0, cBuff, MAX_PATH);
		CString strText(cBuff);

		m_ltIcon.SetItemText(ii, 0, strText);
	}

	m_ltIcon.DeleteItem(ii);
	EnableMove(nIndex);
}



void CToolBarPropertyPage::OnBtnAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	int 	nIndex	= m_cmbCommand.GetCurSel();
	if (nIndex == -1)
		return;

	UINT	nTarID	= (UINT) m_cmbCommand.GetItemData(nIndex);
	if (nTarID == 0)
		return;

	STD_TBBUTTON	stdBtn;
	stdBtn.idCommand = nTarID;
	stdBtn.fsStyle	 = GetToolButtonStyle(nTarID);

	CString 		strCmd;
	CToolTipManager::LoadToolTipText(nTarID, strCmd);

	int 	nLtCnt	= m_ltIcon.GetItemCount();
	nLtCnt			= m_ltIcon.InsertItem(nLtCnt, strCmd, nLtCnt);

	// Redrawさせないと、スクロールバーの領域が増えない
	m_ltIcon.RedrawWindow();

	int 		nMinPos =  999,
				nMaxPos = -999,
				nCurPos =	 0;
	::GetScrollRange(m_ltIcon.m_hWnd, WS_HSCROLL, &nMinPos, &nMaxPos);

	nCurPos 		 = ::GetScrollPos(m_ltIcon.m_hWnd, WS_HSCROLL);
	m_ltIcon.Scroll( CSize(nMaxPos - nCurPos, 0) );

	m_aryStdBtn.Add(stdBtn);
	m_cmbCommand.SetCurSel(nIndex + 1);

	::EnableWindow( GetDlgItem(IDC_BTN03), CanAddCommand() );
	::EnableWindow( GetDlgItem(IDC_BTN05), CanAddCommand() );
}



DWORD CToolBarPropertyPage::GetToolButtonStyle(UINT nTarID)
{
	int ii;

	for (ii = 0; ii < g_uDropDownCommandCount; ii++) {
		if (nTarID == g_uDropDownCommandID[ii])
			return BTNS_BUTTON | BTNS_DROPDOWN;
	}

	for (ii = 0; ii < g_uDropDownWholeCommandCount; ii++) {
		if (nTarID == g_uDropDownWholeCommandID[ii]) {
			if (nTarID == ID_FAVORITES_DROPDOWN)
				return BTNS_BUTTON | BTNS_STD_LIST | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN;
			else
				return BTNS_BUTTON | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN;
		}
	}

	return BTNS_BUTTON;
}



void CToolBarPropertyPage::OnBtnMove(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 	nIndex	  = m_ltIcon.GetSelectedIndex();
	if (nIndex == -1)
		return;

	int 	nAdd	   = 1;
	if (wID == IDC_BTN01)
		nAdd = -1;

	TCHAR	cBuff[MAX_PATH];
	memset( cBuff, 0, sizeof (cBuff) );

	m_ltIcon.GetItemText(nIndex, 0, cBuff, MAX_PATH);
	CString 	 strTextNow(cBuff);

	memset( cBuff, 0, sizeof (cBuff) );
	m_ltIcon.GetItemText(nIndex + nAdd, 0, cBuff, MAX_PATH);
	CString 	 strTextTar(cBuff);

	m_ltIcon.SetItemText(nIndex, 0, strTextTar);
	m_ltIcon.SetItemText(nIndex + nAdd, 0, strTextNow);

	STD_TBBUTTON stdBtn    = m_aryStdBtn[nIndex];
	STD_TBBUTTON stdBtnTar = m_aryStdBtn[nIndex + nAdd];

	m_aryStdBtn.SetAtIndex(nIndex, stdBtnTar);
	m_aryStdBtn.SetAtIndex(nIndex + nAdd, stdBtn);

	m_ltIcon.SelectItem(nIndex + nAdd);
	EnableMove(nIndex + nAdd);
}
