/**
 *	@file	CommandSelectDialog.h
 *	@brief	"コマンドを選ぶ"ダイアログ
 */
#pragma once

#include "../resource.h"

class CCommandSelectDialog
	: public CDialogImpl<CCommandSelectDialog >
	, public CWinDataExchange<CCommandSelectDialog >
{
public:
	enum { IDD = IDD_DIALOG_CMD_SELECT };

private:
	// Data members
	HMENU		m_hMenu;
	CComboBox	m_cmbCategory;
	CComboBox	m_cmbCommand;
	DWORD_PTR	m_dwCommandID;

public:
	CCommandSelectDialog(HMENU hMenu);

	DWORD_PTR	GetCommandID();

  #if 1 	//*+++ 抜けを追加.
	BEGIN_DDX_MAP( CCommandSelectDialog )
	END_DDX_MAP()
  #endif

	// Message map and handlers
	BEGIN_MSG_MAP(CCommandSelectDialog)
		MESSAGE_HANDLER 	(WM_INITDIALOG	, OnInitDialog)

		COMMAND_HANDLER_EX	(IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate)
		COMMAND_HANDLER_EX	(IDC_CMB_COMMAND , CBN_SELCHANGE, OnSelChangeCmd )

		COMMAND_ID_HANDLER	(IDOK			, OnCloseCmd)
		COMMAND_ID_HANDLER	(IDCANCEL		, OnCloseCmd)
	END_MSG_MAP()

private:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);
	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);

private:
	void		SetGuideMsg();
};
