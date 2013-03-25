/**
 *	@file	MainOption.cpp
 *	@brief	donutのオプション : 全般, 全般2
 */

#include "stdafx.h"
#include "MainOption.h"
#include "../MtlWeb.h"
#include "../DonutPFunc.h"
#include "../ie_feature_control.h"
#include "../RecentClosedTabList.h"


////////////////////////////////////////////////////////////////////////////////
//CMainOptionの定義
////////////////////////////////////////////////////////////////////////////////

DWORD		CMainOption::s_dwMainExtendedStyle		=
	  MAIN_EX_INHERIT_OPTIONS 
	| MAIN_EX_EXTERNALNEWTAB 
	| MAIN_EX_EXTERNALNEWTABACTIVE;

DWORD		CMainOption::s_dwMainExtendedStyle2 	= 0;
DWORD		CMainOption::s_dwBackUpTime 			= 1;
DWORD		CMainOption::s_dwAutoRefreshTime		= 10;

volatile bool CMainOption::s_bAppClosing			= false;
volatile bool CMainOption::s_bIgnoreUpdateClipboard	= false;


DWORD	CMainOption::s_dwErrorBlock 			= 0;

CString	CMainOption::s_strExplorerUserDirectory;

bool	CMainOption::s_bTravelLogGroup			= false;
bool	CMainOption::s_bTravelLogClose			= false;

bool	CMainOption::s_bStretchImage			= false;

bool	CMainOption::s_bIgnore_blank			= false;
bool	CMainOption::s_bUseCustomFindBar		= false;
bool	CMainOption::s_bExternalNewTab			= true;
bool	CMainOption::s_bExternalNewTabActive	= true;
bool	CMainOption::s_bDelayLoad				= false;

int		CMainOption::s_nMaxRecentClosedTabCount		= 16;
int		CMainOption::s_RecentClosedTabMenuType		= RECENTDOC_MENUTYPE_URL;

int		CMainOption::s_nAutoImageResizeType	= AUTO_IMAGE_RESIZE_LCLICK;

BROWSEROPERATINGMODE CMainOption::s_BrowserOperatingMode = kMultiThreadMode;


CMainOption::CMainOption()
{
}



void CMainOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("Main") );
	{
		pr.QueryValue( s_dwMainExtendedStyle	, _T("Extended_Style")		);
		pr.QueryValue( s_dwMainExtendedStyle2	, _T("Extended_Style2") 	);
		pr.QueryValue( s_dwBackUpTime			, _T("BackUp_Time") 		);
		pr.QueryValue( s_dwAutoRefreshTime		, _T("Auto_Refresh_Time")	);	// UDT DGSTR ( dai
		pr.QueryValue( s_dwErrorBlock			, _T("ErrorBlock")			);	//minit
		s_bTravelLogGroup	= pr.GetValue(_T("TravelLogGroup"), s_bTravelLogGroup) != 0;
		s_bTravelLogClose	= pr.GetValue(_T("TravelLogClose"), s_bTravelLogClose) != 0;
		pr.QueryValue( s_nMaxRecentClosedTabCount, _T("MaxRecentClosedTabCount"));
		pr.QueryValue( s_RecentClosedTabMenuType , _T("RecentClosedTabMenuType"));
	}

	s_bIgnore_blank		= (s_dwMainExtendedStyle & MAIN_EX_IGNORE_BLANK) != 0;
	s_bUseCustomFindBar = (s_dwMainExtendedStyle & MAIN_EX_USECUSTOMFINDBER) != 0;
	s_bExternalNewTab	= (s_dwMainExtendedStyle & MAIN_EX_EXTERNALNEWTAB) != 0;
	s_bExternalNewTabActive = (s_dwMainExtendedStyle & MAIN_EX_EXTERNALNEWTABACTIVE) != 0;
	s_bDelayLoad		= (s_dwMainExtendedStyle & MAIN_EX_DELAYLOAD) != 0;

	pr.QueryValue(s_nAutoImageResizeType, _T("AutoImageResizeType"));
	s_BrowserOperatingMode	= (BROWSEROPERATINGMODE)pr.GetValue(_T("BrowserOperatingMode"), s_BrowserOperatingMode);

	pr.ChangeSectionName( _T("Explorer_Bar") );
	s_strExplorerUserDirectory = pr.GetStringUW(_T("UserDirectory"));	
}



void CMainOption::WriteProfile()
{
	if ( MtlIsGlobalOffline() )
		s_dwMainExtendedStyle |= MAIN_EX_GLOBALOFFLINE;
	else
		s_dwMainExtendedStyle &= ~MAIN_EX_GLOBALOFFLINE;

	CIniFileO	pr( g_szIniFileName, _T("Main") );
	{
		pr.SetValue( s_dwMainExtendedStyle	, _T("Extended_Style")		);
		pr.SetValue( s_dwMainExtendedStyle2 , _T("Extended_Style2") 	);
		pr.SetValue( s_dwBackUpTime 		, _T("BackUp_Time") 		);
		pr.SetValue( s_dwAutoRefreshTime	, _T("Auto_Refresh_Time")	);	// UDT DGSTR ( dai
		pr.SetValue( s_bTravelLogGroup		, _T("TravelLogGroup")		);
		pr.SetValue( s_bTravelLogClose		, _T("TravelLogClose")		);
		pr.SetValue( s_nMaxRecentClosedTabCount, _T("MaxRecentClosedTabCount"));
		pr.SetValue( s_RecentClosedTabMenuType , _T("RecentClosedTabMenuType"));
	}

	pr.SetValue(s_nAutoImageResizeType, _T("AutoImageResizeType"));


	pr.ChangeSectionName(_T("Explorer_Bar"));
	pr.SetStringUW( s_strExplorerUserDirectory, _T("UserDirectory") );
}

void CMainOption::SetExplorerUserDirectory(const CString &strPath)
{
	s_strExplorerUserDirectory = strPath;

	CIniFileO pr(g_szIniFileName, _T("Explorer_Bar"));
	pr.SetStringUW( s_strExplorerUserDirectory, _T("UserDirectory") );
}

const CString& CMainOption::GetExplorerUserDirectory()
{
	return s_strExplorerUserDirectory;
}

void CMainOption::OnMainExNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (s_dwMainExtendedStyle & MAIN_EX_NEWWINDOW)
		s_dwMainExtendedStyle &= ~MAIN_EX_NEWWINDOW;
	else
		s_dwMainExtendedStyle |= MAIN_EX_NEWWINDOW;
}



void CMainOption::OnMainExNoActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE)
		s_dwMainExtendedStyle &= ~MAIN_EX_NOACTIVATE;
	else
		s_dwMainExtendedStyle |= MAIN_EX_NOACTIVATE;
}



void CMainOption::OnMainExNoActivateNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN)
		s_dwMainExtendedStyle &= ~MAIN_EX_NOACTIVATE_NEWWIN;
	else
		s_dwMainExtendedStyle |= MAIN_EX_NOACTIVATE_NEWWIN;
}



////////////////////////////////////////////////////////////////////////////////
//CMainPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////


BOOL CMainPropertyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CComboBox cmb = GetDlgItem(IDC_COMBO_BROWSREOPERATINGMODE);
	cmb.AddString(_T("マルチスレッド"));
	cmb.AddString(_T("マルチプロセス"));

	return 0;
}

void CMainPropertyPage::OnCheckExternalNewTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	bool bCheck = CButton(GetDlgItem(IDC_CHECK_EXTERNALNEWTAB)).GetCheck() != 0;
	CButton btn = GetDlgItem(IDC_CHECK_EXTERNALNEWTABACTIVE);
	btn.EnableWindow(bCheck);
	if (bCheck == false)
		btn.SetCheck(FALSE);
}


void CMainPropertyPage::OnFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	CFontDialog dlg(&m_lf);

	if (dlg.DoModal() == IDOK) {
		m_lf = dlg.m_lf;
	}
}



// Constructor
CMainPropertyPage::CMainPropertyPage(HWND hWnd)
	: m_wnd(hWnd)
	, m_bInit(false)
{
	_SetData();
}



// Overrides
BOOL CMainPropertyPage::OnSetActive()
{
	if (m_bInit == false) {
		m_bInit = true;
		DoDataExchange(DDX_LOAD);
		OnCheckExternalNewTab(0, 0, NULL);
	}
	SetModified(TRUE);
	return TRUE;
}



BOOL CMainPropertyPage::OnKillActive()
{
	return TRUE;//DoDataExchange(DDX_SAVE);
}



BOOL CMainPropertyPage::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CMainPropertyPage::_GetData()
{
	// Update main style
	CMainOption::s_dwMainExtendedStyle = 0;

	if (m_nNewWindow			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NEWWINDOW;
	if (m_nNoActivate			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOACTIVATE;
	if (m_nNoActivateNewWin 	/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOACTIVATE_NEWWIN;
	if (m_nOneInstance			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_ONEINSTANCE;
	if (m_nNoCloseDFG			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOCLOSEDFG;
	if (m_nBackUp				/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_BACKUP;

	if (m_nRegisterAsBrowser	/*== 1*/) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_REGISTER_AS_BROWSER;
		MtlSendOnCommand(m_wnd, ID_REGISTER_AS_BROWSER);
	} else {
		MtlSendOffCommand(m_wnd, ID_REGISTER_AS_BROWSER);
	}

	if (m_nInheritOptions		/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_INHERIT_OPTIONS;

	//+++ if (m_nNoCloseNL				) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOCLOSE_NAVILOCK;			//+++ 追加

#if 0	//* あとで
	ie_coInternetSetFeatureEnabled(FEATURE_SECURITYBAND    , SET_FEATURE_ON_PROCESS, m_nKillDialog);	//+++
#endif

	if (s_bIgnore_blank)		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_IGNORE_BLANK;
	if (s_bUseCustomFindBar)	CMainOption::s_dwMainExtendedStyle |= MAIN_EX_USECUSTOMFINDBER;
	if (s_bExternalNewTab) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_EXTERNALNEWTAB;
		if (s_bExternalNewTabActive) {
			CMainOption::s_dwMainExtendedStyle |= MAIN_EX_EXTERNALNEWTABACTIVE;
		}
	}
	if (s_bDelayLoad)			CMainOption::s_dwMainExtendedStyle |= MAIN_EX_DELAYLOAD;

	CMainOption::s_dwBackUpTime 	 = m_nBackUpTime;
	// UDT DGSTR ( dai
	m_nAutoRefreshTime				 = m_nAutoRefTimeMin * 60 + m_nAutoRefTimeSec;
	CMainOption::s_dwAutoRefreshTime = m_nAutoRefreshTime;
	// ENDE
	m_wnd.PostMessage(WM_COMMAND, ID_BACKUPOPTION_CHANGED);

	CIniFileO pr( g_szIniFileName, _T("Main") );
	m_lf.WriteProfile(pr);

	pr.SetValue(m_nBrowserOperatingMode, _T("BrowserOperatingMode"));

	pr.Close();

	WriteProfile();
}



void CMainPropertyPage::_SetData()
{
	//+++ メモ: MAIN_EX_FULLSCREEN, MAIN_EX_GLOBALOFFLINE, MAIN_EX2_NOCSTMMENU, MAIN_EX2_DEL_RECENTCLOSE はここで反映しない.

	m_nNewWindow		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NEWWINDOW		  ) != 0;		//+++ ? 1 : 0;
	m_nNoActivate		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE		  ) != 0;		//+++ ? 1 : 0;
	m_nNoActivateNewWin    = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN  ) != 0;		//+++ ? 1 : 0;
	m_nOneInstance		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_ONEINSTANCE		  ) != 0;		//+++ ? 1 : 0;
	m_nNoCloseDFG		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG		  ) != 0;		//+++ ? 1 : 0;
	m_nBackUp			   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP			  ) != 0;		//+++ ? 1 : 0;
	m_nBackUpTime		   = CMainOption::s_dwBackUpTime;
	// UDT DGSTR ( dai
	m_nAutoRefreshTime	   = CMainOption::s_dwAutoRefreshTime;
	m_nAutoRefTimeMin	   = CMainOption::s_dwAutoRefreshTime / 60;
	m_nAutoRefTimeSec	   = CMainOption::s_dwAutoRefreshTime % 60;
	// ENDE
	m_nRegisterAsBrowser   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_REGISTER_AS_BROWSER) != 0;		//+++ ? 1 : 0;
	m_nInheritOptions	   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_INHERIT_OPTIONS	  ) != 0;		//+++ ? 1 : 0;
	//+++ m_nNoCloseNL	   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSE_NAVILOCK   ) != 0;		//+++ 追加.

	// refresh our font
	WTL::CLogFont	lft;
	lft.SetMenuFont();
	m_lf = lft;

	CIniFileI	 pr( g_szIniFileName, _T("Main") );
	MTL::CLogFont	 lf;
	if ( lf.GetProfile(pr) )
		m_lf = lf;

	m_nBrowserOperatingMode	= pr.GetValuei(_T("BrowserOperatingMode"), s_BrowserOperatingMode);
}



////////////////////////////////////////////////////////////////////////////////
//CMainPropertyPage2の定義
////////////////////////////////////////////////////////////////////////////////

int CALLBACK CMainPropertyPage2::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		if (lpData)
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}



CString CMainPropertyPage2::BrowseForFolder(const CString& strTitle, const CString& strNowPath)
{
	TCHAR		szDisplayName[MAX_PATH] = _T("\0");

	BROWSEINFO	bi = {
		m_hWnd,
		NULL,
		szDisplayName,
		LPCTSTR(strTitle),				//+++ strTitle.GetBuffer(0),	//+++
		BIF_RETURNONLYFSDIRS,
		&BrowseCallbackProc,
		(LPARAM) LPCTSTR(strNowPath),	//+++ (LPARAM)strNowPath.GetBuffer(0),
		0
	};

	CItemIDList idl;

	idl.Attach( ::SHBrowseForFolder(&bi) );
	return idl.GetPath();
}



// Constructor
CMainPropertyPage2::CMainPropertyPage2(HWND hWnd, CRecentClosedTabList& rRecent) : 
	m_wnd(hWnd), 
	m_rRecentClosedTabList(rRecent),
	m_nMRUCountMin(CRecentClosedTabList::kMaxEntries_Min),
	m_nMRUCountMax(CRecentClosedTabList::kMaxEntries_Max)
{
	m_bInit 	     = FALSE;

	//+++ mainframe.cppのInitStatusBarでの初期値にあわせて150を125に変更.
	m_nSzPain1	     = 125;
	m_nSzPain2	     = 125;

	m_nChkSwapPain   = FALSE;

	m_nShowMenu      = FALSE;
	m_nShowToolBar   = FALSE;
	m_nShowAdress    = FALSE;
	m_nShowTab	     = FALSE;
	m_nShowLink      = FALSE;
	m_nShowSearch    = FALSE;
	m_nShowStatus    = FALSE;

	m_nMinBtn2Tray   = 0;		//+++

	_SetData();
}



// Overrides
BOOL CMainPropertyPage2::OnSetActive()
{
	if (!m_bInit) {
		m_bInit = TRUE;
		InitCtrls();
		DoDataExchange(DDX_LOAD);
	}

	SetModified(TRUE);
	return TRUE;
}



BOOL CMainPropertyPage2::OnKillActive()
{
	return TRUE;
}



BOOL CMainPropertyPage2::OnApply()
{
	if ( DoDataExchange(DDX_SAVE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CMainPropertyPage2::_GetData()
{
	CIniFileO pr( g_szIniFileName, _T("StatusBar") );
	pr.SetValue( MAKELONG(m_nSzPain1, m_nSzPain2), _T("SizePain") );
	pr.SetValue( m_nChkSwapPain 				 , _T("SwapPain") );
	//x pr.Close(); //+++

	//x CIniFileO pr( g_szIniFileName, _T("FullScreen") );
	pr.ChangeSectionName( _T("FullScreen")	);
	pr.SetValue( m_nShowMenu	, _T("ShowMenu")	);
	pr.SetValue( m_nShowToolBar , _T("ShowToolBar") );
	pr.SetValue( m_nShowAdress	, _T("ShowAdress")	);
	pr.SetValue( m_nShowTab 	, _T("ShowTab") 	);
	pr.SetValue( m_nShowLink	, _T("ShowLink")	);
	pr.SetValue( m_nShowSearch	, _T("ShowSearch")	);
	pr.SetValue( m_nShowStatus	, _T("ShowStatus")	);
	//pr.Close();

	CMainOption::s_bTravelLogGroup = m_nTravelLogGroup != 0;	//+++ ? true : false;
	CMainOption::s_bTravelLogClose = m_nTravelLogClose != 0;	//+++ ? true : false;

	m_rRecentClosedTabList.SetMaxEntries(s_nMaxRecentClosedTabCount);
	m_rRecentClosedTabList.SetMenuType(s_RecentClosedTabMenuType);

  #if 0	//+++ 失敗
	if (m_nTitleBarStrSwap)	CMainOption::s_dwMainExtendedStyle2 	 |=  MAIN_EX2_TITLEBAR_STR_SWAP;	//+++ 追加.
	else					CMainOption::s_dwMainExtendedStyle2 	 &= ~MAIN_EX2_TITLEBAR_STR_SWAP;	//+++ 追加.
  #endif
  #if 1 //+++ 別のオプションが管理している変数を間借りして追加...やめ.. やっぱり復活
	CMainOption::s_dwMainExtendedStyle2 &= ~(MAIN_EX2_CLOSEBTN2TRAY|MAIN_EX2_MINBTN2TRAY);
	if (m_nMinBtn2Tray == 2)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_CLOSEBTN2TRAY;
	else if (m_nMinBtn2Tray == 1)
		CMainOption::s_dwMainExtendedStyle2 |= MAIN_EX2_MINBTN2TRAY;
  #endif

	WriteProfile();
}



void CMainPropertyPage2::_SetData()
{
	// UH
	CIniFileI	pr( g_szIniFileName, _T("StatusBar") );
	DWORD		dwVal = 0;
	if (pr.QueryValue( dwVal, _T("SizePain")	) == ERROR_SUCCESS) {
		m_nSzPain1 = LOWORD(dwVal);
		m_nSzPain2 = HIWORD(dwVal);
	}

	//+++ QueryValue(DWORD,)は、もとより成功したときにしか値を設定しないので、エラーチェックは不要.
	pr.QueryValue( m_nChkSwapPain , _T("SwapPain")	 );

	//+++ QueryValue(DWORD,)は、もとより成功したときにしか値を設定しないので、エラーチェックは不要.
	pr.ChangeSectionName( _T("FullScreen") );
	pr.QueryValue( m_nShowMenu	 , _T("ShowMenu")	 );
	pr.QueryValue( m_nShowToolBar, _T("ShowToolBar") );
	pr.QueryValue( m_nShowAdress , _T("ShowAdress")  );
	pr.QueryValue( m_nShowTab	 , _T("ShowTab")	 );
	pr.QueryValue( m_nShowLink	 , _T("ShowLink")	 );
	pr.QueryValue( m_nShowSearch , _T("ShowSearch")  );
	pr.QueryValue( m_nShowStatus , _T("ShowStatus")  );
	//pr.Close();

	m_nTravelLogGroup = CMainOption::s_bTravelLogGroup != 0;	//+++ ? 1 : 0;
	m_nTravelLogClose = CMainOption::s_bTravelLogClose != 0;	//+++ ? 1 : 0;

  #if 0	//+++ 失敗
	m_nTitleBarStrSwap= (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_TITLEBAR_STR_SWAP) != 0;	//+++ 追加.
  #endif

  #if 1 //+++ 別のオプションが管理している変数を間借りして追加...やめ... やっぱここで
	if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)
		m_nMinBtn2Tray = 2;
	else if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)
		m_nMinBtn2Tray = 1;
  #endif
}



void CMainPropertyPage2::InitCtrls()
{
	CString   strText;
	strText.Format(_T("(%d-%d)"), m_nMRUCountMin, m_nMRUCountMax);

	MtlSetWindowText(GetDlgItem(IDC_STATIC_MRU_MINMAX), strText);

	CComboBox cmbType( GetDlgItem(IDC_COMBO_MRU_MENUTYPE) );
	cmbType.AddString( _T("0 - URL") );
	cmbType.AddString( _T("1 - タイトル") );
	cmbType.AddString( _T("2 - タイトル - URL") );
	cmbType.SetCurSel(0);
}
