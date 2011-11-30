/**
*	@file	UserDefinedJavascriptOption.h
*	@brief	ユーザー定義Javascript
*/

#pragma once

#include <boost/optional.hpp>
#include <atlscrl.h>
#include <atlsync.h>
#include "../resource.h"
#include "../FileNotification.h"


struct UserDefinedJsData {
	CString strScriptFilePath;
	CString	strName;
	CString strDiscription;
	CString strVersion;
	CString strHP;
	vector<CString> vecIncludeUrls;
	vector<CString> vecExcludeUrls;
	CComBSTR	strScript;
};


class CUserDefinedJsOption
{
	friend bool _PrivateInit();
public:
	static boost::optional<vector<BSTR*> >	FindURL(const CString& strUrl);
	static bool	UserDefinedScriptInstall(const CString& strUrl, HWND hWndParent);

protected:
	static void	LoadUserJsConfig();
	static void SaveUserJsConfig();

	static vector<unique_ptr<UserDefinedJsData> >	s_vecpData;
	static CCriticalSection	s_criticalsection;

private:
	static unique_ptr<UserDefinedJsData> _CreateJsData(const CString& strFilePath);
};


class CUserJsListView : 
	public CScrollWindowImpl<CUserJsListView>,
	protected CUserDefinedJsOption
{
public:

	// Constants
	enum {
		kWindowWidth = 460,	// kDiscWidth用に適当なのを書いておく
		kItemHeight = 50,
		kLeftMargin = 5,
		kTopNameMargin = 5,
		kNameHeight = 22,
		kTopDiscMargin = kTopNameMargin + kNameHeight + 3,
		kDiscHeight = 15,
		kTopButton = kTopNameMargin + kNameHeight,
		kRightButtonMargin = 7,
		kButtonHeight = 20,
		kButtonWidth = 45,
		kButtonMargin = 7,
		kButtonCount = 3,
		kDiscWidth = kWindowWidth - (kLeftMargin + ((kButtonWidth + kButtonMargin) * kButtonCount) + kRightButtonMargin),
	};

	// Overrides
	void	DoPaint(CDCHandle dc);

	BEGIN_MSG_MAP( CUserJsListView )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_LBUTTONUP( OnLButtonUp )
        CHAIN_MSG_MAP( CScrollWindowImpl<CUserJsListView> )
        CHAIN_MSG_MAP_ALT( CScrollWindowImpl<CUserJsListView>, 1)
	END_MSG_MAP()

	int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnLButtonUp(UINT nFlags, CPoint point);


private:
	int		_HitTest(CPoint pt);
	CRect	_GetItemClientRect(int nIndex);

	// Data members
	CFont	m_font;
	CFont	m_namefont;
};


////////////////////////////////////////////////////////
/// [unDonutのオプション] - [ユーザー定義JS]

class CUserDefinedJsPropertyPage
	: public CPropertyPageImpl<CUserDefinedJsPropertyPage>
	, public CWinDataExchange<CUserDefinedJsPropertyPage>
	, protected CUserDefinedJsOption
{
public:
	enum { IDD = IDD_PROPPAGE_USERDEFINEDJAVASCRIPT };

	CUserDefinedJsPropertyPage(const CString& strUrl);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	BEGIN_DDX_MAP( CUserDefinedJsPropertyPage )
	END_DDX_MAP()

	BEGIN_MSG_MAP( CUserDefinedJsPropertyPage )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_RELOAD, OnReload )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_OPENJSFOLDER, OnOpenJavascriptFolder )
		CHAIN_MSG_MAP( CPropertyPageImpl<CUserDefinedJsPropertyPage> )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnReload(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnOpenJavascriptFolder(UINT uNotifyCode, int nID, CWindow wndCtl);


private:

	// Data members
	CUserJsListView	m_JsListView;
	CFileNotification	m_fileNotification;
};



































