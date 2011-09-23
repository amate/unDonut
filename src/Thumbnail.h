/**
 *	@file	Thumbnail.h
 *	@brief	サムネール表示.
 *	@note
 *		+++ 本家unDonut R5で実装 R10βで破棄されたルーチンを復活.
 *		+++ タブ数が多いとダイアログでるまでにかなり待ち時間発生する.... スレッドに分けるべきなんだろうなあ.
 */

#ifndef __THUMBNAIL_H_
#define __THUMBNAIL_H_

#define USE_THUMBNAIL
#include <boost/thread.hpp>
#include "resource.h"


class CThumbnailDlg : public CDialogImpl<CThumbnailDlg> /*, public CMessageFilter*/ 
{
public:
	enum { IDD = IDD_DIALOG_THUMBNAIL };

private:
	enum {
		PIC_WIDTH		= 200,
		PIC_HEIGHT 		= 200,
		SPACE 			= 2,
		m_nMinWidth 	= 20,
		m_nMinHeight 	= 50,
		m_nDefWidth 	= 550,
		m_nDefHeight 	= 520
	};

	CListViewCtrl	 m_List;
	CImageList		 m_ImageList;
	HWND			 m_hParent;
	CRect			 m_rcDialog;
	CRect			 m_rcDialogInit;
	int 			 m_nPictWidth;
	int 			 m_nPictHeight;
	BYTE/*BOOL*/	 m_bInit;
	BYTE/*BOOL*/	 m_bCenter;
	BYTE/*BOOL*/	 m_bSavePos;

	CContainedWindow m_wndList;

public:
	CThumbnailDlg()
		: m_wndList(this, 1)
		, m_bCenter(FALSE)
		, m_bInit(FALSE)
	  #if 1	//+++
		, m_hParent(0)
		, m_bSavePos(0)
		, m_nPictWidth(0)
		, m_nPictHeight(0)
	  #endif
	{
	}


	BEGIN_MSG_MAP(CThumbnailDlg)
		MESSAGE_HANDLER( WM_INITDIALOG	 	, OnInitDialog			)
		MESSAGE_HANDLER( WM_ACTIVATE	 	, OnActivate			)
		MESSAGE_HANDLER( WM_SIZE		 	, OnSize				)
		MESSAGE_HANDLER( WM_DESTROY	 	 	, OnDestroy				)
		NOTIFY_HANDLER( IDC_LIST_THUMBNAIL	, NM_DBLCLK,  OnDblClk	)
		NOTIFY_HANDLER( IDC_LIST_THUMBNAIL	, NM_RDBLCLK, OnRDblClk	)
		NOTIFY_HANDLER( IDC_LIST_THUMBNAIL	, NM_RCLICK,  OnRClick	)
		COMMAND_ID_HANDLER( IDOK			, OnCloseCmd			)
		COMMAND_ID_HANDLER( IDCANCEL		, OnCloseCmd			)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER( WM_KEYDOWN			, OnKeyDown				)
	END_MSG_MAP()



	int DoModal(HWND hWndParent, LPARAM dwInitParam = NULL)
	{
		m_hParent = (HWND) dwInitParam;
		return (int)CDialogImpl<CThumbnailDlg>::DoModal(hWndParent, dwInitParam);
	}


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		GetDefaultSize(m_rcDialog);
		//::MoveWindow(m_hWnd, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);

		m_List = GetDlgItem(IDC_LIST_THUMBNAIL);
		m_ImageList.Create(m_nPictWidth, m_nPictHeight, ILC_COLOR24, 8, 8);

		_Function_EnumChild_MakeThumbnail makeThum(m_ImageList, m_nPictWidth, m_nPictHeight);
		_Function_EnumChild_MakeListView makeList(m_List);
		//auto funcThread = [this, makeThum, makeList] () {
			MtlForEachMDIChild( m_hParent, makeThum);
			m_List.SetImageList(m_ImageList.m_hImageList, LVSIL_NORMAL);
			MtlForEachMDIChild( m_hParent, makeList);
		//};
		//boost::thread th(funcThread);
		//funcThread;

		MoveWindow(m_rcDialog, TRUE);

		m_wndList.SubclassWindow(m_List.m_hWnd);

		return TRUE;
	}


	void GetDefaultSize(CRect &rc)
	{
		CIniFileI 	pr( g_szIniFileName, _T("Thumbnail") );

		//画像サイズ取得
		DWORD dwWidth   = pr.GetValue( _T("PicWidth" ) );
		DWORD dwHeight  = pr.GetValue( _T("PicHeight") );

		m_nPictWidth    = (dwWidth > 0) ? dwWidth  : PIC_WIDTH;
		m_nPictHeight	= (dwHeight> 0) ? dwHeight : PIC_HEIGHT;

		//位置を取得
		DWORD	dwSave 		= pr.GetValue( _T("SavePosition") );
		if (dwSave) {
			DWORD	dwLeft 	= pr.GetValue( _T("Left"  ) );
			DWORD	dwTop 	= pr.GetValue( _T("Top"   ) );
			DWORD	dwRight	= pr.GetValue( _T("Right" ) );
			DWORD	dwBottom= pr.GetValue( _T("Bottom") );

			if (dwRight == 0 || dwBottom == 0) {
				dwRight   	= dwLeft + m_nDefWidth;
				dwBottom  	= dwTop  + m_nDefHeight;
				m_bCenter 	= TRUE;
			}
			rc.SetRect(dwLeft, dwTop, dwRight, dwBottom);
		} else {	//+++ ブラウザ側メインウィンドウのクライアント領域の位置＆サイズになるように修正.
			CRect 	rcInit;
			if (m_hParent)
				::GetWindowRect(m_hParent, &rcInit);
			if (m_hParent && rcInit.Width() > 0 && rcInit.Height() > 0) {
				rc 		  = rcInit;
				if (rc.Width() < m_nPictWidth+32)
					rc.right	= rc.left + m_nPictWidth+32;
				if (rc.Height() < m_nPictHeight+32)
					rc.bottom   = rc.top  + m_nPictHeight+32;
				m_bCenter = false;
			} else {
				m_bCenter = TRUE;
				rc.SetRect(0, 0, m_nDefWidth, m_nDefHeight);
			}
		}

		pr.Close();

		m_bSavePos 			= dwSave != 0;	//? TRUE : FALSE;
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		if (m_bSavePos) {
			CRect		rc;
			GetWindowRect(&rc);
			CIniFileO	pr( g_szIniFileName, _T("Thumbnail") );
			pr.SetValue( rc.left  , _T("Left"  ) );
			pr.SetValue( rc.top   , _T("Top"   ) );
			pr.SetValue( rc.right , _T("Right" ) );
			pr.SetValue( rc.bottom, _T("Bottom") );
			pr.Close();
		}
		return 0;
	}

	BYTE	PreTranslateMessage(MSG *pMsg)
	{
		return 0;
	}


	LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		return 0;
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		if (!m_bInit) {
			MoveWindow(m_rcDialog, TRUE);

			if (m_bCenter)
				CenterWindow( GetParent() );

			m_bInit = TRUE;
		}

		CRect 	rc;
		GetClientRect(&rc);
		rc.DeflateRect(SPACE, SPACE);
		m_List.MoveWindow(&rc, TRUE);

		return 0;
	}


	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		int 	nVirtKey = (int) wParam;
	  #if 1	//+++
		int		index	 = 0;
		switch (nVirtKey) {
		case VK_LEFT:
		case VK_UP:
			{
				int n = m_List.GetItemCount();
				if (n <= 0)
					break;
				index = m_List.GetSelectedIndex();
				index = (n + index - 1) % n;
				//if (index > 0)
				m_List.SelectItem(index);
			  #if 0 //def WIN64
				unsigned	 w	   = m_List.GetColumnWidth();
				unsigned	 n	   = m_List.GetSelectedColumn();
				if (w)
					n = (w + n - 1) % w;
				m_List.SetSelectedColumn(n);
			  #endif
			}
			break;
		case VK_RIGHT:
		case VK_DOWN:
			{
				int n = m_List.GetItemCount();
				if (n <= 0)
					break;
				index = m_List.GetSelectedIndex();
				index = (index + 1) % n;
				m_List.SelectItem(index);
			  #if 0 //def WIN64
				unsigned	 w	   = m_List.GetColumnWidth();
				unsigned	 n	   = m_List.GetSelectedColumn();
				if (w)
					n = (n + 1) % w;
				m_List.SetSelectedColumn(n);
			  #endif
			}
			break;

		//case VK_MBUTTON:
		//	index = m_List.GetSelectedIndex();
		//	break;
		case VK_DELETE:
			{
				index = m_List.GetSelectedIndex();
				DoItemClose(m_List.GetSelectedIndex());
			}
			break;

		default:
			break;
		}
	  #else
		if (nVirtKey == 'W' || nVirtKey == 'w') {
			if (::GetKeyState(VK_CONTROL) < 0)
				EndDialog(0);
		}
	  #endif

		return 0;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		//DestroyWindow();
		EndDialog(wID);
		return 0;
	}


	//+++
	LRESULT OnRClick(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMLISTVIEW plv   = (LPNMLISTVIEW) pnmh;
		int 		 index = plv->iItem;

		::SetForegroundWindow(m_hWnd);
		CMenu/*Handle*/ 	menu0;
		menu0.LoadMenu(IDR_THUMBNAIL_MENU);
		if (menu0.m_hMenu == NULL)
			return 0;
		CMenuHandle menu = menu0.GetSubMenu(0);
		if (menu.m_hMenu == NULL)
			return 0;

		// ポップアップメニューを開く.
		POINT 	pt;
		::GetCursorPos(&pt);
		HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);

		switch (hr) {
		case ID_FILE_OPEN:				DoItemOpen(index);			break;
		case ID_FILE_CLOSE:				DoItemClose(index);			break;
		case ID_WINDOW_CLOSE_EXCEPT:	DoItemCloseExcept(index);	break;
		case ID_WINDOW_CLOSE_ALL:		DoItemCloseAll();			break;
		case ID_LEFT_CLOSE:				DoItemCloseLeft(index);		break;
		case ID_RIGHT_CLOSE:			DoItemCloseRight(index);	break;
		default:						;
		}
		return 0;
	}


	LRESULT OnDblClk(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMLISTVIEW plv   = (LPNMLISTVIEW) pnmh;
		return DoItemOpen(plv->iItem);
	}


	LRESULT OnRDblClk(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
//		LPNMLISTVIEW plv   = (LPNMLISTVIEW) pnmh;
//		return DoItemClose(plv->iItem);
		return 0;
	}


	LRESULT	DoItemOpen(int index)
	{
		HWND		 hWnd  = (HWND) m_List.GetItemData(index);
		::SendMessage(m_hParent, WM_MDIACTIVATE, (WPARAM) hWnd, 0);
		EndDialog(index);
		return 0;
	}


	LRESULT DoItemClose(int index)
	{
		HWND		 hWnd  = (HWND) m_List.GetItemData(index);
		m_List.DeleteItem(index);
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
		int n = m_List.GetItemCount();
		if (n > 0) {
			if (index < 0)
				index = 0;
			if (index >= n)
				index =  n-1;
			m_List.SelectItem(index);
			//hWnd  = (HWND) m_List.GetItemData(index);
			//::SendMessage(m_hParent, WM_MDIACTIVATE, (WPARAM) hWnd, 0);
		} else {
			EndDialog(index);
		}
		return 0;
	}


	LRESULT DoItemCloseExcept(int index)
	{
		DoItemCloseRight(index);
		DoItemCloseLeft(index);
		return 0;
	}


	LRESULT DoItemCloseAll()
	{
		DoItemCloseRight(0);
		DoItemClose(0);
		return 0;
	}


	LRESULT DoItemCloseLeft(int index)
	{
		int n = m_List.GetItemCount();
		if (n <= 1)
			return 0;
		while (index > 0) {
			HWND	hWnd  = (HWND) m_List.GetItemData(0);
			m_List.DeleteItem(0);
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			--index;
		}
		return 0;
	}


	LRESULT DoItemCloseRight(int index)
	{
		int n = m_List.GetItemCount();
		if (n <= 1)
			return 0;
		for (int i = n; --i > index;) {
			HWND	hWnd  = (HWND) m_List.GetItemData(i);
			m_List.DeleteItem(i);
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		return 0;
	}


	struct _Function_EnumChild_MakeListView {
		CListViewCtrl &		m_ListView;

	public:
		_Function_EnumChild_MakeListView(CListViewCtrl &List)
			:	m_ListView(List)
		{
		}


		void operator ()(HWND hWnd)
		{
			CString strTitle = MtlGetWindowText(hWnd);
			int 	index	 = m_ListView.GetItemCount();

			m_ListView.InsertItem(LVIF_IMAGE | LVIF_TEXT, index, strTitle, 0, 0, index, 0);
			m_ListView.SetItemData(index, (LPARAM) hWnd);
		}
	};



	struct _Function_EnumChild_MakeThumbnail {
		CImageList&		m_ImgList;
		int 			m_nWidth;
		int 			m_nHeight;

	public:
		_Function_EnumChild_MakeThumbnail(CImageList &ImgList, int PictWidth, int PictHeight)
			: m_ImgList(ImgList)
			, m_nWidth(PictWidth)
			, m_nHeight(PictHeight)
		{
		}

		void 	operator ()(HWND hWnd)
		{
			CComPtr<IWebBrowser2>		pWB2;
			CComPtr<IHTMLDocument2> 	pDoc;
			pWB2 = DonutGetIWebBrowser2(hWnd);
			pWB2->get_Document( (IDispatch **) &pDoc );

			if (pDoc) {
			  #if 1	//+++ ハンドル関係は、WTLなクラスに後片付けも任せる.
				CClientDC 	tmpClientDc( ::GetDesktopWindow() );
				CDC 		hdc 	= ::CreateCompatibleDC(tmpClientDc);
				CBitmap 	bmp		= ::CreateCompatibleBitmap(tmpClientDc, m_nWidth, m_nHeight);

				HBITMAP 	hOldBmp = (HBITMAP) ::SelectObject(hdc, bmp);

				CRect		rc;
				::GetClientRect(hWnd, &rc);

				int 		Width;
				int			Height;
				if ( rc.Width() < rc.Height() ) {
					Width  = m_nWidth;
					Height = (int) ( m_nHeight * ( (double) rc.Height() / rc.Width()  ) );
				} else {
					Width  = (int) ( m_nWidth  * ( (double) rc.Width()  / rc.Height() ) );
					Height = m_nHeight;
				}

				::OleDraw( pDoc, 1, hdc, CRect(0, 0, Width - 1, Height - 1) );
				::SelectObject(hdc, hOldBmp);

				m_ImgList.Add(bmp);
			  #else
				HDC 	hTmp	= ::GetDC( ::GetDesktopWindow() );
				HDC 	hdc 	= ::CreateCompatibleDC(hTmp);
				HBITMAP hBmp	= ::CreateCompatibleBitmap(hTmp, m_nWidth, m_nHeight);
				::ReleaseDC(::GetDesktopWindow(), hTmp);
				HBITMAP hOldBmp = (HBITMAP) ::SelectObject(hdc, hBmp);

				CRect	rc;
				int 	Width;
				int		Height;
				::GetClientRect(hWnd, &rc);

				if ( rc.Width() < rc.Height() ) {
					Width  = m_nWidth;
					Height = (int) ( m_nHeight * ( (double) rc.Height() / rc.Width()  ) );
				} else {
					Width  = (int) ( m_nWidth  * ( (double) rc.Width()  / rc.Height() ) );
					Height = m_nHeight;
				}

				::OleDraw( pDoc, 1, hdc, CRect(0, 0, Width - 1, Height - 1) );
				::SelectObject(hdc, hOldBmp);

				m_ImgList.Add(hBmp);

				::DeleteObject(hBmp);
				::DeleteDC(hdc);
			  #endif
			}
		}


		//+++ 未使用.
		bool SaveBitmap(HDC hDC, HBITMAP hBmp, int index)
		{
			//+++ 200x200固定だったのをメンバー変数をみるように修正.
			BYTE			   	dat[54];
			unsigned			size = m_nWidth * m_nHeight * 3;
			std::vector<BYTE>	pixs( size );

			CString 		   str;
			str.Format(_T("test%02d.bmp"), index);

			FILE* fp	= _tfopen(str, _T("wb"));
			if (fp == NULL)
				return NULL;
			FILE* fpdat	= _tfopen(_T("test.dat"), _T("rb"));
			if (fpdat == NULL) {
				fclose(fp);
				return NULL;
			}

			LPBITMAPFILEHEADER 	pbf = (BITMAPFILEHEADER *) dat;
			LPBITMAPINFOHEADER 	pbi	= (BITMAPINFOHEADER *) ( dat + sizeof (BITMAPFILEHEADER) );

			fread(dat, 1, 54, fpdat);
			::GetDIBits(hDC, hBmp, 0, m_nHeight, &pixs[0], (BITMAPINFO *) pbi, DIB_RGB_COLORS);

			fwrite(dat     , 1,  54 , fp);
			fwrite(&pixs[0], 1, size, fp);

			fclose(fpdat);
			fclose(fp);
			return true;
		}
	};

};



#endif //__THUMBNAIL_H_
