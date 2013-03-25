/**
 *	@file	MainFrame.h
 *	@brief	interface of the CMainFrame class
 *	@note
 *		+++ mainfrm.h を ヘッダとcppに分割。また、クラス名に併せてファイル名もMainFrameに変更.
 */
// 本気で書き直したくなってきた(minit)
/////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////
// CChildFrameClient

class CChildFrameClient : public CWindowImpl<CChildFrameClient>
{
public:
	DECLARE_WND_CLASS_EX(_T("DonutChildFrameClient"), CS_BYTEALIGNCLIENT, COLOR_APPWORKSPACE)

	CChildFrameClient();

	void	SetGetSearchStringFunc(function<bool (CString&)> func) { m_funcGetSearchString = func; }

	HWND	Create(HWND hWndMainFrame);

	void	SetChildFrameWindow(HWND hWndChildFrame);
	HWND	GetActiveChildFrameWindow() const { return m_hWndChildFrame; }

	// Message map
	BEGIN_MSG_MAP( CChildFrameClient )
		MSG_WM_SIZE( OnSize )
		MSG_WM_ERASEBKGND( OnEraseBkgnd )
	END_MSG_MAP()

	void OnSize(UINT nType, CSize size);
	BOOL OnEraseBkgnd(CDCHandle dc);

private:
	// Data members
	HWND	m_hWndChildFrame;
	function<bool (CString&)>	m_funcGetSearchString;
	CBitmap	m_bmpBackground;
};


//////////////////////////////////////////////////////////////////////////////
// CMainFrame

class CMainFrame
{
public:

	struct OpenMultiFileData {
		CString strTitle;
		CString strURL;
		DWORD	DLCtrl;
		DWORD	ExStyle;
		DWORD	AutoRefresh;

		OpenMultiFileData(LPCTSTR url, DWORD  dlctrl = -1, DWORD exstyle = -1, DWORD autorefresh = 0)
			: strURL(url), DLCtrl(dlctrl), ExStyle(exstyle), AutoRefresh(autorefresh)
		{	}
	};


	CMainFrame();
	~CMainFrame();

	HWND	CreateEx();
	void	StartupMainFrameStyle(int nCmdShow, bool bTray);
	void	RestoreAllTab(LPCTSTR strFilePath = nullptr, bool bCloseAllTab = false);

	void	UserOpenFile(LPCTSTR url, DWORD openFlags, 
						 DWORD DLCtrl = -1, 
						 DWORD ExStyle = -1, 
						 DWORD AutoRefresh = 0);
	void	UserOpenMultiFile(const std::vector<OpenMultiFileData>& vecOpenData);

	HWND	GetHWND();
	HWND	GetActiveChildFrameHWND();
	CString GetActiveLocationURL();

private:
	class Impl;
	Impl* pImpl;
};

