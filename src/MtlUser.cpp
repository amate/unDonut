/**
 *	@file	MtlUser.cpp
 */

#include "stdafx.h"
#include "MtlUser.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// __nID == 0 then separator
HWND MtlCreateSimpleToolBarCtrl(
		HWND		hWndParent,
		int *		__firstID,
		int *		__lastID,
		UINT		nImageBmpID,
		UINT		nHotImageBmpID,
		int 		cx,
		int 		cy,
		COLORREF	clrMask,
		UINT		nFlags,
		DWORD		dwStyle,
		UINT		nID)
{
	int 	nCount	= int (__lastID - __firstID);
	if (nCount == 0)
		return NULL;

	CImageList	 imgs;
	MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, nCount, 1) );

	CBitmap 	 bmp;
	MTLVERIFY( bmp.LoadBitmap(nImageBmpID) );
	imgs.Add(bmp, clrMask);

	CImageList	 imgsHot;
	MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, nCount, 1) );

	CBitmap 	 bmpHot;
	MTLVERIFY( bmpHot.LoadBitmap(nHotImageBmpID) );
	imgsHot.Add(bmpHot, clrMask);

	TBBUTTON *	 pTBBtn  = (TBBUTTON *) _alloca( nCount * sizeof (TBBUTTON) );

	int 		 nBmp	 = 0;
	int 		 j		 = 0;

	for (; __firstID < __lastID; ++__firstID) {
		ATLASSERT(j < nCount);

		if (*__firstID != 0) {
			pTBBtn[j].iBitmap	= nBmp++;
			pTBBtn[j].idCommand = *__firstID;
			pTBBtn[j].fsState	= TBSTATE_ENABLED;
			pTBBtn[j].fsStyle	= TBSTYLE_BUTTON;
			pTBBtn[j].dwData	= 0;
			pTBBtn[j].iString	= 0;
		} else {
			pTBBtn[j].iBitmap	= 8;
			pTBBtn[j].idCommand = 0;
			pTBBtn[j].fsState	= 0;
			pTBBtn[j].fsStyle	= TBSTYLE_SEP;
			pTBBtn[j].dwData	= 0;
			pTBBtn[j].iString	= 0;
		}

		++j;
	}

	CToolBarCtrl toolbar = ::CreateWindowEx(
									0,
									TOOLBARCLASSNAME,
									NULL,
									dwStyle,
									0,
									0,
									100,
									100,
									hWndParent,
									(HMENU) LongToHandle(nID),
									_Module.GetModuleInstance(),
									NULL);

	toolbar.SetButtonStructSize( sizeof (TBBUTTON) );
	toolbar.AddButtons(nCount, pTBBtn);
	toolbar.SetImageList(imgs);
	toolbar.SetHotImageList(imgsHot);

	return toolbar.m_hWnd;
}



BOOL MtlCmdBarLoadImages(
		CCommandBarCtrl&	cmdbar,
		int *				__firstID,
		int *				__lastID,
		UINT				nImageBmpID,
		int 				cx,
		int 				cy,
		COLORREF			clrMask,
		UINT				nFlags)
{
	// Add bitmap to our image list (create it if it doesn't exist)
	int 	nCount = int (__lastID - __firstID);
	if (nCount == 0)
		return FALSE;
	if (cmdbar.m_hImageList == NULL) {
		cmdbar.m_hImageList = ::ImageList_Create(cx, cy, nFlags | ILC_MASK, nCount, 1);
		ATLASSERT(cmdbar.m_hImageList != NULL);
		if (cmdbar.m_hImageList == NULL)
			return FALSE;
	}

	CBitmap 	bmp;
	bmp.LoadBitmap(nImageBmpID);
	ATLASSERT(bmp.m_hBitmap != NULL);

	if (bmp.m_hBitmap == NULL)
		return FALSE;

	if (::ImageList_AddMasked(cmdbar.m_hImageList, bmp, cmdbar.m_clrMask = clrMask) == -1)
		return FALSE;

	// Fill the array with command IDs
	for (; __firstID < __lastID; ++__firstID) {
		if (*__firstID != 0)
			cmdbar.m_arrCommand.Add(*__firstID);
	}

	ATLASSERT( ::ImageList_GetImageCount(cmdbar.m_hImageList) == cmdbar.m_arrCommand.GetSize() );
	if ( ::ImageList_GetImageCount(cmdbar.m_hImageList) != cmdbar.m_arrCommand.GetSize() )
		return FALSE;

	// Set some internal stuff
	cmdbar.m_szBitmap.cx = cx;
	cmdbar.m_szBitmap.cy = cy;
	cmdbar.m_szButton.cx = cmdbar.m_szBitmap.cx + 2 * CCommandBarCtrl::s_kcxButtonMargin;
	cmdbar.m_szButton.cy = cmdbar.m_szBitmap.cy + 2 * CCommandBarCtrl::s_kcyButtonMargin;

	return TRUE;
}
