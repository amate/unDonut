/**
*	@file	FavoriteEditDialog.h
*	@brief	お気に入り編集ダイアログ
*/

#pragma once

#include "resource.h"

// 前方宣言
struct LinkItem;

/////////////////////////////////////////////////////////
// お気に入り編集ダイアログ

class CFavoriteEditDialog : 
	public CDialogImpl<CFavoriteEditDialog>,
	public CDialogResize<CFavoriteEditDialog>,
	public CWinDataExchange<CFavoriteEditDialog>
{
public:
	enum { IDD = IDD_FAVORITE_EDIT };

	CFavoriteEditDialog(bool bFavoriteAdd);

	void	SetNameURLFavicon(LPCTSTR name, LPCTSTR url, HICON icon) {
		m_strName = name; m_strURL = url; m_favicon = icon;
	}

	BEGIN_DLGRESIZE_MAP( CFavoriteEditDialog )
		DLGRESIZE_CONTROL( IDC_TREE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL( IDC_STATIC_NAME	, DLSZ_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_STATIC_URL	, DLSZ_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_EDIT_NAME, DLSZ_MOVE_Y | DLSZ_SIZE_X)
		DLGRESIZE_CONTROL( IDC_EDIT_URL	, DLSZ_MOVE_Y | DLSZ_SIZE_X)
		DLGRESIZE_CONTROL( ID_APPLY, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL( ID_CREATE_FOLDER, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL( ID_DELETE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL( IDCANCEL, DLSZ_MOVE_Y | DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP( CFavoriteEditDialog )
		DDX_CONTROL_HANDLE( IDC_TREE	, m_tree	)
		DDX_TEXT( IDC_EDIT_NAME	, m_strName	)
		DDX_TEXT( IDC_EDIT_URL	, m_strURL	)
	END_DDX_MAP()

	BEGIN_MSG_MAP( CFavoriteEditDialog ) 
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( ID_APPLY	, OnApply	)
		COMMAND_ID_HANDLER_EX( ID_CREATE_FOLDER	, OnCreateFolder	)
		COMMAND_ID_HANDLER_EX( ID_DELETE	, OnDelete )
		COMMAND_ID_HANDLER_EX( IDCANCEL	, OnCancel	)
		NOTIFY_HANDLER_EX( IDC_TREE, TVN_SELCHANGED, OnTreeSelChanged	)
		CHAIN_MSG_MAP( CDialogResize<CFavoriteEditDialog> ) 
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnApply(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCreateFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTreeSelChanged(LPNMHDR pnmh);

private:
	// Data members
	bool	m_bFavoriteAdd;
	bool	m_bSave;
	CTreeViewCtrl	m_tree;
	CImageList		m_image_list;
	vector<unique_ptr<LinkItem> >*	m_pBookmarkList;
	CString	m_strName;
	CString	m_strURL;
	CIconHandle	m_favicon;
};









