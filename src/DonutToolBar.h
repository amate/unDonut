/**
 *	@file	DonutToolBar.h
 *	@brief	ツールバー
 */
#pragma once


///////////////////////////////////////////////////////////////////
// CDonutToolBar

class CDonutToolBar
{
public:
	// Constructor/Destructor
	CDonutToolBar();
	~CDonutToolBar();

	HWND	Create(HWND hWndParent);
	void	SetFont(HFONT hFont);
	void	ReloadSkin();
	void	Customize();
	function<void ()> GetInitButtonfunction();

private:
	class Impl;
	Impl*	pImpl;
};
