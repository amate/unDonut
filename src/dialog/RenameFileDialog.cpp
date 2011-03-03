/**
 *	@file	RenameFileDialog.cpp
 *	@brief	ファイル名変更ダイアログ
 */
#include "stdafx.h"
#include "RenameFileDialog.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define RENAMEFILE_BADCHARACTERS	_T("\\:/*?\"<>|")




LRESULT CDonutRenameFileDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);
	return TRUE;
}



LRESULT CDonutRenameFileDialog::OnOkCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strBuf = m_strName;

	DoDataExchange(TRUE);

	if ( IsContainBadCharacterForName(m_strName) ) {
		MessageBox(_T("ファイル名に使用できない文字が使用されています。"), _T("エラー"), MB_OK);
		m_strName = strBuf;
		return 0;
	}

	EndDialog(wID);
	return 0;
}



LRESULT CDonutRenameFileDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}



BOOL CDonutRenameFileDialog::IsContainBadCharacterForName(CString &strFileName)
{
	return (strFileName.FindOneOf(RENAMEFILE_BADCHARACTERS) != -1) /*? TRUE : FALSE*/;
}



void CDonutRenameFileDialog::ReplaceBadCharacterForName(CString &strFileName)
{
	TCHAR	buf[2];
	buf[1] = _T('\0');

	CString strBadChar = RENAMEFILE_BADCHARACTERS;
	int 	nLen	   = strBadChar.GetLength();

	for (int i = 0; i < nLen; i++) {
		buf[0] = strBadChar[i];
		strFileName.Replace( buf, _T("") );
	}
}
