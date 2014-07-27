/**
*	@file	DLListWindow.cpp
*/

#include "stdafx.h"
#include "DLListWindow.h"
#include "../MtlWin.h"
#include "../IniFile.h"
#include "../HlinkDataObject.h"
#include "DownloadOptionDialog.h"
#include "DownloadManager.h"
#include "../MainFrame.h"

/////////////////////////////////////////////////////
// CDLListWindow

/// DLするURLリストを設定
void CDLListWindow::SetDLList(const std::vector<CString>& vecURL)
{
	int nCount = (int)vecURL.size();
	for (int i = 0; i < nCount; ++i) {
		m_DLList.AddItem(i, 0, vecURL[i]);
	}
	_SetTitle();
}

// Overrides

/// Viewウィンドウにキーを取られるので自分でTranslateMessageとDispatchMessageを呼ぶ
BOOL CDLListWindow::PreTranslateMessage(MSG* pMsg)
{
	UINT msg = pMsg->message;
	if (m_editDLFolder.m_hWnd == pMsg->hwnd) {
		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	} else if (m_DLList.m_hWnd == pMsg->hwnd) {
		if (msg == WM_KEYDOWN && pMsg->wParam == VK_DELETE) {	// リストビューでdeleteキー押してアイテム削除する
			int nIndex = -1;
			vector<int> vecIndex;
			int nCount = m_DLList.GetItemCount();
			for (int i = 0; i < nCount; ++i) {
				nIndex = m_DLList.GetNextItem(nIndex, LVNI_SELECTED);
				if (nIndex == -1)
					break;

				vecIndex.push_back(nIndex);
			}
			for (auto rit = vecIndex.rbegin(); rit != vecIndex.rend(); ++rit) {
				m_DLList.DeleteItem(*rit);
			}
			_SetTitle();
		}
	}
	return FALSE;
}


DROPEFFECT CDLListWindow::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	if (_MtlIsHlinkDataObject(pDataObject))
		return DROPEFFECT_LINK | DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
}


DROPEFFECT CDLListWindow::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (_MtlIsHlinkDataObject(pDataObject))
		return DROPEFFECT_LINK | DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
}


DROPEFFECT CDLListWindow::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	vector<CString>	vecUrl;
	if (GetDonutURLList(pDataObject, vecUrl) == false) {
		CString strText;
		if (   MtlGetHGlobalText(pDataObject, strText)
			|| MtlGetHGlobalText(pDataObject, strText, CF_SHELLURLW))
		{
			vecUrl.push_back(strText);
		}
	}

	if (vecUrl.empty() == false) {
		SetDLList(vecUrl);
	}
	return DROPEFFECT_LINK | DROPEFFECT_COPY;
}





BOOL	CDLListWindow::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_nDownloading	= 0;
	WM_DLCOMPLETE	= ::RegisterWindowMessage(REGISTERMESSAGE_DLCOMPLETE);

	DoDataExchange(DDX_LOAD);

	m_editDLFolder = m_cmbDLFolder.GetDlgItem(1001);

	// ダイアログリサイズ初期化
    DlgResize_Init(true, true, WS_THICKFRAME | WS_CLIPCHILDREN);

	for (int i = 1; i <= 5; ++i) {
		CString str;
		str.Append(i);
		m_cmbParallelDL.AddString(str);
	}

	/* 設定を復元する */
	CIniFileI pr(CDLOptions::s_DLIniFilePath, _T("DLList"));
	m_cmbParallelDL.SetCurSel(pr.GetValue(_T("ParallelDL"), 0));

	CRect rcWindow;
	rcWindow.top	= pr.GetValue(_T("top"), -1);
	rcWindow.left	= pr.GetValue(_T("left"), -1);
	rcWindow.right	= pr.GetValue(_T("right"), -1);
	rcWindow.bottom	= pr.GetValue(_T("bottom"), -1);
	if (rcWindow != CRect(-1, -1, -1, -1))
		MoveWindow(rcWindow);	// 位置を復元

	// リストビューを準備
	m_DLList.InsertColumn(0, _T("URL"));
	m_DLList.SetColumnWidth(0, pr.GetValue(_T("ColumnWidthURL"), 300));


	int nCount = (int)CDLOptions::s_vecImageDLFolderHistory.size();
	for (int i = 0; i < nCount; ++i) {
		m_cmbDLFolder.AddString(CDLOptions::s_vecImageDLFolderHistory[i]);
	}

	// ImageDLFolderのパスに設定
	m_cmbDLFolder.SetWindowText(CDLOptions::strImgDLFolderPath);


	m_cmbDLOption.AddString(_T("上書きの確認をする"));
	m_cmbDLOption.AddString(_T("上書きの確認をしない"));
	m_cmbDLOption.AddString(_T("連番を付ける"));
	m_cmbDLOption.SetCurSel(pr.GetValue(_T("DLOption"), 0));

	RegisterDragDrop();

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return FALSE;
}

void	CDLListWindow::OnClose()
{
	/* 設定を保存する */
	CIniFileO	pr(CDLOptions::s_DLIniFilePath, _T("DLList"));
	
	CRect rc;
	GetWindowRect(&rc);
	pr.SetValue(rc.top, _T("top"));
	pr.SetValue(rc.left, _T("left"));
	pr.SetValue(rc.right, _T("right"));
	pr.SetValue(rc.bottom, _T("bottom"));

	pr.SetValue(m_cmbParallelDL.GetCurSel(), _T("ParallelDL"));

	pr.SetValue(m_DLList.GetColumnWidth(0), _T("ColumnWidthURL"));

	CString strPath = MtlGetWindowText(m_cmbDLFolder);
	CDLOptions::SavePathToHistory(strPath, CDLOptions::kImageDLFolderHistory);

	pr.SetValue(m_cmbDLOption.GetCurSel(), _T("DLOption"));

	RevokeDragDrop();

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);

	DestroyWindow();
}

/// [...] フォルダ選択ダイアログを表示してDL先のフォルダを設定する
void	CDLListWindow::OnGetDLFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CShellFileOpenDialog	dlg(NULL, FOS_PICKFOLDERS);
	if (dlg.IsNull()) {
		CFolderDialog olddlg;
		if (olddlg.DoModal(m_hWnd) == IDOK) {
			m_cmbDLFolder.SetWindowText(olddlg.GetFolderPath());
		}

	} else {
		if (dlg.DoModal(m_hWnd) == IDOK) {
			CString strFilePath;
			dlg.GetFilePath(strFilePath);
			m_cmbDLFolder.SetWindowText(strFilePath);
		}
	}
}

/// DLを開始する
void	CDLListWindow::OnDLStart(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_strDLFolder = MtlGetWindowText(m_cmbDLFolder);
	MtlMakeSureTrailingBackSlash(m_strDLFolder);

	GetDlgItem(IDC_DLSTART).EnableWindow(FALSE);

	// DL開始
	_DLStart();
}

void	CDLListWindow::OnDLFinish(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	PostMessage(WM_CLOSE);
}

/// 同時DL数を変えたとき
void	CDLListWindow::OnSelChangeParallelDL(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (GetDlgItem(IDC_DLSTART).IsWindowEnabled() == FALSE)
		_DLStart();
}

/// ダブルクリックされたアイテムURLを開く
LRESULT CDLListWindow::OnListViewDoubleClick(LPNMHDR pnmh)
{
	LPNMLISTVIEW pnml = reinterpret_cast<LPNMLISTVIEW>(pnmh);
	if (pnml->iItem != -1) {
		CString strURL;
		m_DLList.GetItemText(pnml->iItem, 0, strURL);
		g_pMainWnd->UserOpenFile(strURL, D_OPENFILE_CREATETAB | D_OPENFILE_ACTIVATE);
	}
	return 0;
}

/// DLManagerから一つのファイルのDL完了通知
LRESULT CDLListWindow::OnDLComplete(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_DLList.GetItemCount() == 0)
		PostMessage(WM_CLOSE);	// 終了する

	--m_nDownloading;
	_DLStart();

	return 0;
}

void	CDLListWindow::_DLStart()
{
	DWORD	dwOption = 0;
	int nOption = m_cmbDLOption.GetCurSel();
	switch (nOption) {
	case 0: dwOption = DLO_OVERWRITEPROMPT; break;
	case 2: dwOption = DLO_USEUNIQUENUMBER; break;
	}

	int nMaxDL	= m_cmbParallelDL.GetCurSel() + 1;
	for (int i = m_nDownloading; i < nMaxDL; ++i) {
		CString strURL;
		m_DLList.GetItemText(0, 0, strURL);

		CDownloadManager::GetInstance()->DownloadStart(strURL, m_strDLFolder, m_hWnd, dwOption);
		
		++m_nDownloading;
		m_DLList.DeleteItem(0);
	}
	_SetTitle();
}


void	CDLListWindow::_SetTitle()
{
	int nCount = m_DLList.GetItemCount();
	CString strTitle;
	strTitle.Format(_T("アイテム数 %d - DLリスト"), nCount);
	SetWindowText(strTitle);
}




















