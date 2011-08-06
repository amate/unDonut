/**
 *	@file	stdafx.h
 *	@brief	include file for standard system include files,
 *			 or project specific include files that are used frequently, but
 *			are changed infrequently
 */

#if !defined (AFX_STDAFX_H__19D42987_EAF8_11D3_BD32_96A992FCCD39__INCLUDED_)
#define 	  AFX_STDAFX_H__19D42987_EAF8_11D3_BD32_96A992FCCD39__INCLUDED_


// Change these values to use different versions
#ifdef WIN64	//+++ 64ビット版win は winXp64以降のみに対応.
#define WINVER					0x0502
#define _WIN32_WINNT			0x0502
#define _WIN32_IE				0x0603					//+++ _WIN32_IE_IE60SP2
#define _RICHEDIT_VER			0x0100					//+++ 0x200以上(3?)で十分だが、なんとなく
#define DONUT_NAME				_T("64unDonut")
#ifdef NDEBUG
 #define DONUT_WND_CLASS_NAME	_T("WTL:64unDonut") 	//+++ 名前かえるとプラグインとかスクリプトでマズイ?
#else
 #define DONUT_WND_CLASS_NAME	_T("WTL::64unDonut_DEBUG")
#endif
#else		   //+++ 一応、win9xの範囲 //\\2000以降に変更
#define WINVER					0x0502
#define _WIN32_WINNT			0x0502	// XP
#define _WIN32_IE				0x0603
#define _RICHEDIT_VER			0x0100
#define DONUT_NAME				_T("unDonut")
#ifdef NDEBUG
 #define DONUT_WND_CLASS_NAME	_T("WTL:Donut") 		//+++ 名前かえるとプラグインとかスクリプトでマズイ?
#else
 #define DONUT_WND_CLASS_NAME	_T("WTL::Donut_DEBUG")
#endif
#endif

#define ATL_TRACE_CATEGORY		0x00000001
#define ATL_TRACE_LEVEL 		4

#define _WTL_USE_CSTRING
#define _WTL_FORWARD_DECLARE_CSTRING
#define _ATL_USE_CSTRING_FLOAT
#define _CRT_NON_CONFORMING_SWPRINTFS
//#define _ATL_FREE_THREADED
//#define _ATL_SINGLE_THREADED
#define _ATL_APARTMENT_THREADED

#if _ATL_VER < 0x800
#define ATLASSUME(e)			ATLASSERT(e)
#endif


#if 0	// 1 でメモリリークチェック
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// Win32API
#include <windows.h>


// C Standard Library
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#include <string.h>
#include <locale.h>
#include <process.h>
#include <ctype.h>
#include <stdarg.h>
#include <tchar.h>
#include <time.h>
//#include <io.h>

// STL(C++ Standard Library)
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <algorithm>
//#include <functional>
#include <boost/function.hpp>
#include <utility>
#include <iterator>
#include <map>
#include <memory>

using std::vector;
using std::deque;
using std::pair;
using std::make_pair;
using boost::function;
//using namespace std::placeholders;
using std::unique_ptr;


// ATL/WTL
#include <atlbase.h>
#include <atlapp.h>

extern CServerAppModule _Module;						//アプリケーションインスタンス
extern TCHAR			g_szIniFileName[MAX_PATH];		//設定ファイル

#include <atlcom.h>

#include <atlwin.h>
#include <atlctl.h>
#include <atlmisc.h>
#include <atlframe.h>

//\\#include "WtlFixed/atlsplit.h"
#include <atlsplit.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>

#include <atldlgs.h>
#include <atlctrlx.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atldef.h>
//#include <atlsync.h>


#define _WTL_USE_VSSYM32
#include <atltheme.h>	// 改造バージョンを使用しないとダメ

#define USE_AERO
//+++ Aero を使ってみるテスト.
#ifdef USE_AERO
#include <atldwm.h>
#endif

// etc
#include <winerror.h>
#include <winnls32.h>
#include <comdef.h>
//#include <exdisp.h>
//#include <guiddef.h>
//#include <olectl.h>
//#include <rpc.h>
//#include <rpcndr.h>
//#include <rpcproxy.h>
//#include <urlmon.h>


// IEコンポーネントで使う定義
#include <shlobj.h>
#include <wininet.h>
#include <shlwapi.h>
//#include <shlguid.h>
#include <intshcut.h>
#include <MsHTML.h>
#include <mshtmdid.h>
#include <mshtmcid.h>
#include <MsHtmHst.h>
#include <mshtml.h>
#include <tlogstg.h>
#include <urlhist.h>


// XML用
//+++ 使うのはMSXML2の範囲?のようだし、xp64,vista64 には msxml3.dllがでデフォで入っているようなので、3にしてみる.
//#import "msxml4.dll" named_guids	//raw_interfaces_only
#import "msxml3.dll" named_guids	//raw_interfaces_only
using namespace MSXML2;


#include "dbg_wm.h"

#include "DonutPFunc.h"
#include "DonutDefine.h"
#include "Misc.h"
#include "dialog/DebugWindow.h"


#undef min
#undef max
using std::min;
using std::max;



#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。
#endif	// !defined(AFX_STDAFX_H__19D42987_EAF8_11D3_BD32_96A992FCCD39__INCLUDED_)
