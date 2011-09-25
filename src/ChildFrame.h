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

struct ChildFrameDataOnClose {
	CString strURL;
	CString strTitle;
	vector<std::pair<CString, CString> > TravelLogFore;
	vector<std::pair<CString, CString> > TravelLogBack;
	DWORD	dwDLCtrl;

	ChildFrameDataOnClose() : dwDLCtrl(0)
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

	HWND	GetHwnd() const;
	DWORD	GetExStyle() const;
	void	SetExStyle(DWORD dwStyle);
	void	SetDLCtrl(DWORD dwDLCtrl);
	void	SetMarshalDLCtrl(DWORD dwDLCtrl);
	void	SetAutoRefreshStyle(DWORD dwAutoRefresh);
	void	SaveSearchWordflg(bool bSave);
	void	SetSearchWordAutoHilight(const CString& str, bool bAutoHilight);
	void	SetTravelLog(const vector<std::pair<CString, CString> >& fore, const vector<std::pair<CString, CString> >& back);

	CComPtr<IWebBrowser2>	GetIWebBrowser();
	CComPtr<IWebBrowser2>	GetMarshalIWebBrowser();
	CString	GetLocationURL();

private:
	CChildFrame();
	~CChildFrame();

	class Impl;
	Impl*	pImpl;
};











