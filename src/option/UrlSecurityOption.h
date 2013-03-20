/**
 *	@file	UrlSecurityOption.h
 *	@brief	donutのオプション : URL別セキュリティ.
 *	@note
 *		undonut+modで追加.
 */

#pragma once

#include "../resource.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include "../SharedMemoryUtil.h"

struct UrlSecurityData
{
	DWORD	flags;
	DWORD	opts;
	DWORD	opts2;
	std::wstring urlPatern;

	UrlSecurityData() {	}	// for serialize

	UrlSecurityData(DWORD f, DWORD op, DWORD op2, LPCWSTR url) : flags(f), opts(op), opts2(op2), urlPatern(url)
	{	}

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & flags & opts & opts2 & urlPatern;
	}
};

/// URL別セキュリティ
class CUrlSecurityOption {
	friend class CUrlSecurityPropertyPage;
	friend class CUrlSecurityForChildFrame;

public:
	static bool s_bValid;		//+++ ref by CMainFrame

	static void GetProfile();
	static void WriteProfile();

	static void UpdateOriginalUrlSecurityList(HWND hWndMainFrame);

	//static bool SearchString(const CString &strURL);
	static int	MatchTest(const CString& strURL);
	static void Add(unsigned flags, unsigned opts, unsigned opts2, const CString &strURL);

private:
	static CSharedMemory	s_sharedMem;
	static std::list<UrlSecurityData>	s_UrlSecurityList;
};

class CUrlSecurityForChildFrame
{
public:
	void	SetMainFrameHWND(HWND hWnd) { m_hWndMainFrame = hWnd; }

	void	ReloadList();

	bool	FindUrl(const CString& strURL, DWORD* exprop, DWORD* exprop2, DWORD* flags);
	bool	IsUndoSecurity(const CString& strURL);

private:
	// Data members
	HWND	m_hWndMainFrame;	// for Sharedmemory name
	std::list<UrlSecurityData>	m_UrlSecurityList;

};

////////////////////////////////////////////////////////////////////////////
// URL別セキュリティ

class CUrlSecurityPropertyPage
	: public CPropertyPageImpl<CUrlSecurityPropertyPage>
	, public CWinDataExchange<CUrlSecurityPropertyPage>
	, protected CUrlSecurityOption
{
public:
	// Declarations
	enum { IDD = IDD_PROPPAGE_URLSECURITY };

	// Constructor
	CUrlSecurityPropertyPage(const CString &strAddressBar, HWND hWndMainFrame);

	// DDX map
	BEGIN_DDX_MAP( CUrlSecurityPropertyPage )
		DDX_CHECK( IDC_CHK_URLSECURITY		, m_nValid  )
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
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_MATCHTEST	, OnMatchTest )
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
	void	OnMatchTest(UINT wNotifyCode, int wID, HWND hWndCtl);
	void 	DispExOpts();
	void 	DispExOptsFlag(unsigned exopts, unsigned exopts2, unsigned flags);	//+++ exopts2追加

private:
	void _GetData();

	// Data members
	CString 			m_strAddressBar;
	HWND				m_hWndMainFrame;
	CListBox			m_listbox;
	CEdit				m_edit;
	int 				m_nValid;
	CContainedWindow	m_wndList;
};
