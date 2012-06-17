// MainFrm.cpp

#include "stdafx.h"
#include "DownloadFrame.h"
#include "DownloadOptionDialog.h"
#include "DownloadManager.h"
#include "../IniFile.h"


/////////////////////////////////////////////////////////////
// CDownloadFrame

// Constructor
CDownloadFrame::CDownloadFrame()
	: m_bVisible(false)
{
}


////////////////////////////////////////
// Message map

int		CDownloadFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// スプリッタウィンドウを作成
	m_wndSplitter.Create(m_hWnd);
	m_wndSplitter.SetSplitterExtendedStyle(0);

	// DownloadingListView作成
	m_wndDownloadingListView.Create(m_wndSplitter, rcDefault, nullptr
		, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER);

	m_wndSplitter.SetSplitterPane(SPLIT_PANE_TOP, m_wndDownloadingListView);

	// DownloadedListView作成
	m_wndDownloadedListView.Create(m_wndSplitter);
	m_wndDownloadedListView.ModifyStyle(0, WS_BORDER);

	m_wndSplitter.SetSplitterPane(SPLIT_PANE_BOTTOM, m_wndDownloadedListView);

	m_wndDownloadingListView.SetAddDownloadedItemfunc(boost::bind(&CDownloadedListView::AddDownloadedItem, &m_wndDownloadedListView, _1));

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
	HWND hWndToolBar = wndToolBar.Create(m_hWnd, rcDefault, 0, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST);
	wndToolBar.SetButtonStructSize();
	wndToolBar.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS);

	CBitmap bmpToolBar = AtlLoadBitmap(IDB_DLTOOLBAR);
	CImageList	imagelist;
	imagelist.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 1);
	imagelist.Add(bmpToolBar, RGB(0, 0, 0));
	wndToolBar.SetImageList(imagelist);

	wndToolBar.AddButton(ID_DLOPENOPTION , TBSTYLE_BUTTON, TBSTATE_ENABLED , 0, _T("オプション"), 0);
	wndToolBar.AddButton(ID_SET_DLFOLDER , TBSTYLE_BUTTON, TBSTATE_ENABLED , 1, _T("DL先フォルダを設定する"), 0);
	wndToolBar.AddButton(ID_OPEN_DLFOLDER, TBSTYLE_BUTTON, TBSTATE_ENABLED , 2, _T("DL先フォルダを開く"), 0);
	wndToolBar.AddButton(ID_DLAPP_ABOUT	 , TBSTYLE_BUTTON, TBSTATE_ENABLED , 3, _T("About"), 0);

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

//---------------------------------------------
/// ウィンドウ破棄されるとき : ウィンドウの位置サイズを保存する
void	CDownloadFrame::OnDestroy()
{
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

}

//-------------------------------------
/// オプション設定画面を開く
LRESULT CDownloadFrame::OnOpenOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDLOptionDialog dlg;
	if  (dlg.DoModal() == IDOK) {
		// 設定の更新を通知
		::SendMessage(m_hWndParent, WM_SETDLCONFIGTOGLOBALCONFIG, 0, 0);
		//m_wndDownloadingListView.SetDLFolder(CDLOption::strDLFolderPath);
	}
	return 0;
}

//-------------------------------------
/// DL先フォルダを設定する
LRESULT CDownloadFrame::OnSetDLFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString strPath;
	CShellFileOpenDialog SHdlg(NULL, FOS_PICKFOLDERS);
	if (SHdlg.IsNull() == false) {
		if (SHdlg.DoModal(m_hWnd) == IDOK) {
			SHdlg.GetFilePath(strPath);
		}
	} else {
		CFolderDialog dlg(NULL, _T("フォルダを選択してください。"));
		if (dlg.DoModal(m_hWnd) == IDOK) {
			strPath = dlg.GetFolderPath();
			
		}
	}
	if (strPath.IsEmpty() == FALSE) {
		MTL::MtlMakeSureTrailingBackSlash(strPath);
		CDLOptions::strDLFolderPath	   = strPath;
		CDLOptions::_SavePathHistory(strPath, CDLOptions::s_vecDLFolderHistory);
		CDLOptions::SaveProfile();
	}
	return 0;
}

//-------------------------------------
/// DL先フォルダを開く
LRESULT CDownloadFrame::OnOpenDLFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::ShellExecute(NULL, NULL, CDLOptions::strDLFolderPath, NULL, NULL, SW_NORMAL);
	return 0;
}

//-------------------------------------
/// バージョン情報を表示
LRESULT CDownloadFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CAboutDlg dlg;
	//dlg.DoModal();
	MessageBox(_T("　　　version 3.2　　　"), _T("DownloadManager"));
	return 0;
}


//-------------------------------------
/// ウィンドウが破棄されないように＆ウィンドウのサイズを控える
void	CDownloadFrame::OnClose()
{
	ShowWindow(SW_HIDE);
	//SetParent(m_hWndParent);
	m_bVisible = false;
	GetWindowRect(&m_rcWindowPos);
}






















