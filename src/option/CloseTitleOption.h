/**
 *	@file	CloseTitleOption.h
 *	@brief	donutのオプション: タイトルによる表示抑止
 */

#pragma once

#include "../resource.h"


/**
	CCloseTitlesOption
	タイトルによる表示抑止のためのデータベースと機能をもつクラス

	表示禁止ページタイトルのリストと検索機能、
	リストのファイルへの読み書き機能を有します。

	使い方はシンプルで、プログラム開始時にGetProfile、終了時にWriteProfileを呼び出します。
	あとはページを開く際にそのタイトルをSearchStringで検索して、
	trueが返ればページを開くのをキャンセルする処理を入れればいいわけです。
 */
class CCloseTitlesOption {
	friend class CCloseTitlesPropertyPage;

	typedef std::list<CString>	CStringList;			//タイトルリストの型名
	static CStringList *		s_pCloseTitles; 		//タイトルリスト
public:
	static bool 				s_bValid;				//この機能は有効であるか

	static void GetProfile();							//ファイルからのリスト読み込み他初期化処理
	static void WriteProfile(); 						//ファイルへのリスト書き出し他終了処理
	static bool SearchString(const CString &strTitle);	//タイトルの検索
	static void Add(const CString &strTitle);			//リストへの項目追加
};



/**
	CCloseTitlesPropertyPage
	ページタイトルによる表示抑止を設定するためのプロパティベージダイアログクラス

	表示禁止タイトルの一覧を編集するためのダイアログ
 */
class CCloseTitlesPropertyPage
	: public CPropertyPageImpl<CCloseTitlesPropertyPage>
	, public CWinDataExchange<CCloseTitlesPropertyPage>
{
private:
	//メンバ変数
	CCloseTitlesOption::CStringList   m_urls;			//禁止タイトルのリスト(命名はちょっとおかしい)
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

	//メッセージハンドラ
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


	//コマンドハンドラ
	void	OnDelCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnDelAllCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnAddCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	void	OnSelChange(UINT code, int id, HWND hWnd);
	void	OnListKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);


	//内部関数
	void	GetData();									//ダイアログからデータの取得
	BOOL	DataExchange(BOOL bSaveAndValidate);		//コントロールの状態とデータの変換


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

public:
	//メッセージマップ
	BEGIN_MSG_MAP( CCloseTitlesPropertyPage )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG	, OnInitDialog	)
		MESSAGE_HANDLER 	 ( WM_DESTROY		, OnDestroy 	)
		COMMAND_ID_HANDLER_EX( IDC_ADD_BUTTON	, OnAddCmd		)
		COMMAND_ID_HANDLER_EX( IDC_DELALL_BUTTON, OnDelAllCmd	)
		COMMAND_ID_HANDLER_EX( IDC_DEL_BUTTON	, OnDelCmd		)
		COMMAND_HANDLER_EX	 ( IDC_IGNORED_URL_LIST, LBN_SELCHANGE, OnSelChange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CCloseTitlesPropertyPage> )
	ALT_MSG_MAP(1)
		MSG_WM_KEYUP( OnListKeyUp )
	END_MSG_MAP()
};
