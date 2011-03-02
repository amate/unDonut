/**
 *	@file StyleSheetOption.h
 *	@brief	undonutのオプション : スタイルシート
 */

#pragma once

#define KEY_IE_SETTING_USERSTYLESHEET	_T("Software\\Microsoft\\Internet Explorer\\Styles")
#define FIELD_USE_MY_STYLESHEET 		_T("Use My Stylesheet")
#define FIELD_USER_STYLESHEET			_T("User Stylesheet")



class CStyleSheetOption {
	enum { CSS_SETUSERSHEET 			= 0x1, };

public:
	static BOOL 	s_bSetUserSheet;


public:
	static void 	SetUserSheetName(LPCTSTR lpstrFile)
	{
		CString   strFile(lpstrFile);

		if ( strFile.IsEmpty() )
			return;

		Misc::CRegKey   key;
		key.Open(HKEY_CURRENT_USER, KEY_IE_SETTING_USERSTYLESHEET);
		key.SetStringValue(FIELD_USER_STYLESHEET, lpstrFile);

		DWORD_PTR lRet;
		::SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL, SMTO_ABORTIFHUNG, 1000, &lRet);
	}


	static void SetUseUserSheet(bool bOk)
	{
		Misc::CRegKey   key;

		key.Open(HKEY_CURRENT_USER, KEY_IE_SETTING_USERSTYLESHEET);
		key.SetDWORDValue(FIELD_USE_MY_STYLESHEET, bOk != 0/*? 1 : 0*/);

		CString   strBuf;
		DWORD	  dwCount = MAX_PATH;
		LONG	  lRet	  = key.QueryStringValue(FIELD_USER_STYLESHEET, strBuf.GetBuffer(MAX_PATH + 1), &dwCount);
		strBuf.ReleaseBuffer();

		if (lRet != ERROR_SUCCESS) { //フィールドが存在しないと落ちる危険あり
			key.SetStringValue(strBuf, FIELD_USER_STYLESHEET);
		}

		DWORD_PTR dwRet2  = 0;
		::SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL, SMTO_ABORTIFHUNG, 1000, &dwRet2);
	}


	static BOOL GetUseUserSheet()
	{
		Misc::CRegKey 	key;
		DWORD			dwUseUserSheet = 0; 	//+++ 初期値0を付加. pr.QueryValueはエラーだった場合、値を設定しないため、ここで初期値がないと値が不定なまま処理されてしまう.

		key.Open(HKEY_CURRENT_USER, KEY_IE_SETTING_USERSTYLESHEET);
		key.QueryDWORDValue(FIELD_USE_MY_STYLESHEET, dwUseUserSheet);
		return dwUseUserSheet != 0; 	//+++ ? TRUE : FALSE;
	}


	static void GetProfile()
	{
		DWORD		dwSetUserSheet = 0; 	//+++ 初期値0を付加. pr.QueryValueはエラーだった場合、値を設定しないため、ここで初期値がないと値が不定なまま処理されてしまう.

		CIniFileI	pr( g_szIniFileName, _T("StyleSheet") );
		pr.QueryValue( dwSetUserSheet, _T("Style_Flag") );
		pr.Close();

		s_bSetUserSheet = (dwSetUserSheet & CSS_SETUSERSHEET) != 0; /* ? TRUE : FALSE*/;
	}


	static void WriteProfile()
	{
		DWORD			dwFlags = 0;
		CIniFileO		pr( g_szIniFileName, _T("StyleSheet") );

		if (s_bSetUserSheet)
			dwFlags |= CSS_SETUSERSHEET;

		pr.SetValue( dwFlags, _T("Style_Flag") );

		pr.Close();
	}

};



__declspec(selectany) BOOL CStyleSheetOption::s_bSetUserSheet = FALSE;
