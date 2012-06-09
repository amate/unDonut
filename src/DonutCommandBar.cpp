/**
*	@file	DonutCommandBar.cpp
*	@brief	コマンドバークラス
*/

#include "stdafx.h"
#include "DonutCommandBar.h"
#include <atlscrl.h>
#include "resource.h"
#include "MtlMisc.h"
#include "MtlWin.h"
#include "PopupMenu.h"


////////////////////////////////////////////////////////////////////////////
// CDonutCommandBar::Impl

class CDonutCommandBar::Impl :
	public CDoubleBufferWindowImpl<Impl>,
	public CThemeImpl<Impl>,
	public CTrackMouseLeave<Impl>
{
public:
	DECLARE_WND_CLASS(_T("DonutCommandBar"))

	// Constants
	enum { 
		kTopBottomPadding = 7,//6,
		kRightLeftPadding = 7,//6,
		kRightTextMargin = 2,
		//kMaxItemTextWidth	= 100,
		kDragInsertHitWidthOnFolder = 7,

		kcxIcon = 16,
		kcyIcon = 16,
		kTopIconMargin = 3,
		kLeftIconMargin = 3,

		kLeftTextPadding = kLeftIconMargin * 2 + kcxIcon,

		kTextSideMargin = 6,

		kFilePos = 0,
		kFavoritePos = 3,
	};

	enum ChevronState {
		ChvNormal	= CHEVS_NORMAL,
		ChvHot		= CHEVS_HOT,
		ChvPressed	= CHEVS_PRESSED,
	};

// Constructor/destructor
	Impl();
	~Impl();

	HWND	Create(HWND hWndParent);
	void	SetFont(HFONT hFont);

	// Overrides
	void DoPaint(CDCHandle dc);
	void OnTrackMouseMove(UINT nFlags, CPoint pt);
	void OnTrackMouseLeave();

	BEGIN_MSG_MAP( Impl )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_SIZE( OnSize )
		MESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnMouseWheel	)
		MSG_WM_LBUTTONDOWN( OnLButtonDown )
		MSG_WM_LBUTTONUP( OnLButtonUp )
		MSG_WM_RBUTTONUP( OnRButtonUp )
		MSG_WM_MBUTTONDOWN( OnMButtonDown )
		MESSAGE_HANDLER_EX( WM_CLOSEBASESUBMENU, OnCloseBaseSubMenu )
		CHAIN_MSG_MAP( CDoubleBufferWindowImpl<CDonutCommandBar::Impl> )
		CHAIN_MSG_MAP( CThemeImpl<CDonutCommandBar::Impl> )
		CHAIN_MSG_MAP( CTrackMouseLeave<CDonutCommandBar::Impl> )
	END_MSG_MAP()

	 int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	 void OnDestroy();
	 void OnSize(UINT nType, CSize size);
	 LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnLButtonUp(UINT nFlags, CPoint point);
	 void OnRButtonUp(UINT nFlags, CPoint point);
	 void OnMButtonDown(UINT nFlags, CPoint point);
	 LRESULT OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	int		_GetBandHeight();
	void	_RefreshBandInfo();
	void	_UpdateItemPosition();
	bool	_IsValidIndex(int nIndex) const {
		return 0 <= nIndex && nIndex < static_cast<int>(m_vecCommandButton.size());
	}
	int		_HitTest(const CPoint& pt);
	void	_HotItem(int nIndex);
	void	_PressItem(int nIndex);
	void	_CloseSubMenu();
	void	_DoPopupSubMenu(int nIndex);
	bool	_HitTestChevron(const CPoint& pt);
	void	_ChevronStateChange(ChevronState state);
	void	_PopupChevronMenu();
	int		_GetChevronIndex();

	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	struct CommandButton {
		CString	name;
		CRect	rect;

		enum ButtonState {
			kNormal = 0,
			kHot	= 1,
			kPressed= 2,
		};
		ButtonState	state;

		CommandButton(LPCTSTR str, CRect rc) : name(str), rect(rc), state(kNormal)
		{	}

		bool	ModifyState(ButtonState newState)
		{
			ButtonState oldState = state;
			state = newState;
			return oldState != newState;
		}
	};

	// Data members
	std::vector<CommandButton>	m_vecCommandButton;
	CFont	m_font;
	CTheme	m_themeRebar;
	ChevronState	m_ChevronState;
	CSize	m_ChevronSize;

	CMenu	m_menu;

	int		m_nHotIndex;
	int		m_nPressedIndex;

	static IBasePopupMenu* s_pSubMenu;
	static HHOOK	s_hHook;
	static HWND		s_hWnd;

};

IBasePopupMenu* CDonutCommandBar::Impl::s_pSubMenu = nullptr;
HHOOK	CDonutCommandBar::Impl::s_hHook	= NULL;
HWND	CDonutCommandBar::Impl::s_hWnd	= NULL;

#include "DonutCommandBar.inl"


////////////////////////////////////////////////////////////////////////////
// CDonutCommandBar

CDonutCommandBar::CDonutCommandBar() : pImpl(new Impl)
{	}

CDonutCommandBar::~CDonutCommandBar()
{
	delete pImpl;
}

HWND	CDonutCommandBar::Create(HWND hWndParent)
{
	return pImpl->Create(hWndParent);
}

void	CDonutCommandBar::SetFont(HFONT hFont)
{
	pImpl->SetFont(hFont);
}

void	CDonutCommandBar::SetRecentClosedTabList(CRecentClosedTabList* pList)
{
	CRecentClosedTabPopupMenu::SetRecentClosedTabList(pList);
}

