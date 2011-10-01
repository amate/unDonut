/**
*	@file	ChildFrameCommandUIUpdater.cpp
*	@brief	CChildFrameの状態をコマンドに反映させる
*/

#include "stdafx.h"
#include "ChildFrameCommandUIUpdater.h"
#include "ChildFrame.h"
#include "FaviconManager.h"

// Data members
vector<unique_ptr<ChildFrameUIData> >	CChildFrameCommandUIUpdater::s_vecpUIData;
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
	if (hWndChildFrame == NULL) {
		s_nActiveUIIndex = -1;

		CString strapp;
		strapp.LoadString(IDR_MAINFRAME);
		::SetWindowText(s_hWndMainFrame, strapp);
		CWindow(s_hWndMainFrame).GetDlgItem(ATL_IDW_STATUS_BAR).SetWindowText(_T(""));
		::SendMessage(s_hWndMainFrame, WM_BROWSERLOCATIONCHANGE, (WPARAM)_T("\0"), 0);
	} else {
		s_nActiveUIIndex = _GetIndexFromHWND(hWndChildFrame);

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

// Message map

void	CChildFrameCommandUIUpdater::OnAddCommandUIMap(HWND hWndChildFrame)
{
	s_vecpUIData.push_back(unique_ptr<ChildFrameUIData>(new ChildFrameUIData));
	int nIndex = (int)s_vecpUIData.size() - 1;
	s_vecpUIData[nIndex]->pChild = (CChildFrame*)::SendMessage(hWndChildFrame, WM_GET_CHILDFRAME, 0, 0);
	s_mapHWND_int.insert(make_pair(hWndChildFrame, nIndex));
}

void	CChildFrameCommandUIUpdater::OnRemoveCommandUIMap(HWND hWndChildFrame)
{
	auto it = s_mapHWND_int.find(hWndChildFrame);
	ATLASSERT( it != s_mapHWND_int.end() );
	int nDestroyIndex = it->second;
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
	s_nActiveUIIndex = _GetIndexFromHWND(hWndChildFrame);
	ATLASSERT(s_nActiveUIIndex != -1);
}

// UI map

void 	CChildFrameCommandUIUpdater::OnUpdateFontSmallestUI(CCmdUI *pCmdUI)
{
	CComPtr<IWebBrowser2>	spBrowser = m_pUIData->pChild->GetMarshalIWebBrowser();
	if (spBrowser) {
		CComVariant 	var;
		CComVariant		varIn;
		spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);
		pCmdUI->SetRadio( var == CComVariant(0L));
	}
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontSmallerUI(CCmdUI *pCmdUI)
{
	CComPtr<IWebBrowser2>	spBrowser = m_pUIData->pChild->GetMarshalIWebBrowser();
	if (spBrowser) {
		CComVariant 	var;
		CComVariant		varIn;
		spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);
		pCmdUI->SetRadio( var == CComVariant(1L));
	}
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontMediumUI(CCmdUI *pCmdUI)
{
	CComPtr<IWebBrowser2>	spBrowser = m_pUIData->pChild->GetMarshalIWebBrowser();
	if (spBrowser) {
		CComVariant 	var;
		CComVariant		varIn;
		spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);
		pCmdUI->SetRadio( var == CComVariant(2L));
	}
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontLargerUI(CCmdUI *pCmdUI)
{
	CComPtr<IWebBrowser2>	spBrowser = m_pUIData->pChild->GetMarshalIWebBrowser();
	if (spBrowser) {
		CComVariant 	var;
		CComVariant		varIn;
		spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);
		pCmdUI->SetRadio( var == CComVariant(3L));
	}
}

void 	CChildFrameCommandUIUpdater::OnUpdateFontLargestUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) { // popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		CComPtr<IWebBrowser2>	spBrowser = m_pUIData->pChild->GetMarshalIWebBrowser();
		if (spBrowser) {
			CComVariant 	var;
			CComVariant		varIn;
			spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varIn, &var);
			pCmdUI->SetRadio( var == CComVariant(4L));
		}
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

void	CChildFrameUIStateChange::SetNavigateBack(bool b)
{
	ChildFrameUIData& data = GetUIData();
	if (data.bNavigateBack != b) {
		data.bNavigateBack = b;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetNavigateForward(bool b)
{
	ChildFrameUIData& data = GetUIData();
	if (data.bNavigateForward != b) {
		data.bNavigateForward = b;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetTitle(LPCTSTR strTitle)
{
	ChildFrameUIData& data = GetUIData();
	data.strTitle	= strTitle;
	::SendMessage(CChildFrameCommandUIUpdater::s_hWndMainFrame, WM_BROWSERTITLECHANGE, (WPARAM)m_hWndChildFrame, (LPARAM)strTitle);
}

void	CChildFrameUIStateChange::SetLocationURL(LPCTSTR strURL)
{
	ChildFrameUIData& data = GetUIData();
	if (data.strLocationURL != strURL) {
		data.strLocationURL	= strURL;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			::SendMessage(CChildFrameCommandUIUpdater::s_hWndMainFrame, WM_BROWSERLOCATIONCHANGE, (WPARAM)strURL, 0);
	}
}

void	CChildFrameUIStateChange::SetFaviconURL(LPCTSTR strURL)
{
	ChildFrameUIData& data = GetUIData();
	data.strFaviconURL = strURL;
}

void	CChildFrameUIStateChange::SetStatusText(LPCTSTR strText)
{
	ChildFrameUIData& data = GetUIData();
	if (data.strStatusBar != strText) {
		data.strStatusBar = strText;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}

}

void	CChildFrameUIStateChange::SetProgress(long nProgress, long nProgressMax)
{
	ChildFrameUIData& data = GetUIData();
	if (data.nProgress != nProgress || data.nProgressMax != nProgressMax) {
		data.nProgress		= nProgress;
		data.nProgressMax	= nProgressMax;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetSecureLockIcon(int nIcon)
{
	ChildFrameUIData& data = GetUIData();
	if (data.nSecureLockIcon != nIcon) {
		data.nSecureLockIcon = nIcon;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetPrivacyImpacted(bool b)
{
	ChildFrameUIData& data = GetUIData();
	if (data.bPrivacyImpacted != b) {
		data.bPrivacyImpacted = b;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetDLCtrl(DWORD dw)
{
	ChildFrameUIData& data = GetUIData();
	if (data.dwDLCtrl != dw) {
		data.dwDLCtrl = dw;
		if (CChildFrameCommandUIUpdater::s_hWndActiveChildFrame == m_hWndChildFrame)
			CChildFrameCommandUIUpdater::_UIUpdate();
	}
}

void	CChildFrameUIStateChange::SetExStyle(DWORD dw)
{
	ChildFrameUIData& data = GetUIData();
	data.dwExStyle = dw;
}

void	CChildFrameUIStateChange::SetAutoRefreshStyle(DWORD dw)
{
	ChildFrameUIData& data = GetUIData();
	data.dwAutoRefreshStyle = dw;
}


ChildFrameUIData&	CChildFrameUIStateChange::GetUIData()
{
	int nIndex = CChildFrameCommandUIUpdater::_GetIndexFromHWND(m_hWndChildFrame);
	return *CChildFrameCommandUIUpdater::s_vecpUIData[nIndex];
}







