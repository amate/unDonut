/**
 *	@file	MtlWeb.cpp
 *	@brief	MTL : WEB関係
 */
// MTL Version 1.01
// Copyright (C) 2000 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Last updated: July 05, 2000

#include "stdafx.h"
#include "MtlWeb.h"
#include "DonutPFunc.h"
#include  "IniFile.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace MTL {


bool MtlGetFavoritesFolder(CString &rString)
{
	TCHAR	strPath[MAX_PATH] = _T("\0");
	HRESULT hr = ::SHGetFolderPath(NULL, CSIDL_FAVORITES, NULL, SHGFP_TYPE_CURRENT, strPath);
	if (SUCCEEDED(hr)) {
		rString = strPath;
		return true;
	} else {
		return false;
	}
#if 0
	Misc::CRegKey	rk;
	LONG			lRet	= rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders") );

	if (lRet != ERROR_SUCCESS)
		return false;

	TCHAR	szRetString[MAX_PATH+1];
	szRetString[0]	= 0;		//+++
	DWORD	dwCount = MAX_PATH;	//+++	 * sizeof (TCHAR);

	lRet	= rk.QueryStringValue( _T("Favorites"), szRetString, &dwCount);
	if (lRet != ERROR_SUCCESS) {
		rk.Close();
		lRet = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders") );
		if (lRet != ERROR_SUCCESS)
			return false;

		lRet = rk.QueryStringValue( _T("Favorites"), szRetString, &dwCount);
		if (lRet != ERROR_SUCCESS) {
			rk.Close();
			return false;
		}
	}

	rk.Close();

	TCHAR	szRetString2[MAX_PATH+1];
	szRetString2[0]	= 0;	//+++
	::ExpandEnvironmentStrings(szRetString, szRetString2, MAX_PATH);
	rString = szRetString2;
	ATLTRACE2(atlTraceGeneral, 4, _T("MTL: MtlGetFavoritesFolder : %s\n"), szRetString2);
	return true;
#endif
}



bool MtlGetHistoryFolder(CString &rString)
{
	TCHAR	strPath[MAX_PATH] = _T("\0");
	HRESULT hr = ::SHGetFolderPath(NULL, CSIDL_HISTORY, NULL, SHGFP_TYPE_CURRENT, strPath);
	if (SUCCEEDED(hr)) {
		rString = strPath;
		return true;
	} else {
		return false;
	}
#if 0
	Misc::CRegKey 	rk;
	LONG			lRet	= rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders") );

	if (lRet != ERROR_SUCCESS)
		return false;

	TCHAR	szRetString[MAX_PATH+1];
	szRetString[0]	= 0;		//+++
	DWORD	dwCount = MAX_PATH;	//+++ * sizeof (TCHAR);

	lRet	= rk.QueryStringValue( _T("History"), szRetString, &dwCount);
	if (lRet != ERROR_SUCCESS) {
		rk.Close();
		lRet = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders") );

		if (lRet != ERROR_SUCCESS)
			return false;

		lRet = rk.QueryStringValue( _T("History"), szRetString, &dwCount);
		if (lRet != ERROR_SUCCESS)
			return false;
	}

	rk.Close();

	TCHAR	szRetString2[MAX_PATH+1];
	szRetString2[0]	=	0;	//+++
	::ExpandEnvironmentStrings(szRetString, szRetString2, MAX_PATH);
	rString 		= szRetString2;
	ATLTRACE2(atlTraceGeneral, 4, _T("MTL: MtlGetFavoritesFolder : %s\n"), szRetString2);
	return true;
#endif
}




bool MtlGetFavoriteLinksFolder(CString &rString)
{	// inspired by DOGSTORE
	if ( !MtlGetFavoritesFolder(rString) )
		return false;

	MtlMakeSureTrailingBackSlash(rString);

	Misc::CRegKey 	rk;
	LONG			lRet = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Toolbar") );
	if (lRet == ERROR_SUCCESS) {
		TCHAR szName[MAX_PATH+1];
		szName[0] 	  = 0;			//+++
		DWORD dwCount = MAX_PATH;	//+++ * sizeof (TCHAR);

		lRet = rk.QueryStringValue( _T("LinksFolderName"), szName, &dwCount);
		if (lRet == ERROR_SUCCESS && Misc::IsExistFile(szName))
		{
			TCHAR szName2[MAX_PATH+1];
			szName2[0]	= 0;	//+++
			::ExpandEnvironmentStrings(szName, szName2, MAX_PATH);
			rString += szName2;
			MtlMakeSureTrailingBackSlash(rString);
			return true;
		}
	}

	if ( _CheckOsVersion_VistaLater() ) {	//+++
		rString += _T("links\\");			//+++
	} else {
		rString += _T("リンク\\");
	}

	return false;
}



bool MtlCreateInternetShortcutFile(const CString &strFileName, const CString &strUrl)
{	// Thanks to <http://www.nt.sakura.ne.jp/~miwaki/>
	CComPtr<IUniformResourceLocator> spUrl;
	HRESULT hr = ::CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_ALL, IID_IUniformResourceLocator, (void **) &spUrl);

	if ( FAILED(hr) )
		return false;

	CComPtr<IPersistFile>			 spPf;
	hr = spUrl->QueryInterface(IID_IPersistFile, (void **) &spPf);

	if ( FAILED(hr) )
		return false;

	hr = spUrl->SetURL(strUrl, 0);

	if ( FAILED(hr) )
		return false;

	USES_CONVERSION;
	hr = spPf->Save(T2COLE( (LPCTSTR) strFileName ), TRUE);

	if ( FAILED(hr) )
		return false;

	return true;
}



// Note. If the third parameter is "CString strPath", it rarely crash. why?
void MtlOrganizeFavorite(HWND hWnd, bool bOldShell, const CString &strPath_)
{
	HINSTANCE hLib;
	// get favorite path
	CString   strPath(strPath_);

	if ( strPath.IsEmpty() )
		MtlGetFavoritesFolder(strPath);

  #if 1	//+++ IE8beta2 で、非互換になったか、バグってるか、で、DoAddToFavDlgW()の挙動が不正なので、別処理にまかせる
  		//+++ vista用ie8だと大丈夫だけど、xp用ie8だとダメみたいなんで、osチェック.
	if (Misc::getIEMejourVersion() >= 8 && _CheckOsVersion_VistaLater() == 0){//\\ XP+IE8の場合
//		bOldShell = true;
		hLib = ::LoadLibrary( _T("ieframe.dll") );
	} else {//vista+IE8の場合
		hLib = ::LoadLibrary( _T("shdocvw.dll") );
	}
  #endif

	if (bOldShell) {
		// easy way
		::ShellExecute(NULL, NULL, strPath, NULL, NULL, SW_SHOWNORMAL);
		return;
	}

//***	HINSTANCE hLib = ::LoadLibrary( _T("shdocvw.dll") );


	if (hLib) {
	  #ifdef UNICODE
		{
			typedef UINT (CALLBACK	 * LPFNORGFAV)(HWND, LPCTSTR);
			LPFNORGFAV lpfnDoOrganizeFavDlg = (LPFNORGFAV) ::GetProcAddress(hLib, "DoOrganizeFavDlgW");
			if (lpfnDoOrganizeFavDlg) {
				(*lpfnDoOrganizeFavDlg)(hWnd, LPCTSTR(strPath));
				::FreeLibrary(hLib);
				return;
			}
		}
	   #if 0	//+++ お試し.. (～W版があったから、これなし)
		//+++ どうやら、vistaなのかie7かはわからないが、DoOrganizeFavDlg の引数が
		//	  char*,wchar_t*どちらでも動作するように対処されている模様.
		//    xp+ie7での挙動が不明なため、とりあえずvista以降のみwchar_t*ということにしとく.
		if (_CheckOsVersion_VistaLater() /*|| getIEMejourVersion() >= 7 */) {
			typedef UINT (CALLBACK	 * LPFNORGFAV)(HWND, LPCTSTR);
			LPFNORGFAV lpfnDoOrganizeFavDlg = (LPFNORGFAV) ::GetProcAddress(hLib, "DoOrganizeFavDlg");
			if (lpfnDoOrganizeFavDlg) {
				(*lpfnDoOrganizeFavDlg)(hWnd, LPCTSTR(strPath));
				::FreeLibrary(hLib);
				return;
			}
		}
	   #endif
	  #endif
	  #if 1	//+++ お試し. どうやら古いosでは、LPTSTRでなくLPSTRでないと駄目の模様.
		{
			typedef UINT (CALLBACK	 * LPFNORGFAV)(HWND, LPSTR);
			LPFNORGFAV lpfnDoOrganizeFavDlg = (LPFNORGFAV) ::GetProcAddress(hLib, "DoOrganizeFavDlg");
			if (lpfnDoOrganizeFavDlg) {
			  #ifdef UNICODE
				std::vector<char> path = Misc::tcs_to_sjis( strPath );
				(*lpfnDoOrganizeFavDlg)(hWnd, &path[0]);
			  #else
				(*lpfnDoOrganizeFavDlg)(hWnd, LPSTR( LPCSTR(strPath) ));
			  #endif
			}
		}
	  #endif
		::FreeLibrary(hLib);
	} else {
		// easy way
		::ShellExecute(NULL, NULL, strPath, NULL, NULL, SW_SHOWNORMAL);
	}
}



bool __MtlAddFavoriteOldShell(const CString &strUrl, const CString &strName_, const CString &strFavDir)
{
	// easy way for old shell users
	TCHAR			   szOldPath[MAX_PATH+1]; // save current directory
	szOldPath[0]		= 0;				//+++
	::GetCurrentDirectory(MAX_PATH, szOldPath);

	::SetCurrentDirectory(strFavDir);

	CString 		strName(strName_);
	MtlValidateFileName(strName);

	static const TCHAR szFilter[] = _T("URLファイル(*.url)\0*.url\0\0");
	CFileDialog 	fileDlg(FALSE, _T("url"), strName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = strFavDir;
	fileDlg.m_ofn.lpstrTitle	  = _T("お気に入りに追加");

	if (fileDlg.DoModal() == IDOK) {
		CString 	strFile(fileDlg.m_szFileName);

		if ( !MtlIsExt( strFile, _T(".url") ) )
			strFile += _T(".url");

		::WritePrivateProfileString(_T("InternetShortcut"), _T("URL"), strUrl, strFile);
	}

	::SetCurrentDirectory(szOldPath);

	return true;
}



bool _MtlAddFavoriteNewShell(const CString &strUrl, const CString &strName)
{
	CComPtr<IShellUIHelper> spShell;
	HRESULT 				hr = spShell.CoCreateInstance(CLSID_ShellUIHelper);

	if ( FAILED(hr) )
		return false;

	CComBSTR				bstrName(strName);
	CComBSTR				bstrUrl(strUrl);
	CComVariant 			vt(bstrName);

	try {
		hr = spShell->AddFavorite(bstrUrl, &vt);
		if ( FAILED(hr) )
			return false;
	} catch(...) {
		return false;
	}
	return true;
}



bool _MtlAddFavoriteModalSucks(const CString &strUrl, const CString &strName_, const CString &strFavPath, HWND hWnd)
{
  #if 1	//+++ Unicode版対応で書き直し.
	CString 	strName(strName_);
	MtlValidateFileName(strName);

   #if 0 //def UNICODE		//+++ 現状呼び元で行ってるので、ここではしちゃだめ.
	if (Misc::IsEnableUnicodeToSJIS(strName) == 0) {
		return __MtlAddFavoriteOldShell(strUrl, strName, strFavPath);
	}
   #endif

	bool		bResult = false;
	//	NOTE. Some buttons can't be validate. I'm fed up with IE.]
	HMODULE 	hMod = (HMODULE) ::LoadLibrary( _T("shdocvw.dll") );
	if (hMod) {
	 #if 1	//+++ どうも DoAddToFavDlgW の使い方がまずいのか?、W版で追加を行うと文字列が長すぎる、というエラーダイアログが出てしまう
			//+++ ...ので、mb版で逃げることに.
		typedef BOOL (CALLBACK	 * LPFNADDFAV)(HWND, LPSTR, UINT, LPSTR, UINT, LPITEMIDLIST);
		LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlg");
		if (lpfnDoAddToFavDlg) {
		  #ifdef UNICODE
			int 		nLen	   = strName.GetLength();
			char		szName[MAX_PATH * 2 + 2];				//+++ 固定バッファに変更
			::ZeroMemory(szName, sizeof szName);
			::lstrcpynA(szName, &Misc::tcs_to_sjis(LPCTSTR(strName))[0], MAX_PATH);

			char		szPath[MAX_PATH+1];
			::ZeroMemory(szPath, sizeof szPath);
			CItemIDList idlFavPath = strFavPath;
			BOOL		bOK 	   = lpfnDoAddToFavDlg(hWnd, szPath, MAX_PATH/*_countof(szPath)*/, szName, nLen, idlFavPath);
			if (bOK)
				MtlCreateInternetShortcutFile(&Misc::sjis_to_tcs(szPath)[0], strUrl);
			bResult = true;
		  #else
			int 		nLen	   = strName.GetLength();
			//LPTSTR	szName	   = (LPTSTR) _alloca( (nLen + 1 + 1) * sizeof (TCHAR) );
			TCHAR		szName[MAX_PATH+1];							//+++ 固定バッファに変更
			::ZeroMemory(szName, sizeof szName);
			::lstrcpyn(szName, LPCTSTR(strName), MAX_PATH);

			TCHAR		szPath[MAX_PATH+1];
			::ZeroMemory(szPath, sizeof szPath);
			CItemIDList idlFavPath = strFavPath;
			BOOL		bOK 	   = lpfnDoAddToFavDlg(hWnd, szPath, MAX_PATH/*_countof(szPath)*/, szName, nLen, idlFavPath);
			if (bOK)
				MtlCreateInternetShortcutFile(szPath, strUrl);
			bResult = true;
		  #endif
		}
		FreeLibrary(hMod);
	 #else
		typedef BOOL (CALLBACK	 * LPFNADDFAV)(HWND, LPTSTR, UINT, LPTSTR, UINT, LPITEMIDLIST);
	   #ifdef UNICODE
		LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlgW");
	   #else
		LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlg");
	   #endif
		if (lpfnDoAddToFavDlg) {
			int 		nLen	   = strName.GetLength();
			//LPTSTR	szName	   = (LPTSTR) _alloca( (nLen + 1 + 1) * sizeof (TCHAR) );
			TCHAR		szName[MAX_PATH+1];							//+++ 固定バッファに変更
			::ZeroMemory(szName, sizeof szName);
			::lstrcpyn(szName, LPCTSTR(strName), MAX_PATH);

			TCHAR		szPath[MAX_PATH+1];
			::ZeroMemory(szPath, sizeof szPath);
			CItemIDList idlFavPath = strFavPath;
			BOOL		bOK 	   = lpfnDoAddToFavDlg(hWnd, szPath, MAX_PATH/*_countof(szPath)*/, szName, nLen, idlFavPath);
			if (bOK)
				MtlCreateInternetShortcutFile(szPath, strUrl);
			bResult = true;
		}
	   #ifdef UNICODE	//+++ 念のため W なしもチェック.
		else {
			typedef BOOL (CALLBACK	 * LPFNADDFAV)(HWND, LPSTR, UINT, LPSTR, UINT, LPITEMIDLIST);
			LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlg");
			if (lpfnDoAddToFavDlg) {
				int 		nLen	   = strName.GetLength();
				//LPSTR		szName	   = (LPSTR) _alloca( (nLen + 1 + 1) * sizeof (TCHAR) );
				char		szName[MAX_PATH * 2];					//+++ 固定バッファに変更
				::ZeroMemory(szName, sizeof szName);
				::lstrcpynA(szName, &Misc::tcs_to_sjis(LPCTSTR(strName))[0], MAX_PATH);

				char		szPath[MAX_PATH+1];
				::ZeroMemory(szPath, sizeof szPath);
				CItemIDList idlFavPath = strFavPath;
				BOOL		bOK 	   = lpfnDoAddToFavDlg(hWnd, szPath, MAX_PATH/*_countof(szPath)*/, szName, nLen, idlFavPath);
				if (bOK)
					MtlCreateInternetShortcutFile(&Misc::sjis_to_tcs(szPath)[0], strUrl);

				bResult = true;
			}
		}
	   #endif
		FreeLibrary(hMod);
	 #endif
	}

	return bResult;
  #else		//+++
	CString 	strName(strName_);
	MtlValidateFileName(strName);

	bool		bResult = false;
	//	NOTE. Some buttons can't be validate. I'm fed up with IE.]
	HMODULE 	hMod = (HMODULE) ::LoadLibrary( _T("shdocvw.dll") );

	if (hMod) {
		typedef BOOL (CALLBACK	 * LPFNADDFAV)(HWND, LPTSTR, UINT, LPTSTR, UINT, LPITEMIDLIST);
		//typedef BOOL (CALLBACK * LPFNADDFAV)(HWND, LPTSTR, UINT, LPCTSTR, UINT, LPITEMIDLIST);
	  #ifdef UNICODE
		LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlgW");
	  #else
		LPFNADDFAV lpfnDoAddToFavDlg = (LPFNADDFAV) ::GetProcAddress(hMod, "DoAddToFavDlg");
	  #endif

		if (lpfnDoAddToFavDlg) {
		  #if 0	//+++ 一時バッファへコピーする必要ないようにみえるので、省いてみる... と思ったら書き換えられちゃうのね(T T)
			int 		nLen	   = strName.GetLength();
			LPCTSTR		szName	   = LPCTSTR(strName);
		  #else
			int 		nLen	   = strName.GetLength();
			//LPTSTR	szName	   = (LPTSTR) _alloca( (nLen + 1 + 1) * sizeof (TCHAR) );
			TCHAR		szName[MAX_PATH];							//+++ 固定バッファに変更
			::lstrcpyn(szName, LPCTSTR(strName), MAX_PATH);
		  #endif
			TCHAR		szPath[MAX_PATH];
			::ZeroMemory(szPath, sizeof szPath);
			CItemIDList idlFavPath = strFavPath;
			BOOL		bOK 	   = lpfnDoAddToFavDlg(hWnd, szPath, _countof(szPath), szName, nLen, idlFavPath);
			if (bOK)
				MtlCreateInternetShortcutFile(szPath, strUrl);

			bResult = true;
		}

		FreeLibrary(hMod);
	}

	return bResult;
  #endif
}



bool MtlAddFavorite(const CString &strUrl, const CString &strName, bool bOldShell, const CString &strFavDir_, HWND hWnd)
{	// how complex...
	CString 	strStdFavDir;

	if ( !MtlGetFavoritesFolder(strStdFavDir) )
		return false;

	CString strFavDir(strFavDir_);

	if ( strFavDir.IsEmpty() ) {
		strFavDir = strStdFavDir;
	}

	if (bOldShell) {
		return __MtlAddFavoriteOldShell(strUrl, strName, strFavDir);
	}

	bool	bStdFavDir = (strStdFavDir == strFavDir);
  #if 0	//+++ IE8beta2 で、非互換になったか、バグってるか、で、DoAddToFavDlgW()の挙動が不正なので、別処理にまかせる
	if (Misc::getIEMejourVersion() >= 8) {
		bStdFavDir = 0;
		//return __MtlAddFavoriteOldShell(strUrl, strName, strFavDir);
	}
  #endif

	if (bStdFavDir) {
		if ( !_MtlAddFavoriteNewShell(strUrl, strName) ) {
			//+++ お試しで、_MtlAddFavoriteModalSucksを使うようにしてみる...
			if (
			  #ifdef UNICODE
				Misc::IsEnableUnicodeToSJIS(strName) == 0 ||
			  #endif
				!_MtlAddFavoriteModalSucks(strUrl, strName, strFavDir, hWnd)
			) {
				return __MtlAddFavoriteOldShell(strUrl, strName, strFavDir);
			}
		}
	} else {
		if (
		  #ifdef UNICODE
			Misc::IsEnableUnicodeToSJIS(strName) == 0 ||
		  #endif
			!_MtlAddFavoriteModalSucks(strUrl, strName, strFavDir, hWnd)
		) {
			return __MtlAddFavoriteOldShell(strUrl, strName, strFavDir);
		}
	}

	return false;
}



// cf.MSDN "Offline Browsing"
// Bug: Even if an user is working-Offline before your application runs,
//		this function always returns FALSE.
bool MtlIsGlobalOffline()
{
	DWORD	dwState = 0;
	DWORD	dwSize	= sizeof (DWORD);
	bool	bRet	= false;

	if ( ::InternetQueryOption(NULL, INTERNET_OPTION_CONNECTED_STATE, &dwState, &dwSize) ) {
		if (dwState & INTERNET_STATE_DISCONNECTED_BY_USER)
			bRet = true;
	}

	return bRet;
}



// cf.MSDN "Offline Browsing"
void MtlSetGlobalOffline(bool bGoOffline)
{
	if (MtlIsGlobalOffline() == bGoOffline)
		return;

	INTERNET_CONNECTED_INFO ci;
	memset( &ci, 0, sizeof (ci) );

	if (bGoOffline) {
		ci.dwConnectedState = INTERNET_STATE_DISCONNECTED_BY_USER;
		ci.dwFlags			= ISO_FORCE_DISCONNECTED;
	} else {
		ci.dwConnectedState = INTERNET_STATE_CONNECTED;
	}

	::InternetSetOption( NULL, INTERNET_OPTION_CONNECTED_STATE, &ci, sizeof (ci) );
}



#if (_WIN32_IE >= 0x0500)
bool MtlAutoComplete(HWND hWnd)
{
	bool	  bRet = false;
	HINSTANCE hLib = ::LoadLibrary( _T("shlwapi.dll") );

	if (hLib) {
		typedef HRESULT (CALLBACK	* LPFNSHAUTOCOMPLETE)(HWND hWnd, DWORD dw);
		LPFNSHAUTOCOMPLETE lpfnSHAutoComplete = (LPFNSHAUTOCOMPLETE) ::GetProcAddress(hLib, "SHAutoComplete");

		if (lpfnSHAutoComplete) {
			HRESULT hr = (*lpfnSHAutoComplete)(hWnd, SHACF_FILESYSTEM | SHACF_URLALL | SHACF_USETAB);

			if ( SUCCEEDED(hr) )
				bRet = true;
		}

		::FreeLibrary(hLib);
	}

	return bRet;
}
#endif




void MtlShowInternetOptions()
{	// this is modeless
	::ShellExecute(0, _T("open"), _T("control.exe"), _T("inetcpl.cpl"), _T("."), SW_SHOW);
}



bool MtlCreateShortCutFile (
		const CString&	strFile,
		const CString&	strTarget,
		const CString&	strDescription,
		const CString&	strArgs,
		const CString&	strWorkingDir,
		const CString&	strIconPath,
		int 			iIcon,
		int 			iShowCmd )		// ウィンドウスタイル
{										// Thanks to <http://www.nt.sakura.ne.jp/~miwaki/>
	if ( MtlIsExt( strTarget, _T(".url") ) || MtlIsExt( strTarget, _T(".lnk") ) ) {
		// if target is .lnk or .url, just copy!!
		CString 			  strFolder  = MtlGetDirectoryPath(strFile, true);
		CString 			  strTarget_ = strTarget;
		CSimpleArray<CString> arrFiles;
		arrFiles.Add(strTarget_);
		return MtlCopyFile(strFolder, arrFiles);
	}

	CComPtr<IShellLink> 	spLink;
	HRESULT 				hr	= CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLink, (void **) &spLink);

	if ( FAILED(hr) )
		return false;

	CComPtr<IPersistFile> spFile;
	hr = spLink->QueryInterface(IID_IPersistFile, (void **) &spFile);

	if ( FAILED(hr) )
		return false;

	spLink->SetPath(strTarget);

	if ( !strDescription.IsEmpty() )
		spLink->SetDescription(strDescription);

	if ( !strArgs.IsEmpty() )
		spLink->SetArguments(strArgs);

	if ( !strWorkingDir.IsEmpty() )
		spLink->SetWorkingDirectory(strWorkingDir);

	if ( !strIconPath.IsEmpty() )
		spLink->SetIconLocation(strIconPath, iIcon);

	spLink->SetShowCmd(iShowCmd);

	USES_CONVERSION;
	hr = spFile->Save(T2COLE( (LPCTSTR) strFile ), TRUE);

	if ( FAILED(hr) )
		return false;

	return true;
}




CString MtlGetInternetShortcutUrl(const CString &strFile)
{
  #if 0	//+++ .url を WritePrivateProfileで直接書き換えると、どうも、こっちのやり方では、変更が反映されてないみたい...
	if ( !MtlIsExt( strFile, _T(".url") ) )  // to avoid the crash, fixed by DOGSTORE
		return CString();
	CString 	strResult;
	CComPtr<IUniformResourceLocator>	spUrl;
	HRESULT 	hr = CoCreateInstance (
						CLSID_InternetShortcut,
						NULL,
						CLSCTX_INPROC_SERVER,
						IID_IUniformResourceLocator,
						(void **) &spUrl );

	if ( FAILED(hr) )
		return strResult;

	CComPtr<IPersistFile>			 spFile;
	hr = spUrl->QueryInterface(IID_IPersistFile, (void **) &spFile);

	if ( FAILED(hr) )
		return strResult;

	// Load the Internet Shortcut from persistent storage.
	USES_CONVERSION;
	hr = spFile->Load(T2COLE(strFile), STGM_READ);
	if ( FAILED(hr) )
		return strResult;

	LPTSTR		pszUrl = NULL;
	hr = spUrl->GetURL(&pszUrl);

	if ( SUCCEEDED(hr) ) {
		strResult = pszUrl;
		::CoTaskMemFree(pszUrl);
	}

	return strResult;
  #else		//+++ .url を直接弄った場合対策で、ここも GetPrivateProfile経由に変えてみる...
	if ( MtlIsExt( strFile, _T(".url") ) ) {
		CIniFileI	pr(strFile, _T("InternetShortcut"));
		return pr.GetString(_T("URL"));
	}
	return CString();
  #endif
}



void _MtlExecuteWithoutDDE(CString &strPath, const CString &strArg)
{
	STARTUPINFO 		si = { 0 };
	PROCESS_INFORMATION pi;
	CString 			strCom;

	si.cb	= sizeof ( si );
	strCom	= strPath + strArg;

	if (CreateProcess( NULL, (LPTSTR) (LPCTSTR) strCom, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ) != 0) {
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
	}
}



static bool OpenLnkFile(CString &strPath, const CString &strArg);


/** ファイルを開けるとき、http:以外の場合の処理.
 */
bool MtlPreOpenFile(CString &strPath, const CString &strArg)
{
	ATLTRACE( _T("MtlPreOpenFile(%s)\n"), strPath.Left(100) );

	if ( MtlIsExt( strPath, _T(".exe") ) ) {
		_MtlExecuteWithoutDDE(strPath, strArg);
		return true;		// handled
	}

	if ( MtlIsExt( strPath, _T(".lnk") ) )
		return OpenLnkFile(strPath, strArg);		//+++ 関数に分離.

	return false;		// not handled
}



/** .lnkファイルを開ける
 */
static bool OpenLnkFile(CString &strPath, const CString &strArg)
{
	CComPtr<IShellLink> 	spShellLink;
	// Create IShellLink Objec
	HRESULT 	hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLink, (void **) &spShellLink);
	if ( FAILED(hr) )
		return false;

	CComQIPtr<IPersistFile> spPersistFile(spShellLink);

	if (spPersistFile) {
		// load shortcut
		USES_CONVERSION;
		hr = spPersistFile->Load(T2COLE(strPath), STGM_READ);

		if ( SUCCEEDED(hr) ) {
			// get link
			TCHAR			szFilePath[MAX_PATH];
			szFilePath[0]	= 0;	//+++
			WIN32_FIND_DATA wfd;
			hr		= spShellLink->GetPath(szFilePath, MAX_PATH, &wfd, SLGP_UNCPRIORITY);

			if ( FAILED(hr) )
				return false;

			if ( !MtlIsExt( szFilePath, _T(".exe") ) ) {
				strPath = szFilePath;
				return false;
			}

			TCHAR	szCmdLine[2048];
			szCmdLine[0] = _T('\0');
			hr			 = spShellLink->GetArguments(szCmdLine, 2048);

			if ( !strArg.IsEmpty() )
				::lstrcpyn(szCmdLine, strArg, 2048);

			int 	nShowCmd = SW_SHOWNORMAL;
			hr			 = spShellLink->GetShowCmd(&nShowCmd);

			TCHAR	szDirPath[MAX_PATH];
			szDirPath[0] = _T('\0');
			hr			 = spShellLink->GetWorkingDirectory(szDirPath, 2048);

			::ShellExecute(NULL, _T("open"), szFilePath, szCmdLine, szDirPath, nShowCmd);
			return true;	// handled
		}
	}

	return false;
}



CString MtlMakeFavoriteToolTipText(const CString &strName_, const CString &strUrl_, int cchTextMax)
{
	CString strName = MtlCompactString(strName_, cchTextMax);

	if ( strUrl_.IsEmpty() )
		return strName;

	int 	nCount	= cchTextMax - strName.GetLength() - 1; 	// consider _('\n') too
	if (nCount <= 0)
		return strName;

	CString strUrl;
	bool	bRet	= MtlCompactPath(strUrl, strUrl_, nCount);

	if (!bRet || strName == strUrl)
		return strName;
	else
		return strName + _T('\n') + strUrl;
}



// I thought this is the best, but it seems that
// idls which means the same path don't surely have the same bits.
//typedef CSimpleMapInt< CAdapt<CItemIDList> > CFavoritesOrder;


BOOL MtlGetFavoritesOrder(CFavoritesOrder &order, const CString &strDirPath)
{
	ATLASSERT( !strDirPath.IsEmpty() );
	const int s_unknownOffset = 12;

  #if 1 //+++ Vistaでのリンクバー対策...
	CString   strKeyDir 	  = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites");
	CString   strRoot;
	if ( !MtlGetFavoritesFolder(strRoot) )
		return FALSE;
	MtlMakeSureTrailingBackSlash(strRoot);
	int 	  len			  = strDirPath.GetLength() - strRoot.GetLength();
	CString   strRelative;
	if (len >= 0)
		strRelative 		  = strDirPath.Right( len );
	MtlRemoveTrailingBackSlash(strRelative);
	CString   strKeyName	  = strKeyDir;
  #else
	CString   strRoot;
	if ( !MtlGetFavoritesFolder(strRoot) )
		return FALSE;

	MtlMakeSureTrailingBackSlash(strRoot);
	CString   strKeyName	  = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites");
	CString   strRelative	  = strDirPath.Right( strDirPath.GetLength() - strRoot.GetLength() );

  #endif

	if ( !strRelative.IsEmpty() )
		strKeyName += _T('\\') + strRelative;
	MtlRemoveTrailingBackSlash(strKeyName);
	//	ATLTRACE(_T(" strKeyName(%s)\n"), strKeyName);

	Misc::CRegKey   rkOrder;
	LONG			lRet	= rkOrder.Open(HKEY_CURRENT_USER, strKeyName);
	if (lRet != ERROR_SUCCESS) {
	  #if 1 //+++ Vistaでのリンクバー対策...
		//+++ Vista以前や、ユーザー定義フォルダだった場合は、終わり.
		if ( _CheckOsVersion_VistaLater() == 0)
			return FALSE;
		CString 	strLnkDir;
		MtlGetFavoriteLinksFolder(strLnkDir);	//+++ 現在のリンクフォルダの位置を取得.
		if (strLnkDir.IsEmpty())
			return FALSE;
		MtlRemoveTrailingBackSlash( strLnkDir );
		if ( strLnkDir.CompareNoCase( strDirPath ) != 0 )		//+++ ユーザー定義フォルダだったら帰る
			return FALSE;
		//+++ vistaの場合、ディスク中のフォルダ名は links なのに リポジトリ中では "リンク" で設定されている....
		MtlRemoveTrailingBackSlash(strLnkDir);
		if (Misc::GetFileBaseName(strLnkDir).CompareNoCase(_T("Links")) != 0)
			return FALSE;
		strKeyName = strKeyDir + _T("\\リンク");
		lRet	   = rkOrder.Open(HKEY_CURRENT_USER, strKeyName);
		if (lRet != ERROR_SUCCESS)
			return FALSE;
	  #else
		return FALSE;
	  #endif
	}

	DWORD	  dwSize		  = 0;
	DWORD	  dwType		  = REG_BINARY;
	lRet = ::RegQueryValueEx(rkOrder, _T("Order"), NULL, &dwType, NULL, &dwSize);
	if (lRet != ERROR_SUCCESS || dwSize < s_unknownOffset)
		return FALSE;

	BYTE *	  pByte 		  = (BYTE *) _alloca( (dwSize + 10) * sizeof (BYTE) );
	::memset(pByte, 0, dwSize + 10);
	lRet = ::RegQueryValueEx(rkOrder, _T("Order"), NULL, &dwType, pByte, &dwSize);
	if (lRet != ERROR_SUCCESS)
		return FALSE;

	BYTE *	  pBegin		  = pByte + ( (_CFavoritesOrderData *) pByte )->size + s_unknownOffset;
	BYTE *	  pEnd			  = pByte + dwSize;

	while (pBegin < pEnd) {
		_CFavoritesOrderData *pData   = (_CFavoritesOrderData *) pBegin;
		CItemIDList 		  idl(&pData->idl);
		CString 			  strName = MtlGetFileName( idl.GetPath() );

		if ( !strName.IsEmpty() ) {
			//			ATLTRACE(_T("order(%s, %d)\n"), strName, pData->priority);
			order.Add(strName, pData->priority);
		}

		pBegin += pData->size;
	}

	return TRUE;
}




// Based on JOBBY's code
DWORD MtlGetInternetZoneActionPolicy(DWORD dwAction)
{
	// create instance IInternetZoneManager
	CComPtr<IInternetZoneManager> spZoneMgr;

	HRESULT 	hr	   = CoCreateInstance(CLSID_InternetZoneManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetZoneManager, (void **) &spZoneMgr);
	if ( FAILED(hr) )
		return false;

	// get security policy
	DWORD		dwPolicy = DWORD(-1);
	hr = spZoneMgr->GetZoneActionPolicy(URLZONE_INTERNET, dwAction, (BYTE *) &dwPolicy, sizeof (dwPolicy), URLZONEREG_DEFAULT);
	if ( FAILED(hr) )
		return false;

	return dwPolicy;
}



bool MtlIsInternetZoneActionPolicyDisallow(DWORD dwAction)
{
	if (MtlGetInternetZoneActionPolicy(dwAction) == URLPOLICY_DISALLOW)
		return true;
	else
		return false;
}



bool MtlSetInternetZoneActionPolicy(DWORD dwAction, DWORD dwPolicy)
{
	// create instance IInternetZoneManager
	CComPtr<IInternetZoneManager> spZoneMgr;

	HRESULT hr = CoCreateInstance(CLSID_InternetZoneManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetZoneManager, (void **) &spZoneMgr);
	if ( FAILED(hr) )
		return false;

	// change security policy
	hr	= spZoneMgr->SetZoneActionPolicy(URLZONE_INTERNET, dwAction, (BYTE *) &dwPolicy, sizeof (dwPolicy), URLZONEREG_DEFAULT);
	if ( FAILED(hr) )
		return false;

	// set security level custom
	// now the method is applied
	CVersional<ZONEATTRIBUTES>	  ZoneAttrib;

	hr	= spZoneMgr->GetZoneAttributes(URLZONE_INTERNET, &ZoneAttrib);
	if ( FAILED(hr) )
		return false;

	ZoneAttrib.dwTemplateCurrentLevel = URLTEMPLATE_CUSTOM;

	hr	= spZoneMgr->SetZoneAttributes(URLZONE_INTERNET, &ZoneAttrib);
	if ( FAILED(hr) )
		return false;

	return true;
}



// not tested yet...
bool MtlDeleteAllCookies()
{
	return (::InternetSetOption(0, INTERNET_OPTION_END_BROWSER_SESSION, 0, 0) == TRUE);
}



bool __MtlSkipChar(const CString &str, int &nIndex, TCHAR ch)
{
	ATLASSERT( nIndex < str.GetLength() );

	if (str[nIndex] == ch)
		++nIndex;

	if ( nIndex >= str.GetLength() )
		return false;
	else
		return true;
}



void _MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText)
{
	int nLast  = -1;
	int nFirst = 0;
	int nLen   = strHtmlText.GetLength();

	while (nFirst != -1 && nFirst < nLen) {
		nFirst	= __MtlFind( strHtmlText, nFirst, _T("href") );
		nFirst += 4;

		if (nFirst >= nLen)
			break;

		if ( !MTL::__MtlSkipChar( strHtmlText, nFirst, _T(' ') ) )
			break;



		if ( strHtmlText[nFirst] == _T('=') )			// skip a '='
			++nFirst;
		else
			break;										// illegal

		if (nFirst >= nLen)
			break;

		if ( !MTL::__MtlSkipChar( strHtmlText, nFirst, _T(' ') ) )
			break;

		if ( strHtmlText[nFirst] == _T('\"') ) {		// if "
			++nFirst;
			nLast = __MtlFindChar( strHtmlText, nFirst, _T('\"') );

			if (nLast == -1)
				break;									// illegal

			CString str = strHtmlText.Mid(nFirst, nLast - nFirst);
			arrUrl.Add(str);
		} else {
			nLast = __MtlFindChar( strHtmlText, nFirst, _T(' ') );		// first, find a space
			if (nLast != -1) {
				CString str = strHtmlText.Mid(nFirst, nLast - nFirst);
				arrUrl.Add(str);
			} else {
				nLast = __MtlFindChar( strHtmlText, nFirst, _T('>') );	// second, find a '>'
				if (nLast != -1) {
					CString str = strHtmlText.Mid(nFirst, nLast - nFirst);
					arrUrl.Add(str);
				} else {								// illegal
					break;
				}
			}
		}

		nFirst = nLast;
	}
}



CString _MtlGetDirectoryPathFixed(const CString &strPath, bool bAddBackSlash)
{
	bool	bSlash = false;
	int 	nIndex = strPath.ReverseFind( _T('\\') );	// first

	if (nIndex == -1) {
		nIndex = strPath.ReverseFind( _T('/') );
		bSlash = true;
	}

	CString strRet = strPath.Left(nIndex);

	if (bAddBackSlash) {
		if (bSlash)
			MtlMakeSureTrailingChar( strRet, _T('/') );
		else
			MtlMakeSureTrailingChar( strRet, _T('\\') );
	}

	return strRet;
}



CString _MtlGetRootDirectoryPathFixed(const CString &strPath, bool bAddBackSlash)
{
	bool	bSlash = false;

	int 	nIndex = strPath.Find( _T(":\\") ); 		// first

	if (nIndex != -1) {
		nIndex += 2;
	} else {											// second
		nIndex = strPath.Find( _T("://") );
		bSlash = true;

		if (nIndex != -1)
			nIndex += 3;
	}

	TCHAR	chSep;

	if (bSlash)
		chSep = _T('/');
	else
		chSep = _T('\\');

	if (nIndex > 0)
		nIndex = __MtlFindChar(strPath, nIndex, chSep);

	if (nIndex == -1) { 								// if not found, whole string is the root path.
		nIndex = strPath.GetLength();
	}

	CString strRet = strPath.Left(nIndex);

	if (bAddBackSlash) {
		MtlMakeSureTrailingChar(strRet, chSep);
	}

	return strRet;
}



bool _MtlIsIllegalRootSlash(const CString &str)
{
	if (str.GetLength() < 1)
		return false;

	if ( str[0] == _T('/') || str[0] == _T('\\') )
		return true;
	else
		return false;
}



bool _MtlIsMailTo(const CString &str)
{
	if ( str.Left(7) == _T("mailto:") )
		return true;
	else
		return false;
}



bool _MtlIsHrefID(const CString &str)
{
	if (str.GetLength() < 1)
		return false;

	if ( str[0] == _T('#') )
		return true;
	else
		return false;
}



CString _MtlRemoveIDFromUrl(const CString &str)
{
	int nIndex = str.ReverseFind( _T('#') );

	if (nIndex != -1)
		return str.Left(nIndex);
	else
		return str;
}



bool _MtlIsRelativePath(const CString &str)
{
	return (str.Find( _T("://") ) == -1) && (str.Find( _T(":\\") ) == -1) && !_MtlIsMailTo(str);
}



void MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText, const CString &strLocationUrl)
{
	CSimpleArray<CString>	_arrUrl;
	_MtlCreateHrefUrlArray(_arrUrl, strHtmlText);

	CString 	strDirectory = _MtlGetDirectoryPathFixed(strLocationUrl, true);
	CString 	strRoot 	 = _MtlGetRootDirectoryPathFixed(strLocationUrl, true);

	for (int i = 0; i < _arrUrl.GetSize(); ++i) {
		CString 	strUrl = _arrUrl[i];
		CString 	strDir = strDirectory;

		if ( _MtlIsHrefID(strUrl) ) {				// href = #xxx
			strUrl = _MtlRemoveIDFromUrl(strLocationUrl) + strUrl;
		} else {
			CString 	strURL = strUrl;
			strURL.MakeUpper();

			if ( strURL.Left(4) == _T("HTTP") ) {
				strUrl = strUrl;
			} else if (::isalnum(strUrl[0]) != 0) {
				strUrl = strDirectory + strUrl;
			} else {
				for (int ii = 0; ii < 50 && (::isalnum(strUrl[0]) == 0); ii++) {
					int 	nFindIt = strUrl.Find(_T('/'));

					if (nFindIt == 2) {
						strDir = strDir.Left(strDir.GetLength() - 1);
						strDir = strDir.Left(strDir.ReverseFind(_T('/')) + 1);
					} else if (nFindIt == 0) {		//minit
						strDir = strRoot;
					}

					strUrl = strUrl.Mid(nFindIt + 1);
				}

				strUrl = strDir + strUrl;
			}
		}

		strUrl.Replace( _T("&amp;") , _T("&") );	// Replace &amp;
		strUrl.Replace( _T('\\'), _T('/') );
		arrUrl.Add(strUrl);
	}
}



}	//namespace MTL
