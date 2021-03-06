/**
 *	@file	Donut.cpp
 *	@brief	main source file for Donut.exe
 */

#include "stdafx.h"
#include "Donut.h"
#include <regex>
#include <strsafe.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

#include "MtlMisc.h"
#include "AtlHostEx.h"
#include "initguid.h"
#include "DonutP.h"
#include "DonutP_i.c"

#include "DonutOptions.h"
#include "ie_feature_control.h"
#include "ExMenu.h"
#include "MainFrame.h"
#include "appconst.h"
#include "MultiThreadManager.h"
#include "DonutSimpleEventManager.h"
#include "VersionControl.h"
#include "APIHook.h"
#include "SearchBarHelper.h"


extern const UINT	g_uDropDownCommandID[] = {
	ID_FILE_NEW,
	ID_VIEW_BACK,
	ID_VIEW_FORWARD,
	ID_FILE_NEW_CLIPBOARD2,
	ID_DOUBLE_CLOSE,
	ID_DLCTL_CHG_MULTI,
	ID_DLCTL_CHG_SECU,
	ID_URLACTION_COOKIES_CHG,
	ID_RECENT_DOCUMENT
};

extern const UINT	g_uDropDownWholeCommandID[] = {
	ID_VIEW_FONT_SIZE,
	ID_FAVORITES_DROPDOWN,
	ID_AUTO_REFRESH,
	ID_TOOLBAR,
	ID_EXPLORERBAR,
	ID_MOVE,
	ID_OPTION,
	ID_COOKIE_IE6,
	ID_FAVORITES_GROUP_DROPDOWN,
	ID_CSS_DROPDOWN
};


extern const int	g_uDropDownCommandCount 	 = sizeof (g_uDropDownCommandID 	) / sizeof (UINT);
extern const int	g_uDropDownWholeCommandCount = sizeof (g_uDropDownWholeCommandID) / sizeof (UINT);

// グローバル変数
CServerAppModule	_Module;
TCHAR				g_szIniFileName[MAX_PATH];
CMainFrame*			g_pMainWnd		= NULL;
bool				g_bSefShutDown	= false;
CString				g_commandline;


BEGIN_OBJECT_MAP(ObjectMap)
//	OBJECT_ENTRY(CLSID_API, CAPI)
END_OBJECT_MAP()


////////////////////////////////////////////////////////////////////
/// クラッシュダンプ関係

typedef BOOL (WINAPI *MiniDumpWriteDump_fp)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

MiniDumpWriteDump_fp	funcMiniDumpWriteDump = nullptr;

LONG WINAPI GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	GetLocalTime( &stLocalTime );

	TCHAR	strExePath[MAX_PATH];
	::GetModuleFileName(_Module.GetModuleInstance(), strExePath, MAX_PATH);
	::PathRemoveFileSpec(strExePath);

	// 以前のダンプファイルを削除する
	MTL::MtlForEachFile(strExePath, [](const CString& strPath) {
		if (Misc::GetFileExt(strPath) == _T("dmp"))
			::DeleteFile(strPath);
	});

	TCHAR strDmpFilePath[MAX_PATH];
	StringCchPrintf(strDmpFilePath, MAX_PATH, _T("%s\\%s-%s %04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
		strExePath, app::cnt_AppName, app::cnt_AppVersion,
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
		GetCurrentProcessId(), GetCurrentThreadId());

	hDumpFile = CreateFile(strDmpFilePath, GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = funcMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
					hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
	CloseHandle(hDumpFile);

	enum {
		STATUS_INSUFFICIENT_MEM = 0xE0000001
	};

	CString strError = _T("例外が発生しました。\n例外：");
	switch (pExceptionPointers->ExceptionRecord->ExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		strError += _T("スレッドが適切なアクセス権を持たない仮想アドレスに対して、読み取りまたは書き込みを試みました。");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		strError += _T(" 0 で除算しようとしました。");
		break;
	case EXCEPTION_INT_OVERFLOW:
		strError += _T("整数演算結果の最上位ビットが繰り上がりました。");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		strError += _T("スタックオーバーフローしました。");
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		strError += _T("スレッドが範囲外の配列要素にアクセスしようとしました。使用中のハードウェアは境界チェックをサポートしています。");
		break;
	case STATUS_INSUFFICIENT_MEM:
		strError += _T("メモリが不足しています。");
		break;
	default:
		strError += _T("その他の例外が発生しました。");
		break;
	};
	MessageBox(NULL, strError, NULL, MB_ICONERROR);

	return EXCEPTION_CONTINUE_SEARCH;
}

int WINAPI GenerateDumpi(EXCEPTION_POINTERS* pExceptionPointers)
{
	return (int)GenerateDump(pExceptionPointers);
}

// コマンドラインからURLを取り出す
void PerseUrls(LPCTSTR lpszCommandline, std::vector<CString>& vecUrls)
{
	// 検索バーを使って検索する
	CString str = lpszCommandline;
	if (str.Left(13) == _T("SearchEngine:")) {
		vecUrls.push_back(str);
		return ;
	}

	std::wstring	strCommandline = lpszCommandline;
	auto			itbegin = strCommandline.cbegin();
	auto			itend	= strCommandline.cend();
	std::wregex		rx(L"(?:\")([^\"]+)(?:\")");	// "〜"
	std::wsmatch	result;
	while (std::regex_search(itbegin, itend, result, rx)) {
		CString url = result[1].str().c_str();
		if (url.CompareNoCase(_T("/dde")) != 0)
			vecUrls.push_back(url);
		itbegin = result[0].second;
	}
	if (vecUrls.size() == 0) {
		strCommandline = lpszCommandline;
		itbegin = strCommandline.cbegin();
		itend	= strCommandline.cend();
		std::wregex		rx(L"([^ ]+)");
		std::wsmatch	result;
		while (std::regex_search(itbegin, itend, result, rx)) {
			CString url = result[1].str().c_str();
			if (url.CompareNoCase(_T("/dde")) != 0)
				vecUrls.push_back(url);
			itbegin = result[0].second;
		}
	}
}

//+++	起動時しょっぱなのチェック.
static bool CheckOneInstance(LPTSTR lpstrCmdLine)
{
	//ChangeWindowMessageFilter(WM_NEWINSTANCE, MSGFLT_ADD); //+++ ユーザーメッセージを通過させるためにvistaでの開発中は必要?(て、そもそもつかえなかった)
	
	CIniFileI	pr(g_szIniFileName, _T("Main"));
	pr.QueryValue(CMainOption::s_dwMainExtendedStyle, _T("Extended_Style"));
	if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_ONEINSTANCE) { // 複数起動を許可しない
		HWND hWnd = ::FindWindow(DONUT_WND_CLASS_NAME, NULL);
		if (hWnd) {		// 既に起動しているunDonutが見つかった
			COPYDATASTRUCT	cd;
			cd.dwData	= kNewDonutInstance;
			size_t	cbCommandLine = 0;
			StringCbLength(lpstrCmdLine, sizeof(TCHAR) * 4096, &cbCommandLine);
			cd.cbData	= (DWORD)cbCommandLine + sizeof(TCHAR);
			cd.lpData	= lpstrCmdLine;
			::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cd);
		
			//bool	bActive = !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE);
			//if (bActive)
			//	::SetForegroundWindow(hWnd);
			return true;
		}
	}
	return false;
}


// iniファイルから設定を読み込む
static bool _PrivateInit()
{
	CVersionControl().Run();

	CMainOption::GetProfile();
	CToolBarOption::GetProfile();
	CAddressBarOption::GetProfile();	// アドレスバー
	CSearchBarOption::GetProfile();		// 検索バー
	CMenuOption::GetProfile();			// メニュー
	CLinkBarOption::GetProfile();		// リンクバー
	CExplorerBarOption::GetProfile();	// Explorerバー
	CCustomContextMenuOption::GetProfile();
	CDLControlOption::GetProfile();
	CFileNewOption::GetProfile();
	CStartUpOption::GetProfile();
	CUrlSecurityOption::GetProfile();
	CBrowserEmulationOption::GetProfile();
	CUserDefinedCSSOption::LoadUserCSSConfig();
	CUserDefinedJsOption::LoadUserJsConfig();
	CDonutConfirmOption::GetProfile();
	CStyleSheetOption::GetProfile();
	//CToolBarOption::GetProfile();
	CTabBarOption::GetProfile();
	//ie_feature_control_setting();	// もっと手前で実行するようにした

	CFavoritesMenuOption::GetProfile();
	CMouseOption::GetProfile();
	CSkinOption::GetProfile();

	CExMenuManager::Initialize();

	CSearchBarHelper::LoadData();

	return true;
}



// iniファイルに設定を保存する
void _PrivateTerm()
{
	CStyleSheetOption::WriteProfile();

	CExMenuManager::Terminate();
	
	ATLTRACE(_T("_PrivateTerm : 設定の保存完了!\n"));
}


static HRESULT CreateComponentCategory(CATID catid, WCHAR *catDescription)
{
	ICatRegister *pcr = NULL;
	HRESULT 	  hr  = S_OK ;

	hr	= CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void **) &pcr);
	if ( FAILED(hr) )
		return hr;

	// Make sure the HKCR\Component Categories\{..catid...}
	// key is registered.
	CATEGORYINFO  catinfo;
	catinfo.catid	= catid;
	catinfo.lcid	= 0x0409 ; // english

	// Make sure the provided description is not too long.
	// Only copy the first 127 characters if it is.
	int len = (int) wcslen(catDescription);
	if (len > 127)
		len = 127;

	::wcsncpy(catinfo.szDescription, catDescription, len);

	// Make sure the description is null terminated.
	catinfo.szDescription[len] = '\0';

	hr	= pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}

static HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categories information.
	ICatRegister *pcr = NULL ;
	HRESULT 	  hr  = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void **) &pcr);
	if ( SUCCEEDED(hr) ) {
		// Register this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr		   = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

static HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister *pcr = NULL ;
	HRESULT 	  hr  = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void **) &pcr);
	if ( SUCCEEDED(hr) ) {
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr		   = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

// コマンドライン引数からCOMサーバー登録/削除を行う
static int RegisterCOMServer(int &nRet, bool &bRun, bool &bAutomation, bool &bTray)
{
	HRESULT hRes;
	TCHAR	szTokens[] = _T("-/");
	LPCTSTR lpszToken  = _Module.FindOneOf(::GetCommandLine(), szTokens);

	while (lpszToken) {
		CString strToken = Misc::GetStrWord(lpszToken, &lpszToken);
		if (strToken.CompareNoCase(_T("UnregServer") ) == 0) {
			nRet = _Module.UnregisterServer();
			nRet = UnRegisterCLSIDInCategory(CLSID_API, CATID_SafeForInitializing);
			if ( FAILED(nRet) )
				return nRet;

			nRet = UnRegisterCLSIDInCategory(CLSID_API, CATID_SafeForScripting);
			if ( FAILED(nRet) )
				return nRet;

			::MessageBox(NULL, _T("COMサーバー削除しました。"), _T("unDonut"), 0);
			bRun = false;
			break;

		} else if (strToken.CompareNoCase(_T("RegServer") ) == 0) {
			nRet = _Module.RegisterServer(TRUE, &CLSID_API);
			if (nRet == S_OK) {
				// Mark the control as safe for initializing.
				hRes = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data");
				if ( FAILED(hRes) )
					return hRes;

				hRes = RegisterCLSIDInCategory(CLSID_API, CATID_SafeForInitializing);
				if ( FAILED(hRes) )
					return hRes;

				// Mark the control as safe for scripting.
				hRes = CreateComponentCategory(CATID_SafeForScripting, L"Controls that are safely scriptable");
				if ( FAILED(hRes) )
					return hRes;

				hRes = RegisterCLSIDInCategory(CLSID_API, CATID_SafeForScripting);
				if ( FAILED(hRes) )
					return hRes;
				::MessageBox(NULL, _T("COMサーバー登録しました。"), _T("unDonut"), 0);
			} else
				::MessageBox(NULL, _T("COMサーバー登録失敗しました。"), _T("unDonut"), 0);

			bRun = false;
			break;

		} else if (strToken.CompareNoCase(_T("Automation")) == 0
			||    (strToken.CompareNoCase(_T("Embedding" )) == 0) )
		{
			bAutomation = true;
			break;
	  #if 1	//+++
		} else if (strToken.CompareNoCase(_T("tray") ) == 0) {
			bTray = true;
			break;
	  #endif
		}

		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

	return S_OK;
}


static int RunWinMain(HINSTANCE hInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
	TIMERSTART();

	// DLL攻撃対策
	SetDllDirectory(_T(""));
#if 1
	#if defined (_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
	//メモリリーク検出用
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF
				| _CRTDBG_CHECK_ALWAYS_DF
	);
	//_CrtSetBreakAlloc(874);
	#endif

	#ifdef _CRTDBG_MAP_ALLOC
	//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
#endif

#ifdef _DEBUG
	// ATLTRACEで日本語を使うために必要
	_tsetlocale( LC_ALL, _T("japanese") );
#endif

	Misc::setHeapAllocLowFlagmentationMode();	//+++

	// 設定ファイルのフルパスを取得する
	InitDonutConfigFilePath(g_szIniFileName, MAX_PATH);

	// フックを仕掛ける
	DoHookInternetConnect();

	ie_feature_control_setting();

	if (MultiThreadManager::RunChildProcessMessageLoop(hInstance)) 
		return 0;

	// 複数起動の確認
	if (CheckOneInstance(lpstrCmdLine)) 
		return 0;

	g_pMainWnd	 = NULL;
	g_commandline= lpstrCmdLine;
	//	HRESULT hRes = ::CoInitialize(NULL);
	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//	ATLASSERT( SUCCEEDED(hRes) );
	// If you are running on NT 4.0 or higher you can use the following call instead to
	// make the EXE free threaded. This means that calls come in on a random RPC thread
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hRes		 = ::OleInitialize(NULL);
	ATLASSERT( SUCCEEDED(hRes) );

	ATLTRACE(_T("tWinMain\n") _T("CommandLine : %s\n"), lpstrCmdLine);

	/* コモンコントロールを初期化 */
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof (iccx);
	iccx.dwICC	= ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	int 	ret = ::InitCommonControlsEx(&iccx);
	ATLASSERT(ret);

	hRes	= _Module.Init(ObjectMap, hInstance, &LIBID_ATLLib);
	//hRes	= _Module.Init(NULL, hInstance);
	ATLASSERT( SUCCEEDED(hRes) );


	int 	nRet		 = 0;
	bool	bRun		 = true;
	bool	bAutomation  = false;
	bool	bTray		 = false;

	try {	//+++ 念のため例外チェック.
		nRet = _PrivateInit();
	} catch (...) {
		ErrorLogPrintf(_T("_PrivateInitでエラー\n"));
		nRet = -1;
	}
	if (nRet <= 0)
		goto END_APP;

	// ActiveXコントロールをホストするための準備
	AtlAxWinInit();

	// コマンドライン入力によってはCOMサーバー登録及び解除を行う
	nRet = RegisterCOMServer(nRet, bRun, bAutomation, bTray);
	if (FAILED(nRet)) {
		ErrorLogPrintf(_T("RegisterCOMServerでエラー\n"));
		nRet = -1;
		goto END_APP;
	}

	CDonutSimpleEventManager::RaiseEvent(EVENT_PROCESS_START);

	if (bRun) {
		//_Module.StartMonitor();
		//hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);
		//ATLASSERT( SUCCEEDED(hRes) );
		//hRes = ::CoResumeClassObjects();
		//ATLASSERT( SUCCEEDED(hRes) );
		TIMERSTOP(_T("WinMain : preparation"));
		if (bAutomation) {
			CMessageLoop theLoop;
			nRet = theLoop.Run();
		} else {
			//\\nRet = Run(lpstrCmdLine, nCmdShow, bTray);
			nRet = MultiThreadManager::RunMainFrameMessageLoop(lpstrCmdLine, nCmdShow, bTray);
		}
	  #if 1 //+++ WTLのメイン窓クローズが正常終了時に、終了コードとして1を返す...
			//+++ OSに返す値なので0のほうがよいはずで、
			//+++ donutの他の部分では0にしているようなので
			//+++ しかたないので、強制的に変換.
		if (nRet == 1)
			nRet = 0;
	  #endif

		//_Module.RevokeClassObjects();
		//::Sleep(_Module.m_dwPause);
	}


	//_PrivateTerm();
	//g_bSefShutDown = true;

	ATLTRACE(_T("正常終了しました。\n"));
END_APP:
	boost::thread	terminateWatch([]() {
		DWORD	dwTime = ::timeGetTime();
		extern bool g_bSefShutDown;
		while (dwTime + (5 * 1000) > ::timeGetTime()) {
			if (g_bSefShutDown)
				return ;

			::Sleep(50);
		}
		if (g_bSefShutDown == false) {
			TRACEIN(_T("待機時間を過ぎたので強制終了します-------------------"));
			ExitProcess(-5);
		}
	});
	terminateWatch.detach();

	_Module.Term();
	::OleUninitialize();
	::CoUninitialize();

	CDonutSimpleEventManager::RaiseEvent(EVENT_PROCESS_END);
 	return nRet;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// _tWinMain : EntryPoint

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
#ifdef _WIN64
	HMODULE hDll = LoadLibrary(_T("64dbghelp.dll"));
#else
	HMODULE hDll = LoadLibrary(_T("dbghelp.dll"));
#endif
	if (hDll) {
		funcMiniDumpWriteDump = (MiniDumpWriteDump_fp)GetProcAddress(hDll, "MiniDumpWriteDump");
		if (funcMiniDumpWriteDump) {
			__try {
				//PVOID hException = AddVectoredExceptionHandler(0, GenerateDump);
				int nRet = RunWinMain(hInstance, lpstrCmdLine, nCmdShow);
				FreeLibrary(hDll);
				//RemoveVectoredExceptionHandler(hException);
				return nRet;
			}
			__except(GenerateDumpi(GetExceptionInformation()))
			{
			}
		}
		FreeLibrary(hDll);
		return 0;
	}

	return RunWinMain(hInstance, lpstrCmdLine, nCmdShow);
}




//-------------------------------------------------------------------------


void	DonutOpenFile(const CString &strFileOrURL)
{
	g_pMainWnd->UserOpenFile(strFileOrURL, DonutGetStdOpenFlag());
}

void	DonutOpenFile(const CString &strFileOrURL, DWORD dwOpenFlag)
{
	g_pMainWnd->UserOpenFile(strFileOrURL, dwOpenFlag);
}

