/**
 *	@file	ChildFrame.h
 *	@brief	タブページ１つの処理.
 */

#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

class CChildFrame;

namespace boost {
  namespace serialization {

	template <class Archive>
	inline void save(Archive& ar, const WTL::CString& s, const unsigned int version)
	{
		static_cast<void>(version);

		const std::wstring ss(s);
		ar & ss;
	}

	template<class Archive>
	inline void load(Archive& ar, WTL::CString& s, const unsigned int version) 
	{
		static_cast<void>(version);

		std::wstring ss;
		ar & ss;
		s = ss.c_str();
	}


    template <class Archive>
	inline void serialize(Archive& ar, WTL::CString& s, const unsigned int version)
	{
		boost::serialization::split_free(ar, s, version);
	}
  }
}


struct NewChildFrameData {
	HWND	hWndParent;
	CString	strURL;
	vector<std::pair<CString, CString> > TravelLogFore;
	vector<std::pair<CString, CString> > TravelLogBack;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefresh;
	bool	bActive;
	bool	bLink;

	bool	bAutoHilight;
	CString searchWord;

	DWORD	dwThreadIdFromNewWindow;

	NewChildFrameData(HWND Parent) : 
		hWndParent(Parent), dwDLCtrl(-1), dwExStyle(-1), dwAutoRefresh(0), 
		bActive(false), bLink(false), bAutoHilight(false), dwThreadIdFromNewWindow(0)
	{	}

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strURL & TravelLogFore & TravelLogBack & dwDLCtrl & dwExStyle & dwAutoRefresh;
		ar & bActive & bLink & bAutoHilight & searchWord & dwThreadIdFromNewWindow;
#ifdef WIN64
		ar & reinterpret_cast<__int64&>(hWndParent);
#else
		ar & reinterpret_cast<__int32&>(hWndParent);
#endif
	}
};

struct NavigateChildFrame {
	CString strURL;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;
	DWORD	dwAutoRefresh;

	NavigateChildFrame() : dwDLCtrl(-1), dwExStyle(-1), dwAutoRefresh(0) {	}

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strURL & dwDLCtrl & dwExStyle & dwAutoRefresh;
	}
};

#define CHILDFRAMEDATAONCLOSESHAREDMEMNAME	_T("DonutChildFrameDataOnCloseShaedMemName")
#define NOWCHILDFRAMEDATAONCLOSESHAREDMEMNAME _T("DonutNowChildFrameDataOnCloseSharedMemName")

struct ChildFrameDataOnClose {
	CString strURL;
	CString strTitle;
	vector<std::pair<CString, CString> > TravelLogFore;
	vector<std::pair<CString, CString> > TravelLogBack;
	DWORD	dwDLCtrl;
	DWORD	dwExStyle;			// TabList.xml用
	DWORD	dwAutoRefreshStyle;	// 

	ChildFrameDataOnClose() : dwDLCtrl(0), dwExStyle(0), dwAutoRefreshStyle(0)
	{	}

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strURL & strTitle & TravelLogFore & TravelLogBack & dwDLCtrl & dwExStyle & dwAutoRefreshStyle;
	}
};

#define FINDKEYWORDATASHAREDMEMNAME	_T("DonutFindKeywordDataSharedMemName")

struct FindKeywordData
{
	CString strKeyword;
	bool	bFindDown;
	long	Flags;

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strKeyword & bFindDown & Flags;
	}
};


#define MOUSEGESTUREDATASHAREDMEMNAME	_T("DonutMouseGestureDataSharedMemName")

struct MouseGestureData
{
	HWND	hwnd;
	WPARAM	wParam;
	LPARAM	lParam;
	bool	bCursorOnSelectedText;
	CString strSelectedTextLine;

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & wParam & lParam & bCursorOnSelectedText & strSelectedTextLine;

#ifdef WIN64
		ar & reinterpret_cast<__int64&>(hwnd);
#else
		ar & reinterpret_cast<__int32&>(hwnd);
#endif
	}
};


/////////////////////////////////////////////////////////////
// CChildFrame

class CChildFrame
{
public:
	CChildFrame();
	~CChildFrame();

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
	void	SetSearchWordAutoHilight(const CString& str, bool bAutoHilight);
	void	SetTravelLog(const vector<std::pair<CString, CString> >& fore, const vector<std::pair<CString, CString> >& back);
	void	SetThreadIdFromNewWindow2(DWORD dwThreadId);

	CComPtr<IWebBrowser2>	GetIWebBrowser();
	CComPtr<IWebBrowser2>	GetMarshalIWebBrowser();
	CString	GetLocationURL();
	CString GetTitle();
	CString GetSelectedTextLine();

private:
	class Impl;
	Impl*	pImpl;
};











