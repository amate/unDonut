// DonutStatusBarCtrl.cpp

#include "stdafx.h"
#include "DonutStatusBarCtrl.h"

//////////////////////////////////////////////////////////
// CDonutStatusBarCtrl


// Constructor/Destructor
CDonutStatusBarCtrl::CDonutStatusBarCtrl()
	: m_bUseBackColor(FALSE)
	, m_bOwnerdraw(FALSE)
	, m_nStatusStyle(0)
	, m_colText(0)
	, m_colBack(-1)
{
}

CDonutStatusBarCtrl::~CDonutStatusBarCtrl()
{
	std::map<int, HICON>::iterator	itIcon;

	for (itIcon = m_mapIcon.begin(); itIcon != m_mapIcon.end(); ++itIcon) {
		if (itIcon->second)
			DestroyIcon( itIcon->second );
	}
}

void	CDonutStatusBarCtrl::SetProxyComboBoxFont(HFONT hFont, bool bRedraw)
{
	m_cmbProxy.SetFont(hFont, bRedraw);
}


void CDonutStatusBarCtrl::SetIcon(int nIdPane, int nIndexIcon)
{
	HICON hIcon = NULL;

	if (nIndexIcon != USHRT_MAX) {
		if (m_mapIcon[nIndexIcon] == NULL) {
			m_mapIcon[nIndexIcon] = m_imgList.ExtractIcon(nIndexIcon);
		}

		hIcon = m_mapIcon[nIndexIcon];
	}

	SetPaneIcon(nIdPane, hIcon);
}


void CDonutStatusBarCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	int nIndex = lpdis->itemID;

	CDC dc;

	dc.CreateCompatibleDC(lpdis->hDC);

	if (m_bmpBg.m_hBitmap) {
		HBITMAP hbmpOld = dc.SelectBitmap(m_bmpBg);

		SetBkMode(lpdis->hDC, TRANSPARENT);
		CSize	size;
		m_bmpBg.GetSize(size);
		DrawBackGround(&lpdis->rcItem, dc.m_hDC, lpdis->hDC);
		//BitBlt(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,size.cx,size.cy, dc.m_hDC,0,0,SRCCOPY);
		dc.SelectBitmap(hbmpOld);
	}

	if (nIndex == 0) {
		SIZE sizeText;
		int  Height = lpdis->rcItem.bottom - lpdis->rcItem.top + 1;

		::SetTextAlign(lpdis->hDC, TA_TOP | TA_LEFT);
		::SetTextColor(lpdis->hDC, m_colText);
		GetTextExtentPoint32(lpdis->hDC, _T("h"), 1, &sizeText);
		::TextOut( lpdis->hDC, lpdis->rcItem.left + 2, lpdis->rcItem.top + (Height - sizeText.cy) / 2,
					m_strText, m_strText.GetLength() );
	}

	SetMsgHandled(TRUE);
}


void CDonutStatusBarCtrl::SetOwnerDraw(BOOL bOwnerdraw)
{
	if (bOwnerdraw) {
		CString 	strBuf;
		int nCount = GetPaneCount();
		for (int i = 0; i < nCount; i++) {
			GetText(i, strBuf);
			SetText(i, strBuf, SBT_OWNERDRAW);
		}
	}

	m_bOwnerdraw = bOwnerdraw;
}



void CDonutStatusBarCtrl::ReloadSkin(int nStyle, int colText /*=0*/, int colBack /*=-1*/)
{
	CString 	strPath = _GetSkinDir() + _T("secure.bmp");
	_ReplaceImageList(strPath, m_imgList, IDB_SECURE);
	OnImageListUpdated();

	strPath 	   = _GetSkinDir() + _T("status.bmp");
	m_bmpBg.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );

	m_nStatusStyle = nStyle;

	if (m_nStatusStyle == SKN_STATUS_STYLE_THEME || m_bmpBg.m_hBitmap == NULL) {
		SetOwnerDraw(FALSE);
	} else {
		SetOwnerDraw(TRUE);
	}

	//+++ 呼び元から渡すようにした.
	m_colText	= COLORREF( colText <= 0 ? 0 : colText );
	if (colBack >= 0) {
		m_colBack		= colBack; // | 0xFF000000;
		m_bUseBackColor = TRUE;
		SetBkColor(m_colBack);
	} else {
		m_colBack		= COLORREF( 0xFFFFFFFF );
		m_bUseBackColor = FALSE;
		SetBkColor(CLR_DEFAULT);
	}
}


void CDonutStatusBarCtrl::SetText(int nPane, LPCTSTR lpszText, int nType)
{
	m_strText = lpszText;
	__super::SetText(nPane, lpszText, nType);
}

void CDonutStatusBarCtrl::DrawBackGround(LPRECT lpRect, HDC hDCSrc, HDC hDCDest)
{
	SIZE size;
	m_bmpBg.GetSize(size);
	int  BmpWidth  = size.cx;
	int  BmpHeight = size.cy;

	if (m_nStatusStyle == SKN_STATUS_STYLE_TILE) {
		//タイル表示　面倒なのでクリッピングはしない
		for (int y = 0; y < lpRect->bottom; y += BmpHeight) {
			for (int x = 0; x < lpRect->right; x += BmpWidth) {
				::BitBlt(hDCDest, x, y, BmpWidth, BmpHeight, hDCSrc, 0, 0, SRCCOPY);
			}
		}

	} else if (m_nStatusStyle == SKN_STATUS_STYLE_STRETCH) {
		//拡大縮小表示
		RECT rcMin, rcMax;
		GetRect(0, &rcMin);
		GetRect(GetPaneCount() - 1, &rcMax);

		int  PaneWidth	= lpRect->right  - lpRect->left + 1;
		int  PaneHeight = lpRect->bottom - lpRect->top	+ 1;
		int  BarWidth	= rcMax.right	 - rcMin.left	+ 1;

		::StretchBlt(hDCDest, lpRect->left, lpRect->top,
						PaneWidth,
						PaneHeight,
						hDCSrc,
						BmpWidth * lpRect->left / BarWidth ,
						0,
						BmpWidth * PaneWidth / BarWidth ,
						BmpHeight > PaneHeight ? PaneHeight : BmpHeight,
						SRCCOPY);
	}
}


LRESULT CDonutStatusBarCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	DefWindowProc(uMsg, wParam, lParam);
	
	m_wndProgress.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | PBS_SMOOTH, 0, IDC_PROGRESS);
	m_wndProgress.ModifyStyleEx(WS_EX_STATICEDGE, 0);

	// Proxyコンボボックス
	// IEのProxyを使う場合、コンボボックスとステータスバーのProxyペインを削除
	m_cmbProxy.Create(m_hWnd, CRect(), NULL,
		WS_CHILD | WS_CLIPSIBLINGS | PBS_SMOOTH | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, IDC_COMBBOX);

	m_cmbProxy.SetFont( GetFont() );
	m_cmbProxy.SetDrawStyle(CSkinOption::s_nComboStyle);
	m_cmbProxy.IsUseIE() ? m_cmbProxy.ShowWindow(SW_HIDE)
						 : m_cmbProxy.ShowWindow(SW_SHOWNORMAL);
	m_cmbProxy.ResetProxyList();


	CString 	strPath = _GetSkinDir() + _T("secure.bmp");
	CBitmap 	bmp;
	bmp.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL)			//+++
		bmp.LoadBitmap(IDB_SECURE); 	//+++

	if (bmp.m_hBitmap) {
		CSize szIcon;
		bmp.GetSize(szIcon);
		m_imgList.Create(szIcon.cy, szIcon.cy, ILC_COLOR24 | ILC_MASK, 1, 1);
		m_imgList.Add( bmp, RGB(255, 0, 255) );
	} else {
		m_imgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 1);
	}

	strPath   = _GetSkinDir() + _T("status.bmp");
	m_bmpBg.Attach( AtlLoadBitmapImage(LPCTSTR(strPath), LR_LOADFROMFILE) );

	m_strText = _T("レディ");

	return 0;
}


LRESULT CDonutStatusBarCtrl::OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	int nIdPane    = LOWORD(wParam);
	int nIndexIcon = HIWORD(wParam);

	SetIcon(nIdPane, nIndexIcon);
	return S_OK;
}


LRESULT CDonutStatusBarCtrl::OnSetTipText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SetPaneTipText( (int) wParam, (LPCTSTR) lParam );
	return S_OK;
}


LRESULT CDonutStatusBarCtrl::OnSimple(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	BOOL bSimple = (BOOL) wParam;
	if (bSimple) {
		m_strText = _T("");
	} else {
		m_strText = _T("レディ");
	}

	if ( !IsValidBmp() ) {
		return DefWindowProc(uMsg, wParam, lParam);
	} else {
		bHandled = FALSE;
		InvalidateRect(NULL);
		return 0;
	}
}


LRESULT CDonutStatusBarCtrl::OnSetStatusText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	int nStyle = (int) wParam;

	if ( (nStyle & 255) && IsValidBmp() ) {
		nStyle	 |= SBT_OWNERDRAW;
		m_strText = (LPCTSTR) lParam;
	}

	return DefWindowProc(uMsg, (WPARAM) nStyle, lParam);
}


LRESULT CDonutStatusBarCtrl::OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = TRUE;
	if (!m_bOwnerdraw) {
		bHandled = FALSE;
		return 0;
	}

	CString 		 strNewText = (LPCTSTR) lParam;
	if (m_strText != strNewText) {
		SetText( 0,  strNewText, SBT_OWNERDRAW);
		m_strText =  strNewText;
	}

	return 0;
}


void CDonutStatusBarCtrl::OnImageListUpdated()
{
	std::map<int, HICON>::iterator	itIcon;

	for (itIcon = m_mapIcon.begin(); itIcon != m_mapIcon.end(); ++itIcon) {
		if (itIcon->second)
			DestroyIcon( itIcon->second );

		itIcon->second = m_imgList.ExtractIcon(itIcon->first);
	}

	InvalidateRect(NULL, TRUE);
}


void CDonutStatusBarCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	for (int nIndex = 0; nIndex < m_nPanes; nIndex++) {
		CRect rcPane;
		GetRect(nIndex, &rcPane);

		if ( !rcPane.PtInRect(point) )
			continue;

		int   nCommand = 0;
		nCommand = m_mapCmdID.Lookup(m_pPane[nIndex]);

		if (nCommand == 0)
			continue;

		::PostMessage(GetTopLevelParent(), WM_COMMAND, nCommand, 0);
	}
}


void CDonutStatusBarCtrl::OnSize(UINT nType, CSize size)
{
	DefWindowProc();

	SetMsgHandled(FALSE);

	//+++ メモ：ステータスバーのプロクシ・コンボボックスのレサイズ
	{
		CRect	rcComboPart;
		if (g_bSwapProxy == false) {
			GetRect(4, rcComboPart);
		} else {
			GetRect(1, rcComboPart);
		}

		rcComboPart.top   -= 1;
		rcComboPart.bottom = rcComboPart.top + rcComboPart.Height() * 10;
		m_cmbProxy.MoveWindow(rcComboPart, TRUE);
	}
}



























