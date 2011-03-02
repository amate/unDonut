/**
 *	@file	SearchBar.h
 *	@brief	検索バー
 *	@note
 *		templateで実装されていた SearchBar.h を普通のclassにして .h, .cpp 化したもの.
 */

#ifndef SEARCHBAR_H
#define SEARCHBAR_H
#pragma once

#include "MtlDragDrop.h"
#include "FlatComboBox.h"
#include "FlatComboBox_SearchKeyword.h"

#include "option\SearchPropertyPage.h"
#include <boost/thread.hpp>

extern bool 		g_bNoReposition;

struct SearchPostData {
	LPVOID		pPostData;
	int			nPostBytes;

	SearchPostData() :
		pPostData(NULL),
		nPostBytes(0)
	{ }
};

//////////////////////////////////////////////////////////////////////
// CDonutSearchBar : 検索バーの処理

class CDonutSearchBar
//	: public CDialogImpl< CDonutSearchBar >
	: public CWindowImpl< CDonutSearchBar >
	, public IDropTargetImpl< CDonutSearchBar >
	, public IDropSourceImpl< CDonutSearchBar >
	, protected CSearchBarOption
{
public:
	// Constructor/Destructor
	CDonutSearchBar();
	~CDonutSearchBar() { s_pThis_ = NULL; } 	//+++ 手抜きの尻拭い.

	static CDonutSearchBar* GetInstance() { return s_pThis_; }	//+++ あまし気持ちよくないが...手抜きで用意.
	static CString	GetSearchIniPath();

	CString 	GetSearchStr();				//+++
	CString 	GetSearchEngineStr();		//+++
	CMenuHandle GetSearchEngineMenuHandle();

	HWND		GetHWndToolBar() { return m_wndToolBar; }
	CEdit		GetEditCtrl();
	bool		GetHilightSw() const { return m_bShowToolBar && GetHilightBtnFlag(); }	// ハイライトボタンが押されているかどうか
	bool		GetHilightBtnFlag() const { return m_bHilightSw != 0; }				//+++ ハイライトボタンのon/off状態を返す

	void		RefreshEngine() { _InitialEngine(m_cmbEngine.m_hWnd); SetCmbKeywordEmptyStr();} //+++
	void		SetCmbKeywordEmptyStr();	//+++ キーワード入力欄が空の時にエンジン名を表示するための設定

	void		OpenSearch(CString strWord, CString strSearchEng, int &nLoopCnt, BOOL &bFirst);
	const SearchPostData&	GetSearchPostData() { return m_PostData; }

  #if 1 	//+++ ".url"中に記述した拡張情報での検索に対応するため、追加
	bool		GetOpenURLstr(CString& strOpenURL, const CString& strWord0, CIniFileI& pr, const CString& frontURL0 = CString());
  #endif

	BYTE		PreTranslateMessage(MSG *pMsg);
	void		SetFocusToEngine();

	// Overrides
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/);
	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/, DROPEFFECT dropOkEffect);
	void		OnDragLeave();
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint /*point*/);

	void		SetFont(HFONT hFont, BOOL bRedraw = TRUE);
	void		ShowToolBarIcon(BOOL flag);

	bool		ForceSetHilightBtnOn(bool sw);

	void		SearchWeb();
	void		SearchHilight();
	void		SearchPage(BOOL bForward);

	BOOL		GetToolIconState() const { return m_bShowToolBar; }
	void		FilterString(CString &str) { str.Replace( _T("　"), _T(" ") ); }
	void		DeleteMinimumLengthWord(CString &strWord);

	void		ReloadSkin(int nCmbStyle);
	void		SetComboStyle(int nCmbStyle);

	void 		InitComboBox_for_AddressBarPropertyPage(CComboBox& rCmbCtrl, CComboBox& rCmbShift);

	const CString	RemoveShortcutWord(const CString &str);

	//+++ あとでprivateに戻すがとりあえずテスト.
	void		_AddToSearchBoxUnique(const CString &strURL);

	void		SetSearchStr(const CString& strWord); //\\ 渡された文字列を設定する
	void		SetEmptyStrEngine(){ SetCmbKeywordEmptyStr(); m_cmbKeyword.paint_EmptyStr(); }; // 空のときエンジン名を表示	

	// Message map and handlers
	BEGIN_MSG_MAP(CDonutSearchBar)
		MSG_WM_CREATE		(OnCreate		)
		MSG_WM_DESTROY		(OnDestroy		)
		MSG_WM_MOUSEMOVE	(OnMouseMove	)
		MSG_WM_LBUTTONDOWN	(OnLButtonDown	)
		MSG_WM_LBUTTONUP	(OnLButtonUp	)
		MSG_WM_COMMAND		(OnCommand		)
		NOTIFY_CODE_HANDLER (TTN_GETDISPINFO, OnToolTipText)

		MESSAGE_HANDLER 	(WM_SIZE		, OnSize	   )
		MESSAGE_HANDLER 	(WM_ERASEBKGND	, OnEraseBkgnd )
		//COMMAND_HANDLER_EX	(IDC_CMB_ENGIN	, CBN_DBLCLK   , OnSelDblClkForEngine)
		
		COMMAND_HANDLER_EX	(IDC_CMB_KEYWORD, CBN_SELCHANGE, OnSelChange)
		COMMAND_HANDLER_EX	(IDC_CMB_ENGIN	, CBN_SELCHANGE, OnSelChangeForEngine)

		COMMAND_RANGE_HANDLER_EX(ID_INSERTPOINT_SEARCHENGINE, ID_INSERTPOINT_SEARCHENGINE_END, OnSearchWeb_engineId)	//+++
		COMMAND_ID_HANDLER( ID_SEARCHENGINE_MENU, OnSearchEngineMenu )			//+++

		//checkEngineNameEnable();	//* +++ テスト
		//MSG_WM_RBUTTONUP	(OnToolBarRButtonUp	)			//+++

	  #if SEARCH_JIKKEN == 1	//+++
		NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed) //+++
	  #elif SEARCH_JIKKEN == 2	//+++
		CHAIN_MSG_MAP(CChevronHandler<CDonutSearchBar >)		//*+++
	  #endif
	ALT_MSG_MAP(1)	// KeywordEdit
		MSG_WM_CHAR 		(OnChar 		 )
		MSG_WM_KEYDOWN		(OnKeywordKeyDown)
		MSG_WM_KILLFOCUS	(OnKeywordKillFocus)
		NOTIFY_CODE_HANDLER_EX(CBN_DROPDOWN , OnKeywordCbnDropDown)
	ALT_MSG_MAP(2)	// SearchEnginComboBox
		MSG_WM_KEYDOWN		(OnEngineKeyDown  )
		MSG_WM_RBUTTONUP	(OnEngineRButtonUp)
		MSG_WM_SETFOCUS 	(OnEngineSetFocus )
		MSG_WM_KILLFOCUS	(OnEngineKillFocus)
		MESSAGE_HANDLER 	(WM_CTLCOLORLISTBOX, OnCtlColor 	  )
	ALT_MSG_MAP(3)	// KeywordComboBox
		MESSAGE_HANDLER 	(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	END_MSG_MAP()


	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled*/);

	void	OnCommand(UINT uFlag, int nID, HWND hWndCtrl);
	void	_OnCommand_SearchWeb(CString &str);
	void	_OnCommand_SearchHilight(CString &str);
	void	_OnCommand_SearchPage(BOOL bForward, int no=-1);
	void	SearchWeb_str_engine(CString &str, const CString&  strSearchEng);
	void	OnToolBarRButtonUp(UINT nFlags, const CPoint& point);

	CString GetSkinSeachBarPath(BOOL bHot);

	//スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
	void	_SearchThread();

	void	checkToolBarWords();							//+++
	bool	toolBarAddButtons(bool chk = false);					//+++
	int		addDefaultToolBarIcon(unsigned n);				//+++
	int		addDefaultToolBarIcon_tinyWordButton(unsigned n);	//+++
	void	toolBarAddWordTbl(std::vector<CString>& tbl);	//+++

	int		btnWidth() const;								//+++ 検索入力窓の右のボタンの横幅.

	// KeywordEdit
	void	OnEngineKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnKeywordKillFocus(CWindow wndFocus);

	LRESULT OnCtlColorListBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void	OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnKeywordCbnDropDown(LPNMHDR pnmh);
	BOOL	DeleteKeywordHistory();

	void	OnEngineRButtonUp(UINT nFlags, CPoint point);
	void	OnEngineSetFocus(HWND hWndBefore);
	void	OnEngineKillFocus(HWND hWndNew);
	void	OnMouseMove(UINT nFlags, const CPoint& pt);

  #if 0
	void	OnSelDblClkForEngine(UINT code, int id, HWND hWnd); 	///+++ エンジン選択の改造実験... 失敗on_.
  #endif

	///+++ エンジン選択の改造実験... 失敗on_...だけど、ちょっとだけ有用.
	void	OnSelChangeForEngine(UINT code, int id, HWND hWnd);

	void	OnSelChange(UINT code, int id, HWND hWnd);
	void	OnLButtonDown(UINT nFlags, const CPoint& pt);
	void	OnLButtonUp(UINT nFlags, const CPoint& pt);

	void	UpdateLayout(const CPoint& pt);
	void	UpdateLayout2(const CPoint& pt);

	void	_OnEnterKeyDown(int flag);

	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	//x void	OnItemSelected(const CString& str, const CString& strEngine);		//+++ strEngine 追加

	void		OpenSearchGroup(const CString& strWord, const CString& strSearchEng, int &nLoopCnt, BOOL &bFirst);
	CString 	GetShortcutWord(const CString& strSearchEng);

	static void _InitialEngine(LPVOID lpV);
	static void _InitialKeyword(LPVOID lpV);

	int 		ResizeBar(int difA, int difB);						//+++

	void		SaveHistory();

	bool		checkEngineNameEnable();					//+++

	bool 		OnSearchEngineMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);	//+++
	void 		OnSearchWeb_engineId(UINT code, int id, HWND hWnd);		//+++

#if SEARCH_JIKKEN == 1	//+++
private:
	LRESULT 	OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled);
	CMenuHandle PrepareChevronMenu();
	void		CleanupChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm);
	void		DisplayChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm);

#elif SEARCH_JIKKEN == 2	//+++
public:
	LRESULT 	OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL & bHandled); 	//+++
	HMENU		ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy);	//+++
	void		Chevronhandler_OnCleanupChevronMenu();		//+++

private:
	HMENU		_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu);
#endif

private:
	typedef CDonutSearchBar 	thisClass;

	// Constants
	enum {
		STB_SHOWTOOLBARICON 	= 0x00000001,
	};

	enum EEnt_Read {
		ENT_READ_ENG			=	1,
		ENT_READ_HIST			=	2,
	};


	enum EEnter_KeyDown {
		ENTER_KEYDOWN_RETURN	=	0,
		ENTER_KEYDOWN_SELCHANGE =	1,
	};


	enum ECharEncode {
		ENCODE_NONE 			=	0,
		ENCODE_SHIFT_JIS		=	1,
		ENCODE_EUC				=	2,
		ENCODE_UTF8 			=	3,
	};

	//enum { BTN_W    = 21 };	//+++

	enum {
		m_nBarCX	=  2,
		m_nBmpCX	= 16,
		m_nBmpCY	= 16,
		s_kcxGap	=  2
	};

	enum {
		m_nDefEditT =  1,
		//+++ m_nDefDlgH	= 22
	};


private:
	void		_InitCombo();
	void		_InitToolBar(int cx, int cy, COLORREF clrMask, UINT nFlags = ILC_COLOR24);
	void		_RefreshBandInfo(int nHeight);
	void		_SetVerticalItemCount(CComboBox &cmb, int count);
	bool 		_MakeSearchEngineMenu(CMenu& menu0);			//+++
	void		_ShortcutSearch(CString &strWord, CString &strSearchEng);
	void		_EncodeString(CString &str, int dwEncode);	//minit
	void		_DrawDragEffect(bool bRemove);


	// Data members
	CFlatComboBox_SearchKeyword		m_cmbKeyword;	// 検索文字列を入れるコンボボックス
	// CFlatComboBox m_cmbKeyword;
	CFlatComboBox	 m_cmbEngine;
	CToolBarCtrl	 m_wndToolBar;

	HCURSOR 		 m_hCursor;
	CPoint			 m_ptDragHist;
	CPoint			 m_ptDragStart;
	CPoint			 m_ptDragEnd;

	CContainedWindow m_wndKeyword;		// Edit in ComboBox
	CContainedWindow m_wndEngine;		// SearchEnginComboBox
	CContainedWindow m_wndKeywordCombo;	// KeywordComboBox

	HWND			 m_hWndKeywordList;

	CString			 m_strKeyword;					//+++ キーワードのコピー	 ※ルート的に最新の値でない場合あり
	CString			 m_strEngine;					//+++ サーチエンジン名コピー ※ルート的に最新の値でない場合あり
	CString 		 m_toolBarAddWordStr;			//+++ 検索単語が新たに変更されたかのチェック用

	int 			 m_cxBtnsBtn;					///< 旧 横幅調整用
	int 			 m_clrMask;
	int 			 m_has; 						//+++
	int 			 m_nKeywordWidth;				//+++
	int 			 m_nEngineWidth;				//+++

	CSize			 m_ButtonSize;					// ツールバーボタンの大きさ

	DWORD			 m_dwTinyWordButton;			//+++ 単語ボタンでなく5つの数字ボタンを使う場合

	BYTE/*bool*/	 m_bDragAccept;
	BYTE/*bool*/	 m_bDragFromItself;
	BYTE/*BOOL*/	 m_bShowToolBar;
	BYTE/*BOOL*/	 m_bLoadedToolBar;

	BYTE/*BOOL*/	 m_bExistManifest;				//+++ .exe.manifest ファイルの有無(XP Theme使用の有無)
	BYTE/*BOOL*/	 m_bHilightSw;					//+++ ハイライトボタンのon/off状態
	//int			 m_nHilightBtnNo;				//+++
	boost::thread	 m_tdInitComboBox;
	SearchPostData	 m_PostData;

	//+++ 1つしかインスタンスが作られない、だろうとして、ParseInternetShortcutFile()用に手抜きな自分を指すポインタ.
	static CDonutSearchBar* s_pThis_;

};



#endif	// DONUTSEARCHBAR_H
