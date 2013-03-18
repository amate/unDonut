/**
*	@file	DonutFavoriteGroupTreeView.h
*	@brief	お気に入りグループ
*/

#pragma once

#include <vector>
#include "DonutTabList.h"

class CFavoriteGroupEditDialog;

class CDonutFavoriteGroupTreeView : public CWindowImpl<CDonutFavoriteGroupTreeView, CTreeViewCtrl>
{
	friend class CFavoriteGroupEditDialog;	// For _RefreshTree
public:
	DECLARE_WND_SUPERCLASS( _T("DonutFavoriteGroupTreeView"), CTreeViewCtrl::GetWndClassName() )

	enum { kIconIndexFavoriteGroup = 0, kIconIndexLink = 1, };

	CDonutFavoriteGroupTreeView();
	~CDonutFavoriteGroupTreeView();

	static void SetFunctions(std::function<CDonutTabList ()> funcGetAll, std::function<std::unique_ptr<ChildFrameDataOnClose> ()> funcGetActive) {
		s_funcGetAllDonutTabList = funcGetAll;
		s_funcGetActiveDonutTabData = funcGetActive;
	}

	HWND	Create(HWND hWndParent);

	bool	IsRootChildren(HTREEITEM ht);
	int		GetIndexFromHTREEITEM(HTREEITEM htParent, HTREEITEM htItem);
	void	AddActiveTab(HTREEITEM htFavoriteGroup);

	BEGIN_MSG_MAP_EX( CDonutFavoriteGroupTreeView )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_BEGINLABELEDIT, OnTreeBeginLabelEdit )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_ENDLABELEDIT, OnTreeEndLabelEdit )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_SINGLEEXPAND, OnTreeSingleExpand )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_ITEMEXPANDING, OnTreeItemExpanding )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( NM_RCLICK, OnTreeItemRClick )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_SELCHANGED, OnTreeSelChanged )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( NM_DBLCLK, OnTreeItemDoubleClick )

	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();

	LRESULT OnTreeBeginLabelEdit(LPNMHDR pnmh);
	LRESULT OnTreeEndLabelEdit(LPNMHDR pnmh);
	LRESULT OnTreeSingleExpand(LPNMHDR pnmh) { return TVNRET_SKIPOLD; }
	LRESULT OnTreeItemExpanding(LPNMHDR pnmh);
	LRESULT OnTreeItemRClick(LPNMHDR pnmh);
	LRESULT OnTreeSelChanged(LPNMHDR pnmh);
	LRESULT OnTreeItemDoubleClick(LPNMHDR pnmh);

private:
	void	_RefreshTree();
	void	_AddTreeItem(HTREEITEM htParent, const CString& title, const CString& url);

	// Data members
	std::vector<CString>	m_vecURLs;
	CImageList				m_imageList;
	
	static std::function<CDonutTabList ()>	s_funcGetAllDonutTabList;
	static std::function<std::unique_ptr<ChildFrameDataOnClose> ()>	s_funcGetActiveDonutTabData;
};
