// DownloadingListView.h

#pragma once

#include <boost/thread.hpp>
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
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

#define DLITEMSHAREDMEMNAME	_T("DonutDLItemSharedMemName")

struct DLItem
{
	WCHAR	strURL[INTERNET_MAX_URL_LENGTH];
	WCHAR	strDomain[INTERNET_MAX_URL_LENGTH];
	WCHAR	strReferer[INTERNET_MAX_URL_LENGTH];
	WCHAR	strFileName[MAX_PATH];	// ファイル名
	WCHAR	strIncompleteFilePath[MAX_PATH];
	WCHAR	strFilePath[MAX_PATH];	// ファイルパス

	int		nProgress;
	int		nProgressMax;
	bool	bAbort;				// 自分でDLを中止した
	int		nDLFailedCount;
	bool	bTextRefreshStop;
	DWORD	filesizeForResume;
	uintptr_t unique;
	HANDLE	hMapForClose;
	DWORD	dwThreadId;

	DLItem()
		: nProgress(0)
		, nProgressMax(0)		
		, bAbort(false)
		, nDLFailedCount(0)
		, bTextRefreshStop(false)
		, filesizeForResume(0)
		, unique(0)
		, hMapForClose(NULL)
		, dwThreadId(0)
	{
		::wcscpy_s(strURL, L"");
		::wcscpy_s(strDomain, L"");
		::wcscpy_s(strReferer, L"");
		::wcscpy_s(strFileName, L"");
		::wcscpy_s(strIncompleteFilePath, L"");
		::wcscpy_s(strFilePath, L"");
	}

	DLItem(const DLItem& item)
	{
		::wcscpy_s(strURL, item.strURL);
		::wcscpy_s(strDomain, item.strDomain);
		::wcscpy_s(strReferer, item.strReferer);
		::wcscpy_s(strFileName, item.strFileName);
		::wcscpy_s(strIncompleteFilePath, item.strIncompleteFilePath);
		::wcscpy_s(strFilePath, item.strFilePath);
		nProgress	= item.nProgress;
		nProgressMax= item.nProgressMax;
		bAbort	= item.bAbort;
		nDLFailedCount = item.nDLFailedCount;
		bTextRefreshStop	= item.bTextRefreshStop;
		filesizeForResume	= item.filesizeForResume;
		unique	= item.unique;
		hMapForClose	= item.hMapForClose;
		dwThreadId		= item.dwThreadId;
	}
};

struct DLItemInfomation {
	DLItem*	pDLItem;
	HANDLE	hMap;
	DWORD	dwState;
	CRect	rcItem;
	int		nImgIndex;
	int		nOldProgress;
	CString strText;
	deque<pair<int, int> > deqProgressAndTime;

	DLItemInfomation(DLItem* pItem, HANDLE hMapFile) : pDLItem(pItem), hMap(hMapFile), dwState(0), nImgIndex(0), nOldProgress(0)
	{	}
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
	int		GetDownloadingCount() const { return (int)m_vecDLItemInfo.size(); }

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
		MSG_WM_COPYDATA		( OnCopyData	)
		MESSAGE_HANDLER_EX( WM_USER_ADDTODOWNLOADLIST , OnAddToList	)
		MESSAGE_HANDLER_EX( WM_USER_REMOVEFROMDOWNLIST, OnRemoveFromList )
		MESSAGE_HANDLER_EX( WM_USER_USESAVEFILEDIALOG , OnUseSaveFileDialog )
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
	BOOL	OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	LRESULT OnAddToList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRemoveFromList(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnUseSaveFileDialog(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnRenameDLItem(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnOpenSaveFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnOpenReferer(UINT uNotifyCode, int nID, CWindow wndCtl);


private:
	void	_AddIcon(DLItemInfomation& item);
	void	_RefreshList();
	int		_HitTest(CPoint pt);
	CRect	_GetItemClientRect(int nIndex);
	void	_DoResume(DLItem* pDLItem);

	// Data members
	std::vector<DLItemInfomation>	m_vecDLItemInfo;
	CFont					m_Font;
	CImageList				m_ImageList;
	std::map<CString, int>	m_mapImgIndex;		// key:拡張子、value:Imagelistのindex
	CImageList				m_ImgStop;

	function<void (DLItem*)> m_funcAddDownloadedItem;

	CToolTipCtrl			m_ToolTip;
	bool					m_bTimer;
	DWORD					m_dwLastTime;
	boost::optional<DLItem>	m_DLItemForPopup;
};



