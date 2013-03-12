/**
 *	@file	DonutStatusBarCtrl.h
 *	@brief	ステータス・バー
 */

#pragma once

#include "CmbboxPrxy.h"


//////////////////////////////////////////////////////////////
// CCrasyMap	: be care, if no element, &array[0] is invalid.

template <class _Key, class _Val>
class CCrasyMap : public CSimpleArray< std::pair<_Key, _Val> > 
{
public:
	bool FindKey(const _Key &__key)
	{
		if (GetSize() == 0)
			return false;

		return end() != std::find_if( begin(), end(), _Finder(__key) );
	}


	void Add(const _Key &__key, const _Val &__val)
	{
		if (GetSize() == 0) {
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
			return;
		}

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p == end() )
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
		else
			(*p).second = __val;
	}


	_Val Lookup(const _Key &__key)
	{
		if (GetSize() == 0)
			return _Val();

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p != end() )
			return p->second;
		else
			return _Val();
	}


	void Sort()
	{
		if (GetSize() == 0)
			return;

		std::sort( begin(), end(), _Compare() );
	}


	std::pair<_Key, _Val>* begin()
	{
		return &(*this)[0];
	}


	std::pair<_Key, _Val>* end()
	{
		return &(*this)[0] + GetSize();
	}


	//	template <class _Key, class _Val>
	struct _Finder {
		_Key __aKey;
		_Finder(_Key __key) : __aKey(__key) { }
		bool operator ()(const std::pair<_Key, _Val> &src)
		{
			return (__aKey == src.first);
		}
	};

	//	template <class _Key, class _Val>
	struct _Compare {
		bool operator ()(const std::pair<_Key, _Val> &x, const std::pair<_Key, _Val> &y)
		{
			return x.second < y.second;
		}
	};
};



//////////////////////////////////////////////////////////////////////////
// CDonutStatusBarCtrl

class CDonutStatusBarCtrl 
	: public CMultiPaneStatusBarCtrlImpl<CDonutStatusBarCtrl>
	, public COwnerDraw<CDonutStatusBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS( _T("DonutStatusBar"), GetWndClassName() )

	// Constructor/Destructor
	CDonutStatusBarCtrl();
	~CDonutStatusBarCtrl();

	CComboBoxPrxyR& GetProxyComboBox() { return m_cmbProxy; }
	void	SetProxyComboBoxFont(HFONT hFont, bool bRedraw = true);

	void	SetIcon(int nIdPane, int nIndexIcon);
	void	SetCommand(int nPaneID, int nCommand) { m_mapCmdID.Add(nPaneID, nCommand); }

	// Overrides
	void	DrawItem(LPDRAWITEMSTRUCT lpdis);

	void	SetOwnerDraw(BOOL bOwnerdraw);

	bool	IsValidBmp() const { return m_bmpBg.m_hBitmap != 0; }

	void	ReloadSkin(int nStyle, int colText /*=0*/, int colBack /*=-1*/);

	void	SetText(int nPane, LPCTSTR lpszText, int nType = 0);

	int		GetPaneCount() { return GetParts(-1, NULL); }

	void	DrawBackGround(LPRECT lpRect, HDC hDCSrc, HDC hDCDest);

	void	OnImageListUpdated();

public:
	// Message map
	BEGIN_MSG_MAP(CDonutStatusBarCtrl)
		USER_MSG_WM_GET_OWNERDRAWMODE (OnGetOwnerDrawMode)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MESSAGE_HANDLER 	(WM_CREATE			, OnCreate		)
		MESSAGE_HANDLER 	(WM_STATUS_SETICON	, OnSetIcon 	)
		MESSAGE_HANDLER 	(WM_STATUS_SETTIPTEXT,OnSetTipText	)
		MESSAGE_HANDLER 	(WM_SETTEXT 		, OnSetText 	)
		MESSAGE_HANDLER 	(SB_SETTEXT 		, OnSetStatusText)
		MESSAGE_HANDLER 	(SB_SIMPLE			, OnSimple		)
		MSG_WM_SIZE			( OnSize )
		CHAIN_MSG_MAP		(CMultiPaneStatusBarCtrlImpl<CDonutStatusBarCtrl>)
		CHAIN_MSG_MAP_ALT	(COwnerDraw<CDonutStatusBarCtrl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetTipText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSimple(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnGetOwnerDrawMode() { return IsValidBmp(); }
	LRESULT OnSetStatusText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	void	OnSize(UINT nType, CSize size);

private:
  #if 0
	struct _IconInfo {
		int 	nIconIndex;
		int 	nIDPane;
		HICON	hIcon;
	};
  #endif
	CProgressBarCtrl		m_wndProgress;
	CComboBoxPrxyR			m_cmbProxy;

	std::map<int, HICON>	m_mapIcon;
	CBitmap 				m_bmpBg;
	CImageList				m_imgList;
	CCrasyMap<int, int> 	m_mapCmdID; 		// CmdID
	CString 				m_strText;
	BOOL					m_bOwnerdraw;
	int 					m_nStatusStyle;
	COLORREF				m_colText;
	COLORREF				m_colBack;
	BOOL					m_bUseBackColor;
};

