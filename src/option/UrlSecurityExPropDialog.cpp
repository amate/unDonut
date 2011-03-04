// UrlSecurityExPropDialog.cpp

#include "stdafx.h"
#include "UrlSecurityExPropDialog.h"
#include "UrlSecurityOption.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../MtlMisc.h"
#include "../MtlFile.h"
#include "../DonutViewOption.h"
#include "../ExStyle.h"


#if _MSC_VER >= 1500
#include <regex>
using std::tr1::basic_regex;
using std::tr1::match_results;
using std::tr1::regex_match;
#else
#include <boost/regex.hpp>
using boost::basic_regex;
using boost::match_results;
using boost::regex_match;
#endif


/////////////////////////////////////////////////////////////
// CExPropertyDialog2

// Constructor
CExPropertyDialog2::CExPropertyDialog2(CString &strUrlFile, unsigned& rFlags, unsigned& rExopts, unsigned& rExopts2)
	: m_rRsltUrl(strUrlFile)
	, m_rRsltFlags(rFlags)
	, m_rRsltExopts(rExopts)
	, m_rRsltExopts2(rExopts2)	//+++ 引数追加.
	, m_urlEditBtnSw(0)
	, m_bRegex((rFlags & USP_USEREGEX) != 0)
	, m_bOnlyThisURL((rFlags & USP_ONLYTHISURL) != 0)
{
	DWORD	dwExProp = rExopts;
	m_nImage			= exProp2btn(dwExProp, EXPROP_DLIMAGE	);
	m_nVideo			= exProp2btn(dwExProp, EXPROP_VIDEO		);
	m_nSound			= exProp2btn(dwExProp, EXPROP_SOUND		);
	m_nRunActiveX		= exProp2btn(dwExProp, EXPROP_RUNACTIVEX );
	m_nDLActiveX		= exProp2btn(dwExProp, EXPROP_DLACTIVEX	);
	m_nScript			= exProp2btn(dwExProp, EXPROP_SCRIPT 	);
	m_nJava 			= exProp2btn(dwExProp, EXPROP_JAVA		);

	m_nNaviLock 		= exProp2btn(dwExProp, EXPROP_NAVI		);
	m_nFilter			= exProp2btn(dwExProp, EXPROP_FILTER 	);
	m_nGesture			= exProp2btn(dwExProp, EXPROP_GETSTURE	);
	m_nBlockMailTo		= exProp2btn(dwExProp, EXPROP_MAILTO 	);

	DWORD dwExProp2 = rExopts2;										//+++
	m_nFlatView 		= exProp2btn(dwExProp2, EXPROPOPT_FLATVIEW  );	//+++

	//+++ m_nNoCloseNaviLock =	拡張プロパティでは設定できないとにする.

	int   nFlag 		= dwExProp / EXPROP_REFRESH_NONE;
	m_nReload			= 0;

	while ( (nFlag >>= 1) > 0 )
		m_nReload++;
}


// Message map

LRESULT CExPropertyDialog2::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetWindowText(m_rRsltUrl);
	CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetReadOnly(TRUE);
	m_urlEditBtnSw = 0;
	CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).EnableWindow(TRUE);
	CButton( GetDlgItem(IDC_BTN_EXPROP_URL) ).EnableWindow(TRUE);

	DoDataExchange(DDX_LOAD);
	return 1;
}

// OK
void CExPropertyDialog2::OnOK(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	DoDataExchange(DDX_SAVE);

	DWORD		dwExProp  = 0;
	dwExProp  |= btn2ExProp(m_nImage		, EXPROP_DLIMAGE	);
	dwExProp  |= btn2ExProp(m_nVideo		, EXPROP_VIDEO 	 	);
	dwExProp  |= btn2ExProp(m_nSound		, EXPROP_SOUND 	 	);
	dwExProp  |= btn2ExProp(m_nRunActiveX	, EXPROP_RUNACTIVEX );
	dwExProp  |= btn2ExProp(m_nDLActiveX	, EXPROP_DLACTIVEX  );
	dwExProp  |= btn2ExProp(m_nScript		, EXPROP_SCRIPT	 	);
	dwExProp  |= btn2ExProp(m_nJava			, EXPROP_JAVA		);

	dwExProp  |= btn2ExProp(m_nNaviLock		, EXPROP_NAVI		);
	dwExProp  |= btn2ExProp(m_nFilter		, EXPROP_FILTER	 	);
	dwExProp  |= btn2ExProp(m_nGesture 		, EXPROP_GETSTURE	);
	dwExProp  |= btn2ExProp(m_nBlockMailTo	, EXPROP_MAILTO		);

	if (m_nReload != -1)
		dwExProp |= (EXPROP_REFRESH_NONE << m_nReload);

	//+++	m_dwExProp = dwExProp;

	DWORD		dwExProp2 = m_rRsltExopts2 & 3;							//+++
	dwExProp2  |= btn2ExProp(m_nFlatView	, EXPROPOPT_FLATVIEW	);	//+++

	// セーブ
	m_rRsltFlags = 0;
	if (m_bRegex)		m_rRsltFlags |= USP_USEREGEX;
	if (m_bOnlyThisURL)	m_rRsltFlags |= USP_ONLYTHISURL;

	m_rRsltExopts = dwExProp;
	m_rRsltExopts2= dwExProp2;	//+++

	//+++ URL の編集を行った場合
	CString 	strUrl = MtlGetWindowText( GetDlgItem(IDC_EDIT_EXPROP_URL) );
	if (m_rRsltUrl != strUrl) {
		m_rRsltUrl   = strUrl;
		if (m_bRegex) {
			try {
				basic_regex<TCHAR>	rx(strUrl);
				regex_search(_T("test"), rx);
			}
			catch (...) {
				MessageBox(_T("正規表現が間違っています。"));
				return;
			}
		}
	}

	EndDialog(nID);
}

// キャンセル
void CExPropertyDialog2::OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	EndDialog(nID);
}

// 消去
void CExPropertyDialog2::OnDelete(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	m_nImage		= 0;
	m_nVideo		= 0;
	m_nSound		= 0;
	m_nRunActiveX	= 0;
	m_nDLActiveX	= 0;
	m_nScript		= 0;
	m_nJava 		= 0;
	m_nNaviLock 	= 0;
	m_nFlatView 	= 0;	//+++
	m_nFilter		= 0;
	m_nGesture		= 0;
	m_nBlockMailTo	= 0;
	m_nReload		= 0;


	m_bRegex		= 0;
	m_bOnlyThisURL	= 0;

	DoDataExchange(DDX_LOAD);
}

// 現在の標準設定
void CExPropertyDialog2::OnDefault(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	DWORD dwDLFlag	= CDLControlOption::s_dwDLControlFlags;
	DWORD dwExStyle = CDLControlOption::s_dwExtendedStyleFlags;

	//m_bRegex		= 0;
	m_nReload	   	= 0;

	m_nImage	   	=  (dwDLFlag & DLCTL_DLIMAGES		 )	!= 0;
	m_nVideo	   	=  (dwDLFlag & DLCTL_VIDEOS			 )	!= 0;
	m_nSound	   	=  (dwDLFlag & DLCTL_BGSOUNDS		 )	!= 0;
	m_nRunActiveX  	= !(dwDLFlag & DLCTL_NO_RUNACTIVEXCTLS);
	m_nDLActiveX   	= !(dwDLFlag & DLCTL_NO_DLACTIVEXCTLS );
	m_nScript	   	= !(dwDLFlag & DLCTL_NO_SCRIPTS		 );
	m_nJava 	   	= !(dwDLFlag & DLCTL_NO_JAVA 		 );

	m_nNaviLock    	=  (dwExStyle & DVS_EX_OPENNEWWIN	 )	!= 0;
	m_nFlatView    	=  (dwExStyle & DVS_EX_FLATVIEW		 )	!= 0;	//+++
	m_nFilter	   	=  (dwExStyle & DVS_EX_MESSAGE_FILTER )	!= 0;
	m_nGesture	   	=  (dwExStyle & DVS_EX_MOUSE_GESTURE  )	!= 0;
	m_nBlockMailTo 	=  (dwExStyle & DVS_EX_BLOCK_MAILTO	 )	!= 0;

	DoDataExchange(DDX_LOAD);
}

// 標準設定任せ
void CExPropertyDialog2::OnDfltBase(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	m_nReload	    = 0;

	m_nImage		= BST_INDETERMINATE;
	m_nVideo		= BST_INDETERMINATE;
	m_nSound		= BST_INDETERMINATE;
	m_nRunActiveX	= BST_INDETERMINATE;
	m_nDLActiveX	= BST_INDETERMINATE;
	m_nScript		= BST_INDETERMINATE;
	m_nJava 		= BST_INDETERMINATE;
	m_nNaviLock 	= BST_INDETERMINATE;
	m_nFlatView		= BST_INDETERMINATE;	//+++
	m_nFilter		= BST_INDETERMINATE;
	m_nGesture		= BST_INDETERMINATE;
	m_nBlockMailTo	= BST_INDETERMINATE;

	DoDataExchange(DDX_LOAD);
}

// URL
//+++ URL を編集可能に変更. URLを表示させない場合は、URLボタンもなしに.
void CExPropertyDialog2::OnBtnExpropUrl(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetReadOnly( m_urlEditBtnSw );
	m_urlEditBtnSw ^= 1;
	//DoDataExchange(FALSE);
}



