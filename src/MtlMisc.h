/**
 *	@file	MtlMisc.h
 *	@brief	MTL : 雑多
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

#ifndef __MTLMISC_H__
#define __MTLMISC_H__

//#ifndef __ATLCTRLS_H__
//	#error mtlmisc.h requires atlctrls.h to be included first
//#endif

//#ifndef _SHLOBJ_H_
//	#error mtlmisc.h requires shlobj.h to be included first
//#endif

#pragma once

#include "MtlBase.h"
#include "DonutPFunc.h"


namespace MTL {


/////////////////////////////////////////////////////////////////////////////
// Tips

inline bool MtlIsCrossRect(const WTL::CRect &rc1, const WTL::CRect &rc2)
{
	CRect rcCross = rc1 & rc2;
	//+++ return rcCross.IsRectEmpty() ? false : true;
	return rcCross.IsRectEmpty() == 0;
}



inline int MtlComputeWidthOfText(const CString &strText, HFONT hFont)
{
	if ( strText.IsEmpty() )
		return 0;

	CString str = strText;	//+++
	str.Remove(_T('&'));	//+++

	CWindowDC dc(NULL);
	CRect	  rcText(0, 0, 0, 0);
	HFONT	  hOldFont = dc.SelectFont(hFont);
	dc.DrawText(str/*+++strText*/, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
	dc.SelectFont(hOldFont);

	return rcText.Width();
}

inline CRect MtlComputeRectOfText(const CString &strText, HFONT hFont)
{
	if ( strText.IsEmpty() )
		return CRect();

	CString str = strText;	//+++
	str.Remove(_T('&'));	//+++

	CWindowDC dc(NULL);
	CRect	  rcText;
	HFONT	  hOldFont = dc.SelectFont(hFont);
	dc.DrawText(str, -1, &rcText, DT_CALCRECT);
	dc.SelectFont(hOldFont);

	return rcText;
}



/////////////////////////////////////////////////////////////////////////////
// Algorithm

inline bool __MtlIsLastChar(const CString &str, TCHAR ch)
{
	if (str.GetLength() <= 0)
		return false;

  #ifdef _UNICODE
	if (str[str.GetLength() - 1] == ch)
		return true;
	else
		return false;
  #else
	if ( str[str.GetLength() - 1] == ch && !_IsDBCSTrailByte(str, str.GetLength() - 1) )
		return true;
	else
		return false;
  #endif
}


inline TCHAR *Mtl_strstr (const TCHAR *str1, const TCHAR *str2)
{
	TCHAR * 	cp = (TCHAR *) str1;
	TCHAR * 	s1;
	TCHAR * 	s2;

	if (! * str2)
		return ( (TCHAR *) str1 );

	while (*cp) {
		s1 = cp;
		s2 = (TCHAR *) str2;

		/*
		 * MBCS: ok to ++ since doing equality comparison.
		 * [This depends on MBCS strings being "legal".]
		 */
		while ( *s1 && *s2 && (*s1 == *s2) )
			s1++, s2++;

		if (! * s2)
			return (cp);

		cp = ::CharNext(cp);
	}

	return (NULL);
}



// CString::Find is too late...
inline int __MtlFind(LPCTSTR lpszSrc, LPCTSTR lpszSub)
{
	// find first matching substring
	LPTSTR lpsz = Mtl_strstr(lpszSrc, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int) (lpsz - lpszSrc);
}



inline int __MtlFind(const CString &str, int nStart, const CString &strFind)
{
	CString strText = str.Right(str.GetLength() - nStart);
	int 	nIndex	= strText.Find(strFind); //__MtlFind(strText, strFind);

	if (nIndex == -1)
		return -1;
	else {
		return nStart + nIndex;
	}
}



inline int __MtlFindChar(const CString &str, int nStart, TCHAR ch)
{
	CString strText = str.Right(str.GetLength() - nStart);
	int 	nIndex	= strText.Find(ch);

	if (nIndex == -1)
		return -1;
	else {
		ATLASSERT(str[nStart + nIndex] == ch);
		return nStart + nIndex;
	}
}



template <class _InputIter>
bool MtlSearchString(_InputIter __first, _InputIter __last, const CString &str)
{
	if ( str.IsEmpty() )
		return false;

	for (; __first != __last; ++__first) {
		if (str.Find(*__first) != -1)
			return true;
	}

	return false;
}



inline TCHAR Mtl_tolower(TCHAR ch)
{
	return ( ( ch >= _T('A') && ch <= _T('Z') ) ? ( ch - _T('A') + _T('a') ) : ch );
}



inline bool Mtl_istalnum(TCHAR ch)
{
	return ( ch >= _T('a') && ch <= _T('z') )
		   || ( ch >= _T('A') && ch <= _T('Z') ) || ( ch >= _T('0') && ch <= _T('9') );
}



inline bool Mtl_isalpha(TCHAR ch)
{
	return ( ch >= _T('a') && ch <= _T('z') ) || ( ch >= _T('A') && ch <= _T('Z') );
}



//Thanks to http://www.hidecnet.ne.jp/~sinzan/tips/main.htm, and you.
inline bool MtlStrMatchWildCard(LPCTSTR Ptn, LPCTSTR Str)
{
	switch (*Ptn) {
	case _T('\0'):
		return ( *Str == _T('\0') );

	case _T('*'):
		return MtlStrMatchWildCard( Ptn + 1, Str ) || ( *Str != _T('\0') ) && MtlStrMatchWildCard( Ptn, Str + 1 );

	case _T('?'):
		return ( *Str != _T('\0') ) && MtlStrMatchWildCard( Ptn + 1, Str + 1 );

	default:
		return ( Mtl_tolower(*Ptn) == Mtl_tolower(*Str) ) && MtlStrMatchWildCard(Ptn + 1, Str + 1);
	}
}


#if 1	//+++ イテレータを返すバージョンを用意

template <class _InputIter>
_InputIter MtlFindStringWildCard(_InputIter __first, _InputIter __last, const CString &str)
{
	if ( str.IsEmpty() )
		return __last;

	for (; __first != __last; ++__first) {
		CString ptn(*__first);

		if (ptn.Find( _T('*') ) != -1 || ptn.Find( _T('?') ) != -1) {
			if ( MtlStrMatchWildCard(ptn, str) )
				return __first;
		} else {
			if (str == *__first) {
				//				ATLTRACE2(atlTraceGeneral, 4, _T("%s = %s\n"), *__first, str);
				return __first;
			}
		}
	}
	return __last;
}


template <class _InputIter>
bool MtlSearchStringWildCard(_InputIter __first, _InputIter __last, const CString &str)
{
	return MtlFindStringWildCard(__first, __last, str) != __last;
}


#else
template <class _InputIter>
bool MtlSearchStringWildCard(_InputIter __first, _InputIter __last, const CString &str)
{
	if ( str.IsEmpty() )
		return false;

	for (; __first != __last; ++__first) {
		CString ptn(*__first);

		if (ptn.Find( _T('*') ) != -1 || ptn.Find( _T('?') ) != -1) {
			if ( MtlStrMatchWildCard(ptn, str) )
				return true;
		} else {
			if (str == *__first) {
				//				ATLTRACE2(atlTraceGeneral, 4, _T("%s = %s\n"), *__first, str);
				return true;
			}
		}
	}

	return false;
}
#endif



inline void MtlRemoveTrailingBackSlash(CString &str)
{
	if ( __MtlIsLastChar( str, _T('\\') ) )
		str = str.Left(str.GetLength() - 1);
}



inline void MtlRemoveStringHeaderAndFooter(CString &str)
{
	if (str.GetLength() == 0)
		return;

	if ( str.GetAt(0) == _T('\"') && __MtlIsLastChar( str, _T('\"') ) )
		str = str.Mid(1, str.GetLength() - 2);
}



inline void MtlRemoveTrailingSpace(CString &str)
{
	if ( __MtlIsLastChar( str, _T(' ') ) )
		str = str.Left(str.GetLength() - 1);
}



inline void MtlMakeSureTrailingChar(CString &strDirectoryPath, TCHAR ch)
{
	if ( !__MtlIsLastChar(strDirectoryPath, ch) )
		strDirectoryPath += ch;
}



inline void MtlMakeSureTrailingBackSlash(CString &strDirectoryPath)
{
	MtlMakeSureTrailingChar( strDirectoryPath, _T('\\') );
}



inline CString MtlGetDirectoryPath( const CString &strPath, bool bAddBackSlash = false, TCHAR chSep = _T('\\') )
{
	int 	nIndex = strPath.ReverseFind(chSep);
	CString strRet = strPath.Left(nIndex);

	if (bAddBackSlash)
		MtlMakeSureTrailingChar(strRet, chSep);

	return strRet;
}



template <class _Function>
bool MtlForEachObject_OldShell(const CString &strDirectoryPath, _Function __f, bool bIncludeHidden = false)
{
	CString 		strPathFind = strDirectoryPath;

	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 		strPath 	= strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA wfd;
	HANDLE			h			= ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// Now scan the directory
	do {
		bool bDirectory = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		bool bVisible	= bIncludeHidden ? TRUE : ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 );
		if (bVisible == false)
			continue;

		if (bDirectory) {
			// ignore the current and parent directory entries
			if (::lstrcmp( wfd.cFileName, _T(".") ) == 0 || ::lstrcmp( wfd.cFileName, _T("..") ) == 0)
				continue;

			CString strDirectoryPath = strPath + wfd.cFileName;
			MtlMakeSureTrailingBackSlash(strDirectoryPath);
			__f(strDirectoryPath, true);
			continue;
		} else {
			__f(strPath + wfd.cFileName, false);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	return true;
}



template <class _Function>
bool MtlForEachFile(const CString &strDirectoryPath, _Function __f)
{
	CString 		strPathFind = strDirectoryPath;

	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 		strPath 	= strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA wfd;
	HANDLE			h			= ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// Now scan the directory
	do {
		// it is a file
		if ( ( wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) == 0 ) {
			__f(strPath + wfd.cFileName);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	return true;
}


// フォルダの中にファイルが存在するかどうか
inline bool MtlIsFileExist(const CString &strDirectoryPath)
{
	CString 		strPathFind = strDirectoryPath;

	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 		strPath 	= strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA wfd;
	HANDLE			h			= ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// Now scan the directory
	do {
		// it is a file
		if ( ( wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) == 0 ) {
			::FindClose(h);
			return true;
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);
	return false;
}



template <class _Function>
bool MtlForEachFileSort(const CString &strDirectoryPath, _Function __f)
{
	CString 				strPathFind = strDirectoryPath;

	CSimpleArray<CString>	strings;
	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 				strPath   = strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA 		wfd;

	HANDLE	h = ::FindFirstFile(strPathFind, &wfd);
	if (h == INVALID_HANDLE_VALUE)
		return false;

	// Now scan the directory
	do {
		// it is a file
		if ( ( wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) == 0 ) {
			strings.Add(strPath + wfd.cFileName);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	std::sort( _begin(strings), _end(strings) );
	std::for_each(_begin(strings), _end(strings), __f);

	return true;
}



template <class TStringClass>
struct _Function_CompareString {
	bool operator ()(TStringClass strA, TStringClass strB)
	{
		return ::lstrcmp(strA, strB) < 0 /*? true : false*/;
	}
};

//minitのでたらめ仕様ファイルの列挙+ソート関数
template <class _Function>
bool MtlForEachFileSortEx(const CString &strDirectoryPath, _Function &__f, const CString &strFilter)
{
	CString 			  strPathFind = strDirectoryPath;

	CSimpleArray<CString> strings;
	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 			  strPath	  = strPathFind;
	strPathFind += strFilter;

	WIN32_FIND_DATA 	  wfd;
	HANDLE				  h 		  = ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// Now scan the directory
	do {
		// it is a file
		if ( ( wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) == 0 ) {
			strings.Add(strPath + wfd.cFileName);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	std::sort( _begin(strings), _end(strings), _Function_CompareString<CString>() );
	std::for_each(_begin(strings), _end(strings), __f);

	return true;
}



template <class _Function>
bool MtlForEachDirectory(const CString &strDirectoryPath, _Function __f)
{
	CString 		strPathFind = strDirectoryPath;

	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 		strPath 	= strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA wfd;
	HANDLE			h			= ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// check the subdirectories for more
	do {
		bool bDirectory = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		bool bVisible	= (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN   ) == 0;

		if (bDirectory && bVisible) {
			// ignore the current and parent directory entries
			if (::lstrcmp( wfd.cFileName, _T(".") ) == 0 || ::lstrcmp( wfd.cFileName, _T("..") ) == 0)
				continue;

			CString strDirectoryPath = strPath + wfd.cFileName;
			MtlMakeSureTrailingBackSlash(strDirectoryPath);
			__f(strDirectoryPath);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	return true;
}



template <class _Function>
bool MtlForEachDirectorySort(const CString &strDirectoryPath, _Function __f)
{
	CString 			  strPathFind = strDirectoryPath;

	CSimpleArray<CString> strings;
	MtlMakeSureTrailingBackSlash(strPathFind);
	CString 			  strPath	  = strPathFind;
	strPathFind += _T("*.*");

	WIN32_FIND_DATA 	  wfd;
	HANDLE				  h 		  = ::FindFirstFile(strPathFind, &wfd);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	// check the subdirectories for more
	do {
		BOOL bDirectory = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		BOOL bVisible	= (wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0;

		if (bDirectory && bVisible) {
			// ignore the current and parent directory entries
			if (::lstrcmp( wfd.cFileName, _T(".") ) == 0 || ::lstrcmp( wfd.cFileName, _T("..") ) == 0)
				continue;

			CString strDirectoryPath = strPath + wfd.cFileName;
			MtlMakeSureTrailingBackSlash(strDirectoryPath);
			strings.Add(strDirectoryPath);
		}
	} while ( ::FindNextFile(h, &wfd) );

	::FindClose(h);

	std::sort( _begin(strings), _end(strings) );
	std::for_each(_begin(strings), _end(strings), __f);
	return true;
}



struct _MtlEnumMDIChildStruct {
	CSimpleArray<HWND>	_arrChildWnd;
	HWND				_hWndMDIClient;
};



template <class _Function>
_Function MtlForEachMDIChild(HWND hWndMDIClient, _Function __f)
{
	_MtlEnumMDIChildStruct enumstruct;

	enumstruct._hWndMDIClient = hWndMDIClient;

	::EnumChildWindows(hWndMDIClient, &_MtlMDIChildEnumProc, (LPARAM) &enumstruct);

	return std::for_each(_begin(enumstruct._arrChildWnd), _end(enumstruct._arrChildWnd), __f);
}


#if 1	//+++
template <class _Function>
_Function MtlForEachMDIChild_rev(HWND hWndMDIClient, _Function __f)
{
	_MtlEnumMDIChildStruct enumstruct;

	enumstruct._hWndMDIClient = hWndMDIClient;

	::EnumChildWindows(hWndMDIClient, &_MtlMDIChildEnumProc, (LPARAM) &enumstruct);

	unsigned num = enumstruct._arrChildWnd.GetSize();
	if (num == 0)
		return __f;
	std::reverse(&enumstruct._arrChildWnd[0], &enumstruct._arrChildWnd[0] + num);
	return std::for_each(&enumstruct._arrChildWnd[0], &enumstruct._arrChildWnd[0] + num, __f);
}
#endif


BOOL CALLBACK _MtlMDIChildEnumProc(HWND hWnd, LPARAM lParam);



struct _MtlEnumChildren {
	CSimpleArray<HWND> _arrChildWnd;
};

template <class _Function>
_Function MtlForEachChildren(HWND hWndParent, _Function __f)
{
	_MtlEnumChildren enumstruct;

	::EnumChildWindows(hWndParent, &_MtlChildrenEnumProc, (LPARAM) &enumstruct);

	return std::for_each(_begin(enumstruct._arrChildWnd), _end(enumstruct._arrChildWnd), __f);
}



BOOL CALLBACK	_MtlChildrenEnumProc(HWND hWnd, LPARAM lParam);



struct _MtlEnumChildrenCount {
	HWND _hWndParent;
	int  _nCount;
};
BOOL CALLBACK	_MtlChildrenCountEnumProc(HWND hWnd, LPARAM lParam);
int 			MtlGetDirectChildrenCount(HWND hWndParent);



//////////////////////////////////////////////////////////////////////////
struct _MtlEnumChildrenName {
	HWND	_hWndParent;
	LPCTSTR _lpszClassName;
	int 	_nCount;
};

BOOL CALLBACK	_MtlIsDirectChildrenNameEnumProc(HWND hWnd, LPARAM lParam);
int 			MtlCountDirectChildrenName(HWND hWndParent, const CString &strClassName);



//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE	//+++ UNICODE対策.
enum { MTL_CF_TEXT = CF_UNICODETEXT };
CString 		MtlGetClipboardText(bool bUseOLE = true);		//+++ ようわからないが、UNICODEでbUseOLE=falseだと、クリップボードからテキスト取れない. trueだと取れた.
#else
enum { MTL_CF_TEXT = CF_TEXT };
CString 		MtlGetClipboardText(bool bUseOLE = false);
#endif
bool			MtlSetClipboardText(const CString &str, HWND hWnd);

bool			AtlCompactPathFixed(LPTSTR lpstrOut, LPCTSTR lpstrIn, int cchLen);
bool			MtlCompactPath(CString &strOut, const CString &str, int cchLen);
CString 		MtlCompactString(const CString &str, int nMaxTextLength);
CString 		MtlGetHTMLHelpPath();		// UDT DGSTR
CString 		MtlGetFileNameFromCommandLine(const CString &_strCommand);

BOOL			MtlSetExcutable(const CString &strExt, const CString &strExe_, const CString &strName, bool bOn);
BOOL			MtlIsExecutable(const CString &strExt, const CString &strName);



///////////////////////////////////////////////////////////////////////////////
// MtlOpenHttpFiles
bool			MtlIsInternetFileMaybe(const CString &strPath);
bool			__MtlIsMeaninglessCharInHttp(TCHAR ch);
CString 		__MtlRemoveTrailingMeaninglessChar(const CString &str_);
void			MtlOpenHttpFiles(CSimpleArray<CString> &arrFiles, const CString &strCmdLine_);



template <class T>
class CDDEMessageHandler {
	// Data members
	ATOM	m_atomApp;
	ATOM	m_atomSystemTopic;

public:
	// Constructor/Destructor
	CDDEMessageHandler(const CString &strAppName)
	{
		m_atomApp		  = ::GlobalAddAtom(strAppName);
		m_atomSystemTopic = ::GlobalAddAtom( _T("system") );
	}


	~CDDEMessageHandler()
	{
		::GlobalDeleteAtom(m_atomApp);
		::GlobalDeleteAtom(m_atomSystemTopic);

	  #ifdef _DEBUG
		//		TCHAR szAtomName[_MAX_PATH];
		//		ATLASSERT(::GlobalGetAtomName(m_atomApp, szAtomName, _MAX_PATH - 1) == 0);
		//		ATLASSERT(::GlobalGetAtomName(m_atomSystemTopic, szAtomName, _MAX_PATH - 1) == 0);
	  #endif
	}


private:
	// Overridables
	bool OnDDEOpenFile(const CString &strFileName)
	{
		return true;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDDEMessageHandler)
		MESSAGE_HANDLER(WM_DDE_INITIATE , OnDDEInitiate )
		MESSAGE_HANDLER(WM_DDE_EXECUTE	, OnDDEExecute	)
		MESSAGE_HANDLER(WM_DDE_TERMINATE, OnDDETerminate)
	END_MSG_MAP()


private:
	LRESULT OnDDEInitiate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0
		   && (ATOM) LOWORD(lParam) == m_atomApp
		   && (ATOM) HIWORD(lParam) == m_atomSystemTopic)
		{
			ATLTRACE2( atlTraceGeneral, 4, _T("OnDDEInitiate - from windows shell\n") );
			// make duplicates of the incoming atoms (really adding a reference)
			TCHAR	szAtomName[_MAX_PATH];
			szAtomName[0]	= 0;	//+++
			MTLVERIFY(::GlobalGetAtomName(m_atomApp, szAtomName, _MAX_PATH - 1) != 0);
			MTLVERIFY(::GlobalAddAtom(szAtomName) == m_atomApp);
			MTLVERIFY(::GlobalGetAtomName(m_atomSystemTopic, szAtomName, _MAX_PATH - 1) != 0);
			MTLVERIFY(::GlobalAddAtom(szAtomName) == m_atomSystemTopic);

			// send the WM_DDE_ACK (caller will delete duplicate atoms)
			T *   pT = static_cast<T *>(this);
			::SendMessage( (HWND) wParam, WM_DDE_ACK, (WPARAM) pT->m_hWnd, MAKELPARAM(m_atomApp, m_atomSystemTopic) );
		}

		return 0L;
	}


	// always ACK the execute command - even if we do nothing
	LRESULT OnDDEExecute(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		ATLTRACE2( atlTraceGeneral, 4, _T("OnDDEExecute - from windows shell\n") );
		// unpack the DDE message
		UINT_PTR	unused;
		HGLOBAL 	hData;
		MTLVERIFY( UnpackDDElParam(WM_DDE_EXECUTE, lParam, &unused, (PUINT_PTR) &hData) );

		// get the command string
		CString 	strCommand( reinterpret_cast<LPCTSTR>( ::GlobalLock(hData) ) );
		GlobalUnlock(hData);

		T * 	 pT = static_cast<T *>(this);
		// acknowledge now - before attempting to execute
		::PostMessage( (HWND) wParam, WM_DDE_ACK, (WPARAM) pT->m_hWnd, ReuseDDElParam(lParam, WM_DDE_EXECUTE, WM_DDE_ACK, (UINT) 0x8000, (UINT_PTR) hData) );

		// don't execute the command when the window is disabled
		if ( !pT->IsWindowEnabled() ) {
			ATLTRACE(_T("Warning: DDE command '%s' ignored because window is disabled.\n"), strCommand);
			return 0;
		}

		// execute the command
		if ( !_OnDDECommand(strCommand) )
			ATLTRACE(_T("Error: failed to execute DDE command '%s'.\n"), strCommand);

		return 0L;
	}


	LRESULT OnDDETerminate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		T *pT = static_cast<T *>(this);
		::PostMessage( (HWND) wParam, WM_DDE_TERMINATE, (WPARAM) pT->m_hWnd, lParam );
		return 0L;
	}


	bool _OnDDECommand(const CString &_strCommand)
	{
		CString 	strCommand	= _strCommand;

		// open format is "[open("%s")]" - no whitespace allowed, one per line
		if ( strCommand.Left(7) == _T("[open(\"") ) {
			strCommand = strCommand.Right(strCommand.GetLength() - 7);
		} else {
			return false;
		}

		int 	i = strCommand.Find('"');
		if (i == -1)
			return false;

		// illegally terminated

		CString strFileName = strCommand.Left(i);
		T * 	pT			= static_cast<T *>(this);
		return pT->OnDDEOpenFile(strFileName);
	}
};



void MtlDrawDragRectFixed(
		CDCHandle	dc,
		LPCRECT 	lpRect,
		SIZE		size,
		LPCRECT 	lpRectLast,
		SIZE		sizeLast,
		HBRUSH		hBrush		= NULL,
		HBRUSH		hBrushLast	= NULL);



}		// namespace MTL;



#endif	// __MTLMISC_H__
