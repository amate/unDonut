/**
 *	@file	DialogKiller.h
 *	@brief	特定のダイアログを出さないようにする処理.
 */
#pragma once


// for debug
#ifdef _DEBUG
	const bool _Mtl_DialogKiller_traceOn = false;
	#define dgkTRACE	if (_Mtl_DialogKiller_traceOn) ATLTRACE
#else
	#define dgkTRACE
#endif


class CDialogKiller2 {
public:

	static void KillDialog()
	{
		dgkTRACE( _T("KillDialog\n") );

		// I guess this is not so slow.
		MtlForEachTopLevelWindow( _T("#32770"), NULL, _Function_DialogFilter() );
		MtlForEachTopLevelWindow( _T("Internet Explorer_TridentDlgFrame"), NULL, _Function_TridentFrameDlgFilter() );
	}


private:
	struct _Function_DialogFilter {
		bool operator ()(HWND hWnd)
		{
			if ( MtlIsWindowCurrentProcess(hWnd) ) {
				CString 	strCaption = MtlGetWindowText(hWnd);
				if ( _Filter_ActiveXWarningDialog (hWnd, strCaption) )	return true;
				if ( _Filter_SecurityWarningDialog(hWnd, strCaption) )	return true;
				if ( _Filter_OfflineDialog(hWnd, strCaption) )			return true;
				if ( _Filter_ClientAttestationDialog(hWnd,strCaption) ) return true;
			}
			return true;
		}
	};


	struct _Function_TridentFrameDlgFilter {
		bool operator ()(HWND hWnd)
		{
			if ( MtlIsWindowCurrentProcess(hWnd) ) {
				CString strCaption = MtlGetWindowText(hWnd);
				_Filter_ScriptErrorTridentDlgFrame(hWnd, strCaption);
			}
			return true;
		}
	};


	static bool _Filter_ScriptErrorTridentDlgFrame(CWindow wnd, const CString &strCaption)
	{
		if ( (strCaption.Find( _T("スクリプト") ) != -1 && strCaption.Find( _T("エラー") ) != -1)
		   || (strCaption.Compare(_T("Internet Explorer スクリプト エラー")) == 0) )
		{
			// wnd.ModifyStyle(WS_VISIBLE, 0, SWP_NOACTIVATE);
			wnd.SendMessage(WM_CLOSE);
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		}

		return false;
	}


	static bool _Filter_OfflineDialog(CWindow wnd, const CString &strCaption)
	{
		if (strCaption.Find( _T("オフラインでは") ) != -1) {
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDCANCEL, 0) );
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDOK, 0) );
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		}

		return false;
	}


	static bool _Filter_ClientAttestationDialog(CWindow wnd, const CString &strCaption)
	{
		if (strCaption.Find( _T("クライアント認証") ) != -1) {
			wnd.SendMessage(WM_CLOSE);
			return true;
		}

		return false;
	}


	static bool _Filter_SecurityWarningDialog(CWindow wnd, const CString &strCaption)
	{
		if ( strCaption != _T("セキュリティ情報") )
			return false;

		CWindow 	wndInner = wnd.GetDlgItem(0x0000FFFF);
		if (wndInner.m_hWnd == NULL)
			return false;

		CString strOneStaticText = MtlGetWindowText(wndInner.m_hWnd);

		if (strOneStaticText.Find( _T("このページには") ) != -1 && strOneStaticText.Find( _T("保護") ) != -1) {
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDCANCEL, 0) );
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDOK	, 0) );
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		}

		return false;
	}


	static bool _Filter_ActiveXWarningDialog(CWindow wnd, const CString &strCaption)
	{
		if (  strCaption != _T("Microsoft Internet Explorer")			// IE6まで
		   && strCaption != _T("Internet Explorer - Security Warning")	// IE7、多分
		   && strCaption != _T("Internet Explorer") 					// スレで報告があったやつ
		   && strCaption != _T("Windows Internet Explorer") 			//+++ IE7対策. by 2chスレ"unDonut Part16" 149氏.
		   && strCaption != _T("Web ブラウザ")	//\\ IE8かな？flashplayerのせいかもしれんが
		   && strCaption != _T("Web ブラウザー") // IE9beta
		){
			return false;
		}

		CWindow 	wndInner = wnd.GetDlgItem(0x0000FFFF);
		if (wndInner.m_hWnd == NULL)
			return false;

		CString strOneStaticText = MtlGetWindowText(wndInner.m_hWnd);

		if (strOneStaticText.Find( _T("ActiveX") ) != -1 && strOneStaticText.Find( _T("安全") ) != -1) {
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDCANCEL, 0) );
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDOK, 0) );
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		} else if (strOneStaticText.Find( _T("ActiveX") ) != -1 && strOneStaticText.Find( _T("実行") ) != -1) {
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDCANCEL, 0) );
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDOK, 0) );
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		} else if (strOneStaticText.Find( _T("開けません") ) != -1) {
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDCANCEL, 0) );
			wnd.SendMessage( WM_COMMAND, MAKEWPARAM(IDOK, 0) );
			// wnd.SendMessage(WM_KEYDOWN, VK_ESCAPE);
			return true;
		}

		return false;
	}
};

