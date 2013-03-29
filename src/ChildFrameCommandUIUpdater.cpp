/**
*	@file	ChildFrameCommandUIUpdater.cpp
*	@brief	CChildFrameの状態をコマンドに反映させる
*/

#include "stdafx.h"
#include "ChildFrameCommandUIUpdater.h"
#include <boost\range\algorithm.hpp>
#include "ChildFrame.h"
#include "FaviconManager.h"
#include "ExStyle.h"

#define DONUTSHAREDUIDATA	_T("DonutSharedUIData")

// Data members
vector<ChildFrameUIData*>	CChildFrameCommandUIUpdater::s_vecpUIData;
int		CChildFrameCommandUIUpdater::s_nActiveUIIndex	= -1;
HWND	CChildFrameCommandUIUpdater::s_hWndActiveChildFrame	= NULL;
boost::unordered_map<HWND, int>	CChildFrameCommandUIUpdater::s_mapHWND_int;

HWND	CChildFrameCommandUIUpdater::s_hWndMainFrame = NULL;


void	CChildFrameCommandUIUpdater::AddCommandUIMap(HWND hWndChildFrame)
{
	::SendMessage(s_hWndMainFrame, WM_ADDCOMMANDUIMAP, (WPARAM)hWndChildFrame, 0);
}

void	CChildFrameCommandUIUpdater::RemoveCommandUIMap(HWND hWndChildFrame)
{
	::SendMessage(s_hWndMainFrame, WM_REMOVECOMMANDUIMAP, (WPARAM)hWndChildFrame, 0);
}

void	CChildFrameCommandUIUpdater::ChangeCommandUIMap(HWND hWndChildFrame)
{
	s_hWndActiveChildFrame = hWndChildFrame;
	boost::for_each(s_vecpUIData, [hWndChildFrame](ChildFrameUIData* pUIData) {
		pUIData->hWndActiveChildFrame	= hWndChildFrame;
	});
	if (hWndChildFrame == NULL) {
		s_nActiveUIIndex = -1;

		CString strapp;
		strapp.LoadString(IDR_MAINFRAME);
		::SetWindowText(s_hWndMainFrame, strapp);
		CWindow(s_hWndMainFrame).GetDlgItem(ATL_IDW_STATUS_BAR).SetWindowText(_T(""));
		::SendMessage(s_hWndMainFrame, WM_BROWSERLOCATIONCHANGE, (WPARAM)_T("\0"), 0);
	} else {
		s_nActiveUIIndex = _GetIndexFromHWND(hWndChildFrame);
		ATLASSERT( 0 <= s_nActiveUIIndex &&  s_nActiveUIIndex < static_cast<int>(s_vecpUIData.size()) );

		CString strapp;
		strapp.LoadString(IDR_MAINFRAME);
		CString strMainTitle;
		strMainTitle.Format(_T("%s - %s"), s_vecpUIData[s_nActiveUIIndex]->strTitle, strapp);
		::SetWindowText(s_hWndMainFrame, strMainTitle);

		HICON hFavicon = CFaviconManager::GetFavicon(s_vecpUIData[s_nActiveUIIndex]->strFaviconURL);
		::SendMessage(s_hWndMainFrame, WM_BROWSERLOCATIONCHANGE, (WPARAM)(LPCTSTR)s_vecpUIData[s_nActiveUIIndex]->strLocationURL, (LPARAM)hFavicon);
	}
	_UIUpdate();
}

ChildFrameUIData* CChildFrameCommandUIUpdater::GetChildFrameUIData(HWND hWndChildFrame)
{
	auto it = s_mapHWND_int.find(hWndChildFrame);
	if (it == s_mapHWND_int.end()) {
		ATLASSERT( FALSE );
		return nullptr;
	}

	return s_vecpUIData[it->second];
}

// Message map

void	CChildFrameCommandUIUpdater::OnAddCommandUIMap(HWND hWndChildFrame)
{
	CString sharedName;
	sharedName.Format(_T("%s0x%x"), DONUTSHAREDUIDATA, hWndChildFrame);
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedName);
	ATLASSERT( hMap );
	ChildFrameUIData* pUIData = (ChildFrameUIData*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	pUIData->hMapForMainFrameOpen = hMap;
	
	s_vecpUIData.push_back(pUIData);
	int nIndex = (int)s_vecpUIData.size() - 1;
	//s_vecpUIData[nIndex]->pChild = (CChildFrame*)::SendMessage(hWndChildFrame, WM_GET_CHILDFRAME, 0, 0);
	s_mapHWND_int.insert(make_pair(hWndChildFrame, nIndex));
}

void	CChildFrameCommandUIUpdater::OnRemoveCommandUIMap(HWND hWndChildFrame)
{
	auto it = s_mapHWND_int.find(hWndChildFrame);
	ATLASSERT( it != s_mapHWND_int.end() );
	int nDestroyIndex = it->second;

	HANDLE hMap = s_vecpUIData[nDestroyIndex]->hMapForMainFrameOpen;
	::UnmapViewOfFile(s_vecpUIData[nDestroyIndex]);
	::CloseHandle(hMap);

	s_vecpUIData.erase(s_vecpUIData.begin() + nDestroyIndex);
	s_mapHWND_int.erase(it);

	for (auto it = s_mapHWND_int.begin(); it != s_mapHWND_int.end(); ++it) {
		if (it->second > nDestroyIndex)
			--it->second;
	}
	if (s_nActiveUIIndex > nDestroyIndex)
		--s_nActiveUIIndex;
}

/// ChildFrameのUI更新マップを変更する(タブのメニュー用)
void	CChildFrameCommandUIUpdater::OnChangeChildFrameUIMap(HWND hWndChildFrame)
{
	ATLASSERT( ::IsWindow(hWndChildFrame) );
	s_hWndActiveChildFrame	= hWndChildFrame;
	boost::for_each(s_vecpUIData, [hWndChildFrame](ChildFrameUIData* pUIData) {
		pUIData->hWndActiveChildFrame	= hWndChildFrame;
	});
	s_nActiveUIIndex = _GetIndexFromHWND(hWndChildFrame);
	ATLASSERT( 0 <= s_nActiveUIIndex &&  s_nActiveUIIndex < static_cast<int>(s_vecpUIData.size()) );
}


LRESULT CChildFrameCommandUIUpdater::OnBrowserTitleChangeForUIUpdater(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nIndex = _GetIndexFromHWND((HWND)wParam);
	::SendMessage(s_hWndMainFrame, WM_BROWSERTITLECHANGE, wParam, (LPARAM)s_vecpUIData[nIndex]->strTitle);
	return 0;
}

LRESULT CChildFrameCommandUIUpdater::OnBrowserLocationChangeForUIUpdater(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	::SendMessage(s_hWndMainFrame, WM_BROWSERLOCATIONCHANGE, (WPARAM)s_vecpUIData[s_nActiveUIIndex]->strLocationURL, 0);
	return 0;
}

// UI map

BEGIN_UPDATE_COMMAND_UI_MAP_impl( CChildFrameCommandUIUpdater )
	if (s_nActiveUIIndex == -1) {
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_ON_OFF_MULTI	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_CHG_MULTI	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_CHG_SECU	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_DLIMAGES	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_RUNACTIVEXCTLS	, false )

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_BGSOUNDS	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_VIDEOS		, false )

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_ON_OFF_SECU	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_DLACTIVEXCTLS	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_SCRIPTS	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DLCTL_JAVA		, false )

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_DOCHOSTUI_OPENNEWWIN	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_MESSAGE_FILTER	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_MOUSE_GESTURE	, false )
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_BLOCK_MAILTO		, false )
		return FALSE;
	}

	m_pUIData = s_vecpUIData[s_nActiveUIIndex];

	UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_VIEW_BACK   , m_pUIData->bNavigateBack   , true ) 	// with popup
	UPDATE_COMMAND_UI_ENABLE_IF 		  ( ID_VIEW_FORWARD, m_pUIData->bNavigateForward )
	// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_BACK1    )
	// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_FORWARD1 )
	UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_BACK1   , m_pUIData->bNavigateBack	)
	UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_FORWARD1, m_pUIData->bNavigateForward )

	UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLEST, OnUpdateFontSmallestUI )
	UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLER , OnUpdateFontSmallerUI  )
	UPDATE_COMMAND_UI(	ID_VIEW_FONT_MEDIUM  , OnUpdateFontMediumUI   )
	UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGER  , OnUpdateFontLargerUI   )
	UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGEST , OnUpdateFontLargestUI  ) 			// with popup

	UPDATE_COMMAND_UI(	ID_DEFAULT_PANE , OnUpdateStatusBarUI )
	UPDATE_COMMAND_UI(	IDC_PROGRESS	, OnUpdateProgressUI  )
	UPDATE_COMMAND_UI(	ID_SECURE_PANE	, OnUpdateSecureUI	  )
	UPDATE_COMMAND_UI(	ID_PRIVACY_PANE , OnUpdatePrivacyUI   )

	UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_SEARCHBAR_HILIGHT, m_pUIData->bNowHilight )

	UPDATE_COMMAND_UI( ID_STYLESHEET_BASE, OnStyleSheetBaseUI )

	UPDATE_COMMAND_UI( ID_DLCTL_CHG_MULTI		, OnUpdateDLCTL_ChgMulti ) 				// with popup
	UPDATE_COMMAND_UI( ID_DLCTL_CHG_SECU		, OnUpdateDLCTL_ChgSecu  ) 				// with popup
	UPDATE_COMMAND_UI( ID_DLCTL_DLIMAGES		, OnUpdateDLCTL_DLIMAGES		 )		// with popup
	UPDATE_COMMAND_UI( ID_DLCTL_RUNACTIVEXCTLS	, OnUpdateDLCTL_RUNACTIVEXCTLS )		// with popup

	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_BGSOUNDS, DLCTL_BGSOUNDS, m_pUIData->dwDLCtrl )
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_VIDEOS, DLCTL_VIDEOS	 , m_pUIData->dwDLCtrl )

	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_DLACTIVEXCTLS, DLCTL_NO_DLACTIVEXCTLS, m_pUIData->dwDLCtrl )
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_SCRIPTS	  , DLCTL_NO_SCRIPTS	  , m_pUIData->dwDLCtrl )
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_JAVA		  , DLCTL_NO_JAVA		  , m_pUIData->dwDLCtrl )

	UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_MULTI,
			( ( m_pUIData->dwDLCtrl & (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) )
				== (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) ) )
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_SECU,
			( ( m_pUIData->dwDLCtrl & (DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA)) == 0 ) )

	UPDATE_COMMAND_UI(ID_AUTOREFRESH_USER, OnUpdateAutoRefreshUser)
	UPDATE_COMMAND_UI_SETCHECK_IF_PASS(ID_AUTOREFRESH_NONE, m_pUIData->dwAutoRefreshStyle == 0)
	UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_15SEC, DVS_AUTOREFRESH_15SEC, m_pUIData->dwAutoRefreshStyle)
	UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_30SEC, DVS_AUTOREFRESH_30SEC, m_pUIData->dwAutoRefreshStyle)
	UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_1MIN , DVS_AUTOREFRESH_1MIN , m_pUIData->dwAutoRefreshStyle)
	UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_2MIN , DVS_AUTOREFRESH_2MIN , m_pUIData->dwAutoRefreshStyle)
	UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_5MIN , DVS_AUTOREFRESH_5MIN , m_pUIData->dwAutoRefreshStyle)

	UPDATE_COMMAND_UI_SETCHECK_IF_PASS(ID_AUTO_REFRESH, m_pUIData->dwAutoRefreshStyle != 0)
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_DOCHOSTUI_OPENNEWWIN, DVS_EX_OPENNEWWIN, m_pUIData->dwExStyle)
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MESSAGE_FILTER, DVS_EX_MESSAGE_FILTER, m_pUIData->dwExStyle)
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MOUSE_GESTURE , DVS_EX_MOUSE_GESTURE , m_pUIData->dwExStyle)
	UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_BLOCK_MAILTO	, DVS_EX_BLOCK_MAILTO  , m_pUIData->dwExStyle)
END_UPDATE_COMMAND_UI_MAP()


void 	CChildFrameCommandUIUpdater::OnUpdateFontSmallestUI(CCmdUI *pCmdUI)
{
	LRESULT lFontSize = ::SendMessage(s_hWndActiveChildFrame, WM_GETBROWSERFONTSIZE, 0, 0);
	pCmdUI->SetRadio( lFontSize == 0L);
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontSmallerUI(CCmdUI *pCmdUI)
{
	LRESULT lFontSize = ::SendMessage(s_hWndActiveChildFrame, WM_GETBROWSERFONTSIZE, 0, 0);
	pCmdUI->SetRadio( lFontSize == 1L);
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontMediumUI(CCmdUI *pCmdUI)
{
	LRESULT lFontSize = ::SendMessage(s_hWndActiveChildFrame, WM_GETBROWSERFONTSIZE, 0, 0);
	pCmdUI->SetRadio( lFontSize == 2L);
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontLargerUI(CCmdUI *pCmdUI)
{
	LRESULT lFontSize = ::SendMessage(s_hWndActiveChildFrame, WM_GETBROWSERFONTSIZE, 0, 0);
	pCmdUI->SetRadio( lFontSize == 3L);
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontLargestUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) { // popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		LRESULT lFontSize = ::SendMessage(s_hWndActiveChildFrame, WM_GETBROWSERFONTSIZE, 0, 0);
		pCmdUI->SetRadio( lFontSize == 4L);
	}
}


void 	CChildFrameCommandUIUpdater::OnUpdateStatusBarUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(m_pUIData->strStatusBar);
}

void 	CChildFrameCommandUIUpdater::OnUpdateProgressUI(CCmdUI *pCmdUI)
{
	auto SetProgressPaneWidth = [pCmdUI] (int cxWidth) -> BOOL {
		CStatusBarCtrl statusbar   = pCmdUI->m_wndOther.GetParent();

		const int		nPanes	   = 5;
		//+++ プログレスペインとプロキシペインの交換時の不具合修正.
		int 			nIndex	   = (g_bSwapProxy == 0) ? 1 : 4;

		// get pane positions
		int *			pPanesPos  = (int *) _alloca( nPanes * sizeof (int) );

		statusbar.GetParts(nPanes, pPanesPos);

		// calculate offset
		int 		   cxPaneWidth = pPanesPos[nIndex] - ( (nIndex == 0) ? 0 : pPanesPos[nIndex - 1] );
		int 		   cxOff	   = cxWidth - cxPaneWidth;

		// find variable width pane
		int 		   nDef 	   = 0;

		// resize
		if (nIndex < nDef) {						// before default pane
			for (int i = nIndex; i < nDef; i++)
				pPanesPos[i] += cxOff;
		} else {									// after default one
			for (int i = nDef; i < nIndex; i++)
				pPanesPos[i] -= cxOff;
		}

		// set pane postions
		return	statusbar.SetParts(nPanes, pPanesPos) != 0;
	};

	CProgressBarCtrl progressbar = pCmdUI->m_wndOther;
	CStatusBarCtrl	 statusbar	 = pCmdUI->m_wndOther.GetParent();

	if ( m_pUIData->nProgress == -1 || (m_pUIData->nProgress == 0 && m_pUIData->nProgressMax == 0) ) {
		if (g_bSwapProxy == 0) {		//+++ 通常(左側にある)時のみ幅を0にする. プロキシと位置を交換してるときはやめ.
			SetProgressPaneWidth(0);
			progressbar.ShowWindow(SW_HIDE);
		} else {						//+++ 交換してるときは、とりあえず、長さ0で出しっぱなしで対処.
			progressbar.SetPos(0);		//+++ ただしゲージバー自体は消しておく
		}
		return;
	}

	CRect	rcProgressPart;
	if (g_bSwapProxy == false)
		statusbar.GetRect(1, rcProgressPart);
	else
		statusbar.GetRect(4, rcProgressPart);

	rcProgressPart.DeflateRect(2, 2);
	progressbar.MoveWindow(rcProgressPart, TRUE);
	progressbar.SetRange32(0, m_pUIData->nProgressMax);
	//x 0になっているときに毎フレームSetPos(0)するようにしたので、ここでは不要
	//x (というか、すこしでも最大値描画が行われるようにするため、無いほうがよい)
	//x if (m_nProgress >= m_nProgressMax && m_nProgressMax && g_bSwapProxy)
	//x progressbar.SetPos(0);
	//x else
	progressbar.SetPos(m_pUIData->nProgress);
	progressbar.ShowWindow(SW_SHOWNORMAL);

	//+++ サイズ固定でオプション設定でのサイズが反映されていなかったのでそれを反映. グローバル変数なのはとりあえず...
	SetProgressPaneWidth( g_dwProgressPainWidth );
}

void 	CChildFrameCommandUIUpdater::OnStyleSheetBaseUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} 
}

void CChildFrameCommandUIUpdater::OnUpdateSecureUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_pUIData->nSecureLockIcon != secureLockIconUnsecure) {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, 0), 0);

		// ツールチップのセット
		CString tip = _T("SSL 保護付き ");

		switch (m_pUIData->nSecureLockIcon) {
		case secureLockIconUnsecure:			tip  = "";					break;
		case secureLockIconMixed:				tip += _T("(複数）");		break;
		case secureLockIconSecureUnknownBits:	tip += _T("(不明）");		break;
		case secureLockIconSecure40Bit: 		tip += _T("(40ビット)");	break;
		case secureLockIconSecure56Bit: 		tip += _T("(56ビット)");	break;
		case secureLockIconSecureFortezza:		tip += _T("(Fortezza)");	break;
		case secureLockIconSecure128Bit:		tip += _T("(128ビット)");	break;
		default:								tip  = "";
		}
		wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_SECURE_PANE, (LPARAM) tip.GetBuffer(0) );

	} else {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, -1), 0);
	}
}


void CChildFrameCommandUIUpdater::OnUpdatePrivacyUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_pUIData->bPrivacyImpacted == FALSE)
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, 1), 0);
	else
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, -1), 0);

	wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_PRIVACY_PANE, (LPARAM) _T("プライバシー レポート") );
}

void CChildFrameCommandUIUpdater::OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		enum { FLAGS = (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) };
		pCmdUI->SetCheck( (m_pUIData->dwDLCtrl & FLAGS) == FLAGS );
	}
}



void CChildFrameCommandUIUpdater::OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		BOOL bSts = TRUE;

		if (m_pUIData->dwDLCtrl & DLCTL_NO_RUNACTIVEXCTLS)	bSts = FALSE;
		if (m_pUIData->dwDLCtrl & DLCTL_NO_DLACTIVEXCTLS)	bSts = FALSE;
		if (m_pUIData->dwDLCtrl & DLCTL_NO_SCRIPTS) 		bSts = FALSE;
		if (m_pUIData->dwDLCtrl & DLCTL_NO_JAVA)			bSts = FALSE;

		pCmdUI->SetCheck(bSts);
	}
}



void CChildFrameCommandUIUpdater::OnUpdateDLCTL_DLIMAGES(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		pCmdUI->SetCheck((m_pUIData->dwDLCtrl & DLCTL_DLIMAGES) != 0);
	}
}



void CChildFrameCommandUIUpdater::OnUpdateDLCTL_RUNACTIVEXCTLS(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		pCmdUI->SetCheck((m_pUIData->dwDLCtrl & DLCTL_NO_RUNACTIVEXCTLS) ? 0 : 1);
	}
}

void	CChildFrameCommandUIUpdater::OnUpdateAutoRefreshUser(CCmdUI *pCmdUI)
{
	DWORD	dwRefreshTime = CMainOption::s_dwAutoRefreshTime;

	pCmdUI->Enable(dwRefreshTime != 0);
	pCmdUI->SetCheck(m_pUIData->dwAutoRefreshStyle == DVS_AUTOREFRESH_USER);
	CString str;

	// NOTE: INUYA
	// if using StrFromTimeInterval , Load Shlwapi.dll...
	// dwRefreshTime is limited to 3599.
	if (dwRefreshTime % 60 == 0)	str.Format(_T("%d分")    , dwRefreshTime / 60);
	else if (dwRefreshTime >= 60)	str.Format(_T("%d分%d秒"), dwRefreshTime / 60, dwRefreshTime % 60);
	else							str.Format(_T("%d秒")    , dwRefreshTime % 60);

	str += "(ユーザー設定)";
	pCmdUI->SetText(str);
}



int	CChildFrameCommandUIUpdater::_GetIndexFromHWND(HWND hWndChildFrame)
{
	auto it = s_mapHWND_int.find(hWndChildFrame);
	ATLASSERT( it != s_mapHWND_int.end() );
	return it->second;
}

void	CChildFrameCommandUIUpdater::_UIUpdate()
{
	PostMessage(s_hWndMainFrame, WM_UIUPDATE, 0, 0);
}





/////////////////////////////////////////////////////////////////////////////////////
// CChildFrameUIStateChange

void	CChildFrameUIStateChange::AddCommandUIMap()
{
	CString sharedName;
	sharedName.Format(_T("%s0x%x"), DONUTSHAREDUIDATA, m_hWndChildFrame);
	HANDLE hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(ChildFrameUIData), sharedName);
	ATLASSERT( hMap );
	m_pUIData = (ChildFrameUIData*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	ATLASSERT( m_pUIData );
	m_pUIData->hMap	= hMap;

	m_hWndMainFrame = CWindow(m_hWndChildFrame).GetTopLevelWindow();
	::SendMessage(m_hWndMainFrame, WM_ADDCOMMANDUIMAP, (WPARAM)m_hWndChildFrame, 0);
}

void	CChildFrameUIStateChange::RemoveCommandUIMap()
{
	::SendMessage(m_hWndMainFrame, WM_REMOVECOMMANDUIMAP, (WPARAM)m_hWndChildFrame, 0);
	HANDLE hMap = m_pUIData->hMap;
	::UnmapViewOfFile(m_pUIData);
	::CloseHandle(hMap);
}


void	CChildFrameUIStateChange::SetNavigateBack(bool b)
{
	ChildFrameUIData& data = *m_pUIData;
	if (data.bNavigateBack != b) {
		data.bNavigateBack = b;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetNavigateForward(bool b)
{
	ChildFrameUIData& data = *m_pUIData;
	if (data.bNavigateForward != b) {
		data.bNavigateForward = b;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetTitle(LPCTSTR strTitle)
{
	ChildFrameUIData& data = *m_pUIData;
	::wcscpy_s(data.strTitle, strTitle);
	// タブのタイトルも変える
	::SendMessage(m_hWndMainFrame, WM_BROWSERTITLECHANGEFORUIUPDATER, (WPARAM)m_hWndChildFrame, 0);
}

void	CChildFrameUIStateChange::SetLocationURL(LPCTSTR strURL)
{
	ChildFrameUIData& data = *m_pUIData;
	if (::_wcsicmp(data.strLocationURL, strURL) != 0) {
		::wcscpy_s(data.strLocationURL, strURL);
		if (m_pUIData->hWndActiveChildFrame == m_hWndChildFrame)
			::SendMessage(m_hWndMainFrame, WM_BROWSERLOCATIONCHANGEFORUIUPDATER, (WPARAM)m_hWndChildFrame, 0);
	}
}

void	CChildFrameUIStateChange::SetFaviconURL(LPCTSTR strURL)
{
	ChildFrameUIData& data = *m_pUIData;
	::wcscpy_s(data.strFaviconURL, strURL);
}

void	CChildFrameUIStateChange::SetStatusText(LPCTSTR strText)
{
	ChildFrameUIData& data = *m_pUIData;
	if (::_wcsicmp(data.strStatusBar, strText) != 0) {
		::wcscpy_s(data.strStatusBar, strText);
		_UIUpdate();
	}

}

void	CChildFrameUIStateChange::SetProgress(long nProgress, long nProgressMax)
{
	ChildFrameUIData& data = *m_pUIData;
	if (data.nProgress != nProgress || data.nProgressMax != nProgressMax) {
		data.nProgress		= nProgress;
		data.nProgressMax	= nProgressMax;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetSecureLockIcon(int nIcon)
{
	ChildFrameUIData& data = *m_pUIData;
	if (data.nSecureLockIcon != nIcon) {
		data.nSecureLockIcon = nIcon;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetPrivacyImpacted(bool b)
{
	ChildFrameUIData& data = *m_pUIData;
	if (data.bPrivacyImpacted != b) {
		data.bPrivacyImpacted = b;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetDLCtrl(DWORD dw)
{
	if (m_pUIData == nullptr)	// 
		return ;
	ChildFrameUIData& data = *m_pUIData;
	if (data.dwDLCtrl != dw) {
		data.dwDLCtrl = dw;
		_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetExStyle(DWORD dw)
{
	if (m_pUIData == nullptr)	// 
		return ;
	ChildFrameUIData& data = *m_pUIData;
	if (data.dwExStyle != dw) {
		data.dwExStyle = dw;
		::SendMessage(m_hWndMainFrame, WM_CHILDFRAMEEXSTYLECHANGE, (WPARAM)m_hWndChildFrame, dw);
	}
}

void	CChildFrameUIStateChange::SetAutoRefreshStyle(DWORD dw)
{
	ChildFrameUIData& data = *m_pUIData;
	data.dwAutoRefreshStyle = dw;
}


void	CChildFrameUIStateChange::_UIUpdate()
{
	if (m_pUIData->hWndActiveChildFrame == m_hWndChildFrame)
		::PostMessage(m_hWndMainFrame, WM_UIUPDATE, 0, 0);
}




