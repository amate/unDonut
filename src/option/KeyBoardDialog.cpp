/**
 *	@file KeyBoardDialog.cpp
 *	@brief	donutのオプション : キー
 */

#include "stdafx.h"
#include "KeyBoardDialog.h"
#include "../MtlBase.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"
#include "MainOption.h"

/////////////////////////////////////////////////////////////////
// CAcceleratorOption

#define ACCELERATORSHAREDMEMNAME _T("DonutAcceleratorSharedMemName")

CSharedMemory CAcceleratorOption::s_sharedMem;


/// アクセラレータテーブルを読み込んでChildFrameのために共有メモリに書き込んでおく
void CAcceleratorOption::CreateOriginAccelerator(HWND hWndMainFrame, HACCEL& hAccel)
{
	CAccelerManager accel(hAccel);
	accel.LoadAccelaratorState(hAccel);

	s_sharedMem.CloseHandle();

	if (accel.m_nAccelSize == 0)
		return ;

	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), ACCELERATORSHAREDMEMNAME, hWndMainFrame);
	DWORD size = (sizeof(ACCEL) * accel.m_nAccelSize) + sizeof(int);
	void* pView = s_sharedMem.CreateSharedMemory(sharedMemName, size);
	int* pAccelSize = reinterpret_cast<int*>(pView);
	*pAccelSize = accel.m_nAccelSize;
	pView = static_cast<void*>(reinterpret_cast<int*>(pView) + 1);
	ATLVERIFY(::memcpy_s(pView, size - sizeof(int), accel.m_lpAccel, size - sizeof(int)) == 0);
}

void	CAcceleratorOption::DestroyOriginAccelerator(HWND hWndMainFrame, HACCEL hAccel)
{
	s_sharedMem.CloseHandle();
	::DestroyAcceleratorTable(hAccel);
}

void	CAcceleratorOption::ReloadAccelerator(HWND hWndMainFrame)
{
	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), ACCELERATORSHAREDMEMNAME, hWndMainFrame);
	CSharedMemory sharedMem;
	void* pView = sharedMem.OpenSharedMemory(sharedMemName, true);
	if (pView == nullptr)
		return ;

	int AccelSize = *reinterpret_cast<int*>(pView);
	LPACCEL lpAccel = reinterpret_cast<LPACCEL>(reinterpret_cast<int*>(pView) + 1);
	m_hAccel = ::CreateAcceleratorTable(lpAccel, AccelSize);
	ATLASSERT( m_hAccel );
}


bool	CAcceleratorOption::TranslateAccelerator(HWND hWndChildFrame, LPMSG lpMsg)
{
	if (m_hAccel == NULL)
		return false;

	return ::TranslateAccelerator(hWndChildFrame, m_hAccel, lpMsg) != 0;
}

	
	/////////////////////////////////////////////////////////////////
// CKeyBoardPropertyPage

CKeyBoardPropertyPage::CKeyBoardPropertyPage(HACCEL& hAccel, HMENU hMenu, HWND hWndMainFrame, function<void (function<void (HWND)>) > foreach)
	: m_hAccel(hAccel), m_hMenu(hMenu), m_hWndMainFrame(hWndMainFrame), m_TabBarForEachWindow(foreach)
{
	m_nCmdUpdate 	= 0;
	m_nCmdMove	 	= 0;
	//m_nMinBtn2Tray= 0;		//+++
}



CKeyBoardPropertyPage::~CKeyBoardPropertyPage()
{
	//::DestroyAcceleratorTable (m_hAccel);
}



// Overrides
BOOL CKeyBoardPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_editNow.m_hWnd == NULL)
		m_editNow.Attach( GetDlgItem(IDC_EDIT_NOW_KEY) );

	if (m_editNew.m_hWnd == NULL)
		m_editNew.Attach( GetDlgItem(IDC_EDIT_NEW_KEY) );

	if (m_cmbCategory.m_hWnd == NULL)
		m_cmbCategory.Attach( GetDlgItem(IDC_CMB_CATEGORY) );

	if (m_cmbCommand.m_hWnd == NULL)
		m_cmbCommand.Attach( GetDlgItem(IDC_CMB_COMMAND) );

	if (m_ltAccel.m_hWnd == NULL) {
		m_ltAccel.Attach( GetDlgItem(IDC_LISTBOX1) );	//+++ラベル変更
	}

	if (m_editNow.m_hWnd && m_editNew.m_hWnd && m_cmbCategory.m_hWnd && m_cmbCommand.m_hWnd)
		_SetData();

	return DoDataExchange(FALSE);
}



BOOL CKeyBoardPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CKeyBoardPropertyPage::OnApply()
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
void CKeyBoardPropertyPage::_SetData()
{
	// コンボボックの初期化
	InitialCombbox();
	// リストボックスの初期化
	InitialListbox();

  #if 0 //+++ 別のオプションが管理している変数を間借りして追加.
	m_nMinBtn2Tray	  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY) != 0;		//+++ 追加.
  #endif
}



// データを保存
void CKeyBoardPropertyPage::_GetData()
{
	CAccelerManager accelManager(m_hAccel);
	accelManager.SaveAccelaratorState();

	/* ChildFrameにアクセラレーターキーの更新を伝える */
	CAcceleratorOption::CreateOriginAccelerator(m_hWndMainFrame, m_hAccel);
	m_TabBarForEachWindow([](HWND hWnd) {
		::SendMessage(hWnd, WM_ACCELTABLECHANGE, 0, 0);
	});
  #if 0 //+++ 別のオプションが管理している変数を間借りして追加.
	if (m_nMinBtn2Tray)	CMainOption::s_dwMainExtendedStyle2 	 |=  MAIN_EX2_MINBTN2TRAY;		//+++ 追加.
	else				CMainOption::s_dwMainExtendedStyle2 	 &= ~MAIN_EX2_MINBTN2TRAY; 		//+++ 追加.
  #endif
}



LRESULT CKeyBoardPropertyPage::OnChkBtn(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);

	// 設定ボタンのEnable調節
	EnableSetBtn();
	return 0;
}



void CKeyBoardPropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	CString 		strAccel;
	TCHAR			szAccel[MAX_PATH];
	szAccel[0]	= 0;	//+++

	if (m_editNow.GetWindowText(szAccel, MAX_PATH) == 0)
		return;

	int 			nIndexCmd = m_cmbCommand.GetCurSel();
	UINT			nCmdID	  = (UINT) m_cmbCommand.GetItemData(nIndexCmd);

	CAccelerManager accelManager(m_hAccel);
	m_hAccel = accelManager.DeleteAccelerator(nCmdID);

	// 強制的に、変更された事にする(;^_^A ｱｾｱｾ･･･
	OnSelChangeCmd(0, 0, NULL);

	SetAccelList();
}



void CKeyBoardPropertyPage::OnBtnSel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	// CommandID
	int 			nIndexCmd = m_cmbCommand.GetCurSel();
	UINT			nCmdID	  = (UINT) m_cmbCommand.GetItemData(nIndexCmd);

	// キーを得る
	ACCEL			accel;

	memcpy( &accel, m_editNew.GetAccel(), sizeof (ACCEL) );
	accel.fVirt |= FNOINVERT;
	accel.fVirt |= FVIRTKEY;
	accel.cmd	 = nCmdID;

	//
	CAccelerManager accelManager(m_hAccel);

	TCHAR			cBuff[MAX_PATH];
	cBuff[0]		= 0;	//+++
	CString 		strAccelNow;
	CString 		strCmd;

	m_editNow.GetWindowText(cBuff, MAX_PATH);
	strAccelNow  = CString(cBuff);

	::GetWindowText(GetDlgItem(IDC_STC01), cBuff, MAX_PATH);
	strCmd		 = CString(cBuff);

	if ( strAccelNow.IsEmpty() ) {
		// 既存コマンドがあった時
		if ( !strCmd.IsEmpty() ) {
			UINT uOldCmd = accelManager.FindCommandID(&accel);
			m_hAccel = accelManager.DeleteAccelerator(uOldCmd);
		}

		m_hAccel = accelManager.AddAccelerator(&accel);
	} else {
		if ( !strAccelNow.IsEmpty() && !strCmd.IsEmpty() ) {
			UINT uOldCmd = accelManager.FindCommandID(&accel);

			m_hAccel = accelManager.DeleteAccelerator(uOldCmd);
			m_hAccel = accelManager.DeleteAccelerator(nCmdID);
			m_hAccel = accelManager.AddAccelerator(&accel);
		} else {
			m_hAccel = accelManager.DeleteAccelerator(nCmdID);
			m_hAccel = accelManager.AddAccelerator(&accel);
		}
	}

	// 強制的に、変更された事にする(;^_^A ｱｾｱｾ･･･
	OnSelChangeCmd(0, 0, NULL);

	SetAccelList();
}



// コンボボックの初期化
void CKeyBoardPropertyPage::InitialCombbox()
{
	if (_SetCombboxCategory(m_cmbCategory, m_hMenu) == FALSE)
		return;

	// する事
	m_cmbCommand.SetDroppedWidth(250);
	OnSelChangeCate(0, 0, 0);
	::SetWindowText( GetDlgItem(IDC_STC01), _T("") );
}



void CKeyBoardPropertyPage::InitialListbox()
{
	static const TCHAR *titles[] = { _T("コマンド"), _T("ショートカット") };
	static const int	widths[] = { 150, 350 };

	LVCOLUMN			col;

	col.mask = LVCF_TEXT | LVCF_WIDTH;

	for (int i = 0; i < _countof(titles); ++i) {
		col.pszText = (LPTSTR) titles[i];
		col.cx		= widths[i];
		m_ltAccel.InsertColumn(i, &col);
	}

	m_ltAccel.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	SetAccelList();
}



void CKeyBoardPropertyPage::SetAccelList()
{
	m_ltAccel.DeleteAllItems();

	CAccelerManager accelManager(m_hAccel);

	for (int ii = 0; ii < accelManager.GetCount(); ii++) {
		LPACCEL    lpAccel = accelManager.GetAt(ii);

		CString    strShortCut;
		CKeyHelper helper(lpAccel);
		helper.Format(strShortCut);

		CString    strCmdName;
		CToolTipManager::LoadToolTipText(lpAccel->cmd, strCmdName);

		m_ltAccel.InsertItem(ii, strCmdName);
		m_ltAccel.SetItemText(ii, 1, strShortCut);
	}
}



// カテゴリ変更時
void CKeyBoardPropertyPage::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	m_editNew.EnableWindow(FALSE);
	int nIndex = m_cmbCategory.GetCurSel();

	// コマンド選択
	_PickUpCommand(m_hMenu, nIndex, m_cmbCommand);
}



// コマンド変更時
void CKeyBoardPropertyPage::OnSelChangeCmd(UINT code, int id, HWND hWnd)
{
	int 			nIndex = m_cmbCommand.GetCurSel();

	if (nIndex == -1)
		return;

	// コマンドID
	UINT			nCmdID = (UINT) m_cmbCommand.GetItemData(nIndex);

	// まず、有効化
	if (nCmdID == 0)
		m_editNew.EnableWindow(FALSE);
	else
		m_editNew.EnableWindow(TRUE);

	// ショートカットキーを得る
	CString 		strAccel;
	CAccelerManager accelManager(m_hAccel);

	if ( accelManager.FindAccelerator(nCmdID, strAccel) ) {
		m_editNow.SetWindowText(strAccel);
		::EnableWindow(GetDlgItem(IDC_BTN_DEL), TRUE);
	} else {
		m_editNow.SetWindowText(_T(""));
		::EnableWindow(GetDlgItem(IDC_BTN_DEL), FALSE);
	}

	m_editNew.SetWindowText(_T(""));
	::EnableWindow(GetDlgItem(IDC_BTN_SET), FALSE);
	::SetWindowText( GetDlgItem(IDC_STC01), _T("") );
}



BOOL CKeyBoardPropertyPage::OnTranslateAccelerator(LPMSG lpMsg)
{
	BOOL bSts = m_editNew.OnTranslateAccelerator(lpMsg);

	if (bSts) {
		// キーを得る
		ACCEL			accel;
		memcpy( &accel, m_editNew.GetAccel(), sizeof (ACCEL) );
		CAccelerManager accelManager(m_hAccel);
		UINT			nCmdID = accelManager.FindCommandID(&accel);

		// New アクセスキーテキスト
		TCHAR			cBuff[MAX_PATH + 1];
		cBuff[0]	= 0;	//+++
		::GetWindowText(m_editNew.m_hWnd, cBuff, MAX_PATH);
		CString 		strAccelNew(cBuff);

		// Now アクセスキーテキスト
		::GetWindowText(m_editNow.m_hWnd, cBuff, MAX_PATH);
		CString 		strAccelNow(cBuff);

		// 既存コマンドテキスト
		CString 		strCmd;

		if (nCmdID != 0)
			CToolTipManager::LoadToolTipText(nCmdID, strCmd);

		::SetWindowText(GetDlgItem(IDC_STC01), strCmd);

		// 設定ボタンのEnable調節
		EnableSetBtn();
	}

	return bSts;
}



// 設定ボタンのEnable調節
void CKeyBoardPropertyPage::EnableSetBtn()
{
	// New アクセスキーテキスト
	TCHAR		cBuff[MAX_PATH];
	cBuff[0]	= 0;	//+++

	::GetWindowText(m_editNew.m_hWnd, cBuff, MAX_PATH);
	CString strAccelNew(cBuff);

	// Now アクセスキーテキスト
	::GetWindowText(m_editNow.m_hWnd, cBuff, MAX_PATH);
	CString strAccelNow(cBuff);

	// 既存コマンドテキスト
	::GetWindowText(GetDlgItem(IDC_STC01), cBuff, MAX_PATH);
	CString strCmd(cBuff);

	BOOL	bEnableSetBtn = TRUE;

	if (strAccelNew.IsEmpty() == FALSE) {
		// Now アクセスキーがない時
		if ( strAccelNow.IsEmpty() ) {
			if ( strCmd.IsEmpty() )
				bEnableSetBtn = TRUE;
			else if (m_nCmdMove == 1)
				bEnableSetBtn = TRUE;
			else
				bEnableSetBtn = FALSE;
		} else {
			if ( strCmd.IsEmpty() ) {
				if (m_nCmdUpdate == 1)
					bEnableSetBtn = TRUE;
				else
					bEnableSetBtn = FALSE;
			} else {
				if (m_nCmdMove == 1 && m_nCmdUpdate == 1)
					bEnableSetBtn = TRUE;
				else
					bEnableSetBtn = FALSE;
			}
		}
	} else {
		bEnableSetBtn = FALSE;
	}
	::EnableWindow(GetDlgItem(IDC_BTN_SET), bEnableSetBtn);
}
