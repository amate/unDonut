/** +++
 *	@file	ie_feature_control.h
 *	@brief	Featureコントロール関係の起動時設定.
 */

#ifndef IE_FEATURE_CONTROL_H
#pragma once

#ifndef _WIN32_IE_IE60SP2
#define _WIN32_IE_IE60SP2	0x0603
#endif

#if _WIN32_IE < _WIN32_IE_IE60SP2 && _MSC_VER != 1400 //+++
enum INTERNETFEATURELIST {
	FEATURE_MIME_SNIFFING = 3,
	FEATURE_SECURITYBAND  = 9,
};

enum {
	SET_FEATURE_ON_PROCESS	= 2,
};
#endif



inline HRESULT ie_coInternetSetFeatureEnabled(INTERNETFEATURELIST ifl, DWORD flgs, BOOL f)
{
  #if _WIN32_IE >= _WIN32_IE_IE60SP2	// _MSC_VER >= 1400 //+++
	//x if (_CheckOsVersion_XPLater())
	return ::CoInternetSetFeatureEnabled(ifl, flgs, f);
  #else 		// Win2Kだとurlmon.dllがないので、その対策. そんな頻繁に使う処理じゃないので、速度気にせず手抜き.
	HRESULT hr		= S_FALSE;
	HMODULE hUrlMon = ::LoadLibrary( _T( "urlmon.dll" ) );
	if (hUrlMon) {
		typedef HRESULT (__stdcall *FUNC)(INTERNETFEATURELIST ifl, DWORD flgs, BOOL f);
		FUNC pFunc = (FUNC)::GetProcAddress(hUrlMon, "CoInternetSetFeatureEnabled");
		if (pFunc) {
			hr = (*pFunc)(ifl, flgs, f);
		}
		::FreeLibrary( hUrlMon );
	}
	return hr;
  #endif
}



inline void ie_feature_control_setting()
{
	// 拡張子ではなく内容によって開く、をやめる
	ie_coInternetSetFeatureEnabled(FEATURE_MIME_SNIFFING   , SET_FEATURE_ON_PROCESS, TRUE);

	//::CoInternetSetFeatureEnabled(FEATURE_TABBED_BROWSING, SET_FEATURE_ON_PROCESS, TRUE);
#if 0	//* あとで
	// 情報バー表示。ActivXの実行オフ時ActiveXオブジェクトがある場合等に表示
	bool	sw = _check_flag(MAIN_EX_KILLDIALOG, CMainOption::s_dwMainExtendedStyle);
	ie_coInternetSetFeatureEnabled(FEATURE_SECURITYBAND    , SET_FEATURE_ON_PROCESS, sw);
#endif
#if 0	// ラベルがないので駄目
	// swf を使えるようにするため、スクリプトの制限を緩める.
	ie_coInternetSetFeatureEnabled(FEATURE_BLOCK_LMZ_SCRIPT, SET_FEATURE_ON_PROCESS, FALSE);
#endif
#if 0	//駄目
	//レジストリからWindowsのアップデート情報を取得
	Misc::CRegKey 	reg;
	reg.Open( HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl") );
	reg.SetDWORDValue(_T("unDonut.exe"), 0);
	reg.Close();
#endif
}


#endif	// IE_FEATURE_CONTROL_H
