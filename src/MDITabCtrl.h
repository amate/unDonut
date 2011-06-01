/**
 *	@file	MDITabCtrl.h
 *	@brief	interface of the CMDITabCtrl class
 */
/////////////////////////////////////////////////////////////////////////////
// MDITabCtrl.h : interface of the CMDITabCtrl class
//
// How to use:
//	1.Place CMDITabCtrl m_MDITab in MDI mainframe class.
//	2.Place CMDITabCtrl& m_MDITab in MDI child class.
//	3.Place PASS_MSG_MAP_MDICHILD_TO_MDITAB(m_MDITab) in the head of MDI child's message map
//
// Note:
//	If you click system menu item which has short cut(Ctrl+F6 etc) on frame bar,
//	it send WM_COMMAND and send WM_SYSCOMMAND.
//	But if you TrackPopup system menu by yourself, only WM_COMMAND is sent.
//
// Why I choose not MDI child window but CMDITabctrl itself as popup menu owner:
//	If you cancel popup menu with pressing ESC key or clicking outside
//	(how do I know when canceled!?) and select menu on main frame bar,
//	cause menu command sender can't be determine,
//	I can't skip this message and WM_COMMAND is handled twice.
//
// Written by MB<mb2@geocities.co.jp> 2000.06.28
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OleDragDropTabCtrl.h"
#include "HlinkDataObject.h"
#include "dialog/CommandSelectDialog.h"
#include "FaviconManager.h"

namespace MTL {

// Extended styles
enum EMtb_Ex {
	MTB_EX_ADDLEFT				= 0x00000001L,
	MTB_EX_RIGHTCLICKCLOSE		= 0x00000002L,
	MTB_EX_DELAYED				= 0x00000004L,
	MTB_EX_MULTILINE			= 0x00000008L,
	MTB_EX_DOUBLECLICKCLOSE 	= 0x00000010L,
	MTB_EX_XCLICKCLOSE			= 0x00000020L,
	MTB_EX_RIGHTCLICKREFRESH	= 0x00000040L,
	MTB_EX_DOUBLECLICKREFRESH	= 0x00000080L,
	MTB_EX_XCLICKREFRESH		= 0x00000100L,
	MTB_EX_RIGHTACTIVEONCLOSE	= 0x00000200L,
	MTB_EX_LEFTACTIVEONCLOSE	= 0x00000400L,
	MTB_EX_ADDRIGHTACTIVE		= 0x00000800L,
	MTB_EX_ADDLEFTACTIVE		= 0x00001000L,
	MTB_EX_WHEEL				= 0x00002000L,
	MTB_EX_FIXEDSIZE			= 0x00004000L,
	MTB_EX_ANCHORCOLOR			= 0x00008000L,
	// UDT DGSTR
	MTB_EX_DOUBLECLICKNLOCK 	= 0x00010000L,
	MTB_EX_XCLICKNLOCK			= 0x00020000L,
	MTB_EX_ADDLINKACTIVERIGHT	= 0x00040000L,
	// minit
	MTB_EX_DOUBLECLICKCOMMAND	= 0x00080000L,
	MTB_EX_XCLICKCOMMAND		= 0x00100000L,
	MTB_EX_RIGHTCLICKCOMMAND	= 0x00200000L,
	MTB_EX_MOUSEDOWNSELECT		= 0x00400000L,
	// +mod
	MTB_EX_CTRLTAB_MDI			= 0x00800000L,	//+++

	MTB_EX_DEFAULT_BITS	= MTB_EX_MULTILINE
						| MTB_EX_WHEEL 
						| MTB_EX_ANCHORCOLOR 
						| MTB_EX_XCLICKCLOSE 
						| MTB_EX_DOUBLECLICKNLOCK 
						| MTB_EX_RIGHTACTIVEONCLOSE,	//+++
		
};



//minit A_ONはGetLinkStateで消されるB_ONは残る
enum ELinkState {
	LINKSTATE_OFF		= 0,
	LINKSTATE_A_ON		= 1,
	LINKSTATE_B_ON		= 2,
};



//#undef  WM_XBUTTONUP
//#define WM_XBUTTONUP		0x020C


////////////////////////////////////////////////////////////////////////
// CMDITabCtrl

class CMDITabCtrl : public COleDragDropTabCtrl<CMDITabCtrl>
{
public:
	DECLARE_WND_CLASS_EX(_T("Mtl_MDI_TabCtrl"), CS_DBLCLKS, COLOR_BTNFACE)

	// Constructor
	CMDITabCtrl();

	BOOL	LoadMenu(_U_STRINGorID menu);
	void	SetMDIClient(HWND hWndMDIClient);

	// Attributes
	int 	GetTabIndex(HWND hWndMDIChild);
	HWND	GetTabHwnd(int nIndex);

	DWORD	GetMDITabExtendedStyle() const;
	DWORD	SetMDITabExtendedStyle(DWORD dwExtendedStyle);

	void	SetMaxTabItemTextLength(int nLength);
	int 	GetMaxTabItemTextLength();

	// Operations
	void	RightTab();
	void	SetCurSelEx(int nIndex, bool bActivate = true);
	void	LeftTab();

	void	OnMDIChildCreate(HWND hWnd);
	void	OnMDIChildDestroy(HWND hWnd);


	BOOL	LoadConnectingAndDownloadingImageList(
					UINT		nImageBmpID,
					int 		cx,
					int 		cy,
					COLORREF	clrMask,
					UINT		nFlags = ILC_COLOR24);

	void	SetConnecting(HWND hWnd);
	void	SetDownloading(HWND hWnd);
	void	SetComplete(HWND hWnd);

	int 	ManageClose(HWND m_hWnd);
	void	NavigateLockTab(HWND hWnd, bool bOn);
	int 	ShowTabListMenuDefault (int nX, int nY);
	int 	ShowTabListMenuVisible (int nX, int nY);
	int 	ShowTabListMenuAlphabet(int nX, int nY);
	void	ShowTabMenu(int nIndex);

	void	SetLinkState(int nState);


	// Overrides
	void		OnDropDownButton();
	CString 	OnGetToolTipText(int nIndex);

	void		OnDeleteItemDrag(int nIndex);
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT	OnDragOver (IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);

	bool		OnNewTabCtrlItems(
						int 						nInsertIndex,
						CSimpleArray<CTabCtrlItem>& /*items*/,
						IDataObject *				pDataObject,
						DROPEFFECT	&				dropEffect);
	bool		OnDropTabCtrlItem(int nIndex, IDataObject *pDataObject, DROPEFFECT &dropEffect);
	HRESULT 	OnGetTabCtrlDataObject(CSimpleArray<int> &arrIndex, IDataObject **ppDataObject);


	//外部から使う関数オブジェクト
	//+++	リターン値でなく引数を書き換えて結果を返すように変更.
	template <class _Function>
	void	ForEachWindow(_Function& __f) {
		DWORD_PTR	dwData;
		UINT		nCount = GetItemCount();
		for (UINT i = 0; i < nCount; ++i) {
			dwData = 0;
			GetItemUserData(i, dwData);
			HWND	hWnd = (HWND) dwData;
			ATLASSERT( ::IsWindow(hWnd) );
			__f(hWnd);
		}
	}

	//外部から使うクラス
	class CLockRedraw {
	public:
		CMDITabCtrl &m_tab;

		CLockRedraw(CMDITabCtrl &tab) : m_tab(tab)
		{
			m_tab._LockRedraw(true);
			m_tab.SetRedraw(FALSE);
		}


		~CLockRedraw()
		{
			m_tab._LockRedraw(false);
			m_tab.SetRedraw(TRUE);
			m_tab.Invalidate();
			m_tab.UpdateWindow();
		}
	};

	// Message map and handlers
	BEGIN_MSG_MAP(CMDITabCtrl)
		MSG_WM_RBUTTONUP	( OnRButtonUp	  )
		MSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		MSG_WM_MBUTTONUP	( OnMButtonUp )
		REFLECTED_NOTIFY_CODE_HANDLER_EX (TCN_SELCHANGE, OnTcnSelChange)
		MSG_WM_COMMAND		( OnCommand 	  )
		MESSAGE_HANDLER 	( WM_MENUSELECT, OnMenuSelect	  )
		USER_MSG_WM_SETFAVICONIMAGE( OnSetFaviconImage )
		CHAIN_MSG_MAP		( COleDragDropTabCtrl<CMDITabCtrl>)
	ALT_MSG_MAP(1)							// MDI child windows messages
		m_wndMDIChildProcessing = hWnd;
		MSG_WM_MDIACTIVATE	( OnMDIActivate )
		MSG_WM_SETTEXT		( OnSetText 	)
		m_wndMDIChildProcessing = NULL;
	END_MSG_MAP()


	void		OnLButtonDblClk(UINT nFlags, CPoint point);
	void		OnRButtonUp(UINT nFlags, CPoint point);
	void		OnMButtonUp(UINT nFlags, CPoint point);
	LRESULT 	OnTcnSelChange(LPNMHDR lpnhmdr);

	LRESULT 	OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/);
	void		OnCommand(UINT wNotifyCode, int wID, HWND hwndCtl);
	void		OnSetFaviconImage(HWND hWnd, HICON hIcon);

	// CChildFrameから
	void		OnMDIActivate(HWND hWndChildDeact, HWND hWndChildAct);
	LRESULT 	OnSetText(LPCTSTR lpszText);


private:
	// Implementation
	bool		_SetTabText(int nIndex, LPCTSTR lpszTab);
	CString 	_GetTabText(int nIndex);

	void		_UpdateMenu(CMenuHandle &menuSys);

	int 	AddTabItem(HWND hWndMDIChild, LPCTSTR lpszText = NULL);

	void	_SendSelChange(int nIndex);
	void	SetInitialText(LPCTSTR lpszInitialText);

	void		_SetImageListIndex(int nItem, int nIndex);
	bool		IsConnecting(HWND hWnd);
	bool		IsDownloading(HWND hWnd);
	bool		IsCompleted(HWND hWnd);
	int 		_GetImageListIndex(int nItem);

	void		_LockRedraw(bool bLock);
	BOOL		GetLinkState();


	// Data members
	CWindow 	m_wndMDIChildProcessing;		// while message processing
	CMDIWindow	m_wndMDIChildPopuping;			// while menu popuping (m_bPopup is not enough)
	DWORD		m_dwExtendedStyle;
	CString 	m_strInitial;

	int 		m_nMaxTabItemTextLength;
	bool		m_bRedrawLocked;
	int 		m_nLinkState;

	bool		m_bDragAccept;

	//内部で使う構造体
	struct _Object_TabSorting;
	struct _Function_CompareTitle;

public:
	CMenu		m_menuPopup;

	bool		m_bInsertHere;
	int 		m_nInsertIndex;

	int 		m_nRClickCommand;
	int			m_nXClickCommand;
	int			m_nDClickCommand;				//minit
};



#define PASS_MSG_MAP_MDICHILD_TO_MDITAB(x)		CHAIN_MSG_MAP_ALT_MEMBER(x, 1)



template <class _Profile>
void MtlGetProfileMDITab(_Profile &__profile, CMDITabCtrl &tabMDI)
{
	DWORD dwStyle  = MTB_EX_DEFAULT_BITS/*MTB_EX_XCLICKCLOSE | MTB_EX_DOUBLECLICKNLOCK | MTB_EX_WHEEL | MTB_EX_RIGHTACTIVEONCLOSE*/;
		  dwStyle  = __profile.GetValue( _T("Extended_Style"), dwStyle );
	DWORD dwLength = __profile.GetValue( _T("Max_Text_Length"), 30 );

	// load fixed size
	DWORD dwX	= __profile.GetValue(_T("Fixed_Size_X"), 60 );
	DWORD dwY	= __profile.GetValue(_T("Fixed_Size_Y"), 30 );
	tabMDI.SetItemSize( CSize(dwX, dwY) );

	//minit
	tabMDI.m_nXClickCommand = __profile.GetValue(_T("XClickCommand"), 0 );
	tabMDI.m_nRClickCommand = __profile.GetValue(_T("RClickCommand"), 0 );
	tabMDI.m_nDClickCommand = __profile.GetValue(_T("DClickCommand"), 0 );

	tabMDI.SetMDITabExtendedStyle(dwStyle);
	tabMDI.SetMaxTabItemTextLength(dwLength);

	int nWidth = (int)__profile.GetValue(_T("SeparatorWidth"), 10);
	tabMDI.SetTabSeparatorWidth(nWidth);
	tabMDI.SetFirefoxLike(__profile.GetValue(_T("FirefoxLike"), 0) != 0);
}



template <class _Profile>
void MtlWriteProfileMDITab(_Profile &__profile, CMDITabCtrl &tabMDI)
{
	__profile.SetValue( tabMDI.GetMDITabExtendedStyle(), _T("Extended_Style") );
	__profile.SetValue( tabMDI.GetMaxTabItemTextLength(), _T("Max_Text_Length") );

	__profile.SetValue( tabMDI.m_nXClickCommand, _T("XClickCommand") ); //minit
	__profile.SetValue( tabMDI.m_nRClickCommand, _T("RClickCommand") );
	__profile.SetValue( tabMDI.m_nDClickCommand, _T("DClickCommand") );

	CSize sizeItem = tabMDI.GetItemSize();
	__profile.SetValue( sizeItem.cx, _T("Fixed_Size_X") );
	__profile.SetValue( sizeItem.cy, _T("Fixed_Size_Y") );

	__profile.SetValue( tabMDI.GetTabSeparatorWidth(), _T("SeparatorWidth") );
	__profile.SetValue( tabMDI.GetFirefoxLike() , _T("FirefoxLike") );
}



}	//namespace MTL




#ifndef _WTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_WTL_NO_AUTOMATIC_NAMESPACE
