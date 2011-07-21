/**
 *	@file	OleDragDropExpTreeViewCtrl.h
 *	@brief	エクスプローラバーでのツリー表示でのドラッグ＆ドロップ処理.
 */

#pragma once

#include "ExplorerTreeViewCtrl.h"
#include "MtlDragDrop.h"
#include "HLinkDataObject.h"
#include "FavoriteOrder.h"


template <class T>
class COleDragDrogExplorerTreeViewCtrlImpl
	: public CExplorerTreeViewCtrlImpl<T>
	, public IDropTargetImpl<T>
	, public IDropSourceImpl<T>
{
public:
	DECLARE_WND_SUPERCLASS( NULL, CTreeViewCtrl::GetWndClassName() )

private:
	typedef CExplorerTreeViewCtrlImpl<T>  baseClass;

	// Data members
	bool		m_bDragAccept;
	bool		m_bRButtonDown;
	HTREEITEM	m_hItemDragging;
	UINT_PTR	m_nIDTimer;
	CPoint		m_ptMouse;
public:
	BOOL		m_bDragDrop;


public:
	COleDragDrogExplorerTreeViewCtrlImpl()
		: m_bDragAccept(false)
		, m_bDragDrop(false)
		, m_hItemDragging(NULL)
		, m_nIDTimer(0)
		, m_ptMouse(-1, -1)
		, m_bRButtonDown(false)
	{
	}


private:
	// Overridables
	HRESULT OnGetExplorerTreeViewDataObject(HTREEITEM hItem, IDataObject **ppDataObject)
	{
		ATLASSERT(ppDataObject != NULL);

		CItemIDList 	idl = _GetItemIDList(hItem);
		ATLASSERT( !idl.IsNull() );

		HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL, IID_IDataObject, (void **) ppDataObject);
		if ( FAILED(hr) ) {
			*ppDataObject = NULL;
			return E_NOTIMPL;
		}

	  #ifdef _ATL_DEBUG_INTERFACES
		ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
	  #endif

		CHlinkDataObject*	pHlinkDataObject = NULL; // this is hack, no need to release
		hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);

		if ( SUCCEEDED(hr) ) {
			CString strUrl = idl.GetPath();
			strUrl.Replace( _T('/'), _T('\\') );
			pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(CString(), strUrl) );
			return S_OK;
		}

		return E_NOTIMPL;
	}


public:
	// Overrides
	void OnExplorerTreeViewInit()
	{
		RegisterDragDrop();
	}


	void OnExplorerTreeViewTerm()
	{
		RevokeDragDrop();
		_CleanUpStayTimer();
	}


	// Message map and handlers
	BEGIN_MSG_MAP(COleDragDrogExplorerTreeViewCtrlImpl)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_TIMER	  , OnTimer 	 )
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()


private:
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT	  pt	   = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		HTREEITEM hItemSel = GetSelectedItem();
		HTREEITEM hItem    = _HitTestForFullLow(pt);

		if (hItem == NULL) {
			bHandled = FALSE;
			return 0;
		}

		EndEditLabelNow(TRUE);	// cancel

		//		HTREEITEM hItemSelected = GetSelectedItem();
		//		bool bHilight = (hItem != hItemSelected);

		_DoDragDrop(hItem, pt, (UINT) wParam, true);

		//		if (hItemExpanded == NULL && bHilight)	// reset hilight
		//			SetItem(hItem, TVIF_STATE, NULL, 0, 0, 0, TVIS_SELECTED/*TVIS_SELECTED*/, NULL);

		return 0;
	}


	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT	  	pt		= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		HTREEITEM 	hItem 	= _HitTestForFullLow(pt);
		if (hItem == NULL) {
			bHandled 		= FALSE;
			return 0;
		}
		EndEditLabelNow(TRUE);	// cancel
		_DoDragDrop(hItem, pt, (UINT) wParam, false);
		return 0;
	}


public:
	// IDropTargetImpl
	// Overrides
	bool OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true)
	{
		bool bResult = false;

		if (HIBYTE(nScrollCode) == SB_LINEUP) {
			if (bDoScroll)
				bResult = _ScrollItem(false);
			else
				bResult = _CanScrollItem(false);
		} else if (HIBYTE(nScrollCode) == SB_LINEDOWN) {
			if (bDoScroll)
				bResult = _ScrollItem(true);
			else
				bResult = _CanScrollItem(true);
		}

		return bResult;
	}


	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		m_bDragAccept  = _MtlIsHlinkDataObject(pDataObject);

		if (!m_bDragAccept)
			return DROPEFFECT_NONE;

		m_bRButtonDown = ::GetAsyncKeyState(VK_RBUTTON) < 0;

		_SetUpStayTimer(point);

		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		if (!m_bDragAccept)
			return DROPEFFECT_NONE;

		CString   strPath;
		HTREEITEM hItem = _HitTest(point, strPath);
		SelectDropTarget(hItem);

		if (hItem != NULL && hItem == m_hItemDragging) {
			return DROPEFFECT_NONE;
		}

		if (m_hItemDragging)
			return _MtlStandardDropEffect(dwKeyState);
		else
			return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect);
	}


	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		DROPEFFECT			  dropResult = DROPEFFECT_NONE;

		SelectDropTarget(NULL);
		_CleanUpStayTimer();

		CString 			  strPath;
		HTREEITEM			  hItem 	 = _HitTest(point, strPath);

		// first, file dropped or not
		CSimpleArray<CString> arrFiles;

		if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
			if ( _PreDropFile(hItem, arrFiles) )
				return dropResult;

			if ( m_bRButtonDown && MtlOnRButtonDrop(m_hWnd, dropEffect, point) )
				return dropResult;

		  #if 1	//+++ お気に入りバーの並べ替え対策... ちょっと無理やりすぎ...
			int  		cpyFlags = 0;
			if (  (dropEffect & (DROPEFFECT_COPY | DROPEFFECT_MOVE))
				&& CFavoritesMenuOption::s_bCstmOrder ) {
				CFavoriteOrderHandler	dstOrder;
				if (dstOrder.ReadData(strPath) != 0) {	//+++ strPathのお気に入り一覧取得. お気に入りバー以外は、ここでfalseになる.
					CString		strNextItemPath = HitItemPath(point);	//+++
					int 		dstPos = dstOrder.FindNameToPosition(strNextItemPath);
					if (dstPos >= 0) {
						for (int i = 0; i < arrFiles.GetSize(); ++i) {
							CString&	nm = arrFiles[i];
							if (dropEffect & DROPEFFECT_MOVE) {
								int		srcPos = dstOrder.FindNameToPosition(nm);
								if (srcPos >= 0) {
									cpyFlags |= 1;
									dstOrder.MoveData(srcPos, dstPos);
								} else {
									CString 				srcDir = Misc::GetDirName(nm);
									CFavoriteOrderHandler	srcOrder;
									srcOrder.ReadData(srcDir);
									if (srcOrder.FindName(nm) >= 0) {
										cpyFlags |= 3;
										dstOrder.MoveData(srcOrder, nm, dstPos);	//お気に入りの移動. urlファイルの移動が行われるので注意!
										srcOrder.SaveData();
									} else {
										cpyFlags |= 1;
										dstOrder.AddData(nm, dstPos);
									}
								}
							} else {
								cpyFlags |= 1;
								dstOrder.AddData(nm, dstPos);
							}
							++dstPos;
						}
						if (cpyFlags & 1)
							dstOrder.SaveData();
					}
				}
			}
		  #endif
			if (dropEffect & DROPEFFECT_COPY) {
				if ( MtlCopyFile(strPath, arrFiles) ) {
					dropResult = DROPEFFECT_COPY;
					_RefreshChildren(hItem);
				}
			} else if (dropEffect & DROPEFFECT_MOVE) {
				if (arrFiles.GetSize() == 1 && Misc::GetDirName(arrFiles[0]) == strPath) {
					dropResult = DROPEFFECT_MOVE;

					//if (m_hItemDragging) {
					//	_CLockRedrawTreeView lock(m_hItemDragging, m_hWnd);
					//	_CleanUpChildren(m_hItemDragging);
					//	DeleteItem(m_hItemDragging);
					//}

					_RefreshChildren(hItem);
				} else if (cpyFlags == 3 || MtlMoveFile(strPath, arrFiles) ) {
					dropResult = DROPEFFECT_MOVE;

					if (m_hItemDragging) {
						_CLockRedrawTreeView lock(m_hItemDragging, m_hWnd);
						_CleanUpChildren(m_hItemDragging);
						DeleteItem(m_hItemDragging);
					}

					_RefreshChildren(hItem);
				}
			} else if (dropEffect & DROPEFFECT_LINK) {
				dropResult = DROPEFFECT_LINK;

				MtlMakeSureTrailingBackSlash(strPath);

				for (int i = 0; i < arrFiles.GetSize(); ++i) {
					MtlCreateShortCutFile(strPath + MtlGetDisplayTextFromPath(arrFiles[i]) + _T("へのｼｮｰﾄｶｯﾄ.lnk"), arrFiles[i]);
				}

				_RefreshChildren(hItem);
			}

			return dropResult;
		}

		// next, text dropped or not
		CString 			  strText;

		if ( MtlGetHGlobalText(pDataObject, strText)
		   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
		{
			if ( _CreateNewItem(hItem, strText) ) {
				return DROPEFFECT_COPY;
			}
		}

		return dropResult;
	}


	void OnDragLeave()
	{
		SelectDropTarget(NULL);
		_CleanUpStayTimer();
	}


private:
	bool _CreateNewItem(HTREEITEM hItemParent, const CString &strUrl)
	{
		bool			bRet		= false;
		CString 		str( _T("新しいアイテム") );

		CString 		strPath 	= CItemIDList( _GetItemIDList(hItemParent) ).GetPath();

		if ( strPath.IsEmpty() )
			return bRet;

		MtlMakeSureTrailingBackSlash(strPath);

		int 			i			= 2;
		CString 		strTmp		= str;

		for (;;) {
			if ( !MtlIsValidPath( strPath + strTmp + _T(".url") ) )
				break;

			strTmp	= str + _T(" (");
			strTmp.Append(i++);
			strTmp += _T(')');
		}

		strPath = strPath + strTmp + _T(".url");

		ATLTRACE(_T("_CreateNewItem(%s)\n"), strPath);

		if ( !MtlCreateInternetShortcutFile(strPath, strUrl) )
			return false;

		CString 		strItemName = MtlGetDisplayTextFromPath(strPath);

		// insert
		TV_INSERTSTRUCT tvis = {0};
		tvis.hParent			 = hItemParent;
		tvis.hInsertAfter		 = TVI_LAST;
		tvis.item.mask			 = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvis.item.pszText		 = (LPTSTR) (LPCTSTR) strItemName;

		// set up full item id list!
		CItemIDList 	idlFull 	= strPath;

		if ( idlFull.IsNull() )
			return bRet;

		ATLASSERT(idlFull.m_pidl != NULL);
		tvis.item.lParam		 = (LPARAM) idlFull.m_pidl;

		tvis.item.iImage		 = I_IMAGECALLBACK;
		tvis.item.iSelectedImage = I_IMAGECALLBACK;

		if ( hItemParent != NULL && !_IsItemExpanded(hItemParent) ) {
			Select(hItemParent, TVGN_CARET);
			Expand(hItemParent, TVE_EXPAND);	// required. cuz if already selected, Select(hTreeItem, TVGN_CARET) does nothing.
			HTREEITEM hItem = _FindItemIDList(hItemParent, idlFull);
			EditLabel(hItem);
		} else {
			HTREEITEM hItem = InsertItem(&tvis);
			EditLabel(hItem);
		}

		idlFull.Detach();	// *********don't forget clean up the lParam!!!!!!!********
		return true;
	}


	HTREEITEM _HitTest(CPoint pt, CString &strPath)
	{
		HTREEITEM		hItem = _HitTestForFullLow(pt);
		CItemIDList 	idl   = _GetItemIDList(hItem);

		strPath = idl.GetPath();
		if (hItem == NULL) {
			return NULL;
		}

		if ( MtlIsExt( strPath, _T(".exe") ) || MtlIsExt( strPath, _T(".lnk") ) ) {
			return hItem;
		} else if ( _IsItemFolder(hItem) ) {
			return hItem;
		} else {
			HTREEITEM hItemParent = GetParentItem(hItem);

			if (hItemParent == NULL) {
				strPath = m_idlRootFolder.GetPath();
				return NULL;	// root
			}

			strPath = CItemIDList( _GetItemIDList(hItemParent) ).GetPath(); // root folder
			return hItemParent;
		}
	}

	//+++
	CString HitItemPath(CPoint pt)
	{
		HTREEITEM		hItem = _HitTestForFullLow(pt);
		CItemIDList 	idl   = _GetItemIDList(hItem);
		return idl.GetPath();
	}


	bool _ScrollItem(bool bDown)
	{
		CRect rc;
		GetClientRect(&rc);

		if (bDown) {
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
			return true;
		} else {
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
			return true;
		}
	}


	bool _CanScrollItem(bool bDown)
	{
		if (bDown) {
			CRect rc;
			GetClientRect(&rc);
			return (_HitTestForFullLow( CPoint(rc.left + 1, rc.bottom - 1) ) != NULL);
		} else {
			return GetRootItem() != GetFirstVisibleItem();
		}
	}


	void _DoDragDrop(HTREEITEM hItem, CPoint pt, UINT nFlags, bool bLeftButton)
	{
		SelectDropTarget(hItem);

		if ( m_bDragDrop && PreDoDragDrop(m_hWnd, NULL, false) ) {			// now dragging
			CComPtr<IDataObject> spDataObject;
			T * 		pT = static_cast<T *>(this);
			HRESULT 	hr = pT->OnGetExplorerTreeViewDataObject(hItem, &spDataObject);

			if ( SUCCEEDED(hr) ) {
				m_hItemDragging 		= hItem;
				DROPEFFECT	dropEffect	= DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
				m_hItemDragging 		= NULL;
			}
		} else {
			if (bLeftButton) {
				if ( _IsItemExpanded(hItem) ) {
					Expand(hItem, TVE_COLLAPSE);
				} else {
					Select(hItem, TVGN_CARET);
					Expand(hItem, TVE_EXPAND);	// required. cuz if already selected, Select(hTreeItem, TVGN_CARET) does nothing.
				}

				if ( !_IsItemFolder(hItem) ) {
					T *pT = static_cast<T *>(this);
					pT->OnTreeItemClicked(hItem, nFlags);
				} else
					::SetFocus(m_hWnd); 	// set focus later
			} else {
				::SetFocus(m_hWnd); 		// set focus now
				SendMessage( WM_RBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
			}

			SelectDropTarget(NULL);
		}
	}


	void _SetUpStayTimer(CPoint pt)
	{
		_CleanUpStayTimer();

		m_nIDTimer = SetTimer(12, 650);
		m_ptMouse  = pt;
	}


	void _CleanUpStayTimer()
	{
		if (m_nIDTimer != 0) {
			KillTimer(m_nIDTimer);
			m_nIDTimer = 0;
		}

		m_ptMouse = CPoint(-1, -1);
	}


	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT   nTimerID = (UINT) wParam;

		if (nTimerID != m_nIDTimer) {
			bHandled = FALSE;
			return 1;
		}

		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		if (ptCursor == m_ptMouse) {
			HTREEITEM hItem = _HitTestForFullLow(ptCursor);

			if ( _IsItemFolder(hItem) ) {
				Select(hItem, TVGN_CARET);
				Expand(hItem, TVE_EXPAND);	// required. cuz if already selected, Select(hTreeItem, TVGN_CARET) does nothing.
				m_ptMouse = CPoint(-1, -1);
				return 0;
			}
		}

		m_ptMouse = ptCursor;

		return 0;
	}


	bool _PreDropFile(HTREEITEM hItem, CSimpleArray<CString> &arrFiles)
	{
		if ( _IsItemFolder(hItem) )
			return false;

		CItemIDList idl    = _GetItemIDList(hItem);

		if ( idl.IsNull() )
			return false;

		CString 	strExe = idl.GetPath();

		if ( !MtlIsExt( strExe, _T(".lnk") ) && !MtlIsExt( strExe, _T(".exe") ) ) {
			return false;
		}

		for (int i = 0; i < arrFiles.GetSize(); ++i) {
			MtlPreOpenFile(strExe, arrFiles[i]);
		}

		return true;
	}


	bool _OnRButtonDrop(DROPEFFECT &dropEffect, CPoint pt)
	{
		if (!m_bRButtonDown)
			return true;

		enum { s_nIDMove = 1, s_nIDCopy = 2, s_nIDShortCut = 3, s_nIDCancel = 4, };
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDMove		, _T("ここに移動(&M)") );
		menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDCopy		, _T("ここにコピー(&C)") );
		menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDShortCut	, _T("ショートカットをここに作成(&S)") );
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu( MF_ENABLED | MF_STRING, s_nIDCancel	, _T("キャンセル") );

		if (dropEffect & DROPEFFECT_COPY)
			menu.SetMenuDefaultItem(s_nIDCopy, FALSE);
		else if (dropEffect & DROPEFFECT_MOVE)
			menu.SetMenuDefaultItem(s_nIDMove, FALSE);
		else if (dropEffect & DROPEFFECT_LINK)
			menu.SetMenuDefaultItem(s_nIDShortCut, FALSE);

		ClientToScreen(&pt);
		UINT  uMenuFlags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN;
		int   nCmdID	 = menu.TrackPopupMenu(uMenuFlags | TPM_RETURNCMD , pt.x, pt.y, m_hWnd, NULL);

		if (nCmdID == s_nIDMove)
			dropEffect = DROPEFFECT_MOVE;
		else if (nCmdID == s_nIDCopy)
			dropEffect = DROPEFFECT_COPY;
		else if (nCmdID == s_nIDShortCut)
			dropEffect = DROPEFFECT_LINK;
		else
			return false;

		return true;
	}
};



class COleDragDrogExplorerTreeViewCtrl : public COleDragDrogExplorerTreeViewCtrlImpl<COleDragDrogExplorerTreeViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS( _T("MTL_DragDropExpTreeViewCtrl"), GetWndClassName() )
};

