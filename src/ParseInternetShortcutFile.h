/**
 *	@file	ParseInternetShortcutFile.h
 *	@brief	インターネットショートカット(.url)からurlを取得. (検索拡張あり)
 *	@note
 *		MtlWeb.cpp にあった MdlParseInternetShortcutFile を、検索関係の拡張を
 *		したもの.
 *		検索バーやアドレスバーを呼び出してて、すでに MTL の範疇外だが、
 *		Mtl::MDITabCtrl からも呼ばれるので、とりあえず MTL のフリをする.
 *		(MDITabCtrlからの呼び方を１クッションおくようにすべき...)
 */

#pragma once

namespace MTL {

///+++ unDonut側の処理を呼びまくるような改変をしたので、MtlWeb.cpp から移動.
bool	ParseInternetShortcutFile(CString &strFilePath);

///+++ リンクバーでドロップされた文字列を検索する場合用.
int 	ParseInternetShortcutFile_SearchMode(CString &strFilePath, const CString& strSearch);

///+++ アドレスバーの文字列を返す.
///    ※ CSearchBar向けに用意. 本来は CDonutAddressBar::の同名関数を呼べばいいだけだが、
/// 	  DonutAddressBar.hをSearchBar.hでincludeすると後がしんどいので...
CString GetAddressBarText();

}	// namespace MTL
