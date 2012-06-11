/**
*	@file	ChildFrameCommandUIUpdater.h
*	@brief	CChildFrameの状態をコマンドに反映させる
*/

#pragma once

#include <boost/unordered_map.hpp>
#include "MtlUpdateCmdUI.h"
#include "resource.h"
#include "DonutView.h"

inline size_t hash_value(HWND hWnd)
{
	size_t h = 0;
	boost::hash_combine(h, hWnd);
	return h;
}

// 前方宣言
class CChildFrame;
class CChildFrameUIStateChange;


struct ChildFrameUIData {
	HANDLE	hMap;
	HWND	hWndActiveChildFrame;
	bool	bNavigateBack;
	bool	bNavigateForward;
	WCHAR	strTitle[INTERNET_MAX_URL_LENGTH];
	WCHAR	strLocationURL[INTERNET_MAX_URL_LENGTH];
	WCHAR	strFaviconURL[INTERNET_MAX_URL_LENGTH];
	WCHAR	strStatusBar[INTERNET_MAX_URL_LENGTH];
	long	nProgress;
	long	nProgressMax;
	int		nSecureLockIcon;
	bool	bPrivacyImpacted;
	bool	bNowHilight;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefreshStyle;

	ChildFrameUIData() :
		hMap(NULL),
		hWndActiveChildFrame(NULL),
		bNavigateBack(false),
		bNavigateForward(false),
		nProgress(0),
		nProgressMax(0),
		nSecureLockIcon(0),
		bPrivacyImpacted(true),
		bNowHilight(false),
		dwDLCtrl(0),
		dwExStyle(0),
		dwAutoRefreshStyle(0)
	{
		strTitle[0] = 0;
		strLocationURL[0] = 0;
		strFaviconURL[0] = 0;
		strStatusBar[0] = 0;
	}
};


//////////////////////////////////////////////////////////////////
// CChildFrameCommandUIUpdater : メインフレームからチェインされてくる

class CChildFrameCommandUIUpdater : public CUpdateCmdUI<CChildFrameCommandUIUpdater>
{
	friend class CChildFrameUIStateChange;

public:
	static void SetMainFrameHWND(HWND hWnd) { s_hWndMainFrame = hWnd; }

	static void	AddCommandUIMap(HWND hWndChildFrame);
	static void	RemoveCommandUIMap(HWND hWndChildFrame);

	static void	ChangeCommandUIMap(HWND hWndChildFrame);

	static HWND	GetActiveChildFrameWindowHandle() { return s_hWndActiveChildFrame; }
	static ChildFrameUIData* GetActiveChildFrameUIData() { 
		return s_nActiveUIIndex != -1 ? s_vecpUIData[s_nActiveUIIndex] : nullptr;
	}
	static ChildFrameUIData* GetChildFrameUIData(HWND hWndChildFrame);

	// Message map
	BEGIN_MSG_MAP( CChildFrameCommandUIUpdater )
		USER_MSG_WM_ADDCOMMANDUIMAP( OnAddCommandUIMap )
		USER_MSG_WM_REMOVECOMMANDUIMAP( OnRemoveCommandUIMap )
		USER_MSG_WM_CHANGECHILDFRAMEUIMAP( OnChangeChildFrameUIMap )

		MESSAGE_HANDLER_EX( WM_BROWSERTITLECHANGEFORUIUPDATER	, OnBrowserTitleChangeForUIUpdater	  )
		MESSAGE_HANDLER_EX( WM_BROWSERLOCATIONCHANGEFORUIUPDATER, OnBrowserLocationChangeForUIUpdater )
	END_MSG_MAP()

	void	OnAddCommandUIMap(HWND hWndChildFrame);
	void	OnRemoveCommandUIMap(HWND hWndChildFrame);
	void	OnChangeChildFrameUIMap(HWND hWndChildFrame);

	LRESULT OnBrowserTitleChangeForUIUpdater(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowserLocationChangeForUIUpdater(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Update Command UI Map
	BEGIN_UPDATE_COMMAND_UI_MAP_decl( CChildFrameCommandUIUpdater )

	void 	OnUpdateFontSmallestUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontSmallerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontMediumUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargestUI(CCmdUI *pCmdUI);

	void 	OnUpdateStatusBarUI(CCmdUI *pCmdUI);
	void 	OnUpdateProgressUI(CCmdUI *pCmdUI);
	void	OnUpdateSecureUI(CCmdUI *pCmdUI);
	void	OnUpdatePrivacyUI(CCmdUI *pCmdUI);

	void 	OnStyleSheetBaseUI(CCmdUI *pCmdUI);

	void	OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI);
	void	OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI);
	void	OnUpdateDLCTL_DLIMAGES(CCmdUI *pCmdUI);
	void	OnUpdateDLCTL_RUNACTIVEXCTLS(CCmdUI *pCmdUI);

	void	OnUpdateAutoRefreshUser(CCmdUI *pCmdUI);

private:
	static int	_GetIndexFromHWND(HWND hWndChildFrame);
	static void	_UIUpdate();

	// Data members
	static vector<ChildFrameUIData*>	s_vecpUIData;
	static int	s_nActiveUIIndex;
	static HWND	s_hWndActiveChildFrame;
	static boost::unordered_map<HWND, int>	s_mapHWND_int;

	static HWND	s_hWndMainFrame;

	ChildFrameUIData*	m_pUIData;
};

//////////////////////////////////////////////////////////////////////////
// CChildFrameUIStateChange : ChildFrameがメインフレームにUIの変更を通知するために利用する

class CChildFrameUIStateChange
{
public:
	CChildFrameUIStateChange() : m_hWndChildFrame(NULL), m_hWndMainFrame(NULL), m_pUIData(NULL)
	{	}

	void	SetChildFrame(HWND hWnd) { m_hWndChildFrame = hWnd; }
	void	AddCommandUIMap();
	void	RemoveCommandUIMap();

	void	SetNavigateBack(bool b);
	void	SetNavigateForward(bool b);
	void	SetTitle(LPCTSTR strTitle);
	void	SetLocationURL(LPCTSTR strURL);
	void	SetFaviconURL(LPCTSTR strURL);
	void	SetStatusText(LPCTSTR strText);
	void	SetProgress(long nProgress, long nProgressMax);
	void	SetSecureLockIcon(int nIcon);
	void	SetPrivacyImpacted(bool b);
	void	SetDLCtrl(DWORD dw);
	void	SetExStyle(DWORD dw);
	void	SetAutoRefreshStyle(DWORD dw);

private:
	void	_UIUpdate();

	// Data members
	HWND	m_hWndChildFrame;
	HWND	m_hWndMainFrame;
	ChildFrameUIData* m_pUIData;
};












