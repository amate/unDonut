/**
 *	@file	OpenURLDialog.h
 *	@brief	"ファイルを開く"ダイアログ
 */

#pragma once

#include "../resource.h"

class COpenURLDlg : public CDialogImpl<COpenURLDlg>, public CWinDataExchange<COpenURLDlg> 
{
public:
	// Declarations
	enum { IDD = IDD_DIALOG_OPENURL };

	// Data members
	CString 	m_strEdit;

private:
	CEdit		m_edit;

public:
	// Constructor
	COpenURLDlg();


	// DDX map
	BEGIN_DDX_MAP(COpenURLDlg)
		DDX_TEXT_LEN ( IDC_EDIT_URL, m_strEdit, INTERNET_MAX_PATH_LENGTH )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( COpenURLDlg )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG , OnInitDialog )
		COMMAND_ID_HANDLER	 ( IDOK 		 , OnCloseCmd	)
		COMMAND_ID_HANDLER	 ( IDCANCEL 	 , OnCloseCmd	)
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_REF, OnRefCmd 	)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void	OnRefCmd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
};
