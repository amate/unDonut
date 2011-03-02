/**
 *	@file	IniFile.h
 *	@brief	iniファイルの読み書き.
 *	@note
 *		MtlPrivateProfile.h を元に作り変えたモノ
 */

#ifndef INIFILE_H
#define INIFILE_H

#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#if _ATL_VER >= 0x700
#include <atlsync.h>
#define INISECTION_USE_MUTEX		1		// 0 1 2
#else	//+++ atl3で無理やりコンパイルしてみるテスト用.
#define INISECTION_USE_MUTEX		0		// 0 1 2
#endif



///+++ 初期化ファイル(*.ini)の読みこみを行うためのクラス.  ※書き込みは これを継承した CIniFileIO で可能にする.
/// @note
///   注意点(minit) ファイル名はフルパスで与えないとファイルはWindowsディレクトリ辺りに生成される.
class CIniFileI {
public:
	CIniFileI(const CString &strFileName, const CString &strSectionName);
	//x ~CIniFileI() {;}

	virtual void Close();

	LONG	QueryValue(DWORD& dwValue, LPCTSTR lpszValueName);
	LONG	QueryValue(int&  nValue  , LPCTSTR lpszValueName) { return QueryValue(*(DWORD*)&nValue, lpszValueName); }
	//LONG	QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount);
	LONG	QueryString(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount);

	DWORD	GetValue(LPCTSTR lpszValueName, DWORD defalutValue=0);
	const CString GetString(LPCTSTR lpszValueName, const TCHAR* pszDefult=0, DWORD dwBufSize = 0);
	const CString GetStringUW(LPCTSTR lpszValueName, const TCHAR* pszDefult=0, DWORD dwBufSize = 0);

	///+++ セクション名の変更 (多少不細工だが、現状、重要なのはファイル名(の変更)だけなので、これはありにしておく)
	void	ChangeSectionName(LPCTSTR sectionName) { ATLASSERT(!m_strFileName.IsEmpty()); m_strSectionName = sectionName; }

protected:
	enum { cnt_nDefault = 0xABCD0123 }; // magic number
	BOOL	IsOpen() {	return !m_strFileName.IsEmpty(); }
	void	Open(const CString &strFileName, const CString &strSectionName);
	const CString ConvPathNameToAlNumName(const TCHAR* src);


protected:
	CString 	m_strFileName;
	CString 	m_strSectionName;

  #if INISECTION_USE_MUTEX == 2 //+++	APIなPrivateProfileを使ってるので無意味かも?だが、念のため mutex で排他制御を試してみる.
	CString 	m_mutexName;
	CMutex		m_mutex;
	CMutexLock* m_mutexLock;
  #endif
};



///+++ 初期化ファイル(*.ini)の読み書きを行うためのクラス.
class CIniFileIO : public CIniFileI {
public:
	CIniFileIO(const CString &strFileName, const CString &strSectionName = _T(""));
	~CIniFileIO() { Close();}

	virtual void Close();

	LONG	SetValue(DWORD dwValue, LPCTSTR lpszValueName);
//	LONG	SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName) { return SetString(lpszValue, lpszValueName); }
	LONG	SetString(LPCTSTR lpszValue, LPCTSTR lpszValueName);
	LONG	SetStringUW(LPCTSTR lpszValue, LPCTSTR lpszValueName);
	LONG	DeleteValue(LPCTSTR lpszValueName);

	//+++ セクション消去してから書く場合用.
	bool	DeleteSection();

	//+++ iniファイルを.bakにして削除したことにする.
	void	RemoveFileToBak();

	//x void ForceWriteFile(const CString str); 	//+++ 企画倒れ

private:
  #if INISECTION_USE_MUTEX == 1 //+++	APIなPrivateProfileを使ってるので無意味かも?だが、念のため mutex で排他制御を試してみる.
	CString 	m_mutexName;
	CMutex		m_mutex;
	CMutexLock* m_mutexLock;
  #endif
};



#ifdef NDEBUG
typedef CIniFileIO	CIniFileO;
#else
///+++ 初期化ファイル(*.ini)への書き込みを行うクラス.
class CIniFileO : protected CIniFileIO {
public:
	CIniFileO(const CString &strFileName, const CString &strSectionName = _T("")) : CIniFileIO(strFileName, strSectionName) {;}
	~CIniFileO() {;}

	virtual void Close() { CIniFileIO::Close(); }

	LONG	SetValue(DWORD dwValue, LPCTSTR lpszValueName) { return CIniFileIO::SetValue(dwValue, lpszValueName); }
	//LONG	SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName) { return CIniFileIO::SetValue(lpszValue, lpszValueName); }
	LONG	SetString(LPCTSTR lpszValue, LPCTSTR lpszValueName) { return CIniFileIO::SetString(lpszValue, lpszValueName); }
	LONG	SetStringUW(LPCTSTR lpszValue, LPCTSTR lpszValueName) { return CIniFileIO::SetStringUW(lpszValue, lpszValueName); }
	LONG	DeleteValue(LPCTSTR lpszValueName) { return CIniFileIO::DeleteValue(lpszValueName); }
	void	ChangeSectionName(LPCTSTR sectionName) { CIniFileI::ChangeSectionName(sectionName); }

	//+++ セクション消去してから書く場合用.
	bool	DeleteSection() { return CIniFileIO::DeleteSection(); }

	//+++ iniファイルを.bakにして削除したことにする.
	void	RemoveFileToBak() { CIniFileIO::RemoveFileToBak(); }
};
#endif



#endif
