// DownloadingListView.h

#pragma once

#include <boost/thread.hpp>
#include <atlctrls.h>
#include <atlscrl.h>
#include "../MtlDragDrop.h"
#include "CustomBindStatusCallBack.h"
#include "../resource.h"

class CDownloadedListView;

enum EDLItem_Ex
{
	DLITEMSTATE_SELECTED = 0x01,
	//DLITEMSTATE_
};

struct DLItem
{

	CString strURL;
	CString	strDomain;
	CString strReferer;
	CRect	rcItem;
	CString strFileName;	// ファイル名
	CString	strIncompleteFilePath;
	CString strFilePath;	// ファイルパス
	int		nImgIndex;
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
	{
		strText.GetBufferSetLength(512);
		strText.ReleaseBuffer(512);
	}
};



////////////////////////////////////////////////////
// CDownloadingListView

class CDownloadingListView 
	: public CScrollWindowImpl<CDownloadingListView>
	, public CThemeImpl<CDownloadingListView>
	, public IDropTargetImpl<CDownloadingListView>
{
public:
	//DECLARE_WND_SUPERCLASS(_T("DownloadingListView"), GetWndClassName())

	// Constants
	enum {
		ItemHeight = 50,
		UnderLineHeight = 1,

		cxImageMargin = 3,
		cyImageMargin = (ItemHeight - 32 ) / 2,

		cxFileNameMargin = cxImageMargin + 32 + cxImageMargin,
		cyFileNameMargin = 3,

		ProgressHeight = 12,
		cyProgressMargin = 21,
		cleftProgressMargin = 30,

		cxStopLeftSpace  = 23,
		cyStopTopMargin  = 19,
		cxStop = 16,
		cyStop = 16,

		TOOLTIPTIMERID	= 2,
		TOOLTIPDELAY	= 250,
	};

	// Constructor/Destructor
	CDownloadingListView();
	~CDownloadingListView();

	void	SetAddDownloadedItemfunc(function<void (DLItem*)> func) { m_funcAddDownloadedItem = func; }

	// Attributes
	int		GetDownloadingCount() const { return (int)m_vecpDLItem.size(); }

	// Overrides
	void	DoPaint(CDCHandle dc);

	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);

	// Message map
    BEGIN_MSG_MAP_EX(CDownloadingListView)
        MSG_WM_CREATE		( OnCreate  )
		MSG_WM_DESTROY		( OnDestroy )
		MSG_WM_SIZE			( OnSize	)
		MSG_WM_LBUTTONDOWN	( OnLButtonDown )
		MSG_WM_RBUTTONUP	( OnRButtonUp )
		MSG_WM_ERASEBKGND	( OnEraseBkgnd )
		MSG_WM_TIMER		( OnTimer )
		MESSAGE_HANDLER_EX( WM_USER_ADDTODOWNLOADLIST , OnAddToList	)
		MESSAGE_HANDLER_EX( WM_USER_REMOVEFROMDOWNLIST, OnRemoveFromList )
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		MSG_WM_MOUSEMOVE	( OnMouseMove )
		COMMAND_ID_HANDLER_EX( ID_RENAME_DLITEM		, OnRenameDLItem )
		COMMAND_ID_HANDLER_EX( ID_OPEN_SAVEFOLDER	, OnOpenSaveFolder )
		COMMAND_ID_HANDLER_EX( ID_OPEN_REFERER		, OnOpenReferer )
        CHAIN_MSG_MAP( CScrollWindowImpl<CDownloadingListView> )
        CHAIN_MSG_MAP_ALT( CScrollWindowImpl<CDownloadingListView>, 1)
		CHAIN_MSG_MAP(CThemeImpl<CDownloadingListView>)
    END_MSG_MAP()


    int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	void	OnSize(UINT nType, CSize size);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnRButtonUp(UINT nFlags, CPoint point);
	BOOL	OnEraseBkgnd(CDCHandle dc) { return TRUE; }
	void	OnTimer(UINT_PTR nIDEvent);
	LRESULT OnAddToList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRemoveFromList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnRenameDLItem(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnOpenSaveFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnOpenReferer(UINT uNotifyCode, int nID, CWindow wndCtl);


private:
	void	_AddItemToList(DLItem* pItem);
	void	_AddIcon(DLItem *pItem);
	void	_RefreshList();
	int		_HitTest(CPoint pt);
	CRect	_GetItemClientRect(int nIndex);

	// Data members
	std::vector<std::unique_ptr<DLItem> >	m_vecpDLItem;
	CFont					m_Font;
	CImageList				m_ImageList;
	std::map<CString, int>	m_mapImgIndex;		// key:拡張子、value:Imagelistのindex
	CImageList				m_ImgStop;

	function<void (DLItem*)> m_funcAddDownloadedItem;

	CToolTipCtrl			m_ToolTip;
	bool					m_bTimer;
	DLItem*	m_pItemPopup;
};



