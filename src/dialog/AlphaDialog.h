/**
 *	@file	AlphaDialog.h
 *	@brief	透明度設定ダイアログ
 */
#pragma once

#include <atlddx.h>
#include <atlcrack.h>
#include "../resource.h"



class CAlphaDialog
	: public CDialogImpl<CAlphaDialog>
	, public CWinDataExchange<CAlphaDialog>
{
protected:
	DWORD			m_dwAlpha;
	CTrackBarCtrl	m_wndTrack;

public:
	// コンストラクタ
	CAlphaDialog(BYTE bytDefAlpha);

	// メソッド
	BYTE GetAlpha();

	enum { IDD = IDD_DIALOG_ALPHA };

	//　DDXマップ
	//x BEGIN_DDX_MAP( CMainDlg )	//*+++
	BEGIN_DDX_MAP( CAlphaDialog )
		DDX_UINT_RANGE( IDC_EDIT, m_dwAlpha, (DWORD) 16, (DWORD) 255 )
	END_DDX_MAP()

	// メッセージマップ
	//x BEGIN_MSG_MAP_EX( CMainDlg )	//*+++
	BEGIN_MSG_MAP_EX( CAlphaDialog )
		MSG_WM_INITDIALOG	 ( OnInitDialog )
		MSG_WM_HSCROLL		 ( OnHScroll )
		COMMAND_ID_HANDLER_EX( IDOK, OnOK )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_HANDLER_EX	 ( IDC_EDIT, EN_CHANGE, OnEditTextChange )
	END_MSG_MAP()

private:
	//ハンドラ
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void	OnHScroll(int nCode, short nPos, HWND hWnd);
	void	OnOK(UINT uNotifyCode, int nID, HWND hWndCtl);
	void	OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl);
	void	OnEditTextChange(UINT uNotifyCode, int nID, HWND hWndCtl);

};

