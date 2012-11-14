/**
 *	@file	aboutdlg.cpp
 *	@brief	バージョン情報を表示するダイアログ
 */
#include "stdafx.h"
#include "aboutdlg.h"
#include "../MtlWin.h"
#include "../MtlMisc.h"
#include "../appconst.h"
#include "../DonutPFunc.h"
#include "../option/MainOption.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//使用する名前空間の定義

using namespace WTL;


//以下実装
CAboutDlg::CAboutDlg()
	: m_wndEdit(this, 1)			//テキストボックスへのメッセージはALT_MSG_MAP(1)で受け取る
	, m_nSubclassPos(0)
{	}

static BOOL	CALLBACK EnumChildProc(HWND hWndChild, LPARAM pfunc)
{
	function<void (HWND)>& func = *(function<void (HWND)>*)pfunc;
	func(hWndChild);
	return TRUE;
}


void	CAboutDlg::_subclassWindows(HWND hWndChild)
{
	CString strClass;
	GetClassName(hWndChild, strClass.GetBuffer(128), 128);
	if (strClass.CompareNoCase(_T("Static")) == 0) {
		if (::GetWindowTextLength(hWndChild) > 0) {
			ATLASSERT(m_nSubclassPos < _countof(m_static));
			m_static[m_nSubclassPos].SubclassWindow(hWndChild);
			++m_nSubclassPos;
		} else {
			if (CWindow(hWndChild).GetStyle() & SS_ICON)
				m_staticIcon.SubclassWindow(hWndChild);
		}
	} else if (::GetDlgCtrlID(hWndChild) == IDOK)
		m_btnOk.SubclassWindow(hWndChild);
}



LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	SetMsgHandled(FALSE);

	CenterWindow( GetParent() );	//ダイアログは常に中央表示

	//ラベルに情報を設定
	CStatic label	   = GetDlgItem(IDC_STATIC_VERSION);
	CString strAppName = app::cnt_AppName;
	CString strAppVer  = app::cnt_AppVersion;
	MtlSetWindowText(label.m_hWnd, strAppName + _T(" ") + strAppVer);

	//テキストボックスに情報を設定
	CEdit	edit	   = GetDlgItem(IDC_EDITINFO);
	MtlSetWindowText( edit.m_hWnd, GetEnvironInfo() );

	//テキストボックス宛てのメッセージを処理できるようにする
	m_wndEdit.SubclassWindow(edit.m_hWnd);

	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_wndEdit.SetFont(lf.CreateFontIndirect());

	if (IsCompositionEnabled()) {
		MARGINS m = {-1};
		CRect rcEdit;
		CRect rcWindow;
		m_wndEdit.GetWindowRect(&rcEdit);
		GetWindowRect(&rcWindow);
		m.cxLeftWidth	= rcEdit.left - rcWindow.left;
		m.cxRightWidth	= rcWindow.right - rcEdit.right;
		m.cyBottomHeight= rcWindow.bottom	- rcEdit.bottom;

		CPoint ptEdit;
		::MapWindowPoints(m_wndEdit, m_hWnd, &ptEdit, 1);
		m.cyTopHeight	= ptEdit.y;
		SetMargins(m);

		function<void (HWND)>	func = std::bind(&CAboutDlg::_subclassWindows, this, std::placeholders::_1);
		::EnumChildWindows(m_hWnd, EnumChildProc, (LPARAM)&func);
	}
	return TRUE;
}

/// エディットコントロールを白背景に
HBRUSH	CAboutDlg::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
{
	if (wndStatic.m_hWnd == m_wndEdit.m_hWnd)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	SetMsgHandled(FALSE);
	return 0;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//ダイアログ消去
	EndDialog(wID);
	return 0;
}



LRESULT CAboutDlg::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	//テキスト全選択
	CEdit  edit = GetDlgItem(IDC_EDITINFO);
	edit.SetSelAll();

	//メニューを表示(クリップボードにテキストをコピーするか否か)
	CMenu  menu;
	menu.CreatePopupMenu();
	menu.AppendMenu( 0, 1, _T("クリップボードにコピー") );

	//項目が選択されたらテキストをコピー
	CPoint pos;
	::GetCursorPos(&pos);
	int    nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pos.x, pos.y, m_hWnd);

	if (nRet == 1)
		MtlSetClipboardText(MtlGetWindowText(edit.m_hWnd), m_hWnd);

	return 0;
}



//OSが識別できなかった場合の文字列
#define UNKNOWNCODE 		_T("OS:unknown")

static CString GetRenderingMode()
{
	CString exeFileName = Misc::GetFileBaseName(Misc::GetExeFileName());

	// http://msdn.microsoft.com/en-us/library/ee330730(v=vs.85).aspx#browser_emulation
	enum RenderingMode { 
		kIE7mode = 7,
		kIE8mode = 8,
		kIE9mode = 9,
		kIE10mode = 10,
	};
	RenderingMode mode = kIE7mode;
	bool bForce = false;

	auto funcGetRenderingModeFromRegistory = [&] (HKEY hk) {
		static LPCTSTR BROWSEREMULATIONKEY	= _T("SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");
		ATL::CRegKey rk;
		LONG result = rk.Open(hk, BROWSEREMULATIONKEY, KEY_QUERY_VALUE);
		if (result == ERROR_SUCCESS) {
			DWORD dwMode = 0;
			rk.QueryDWORDValue(exeFileName, dwMode);
			switch (dwMode) {
			// IE7mode
			case 7000:
			default:
				mode = kIE7mode;
				break;

			// IE8mode
			case 8888:
				bForce = true;
			case 8000:
				mode = kIE8mode;
				break;

			// IE9mode
			case 9999:
				bForce = true;
			case 9000:
				mode = kIE9mode;
				break;

			// IE10mode
			case 10001:
				bForce = true;
			case 10000:			
				mode = kIE10mode;
				break;
			}
		}
	};

	funcGetRenderingModeFromRegistory(HKEY_LOCAL_MACHINE);
	funcGetRenderingModeFromRegistory(HKEY_CURRENT_USER);

	CString temp;
	temp.Format(_T("(IE%dmode"), mode);
	if (bForce) {
		temp += _T(" force");
	}
	if (Misc::IsGpuRendering()) {
		temp += _T(", GPURendering 有効");
	}
	temp += _T(")");
	return temp;
}

CString CAboutDlg::GetEnvironInfo()
{
	//一通りの環境情報を取得
	CString strOSName	  = GetOSName(); //判定失敗時はUNKNOWNCODEを返す
	CString strVersion	  = GetVersion();
	CString strOSInfo	  = GetOSInfo();
	CString strIEVersion  = GetIEVersion();
	CString strUpdateInfo = GetUpdateInfo();
	CString strRenderingMode = GetRenderingMode();

	//判定失敗時の処理
	if (strOSName != UNKNOWNCODE)
		strOSName = _T("Windows ") + strOSName;

	CString strmode = CMainOption::s_BrowserOperatingMode == kMultiThreadMode ? _T("(mt)") : _T("(mp)");

	//情報をテキストボックスに表示
	CString strInfo;
	strInfo.Format(_T("%s %s %s\r\nOS : %s (%s) %s\r\nIE : %s%s %s\r\n"),
				   app::cnt_AppName, app::cnt_AppVersion, strmode, strOSName, strVersion, strOSInfo, strIEVersion, strUpdateInfo, strRenderingMode);

	return strInfo;
}

CString CAboutDlg::GetIEVersion()
{
	//レジストリからIEのバージョンを取得
	TCHAR	buf[MAX_PATH+2];
	DWORD	dwCount = MAX_PATH;
	Misc::CRegKey reg;

	memset(buf, 0, sizeof buf); //+++
	LRESULT rc = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"), KEY_QUERY_VALUE );
	if (rc == ERROR_SUCCESS) {
		rc = reg.QueryStringValue( _T("Version"), buf, &dwCount);
		reg.Close();
	}

	if (rc != ERROR_SUCCESS) {
		rc = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector"), KEY_QUERY_VALUE );
		if (rc == ERROR_SUCCESS) {
			rc = reg.QueryStringValue( _T("IE"), buf, &dwCount);
			//DWORD dw = 0;
			//LRESULT rc = reg.QueryDWORDValue( _T("IE"), dw);
			//sprintf(buf, _T("%d"), dw);
			reg.Close();
		}
	}

	return buf;
}



CString CAboutDlg::GetUpdateInfo()
{
	//レジストリからWindowsのアップデート情報を取得
	TCHAR	buf[MAX_PATH+2];
	DWORD	dwCount = MAX_PATH;
	Misc::CRegKey reg;

	memset(buf, 0, sizeof buf); //+++
	LRESULT rc = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), KEY_QUERY_VALUE );
	//+++ reg.Open( HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings") );
	if (rc == ERROR_SUCCESS) {
		reg.QueryStringValue( _T("MinorVersion"), buf, &dwCount);
		reg.Close();
	}

	return CString(buf);
}



CString CAboutDlg::GetOSInfo()
{
	//OSの追加情報を取得
	OSVERSIONINFO osvi = { sizeof (OSVERSIONINFO) };

	::GetVersionEx(&osvi);

	if (osvi.szCSDVersion[1] != 'A' && osvi.szCSDVersion[1] != 'B' && osvi.szCSDVersion[1] != 'C') {
		return CString(osvi.szCSDVersion);
	} else {
		return CString();
	}
}



CString CAboutDlg::GetVersion()
{
	//OSのバージョンをメジャー・マイナー・ビルドナンバーの順で表示
	OSVERSIONINFO osvi = { sizeof (OSVERSIONINFO) };

	::GetVersionEx(&osvi);

	CString 	  strVersion;
	strVersion.Format(_T("%d.%d.%d"), osvi.dwMajorVersion,
					  osvi.dwMinorVersion,
					  osvi.dwBuildNumber & 0xffff);
	return strVersion;
}



CString CAboutDlg::GetOSName()
{
	//OS名を取得する
	OSVERSIONINFO osvi = { sizeof (OSVERSIONINFO) };

	::GetVersionEx(&osvi);

	//バージョン毎に処理を分ける
	if (osvi.dwMajorVersion == 3) {
		return GetOSName_Version3(osvi);
	} else if (osvi.dwMajorVersion == 4) {
		return GetOSName_Version4(osvi);
	} else if (osvi.dwMajorVersion == 5) {
		return GetOSName_Version5(osvi);
	} else if (osvi.dwMajorVersion == 6) {
		return GetOSName_Version6(osvi);
	} else {
		return UNKNOWNCODE;
	}
}



CString CAboutDlg::GetOSName_Version3(OSVERSIONINFO &osvi)	//NT3.51
{
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (osvi.dwMinorVersion == 51)
			return _T("NT 3.51");
		else
			return UNKNOWNCODE;
	} else {
		return UNKNOWNCODE;
	}
}



CString CAboutDlg::GetOSName_Version4(OSVERSIONINFO &osvi)	//95 98 Me NT4
{
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		if (osvi.dwMinorVersion >= 90) {
			//Me
			return _T("Me");
		} else if (osvi.dwMinorVersion >= 10) {
			//98
			if (osvi.szCSDVersion[1] == 'A') {
				return _T("98 SE");
			} else {
				return _T("98");
			}
		} else if (osvi.dwMinorVersion >= 0) {
			//95
			if (osvi.szCSDVersion[1] == 'B' || osvi.szCSDVersion[1] == 'C') {
				return _T("95 OSR2");
			} else {
				return _T("95");
			}
		} else {
			return UNKNOWNCODE;
		}
	} else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		//NT4
		if (osvi.dwMinorVersion == 0) {
			return _T("NT 4.0");
		} else {
			return UNKNOWNCODE;
		}
	} else {
		return UNKNOWNCODE;
	}
}



CString CAboutDlg::GetOSName_Version5(OSVERSIONINFO &osvi)	//2000 XP 2003Server
{
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
	  #if defined _WIN64
		if (osvi.dwMinorVersion >= 2) {
			return _T("Server 2003(x64)");
		} else if (osvi.dwMinorVersion == 1) {
			return _T("XP(x64)");
		} else {
			return _T("2000(x64)"); 						//ない?
		}

	  #else
		if ( Misc::IsWow64() ) {
			if (osvi.dwMinorVersion >= 2) {
				return _T("Server 2003(x64)");
			} else if (osvi.dwMinorVersion == 1) {
				return _T("XP(x64)");
			} else {
				return _T("2000(x64)"); 					//ない?
			}
		} else {
			if (osvi.dwMinorVersion >= 2) {
				return _T("Server 2003");
			} else if (osvi.dwMinorVersion == 1) {
				return _T("XP");
			} else {
				return _T("2000");
			}
		}
	  #endif

	} else {
		return UNKNOWNCODE;
	}
}



CString CAboutDlg::GetOSName_Version6(OSVERSIONINFO &osvi)	//Vista,Windows7
{
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (osvi.dwMinorVersion == 0) {
		  #if defined _WIN64
			return _T("Vista(x64)");
		  #else
			if ( Misc::IsWow64() )
				return _T("Vista(x64)");
			else
				return _T("Vista");
		  #endif
		} else if (osvi.dwMinorVersion == 1) {
		  #if defined _WIN64
			return _T("Windows7(x64)");
		  #else
			if ( Misc::IsWow64() )
				return _T("Windows7(x64)");
			else
				return _T("Windows7");
		  #endif
		} else {
			return UNKNOWNCODE;
		}
	} else {
		return UNKNOWNCODE;
	}
}
