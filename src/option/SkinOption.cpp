/**
 *	@file	SkinOpiton.cpp
 *	@brief	donutのオプション : スキン
 */

#include "stdafx.h"
#include "SkinOption.h"
#include "../DonutPFunc.h"
#include "../DonutDefine.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




////////////////////////////////////////////////////////////////////////////////
//CSkinPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

int 	CSkinOption::s_nComboStyle		= SKN_COMBO_STYLE_DEFAULT;
int 	CSkinOption::s_nTabStyle		= SKN_TAB_STYLE_DEFAULT;
int 	CSkinOption::s_nRebarBGStyle	= SKN_REBAR_STYLE_TILE;
int 	CSkinOption::s_nRebarNoBoader	= SKN_CONST_NO_INT;
int 	CSkinOption::s_nStatusStyle 	= SKN_STATUS_STYLE_TILE;
int 	CSkinOption::s_nStatusTextColor = -1;						//+++
int 	CSkinOption::s_nStatusBackColor = -1;						//+++
int 	CSkinOption::s_nMainFrameBgColor= -1;						//+++
int 	CSkinOption::s_nMainFrameCaption= SKN_CONST_YES_INT;		//+++
int 	CSkinOption::s_nMenuStyle		= SKN_MENU_STYLE_TINY;		//+++

WTL::CLogFont CSkinOption::s_lfTabBar;
WTL::CLogFont CSkinOption::s_lfAddressBar;
WTL::CLogFont CSkinOption::s_lfSearchBar;
WTL::CLogFont CSkinOption::s_lfLinkBar;
WTL::CLogFont CSkinOption::s_lfProxyComboBox;



void CSkinOption::_GetSkinFont(CIniFileI &pr,const CString& strPrefix, WTL::CLogFont& lf, const WTL::CLogFont& lfDef)
{
	::lstrcpy(lf.lfFaceName, pr.GetString(strPrefix + _T(".lfFaceName"), lfDef.lfFaceName));
	int nHeight = pr.GetValue(strPrefix + _T(".lfPointSize"));
	if (nHeight != 0) {
		HDC hDC = ::GetDC(NULL);
		ATLASSERT(hDC);
		lf.SetHeight(nHeight, hDC);
		::ReleaseDC(NULL, hDC);
	} else {
		lf.lfHeight = lfDef.lfHeight;
	}
	lf.lfWeight		= pr.GetValue(strPrefix + _T(".lfWeight"), lfDef.lfWeight);
	lf.lfItalic		= (BYTE)pr.GetValue(strPrefix + _T(".lfItalic")		, lfDef.lfItalic);
	lf.lfUnderline	= (BYTE)pr.GetValue(strPrefix + _T(".lfUnderLine")	, lfDef.lfUnderline);
	lf.lfStrikeOut	= (BYTE)pr.GetValue(strPrefix + _T(".lfStrikeOut")	, lfDef.lfStrikeOut);
	lf.lfCharSet	= (BYTE)pr.GetValue(strPrefix + _T(".lfCharSet")	, lfDef.lfCharSet);
}

int CSkinOption::GetIntegerFromStringConst(const CString &strConst, int dflt)
{
	if		(strConst == SKN_CONST_DEFAULT) return SKN_CONST_DEFAULT_INT;
	else if (strConst == SKN_CONST_CLASSIC) return SKN_CONST_CLASSIC_INT;
	else if (strConst == SKN_CONST_THEME  ) return SKN_CONST_THEME_INT;
	else if (strConst == SKN_CONST_TILE   ) return SKN_CONST_TILE_INT;
	else if (strConst == SKN_CONST_STRETCH) return SKN_CONST_STRETCH_INT;
	else if (strConst == SKN_CONST_YES	  ) return SKN_CONST_YES_INT;
	else if (strConst == SKN_CONST_NO	  ) return SKN_CONST_NO_INT;
	else if (strConst == SKN_CONST_TINY   ) return SKN_CONST_TINY_INT;		//+++
	else if (strConst == SKN_CONST_ENGLISH) return SKN_CONST_ENGLISH_INT;	//+++
	else if (strConst.IsEmpty() == 0 && isxdigit(strConst[0])) return GetRGBval(strConst); //+++
	else									return dflt/*0*/;
}



#if 1	//+++
int CSkinOption::GetRGBval(const CString& str)
{
	LPCTSTR s	= str;
	DWORD	col = _tcstol(s, NULL, 16);
	//+++ winの描画側(brush)はbgr順のよう？なので、r,bを入れ替える...
	col 		= ((col >> 16) & 0xFF) | (col & 0x00ff00) | ((col & 0xff) << 16) ;
	return col;
}
#endif



__inline int CSkinOption::QueryValueCustom(CIniFileI &pr, LPCTSTR lpKey, int dflt)
{
	return GetIntegerFromStringConst( pr.GetString( lpKey ), dflt );
}



void CSkinOption::GetProfile()
{
	CString 	strSkinPath = _GetSkinDir() + "skin.ini";

	CIniFileI	pr( strSkinPath, _T("Interface") );
	s_nTabStyle 		= QueryValueCustom( pr, _T("TabStyle")	);
	s_nComboStyle		= QueryValueCustom( pr, _T("ComboStyle")	);
	s_nRebarBGStyle 	= QueryValueCustom( pr, _T("RebarBGStyle") );
	s_nRebarNoBoader	= QueryValueCustom( pr, _T("RebarNoBoader"));
	s_nStatusStyle		= QueryValueCustom( pr, _T("StatusStyle")	);
	s_nStatusTextColor	= QueryValueCustom( pr, _T("StatusTextColor" ), -1 );	//+++
	s_nStatusBackColor	= QueryValueCustom( pr, _T("StatusBackColor" ), -1 );	//+++
	s_nMainFrameBgColor = QueryValueCustom( pr, _T("MainFrameBgColor"), -1 );	//+++
	s_nMainFrameCaption = QueryValueCustom( pr, _T("MainFrameCaption"),  1 );	//+++
	s_nMenuStyle		= QueryValueCustom( pr, _T("MenuStyle") );				//+++
	pr.Close();

	// フォント
	{
		WTL::CLogFont	lfDefault;
		lfDefault.SetMenuFont();

		CIniFileI	pr(g_szIniFileName, _T("Main"));
		_GetSkinFont(pr, _T("lf"), lfDefault, lfDefault);
#if 0
		::lstrcpy(lfDefault.lfFaceName, pr.GetString(_T("lf.lfFaceName"), lfDefault.lfFaceName));
		int nHeight = pr.GetValue(_T("lf.lfPointSize"));
		if (nHeight != 0) {
			HDC hDC = ::GetDC(NULL);
			ATLASSERT(hDC);
			lfDefault.SetHeight(nHeight, hDC);
			::ReleaseDC(NULL, hDC);
		}
		lfDefault.lfWeight		= pr.GetValue(_T("lf.lfWeight")			, lfDefault.lfWeight);
		lfDefault.lfItalic		= (BYTE)pr.GetValue(_T("lf.lfItalic")	, lfDefault.lfItalic);
		lfDefault.lfUnderline	= (BYTE)pr.GetValue(_T("lf.lfUnderLine"), lfDefault.lfUnderline);
		lfDefault.lfStrikeOut	= (BYTE)pr.GetValue(_T("lf.lfStrikeOut"), lfDefault.lfStrikeOut);
		lfDefault.lfCharSet		= (BYTE)pr.GetValue(_T("lf.lfCharSet")	, lfDefault.lfCharSet);
#endif
		pr.ChangeSectionName(_T("Skin"));
		_GetSkinFont(pr, _T("TabBar")		, s_lfTabBar		, lfDefault);
		_GetSkinFont(pr, _T("AddressBar")	, s_lfAddressBar	, lfDefault);
		_GetSkinFont(pr, _T("SearchBar")	, s_lfSearchBar		, lfDefault);
		_GetSkinFont(pr, _T("LinkBar")		, s_lfLinkBar		, lfDefault);
		_GetSkinFont(pr, _T("ProxyComboBox"), s_lfProxyComboBox	, lfDefault);
	}
}



////////////////////////////////////////////////////////////////////////////////
//CSkinPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

CSkinPropertyPage::CSkinPropertyPage(HWND hTopWindow, BOOL *pbSkinChange)
	: m_bInit(FALSE)
	, m_hTopWindow(hTopWindow)
	, m_pbChanged(pbSkinChange)
{
}



void CSkinPropertyPage::_SetData()
{
	/* フォント */
	m_lfTabBar			= CSkinOption::s_lfTabBar;	
	m_lfAddressBar		= CSkinOption::s_lfAddressBar;
	m_lfSearchBar		= CSkinOption::s_lfSearchBar;	
	m_lfLinkBar			= CSkinOption::s_lfLinkBar;	
	m_lfProxyComboBox	= CSkinOption::s_lfProxyComboBox;

	m_sTabBar		= GetDlgItem(IDC_TABBAR_STATIC);
	m_sAddressBar	= GetDlgItem(IDC_ADDRESSBAR_STATIC);
	m_sSearchBar	= GetDlgItem(IDC_SEARCHBAR_STATIC);
	m_sLinkBar		= GetDlgItem(IDC_LINKBAR_STATIC);
	m_sProxyComboBox= GetDlgItem(IDC_PROXYCMB_STATIC);

	_InitStaticText();

	/* スキン */
	CString   strPath	  = _GetSkinDir();

	strPath.TrimRight( _T("\\") );
	strPath = strPath.Mid(strPath.ReverseFind('\\') + 1);

	CStatic   stName	  = GetDlgItem(IDC_STATIC_NAME);
	//+++ CStatic  stDesc = GetDlgItem(IDC_STATIC_DESC);		//+++ 未使用
	CListBox  List		  = GetDlgItem(IDC_LIST_SKIN);

	stName.SetWindowText(strPath);

	//フォルダを列挙
	CString 	strSkinPath = Misc::GetExeDirectory() + _T("Skin\\*");
	CFindFile	find;

	int nIndex = 0;
	if ( find.FindFile(strSkinPath) ) {
		do {
			if ( find.IsDirectory() ) {
				CString strTitle = find.GetFileTitle();

				if ( strTitle != _T("") && strTitle != _T(".") ) {
					int 	nRet		= List.AddString( find.GetFileTitle() );
					LPTSTR	lpstr		= _AllocString( find.GetFilePath() ); //newで確保 必ず_FreeStringを呼ぶこと
					List.SetItemData(nRet, (DWORD_PTR) lpstr);
					if (strPath == strTitle) {
						nIndex = nRet;
					}
				}
			}
		} while ( find.FindNextFile() );

		find.Close();
	}

	List.SetCurSel(nIndex);

	BOOL	  bHandled	  = FALSE;
	OnSelChanged(0, 0, NULL, bHandled);
}


// スキンの説明文を読み込む
LRESULT CSkinPropertyPage::OnSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	CStatic 	label		= GetDlgItem(IDC_STATIC_DESC);		//+++ メモ：スキンの説明.
	CListBox	List		= GetDlgItem(IDC_LIST_SKIN);
	int 		nIndex		= List.GetCurSel();

	if (nIndex == LB_ERR)
		return 0;

	CString 	strSkinPath = (LPTSTR) List.GetItemData(nIndex);
	strSkinPath += _T("\\Skin.ini");
	CIniFileI	pr( strSkinPath, _T("Interface") );
	CString 	strBuf = pr.GetString( _T("Description") /*, NULL, 1024 */ );
	pr.Close();
	strBuf.Replace( _T("\\n"), _T("\r\n") );

	label.SetWindowText(strBuf);

	return 0;
}



LRESULT CSkinPropertyPage::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CListBox List	= GetDlgItem(IDC_LIST_SKIN);
	int 	 nCount = List.GetCount();

	for (int i = 0; i < nCount; i++) {
		LPTSTR lpstr = (LPTSTR) List.GetItemData(i);
		_FreeString(lpstr);
		List.SetItemData(i, 0); //+++ delete後はクリア.
	}

	return 1;
}



LRESULT CSkinPropertyPage::OnSkinApply(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CIniFileO	pr( g_szIniFileName, _T("Skin") );

	_SaveFont(pr, _T("TabBar"),			m_lfTabBar);
	_SaveFont(pr, _T("AddressBar"),		m_lfAddressBar);
	_SaveFont(pr, _T("SearchBar"),		m_lfSearchBar);
	_SaveFont(pr, _T("LinkBar"),		m_lfLinkBar);
	_SaveFont(pr, _T("ProxyComboBox"),	m_lfProxyComboBox);

	CSkinOption::s_lfTabBar			= m_lfTabBar;	
	CSkinOption::s_lfAddressBar		= m_lfAddressBar;
	CSkinOption::s_lfSearchBar		= m_lfSearchBar;	
	CSkinOption::s_lfLinkBar		= m_lfLinkBar;	
	CSkinOption::s_lfProxyComboBox	= m_lfProxyComboBox;


	CListBox List		 = GetDlgItem(IDC_LIST_SKIN);
	int 	 index		 = List.GetCurSel();

	if (index == LB_ERR)
		return 0;

	CString  strPath	 = (LPTSTR) List.GetItemData(index);

	CString  strSkinPath = Misc::GetExeDirectory() + _T("Skin\\");

	if (strPath.Find(strSkinPath) == 0) {
		CString strName = strPath.Mid( strSkinPath.GetLength() );

		if ( !strName.IsEmpty() ) {
			CIniFileO	pr( g_szIniFileName, _T("Skin") );
			pr.SetStringUW( strName, _T("SkinFolder") );
			pr.Close();
			::SendMessage(m_hTopWindow, WM_CHANGE_SKIN, 0, 0);

			if (m_pbChanged)
				*m_pbChanged = TRUE;

			CStatic   stName = GetDlgItem(IDC_STATIC_NAME);
			stName.SetWindowText(strName);
		}
	}

	return 0;
}

void	CSkinPropertyPage::OnFontSetting(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	WTL::CLogFont*	plf = NULL;
	switch (nID) {
	case IDC_TABBAR_FONT:
		plf = &m_lfTabBar;
		break;
	case IDC_ADDRESSBAR_FONT:
		plf = &m_lfAddressBar;
		break;
	case IDC_SEARCHBAR_FONT:
		plf = &m_lfSearchBar;
		break;
	case IDC_LINKBAR_FONT:
		plf = &m_lfLinkBar;
		break;
	case IDC_PROXYCMB_FONT:
		plf = &m_lfProxyComboBox;
		break;
	default:
		ATLASSERT(FALSE);
		return ;
	}
	CFontDialog dlg(plf);

	if (dlg.DoModal() == IDOK) {
		*plf = dlg.m_lf;
		_InitStaticText();
	}
}



BOOL CSkinPropertyPage::OnSetActive()
{
	if (!m_bInit) {
		m_bInit = TRUE;
		_SetData();
	}

	SetModified(TRUE);
	return TRUE;
}



LPTSTR CSkinPropertyPage::_AllocString(const CString& str)
{
	DWORD	l	= str.GetLength() + 1;
	TCHAR*	buf = new TCHAR[l];
	if (buf) {
		::lstrcpyn(buf, LPCTSTR(str), l);
	}
	return buf;
}



void CSkinPropertyPage::_FreeString(LPTSTR lpstr)
{
	delete[] lpstr;
}

void	CSkinPropertyPage::_SaveFont(CIniFileO& pr,const CString& strPrefix, const WTL::CLogFont& rlf)
{
	pr.SetString(rlf.lfFaceName,	strPrefix + _T(".lfFaceName"));

	HDC hDC = ::GetDC(NULL);
	ATLASSERT(hDC);
	pr.SetValue(rlf.GetHeight(hDC), strPrefix + _T(".lfPointSize"));
	::ReleaseDC(NULL, hDC);

	pr.SetValue(rlf.lfWeight,		strPrefix + _T(".lfWeight"));
	pr.SetValue(rlf.lfItalic,		strPrefix + _T(".lfItalic"));
	pr.SetValue(rlf.lfUnderline,	strPrefix + _T(".lfUnderline"));
	pr.SetValue(rlf.lfStrikeOut,	strPrefix + _T(".lfStrikeOut"));
	pr.SetValue(rlf.lfCharSet,		strPrefix + _T(".lfCharSet"));
}


void	CSkinPropertyPage::_InitStaticText()
{
	m_sTabBar.SetFont		(m_lfTabBar.CreateFontIndirect());	
	m_sAddressBar.SetFont	(m_lfAddressBar.CreateFontIndirect());
	m_sSearchBar.SetFont	(m_lfSearchBar.CreateFontIndirect());
	m_sLinkBar.SetFont		(m_lfLinkBar.CreateFontIndirect());
	m_sProxyComboBox.SetFont(m_lfProxyComboBox.CreateFontIndirect());

	m_sTabBar.SetWindowText			(m_lfTabBar.lfFaceName);
	m_sAddressBar.SetWindowText		(m_lfAddressBar.lfFaceName);
	m_sSearchBar.SetWindowText		(m_lfSearchBar.lfFaceName);
	m_sLinkBar.SetWindowText		(m_lfLinkBar.lfFaceName);
	m_sProxyComboBox.SetWindowText	(m_lfProxyComboBox.lfFaceName);
}





