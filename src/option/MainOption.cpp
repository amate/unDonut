/**
 *	@file	MainOption.cpp
 *	@brief	donutのオプション : 全般, 全般2
 */

#include "stdafx.h"
#include "MainOption.h"
#include "../MtlWeb.h"
#include "../DonutPFunc.h"
#include "../ie_feature_control.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




////////////////////////////////////////////////////////////////////////////////
//CMainOptionの定義
////////////////////////////////////////////////////////////////////////////////

DWORD		CMainOption::s_dwMainExtendedStyle		= /*MAIN_EX_NEWWINDOW |*/ MAIN_EX_KILLDIALOG | MAIN_EX_NOMDI | MAIN_EX_INHERIT_OPTIONS;

DWORD		CMainOption::s_dwMainExtendedStyle2 	= 0;
DWORD		CMainOption::s_dwExplorerBarStyle		= 0;
DWORD		CMainOption::s_dwMaxWindowCount 		= 0;
DWORD		CMainOption::s_dwBackUpTime 			= 1;
DWORD		CMainOption::s_dwAutoRefreshTime		= 10;			// UDT DGSTR ( dai
bool		CMainOption::s_bTabMode 				= false;

volatile bool CMainOption::s_bAppClosing			= false;
volatile bool CMainOption::s_bIgnoreUpdateClipboard	= false;

CRecentDocumentListFixed *		CMainOption::s_pMru = NULL;


DWORD		CMainOption::s_dwErrorBlock 			= 0;

CString *	CMainOption::s_pstrExplorerUserDirectory= NULL;

bool		CMainOption::s_bTravelLogGroup			= false;
bool		CMainOption::s_bTravelLogClose			= false;

bool		CMainOption::s_bStretchImage			= false;

bool		CMainOption::s_bIgnore_blank			= false;
bool		CMainOption::s_bUseCustomFindBar		= false;

CMainOption::CMainOption()
{
}



void CMainOption::GetProfile()
{
	DWORD			dwTravelLogGroup = 0;
	DWORD			dwTravelLogClose = 0;
	{
		CIniFileI	pr( g_szIniFileName, _T("Main") );
		pr.QueryValue( s_dwMainExtendedStyle	, _T("Extended_Style")		);
		pr.QueryValue( s_dwMainExtendedStyle2	, _T("Extended_Style2") 	);
		pr.QueryValue( s_dwMaxWindowCount		, _T("Max_Window_Count")	);
		pr.QueryValue( s_dwBackUpTime			, _T("BackUp_Time") 		);
		pr.QueryValue( s_dwAutoRefreshTime		, _T("Auto_Refresh_Time")	);	// UDT DGSTR ( dai
		pr.QueryValue( s_dwExplorerBarStyle 	, _T("ExplorerBar_Style")	);	// UH
		pr.QueryValue( s_dwErrorBlock			, _T("ErrorBlock")			);	//minit
		pr.QueryValue( dwTravelLogGroup 		, _T("TravelLogGroup")		);
		pr.QueryValue( dwTravelLogClose 		, _T("TravelLogClose")		);
		pr.Close();
	}

	s_bTravelLogGroup			= dwTravelLogGroup != 0;		//+++ ? true : false;
	s_bTravelLogClose			= dwTravelLogClose != 0;		//+++ ? true : false;

	ATLASSERT(s_pMru == 0);
	s_pMru	= new CRecentDocumentListFixed;
	s_pMru->SetMaxEntries(9);
	s_pMru->ReadFromIniFile();

	s_bTabMode		= (s_dwMainExtendedStyle & MAIN_EX_NOMDI) != 0;
	s_bIgnore_blank	= (s_dwMainExtendedStyle & MAIN_EX_IGNORE_BLANK) != 0;
	s_bUseCustomFindBar = (s_dwMainExtendedStyle & MAIN_EX_USECUSTOMFINDBER) != 0;

	// NOTE. If all the Web Browser server on your desktop is unloaded, some OS automatically goes online.
	//		 And if all the Web Browser server on you application is unloaded, some OS automatically goes online.
	if ( _check_flag(MAIN_EX_LOADGLOBALOFFLINE, s_dwMainExtendedStyle) ) {
		MtlSetGlobalOffline( _check_flag(MAIN_EX_GLOBALOFFLINE, s_dwMainExtendedStyle) );
	}

	//メモリ確保するんでちゃんと消すように
	{
		CIniFileI	pr( g_szIniFileName, _T("Explorer_Bar") );
		ATLASSERT(s_pstrExplorerUserDirectory == NULL);
		s_pstrExplorerUserDirectory = new CString( pr.GetStringUW(_T("UserDirectory")) );
	}

	DWORD dwAutoResize = 0;
	CIniFileI	pr( g_szIniFileName, _T("Main") );
	pr.QueryValue( dwAutoResize, _T("AutoResize") );
}



void CMainOption::WriteProfile()
{
	if ( MtlIsGlobalOffline() )
		s_dwMainExtendedStyle |= MAIN_EX_GLOBALOFFLINE;
	else
		s_dwMainExtendedStyle &= ~MAIN_EX_GLOBALOFFLINE;

	DWORD		dwTravelLogGroup = s_bTravelLogGroup != 0;		//+++ ? 1 : 0;
	DWORD		dwTravelLogClose = s_bTravelLogClose != 0;		//+++ ? 1 : 0;
	CIniFileO	pr( g_szIniFileName, _T("Main") );
	{
		pr.SetValue( s_dwMainExtendedStyle	, _T("Extended_Style")		);
		pr.SetValue( s_dwMainExtendedStyle2 , _T("Extended_Style2") 	);
		pr.SetValue( s_dwExplorerBarStyle	, _T("ExplorerBar_Style")	);	// UDT DGSTR ( dai
		pr.SetValue( s_dwMaxWindowCount 	, _T("Max_Window_Count")	);
		pr.SetValue( s_dwBackUpTime 		, _T("BackUp_Time") 		);
		pr.SetValue( s_dwAutoRefreshTime	, _T("Auto_Refresh_Time")	);	// UDT DGSTR ( dai
		pr.SetValue( dwTravelLogGroup		, _T("TravelLogGroup")		);
		pr.SetValue( dwTravelLogClose		, _T("TravelLogClose")		);
	}

	ATLASSERT(s_pMru != NULL);
	if (s_pMru) {									//+++ 念のためチェック.
		s_pMru->WriteToIniFile();

		if (s_dwMainExtendedStyle2 & MAIN_EX2_DEL_RECENTCLOSE)
			s_pMru->DeleteIniKeys();

		delete	s_pMru;
		s_pMru	= 0;								//+++ deleteしたらクリア.
	}

	//確保した分はちゃんと消す
	if (s_pstrExplorerUserDirectory) {			//+++ 念のため先にチェック.
		pr.ChangeSectionName(_T("Explorer_Bar"));
		pr.SetStringUW( *s_pstrExplorerUserDirectory, _T("UserDirectory") );
		delete s_pstrExplorerUserDirectory;
		s_pstrExplorerUserDirectory = 0;		//+++ deleteしたらクリア.
	}
}



void CMainOption::SetExplorerUserDirectory(const CString &strPath)
{
	ATLASSERT(s_pstrExplorerUserDirectory != NULL);
	if (s_pstrExplorerUserDirectory)
		*s_pstrExplorerUserDirectory	= strPath;
}



const CString& CMainOption::GetExplorerUserDirectory()
{
	ATLASSERT(s_pstrExplorerUserDirectory != NULL);
	return *s_pstrExplorerUserDirectory;
}



void CMainOption::SetMRUMenuHandle(HMENU hMenu, int nPos)
{
	HMENU hFileMenu = ::GetSubMenu(hMenu, 0);

  #ifdef _DEBUG
	// absolute position, can change if menu changes
	TCHAR 	szMenuString[256];
	szMenuString[0] = 0;	//+++
	::GetMenuString(hFileMenu, nPos, szMenuString, sizeof (szMenuString), MF_BYPOSITION);
	ATLASSERT(lstrcmp( szMenuString, _T("最近閉じたファイル(&F)") ) == 0);
  #endif	//_DEBUG
	HMENU hMruMenu	= ::GetSubMenu(hFileMenu, nPos);
	CMainOption::s_pMru->SetMenuHandle(hMruMenu);
	CMainOption::s_pMru->UpdateMenu();
}



bool CMainOption::IsQualify(int nWindowCount)
{
	if ( !(s_dwMainExtendedStyle & MAIN_EX_WINDOWLIMIT) )
		return true;

	if (nWindowCount < (int) s_dwMaxWindowCount)
		return true;
	else
		return false;
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
	if (m_nLimit				/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_WINDOWLIMIT;
	if (m_nNoCloseDFG			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOCLOSEDFG;
	if (m_nNoMDI				/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOMDI;
	if (m_nBackUp				/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_BACKUP;
	if (m_nAddFavoriteOldShell	/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_ADDFAVORITEOLDSHELL;
	if (m_nOrgFavoriteOldShell	/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_ORGFAVORITEOLDSHELL;

	if (m_nRegisterAsBrowser	/*== 1*/) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_REGISTER_AS_BROWSER;
		MtlSendOnCommand(m_wnd, ID_REGISTER_AS_BROWSER);
	} else {
		MtlSendOffCommand(m_wnd, ID_REGISTER_AS_BROWSER);
	}

	if (m_nLoadGlobalOffline	/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_LOADGLOBALOFFLINE;
	if (m_nKillDialog			/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_KILLDIALOG;
	if (m_nInheritOptions		/*== 1*/) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_INHERIT_OPTIONS;

	//+++ if (m_nNoCloseNL				) CMainOption::s_dwMainExtendedStyle |= MAIN_EX_NOCLOSE_NAVILOCK;			//+++ 追加

#if 0	//* あとで
	ie_coInternetSetFeatureEnabled(FEATURE_SECURITYBAND    , SET_FEATURE_ON_PROCESS, m_nKillDialog);	//+++
#endif

	if (s_bIgnore_blank)		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_IGNORE_BLANK;
	if (s_bUseCustomFindBar)	CMainOption::s_dwMainExtendedStyle |= MAIN_EX_USECUSTOMFINDBER;

	// update max window count
	CMainOption::s_dwMaxWindowCount  = m_nMaxWindowCount;
	CMainOption::s_dwBackUpTime 	 = m_nBackUpTime;
	// UDT DGSTR ( dai
	m_nAutoRefreshTime				 = m_nAutoRefTimeMin * 60 + m_nAutoRefTimeSec;
	CMainOption::s_dwAutoRefreshTime = m_nAutoRefreshTime;
	// ENDE
	m_wnd.PostMessage(WM_COMMAND, ID_BACKUPOPTION_CHANGED);

	CIniFileO pr( g_szIniFileName, _T("Main") );
	m_lf.WriteProfile(pr);
}



void CMainPropertyPage::_SetData()
{
	//+++ メモ: MAIN_EX_FULLSCREEN, MAIN_EX_GLOBALOFFLINE, MAIN_EX2_NOCSTMMENU, MAIN_EX2_DEL_RECENTCLOSE はここで反映しない.

	m_nNewWindow		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NEWWINDOW		  ) != 0;		//+++ ? 1 : 0;
	m_nNoActivate		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE		  ) != 0;		//+++ ? 1 : 0;
	m_nNoActivateNewWin    = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN  ) != 0;		//+++ ? 1 : 0;
	m_nOneInstance		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_ONEINSTANCE		  ) != 0;		//+++ ? 1 : 0;
	m_nLimit			   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_WINDOWLIMIT		  ) != 0;		//+++ ? 1 : 0;
	m_nNoCloseDFG		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG		  ) != 0;		//+++ ? 1 : 0;
	m_nNoMDI			   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOMDI			  ) != 0;		//+++ ? 1 : 0;
	m_nBackUp			   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_BACKUP			  ) != 0;		//+++ ? 1 : 0;
	m_nMaxWindowCount	   = CMainOption::s_dwMaxWindowCount;
	m_nBackUpTime		   = CMainOption::s_dwBackUpTime;
	// UDT DGSTR ( dai
	m_nAutoRefreshTime	   = CMainOption::s_dwAutoRefreshTime;
	m_nAutoRefTimeMin	   = CMainOption::s_dwAutoRefreshTime / 60;
	m_nAutoRefTimeSec	   = CMainOption::s_dwAutoRefreshTime % 60;
	// ENDE
	m_nAddFavoriteOldShell = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_ADDFAVORITEOLDSHELL) != 0;		//+++ ? 1 : 0;
	m_nOrgFavoriteOldShell = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_ORGFAVORITEOLDSHELL) != 0;		//+++ ? 1 : 0;
	m_nLoadGlobalOffline   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_LOADGLOBALOFFLINE  ) != 0;		//+++ ? 1 : 0;
	m_nKillDialog		   = (CMainOption::s_dwMainExtendedStyle & MAIN_EX_KILLDIALOG		  ) != 0;		//+++ ? 1 : 0;
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
	TCHAR		szDisplayName[MAX_PATH];
	ZeroMemory(szDisplayName, sizeof szDisplayName);

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
CMainPropertyPage2::CMainPropertyPage2(HWND hWnd) : m_wnd(hWnd)
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

	m_nMRUCount      = 9;
	m_nMRUCountMin   = 2;
	m_nMRUCountMax   = 64;
	m_nMRUMenuType   = 0;
	m_nMinBtn2Tray   = 0;		//+++
  #ifndef USE_DIET
	m_nImgAutoResize = 0;		//+++
  #endif

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

	CMainOption::s_pMru->ResetMenu();
	CMainOption::s_pMru->SetMaxEntries(m_nMRUCount);
	CMainOption::s_pMru->SetMenuType(m_nMRUMenuType);
	CMainOption::s_pMru->UpdateMenu();

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

  #ifndef USE_DIET	//+++ 画像ファイルの自動リサイズの設定
	pr.ChangeSectionName( _T("ETC") );
	pr.SetValue( m_nImgAutoResize == 0, _T("ImageAutoSize_NouseLClick") );
	pr.SetValue( m_nImgAutoResize == 1, _T("ImageAutoSize_FirstOn")     );
	pr.Close();
  #endif
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

	m_nMRUCountMin	  = CMainOption::s_pMru->m_nMaxEntries_Min;
	m_nMRUCountMax	  = CMainOption::s_pMru->m_nMaxEntries_Max;
	m_nMRUCount 	  = CMainOption::s_pMru->GetMaxEntries();
	m_nMRUMenuType	  = CMainOption::s_pMru->GetMenuType();

  #if 0	//+++ 失敗
	m_nTitleBarStrSwap= (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_TITLEBAR_STR_SWAP) != 0;	//+++ 追加.
  #endif

  #if 1 //+++ 別のオプションが管理している変数を間借りして追加...やめ... やっぱここで
	if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)
		m_nMinBtn2Tray = 2;
	else if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)
		m_nMinBtn2Tray = 1;
  #endif

  #ifndef USE_DIET	//+++ 画像ファイルの自動リサイズの設定
	pr.ChangeSectionName( _T("ETC") );
	bool bImgAuto_NouseLClk	= pr.GetValue(_T("ImageAutoSize_NouseLClick")) != 0;
	bool nImgSclSw			= pr.GetValue(_T("ImageAutoSize_FirstOn")) != 0;
	m_nImgAutoResize		= bImgAuto_NouseLClk ? 0 : nImgSclSw ? 1 : 2;
	pr.Close();
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
