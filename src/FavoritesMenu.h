/**
 *	@file	FavoritesMenu.h
 *	@brief	お気に入りメニュー.
 */
#pragma once

// for debug
#ifdef _DEBUG
	const bool _Mtl_FavoritesMenu_traceOn = false;
	#define fvmTRACE	if (_Mtl_FavoritesMenu_traceOn) ATLTRACE
#else
	#define fvmTRACE	__noop
#endif


#ifndef WM_MENURBUTTONUP
#define WM_MENURBUTTONUP			0x0122
#endif

#ifndef TPM_RECURSE
#define TPM_RECURSE 				0x0001L
#endif


enum EAdditional_Type {
	ADDITIONAL_TYPE_OPENALL 	= 0x00000001L,	// これらをすべて開く
	ADDITIONAL_TYPE_SAVEFAV 	= 0x00000002L,	// この位置にお気に入り追加
	ADDITIONAL_TYPE_RESETORDER	= 0x00000004L,	// 並び順を初期化する
};


//////////////////////////////////////////////////////////////
// CCrasyMap	: be care, if no element, &array[0] is invalid.

template <class _Key, class _Val>
class CCrasyMap : public CSimpleArray< std::pair<_Key, _Val> > 
{
public:
	bool FindKey(const _Key &__key)
	{
		if (GetSize() == 0)
			return false;

		return end() != std::find_if( begin(), end(), _Finder(__key) );
	}


	void Add(const _Key &__key, const _Val &__val)
	{
		if (GetSize() == 0) {
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
			return;
		}

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p == end() )
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
		else
			(*p).second = __val;
	}


	_Val Lookup(const _Key &__key)
	{
		if (GetSize() == 0)
			return _Val();

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p != end() )
			return p->second;
		else
			return _Val();
	}


	void Sort()
	{
		if (GetSize() == 0)
			return;

		std::sort( begin(), end(), _Compare() );
	}


	std::pair<_Key, _Val>* begin()
	{
		return &(*this)[0];
	}


	std::pair<_Key, _Val>* end()
	{
		return &(*this)[0] + GetSize();
	}


	//	template <class _Key, class _Val>
	struct _Finder {
		_Key __aKey;
		_Finder(_Key __key) : __aKey(__key) { }
		bool operator ()(const std::pair<_Key, _Val> &src)
		{
			return (__aKey == src.first);
		}
	};

	//	template <class _Key, class _Val>
	struct _Compare {
		bool operator ()(const std::pair<_Key, _Val> &x, const std::pair<_Key, _Val> &y)
		{
			return x.second < y.second;
		}
	};
};



#define MESSAGE_HANDLER_WND(msg, func)						   \
	if (uMsg == msg) {										   \
		bHandled = TRUE;									   \
		lResult  = func(hWnd, uMsg, wParam, lParam, bHandled); \
		if (bHandled)										   \
			return TRUE;									   \
	}


#define PASS_MSG_MAP_MENUOWNER_TO_EXPMENU(x) \
	if ( x.GetMenu().IsMenu() ) {				 \
		CHAIN_MSG_MAP_MEMBER(x) 			 \
	}

#define PASS_MSG_MAP_MENUOWNER_TO_EXPMENU_DEFAULT(x) \
	if (x.GetMenu().IsMenu()  && m_bShow == FALSE) {	 \
		CHAIN_MSG_MAP_MEMBER(x) 					 \
	}

#define HANDLE_MENU_MESSAGE_CONTEXTMENU_EX(x)										 \
	if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM || uMsg == WM_MEASUREITEM) { \
		if (x != NULL) {															 \
			bHandled = TRUE;														 \
			OnMsgHandle(uMsg, wParam, lParam);										 \
			lResult  = x->HandleMenuMsg(uMsg, wParam, lParam);						 \
			if (lResult != NOERROR) bHandled = FALSE;								 \
		} else {																	 \
			bHandled = FALSE;														 \
		}																			 \
		if (bHandled)																 \
			return TRUE;															 \
	}



struct CMenuItem {
	CString 	m_strText;
	CString 	m_strPath;
	BOOL		m_bDir;
	BOOL		m_bSelected;
	HWND		m_hItemWnd;
	BOOL		m_bIcon;		//minit
	HMENU		m_hMenu;		//minit
	CIconHandle	m_Icon;

	// Constructor
	CMenuItem()
	{
		m_strText	= _T("");
		m_strPath	= _T("");
		m_bDir		= FALSE;
		m_bSelected = FALSE;
		m_hItemWnd	= NULL;
		m_bIcon 	= FALSE;
		m_hMenu 	= NULL; 	//+++
	}
};


////////////////////////////////////////////////////////////////////////////////////
// CExplorerMenu

class CExplorerMenu
{
public:
	enum { _nMaxMenuItemTextLength	= 100 };

	// Constructor/Destructor
	CExplorerMenu(int 				nInsertPointMenuItemID,
				  const CString&	strNoEntries			= _T("(empty)"),
				  int 				nMinID					= 0x7000,
				  int 				nMaxID					= 0x7FFF,
				  const CString&	strAdditional			= _T("Open This Directory"),
				  int 				nMaxMenuItemTextLength	= 55 );
	~CExplorerMenu();

	BOOL	InstallExplorerMenu(HMENU hMenu);
	void	SetTargetWindow(HWND hWnd);
	void	SetRootDirectoryPath(const CString& strPath);
	void	RefreshMenu();
	void	SetExcuteFunction(std::function<void (const CString&)> func);
	CString GetFilePath(const CMenuItemInfo& mii);
	CString GetFilePath(int nID);
	DWORD	GetStyle() const;
	void	SetStyle(DWORD dwStyle);
	CMenuHandle GetMenu();
	void	SetMaxMenuItemTextLength(int nCount);
	void	SetMaxMenuBreakCount(int nCount);
	void	AddIgnoredPath(const CString& strPath);
	void	ResetIgnoredPath();
	CMenuHandle GetFolderMenu(const CString& strPath);
	bool	IsSubMenu(HMENU hSubMenu);

	// Overridables
	virtual void	OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos);

	BEGIN_MSG_MAP_EX_decl( CExplorerMenu )

private:
	class Impl;
	Impl*	pImpl;
};



////////////////////////////////////////////////////////////////
// CStyleSheetMenu : スタイルシート用メニュー

class CStyleSheetMenu : public CExplorerMenu
{
public:
	//*+++ 適当にデフォルトコンストラクタを用意.
	CStyleSheetMenu(const HWND& hWnd);

private:
	const HWND& m_rhWnd;
};






// strFileTitle : ファイル名
// strPath		: ファイルを保存する場所
// strURL		: URL
BOOL SaveInternetShortcutFile(const CString &strFileTitle, const CString &strPath, const CString &strURL);



// Helper
template <class _MainFrame>
void _The_OpenAllFiles(const CString &strFilePath, _MainFrame *__pMainFrame)
{
	CString strCheck = strFilePath.Left(2);
	CString strPath  = strFilePath;

	if ( strPath.Mid(1, 1) == _T(" ") )
		strPath = strPath.Mid(2);


	if ( strCheck == _T("s ") ) {
		HWND		 hActiveWnd  = __pMainFrame->MDIGetActive();
		CChildFrame *child		 = __pMainFrame->GetChildFrame(hActiveWnd);

		if (!child)
			return;

		CString 	 strFileName = MtlGetWindowText(hActiveWnd);
		SaveInternetShortcutFile( strFileName, strPath, child->GetLocationURL() );
		return;

	} else if ( strCheck == _T("a ") && MtlIsDirectoryPath(strPath) ) {
		CLockRedrawMDIClient	 lock(__pMainFrame->m_hWndMDIClient);
		CDonutTabBar::CLockRedraw lock2(__pMainFrame->mdiTab());
		MtlForEachFile( strPath, [__pMainFrame](const CString &strFileName) {
			DonutOpenFile(__pMainFrame->m_hWnd, strFileName);
		});

	} else if ( strCheck == _T("r ") && MtlIsDirectoryPath(strPath) ) {
		CFavoriteOrderHandler	order;
		CString 				strRegKey = order.GetOrderKeyName(strPath);

		if ( strRegKey.IsEmpty() )
			return;

		Misc::CRegKey 	rkOrder;

		if (rkOrder.Open(HKEY_CURRENT_USER, strRegKey) != ERROR_SUCCESS) {
			CString strMsg;
			strMsg.Format(_T("レジストリキー\n%s\nのオープンに失敗しました。キーが存在しない可能性があります。"), strRegKey);
			MessageBox(NULL, strMsg, _T("エラー"), MB_OK);
			return;
		}

		rkOrder.DeleteValue( _T("Order") );
		rkOrder.Close();
	} else {
		DonutOpenFile( __pMainFrame->m_hWnd, strPath, DonutGetStdOpenFlag() );
	}
}





//////////////////////////////////////////////////////////////////////
// CFavoritesMenu : IEの順番にお気に入りを並べ替える機能付き

class CFavoritesMenu : public CExplorerMenu
{
public:
	// Ctor/Dtor
	CFavoritesMenu( int 			nInsertPointMenuItemID,
					const CString&	strNoEntries			= _T("(empty)"),
					int 			nMinID					= 0x7000,
					int 			nMaxID					= 0x7FFF,
					const CString&	strAdditional			= _T("Open This Directory"),
					int 			nMaxMenuItemTextLength	= 55 )
		: CExplorerMenu(nInsertPointMenuItemID, strNoEntries, nMinID, nMaxID, strAdditional, nMaxMenuItemTextLength)
	{ }

	// Overrides
	void OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos);
};


//////////////////////////////////////////////////////////////////////////////////////////
// CFavoriteGroupMenu : お気に入りグループ メニュー

template <class _MainFrame>
class	CFavoriteGroupMenu : public CExplorerMenu
{
	_MainFrame*		__m_pMainFrame;

public:
	// Constructor
	CFavoriteGroupMenu(_MainFrame *__pMainFrame, int nInsertPointMenuItemID)
		: CExplorerMenu(nInsertPointMenuItemID, _T("(なし)"), FAVGROUP_MENU_ID_MIN, FAVGROUP_MENU_ID_MAX)
		, __m_pMainFrame(__pMainFrame)
	{
		SetRootDirectoryPath( DonutGetFavoriteGroupFolder() );
		SetExcuteFunction(boost::bind(&CFavoriteGroupMenu::OnExecute, this, _1));
	}

	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CFavoriteMenu::OnExecute: %s\n"), strFilePath);

		DonutOpenFile(__m_pMainFrame->m_hWnd, strFilePath);
	}

	void RefreshMenu()
	{
		// set up option
		SetStyle(0);

		SetMaxMenuItemTextLength(CFavoritesMenuOption::GetMaxMenuItemTextLength());
		SetMaxMenuBreakCount(CFavoritesMenuOption::GetMaxMenuBreakCount());

		__super::RefreshMenu();
	}
};


#if 0

///////////////////////////////////////////////////////////////////////////
// CChildFavoriteMenu : for ChildFrame

template <class _ChildFrame>
class	CChildFavoriteMenu : public CFavoritesMenu<CChildFavoriteMenu<_ChildFrame> > 
{
	_ChildFrame *__m_pChildFrame;

public:
	// Constructor
	CChildFavoriteMenu(_ChildFrame *__pChildFrame, int nInsertPointMenuItemID)
		: CFavoritesMenu<CChildFavoriteMenu>(nInsertPointMenuItemID, _T("(なし)"), FAVORITE_MENU_ID_MIN, FAVORITE_MENU_ID_MAX/*0x012C, 0x7530*/)
		, __m_pChildFrame(__pChildFrame)
	{
		RefreshMenu();
	}

	// Overrides
	void	OnExecute(const CString &strFilePath)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CFavoriteMenu::OnExecute: %s\n"), strFilePath);
		CString strCheck = strFilePath.Left(2);
		CString strFile;

		if (strFilePath[1] == ' ') {
			strFile = strFilePath.Mid(2);
		} else {
			strFile = strFilePath;
		}
		if ( strCheck == _T("s ") ) {
			CString strFileTitle = MtlGetWindowText(__m_pChildFrame->m_hWnd);
			SaveInternetShortcutFile( strFileTitle, strFile, __m_pChildFrame->GetLocationURL() );
			return;

		} else if ( strCheck == _T("a ") ) {

		} else if ( strCheck == _T("r ") ) {
			ATLASSERT(FALSE);
		}

		if (  !MtlIsProtocol( strFile, _T("http") )
		   && !MtlIsProtocol( strFile, _T("https") ) )
		{
			if ( MtlPreOpenFile(strFile) )
				return;
			// handled
		}

		MTL::ParseInternetShortcutFile(strFile);

		if ( strFile == _T("javascript:location.reload()") ) return;

		__m_pChildFrame->Navigate2(strFile);
	}

	void	RefreshMenu()
	{
		MakeMenu();

		// set up option
		DWORD dwStyle = CFavoritesMenuOption::GetStyle();
		dwStyle 				&= ~EMS_ADDITIONAL_OPENALL;
		SetStyle(dwStyle);
		m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();

		SetRootDirectoryPath( DonutGetFavoritesFolder() );
	}

};


#endif