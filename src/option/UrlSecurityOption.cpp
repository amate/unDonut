/**
 *	@file	UrlSecurityOption.cpp
 *	@brief	donutのオプション : URL別セキュリティ.
 *	@note
 *		undonut+modで追加.
 */

#include "stdafx.h"

#include "UrlSecurityOption.h"
#include <sstream>
#include <boost\archive\text_woarchive.hpp>
#include <boost\archive\text_wiarchive.hpp>
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


#define DONUTURLSECURITYSHAREDMEMNAME	_T("DonutUrlSecuritySharedMemName")


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


std::list<UrlSecurityData>	CUrlSecurityOption::s_UrlSecurityList;
bool 						CUrlSecurityOption::s_bValid			= true;



void CUrlSecurityOption::GetProfile()
{
	CIniFileI	pr( g_szIniFileName, _T("UrlSecurity") );
	int			nValid	= int( pr.GetValue( _T("IsValid"), -1 ) );
	if (nValid >= 0)
		s_bValid = nValid != 0;

	pr.Close();

	//+++ ファイルより読み込む
	std::list<CString>	urls;
	::FileReadString(_GetFilePath( _T("UrlEntry.ini") ), urls);

	//+++ ファイルから読み込んだデータを分解する
	s_UrlSecurityList.clear();
	for (auto it = urls.begin(); it != urls.end(); ++it) {
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
#if _MSC_VER >= 1700
		s_UrlSecurityList.emplace_back(f, o, o2, (LPCTSTR)url);
#else
		s_UrlSecurityList.push_back(UrlSecurityData(f, o, o2, (LPCTSTR)url));
#endif
	}
}



void CUrlSecurityOption::WriteProfile()
{
	CIniFileO	 pr( g_szIniFileName, _T("UrlSecurity") );
	pr.SetValue( s_bValid, _T("IsValid") );

	//+++ ファイルから読み込んだデータを分解する
	std::list<CString>		urls;
	for (auto it = s_UrlSecurityList.begin(); it != s_UrlSecurityList.end(); ++it)	{
	  #if 1	//+++
		urls.push_back( Misc::StrFmt(_T("%u,%u,%u,%s"), it->flags, it->opts, it->opts2, it->urlPatern.c_str()) );
	  #else
		unsigned fl = (it->m_flags & 1) | (it->opts2 & ~3);
		urls.push_back( Misc::StrFmt(_T("%u,%u,%s"), fl, it->m_opts, LPCTSTR(it->m_url)) );
	  #endif
	}

	FileWriteString( _GetFilePath( _T("UrlEntry.ini") ), urls );
}


void CUrlSecurityOption::UpdateOriginalUrlSecurityList(HWND hWndMainFrame)
{
	CString sharedMemName;
	sharedMemName.Format(_T("%s0x%x"), DONUTURLSECURITYSHAREDMEMNAME, hWndMainFrame);
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_READ, FALSE, sharedMemName);
	if (hMap) {
		::CloseHandle(hMap);
	}

	std::wstringstream ss;
	boost::archive::text_woarchive ar(ss);
	ar << s_UrlSecurityList;
	std::wstring serializedData = ss.str();

	hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(serializedData.size() + sizeof(WCHAR)) * sizeof(WCHAR), sharedMemName);
	ATLASSERT( hMap );
	LPTSTR sharedMemData = (LPTSTR)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	::wcscpy_s(sharedMemData, serializedData.size() + sizeof(WCHAR), serializedData.c_str());
	::UnmapViewOfFile((LPVOID)sharedMemData);

	::SendMessage(hWndMainFrame, WM_UPDATEURLSECURITYLIST, 0, 0);
}

void CUrlSecurityOption::CloseOriginalUrlSecurityList(HWND hWndMainFrame)
{
	CString sharedMemName;
	sharedMemName.Format(_T("%s0x%x"), DONUTURLSECURITYSHAREDMEMNAME, hWndMainFrame);
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_READ, FALSE, sharedMemName);
	if (hMap) {
		::CloseHandle(hMap);
	}
}


#if 0
bool CUrlSecurityOption::SearchString(const CString &strURL)
{
	if (!s_bValid)
		return false;

	return MtlSearchStringWildCard(s_urlSecurity.begin(), s_urlSecurity.end(), strURL);
}
#endif


int	CUrlSecurityOption::MatchTest(const CString& strURL)
{
	int nIndex = 0;
	auto it = s_UrlSecurityList.begin();
	for (; it != s_UrlSecurityList.end(); ++it) {
		//CString ptn = it->m_url;
		const std::wstring& patern = it->urlPatern;
		if (it->flags & USP_USEREGEX) {	//+++ 正規表現を有効にする場合.
			if (patern[0] != L'*') {
				try {	//正規表現が不正だとエラーが投げられるので、無視するようにする
					basic_regex<TCHAR> 	re(patern.c_str());
					if ( regex_search(LPCTSTR(strURL), re) ) {
						return nIndex;
						break;
	 				}
				} catch (...) {
					;
				}
			}
		} else {
			if (patern.find(L'*') == std::wstring::npos && patern.find(L'?') == std::wstring::npos) {
				//ptn += _T('*');
				if (patern == (LPCTSTR)strURL) {
					return nIndex;
					break;
				}
			} else {
				if ( MtlStrMatchWildCard(patern.c_str(), strURL) ) {
					return nIndex;
					break;
				}
			}
		}
		++nIndex;
	}
	return -1;
}

void CUrlSecurityOption::Add(unsigned flags, unsigned opts, unsigned opts2, const CString &strURL)
{
	auto it = std::find_if(s_UrlSecurityList.begin(), s_UrlSecurityList.end(), [&](const UrlSecurityData& data) {
		return strURL == data.urlPatern.c_str();
	});
	if (it != s_UrlSecurityList.end()) {
		it->flags = flags;
		it->opts  = opts;
		it->opts2 = opts2 & ~1;
	} else {
#if _MSC_VER >= 1700
		s_UrlSecurityList.emplace_back( flags, opts, opts2, (LPCTSTR)strURL );
#else
		s_UrlSecurityList.push_back(UrlSecurityData(flags, opts, opts2, (LPCTSTR)strURL));
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////
// CUrlSecurityForChildFrame

void	CUrlSecurityForChildFrame::ReloadList()
{
	CString sharedMemName;
	sharedMemName.Format(_T("%s0x%x"), DONUTURLSECURITYSHAREDMEMNAME, m_hWndMainFrame);
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_READ, FALSE, sharedMemName);
	ATLASSERT( hMap );
	LPTSTR shardMemData = static_cast<LPTSTR>(::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0));
	std::wstringstream ss;
	ss << shardMemData;
	::UnmapViewOfFile(shardMemData);
	::CloseHandle(hMap);
	boost::archive::text_wiarchive	ar(ss);
	ar >> m_UrlSecurityList;
}


bool CUrlSecurityForChildFrame::FindUrl(const CString &strURL, DWORD* pExProp, DWORD* pExPropOpt, DWORD* pFlags)
{
	bool rc = false;

	auto it = m_UrlSecurityList.begin();
	for (; it != m_UrlSecurityList.end(); ++it) {
		//CString ptn = it->m_url;
		const std::wstring& patern = it->urlPatern;
		if (it->flags & USP_USEREGEX) {	//+++ 正規表現を有効にする場合.
			if (patern[0] != L'*') {
				try {	//正規表現が不正だとエラーが投げられるので、無視するようにする
					basic_regex<TCHAR> 	re(patern.c_str());
					if ( regex_search(LPCTSTR(strURL), re) ) {
						rc = true;
						break;
	 				}
				} catch (...) {
					;
				}
			}
		} else {
			if (patern.find(L'*') == std::wstring::npos && patern.find(L'?') == std::wstring::npos) {
				//ptn += _T('*');
				if (patern == (LPCTSTR)strURL) {
					rc = true;
					break;
				}
			} else {
				if ( MtlStrMatchWildCard(patern.c_str(), strURL) ) {
					rc = true;
					break;
				}
			}
		}
	}
	if (rc) {
		if (pFlags)
			*pFlags		= it->flags;
		if (pExProp)
			*pExProp	= it->opts;
		if (pExPropOpt)
			*pExPropOpt	= it->opts2 & ~1;
	}
	return rc;
}

// セキュリティをもとに戻すかどうか(このURLでのみ有効)
bool CUrlSecurityForChildFrame::IsUndoSecurity(const CString& strURL)
{
	for (auto it = m_UrlSecurityList.cbegin(); it != m_UrlSecurityList.cend(); ++it) {
		if (it->flags & USP_ONLYTHISURL) {
			const std::wstring& patern = it->urlPatern;
			if (it->flags & USP_USEREGEX) {	//+++ 正規表現を有効にする場合.
				if (patern[0] != _T('*')) {
					try {	//正規表現が不正だとエラーが投げられるので、無視するようにする
						basic_regex<TCHAR> 	re(patern.c_str());
						if ( regex_search(LPCTSTR(strURL), re) ) {
							return true;
	 					}
					} catch (...) {
						;
					}
				}
			} else {
				if (patern.find(L'*') == std::wstring::npos && patern.find(L'?') == std::wstring::npos) {
					//ptn += _T('*');
					if (strURL == patern.c_str()) {
						return true;
					}
				} else {
					if ( MtlStrMatchWildCard(patern.c_str(), strURL) ) {
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
CUrlSecurityPropertyPage::CUrlSecurityPropertyPage(const CString &strAddressBar, HWND hWndMainFrame)
	: m_strAddressBar(strAddressBar)
	, m_hWndMainFrame(hWndMainFrame)
	, m_wndList(this, 1)
{
	m_nValid  = CUrlSecurityOption::s_bValid;
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
		
		std::for_each(s_UrlSecurityList.begin(), s_UrlSecurityList.end(), [this](const UrlSecurityData& data) {
			m_listbox.AddString(data.urlPatern.c_str());
		});

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
			auto it2 = std::find_if(s_UrlSecurityList.begin(), s_UrlSecurityList.end(), [&](const UrlSecurityData& data) {
				return strURL == data.urlPatern.c_str();
			});
			if (it2 != s_UrlSecurityList.end())
				s_UrlSecurityList.erase(it2);

			m_listbox.DeleteString(*it);
		}
	}

	delete[] pIdx;

	DispExOpts();
}



void CUrlSecurityPropertyPage::OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	m_listbox.ResetContent();
	s_UrlSecurityList.clear();

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

	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());

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

	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());
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
	CUrlSecurityOption::s_bValid = m_nValid != 0;
	WriteProfile();
	UpdateOriginalUrlSecurityList(m_hWndMainFrame);
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
	auto it = std::next(s_UrlSecurityList.begin(), nIndex - 1);
	UrlSecurityData	tmp(*it);
	it = s_UrlSecurityList.erase(it);
	++it;
	s_UrlSecurityList.insert(it, tmp);

	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());
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

	auto it = std::next(s_UrlSecurityList.begin(), nIndex);
	UrlSecurityData tmp(*it);
	it = s_UrlSecurityList.erase(it);
	if (it == s_UrlSecurityList.end())
		return;
	++it;
	s_UrlSecurityList.insert(it, tmp);

	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());
}



void CUrlSecurityPropertyPage::OnBtnSetting(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 	nIndex = 0;
	if (m_listbox.GetSelItems(1, &nIndex) == 0)
		return;
	int l = m_listbox.GetCount();
	if (nIndex < 0 || nIndex >= l)
		return;
	auto it = std::next(s_UrlSecurityList.begin(), nIndex);
	
	CString str	= it->urlPatern.c_str();
	unsigned	flags	= it->flags;
	unsigned	opts	= it->opts;
	unsigned	opts2	= it->opts2 & ~1;		//+++
	CExPropertyDialog2	dlg(str, flags, opts, opts2);
	dlg.DoModal();
	if (it->urlPatern.c_str() != str || flags != it->flags || opts != it->opts || opts2 != it->opts2) {
		if (str.IsEmpty()) {
			OnDelCmd(0,0,NULL);
		} else {
			it->urlPatern   = str;
			it->flags = flags;
			it->opts  = opts;
			it->opts2 = opts2;
			m_listbox.DeleteString(nIndex);
			m_listbox.InsertString(nIndex, str);
		}
	}

	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());

	DispExOpts();
}

void	CUrlSecurityPropertyPage::OnMatchTest(UINT wNotifyCode, int wID, HWND hWndCtl)
{
	int nIndex = MatchTest(m_strAddressBar);
	if (nIndex != -1) {
		auto it = std::next(s_UrlSecurityList.begin(), nIndex);
		CString msg;
		msg.Format(_T("「%s」にマッチしました"), it->urlPatern.c_str());
		MessageBox(msg, m_strAddressBar);
	} else {
		MessageBox(_T("どれにもマッチしませんでした"), m_strAddressBar);
	}
}


void CUrlSecurityPropertyPage::DebugPrint()
{
	ATLASSERT(s_UrlSecurityList.size() == m_listbox.GetCount());
	int 	n = 0;
	for (auto it = s_UrlSecurityList.begin(); it != s_UrlSecurityList.end(); ++it) {
		CString		name;
		m_listbox.GetText(n, name);
		//ErrorLogPrintf(_T("%3d %3d,%#08x %s\t%s\n"), n, it->m_flags, it->m_opts, LPCTSTR(it->m_url), LPCTSTR(name));
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
		auto 	it = std::next(s_UrlSecurityList.begin(), nIndex);
		exopts  = it->opts;
		exopts2 = it->opts2 & ~1;
		flags   = it->flags;
	}
	DispExOptsFlag(exopts, exopts2, flags);
}


void CUrlSecurityPropertyPage::DispExOptsFlag(unsigned exopts, unsigned exopts2, unsigned flags)
{
	CButton(GetDlgItem(IDC_CHK_URLSEC_REGEX)).SetCheck( (flags & 1 ) != 0 );

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
