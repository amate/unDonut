/**
 *	@file	DonutConfirmOption.h
 *	@brief	donutオプション: 確認ダイアログに関する設定プロパティページ. 終了時にユーザーへ確認を出す処理を含む.
 *	@note
 *			終了時にユーザーへ確認を出す処理を含みます。
 *			終了時に確認を行うCDonutConfirmOptionとその設定を行う
 *			プロパティページCDonutConfirmPropertyPageを有します。
 */

#pragma once

#include "../resource.h"
#include "../MtlWin.h"



/**
	CDonutConfirmOption
	各種確認ダイアログを出すためのクラス
 */
class CDonutConfirmOption {
	friend class CDonutConfirmPropertyPage;

private:
	//確認ダイアログの設定フラグの定数
	enum EDonut_Confirm {
		DONUT_CONFIRM_EXIT			 = 0x00000001L,
		DONUT_CONFIRM_CLOSEALL		 = 0x00000002L,
		DONUT_CONFIRM_CLOSEALLEXCEPT = 0x00000004L,
		DONUT_CONFIRM_STOPSCRIPT	 = 0x00000008L,
		DONUT_CONFIRM_CLOSELEFTRIGHT = 0x00000010L,
	};

	//メンバ変数
	static DWORD	s_dwFlags;
	//x static BOOL s_bStopScript;
	static DWORD	s_dwStopScript;

public:
	//メンバ関数
	static void 	GetProfile();
	static void 	WriteProfile();

	static bool 	OnDonutExit(HWND hWnd = NULL);
	static bool 	OnCloseAll(HWND hWnd = NULL);
	static bool 	OnCloseAllExcept(HWND hWnd = NULL);
	static bool		OnCloseLeftRight(HWND hWnd = NULL, bool bLeft = false);

	static BOOL 	WhetherConfirmScript();

private:
	static bool 	_SearchDownloadingDialog();

	//内部で使用する構造体
	struct _Function_Searcher {
		bool	m_bFound;

		_Function_Searcher() : m_bFound(false) { }

		bool operator ()(HWND hWnd)
		{
			if ( MtlIsWindowCurrentProcess(hWnd) ) {
				CString strCaption = MtlGetWindowText(hWnd);

				if ( (strCaption.Find( _T('%') ) != -1 && strCaption.Find( _T("完了しました") ) != -1)
				   || strCaption.Find( _T("ファイルのダウンロード") ) != -1 )
				{
					m_bFound = true;
					return false;
				}
			}

			return true; // continue finding
		}
	};

};


/**
	CDonutConfirmPropertyPage
	確認ダイアログに関する設定プロパティページ
 */
class CDonutConfirmPropertyPage
	: public CPropertyPageImpl< CDonutConfirmPropertyPage >
	, public CWinDataExchange < CDonutConfirmPropertyPage >
{
public:
	//ダイアログリソースID
	enum { IDD = IDD_PROPPAGE_CONFIRMDLG };

private:
	//メンバ変数
	int 	m_nExit;
	int 	m_nCloseAll;
	int 	m_nCloseAllExcept;
	int 	m_nStopScript;
	int 	m_nCloseLeftRight;

public:
	//DDXマップ
	BEGIN_DDX_MAP(CDonutConfirmPropertyPage)
		DDX_CHECK( IDC_CHECK_CFD_EXIT,			m_nExit 		 )
		DDX_CHECK( IDC_CHECK_CFD_CLOSEALL,		m_nCloseAll 	 )
		DDX_CHECK( IDC_CHECK_CFD_CLOSEEXCEPT,	m_nCloseAllExcept)
		DDX_CHECK( IDC_CHECK_CFD_STOPSCRIPT,	m_nStopScript	 )
		DDX_CHECK( IDC_CHECK_CFD_CLOSELEFTRIGHT,m_nCloseLeftRight)
	END_DDX_MAP()

	//コンストラクタ
	CDonutConfirmPropertyPage();

	//プロパティページのオーバーライド関数
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	//内部関数
private:
	void	_GetData();
	void	_SetData();
};


