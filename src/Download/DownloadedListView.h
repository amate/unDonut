// DownloadedListView.h

#pragma once

#include "DownloadingListView.h"

typedef ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
    LVS_REPORT | LVS_SHOWSELALWAYS>   CCustomSortListViewCtrlTraits;

///////////////////////////////////////////////
// CDownloadedListView

class CDownloadedListView 
	: public CSortListViewCtrlImpl<CDownloadedListView, CListViewCtrl, CCustomSortListViewCtrlTraits>
{
public:
    DECLARE_WND_SUPERCLASS(_T("DonwloadedListView"), GetWndClassName())

private:
	std::vector<std::unique_ptr<DLItem> >	m_vecpDLItem;

public:
	void	AddDownloadedItem(DLItem* pItem);


	// Message map
    BEGIN_MSG_MAP_EX(CDownloadedListView)
        MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		CHAIN_MSG_MAP( __super )
		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK, OnListRClick)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	LRESULT OnListRClick(LPNMHDR pnmh);

};