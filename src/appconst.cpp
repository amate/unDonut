/**
 *	@file	appconst.cpp
 *	@brief	アプリケーション独自のデータを記述するファイル
 */
#include "stdafx.h"
#include "appconst.h"

namespace app {

#define VERSTR			_T("2.00b46_mp")

#ifdef UNICODE
#define MBVER
#else
#define MBVER			_T("-mb")
#endif

#ifdef USE_DIET
#define DIETVER			_T("-DIET")
#else
#define DIETVER
#endif

/// アプリケーション名
const TCHAR *cnt_AppName = DONUT_NAME;	// _T("unDonut");

#if defined _WIN64
/// アプリケーションバージョン
const TCHAR *cnt_AppVersion = VERSTR DIETVER MBVER ;
#else
/// アプリケーションバージョン
//const TCHAR *cnt_AppVersion = _T("release13 test +4 .+mod." VERSTR);
const TCHAR *cnt_AppVersion = /*_T(" +mod.")*/ VERSTR DIETVER MBVER;
#endif



} //namespace app
