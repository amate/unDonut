/**
*	@file	DonutCommandBar.h
*	@brief	コマンドバークラス
*/

#pragma once

// 前方宣言
class CRecentClosedTabList;


///////////////////////////////////////////////////////
// コマンドバー

class CDonutCommandBar
{
public:
	CDonutCommandBar();
	~CDonutCommandBar();

	HWND	Create(HWND hWndParent);
	void	SetFont(HFONT hFont);

	HWND	GetHWND() const;

	void	SetRecentClosedTabList(CRecentClosedTabList* pList);

private:
	class Impl;
	Impl*	pImpl;
};















