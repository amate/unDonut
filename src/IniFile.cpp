/**
 *	@file	IniFile.cpp
 *	@brief	iniファイルの読み書き.
 *	@note
 *		MtlPrivate.cpp を元に作り変えたモノ
 */

#include "stdafx.h"
#include "IniFile.h"
//#include "DonutPFunc.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




void CIniFileI::Close()
{
	m_strFileName.Empty();
	m_strSectionName.Empty();

  #if INISECTION_USE_MUTEX == 2
	delete		m_mutexLock;
	m_mutexLock = NULL;
  #endif
}



CIniFileI::CIniFileI(const CString &strFileName, const CString &strSectionName)
  #if INISECTION_USE_MUTEX == 2
	// : m_mutexName( DONUT_NAME _T("_IniFile_") + GetFileBaseName( strFileName ) )
	: m_mutexName( CIniFileI::ConvPathNameToAlNumName(strFileName) )
	, m_mutex( NULL, false, m_mutexName )
	, m_mutexLock( new CMutexLock(m_mutex) )
  #endif
{
	Open(strFileName, strSectionName);
}



void CIniFileI::Open(const CString &strFileName, const CString &strSectionName)
{
	ATLASSERT( !strFileName.IsEmpty() );
	ATLASSERT( !IsOpen() );
	m_strFileName	 = strFileName;
	m_strSectionName = strSectionName;
}



///+++ メモ： ValueNameの持つ値を返す. エラーの時、dwValueの値は元のまま.
LONG CIniFileI::QueryValue(DWORD &dwValue, LPCTSTR lpszValueName)
{
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );

	UINT nValue = ::GetPrivateProfileInt(m_strSectionName, lpszValueName, cnt_nDefault, m_strFileName);
	if (nValue == cnt_nDefault)
		return ERROR_CANTREAD;

	dwValue = nValue;
	return ERROR_SUCCESS;
}



#if 1
//+++ LONG CIniFileI::QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount)
LONG CIniFileI::QueryString(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount)
{
	ATLASSERT(pdwCount != NULL);
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );
	ATLASSERT(szValue != NULL); 		// Ini section can't determine dwCount.

	DWORD dw = ::GetPrivateProfileString(m_strSectionName, lpszValueName, _T(""), szValue, *pdwCount, m_strFileName);
	if (dw == 0)
		return ERROR_CANTREAD;

	return ERROR_SUCCESS;
}
#endif



#if 1	//+++ エラーを返す意味がない場合も多いので、エラー時用デフォルト指定ありで、値そのものを返すバージョンを用意.
DWORD CIniFileI::GetValue(LPCTSTR lpszValueName, DWORD defaultValue)
{
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );
	return ::GetPrivateProfileInt(m_strSectionName, lpszValueName, defaultValue, m_strFileName);
}
#endif

int	CIniFileI::GetValuei(LPCTSTR lpszValueName, int defaultValue/* = 0*/)
{
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );
	return (int)::GetPrivateProfileInt(m_strSectionName, lpszValueName, defaultValue, m_strFileName);
}


#if 1	//+++ エラーを返す意味がない場合も多いので、エラー時用デフォルト指定ありで、値そのものを返すバージョンを用意.

const CString CIniFileI::GetString(LPCTSTR lpszValueName, const TCHAR* pszDefult, DWORD dwBufSize)
{
	enum { SIZE = MAX_PATH > 0x4000 ? MAX_PATH : 0x4000 };		//+++ 手抜きで固定サイズ.
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );

	if (dwBufSize != 0xFFFFFFFF) {
		ATLASSERT( dwBufSize <= SIZE );
		TCHAR	buf[ SIZE ];
		if (dwBufSize == 0)
			dwBufSize  = SIZE;
		buf[0] = 0;		//x memset(buf, 0, sizeof buf);
		DWORD dw = ::GetPrivateProfileString(m_strSectionName, lpszValueName, _T(""), buf, dwBufSize, m_strFileName);
		if (dw == 0 && pszDefult)
			return CString(pszDefult);
		buf[dw] = '\0';
		return CString(buf);
	} else {
		DWORD	dwBufSize	= SIZE;
		DWORD	dw			= 0;
		TCHAR*	buf 		= _T("");
		for (;;) {
			buf = new TCHAR[ dwBufSize ];
			dw	= ::GetPrivateProfileString(m_strSectionName, lpszValueName, _T(""), buf, dwBufSize, m_strFileName);
			if (dw < dwBufSize - 1)
				break;
			delete[] buf;
			dwBufSize <<= 1;
		}
		if (dw == 0 && pszDefult) {
			delete[]	buf;
			return CString(pszDefult);
		}
		buf[dw] = '\0';
		CString 	str(buf);
		delete[]	buf;
		return str;
	}
}

#endif


///+++ 文字列取得。もし、文字列が半角英数記号のみで%??が混ざっている場合、utf8文字列をエンコードしたものとして、デコードして返す.
const CString CIniFileI::GetStringUW(LPCTSTR lpszValueName, const TCHAR* pszDefult, DWORD dwBufSize)
{
	return Misc::urlstr_decodeWhenASC(GetString(lpszValueName, pszDefult, dwBufSize ));
}



///+++ ファイルパス名中の記号を_に変えた名前にして返す. シフトJIS文字もそのままぶっ潰す. Mutexのキー文字列のために用意.
const CString CIniFileI::ConvPathNameToAlNumName(const TCHAR* src)
{
	//+++ シフトJIS前提.
	CString dst( src );
	unsigned l = dst.GetLength();
	for (unsigned i = 0; i < l; ++i) {
		unsigned c = dst.GetAt(i);
		if ( isalnum((unsigned char)c) || c == _T('_') ) {
			;
		} else {
			dst.SetAt(i, _T('_'));
		}
	}
	return dst;
}



// =================================================================
// 書き込み関係

///+++ mutexでガードしてみる
CIniFileIO::CIniFileIO(const CString &strFileName, const CString &strSectionName)
	: CIniFileI(strFileName, strSectionName)
  #if INISECTION_USE_MUTEX == 1
	, m_mutexName( CIniFileI::ConvPathNameToAlNumName(strFileName) )
	, m_mutex( NULL, false, m_mutexName )
	, m_mutexLock( new CMutexLock(m_mutex) )
  #endif
{
}

///+++ 閉じる
void CIniFileIO::Close()
{
	CIniFileI::Close();
  #if INISECTION_USE_MUTEX == 1
	delete		m_mutexLock;
	m_mutexLock = NULL;
  #endif
}



///+++ バックアップファイルを作って削除.
void CIniFileIO::RemoveFileToBak()
{
	ATLASSERT( IsOpen() );
  #if 1
	Misc::MoveToBackupFile( m_strFileName );
  #else
	CString		strFileName	= m_strFileName;
	CString 	strBakName  = strFileName + ".bak";
	if (::GetFileAttributes(strBakName) != 0xFFFFFFFF)
		::DeleteFile(strBakName);					// 古いバックアップファイルを削除.
	if (::GetFileAttributes(strFileName) != 0xFFFFFFFF)
		::MoveFile(strFileName, strBakName);		// 既存のファイルをバックアップファイルにする.
  #endif
}



LONG CIniFileIO::SetValue(DWORD dwValue, LPCTSTR lpszValueName)
{
	ATLASSERT(lpszValueName != NULL);
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );

	TCHAR 	szT[32];
	::wsprintf(szT, _T("%d"), dwValue);

	if ( ::WritePrivateProfileString(m_strSectionName, lpszValueName, szT, m_strFileName) )
		return ERROR_SUCCESS;
	else
		return ERROR_CANTWRITE;
}



LONG CIniFileIO::SetString(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	ATLASSERT(lpszValue != NULL);
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );

	ATLASSERT(m_strFileName.GetLength() < 4095);

	if ( ::WritePrivateProfileString(m_strSectionName, lpszValueName, lpszValue, m_strFileName) )
		return ERROR_SUCCESS;
	else
		return ERROR_CANTWRITE;
}



///+++ 文字列を書き込む. 文字列中にSJISにできないUNICODE文字が混ざっていたら%??形式に変換してから書き込む.
LONG CIniFileIO::SetStringUW(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	return SetString( Misc::urlstr_encodeWhenNeedUnicode(lpszValue),lpszValueName );
}



LONG CIniFileIO::DeleteValue(LPCTSTR lpszValueName)
{
	ATLASSERT(lpszValueName != NULL);
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );
	if ( ::WritePrivateProfileString(m_strSectionName, lpszValueName, NULL, m_strFileName) )
		return ERROR_SUCCESS;
	else
		return ERROR_CANTWRITE;
}



///+++ Iniファイル中のセクションの削除.
bool CIniFileIO::DeleteSection()
{
	ATLASSERT( IsOpen() );
	ATLASSERT( !m_strSectionName.IsEmpty() );

	return ::WritePrivateProfileString(m_strSectionName, NULL, NULL, m_strFileName) != 0;
}



#if 0
///+++ 単純に .ini でなくただのファイルとして文字列を書き込む
void CIniFileIO::ForceWriteFile(const CString str)
{
	FILE* fp = fopen(m_strFileName, "wt");
	if (fp == NULL)
		return;
	//x size_t sz = str.GetLength();
	fputs( str, fp);
	fclose(fp);
}
#endif




