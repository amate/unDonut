/**
 *	@file	StdToolBarCtrl.h
 *	@brief	標準ツールバー
 */
#pragma once

#include <utility>
#include "MtlBase.h"
#include "MtlWin.h"
#include "MtlMisc.h"
#include "MtlUser.h"
#include "ToolTipManager.h"



/////////////////////////////////////////////////////////////////////////////
// Standard Tool Bars

// Note. I can't understand the relation between TBBUTTON::iString and TBBUTTONINFO::pszText.
//		 I guess the tool bar control's implementation is broken.
//		 If bellow IE5.01, there is no way to hide toolbar's text and
//		 once you call CToolBarCtrl::AddStrings or set text to button, you can't avoid to show button' text.
//		 In fact, if you set iString with -1, you can hide text, but I'm afraid and did'nt do that.
//		 I have to set iString valid and moreover set text with SetButtonInfo.
//		 cuz while customizing buttons a valid iString is required to TBBUTTON



// Standard ToolBar Styles:
enum EStd_TBStyle {
	STD_TBSTYLE_SMALLICON		= 0x00000001L,
	STD_TBSTYLE_LARGEICON		= 0x00000002L,
	STD_TBSTYLE_ICONS			= (STD_TBSTYLE_SMALLICON | STD_TBSTYLE_LARGEICON),

	STD_TBSTYLE_HIDELABEL		= 0x00000004L,
	STD_TBSTYLE_SHOWLABEL		= 0x00000008L,
	STD_TBSTYLE_SHOWLABELRIGHT	= 0x00000010L,
	STD_TBSTYLE_LABELS			= (STD_TBSTYLE_HIDELABEL | STD_TBSTYLE_SHOWLABEL | STD_TBSTYLE_SHOWLABELRIGHT),

	STD_TBSTYLE_DEFAULT 		= (STD_TBSTYLE_SMALLICON | STD_TBSTYLE_HIDELABEL),	//+++ 好みで変更. (STD_TBSTYLE_LARGEICON | STD_TBSTYLE_SHOWLABEL),
	STD_TBSTYLE_ALL 			= (STD_TBSTYLE_ICONS | STD_TBSTYLE_LABELS),
};


enum { BTNS_STD_LIST			= 0x00010000L, };

// // UH
// enum { STD_BUTTON_CNT			= 42, };			//+++ 未使用のよう


// Standard ToolBar Button:
typedef struct _STD_TBBUTTON {
	int 	idCommand;
	DWORD	fsStyle;		// BTNS_
} STD_TBBUTTON, NEAR *PSTD_TBBUTTON, FAR *LPSTD_TBBUTTON;


typedef const STD_TBBUTTON FAR	 *LPCSTD_TBBUTTON;




/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, class TBase = CStdToolBarCtrlBase, class TWinTraits = CControlWinTraits>
class CStdToolBarCtrlImpl;

class CStdToolBarCtrl;

template <class T>
class	CCustomizableStdToolBarHandler;

class	CStdToolBarAdditionalCustomizeDlg;



// for debug
#ifdef _DEBUG
const bool	_Mtl_StdToolBarCtrl_traceOn = false;
	#define stbTRACE	if (_Mtl_StdToolBarCtrl_traceOn)  ATLTRACE
#else
	#define stbTRACE
#endif



/////////////////////////////////////////////////////////////////////////////
// CStdToolBarCtrlBase - base class for the Standard Tool Bar implementation

class CStdToolBarCtrlBase : public CToolBarCtrl {
	enum { s_kcxSeparator	= 8 };

public:
	static bool _StdToolBar_HideLabel(HWND hWndToolBar)
	{
		CToolBarCtrl toolbar(hWndToolBar);
		_CLockRedraw lock(hWndToolBar);

		toolbar.ModifyStyle(0, TBSTYLE_LIST);

		// If IE < 5.01, there is no methods to hide button's texts.
		if ( !_check_flag( TBSTYLE_EX_MIXEDBUTTONS, toolbar.GetExtendedStyle() ) )
			return false;

		for (int i = 0; i < toolbar.GetButtonCount(); ++i) {
			// get button ID
			TBBUTTON				 tbb;
			toolbar.GetButton(i, &tbb);
			int 					 nCmdID  = tbb.idCommand;
			BYTE					 fsStyle = tbb.fsStyle;

			// separator or not
			if (fsStyle & BTNS_SEP)
				continue;

			//			fsStyle &= ~BTNS_AUTOSIZE;	// if no label, no need to do it
			fsStyle   |= BTNS_AUTOSIZE;
			fsStyle   &= ~BTNS_SHOWTEXT;

			CVersional<TBBUTTONINFO> bi;
			bi.dwMask  = TBIF_STYLE;
			bi.fsStyle = fsStyle;
			toolbar.SetButtonInfo(nCmdID, &bi);
		}

		toolbar.AutoSize();
		return true;
	}


public:
	static bool _StdToolBar_ShowLabel(HWND hWndToolBar, const STD_TBBUTTON *ptbbFirst, const STD_TBBUTTON *ptbbLast)
	{
		CToolBarCtrl toolbar(hWndToolBar);
		_CLockRedraw lock(hWndToolBar);

		toolbar.ModifyStyle(TBSTYLE_LIST, 0);
		StdToolBar_AddStrings(hWndToolBar, ptbbFirst, ptbbLast);	// add it now!

		for (int i = 0; i < toolbar.GetButtonCount(); ++i) {
			// get button ID and Style
			TBBUTTON				 tbb;
			toolbar.GetButton(i, &tbb);
			int 					 nCmdID  = tbb.idCommand;
			BYTE					 fsStyle = tbb.fsStyle;

			// separator or not
			if (fsStyle & BTNS_SEP)
				continue;

			// update info
			CVersional<TBBUTTONINFO> bi;
			bi.dwMask  = TBIF_STYLE;

			fsStyle   |= BTNS_AUTOSIZE; 		// fixed button size (IE5 style) or not
			fsStyle   |= BTNS_SHOWTEXT;
			bi.fsStyle = fsStyle;
			toolbar.SetButtonInfo(nCmdID, &bi);
		}

		//		StdToolBar_AddStrings(hWndToolBar, ptbbFirst, ptbbLast);// add it now!
		toolbar.AutoSize();
		return true;
	}


	static bool _StdToolBar_ShowLabelRight(HWND hWndToolBar, const STD_TBBUTTON *ptbbFirst, const STD_TBBUTTON *ptbbLast)
	{
		CToolBarCtrl	  toolbar(hWndToolBar);
		_CLockRedraw	  lock(hWndToolBar);

		toolbar.ModifyStyle(0, TBSTYLE_LIST);
		StdToolBar_AddStrings(hWndToolBar, ptbbFirst, ptbbLast);

		CSimpleArray<int> arrShowRightID;

		for (; ptbbFirst != ptbbLast; ++ptbbFirst) {
			if (ptbbFirst->fsStyle & BTNS_STD_LIST) {
				int nCmdID = ptbbFirst->idCommand;
				arrShowRightID.Add(nCmdID);
			}
		}

		for (int i = 0; i < toolbar.GetButtonCount(); ++i) {
			// get button ID and Style
			TBBUTTON	tbb;
			toolbar.GetButton(i, &tbb);

			int 		nCmdID	= tbb.idCommand;
			BYTE		fsStyle = tbb.fsStyle;

			// separator or not
			if (fsStyle & BTNS_SEP)
				continue;

			// update info
			CVersional<TBBUTTONINFO>	bi;
			bi.dwMask  = TBIF_STYLE;

			fsStyle   |= BTNS_AUTOSIZE;

			if (arrShowRightID.Find(nCmdID) != -1)
				fsStyle |= BTNS_SHOWTEXT;
			else
				fsStyle &= ~BTNS_SHOWTEXT;

			bi.fsStyle = fsStyle;
			toolbar.SetButtonInfo(nCmdID, &bi);
		}

		toolbar.AutoSize();
		return true;
	}


private:
	static bool StdToolBar_AddStrings(HWND hWndToolBar, const STD_TBBUTTON *ptbbFirst, const STD_TBBUTTON *ptbbLast)
	{
		LPTSTR		 lpsz = _MakeStrings(ptbbFirst, ptbbLast);

		if (lpsz == NULL)
			return false;

		CToolBarCtrl toolbar(hWndToolBar);
		toolbar.AddStrings(lpsz);
		delete[] lpsz;


		//toolbar.AddStrings(_T("NS\0"));
		return true;
	}


	static LPTSTR _MakeStrings(const STD_TBBUTTON *ptbbFirst, const STD_TBBUTTON *ptbbLast)
	{
		int 					nLen	= 0;
		CSimpleArray<CString>	arrText;

		for (; ptbbFirst != ptbbLast; ++ptbbFirst) {
			CString 	strText;

			if ( _check_flag(BTNS_SEP, ptbbFirst->fsStyle) )
				continue;

			if ( CToolTipManager::LoadToolTipText(ptbbFirst->idCommand, strText) ) {
				nLen += strText.GetLength() + 1;
				arrText.Add(strText);
			}
		}

		if (nLen == 0)
			return NULL;

		nLen += 1;

		LPTSTR	lpsz		 = new TCHAR[nLen];
		::memset(lpsz, 0, nLen);

		LPTSTR	lpszRunner = lpsz;

		for (int i = 0; i < arrText.GetSize(); ++i) {
			::lstrcpy(lpszRunner, arrText[i]);
			lpszRunner += arrText[i].GetLength() + 1;
		}

		return lpsz;
	}


	static bool _LoadToolTipText(int nCmdID, CString &strText)
	{
		enum { TEXT_LEN = 256 };
		TCHAR szBuff[TEXT_LEN+1];
		TCHAR szText[TEXT_LEN+1];

		szBuff[0] = 0;
		int   nRet = ::LoadString(_Module.GetResourceInstance(), nCmdID, szBuff, TEXT_LEN);

		for (int i = 0; i < nRet; i++) {
			if ( szBuff[i] == _T('\n') ) {
				lstrcpyn(szText, &szBuff[i + 1], TEXT_LEN);
				strText = szText;
				return true;
			}
		}

		return false;
	}


	static UINT _HwndToId(HWND hWndReBar, HWND hWndBand)
	{
		CReBarCtrl	  rebar(hWndReBar);

		REBARBANDINFO rbbi;

		rbbi.cbSize = sizeof (REBARBANDINFO);

		for (UINT i = 0; i < rebar.GetBandCount(); ++i) {
			rbbi.fMask = RBBIM_CHILD | RBBIM_ID;
			rebar.GetBandInfo(i, &rbbi);

			if (rbbi.hwndChild == hWndBand) {
				return rbbi.wID;
			}
		}

		return 0;
	}


	static int _HwndToIndex(HWND hWndReBar, HWND hWndBand)
	{
		CReBarCtrl rebar(hWndReBar);

		UINT	   nID = _HwndToId(hWndReBar, hWndBand);

		if (nID == 0)
			return -1;

		return rebar.IdToIndex(nID);
	}


public:
	static bool _UpdateBandInfo(HWND hWndToolBar)
	{
		CToolBarCtrl  toolbar(hWndToolBar);
		CReBarCtrl	  rebar 	= toolbar.GetParent();

		int 		  nCount	= rebar.GetBandCount();

		if (nCount <= 0)  // probably not a rebar
			return false;

		int 		  nBtnCount = toolbar.GetButtonCount();
		CRect		  rcRight;

		if ( !toolbar.GetItemRect(nBtnCount - 1, &rcRight) )
			return false;

		CRect		  rcLeft;

		if ( !toolbar.GetItemRect(0, &rcLeft) )
			return false;

		REBARBANDINFO rbbi;
		rbbi.cbSize 	= sizeof (REBARBANDINFO);
		rbbi.fMask		= RBBIM_IDEALSIZE | RBBIM_CHILDSIZE;
		rbbi.cxMinChild = rcLeft.right;
		rbbi.cyMinChild = rcLeft.Height();
		rbbi.cxIdeal	= rcRight.right;

		int 		  nIndex	= _HwndToIndex(rebar, hWndToolBar);

		return (rebar.SetBandInfo(nIndex, &rbbi) == TRUE);
	}


	static TBBUTTON *_MakeFullTBBtns(const STD_TBBUTTON *ptbbFirst, const STD_TBBUTTON *ptbbLast, bool bValidateString)
	{
		int 	  nCount = int (ptbbLast - ptbbFirst);

		if (nCount == 0)
			return NULL;

		TBBUTTON *pTBBtn = new TBBUTTON[nCount];

		int 	  nBmp	 = 0;
		int 	  j 	 = 0;

		for (; ptbbFirst != ptbbLast; ++ptbbFirst) {
			ATLASSERT(j < nCount);

			pTBBtn[j].iBitmap	= nBmp;
			pTBBtn[j].idCommand = ptbbFirst->idCommand;
			pTBBtn[j].fsState	= TBSTATE_ENABLED;
			pTBBtn[j].fsStyle	= (BYTE) (ptbbFirst->fsStyle & ~BTNS_STD_LIST);
			pTBBtn[j].dwData	= 0;
			// Note. If invalid index, no text.
			pTBBtn[j].iString	= bValidateString ? nBmp : 0;

			++nBmp;
			++j;
		}

		return pTBBtn;
	}


	static TBBUTTON _GetSeparatorTBBtn()
	{
		TBBUTTON tbBtn;

		tbBtn.iBitmap	= s_kcxSeparator;
		tbBtn.idCommand = 0;
		tbBtn.fsState	= 0;
		tbBtn.fsStyle	= TBSTYLE_SEP;
		tbBtn.dwData	= 0;
		tbBtn.iString	= 0;
		return tbBtn;
	}


	static TBBUTTON _GetTBBtn(const STD_TBBUTTON &stb, int iBitmap)
	{
		TBBUTTON tbBtn;

		tbBtn.iBitmap	= iBitmap;
		tbBtn.idCommand = stb.idCommand;
		tbBtn.fsState	= TBSTATE_ENABLED;
		tbBtn.dwData	= 0;
		tbBtn.fsStyle	= (BYTE) (stb.fsStyle & ~BTNS_STD_LIST);
		tbBtn.iString	= iBitmap;
		return tbBtn;
	}


private:
	// Locking redraw
	struct _CLockRedraw {
		CWindow m_wnd;
		_CLockRedraw(HWND hWnd) : m_wnd(hWnd)
		{
			if (m_wnd.m_hWnd) {
				m_wnd.SetRedraw(FALSE);
			}
		}


		~_CLockRedraw()
		{
			if (m_wnd.m_hWnd) {
				m_wnd.SetRedraw(TRUE);
				m_wnd.Invalidate();
				m_wnd.UpdateWindow();
			}
		}
	};

};



/////////////////////////////////////////////////////////////////////////////
// CStdToolBarCtrlImpl - ATL implementation of Standard Tool Bars
//template <class T, class TBase = CStdToolBarCtrlBase, class TWinTraits = CControlWinTraits>

template <class T, class TBase, class TWinTraits>
class ATL_NO_VTABLE CStdToolBarCtrlImpl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public CCustomizableStdToolBarHandler<T>
{
public:
	DECLARE_WND_SUPERCLASS( NULL, TBase::GetWndClassName() )

	// Data members
	DWORD						m_dwStdToolBarStyle;
	CSimpleArray<STD_TBBUTTON>	m_arrStdBtn;
	CSimpleArray<int>			m_arrBmpDefaultIndex;

private:
	std::pair<int, int> 		m_bmpIDSmall;
	CSize						m_sizeSmall;
	std::pair<int, int> 		m_bmpIDLarge;
	CSize						m_sizeLarge;
	COLORREF					m_clrMask;
	UINT						m_nFlags;


public:
	// Ctor/Dtor
	CStdToolBarCtrlImpl()
		: m_dwStdToolBarStyle(0)
	{
	}


	bool StdToolBar_Init(
			const STD_TBBUTTON *		ptbbFirst,
			const STD_TBBUTTON *		ptbbLast,
			const int * 				pBmpFirst,
			const int * 				pBmpLast,
			const std::pair<int, int>&	bmpIDSmall,
			const std::pair<int, int>&	bmpIDLarge	= std::make_pair(0, 0),
			CSize						sizeSmall	= CSize(16, 16),
			CSize						sizeLarge	= CSize(20, 20),
			COLORREF					clrMask 	= RGB(255, 0, 255),
			UINT						nFlags		= ILC_COLOR24)
	{
		for (; ptbbFirst != ptbbLast; ++ptbbFirst) {
			ATLASSERT(_check_flag(BTNS_SEP, ptbbFirst->fsStyle) == false);
		  #if _ATL_VER >= 0x700
			m_arrStdBtn.Add(*ptbbFirst);
		  #else //+++
			m_arrStdBtn.Add(*(STD_TBBUTTON*)ptbbFirst);
		  #endif
		}

		for (; pBmpFirst != pBmpLast; ++pBmpFirst) {
		  #if _ATL_VER >= 0x700
			m_arrBmpDefaultIndex.Add(*pBmpFirst);
		  #else //+++
			m_arrBmpDefaultIndex.Add(*(int*)pBmpFirst);
		  #endif
		}

		m_bmpIDSmall = bmpIDSmall;
		m_sizeSmall  = sizeSmall;
		m_bmpIDLarge = bmpIDLarge;
		m_sizeLarge  = sizeLarge;

		m_clrMask	 = clrMask;
		m_nFlags	 = nFlags;

		return true;
	}


	bool StdToolBar_InitButtons(int *pFirstIndex, int *pLastIndex)
	{
		// clean up previous toolbar buttons
		while ( DeleteButton(0) )
			;

		for (; pFirstIndex != pLastIndex; ++pFirstIndex) {
			if (*pFirstIndex == -1) {			// separator
				TBBUTTON tbBtn = _GetSeparatorTBBtn();
				InsertButton(-1, &tbBtn);
			} else if ( 0 <= *pFirstIndex && *pFirstIndex < m_arrStdBtn.GetSize() ) {
				TBBUTTON tbBtn = _GetTBBtn(m_arrStdBtn[*pFirstIndex], *pFirstIndex);
				tbBtn.fsStyle |= BTNS_AUTOSIZE; // default
				InsertButton(-1, &tbBtn);
			}
		}

		return true;
	}


private:
	void StdToolBar_Term()
	{
		StdToolBar_DestroyImageList();

		T	*pT = static_cast<T *>(this);	//+++ 元 USES_PT;
		pT->StdToolBar_WriteProfile();
	}


public:
	HWND StdToolBar_Create(
			HWND	hWndParent,
			DWORD	dwStyle 		= ATL_SIMPLE_TOOLBAR_PANE_STYLE,
			DWORD	dwExtendedStyle = TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS,
			UINT	nID 			= ATL_IDW_TOOLBAR	)
	{
		m_hWnd = Create(hWndParent, rcDefault, NULL, dwStyle, 0, nID);
		ATLASSERT( ::IsWindow(m_hWnd) );

		SetExtendedStyle(dwExtendedStyle);		// before adding buttons!
		SetButtonStructSize( sizeof (TBBUTTON) );

		T	*pT = static_cast<T *>(this);	//+++ 元 USES_PT;
		pT->StdToolBar_GetProfile();

		return m_hWnd;
	}


private:
	// Attributes
	int StdToolBar_GetIconCount()
	{
		return m_arrStdBtn.GetSize();
	}


	// Overridables
	LRESULT StdToolBar_OnDropDown(int nCmdID)
	{
		return TBDDRET_DEFAULT;
	}


	void StdToolBar_GetProfile()
	{
		StdToolBar_InitButtons( _begin(m_arrBmpDefaultIndex), _end(m_arrBmpDefaultIndex) );
	}


	void StdToolBar_WriteProfile()
	{
	}


public:
	// Methods
	bool StdToolBar_Reset()
	{
		StdToolBar_InitButtons( _begin(m_arrBmpDefaultIndex), _end(m_arrBmpDefaultIndex) );
		return StdToolBar_ModifyStyle(STD_TBSTYLE_ALL, STD_TBSTYLE_DEFAULT);
	}


private:
	bool StdToolBar_ModifyStyle(DWORD dwRemove, DWORD dwAdd)
	{
		DWORD dwStyleOld = m_dwStdToolBarStyle;

		m_dwStdToolBarStyle = (m_dwStdToolBarStyle & ~dwRemove) | dwAdd;

		if (dwStyleOld != m_dwStdToolBarStyle) {
			StdToolBar_UpdateImageList();
			StdToolBar_UpdateStyle();
			_UpdateBandInfo(m_hWnd);
		}

		return true;
	}


public:
	void StdToolBar_SetStyle(DWORD dwStyle, bool bForceUpdateStyle = false)
	{
		DWORD	dwStyleOld  = m_dwStdToolBarStyle;

		m_dwStdToolBarStyle = dwStyle;

		if (dwStyleOld != m_dwStdToolBarStyle) {
			StdToolBar_UpdateImageList();
			StdToolBar_UpdateStyle();
			_UpdateBandInfo(m_hWnd);
		} else if (bForceUpdateStyle) {
			StdToolBar_UpdateStyle();	// for BTNS_STD_LIST
			_UpdateBandInfo(m_hWnd);
		}
	}


private:
	DWORD StdToolBar_GetStyle() const
	{
		return m_dwStdToolBarStyle;
	}


public:
	bool StdToolBar_TrackDropDownMenu(int nCmdID, HMENU hMenu, HWND hWndOwner)
	{
		CRect	  rc;

		GetItemRect(CommandToIndex(nCmdID), &rc);
		ClientToScreen(&rc);
		UINT	  uMenuFlags = TPM_LEFTBUTTON /*| TPM_RIGHTBUTTON*/ | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN |
							   (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
		TPMPARAMS TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rc;

		return (::TrackPopupMenuEx(hMenu, uMenuFlags, rc.left, rc.bottom, hWndOwner, &TPMParams) == TRUE);
	}


public:
	void StdToolBar_Customize()
	{
		CStdToolBarAdditionalCustomizeDlg dlg;

		dlg.Install(m_dwStdToolBarStyle, m_hWnd);

		Customize();

		StdToolBar_SetStyle(dlg.m_dwStdToolBarStyle, true);
	}


private:
	// Implementation
	bool StdToolBar_UpdateStyle()
	{
		if ( _check_flag(STD_TBSTYLE_HIDELABEL, m_dwStdToolBarStyle) )
			_StdToolBar_HideLabel(m_hWnd);
		else if ( _check_flag(STD_TBSTYLE_SHOWLABEL, m_dwStdToolBarStyle) )
			_StdToolBar_ShowLabel( m_hWnd, _begin(m_arrStdBtn), _end(m_arrStdBtn) );
		else if ( _check_flag(STD_TBSTYLE_SHOWLABELRIGHT, m_dwStdToolBarStyle) )
			_StdToolBar_ShowLabelRight( m_hWnd, _begin(m_arrStdBtn), _end(m_arrStdBtn) );
		else
			ATLASSERT(FALSE);

		return true;
	}



public:
	bool StdToolBar_UpdateImageList()
	{
		static const TCHAR* tbl[2][2][2] = {
			{
				{ _T("Small.bmp")	, _T("SmallDisable.bmp"), },
				{ _T("SmallHot.bmp"), _T("SmallDisable.bmp"), },
			}, {
				{ _T("Big.bmp") 	, _T("BigDisable.bmp"), },
				{ _T("BigHot.bmp")	, _T("BigDisable.bmp"), },
			},
		};

		if ( !StdToolBar_IsImageListNeedUpdate() )
			return false;

		int mode =	_check_flag(STD_TBSTYLE_SMALLICON, m_dwStdToolBarStyle) ? 0
				 :	_check_flag(STD_TBSTYLE_LARGEICON, m_dwStdToolBarStyle) ? 1
				 :															 -1;
		if (mode < 0) {
			ATLASSERT(FALSE);
			return false;
		}

		bool	   bmpF   = true;
		CString strDir	  = _GetSkinDir();
		CSize*	   pSize  = mode ? &m_sizeLarge: &m_sizeSmall;
		int 	   nCount = StdToolBar_GetIconCount();
		CString strBase   = strDir + tbl[mode][0][0];
		CString strHot	  = strDir + tbl[mode][1][0];
		CString strDis	  = strDir + tbl[mode][0][1];

		CBitmap    bmp;
		bmp.Attach( AtlLoadBitmapImage( strBase.GetBuffer(0), LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL) {			//+++ 一応デフォルトを用意.
			//if (IsExistManifestFile() == 0)
			bmp.LoadBitmap(IDB_MAINFRAME_TOOLBAR);
			bmpF = false;						//+++ 内蔵のを使った場合off
		}
		if (bmp.m_hBitmap) {
			bmp.GetSize( *pSize );
		} else {
			if (mode == 0)
				*pSize = CSize(16, 16);
			else
				*pSize = CSize(21, 21);
		}

		pSize->cx  = pSize->cy;

		CImageList imgs;
		MTLVERIFY( imgs.Create(pSize->cx, pSize->cy, m_nFlags | ILC_MASK, nCount, 1) );
		if (bmp.m_hBitmap) {
			MTLVERIFY( imgs.Add(bmp, m_clrMask) != -1 );
		}

		CBitmap    bmpHot;
		bmpHot.Attach( AtlLoadBitmapImage( strHot.GetBuffer(0), LR_LOADFROMFILE ) );
		if (bmpHot.m_hBitmap == NULL) { 		//+++ 一応デフォルトを用意.
			if (bmpF == 0)
				bmpHot.LoadBitmap(IDB_MAINFRAME_TOOLBAR_HOT);
		}
		CImageList imgsHot;
		MTLVERIFY( imgsHot.Create(pSize->cx, pSize->cy, m_nFlags | ILC_MASK, nCount, 1) );
		if (bmpHot.m_hBitmap) {
			MTLVERIFY( imgsHot.Add(bmpHot, m_clrMask) != -1 );
		}

		CBitmap    bmpDis;
		bmpDis.Attach( AtlLoadBitmapImage( strDis.GetBuffer(0), LR_LOADFROMFILE) );
		if (bmpDis.m_hBitmap == NULL) { 		//+++ 一応デフォルトを用意.
			if (bmpF == 0)
				bmpDis.LoadBitmap(IDB_MAINFRAME_TOOLBAR_DIS);
		}
		CImageList imgsDis;
		MTLVERIFY( imgsDis.Create(pSize->cx, pSize->cy, m_nFlags | ILC_MASK, nCount, 1) );
		if (bmpDis.m_hBitmap) {
			MTLVERIFY( imgsDis.Add(bmpDis, m_clrMask) != -1 );
		}

		SetImageList(imgs).Destroy();
		SetHotImageList(imgsHot).Destroy();
		if (bmpDis.m_hBitmap)
			SetDisabledImageList(imgsDis).Destroy();

		return true;
	}



public:
	void StdToolBar_DestroyImageList()
	{
		CImageList imgsOld	  = GetImageList();
		SetImageList(NULL);
		imgsOld.Destroy();

		CImageList imgsHotOld = GetHotImageList();
		SetHotImageList(NULL);
		imgsHotOld.Destroy();

		CImageList imgsDisabledOld = GetDisabledImageList();
		SetDisabledImageList(NULL);
		imgsDisabledOld.Destroy();
	}


private:
	bool StdToolBar_IsImageListNeedUpdate()
	{
		CImageList imgs = GetImageList();

		if ( imgs.IsNull() )
			return true;

		CSize	   size;
		imgs.GetIconSize(size);

		if (_check_flag(STD_TBSTYLE_SMALLICON, m_dwStdToolBarStyle) && size == m_sizeSmall)
			return false;
		else if (_check_flag(STD_TBSTYLE_LARGEICON, m_dwStdToolBarStyle) && size == m_sizeLarge)
			return false;
		else
			return true;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CStdToolBarCtrlImpl)
		MESSAGE_HANDLER( WM_LBUTTONDBLCLK, OnLButtonDblClk )
		MESSAGE_HANDLER( WM_DESTROY 	 , OnDestroy	   )
		REFLECTED_NOTIFY_CODE_HANDLER( TBN_DROPDOWN , OnDropDown   )
		REFLECTED_NOTIFY_CODE_HANDLER( NM_CUSTOMDRAW, OnCustomDraw )
		CHAIN_MSG_MAP  ( CCustomizableStdToolBarHandler<T> )
	END_MSG_MAP()


private:
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		LRESULT lRet   = 0;
		CPoint	pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );

		int 	nIndex = HitTest(&pt);
		if (nIndex < 0) {
			StdToolBar_Customize();
		} else {
			bHandled = FALSE;
		}

		return lRet;
	}


	LRESULT OnCustomDraw(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		//x 	LPNMTBCUSTOMDRAW lpnmtb = (LPNMTBCUSTOMDRAW)pnmh;
		bHandled = TRUE;
		return S_OK;
	}


	LRESULT OnDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL & /*bHandled*/)
	{
		LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;
		int 		nCmdID = lpnmtb->iItem;

		T	*pT = static_cast<T *>(this);	//+++ 元 USES_PT;
		return pT->StdToolBar_OnDropDown(nCmdID);
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		bHandled = FALSE;

		StdToolBar_Term();

		return 1;
	}

};



/////////////////////////////////////////////////////////////////////////////
// CCustomizableStdToolBarHandler

template <class T>
class CCustomizableStdToolBarHandler {
public:
	// Data members
	TBBUTTON *	m_pTBBtn;	// on demand
	int 		m_nItems;

	// Ctor
	CCustomizableStdToolBarHandler() : m_pTBBtn(NULL), m_nItems(0) { }

	// Overridable
	void OnTbnCustHelp()
	{
		// you can override to deal with HELP.
	}


	// Message map and handlers
	BEGIN_MSG_MAP(CCustomizableStdToolBarHandler)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_BEGINADJUST  , OnTbnBeginAdjust	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_ENDADJUST    , OnTbnEndAdjust 	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_QUERYINSERT  , OnTbnQueryInsert	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_QUERYDELETE  , OnTbnQueryDelete	)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_GETBUTTONINFO, OnTbnGetButtonInfo )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_TOOLBARCHANGE, OnTbnToolBarChange )
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_RESET 	   , OnTbnReset 		)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_CUSTHELP	   , OnTbnCustHelp		)
		// NOTIFY_HANDLER_EX( m_toolbar.GetDlgCtrlID(), TBN_BEGINDRAG, OnTbnBeginDrag )
		// NOTIFY_HANDLER_EX( m_toolbar.GetDlgCtrlID(), TBN_ENDDRAG  , OnTbnEndDrag   )
	END_MSG_MAP()


	LRESULT OnTbnBeginAdjust(LPNMHDR pnmh)
	{
		_CreateTBBtns();
		return 0;
	}


	LRESULT OnTbnEndAdjust(LPNMHDR pnmh)
	{
		_DestroyTBBtns();
		return 0;
	}


	LRESULT OnTbnQueryInsert(LPNMHDR pnmh)
	{
		return TRUE;
	}


	LRESULT OnTbnQueryDelete(LPNMHDR pnmh)
	{
		return TRUE;
	}


	LRESULT OnTbnGetButtonInfo(LPNMHDR pnmh)
	{
		ATLASSERT(m_pTBBtn != NULL);
		//x 	T* pT = static_cast<T*>(this);

		LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;

		// Note. If over IE5.01, I guess this is the best way to call
		//		 CStdToolBarCtrlBase::StdToolBar_AddStrings(pT->m_hWnd, _begin(arrStdBtn), _end(arrStdBtn));
		//		 I can't understand the relation between lpnmtb->pszText and lpnmtb->tbButton.iString.
		if (lpnmtb->iItem < m_nItems) {
			lpnmtb->pszText[0] = 0;

			CString strText;

			if ( CToolTipManager::LoadToolTipText(m_pTBBtn[lpnmtb->iItem].idCommand, strText) ) {
				// I don't know the buffer size of lpnmtb->pszText...
				strText = strText.Left(30);
				::lstrcpy(lpnmtb->pszText, strText);
			}

			lpnmtb->tbButton   = m_pTBBtn[lpnmtb->iItem];

			return TRUE;
		} else {
			return FALSE;
		}
	}


	LRESULT OnTbnToolBarChange(LPNMHDR pnmh)
	{
		T *pT = static_cast<T *>(this);

		CStdToolBarCtrlBase::_UpdateBandInfo(pT->m_hWnd);

		return 0;
	}


	LRESULT OnTbnReset(LPNMHDR pnmh)
	{
		ATLASSERT(m_pTBBtn != NULL);
		T *pT = static_cast<T *>(this);
		pT->StdToolBar_Reset();
		return 0;
	}


	LRESULT OnTbnCustHelp(LPNMHDR pnmh)
	{
		T *pT = static_cast<T *>(this);

		pT->OnTbnCustHelp();
		return 0;
	}


	LRESULT OnTbnBeginDrag(LPNMHDR pnmh)
	{
		LPTBNOTIFY ptbn = (LPTBNOTIFY) pnmh;

		return 0;
	}


	LRESULT OnTbnEndDrag(LPNMHDR pnmh)
	{
		LPTBNOTIFY ptbn = (LPTBNOTIFY) pnmh;

		return 0;
	}


	// Implementation helpers
	bool _CreateTBBtns()
	{
		ATLASSERT(m_pTBBtn == NULL);
		T * 	pT	= static_cast<T *>(this);
		CSimpleArray<STD_TBBUTTON> &arrStdBtn = pT->m_arrStdBtn;
		m_pTBBtn = CStdToolBarCtrlBase::_MakeFullTBBtns(_begin(arrStdBtn), _end(arrStdBtn), true);
		if (m_pTBBtn == NULL)
			return false;
		m_nItems = arrStdBtn.GetSize();
		return true;
	}


	void _DestroyTBBtns()
	{
		delete[] m_pTBBtn;
		m_pTBBtn = NULL;
		m_nItems = 0;
	}
};



/////////////////////////////////////////////////////////////////////////////
// Adding the special form to the toolbar's customize dialog

class CStdToolBarAdditionalCustomizeDlg
	: public CInitDialogImpl<CStdToolBarAdditionalCustomizeDlg>
	, public CWinDataExchange<CStdToolBarAdditionalCustomizeDlg>
{
public:
	enum { IDD = IDD_FORM_TOOLBAR };

private:
	typedef CInitDialogImpl<CStdToolBarAdditionalCustomizeDlg>	baseClass;

	// Data members
	static HHOOK								s_hCreateHook;
	static CStdToolBarAdditionalCustomizeDlg *	s_pThis;

	CContainedWindow							m_wndParentDlg;
	CContainedWindow							m_wndToolBar;

	CSize	m_sizeDlg;
public:
	DWORD	m_dwStdToolBarStyle;
private:
	int 	m_nIcon;
	int 	m_nText;

public:
	// Ctor/Dtor
	CStdToolBarAdditionalCustomizeDlg() : m_wndParentDlg(this, 1), m_wndToolBar(this, 2) { }

	~CStdToolBarAdditionalCustomizeDlg()
	{
		if ( m_wndParentDlg.IsWindow() )
			m_wndParentDlg.UnsubclassWindow();

		if ( m_wndToolBar.IsWindow() )
			m_wndToolBar.UnsubclassWindow();
	}


	// Methods
	void Install(DWORD dwStyle, HWND hWndToolBar)
	{
		m_wndToolBar.SubclassWindow(hWndToolBar);

		::EnterCriticalSection(&_Module.m_csWindowCreate);

		ATLASSERT(s_hCreateHook == NULL);

		s_pThis 			= this;
		m_dwStdToolBarStyle = dwStyle;

		s_hCreateHook		= ::SetWindowsHookEx( WH_CBT, _CreateHookProc, _Module.GetModuleInstance(), GetCurrentThreadId() );
		ATLASSERT(s_hCreateHook != NULL);
	}


private:
	void Uninstall()
	{
		::UnhookWindowsHookEx(s_hCreateHook);
		s_hCreateHook = NULL;
		s_pThis 	  = NULL;

		::LeaveCriticalSection(&_Module.m_csWindowCreate);
	}


	void _Setup(HWND hWndDlg)
	{
		// uninstall here! otherwize, it will be hanged up.
		Uninstall();

		if (m_wndParentDlg.m_hWnd == NULL) {
			Create(hWndDlg);
			m_wndParentDlg.SubclassWindow(hWndDlg);
			ATLASSERT( ::IsWindow(m_hWnd) );
		}
	}


	// Implementation - Hook procs
	static LRESULT CALLBACK _CreateHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRet = 0;

		if (nCode == HCBT_CREATEWND) {
			HWND 	hWndDlg = (HWND) wParam;
			TCHAR	szClassName[16];
			szClassName[0]	= 0;	//+++
			::GetClassName(hWndDlg, szClassName, 7);

			if ( !lstrcmp(_T("#32770"), szClassName) ) {
				stbTRACE( _T(" Customize dialog found!!\n") );
				s_pThis->_Setup(hWndDlg);
			}
		} else if (nCode < 0) {
			lRet = ::CallNextHookEx(s_hCreateHook, nCode, wParam, lParam);
		}

		return lRet;
	}


	// Message map and handlers
	BEGIN_MSG_MAP( CStdToolBarAdditionalCustomizeDlg )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER( WM_DESTROY   , OnDestroy	 )
	ALT_MSG_MAP(1) // original dlg
		MESSAGE_HANDLER( WM_INITDIALOG, OnParentInitDialog )
	ALT_MSG_MAP(2)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( TBN_RESET, OnTbnReset )
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled )
	{
		stbTRACE( _T("OnInitDialog\n") );
		bHandled  = FALSE;

		CRect rc;
		GetClientRect(&rc);
		m_sizeDlg = CSize( rc.Width(), rc.Height() );

		ExecuteDlgInit();
		_SetData();

		return TRUE;
	}


	LRESULT OnParentInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		stbTRACE( _T("OnParentInitDialog\n") );

		LRESULT lRet = m_wndParentDlg.DefWindowProc(uMsg, wParam, lParam);

		CRect	rcParent;
		m_wndParentDlg.GetWindowRect(&rcParent);
		CRect	rcClient;
		m_wndParentDlg.GetClientRect(&rcClient);

		rcParent.bottom += m_sizeDlg.cy;
		m_wndParentDlg.MoveWindow(&rcParent);

		MoveWindow(0, rcClient.Height(), m_sizeDlg.cx, m_sizeDlg.cy);
		ShowWindow(SW_SHOWNORMAL);

		// now no need.
		m_wndParentDlg.UnsubclassWindow();

		return lRet;
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		stbTRACE( _T("OnDestroy\n") );
		_GetData();
		return 0;
	}


	LRESULT OnTbnReset(LPNMHDR pnmh)
	{
		SetMsgHandled(FALSE);
		m_dwStdToolBarStyle = STD_TBSTYLE_DEFAULT;
		_SetData();
		return 0;
	}


public:
	BEGIN_DDX_MAP(CStdToolBarAdditionalCustomizeDlg)
		DDX_CBINDEX(IDC_COMBO_STDTB_TEXT, m_nText)
		DDX_CBINDEX(IDC_COMBO_STDTB_ICON, m_nIcon)
	END_DDX_MAP()


private:
	void _GetData()
	{
		DoDataExchange(TRUE);

		m_dwStdToolBarStyle = 0;

		if (m_nText == 0)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SHOWLABEL;
		else if (m_nText == 1)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SHOWLABELRIGHT;
		else if (m_nText == 2)
			m_dwStdToolBarStyle |= STD_TBSTYLE_HIDELABEL;

		if (m_nIcon == 0)
			m_dwStdToolBarStyle |= STD_TBSTYLE_SMALLICON;
		else if (m_nIcon == 1)
			m_dwStdToolBarStyle |= STD_TBSTYLE_LARGEICON;
	}


	void _SetData()
	{
		if ( _check_flag(STD_TBSTYLE_HIDELABEL			, m_dwStdToolBarStyle) )
			m_nText = 2;
		else if ( _check_flag(STD_TBSTYLE_SHOWLABEL		, m_dwStdToolBarStyle) )
			m_nText = 0;
		else if ( _check_flag(STD_TBSTYLE_SHOWLABELRIGHT, m_dwStdToolBarStyle) )
			m_nText = 1;
		else
			ATLASSERT(FALSE);

		if ( _check_flag(STD_TBSTYLE_SMALLICON			, m_dwStdToolBarStyle) )
			m_nIcon = 0;
		else if ( _check_flag(STD_TBSTYLE_LARGEICON		, m_dwStdToolBarStyle) )
			m_nIcon = 1;
		else
			ATLASSERT(FALSE);

		DoDataExchange(FALSE);
	}
};



__declspec(selectany) HHOOK 								CStdToolBarAdditionalCustomizeDlg::s_hCreateHook	= NULL;
__declspec(selectany) CStdToolBarAdditionalCustomizeDlg *	CStdToolBarAdditionalCustomizeDlg::s_pThis			= NULL;



class CStdToolBarCtrl : public CStdToolBarCtrlImpl<CStdToolBarCtrl> {
public:
	DECLARE_WND_SUPERCLASS( _T("MTL_StdToolBar"), GetWndClassName() )
};

