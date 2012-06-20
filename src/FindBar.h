/**
*	@ file	FindBar.h
*	@brief	Firefoxライクなページ内検索バー
*/

#pragma once


class CFindBar
{
public:
	CFindBar();
	~CFindBar();

	HWND	Create(HWND hWndParent);
	void	SetUpdateLayoutFunc(function<void (BOOL)> func);

	HWND	GetHWND();
	bool	FindBarBottom();
	void	ShowFindBar(const CString& strKeyword);

private:
	class Impl;
	Impl*	pImpl;
};





























