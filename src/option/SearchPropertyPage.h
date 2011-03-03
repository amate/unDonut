/**
 *	@file	SearchPropertyPage.h
 *	@brief	検索バーに関するオプション設定
 *	@note
 *		+++ SearchBar.h より分離.
 */

#ifndef SEARCHPROPETRYPAGE_H
#define SEARCHPROPETRYPAGE_H
#pragma once

enum ESts {
	STS_ENG_CHANGE_GO		=	0x00000001,	// 検索エンジン変更時に即検索
	STS_DROP_GO 			=	0x00000002,	// ドロップ時に即検索
	STS_HISTORY_SAVE		=	0x00000004,	// 検索履歴を保存する
	STS_NO_REPOSITION		=	0x00000008,	// 検索エンジンはみ出し防止動作をやめる
	STS_KEY_CHANGE_GO		=	0x00000010,	// キーワード変更時に即検索
	STS_HEIGHTCOUNT 		=	0x00000020,	// リストの最大表示項目数を指定する:
	STS_TEXT_FILTER 		=	0x00000040,	// 全角スペースを半角とみなして検索する
	STS_LASTSEL 			=	0x00000080,	// 最後に選択した検索エンジンを記憶する
	STS_SCROLLCENTER		=	0x00000100,	// 検索結果が中央に来るようにスクロールする
	STS_ACTIVEWINDOW		=	0x00000200,	// 検索結果はアクティブなウィンドウに表示する
	STS_AUTOHILIGHT 		=	0x00000400,	// 検索結果に対して自動的にハイライトを実行する
	STS_SAVE_SEARCHWORD		=	0x00000800,	// タブごとに検索文字列を保存する
	STS_USE_SHORTCUT		=	0x00001000,	// 検索エンジンのショートカット機能を有効にする
	STS_SEARCHLIST_WIDTH	=	0x00002000,	// 検索エンジンリストの幅を指定する
};


enum {
	DEFAULT_HEIGHTCOUNT 	=  25,
	MAXHEIGHTCOUNT			= 100,
	DEFAULT_SEARCHLISTWIDTH = 170,
};

#include "SearchBar.h"


/////////////////////////////////////////////////////////////
// CSearchBarOption

class CSearchBarOption
{
public:
	static DWORD	s_dwStatus;
	static int		s_nHistorySaveCnt;
	static int		s_nHeightCountCnt;
	static int		s_nMinimumLength;
	static int		s_nSearchListWidth;

	static bool 	s_bEngChgGo;
	static bool 	s_bDropGo;
	static bool 	s_bHistorySave;
	static bool 	s_bNoReposition;
	static bool 	s_bKeyChgGo;
	static bool 	s_bHeightCount;
	static bool 	s_bFiltering;
	static bool 	s_bLastSel;
	static bool 	s_bScrollCenter;
	static bool 	s_bActiveWindow;
	static bool 	s_bAutoHilight;
	static bool		s_bSaveSearchWord;
	static bool		s_bUseShortcut;
	static bool		s_bSearchListWidth;

	static bool		s_bNoWordButton;	//+++ 単語ボタン無しならtrue(旧処理を有効にする)
	static bool		s_bUsePageButton;	//+++ 単語ボタン使用時にも単独のページ内検索ボタンを使う

public:
	static void 	GetProfile()
	{
		CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
		s_dwStatus			= pr.GetValue( _T("Status"), 
			STS_TEXT_FILTER | STS_DROP_GO | STS_HEIGHTCOUNT | STS_HISTORY_SAVE | STS_SEARCHLIST_WIDTH);
		s_nHistorySaveCnt	= pr.GetValue( _T("HistoryCnt"), 10 );
		s_nHeightCountCnt	= pr.GetValue( _T("HeightCnt") , DEFAULT_HEIGHTCOUNT );
		s_nMinimumLength	= pr.GetValue( _T("MinimumLength"), 1);
		s_nSearchListWidth	= pr.GetValue( _T("SearchListWidth"), DEFAULT_SEARCHLISTWIDTH );

		s_bEngChgGo 		= (s_dwStatus & STS_ENG_CHANGE_GO	) == STS_ENG_CHANGE_GO;
		s_bDropGo			= (s_dwStatus & STS_DROP_GO 		) == STS_DROP_GO;
		s_bHistorySave		= (s_dwStatus & STS_HISTORY_SAVE	) == STS_HISTORY_SAVE;
		s_bNoReposition 	= (s_dwStatus & STS_NO_REPOSITION	) == STS_NO_REPOSITION;
		s_bKeyChgGo 		= (s_dwStatus & STS_KEY_CHANGE_GO	) == STS_KEY_CHANGE_GO;
		s_bHeightCount		= (s_dwStatus & STS_HEIGHTCOUNT		) == STS_HEIGHTCOUNT;
		s_bFiltering		= (s_dwStatus & STS_TEXT_FILTER		) == STS_TEXT_FILTER;
		s_bLastSel			= (s_dwStatus & STS_LASTSEL 		) == STS_LASTSEL;
		s_bScrollCenter 	= (s_dwStatus & STS_SCROLLCENTER	) == STS_SCROLLCENTER;
		s_bActiveWindow 	= (s_dwStatus & STS_ACTIVEWINDOW	) == STS_ACTIVEWINDOW;
		s_bAutoHilight		= (s_dwStatus & STS_AUTOHILIGHT		) == STS_AUTOHILIGHT;
		s_bSaveSearchWord	= (s_dwStatus & STS_SAVE_SEARCHWORD	) == STS_SAVE_SEARCHWORD;
		s_bUseShortcut		= (s_dwStatus & STS_USE_SHORTCUT	) == STS_USE_SHORTCUT;
		s_bSearchListWidth	= (s_dwStatus & STS_SEARCHLIST_WIDTH) == STS_SEARCHLIST_WIDTH;

		DWORD	dwNoWordButton = pr.GetValue( _T("NoWordButton"), 0 );
		s_bNoWordButton 	= (dwNoWordButton & 0x1) != 0;		//+++
		s_bUsePageButton	= (dwNoWordButton & 0x2) != 0;				//+++ キー名が不適になるが、単独ページボタンのフラグもついでに含める...
		pr.Close();
	}

	static void 	WriteProfile()
	{
		CIniFileO	pr( g_szIniFileName, _T("SEARCH") );

		DWORD dwStatus = 0;
		if (s_bEngChgGo)		dwStatus |= STS_ENG_CHANGE_GO;
		if (s_bDropGo)			dwStatus |= STS_DROP_GO;
		if (s_bHistorySave) 	dwStatus |= STS_HISTORY_SAVE;
		if (s_bNoReposition)	dwStatus |= STS_NO_REPOSITION;
		if (s_bKeyChgGo)		dwStatus |= STS_KEY_CHANGE_GO;
		if (s_bHeightCount) 	dwStatus |= STS_HEIGHTCOUNT;
		if (s_bFiltering)		dwStatus |= STS_TEXT_FILTER;
		if (s_bLastSel) 		dwStatus |= STS_LASTSEL;
		if (s_bScrollCenter)	dwStatus |= STS_SCROLLCENTER;
		if (s_bActiveWindow)	dwStatus |= STS_ACTIVEWINDOW;
		if (s_bAutoHilight) 	dwStatus |= STS_AUTOHILIGHT;
		if (s_bSaveSearchWord)	dwStatus |= STS_SAVE_SEARCHWORD;
		if (s_bUseShortcut)		dwStatus |= STS_USE_SHORTCUT;
		if (s_bSearchListWidth) dwStatus |= STS_SEARCHLIST_WIDTH;

		s_dwStatus = dwStatus;

		pr.SetValue( s_dwStatus, _T("Status") );
		pr.SetValue( (DWORD) s_nHistorySaveCnt	, _T("HistoryCnt") );
		pr.SetValue( (DWORD) s_nHeightCountCnt	, _T("HeightCnt") );
		pr.SetValue( (DWORD) s_nMinimumLength	, _T("MinimumLength") );
		pr.SetValue( (DWORD) s_nSearchListWidth	, _T("SearchListWidth") );

		DWORD nowordbutton = 0;	//+++ キー名が不適になるが、単独ページボタンのフラグもついでに含める...
		if (s_bNoWordButton)	nowordbutton |= 0x1;
		if (s_bUsePageButton)	nowordbutton |= 0x2;
		pr.SetValue( (DWORD) nowordbutton, _T("NoWordButton") );
	}
};

__declspec(selectany) DWORD	CSearchBarOption::s_dwStatus = 0;
__declspec(selectany) int	CSearchBarOption::s_nHistorySaveCnt= 10;
__declspec(selectany) int	CSearchBarOption::s_nHeightCountCnt= DEFAULT_HEIGHTCOUNT;
__declspec(selectany) int	CSearchBarOption::s_nMinimumLength = 1;
__declspec(selectany) int	CSearchBarOption::s_nSearchListWidth = DEFAULT_SEARCHLISTWIDTH;

__declspec(selectany) bool 	CSearchBarOption::s_bEngChgGo			= false;
__declspec(selectany) bool	CSearchBarOption::s_bDropGo				= false;
__declspec(selectany) bool 	CSearchBarOption::s_bHistorySave		= false;
__declspec(selectany) bool 	CSearchBarOption::s_bNoReposition		= false;
__declspec(selectany) bool 	CSearchBarOption::s_bKeyChgGo			= false;
__declspec(selectany) bool 	CSearchBarOption::s_bHeightCount		= false;
__declspec(selectany) bool 	CSearchBarOption::s_bFiltering			= false;
__declspec(selectany) bool 	CSearchBarOption::s_bLastSel			= false;
__declspec(selectany) bool 	CSearchBarOption::s_bScrollCenter		= false;
__declspec(selectany) bool 	CSearchBarOption::s_bActiveWindow		= false;
__declspec(selectany) bool 	CSearchBarOption::s_bAutoHilight		= false;
__declspec(selectany) bool	CSearchBarOption::s_bSaveSearchWord		= false;
__declspec(selectany) bool	CSearchBarOption::s_bUseShortcut		= false;
__declspec(selectany) bool	CSearchBarOption::s_bSearchListWidth	= false;

__declspec(selectany) bool	CSearchBarOption::s_bNoWordButton		= false;
__declspec(selectany) bool	CSearchBarOption::s_bUsePageButton		= false;




/////////////////////////////////////////////////////////////
// CSearchPropertyPage

class CSearchPropertyPage
	: public CPropertyPageImpl<CSearchPropertyPage>
	, public CWinDataExchange<CSearchPropertyPage>
	, protected CSearchBarOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_SEARCHBAR };

private:
	// Data members
	bool 	m_bEngChgGo;
	bool 	m_bDropGo;
	bool 	m_bHistorySave;
	int 	m_nHistorySaveCnt;
	bool 	m_bNoReposition;
	bool 	m_bKeyChgGo;
	bool 	m_bHeightCount;
	int 	m_nHeightCountCnt;
	bool 	m_bFiltering;
	bool 	m_bLastSel;
	bool 	m_bScrollCenter;
	bool 	m_bActiveWindow;
	bool 	m_bAutoHilight;
	bool	m_bSaveSearchWord;
	bool	m_bUseShortcut;
	bool 	m_bNoWordButton;		//+++
	bool	m_bUsePageButton;		//+++
	bool	m_bSearchListWidth;
	int		m_nSearchListWidth;

	CComboBox	m_cmbMinimumLength;

public:
	// DDX map
	BEGIN_DDX_MAP(CSearchPropertyPage)
		DDX_CHECK( IDC_CHECK_ENG_CHG_GO			, m_bEngChgGo 		)
		DDX_CHECK( IDC_CHECK_KEY_CHG_GO			, m_bKeyChgGo 		)
		DDX_CHECK( IDC_CHECK_DROP_GO			, m_bDropGo			)
		DDX_CHECK( IDC_CHECK_HISTORY_SAVE		, m_bHistorySave	)
		DDX_CHECK( IDC_CHECK_NO_REPOSITION		, m_bNoReposition	)
		DDX_CHECK( IDC_CHECK_HEIGHTCOUNT		, m_bHeightCount	)
		DDX_CHECK( IDC_CHECK_FILTER 			, m_bFiltering		)
		DDX_CHECK( IDC_CHECK_LASTSEL			, m_bLastSel		)
		DDX_CHECK( IDC_CHECK_SCROLLCENTER		, m_bScrollCenter	)
		DDX_CHECK( IDC_CHECK_ACTIVEWINDOW		, m_bActiveWindow	)
		DDX_CHECK( IDC_CHECK_AUTOHILIGHT		, m_bAutoHilight	)
		DDX_CHECK( IDC_CHECK_SAVE_SEARCHWORD	, m_bSaveSearchWord )
		DDX_CHECK( IDC_CHECK_USE_SHORTCUT		, m_bUseShortcut	)
		DDX_CHECK( IDC_SEARCH_NOWORDBUTTON		, m_bNoWordButton	)		//+++
		DDX_CHECK( IDC_SEARCH_PAGEBUTTON		, m_bUsePageButton	)		//+++
		DDX_CHECK( IDC_CHECK_SEARCHLIST_WIDTH	, m_bSearchListWidth)
		DDX_INT_RANGE (	IDC_EDIT_SEARCHLIST_WIDTH, m_nSearchListWidth, 1, 500 )

		DDX_INT  ( IDC_ED_HISTORY_CNT		, m_nHistorySaveCnt )
		DDX_INT_RANGE( IDC_EDIT_HEIGHTCOUNT	, m_nHeightCountCnt, 1, (int)MAXHEIGHTCOUNT )
	END_DDX_MAP()


public:
	// Constructor
	CSearchPropertyPage()
	{
	  #if 0 //+++ _SetData()で設定されているのでやめにしておく.
		m_dwStatus		  = 0;
		m_nEngChgGo 	  = 0;
		m_nDropGo		  = 0;
		m_nHistorySave	  = 0;
		m_nHistorySaveCnt = 0;
		m_nNoReposition   = 0;
		m_nKeyChgGo 	  = 0;
		m_nHeightCount	  = 0;
		m_nFiltering	  = 0;
		m_nLastSel		  = 0;
		m_nScrollCenter   = 1;
		m_nActiveWindow   = 0;
		m_nAutoHilight	  = 0;
	  #endif
		_SetData();
	}


	// Overrides
	BOOL OnSetActive()
	{
		SetModified(TRUE);
		return DoDataExchange(FALSE);
	}


	BOOL OnKillActive()
	{
		return DoDataExchange(TRUE);
	}


	BOOL OnApply()
	{
		if ( DoDataExchange(TRUE) ) {
			_GetData();
			return TRUE;
		} else {
			return FALSE;
		}
	}


private:

	/// データを得る
	void _SetData()
	{
		m_nHistorySaveCnt	= s_nHistorySaveCnt;
		m_nHeightCountCnt	= s_nHeightCountCnt;
		m_nSearchListWidth	= s_nSearchListWidth;
		
		m_bEngChgGo 		= s_bEngChgGo;
		m_bKeyChgGo 		= s_bKeyChgGo;
		m_bDropGo			= s_bDropGo;
		m_bHistorySave		= s_bHistorySave;
		m_bNoReposition		= s_bNoReposition;
		m_bHeightCount		= s_bHeightCount;
		m_bFiltering		= s_bFiltering;	
		m_bLastSel			= s_bLastSel;
		m_bScrollCenter		= s_bScrollCenter;
		m_bActiveWindow		= s_bActiveWindow;
		m_bAutoHilight		= s_bAutoHilight;
		m_bSaveSearchWord	= s_bSaveSearchWord;
		m_bUseShortcut		= s_bUseShortcut;
		m_bSearchListWidth	= s_bSearchListWidth;
		  
		m_bNoWordButton		= s_bNoWordButton;
		m_bUsePageButton	= s_bUsePageButton;
	}


	/// データを保存
	void _GetData()
	{
		s_nHistorySaveCnt	= m_nHistorySaveCnt;
		s_nHeightCountCnt	= m_nHeightCountCnt;
		s_nMinimumLength	= m_cmbMinimumLength.GetCurSel() + 1;
		s_nSearchListWidth	= m_nSearchListWidth;
		
		s_bEngChgGo			= m_bEngChgGo;
		s_bKeyChgGo			= m_bKeyChgGo;
		s_bDropGo			= m_bDropGo;
		s_bHistorySave		= m_bHistorySave;
		s_bNoReposition		= m_bNoReposition;
		s_bHeightCount		= m_bHeightCount;
		s_bFiltering		= m_bFiltering;	
		s_bLastSel			= m_bLastSel;
		s_bScrollCenter		= m_bScrollCenter;
		s_bActiveWindow		= m_bActiveWindow;
		s_bAutoHilight		= m_bAutoHilight;
		s_bSaveSearchWord	= m_bSaveSearchWord;
		s_bUseShortcut		= m_bUseShortcut;
		s_bSearchListWidth	= m_bSearchListWidth;
								
		s_bNoWordButton		= m_bNoWordButton;
		s_bUsePageButton	= m_bUsePageButton;

		WriteProfile();
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CSearchPropertyPage)
		MSG_WM_INITDIALOG( OnInitDialog )
		CHAIN_MSG_MAP(CPropertyPageImpl<CSearchPropertyPage>)
	END_MSG_MAP()

	BOOL	OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		m_cmbMinimumLength = GetDlgItem(IDC_COMBO_MINIMUM_LENGTH);
		for (int i = 1; i <= 5; ++i) {
			CString str;
			str.Append(i);
			m_cmbMinimumLength.AddString(str);
		}
		m_cmbMinimumLength.SetCurSel(s_nMinimumLength - 1);

		return 0;
	}

};


#endif
