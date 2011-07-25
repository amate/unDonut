/**
 *	@file	OleDragDropExpToolBarCtrl.h
 *	@brie	Linkバーの処理(Drag&Drog処理を付加)
 */

#pragma once

#include "ExplorerToolBarCtrl.h"
#include "ParseInternetShortcutFile.h"		//+++


///+++ メモ:Linkバーの処理(Drag&Drog処理を付加)
template <class T>
class COleDragDrogExplorerToolBarCtrlImpl
	: public CExplorerToolBarCtrlImpl<T>
	, public IDropTargetImpl<T>
	, public IDropSourceImpl<T>
{
public:
	DECLARE_WND_SUPERCLASS( NULL, CToolBarCtrl::GetWndClassName() )

private:
	typedef CExplorerToolBarCtrlImpl<T>  baseClass;

	// Data members
	bool m_bDragAccept;
	bool m_bRButtonDown;
	int  m_nDraggingCmdID;
	int  m_nCurrentDrop;

public:
	// Ctor
	COleDragDrogExplorerToolBarCtrlImpl(int nInsertPointMenuItemID, int nMinID, int nMaxID)
		: baseClass(nInsertPointMenuItemID, nMinID, nMaxID)
		, m_bDragAccept(false)
		, m_bRButtonDown(false)
		, m_nDraggingCmdID(-1)
		, m_nCurrentDrop(-1)
	{
	}


	// Overrides
	void OnInitExplorerToolBarCtrl()
	{
		RegisterDragDrop();
	}


	void OnTermExplorerToolBarCtrl()
	{
		RevokeDragDrop();
	}


private:
	HRESULT OnGetExplorerToolBarDataObject(int nCmdID, IDataObject **ppDataObject)
	{
		ATLASSERT(ppDataObject != NULL);
		CString 		  strUrl		   = _GetPathFromCmdID(nCmdID);
		ATLASSERT( !strUrl.IsEmpty() );
		MtlRemoveTrailingBackSlash(strUrl);

		HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL, IID_IDataObject, (void **) ppDataObject);
		if ( FAILED(hr) ) {
			*ppDataObject = NULL;
			return E_NOTIMPL;
		}

	  #ifdef _ATL_DEBUG_INTERFACES
		ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
	  #endif

		CHlinkDataObject *pHlinkDataObject = NULL;	// this is hack, no need to release
		hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);
		if ( SUCCEEDED(hr) ) {
			pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(CString(), strUrl) );
			return S_OK;
		}

		return E_NOTIMPL;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(COleDragDrogExplorerToolBarCtrlImpl<T>)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		CHAIN_MSG_MAP(baseClass)
	ALT_MSG_MAP(1)
		CHAIN_MSG_MAP_ALT(baseClass, 1)
	END_MSG_MAP()


private:
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT	pt	   = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		LRESULT lRet   = DefWindowProc();			// required
		int 	nCmdID = _HitTestCmdID(pt);
		if (nCmdID == -1) {
			return 0;
		}

		_DoDragDrop(nCmdID, pt, (UINT) wParam, true);

		return lRet;
	}


	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int   nCmdID = _HitTestCmdID(pt);
		if (nCmdID == -1) {
			return 0;
		}
		_DoDragDrop(nCmdID, pt, (UINT) wParam, false);
		return 0;
	}


public:
	// IDropTarget
	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		m_bDragAccept  = _MtlIsHlinkDataObject(pDataObject);
		if (!m_bDragAccept)
			return DROPEFFECT_NONE;
		m_bRButtonDown = ::GetAsyncKeyState(VK_RBUTTON) < 0;
		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		if (!m_bDragAccept)
			return DROPEFFECT_NONE;
		int nID = _HitTestCmdID(point);
		if (nID != -1) {
			if (m_nCurrentDrop != nID)
				MarkButton(m_nCurrentDrop, FALSE);
			MarkButton(m_nCurrentDrop = nID, TRUE);
			if (m_nDraggingCmdID == nID)
				return DROPEFFECT_NONE;
		} else {
			OnDragLeave();
		}

		if (m_nDraggingCmdID != -1)
			return _MtlStandardDropEffect(dwKeyState);
		else
			return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect);
	}


	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		DROPEFFECT	dropResult = DROPEFFECT_NONE;
		MarkButton(m_nCurrentDrop, FALSE);
		CString 	strFolder;
		CString 	strPath;
		CString 	strLink;
		_HitTest(point, strFolder, strPath, strLink);

		if ( strFolder.IsEmpty() && strPath.IsEmpty() )
			return DROPEFFECT_NONE;

		// first, file dropped or not
		CSimpleArray<CString>	arrFiles;

		if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
			if ( m_bRButtonDown && !strPath.IsEmpty() ) {
				if ( MtlOnRButtonDropOpenFromApp(m_hWnd, point, strPath, arrFiles[0]) )
					return dropResult;
			}

			if ( m_bRButtonDown && MtlOnRButtonDrop(m_hWnd, dropEffect, point) )
				return dropResult;

			if (dropEffect & DROPEFFECT_COPY) {
				if ( MtlCopyFile(strFolder, arrFiles) ) {
					dropResult = DROPEFFECT_COPY;
				}
			} else if (dropEffect & DROPEFFECT_MOVE) {
				if ( MtlMoveFile(strFolder, arrFiles) ) {
					dropResult = DROPEFFECT_MOVE;
				}
			} else if (dropEffect & DROPEFFECT_LINK) {
				dropResult = DROPEFFECT_LINK;
				MtlMakeSureTrailingBackSlash(strFolder);

				for (int i = 0; i < arrFiles.GetSize(); ++i) {
					MtlCreateShortCutFile(strFolder + MtlGetDisplayTextFromPath(arrFiles[i]) + _T("へのｼｮｰﾄｶｯﾄ.lnk"), arrFiles[i]);
				}
			}

			return dropResult;
		} else {
			//+++ 拡張プロパティで検索設定されている.urlにドロップしたならば、ドロップされた文字列を検索してみる.
			//m_nCurrentDrop
			CString strSearch;
			if (MtlGetHGlobalText( pDataObject, strSearch)) {
				LPCTSTR		strExcept = _T(" \t\"\r\n　");
				strSearch.TrimLeft(strExcept);
				strSearch.TrimRight(strExcept);
				if (MTL::ParseInternetShortcutFile_SearchMode(strLink, strSearch) > 1) {
					//+++ 暫定：あとで拡張プロパティ対応版に変更しないと駄目.
					DonutOpenFile(GetTopLevelParent(), strLink, D_OPENFILE_ACTIVATE);
				}
			}
		}

		return dropResult;
	}


	void OnDragLeave()
	{
		if (m_nCurrentDrop != -1) {
			MarkButton(m_nCurrentDrop, FALSE);
			m_nCurrentDrop = -1;
		}
	}


private:
	int _HitTestCmdID(CPoint pt)
	{
		int 	 nIndex = HitTest(&pt);

		if (nIndex < 0)
			return -1;			// outside

		TBBUTTON tbb;
		GetButton(nIndex, &tbb);
		int 	 nCmdID = tbb.idCommand;

		return nCmdID;
	}


	void _HitTest(CPoint pt, CString &strFolderPath, CString& strPathExe, CString& strLink)
	{
		int 	nCmdID	= _HitTestCmdID(pt);

		if (nCmdID == -1) { 								// out side
			strFolderPath = GetExplorerToolBarRootPath();	// target is links folder
			return;
		}

		CString strPath = _GetPathFromCmdID(nCmdID);

		if ( MtlIsDirectoryPath(strPath) ) {
			strFolderPath = strPath;						// target is the folder
			return;
		}

		/*CString*/ strLink = MtlGetShortcutLink(strPath);

		if ( MtlIsDirectory(strLink) ) {					// target is the folder
			strFolderPath = strLink;
		} else if ( MtlIsExt( strLink, _T(".exe") ) ) {
			strPathExe	  = strPath;						// fill executable target
			strFolderPath = GetExplorerToolBarRootPath();
		} else {											// target is links folder
			strFolderPath = GetExplorerToolBarRootPath();
		}
	}


	// IDropSource
	void _DoDragDrop(int nCmdID, CPoint pt, UINT nFlags, bool bLeftButton)
	{
		if ( PreDoDragDrop(m_hWnd, NULL, false) ) { 		// now dragging
			CComPtr<IDataObject> spDataObject;
			T * 		pT = static_cast<T *>(this);
			HRESULT 	hr = pT->OnGetExplorerToolBarDataObject(nCmdID, &spDataObject);

			if ( SUCCEEDED(hr) ) {
				m_nDraggingCmdID = nCmdID;
				DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
				m_nDraggingCmdID = -1;
			}

			if (bLeftButton)
				SetState(nCmdID, TBSTATE_ENABLED);			// reset button
		} else {
			if (bLeftButton) {
				SendMessage( WM_LBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
			} else {
				SendMessage( WM_RBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
			}
		}
	}

};

