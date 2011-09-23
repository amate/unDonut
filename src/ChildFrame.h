/**
 *	@file	ChildFrame.h
 *	@brief	タブページ１つの処理.
 */

#pragma once

class CChildFrame;

struct NewChildFrameData {
	HWND	hWndParent;
	CString	strURL;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefresh;
	bool	bActive;
	function<void (CChildFrame*)>	funcCallAfterCreated;

	NewChildFrameData(HWND Parent) : 
		hWndParent(Parent), dwDLCtrl(-1), dwExStyle(-1), dwAutoRefresh(0), bActive(false)
	{	}
};


/////////////////////////////////////////////////////////////
// CChildFrame

class CChildFrame
{
public:
	/// スレッドを立ててCChildFrameのインスタンスを作る
	static void	AsyncCreate(NewChildFrameData& data);

	void	SetThreadRefCount(int* pCount);
	HWND	CreateEx(HWND hWndParent);
	void	Navigate2(LPCTSTR lpszURL);

	DWORD	GetExStyle() const;
	void	SetExStyle(DWORD dwStyle);
	void	SetDLCtrl(DWORD dwDLCtrl);
	void	SetMarshalDLCtrl(DWORD dwDLCtrl);
	void	SetAutoRefreshStyle(DWORD dwAutoRefresh);
	void	SaveSearchWordflg(bool bSave);
	void	SetSearchWordAutoHilight(const CString& str, bool bAutoHilight);

	CComPtr<IWebBrowser2>	GetIWebBrowser();
	CComPtr<IWebBrowser2>	GetMarshalIWebBrowser();
	CString	GetLocationURL();

private:
	CChildFrame();
	~CChildFrame();

	class Impl;
	Impl*	pImpl;
};











