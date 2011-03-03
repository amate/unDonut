/**
 *	@file	DebugWindow.cpp
 *	@brief	とりあえず的な適当デバッグ用ダイアログを生成するクラス
 */
#include "stdafx.h"
#include "DebugWindow.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef NDEBUG

CDebugWindow *		CDebugWindow::s_pThis = NULL;




void CDebugWindow::Create(HWND hWndParent)
{
	if (g_cnt_b_use_debug_window) {
		CWindow hWnd = baseclass::Create(hWndParent);
		hWnd.ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
		s_pThis = this;

		::DeleteFile(_GetFilePath(_T("log.txt")));
	}
}



void CDebugWindow::Destroy()
{
	if (g_cnt_b_use_debug_window) {
		if ( IsWindow() ) {
			baseclass::DestroyWindow();
		}
		s_pThis = NULL;
	}
}



void CDebugWindow::OutPutString(LPCTSTR pstrFormat, ...)
{
	if (!s_pThis)
		return;

	if ( !s_pThis->m_wndEdit.IsWindow() )
		return;

	if (!g_cnt_b_use_debug_window)
		return;

	CString strText;
	{
		va_list args;
		va_start(args, pstrFormat);
		
		strText.FormatV(pstrFormat, args);

		va_end(args);
	}

	FILE *fp = _tfopen(_GetFilePath(_T("log.txt")), _T("a"));
	if (fp) {
		_ftprintf(fp, _T("%s"), strText);
		fclose(fp);
	}
	strText.Replace(_T("\n"), _T("\r\n"));
//	if (bReturn) strText += _T("\r\n");

	s_pThis->m_wndEdit.AppendText(strText);
}

void CDebugWindow::OutPutMenu(CMenuHandle menu)
{
	OutPutString(_T("=======================\n"));
	for (int i = 0; i < menu.GetMenuItemCount(); ++i) {
		CString strText;
		menu.GetMenuString(i, strText, MF_BYPOSITION);
		UINT uCmdID = menu.GetMenuItemID(i);
		if (uCmdID == 0) { strText = _T("――――――"); }
		if (uCmdID == -1) {	// サブメニュー？
			OutPutString(_T("           : > %s\n"), strText);
		} else {
			OutPutString(_T("%05d : %s\n"), uCmdID, strText);
		}
	}
	OutPutString(_T("=======================\n\n"));

}


BOOL	CDebugWindow::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	SetMsgHandled(FALSE);

	//CRect rc(0, 170, 300, 600);
	CRect rc(-300, 0, 0, 500);
	MoveWindow(rc);

	m_wndEdit = GetDlgItem(IDC_EDIT1);
	//m_wndEdit.Create(m_hWnd, rc, NULL, ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL);

	// メッセージループにメッセージフィルタとアイドルハンドラを追加
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);
//	pLoop->AddIdleHandler(this);

	SetWindowPos(HWND_TOPMOST, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	OnSize(0, CSize(300, 600));

	return TRUE;
}

void	CDebugWindow::OnDestroy()
{
	// メッセージループからメッセージフィルタとアイドルハンドラを削除
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
	//pLoop->RemoveIdleHandler(this);
}


LRESULT CDebugWindow::OnSize(UINT, CSize size)
{
	SetMsgHandled(FALSE);

	if (m_wndEdit.IsWindow()) {
		CRect rc;
		GetClientRect(&rc);
		::InflateRect(&rc, -4, -4);
		m_wndEdit.MoveWindow(rc);
	}
	return 0;
}


void	CDebugWindow::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DestroyWindow();
}


#endif
