/**
 *	@file	DonutStatusBarCtrl.h
 *	@brief	ステータス・バー
 */

#pragma once

#include "CmbboxPrxy.h"
#include "FavoritesMenu.h"

//////////////////////////////////////////////////////////////////////////
// CDonutStatusBarCtrl

class CDonutStatusBarCtrl 
	: public CMultiPaneStatusBarCtrlImpl<CDonutStatusBarCtrl>
	, public COwnerDraw<CDonutStatusBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS( _T("DonutStatusBar"), GetWndClassName() )

	// Constructor/Destructor
	CDonutStatusBarCtrl();
	~CDonutStatusBarCtrl();

	CComboBoxPrxyR& GetProxyComboBox() { return m_cmbProxy; }
	void	SetProxyComboBoxFont(HFONT hFont, bool bRedraw = true);

	void	SetIcon(int nIdPane, int nIndexIcon);
	void	SetCommand(int nPaneID, int nCommand) { m_mapCmdID.Add(nPaneID, nCommand); }

	// Overrides
	void	DrawItem(LPDRAWITEMSTRUCT lpdis);

	void	SetOwnerDraw(BOOL bOwnerdraw);

	bool	IsValidBmp() const { return m_bmpBg.m_hBitmap != 0; }

	void	ReloadSkin(int nStyle, int colText /*=0*/, int colBack /*=-1*/);

	void	SetText(int nPane, LPCTSTR lpszText, int nType = 0);

	int		GetPaneCount() { return GetParts(-1, NULL); }

	void	DrawBackGround(LPRECT lpRect, HDC hDCSrc, HDC hDCDest);

	void	OnImageListUpdated();

public:
	// Message map
	BEGIN_MSG_MAP(CDonutStatusBarCtrl)
		USER_MSG_WM_GET_OWNERDRAWMODE (OnGetOwnerDrawMode)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MESSAGE_HANDLER 	(WM_CREATE			, OnCreate		)
		MESSAGE_HANDLER 	(WM_STATUS_SETICON	, OnSetIcon 	)
		MESSAGE_HANDLER 	(WM_STATUS_SETTIPTEXT,OnSetTipText	)
		MESSAGE_HANDLER 	(WM_SETTEXT 		, OnSetText 	)
		MESSAGE_HANDLER 	(SB_SETTEXT 		, OnSetStatusText)
		MESSAGE_HANDLER 	(SB_SIMPLE			, OnSimple		)
		MSG_WM_SIZE			( OnSize )
		CHAIN_MSG_MAP		(CMultiPaneStatusBarCtrlImpl<CDonutStatusBarCtrl>)
		CHAIN_MSG_MAP_ALT	(COwnerDraw<CDonutStatusBarCtrl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetTipText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSimple(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnGetOwnerDrawMode() { return IsValidBmp(); }
	LRESULT OnSetStatusText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	void	OnSize(UINT nType, CSize size);

private:
  #if 0
	struct _IconInfo {
		int 	nIconIndex;
		int 	nIDPane;
		HICON	hIcon;
	};
  #endif
	CProgressBarCtrl		m_wndProgress;
	CComboBoxPrxyR			m_cmbProxy;

	std::map<int, HICON>	m_mapIcon;
	CBitmap 				m_bmpBg;
	CImageList				m_imgList;
	CCrasyMap<int, int> 	m_mapCmdID; 		// CmdID
	CString 				m_strText;
	BOOL					m_bOwnerdraw;
	int 					m_nStatusStyle;
	COLORREF				m_colText;
	COLORREF				m_colBack;
	BOOL					m_bUseBackColor;
};

