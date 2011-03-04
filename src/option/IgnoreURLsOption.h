/**
 *	@file	IgnoreURLsOption.h
 *	@brief	donutのオプション : ポップアップ抑止
 */

#pragma once

#include "../resource.h"


class CIgnoredURLsOption {
	friend class CIgnoredURLsPropertyPage;
private:
	typedef std::list<CString>	CStringList;
	static CStringList *		s_pIgnoredURLs;

public:
	static bool 				s_bValid;		//+++ ref by CMainFrame

	static void GetProfile();
	static void WriteProfile();
	static bool SearchString(const CString &strURL);
	static void Add(const CString &strURL);
};


class CIgnoredURLsPropertyPage
	: public CPropertyPageImpl<CIgnoredURLsPropertyPage>
	, public CWinDataExchange<CIgnoredURLsPropertyPage>
{
public:
	// Declarations
	enum { IDD = IDD_PROPPAGE_IGNOREDURLS };

private:
	// Data members
	CIgnoredURLsOption::CStringList m_urls;
	CString 						m_strAddressBar;
	CListBox						m_listbox;
	CEdit							m_edit;
	int 							m_nValid;
	CContainedWindow				m_wndList;

public:
	// Constructor
	CIgnoredURLsPropertyPage(const CString &strAddressBar);

	// DDX map
	BEGIN_DDX_MAP( CIgnoredURLsPropertyPage )
		DDX_CHECK( IDC_IGNORED_URL_VALID, m_nValid )
	END_DDX_MAP()

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	// overrides
	BOOL	_DoDataExchange(BOOL bSaveAndValidate); // get data from controls?

public:
	// Message map and handlers
	BEGIN_MSG_MAP( CIgnoredURLsPropertyPage )
		MESSAGE_HANDLER( WM_INITDIALOG	, OnInitDialog )
		MESSAGE_HANDLER( WM_DESTROY 	, OnDestroy    )
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON	, OnAddCmd	  )
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON, OnDelAllCmd )
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON	, OnDelCmd	  )
		COMMAND_HANDLER_EX( IDC_IGNORED_URL_LIST, LBN_SELCHANGE, OnSelChange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CIgnoredURLsPropertyPage> )
	ALT_MSG_MAP(1)
		MSG_WM_KEYUP( OnListKeyUp )
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
	void	OnDelCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnAddCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnSelChange(UINT code, int id, HWND hWnd);
	void	OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// Implementation
	// function objects
	struct _AddToListBox : public std::unary_function<const CString &, void> {
		CListBox &_box;
		_AddToListBox(CListBox &box) : _box(box) { }
		result_type operator ()(argument_type src)
		{
			_box.AddString(src);
		}
	};

	void _GetData();
};
