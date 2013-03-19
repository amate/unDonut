/**
*	@file	PopupMenu.h
*	@brief	カスタムポップアップメニュー
*/

#pragma once

#include <boost\thread.hpp>
#include "MtlWin.h"
#include "MtlMisc.h"
#include "Misc.h"
#include "LinkPopupMenu.h"
#include "BasePopupMenu.h"

////////////////////////////////////////////////////////////////////////
// サブメニューに CRecentClosedTabPopupMenu を表示するためのクラス

class CFilePopupMenu : public CBasePopupMenuImpl<CFilePopupMenu>
{
public:
	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;
};


////////////////////////////////////////////////////////////////////////
// 最近閉じたタブ サブメニューの実装クラス

// 前方宣言
class CRecentClosedTabList;
struct ChildFrameDataOnClose;


class CRecentClosedTabPopupMenu : 
	public CBasePopupMenuImpl<CRecentClosedTabPopupMenu>,
	public CMessageFilter
{
public:
	static void	SetRecentClosedTabList(CRecentClosedTabList* pList) { s_pRecentClosedTabList = pList; }

	// Overrides
	void DoPaint(CDCHandle dc);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	BEGIN_MSG_MAP( CRecentClosedTabPopupMenu )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_MBUTTONDOWN( OnMButtonDown )
		MSG_WM_MOUSEMOVE( OnMouseMove )
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		CHAIN_MSG_MAP( CBasePopupMenuImpl<CRecentClosedTabPopupMenu> )
	END_MSG_MAP()

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);

private:
	void InitMenuItem() override;
	void _InitTooltip();
	int	ComputeWindowWidth() override;

	CToolTipCtrl	m_tip;
	CString			m_strTipText;

	static CRecentClosedTabList*	s_pRecentClosedTabList;
};

////////////////////////////////////////////////////////////////////////
// コマンドバーのチェブロンメニュー表示用

class CChevronPopupMenu : public CBasePopupMenuImpl<CChevronPopupMenu>
{
public:
	CChevronPopupMenu(int nIndex) : m_nIndex(nIndex) {	}

	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;
	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override;

private:
	int	m_nIndex;

};

////////////////////////////////////////////////////////////////////////
// ツールバーのチェブロンメニュー表示用

class CToolBarChevronPopupMenu : public CBasePopupMenuImpl<CToolBarChevronPopupMenu>
{
public:
	CToolBarChevronPopupMenu() { }

	void	AddCreater(HMENU hMenu, function<IBasePopupMenu* ()> func) { m_vecCreateSubMenuFactory.emplace_back(hMenu, func); }

	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;

private:
	std::vector<std::pair<HMENU, function<IBasePopupMenu* ()>>>	m_vecCreateSubMenuFactory;

};

////////////////////////////////////////////////////////////////////////////
// お気に入りグループのルートメニュー実装
class CRootFavoriteGroupPopupMenu : public CBasePopupMenuImpl<CRootFavoriteGroupPopupMenu>
{
public:
	static void	LoadFavoriteGroup();

	static std::vector<CString>*	GetFavoriteGroupFilePathList() { return &s_vecFavoriteGroupFilePath; }

	static void SetRefreshNotify(HWND hWnd, std::function<void ()> callback, bool bRegister);

	BEGIN_MSG_MAP( CRootFavoriteGroupPopupMenu )
		CHAIN_MSG_MAP( CBasePopupMenuImpl<CRootFavoriteGroupPopupMenu> )
	END_MSG_MAP()

private:
	virtual void InitMenuItem() override;
	virtual void OnClick(const CPoint& pt, bool bLButtonUp = false) override;

	// Constants
	enum { kFavoriteGroupFirstID = 40000, };

	// Data members
	static std::vector<CString>	s_vecFavoriteGroupFilePath;
	static std::vector<std::pair<HWND, std::function<void ()>>>		s_vecfuncRefreshNotify;
};


////////////////////////////////////////////////////////////////////////
// お気に入りのルートメニュー実装

class CRootFavoritePopupMenu : public CBasePopupMenuImpl<CRootFavoritePopupMenu>
{
public:

	static void	LoadFavoriteBookmark();
	static void JoinSaveBookmarkThread();

	// for FavoriteEditDialog
	static vector<unique_ptr<LinkItem> >* GetBookmarkListPtr() {
		return &s_BookmarkList;
	}
	static CCriticalSection&	GetBookmarkListCriticalSection() { 
		return s_csBookmarkLock;
	}
	static bool IsLoadingBookmarkList() { return s_bBookmarkLoading; }

	static void SaveFavoriteBookmark() { _SaveFavoriteBookmark(); }

	static void SetRefreshNotify(HWND hWnd, std::function<void ()> callback, bool bRegister);
	static void NotifyRefresh();

	// オプションから呼ばれる
	static void LinkImportFromFolder(LPCTSTR folder);
	static void LinkExportToFolder(LPCTSTR folder, bool bOverWrite);

	// Overrides
	virtual IBasePopupMenu* CreateSubMenu(int nIndex) override;
	virtual void	DoTrackPopupMenu(CMenuHandle menu, CPoint ptLeftBottom, HWND hWndParent) override;
	virtual void	DoTrackSubPopupMenu(CMenuHandle menu, CRect rcClientItem, HWND hWndParent, int nInheritIndex) override;

	BEGIN_MSG_MAP( CRootFavoritePopupMenu )
		MSG_WM_DESTROY	( OnDestroy	)
		MESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnMouseWheel	)
		MESSAGE_HANDLER_EX( WM_CLOSEBASESUBMENU, OnCloseBaseSubMenu )
		MESSAGE_HANDLER_EX( WM_UPDATESUBMENUITEMPOS, OnUpdateSubMenuItemPosition	)
		MESSAGE_HANDLER_EX( WM_SAVELINKBOOKMARK, OnSaveLinkBookmark	)
		MESSAGE_HANDLER_EX( WM_GETROOTLINKFOLDERPTR	, OnGetRootLinkFolderPtr	)
		CHAIN_MSG_MAP( CBasePopupMenuImpl<CRootFavoritePopupMenu> )
	END_MSG_MAP()

	void OnDestroy();
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSaveLinkBookmark(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetRootLinkFolderPtr(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return reinterpret_cast<LRESULT>(&s_BookmarkList);
	}
	
private:
	static void	_SaveFavoriteBookmark();

	// Data members
	static vector<unique_ptr<LinkItem> >	s_BookmarkList;
	static CCriticalSection					s_csBookmarkLock;
	static bool								s_bSaveBookmark;
	static boost::thread					s_SaveBookmarkListThread;
	static bool								s_bBookmarkLoading;
#if _MSC_VER >= 1700
	static std::atomic<bool>				s_bCancel;
#else
	static bool	s_bCancel;
#endif
	static std::vector<std::pair<HWND, std::function<void ()>>>		s_vecfuncRefreshNotify;

	CLinkPopupMenu*	m_pLinkSubMenu;
	HWND	m_hWndLinkBar;

};


