/**
 *	@file AlphaDialog.cpp
 *	@brief	透明度設定ダイアログ
 */

#include "stdafx.h"
#include "AlphaDialog.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CAlphaDialog::CAlphaDialog(BYTE bytDefAlpha)
	: m_dwAlpha(bytDefAlpha)
{
}



BYTE CAlphaDialog::GetAlpha()
{
	return (BYTE) m_dwAlpha;
}



LRESULT CAlphaDialog::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	// スライダー設定
	m_wndTrack = GetDlgItem(IDC_SLIDER);
	m_wndTrack.SetRange(16, 255);
	m_wndTrack.SetTicFreq(1);
	m_wndTrack.SetPageSize(16);
	m_wndTrack.SetLineSize(1);
	m_wndTrack.SetPos(m_dwAlpha);
	DoDataExchange(FALSE);
	::SendMessage(GetDlgItem(IDC_EDIT), EM_LIMITTEXT, 3, 0);
	return 0;
}



void CAlphaDialog::OnHScroll(int nCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl track = hWnd;
	CString 	  strPos;

	strPos.Format( _T("%d"), track.GetPos() );
	::SetWindowText(GetDlgItem(IDC_EDIT), strPos);
}



void CAlphaDialog::OnOK(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	if ( !DoDataExchange(TRUE) )
		return;

	EndDialog(nID);
}



void CAlphaDialog::OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	EndDialog(nID);
}



void CAlphaDialog::OnEditTextChange(UINT uNotifyCode, int nID, HWND hWndCtl)
{
	int 		  nTextNum = GetDlgItemInt(IDC_EDIT, NULL, FALSE);

	if (nTextNum > 255)
		return;

	CTrackBarCtrl track    = GetDlgItem(IDC_SLIDER);
	track.SetPos(nTextNum);
}
