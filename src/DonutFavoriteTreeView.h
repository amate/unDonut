/**
*	@file	DonutFavoriteTreeView.h
*	@brief	お気に入りツリービュー
*/

#pragma once

#include <unordered_map>
#include "MtlDragDrop.h"
#include "LinkPopupMenu.h"

class CDonutFavoriteTreeView : 
	public CWindowImpl<CDonutFavoriteTreeView, CTreeViewCtrl>,
	public IDropTargetImpl<CDonutFavoriteTreeView>,
	public IDropSourceImpl<CDonutFavoriteTreeView>
{
public:
	DECLARE_WND_SUPERCLASS( _T("DonutFavoriteTreeView"), CTreeViewCtrl::GetWndClassName() )

	// Constants
	enum { 
		kLoadBookmarkListTimerId = 1, 
		kLoadBookmarkListTimerInterval = 700,

		kDragOverExpandTimerId = 2,
		kDragOverExpandTimerInterval = 500,

		kIconFolderIndex = 0,
		kIconLinkIndex = 1,

		kDragFolderMargin = 3,
	};

	CDonutFavoriteTreeView();
	~CDonutFavoriteTreeView();

	HWND	Create(HWND hWndParent);

	/// お気に入りの追加/整理用
	void	SetItemClickFunc(std::function<void (LinkItem*)> func) { m_funcItemClick = func; }

	LinkFolderPtr	GetParentLinkFolderPtr(HTREEITEM htItem);

	// IDropTargetImpl
	bool OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);
	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	void	OnDragLeave();


	BEGIN_MSG_MAP_EX( CDonutFavoriteTreeView )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_TIMER( OnTimer )
		MSG_WM_SHOWWINDOW( OnShowWindow )
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_SINGLEEXPAND, OnTreeSingleExpand )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TVN_ITEMEXPANDING, OnTreeItemExpanding )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( NM_RCLICK, OnTreeItemRClick )
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnTimer(UINT_PTR nIDEvent);
	void OnShowWindow(BOOL bShow, UINT nStatus);

	void OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnTreeSingleExpand(LPNMHDR pnmh) { return TVNRET_SKIPOLD; }
	LRESULT OnTreeItemExpanding(LPNMHDR pnmh);
	LRESULT OnTreeItemRClick(LPNMHDR pnmh);

private:
	void _RefreshTree();
	void _DoDragDrop(HTREEITEM hItem, CPoint pt, UINT nFlags);
	HTREEITEM _AddTreeItem(HTREEITEM htRoot, HTREEITEM htInsertAfter, LinkItem* pItem);
	void	_AddTreeItemList(HTREEITEM htRoot, LinkFolderPtr list);

	bool _ScrollItem(bool bDown);
	bool _CanScrollItem(bool bDown);
	bool _IsFolderMargin(HTREEITEM htFolder, CPoint pt);

	// Data members
	std::function<void (LinkItem*)>	m_funcItemClick;
	CImageList	m_imageList;
	HTREEITEM	m_hItemDragging;
	LinkFolderPtr	m_pParentDragFolder;
	bool	m_bAcceptDrag;
	bool	m_bDragItemIsLinkFile;
	std::pair<LinkFolderPtr, int>	m_DragItemData;
	HTREEITEM	m_htLastDragOver;
	std::unordered_map<HICON, int>	m_mapIconIndex;

};

