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
	CChildFrame*	pChild;
	bool	bNavigateBack;
	bool	bNavigateForward;
	CString strTitle;
	CString strLocationURL;
	CString strFaviconURL;
	CString	strStatusBar;
	long	nProgress;
	long	nProgressMax;
	int		nSecureLockIcon;
	bool	bPrivacyImpacted;
	bool	bNowHilight;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefreshStyle;

	ChildFrameUIData() :
		pChild(nullptr),
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
	{	}
};


class CChildFrameCommandUIUpdater : public CUpdateCmdUI<CChildFrameCommandUIUpdater>
{
	friend class CChildFrameUIStateChange;

public:
	static void SetMainFrameHWND(HWND hWnd) { s_hWndMainFrame = hWnd; }

	static void	AddCommandUIMap(HWND hWndChildFrame);
	static void	RemoveCommandUIMap(HWND hWndChildFrame);

	static void	ChangeCommandUIMap(HWND hWndChildFrame);

	static HWND	GetActiveChildFrameWindowHandle() { return s_hWndActiveChildFrame; }

	// Message map
	BEGIN_MSG_MAP( CChildFrameCommandUIUpdater )
		USER_MSG_WM_ADDCOMMANDUIMAP( OnAddCommandUIMap )
		USER_MSG_WM_REMOVECOMMANDUIMAP( OnRemoveCommandUIMap )
		USER_MSG_WM_CHANGECHILDFRAMEUIMAP( OnChangeChildFrameUIMap )
	END_MSG_MAP()

	void	OnAddCommandUIMap(HWND hWndChildFrame);
	void	OnRemoveCommandUIMap(HWND hWndChildFrame);
	void	OnChangeChildFrameUIMap(HWND hWndChildFrame);

	// Update Command UI Map
	BEGIN_UPDATE_COMMAND_UI_MAP( CChildFrameCommandUIUpdater )
		if (s_nActiveUIIndex == -1)
			return FALSE;

		m_pUIData = s_vecpUIData[s_nActiveUIIndex].get();

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

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_DOCHOSTUI_OPENNEWWIN, DVS_EX_OPENNEWWIN, m_pUIData->dwExStyle)

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS(ID_AUTO_REFRESH, m_pUIData->dwAutoRefreshStyle != 0)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MESSAGE_FILTER, DVS_EX_MESSAGE_FILTER, m_pUIData->dwExStyle)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MOUSE_GESTURE , DVS_EX_MOUSE_GESTURE , m_pUIData->dwExStyle)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_BLOCK_MAILTO	, DVS_EX_BLOCK_MAILTO  , m_pUIData->dwExStyle)
	END_UPDATE_COMMAND_UI_MAP()

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
	static vector<unique_ptr<ChildFrameUIData> >	s_vecpUIData;
	static int	s_nActiveUIIndex;
	static HWND	s_hWndActiveChildFrame;
	static boost::unordered_map<HWND, int>	s_mapHWND_int;

	static HWND	s_hWndMainFrame;

	ChildFrameUIData*	m_pUIData;
};


class CChildFrameUIStateChange
{
public:
	CChildFrameUIStateChange() : m_hWndChildFrame(NULL)
	{	}

	void	SetChildFrame(HWND hWnd) { m_hWndChildFrame = hWnd; }

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
	ChildFrameUIData&	GetUIData();

	// Data members
	HWND	m_hWndChildFrame;
};












