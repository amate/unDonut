/**
 *	@file	SupressPopupOption.h
 *	@brief	donutのオプション : ポップアップ/タイトル抑止
 */

#pragma once

#include "../resource.h"
#include "../SharedMemoryUtil.h"

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
	CIgnoredURLsPropertyPage
	URLによる表示抑止を設定するためのプロパティベージダイアログクラス

	表示禁止タイトルの一覧を編集するためのダイアログ
 */
class CIgnoredURLsPropertyPage
	: public CPropertyPageImpl<CIgnoredURLsPropertyPage>
	, public CWinDataExchange<CIgnoredURLsPropertyPage>
	, protected CSupressPopupOption
{
public:
	// Declarations
	enum { IDD = IDD_PROPPAGE_IGNOREDURLS };

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
		COMMAND_ID_HANDLER_EX( IDC_APPLY		, OnApply	)
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
	void	OnApply(UINT wNotifyCode, int wID, HWND hWndCtl);
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

	// Data members
	bool	m_bInit;
	CString 						m_strAddressBar;
	CListBox						m_listbox;
	CEdit							m_edit;
	int 							m_nValid;
	CContainedWindow				m_wndList;
};




/**
	CCloseTitlesPropertyPage
	ページタイトルによる表示抑止を設定するためのプロパティベージダイアログクラス

	表示禁止タイトルの一覧を編集するためのダイアログ
 */
class CCloseTitlesPropertyPage
	: public CPropertyPageImpl<CCloseTitlesPropertyPage>
	, public CWinDataExchange<CCloseTitlesPropertyPage>
	, protected CSupressPopupOption
{
public:
	//ダイアログのリソースID
	enum { IDD = IDD_PROPPAGE_CLOSETITLES };

	//コンストラクタ
	CCloseTitlesPropertyPage(const CString &strAddressBar);

	//DDXマップ
	BEGIN_DDX_MAP(CCloseTitlesPropertyPage) 			//+++
		DDX_CHECK( IDC_CHK_TITLECLOSE, m_nValid )
	END_DDX_MAP()

	//プロパティベージとしてのオーバーライド関数
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	//メッセージマップ
	BEGIN_MSG_MAP( CCloseTitlesPropertyPage )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG	, OnInitDialog	)
		MESSAGE_HANDLER 	 ( WM_DESTROY		, OnDestroy 	)
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON	, OnAddCmd		)
		COMMAND_ID_HANDLER_EX( IDC_APPLY		, OnApply	)
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON, OnDelAllCmd	)
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON	, OnDelCmd		)
		COMMAND_HANDLER_EX	 ( IDC_IGNORED_URL_LIST, LBN_SELCHANGE, OnSelChange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CCloseTitlesPropertyPage> )
	ALT_MSG_MAP(1)
		MSG_WM_KEYUP( OnListKeyUp )
	END_MSG_MAP()

	//メッセージハンドラ
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


	//コマンドハンドラ
	void	OnDelCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnApply(UINT wNotifyCode, int wID, HWND hWndCtl);
	void	OnDelAllCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnAddCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnSelChange(UINT code, int id, HWND hWnd);
	void	OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	//内部関数
	void	GetData();									//ダイアログからデータの取得
	BOOL	DataExchange(BOOL bSaveAndValidate);		//コントロールの状態とデータの変換

private:
	//メンバ変数
	bool	m_bInit;
	CString 						  m_strAddressBar;	//今開いているページのタイトル
	CListBox						  m_listbox;		//リストボックスの操作クラス
	CEdit							  m_edit;			//テキストボックスの操作クラス
	int 							  m_nValid; 		//タイトル抑止機能は有効か
	CContainedWindow				  m_wndList;		//リストボックスの操作クラス

	//関数オブジェクト
	struct AddToListBox : public std::unary_function<const CString &, void> {
		CListBox &m_box;
		AddToListBox(CListBox &box) : m_box(box) { }
		result_type operator ()(argument_type src)
		{
			m_box.AddString(src);
		}
	};
};
