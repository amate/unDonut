/**
 *	DonutConfirmOption.cpp
 *	@brief	donutオプション: 確認ダイアログに関する設定プロパティページ. 終了時にユーザーへ確認を出す処理を含む.
 *	@note
 *			終了時にユーザーへ確認を出す処理を含みます。
 *			終了時に確認を行うCDonutConfirmOptionとその設定を行う
 *			プロパティページCDonutConfirmPropertyPageを有します。
 */

#include "stdafx.h"
#include "DonutConfirmOption.h"
#include "../IniFile.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////////////
//CDonutConfirmOptionの定義
////////////////////////////////////////////////////////////////////////////////

//static変数の定義
DWORD		CDonutConfirmOption::s_dwFlags		= 0/*DONUT_CONFIRM_EXIT | DONUT_CONFIRM_CLOSEALLEXCEPT*/;
DWORD		CDonutConfirmOption::s_dwStopScript = TRUE; 	//+++ sizeof(BOOL)=sizeof(DWORD)に依存しないように、BOOLだったのをDWORDに変更.



//メンバ関数
void CDonutConfirmOption::GetProfile()
{
	CIniFileI		pr( g_szIniFileName, _T("Confirmation") );
	pr.QueryValue( s_dwFlags, _T("Confirmation_Flags") );
	pr.QueryValue( /*(DWORD&)*/s_dwStopScript, _T("Script") );
	pr.Close();
}



void CDonutConfirmOption::WriteProfile()
{
	CIniFileO	pr( g_szIniFileName, _T("Confirmation") );
	pr.SetValue( s_dwFlags, _T("Confirmation_Flags") );
	pr.SetValue( s_dwStopScript, _T("Script") );
	pr.Close();
}



bool CDonutConfirmOption::OnDonutExit(HWND hWnd)
{
	if ( _SearchDownloadingDialog() ) {
		if ( IDYES == ::MessageBox(hWnd,
						_T("ダウンロード中のファイルがありますが、Donutを終了してもよろしいですか？"),
						_T("確認ダイアログ"), MB_YESNO | MB_ICONQUESTION ) )
			return true;
		else
			return false;
	}

	if ( !_check_flag(DONUT_CONFIRM_EXIT, s_dwFlags) )
		return true;

	// Note. On debug mode, If DONUT_CONFIRM_EXIT set, the process would be killed
	//		 before Module::Run returns. What can I do?
	if ( IDYES == ::MessageBox(hWnd, _T("Donutを終了してもよろしいですか？"),
								_T("確認ダイアログ"), MB_YESNO | MB_ICONQUESTION) ) {
		return true;
	}

	return false;
}



bool CDonutConfirmOption::OnCloseAll(HWND hWnd)
{
	if ( !_check_flag(DONUT_CONFIRM_CLOSEALL, s_dwFlags) )
		return true;

	if ( IDYES == ::MessageBox(hWnd, _T("ウィンドウをすべて閉じてもよろしいですか？"),
							   _T("確認ダイアログ"), MB_YESNO | MB_ICONQUESTION) )
		return true;

	return false;
}



bool CDonutConfirmOption::OnCloseAllExcept(HWND hWnd)
{
	if ( !_check_flag(DONUT_CONFIRM_CLOSEALLEXCEPT, s_dwFlags) )
		return true;

	if ( IDYES == ::MessageBox(hWnd, _T("これ以外のウィンドウをすべて閉じてもよろしいですか？"),
							   _T("確認ダイアログ"), MB_YESNO | MB_ICONQUESTION) )
		return true;

	return false;
}



bool CDonutConfirmOption::OnCloseLeftRight(HWND hWnd, bool bLeft)
{
	if ( !_check_flag(DONUT_CONFIRM_CLOSELEFTRIGHT, s_dwFlags) )
		return true;
	const TCHAR* pStr = (bLeft) ? _T("このタブより左側のタブをすべて閉じてもよろしいですか？")
								: _T("このタブより右側のタブをすべて閉じてもよろしいですか？") ;

	if ( IDYES == ::MessageBox(hWnd, pStr, _T("確認ダイアログ"), MB_YESNO | MB_ICONQUESTION) )
		return true;

	return false;
}



bool CDonutConfirmOption::_SearchDownloadingDialog()
{
	_Function_Searcher f;

	f = MtlForEachTopLevelWindow(_T("#32770"), NULL, f);

	return f.m_bFound;
}



BOOL CDonutConfirmOption::WhetherConfirmScript()
{
	if (s_dwStopScript)
		return TRUE;

	return FALSE;
}



////////////////////////////////////////////////////////////////////////////////
//CDonutConfirmPropertyPageの定義
////////////////////////////////////////////////////////////////////////////////

//コンストラクタ
CDonutConfirmPropertyPage::CDonutConfirmPropertyPage()
{
	_SetData();
}



//プロパティページのオーバーライド関数
BOOL CDonutConfirmPropertyPage::OnSetActive()
{
	SetModified(TRUE);
	return DoDataExchange(FALSE);
}



BOOL CDonutConfirmPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CDonutConfirmPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



//内部関数
void CDonutConfirmPropertyPage::_GetData()
{
	DWORD dwFlags = 0;

	if (m_nExit /*== 1*/)
		dwFlags |= CDonutConfirmOption::DONUT_CONFIRM_EXIT;

	if (m_nCloseAll /*== 1*/)
		dwFlags |= CDonutConfirmOption::DONUT_CONFIRM_CLOSEALL;

	if (m_nCloseAllExcept /*== 1*/)
		dwFlags |= CDonutConfirmOption::DONUT_CONFIRM_CLOSEALLEXCEPT;

	if (m_nCloseLeftRight)
		dwFlags |= CDonutConfirmOption::DONUT_CONFIRM_CLOSELEFTRIGHT;

	CDonutConfirmOption::s_dwStopScript = (m_nStopScript != 0);
	CDonutConfirmOption::s_dwFlags		= dwFlags;

	CDonutConfirmOption::WriteProfile();
}



void CDonutConfirmPropertyPage::_SetData()
{
	DWORD	dwFlags = CDonutConfirmOption::s_dwFlags;

	m_nExit 		  = _check_flag(CDonutConfirmOption::DONUT_CONFIRM_EXIT 		, dwFlags);
	m_nCloseAll 	  = _check_flag(CDonutConfirmOption::DONUT_CONFIRM_CLOSEALL 	, dwFlags);
	m_nCloseAllExcept = _check_flag(CDonutConfirmOption::DONUT_CONFIRM_CLOSEALLEXCEPT,dwFlags);
	m_nStopScript	  = CDonutConfirmOption::s_dwStopScript != 0;
	m_nCloseLeftRight = _check_flag(CDonutConfirmOption::DONUT_CONFIRM_CLOSELEFTRIGHT,dwFlags);
}
