/**
 *	@file	DebugWindow.cpp
 *	@brief	とりあえず的な適当デバッグ用ダイアログを生成するクラス
 */
#include "stdafx.h"
#include "DebugWindow.h"
#include <fstream>
#include <codecvt>
#include <boost\timer.hpp>
#include "..\Misc.h"
#include "..\DonutDefine.h"

#ifdef _DEBUG
static CDebugUtility	g_debugutil;
boost::timer	g_timer;
#endif

/////////////////////////////////////////////////
// CDebugUtility::Impl

class CDebugUtility::Impl
{
public:
	Impl();
	~Impl();

	void	Write(LPCTSTR strFormat, va_list argList);
	void	WriteIn(LPCTSTR strFormat, va_list argList);

	void	_WriteConsole(LPCTSTR str);

	HANDLE	m_hOut;
	HWND	m_hWndMainFrame;
};


//------------------------------------
CDebugUtility::Impl::Impl()  : m_hOut(NULL), m_hWndMainFrame(NULL) 
{
	HWND hWnd = ::FindWindow(DONUT_WND_CLASS_NAME, NULL);
	if (hWnd) {
		m_hWndMainFrame = hWnd;
		return ;	// マルチプロセスモードの時コンソールが乱立しないように
	} else {
		// ログファイル作成
		FILE* fp = _wfopen(Misc::GetExeDirectory() + _T("log.txt"), L"w");
		fclose(fp);
	}
	::AllocConsole();
	m_hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

//------------------------------------
CDebugUtility::Impl::~Impl()
{
	::FreeConsole();
}

//------------------------------------
void	CDebugUtility::Impl::Write(LPCTSTR strFormat, va_list argList)
{
	CString str;
	str.FormatV(strFormat, argList);
	_WriteConsole(str);
}


//------------------------------------
void	CDebugUtility::Impl::WriteIn(LPCTSTR strFormat, va_list argList)
{
	CString str;
	str.FormatV(strFormat, argList);
	str += _T("\n");
	_WriteConsole(str);
}

//------------------------------------
void	CDebugUtility::Impl::_WriteConsole(LPCTSTR str)
{
	if (m_hWndMainFrame) {
		COPYDATASTRUCT cds;
		cds.dwData	= kDebugTrace;
		cds.lpData	= (LPVOID)str;
		cds.cbData	= (::lstrlen(str) + 1) * sizeof(TCHAR);
		::SendMessage(m_hWndMainFrame, WM_COPYDATA, NULL, (LPARAM)&cds);

	} else {
		static CString strFilePath = Misc::GetExeDirectory() + _T("log.txt");
		std::wfstream	filestream(strFilePath, std::ios::app);
		if (filestream) {
			filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
			filestream << str;
		}
		DWORD dwWrite;
		::WriteConsole(m_hOut, str, lstrlen(str), &dwWrite, NULL);
	}
}




/////////////////////////////////////////////////
// CDebugUtility

CDebugUtility::Impl* CDebugUtility::pImpl = NULL;

//------------------------------------
CDebugUtility::CDebugUtility()
{ 
	pImpl = new Impl;
}

//------------------------------------
CDebugUtility::~CDebugUtility()
{
	delete pImpl;
}

//------------------------------------
void CDebugUtility::Write(LPCTSTR pstrFormat, ...)
{
	va_list args;
	va_start(args, pstrFormat);	
	pImpl->Write(pstrFormat, args);
	va_end(args);
}

//------------------------------------
void CDebugUtility::WriteIn(LPCTSTR pstrFormat, ...)
{
	va_list args;
	va_start(args, pstrFormat);	
	pImpl->WriteIn(pstrFormat, args);
	va_end(args);
}

void	CDebugUtility::TimerStart()
{
#ifdef _DEBUG
	g_timer.restart();
#endif
}

void	CDebugUtility::TimerStop(LPCTSTR pstrFormat, ...)
{	
#ifdef _DEBUG
	va_list args;
	va_start(args, pstrFormat);	
	CString str;
	str.FormatV(pstrFormat, args);
	va_end(args);
	CString strTime;
	strTime.Format(_T(" : %.4lf\n"), g_timer.elapsed());

	str += strTime;
	pImpl->_WriteConsole(str);
#endif
}


#if 0
#include "../resource.h"



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
#endif