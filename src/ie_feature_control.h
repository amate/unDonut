/** +++
 *	@file	ie_feature_control.h
 *	@brief	Featureコントロール関係の起動時設定.
 */

#pragma once


inline void ie_feature_control_setting()
{
	// 拡張子ではなく内容によって開く、をやめる
	::CoInternetSetFeatureEnabled(FEATURE_MIME_SNIFFING   , SET_FEATURE_ON_PROCESS, TRUE);

	// タブブラウジングを有効
	::CoInternetSetFeatureEnabled(FEATURE_TABBED_BROWSING, SET_FEATURE_ON_PROCESS, TRUE);

	// ポップアップブロックを有効
	//::CoInternetSetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS, TRUE);

	// SSLerrorダイアログを表示させない
	::CoInternetSetFeatureEnabled(FEATURE_SSLUX, SET_FEATURE_ON_PROCESS, FALSE);


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


