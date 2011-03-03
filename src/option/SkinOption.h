/**
 *	@file	SkinOpiton.h
 *	@brief	donutのオプション : スキン
 */

#pragma once

#include "../resource.h"
#include "../IniFile.h"

enum ESkn_Combo_Style {
	SKN_COMBO_STYLE_DEFAULT 	= 0,
	SKN_COMBO_STYLE_CLASSIC 	= 1,
	SKN_COMBO_STYLE_THEME		= 2,
};

enum ESkn_Tab_Style {
	SKN_TAB_STYLE_DEFAULT		= 0,
	SKN_TAB_STYLE_CLASSIC		= 1,
	SKN_TAB_STYLE_THEME 		= 2,
};

enum ESkn_Rebar_Style {
	SKN_REBAR_STYLE_TILE		= 0,
	SKN_REBAR_STYLE_STRETCH 	= 1,
	SKN_REBAR_STYLE_THEME		= 2,
};

enum ESkn_Status_Style {
	SKN_STATUS_STYLE_TILE		= 0,
	SKN_STATUS_STYLE_STRETCH	= 1,
	SKN_STATUS_STYLE_THEME		= 2,
};

enum ESkin_Menu_Style {
	SKN_MENU_STYLE_DEFAULT		= 0,
	SKN_MENU_STYLE_TINY 		= 1,
};

#define SKN_CONST_DEFAULT		 _T("DEFAULT")
enum {	SKN_CONST_DEFAULT_INT	= 0 };

#define SKN_CONST_CLASSIC		 _T("CLASSIC")
enum {	SKN_CONST_CLASSIC_INT	= 1 };

#define SKN_CONST_THEME 		 _T("THEME")
enum {	SKN_CONST_THEME_INT 	= 2 };

#define SKN_CONST_TILE			 _T("TILE")
enum {	SKN_CONST_TILE_INT		= 0 };

#define SKN_CONST_STRETCH		 _T("STRETCH")
enum {	SKN_CONST_STRETCH_INT	= 1 };

#define SKN_CONST_NO			 _T("NO")
enum {	SKN_CONST_NO_INT		= 0 };

#define SKN_CONST_YES			 _T("YES")
enum {	SKN_CONST_YES_INT		= 1 };

#define SKN_CONST_TINY			 _T("TINY")
enum {	SKN_CONST_TINY_INT		= 1 };

#define SKN_CONST_ENGLISH		 _T("ENGLISH")
enum {	SKN_CONST_ENGLISH_INT	= 2 };

//////////////////////////////////////////////////////
// CSkinOption

class CSkinOption {
	//x friend	class CSkinPropertyPage;
public:
	static int	s_nComboStyle;
	static int	s_nTabStyle;
	static int	s_nRebarBGStyle;
	static int	s_nRebarNoBoader;
	static int	s_nStatusStyle;
	static int	s_nStatusTextColor; 	//+++ スキンのリロード側でやってたのをここで取得するように変更.
	static int	s_nStatusBackColor; 	//+++ スキンのリロード側でやってたのをここで取得するように変更.
	static int	s_nMenuStyle;			//+++ default|tiny	メニューの一文字化の指定
	static int	s_nMainFrameCaption;	//+++ yes/no メインフレームのキャプションの有無
	static int	s_nMainFrameBgColor;	//+++ 窓がないときのウィンドウの色. (bg.bmpがない時のみ)

	static WTL::CLogFont s_lfTabBar;
	static WTL::CLogFont s_lfAddressBar;
	static WTL::CLogFont s_lfSearchBar;
	static WTL::CLogFont s_lfLinkBar;
	static WTL::CLogFont s_lfProxyComboBox;

private:
	static void _GetSkinFont(CIniFileI &pr, const CString &strPrefix, WTL::CLogFont& lf, const WTL::CLogFont& lfDef);

public:
	static int	GetIntegerFromStringConst(const CString &strConst, int dflt = 0);	//+++ 引数追加
	static void GetProfile();
	static int	QueryValueCustom(CIniFileI &pr, LPCTSTR lpKey, int dflt = 0);

private:
	static int	GetRGBval(const CString& str);
};


///////////////////////////////////////////////////////////////////////////////
// CSkinPropertyPage

class CSkinPropertyPage : public CPropertyPageImpl<CSkinPropertyPage> {
public:
	enum { IDD = IDD_PROPPAGE_SKIN };

private:
	BOOL		m_bInit;
	HWND		m_hTopWindow;
	BOOL *		m_pbChanged;

	WTL::CLogFont	m_lfTabBar;
	WTL::CLogFont	m_lfAddressBar;
	WTL::CLogFont	m_lfSearchBar;
	WTL::CLogFont	m_lfLinkBar;
	WTL::CLogFont	m_lfProxyComboBox;

	CStatic			m_sTabBar;
	CStatic			m_sAddressBar;
	CStatic			m_sSearchBar;
	CStatic			m_sLinkBar;
	CStatic			m_sProxyComboBox;

public:
	CSkinPropertyPage(HWND hTopWindow, BOOL *pbSkinChange);

	BOOL	OnSetActive();

	BEGIN_MSG_MAP( CSkinPropertyPage )
		MESSAGE_HANDLER   ( WM_DESTROY		 , OnDestroy	)
		COMMAND_ID_HANDLER( IDC_BUTTON_APPLY , OnSkinApply	)
		CHAIN_MSG_MAP	  ( CPropertyPageImpl<CSkinPropertyPage> )
		COMMAND_HANDLER   ( IDC_LIST_SKIN	 , LBN_SELCHANGE , OnSelChanged )
		COMMAND_ID_HANDLER_EX(IDC_TABBAR_FONT		, OnFontSetting )
		COMMAND_ID_HANDLER_EX(IDC_ADDRESSBAR_FONT	, OnFontSetting )
		COMMAND_ID_HANDLER_EX(IDC_SEARCHBAR_FONT	, OnFontSetting )
		COMMAND_ID_HANDLER_EX(IDC_LINKBAR_FONT		, OnFontSetting )
		COMMAND_ID_HANDLER_EX(IDC_PROXYCMB_FONT		, OnFontSetting )
	END_MSG_MAP()

	LRESULT OnSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnSkinApply(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void	OnFontSetting(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	void	_SetData();
	LPTSTR	_AllocString(const CString& str);
	void	_FreeString(LPTSTR lpstr);

	void	_SaveFont(CIniFileO& pr,const CString& strPrefix, const WTL::CLogFont& rlf);
	void	_InitStaticText();

};
