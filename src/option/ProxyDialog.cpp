/**
 *	@file	ProxyDialog.h
 *	@brief	donutのオプション : プロキシ
 */

#include "stdafx.h"
#include "ProxyDialog.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace MTL;



// Constructor
CProxyPropertyPage::CProxyPropertyPage()
{
	m_nRandTimeMin = 5;
	m_nRandTimeSec = 0;
	m_nRandChk	   = 0;
	m_nLocalChk    = 0;
	m_nUseIE	   = 1;
}



// Overrides
BOOL CProxyPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_editPrx.m_hWnd == NULL) {
		m_editPrx.Attach( GetDlgItem(IDC_EDIT1) );
	}

	if (m_editNoPrx.m_hWnd == NULL) {
		m_editNoPrx.Attach( GetDlgItem(IDC_EDIT2) );
	}

	if (m_editPrx.m_hWnd && m_editNoPrx.m_hWnd) {
		//+++ きっと r13test10のほうが正しいと思われる
	  #if 1 //+++ メモ: r13test10での処理.
		_SetData();
	  #else //+++ メモ：undonut+ での処理.... OnApplyでも_GetData()で、_SetDataしてる箇所がない...
		_GetData();
	  #endif
	}

	return DoDataExchange(FALSE);
}



BOOL CProxyPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CProxyPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else {
		return FALSE;
	}
}



void CProxyPropertyPage::_SetData()
{
	CString 	strFile = _GetFilePath( _T("Proxy.ini") );

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// プロキシ
	{
		CIniFileI	pr( strFile, _T("PROXY") );
		DWORD		dwLineCnt = 0;
		pr.QueryValue( dwLineCnt, _T("MAX") );
		m_editPrx.SetSelAll();

		for (int ii = 0; ii < (int) dwLineCnt; ++ii) {
			CString strKey;
			strKey.Format(_T("%d"), ii);
			CString 	strProxy = pr.GetString( strKey );		//*+++ ここをGetStringUWにするかは後で.
			if ( strProxy.IsEmpty() )
				continue;

			m_editPrx.ReplaceSel( LPCTSTR(strProxy) );
			m_editPrx.ReplaceSel( _T("\r\n") );
		}
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ランダム
	{
		CIniFileI	pr( strFile, _T("RAND") );

		DWORD		dwRandChk	  = 0;
		pr.QueryValue( dwRandChk, STR_ENABLE );
		m_nRandChk	   = dwRandChk;

		DWORD		dwRandTimeMin = 5;
		pr.QueryValue( dwRandTimeMin, _T("Min") );
		m_nRandTimeMin = dwRandTimeMin;

		DWORD		dwRandTimeSec = 0;
		pr.QueryValue( dwRandTimeSec, _T("Sec") );
		m_nRandTimeSec = dwRandTimeSec;
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// 非プロキシ
	{
		CIniFileI		pr( strFile, _T("NOPROXY") );
		DWORD			dwLineCnt	   = 0;
		pr.QueryValue( dwLineCnt, _T("MAX") );
		m_editNoPrx.SetSelAll();

		for (int ii = 0; ii < (int) dwLineCnt; ++ii) {
			CString 	strKey;
			strKey.Format(_T("%d"), ii);

			CString strProxy = pr.GetString( strKey );		//*+++ ここをGetStringUWにするかは後で.
			if ( strProxy.IsEmpty() )
				continue;

			m_editNoPrx.ReplaceSel( LPCTSTR(strProxy) );
			m_editNoPrx.ReplaceSel( _T("\r\n") );
		}
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ローカル
	{
		CIniFileI	pr( strFile, _T("LOCAL") );
		DWORD		dwLocalChk = 0;
		pr.QueryValue( dwLocalChk, STR_ENABLE );
		m_nLocalChk = dwLocalChk;
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ローカル
	{
		CIniFileI	pr( strFile, _T("USE_IE") );
		DWORD		dwUseIE    = 1;
		pr.QueryValue( dwUseIE, STR_ENABLE );
		m_nUseIE	= dwUseIE;
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}



void CProxyPropertyPage::_GetData()
{
	CString 	strFile;

	strFile = _GetFilePath( _T("Proxy.ini") );

  #if 1 	//+++ メモ: r13test10に対し undonut+ で増えた処理...
   #if 1	//+++ 念のためバックアップファイルにする.
	Misc::MoveToBackupFile(strFile);
   #else
	if (GetFileAttributes(strFile) != 0xFFFFFFFF)	//Proxy.iniが存在していれば削除
		DeleteFile(strFile);
   #endif
  #endif

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// プロキシ
	{
		CIniFileO	pr( strFile, _T("PROXY") );
		int 		nLineCnt = m_editPrx.GetLineCount();
		pr.SetValue( (DWORD) nLineCnt, _T("MAX") );

		for (int ii = 0; ii < nLineCnt; ++ii) {
			CString strKey;
			strKey.Format(_T("%d"), ii);

			TCHAR	cBuff[MAX_PATH];
			memset(cBuff, 0, MAX_PATH);
			int 	nTextSize = m_editPrx.GetLine(ii, cBuff, MAX_PATH);

		  #if 1 //+++ メモ: r13test10に対し undonut+ で増えた処理.
			if (cBuff[0] == '\0')
				break;									//何もなかったら終了
		  #endif

			cBuff[nTextSize] = '\0';

			CString strBuff(cBuff);
			pr.SetString(strBuff, strKey);
		}

		//x pr.Close(); 	//+++
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ランダム
	{
		CIniFileO	pr( strFile, _T("RAND") );
		pr.SetValue( (DWORD) m_nRandChk, STR_ENABLE );
		pr.SetValue( (DWORD) m_nRandTimeMin, _T("Min") );
		pr.SetValue( (DWORD) m_nRandTimeSec, _T("Sec") );
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// 非プロキシ
	{
		CIniFileO	pr( strFile, _T("NOPROXY") );
		int 		nLineCnt = m_editNoPrx.GetLineCount();
		pr.SetValue( (DWORD) nLineCnt, _T("MAX") );

		for (int ii = 0; ii < nLineCnt; ++ii) {
			CString strKey;
			strKey.Format(_T("%d"), ii);

			TCHAR	cBuff[MAX_PATH];
			memset(cBuff, 0, MAX_PATH);
			int 	nTextSize = m_editNoPrx.GetLine(ii, cBuff, MAX_PATH);

		  #if 1 //+++ メモ: r13test10に対し undonut+ で増えた処理.
			if (cBuff[0] == _T('\0'))
				break;									//何もなかったら終了
		  #endif

			cBuff[nTextSize] = _T('\0');

			CString strBuff(cBuff);
		  #if 1 //+++ メモ: undonut+ でコメントアウトされた部分.
			//strBuff += _T("\n");
		  #endif
			pr.SetString(strBuff, strKey);
		}
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// ローカル
	{
		CIniFileO	pr( strFile, _T("LOCAL") );
		pr.SetValue( (DWORD) m_nLocalChk, STR_ENABLE );
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// IE
	{
		CIniFileO	pr( strFile, _T("USE_IE") );
		pr.SetValue( (DWORD) m_nUseIE, STR_ENABLE );
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}
