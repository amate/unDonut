// MainFrm.h : CMainFrame クラスのインターフェイス
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DownloadingListView.h"
#include "DownloadedListView.h"
#include "CustomBindStatusCallBack.h"
#include "../IniFile.h"
//#include "OptionDialog.h"
//#include "AboutDlg.h"



//////////////////////////////////////////////////////////
// CDownloadFrame

class CDownloadFrame
	: public CFrameWindowImpl<CDownloadFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(_T("DonutDownloadManager"), NULL)
	
	// Constructor
	CDownloadFrame();

	void	SetParentWindow(HWND hWnd) { m_hWndParent = hWnd; }
	void	EnableVisible() { m_bVisible = true; }
	CCustomBindStatusCallBack*	StartBinding();

	// Message map and Handler
	BEGIN_MSG_MAP(CDownloadFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_CLOSE( OnClose )
		COMMAND_ID_HANDLER(ID_DLAPP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_DLOPENOPTION,  OnOpenOption)
		CHAIN_MSG_MAP(CFrameWindowImpl<CDownloadFrame>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	// オプション設定画面を開く
	LRESULT OnOpenOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void	OnClose();

private:
	CHorSplitterWindow		m_wndSplitter;
	CDownloadingListView	m_wndDownloadingListView;
	CDownloadedListView		m_wndDownloadedListView;
	HWND					m_hWndParent;
	bool					m_bVisible;
	CRect					m_rcWindowPos;
	//std::vector<std::unique_ptr<CCustomBindStatusCallBack> >	m_vecpCSCB;

};
