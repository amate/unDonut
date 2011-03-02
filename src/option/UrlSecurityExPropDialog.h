/**
 *	@file	UrlSecurityExPropDialog.h
 *	@brief	Url別セキュリティの(拡張プロパティ)設定
 */

#pragma once

#include "../ExStyle.h"

enum EURLSECURITYPROPERTYEX {
	USP_USEREGEX	= 0x1,
	USP_ONLYTHISURL	= 0x2,
};

//////////////////////////////////////////////////////////////
// CExPropertyDialog2 : 拡張プロパティデータ設定用ダイアログ

class CExPropertyDialog2 : 
	public CDialogImpl<CExPropertyDialog2> ,
	public CWinDataExchange<CExPropertyDialog2> 
{
public:
	enum { IDD = IDD_DIALOG_EXPROPERTY2 };

	// Constructor
	CExPropertyDialog2(CString &strUrlFile, unsigned& rFlags, unsigned& rExopts, unsigned& rExopts2);

	//+++ 拡張プロパティ情報をボタン状態に変換.
	static int		exProp2btn(DWORD dwExProp, DWORD flag) { return (dwExProp & flag) / (flag / 3); }

	//+++ 拡張プロパティ情報を設定
	static DWORD	btn2ExProp(int stat, DWORD flag) {
	  #if 1
		if (stat == BST_UNCHECKED)
			return 0;					//+++ チェックされていない場合
		if (stat == BST_INDETERMINATE)
			return 2 * flag / 3;		//+++ デフォルト設定を用いる場合
		return flag / 3;				//+++ チェックされている場合
	  #else
		return stat * (flag / 3);
	  #endif
	}

	// DDX map
	BEGIN_DDX_MAP(CExPropertyDialogT<IDD_DLG> )
		DDX_CHECK(IDC_CHECK_DLACTIVEXCTLS	, m_nDLActiveX	)
		DDX_CHECK(IDC_CHECK_IMAGE			, m_nImage		)
		DDX_CHECK(IDC_CHECK_JAVA			, m_nJava		)
		DDX_CHECK(IDC_CHECK_NAVILOCK		, m_nNaviLock	)
		DDX_CHECK(IDC_CHECK_SCROLLBAR		, m_nFlatView	)	//+++
		DDX_CHECK(IDC_CHECK_RUNACTIVEXCTLS	, m_nRunActiveX )
		DDX_CHECK(IDC_CHECK_SCRIPTS 		, m_nScript 	)
		DDX_CHECK(IDC_CHECK_SOUND			, m_nSound		)
		DDX_CHECK(IDC_CHECK_VIDEO			, m_nVideo		)
		DDX_CHECK(IDC_CHK_BLOCK_MAILTO		, m_nBlockMailTo)
		DDX_CHECK(IDC_CHK_MOUSE_GESTURE 	, m_nGesture	)
		DDX_CHECK(IDC_CHK_MSG_FILTER		, m_nFilter 	)
		DDX_RADIO(IDC_RADIO_RELOAD_NONE 	, m_nReload 	)

		DDX_CHECK(IDC_CHK_URLSEC_REGEX		, m_bRegex		)
		DDX_CHECK(IDC_CHK_ONLYTHISURL		, m_bOnlyThisURL)
	END_DDX_MAP()


	BEGIN_MSG_MAP(CExPropertyDialogT<IDD_DLG> )
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK				, OnOK		)
		COMMAND_ID_HANDLER_EX(IDCANCEL			, OnCancel	)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DELETE , OnDelete	)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DEFAULT, OnDefault )
		COMMAND_ID_HANDLER_EX(IDC_BTN_DFLTBASE  , OnDfltBase )
		COMMAND_ID_HANDLER_EX(IDC_BTN_EXPROP_URL, OnBtnExpropUrl )
	END_MSG_MAP()


	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);

	void OnOK(UINT uNotifyCode, int nID, HWND hWndCtl);
	void OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl);

	void OnDelete(UINT uNotifyCode, int nID, HWND hWndCtl);
	void OnDefault(UINT uNotifyCode, int nID, HWND hWndCtl);
	void OnDfltBase(UINT uNotifyCode, int nID, HWND hWndCtl);

	void OnBtnExpropUrl(UINT uNotifyCode, int nID, HWND hWndCtl);

private:

	bool		m_bRegex;
	bool		m_bOnlyThisURL;

	//DWORD		m_dwExProp;		//+++ 未使用.
	int 		m_nDLActiveX;
	int 		m_nImage;
	int 		m_nJava;
	int 		m_nNaviLock;
	int 		m_nRunActiveX;
	int 		m_nScript;
	int 		m_nFlatView;	//+++ 名前変更.
	int 		m_nSound;
	int 		m_nVideo;
	int 		m_nBlockMailTo;
	int 		m_nGesture;
	int 		m_nFilter;
	int 		m_nReload;

	int			m_urlEditBtnSw;			//+++ url編集のon,off
	CString&	m_rRsltUrl;			//+++ エディットチェック用
	unsigned&	m_rRsltFlags;
	unsigned&	m_rRsltExopts;
	unsigned&	m_rRsltExopts2;		//+++

};

