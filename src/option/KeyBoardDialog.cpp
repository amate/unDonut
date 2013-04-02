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

CKeyBoardPropertyPage::CKeyBoardPropertyPage(HACCEL& hAccel, HMENU hMenu, HWND hWndMainFrame, function<void (function<void (HWND)>) > foreach) : 
	m_hAccel(hAccel), 
	m_hMenu(hMenu), 
	m_hWndMainFrame(hWndMainFrame), 
	m_TabBarForEachWindow(foreach), 
	m_bInit(false)
{
}



CKeyBoardPropertyPage::~CKeyBoardPropertyPage()
{
	//::DestroyAcceleratorTable (m_hAccel);
}



// Overrides
BOOL CKeyBoardPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_bInit == false) {
		m_bInit = true;

		m_editNew.Attach(GetDlgItem(IDC_EDIT_NEW_KEY));

		DoDataExchange(DDX_LOAD);

		_SetData();
	}
	return TRUE;
}



BOOL CKeyBoardPropertyPage::OnKillActive()
{
	return TRUE;
}



BOOL CKeyBoardPropertyPage::OnApply()
{
	_GetData();
	return TRUE;
}



// Constructor
// データを得る
void CKeyBoardPropertyPage::_SetData()
{
	// コンボボックの初期化
	InitialCombbox();
	// リストボックスの初期化
	InitialListbox();
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

}

/// 削除
void CKeyBoardPropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	CString strAccel = MtlGetWindowText(m_editNow);
	if (strAccel.IsEmpty())
		return ;

	int 	nIndexCmd = m_listCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_listCommand.GetItemData(nIndexCmd);

	CAccelerManager accelManager(m_hAccel);
	m_hAccel = accelManager.DeleteAccelerator(nCmdID);

	OnSelChangeCommandList(0, 0, NULL);

	SetAccelList();
}


/// 割り当て
void CKeyBoardPropertyPage::OnBtnSet(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	// CommandID
	int 	nIndexCmd = m_listCommand.GetCurSel();
	UINT	nCmdID	  = (UINT) m_listCommand.GetItemData(nIndexCmd);

	// キーを得る
	ACCEL			accel;

	memcpy( &accel, m_editNew.GetAccel(), sizeof (ACCEL) );
	accel.fVirt |= FNOINVERT;
	accel.fVirt |= FVIRTKEY;
	accel.cmd	 = nCmdID;

	CAccelerManager accelManager(m_hAccel);

	CString strAccelNow	= MtlGetWindowText(m_editNow);
	CString strCmd		= MtlGetWindowText(GetDlgItem(IDC_STC01));

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

	OnSelChangeCommandList(0, 0, NULL);

	SetAccelList();

	int nCount = m_ltAccel.GetItemCount();
	for (int i = 0; i < nCount; ++i) {
		if (m_ltAccel.GetItemData(i) == nCmdID) {
			m_ltAccel.SelectItem(i);
			break;
		}
	}
}



// コンボボックの初期化
void CKeyBoardPropertyPage::InitialCombbox()
{
	if (_SetCombboxCategory(m_cmbCategory, m_hMenu) == FALSE)
		return;

	// する事
	OnSelChangeCate(0, 0, 0);
	::SetWindowText( GetDlgItem(IDC_STC01), _T("") );
}



void CKeyBoardPropertyPage::InitialListbox()
{
	static const TCHAR *titles[] = { _T("コマンド"), _T("ショートカット") };
	static const int	widths[] = { 140, 90 };

	m_ltAccel.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);

	LVCOLUMN	col = {};
	col.mask = LVCF_TEXT | LVCF_WIDTH;

	for (int i = 0; i < _countof(titles); ++i) {
		col.pszText = (LPTSTR) titles[i];
		col.cx		= widths[i];
		m_ltAccel.InsertColumn(i, &col);
	}
	SetAccelList();
}


/// アクセルリストからリストビューに登録する
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
		m_ltAccel.SetItemData(ii, lpAccel->cmd);
		m_ltAccel.SetItemText(ii, 1, strShortCut);
	}
}



// カテゴリ変更時
void CKeyBoardPropertyPage::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	m_editNew.EnableWindow(FALSE);
	int nIndex = m_cmbCategory.GetCurSel();

	// コマンド選択
	PickUpCommand(m_hMenu, nIndex, m_listCommand);
}



// コマンド変更時
void CKeyBoardPropertyPage::OnSelChangeCommandList(UINT code, int id, HWND hWnd)
{
	int nIndex = m_listCommand.GetCurSel();
	if (nIndex == -1)
		return;

	// コマンドID
	UINT	nCmdID = (UINT)m_listCommand.GetItemData(nIndex);

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

/// アクセルリストのアイテムをコマンドリストで選択状態にする
LRESULT CKeyBoardPropertyPage::OnAccelListDblClk(LPNMHDR pnmh)
{
	CPoint pt;
	::GetCursorPos(&pt);
	m_ltAccel.ScreenToClient(&pt);
	UINT flags = 0;
	int nIndex = m_ltAccel.HitTest(pt, &flags);
	if (nIndex != -1) {
		UINT cmd = m_ltAccel.GetItemData(nIndex);
		if (cmd) {
			int nCategoryCount = m_cmbCategory.GetCount();
			for (int i = 0; i < nCategoryCount; ++i) {
				PickUpCommand(m_hMenu, i, m_listCommand);
				int nListCount = m_listCommand.GetCount();
				for (int k = 0; k < nListCount; ++k) {
					if (m_listCommand.GetItemData(k) == cmd) {
						m_cmbCategory.SetCurSel(i);
						m_listCommand.SetCurSel(k);
						OnSelChangeCommandList(0, 0, NULL);
						return 0;
					}
				}
			}
		}
	}
	return 0;
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
		CString strAccelNew = MtlGetWindowText(m_editNew);

		// Now アクセスキーテキスト
		CString strAccelNow = MtlGetWindowText(m_editNow);

		// 既存コマンドテキスト
		CString strCmd;

		if (nCmdID != 0)
			CToolTipManager::LoadToolTipText(nCmdID, strCmd);

		::SetWindowText(GetDlgItem(IDC_STC01), strCmd);

		// 設定ボタンのEnable調節
		EnableSetBtn();
		return TRUE;
	} else {
		// Deleteキーでコマンドを削除する
		if (lpMsg->hwnd == m_ltAccel && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_DELETE) {
			int nSelIndex = m_ltAccel.GetSelectedIndex();
			if (nSelIndex != -1) {
				UINT cmd = (UINT)m_ltAccel.GetItemData(nSelIndex);
				if (cmd) {
					CAccelerManager accelManager(m_hAccel);
					m_hAccel = accelManager.DeleteAccelerator(cmd);
					m_ltAccel.DeleteItem(nSelIndex);
					if (m_ltAccel.GetItemCount() == nSelIndex)
						--nSelIndex;
					m_ltAccel.SelectItem(nSelIndex);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}



// 設定ボタンのEnable調節
void CKeyBoardPropertyPage::EnableSetBtn()
{
	// New アクセスキーテキスト
	CString strAccelNew = MtlGetWindowText(m_editNew);

	// Now アクセスキーテキスト
	CString strAccelNow = MtlGetWindowText(m_editNow);

	// 既存コマンドテキスト
	CString strCmd	= MtlGetWindowText(GetDlgItem(IDC_STC01));

	bool	bEnableSetBtn = strAccelNew.GetLength() > 0;
	::EnableWindow(GetDlgItem(IDC_BTN_SET), bEnableSetBtn);
}
