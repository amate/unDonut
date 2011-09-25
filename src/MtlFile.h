/**
 *	@file	MtlFile.h
 *	@brief	MTL : ファイル関係
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

#ifndef __MTLFILE_H__
#define __MTLFILE_H__

#pragma once

#include <atlmisc.h>
#include "ItemIDList.h"
#include "MtlMisc.h"
#include "Misc.h"


namespace MTL {



__inline bool MtlIsValidPath(const CString &strPath)
{
  #if 1 //+++
	return ::GetFileAttributes(strPath) != 0xFFFFFFFF;
  #else
	DWORD dw = ::GetFileAttributes(strPath);

	if (dw == 0xFFFFFFFF)
		return false;
	else
		return true;
  #endif
}



inline bool MtlIsDirectory(const CString &strPath)
{
	DWORD dw = ::GetFileAttributes(strPath);
	if (dw == 0xFFFFFFFF)
		return false;
	return (dw & FILE_ATTRIBUTE_DIRECTORY) != 0;
}



inline bool MtlIsDirectoryPath(const CString &strPath, bool bUseSystem = false)
{
	if (!bUseSystem) {
		return __MtlIsLastChar( strPath, _T('\\') ) != 0;
	} else {
		return MtlIsDirectory(strPath);
	}
}



///+++ メモ:実行ファイル名の拡張子をstrExtに付け替える. strExtには予め'.'を含んでいること.
inline CString MtlGetChangedExtFromModuleName(const CString &strExt)
{
	CString 	str = Misc::GetExeFileName();
	if (str.IsEmpty() == 0) {
		int 	nIndex = str.ReverseFind( _T('.') );
		if (nIndex != -1)
			return str.Left(nIndex) + strExt;
	}
	return CString();
}



inline bool MtlIsExt(const CString &strPath, const CString &strExt)
{
	CString 	strExtSrc = strPath.Right(4);
	return (strExtSrc.CompareNoCase(strExt) == 0);
}



inline bool MtlIsProtocol(const CString &strPath, const CString &strProSrc)
{
	int 	nIndex = strPath.Find( _T(':') );
	if (nIndex < 0)
		return false;
	CString strPro = strPath.Left(nIndex);
	return (strPro == strProSrc);
}


/// ファイル作成時の無効な文字を置換する
inline void MtlValidateFileName(CString &strName)
{
	strName.Replace( _T("\\"), _T("-") );
	strName.Replace( _T("/" ), _T("-") );
	strName.Replace( _T(":" ), _T("-") );
	strName.Replace( _T("*" ), _T("-") );
	strName.Replace( _T("?" ), _T("-") );
	strName.Replace( _T("\""), _T("-") );
	strName.Replace( _T("<" ), _T("-") );
	strName.Replace( _T(">" ), _T("-") );
	strName.Replace( _T("|" ), _T("-") );
}

/// 有効なファイル名ならtrueを返す
inline bool MtlIsValidateFileName(const CString& strFileName)
{
	return strFileName.FindOneOf(_T("\\/:*?\"<>|")) == -1;
}



inline CItemIDList MtlGetHtmlFileIDList()
{
	// strNewFile == .../unDonut.html
	CString strNewFile = MtlGetChangedExtFromModuleName( _T(".html") );

	if ( strNewFile.IsEmpty() )
		return CItemIDList();

	HANDLE	h	= ::CreateFile(strNewFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return CItemIDList();

	::CloseHandle(h);

	return (LPCTSTR) strNewFile;
}



inline void MtlDeleteHtmlFileIDList()
{
	CItemIDList idl = MtlGetChangedExtFromModuleName( _T(".html") );

	::DeleteFile( idl.GetPath() );
}



inline int MtlGetNormalIconIndex(LPCITEMIDLIST pidl, LPCITEMIDLIST pidlHtm = NULL)
{
	if ( pidlHtm != NULL && MtlIsTooSlowIDList(pidl) ) {
		return MtlGetSystemIconIndex(pidlHtm);
	} else {
		return MtlGetSystemIconIndex(pidl);
	}
}



inline int MtlGetSelectedIconIndex(LPCITEMIDLIST pidl, bool bFolder, LPCITEMIDLIST pidlHtm = NULL)
{
	if ( pidlHtm != NULL && MtlIsTooSlowIDList(pidl) ) {
		return MtlGetSystemIconIndex(pidlHtm);
	} else if (bFolder) {
		return MtlGetSystemIconIndex(pidl, SHGFI_SMALLICON | SHGFI_OPENICON);
	} else {
		return MtlGetSystemIconIndex(pidl);
	}
}



inline CString MtlGetDisplayTextFromPath(const CString &strPath)
{
	CString str(strPath);

	if ( MtlIsDirectoryPath(strPath) ) {
		int nIndex = str.ReverseFind( _T('\\') );
		str    = str.Left(nIndex);
		nIndex = str.ReverseFind( _T('\\') );
		str    = str.Right(str.GetLength() - nIndex - 1);
	} else {
		int nIndex = str.ReverseFind( _T('\\') );
		str    = str.Right(str.GetLength() - nIndex - 1);
		nIndex = str.ReverseFind( _T('.') );

		if (nIndex != -1)
			str = str.Left(nIndex);
	}

	return str;
}



inline CString MtlGetFileName(const CString &strPath)
{
	int nIndex = strPath.ReverseFind( _T('\\') );

	if (nIndex == strPath.GetLength() - 1) { // maybe directory
		return MtlGetDisplayTextFromPath(strPath);
	}

	return strPath.Right(strPath.GetLength() - nIndex - 1);
}



inline CString MtlGetFileTitle(CString &strPath)
{
	int nPos = strPath.Find( _T(".") );

	if (nPos == -1)
		return strPath;

	return strPath.Left(nPos);
}



inline bool MtlIsInternetFile(const CString &strPath)
{
	CString strExt	= strPath.Right(4);
	CString strExt2 = strPath.Right(5);
	CString strExt3 = strPath.Right(6);

	if (  strExt2.CompareNoCase( _T(".html" ) ) == 0
	   || strExt3.CompareNoCase( _T(".shtml") ) == 0
	   || strExt.CompareNoCase ( _T(".htm"	) ) == 0
	   || strExt.CompareNoCase ( _T(".url"	) ) == 0
	   || strExt.CompareNoCase ( _T(".cgi"	) ) == 0
	   || strExt.CompareNoCase ( _T(".asp"	) ) == 0
	   || strPath.Right(1) == _T('/') )
		return true;
	else
		return false;
}



inline LPTSTR _MtlMakeFileOperationBuffer(const CSimpleArray<CString> &arrFiles)
{
	int    nLen 	  = 0;
	int    i;

	for (i = 0; i < arrFiles.GetSize(); ++i)
		nLen += arrFiles[i].GetLength() + 1;

	nLen += 1;

	LPTSTR lpsz 	  = new TCHAR[nLen];
	::memset(lpsz, 0, nLen);

	LPTSTR lpszRunner = lpsz;
	LPTSTR	pEnd	= lpsz + nLen;
	for (i = 0; i < arrFiles.GetSize(); ++i) {
		::_tcscpy_s(lpszRunner, (pEnd - lpszRunner) / sizeof(TCHAR), arrFiles[i]);
		lpszRunner += arrFiles[i].GetLength() + 1;
	}

	return lpsz;
}



inline bool MtlCopyFile(const CString &strTo, const CSimpleArray<CString> &arrFiles)
{
	LPTSTR		   lpszFrom = _MtlMakeFileOperationBuffer(arrFiles);

	CString 	   strTo_	= strTo + _T('\0');
	SHFILEOPSTRUCT fop		= {
		NULL,		   FO_COPY, lpszFrom, strTo_,
		FOF_ALLOWUNDO, FALSE,	NULL,	  NULL
	};

	bool		   bOk		= (::SHFileOperation(&fop) == 0);

	delete[] lpszFrom;
	return bOk;
}



inline bool MtlMoveFile(const CString &strTo, const CSimpleArray<CString> &arrFiles)
{
	CString strDir = strTo;
	if (strTo[strTo.GetLength() - 1] != _T('\\'))
		strDir += _T('\\');
	int nCount = arrFiles.GetSize();
	for (int i = 0; i < nCount; ++i) {
		CString strNew = strDir + Misc::GetFileBaseName(arrFiles[i]);
		::MoveFileEx(arrFiles[i], strNew, MOVEFILE_COPY_ALLOWED);
	}
	return true;
#if 0
	LPTSTR		   lpszFrom = _MtlMakeFileOperationBuffer(arrFiles);

	SHFILEOPSTRUCT fop		= {
		NULL,		   FO_MOVE, lpszFrom, strDir,
		FOF_ALLOWUNDO, FALSE,	NULL,	  NULL
	};
	int nError = ::SHFileOperation(&fop);
	bool	bOk	= nError == 0;
	delete[] lpszFrom;
	return bOk;
#endif
}



inline CString MtlGetShortcutLink(const CString &strPath)
{
	if ( !MtlIsExt( strPath, _T(".lnk") ) )
		return strPath;

	CComPtr<IShellLink> 	spShellLink;

	// Create IShellLink Objec
	HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLink, (void **) &spShellLink);
	if ( SUCCEEDED(hr) ) {
		CComQIPtr<IPersistFile> spPersistFile(spShellLink);
		if (spPersistFile) {
			// load shortcut
			USES_CONVERSION;
			hr = spPersistFile->Load(T2COLE(strPath), STGM_READ);
			if ( SUCCEEDED(hr) ) {
				// get link
				TCHAR	szFilePath[MAX_PATH];
				::_tcscpy_s(szFilePath, strPath);
				WIN32_FIND_DATA wfd;
				hr = spShellLink->GetPath(szFilePath, MAX_PATH, &wfd, SLGP_UNCPRIORITY);
				if ( SUCCEEDED(hr) )
					return szFilePath;
			}
		}
	}

	return strPath;
}



inline bool MtlIsFileExtExe(const CString &strPath)
{
	if ( MtlIsExt( strPath, _T(".exe") ) )
		return true;

	CString strLink = MtlGetShortcutLink(strPath);
	return MtlIsExt( strLink, _T(".exe") );
}


#if 1	//+++ 役割を限定.
inline CString MtlGetExt_fromLeft(const CString &strUrl)
{
	if ( strUrl.IsEmpty() )
		return CString();

	int 	nIndex = strUrl.ReverseFind( _T('.') );
	if (nIndex == -1)
		return CString();

  	CString str_   = strUrl.Mid(nIndex + 1);
	if (str_.IsEmpty())
		return CString();
	LPCTSTR	lpsz   = LPCTSTR(str_);
	LPCTSTR lpszRunner;
	for ( lpszRunner = lpsz; *lpszRunner != _T('\0'); lpszRunner = ::CharNext(lpszRunner) ) {
		if ( !Mtl_isalpha(*lpszRunner) )
			break;
	}
	return strUrl.Mid( nIndex + 1, int (lpszRunner - lpsz) );
}
#else
inline CString MtlGetExt(const CString &strUrl, bool bFromLeft)
{
	if ( strUrl.IsEmpty() )
		return CString();

	int 	nIndex = -1;

	if (bFromLeft) {
		int nIndexName = strUrl.ReverseFind( _T('/') );

		if (nIndexName == -1)
			nIndexName = strUrl.ReverseFind( _T('\\') );

		if (nIndexName == -1)
			return CString();

		nIndex = __MtlFindChar( strUrl, nIndexName, _T('.') );
	} else {
		nIndex = strUrl.ReverseFind( _T('.') );
	}

	if (nIndex == -1)
		return CString();

	CString str_   = strUrl.Mid(nIndex + 1);

	int 	nLen   = str_.GetLength() + 1;
	LPTSTR	lpsz   = (LPTSTR) _alloca( nLen * sizeof (TCHAR) );
	::lstrcpy(lpsz, str_);

	LPCTSTR lpszRunner;

	for ( lpszRunner = lpsz; *lpszRunner != _T('\0'); lpszRunner = ::CharNext(lpszRunner) ) {
		if ( !Mtl_isalpha(*lpszRunner) )
			break;
	}

	return strUrl.Mid( nIndex + 1, int (lpszRunner - lpsz) );
}
#endif



inline void _MtlRemoveHeaderNonAlpha(CString &str)
{
	if ( str.IsEmpty() )
		return;

	int 	nLen = str.GetLength() + 1;
	LPTSTR	lpsz = (LPTSTR) _alloca( nLen * sizeof (TCHAR) );
	::_tcscpy_s(lpsz, nLen, str);

	LPCTSTR lpszRunner;

	for ( lpszRunner = lpsz; *lpszRunner != _T('\0'); lpszRunner = ::CharNext(lpszRunner) ) {
		if ( Mtl_isalpha(*lpszRunner) )
			break;
	}

	str = str.Mid( int (lpszRunner - lpsz) );
}



inline bool _MtlFindExtFromArray(const CSimpleArray<CString> &arrExt, const CString &strExt)
{
	if ( strExt.IsEmpty() )
		return false;

	for (int i = 0; i < arrExt.GetSize(); ++i) {
		if (strExt.CompareNoCase(arrExt[i]) == 0)
			return true;
	}

	return false;
}



inline CString _MtlGetTrailingSeparator(const CString &strUrl)
{
	int nIndex =  strUrl.ReverseFind( _T('/') );
	if (nIndex == strUrl.GetLength() - 1)
		return _T('/');

	nIndex = strUrl.ReverseFind( _T('\\') );
	if (nIndex == strUrl.GetLength() - 1)
		return _T('\\');

	return CString();
}



inline void MtlBuildExtArray(CSimpleArray<CString> &arrExt, const CString &strExts)
{
	int nFirst = 0;
	int nLast  = 0;

	while ( nFirst < strExts.GetLength() ) {
		nLast  = __MtlFindChar( strExts, nFirst, _T(';') );

		if (nLast == -1)
			nLast = strExts.GetLength();

		CString strExt = strExts.Mid(nFirst, nLast - nFirst);

		if (!strExt.IsEmpty() && arrExt.Find(strExt) == -1) {
			//			clbTRACE(_T("_BuildExtsArray.Add(%s)\n"), strExt);
			arrExt.Add(strExt);
		}

		nFirst = nLast + 1;
	}
}



inline void MtlBuildUrlArray(CSimpleArray<CString> &arrUrl, const CSimpleArray<CString> &arrExt, const CString &strText_)
{
	int 	nFirst	= 0;
	int 	nLast	= 0;

	CString strText = strText_;

	strText.Replace( _T("\r"), _T("") );

	while ( nFirst < strText.GetLength() ) {
		nLast = __MtlFindChar( strText, nFirst, _T('\n') );

		if (nLast == -1)
			nLast = strText.GetLength();

		CString strPart = strText.Mid(nFirst, nLast - nFirst);

		if ( !strPart.IsEmpty() ) {
			//+++ CString strExt = MtlGetExt(strPart, true);
			CString strExt = Misc::GetFileExt(strPart);
			CString strUrl;

			if ( _MtlFindExtFromArray(arrExt, strExt) ) {
				// clbTRACE(_T("_BuildUrlArray.Add(%s)\n"), strPart);
				strUrl = strPart;
			} else {
				//+++ strExt = MtlGetExt(strPart, false);
				strExt = MtlGetExt_fromLeft(strPart);

				if ( _MtlFindExtFromArray(arrExt, strExt) ) {
					// clbTRACE(_T("_BuildUrlArray.Add(%s)\n"), strPart);
					strUrl = strPart;
				} else {
					MtlRemoveTrailingSpace(strPart);
					strExt = _MtlGetTrailingSeparator(strPart);

					if ( _MtlFindExtFromArray(arrExt, strExt) ) {
						// clbTRACE(_T("_BuildUrlArray.Add(%s)\n"), strPart);
						strUrl = strPart;
					}
				}
			}

			MtlRemoveTrailingSpace(strUrl);
			_MtlRemoveHeaderNonAlpha(strUrl);

			if ( !strUrl.IsEmpty() ) {
			  #if 0	//+++ きっと tp://や ttp://の補完をすべきなんだろう、で処理を変更.
				Misc::StrToNormalUrl( strUrl );
			  #else	//+++ ひょっとして、これで、http://とかが外されるのまずいのでは?
			  		//+++ と思ったが、表示用に外しているだけのようなんで、違ったon_
				int nFind = strUrl.Find(_T("p://"));
				if (nFind != -1) {
					strUrl = strUrl.Mid(nFind + 4);
				}
			  #endif
				arrUrl.Add(strUrl);
			}
		}

		nFirst = nLast + 1;
	}
}


}		// namespace MTL;


#endif	// __MTLMISC_H__
