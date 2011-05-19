// DownloadingListView.h

#pragma once

#include <boost/thread.hpp>
#include <atlctrls.h>
#include <atlscrl.h>
#include "CustomBindStatusCallBack.h"


class CDownloadedListView;

enum EDLItem_Ex
{
	DLITEMSTATE_SELECTED = 0x01,
	//DLITEMSTATE_
};

struct DLItem
{

	CString strURL;
	CString strReferer;
	CRect	rcItem;
	CString strFileName;	// ファイル名
	CString strFilePath;	// ファイルパス
	int		nImgIndex;
	CProgressBarCtrl	wndProgress;
	CString strText;

	DWORD	dwState;
	int		nProgress;
	int		nProgressMax;
	int		nOldProgress;
	deque<pair<int, int> > deq;
	bool	bAbort;

	DLItem()
		: dwState(0)
		, nProgress(0)
		, nProgressMax(0)
		, nOldProgress(0)
		, bAbort(false)
		, strText(_T("　　　　　　　　　　　　　　　　　　　　"))
	{
		
	}
};


////////////////////////////////////////////////////
// CDownloadingListView

class CDownloadingListView 
	: public CScrollWindowImpl<CDownloadingListView>
{
public:
	//DECLARE_WND_SUPERCLASS(_T("DownloadingListView"), GetWndClassName())
	friend class CCustomBindStatusCallBack;

	// Constants
	enum {
		ItemHeight = 50,
		ItemMinWidth = 20000,
		UnderLineHeight = 1,

		cxImageMargin = 3,
		cyImageMargin = (ItemHeight - 32 ) / 2,

		cxFileNameMargin = cxImageMargin + 32 + cxImageMargin,
		cyFileNameMargin = 3,

		ProgressHeight = 12,
		cyProgressMargin = 21,
		cleftProgressMargin = 30,

		TOOLTIPTIMERID	= 2,
		TOOLTIPDELAY	= 250,
	};

	// Constructor/Destructor
	CDownloadingListView(CDownloadedListView* pDLed);
	~CDownloadingListView();


	DLItem*	CreateDLItem();

	int		GetDownloadingCount() const { return (int)m_vecpDLItem.size(); }

	// Overrides
	void	DoPaint(CDCHandle dc);

	// Message map
    BEGIN_MSG_MAP_EX(CDownloadingListView)
        MSG_WM_CREATE	( OnCreate  )
		MSG_WM_DESTROY	( OnDestroy )
		MSG_WM_SIZE		( OnSize	)
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_PARENTNOTIFY( OnParentNotify	)
		MSG_WM_ERASEBKGND( OnEraseBkgnd )
		MSG_WM_TIMER( OnTimer )
		MESSAGE_HANDLER_EX( WM_USER_ADDTODOWNLOADLIST , OnAddToList	)
		MESSAGE_HANDLER_EX( WM_USER_REMOVEFROMDOWNLIST, OnRemoveFromList )
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		MSG_WM_MOUSEMOVE( OnMouseMove )
        CHAIN_MSG_MAP( CScrollWindowImpl<CDownloadingListView> )
        CHAIN_MSG_MAP_ALT( CScrollWindowImpl<CDownloadingListView>, 1)
    END_MSG_MAP()


    int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	void	OnSize(UINT nType, CSize size);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnParentNotify(UINT message, UINT nChildID, LPARAM lParam);
	BOOL	OnEraseBkgnd(CDCHandle dc) { return TRUE; }
	void	OnTimer(UINT_PTR nIDEvent);
	LRESULT OnAddToList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRemoveFromList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);
	void	OnMouseMove(UINT nFlags, CPoint point);

private:
	void	_AddItemToList(DLItem* pItem);
	void	_AddIcon(DLItem *pItem);
	void	_RefreshList();
	int		_HitTest(CPoint pt);

	// Data members
	std::vector<std::unique_ptr<DLItem> >	m_vecpDLItem;
	CFont					m_Font;
	CImageList				m_ImageList;
	std::map<CString, int>	m_mapImgIndex;		// key:拡張子、value:Imagelistのindex
	CImageList				m_ImgStop;

	CDownloadedListView*	m_pDownloadedListView;

	CToolTipCtrl			m_ToolTip;
	bool					m_bTimer;

};



