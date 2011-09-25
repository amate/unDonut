/**
 *	@file	ExplorerTreeViewCtrl.h
 *	@brief	エクスプローラ・バーでのツリー表示
 */


#if !defined (AFX_SHVIEW_DEMOVIEW_H__0DD77E8E_1C9C_11D5_8A9C_9D5C3CCEE371__INCLUDED_)
#define AFX_SHVIEW_DEMOVIEW_H__0DD77E8E_1C9C_11D5_8A9C_9D5C3CCEE371__INCLUDED_

#include "ItemIDList.h"
#include "MtlCom.h"
#include "DonutDefine.h"
#include "option/MainOption.h"
#include "option/ExplorerBarDialog.h"
#include "DonutPfunc.h" 		//+++


#pragma once

// for debug
#ifdef _DEBUG
	const bool _Mtl_ExplorerTreeView_traceOn = false;
	#define etvTRACE	if (_Mtl_ExplorerTreeView_traceOn) ATLTRACE
#else
	#define etvTRACE
#endif


// Extended styles
enum EEtv_Ex {
	ETV_EX_LOADREMOTEICON	=	0x00000001L,
	ETV_EX_SINGLEEXPAND 	=	0x00000002L,
	ETV_EX_HISTORY_TIME 	=	0x00000004L,
	ETV_EX_USER 			=	0x00010000L,

	// Options
	ETV_EX_MYCOMPUTER		=	0x00010000L,
	ETV_EX_FAVORITES		=	0x00020000L,
	ETV_EX_FAVORITEGROUP	=	0x00040000L,
	ETV_EX_USERDEFINED		=	0x00080000L,
	ETV_EX_SHELLEXECUTE 	=	0x00100000L,
	ETV_EX_HISTORY			=	0x00200000L,	// UDT DGSTR りれき
	ETV_EX_SCRIPT			=	0x00400000L,	// UH Script

	ETV_EX_FOLDERS			=	(ETV_EX_MYCOMPUTER | ETV_EX_FAVORITES | ETV_EX_FAVORITEGROUP | ETV_EX_USERDEFINED | ETV_EX_HISTORY | ETV_EX_SCRIPT),
};



template <class T>
class CExplorerTreeViewCtrlImpl : public CWindowImpl<T, CTreeViewCtrl> 
{
public:
	DECLARE_WND_SUPERCLASS( NULL, CTreeViewCtrl::GetWndClassName() )

public:
	CItemIDList 			m_idlRootFolder;
	CImageList				m_imgDonut;
	DWORD					m_dwExplorerTreeViewExtendedStyle;
	bool					m_bOrgImage;

	// Constants
	enum { s_nIDExpand	= 1, s_nIDCollapse = 2, s_nIDNewFolder = 3, s_nIDOpen = 4, s_nIDExProp = 5 /*minit*/, s_nIDLast = 100 };

private:
	// Data members
	CItemIDList 			m_idlHtm;			// used to draw .url icon faster
	CComPtr<IShellFolder>	m_spDesktopFolder;
	CComPtr<IContextMenu2>	m_spContextMenu2;
	BYTE/*bool*/			m_bDestroying;
	BYTE/*BOOL*/			m_bSettingFocus;

	enum { s_kcxItemGap = 4 };

public:
	// Ctor/Detor
	CExplorerTreeViewCtrlImpl()
		: m_bDestroying(false)
		, m_dwExplorerTreeViewExtendedStyle(0)
		, m_bOrgImage(0)				//+++
		, m_bSettingFocus(FALSE)
	{
		m_idlHtm = MtlGetHtmlFileIDList();
	}


	~CExplorerTreeViewCtrlImpl()
	{
		MtlDeleteHtmlFileIDList();
	}


	BOOL PreTranslateMessage(MSG * /*pMsg*/)
	{
		return FALSE;
	}


private:
	bool _IsFavorites()
	{
		return _check_flag( ETV_EX_FAVORITES, GetExplorerTreeViewExtendedStyle() );
	}


public:
	// Attributes
	void ModifyExplorerTreeViewExtendedStyle(DWORD dwRemove, DWORD dwAdd)
	{
		m_dwExplorerTreeViewExtendedStyle = (m_dwExplorerTreeViewExtendedStyle & ~dwRemove) | dwAdd;

		if ( _check_flag(ETV_EX_SINGLEEXPAND, m_dwExplorerTreeViewExtendedStyle) )
			ModifyStyle(0, TVS_SINGLEEXPAND);
		else
			ModifyStyle(TVS_SINGLEEXPAND, 0);
	}


	void SetExplorerTreeViewExtendedStyle(DWORD dwStyle)
	{
		m_dwExplorerTreeViewExtendedStyle = dwStyle;

		if ( _check_flag(ETV_EX_SINGLEEXPAND, m_dwExplorerTreeViewExtendedStyle) )
			ModifyStyle(0, TVS_SINGLEEXPAND);
		else
			ModifyStyle(TVS_SINGLEEXPAND, 0);
	}


	DWORD GetExplorerTreeViewExtendedStyle() const
	{
		return m_dwExplorerTreeViewExtendedStyle;
	}


private:
	// Overridables
	CItemIDList OnInitRootFolder()
	{
		CItemIDList idl;
		HRESULT 	hr = ::SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &idl);

		return idl;
	}


	void OnGetAdditionalTreeItems(CSimpleArray<TV_INSERTSTRUCT> &/*items*/, LPCITEMIDLIST /*pidl*/)
	{
	}


public:
	void OnInitialUpdateTreeItems(CSimpleArray<TV_INSERTSTRUCT> &items, LPCITEMIDLIST /*pidl*/)
	{
		std::sort( _begin(items), _end(items), _DefaultTreeItemCompare() );
	}


private:
	void OnOpenTreeItem(HTREEITEM hItem, LPITEMIDLIST pidl)
	{
		MtlShellExecute(m_hWnd, pidl);
	}


public:
	void OnTreeItemClicked(HTREEITEM hTreeItem, UINT uFlags)
	{
		CItemIDList idl = _GetItemIDList(hTreeItem);

		if ( !idl.IsNull() )
			MtlShellExecute(m_hWnd, idl);
	}


private:
	void OnExplorerTreeViewInit()
	{
	}


	void OnExplorerTreeViewTerm()
	{
	}


	bool OnGetInfoToolTip(HTREEITEM /*hItem*/, CString &/*strTip*/, int /*cchTextMax*/)
	{
		return false;
	}


	UINT OnPrepareMenuForContext(HTREEITEM /*hTreeItem*/, CMenuHandle /*menu*/, int &/*nPos*/)
	{
		return CMF_EXPLORE | CMF_CANRENAME;
	}


	void OnPrepareMenuForPopup(HTREEITEM /*hTreeItem*/, CMenuHandle /*menu*/)
	{
	}


	bool OnCmdContextMenu(HTREEITEM /*hTreeItem*/, int /*nID*/)
	{
		return false;
	}


	// U.H
	void OnFolderEmpty()
	{
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CExplorerTreeViewCtrlImpl)
		MESSAGE_HANDLER (	WM_CREATE		, OnCreate			)
		MESSAGE_HANDLER (	WM_DESTROY		, OnDestroy 		)
		HANDLE_MENU_MESSAGE_CONTEXTMENU( m_spContextMenu2		)
		MESSAGE_HANDLER (	WM_RBUTTONDOWN	, OnRButtonDown 	)
		MESSAGE_HANDLER (	WM_RBUTTONUP	, OnRButtonUp		)
		MESSAGE_HANDLER (	WM_KEYDOWN		, OnKeyDown 		)
		MESSAGE_HANDLER (	WM_ERASEBKGND	, OnEraseBackground )
		MESSAGE_HANDLER (	WM_SETFOCUS 	, OnSetFocus		)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		// MESSAGE_HANDLER(WM_LBUTTONDBLCLK , OnLButtonDblClk	)
		NOTIFY_CODE_HANDLER(TVN_GETDISPINFO , OnGetDispInfo 	)
		NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING,OnItemExpanding	)
		NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDED, OnItemExpanded	)
		NOTIFY_CODE_HANDLER(TVN_ENDLABELEDIT, OnEndLabelEdit	)
		NOTIFY_CODE_HANDLER(TVN_GETINFOTIP	, OnGetInfoTip		)
	END_MSG_MAP()


private:
	LRESULT OnMouseWheel(UINT fwKeys, short zDelta, CPoint point)
	{
		// I've never touched a wheel mouse.
		if ( IsWindowVisible() ) {
			CRect rc;
			GetWindowRect(&rc);

			if ( rc.PtInRect(point) ) {
				SetMsgHandled(FALSE);	// pass to tree view control
				return 1;
			}
		}

		SetMsgHandled(TRUE);	// eat it
		return	1;				// but not processed
	}


	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		RECT rect;

		GetClientRect(&rect);
		::FillRect( (HDC) wParam, &rect, (HBRUSH) LongToPtr(COLOR_WINDOW + 1) );

		return 1;										// no background needed
	}


public:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		etvTRACE( _T("OnCreate in\n") );
		LRESULT lRet = DefWindowProc();
		m_bOrgImage = (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_FAV_ORGIMG) != 0 /*? true : false*/;

		_SetSystemImageList();

		HRESULT hr	 = ::SHGetDesktopFolder(&m_spDesktopFolder);

		if ( FAILED(hr) )
			return lRet;

		SetActiveWindow();

		T * 	pT	 = static_cast<T *>(this);
		pT->OnExplorerTreeViewInit();					// before _InitRootTree()!

		_InitRootTree();

		if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_NOSPACE) != MAIN_EXPLORER_NOSPACE )
			SetItemHeight(GetItemHeight() + s_kcxItemGap);

		etvTRACE( _T("OnCreate out\n") );
		return lRet;
	}


private:
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled	  = FALSE;
		m_bDestroying = true;

		T *pT = static_cast<T *>(this);
		pT->OnExplorerTreeViewTerm();

		// Make Cash
		if (_IsFavorites() && (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MAKECASH))
			MakeCash();

		etvTRACE( _T("OnDestroy(%d)\n"), GetCount() );
		_CleanUpAll();

		return 0;
	}


	void MakeCash()
	{
		HTREEITEM	hItem	  = GetRootItem();
		//+++ なぜか、Destroyが2回呼ばれるようで2回目を無視するためのチェック
		//+++ 2回目でGetCount()が0を返しそれを書き込むため、次起動すると、
		//+++ エクスプローラのお気に入りの項目が表示されないというバグになっていた.
		if (hItem == 0)
			return;

		CIniFileO	pr( _GetFilePath( _T("FavCash.ini") ), _T("Fav") );

		CString 	strKey;
		DWORD		dwTreeCnt = GetCount();
		pr.SetValue( dwTreeCnt, _T("TreeCount") );

		int 		nIndex	  = 0;
		while (hItem != NULL) {
			CItemIDList idl = _GetItemIDList(hItem);
			hItem	= GetNextItem(hItem, TVGN_NEXT);	// cache

			CString strPath = idl.GetPath();
			strKey.Format(_T("Path%d"), nIndex++);
			pr.SetStringUW(strPath, strKey);
		}
	}


	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		int nVirtKey = (int) wParam;

		if (nVirtKey == VK_RETURN || nVirtKey == VK_SPACE) {
			HTREEITEM hItem = GetSelectedItem();

			if (hItem == NULL)
				return 0;

			if ( _IsItemFolder(hItem) ) {
				bHandled = FALSE;
				return 1;
			}

			T * 	  pT	= static_cast<T *>(this);
			//			pT->OnOpenTreeItem(hItem, _GetItemIDList(hItem));
			pT->OnTreeItemClicked(hItem, 0);
		} else {
			bHandled = FALSE;
			return 1;
		}

		return 0;
	}

  #if 1	//* +++ 未使用かも
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &bHandled)
	{
		POINT	  pt	= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		HTREEITEM hItem = _HitTestForFullLow(pt);

		if (hItem == NULL)
			return 0;

		MtlShellExecute( m_hWnd, _GetItemIDList(hItem) );
		//		GetUrlFromHistory(hItem);

		return 0;
	}
  #endif

public:
	HTREEITEM _HitTestForFullLow(CPoint pt)
	{
		UINT	  flag;
		HTREEITEM hItem = HitTest(pt, &flag);

		if ( hItem == NULL || !_CheckTVHTFlag_For_FullLow(flag) )
			return NULL;
		else
			return hItem;
	}


private:
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &bHandled)
	{
		POINT	  pt			= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		HTREEITEM hItem 		= _HitTestForFullLow(pt);

		if (hItem == NULL)
			return 0;

		EndEditLabelNow(TRUE);					// cancel

		HTREEITEM hItemSelected = GetSelectedItem();

		bool	  bHilight		= (hItem != hItemSelected);

		if (bHilight)							// hilight item
			SetItem(hItem, TVIF_STATE, NULL, 0, 0, TVIS_SELECTED , TVIS_SELECTED, NULL);

		//		SelectItem(hItem);
		HTREEITEM hItemExpanded = NULL;
		_PopupContextMenu(hItem, pt, hItemExpanded);
		//		SelectItem(NULL);

		if (hItemExpanded == NULL && bHilight)	// reset hilight
			SetItem(hItem, TVIF_STATE, NULL, 0, 0, 0, TVIS_SELECTED /*TVIS_SELECTED*/, NULL);

		return 0;
	}


	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &bHandled)
	{
		POINT	  pt			= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		HTREEITEM hItem 		= _HitTestForFullLow(pt);

		if (hItem == NULL) {
			_PopupContextMenu_blank(pt);
			return 0;
		}

		HTREEITEM hItemExpanded = NULL;
		_PopupContextMenu(hItem, pt, hItemExpanded);

		return 0;
	}


	LRESULT OnItemExpanding(int, LPNMHDR lpnmhdr, BOOL &)
	{
		if (m_bSettingFocus)
			return 0;

		if (m_bDestroying)
			return 0;

		LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lpnmhdr;

		if (pnmtv->action == TVE_EXPAND) {
			TVITEM &	 item		= pnmtv->itemNew;

			LPITEMIDLIST pidlFolder = _GetItemIDList(item.hItem);

			if (pidlFolder == NULL)
				return 0;

			CItemIDList  idl		= pidlFolder;
			CString 	 strID		= idl.GetPath();


			_CleanUpChildren(item.hItem);
			_AddChildren(item.hItem);
		}

		//		else if (pnmtv->action == TVE_COLLAPSE) {
		//			etvTRACE(_T("collapse\n"));
		//			TVITEM& item = pnmtv->itemNew;
		//			_CleanUpChildren(item.hItem);
		//		}

		return 0;
	}


	LRESULT OnItemExpanded(int, LPNMHDR lpnmhdr, BOOL &)
	{
		if (m_bDestroying)
			return 0;

		LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lpnmhdr;

		if (pnmtv->action == TVE_COLLAPSE) {
			etvTRACE( _T("collapse\n") );
			TVITEM &	 item		= pnmtv->itemNew;

			LPITEMIDLIST pidlFolder = _GetItemIDList(item.hItem);

			if (pidlFolder == NULL)
				return 0;

			_CleanUpChildren(item.hItem);
			::ShowScrollBar(m_hWnd, SB_HORZ, FALSE);
		}

		return 0;
	}


	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &bHandled)
	{
		m_bSettingFocus 	= TRUE;
		LRESULT 	lRet	= DefWindowProc();
		m_bSettingFocus 	= FALSE;
		return		lRet;
	}


	LRESULT OnEndLabelEdit(int, LPNMHDR lpnmhdr, BOOL &)
	{
		NMTVDISPINFO *	pnmv = (NMTVDISPINFO *) lpnmhdr;
		TVITEM &		item = pnmv->item;

		if (item.pszText != NULL) {
			CItemIDList 	idl 		= _GetItemIDList(item.hItem);					// full path of file
			CItemIDList 	idlFolder	= _GetItemIDList( GetParentItem(item.hItem) );	// full path of folder
			idl -= idlFolder;																// get file name

			if ( idl.IsNull() )
				return 0;

			CComPtr<IShellFolder> spFolder;
			HRESULT 	hr	= m_spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);

			if ( FAILED(hr) )
				return 0;

			CItemIDList idlRet	= MtlSetDisplayName(spFolder, idl, item.pszText, m_hWnd);

			if ( idlRet.IsNull() ) {	// failed
				EditLabel(item.hItem);
			} else {
				CItemIDList idlNew = idlFolder + idlRet;

				TVITEM		tvi = {0};
				tvi.mask	= TVIF_HANDLE | TVIF_PARAM;
				tvi.hItem	= item.hItem;
				GetItem(&tvi);
				CItemIDList::FreeIDList( (LPITEMIDLIST) tvi.lParam );
				tvi.lParam	= (LPARAM) idlNew.Detach();

				tvi.mask   |= TVIF_TEXT;
				//				SetItemText(item.hItem, item.pszText);

				tvi.pszText = item.pszText;
				SetItem(&tvi);

				_RefreshChildren(item.hItem);
			}
		}

		return 0;
	}


	LRESULT OnGetInfoTip(int, LPNMHDR lpnmhdr, BOOL &)
	{
		LPNMTVGETINFOTIP	lpGetInfoTip = (LPNMTVGETINFOTIP) lpnmhdr;
		CString 			strTip;

		ATLASSERT(lpGetInfoTip->pszText != NULL);

		T * 				pT			 = static_cast<T *>(this);

		if ( pT->OnGetInfoToolTip(lpGetInfoTip->hItem, strTip, lpGetInfoTip->cchTextMax) )
			::lstrcpyn(lpGetInfoTip->pszText, strTip, lpGetInfoTip->cchTextMax);

		return 0;
	}


	LRESULT OnGetDispInfo(int, LPNMHDR lpnmhdr, BOOL &)
	{
		NMTVDISPINFO *pnmv		= (NMTVDISPINFO *) lpnmhdr;
		TVITEM &	  item		= pnmv->item;

		if ( !( item.mask & (TVIF_SELECTEDIMAGE | TVIF_IMAGE) ) )
			return 0;

		CItemIDList   idl		= (LPCITEMIDLIST) item.lParam;
		bool		  bExpanded = _IsItemExpanded(item.hItem);
		bool		  bFolder	= _IsItemFolder(item.hItem);

		LPCITEMIDLIST pidl;

		if ( _check_flag(ETV_EX_LOADREMOTEICON, m_dwExplorerTreeViewExtendedStyle) )
			pidl = NULL;
		else
			pidl = m_idlHtm;

		if (bFolder) {	// if folder, never mind selected or not
			if (bExpanded) {
				item.iImage 		= MtlGetSelectedIconIndex(idl, bFolder, pidl);
				item.iSelectedImage = item.iImage;
			} else {
				item.iImage 		= MtlGetNormalIconIndex(idl, pidl);
				item.iSelectedImage = item.iImage;
			}
		} else {
			// Note. item.mask is vague
			if (item.mask & TVIF_SELECTEDIMAGE) {
				item.iSelectedImage = MtlGetSelectedIconIndex(idl, bFolder, pidl);
			}

			if (item.mask & TVIF_IMAGE) {
				item.iImage = MtlGetNormalIconIndex(idl, pidl);
			}

			_NoImageCallBack(item);
		}

		return 0;
	}


	void _SetSystemImageList()
	{
		ATLASSERT( ::IsWindow(m_hWnd) );

		if (m_bOrgImage) {
			CBitmap bmp;

			CString strPath = _GetSkinDir();
			strPath += _T("ExpFav.bmp");
			bmp.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );
			CSize	szBmp(16, 16);

			if (bmp.m_hBitmap)
				bmp.GetSize(szBmp);

			m_imgDonut.Create(szBmp.cy, szBmp.cy, ILC_COLOR24 | ILC_MASK, 4, 1);
			m_imgDonut.Add( bmp, RGB(255, 0, 255) );
		}

		SHFILEINFO sfi;
		HIMAGELIST hImgs = (HIMAGELIST) ::SHGetFileInfo(_T("C:\\"), 0, &sfi, sizeof (sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

		ATLASSERT(hImgs != NULL);
		SetImageList(hImgs, TVSIL_NORMAL);
	}


public:
	void ReloadSkin()
	{
		if ( !::IsWindow(m_hWnd) )
			return;

		if (m_imgDonut.m_hImageList) {
			CString strPath = _GetSkinDir();
			strPath += _T("ExpFav.bmp");
			_ReplaceImageList(strPath, m_imgDonut);
		}

		InvalidateRect(NULL, TRUE);
	}


private:
	void _InitRootTree()
	{
		m_bDestroying	= true;
		_CleanUpAll();

		T * 	  pT = static_cast<T *>(this);
		m_idlRootFolder = pT->OnInitRootFolder();

		CFindFile finder;

		if ( finder.FindFile( _GetFilePath( _T("FavCash.ini") ) ) ) {
			_AddCashFile();
			::DeleteFile( _GetFilePath( _T("FavCash.ini") ) );
		} else {
			_AddChildrenAux(NULL);
		}

		// U.H
		if (m_idlRootFolder == NULL)
			pT->OnFolderEmpty();

		SetFocus();
		SetScrollPos(SB_VERT, 0);

		m_bDestroying	= false;
	}


public:
	HWND SetFocus()
	{
		m_bSettingFocus = TRUE;
		HWND	   hRet = ::SetFocus(m_hWnd);
		m_bSettingFocus = FALSE;
		return hRet;
	}


private:
	void _AddCashFile()
	{
		CIniFileI	pr( _GetFilePath( _T("FavCash.ini") ), _T("Fav") );
		DWORD		dwTreeCnt = pr.GetValue(_T("TreeCount"), GetCount() );
		CString 	strKey;

		TV_INSERTSTRUCT 	tvis = {0};
		tvis.hParent			 = NULL;
		tvis.hInsertAfter		 = TVI_LAST;
		tvis.item.mask			 = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvis.item.iImage		 = I_IMAGECALLBACK; //MtlGetNormalIconIndex(idlFull, _pidlHtm);
		tvis.item.iSelectedImage = I_IMAGECALLBACK; //MtlGetSelectedIconIndex(idlFull, false, _pidlHtm);
		tvis.item.cChildren 	 = 1;

		for (int ii = 0; ii < (int) dwTreeCnt; ii++) {
			strKey.Format(_T("Path%d"), ii);
			CString strPath = pr.GetStringUW( strKey );
			if ( strPath.IsEmpty() )
				continue;

			CItemIDList idl(strPath);

			CString 	strTitle = strPath.Mid(strPath.ReverseFind('\\') + 1);
			CString 	strExt;
			int 		nFindIt  = strTitle.ReverseFind('.');
			bool		bFolder  = FALSE;

			if (nFindIt == -1) {
				bFolder = true;
			} else {
				strExt	 = strTitle.Mid(nFindIt + 1);
				strExt.MakeUpper();
				strTitle = strTitle.Left(nFindIt);
			}

			//	pszTextの型が悪いのだと思うが、pszTextの先が書き換えられることはないはず?なので、
			//	バッファを確保せずに、テキスト文字列へのポインタを渡す.
			tvis.item.pszText = (LPTSTR) LPCTSTR( strTitle );
			tvis.item.lParam  = (LPARAM) idl.m_pidl;

			if (bFolder) {
				tvis.item.mask |= TVIF_CHILDREN;

				if ( m_bOrgImage && _IsFavorites() ) {
					tvis.item.iImage		 = 0;
					tvis.item.iSelectedImage = 1;
				}
			} else {
				tvis.item.mask &= ~TVIF_CHILDREN;

				if ( m_bOrgImage && _IsFavorites() ) {
					if ( strExt == _T("URL") )
						tvis.item.iImage = tvis.item.iSelectedImage = 2;
					else
						tvis.item.iImage = tvis.item.iSelectedImage = 3;
				}
			}

			InsertItem(&tvis);
			idl.Detach();
		}
	}


	void _AddChildren(HTREEITEM hItemParent)
	{
		_AddChildrenAux(hItemParent);
	}


	void _AddChildrenAux(HTREEITEM hItemParent)
	{		// hItemParent can be NULL
		T * 			pT			= static_cast<T *>(this);
		LPITEMIDLIST	pidlFolder	= _GetItemIDList(hItemParent);
		if (pidlFolder == NULL)
			return;

		// folders
		CSimpleArray<TV_INSERTSTRUCT> items;

		{	// add additional items
			pT->OnGetAdditionalTreeItems(items, pidlFolder);
			int itemsSize = items.GetSize();		//+++
			for (int i = 0; i < itemsSize; ++i)
				InsertItem(&items[i]);

			for (int i = 0; i < itemsSize; ++i) {
				delete[] items[i].item.pszText;
				items[i].item.pszText = NULL;		//+++ 不要だが念のためクリア.
			}

			items.RemoveAll();
		}

		// for each object
		MtlForEachObject( pidlFolder, _Function_AddTreeItem(items, hItemParent, m_idlHtm) );
		pT->OnInitialUpdateTreeItems(items, pidlFolder);

		int itemsSize = items.GetSize();		//+++
		for (int i = 0; i < itemsSize; ++i) {
			if (items[i].item.cChildren == 1) {
				if ( m_bOrgImage && _IsFavorites() ) {
					items[i].item.iImage		 = 0;
					items[i].item.iSelectedImage = 1;
				}
			} else {
				if ( m_bOrgImage && _IsFavorites() ) {
					CItemIDList idl( (LPCITEMIDLIST) items[i].item.lParam );

					CString 	strText;
					strText = idl.GetPath();
					strText.MakeUpper();

					if (strText.Find( _T(".URL") ) != -1) {
						items[i].item.iImage		 = 2;
						items[i].item.iSelectedImage = 2;
					} else {
						items[i].item.iImage		 = 3;
						items[i].item.iSelectedImage = 3;
					}
				}
			}
			InsertItem(&items[i]);
		}

		for (int i = 0; i < itemsSize; ++i) {
			delete[] items[i].item.pszText;
			items[i].item.pszText = NULL;	//+++ 不要だけれど念のためクリア.
		}
	}


	struct _Function_AddTreeItem {
		CSimpleArray<TV_INSERTSTRUCT> &_arrTvis;
		HTREEITEM					   _hItemParent;
		LPCITEMIDLIST				   _pidlHtm;

		_Function_AddTreeItem(CSimpleArray<TV_INSERTSTRUCT> &arrTvis, HTREEITEM hItemParent, LPITEMIDLIST pidlHtm)
			:						   _arrTvis(arrTvis)
			  , 					   _hItemParent(hItemParent)
			  , 					   _pidlHtm(pidlHtm) { }

		void operator ()(IShellFolder *pFolder, const CItemIDList &idlFolder, const CItemIDList &idlFile, bool bFolder)
		{
			CString 		strText;

			MtlGetDisplayName(pFolder, idlFile, strText);

			LPTSTR			lpszText	 = NULL;
			ATLTRY(lpszText = new TCHAR[strText.GetLength() + 1]);
			if (lpszText == NULL)
				return;

			::_tcscpy_s(lpszText, strText.GetLength() + 1, strText);
			TV_INSERTSTRUCT tvis = {0};
			tvis.hParent			 = _hItemParent;
			tvis.hInsertAfter		 = TVI_LAST;
			tvis.item.mask			 = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvis.item.pszText		 = lpszText;
			tvis.item.iImage		 = I_IMAGECALLBACK; 						//MtlGetNormalIconIndex(idlFull, _pidlHtm);
			tvis.item.iSelectedImage = I_IMAGECALLBACK; 						//MtlGetSelectedIconIndex(idlFull, false, _pidlHtm);

			if (bFolder) {														// if folder, it can expand.
				tvis.item.mask	   |= TVIF_CHILDREN;
				tvis.item.cChildren = 1;
			} else {
				tvis.item.mask	   |= TVIF_CHILDREN;
				tvis.item.cChildren = 0;
			}

			// set up full item id list!
			CItemIDList 	idlFull 	 = idlFolder + idlFile;
			ATLASSERT(idlFull.m_pidl != NULL);

			// fill mask
			DWORD			dwAttributes = SFGAO_LINK | SFGAO_SHARE | SFGAO_GHOSTED;
			HRESULT 		hr			 = pFolder->GetAttributesOf(1, (LPCITEMIDLIST *) &idlFile.m_pidl, &dwAttributes);

			if ( SUCCEEDED(hr) && !MtlIsTooSlowIDList(idlFull) )
				_TreeItemMask(tvis.item, dwAttributes);

			// set up!!
			tvis.item.lParam		 = (LPARAM) idlFull.m_pidl;

			idlFull.Detach();													// *********don't forget clean up the lParam!!!!!!!********
			_arrTvis.Add(tvis);
		}
	};


public:
	void _CleanUpAll()
	{
		_CleanUpChildrenFromChild( GetRootItem() );
		ATLASSERT(GetCount() == 0);
	}


	void RefreshRootTree()
	{
		_CLockRedrawTreeView lock(NULL, m_hWnd);

		_InitRootTree();
	}


private:

	void _CleanUpChildrenFromChild(HTREEITEM hItemChild)
	{
		etvTRACE( _T("_CleanUpChildren\n") );

		if (hItemChild == NULL)
			return;

		do {
			HTREEITEM	hItemNextChild = GetNextItem(hItemChild, TVGN_NEXT);	// cache

			CItemIDList idl 		   = _GetItemIDList(hItemChild);
			etvTRACE( _T(" %s\n"), idl.GetPath() );

			// clean pidl
			_FreeIDListSafely(hItemChild);

			// clean children of this child
			_CleanUpChildren(hItemChild);

			DeleteItem(hItemChild);

			hItemChild = hItemNextChild;
		} while (hItemChild != NULL);
	}


public:
	void _RefreshChildren(HTREEITEM hItemParent)
	{
		m_bDestroying = true;

		_CLockRedrawTreeView lock(hItemParent, m_hWnd);

		if (hItemParent == NULL) {												// root
			_InitRootTree();
		} else if ( _IsItemExpanded(hItemParent) ) {
			_CleanUpChildren(hItemParent);
			_AddChildren(hItemParent);
		}

		m_bDestroying = false;
	}


	void _CleanUpChildren(HTREEITEM hItemParent)
	{
		etvTRACE( _T("_CleanUpChildren\n") );
		HTREEITEM hItemChild = GetChildItem(hItemParent);

		if (hItemChild == NULL)
			return;

		do {
			HTREEITEM hItemNextChild = GetNextItem(hItemChild, TVGN_NEXT);		// cache

			// clean pidl
			_FreeIDListSafely(hItemChild);

			// clean children of this child
			_CleanUpChildren(hItemChild);

			DeleteItem(hItemChild);

			hItemChild = hItemNextChild;
		} while (hItemChild != NULL);

		ATLASSERT(GetChildItem(hItemParent) == NULL);
	}


	static void _TreeItemMask(TVITEM &item, ULONG dwAttributes)
	{
		if (dwAttributes & SFGAO_LINK) {
			item.mask	  |= TVIF_STATE;
			item.stateMask = TVIS_OVERLAYMASK;
			item.state	   = INDEXTOOVERLAYMASK(2);
		}

		if (dwAttributes & SFGAO_SHARE) {
			item.mask	  |= TVIF_STATE;
			item.stateMask = TVIS_OVERLAYMASK;
			item.state	   = INDEXTOOVERLAYMASK(1);
		}

		if (dwAttributes & SFGAO_GHOSTED) {
			item.mask	   |= TVIF_STATE;
			item.stateMask |= TVIS_CUT;
			item.state	   |= TVIS_CUT;
		}
	}


private:
	static bool _CheckTVHTFlag_For_FullLow(UINT flag)
	{
		return ( flag & (TVHT_ONITEM | TVHT_ONITEMINDENT | TVHT_ONITEMRIGHT) ) != 0/* ? true : false*/;
	}


	bool _PopupContextMenu_blank(CPoint pt)
	{
		ClientToScreen(&pt);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu( 0, 1, _T("表示の更新") );

		if ( menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
								 pt.x, pt.y, m_hWnd, NULL) )
		{
			_InitRootTree();
		}

		return true;
	}


	bool _PopupContextMenu(HTREEITEM hTreeItem, CPoint pt, HTREEITEM &hItemExpanded)
	{
		CItemIDList 		  idl				= _GetItemIDList(hTreeItem);					// full path of file
		CItemIDList 		  idlFolder 		= _GetItemIDList( GetParentItem(hTreeItem) );	// full path of folder
		CString 			  strFile			= idl.GetPath();								//minit
		BOOL				  bHavePath 		= (::GetFileAttributes(strFile) != 0xFFFFFFFF); //ファイルがあるかどうかチェック

		idl -= idlFolder;																		// get file name
		if ( idl.IsNull() )
			return false;

		CString 			  strCheckFile		= idl.GetPath();
		CString 			  strCheckFolder	= idlFolder.GetPath();

		// get parent IShellFolder //デスクトップを取得し、spFolderを結びつける
		CComPtr<IShellFolder> spFolder;
		HRESULT hr	= m_spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);

		if ( FAILED(hr) )
			return false;

		LPITEMIDLIST		  pidls[]			= { idl.m_pidl };

		// コンテキストメニューの取得
		CComPtr<IContextMenu> spContextMenu;
		hr	 = spFolder->GetUIObjectOf(m_hWnd, 1, (const struct _ITEMIDLIST **) pidls, IID_IContextMenu, 0, (void **) &spContextMenu);

		if ( FAILED(hr) )
			return false;

		m_spContextMenu2.Release();

		BOOL	bMenuEx	= TRUE;

		if (bHavePath) {
			hr = spContextMenu->QueryInterface(IID_IContextMenu2, (void **) &m_spContextMenu2);

			if ( FAILED(hr) ) {
				bMenuEx = FALSE;
				hr		= spFolder->GetUIObjectOf(m_hWnd, 1, (const struct _ITEMIDLIST **) pidls, IID_IContextMenu, 0, (void **) &m_spContextMenu2);
				if ( FAILED(hr) )
					return false;
			}
		}

		// setup menu
		CMenu				  menu;
		menu.CreatePopupMenu();
		int 				  nPos				= 0;
		T * 				  pT				= static_cast<T *>(this);
		UINT				  uFlags			= pT->OnPrepareMenuForContext(hTreeItem, menu.m_hMenu, nPos);

		//元のコンテキストメニューを取得して追加
		if (bHavePath) {
			hr = m_spContextMenu2->QueryContextMenu(menu.m_hMenu, nPos, s_nIDLast, 0x7fff, uFlags);
		} else {
			hr = spContextMenu->QueryContextMenu(menu.m_hMenu, nPos, s_nIDLast, 0x7fff, uFlags);
		}

		if ( FAILED(hr) )
			return false;

		//履歴ならメニューアイテム[開く(O)]とセパレータを削除
		if ( _check_flag( ETV_EX_HISTORY, GetExplorerTreeViewExtendedStyle() ) ) {
			menu.DeleteMenu(3, MF_BYPOSITION);	//[開く(O)]の下のセパレータを削除
			menu.DeleteMenu(2, MF_BYPOSITION);	//[開く(O)]を削除
		}

		int 				  nRenameID 		= MtlGetCmdIDFromAccessKey( menu.m_hMenu, _T("&M") );
		int 				  nDeleteID 		= MtlGetCmdIDFromAccessKey( menu.m_hMenu, _T("&D") );
		int 				  nCreateShortCutID = MtlGetCmdIDFromAccessKey( menu.m_hMenu, _T("&S") );
		int 				  nNewOpenID		= MtlGetCmdIDFromAccessKey( menu.m_hMenu, _T("&N") );

		if (bMenuEx)
			pT->OnPrepareMenuForPopup(hTreeItem, menu.m_hMenu);

		ClientToScreen(&pt);

		// ここでポップアップ
		UINT				  uMenuFlags		= TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
		int 				  idCmd 			= menu.TrackPopupMenu(uMenuFlags | TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);


		CMINVOKECOMMANDINFO   cmi;
		cmi.cbSize		 = sizeof (CMINVOKECOMMANDINFO);
		cmi.fMask		 = 0;
		cmi.hwnd		 = m_hWnd;
		cmi.lpVerb		 = (LPCSTR) MAKEINTRESOURCE(idCmd - s_nIDLast);		//+++ //*メモ UNICODE対策いる?いらない?
		cmi.lpParameters = NULL;
		cmi.lpDirectory  = NULL;
		cmi.nShow		 = SW_SHOWNORMAL;
		cmi.dwHotKey	 = 0;
		cmi.hIcon		 = NULL;


		if (idCmd == 0) {
			return false;
		} else if (idCmd == nNewOpenID) {
			OnTreeItemClicked(hTreeItem, 0);
			return true;
		} else if (idCmd == nRenameID) {
			EditLabel(hTreeItem);
			return true;
		} else if (idCmd == nDeleteID) {
			_CLockRedrawTreeView lock(hTreeItem, m_hWnd);
			m_bDestroying = true;
			_CleanUpChildren(hTreeItem);
			DeleteItem(hTreeItem);
			m_bDestroying = false;
			// pass it
		} else if (idCmd == nCreateShortCutID) {
			if (bHavePath) {
				hr = m_spContextMenu2->InvokeCommand(&cmi);
			} else {
				hr = spContextMenu->InvokeCommand(&cmi);
			}

			_RefreshChildren( GetParentItem(hTreeItem) );
			return true;
		} else if (idCmd == s_nIDExpand) {
			hItemExpanded = hTreeItem;
			//			_CollapseChildrenForSingleExpand(GetParentItem(hTreeItem));
			Select(hTreeItem, TVGN_CARET);
			Expand(hTreeItem, TVE_EXPAND);	// required. cuz if already selected, Select(hTreeItem, TVGN_CARET) does nothing.
			return true;
		} else if (idCmd == s_nIDCollapse) {
			Expand(hTreeItem, TVE_COLLAPSE);
			return true;
		} else if (idCmd == s_nIDOpen) {
			// overridable
			T *pT = static_cast<T *>(this);
			pT->OnOpenTreeItem( hTreeItem, _GetItemIDList(hTreeItem) );
			return true;
		} else if (idCmd == s_nIDNewFolder) {
			_CreateNewFolder( GetParentItem(hTreeItem) );
			return true;
		} else {
			if ( pT->OnCmdContextMenu(hTreeItem, idCmd) )
				return true;
		}

		// Execute command now
		if (bHavePath) {
			hr = m_spContextMenu2->InvokeCommand(&cmi);
			m_spContextMenu2.Release();
		} else {
			hr = spContextMenu->InvokeCommand(&cmi);
		}

		if ( FAILED(hr) )
			return false;

		return true;
	}


public:
	LPITEMIDLIST _GetItemIDList(HTREEITEM hTreeItem)
	{										// hTreeItem can be NULL
		if (hTreeItem == NULL)
			return m_idlRootFolder;

		TVITEM tvi = {0};
		tvi.mask  = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hTreeItem;
		GetItem(&tvi);
		//		ATLASSERT(tvi.lParam != NULL);
		return (LPITEMIDLIST) tvi.lParam;
	}


	HTREEITEM _FindItemIDList(HTREEITEM hItemParent, LPCITEMIDLIST pidl)
	{
		HTREEITEM	hItem;

		if (hItemParent == NULL)
			hItem = GetRootItem();
		else
			hItem = GetChildItem(hItemParent);

		for ( ; hItem != NULL; hItem = GetNextItem(hItem, TVGN_NEXT) ) {
			CItemIDList idl = _GetItemIDList(hItem);

			if (idl == pidl)
				return hItem;
		}

		return NULL;
	}


	bool _IsItemFolder(HTREEITEM hItem)
	{
		return ItemHasChildren(hItem) == TRUE;
	}


	bool _IsItemExpanded(HTREEITEM hItem)
	{
		if (hItem == NULL)
			return false;

		TVITEM tvi = { 0 };
		tvi.mask  = TVIF_HANDLE | TVIF_STATE;
		tvi.hItem = hItem;
		GetItem(&tvi);

		return (tvi.state & TVIS_EXPANDED) != 0;
	}


private:
	bool _CreateNewFolder(HTREEITEM hItemParent)
	{ // I've found CMDSTR_NEWFOLDER command, but I wanna handle a hTreeItem.
		bool				bRet	= false;
		CString 			str( _T("新しいフォルダ") );

		CString 			strPath = CItemIDList( _GetItemIDList(hItemParent) ).GetPath();

		if ( strPath.IsEmpty() )
			return bRet;

		MtlMakeSureTrailingBackSlash(strPath);

		int 				i		= 2;
		CString 			strTmp	= str;

		for (;;) {
			if ( !MtlIsDirectory(strPath + strTmp) )
				break;

			strTmp	= str + _T(" (");
			strTmp.Append(i++);
			strTmp += _T(')');
		}

		strPath 				= strPath + strTmp;

		SECURITY_ATTRIBUTES sa = {0};
		sa.nLength				= sizeof (SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle		= FALSE;

		if ( !::CreateDirectory(strPath, &sa) )
			return bRet;

		// insert
		TV_INSERTSTRUCT 	tvis = {0};
		tvis.hParent			= hItemParent;
		tvis.hInsertAfter		= TVI_LAST;
		tvis.item.mask			= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvis.item.pszText		= (LPTSTR) (LPCTSTR) strTmp;

		// set up full item id list!
		CItemIDList 		idlFull = strPath;

		if ( idlFull.IsNull() )
			return bRet;

		tvis.item.lParam		= (LPARAM) idlFull.m_pidl;

		tvis.item.mask		   |= TVIF_CHILDREN;
		tvis.item.cChildren 	= 1;

		if ( m_bOrgImage && _IsFavorites() ) {
			tvis.item.iImage		 = 0;
			tvis.item.iSelectedImage = 1;
		} else {
			tvis.item.iImage		 = I_IMAGECALLBACK;
			tvis.item.iSelectedImage = I_IMAGECALLBACK;
		}

		idlFull.Detach(); // *********don't forget clean up the lParam!!!!!!!********
		EditLabel( InsertItem(&tvis) );
		return true;
	}


	bool _InsertItem(HTREEITEM hItemParent, LPCITEMIDLIST pidl, const CString &strItem, bool bFolder)
	{
		TV_INSERTSTRUCT tvis = {0};

		tvis.hParent			 = hItemParent;
		tvis.hInsertAfter		 = TVI_LAST;
		tvis.item.mask			 = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvis.item.pszText		 = (LPTSTR) (LPCTSTR) strItem;

		// set up full item id list!
		CItemIDList 	idlFull = strPath;

		if ( idlFull.IsNull() )
			return bRet;

		tvis.item.lParam		 = (LPARAM) pidl;

		tvis.item.iImage		 = I_IMAGECALLBACK;
		tvis.item.iSelectedImage = I_IMAGECALLBACK;

		if (bFolder) {
			tvis.item.mask	   |= TVIF_CHILDREN;
			tvis.item.cChildren = 1;

			if ( m_bOrgImage && _IsFavorites() ) {
				tvis.item.iImage		 = 0;
				tvis.item.iSelectedImage = 1;
			}
		} else {
			if ( m_bOrgImage && _IsFavorites() ) {
				CString strExt = strTitle.Mid(strPath.ReverseFind('.') + 1);
				strExt.MakeUpper();

				if ( strExt == _T("URL") )
					tvis.item.iImage = tvis.item.iSelectedImage = 2;
				else
					tvis.item.iImage = tvis.item.iSelectedImage = 3;
			}
		}
	}


	struct _Function_CompareTreeItem {
		bool operator ()(const TV_INSERTSTRUCT &tv1, const TV_INSERTSTRUCT &tv2)
		{
			return ::lstrcmp(tv1.item.pszText, tv2.item.pszText) < 0;
		}
	};


public:
	struct _CLockRedrawTreeView {
		HTREEITEM	  _hItemParent;
		CTreeViewCtrl _tree;
		CRect		  _rcInvalid;
		HWND		  _hWndFocus;

		_CLockRedrawTreeView(HTREEITEM hItemParent, HWND hWnd)
			: _hItemParent(hItemParent)
			, _tree(hWnd)
			, _hWndFocus(NULL)
		{
			ATLASSERT( ::IsWindow(hWnd) );

			CRect  rcItem;
			rcItem.SetRectEmpty();

			if (_hItemParent != NULL)
				_tree.GetItemRect(_hItemParent, &rcItem, FALSE);

			_tree. GetClientRect( &_rcInvalid);
			_rcInvalid = CRect(_rcInvalid.left, (rcItem.bottom == 0) ? _rcInvalid.top : rcItem.bottom,
							   _rcInvalid.right, _rcInvalid.bottom);

			_tree. SetRedraw(FALSE);

			if (_hItemParent == NULL) {
				_hWndFocus = ::GetFocus();
				_tree.ShowWindow(SW_HIDE);
			}
		}


		~_CLockRedrawTreeView()
		{
			if (_hItemParent == NULL) {
				_tree.ShowWindow(SW_SHOWNORMAL);

				if (_tree.m_hWnd == _hWndFocus)
					_tree.SetFocus();
			}

			_tree.SetRedraw(TRUE);
			_tree.InvalidateRect(_rcInvalid);
		}
	};


private:
	void _CollapseRootItem()
	{
		_CLockRedrawTreeView lock(NULL, m_hWnd);

		HTREEITEM			 hItem = GetRootItem();

		if ( _IsItemExpanded(hItem) )
			Expand(hItem, TVE_COLLAPSE);
	}


	void _FreeIDListSafely(HTREEITEM hItem)
	{
		TVITEM tvi = {0};

		tvi.mask   = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem  = hItem;
		GetItem(&tvi);

		CItemIDList::FreeIDList( (LPITEMIDLIST) tvi.lParam );
		tvi.lParam = NULL;
		SetItem(&tvi);
	}


	void _NoImageCallBack(TVITEM tvi)
	{
		ATLASSERT(tvi.hItem != NULL);

		tvi.mask |= TVIF_HANDLE;
		tvi.mask  = tvi.mask & (TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE); // update only these

		if (tvi.mask == TVIF_HANDLE)											// no need to do anything
			return;

		SetItem(&tvi);
	}


	struct _DefaultTreeItemCompare : public std::binary_function<const TV_INSERTSTRUCT &, const TV_INSERTSTRUCT &, bool> {
		_DefaultTreeItemCompare()
		{
		}


		bool operator ()(const TV_INSERTSTRUCT &x, const TV_INSERTSTRUCT &y)
		{
			bool bDirA = (x.item.cChildren == 1);
			bool bDirB = (y.item.cChildren == 1);

			if (bDirA == bDirB)
				return ::lstrcmp(x.item.pszText, y.item.pszText) < 0;
			else {
				if (bDirA)
					return true;
				else
					return false;
			}
		}
	};
};



#if 0	//+++ 未使用
class CExplorerTreeViewCtrl : public CExplorerTreeViewCtrlImpl<CExplorerTreeViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS( _T("MTL_ExpTreeViewCtrl"), GetWndClassName() )
};
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。
#endif	// !defined(AFX_SHVIEW_DEMOVIEW_H__0DD77E8E_1C9C_11D5_8A9C_9D5C3CCEE371__INCLUDED_)
