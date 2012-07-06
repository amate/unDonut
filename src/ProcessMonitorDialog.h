/**
*	@file	ProcessMonitorDialog.h
*	@brief	プロセス モニター
*/

#pragma once

#include "resource.h"
#include <wbemidl.h>
#include <map>


class CProcessMonitorDialog : 
	public CDialogImpl<CProcessMonitorDialog>,
	public CDialogResize<CProcessMonitorDialog>
{
public:
	enum { IDD = IDD_PROCESS_MONITOR };

	CProcessMonitorDialog(function<void (function<void (HWND)>) > tabbarForEach, bool bMode);

	BEGIN_DLGRESIZE_MAP( CProcessMonitorDialog )
		DLGRESIZE_CONTROL( IDC_LIST_PROCESS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL( ID_KILLPROCESS, DLSZ_MOVE_Y | DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP( CProcessMonitorDialog ) 
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE	( OnSize	)
		MESSAGE_HANDLER_EX( WM_CHANGESIZE, OnChangeSize )
		MSG_WM_TIMER( OnTimer	)
		MSG_WM_DRAWITEM( OnDrawItem	)
		NOTIFY_HANDLER_EX( IDC_LIST_PROCESS, LVN_ITEMCHANGED, OnListViewItemChanged	)
		COMMAND_ID_HANDLER_EX( IDCANCEL	, OnCancel	)
		COMMAND_ID_HANDLER_EX( ID_KILLPROCESS	, OnKillProcess )
		CHAIN_MSG_MAP( CDialogResize<CProcessMonitorDialog> ) 
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSize(UINT nType, CSize size);
	LRESULT OnChangeSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	LRESULT OnListViewItemChanged(LPNMHDR pnmh);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnKillProcess(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	// Constants
	enum { kRefreshTimerID = 1, kRefreshInterval = 1000, WM_CHANGESIZE = WM_APP + 1,};

	// Data members
	struct ItemData {
		DWORD	processId;
		HWND	hWnd;
		int		iconIndex;
		LPCTSTR	strTitle;
		bool	bDrawSep;
		ItemData(DWORD id, HWND h, int i, LPCTSTR title) : processId(id), hWnd(h), iconIndex(i), strTitle(title), bDrawSep(true)
		{	}
	};
	std::map<DWORD, std::vector<ItemData> > m_mapItem;
	CListViewCtrl	m_listview;
	CImageList		m_imagelist;
	function<void (function<void (HWND)>) > m_funcTabBarForEach;
	bool	m_bMultiProcessMode;

};










