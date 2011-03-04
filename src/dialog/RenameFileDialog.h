/**
 *	@file	RenameFileDialog.h
 *	@brief	ファイル名変更ダイアログ
 */

#pragma once

#include "../resource.h"

class CDonutRenameFileDialog
	: public CDialogImpl<CDonutRenameFileDialog>
	, public CWinDataExchange<CDonutRenameFileDialog>
{
public:
	enum { IDD = IDD_DIALOG_RENAMEFILE };

	// Data members
	CString 	m_strName;

public:
	// DDX map
	BEGIN_DDX_MAP(CDonutRenameFileDialog)
		DDX_TEXT_LEN ( IDC_EDIT_RENAMEFILE, m_strName, MAX_PATH )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CDonutRenameFileDialog ) 		//+++
		MESSAGE_HANDLER 	( WM_INITDIALOG , OnInitDialog	)
		COMMAND_ID_HANDLER	( IDOK			, OnOkCmd		)
		COMMAND_ID_HANDLER	( IDCANCEL		, OnCloseCmd	)
	END_MSG_MAP()

private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnOkCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

public:
	static BOOL IsContainBadCharacterForName(CString &strFileName);
	static void ReplaceBadCharacterForName(CString &strFileName);
};

