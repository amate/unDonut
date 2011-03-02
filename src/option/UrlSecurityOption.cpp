/**
 *	@file	UrlSecurityOption.cpp
 *	@brief	donutのオプション : URL別セキュリティ.
 *	@note
 *		undonut+modで追加.
 */

#include "stdafx.h"

#include "UrlSecurityOption.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../MtlMisc.h"
#include "../MtlFile.h"
#include "../DonutViewOption.h"
#include "../ExStyle.h"
#include "UrlSecurityExPropDialog.h"


#if defined USE_ATLDBGMEM
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#elif defined USE_DIET || _ATL_VER < 0x700	//+++ Diet時。(atl3のときはdiet扱いでコンパイルを無理やり通す)
 #undef USE_REGEX
#else
 #define USE_REGEX
#endif





#ifdef USE_REGEX
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
#endif


CUrlSecurityOption::COptUrlList 	CUrlSecurityOption::s_urlSecurity;
bool 								CUrlSecurityOption::s_bValid			= true;
bool 								CUrlSecurityOption::s_bActivePageToo	= true;



void CUrlSecurityOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("UrlSecurity") );
	int			nValid	= int( pr.GetValue( _T("IsValid"), -1 ) );
	if (nValid >= 0)
		s_bValid = nValid != 0;
  #if 1	//+++ +mod.1.35にて、必ず0書き込みされてしまってるので、しばらく強制的に1にする...
	s_bActivePageToo = 1;
  #else
	int			nActPagNavi	= int( pr.GetValue( _T("ActPagNavi"), -1 ) );
	if (nActPagNavi >= 0)
		s_bActivePageToo = nActPagNavi != 0;
  #endif
	pr.Close();

	//+++ ファイルより読み込む
	std::list<CString>	urls;
	::FileReadString(_GetFilePath( _T("UrlEntry.ini") ), &urls);

	//+++ ファイルから読み込んだデータを分解する
	COptUrlList().swap( s_urlSecurity );
	for (std::list<CString>::iterator it = urls.begin();
		it != urls.end();
		++it)
	{
		LPCTSTR		p = LPCTSTR( *it );
		if (p == NULL)
			continue;
		unsigned	f = _tcstoul(p, (LPTSTR*)&p, 0);
		if (*p != _T(','))
			continue;
		unsigned	o = _tcstoul(p+1, (LPTSTR*)&p, 0);
		if (*p != _T(','))
			continue;
	  #if 0
		unsigned o2 = f & ~3;
		f &= 3;
	  #else	// 1.47で引パラメータ増えたので、それ以前のファイルを読んだときの辻褄あわせ.
		unsigned o2 = 8;	//* あとで	//+++
		if (p[1] >= _T('0') && p[1] <= _T('9')) {
			LPCTSTR q = p;
			o2		= _tcstoul(p+1, (LPTSTR*)&p, 0);
			if (*p != _T(',')) {
				p   = q;
				o2  = 8;
			}
		}
	  #endif
		CString	url( p + 1 );
		url.TrimLeft(_T(" \t\r\n　"));
		url.TrimRight(_T(" \t\r\n　"));
		s_urlSecurity.push_back( COptUrl( f, o, o2, url) );
	}
}



void CUrlSecurityOption::WriteProfile()
{
	CIniFileO	 pr( g_szIniFileName, _T("UrlSecurity") );
	pr.DeleteSection();
	pr.SetValue( s_bValid		  != 0, _T("IsValid"   ) );
	pr.SetValue( s_bActivePageToo != 0, _T("ActPagNavi") );

	//+++ ファイルから読み込んだデータを分解する
	std::list<CString>		urls;
	for (COptUrlList::iterator it = s_urlSecurity.begin();
		it != s_urlSecurity.end();
		++it)
	{
	  #if 1	//+++
		urls.push_back( Misc::StrFmt(_T("%u,%u,%u,%s"), it->m_flags, it->m_opts, it->m_opts2, LPCTSTR(it->m_url)) );
	  #else
		unsigned fl = (it->m_flags & 1) | (it->opts2 & ~3);
		urls.push_back( Misc::StrFmt(_T("%u,%u,%s"), fl, it->m_opts, LPCTSTR(it->m_url)) );
	  #endif
	}

	COptUrlList().swap( s_urlSecurity );

	::FileWriteString( _GetFilePath( _T("UrlEntry.ini") ), &urls );
}



CUrlSecurityOption::COptUrlList::iterator	CUrlSecurityOption::get_optUrlList_iterator(unsigned no)
{
	CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::s_urlSecurity.begin();
	for (unsigned i = 0; i < no && it != s_urlSecurity.end(); ++i)
		++it;
	return it;
}



#if 0
bool CUrlSecurityOption::SearchString(const CString &strURL)
{
	if (!s_bValid)
		return false;

	return MtlSearchStringWildCard(s_urlSecurity.begin(), s_urlSecurity.end(), strURL);
}
#endif



bool CUrlSecurityOption::FindUrl(const CString &strURL, DWORD* pExProp, DWORD* pExPropOpt, DWORD* pFlags)
{
	if (!s_bValid)
		return false;
	bool rc = false;
  #if 1	//+++
	int	n = 0;
	auto it = s_urlSecurity.begin();
	for (; it != s_urlSecurity.end(); ++it) {
		CString 	ptn = it->m_url;
		if (it->m_flags & USP_USEREGEX) {	//+++ 正規表現を有効にする場合.
			if (ptn[0] != _T('*')) {
				try {	//正規表現が不正だとエラーが投げられるので、無視するようにする
					basic_regex<TCHAR> 	re(LPCTSTR(it->m_url));
					if ( regex_search(LPCTSTR(strURL), re) ) {
						rc = true;
						break;
	 				}
				} catch (...) {
					;
				}
			}
		} else {
			if (ptn.Find(_T('*')) < 0 && ptn.Find(_T('?')) < 0) {
				//ptn += _T('*');
				if (ptn == strURL) {
					rc = true;
					break;
				}
			} else {
				if ( MtlStrMatchWildCard(ptn, strURL) ) {
					rc = true;
					break;
				}
			}
		}
	}
	if (rc) {
		if (pFlags)
			*pFlags		= it->m_flags;
		if (pExProp)
			*pExProp	= it->m_opts;
		if (pExPropOpt)
			*pExPropOpt	= it->m_opts2 & ~1;
	}
	return rc;
  #else
	COptUtlList::iterator it = MtlFindStringWildCard(s_urlSecurity.begin(), s_urlSecurity.end(), strURL);
	if (it == s_urlSecurity.end())
		return false;
	if (pFlags)
		*pFlags		= it->m_flags;
	if (pExprop)
		*pExprop	= it->m_opts;
  #endif
}



void CUrlSecurityOption::Add(unsigned flags, unsigned opts, unsigned opts2, const CString &strURL)
{
	COptUrlList::iterator it = std::find(s_urlSecurity.begin(), s_urlSecurity.end(), COptUrl(0,0,0,strURL));
	if (it != s_urlSecurity.end()) {
		it->m_flags = flags;
		it->m_opts  = opts;
		it->m_opts2 = opts2 & ~1;
	} else {
		s_urlSecurity.push_back( COptUrl(flags, opts, opts2, strURL) );
	}
}

// セキュリティをもとに戻すかどうか(このURLでのみ有効)
bool CUrlSecurityOption::IsUndoSecurity(const CString& strURL)
{
	if (!s_bValid)
		return false;

	for (auto it = s_urlSecurity.cbegin(); it != s_urlSecurity.cend(); ++it) {
		if (it->m_flags & USP_ONLYTHISURL) {
			CString 	ptn = it->m_url;
			if (it->m_flags & USP_USEREGEX) {	//+++ 正規表現を有効にする場合.
				if (ptn[0] != _T('*')) {
					try {	//正規表現が不正だとエラーが投げられるので、無視するようにする
						basic_regex<TCHAR> 	re(LPCTSTR(it->m_url));
						if ( regex_search(LPCTSTR(strURL), re) ) {
							return true;
	 					}
					} catch (...) {
						;
					}
				}
			} else {
				if (ptn.Find(_T('*')) < 0 && ptn.Find(_T('?')) < 0) {
					//ptn += _T('*');
					if (ptn == strURL) {
						return true;
					}
				} else {
					if ( MtlStrMatchWildCard(ptn, strURL) ) {
						return true;
					}
				}
			}
		}
	}
	return false;
}



////////////////////////////////////////////////////////////////////////////////
//CUrlSecurityPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

// Constructor
CUrlSecurityPropertyPage::CUrlSecurityPropertyPage(const CString &strAddressBar)
	: m_strAddressBar(strAddressBar)
	, m_wndList(this, 1)
{
	m_nValid  = CUrlSecurityOption::s_bValid		 != 0;
  #if 1	//+++ +mod.1.35にて、必ず0書き込みされてしまってるので、しばらく強制的に1にする...
	m_nActPag = 1;
  #else
	m_nActPag = CUrlSecurityOption::s_bActivePageToo != 0;
  #endif
}



// Overrides
BOOL CUrlSecurityPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return _DoDataExchange(FALSE);
}



BOOL CUrlSecurityPropertyPage::OnKillActive()
{
	return _DoDataExchange(TRUE);
}



BOOL CUrlSecurityPropertyPage::OnApply()
{
	if ( _DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



// overrides
BOOL CUrlSecurityPropertyPage::_DoDataExchange(BOOL bSaveAndValidate)	// get data from controls?
{
	if (!bSaveAndValidate) {											// set data of control
		if (!m_listbox.m_hWnd)
			m_listbox.Attach( GetDlgItem(IDC_IGNORED_URL_LIST) );

		m_listbox.ResetContent();
		
		auto funcAddString = [this](const COptUrl& it) { m_listbox.AddString(it.m_url); };
		std::for_each(CUrlSecurityOption::s_urlSecurity.begin(), CUrlSecurityOption::s_urlSecurity.end(), funcAddString);

		if (!m_edit.m_hWnd)
			m_edit.Attach( GetDlgItem(IDC_URL_EDIT) );

		m_edit.SetWindowText(m_strAddressBar);
	}

	return DoDataExchange(bSaveAndValidate);
}



LRESULT CUrlSecurityPropertyPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.SubclassWindow( GetDlgItem(IDC_IGNORED_URL_LIST) );


	return 0;
}



LRESULT CUrlSecurityPropertyPage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_wndList.UnsubclassWindow();
	return 0;
}



void CUrlSecurityPropertyPage::OnDelCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	//とりあえず選択項目の番号をリストにしてソート
	std::list<int> sellist;
	int 		   nSelCount = m_listbox.GetSelCount();
	int *		   pIdx 	 = (int *) new int[nSelCount];

	if (m_listbox.GetSelItems(nSelCount, pIdx) != LB_ERR) {
		for (int i = 0; i < nSelCount; i++)
			sellist.push_back(pIdx[i]);

		sellist.sort();
		sellist.reverse();

		for (std::list<int>::iterator it = sellist.begin(); it != sellist.end(); ++it) {
			CString 	strURL;
			m_listbox.GetText(*it, strURL);
			CUrlSecurityOption::COptUrlList::iterator it2 = std::find(CUrlSecurityOption::s_urlSecurity.begin(), CUrlSecurityOption::s_urlSecurity.end(), CUrlSecurityOption::COptUrl(0,0,0,strURL));
			if (it2 != CUrlSecurityOption::s_urlSecurity.end())
				CUrlSecurityOption::s_urlSecurity.erase(it2);

			m_listbox.DeleteString(*it);
		}
	}

	delete[] pIdx;

	DispExOpts();
}



void CUrlSecurityPropertyPage::OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	m_listbox.ResetContent();
	CUrlSecurityOption::COptUrlList().swap( CUrlSecurityOption::s_urlSecurity );

	DispExOpts();
}



void CUrlSecurityPropertyPage::OnAddCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	TCHAR szEdit[INTERNET_MAX_PATH_LENGTH+4];
	szEdit[0]	= 0;	//+++

	if (m_edit.GetWindowText(szEdit, INTERNET_MAX_PATH_LENGTH) == 0)
		return;

	int   nIndex = m_listbox.FindStringExact(-1, szEdit);

	if (nIndex == LB_ERR) {
		m_listbox.AddString(szEdit);
		CUrlSecurityOption::Add(0, 0xAAAAAA, 0x8, szEdit);
	} else {
		//m_listbox.SetCurSel(nIndex);
		m_listbox.SetSel(nIndex, TRUE);
	}

	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());

	DispExOpts();
}



void CUrlSecurityPropertyPage::OnSelChange(UINT /*code*/, int /*id*/, HWND /*hWnd*/)
{
	//int nIndex = m_listbox.GetCurSel();
	int 	nIndex	= 0;

	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;

	CString 	strBox;
	m_listbox.GetText(nIndex, strBox);
	m_edit.SetWindowText(strBox);

	DispExOpts();

	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());
}

// リストをダブルクリックした
void CUrlSecurityPropertyPage::OnDblclkList(UINT code, int id, HWND hWnd)
{
	OnBtnSetting(0, 0, NULL);
}


void CUrlSecurityPropertyPage::OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
		OnDelCmd(0, 0, NULL);
}



void CUrlSecurityPropertyPage::_GetData()
{
	// update ignored urls list
	int 	nCount = m_listbox.GetCount();
	CString strbuf;

	CUrlSecurityOption::COptUrlList 	urlSecurity;
	for (int i = 0; i < nCount; i++) {
		m_listbox.GetText(i, strbuf);
		CUrlSecurityOption::COptUrlList::iterator it = std::find(CUrlSecurityOption::s_urlSecurity.begin(), CUrlSecurityOption::s_urlSecurity.end(), CUrlSecurityOption::COptUrl(0,0,0,strbuf));
		if (it != CUrlSecurityOption::s_urlSecurity.end())
			urlSecurity.push_back( *it );
		else
			urlSecurity.push_back( CUrlSecurityOption::COptUrl(0,0,0,strbuf) );
	}
	CUrlSecurityOption::s_urlSecurity.swap( urlSecurity );

	CUrlSecurityOption::s_bValid		 = m_nValid  != 0;
  #if 1	//+++ +mod.1.35にて、必ず0書き込みされてしまってるので、しばらく強制的に1にする...
	CUrlSecurityOption::s_bActivePageToo = 1;
  #else
	CUrlSecurityOption::s_bActivePageToo = m_nActPag != 0;
  #endif
}



void CUrlSecurityPropertyPage::OnBtnUp(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 	nIndex = 0;
	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;
	if (nIndex == 0 || m_listbox.GetCount() < 2)
		return;
	CString 	strKeep;
	m_listbox.GetText(nIndex - 1, strKeep);
	m_listbox.DeleteString(nIndex - 1);
	m_listbox.InsertString(nIndex, strKeep);

	CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::get_optUrlList_iterator(nIndex - 1);
	CUrlSecurityOption::COptUrl					tmp ( *it );
	it = CUrlSecurityOption::s_urlSecurity.erase(it);
	//if (it == CUrlSecurityOption::s_urlSecurity.end())
	//	return;
	++it;
	CUrlSecurityOption::s_urlSecurity.insert(it, tmp);

	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());
}



void CUrlSecurityPropertyPage::OnBtnDown(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 	nIndex = 0;
	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;
	int l = m_listbox.GetCount();
	if (l < 2 || nIndex >= l-1)
		return;
	CString 	strKeep;
	m_listbox.GetText(nIndex + 1, strKeep);
	m_listbox.DeleteString(nIndex + 1);
	m_listbox.InsertString(nIndex, strKeep);

	CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::get_optUrlList_iterator(nIndex);
	CUrlSecurityOption::COptUrl					tmp ( *it );
	it = CUrlSecurityOption::s_urlSecurity.erase(it);
	if (it == CUrlSecurityOption::s_urlSecurity.end())
		return;
	++it;
	CUrlSecurityOption::s_urlSecurity.insert(it, tmp);

	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());
}



void CUrlSecurityPropertyPage::OnBtnSetting(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 	nIndex = 0;
	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;
	int l = m_listbox.GetCount();
	if (nIndex < 0 || nIndex >= l)
		return;
	CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::get_optUrlList_iterator(nIndex);

	CString		str		= it->m_url;
	unsigned	flags	= it->m_flags;
	unsigned	opts	= it->m_opts;
	unsigned	opts2	= it->m_opts2 & ~1;		//+++
	CExPropertyDialog2	dlg(str, flags, opts, opts2);
	dlg.DoModal();
	if (it->m_url != str || flags != it->m_flags || opts != it->m_opts || opts2 != it->m_opts2) {
		if (str.IsEmpty()) {
			OnDelCmd(0,0,NULL);
		} else {
			it->m_url   = str;
			it->m_flags = flags;
			it->m_opts  = opts;
			it->m_opts2 = opts2;
			m_listbox.DeleteString(nIndex);
			m_listbox.InsertString(nIndex, str);
		}
	}

	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());

	DispExOpts();
}



void CUrlSecurityPropertyPage::DebugPrint()
{
	ATLASSERT(CUrlSecurityOption::s_urlSecurity.size() == m_listbox.GetCount());
	int 	n = 0;
	for (CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::s_urlSecurity.begin();
		it != CUrlSecurityOption::s_urlSecurity.end();
		++it)
	{
		CString		name;
		m_listbox.GetText(n, name);
		ErrorLogPrintf(_T("%3d %3d,%#08x %s\t%s\n"), n, it->m_flags, it->m_opts, LPCTSTR(it->m_url), LPCTSTR(name));
		++n;
	}
}



void CUrlSecurityPropertyPage::DispExOpts()
{
	int 	nIndex = 0;
	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;
	int		exopts 	= 0xAAAAAA;
	int		exopts2	= 0x8;
	int		flags	= 0;
	int		l 		= m_listbox.GetCount();
	if (0 <= nIndex && nIndex < l) {
		CUrlSecurityOption::COptUrlList::iterator 	it = CUrlSecurityOption::get_optUrlList_iterator(nIndex);
		exopts  = it->m_opts;
		exopts2 = it->m_opts2 & ~1;
		flags   = it->m_flags;
	}
	DispExOptsFlag(exopts, exopts2, flags);
	InvalidateRect(NULL, TRUE);
}



void CUrlSecurityPropertyPage::DispExOptsFlag(unsigned exopts, unsigned exopts2, unsigned flags)
{
	CButton(GetDlgItem(IDC_CHK_URLSEC_REGEX    )).SetCheck( (flags & 1 ) != 0 );

	CExProperty		prop(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, exopts2);
	//CExProperty	prop(0, 0, 0, exopts, 0);
	DWORD	dlc = prop.GetDLControlFlags();
	CButton(GetDlgItem(IDC_CHECK_VIDEO         )).SetCheck( (dlc & DLCTL_VIDEOS  ) != 0 );
	CButton(GetDlgItem(IDC_CHECK_SOUND         )).SetCheck( (dlc & DLCTL_BGSOUNDS) != 0 );
	CButton(GetDlgItem(IDC_CHECK_IMAGE         )).SetCheck( (dlc & DLCTL_DLIMAGES) != 0 );
	CButton(GetDlgItem(IDC_CHECK_RUNACTIVEXCTLS)).SetCheck( (dlc & DLCTL_NO_RUNACTIVEXCTLS) == 0 );
	CButton(GetDlgItem(IDC_CHECK_DLACTIVEXCTLS )).SetCheck( (dlc & DLCTL_NO_DLACTIVEXCTLS) == 0 );
	CButton(GetDlgItem(IDC_CHECK_SCRIPTS       )).SetCheck( (dlc & DLCTL_NO_SCRIPTS) == 0 );
	CButton(GetDlgItem(IDC_CHECK_JAVA          )).SetCheck( (dlc & DLCTL_NO_JAVA) == 0 );

	DWORD	styl = prop.GetExtendedStyleFlags();
	CButton(GetDlgItem(IDC_CHECK_NAVILOCK      )).SetCheck( (styl & DVS_EX_OPENNEWWIN) != 0 );
	CButton(GetDlgItem(IDC_CHECK_SCROLLBAR     )).SetCheck( (styl & DVS_EX_FLATVIEW) != 0 );
	CButton(GetDlgItem(IDC_CHK_MSG_FILTER      )).SetCheck( (styl & DVS_EX_MESSAGE_FILTER) != 0 );
	CButton(GetDlgItem(IDC_CHK_MOUSE_GESTURE   )).SetCheck( (styl & DVS_EX_MOUSE_GESTURE) != 0 );
	CButton(GetDlgItem(IDC_CHK_BLOCK_MAILTO    )).SetCheck( (styl & DVS_EX_BLOCK_MAILTO) != 0 );

	static const TCHAR*	autoRefresh[7] = {
		_T("なし"),
		_T("15秒"),
		_T("30秒"),
		_T("1分"),
		_T("2分"),
		_T("5分"),
		_T("ユーザー設定時間"),
	};
	int i = prop.GetAutoRefreshNo();
	if (i < 0 || i >= 7)
		i = 0;
	CStatic(GetDlgItem(IDC_STATIC_AUTOUPDATE   )).SetWindowText( autoRefresh[ i ] );
}
