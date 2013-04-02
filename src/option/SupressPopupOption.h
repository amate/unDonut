/**
 *	@file	SupressPopupOption.h
 *	@brief	donutのオプション : ポップアップ/タイトル抑止
 */

#pragma once

#include "../resource.h"
#include "../SharedMemoryUtil.h"
#include <tuple>

struct PopupBlockData
{
	bool	bValidIgnoreURL;
	std::list<CString>	IgnoreURLList;
	bool	bValidCloseTitle;
	std::list<CString>	CloseTitleList;

	PopupBlockData() : bValidIgnoreURL(false), bValidCloseTitle(false)
	{	}


private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & bValidIgnoreURL  & IgnoreURLList;
		ar & bValidCloseTitle & CloseTitleList;
	}
};

class CSupressPopupOption 
{
public:
	static PopupBlockData	s_PopupBlockData;

	// for MainFrame
	static void CreateSupressPopupData(HWND hWndMainFrame);

	// for ChildFrame
	static void UpdateSupressPopupData(HWND hWndMainFrame);

	static bool SearchURLString(const CString &strURL);
	static bool SearchTitleString(const CString &strTitle);

	// for MainFrame
	static void AddIgnoreURL(const CString &strURL);
	static void AddCloseTitle(const CString &strTitle);

	static void ReCreateSupressPopupDataAndNotify();

protected:
	static void GetProfile();
	static void WriteProfile(bool bIgnoreURLPropertyPage);	
	static void	NotifyUpdateToChildFrame();

	// Data members
	static HWND				s_hWndMainFrame;
	static CSharedMemory	s_sharedMem;
	
};

/**
	CSurpressPopupPropertyPage
	URL/タイトルによる表示抑止を設定するためのプロパティベージダイアログクラス

	表示禁止タイトルの一覧を編集するためのダイアログ
 */
class CSurpressPopupPropertyPage
	: public CPropertyPageImpl<CSurpressPopupPropertyPage>
	, public CWinDataExchange<CSurpressPopupPropertyPage>
	, protected CSupressPopupOption
{
public:
	// Declarations
	enum { IDD = IDD_PROPPAGE_SURPRESSPOPUP };

	// Constructor
	CSurpressPopupPropertyPage(const CString& url, const CString& title);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// DDX map
	BEGIN_DDX_MAP( CSurpressPopupPropertyPage )
		DDX_CHECK( IDC_IGNORED_URL_VALID, m_bIgnoreURLValid )
		DDX_CHECK( IDC_CHK_TITLECLOSE	, m_bTitleCloseValid )
		DDX_CONTROL_HANDLE( IDC_IGNORED_URL_LIST, m_listboxIgnoreURLs )
		DDX_CONTROL_HANDLE( IDC_CLOSE_TITLE_LIST, m_listboxCloseTitle )
		DDX_TEXT( IDC_URL_EDIT , m_strURL )
		DDX_TEXT(IDC_TITLE_EDIT, m_strTitle )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CSurpressPopupPropertyPage )
		MESSAGE_HANDLER( WM_INITDIALOG	, OnInitDialog )
		MESSAGE_HANDLER( WM_DESTROY 	, OnDestroy    )
		// ポップアップ抑止
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON	, OnAddCmd	  )
		COMMAND_ID_HANDLER_EX( IDC_APPLY		, OnApply	)
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON, OnDelAllCmd )
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON	, OnDelCmd	  )
		COMMAND_HANDLER_EX( IDC_IGNORED_URL_LIST, LBN_SELCHANGE, OnSelChange )

		// タイトル抑止
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON2	, OnAddCmd	  )
		COMMAND_ID_HANDLER_EX( IDC_APPLY2		, OnApply	)
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON2, OnDelAllCmd )
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON2	, OnDelCmd	  )
		COMMAND_HANDLER_EX( IDC_CLOSE_TITLE_LIST, LBN_SELCHANGE, OnSelChange )

		CHAIN_MSG_MAP( CPropertyPageImpl<CSurpressPopupPropertyPage> )
	ALT_MSG_MAP(1)
		MSG_WM_KEYUP( OnURLListKeyUp )
	ALT_MSG_MAP(2)
		MSG_WM_KEYUP( OnTitleListKeyUp )
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


	void	OnDelCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnApply(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnDelAllCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnAddCmd(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnSelChange(UINT code, int id, HWND hWnd);
	void	OnURLListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnTitleListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	// Implementation
	std::tuple<CListBox, CEdit, std::list<CString>&> _GetListFromID(int ID);
	void _GetData();

	// Data members
	bool				m_bInit;
	CListBox			m_listboxIgnoreURLs;
	CListBox			m_listboxCloseTitle;
	CString				m_strURL;
	CString				m_strTitle;
	bool 				m_bIgnoreURLValid;
	bool				m_bTitleCloseValid;
	CContainedWindow	m_wndURLList;
	CContainedWindow	m_wndTitleList;
};



