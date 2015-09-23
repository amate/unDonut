/**
 *	@file	DonutLinkBarCtrl.cpp
 *	@brief	ÉäÉìÉNÉoÅ[
 */

#include "stdafx.h"
#include "DonutLinkBarCtrl.h"
#include <codecvt>
#include <atomic>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>
#include <atlenc.h>
#include "MtlMisc.h"
#include "MtlWin.h"
#include "MtlDragDrop.h"
#include "Misc.h"
#include "GdiplusUtil.h"
#include "HlinkDataObject.h"
#include "ExStyle.h"
#include "Donut.h"
#include "LinkPopupMenu.h"
#include "FaviconManager.h"
#include "option/LinkBarPropertyPage.h"

using boost::property_tree::wptree;
using namespace boost::property_tree::xml_parser;


///////////////////////////////////////////////////////
// CDonutLinkBarCtrl::Impl

class CDonutLinkBarCtrl::Impl : 
	public CDoubleBufferWindowImpl<CDonutLinkBarCtrl::Impl>,
	public CThemeImpl<CDonutLinkBarCtrl::Impl>,
	public CTrackMouseLeave<CDonutLinkBarCtrl::Impl>,
	public IDropTargetImpl<CDonutLinkBarCtrl::Impl>,
	public IDropSourceImpl<CDonutLinkBarCtrl::Impl>,
	public CMessageFilter
{
public:
	DECLARE_WND_CLASS(_T("DonutLinkBarCtrl"))

	// Constants
	enum { 
		kTopBottomPadding = 7,//6,
		kRightLeftPadding = 7,//6,
		kRightTextMargin = 2,
		//kMaxItemTextWidth	= 100,
		kLeftMargin	= 2,
		kDragInsertHitWidthOnFolder = 7,

		kcxIcon = 16,
		kcyIcon = 16,
		kTopIconMargin = 3,
		kLeftIconMargin = 3,

		kLeftTextPadding = kLeftIconMargin * 2 + kcxIcon,

		WM_REFRESH = WM_APP + 20,
	};

	enum hitTestDragCategory {
		htNone,
		htInsert,
		htFolder,
		htChevron,
	};

	enum ChevronState {
		ChvNormal	= CHEVS_NORMAL,
		ChvHot		= CHEVS_HOT,
		ChvPressed	= CHEVS_PRESSED,
	};

	// Constructer/Destructor
	Impl();
	~Impl();

	void	SetFont(HFONT hFont);
	void	Refresh();

	void	LinkImportFromFolder(LPCTSTR folder);
	void	LinkExportToFolder(LPCTSTR folder);

	// Overrides
	void DoPaint(CDCHandle dc);
	void OnTrackMouseMove(UINT nFlags, CPoint pt);
	void OnTrackMouseLeave();
	BOOL PreTranslateMessage(MSG* pMsg) override;

	// IDropTargetImpl
	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	void	OnDragLeave();

	BEGIN_MSG_MAP( CDonutLinkBarCtrl::Impl )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_SIZE( OnSize )
		MESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnMouseWheel	)
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_LBUTTONUP( OnLButtonUp )
		MSG_WM_RBUTTONUP( OnRButtonUp )
		MSG_WM_MBUTTONDOWN( OnMButtonDown )

		MSG_WM_KILLFOCUS( OnKillFocus )

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		MESSAGE_HANDLER_EX( WM_CLOSEBASESUBMENU, OnCloseBaseSubMenu )
		MESSAGE_HANDLER_EX( WM_UPDATESUBMENUITEMPOS, OnUpdateSubMenuItemPosition	)
		MESSAGE_HANDLER_EX( WM_SAVELINKBOOKMARK, OnSaveLinkBookmark )
		MESSAGE_HANDLER_EX( WM_GETROOTLINKFOLDERPTR	, OnGetRootLinkFolderPtr	)
		MESSAGE_HANDLER_EX( WM_REFRESH				, OnRefresh )
		CHAIN_MSG_MAP( CDoubleBufferWindowImpl<CDonutLinkBarCtrl::Impl> )
		CHAIN_MSG_MAP( CThemeImpl<CDonutLinkBarCtrl::Impl> )
		CHAIN_MSG_MAP( CTrackMouseLeave<CDonutLinkBarCtrl::Impl> )
	END_MSG_MAP()

	 int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	 void OnDestroy();
	 void OnSize(UINT nType, CSize size);
	 LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnLButtonUp(UINT nFlags, CPoint point);
	 void OnRButtonUp(UINT nFlags, CPoint point);
	 void OnMButtonDown(UINT nFlags, CPoint point);

	 void OnKillFocus(CWindow wndFocus);

	 LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);
	 LRESULT OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);
	 LRESULT OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		 return s_pSubMenu->SendMessage(WM_UPDATESUBMENUITEMPOS);
	 }
	 LRESULT OnSaveLinkBookmark(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		 _SaveLinkBookmark();
		 return 0;
	 }
	 LRESULT OnGetRootLinkFolderPtr(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		 return reinterpret_cast<LRESULT>(&m_BookmarkList);
	 }
	 LRESULT OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		 Refresh();
		 return 0;
	 }

private:
	void	_InitTooltip();
	void	_RefreshBandInfo();
	void	_LoadLinkBookmark();
	void	_SaveLinkBookmark();
	void	_UpdateItemPosition();
	bool	_IsValidIndex(int nIndex) const { 
		return 0 <= nIndex && nIndex < (int)m_BookmarkList.size(); 
	}
	boost::optional<int>	_HitTest(const CPoint& point);
	void	_HotItem(int nNewHotIndex);
	void	_PressItem(int nPressedIndex);
	void	_CloseSubMenu();
	bool	_DoPopupSubMenu(int nIndex, bool bPressItem = true);
	void	_PopupChevronMenu();
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	void	_DoDragDrop(const CPoint& pt, UINT nFlags, int nIndex);
	void	_DrawInsertEdge(const CPoint& ptRightBottom);
	void	_ClearInsertionEdge();
	bool	_IsDragNone(int nOverIndex, const CPoint& point);
	int		_HitTestOnDragging(const CPoint& point, hitTestDragCategory& htc);
	bool	_HitTestChevron(const CPoint& pt);
	void	_ChevronStateChange(ChevronState state);
	void	_DrawItem(CDCHandle dc, const LinkItem& item);
	void	_DrawDragImage(CDCHandle dc, const LinkItem& item);
	void	_ClearLinkBookmark();

	// Data members
	CFont	m_font;
	CTheme	m_themeRebar;
	vector<unique_ptr<LinkItem> >	m_BookmarkList;
	int		m_nHotIndex;
	int		m_nPressedIndex;
	ChevronState	m_ChevronState;
	CSize	m_ChevronSize;
	static unique_ptr<CLinkPopupMenu>	s_pSubMenu;
	static HHOOK			s_hHook;
	static HWND				s_hWnd;
	CPoint	m_ptInvalidateOnDrawingInsertionEdge;
	int		m_nNowDragItemIndex;

	std::pair<LinkFolderPtr, int>	m_DragItemData;
	bool	m_bAcceptDrag;
	bool	m_bDragItemIsLinkFile;
	int		m_nBeforeBookmarkListSize;

	CToolTipCtrl	m_tip;
	CString			m_strTipText;

	std::atomic_bool	m_bLoading;
	boost::thread	m_thread_load;
	boost::thread	m_thread_save;
};

unique_ptr<CLinkPopupMenu>	CDonutLinkBarCtrl::Impl::s_pSubMenu;
HHOOK			CDonutLinkBarCtrl::Impl::s_hHook	= NULL;
HWND			CDonutLinkBarCtrl::Impl::s_hWnd		= NULL;


#include "DonutLinkBarCtrl.inl"


///////////////////////////////////////////////////////
// CDonutLinkBarCtrl

CDonutLinkBarCtrl::CDonutLinkBarCtrl()
	: pImpl(new Impl)
{}

CDonutLinkBarCtrl::~CDonutLinkBarCtrl()
{
	delete pImpl;
}

HWND	CDonutLinkBarCtrl::Create(HWND hWndParent)
{
	HWND hwnd = pImpl->Create(hWndParent, CRect(0, 0, -1, 20));
	pImpl->SetDlgCtrlID(IDC_LINKBAR);
	return hwnd;
}


void	CDonutLinkBarCtrl::SetFont(HFONT hFont)
{
	pImpl->SetFont(hFont);
}


void	CDonutLinkBarCtrl::Refresh()
{
	pImpl->Refresh();
}

void	CDonutLinkBarCtrl::LinkImportFromFolder(LPCTSTR folder)
{
	pImpl->LinkImportFromFolder(folder);
}

void	CDonutLinkBarCtrl::LinkExportToFolder(LPCTSTR folder)
{
	pImpl->LinkExportToFolder(folder);
}