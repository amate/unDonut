/**
*	@file	FavoriteGroupEditDialog.h
*	@brief	お気に入りグループ編集ダイアログ
*/

#pragma once

#include "resource.h"
#include "DonutFavoriteGroupTreeView.h"

class CFavoriteGroupEditDialog :
	public CDialogImpl<CFavoriteGroupEditDialog>,
	public CDialogResize<CFavoriteGroupEditDialog>
{
public:
	enum { IDD = IDD_FAVORITEGROUP_EDIT };

	enum FavoriteGroupEditMode {
		kFavoriteGroupAdd,
		kFavoriteGroupOrganize,
	};

	CFavoriteGroupEditDialog(FavoriteGroupEditMode mode) : m_favoriteGroupMode(mode) { }

	BEGIN_DLGRESIZE_MAP( CFavoriteGroupEditDialog )
		DLGRESIZE_CONTROL( IDC_TREE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL( IDOK, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL( ID_DELETE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL( IDCANCEL, DLSZ_MOVE_Y | DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP( CFavoriteGroupEditDialog ) 
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK	, OnOK	)
		COMMAND_ID_HANDLER_EX( ID_DELETE , OnDelete )
		COMMAND_ID_HANDLER_EX( IDCANCEL	, OnCancel	)
		CHAIN_MSG_MAP( CDialogResize<CFavoriteGroupEditDialog> ) 
		NOTIFY_HANDLER_EX( IDC_TREE, TVN_SELCHANGED, OnTreeSelChanged )
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	// handleする
	LRESULT OnTreeSelChanged(LPNMHDR pnmh) { return 0; }

private:
	// Data members
	FavoriteGroupEditMode	m_favoriteGroupMode;
	CDonutFavoriteGroupTreeView	m_tree;
};

