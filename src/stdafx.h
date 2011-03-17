/**
 *	@file	stdafx.h
 *	@brief	include file for standard system include files,
 *			 or project specific include files that are used frequently, but
 *			are changed infrequently
 */

#if !defined (AFX_STDAFX_H__19D42987_EAF8_11D3_BD32_96A992FCCD39__INCLUDED_)
#define 	  AFX_STDAFX_H__19D42987_EAF8_11D3_BD32_96A992FCCD39__INCLUDED_


#if 1	//+++	デバッグ用
 #define USE_ZEROFILL_NEW				//+++ 手抜きで 0 クリアをする new を使う. まだはずさないほうがよさそう...
 //#define USE_DIET
 //#define USE_DLMALLOC
 //#define USE_ORG_UNDONUT_INI			//+++ unDonut+ から変わってしまった .ini や拡張プロパティの値をなるべく、オリジナルのunDonutにあわせる場合に定義.
 //#define USE_MEMORYLOG 				//+++ donutでのnew,deleteログ生成.
 //#define USE_ATL3_BASE_HOSTEX			//+++ about:blankがらみのバグのデバッグで用意. 突き止めたのでatl3用以外で定義する必要なし.
 //x #define USE_UNDONUT_G_SRC 			//+++ gae氏のunDonut_g 2006-08-05 の公開ソースより移植した部分を有効にしてみる.(お試し) ...デフォルトで反映しとくのでラベルは破棄.
 #ifndef NDEBUG
  //#define _CRTDBG_MAP_ALLOC 			//+++ 定義するとVCライブラリによるmalloc系のチェック強化...
  //#define USE_ATLDBGMEM				//+++ atldbgmem.h を使う場合... ※include,マクロの依存関係の都合、現状、regexは使用できない状態.
 #endif
#endif

// Change these values to use different versions
#ifdef WIN64	//+++ 64ビット版win は winXp64以降のみに対応.
#define WINVER					0x0502
#define _WIN32_WINNT			0x0502
#define _WIN32_IE				0x0603					//+++ _WIN32_IE_IE60SP2
#define _RICHEDIT_VER			0x0100					//+++ 0x200以上(3?)で十分だが、なんとなく
#define DONUT_NAME				_T("64unDonut")
#define DONUT_WND_CLASS_NAME	_T("WTL:") DONUT_NAME	//+++ 名前かえるとプラグインとかスクリプトでマズイ?
#else		   //+++ 一応、win9xの範囲 //\\2000以降に変更
#define WINVER					0x0502
#define _WIN32_WINNT			0x0502
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


// unDonut と unDonut+(mod) との非互換部分の切り替え
#ifdef USE_ORG_UNDONUT_INI								//+++ unDonut r13testの記述.
#define STR_ADDRESS_BAR 		_T("AddresBar")
#define STR_ENABLE				_T("Enabel")
#else													//+++ unDonut+ より変更(typo修正された)
#define STR_ADDRESS_BAR 		_T("AddressBar")
#define STR_ENABLE				_T("Enable")
#endif


#if 0
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// Win32API
#include <windows.h>

#ifdef USE_DLMALLOC	//+++ new,deleteにdlmallocを用いてみる. 
 #undef 	_CRTDBG_MAP_ALLOC	// cランタイムなデバッグ関係は使えない
 #undef 	USE_ATLDBGMEM		// atlのメモリデバッグ関係は使えない...
 #define 	USE_DL_PREFIX		// 本物のmalloc,freeの置換は大変なので、dlmalloc名のまま使う.
 #include 	"dlmalloc.h"
  #if 0 //def USE_DLMALLOC
   #define 	malloc 		dlmalloc
   #define 	calloc 		dlcalloc
   #define 	realloc 	dlrealloc
   #define 	free 		dlfree
 #endif
#endif

#if 1	//+++ メモリー＋デバッグの辻褄あわせ等
 #include <new>
 #if defined NDEBUG == 0 && defined USE_ATL3_BASE_HOSTEX == 0
  #if defined USE_ATLDBGMEM
   //#define _ATL_NO_TRACK_HEAP
   #include <atldbgmem.h>
  #endif
  #ifdef _CRTDBG_MAP_ALLOC
   #include <malloc.h>
   #include <crtdbg.h>
  #endif
 #pragma push_macro("new")
 #undef  new
 #include <xdebug>
 #include <xmemory>
 #include <xlocale>
 #pragma pop_macro("new")
 #endif
#endif


//C Standard Library
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

//STL(C++ Standard Library)
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
//#include <functional>
#include <boost/function.hpp>
#include <utility>
#include <iterator>
#include <map>
#include <memory>

using std::vector;
using std::pair;
using std::make_pair;
using boost::function;
//using namespace std::placeholders;
using std::unique_ptr;

#define DBG_M() 		ErrorLogPrintf(_T("%s (%d)\n"), __FILE__, __LINE__)

#ifdef USE_DLMALLOC
#define _recalloc		recalloc
inline void* recalloc(void* p, size_t a, size_t b) { void* q = realloc(p,a*b); if (q) memset(q, 0, a*b); return q; }
#endif

//ATL/WTL
#include <atlbase.h>
#include <atlapp.h>

extern CServerAppModule _Module;						//アプリケーションインスタンス
extern TCHAR			g_szIniFileName[MAX_PATH];		//設定ファイル

#include <atlcom.h>


#if _ATL_VER < 0x700 && defined(USE_ATL3_BASE_HOSTEX) == 0
#define USE_ATL3_BASE_HOSTEX
#endif

#if defined USE_ATL3_BASE_HOSTEX == 0	/*_ATL_VER >= 0x700*/	//+++
//#include "AtlifaceEx.h"	//+++ すでに不要のよう?.
//#include "AtlifaceEx_i.c" //+++ すでに不要のよう?.
//#include <AtlHost.h>
//#include "AtlHostEx.h"
#else
#endif

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

#ifndef  WM_THEMECHANGED
 #define WM_THEMECHANGED	0x031A
#endif
//非XPでも動作するように動的リンクするようにした改造版ヘッダ
#if _MSC_VER >= 1500	//+++ メモ:undonutで使うWTL80側を改造したのでこちらを使ってもok.
						//+++ だが、ヘッダがそろってないとダメなようなんで、手抜きでコンパイラバージョンで切り替え
#include <atltheme.h>
#else					//+++ 古いコンパイラ用...だが、こっちのほうがサイズ小さくなるかも...
#include "WtlFixed/atltheme_d.h"
#endif

//+++ Aero を使ってみるテスト.
#ifdef USE_AERO
#include <atldwm.h>
#endif

//IEコンポーネントで使う定義
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


#ifndef USE_DIET	//XML用
//+++ 使うのはMSXML2の範囲?のようだし、xp64,vista64 には msxml3.dllがでデフォで入っているようなので、3にしてみる.
//#import "msxml4.dll" named_guids	//raw_interfaces_only
#import "msxml3.dll" named_guids	//raw_interfaces_only
using namespace MSXML2;
#endif

//+++ 手抜きで 0 クリアをする new を用意.
//メモリリーク確認用のnew/deleteオーバーロード
#if defined USE_ATLDBGMEM
#define DEBUG_NEW	new(__FILE__, __LINE__)
#elif (defined USE_ZEROFILL_NEW) || (defined USE_MEMORYLOG && defined _DEBUG)
void *operator	new(size_t t);
void *operator	new[] (size_t t);
void operator	delete(void *p);
void operator	delete[] (void *p);
//#undef USE_MEMORYLOG
#endif

#if 0 //defined _DEBUG && defined _CRTDBG_MAP_ALLOC
void* operator	new(size_t sz, const char* fname, unsigned line);
void* operator	new[](size_t sz, const char* fname, unsigned line);
void  operator	delete(void* p, const char* fname, unsigned line);
void  operator	delete[](void* p, const char* fname, unsigned line);
#endif

#include "dbg_wm.h"


#include "DonutDefine.h"
#include "Misc.h"
#include "DonutPFunc.h"
#include "dialog/DebugWindow.h"

#ifdef USE_ATL3_BASE_HOSTEX/*_ATL_VER < 0x700*/ //+++
#include "for_atl3/AtlifaceEx.h"
#include "for_atl3/AtlHostEx_for_atl3.h"
#endif


#undef min
#undef max

#if _ATL_VER < 0x700
namespace std {
template<typename T> inline const T min(const T& a, const T& b) { return a < b; }
template<typename T> inline const T max(const T& a, const T& b) { return b < a; }
}
#endif





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
