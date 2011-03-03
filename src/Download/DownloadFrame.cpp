// MainFrm.cpp

#include "stdafx.h"
#include "DownloadFrame.h"
#include "DownloadOptionDialog.h"
#include "DownloadManager.h"


// Constructor
CDownloadFrame::CDownloadFrame()
	: m_wndDownloadingListView(&m_wndDownloadedListView)
	, m_bVisible(false)
{
}


CCustomBindStatusCallBack*	CDownloadFrame::StartBinding()
{
	DLItem* pItem = m_wndDownloadingListView.CreateDLItem();
	CCustomBindStatusCallBack* p = new CCustomBindStatusCallBack(pItem, &m_wndDownloadingListView);
	// 追加
	//m_vecpCSCB.push_back(std::unique_ptr<CCustomBindStatusCallBack>(p));
	return p;
}



////////////////////////////////////////
// Message map

LRESULT CDownloadFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// スプリッタウィンドウを作成
	m_wndSplitter.Create(m_hWnd);
	m_wndSplitter.SetSplitterExtendedStyle(0);

	// DownloadingListView作成
	m_wndDownloadingListView.Create(m_wndSplitter, rcDefault, 0
		, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		| LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER| LVS_SINGLESEL/*| LVS_SHOWSELALWAYS*/);
	//m_wndDownloadingListView.SetView(LV_VIEW_TILE);

	m_wndSplitter.SetSplitterPane(SPLIT_PANE_TOP, m_wndDownloadingListView);

	// DownloadedListView作成
	m_wndDownloadedListView.Create(m_wndSplitter);

	m_wndSplitter.SetSplitterPane(SPLIT_PANE_BOTTOM, m_wndDownloadedListView);

#if 0
	// コマンドバー ウィンドウの作成
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// メニューのアタッチ
	m_CmdBar.AttachMenu(GetMenu());
	// コマンドバー画像の読み込み
	m_CmdBar.LoadImages(IDR_MAINFRAME);
#endif
	// 以前のメニューの削除
	SetMenu(NULL);

	//HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	CToolBarCtrl	wndToolBar;
	HWND hWndToolBar = wndToolBar.Create(m_hWnd, rcDefault, 0, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	wndToolBar.SetButtonStructSize();

	CImageList	imagelist;
	imagelist.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1);
	CBitmap bmpOption	= AtlLoadBitmap(IDB_DLOPTIONS);
	CBitmap bmpHelp		= AtlLoadBitmap(IDB_DLHELP);
	imagelist.Add(bmpOption);
	imagelist.Add(bmpHelp);
	wndToolBar.SetImageList(imagelist);
	wndToolBar.AddButton(ID_DLOPENOPTION, TBSTYLE_BUTTON, TBSTATE_ENABLED,  0, (LPCTSTR)NULL, 0);
	wndToolBar.AddButton(ID_DLAPP_ABOUT, TBSTYLE_BUTTON, TBSTATE_ENABLED, 1, (LPCTSTR)NULL, 0);


	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	//AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	//CreateSimpleStatusBar();

	//m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_hWndClient = m_wndSplitter;

#if 1
	// 位置を復元する
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileI	pr(strIniFile, _T("Main"));
	CRect rcWindow;
	rcWindow.top	= pr.GetValue(_T("top"), -1);
	rcWindow.left	= pr.GetValue(_T("left"), -1);
	rcWindow.right	= pr.GetValue(_T("right"), -1);
	rcWindow.bottom	= pr.GetValue(_T("bottom"), -1);
	if (rcWindow != CRect(-1, -1, -1, -1)) {
		MoveWindow(rcWindow);
	}

	int nSplitterPos = pr.GetValue(_T("SplitterPos"), -1);
	if (nSplitterPos != -1) {
		m_wndSplitter.SetSplitterPos(nSplitterPos);
	}
	if (pr.GetValue(_T("ShowWindow"), FALSE) && CDownloadManager::UseDownloadManager()) {
		ShowWindow(TRUE);
		m_bVisible = true;
	}

	CDLOptions::LoadProfile();
	if (::PathIsDirectory(CDLOptions::strDLFolderPath) == FALSE && CDownloadManager::UseDownloadManager()) {
		MessageBox(_T("ダウンロード先のフォルダが存在しません。\n")
			_T("ダウンロード開始前にオプションから保存先フォルダを設定してください。"));
	}
	//m_wndDownloadingListView.SetDLFolder(CDLOption::strDLFolderPath);
#endif

	return 0;
}


LRESULT CDownloadFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
#if 1
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileO pr(strIniFile, _T("Main"));

	// 位置を保存する
	CRect rcWindow;
	if (m_bVisible) {
		GetWindowRect(rcWindow);
		pr.SetValue(TRUE, _T("ShowWindow"));
	} else {
		rcWindow = m_rcWindowPos;
		pr.SetValue(FALSE, _T("ShowWindow"));
	}
	if (rcWindow.IsRectNull() == FALSE && CDownloadManager::UseDownloadManager()) {
		pr.SetValue(rcWindow.top, _T("top"));
		pr.SetValue(rcWindow.left, _T("left"));
		pr.SetValue(rcWindow.right, _T("right"));
		pr.SetValue(rcWindow.bottom, _T("bottom"));
	}


	int nSplitterPos = m_wndSplitter.GetSplitterPos();
	pr.SetValue(nSplitterPos, _T("SplitterPos"));

#endif

	//bHandled = FALSE;
	return 1;
}

LRESULT CDownloadFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CAboutDlg dlg;
	//dlg.DoModal();
	MessageBox(_T("　　　version 2.2　　　"), _T("DownloadManager"));
	return 0;
}

// オプション設定画面を開く
LRESULT CDownloadFrame::OnOpenOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDLOptionDialog dlg;
	if  (dlg.DoModal() == IDOK) {
		//m_wndDownloadingListView.SetDLFolder(CDLOption::strDLFolderPath);
	}
	return 0;
}

void	CDownloadFrame::OnClose()
{
	ShowWindow(SW_HIDE);
	//SetParent(m_hWndParent);
	m_bVisible = false;
	GetWindowRect(&m_rcWindowPos);
}






















