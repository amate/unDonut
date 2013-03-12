/**
*	@file	LinkPopupMenu.h
*	@brief	リンクバー用のポップアップメニューウィンドウ
*/

#pragma once

#if _MSC_VER >= 1700
#include <atomic>
#endif
#include <boost\optional.hpp>
#include <boost\property_tree\ptree.hpp>
#include <atlscrl.h>
#include "MtlWin.h"
#include "MtlMisc.h"
#include "MtlCom.h"
#include "MtlFile.h"
#include "MtlDragDrop.h"
#include "DonutPFunc.h"
#include "Misc.h"

struct LinkItem
{
	CString strName;
	CString strUrl;
	vector<unique_ptr<LinkItem> >*	pFolder;

	CRect	rcItem;
	enum LinkItemState { kItemNormal, kItemHot, kItemPressed, };
	DWORD	state;
	CIcon	icon;

	bool	bExPropEnable;
	boost::optional<DWORD>	dwExProp;
	boost::optional<DWORD>	dwExPropOpt;

	// Attributes
	bool ModifyState(DWORD newState)
	{
		DWORD stateOld = state;
		state = newState;
		return state != stateOld;
	}

	LinkItem() : pFolder(nullptr), state(kItemNormal), bExPropEnable(false)
	{}

	~LinkItem() 
	{
		if (pFolder)
			delete pFolder; 
	}

	LinkItem(LinkItem& item)
	{
		strName	= item.strName;
		strUrl	= item.strUrl;
		if (item.icon)
			icon	= item.icon.DuplicateIcon();	// 全体で0.05秒ほど遅くなる
		if (bExPropEnable = item.bExPropEnable) {
			dwExProp	= item.dwExProp;
			dwExPropOpt	= item.dwExPropOpt;
		}
		pFolder = nullptr;
	}

};
typedef vector<unique_ptr<LinkItem> >	LinkFolder;
typedef vector<unique_ptr<LinkItem> >*	LinkFolderPtr;


// 定義はDonutLinkBarCtrl.iniにある
void	_AddFaviconDataToLinkItem(boost::property_tree::wptree& ptItem, LinkItem* pLinkItem);	
void	_AddLinkItem(LinkFolderPtr pFolder, boost::property_tree::wptree pt);

 void	_AddFaviconData(boost::property_tree::wptree& ptItem, LinkItem& item);
 #if _MSC_VER >= 1700
 void	_AddPtree(boost::property_tree::wptree& ptFolder, LinkFolderPtr pLinkFolder, std::atomic<bool>* pbCancel);
#else
 void	_AddPtree(boost::property_tree::wptree& ptFolder, LinkFolderPtr pLinkFolder, bool* pbCancel);
#endif

#define WM_CLOSEBASESUBMENU		(WM_APP + 1)
#define WM_SAVELINKBOOKMARK		(WM_APP + 2)
#define WM_UPDATESUBMENUITEMPOS	(WM_APP + 3)
#define WM_GETROOTLINKFOLDERPTR	(WM_APP + 4)

/////////////////////////////////////////////////////////////
// CLinkItemDataObject

extern const UINT CF_DONUTLINKITEM;

class CLinkItemDataObject
	: public CComCoClass<CLinkItemDataObject, &CLSID_NULL>
	, public CComObjectRootEx<CComSingleThreadModel>
	, public IDataObjectImpl<CLinkItemDataObject>
{
public:

	CComPtr<IDataAdviseHolder>	m_spDataAdviseHolder;	// expected by IDataObjectImpl

	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CLinkItemDataObject)

	BEGIN_COM_MAP(CLinkItemDataObject)
		COM_INTERFACE_ENTRY(IDataObject)
		COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)
	END_COM_MAP()

	static CComPtr<IDataObject>	CreateInstance(LinkFolderPtr pFolder, int nIndex);

	// Constructor
	CLinkItemDataObject();
	~CLinkItemDataObject();

	static bool	IsAcceptDrag(IDataObject* pDataObject);
	static std::pair<CString, CString> GetNameAndURL(IDataObject* pDataObject);

public:

	std::pair<LinkFolderPtr, int>	GetFolderAndIndex();

	// Overrides
	HRESULT IDataObject_GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	STDMETHOD	(QueryGetData) (FORMATETC* pformatetc);
	STDMETHOD	(EnumFormatEtc) (DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc);
	STDMETHOD	(SetData) (FORMATETC *pFormatetc, STGMEDIUM *pmedium, BOOL fRelease);

private:
	// Implementation

	// For DragSourceHelper
    typedef struct {
        FORMATETC   fe;
        STGMEDIUM   stgm;
    } DATAENTRY, *LPDATAENTRY;  /* アクティブな FORMATETC ごとにこれらのうちの 1 つを取得  */

    HRESULT FindFORMATETC(FORMATETC *pfe, LPDATAENTRY *ppde, BOOL fAdd);
    HRESULT AddRefStgMedium(STGMEDIUM *pstgmIn, STGMEDIUM *pstgmOut, BOOL fCopyIn);

    LPDATAENTRY m_rgde;            /* アクティブな DATAENTRY エントリの配列 */
    int m_cde;                     /* m_rgde のサイズ  */


	/// ショートカットファイルを作成する
	void _InitFileNamesArrayForHDrop();
	static CString _CompactFileName(const CString &strDir, const CString &strFile, const CString &strExt);
	HGLOBAL _CreateText();
	bool _CreateInternetShortcutFile(const CString &strFileName, const CString &strUrl);
	/// arrFileNamesに入ってるファイル名と被らないファイル名を返す
	CString _UniqueFileName(CSimpleArray<CString> &arrFileNames, const CString &strFileName);


	// Data members
	bool					m_bInitialized;
	CSimpleArray<CString>	m_arrFileNames;
	
	LinkFolderPtr	m_pBaseFolder;
	int				m_nIndex;		// m_pBaseFolder のインデックス
	bool			m_bItemIsFolder;
};


class CLinkEditDialog;

////////////////////////////////////////////////////////////
// CLinkPopupMenu

class CLinkPopupMenu : 
	public CScrollWindowImpl<CLinkPopupMenu>,
	public CThemeImpl<CLinkPopupMenu>,
	public CTrackMouseLeave<CLinkPopupMenu>,
	public IDropTargetImpl<CLinkPopupMenu>,
	public IDropSourceImpl<CLinkPopupMenu>,
	public CMessageFilter
{
	friend class CLinkEditDialog;
public:
	DECLARE_WND_CLASS_EX(_T("DonutLinkPopupMenu"), CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW, COLOR_MENU)

	// Constants
	enum {
		kBoundMargin = 2,
		kBoundBorder = 1,
		//kItemHeight = 14,//22, 
		kIconWidth	= 26,
		kVerticalLineWidth = 2,
		kTextMargin	= 4,
		kLeftTextPos = kBoundMargin + kIconWidth + kVerticalLineWidth + kTextMargin,
		kArrowWidth	= 25,
		kNoneTextWidth = 50,
		kMaxMenuTextWidth	= 400,
		kBiteWidth	= 3,

		kcxIcon = 16,
		kcyIcon = 16,
		kTopIconMargin = 3,
		kLeftIconMargin = kBoundMargin + 4,

		kSubMenuPopupCloseTimerID = 1,
		kSubMenuPopupCloseTime	= 500,
		kSubMenuPopupTimerID = 2,
		kSubMenuPopupTime	= 250,

		kCloseBaseSubMenuTimerID = 3,
		kCloseBaseSubMenuTime = 300,

		kDragInsertHitWidthOnFolder = 3,
	};
	int	kItemHeight;

	// Constructor
	CLinkPopupMenu(LinkFolderPtr pFolder, int nInheritIndex = -1);

	static HWND	SetLinkBarHWND(HWND hWnd);

	int	ComputeWindowWidth();
	int ComputeWindowHeight();

	LinkFolderPtr	GetLinkFolderPtr() const { return m_pFolder; }
	CLinkPopupMenu*	GetSubMenu() const { return m_pSubMenu;	}

	static void	OpenLink(const LinkItem& item, DWORD openFlag);
	static void OpenMultiLink(LinkFolderPtr pFolder);
	static int	ShowRClickMenuAndExecCommand(LinkFolderPtr pFolder, LinkItem* pLinkItem, HWND hwnd);
	static bool	s_bNowShowRClickMenu;
	static void SaveLinkBookmark();
	static void SortByName(LinkFolderPtr pFolder);

	static void	GetFaviconToLinkItem(const CString& url, LinkFolderPtr pFolder, LinkItem* pItem, HWND hwnd);

	static CIcon	s_iconFolder;
	static CIcon	s_iconLink;

	static std::pair<LinkFolderPtr, unique_ptr<LinkItem> > s_TrashItem;

	// Overrides
	virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		/*if (pMsg->hwnd == m_hWnd && pMsg->message == WM_LBUTTONDOWN) {
			POINT pt = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
			OnLButtonDown((UINT)pMsg->wParam, pt);
			return TRUE;
		}*/
		return FALSE;
	}

	void DoPaint(CDCHandle dc);

	void DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll, int cxySizePage, int cxySizeLine);

	/// カーソル上のアイテムをHOT状態にする
	void OnTrackMouseMove(UINT nFlags, CPoint pt);

	/// アイテムのHOT状態を解除する
	void OnTrackMouseLeave();

	// IDropTargetImpl
	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	void	OnDragLeave();

	BEGIN_MSG_MAP( CLinkPopupMenu )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_TIMER( OnTimer )
		MSG_WM_ERASEBKGND	( OnEraseBkgnd )
		MSG_WM_MOUSEACTIVATE( OnMouseActivate )
		MSG_WM_KEYDOWN( OnKeyDown )
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_RBUTTONUP	( OnRButtonUp )
		MSG_WM_MBUTTONDOWN( OnMButtonDown )
		MESSAGE_HANDLER_EX( WM_UPDATESUBMENUITEMPOS, OnUpdateSubMenuItemPosition	)
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnTooltipGetDispInfo)
		CHAIN_MSG_MAP( CScrollWindowImpl<CLinkPopupMenu> )
		CHAIN_MSG_MAP( CThemeImpl<CLinkPopupMenu> )
		CHAIN_MSG_MAP( CTrackMouseLeave<CLinkPopupMenu> )
		CHAIN_MSG_MAP_ALT(CScrollWindowImpl<CLinkPopupMenu>, 1)
	END_MSG_MAP()

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnTimer(UINT_PTR nIDEvent);
	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }

	/// クリックでアクティブにならないようにする
	int  OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message) { return MA_NOACTIVATE; }

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	LRESULT OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTooltipGetDispInfo(LPNMHDR pnmh);

private:
	void	_InitTooltip();
	void	_UpdateItemPosition(bool bFirst = false);
	bool	_IsValidIndex(int nIndex) const;
	CRect	_GetClientItemRect(const CRect& rcItem);
	boost::optional<int>	_HitTest(const CPoint& point);
	void	_HotItem(int nNewHotIndex);
	void	_DoExec(const CPoint& pt, bool bLButtonDown = false);
	void	_CloseSubMenu();
	static void	_CloseBaseSubMenu();

	void	_DrawInsertEdge(const CRect& rcEdge);
	void	_ClearInsertionEdge();
	void	_DrawDragImage(CDCHandle dc, const LinkItem& item);

	enum hitTestFlag {
		htItemNone,
		htItemTop,
		htItemBottom,
		htFolder,
	};
	int		_HitTestOnDragging(const CPoint& pt, hitTestFlag& htflag);

	// Data members
	LinkFolderPtr	m_pFolder;
	CFont	m_font;
	int		m_nHotIndex;
	CLinkPopupMenu*	m_pSubMenu;
	const int m_nInheritFolderIndex;
	static HWND	s_wndLinkBar;

	CRect	m_rcInvalidateOnDrawingInsertionEdge;
	int		m_nNowDragItemIndex;
	std::pair<LinkFolderPtr, int>	m_DragItemData;
	bool			m_bAcceptDrag;
	bool			m_bDragItemIsLinkFile;
	bool			m_bDrawDisableHot;
	static bool		s_bNoCloseBaseSubMenu;

	CToolTipCtrl	m_tip;
	CString			m_strTipText;
};



























