/**
 *	@file	MtlProfile.h
 *	@brief	MTL : プロファイラル.
 */
// MTL Version 0.03
// Copyright (C) 2000 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Last updated: August 16, 2000

#ifndef __MTLPROFILE_H__
#define __MTLPROFILE_H__

#pragma once

#include <stdafx.h>
#include <functional>
#include <iterator>
#include <algorithm>
#include <atlctrls.h>
#include <stdlib.h>
#include "Misc.h"
#include "MtlMisc.h"
#include "IniFile.h"

// #define _MTL_PROFILE_EXTRA_TRACE


namespace MTL {


using Misc::CRegKey;	// using ATL::CRegKey;		//+++ atl3対策でラッパーを通すようにした.
using ATL::CSimpleArray;
using WTL::CString;
using WTL::CReBarCtrl;
using WTL::CToolBarCtrl;




/////////////////////////////////////////////////////////////////////////////
// CProfileBinary, an adaptor

template <class _Profile>
class CProfileBinary {
public:
	explicit CProfileBinary(_Profile &__x) : profile(&__x) { }


private:
	// Operations
	LONG SetValue(LPBYTE pValue, LPCTSTR lpszValueName, UINT nBytes)
	{
		// convert to string and write out
		LPTSTR lpsz = new TCHAR[nBytes * 2 + 1];

		for (UINT i = 0; i < nBytes; i++) {
			lpsz[i * 2] 	= (TCHAR) ( (pValue[i] & 0x0F) + 'A' ); 			//low nibble
			lpsz[i * 2 + 1] = (TCHAR) ( ( (pValue[i] >> 4) & 0x0F ) + 'A' );	//high nibble
		}

		lpsz[i * 2] = 0;

		LONG   lRet = profile->SetValue(lpsz, lpszValueName);
		delete[] lpsz;
		return lRet;
	}


	LONG QueryValue(BYTE **ppValue, LPCTSTR lpszValueName, UINT *pBytes)
	{
		ATLASSERT(ppValue != NULL);
		ATLASSERT(pBytes != NULL);
		*ppValue = NULL;
		*pBytes  = 0;

		enum { SIZE = 4096 };
		TCHAR	szT[SIZE + 1];
		ZeroMemory(szT, sizeof szT);	//+++
		DWORD	dwCount = SIZE;
		LONG	lRet	  = profile->QueryValue(szT, lpszValueName, &dwCount);

		if (lRet != ERROR_SUCCESS)
			return lRet;

		ATLASSERT(::lstrlen(szT) % 2 == 0);
		int   nLen	  = ::lstrlen(szT);
		*pBytes  = nLen / 2;
		*ppValue = new BYTE[*pBytes];

		for (int i = 0; i < nLen; i += 2) {
			(*ppValue)[i / 2] = (BYTE) ( ( (szT[i + 1] - 'A') << 4 ) + (szT[i] - 'A') );
		}

		return lRet;
	}


private:	//protected:
	_Profile *profile;
};



// Specialization for Misc::CRegKey
template <>
class CProfileBinary<Misc::CRegKey> {
public:
	explicit CProfileBinary(Misc::CRegKey &__x) : regkey(&__x) { }

private:
	// Operations
	LONG SetValue(LPBYTE lpValue, LPCTSTR lpszValueName, UINT nBytes)
	{
		ATLASSERT(lpValue != NULL);
		ATLASSERT(regkey->m_hKey != NULL);
		return ::RegSetValueEx(regkey->m_hKey, lpszValueName, NULL, REG_BINARY, lpValue, nBytes);
	}


	LONG QueryValue(BYTE **ppValue, LPCTSTR lpszValueName, UINT *pBytes)
	{
		ATLASSERT(regkey->m_hKey != NULL);

		DWORD dwType, dwCount;
		LONG  lResult = ::RegQueryValueEx(regkey->m_hKey, (LPTSTR) lpszValueName, NULL, &dwType, NULL, &dwCount);
		*pBytes = dwCount;

		if (lResult == ERROR_SUCCESS) {
			ATLASSERT(dwType == REG_BINARY);
			*ppValue = new BYTE[*pBytes];
			lResult  = ::RegQueryValueEx(regkey->m_hKey, (LPTSTR) lpszValueName, NULL, &dwType, *ppValue, &dwCount);
		}

		if (lResult != ERROR_SUCCESS) {
			delete[] *ppValue;
			*ppValue = NULL;
		}

		return lResult;
	}


private:
	Misc::CRegKey *regkey;
};



template <class _Profile>
inline CProfileBinary<_Profile> MtlProfileBinary(_Profile &__x)
{
	return CProfileBinary<_Profile>(__x);
}



/////////////////////////////////////////////////////////////////////////////
// CWindowPlacement

class CWindowPlacement : public WINDOWPLACEMENT {
public:
	// Constructors
	CWindowPlacement()
	{
		length = sizeof (WINDOWPLACEMENT);
		// random filled
	}


	CWindowPlacement(const WINDOWPLACEMENT &srcWndpl)
	{
		*(WINDOWPLACEMENT *) this = srcWndpl;
	}


	// Profile Operations
	template <class _Profile>
	void WriteProfile( _Profile &__profile, const CString &strPrefix = _T("wp.") ) const
	{
		MTLVERIFY(__profile.SetValue( flags 				, strPrefix + _T("flags")					) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( showCmd				, strPrefix + _T("showCmd") 				) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( ptMinPosition.x		, strPrefix + _T("ptMinPosition.x") 		) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( ptMinPosition.y		, strPrefix + _T("ptMinPosition.y") 		) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( ptMaxPosition.x		, strPrefix + _T("ptMaxPosition.x") 		) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( ptMaxPosition.y		, strPrefix + _T("ptMaxPosition.y") 		) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rcNormalPosition.left , strPrefix + _T("rcNormalPosition.left")	) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rcNormalPosition.top	, strPrefix + _T("rcNormalPosition.top")	) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rcNormalPosition.right, strPrefix + _T("rcNormalPosition.right")	) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rcNormalPosition.bottom,strPrefix + _T("rcNormalPosition.bottom") ) == ERROR_SUCCESS);
	}


	template <class _Profile>
	bool GetProfile( _Profile &__profile, const CString &strPrefix = _T("wp.") )
	{
		DWORD dwFlags = 0, dwShowCmd = 0,
			  dwMinX  = 0, dwMinY	 = 0, dwMaxX  = 0, dwMaxY	= 0,
			  dwLeft  = 0, dwTop	 = 0, dwRight = 0, dwBottom = 0;

		if (  __profile.QueryValue( dwFlags  , strPrefix + _T("flags")					) == ERROR_SUCCESS
		   && __profile.QueryValue( dwShowCmd, strPrefix + _T("showCmd")				) == ERROR_SUCCESS
		   && __profile.QueryValue( dwMinX	 , strPrefix + _T("ptMinPosition.x")		) == ERROR_SUCCESS
		   && __profile.QueryValue( dwMinY	 , strPrefix + _T("ptMinPosition.y")		) == ERROR_SUCCESS
		   && __profile.QueryValue( dwMaxX	 , strPrefix + _T("ptMaxPosition.x")		) == ERROR_SUCCESS
		   && __profile.QueryValue( dwMaxY	 , strPrefix + _T("ptMaxPosition.y")		) == ERROR_SUCCESS
		   && __profile.QueryValue( dwLeft	 , strPrefix + _T("rcNormalPosition.left")	) == ERROR_SUCCESS
		   && __profile.QueryValue( dwTop	 , strPrefix + _T("rcNormalPosition.top")	) == ERROR_SUCCESS
		   && __profile.QueryValue( dwRight  , strPrefix + _T("rcNormalPosition.right") ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwBottom , strPrefix + _T("rcNormalPosition.bottom")) == ERROR_SUCCESS)
		{
			flags					= dwFlags;
			showCmd 				= dwShowCmd;
			ptMinPosition.x 		= dwMinX;
			ptMinPosition.y 		= dwMinY;
			ptMaxPosition.x 		= dwMaxX;
			ptMaxPosition.y 		= dwMaxY;
			rcNormalPosition.left	= dwLeft;
			rcNormalPosition.top	= dwTop;
			rcNormalPosition.right	= dwRight;
			rcNormalPosition.bottom = dwBottom;
			return true;
		}

		return false;
	}


private:
	// Additional Operations
	void operator =(const WINDOWPLACEMENT &srcWndpl)
	{
		*(WINDOWPLACEMENT *) this = srcWndpl;
	}


public:
	// Helper Operations
	bool IsInsideScreen() const
	{
		#ifndef  SM_CMONITORS
		 #define SM_XVIRTUALSCREEN	  76
		 #define SM_YVIRTUALSCREEN	  77
		 #define SM_CXVIRTUALSCREEN   78
		 #define SM_CYVIRTUALSCREEN   79
		 #define SM_CMONITORS		  80
		#endif

		// I separate the code with multi or not, so I guess no need to install multimon.h's stub
		int nMonitorCount = ::GetSystemMetrics(SM_CMONITORS);

		// if win95, it will fail and return 0.
		if (nMonitorCount > 0) {	// multi monitor
			int   left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
			int   top  = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
			CRect rcMonitor(
						left,
						top,
						left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
						top  + ::GetSystemMetrics(SM_CYVIRTUALSCREEN) );

			if ( MtlIsCrossRect(rcMonitor, rcNormalPosition) )
				return true;
			else
				return false;

		} else {
			CRect rcScreen( 0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN) );

			if ( MtlIsCrossRect(rcScreen, rcNormalPosition) )
				return true;
			else
				return false;
		}
	}


private:
	// Binary Profile Operations
	template <class _ProfileBinary>
	bool WriteProfileBinary( _ProfileBinary &__profile, const CString &strValueName = _T("WindowPlacement") ) const
	{
		LONG lRet = __profile.SetValue( (LPBYTE) this, strValueName, sizeof (WINDOWPLACEMENT) );

		if (lRet == ERROR_SUCCESS)
			return true;
		else
			return false;
	}


	template <class _ProfileBinary>
	bool GetProfileBinary( _ProfileBinary &__profile, const CString &strValueName = _T("WindowPlacement") )
	{
		LPBYTE pData;
		UINT   nBytes;
		LONG   lRet = __profile.QueryValue(&pData, strValueName, &nBytes);

		if (lRet != ERROR_SUCCESS)
			return false;

		::memcpy(static_cast<WINDOWPLACEMENT *>(this), pData, nBytes);
		delete[] pData;

		return true;
	}

};



/////////////////////////////////////////////////////////////////////////////
// CLogFont
static const LOGFONT _lfDefault = {
	0,					0,					 0, 			  0,			 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET,	//DEFAULT_CHARSET,
	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("")
};



class CLogFont : public LOGFONT {	// Fixed by DOGSTORE for serialization of 'lfCharSet', Thanks
public:
	// Constructor
	CLogFont()
	{
		// random filled
	}


	CLogFont(const LOGFONT &srcLogFont)
	{
		*(LOGFONT *) this = srcLogFont;
	}


	void InitDefault()
	{
		*(LOGFONT *) this = _lfDefault;
	}


	// Profile methods
	template <class _Profile>
	void WriteProfile( _Profile &__profile, HDC hDC = NULL, const CString &strPrefix = _T("lf.") ) const
	{
		MTLVERIFY(__profile.SetStringUW(lfFaceName, strPrefix  + _T("lfFaceName" ) ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( GetPointSizeFromHeight(lfHeight, hDC), strPrefix + _T("lfPointSize") ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( lfWeight, strPrefix	 + _T("lfWeight"   ) ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( lfItalic, strPrefix	 + _T("lfItalic"   ) ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( lfUnderline, strPrefix + _T("lfUnderLine") ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( lfStrikeOut, strPrefix + _T("lfStrikeOut") ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( lfCharSet, strPrefix	 + _T("lfCharSet"  ) ) == ERROR_SUCCESS);
	}


	template <class _Profile>
	bool GetProfile( _Profile &__profile, HDC hDC = NULL, const CString &strPrefix = _T("lf.") )
	{
		DWORD dwFaceSize = LF_FACESIZE, dwPointSize = 0, dwWeight = 0, dwItalic = 0, dwUnderline = 0, dwStrikeOut = 0, dwCharSet = 0;

		CString	strLfFaceName = __profile.GetStringUW(strPrefix + _T("lfFaceName"));	//+++ UNICODE文字対応
		if (  /* __profile.QueryString(lfFaceName , strPrefix + _T("lfFaceName"), &dwFaceSize) == ERROR_SUCCESS */
		      ! strLfFaceName.IsEmpty()													//+++
		   && __profile.QueryValue( dwPointSize, strPrefix + _T("lfPointSize") ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwWeight   , strPrefix + _T("lfWeight")    ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwItalic   , strPrefix + _T("lfItalic")    ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwUnderline, strPrefix + _T("lfUnderLine") ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwStrikeOut, strPrefix + _T("lfStrikeOut") ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwCharSet  , strPrefix + _T("lfCharSet")   ) == ERROR_SUCCESS)
		{
			::lstrcpyn(lfFaceName, strLfFaceName, dwFaceSize);							//+++
			lfHeight	= GetHeightFromPointSize(dwPointSize, hDC);
			lfWeight	= dwWeight;
			lfItalic	= (BYTE) dwItalic;
			lfUnderline = (BYTE) dwUnderline;
			lfStrikeOut = (BYTE) dwStrikeOut;
			lfCharSet	= (BYTE) dwCharSet;
			return true;
		}

		return false;
	}


	// Additional Operations
	void operator =(const LOGFONT &srcLogFont)
	{
		*(LOGFONT *) this = srcLogFont;
	}


	bool operator ==(const LOGFONT &lf) const
	{
		return (  lfHeight			== lf.lfHeight
			   && lfWidth			== lf.lfWidth
			   && lfEscapement		== lf.lfEscapement
			   && lfOrientation 	== lf.lfOrientation
			   && lfWeight			== lf.lfWeight
			   && lfItalic			== lf.lfItalic
			   && lfUnderline		== lf.lfUnderline
			   && lfStrikeOut		== lf.lfStrikeOut
			   && lfCharSet 		== lf.lfCharSet
			   && lfOutPrecision	== lf.lfOutPrecision
			   && lfClipPrecision	== lf.lfClipPrecision
			   && lfQuality 		== lf.lfQuality
			   && lfPitchAndFamily	== lf.lfPitchAndFamily
			   && ::lstrcmp(lfFaceName, lf.lfFaceName) == 0);
	}


	bool operator !=(const LOGFONT &lf) const
	{
		return !(*this == lf);
	}


private:
	// Helper Operations
	static LONG GetHeightFromPointSize(LONG nPointSize, HDC hDC = NULL)
	{ // cf.WTL::CFont::CreatePointFontIndirect
		HDC   hDC1	= (hDC != NULL) ? hDC : ( ::GetDC(NULL) );

		// convert nPointSize to logical units based on hDC
		POINT pt = {0, 0};

		//pt.y = ::GetDeviceCaps(hDC1, LOGPIXELSY) * nPointSize;
		//pt.y /= 72;	 // 72 points/inch, 10 decipoints/point
		pt.y = ::MulDiv(::GetDeviceCaps(hDC1, LOGPIXELSY), nPointSize, 72);
		::DPtoLP(hDC1, &pt, 1);
		POINT ptOrg = { 0, 0 };
		::DPtoLP(hDC1, &ptOrg, 1);

		if (hDC == NULL)
			::ReleaseDC(NULL, hDC1);

		return -abs(pt.y - ptOrg.y);
	}


	static LONG GetPointSizeFromHeight(LONG nHeight, HDC hDC = NULL)
	{
		HDC  hDC1		= (hDC != NULL) ? hDC : ( ::GetDC(NULL) );

		LONG nPointSize = ::MulDiv( -nHeight, 72, ::GetDeviceCaps(hDC1, LOGPIXELSY) );

		if (hDC == NULL)
			::ReleaseDC(NULL, hDC1);

		return nPointSize;
	}
};


/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileMainFrameState

template <class _Profile>
void	MtlWriteProfileMainFrameState( _Profile &__profile, HWND hWnd, const CString strPrefix = _T("frame.") )
{
	ATLASSERT( ::IsWindow(hWnd) );

	CWindowPlacement wndpl;
	MTLVERIFY( ::GetWindowPlacement(hWnd, &wndpl) );
	wndpl.WriteProfile(__profile, strPrefix);
}


//+++ 返値を追加: 0=最小化 1=通常 2=最大化 3:フル (とりあえずのtray復帰対策用...あとで変更予定)
template <class _Profile>
int 	MtlGetProfileMainFrameState(
		_Profile &		__profile,
		HWND			hWnd,
		int 			nCmdShow,
		bool			bShowNoMinimized = true,
		const CString	strPrefix		 = _T("frame.") )
{
	ATLASSERT( ::IsWindow(hWnd) );

	CWindowPlacement wndpl;

	if ( wndpl.GetProfile(__profile, strPrefix) && wndpl.IsInsideScreen() ) {
		if (bShowNoMinimized) {
			if (wndpl.showCmd == SW_SHOWMINIMIZED) {
				if (wndpl.flags & WPF_RESTORETOMAXIMIZED)
					wndpl.showCmd = SW_SHOWMAXIMIZED;
				else
					wndpl.showCmd = SW_SHOWNORMAL;
			}
		}
		nCmdShow = wndpl.showCmd;		//+++
		MTLVERIFY( ::SetWindowPlacement(hWnd, &wndpl) );
	} else {
		::ShowWindow(hWnd, nCmdShow);
	}
  #if 1 //+++ とりあえずのtray復帰対策
	if (nCmdShow == SW_SHOWMAXIMIZED)
		return 2;
	//if (nCmdShow == SW_SHOWMINIMIZED)
	//	return 0;
	return 1;
  #endif
}




/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileChildFrameState

template <class _Profile>
void MtlWriteProfileChildFrameState(
		_Profile&		__profile,
		HWND			hWnd,
		const CString	strPrefix = _T("frame.") )
{
	ATLASSERT( ::IsWindow(hWnd) );

	CWindowPlacement wndpl;
	MTLVERIFY( ::GetWindowPlacement(hWnd, &wndpl) );
	wndpl.WriteProfile(__profile, strPrefix);
}


template <class _Profile>
void MtlGetProfileChildFrameState(
		_Profile &		__profile,
		HWND			hWnd,
		int 			nCmdShowDefault,
		bool			bShowNoActivate = false,
		const CString	strPrefix		= _T("frame.") )
{
	ATLASSERT( ::IsWindow(hWnd) );
	CWindowPlacement	wndpl;
	if ( wndpl.GetProfile(__profile, strPrefix) && wndpl.IsInsideScreen() ) {
		if (bShowNoActivate) {
			if (wndpl.showCmd == SW_SHOWMINIMIZED)
				wndpl.showCmd = SW_SHOWMINNOACTIVE;
			else {
				wndpl.showCmd = SW_SHOWNOACTIVATE;
			}
		}

		MTLVERIFY( ::SetWindowPlacement(hWnd, &wndpl) );
	} else {
		if (bShowNoActivate)
			nCmdShowDefault = SW_SHOWNOACTIVATE;

		::ShowWindow(hWnd, nCmdShowDefault);
	}
}


/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileRebarBandsState

//+++
inline const CString	dword_ptr_to_str(DWORD_PTR n)
{
	char buf[128];
	buf[0] = 0;
   #ifdef _WIN64
	_ui64toa(DWORD_PTR(n), buf, 10);
   #else
	_ultoa	(DWORD(n), buf, 10);
   #endif
	return CString(buf);
}


template <class _Profile>
void MtlWriteProfileReBarBandsState( _Profile &__profile, CReBarCtrl rebar, const CString &strPrefix = _T("band") )
{
	for (int nIndex = 0;; ++nIndex) {
		CVersional<REBARBANDINFO> rbBand;
		rbBand.fMask = RBBIM_SIZE | RBBIM_STYLE | RBBIM_CHILD;
		if ( !rebar.GetBandInfo(nIndex, &rbBand) )
			break;

		CString strBuff = strPrefix + _T("#");
		HANDLE	hID 	= (HANDLE) ::GetProp( rbBand.hwndChild, _T("Donut_Plugin_ID") );
		if (hID == NULL) {
			strBuff.Append( CWindow(rbBand.hwndChild).GetDlgCtrlID() );
		} else {
			strBuff += dword_ptr_to_str(DWORD_PTR(hID));
			::RemoveProp( rbBand.hwndChild, _T("Donut_Plugin_ID") );
		}

		MTLVERIFY(__profile.SetValue( nIndex, strBuff + _T(".index") ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rbBand.cx, strBuff + _T(".cx") ) == ERROR_SUCCESS);
		MTLVERIFY(__profile.SetValue( rbBand.fStyle, strBuff + _T(".fStyle") ) == ERROR_SUCCESS);
	}
}



struct CReBarBandInfo {
	UINT		nIndex; 	// must be filled, cause stable_sort requires CRT
	HWND		hWnd;
	UINT_PTR	nID;
	UINT		fStyle;
	LPTSTR		lpText;
	UINT		cx; 		// can be 0
};



template <class _MainFrame>
class __AddBand : public std::unary_function<const CReBarBandInfo &, void> {
public:
	explicit __AddBand(_MainFrame &__m) : mainFrame(&__m) { }
	void operator ()(const CReBarBandInfo &arg)
	{
		_MainFrame::AddSimpleReBarBandCtrl(mainFrame->m_hWndToolBar, arg.hWnd, arg.nID, arg.lpText, arg.fStyle, arg.cx, FALSE);
	}

protected:
	_MainFrame *mainFrame;
};



class __ShowBand : public std::unary_function<const CReBarBandInfo &, void> {
public:
	explicit __ShowBand(HWND hWndReBar) : rebar(hWndReBar) , nIndex(0) { }
	void operator ()(const CReBarBandInfo &arg)
	{
		rebar.ShowBand(nIndex++, (arg.fStyle & RBBS_HIDDEN) == 0 );
	}

protected:
	CReBarCtrl rebar;
	int 	   nIndex;
};



struct __ReBarBandInfoIndexOrdering : public std::binary_function<const CReBarBandInfo &, const CReBarBandInfo &, bool> {
	bool operator ()(const CReBarBandInfo &arg1, const CReBarBandInfo &arg2) const
	{
		return arg1.nIndex < arg2.nIndex;
	}
};



template <class _Profile, class _MainFrame>
bool MtlGetProfileReBarBandsState( CReBarBandInfo *__first, CReBarBandInfo *__last, _Profile __profile, _MainFrame &__mainFrame, const CString &strPrefix = _T("band") )
{
	if (__first == __last)
		return false;

	CSimpleArray<CReBarBandInfo>	tmp;

	for (; __first != __last; ++__first) {
		CString strBuff = strPrefix + _T("#");
		strBuff += dword_ptr_to_str(__first->nID);
		DWORD	dwIndex = 0, dwCx = 0, dwStyle = 0;

		if (  __profile.QueryValue( dwIndex , strBuff + _T(".index" ) ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwCx	, strBuff + _T(".cx"	) ) == ERROR_SUCCESS
		   && __profile.QueryValue( dwStyle , strBuff + _T(".fStyle") ) == ERROR_SUCCESS)
		{
			__first->nIndex = dwIndex;
			__first->cx 	= dwCx;
			__first->fStyle = dwStyle;
		}

		tmp.Add(*__first);		// even if value not found, user's supplements used as default
	}

	if (tmp.GetSize() == 0) 	// need
		return false;

	// I want use std::stable_sort for users, but it requires CRT.
	// I think easy sort algorithms have to be added to ATL/WTL.
	std::sort	 ( &tmp[0], &tmp[0] + tmp.GetSize(), __ReBarBandInfoIndexOrdering() 		);
	std::for_each( &tmp[0], &tmp[0] + tmp.GetSize(), __AddBand<_MainFrame>(__mainFrame) 	);
	std::for_each( &tmp[0], &tmp[0] + tmp.GetSize(), __ShowBand(__mainFrame.m_hWndToolBar)	);

	return true;
}



/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileStatusBarState

template <class _Profile>
bool MtlGetProfileStatusBarState(
	_Profile &		__profile,
	HWND			hWndStatusBar,
	BOOL &			bVisible,
	const CString&	strPrefix = _T("statusbar.") )
{
	ATLASSERT( ::IsWindow(hWndStatusBar) );
	DWORD	dwValue = 0;
	LONG	lRet = __profile.QueryValue( dwValue, strPrefix + _T("Visible") );
	if (lRet != ERROR_SUCCESS) {
		bVisible = TRUE;
		return false;
	}

	bVisible = (dwValue == 1) /*? TRUE : FALSE*/;
	MTLVERIFY( ::ShowWindow(hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE) );
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// Global functions for Profile

// app.exe -> APP.INI
// App.exe -> App.ini

inline void MtlIniFileNameInit( LPTSTR lpszIniFileName, DWORD nSize, LPCTSTR lpszExtText = _T(".INI") )
{
	// cf.ATL::AtlModuleLoadTypeLib (we can't use _tsplitpath that requires CRT)
	::GetModuleFileName(_Module.GetModuleInstance(), lpszIniFileName, nSize);
	LPTSTR	lpszExt = NULL;
	LPTSTR	lpsz;
	for ( lpsz = lpszIniFileName; *lpsz != NULL; lpsz = ::CharNext(lpsz) ) {
		if ( *lpsz == _T('.') )
			lpszExt = lpsz;
	}

	if (lpszExt == NULL)
		lpszExt = lpsz;

	ATLASSERT(::lstrlen(lpszExtText) == 4);
	::lstrcpy(lpszExt, lpszExtText);

  #ifdef _MTL_PROFILE_EXTRA_TRACE
	ATLTRACE2(atlTraceUI, 0, _T("MtlIniFileNameInit : %s\n"), lpszIniFileName);
  #endif
}



/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileString

template <class _InputIterString, class _Profile>
bool MtlWriteProfileString(
		_InputIterString	__first,
		_InputIterString	__last,
		_Profile &			__profile,
		const CString & 	strPrefix = _T("string"),
		DWORD				nStartSuffix = 0)
{
	// set new values
	for (int n = nStartSuffix; __first != __last; ++__first, ++n) {
		CString 	strBuff = strPrefix;
		strBuff.Append(n);
		MTLVERIFY(__profile.SetStringValue(strBuff, *__first) == ERROR_SUCCESS);
	}

	return true;
}


template <class _Profile, class _OutputIterString>
bool MtlGetProfileString(
		_Profile &			__profile,
		_OutputIterString	__result,
		const CString & 	strPrefix	 = _T("string"),
		DWORD				nStartSuffix = 0,
		DWORD				nLastSuffix  = -1)
{
	_OutputIterString	__resultSrc = __result;
	DWORD				n;
	for (n = nStartSuffix;; ++n) {
		CString strBuff = strPrefix;
		strBuff.Append(n);
		enum { SIZE = 4096 };
		TCHAR	szT[SIZE + 1];
		ZeroMemory(szT, sizeof szT);	//+++
		DWORD	dwCount = SIZE;
		LONG	lRet	= __profile.QueryStringValue(strBuff, szT, &dwCount);

		if (lRet == ERROR_SUCCESS)
			*__result++ = szT;
		else
			break;

		if (n == nLastSuffix)
			break;
	}

	if (n == nStartSuffix)
		return false;
	else
		return true;
}



/////////////////////////////////////////////////////////////////////////////
// MtlWrite&GetProfileTBBtns
// This will save a bitmap index, so you can't change it by toolbar resource editor.
// If you want to add a new button for your new app's version, you have to add it last of all.

template <class _Profile>
void MtlWriteProfileTBBtns( _Profile &__profile, CToolBarCtrl toolbar, const CString &strPrefix = _T("button") )
{
	int 	iBtn = toolbar.GetButtonCount();
	MTLVERIFY(__profile.SetValue( iBtn, strPrefix + _T(".count") ) == ERROR_SUCCESS);
	for (int n = 0; n < iBtn; ++n) {
		TBBUTTON tbBtn;
		MTLVERIFY( toolbar.GetButton(n, &tbBtn) );
		CString  strBuff = strPrefix;
		strBuff.Append(n);
		int 	 nIndex;
		if (tbBtn.fsStyle & TBSTYLE_SEP)
			nIndex = -1;
		else
			nIndex = tbBtn.iBitmap;
		MTLVERIFY(__profile.SetValue( nIndex, strBuff + _T(".iBitmap") ) == ERROR_SUCCESS);
	}
}


template <class _Profile>
bool MtlGetProfileTBBtns( _Profile &__profile, CSimpleArray<int> &arrBmpIndex, const CString &strPrefix = _T("button") )
{
	// load profile
	DWORD dwCount;
	LONG  lRet = __profile.QueryValue( dwCount, strPrefix + _T(".count") );

	if (lRet != ERROR_SUCCESS)
		return false;			//not found

	// insert buttons
	for (DWORD n = 0; n < dwCount; ++n) {
		CString strBuff = strPrefix;
		strBuff.Append(n);
		DWORD	dwBitmap;

		if (__profile.QueryValue( dwBitmap, strBuff + _T(".iBitmap") ) == ERROR_SUCCESS) {
			int iBmpIndex = dwBitmap;
			arrBmpIndex.Add(iBmpIndex);
		}
	}

	return true;
}


template <class _Profile>
bool MtlGetProfileTBBtns( _Profile &__profile, CToolBarCtrl toolbar, const CString &strPrefix = _T("button") )
{
	ATLASSERT( toolbar.IsWindow() );

	// load profile
	DWORD	  dwCount;
	LONG	  lRet	 = __profile.QueryValue( dwCount, strPrefix + _T(".count") );
	if (lRet != ERROR_SUCCESS)
		return false;		//not found

	int 		iBtn	= toolbar.GetButtonCount();
	TBBUTTON*	pTBBtn	= (TBBUTTON *) _alloca( iBtn * sizeof (TBBUTTON) );

	// save original tbbuttons
	for (int i = 0; i < iBtn; ++i) {
		TBBUTTON tbBtn;
		MTLVERIFY( toolbar.GetButton(i, &tbBtn) );
		pTBBtn[i] = tbBtn;
	}

	// clean up previous toolbar buttons
	while ( toolbar.DeleteButton(0) )
		;

	// insert buttons
	for (DWORD n = 0; n < dwCount; ++n) {
		CString strBuff = strPrefix;
		strBuff.Append(n);
		DWORD	dwBitmap;

		if (__profile.QueryValue( dwBitmap, strBuff + _T(".iBitmap") ) == ERROR_SUCCESS) {
			if (dwBitmap == -1) {	// separator
				TBBUTTON	tbBtn;
				tbBtn.iBitmap	= 8;
				tbBtn.idCommand = 0;
				tbBtn.fsState	= 0;
				tbBtn.fsStyle	= TBSTYLE_SEP;
				tbBtn.dwData	= 0;
				tbBtn.iString	= 0;
				MTLVERIFY( toolbar.InsertButton(toolbar.GetButtonCount(), &tbBtn) );
			} else {
				int i;
				for (i = 0; i < iBtn; ++i) {
					if (!(pTBBtn[i].fsStyle & TBSTYLE_SEP) && pTBBtn[i].iBitmap == dwBitmap) {
						MTLVERIFY( toolbar.InsertButton(toolbar.GetButtonCount(), &pTBBtn[i]) );
						break;
					}
				}
				ATLASSERT(i != iBtn);
			}
		}
	}

	return true;
}


}	//namespace MTL



#endif	// __MTLPROFILE_H__
