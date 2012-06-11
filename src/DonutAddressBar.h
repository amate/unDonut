// DonutAddressBar.h

#pragma once

#include "BeginMsgMap_decl.h"

///////////////////////////////////////////////////////////////
// CDonutAddressBar

class CDonutAddressBar
{
public:
	// Constructor/Destructor
	CDonutAddressBar();
	~CDonutAddressBar();

	static CDonutAddressBar* GetInstance();
	HWND	Create(HWND	hWndParent, UINT nID, UINT nGoBtnCmdID, int cx, int cy, COLORREF clrMask, UINT nFlags = ILC_COLOR24);
	void	InitReBarBandInfo(CReBarCtrl rebar);
	
	// Attribute
	void	SetFont(HFONT hFont);
	void	ReloadSkin(int nCmbStyle);
	static CString GetAddressBarText();
	BOOL	IsWindow() const;
	bool	GetDroppedStateEx() const;
	CEdit	GetEditCtrl();
	void	ReplaceIcon(HICON hIcon);

	// Oparation
	void	ShowDropDown(BOOL bShow);
	void	SetWindowText(LPCTSTR str);
	void	SetFocus();
	void	ShowGoButton(bool bShow);
	void	ShowAddresText(CReBarCtrl rebar, BOOL bShow);


	BOOL	PreTranslateMessage(MSG *pMsg);

	// Message map
	BEGIN_MSG_MAP_EX_decl( CDonutAddressBar )

private:
	struct Impl;
	Impl*	pImpl;
	static CDonutAddressBar*	s_pThis;
};


