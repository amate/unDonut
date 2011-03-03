/**
 *	@file	AccelManager.cpp
 *	@brief	キーアクセラレータ(ショートカットキー)に関する処理を受け持つクラス群
 */

#include "stdafx.h"
#include "AccelManager.h"
#include "MtlMisc.h"
#include "IniFile.h"
#include "DonutPFunc.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//extern CServerAppModule _Module;

using namespace MTL;



////////////////////////////////////////////////////////////////////////////////
//CKeyHelperの定義
////////////////////////////////////////////////////////////////////////////////

CKeyHelper::CKeyHelper()
	:	m_lpAccel(NULL)
{
}



CKeyHelper::CKeyHelper(LPACCEL lpAccel)
	:	m_lpAccel(lpAccel)
{
}



CKeyHelper::~CKeyHelper()
{
}



/// 文字列にショートカットキーを表す文字列(ex. "Ctrl+F")を追加する
bool CKeyHelper::Format(CString &str) const
{
	str.Empty();

	if (m_lpAccel == NULL)
		return 0;

	if (m_lpAccel->fVirt & FCONTROL)
		AddVirtKeyStr(str, VK_CONTROL    , _T("ctrl")	);

	if (m_lpAccel->fVirt & FSHIFT)
		AddVirtKeyStr(str, VK_SHIFT      , _T("shift")	);

	if (m_lpAccel->fVirt & FALT)
		AddVirtKeyStr(str, VK_MENU       , _T("alt" )   );

	if (m_lpAccel->fVirt & FVIRTKEY) {
		return AddVirtKeyStr(str, m_lpAccel->key, NULL, TRUE);		//+++ 帰値を追加.
	} else {
		str += (TCHAR) m_lpAccel->key;	//+++ str += (char) m_lpAccel->key;
		return true;					//+++ 帰値を追加.
	}
}



/// 指定された仮想キーに基づき文字列を追加する
bool CKeyHelper::AddVirtKeyStr(CString &str, UINT uiVirtKey, LPCTSTR xname, BOOL bLast) const
{
	enum { BUFFER_LEN	= 260/*50*/ };				//+++ 気分でバッファ増やしとく
	TCHAR	szBuffer[BUFFER_LEN + 1];
	::ZeroMemory(szBuffer, sizeof szBuffer);		//+++

	HKL 	hKl		  = ::GetKeyboardLayout(0);		//+++ 一旦変数にいれる.
	// hKl=0;										//+++ 0でも正常に動作する模様...?
	UINT	nScanCode = ::MapVirtualKeyEx( uiVirtKey, 0, hKl );
	nScanCode		  = (nScanCode << 16) | 0x1;

	if (uiVirtKey >= VK_PRIOR && uiVirtKey <= VK_HELP) {
		nScanCode |= 0x01000000;		// 拡張キーボードのいくつかのキーを区別するためのフラグ
	}

	int		l		 = ::GetKeyNameText(nScanCode, szBuffer, BUFFER_LEN);
 #if 1
	if (l == 0) {
		if (xname) {
			_tcscpy(szBuffer, xname);
		} else if ((uiVirtKey >= 'A' && uiVirtKey <= 'Z') || isdigit(uiVirtKey) /* || ispunct(uiVirtKey)*/) {
			szBuffer[0] = uiVirtKey, szBuffer[1] = 0;
		} else {
			_tcscpy(szBuffer, _T("(キー文字列取得失敗)"));
		}
	}
 #endif
	CString 	strKey(szBuffer);

	//--------------------------------------
	// The first letter should be uppercase:
	//--------------------------------------
	if (l > 0) {
		strKey.MakeLower();
		for (int nCount = 0; nCount < strKey.GetLength(); nCount++) {
			TCHAR 	c = strKey[nCount];
			if ( IsCharLower(c) ) {
				// Convert single character JY 4-Dec-99
				c = (TCHAR) toupper(c);
				strKey.SetAt(nCount, c);
				break;
			}
		}
	}
	str += strKey;
	if (!bLast) {
		str += _T('+');
	}

	return (l > 0);
}



void CKeyHelper::SetAccelerator(LPACCEL lpAccel)
{
	m_lpAccel = lpAccel;
}



////////////////////////////////////////////////////////////////////////////////
//CAccelerManagerの定義
////////////////////////////////////////////////////////////////////////////////

CAccelerManager::CAccelerManager(HACCEL hAccel)
{
	m_hAccel	 = hAccel;
	m_nAccelSize = ::CopyAcceleratorTable(m_hAccel, NULL, 0);
	m_lpAccel	 = new ACCEL[m_nAccelSize];
	::CopyAcceleratorTable(m_hAccel, m_lpAccel, m_nAccelSize);
}



CAccelerManager::~CAccelerManager()
{
	if (m_lpAccel) {
		delete[] m_lpAccel;
		m_lpAccel = NULL;		//+++ 念のためクリア.
	}
}



int CAccelerManager::GetCount() const
{
	return m_nAccelSize;
}



LPACCEL CAccelerManager::GetAt(int index)
{
	return &m_lpAccel[index];
}



/// 指定したコマンドIDを持つアクセラレータを探す
BOOL CAccelerManager::FindAccelerator(UINT uiCmd, CString &str)
{
	BOOL bFound = FALSE;

	for (int ii = 0; ii < m_nAccelSize; ii++) {
		if (m_lpAccel[ii].cmd == uiCmd) {
			CKeyHelper 	helper(&m_lpAccel[ii]);

			if (helper.Format(str))	//キーの組み合わせを文字列で返す	//+++ 文字列取得に成功したときのみにする.
				bFound = TRUE;
			break;
		}
	}

	return bFound;				//戻り値は見つかったかどうか
}



/// 指定したアクセラレータとキーの組み合わせが一致するものを探し、そのコマンドIDを返す
UINT CAccelerManager::FindCommandID(ACCEL *pAccel)
{
	UINT uiCmd = 0;

	for (int ii = 0; ii < m_nAccelSize; ii++) {
		if ( (pAccel->fVirt & FALT	  ) != (m_lpAccel[ii].fVirt & FALT	  ) )
			continue;

		if ( (pAccel->fVirt & FCONTROL) != (m_lpAccel[ii].fVirt & FCONTROL) )
			continue;

		if ( (pAccel->fVirt & FSHIFT  ) != (m_lpAccel[ii].fVirt & FSHIFT  ) )
			continue;

		if (pAccel->key != m_lpAccel[ii].key)
			continue;

		uiCmd = m_lpAccel[ii].cmd;
		break;
	}

	return uiCmd;
}



/// 指定したコマンドIDを持つアクセラレータを削除してテーブルを再編する
HACCEL CAccelerManager::DeleteAccelerator(UINT uCmd)
{
	ACCEL	   *lpAccel   = new ACCEL[m_nAccelSize - 1];

	BOOL		bFound	  = FALSE;
	int 		iNewIndex = 0;

	for (int ii = 0; ii < m_nAccelSize; ii++) {
		if (uCmd != m_lpAccel[ii].cmd)
			lpAccel[iNewIndex++] = m_lpAccel[ii];
		else
			bFound = TRUE;
	}

	if (bFound == FALSE) {
		delete[] lpAccel;
		return m_hAccel;
	} else {
		m_nAccelSize--;
		delete[] m_lpAccel;
		m_lpAccel = lpAccel;
		::DestroyAcceleratorTable(m_hAccel);
	}

	m_hAccel = ::CreateAcceleratorTable(m_lpAccel, m_nAccelSize);
	return m_hAccel;
}



HACCEL CAccelerManager::AddAccelerator(ACCEL *lpAccel)
{
	ACCEL	   *lpAccelList = new ACCEL[m_nAccelSize + 1];

	memcpy( lpAccelList, m_lpAccel, m_nAccelSize * sizeof (ACCEL) );
	lpAccelList[m_nAccelSize] = *lpAccel;

	m_nAccelSize++;
	delete[] m_lpAccel;
	m_lpAccel				  = lpAccelList;
	::DestroyAcceleratorTable(m_hAccel);
	m_hAccel				  = ::CreateAcceleratorTable(m_lpAccel, m_nAccelSize);
	return m_hAccel;
}



///+++ メモ:KeyBoard.iniよりキー定義を読み込む
HACCEL CAccelerManager::LoadAccelaratorState(HACCEL hAccel)
{
	CIniFileI	pr( _GetFilePath( _T("KeyBoard.ini") ), _T("KEYBOARD") );

	DWORD		dwMax = 0;
	pr.QueryValue( (DWORD) dwMax, _T("MAX") );
	m_nAccelSize = dwMax;

	if (m_nAccelSize == 0)
		return hAccel;

	delete[] m_lpAccel;
	m_lpAccel	 = new ACCEL[m_nAccelSize];

	for (int ii = 0; ii < m_nAccelSize; ii++) {
		CString 	strVirt, strKey, strCmd;

		strVirt.Format(_T("fVirt%02d"), ii);
		strKey.Format (_T("key%02d")  , ii);
		strCmd.Format (_T("cmd%02d")  , ii);

		DWORD	dwVirt = 0, dwKey = 0, dwCmd = 0;

		pr.QueryValue(dwVirt, strVirt);
		pr.QueryValue(dwKey , strKey );
		pr.QueryValue(dwCmd , strCmd );

		m_lpAccel[ii].fVirt = (BYTE) dwVirt;
		m_lpAccel[ii].key	= (WORD) dwKey;
		m_lpAccel[ii].cmd	= (WORD) dwCmd;
	}

	pr.Close();

	::DestroyAcceleratorTable(hAccel);			//外部から与えられたものを削除するって設計的にまずくないかなぁ(minit)
	m_hAccel	 = ::CreateAcceleratorTable(m_lpAccel, m_nAccelSize);
	return m_hAccel;
}



///+++ メモ:キー定義をKeyBoard.iniへ書き出す.
BOOL CAccelerManager::SaveAccelaratorState()
{
	CIniFileO	pr( _GetFilePath( _T("KeyBoard.ini") ), _T("KEYBOARD") );

	pr.SetValue( (DWORD) m_nAccelSize, _T("MAX") );

	for (int ii = 0; ii < m_nAccelSize; ii++) {
		CString strVirt, strKey, strCmd;

		strVirt.Format(_T("fVirt%02d"), ii);
		strKey.Format (_T("key%02d")  , ii);
		strCmd.Format (_T("cmd%02d")  , ii);

		pr.SetValue( (DWORD) m_lpAccel[ii].fVirt, strVirt );
		pr.SetValue( (DWORD) m_lpAccel[ii].key  , strKey  );
		pr.SetValue( (DWORD) m_lpAccel[ii].cmd  , strCmd  );
	}

	//x pr.Close(); //+++
	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////
//CEditAccelの定義
////////////////////////////////////////////////////////////////////////////////

CEditAccel::CEditAccel()
	:	m_Helper(&m_Accel)
{
	m_bKeyDefined = FALSE;
	ResetKey();
}



ACCEL const *CEditAccel::GetAccel() const
{
	return &m_Accel;
}



void CEditAccel::ResetKey()
{
	memset( &m_Accel, 0, sizeof (ACCEL) );
	m_bKeyDefined = FALSE;

	if (m_hWnd != NULL) {
		SetWindowText( _T("") );
	}
}



void CEditAccel::SetAccelFlag(BYTE bFlag, BOOL bOn)
{
	if (bOn)
		m_Accel.fVirt |= bFlag;
	else
		m_Accel.fVirt &= ~bFlag;
}



BOOL CEditAccel::OnTranslateAccelerator(MSG *pMsg)
{
	if (pMsg->hwnd != m_hWnd)
		return FALSE;

	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN) {
		SetFocus();
		return TRUE;
	}

	BOOL bPressed;
	if (   ( bPressed = (pMsg->message == WM_KEYDOWN   ) ) != 0 || pMsg->message == WM_KEYUP
		|| ( bPressed = (pMsg->message == WM_SYSKEYDOWN) ) != 0 || pMsg->message == WM_SYSKEYUP )
	{
		if ( bPressed && m_bKeyDefined && !( (1 << 30) & pMsg->lParam ) )
			ResetKey();

		if      (pMsg->wParam == VK_SHIFT   && !m_bKeyDefined)
			SetAccelFlag(FSHIFT  , bPressed);
		else if (pMsg->wParam == VK_CONTROL && !m_bKeyDefined)
			SetAccelFlag(FCONTROL, bPressed);
		else if (pMsg->wParam == VK_MENU    && !m_bKeyDefined)
			SetAccelFlag(FALT    , bPressed);
		else {
			if (!m_bKeyDefined) {
				m_Accel.key = (WORD) pMsg->wParam;

				if (bPressed) {
					m_bKeyDefined = TRUE;
					SetAccelFlag(FVIRTKEY, TRUE);
				}
			}
		}

		BOOL bDefaultProcess = FALSE;

		if ( (m_Accel.fVirt & FCONTROL) == 0 && (m_Accel.fVirt & FSHIFT) == 0 && (m_Accel.fVirt & FALT) == 0 &&
			(m_Accel.fVirt & FVIRTKEY) ) {
			switch (m_Accel.key) {
			case VK_ESCAPE:
			case VK_TAB:
			case VK_BACK:
				bDefaultProcess = TRUE;
			}
		}

		// if (!bDefaultProcess)
		{
			CString strKbd;

			m_Helper.Format(strKbd);

			SetWindowText(strKbd);
			return TRUE;
		}

		ResetKey();
	}

	return TRUE;

	//	return baseClass::PreTranslateMessage(pMsg);
}
