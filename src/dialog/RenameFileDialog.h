/**
 *	@file	RenameFileDialog.h
 *	@brief	ファイル名変更ダイアログ
 */

#pragma once

#include "../resource.h"

/////////////////////////////////////////////////////////////////
/// ファイル名変更ダイアログ

class CRenameDialog : public CDialogImpl<CRenameDialog>
{
public:
	enum { IDD = IDD_RENAMEDIALOG };

	enum { WM_SELTEXTWITHOUTEXT = WM_APP + 1 };
	
	// Constructor
	CRenameDialog(LPCTSTR strOldFileName, LPCTSTR strFilePath, bool bDoRename = true);
	
	CString	GetNewFileName() const { return m_strNewFileName; }
	CString GetNewFilePath() const { return m_strFolder + m_strNewFileName; }

	void	DoRename() const;

	BEGIN_MSG_MAP( CRenameDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		MESSAGE_HANDLER_EX( WM_SELTEXTWITHOUTEXT, OnSelTextWithoutExt )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk)
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	LRESULT OnSelTextWithoutExt(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl) {
		EndDialog(nID);
	}

private:
	// Data members
	CString	m_strOldFileName;
	CString m_strFolder;
	CString m_strNewFileName;
	bool	m_bDoRename;
};
