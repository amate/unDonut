/**
 *	@file	ChildFrame.h
 *	@brief	�^�u�y�[�W�P�̏���.
 */

#pragma once

#include <boost/serialization/serialization.hpp>
#include "CustomSerializeClass.h"

class CChildFrame;

struct NewChildFrameData {
	HWND	hWndParent;
	CString	strTitle;	// optional
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
	CString	strNewWindowURL;

	bool	bDelayLoad;

	NewChildFrameData(HWND Parent) : 
		hWndParent(Parent), dwDLCtrl(-1), dwExStyle(-1), dwAutoRefresh(0), 
		bActive(false), bLink(false), bAutoHilight(false), dwThreadIdFromNewWindow(0), bDelayLoad(false)
	{	}

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strTitle & strURL & TravelLogFore & TravelLogBack & dwDLCtrl & dwExStyle & dwAutoRefresh;
		ar & bActive & bLink & bAutoHilight & searchWord & dwThreadIdFromNewWindow & strNewWindowURL & bDelayLoad;
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
	DWORD	dwExStyle;			// TabList.donutTabList�p
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

	/// �X���b�h�𗧂Ă�CChildFrame�̃C���X�^���X�����
	static void	AsyncCreate(NewChildFrameData& data);

	HWND	CreateChildFrame(const NewChildFrameData& data, int* pThreadRefCount);
	void	InitChildFrame(const NewChildFrameData& data);

	HWND	GetHwnd() const;

	CString GetSelectedTextLine();

private:
	class Impl;
	Impl*	pImpl;
};











