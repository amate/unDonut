/**
 *	@file	MtlCtrls.cpp
 *	@brief	MTL : テンプレートライブラリでなくコントロールクラスを使ったモノ等雑多
 */

#include "stdafx.h"
#include "MtlCtrls.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




// この関数は何故か MtlMisc.h の中に存在し、CReBarCtrl といったコントロールクラスを
// 使用し、他の関数が使用しないヘッダファイルを要求するので隔離した次第である。
void MtlRefreshBandIdealSize(CReBarCtrl rebar, CToolBarCtrl toolbar)
{
	REBARBANDINFO rbBand;

	rbBand.cbSize = sizeof (REBARBANDINFO);
	rbBand.fMask  = RBBIM_IDEALSIZE
				  // | RBBIM_SIZE			//+++
				  ;

	// Calculate the size of the band
	int 		  nBtnCount = toolbar.GetButtonCount();

	if (nBtnCount > 0) {
		RECT	rcTmp;
		BOOL	bRet	= toolbar.GetItemRect(nBtnCount - 1, &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cxIdeal	= rcTmp.right;
		// rbBand.cx	= rcTmp.right;		//+++
		int 	nIndex	= rebar.IdToIndex( toolbar.GetDlgCtrlID() );
		rebar.SetBandInfo(nIndex, &rbBand);
	}
}



// この関数もMtlWin.hの中で浮いていたので隔離した
// もしかしたら作ったの自分かも
HWND _CreateSimpleReBarCtrl(HWND hWndParent, DWORD dwStyle, UINT nID)
{
	// Ensure style combinations for proper rebar painting
	if (dwStyle & CCS_NODIVIDER && dwStyle & WS_BORDER)
		dwStyle &= ~WS_BORDER;
	else if ( !(dwStyle & WS_BORDER) && !(dwStyle & CCS_NODIVIDER) )
		dwStyle |= CCS_NODIVIDER;

	// Create rebar window
	HWND	  hWndReBar = ::CreateWindowEx(
								0,
								REBARCLASSNAME,
								NULL,
								dwStyle,
								0,
								0,
								100,
								100,
								hWndParent,
								(HMENU) LongToHandle(nID),
								_Module.GetModuleInstance(),
								NULL );

	if (hWndReBar == NULL) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to create rebar.\n") );
		return NULL;
	}

	// Initialize and send the REBARINFO structure
	REBARINFO rbi;
	rbi.cbSize = sizeof (REBARINFO);
	rbi.fMask  = 0;

	if ( !::SendMessage(hWndReBar, RB_SETBARINFO, 0, (LPARAM) &rbi) ) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to initialize rebar.\n") );
		::DestroyWindow(hWndReBar);
		return NULL;
	}

	return hWndReBar;
}


