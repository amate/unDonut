/**
 *	@file	DialogHook.h
 *	@brief	ダイアログ・フック
 */

#pragma once


#define PROMPT_CAPTION					_T("Explorer ユーザー プロンプト")
#define STRING_PROMPT_TOOLTIP			_T( "このウィンドウがどのタブによって開かれたものかunDonutは把握しておりません。\n場合によってはアクティブではないタブに開かれた、悪質なサイトが開いた偽装ウィンドウである\n可能性もあります。OKボタンを押して情報を送信する際はご注意ください。")
#define ACTIVEXDIALOG_WINDOWTEXT		_T("Microsoft Internet Explorer")
#define ACTIVEXDIALOG_WINDOWTEXT2		_T("Windows Internet Explorer")
#define ACTIVEXDIALOG_MESSAGE_PARTIAL	_T("現在のセキュリティの設定では、このページの")
#define ACTIVEXDIALOG_MESSAGE_REASONS	_T("次のいずれかの理由により 1 つまたはそれ以上の ActiveX") 	//+++ 2chスレ"unDonut Part16" 149 を反映.



class CDialogHook {
private:
	static HHOOK	s_hHook;
	static HWND 	s_hWndMainFrame;


	enum {
		cnt_offsetX 	= 4,
		cnt_offsetY 	= 4
	};


public:
	static BOOL InstallHook(HWND hWndMainFrame)
	{
		if (s_hHook)
			return FALSE;

		s_hWndMainFrame 	= hWndMainFrame;
		DWORD	dwThreadID	= ::GetCurrentThreadId();
		s_hHook 			= ::SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, NULL, dwThreadID);
		return s_hHook != 0 /*? TRUE : FALSE*/;
	}


	static BOOL UninstallHook()
	{
		if (!s_hHook)
			return FALSE;

		BOOL bRet = ::UnhookWindowsHookEx(s_hHook) != 0;
		if (bRet)
			s_hHook = NULL;
		return bRet;
	}


private:
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode < 0)
			return ::CallNextHookEx(s_hHook, nCode, wParam, lParam);

		CWPSTRUCT *pcwp = (CWPSTRUCT *) lParam;

		if (pcwp->message == WM_INITDIALOG) {
			//ダイアログ初期化を捕捉
			LRESULT lRet	   = ::CallNextHookEx(s_hHook, nCode, wParam, lParam); //初期化実行

			//ダイアログのウィンドウハンドルを取得
			HWND	hWndFocus  = (HWND) pcwp->wParam;
			if ( !::IsWindow(hWndFocus) )
				return lRet;

			HWND	hDialog    = ::GetParent(hWndFocus);
			if ( !::IsWindow(hDialog) )
				return lRet;

			//ダイアログの親ウィンドウが最上位ウィンドウであることを確認
			if (::GetParent(hDialog) != s_hWndMainFrame)
				return lRet;

			//キャプションをチェック
			CString strCaption = MtlGetWindowText(hDialog);

			if (strCaption == PROMPT_CAPTION) {
				//promptによって生成されたダイアログと判定
				PromptWindowProc(hDialog);
			} else if (strCaption == ACTIVEXDIALOG_WINDOWTEXT
					|| strCaption == ACTIVEXDIALOG_WINDOWTEXT2		//+++ for IE7.
			) {
				//IEのメッセージボックス
				HWND hWndStatic = ::GetDlgItem(hDialog, 0x0000FFFF);

				if ( ::IsWindow(hWndStatic) ) {
					CString strStaticText = MtlGetWindowText(hWndStatic);

					if (   strStaticText.Find(ACTIVEXDIALOG_MESSAGE_PARTIAL) != -1
						|| strStaticText.Find(ACTIVEXDIALOG_MESSAGE_REASONS) != -1		//+++ 2chスレ"unDonut Part16" 149 を反映.
					) {
						//if ( _check_flag(MAIN_EX_KILLDIALOG, CMainOption::s_dwMainExtendedStyle) ) {
						//	//::SendMessage(hDialog,WM_CLOSE,0,0);
						//	::DestroyWindow(hDialog);
						//}
					}
				}
			}

			return lRet;
		}

		return ::CallNextHookEx(s_hHook, nCode, wParam, lParam);
	}


	static BOOL PromptWindowProc(HWND hWnd)
	{
		HWND	 hOkButton = GetDlgItem(hWnd, 1);	//OKボタンのIDを1と仮定してハンドルを取得
		if (!hOkButton)
			return FALSE;

		HWND	 hToolTip  = ::CreateWindow(TOOLTIPS_CLASS,
											NULL,
											TTS_ALWAYSTIP,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											hWnd,
											NULL,
											_Module.GetModuleInstance(),
											NULL);
		if (!hToolTip)
			return FALSE;

		TOOLINFO	ti	= { sizeof (TOOLINFO) };
		ti.uFlags		= TTF_SUBCLASS | TTF_IDISHWND;
		ti.hwnd 		= hOkButton;
		ti.uId			= (UINT_PTR) hOkButton;

		RECT	 rc;
		::GetWindowRect(hOkButton, &rc);
		ti.hinst		= _Module.GetModuleInstance();
		ti.lpszText 	= STRING_PROMPT_TOOLTIP;

		//ツールチップの設定
		::SendMessage(hToolTip , TTM_ADDTOOL	   , 0, (LPARAM) &ti);						//ツールチップの追加
		::SendMessage(hToolTip , TTM_SETMAXTIPWIDTH, 0, (LPARAM) 500);						//テキストを改行するようにする
		::SendMessage( hToolTip, TTM_SETDELAYTIME  , TTDT_INITIAL, MAKELONG(1	 , 0) );	//表示遅延をほぼ0に
		::SendMessage( hToolTip, TTM_SETDELAYTIME  , TTDT_AUTOPOP, MAKELONG(30000, 0) );	//表示時間は30秒に

		ti.hwnd 	= hWnd;
		ti.uId		= (UINT_PTR) hWnd;
		::SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM) &ti);

		return TRUE;
	}
};


__declspec(selectany) HHOOK CDialogHook::s_hHook			= NULL;
__declspec(selectany) HWND	CDialogHook::s_hWndMainFrame	= NULL;



#if 0	//+++	すでに未使用のよう.

class CSearchBoxHook {
	static HINSTANCE	s_hInstDLL;

	typedef BOOL (WINAPI   * LPFINSTALLHOOK)(HWND);
	typedef BOOL (WINAPI   * LPFUNINSTALLHOOK)();


public:
	static BOOL InstallSearchHook(HWND hWndMainFrame)
	{
		LPFINSTALLHOOK		fpInstallHook;
		LPFUNINSTALLHOOK	fpUninstallHook;

		if (s_hInstDLL)
			return FALSE;

		s_hInstDLL		= ::LoadLibrary( _T("SearchHook.dll") );
		if (!s_hInstDLL)
			return FALSE;

		//ここで、フックする前にフック解除関数が存在するか確認する（もし無かったりすると悲惨）
		fpUninstallHook = (LPFUNINSTALLHOOK) ::GetProcAddress( s_hInstDLL, _T("UninstallHook") );
		if (!fpUninstallHook)
			goto CLEAR;

		fpInstallHook	= (LPFINSTALLHOOK) ::GetProcAddress( s_hInstDLL, _T("InstallHook") );
		if (!fpInstallHook)
			goto CLEAR;

		if ( !fpInstallHook(hWndMainFrame) )
			goto CLEAR;

		return TRUE;

	  CLEAR:
		if (s_hInstDLL)
			::FreeLibrary(s_hInstDLL);

		s_hInstDLL		= NULL;
		return FALSE;
	}


	static BOOL UninstallSearchHook()
	{
		if (!s_hInstDLL)
			return FALSE;

		LPFUNINSTALLHOOK fpUninstallHook = (LPFUNINSTALLHOOK) ::GetProcAddress( s_hInstDLL, _T("UninstallHook") );

		if (!fpUninstallHook)
			return FALSE;

		//ここで無いとか言われても正直困る
		BOOL	bRet	= fpUninstallHook();
		if (!bRet)
			return FALSE;

		//これもものすごく困る
		::FreeLibrary(s_hInstDLL);
		s_hInstDLL = NULL;
		return TRUE;
	}
};


__declspec(selectany) HINSTANCE CSearchBoxHook::s_hInstDLL = NULL;


#endif
