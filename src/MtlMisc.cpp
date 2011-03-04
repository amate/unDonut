/**
 *	@file	MtlMisc.cpp
 */
#include "stdafx.h"
#include "MtlMisc.h"
#include "DonutPFunc.h" 		//+++


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//extern CServerAppModule _Module;


namespace MTL {

BOOL CALLBACK _MtlMDIChildEnumProc(HWND hWnd, LPARAM lParam)
{
	if ( ::GetWindow(hWnd, GW_OWNER) )
		return TRUE;

	_MtlEnumMDIChildStruct *penumstruct = (_MtlEnumMDIChildStruct *) lParam;

	if (::GetParent(hWnd) != penumstruct->_hWndMDIClient)
		return TRUE;

	penumstruct->_arrChildWnd.Add(hWnd);
	return TRUE;
}



BOOL CALLBACK _MtlChildrenEnumProc(HWND hWnd, LPARAM lParam)
{
	_MtlEnumChildren *penumstruct = (_MtlEnumChildren *) lParam;

	penumstruct->_arrChildWnd.Add(hWnd);
	return TRUE;
}



BOOL CALLBACK _MtlChildrenCountEnumProc(HWND hWnd, LPARAM lParam)
{
	_MtlEnumChildrenCount *penumstruct = (_MtlEnumChildrenCount *) lParam;

	if (::GetParent(hWnd) == penumstruct->_hWndParent)
		++penumstruct->_nCount;

	return TRUE;
}



int MtlGetDirectChildrenCount(HWND hWndParent)
{
	_MtlEnumChildrenCount enumstruct = { hWndParent, 0 };

	::EnumChildWindows(hWndParent, &_MtlChildrenCountEnumProc, (LPARAM) &enumstruct);

	return enumstruct._nCount;
}



//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _MtlIsDirectChildrenNameEnumProc(HWND hWnd, LPARAM lParam)
{
	_MtlEnumChildrenName *penumstruct = (_MtlEnumChildrenName *) lParam;

	if (::GetParent(hWnd) == penumstruct->_hWndParent) {
		enum { NAMESZ = 260/*50*/ };
		TCHAR	name[NAMESZ];
		name[0]	= 0;	//+++
		::GetClassName(hWnd, name, NAMESZ);

		if (::lstrcmp(name, penumstruct->_lpszClassName) == 0) {
			penumstruct->_nCount++;
		}
	}

	return TRUE;
}



int MtlCountDirectChildrenName(HWND hWndParent, const CString &strClassName)
{
	_MtlEnumChildrenName enumstruct = { hWndParent, (LPCTSTR) strClassName, 0 };

	::EnumChildWindows(hWndParent, &_MtlIsDirectChildrenNameEnumProc, (LPARAM) &enumstruct);

	return enumstruct._nCount;
}




//////////////////////////////////////////////////////////////////////////
CString MtlGetClipboardText(bool bUseOLE)
{
	CString strText;
	if (bUseOLE == false) {
		if ( ::IsClipboardFormatAvailable(MTL_CF_TEXT) && ::OpenClipboard(NULL) ) {
			HGLOBAL hText = ::GetClipboardData(MTL_CF_TEXT);

			if (hText) {
				//+++ strText = reinterpret_cast<LPSTR>( ::GlobalLock(hText) );
				strText = reinterpret_cast<LPTSTR>( ::GlobalLock(hText) );		//+++ UNICODE修正
				::GlobalUnlock(hText);
			}
		  #if 0 //def UNICODE	//+++ おためし...無意味だった....
			else {
				HGLOBAL hText = ::GetClipboardData(CF_TEXT);
				if (hText) {
					strText = reinterpret_cast<LPSTR>( ::GlobalLock(hText) );
					::GlobalUnlock(hText);
				}
			}
		  #endif

			::CloseClipboard();
		}
	} else {
		CComPtr<IDataObject> spDataObject;
		HRESULT 			 hr 	   = ::OleGetClipboard(&spDataObject);

		if ( FAILED(hr) )
			return strText;
		FORMATETC			 formatetc = { MTL_CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM			 stgmedium;
		hr = spDataObject->GetData(&formatetc, &stgmedium);

		if ( SUCCEEDED(hr) ) {
			if (stgmedium.hGlobal != NULL) {
				HGLOBAL hText = stgmedium.hGlobal;
				//+++ strText = reinterpret_cast<LPSTR>( ::GlobalLock(hText) );
				strText = reinterpret_cast<LPTSTR>( ::GlobalLock(hText) );		//+++ UNICODE 修正
				::GlobalUnlock(hText);
			}

			::ReleaseStgMedium(&stgmedium);
		}
	}

	return strText;
}



bool MtlSetClipboardText(const CString &str, HWND hWnd)
{
	if ( str.IsEmpty() )
		return false;

  #if 1	//+++ UNICODE対策
	int 	nByte = (str.GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL hText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nByte);
  #else
	int 	nByte = str.GetLength();
	HGLOBAL hText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nByte + 1);
  #endif

	if (hText == NULL)
		return false;

	BYTE *	pText = (BYTE *) ::GlobalLock(hText);

	if (pText == NULL)
		return false;

	::memcpy(pText, (LPCTSTR) str, nByte);

	::GlobalUnlock(hText);

	::OpenClipboard(hWnd);
	::EmptyClipboard();
	::SetClipboardData(MTL_CF_TEXT, hText);
	::CloseClipboard();
	return true;
}



bool AtlCompactPathFixed(LPTSTR lpstrOut, LPCTSTR lpstrIn, int cchLen)
{
	ATLASSERT(lpstrOut != NULL);
	ATLASSERT(lpstrIn  != NULL);
	ATLASSERT(cchLen > 0);

	LPCTSTR   szEllipsis	 = _T("...");
	const int cchEndEllipsis = 3;
	const int cchMidEllipsis = 4;

	if (lstrlen(lpstrIn) < cchLen)						// ******changed****** I can't understand why (+1) needed.
		return (lstrcpy(lpstrOut, lpstrIn) != NULL);

	*lpstrOut = _T('\0');								// ******added******

	// As lstrcat needs NULL-terminated string, lstrcat can't touch lpstrOut without this,
	// and lpstrOut will be endless string and it may crash your program.

	// check if the separator is a slash or a backslash
	TCHAR	  chSlash		 = _T('\\');

	for ( LPTSTR lpstr = (LPTSTR) lpstrIn; *lpstr != 0; lpstr = ::CharNext(lpstr) ) {
		if ( ( *lpstr == _T('/') ) || ( *lpstr == _T('\\') ) )
			chSlash = *lpstr;
	}

	// find the filename portion of the path
	LPCTSTR   lpstrFileName  = lpstrIn;

	for ( LPCTSTR pPath = lpstrIn; *pPath; pPath = ::CharNext(pPath) ) {
		if ( ( pPath[0] == _T('\\') || pPath[0] == _T(':') || pPath[0] == _T('/') )
		   && pPath[1] && pPath[1] != _T('\\') && pPath[1] != _T('/') )
			lpstrFileName = pPath + 1;
	}

	int 	  cchFileName	 = lstrlen(lpstrFileName);

	// handle just the filename without a path
	if (lpstrFileName == lpstrIn && cchLen > cchEndEllipsis) {
		bool bRet = (lstrcpyn(lpstrOut, lpstrIn, cchLen - cchEndEllipsis) != NULL);

		if (bRet) {
		  #ifndef _UNICODE
			// The index that must be checked was wrong.
			if ( _IsDBCSTrailByte(lpstrIn, cchLen - cchEndEllipsis - 1) ) { 	// ******changed******
				ATLASSERT(cchLen - cchEndEllipsis - 2 >= 0);					// ******added******
				lpstrOut[cchLen - cchEndEllipsis - 2] = 0;						// ******changed******
			}
		  #endif	//_UNICODE

			bRet = (lstrcat(lpstrOut, szEllipsis) != NULL);
		}

		return bRet;
	}

	// handle just ellipsis
	if ( ( cchLen < (cchMidEllipsis + cchEndEllipsis) ) ) {
		int i;

		for (i = 0; i < cchLen - 1; i++)
			lpstrOut[i] = ( (i + 1) == cchMidEllipsis ) ? chSlash : _T('.');

		lpstrOut[i] = 0;

		return true;
	}

	// calc how much we have to copy
	int 	  cchToCopy  = cchLen - (cchMidEllipsis + cchFileName);

	if (cchToCopy < 0)
		cchToCopy = 0;

  #ifndef _UNICODE
	if ( cchToCopy > 0 && _IsDBCSTrailByte(lpstrIn, cchToCopy - 1) )	// ******changed******
		cchToCopy--;
  #endif	//_UNICODE

	bool  bRet	= (lstrcpyn(lpstrOut, lpstrIn, cchToCopy) != NULL);
	if (!bRet)
		return false;

	// add ellipsis
	bRet = (lstrcat(lpstrOut, szEllipsis) != NULL);

	if (!bRet)
		return false;

	TCHAR	  szSlash[2] = { chSlash, 0 };
	bRet = (lstrcat(lpstrOut, szSlash) != NULL);

	if (!bRet)
		return false;

	// add filename (and ellipsis, if needed)
	if ( cchLen > (cchMidEllipsis + cchFileName) ) {
		bRet = (lstrcat(lpstrOut, lpstrFileName) != NULL);
	} else {
		cchToCopy = cchLen - cchMidEllipsis - cchEndEllipsis;
	  #ifndef _UNICODE
		if ( cchToCopy > 0 && _IsDBCSTrailByte(lpstrFileName, cchToCopy - 1) )	// ******changed******
			cchToCopy--;
	  #endif	//_UNICODE

		bRet	  = (lstrcpyn(&lpstrOut[cchMidEllipsis], lpstrFileName, cchToCopy) != NULL);

		if (bRet)
			bRet = (lstrcat(lpstrOut, szEllipsis) != NULL);
	}

	return bRet;
}



bool MtlCompactPath(CString &strOut, const CString &str, int cchLen)
{
	ATLASSERT(cchLen > 0);
	if (cchLen <= 0)		//+++
		return false;

	bool bRet = AtlCompactPathFixed(strOut.GetBufferSetLength(cchLen), str, cchLen);
	strOut.ReleaseBuffer();

	if ( !bRet || strOut.IsEmpty() || strOut == _T("...") )
		return false;

	return true;
}



CString MtlCompactString(const CString &str, int nMaxTextLength)
{
	ATLASSERT(nMaxTextLength > 0);

  #ifdef UNICODE
	int l = Misc::eseHankakuStrLen(str);
  #else
	int l = str.GetLength();
  #endif
	if (l <= nMaxTextLength)
		return str;

	LPCTSTR   szEllipsis	 = _T("...");
	const int cchEndEllipsis = 3;

	if (nMaxTextLength <= cchEndEllipsis) { 	// only ellipsis
		return CString(_T('.'), nMaxTextLength);
	}

	int 	  nIndex		 = nMaxTextLength - cchEndEllipsis;
	ATLASSERT(nIndex > 0);
	ATLASSERT( nIndex < l/*str.GetLength()*/ );

  #ifndef UNICODE
	if ( _IsDBCSTrailByte(str, nIndex) )
		--nIndex;
  #endif
	// one step back
	ATLASSERT(nIndex >= 0);

  #ifdef UNICODE
	return Misc::eseHankakuStrLeft(str, nIndex) + szEllipsis;
  #else
	return str.Left(nIndex) + szEllipsis;
  #endif
}



#if 0	//+++
CString MtlGetModuleFileName()
{
	TCHAR sz[MAX_PATH];
	sz[0] = 0;
	if (::GetModuleFileName(_Module.GetModuleInstance(), sz, MAX_PATH) == 0)
		sz[0] = 0;
	return sz;
}
#endif



#if 0	//+++ 未使用
CString MtlCurrentDirectoryFileName(const CString &strFileName)
{
  #if 1
	return GetExeDirectory() + strFileName;
  #else
	TCHAR	sz[MAX_PATH];

	::GetModuleFileName(_Module.GetModuleInstance(), sz, MAX_PATH);

	CString str(sz);
	int 	nIndex = str.ReverseFind( _T('\\') );

	return str.Left(nIndex + 1) + strFileName;
  #endif
}
#endif



// UDT DGSTR
CString MtlGetHTMLHelpPath()
{
  #if 1	//+++ ヘルプ名をふやす..
	CString strExePath 	= Misc::GetExeDirectory();
	CString strHelpPath = strExePath + _T("help\\");
	static const TCHAR* help_name[] = {
		_T("unDonut+mod.chm"),
		_T("unDonut.chm"),
		_T("DonutP.chm"),
		NULL,
	};
	for (unsigned i = 0; help_name[i]; ++i) {
		LPCTSTR 	name = help_name[i];
		CString 	path = strHelpPath + name;
		if (Misc::IsExistFile(path))
			return path;
		CString 	path2 = strExePath + name;
		if (Misc::IsExistFile(path2))
			return path2;
	}
	return strExePath + help_name[0];	//+++ 無かった場合は一番無難なパス名にしとく.
  #else
	CString strExePath 	= Misc::GetExeDirectory();
	CString strPath 	= strExePath + _T("help\\");
	CString strFullPath = strPath + _T("unDonut.chm");
	if (Misc::IsExistFile(strFullPath) == 0) {
		strFullPath = strExePath + _T("unDonut.chm");
		if (Misc::IsExistFile(strFullPath) == 0) {
			strFullPath = strPath + _T("DonutP.chm");
			if (Misc::IsExistFile(strFullPath) == 0) {
				strFullPath = strExePath + _T("DonutP.chm");
				if (Misc::IsExistFile(strFullPath) == 0) {
					strFullPath = strPath + _T("unDonut.chm");
				}
			}
		}
	}
	return strFullPath;
   #endif
}
// ENDE



CString MtlGetFileNameFromCommandLine(const CString &_strCommand)
{
	CString strCommand = _strCommand;

	if ( strCommand.Left(1) == _T("\"") ) {
		strCommand = strCommand.Right(strCommand.GetLength() - 1);
	} else {
		return strCommand;
	}

	int i =  strCommand.Find('"');
	if (i == -1)
		return _strCommand;

	// illegally terminated
	return strCommand.Left(i);
}


//\\ 関連付け（レジストリに書き込み）
BOOL MtlSetExcutable(const CString &strExt, const CString &strExe_, const CString &strName, bool bOn)
{
	CString 		strExe = _T('"') + strExe_ + _T('"');	// fixed by Shimawari, thanks!
	Misc::CRegKey 	rkExt;
	Misc::CRegKey 	rkShell;
	Misc::CRegKey 	rkDonut;
	Misc::CRegKey 	rkCommand;
	LONG			lRet;

	// open ROOT
	lRet = rkExt.Create(HKEY_CLASSES_ROOT, strExt);
	if (lRet != ERROR_SUCCESS)
		return FALSE;

	if (bOn) {
		// set shell value to name (means default)
		lRet = rkExt.SetKeyValue(_T("shell"), strName);
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		// open shell
		lRet = rkShell.Create( rkExt, _T("shell") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkDonut.Create(rkShell, strName);
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkDonut.SetStringValue( NULL, strName);
		//lRet = rkDonut.SetValue(strName);
		ATLASSERT(lRet == ERROR_SUCCESS);

		// command has /dde
		lRet = rkCommand.Create( rkDonut, _T("command") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkCommand.SetStringValue( NULL, strExe + _T(" /dde") );
		//lRet = rkCommand.SetValue(strExe + _T(" /dde"));//\"%1\""));
		ATLASSERT(lRet == ERROR_SUCCESS);

		// DDE
		Misc::CRegKey rkDDEExec;		// order is important
		Misc::CRegKey rkApplication;
		Misc::CRegKey rkTopic;

		lRet = rkDDEExec.Create( rkDonut, _T("ddeexec") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkDDEExec.SetStringValue( NULL, _T("[open(\"%1\")]") );
		//lRet = rkDDEExec.SetValue(_T("[open(\"%1\")]"));
		ATLASSERT(lRet == ERROR_SUCCESS);

		lRet = rkApplication.Create( rkDDEExec, _T("application") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkApplication.SetStringValue( NULL, strName);
		//lRet = rkApplication.SetValue(strName);
		ATLASSERT(lRet == ERROR_SUCCESS);

		lRet = rkTopic.Create( rkDDEExec, _T("topic") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		lRet = rkTopic.SetStringValue( NULL, _T("system") );
		//lRet = rkTopic.SetValue(_T("system"));
		ATLASSERT(lRet == ERROR_SUCCESS);

	} else {
		// open shell
		lRet = rkShell.Open( rkExt, _T("shell") );
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		// clean up name if my name
		DWORD  nLen;
		lRet = rkShell.QueryStringValue( NULL, NULL, &nLen);
		//lRet = rkShell.QueryValue(NULL, NULL, &nLen);
		if (lRet != ERROR_SUCCESS)
			return FALSE;

		ATLTRACE2(atlTraceGeneral, 4, _T("MtlSetExcutable :%d\n"), nLen);

		LPTSTR lpszText = (LPTSTR) _alloca( (nLen + 1) * sizeof (TCHAR) );
		lRet = rkShell.QueryStringValue( NULL, lpszText, &nLen);

		//lRet = rkShell.QueryValue(lpszText, NULL, &nLen);
		if (::lstrcmp(lpszText, strName) == 0) {
			lRet = rkExt.SetKeyValue( _T("shell"), _T("") );
			if (lRet != ERROR_SUCCESS)
				return FALSE;
		}

		lRet = rkShell.RecurseDeleteKey(strName);
		//for NT,2k,XP RegDeleteKey
		//	lRet = rkShell.DeleteSubKey(strName);
		if (lRet != ERROR_SUCCESS)
			return FALSE;
	}

	return TRUE;
}


//\\ 関連付けの読み込み
BOOL MtlIsExecutable(const CString &strExt, const CString &strName)
{
	Misc::CRegKey rkExt;
	Misc::CRegKey rkShell;

	// open ROOT
	LONG	lRet = rkExt.Open(HKEY_CLASSES_ROOT, strExt, KEY_QUERY_VALUE);
	if (lRet != ERROR_SUCCESS)
		return FALSE;

	// open shell
	lRet = rkShell.Open( rkExt, _T("shell"), KEY_QUERY_VALUE );
	if (lRet != ERROR_SUCCESS)
		return FALSE;

	DWORD	nLen = 0;
	lRet = rkShell.QueryStringValue( NULL, NULL, &nLen);
	//lRet = rkShell.QueryValue(NULL, NULL, &nLen);
	if (lRet != ERROR_SUCCESS)
		return FALSE;

	ATLTRACE2(atlTraceGeneral, 4, _T("MtlIsExecutable :%d\n"), nLen);

	LPTSTR	lpszText = (LPTSTR) _alloca( (nLen + 1) * sizeof (TCHAR) );

	lRet = rkShell.QueryStringValue( NULL, lpszText, &nLen);
	//lRet = rkShell.QueryValue(lpszText, NULL, &nLen);
	//	ATLTRACE2(atlTraceGeneral, 4, _T("MtlIsExecutable :%s\n"), lpszText);
  #if 1	//+++
	return ::lstrcmp(lpszText, strName) == 0;
  #else
	if (::lstrcmp(lpszText, strName) == 0)
		return TRUE;
	else
		return FALSE;
  #endif
}



///////////////////////////////////////////////////////////////////////////////
// MtlOpenHttpFiles
bool MtlIsInternetFileMaybe(const CString &strPath)
{
	if ( strPath.Right(1) == _T("/") )	// all I can do
		return true;

	int  nIndex = strPath.ReverseFind( _T('/') );
	if (nIndex == -1)
		return false;

	nIndex = __MtlFindChar( strPath, nIndex, _T('.') );
	if (nIndex == -1)
		return false;

	CString 	strExt3 = strPath.Mid(nIndex, 4);
	CString 	strExt4 = strPath.Mid(nIndex, 5);
	CString 	strExt5 = strPath.Mid(nIndex, 6);

	if (  strExt4.CompareNoCase( _T(".html" ) ) == 0
	   || strExt5.CompareNoCase( _T(".shtml") ) == 0
	   || strExt3.CompareNoCase( _T(".htm"	) ) == 0
	   || strExt3.CompareNoCase( _T(".url"	) ) == 0
	   || strExt3.CompareNoCase( _T(".cgi"	) ) == 0
	   || strExt3.CompareNoCase( _T(".asp"	) ) == 0)
		return true;
	else
		return false;
}



bool __MtlIsMeaninglessCharInHttp(TCHAR ch)
{
	return	  ch == _T('\\') || ch == _T(':' )
		   || ch == _T(',' ) || ch == _T(';' )
		   || ch == _T('*' ) || ch == _T('\"')
		   || ch == _T('<' ) || ch == _T('>' )
		   || ch == _T('|' )
		   || ch == _T(' ' ) || ch == _T('\t')
		   || ch == _T('\r');
}



CString __MtlRemoveTrailingMeaninglessChar(const CString &str_)
{
	//	ATLTRACE2(atlTraceGeneral, 4, _T("__MtlRemoveTrailingMeaninglessChar: (%s)\n"), str_);
	CString str    = str_;
	int 	nCount = 0;

	for (int i = str.GetLength() - 1; i >= 0; --i) {
		// ATLTRACE2(atlTraceGeneral, 4, _T("									: (%d, %d)\n"), str[i], i);
		if ( __MtlIsMeaninglessCharInHttp(str[i]) ) {
			nCount++;
		} else
			break;
	}

	return str.Left(str.GetLength() - nCount);
}



void MtlOpenHttpFiles(CSimpleArray<CString> &arrFiles, const CString &strCmdLine_)
{
	// ATLTRACE2(atlTraceGeneral, 4, _T("MtlOpenHttpFiles :%s\n"), strCmdLine_.Left(100));
	CString 	strCmdLine = strCmdLine_;
	if ( strCmdLine.IsEmpty() )
		return;
	int 		nFirst	   = 0;
	int 		nLast;
	CString 	strFile;

	do {
		// ATLTRACE2(atlTraceGeneral, 4, _T(" step1\n"));
		nFirst	= MTL::__MtlFind( strCmdLine, nFirst, _T("http:") );
		if (nFirst == -1)
			break;

		// ATLTRACE2(atlTraceGeneral, 4, _T(" step2\n"));
		nLast	= MTL::__MtlFindChar( strCmdLine, nFirst + 4, _T('\n') );
		if (nLast == -1) {
			strFile = strCmdLine.Mid(nFirst);
		} else {
			ATLASSERT( strCmdLine.GetAt(nLast) == _T('\n') );
			strFile = strCmdLine.Mid(nFirst, nLast - nFirst);
			ATLASSERT( strFile.Right(1) != _T('\n') );
		}

		// ATLTRACE2(atlTraceGeneral, 4, _T(" step3\n"));
		strFile = MTL::__MtlRemoveTrailingMeaninglessChar(strFile);
		// ATLTRACE2(atlTraceGeneral, 4,  _T("				   :cleaned(%s)\n"), strFile);
		if ( MTL::MtlIsInternetFileMaybe(strFile) ) {
			ATLTRACE2(atlTraceGeneral, 4, _T("                 :open   (%s)\n"), strFile);
			arrFiles.Add(strFile);
		}

		if (nLast == -1)
			break;

		// ATLTRACE2(atlTraceGeneral, 4, _T(" step4\n"));
		nFirst	= nLast;
	} while ( nFirst < strCmdLine.GetLength() );
}



void MtlDrawDragRectFixed(
		CDCHandle	dc,
		LPCRECT 	lpRect,
		SIZE		size,
		LPCRECT 	lpRectLast,
		SIZE		sizeLast,
		HBRUSH		hBrush,
		HBRUSH		hBrushLast)
{
	// first, determine the update region and select it
	HRGN	hRgnNew;
	HRGN	hRgnOutside;
	HRGN	hRgnInside;

	hRgnOutside = ::CreateRectRgnIndirect(lpRect);

	RECT   rect 	 = *lpRect;
	::InflateRect(&rect, -size.cx, -size.cy);
	::IntersectRect(&rect, &rect, lpRect);
	hRgnInside	= ::CreateRectRgnIndirect(&rect);

	hRgnNew 	= ::CreateRectRgn(0, 0, 0, 0);
	::CombineRgn(hRgnNew, hRgnOutside, hRgnInside, RGN_XOR);

	HBRUSH	hBrushOld = NULL;

	if (hBrush == NULL)
		hBrush = CDCHandle::GetHalftoneBrush();

	if (hBrushLast == NULL)
		hBrushLast = hBrush;

	HRGN   hRgnLast  = NULL, hRgnUpdate = NULL;

	if (lpRectLast != NULL) {
		// find difference between new region and old region
		hRgnLast = ::CreateRectRgn(0, 0, 0, 0);
		::SetRectRgn(hRgnOutside, lpRectLast->left, lpRectLast->top, lpRectLast->right, lpRectLast->bottom);
		rect	 = *lpRectLast;
		::InflateRect(&rect, -sizeLast.cx, -sizeLast.cy);
		::IntersectRect(&rect, &rect, lpRectLast);
		::SetRectRgn(hRgnInside, rect.left, rect.top, rect.right, rect.bottom);
		::CombineRgn(hRgnLast, hRgnOutside, hRgnInside, RGN_XOR);

		// only diff them if brushes are the same
		if (hBrush == hBrushLast) {
			hRgnUpdate = ::CreateRectRgn(0, 0, 0, 0);
			::CombineRgn(hRgnUpdate, hRgnLast, hRgnNew, RGN_XOR);
		}
	}

	if (hBrush != hBrushLast && lpRectLast != NULL) {
		// brushes are different -- erase old region first
		dc.SelectClipRgn(hRgnLast);
		dc.GetClipBox(&rect);
		hBrushOld = dc.SelectBrush(hBrushLast);
		dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
		dc.SelectBrush(hBrushOld);
		hBrushOld = NULL;
	}

	// draw into the update/new region
	dc.SelectClipRgn(hRgnUpdate != NULL ? hRgnUpdate : hRgnNew);
	dc.GetClipBox(&rect);
	hBrushOld = dc.SelectBrush(hBrush);
	dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);

	// cleanup DC ******************* fixed ************************
	if (hBrushOld != NULL)
		dc.SelectBrush(hBrushOld);

	dc.SelectClipRgn(NULL);

	if (NULL != hBrush)
		DeleteObject(hBrush);

	//Free our halftone brush
	DeleteObject(hRgnNew);
	DeleteObject(hRgnOutside);
	DeleteObject(hRgnInside);

	if (NULL != hRgnLast)
		DeleteObject(hRgnLast);

	if (NULL != hRgnUpdate)
		DeleteObject(hRgnUpdate);
}



}	// namespace MTL;
