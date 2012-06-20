/**
 *	@file	DonutFavoritesMenu.h
 *	@brief	お気に入りメニュー.
 */

#pragma once

#include "option/FavoriteMenuDialog.h"
#include "FavoritesMenu.h"
#include "IniFile.h"
#include "MtlWeb.h"


inline CString DonutGetFavoriteGroupFolder()
{
	return Misc::GetExeDirectory() + _T("FavoriteGroup\\");
}

#if 0

/////////////////////////////////////////////////////////////////////////
// CDonutFavoritesMenu

template <class _MainFrame>
class CDonutFavoritesMenu : public CFavoritesMenu
{
	_MainFrame *		__m_pMainFrame;

public:
	// Constructor
	CDonutFavoritesMenu(_MainFrame *__pMainFrame, int nInsertPointMenuItemID)
		: CFavoritesMenu(nInsertPointMenuItemID
						, _T("(なし)")
						, FAVORITE_MENU_ID_MIN, FAVORITE_MENU_ID_MAX
						, _T("これらをすべて開く") )
		, __m_pMainFrame(__pMainFrame)
		, m_nSelIndex(-1)
	{
		SetRootDirectoryPath( DonutGetFavoritesFolder() );
		SetExcuteFunction(boost::bind(&CDonutFavoritesMenu::OnExecute, this, _1));
	}

	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		_The_OpenAllFiles(strFilePath, __m_pMainFrame);
	}

	void RefreshMenu()
	{
		SetStyle( CFavoritesMenuOption::GetStyle() );
		SetMaxMenuItemTextLength(CFavoritesMenuOption::GetMaxMenuItemTextLength());
		SetMaxMenuBreakCount(CFavoritesMenuOption::GetMaxMenuBreakCount());

		__super::RefreshMenu();
	}


public:
	// Message map
	BEGIN_MSG_MAP(CDonutFavoritesMenu)
		MSG_WM_MENUCHAR( OnMenuChar )
		CHAIN_MSG_MAP( CFavoritesMenu )
	END_MSG_MAP()


	// メニュー上で文字キーを押したとき、もしメニュー項目があればそれを選択する
	LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenuHandle menu)
	{
		LRESULT lRet;

		if (nChar != 0x0D) {
			lRet = 0;
		} else {
			lRet = MAKELRESULT(1, 1);
		}
		if (nFlags == MF_POPUP) {
			// Convert character to lower/uppercase and possibly Unicode, using current keyboard layout
			TCHAR		ch		  = nChar;
			int 		nCount	  = ::GetMenuItemCount(menu);
			int 		nRetCode  = MNC_EXECUTE;
			BOOL		bRet	  = FALSE;
			TCHAR		szString[_nMaxMenuItemTextLength] = _T("\0");
			WORD		wMnem	  = 0;
			bool		bFound	  = false;
			int 		nSelIndex = m_nSelIndex;
			int 		nSelFirst = -1;
			int 		nHitCnt   = 0;

			for (int i = 0; i < nCount; i++) {
				CMenuItemInfo 	mii;
				mii.cch 	   = _nMaxMenuItemTextLength;
				mii.fMask	   = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
				mii.dwTypeData = szString;
				bRet		   = menu.GetMenuItemInfo(i, TRUE, &mii);
				if ( !bRet || (mii.fType & MFT_SEPARATOR) )
					continue;

				CMenuItem* pMenuItem = (CMenuItem*) mii.dwItemData;
				if (pMenuItem == NULL)
					continue;

				LPTSTR	p = pMenuItem->m_strText.GetBuffer(0);
				if (p != NULL && *p) {
					DWORD 	dwP = MAKELONG(*(p), 0);
					DWORD 	dwC = MAKELONG( ch , 0);

					if ( ::CharLower( (LPTSTR) ULongToPtr(dwP) ) == ::CharLower( (LPTSTR) ULongToPtr(dwC) ) ) {
						if (nSelFirst == -1)
							nSelFirst = i;

						nHitCnt++;

						if (!bFound && m_nSelIndex < i) {
							wMnem		= (WORD) i;
							PostMessage(__m_pMainFrame->m_hWnd, TB_SETHOTITEM, (WPARAM) -1, 0L);

							if (m_nSelIndex < i) {
								m_nSelIndex = i;
								bFound		= true;
								// nRetCode = MNC_SELECT;
								// break;
							}

							bFound		= true;
							m_nSelIndex = i;
							// GiveFocusBack();
						} else {
							if (bFound) {
								// nRetCode = MNC_SELECT;
								// break;
							}
						}
					}
				}
			}

			if (nSelIndex == m_nSelIndex && nSelFirst != -1) {
				wMnem		= (WORD) nSelFirst;
				m_nSelIndex = nSelFirst;
				bFound		= true;
			}

			if (nHitCnt > 1)
				nRetCode = MNC_SELECT;

			if (bFound) {
				lRet	 = MAKELRESULT(wMnem, nRetCode);
			}
		}

		return lRet;
	}
private:
	// Data members
	int	m_nSelIndex;

};

#endif
