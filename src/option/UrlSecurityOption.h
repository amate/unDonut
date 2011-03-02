/**
 *	@file	UrlSecurityOption.h
 *	@brief	donutのオプション : URL別セキュリティ.
 *	@note
 *		undonut+modで追加.
 */

#pragma once

#include "../resource.h"


/// URL別セキュリティ
class CUrlSecurityOption {
	friend class CUrlSecurityPropertyPage;

public:
	class COptUrl {
	public:
		COptUrl() : m_opts(0), m_flags(0), m_opts2(0) {}
		COptUrl(unsigned flags, unsigned opts, unsigned opts2, LPCTSTR pUrl) : m_flags(flags), m_opts(opts), m_opts2(opts2), m_url(pUrl) {;}

		operator LPCTSTR() const { return LPCTSTR(m_url); }		//+++ 無理やり文字列のふりをする
		bool operator==(const COptUrl& rhs) const { return m_url == rhs.m_url; }
		bool operator==(LPCTSTR r) const { return m_url == r; }

	public:
		unsigned	m_flags;
		unsigned	m_opts;
		unsigned	m_opts2;		//+++ flatview用に追加
		CString		m_url;
	};
	typedef std::list<COptUrl>		COptUrlList;

	static bool 					s_bValid;		//+++ ref by CMainFrame

	static void GetProfile();
	static void WriteProfile();

	//static bool SearchString(const CString &strURL);
	static bool FindUrl(const CString &strURL, DWORD* exprop, DWORD* exprop2, DWORD* flags);
	static void Add(unsigned flags, unsigned opts, unsigned opts2, const CString &strURL);
	static bool IsUndoSecurity(const CString& strURL);

  #if 1	//+++ とりあえず常時onにしとく
	static bool activePageToo() { return 1; }
  #else
	static bool activePageToo() { return s_bActivePageToo; }
  #endif

private:
	static COptUrlList				s_urlSecurity;
	static bool						s_bActivePageToo;

	static COptUrlList::iterator	get_optUrlList_iterator(unsigned no);
};


class CUrlSecurityPropertyPage
	: public CPropertyPageImpl<CUrlSecurityPropertyPage>
	, public CWinDataExchange<CUrlSecurityPropertyPage>
	, protected CUrlSecurityOption
{
public:
	// Declarations
	enum { IDD = IDD_PROPPAGE_URLSECURITY };

	// Constructor
	CUrlSecurityPropertyPage(const CString &strAddressBar);

	// DDX map
	BEGIN_DDX_MAP( CUrlSecurityPropertyPage )
		DDX_CHECK( IDC_CHK_URLSECURITY		, m_nValid  )
		DDX_CHECK( IDC_CHK_URLSEC_ACTPAG    , m_nActPag )
	END_DDX_MAP()

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	void	DebugPrint();

private:
	// overrides
	BOOL	_DoDataExchange(BOOL bSaveAndValidate); // get data from controls?

public:
	// Message map and handlers
	BEGIN_MSG_MAP( CUrlSecurityPropertyPage )
		MESSAGE_HANDLER( WM_INITDIALOG	, OnInitDialog )
		MESSAGE_HANDLER( WM_DESTROY 	, OnDestroy    )
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON		, OnAddCmd	  )
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON	, OnDelAllCmd )
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON		, OnDelCmd	  )
		COMMAND_ID_HANDLER_EX( IDC_BTN_URLSEC_UP	, OnBtnUp	  )
		COMMAND_ID_HANDLER_EX( IDC_BTN_URLSEC_DOWN	, OnBtnDown	  )
		COMMAND_ID_HANDLER_EX( IDC_BTN_URLSEC_SETTING,OnBtnSetting)
		COMMAND_HANDLER_EX( IDC_IGNORED_URL_LIST, LBN_SELCHANGE	, OnSelChange )
		COMMAND_HANDLER_EX( IDC_IGNORED_URL_LIST, LBN_DBLCLK	, OnDblclkList)
		CHAIN_MSG_MAP( CPropertyPageImpl<CUrlSecurityPropertyPage> )
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
	void	OnDblclkList(UINT code, int id, HWND hWnd);
	void	OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnBtnUp(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnBtnDown(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnBtnSetting(UINT wNotifyCode, int wID, HWND hWndCtl);
	void 	DispExOpts();
	void 	DispExOptsFlag(unsigned exopts, unsigned exopts2, unsigned flags);	//+++ exopts2追加

	struct _AddToListBox : public std::unary_function<const CUrlSecurityOption::COptUrl &, void> {
		CListBox &_box;
		_AddToListBox(CListBox &box) : _box(box) { }
		result_type operator ()(argument_type src)
		{
			_box.AddString(src.m_url);
		}
	};

	void _GetData();

private:
	// Data members
	CString 						m_strAddressBar;
	CListBox						m_listbox;
	CEdit							m_edit;
	int 							m_nValid;
	int 							m_nActPag;
	CContainedWindow				m_wndList;
};
