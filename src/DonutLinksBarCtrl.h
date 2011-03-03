/**
 *	@file	DonutLinksBarCtrl.h
 *	@brief	リンクバー
 */
#pragma once

#include "OleDragDropExpToolBarCtrl.h"
#include "dialog/RenameFileDialog.h"


///////////////////////////////////////////////////////////////////
// CDonutLinksBarCtrl

template <class _MainFrame>
class CDonutLinksBarCtrl : public COleDragDrogExplorerToolBarCtrlImpl<CDonutLinksBarCtrl<_MainFrame> > 
{
public:
	DECLARE_WND_SUPERCLASS( _T("Donut_LinksBar"), GetWndClassName() )

private:
	typedef COleDragDrogExplorerToolBarCtrlImpl<CDonutLinksBarCtrl>  baseClass;
	typedef CDonutLinksBarCtrl<_MainFrame>							 thisClass;

	// Data members
	_MainFrame *		__m_pMainFrame;


public:
	// Constructor
	CDonutLinksBarCtrl(_MainFrame *__pMainFrame, int nInsertPointMenuItemID)
		: __m_pMainFrame(__pMainFrame)
		, COleDragDrogExplorerToolBarCtrlImpl<CDonutLinksBarCtrl>( nInsertPointMenuItemID, 0x012C, 0x0300 )
	{
		CIniFileI	pr( g_szIniFileName, _T("LinkBar") );
		CString 	strPath = pr.GetStringUW( _T("RootPath") );
		pr.Close();

		if (strPath.IsEmpty() == FALSE) //+++ 何か設定されていたら、フルパス化しておく.
			strPath = Misc::GetFullPath_ForExe( strPath );

		if (::PathFileExists(strPath) == FALSE) {
			MtlGetFavoriteLinksFolder(strPath);
		}

		ATLASSERT( !strPath.IsEmpty() );
		SetExplorerToolBarRootPath(strPath);
	}

	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CFavoriteMenu::OnExecute: %s\n"), strFilePath);
		_The_OpenAllFiles(strFilePath, __m_pMainFrame);
	}

	bool OnRenameExplorerToolBarCtrl(CString &strName)
	{
		CDonutRenameFileDialog	dlg;
		dlg.m_strName = strName;
		if ( dlg.DoModal() == IDOK && !dlg.m_strName.IsEmpty() ) {
			strName = dlg.m_strName;
			return true;
		}

		return false;
	}

	void OnInitialUpdateTBButtons(CSimpleArray<TBBUTTON> &items, LPCITEMIDLIST pidl)
	{
		ATLASSERT(pidl != NULL);
		if ( CFavoritesMenuOption::s_bIEOrder ) {
			CFavoritesOrder 	order;
			CString 			strDir = CItemIDList(pidl).GetPath();
			MtlGetFavoritesOrder( order, strDir );
			std::sort( _begin(items), _end(items), _FavoritesTBButtonCompare<thisClass>(order, this) );
		} else {
			baseClass::OnInitialUpdateTBButtons(items, pidl);
		}
	}


	// Message map
	BEGIN_MSG_MAP(thisClass)
		COMMAND_ID_HANDLER(ID_VIEW_REFRESH_FAVBAR, OnViewRefreshLinkBar)
		CHAIN_MSG_MAP(baseClass)
	ALT_MSG_MAP(1)
		CHAIN_MSG_MAP_ALT(baseClass, 1)
	END_MSG_MAP()


	LRESULT OnViewRefreshLinkBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		m_ExpMenu.RefreshStyle();

		_UpdateButtons(); // sorry

		CReBarCtrl rebar = GetParent();
		MtlRefreshBandIdealSize(rebar, m_hWnd);
		return 0;
	}


private:
	///////////////////////////////////////////
	// _FavoritesTBButtonCompare

	template <class _This>
	struct _FavoritesTBButtonCompare : public std::binary_function<const TBBUTTON &, const TBBUTTON &, bool> 
	{
	private:
		CFavoritesOrder&	_order;
		_This * 			_pThis;

	public:
		// Constructor
		_FavoritesTBButtonCompare(CFavoritesOrder &order, _This *pThis)
			: _order(order) , _pThis(pThis)
		{
		}

		bool operator ()(const TBBUTTON &x, const TBBUTTON &y)
		{
			CItemIDList idlA   = _pThis->_GetIDListFromCmdID(x.idCommand);
			ATLASSERT( !idlA.IsNull() );

			CItemIDList idlB   = _pThis->_GetIDListFromCmdID(y.idCommand);
			ATLASSERT( !idlB.IsNull() );

			const int	enough = 10000;
			CString 	strA   = MtlGetFileName( idlA.GetPath() );
			CString 	strB   = MtlGetFileName( idlB.GetPath() );
			ATLASSERT( !strA.IsEmpty() && !strB.IsEmpty() );

			int 		itA    = _order.Lookup(strA);
			int 		itB    = _order.Lookup(strB);

			if (itA == -1 || itA == FAVORITESORDER_NOTFOUND)  // fixed by fub, thanks.
				itA = enough;

			if (itB == -1 || itB == FAVORITESORDER_NOTFOUND)
				itB = enough;

			if (itA == enough && itB == enough) {
				bool bParentA = _pThis->_IsFolderCmdID(x.idCommand);
				bool bParentB = _pThis->_IsFolderCmdID(y.idCommand);

				if (bParentA == bParentB) {
					int nRet = ::lstrcmp(strA, strB);
					return (nRet < 0) /*? true : false*/;
					//return strA < strB;
				} else {
					if (bParentA)
						return true;
					else
						return false;
				}
			} else {
				return itA < itB;
			}
		}
	};

};
