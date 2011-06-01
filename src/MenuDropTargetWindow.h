/**
 *	@file	MenuDropTargetWindow.h
 *	@brief	メニューでのドラッグ＆ドロップ処理(IEお気に入りの並べ替え処理)
 */
#pragma once

#include "FavoriteOrder.h"
//#include "DonutExplorerBar.h"		//+++ お気に入りメニューで移動が発生したとき、お気に入りバー側にも反映
#include "Donut.h"


template <class T>
class ATL_NO_VTABLE 	CMenuDropTargetWindowImpl
	: public CWindowImpl<T>
	, public IDropTargetImpl<T>
	, public IDropSourceImpl<T>
{
	enum {
		MNGOF_CENTERGAP 	= 0,
	};

	HMENU			m_hDDMenu;
	bool			m_bDragAccept;
	//+++ char		_text[MAX_PATH];		//+++ 未使用

	struct DragMenuData {
		int 		nPos;
		DWORD		dwPos;
		HMENU		hMenu;
		CString 	strFilePath;
		int 		nDir;
		BOOL		bBottom;
		HWND		hWndParent;		//+++ 親

	public:
		DragMenuData()
			: bBottom(FALSE)
			, hMenu(NULL)
		  #if 1 //*+++
			, nPos(0)
			, dwPos(0)
			, nDir(0)
			, hWndParent(0)	//+++
		  #endif
		{
		}
	};

	DragMenuData	m_SrcData;
	DragMenuData	m_DstData;


public:
	CMenuDropTargetWindowImpl()
		: m_hDDMenu(NULL)
	  #if 1 	//*+++
		, m_bDragAccept(0)
	  #endif
	{
		//+++ _text[0] = 0;		//+++未使用
	}


	void SetTargetMenu(HMENU hMenu)
	{
		m_hDDMenu	 = hMenu;
		MENUINFO		info			= { sizeof (MENUINFO) };
		info.fMask	 = MIM_STYLE | MIM_APPLYTOSUBMENUS;
		info.dwStyle = MNS_DRAGDROP;
		::SetMenuInfo(hMenu, &info);
		HMENU hSubMenu = ::GetSubMenu(hMenu, 2);

		if (hSubMenu) {
			info.dwStyle = 0;
			::SetMenuInfo(hSubMenu, &info);
		}
	}


	BEGIN_MSG_MAP(CMenuDropTargetWindowImpl)
		MESSAGE_HANDLER(WM_CREATE		, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY		, OnDestroy)
		MESSAGE_HANDLER(WM_MENUDRAG 	, OnMenuDrag)
		MESSAGE_HANDLER(WM_MENUGETOBJECT, OnMenuGetObject)
	END_MSG_MAP()


	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		if (!m_bDragAccept)
			return DROPEFFECT_NONE;

		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
	}


	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		CString strText;

		if (  MtlGetHGlobalText( pDataObject, strText)
		   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
		{
			//ドロップ完了
			if (MoveItemOrder()) {							//メニュー項目の移動
				//+++ お気に入りメニューで移動したとき、お気に入りバーも更新されるようにしとく
				// CDonutExplorerBar::GetInstance()->RefreshExpBar(0);
				Donut_ExplorerBar_RefreshFavoriteBar();		//強引
			}
			return DROPEFFECT_NONE;
		}

		return DROPEFFECT_NONE;
	}



	LRESULT OnMenuDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (CFavoritesMenuOption::s_bCstmOrder == false) return MND_ENDMENU;

		int 					nPos		= (int) wParam;
		CComPtr<IDataObject>	spDataObject;

		HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL,IID_IDataObject, (void **) &spDataObject);
		if ( FAILED(hr) )
			return MND_CONTINUE;

		CHlinkDataObject *	 pHlinkDataObject = NULL;	//リリースの必要なし

		hr		  = spDataObject->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);
		if ( FAILED(hr) )
			return MND_CONTINUE;

		HMENU			hMenu	= (HMENU) lParam;
		MENUITEMINFO	mii 	= { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_DATA;

		if ( !::GetMenuItemInfo(hMenu, nPos, TRUE, &mii) )
			return MND_CONTINUE;

		CMenuItem * 	 pmi	= (CMenuItem *) mii.dwItemData;

		if (!pmi)
			return MND_CONTINUE;

		CString 		strFileName   = pmi->m_strText;
		CString 		strUrl		  = pmi->m_strPath;

		MTL::ParseInternetShortcutFile(strUrl);

		pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(strFileName, strUrl) );

		if ( PreDoDragDrop(m_hWnd) && CheckFolder(pmi->m_strPath) ) {
			//位置（元）を保存
			HMENU	   hMenu	  = (HMENU) lParam;
			int 	   nOffSet;

			if ( !GetPosOffset(hMenu, nOffSet) )
				return MND_CONTINUE;

			m_SrcData.nPos		  = nPos;
			m_SrcData.dwPos 	  = nPos - nOffSet;
			m_SrcData.hMenu 	  = hMenu;
			m_SrcData.strFilePath = pmi->m_strPath;
			m_SrcData.hWndParent  = GetParent();		//++++

			MtlRemoveTrailingBackSlash(m_SrcData.strFilePath);
			//ドラッグ開始
			DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);

			if (dropEffect != DROPEFFECT_NONE)
				return MND_ENDMENU;
		}

		return MND_CONTINUE;
	}


	LRESULT OnMenuGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		MENUGETOBJECTINFO *pInfo = (MENUGETOBJECTINFO *) lParam;

		//pInfoのdwFlagsはMNGOF_CENTERCGAP(0)のときはメニューの中央部にカーソルがあることを意味するが
		//それ以外の時はメニューの上部にあることを示す。MSDNには例によって嘘がかいてある。

	  #if 1	//+++	フォルダ間移動を許す
		if (CheckMenuPosition(pInfo->hmenu, pInfo->uPos)) {
			CComPtr<IDropTarget> pTarget;
			_LocDTQueryInterface(IID_IDropTarget, (void **) &pTarget);

			if (!pTarget)
				return MNGO_NOINTERFACE;

			if (pInfo->hmenu == m_SrcData.hMenu) {		 //まだフォルダ間移動はサポートしない
				//自分自身は受け付けない
				if (pInfo->uPos == m_SrcData.nPos && m_SrcData.hMenu == pInfo->hmenu)
					return MNGO_NOINTERFACE;

				if (pInfo->uPos == m_SrcData.nPos + 1 && pInfo->dwFlags != MNGOF_CENTERGAP)
					return MNGO_NOINTERFACE;

				if (pInfo->dwFlags != MNGOF_CENTERGAP) {
					if (m_SrcData.nPos < (int) pInfo->uPos)
						pInfo->uPos--;
				}
			}

			//位置（先）を保存
			int 	nOffSet;
			if ( !GetPosOffset(pInfo->hmenu, nOffSet) )
				return MNGO_NOINTERFACE;

			if ( (int) pInfo->uPos - nOffSet < 0 )
				pInfo->uPos++;

			m_DstData.nPos			= pInfo->uPos;
			m_DstData.dwPos			= pInfo->uPos - nOffSet;
			m_DstData.hMenu			= pInfo->hmenu;
			m_DstData.hWndParent	= GetParent();		//++++

			CMenuItem * 	 pItem	= GetMenuData(pInfo->hmenu, pInfo->uPos);

			if (pItem)
				m_DstData.strFilePath = pItem->m_strPath;
			else
				m_DstData.strFilePath = _T("");

			m_DstData.nDir	= pInfo->dwFlags;

			if (::GetMenuItemCount(pInfo->hmenu) == pInfo->uPos) {
				m_DstData.bBottom	  = TRUE;
				m_DstData.strFilePath = MtlGetDirectoryPath( GetDirPathForBottom(pInfo->hmenu, pInfo->uPos) );
			} else {
				m_DstData.bBottom = FALSE;
			}

			MtlRemoveTrailingBackSlash(m_DstData.strFilePath);

			pInfo->pvObj	= pTarget;
			return MNGO_NOERROR;
		}
		return MNGO_NOINTERFACE;
	  #else
		if (CheckMenuPosition(pInfo->hmenu, pInfo->uPos)
		   && pInfo->hmenu == m_SrcData.hMenu)		 //まだフォルダ間移動はサポートしない
		{
			CComPtr<IDropTarget> pTarget;
			_LocDTQueryInterface(IID_IDropTarget, (void **) &pTarget);

			if (!pTarget)
				return MNGO_NOINTERFACE;

			//自分自身は受け付けない
			if (pInfo->uPos == m_SrcData.nPos && m_SrcData.hMenu == pInfo->hmenu)
				return MNGO_NOINTERFACE;

			if (pInfo->uPos == m_SrcData.nPos + 1 && pInfo->dwFlags != MNGOF_CENTERGAP)
				return MNGO_NOINTERFACE;

			if (pInfo->dwFlags != MNGOF_CENTERGAP) {
				if (m_SrcData.nPos < (int) pInfo->uPos)
					pInfo->uPos--;
			}

			//位置（先）を保存
			int 	nOffSet;
			if ( !GetPosOffset(pInfo->hmenu, nOffSet) )
				return MNGO_NOINTERFACE;

			if ( (int) pInfo->uPos - nOffSet < 0 )
				pInfo->uPos++;

			m_DstData.nPos	= pInfo->uPos;
			m_DstData.dwPos = pInfo->uPos - nOffSet;
			m_DstData.hMenu = pInfo->hmenu;
			CMenuItem * 		 pItem = GetMenuData(pInfo->hmenu, pInfo->uPos);

			if (pItem)
				m_DstData.strFilePath = pItem->m_strPath;
			else
				m_DstData.strFilePath = _T("");

			m_DstData.nDir	= pInfo->dwFlags;

			if (::GetMenuItemCount(pInfo->hmenu) == pInfo->uPos) {
				m_DstData.bBottom	  = TRUE;
				m_DstData.strFilePath = MtlGetDirectoryPath( GetDirPathForBottom(pInfo->hmenu, pInfo->uPos) );
			} else {
				m_DstData.bBottom = FALSE;
			}

			MtlRemoveTrailingBackSlash(m_DstData.strFilePath);

			pInfo->pvObj	= pTarget;
			return MNGO_NOERROR;
		}
		return MNGO_NOINTERFACE;
	  #endif
	}


private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		RegisterDragDrop();
		return 0;
	}


	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		RevokeDragDrop();
		return 0;
	}


	BOOL CheckFolder(const CString& strDirPath) const
	{
		//ユーザー定義の場合は働かない
		if ( _check_flag(CFavoritesMenuOption::s_dwStyle, EMS_USER_DEFINED_FOLDER) )
			return FALSE;

		//お気に入りフォルダ以下でない場合も働かない
		CString 	strFavRoot;
		if ( !MtlGetFavoritesFolder(strFavRoot) )
			return FALSE;

		if (strDirPath.Find(strFavRoot) == -1)
			return FALSE;

		//IEの並び順でない場合も動かない
		if (CFavoritesMenuOption::s_bIEOrder == false)
			return FALSE;

		return TRUE;
	}


	CString GetDirPathForBottom(HMENU hMenu, UINT uPos)
	{
		MENUITEMINFO mii = { sizeof (MENUITEMINFO) };

		mii.fMask = MIIM_DATA;

		for (int i = uPos - 1; i >= 0; i--) {
			if ( !::GetMenuItemInfo(hMenu, i, TRUE, &mii) )
				return _T("");

			CMenuItem *pItem = (CMenuItem *) mii.dwItemData;

			if (pItem)
				return pItem->m_strPath;
		}

		return _T("");
	}


	CMenuItem *GetMenuData(HMENU hMenu, UINT uPos)
	{
		MENUITEMINFO	mii = { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_TYPE | MIIM_DATA;

		if ( !::GetMenuItemInfo(hMenu, uPos, TRUE, &mii) )
			return NULL;

		return (CMenuItem *) mii.dwItemData;
	}


	BOOL GetPosOffset(HMENU hMenu, int &nPosOffset)
	{
		MENUITEMINFO mii	= { sizeof (MENUITEMINFO) };

		mii.fMask = MIIM_TYPE | MIIM_DATA;
		int 		 nCount = ::GetMenuItemCount(hMenu);

		for (int i = 0; i < nCount; i++) {
			if ( !::GetMenuItemInfo(hMenu, i, TRUE, &mii) )
				return FALSE;

			CMenuItem *pItem = (CMenuItem *) mii.dwItemData;

			if (pItem) {
				nPosOffset = i;
				return TRUE;
			}
		}

		return FALSE;
	}


	BOOL CheckMenuPosition(HMENU hMenu, UINT uPos)
	{
		MENUITEMINFO mii	= { sizeof (MENUITEMINFO) };

		mii.fMask = MIIM_TYPE | MIIM_DATA;
		int 		 nCount = ::GetMenuItemCount(hMenu);

		if (nCount == uPos)
			return TRUE;

		//uPos番目の項目をチェック:セパレータから上にある場合はFALSE;
		if ( !::GetMenuItemInfo(hMenu, uPos, TRUE, &mii) )
			return FALSE;

		CMenuItem *  pItem	= (CMenuItem *) mii.dwItemData;

		if (!pItem || mii.fType == MFT_SEPARATOR /*|| !MtlIsInternetFile(pItem->m_strPath)*/)
			return FALSE;

		return TRUE;
	}


	///+++ 項目移動
	bool	MoveItemOrder()
	{
		if (CFavoritesMenuOption::s_bCstmOrder == false) return false;

		CString strSrcDir = MtlGetDirectoryPath(m_SrcData.strFilePath);
		if (m_SrcData.bBottom)	//+++ メニューの一番最後の場合は、次のurlのファイル名でなく、そのフォルダ名のみが入っている.
			strSrcDir = m_SrcData.strFilePath;
		CString strDstDir = MtlGetDirectoryPath(m_DstData.strFilePath);
		if (m_DstData.bBottom)	//+++ メニューの一番最後の場合は、次のurlのファイル名でなく、そのフォルダ名のみが入っている.
			strDstDir = m_DstData.strFilePath;

		if (m_SrcData.hMenu == m_DstData.hMenu) {
			//同ディレクトリ内移動
			CFavoriteOrderHandler	order;
			order.ReadData(strSrcDir);

			//データのチェック:無ければ再構築させて、もういっぺん確認する
			if (GetOrderItemIndex(order, m_SrcData) == -1)
				return false;

			if (!m_DstData.bBottom && GetOrderItemIndex(order, m_DstData) == -1)
				return false;

			//移動して保存
			if (m_DstData.bBottom)
				m_DstData.dwPos--;

			order.MoveData(m_SrcData.dwPos, m_DstData.dwPos);
			order.SaveData();

			//今表示しているメニュー自体の並べ替え
			if (m_DstData.bBottom)
				m_DstData.nPos--;

			MoveMenuItems(m_SrcData.hMenu, m_SrcData.nPos, m_DstData.nPos);
			::InvalidateRect( (HWND) m_DstData.hMenu, NULL, TRUE );
			::DrawMenuBar( GetParent() );
		} else {
			//異なるディレクトリ間の移動
		  #if 1	//+++	かなり無理やり...
			CFavoriteOrderHandler	srcOrder;
			srcOrder.ReadData(strSrcDir);
			CFavoriteOrderHandler	dstOrder;
			dstOrder.ReadData(strDstDir);
			if (strSrcDir == strDstDir)
				return false;

			//データのチェック:無ければ再構築させて、もういっぺん確認する
			if (m_SrcData.bBottom)
				;
			else if (GetOrderItemIndex(srcOrder, m_SrcData) == -1)
				return false;

			//移動して保存
			if (m_DstData.bBottom)
				;	//--m_DstData.dwPos;
			else if (GetOrderItemIndex(dstOrder, m_DstData) == -1)
				return false;

		  #if 1	//+++
			dstOrder.MoveData(srcOrder, m_SrcData.strFilePath, m_DstData.dwPos);
		  #elif 1
			CString name     = GetFileBaseName(m_SrcData.strFilePath);
			//CString dstName= (m_SrcData.bBottom) ? m_DstData.strFilePath : GetDirName(m_DstData.strFilePath);
			//dstName		+= _T('\\') + name;
			CString dstName  = strDstDir + _T('\\') + name;
			bool	bCopy	 = false;
			if (::MoveFile( m_SrcData.strFilePath, dstName ) == 0) {
				ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、ファイル移動失敗(%s)\n"), LPCTSTR(name));
				return false;
			} else {
				bCopy = ::CopyFile(dstName, m_SrcData.strFilePath, TRUE) != 0;	//+++ 項目の削除時の実体として、ダミーを作成
			}
			if (srcOrder.DeleteData( m_SrcData.strFilePath, true ) == 0) {
				ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、元フォルダ側で削除がされなかった(%s)\n"), LPCTSTR(name));
			}
			if (bCopy)	//+++ もし何か失敗していた場合のための削除.
				::DeleteFile(m_SrcData.strFilePath);
			if (dstOrder.AddData(m_SrcData.strFilePath, m_DstData.dwPos) == 0) {
				ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、行き先への追加失敗(%s)\n"), LPCTSTR(name));
				return false;
			}
		  #else
			//srcOrder.MoveData(m_SrcData.dwPos, dstOrder, m_DstData.dwPos);
		  #endif
			srcOrder.SaveData();
			dstOrder.SaveData();

			//今表示しているメニュー自体の並べ替え
			//if (m_SrcData.bBottom)
			//	m_SrcData.nPos--;
			MoveMenuItems(m_SrcData.hMenu, m_SrcData.nPos, m_DstData.hMenu, m_DstData.nPos);
			::InvalidateRect( (HWND) m_SrcData.hMenu, NULL, TRUE );
			::InvalidateRect( (HWND) m_DstData.hMenu, NULL, TRUE );
			::DrawMenuBar( m_SrcData.hWndParent );
			::DrawMenuBar( GetParent() );
			m_SrcData = m_DstData;			//+++ 移動した直後に、さらに移動するとき用の辻褄あわせ
		  #endif
		}
		return true;
	}


	int GetOrderItemIndex(CFavoriteOrderHandler &order, DragMenuData &data)
	{
		int 	nIndex;
		int 	dwPosition = data.dwPos;

		nIndex = order.FindPosition(dwPosition);

		if (nIndex == -1) {
			order.ReConstructData(order.DirPath());				//+++ メンバー変数m_strDirPathを関数DirPath()に変更.
			nIndex = order.FindPosition(dwPosition);
			if (nIndex == -1) {
				nIndex	= order.FindName(data.strFilePath);
				if (nIndex == -1)
					return -1;
				//データが壊れちゃってるので補修
				//+++ order.m_aryData[nIndex].dwPosition = dwPosition;
				order.setAryData_position(nIndex, dwPosition);	//+++ メンバー変数を隠す
			}
		}

		return nIndex;
	}


	void MoveMenuItems(HMENU hMenu, int nSrcPos, int nDstPos)
	{
		if (nSrcPos == nDstPos)
			return;

		if (nSrcPos < 0 || nDstPos < 0)
			return;

		MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
		TCHAR		 buf[MAX_PATH] = _T("\0");
		mii.fMask	   = MIIM_TYPE | MIIM_STATE | MIIM_CHECKMARKS | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = buf;
		mii.cch 	   = MAX_PATH;
		::GetMenuItemInfo(hMenu, nSrcPos, TRUE, &mii);
		::RemoveMenu(hMenu, nSrcPos, MF_BYPOSITION);
		::InsertMenuItem(hMenu, nDstPos, TRUE, &mii);
	}


	void MoveMenuItems(HMENU hSrcMenu, int nSrcPos, HMENU hDstMenu, int nDstPos)
	{
		if (nSrcPos < 0 || nDstPos < 0 || hSrcMenu == 0 || hDstMenu == 0)
			return;
		if (hSrcMenu == hDstMenu)
			return MoveMenuItems(hSrcMenu, nSrcPos, nDstPos);

		MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
		TCHAR		 buf[MAX_PATH] = _T("\0");
		mii.fMask	   = MIIM_TYPE | MIIM_STATE | MIIM_CHECKMARKS | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = buf;
		mii.cch 	   = MAX_PATH;
		::GetMenuItemInfo(hSrcMenu, nSrcPos, TRUE, &mii);
		::RemoveMenu(hSrcMenu, nSrcPos, MF_BYPOSITION);
		::InsertMenuItem(hDstMenu, nDstPos, TRUE, &mii);
	}
};



class CMenuDropTargetWindow : public CMenuDropTargetWindowImpl<CMenuDropTargetWindow> {
public:
	DECLARE_WND_SUPERCLASS( _T("Donut_MenuDDTarget"), GetWndClassName() )
};

