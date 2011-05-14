// FavoritesMenu.cpp

#include "stdafx.h"
#include "FavoritesMenu.h"
#include <algorithm>
#include <boost/thread.hpp>

#include "DonutDefine.h"
#include "FavoriteOrder.h"
#include "MtlCtrlw.h"
#include "FileNotification.h"
#include "resource.h"
#include "dialog/RenameFileDialog.h"
#include "option/FavoriteMenuDialog.h"

/////////////////////////////////////////////////////////////////
// CExplorerMenu::Impl

class CExplorerMenu::Impl : public CFileNotification
{
	friend CExplorerMenu;
public:
	// Constants
	enum _CmdBarDrawConstants {
		s_kcxGap		  = 1,
		s_kcxTextMargin   = 2,
		s_kcxButtonMargin = 3,
		s_kcyButtonMargin = 3
	};

	enum {
		s_nIDLast	= 0x0100, 
		s_nIDExProp = 1
	};

	struct IgnoreItem {
		CMenuItem*	pItem;
		HMENU		hSubMenu;
		int			nInsertPoint;
		CMenuItemInfo info;
	};


	// Constructro
	Impl(CExplorerMenu* p);
	~Impl();

	void	Init( int 				nInsertPointMenuItemID,
				  const CString&	strNoEntries			= _T("(empty)"),
				  int 				nMinID					= 0x7000,
				  int 				nMaxID					= 0x7FFF,
				  const CString&	strAdditional			= _T("Open This Directory"),
				  int 				nMaxMenuItemTextLength	= 55 );

	BOOL	InstallExplorerMenu(HMENU hMenu);
	void	SetTargetWindow(HWND hWnd);
	void	RefreshMenu();
	void	SetExcuteFunction(std::function<void (const CString&)> func) { m_funcExcute = func; }
	void	OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos);
	CString GetFilePath(const CMenuItemInfo& mii);
	CString GetFilePath(int nID);
	DWORD	GetStyle() const { return m_dwStyle; }
	void	SetStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	CMenuHandle GetFolderMenu(const CString& strPath);
	void	AddIgnoredPath(const CString &strPath);
	void	ResetIgnoredPath();
	bool	IsSubMenu(HMENU hSubMenu);

	void OnExecute(const CString& strFilePath);

	// Message map and handlers
	BEGIN_MSG_MAP(CExplorerMenu)
		HANDLE_MENU_MESSAGE_CONTEXTMENU_EX(m_pContMenu2)
		MSG_WM_DESTROY		( OnDestroy			)
		MSG_WM_MENUSELECT	( OnMenuSelect		)

		MESSAGE_HANDLER_WND( WM_DRAWITEM		, OnDrawItem)
		MESSAGE_HANDLER( WM_MEASUREITEM			, OnMeasureItem)

		MESSAGE_HANDLER_WND( WM_MENURBUTTONUP	, OnRButtonUp)
		
		MESSAGE_HANDLER_WND( SC_VSCROLL			, OnVScroll)

		COMMAND_RANGE_HANDLER_EX(m_nMinID, m_nMaxID, OnCommandRange)
		MESSAGE_HANDLER_EX( WM_FILENOTIFICATION, OnFileNotification )
	END_MSG_MAP()


	void	OnDestroy();
	void	OnMenuSelect(UINT nItemID, UINT nFlags, CMenuHandle menu);
	LRESULT OnRButtonUp(HWND hWnd, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void	OnCommandRange(UINT, int nID, HWND hWndCtrl);
	LRESULT OnFileNotification(UINT uMsg, WPARAM wParam, LPARAM lParam);


private:
	void	_Explore(CMenuHandle menuPopup, int nInsertPoint, const CString &strDirectoryPath);
	void	_AddAdditionalMenuItem( CMenuHandle menuDest,
									int &nInsertPoint,
									int &nBreakCount,
									const CString &strDirectory,
									int nType );
	void	_InsertSeparator(CMenuHandle menuDest, int &nInsertPoint);
	void	_DrawMenuText(CDCHandle& dc, RECT& rc, LPCTSTR lpstrText, COLORREF color);
	void	_thread_SetMenuIcon(CMenuHandle menu);
	BOOL	_CheckID(int nID);

	// Data members
	UINT	WM_FILENOTIFICATION;

	int 						 m_nOriginalMenuItemCountExceptInsertPointMenuItem;
	int 						 m_nOriginalInsertPoint;

	CString 					 m_strRootDirectoryPath;
	CString 					 m_strNoEntries;
	CString 					 m_strAdditional;

	int 						 m_nCurrentMenuID;
	CCrasyMap<int, CString> 	 m_mapID;		// id and file path
	CCrasyMap<HMENU, CString>	 m_mapSubMenu;	// menu handle and file path

	int 						 m_nInsertPointMenuItemID;
	int 						 m_nMinID;
	int 						 m_nMaxID;

	CSimpleArray<CString>		 m_arrIgnoredPath;

	CSimpleArray<CMenuItem *>	 m_arrMenuItem;
	HIMAGELIST					 m_hImgList;
	CImageList					 m_img;
	CFont						 m_fontMenu;
	int 						 m_nImgDir;
	int 						 m_nImgUrl;

	CCrasyMap<CString, int> 	 m_mapIcon; 	// id and file path
	CItemIDList 				 m_idlHtm;		// used to draw .url icon faster

	CComPtr<IShellFolder>		 m_pSHDesktopFolder;
	CComPtr<IContextMenu2>		 m_pContMenu2;

	HMENU						 m_hLastPopupMenu;
	int 						 m_nLastID;

	int 						 m_nScrollCount;

	CBitmap						 m_bmpPageIcon;
	CIcon						 m_iconPageIcon;

	DWORD						 m_dwStyle;

	CMenuHandle 				 m_menu;
	HWND						 m_hWnd;

	int 						 m_nMaxMenuBreakCount;
	int 						 m_nMaxMenuItemTextLength;

	int 						 m_nSelIndex;
	std::function<void (const CString&)>	m_funcExcute;
	CSimpleArray<IgnoreItem>	m_arrIgnore;
	CExplorerMenu*				m_pExpMenu;
	boost::thread				m_tdCreateMenu;
	volatile bool				m_bStopCreate;
	volatile bool				m_bCreating;	// メニューが作成中かどうか
};



// Constructor
CExplorerMenu::Impl::Impl(CExplorerMenu* p)
	: m_dwStyle(0)
	, m_nMaxMenuBreakCount(5000)
	, m_nMaxMenuItemTextLength(55)
	, m_nScrollCount(0)
	#if 1 //+++
	, m_nOriginalMenuItemCountExceptInsertPointMenuItem(0)
	, m_nOriginalInsertPoint(0)
	, m_nLastID(0)
	, m_idlHtm( MtlGetHtmlFileIDList() )
	#endif
	, m_nImgDir(-1)
	, m_nImgUrl(-1)
	, m_nSelIndex(-1)
	, m_hWnd(NULL)
	, m_hLastPopupMenu(NULL)
	, m_pExpMenu(p)
	, m_bStopCreate(false)
	, m_bCreating(false)
{
	SHFILEINFO		sfi = { 0 };
	m_hImgList		 = (HIMAGELIST) ::SHGetFileInfo(_T("C:\\"), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	m_img.Attach(m_hImgList);

	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_fontMenu		= lf.CreateFontIndirect();

	WM_FILENOTIFICATION = GET_REGISTERED_MESSAGE(Mtl_FileNotification);
}

// Destructor
CExplorerMenu::Impl::~Impl()
{
}

void	CExplorerMenu::Impl::Init( 
	int 			nInsertPointMenuItemID,
	const CString&	strNoEntries,
	int 			nMinID,
	int 			nMaxID,
	const CString&	strAdditional,
	int 			nMaxMenuItemTextLength)
{
	m_nInsertPointMenuItemID	= nInsertPointMenuItemID;
	m_strNoEntries				= strNoEntries;
	m_nMinID					= nMinID;
	m_nMaxID					= nMaxID;
	m_strAdditional				= strAdditional;

	m_nCurrentMenuID = nMinID;

	if (m_pSHDesktopFolder == NULL) {
		HRESULT hr = ::SHGetDesktopFolder(&m_pSHDesktopFolder);
		ATLASSERT(m_pSHDesktopFolder);
	}

	::ExtractIconEx(_T("url.dll"), 0, NULL, &m_iconPageIcon.m_hIcon, 1);
	m_bmpPageIcon = CreateBitmapFromHICON(m_iconPageIcon);
}


BOOL	CExplorerMenu::Impl::InstallExplorerMenu(HMENU hMenu)
{
	ATLASSERT( ::IsMenu(hMenu) );

	auto funcFindInsertPoint = [this](CMenuHandle menu) -> int {
		int 	nItems 			= menu.GetMenuItemCount();
		int 	nInsertPoint;

		for (nInsertPoint = 0; nInsertPoint < nItems; ++nInsertPoint) {
			CMenuItemInfo mii;
			mii.fMask = MIIM_ID;
			menu.GetMenuItemInfo(nInsertPoint, TRUE, &mii);

			if (mii.wID == m_nInsertPointMenuItemID)
				break;
		}

		if (nInsertPoint >= nItems)  // not found
			return -1;

		ATLASSERT(nInsertPoint < nItems && "You need a menu item with an ID = m_nInsertPointMenuItemID");
		return nInsertPoint;
	};

	// save original menu infos
	//ATLASSERT(_FindInsertPoint(hMenu) != -1); // need InsertPointMenuItem
	m_nOriginalMenuItemCountExceptInsertPointMenuItem = ::GetMenuItemCount(hMenu) - 1;
	m_nOriginalInsertPoint							  = funcFindInsertPoint(hMenu);

	m_menu = hMenu;

	return TRUE;
}

void	CExplorerMenu::Impl::SetTargetWindow(HWND hWnd)
{
	m_hWnd = hWnd;

	ATLASSERT(m_strRootDirectoryPath.IsEmpty() == FALSE);
	SetUpFileNotificationThread(m_hWnd, m_strRootDirectoryPath, true);
}

// メニューを作成更新する
void	CExplorerMenu::Impl::RefreshMenu()
{
	ATLASSERT(m_strRootDirectoryPath.IsEmpty() == FALSE);

	if (m_bCreating)
		return;				// メニュー作成中なら何もしない
	m_bCreating = true;

	// restore original menu
	while (m_menu.GetMenuItemCount() > m_nOriginalMenuItemCountExceptInsertPointMenuItem) {
		CMenuHandle menuSub = m_menu.GetSubMenu(m_nOriginalInsertPoint);
		if (menuSub.m_hMenu) {
			MTLVERIFY(menuSub.DestroyMenu());
		}
		m_menu.DeleteMenu(m_nOriginalInsertPoint, MF_BYPOSITION);
	}
	m_menu.InsertMenu(m_nOriginalInsertPoint, MF_BYPOSITION, m_nInsertPointMenuItemID, _T("Insert Point"));

	m_mapID.RemoveAll();
	m_mapSubMenu.RemoveAll();

	for (int ii = 0; ii < m_arrMenuItem.GetSize(); ii++) {
		CMenuItem *pMenuData = m_arrMenuItem[ii];
		delete pMenuData;
		m_arrMenuItem[ii] = NULL;	//+++ delete後は強制クリアしておく.
	}
	m_arrMenuItem.RemoveAll();

	m_nCurrentMenuID = m_nMinID;

	// insertPointを削除する
	m_menu.DeleteMenu(m_nInsertPointMenuItemID, MF_BYCOMMAND);

	m_bStopCreate	= false;
	// メニュー作成
	m_tdCreateMenu	= boost::thread(boost::bind(&CExplorerMenu::Impl::_Explore, this, m_menu, m_nOriginalInsertPoint, m_strRootDirectoryPath));
}


void	CExplorerMenu::Impl::OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
{
	//CDebugWindow::OutPutString("Menu Sorting - ascending method");
	std::sort( _begin(infos), _end(infos), [this](const CMenuItemInfo& x, const CMenuItemInfo& y) -> bool {
		CString 	strPathA = this->GetFilePath(x);
		CString 	strPathB = this->GetFilePath(y);

		bool	bDirA = MtlIsDirectoryPath(strPathA);
		bool	bDirB = MtlIsDirectoryPath(strPathB);

		if (bDirA == bDirB) {
			//Old WTL Use lstrcmp on Compare() , but WTL 7 Use _tcscmp.
			int 	nRet = ::lstrcmp(strPathA,strPathB);
			return (nRet <= 0) /*? true : false*/;
			//return strPathA < strPathB;
		} else {
			if (bDirA)
				return true;
			else
				return false;
		}
	});

	//CDebugWindow::OutPutString("Menu Sorting - ascending method End");
}


void	CExplorerMenu::Impl::AddIgnoredPath(const CString &strPath)
{
	IgnoreItem	item;
	int nCount = m_menu.GetMenuItemCount();
	for (int i = 0; i < nCount; ++i) {
		CMenuItemInfo	info;
		info.fMask	= MIIM_DATA | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_STATE;
		m_menu.GetMenuItemInfo(i, TRUE, &info);
		CMenuItem*	pItem = (CMenuItem*)info.dwItemData;
		if (pItem && pItem->m_strPath == strPath) {
			item.info = info;
			if (info.hSubMenu) 
				item.hSubMenu = item.info.hSubMenu;

			item.nInsertPoint = i;
			item.pItem = pItem;
			m_arrIgnore.Add(item);
			m_menu.RemoveMenu(i, MF_BYPOSITION);
			return ;
		}
	}
}

void	CExplorerMenu::Impl::ResetIgnoredPath()
{
	int nCount = m_arrIgnore.GetSize();
	for (int i = nCount - 1; i>= 0 ; --i) {
		IgnoreItem& item = m_arrIgnore[i];
		item.info.fMask |= MIIM_STRING;
		item.info.dwTypeData = item.pItem->m_strText.GetBuffer(0);
		m_menu.InsertMenuItem(item.nInsertPoint, TRUE, &item.info);
		if (item.info.hSubMenu) 
			ATLASSERT(::IsMenu(item.info.hSubMenu));
	}
	m_arrIgnore.RemoveAll();
}


bool	CExplorerMenu::Impl::IsSubMenu(HMENU hSubMenu)
{
	return m_mapSubMenu.FindKey(hSubMenu);
}


void CExplorerMenu::Impl::OnExecute(const CString& strFilePath)
{
	if (m_funcExcute) {
		m_funcExcute(strFilePath);
	} else {
		::ShellExecute(NULL, _T("open"), strFilePath, NULL, NULL, SW_SHOWNORMAL);
	}
}


void	CExplorerMenu::Impl::OnCommandRange(UINT, int nID, HWND hWndCtrl)
{
	if (hWndCtrl) {
		SetMsgHandled(FALSE);
		return;
	}

	if (m_mapID.Lookup(nID).IsEmpty() == FALSE) {
		OnExecute(m_mapID.Lookup(nID));
	}
	//m_mapID.RemoveAll();// delayed clean up.
}


LRESULT	CExplorerMenu::Impl::OnFileNotification(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	if (m_strRootDirectoryPath == (LPCTSTR)wParam) {	// 同じウィンドウハンドルで実行してる時用
		RefreshMenu();
	} else {
		SetMsgHandled(FALSE);
	}
	
	return 0;
}



void	CExplorerMenu::Impl::OnDestroy()
{
	SetMsgHandled(FALSE);
	m_bStopCreate = true;
	m_tdCreateMenu.timed_join(boost::posix_time::seconds(5));
}

void	CExplorerMenu::Impl::OnMenuSelect(UINT nItemID, UINT nFlags, CMenuHandle menu)
{
	SetMsgHandled(FALSE);
#if 0	//\\ とりあえず挿入したメニュー項目の削除はしない
	// Check if a menu is closing, do a cleanup
	if (nFlags == 0xFFFF && menu.m_hMenu == NULL) {				// menu closing
		// NOTE: this message sent only if top level menu!!!
		epmTRACE( _T(" Cleanup\n") );
		_CleanUpExceptIDMap();
	}
	//else if (m_arrMenuHandle.Find(hmenu) != -1){// it's my menu
	//}
#endif
	return;
}



LRESULT	CExplorerMenu::Impl::OnMsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITMENUPOPUP)
		ATLTRACE("##### WM_INITMENUPOPUP %x %x\n", wParam, lParam);
	else if (uMsg == WM_MEASUREITEM)
		ATLTRACE("##### WM_MEASUREITEM   %x %x\n", wParam, lParam);
	else
		ATLTRACE("##### WM_DRAWITEM      %x %x\n", wParam, lParam);

	return 0;
}


LRESULT	CExplorerMenu::Impl::OnMeasureItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItemStruct = (LPMEASUREITEMSTRUCT) lParam;

	ATLASSERT( (int) wParam == 0 );

	//まずメニュー項目のIDをチェック
	if (_CheckID(lpMeasureItemStruct->itemID) == FALSE) {
		bHandled = FALSE;
		return TRUE;
	}

	if (m_dwStyle & EMS_DRAW_FAVICON) {
		bHandled = FALSE;
		lpMeasureItemStruct->itemWidth = 200;
		return TRUE;
	}

	if (   lpMeasureItemStruct->CtlType == ODT_MENU 
		&& lpMeasureItemStruct->itemData != NULL
		&& ( (CMenuItem *) lpMeasureItemStruct->itemData )->m_bIcon)
	{
		CMenuItem *pMenuData = (CMenuItem *) lpMeasureItemStruct->itemData;
		int 	nImgCx = 0;
		int		nImgCy = 0;
		m_img.GetIconSize(nImgCx, nImgCy);

		// compute size of text - use DrawText with DT_CALCRECT
		CWindowDC  dc(NULL);
		HFONT	   hOldFont;

		// need bold version of font
		LOGFONT    lf;
		m_fontMenu.GetLogFont(lf);
		lf.lfWeight 				  += 200;
		CFont	   fontBold;
		fontBold.CreateFontIndirect(&lf);
		ATLASSERT(fontBold.m_hFont != NULL);
		hOldFont					   = dc.SelectFont(fontBold);

		RECT	   rcText	 = { 0, 0, 0, 0 };
		dc.DrawText(pMenuData->m_strText, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
		int 	   cx		 = rcText.right - rcText.left;
		dc.SelectFont(hOldFont);

		// height of item is the bigger of these two
		if (CFavoritesMenuOption::s_bSpaceMin) {
			lpMeasureItemStruct->itemHeight = nImgCy;
		} else {
			lpMeasureItemStruct->itemHeight = (int) (nImgCy * 1.3);
		}

		// width is width of text plus a bunch of stuff
		cx		+= 2 * s_kcxTextMargin; 	// L/R margin for readability
		cx		+= s_kcxGap;				// space between button and menu text
		cx		+= 2 * nImgCx;				// button width (L=button; R=empty margin)

		// Windows adds 1 to returned value
		cx		-= ::GetSystemMetrics(SM_CXMENUCHECK) - 2;
		lpMeasureItemStruct->itemWidth = (int) (cx * 0.80); 		// done deal

	} else {
		bHandled = FALSE;
	}

	return (LRESULT) TRUE;
}


LRESULT	CExplorerMenu::Impl::OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT) lParam;

	if (   lpDrawItemStruct->CtlType != ODT_MENU
		|| lpDrawItemStruct->itemData == 0
		|| _CheckID(lpDrawItemStruct->itemID) == FALSE ) {
		bHandled = FALSE;
		return TRUE;
	}

	CDCHandle	dc 			= lpDrawItemStruct->hDC;
	const RECT&	rcItem 		= lpDrawItemStruct->rcItem;

	CMenuHandle	menu		= (HMENU) lpDrawItemStruct->hwndItem;

	CString 	strCmd;
	CMenuItem*	pMenuData	= (CMenuItem *) lpDrawItemStruct->itemData;

	int			nIndex 		= 0;

	if (pMenuData && pMenuData->m_bIcon) {
		if (pMenuData->m_bDir) {
			if (m_nImgDir == -1) {
				CItemIDList idlURL(pMenuData->m_strPath);
				//					m_nImgDir = MtlGetSystemIconIndex(idlURL);
				m_nImgDir = MtlGetNormalIconIndex(idlURL);
			}

			nIndex = m_nImgDir;
		} else {
			CString strExt;
			strExt = pMenuData->m_strPath.Mid(pMenuData->m_strPath.ReverseFind('.') + 1);
			strExt.MakeUpper();

			if ( strExt == _T("EXE") || strExt == _T("LNK") ) {
				
				CItemIDList idlURL(pMenuData->m_strPath);
				//					nIndex = MtlGetSystemIconIndex(idlURL);
				nIndex = MtlGetNormalIconIndex(idlURL);
			} else {
				int nIcon = m_mapIcon.Lookup(strExt);
				if (nIcon == 0) {
					CItemIDList idlURL(pMenuData->m_strPath);
					//						nIcon = MtlGetSystemIconIndex(idlURL);
					nIcon = MtlGetNormalIconIndex(idlURL, m_idlHtm);
					m_mapIcon.Add(strExt, nIcon);
				}

				nIndex = nIcon;
			}
		}
	}

	BOOL		bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
	if (bSelected || lpDrawItemStruct->itemAction == ODA_SELECT) {
		RECT	rcBG	= rcItem;
		BOOL	bRet	= dc.FillRect( &rcBG, (HBRUSH) LongToPtr( bSelected ? (COLOR_HIGHLIGHT + 1) : (COLOR_MENU + 1) ) );
	}

	pMenuData->m_bSelected = bSelected;
	pMenuData->m_hItemWnd  = lpDrawItemStruct->hwndItem;

	if (m_dwStyle & EMS_DRAW_FAVICON) {
		if (pMenuData->m_Icon.IsNull() == false) {
			pMenuData->m_Icon.DrawIconEx(dc, rcItem.left + 1, rcItem.top + 1, 16, 16, 0, (HBRUSH) LongToPtr( bSelected ? (COLOR_HIGHLIGHT + 1) : (COLOR_MENU + 1) ) );
		} else {
			m_iconPageIcon.DrawIconEx(dc, rcItem.left + 1, rcItem.top + 1, 16, 16, 0, (HBRUSH) LongToPtr( bSelected ? (COLOR_HIGHLIGHT + 1) : (COLOR_MENU + 1) ) );
		}
	} else {
		// draw pushed-in or popped-out edge
		::ImageList_Draw(m_hImgList, nIndex, dc, rcItem.left + 1, rcItem.top + 1, ILD_TRANSPARENT);
	}

	RECT	rcText	= rcItem;
	rcText.left 	+= 18;

	COLORREF		 colorText = ::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT);

	dc.SetBkMode(TRANSPARENT);

	if (pMenuData) strCmd = pMenuData->m_strText;

	_DrawMenuText(dc, rcText, strCmd, colorText);			// finally!

	return (LRESULT) TRUE;
}


LRESULT	CExplorerMenu::Impl::OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	DefWindowProc(hWnd, uMsg, wParam, lParam);
	return 0;
}

// メニューアイテムのメニューを表示する
LRESULT	CExplorerMenu::Impl::OnRButtonUp(HWND hWnd, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	CMenuItem * pMenuItemSel = NULL;
	WORD		wID 		 = -1;
	CMenuHandle menu		 = (HMENU) lParam;
	HMENU		hMyMenu 	 = m_menu.m_hMenu;

	int 		nID 		 = (int)menu.GetMenuItemID( (int)wParam );

	if (nID < m_nMinID || m_nMaxID < nID) {
		bHandled = FALSE;
		return 0;	// IDが範囲外だったので帰る
	}

	int	nItemCount = menu.GetMenuItemCount();
	for (int ii = 0; ii < nItemCount; ii++) {
		CMenuItemInfo mii;
		mii.fMask	 = MIIM_DATA | MIIM_ID | MIIM_STATE;
		menu.GetMenuItemInfo(ii, TRUE, &mii);

		CMenuItem *   pMenuItem = (CMenuItem *) mii.dwItemData;
		if (pMenuItem == NULL) 
			continue;

		//if (pMenuItem->m_bSelected==FALSE) continue;
		if (pMenuItem->m_hMenu != hMyMenu) {
			bHandled = FALSE;
			return 0;
		}

		if ((mii.fState & MFS_HILITE) == 0) 
			continue; 	//minit

		pMenuItemSel = pMenuItem;
		wID 		 = mii.wID;
		break;
	}

	if (pMenuItemSel == NULL) 
		return 0;

	//さらに右クリックメニューを表示させる
	if (::IsWindow(hWnd) == FALSE) 	//!::IsWindow(m_hWnd)) //メニュー表示にハンドルが必要
		return 1;
	
	CString strFileName;
	if (pMenuItemSel->m_bDir) {
		strFileName = pMenuItemSel->m_strPath.Left(pMenuItemSel->m_strPath.GetLength() - 1);
	} else {
		strFileName = pMenuItemSel->m_strPath;
	}
	//if(CMainOption::s_dwErrorBlock < 5)
	//ShowContextMenuOnPopup(strFileName, hWnd, this);
	////////////////////////////////////////////////////////////////////////////
	HRESULT 			  hr		= S_OK;

	CString 			  strFolder;
	CComPtr<IShellFolder> pSHFolder;
	CComPtr<IContextMenu> pContMenu;
	CItemIDList 		  idlFile;
	CItemIDList 		  idlFolder;
		
	int 				  iCmd		= 0;
	int 				  flags		= 0;
	HWND				  hWndPopup	= NULL;
	CMINVOKECOMMANDINFO   InvokeInfo= { sizeof (CMINVOKECOMMANDINFO) };

	strFolder = strFileName.Left( strFileName.ReverseFind('\\') );
	idlFolder = strFolder;
	idlFile   = strFileName;
	idlFile  -= idlFolder;
	if (idlFile.IsNull()) 
		return 1;

	CString strCheckFile   = idlFile.GetPath();
	CString strCheckFolder = idlFolder.GetPath();

	{
		/* デスクトップからIShellFolderを取得 */
		hr	= m_pSHDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &pSHFolder);
		if (FAILED(hr) || pSHFolder == NULL) 
			return 1;

		/* アイテムＩＤリストからIContextMenuを取得 */
		LPCITEMIDLIST	pIidList = (LPITEMIDLIST) idlFile;
		hr	= pSHFolder->GetUIObjectOf(m_hWnd, 1, &pIidList, IID_IContextMenu, NULL, (void **) &pContMenu);
		if (FAILED(hr) || pContMenu == NULL) 
			return 1;

		BOOL	bMenuEx	= TRUE;
		m_pContMenu2.Release();
		/* IContextMenuからIContextMenu2を取得 */
		hr	= pContMenu->QueryInterface(IID_IContextMenu2, (void **) &m_pContMenu2);
		if (FAILED(hr) || m_pContMenu2 == NULL) {
			bMenuEx = FALSE;
			/* IContextMenu2が取得できなかったのでIContextMenuで我慢する */
			hr	= pSHFolder->GetUIObjectOf(m_hWnd, 1, &pIidList, IID_IContextMenu, 0, (void**)&m_pContMenu2);
			if (FAILED(hr)) 
				return 1;
		}
	}

	CPoint	pos;
	::GetCursorPos(&pos);

	CMenu	menuC;
	menuC.CreatePopupMenu();
	ATLASSERT(menuC.m_hMenu);
	int		nPos = 0;
	menuC.InsertMenu( 0, MF_BYPOSITION | MF_ENABLED, s_nIDExProp, _T("拡張プロパティ(&E)") );
	menuC.InsertMenu( 1, MF_BYPOSITION | MF_SEPARATOR, 0);
	nPos	+= 2;

	// 戻り値はメニュー項目の数？
	hr	= m_pContMenu2->QueryContextMenu(menuC, nPos, s_nIDLast, 0x7FFF, CMF_NORMAL);
	if (hr == 0) 
		goto CleanUp;

	//m_bMessageStop = TRUE;
	flags	  = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | 0x00000001; //TPM_RECURSE
	iCmd	  = menuC.TrackPopupMenu(flags | TPM_RETURNCMD, pos.x, pos.y, hWnd, NULL);
	//m_bMessageStop = FALSE;

	// お気に入りメニュー表示を消す
	while (1) {
		hWndPopup = ::FindWindow(_T("#32768"), NULL);
		if ( ::IsWindow(hWndPopup) && ::IsWindowVisible(hWndPopup) ) {
			::SendMessage(hWndPopup, WM_KEYDOWN, VK_ESCAPE, 0);
		} else {
			break;
		}
	}

	if (iCmd == 0) {
		goto CleanUp;
	} else if (iCmd == s_nIDExProp) {	// 拡張プロパティを表示する
		::SendMessage(CWindow(hWnd).GetTopLevelParent(), WM_SET_EXPROPERTY, (WPARAM) (LPCTSTR) strFileName, 0);
		goto CleanUp;
	}

	InvokeInfo.fMask	= 0;
	InvokeInfo.hwnd 	= m_hWnd;
	InvokeInfo.lpVerb	= (LPCSTR)MAKEINTRESOURCE(iCmd - s_nIDLast);
	InvokeInfo.nShow	= SW_SHOWNORMAL;
	hr					= m_pContMenu2->InvokeCommand(&InvokeInfo);

CleanUp:
	m_pContMenu2.Release();

	return 1;
}




// menuPopup の nInsertPoint に strDirectoryPathの中身を挿入する
void	CExplorerMenu::Impl::_Explore(CMenuHandle menuPopup, int nInsertPoint, const CString &strDirectoryPath)
{
	if (m_bStopCreate)
		return;

	int	nBreakCount = nInsertPoint;

	//「これらを全て開く」を追加
	if ( m_dwStyle & EMS_ADDITIONAL_OPENALL && ::PathFileExists(strDirectoryPath) )
		_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_OPENALL);

	//「ここにお気に入り追加」を追加
	if ( m_dwStyle & EMS_ADDITIONAL_SAVEFAV_TO )
		_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_SAVEFAV);

	// 「並び順を初期化する」を追加
	if ( m_dwStyle & EMS_ADDITIONAL_RESETORDER && ::PathFileExists(strDirectoryPath) )
		_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_RESETORDER);

	if (m_dwStyle & (EMS_ADDITIONAL_OPENALL | EMS_ADDITIONAL_SAVEFAV_TO | EMS_ADDITIONAL_RESETORDER))
		_InsertSeparator(menuPopup, nInsertPoint);

	auto funcAddMenuItem = [this](BOOL bDir, LPCTSTR strText, const CString strPath) -> CMenuItem* {
		CMenuItem*   pMenuData	= new CMenuItem;
		pMenuData->m_strText	= strText;
		pMenuData->m_strPath	= strPath;
		pMenuData->m_bDir		= bDir;

		if (GetStyle() & EMS_DRAW_ICON) {
			pMenuData->m_bIcon = TRUE;
		}

		pMenuData->m_hMenu = m_menu.m_hMenu;
		m_arrMenuItem.Add(pMenuData);	// 追加

		return pMenuData;
	};

	auto funcCreateMenuItemInfo = [this, funcAddMenuItem](CMenuItemInfo& mii, UINT mask, LPTSTR strText, const CString& strPath, BOOL bDir) {
		mii.fMask		= MIIM_TYPE | MIIM_DATA | mask;
		mii.fType		= MFT_STRING;
		if (GetStyle() & EMS_DRAW_ICON) 
			mii.fType |= MFT_OWNERDRAW;

		mii.dwTypeData = strText;
		mii.dwItemData = (DWORD_PTR) funcAddMenuItem(bDir, strText, strPath);
	};

	CSimpleArray<CMenuItemInfo> infos;
	// フォルダ内のリンクを列挙してinfosに追加
	MtlForEachObject_OldShell( strDirectoryPath, [this, funcCreateMenuItemInfo, &infos](const CString &strPath, bool bDir) {
		//if (m_arrIgnoredPath.Find(strPath) != -1) 
		//	return;

		LPTSTR	lpstrText = new TCHAR[strPath.GetLength() + 1];
		::lstrcpy(lpstrText, strPath);

		if (bDir) {
			CMenuHandle   menuSub;
			menuSub.CreatePopupMenu();

			// add to map and array of menus
			m_mapSubMenu.Add(menuSub.m_hMenu, strPath);

			CMenuItemInfo mii;
			mii.hSubMenu   = menuSub.m_hMenu;
			funcCreateMenuItemInfo(mii, MIIM_SUBMENU, lpstrText, strPath, bDir);
			_Explore(menuSub, 0, strPath);	// 再帰
			infos.Add(mii);
		} else {
			CMenuItemInfo mii;
			m_nCurrentMenuID++;
			mii.wID = m_nCurrentMenuID;
			ATLASSERT(mii.wID != 0);
			funcCreateMenuItemInfo(mii, MIIM_ID, lpstrText, strPath, bDir);
			if (m_nCurrentMenuID < m_nMaxID) {
				// add to command map
				m_mapID.Add(m_nCurrentMenuID, strPath);
				infos.Add(mii);
			}

		}
	});
#if 0	//\\ アイコンテスト
	if (m_dwStyle & EMS_DRAW_FAVICON) {
		MENUINFO	menuInfo = { sizeof(MENUINFO) };
		menuInfo.fMask = MIM_STYLE; 
		GetMenuInfo(menuPopup, &menuInfo);
		menuInfo.dwStyle = (menuInfo.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
		SetMenuInfo(menuPopup, &menuInfo);
	}
#endif

	// setup menu item info
	m_pExpMenu->OnMenuItemInitialUpdate(strDirectoryPath, infos);

	auto funcGetMenuItemText = [this](const CString &strPath) -> CString {
		CString str = MtlGetDisplayTextFromPath(strPath);
		str = MtlCompactString(str, m_nMaxMenuItemTextLength);
		str.Replace( _T("&"), _T("&&") );
		return str;
	};
	// shorten text size
	for (int i = 0; i < infos.GetSize(); ++i) {
		CMenuItemInfo &info 	 = infos[i];
		LPTSTR		   lpsz 	 = info.dwTypeData;
		CString 	   str		 = funcGetMenuItemText(lpsz);
		delete[] lpsz;
		lpsz			= new TCHAR[str.GetLength() + 1];
		::lstrcpy(lpsz, str);
		info.dwTypeData = lpsz;

		CMenuItem* pMenuItem = (CMenuItem*) info.dwItemData;
		if (pMenuItem/* && pMenuItem->m_bIcon*/)
			pMenuItem->m_strText = lpsz;
	}

	// メニューアイテムを挿入する
	std::for_each( _begin(infos), _end(infos), [this, &nInsertPoint, &nBreakCount, &menuPopup](CMenuItemInfo &mii) {
		if (m_bStopCreate)
			return;

		if (nBreakCount + 1 > m_nMaxMenuBreakCount) {	// what a difficult algorithm...
			nBreakCount = 0;							// reset
			mii.fType	|= MFT_MENUBREAK;
		}
		++nBreakCount;
		menuPopup.InsertMenuItem(nInsertPoint, TRUE, &mii);
		nInsertPoint++;
	});

	if (m_bStopCreate)
			return;
	if (m_dwStyle & EMS_DRAW_FAVICON) {
		//boost::thread tdIcon(boost::bind(&CExplorerMenu::Impl::_thread_SetMenuIcon, this, menuPopup.m_hMenu));
		_thread_SetMenuIcon(menuPopup);
	}
#if 0
	CMenuItemInfo mii;
	mii.fMask = MIIM_BITMAP;
	HICON	hIcon = CreateIconFromIDList(CItemIDList(strDirectoryPath));
	mii.hbmpItem = CreateBitmapFromHICON(hIcon);
	::DestroyIcon(hIcon);
	menuPopup.SetMenuItemInfo(7, TRUE, &mii);
#endif
	// add "none" menu item
	if (infos.GetSize() == 0) {
		CMenuItemInfo mii;
		mii.fMask	   = MIIM_STATE | MIIM_TYPE;
		mii.fType	   = MFT_STRING;
		mii.fState	   = MFS_GRAYED;
		mii.dwTypeData = (LPTSTR) (LPCTSTR) m_strNoEntries;
		menuPopup.InsertMenuItem(nInsertPoint, TRUE, &mii);
		nInsertPoint++;
	}

	// clean up texts
	for (int i = 0; i < infos.GetSize(); ++i) {
		delete[] infos[i].dwTypeData;
		infos[i].dwTypeData = NULL; 	//*+++ 不要だけれど、念のためクリア.
	}

	if (strDirectoryPath == m_strRootDirectoryPath)
		m_bCreating = false;	// メニューの作成終了
}



void	CExplorerMenu::Impl::_AddAdditionalMenuItem( CMenuHandle menuDest,
													int &nInsertPoint,
													int &nBreakCount,
													const CString &strDirectory,
													int nType )
{
	CString 	  strDirectoryPath = strDirectory;

	MtlMakeSureTrailingBackSlash(strDirectoryPath);

	CString 	  strCaption	   = m_strAdditional;
	CString 	  strUserData	   = strDirectoryPath;

	switch (nType) {
	case ADDITIONAL_TYPE_OPENALL:
		strUserData.Insert( 0, _T("a ") );
		break;
	case ADDITIONAL_TYPE_SAVEFAV:
		strUserData.Insert( 0, _T("s ") );
		strCaption = _T("この位置にお気に入りを追加");
		break;
	case ADDITIONAL_TYPE_RESETORDER:
		strUserData.Insert( 0, _T("r ") );
		strCaption = _T("並び順を初期化する");
		break;
	}

	++m_nCurrentMenuID;
	CMenuItemInfo mii;
	mii.fMask	   = MIIM_ID | MIIM_TYPE;
	mii.fType	   = MFT_STRING;
	mii.wID 	   = m_nCurrentMenuID;
	mii.dwTypeData = (LPTSTR) (LPCTSTR) strCaption;
	menuDest.InsertMenuItem(nInsertPoint, TRUE, &mii);
	nInsertPoint++;
	++nBreakCount;

	if (m_nCurrentMenuID < m_nMaxID) {
		m_mapID.Add(m_nCurrentMenuID, strUserData);
	} else {
		ATLASSERT(FALSE);
	}

//	if ( !(m_dwStyle & EMS_ADDITIONALMENUITEMNOSEP) )
//		_InsertSeparator(menuDest, nInsertPoint);
}


void	CExplorerMenu::Impl::_InsertSeparator(CMenuHandle menuDest, int &nInsertPoint)
{
	CMenuItemInfo	mii;
	mii.fMask = MIIM_TYPE;
	mii.fType = MF_SEPARATOR;
	menuDest.InsertMenuItem(nInsertPoint++, TRUE, &mii);
}


CString CExplorerMenu::Impl::GetFilePath(const CMenuItemInfo& mii)
{
	CString str = m_mapID.Lookup(mii.wID);
	if (str.IsEmpty()) {		// dir path
		str = m_mapSubMenu.Lookup(mii.hSubMenu);
	}

	ATLASSERT(str.IsEmpty() == FALSE);
	return str;
}

CString CExplorerMenu::Impl::GetFilePath(int nID)
{
	return m_mapID.Lookup(nID);
}

// strPathのメニューを返す
CMenuHandle CExplorerMenu::Impl::GetFolderMenu(const CString& strPath)
{
	auto it = std::find_if(m_mapSubMenu.begin(), m_mapSubMenu.end(), [&strPath](std::pair<HMENU, CString> it) {
		return it.second == strPath;
	});
	if (it != m_mapSubMenu.end()) 
		return it->first;
	return NULL;
}


void	CExplorerMenu::Impl::_DrawMenuText(CDCHandle& dc, RECT& rc, LPCTSTR lpstrText, COLORREF color)
{
	int nTab = -1;
	int len  = lstrlen(lpstrText);
	for (int i = 0; i < len; i++)
	{
		if(lpstrText[i] == '\t')
		{
			nTab = i;
			break;
		}
	}
	dc.SetTextColor(color);
	dc.DrawText(lpstrText, nTab, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	if(nTab != -1)
		dc.DrawText(&lpstrText[nTab + 1], -1, &rc, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
}



void	CExplorerMenu::Impl::_thread_SetMenuIcon(CMenuHandle menu)
{
	::CoInitialize(NULL);

	int nMenuItemCount = menu.GetMenuItemCount();
	for (int i = 0; i < nMenuItemCount; ++i) {
		if (m_bStopCreate)
			break;
		CMenuItemInfo	mii;
		mii.fMask	= MIIM_DATA;
		menu.GetMenuItemInfo(i, TRUE, &mii);
		
		CMenuItem* pItem = (CMenuItem*)mii.dwItemData;
		if (pItem == NULL) continue;

		CMenuItemInfo miib;
		miib.fMask	= MIIM_CHECKMARKS | MIIM_STATE;
		miib.fState	= MFS_CHECKED;
		HICON hIcon = CreateIconFromIDList(CItemIDList(pItem->m_strPath));
		if (hIcon) {
			miib.hbmpChecked	= CreateBitmapFromHICON(hIcon);
			pItem->m_Icon	= hIcon;
		} else {
			miib.hbmpChecked	= m_bmpPageIcon;
		}
		menu.SetMenuItemInfo(i, TRUE, &miib);
	}
	//::DrawMenuBar(m_hWnd);

	::CoUninitialize();
}

BOOL	CExplorerMenu::Impl::_CheckID(int nID)
{
	if (nID == 0xFFFFFFFF || nID == 0) {
		return TRUE;
	} else if (m_nMinID <= nID && nID <= m_nMaxID) {
		return TRUE;
	//}else if(COMMAND_RANGE_START <= nID && nID <= COMMAND_RANGE_END) {
	//	return TRUE;
	}

	return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////
// CExplorerMenu

// Constructor
CExplorerMenu::CExplorerMenu(
	int 			nInsertPointMenuItemID,
	const CString&	strNoEntries,
	int 			nMinID,		
	int 			nMaxID,		
	const CString&	strAdditional,
	int 			nMaxMenuItemTextLength	)
	: pImpl(new Impl(this))
{
	pImpl->Init(nInsertPointMenuItemID, strNoEntries, nMinID, nMaxID, strAdditional, nMaxMenuItemTextLength);
}

CExplorerMenu::~CExplorerMenu()
{
	delete pImpl;
}

BOOL	CExplorerMenu::InstallExplorerMenu(HMENU hMenu)
{
	return pImpl->InstallExplorerMenu(hMenu);
}

void	CExplorerMenu::SetTargetWindow(HWND hWnd)
{
	pImpl->SetTargetWindow(hWnd);
}

void	CExplorerMenu::SetRootDirectoryPath(const CString& strPath)
{
	pImpl->m_strRootDirectoryPath	= strPath;
}

void	CExplorerMenu::RefreshMenu()
{
	pImpl->RefreshMenu();
}

void	CExplorerMenu::SetExcuteFunction(std::function<void (const CString&)> func)
{
	pImpl->SetExcuteFunction(func);
}

CString CExplorerMenu::GetFilePath(const CMenuItemInfo& mii)
{
	return pImpl->GetFilePath(mii);
}

CString CExplorerMenu::GetFilePath(int nID)
{
	return pImpl->GetFilePath(nID);
}

DWORD	CExplorerMenu::GetStyle() const
{
	return pImpl->GetStyle();
}

void	CExplorerMenu::SetStyle(DWORD dwStyle)
{
	pImpl->SetStyle(dwStyle);
}

CMenuHandle CExplorerMenu::GetMenu()
{
	return pImpl->m_menu;
}

void	CExplorerMenu::OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
{
	pImpl->OnMenuItemInitialUpdate(strDirPath, infos);
}

void	CExplorerMenu::SetMaxMenuItemTextLength(int nCount)
{
	pImpl->m_nMaxMenuItemTextLength = nCount;
}

void	CExplorerMenu::SetMaxMenuBreakCount(int nCount)
{
	pImpl->m_nMaxMenuBreakCount = nCount;
}

void	CExplorerMenu::AddIgnoredPath(const CString &strPath)
{
	pImpl->AddIgnoredPath(strPath);
}

void	CExplorerMenu::ResetIgnoredPath()
{
	pImpl->ResetIgnoredPath();
}

CMenuHandle CExplorerMenu::GetFolderMenu(const CString& strPath)
{
	return pImpl->GetFolderMenu(strPath);
}

bool	CExplorerMenu::IsSubMenu(HMENU hSubMenu)
{
	return pImpl->IsSubMenu(hSubMenu);
}

BEGIN_MSG_MAP_EX_impl( CExplorerMenu )
	CHAIN_MSG_MAP_MEMBER( (*pImpl) )
END_MSG_MAP()



////////////////////////////////////////////////////////////////
// CStyleSheetMenu : スタイルシート用メニュー


CStyleSheetMenu::CStyleSheetMenu(const HWND& hWnd) : 
	CExplorerMenu(ID_INSERTPOINT_CSSMENU, _T("(empty)"), ID_INSERTPOINT_CSSMENU, ID_INSERTPOINT_CSSMENU_END),
	m_rhWnd(hWnd)
{
	auto funcExcute = [this](const CString& strFilePath) {
		::SendMessage(m_rhWnd, WM_USER_CHANGE_CSS, (WPARAM)(LPCTSTR)strFilePath, 0);
	};
	SetExcuteFunction(funcExcute);
}



BOOL SaveInternetShortcutFile(const CString &strFileTitle, const CString &strPath, const CString &strURL)
{
	CString strFileName 	= strFileTitle;

	if ( CDonutRenameFileDialog::IsContainBadCharacterForName(strFileName) ) {
		MessageBox(NULL, _T("タイトルに名前として使用できない文字が入っています。別の名前をつけてください。"), _T("注意"), MB_OK);
		CDonutRenameFileDialog::ReplaceBadCharacterForName(strFileName);

		CDonutRenameFileDialog	dlg;
		dlg.m_strName = strFileName;

		if ( dlg.DoModal() == IDOK && !dlg.m_strName.IsEmpty() ) {
			strFileName = dlg.m_strName;
		} else {
			return FALSE;
		}
	}

	CString strShortcutPath = strPath + strFileName + _T(".url");

	if (::PathFileExists(strShortcutPath)) {
		CString strMsg;
		strMsg.Format(_T("%s\nはすでに存在しています。上書きしますか？"), strShortcutPath);
		int 	nRet = MessageBox(NULL, strMsg, _T("確認"), MB_YESNO);

		if (nRet == IDYES) {
			::DeleteFile(strShortcutPath);
		} else {
			return FALSE;
		}
	}

	MtlCreateInternetShortcutFile(strShortcutPath, strURL);
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// CFavoritesMenu : IEの順番にお気に入りを並べ替える機能付き

// Overrides
void CFavoritesMenu::OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
{
	fvmTRACE( _T("CFavoritesMenu::OnMenuItemInitialUpdate\n") );

	DWORD	dwStyle = GetStyle();
	if (   _check_flag( dwStyle, EMS_IE_ORDER  ) 
		&& _check_flag( dwStyle, EMS_USER_DEFINED_FOLDER ) == false ) {
		fvmTRACE( _T(" try to order favorites\n") );	// IEの順番にお気に入りを並べ替える
		CFavoritesOrder order;

		//if (false) {
		if ( MtlGetFavoritesOrder(order, strDirPath) ) {
			//CDebugWindow::OutPutString("Menu Sorting - ie method");
			std::sort( _begin(infos), _end(infos), [this, &order](const CMenuItemInfo &x, const CMenuItemInfo &y) -> bool {
				CString   strPathA = GetFilePath(x);
				CString   strPathB = GetFilePath(y);

				CString   strA	   = MtlGetFileName(strPathA);
				CString   strB	   = MtlGetFileName(strPathB);
				//			ATLTRACE(_T("strA(%s), strB(%s)\n"), strA, strB);

				const int enough   = 10000;
				bool	  bDirA    = MtlIsDirectoryPath(strPathA);
				bool	  bDirB    = MtlIsDirectoryPath(strPathB);

				int 	  itA	   = order.Lookup(strA);
				int 	  itB	   = order.Lookup(strB);

				if (itA == -1 || itA == FAVORITESORDER_NOTFOUND)	// fixed by fub, thanks.
					itA = enough;

				if (itB == -1 || itB == FAVORITESORDER_NOTFOUND)
					itB = enough;

				if (itA == enough && itB == enough) {
					if (bDirA == bDirB) {
						int nRet = ::lstrcmp(strA, strB);
						return (nRet < 0) /*? true : false*/;
						//return strA < strB;
					} else {
					  #if 1 //+++
						return bDirA;
					  #else
						if (bDirA)
							return true;
						else
							return false;
					  #endif
					}
				} else {
					return itA < itB;
				}
			});
			//CDebugWindow::OutPutString("Menu Sorting - ie method End");
		} else {
			fvmTRACE( _T(" default order\n") );
			__super::OnMenuItemInitialUpdate(strDirPath, infos);
		}
	} else {
		fvmTRACE( _T(" default order\n") );
		__super::OnMenuItemInitialUpdate(strDirPath, infos);
	}
}