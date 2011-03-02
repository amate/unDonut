// DownloadedListView.cpp

#include "stdafx.h"
#include "DownloadedListView.h"
#include "DownloadingListView.h"
#include "IniFile.h"


void	CDownloadedListView::AddDownloadedItem(DLItem* pItem)
{
	m_vecpDLItem.insert(m_vecpDLItem.begin(), std::unique_ptr<DLItem>(pItem));

	InsertItem(LVIF_TEXT | LVIF_IMAGE, 0, _T(""), 0, 0, pItem->bAbort ? 1 : 0,  NULL);

	LVITEM	Item = { 0 };
	Item.mask		= LVIF_TEXT;
	Item.iSubItem	= 1;
	Item.pszText	= (LPWSTR)(LPCTSTR)pItem->strFileName;
	SetItem(&Item);

	Item.iSubItem	= 2;
	Item.pszText	= (LPWSTR)(LPCTSTR)pItem->strURL;
	SetItem(&Item);

	Item.iSubItem	= 3;
	Item.pszText	= (LPWSTR)(LPCTSTR)pItem->strFilePath;
	SetItem(&Item);

	time_t	timer = time(NULL);
	struct tm* date = localtime(&timer);
	CString strDate;
	strDate.Format(_T("%d月%d日 %02d:%02d:%02d"), date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
	Item.iSubItem	= 4;
	Item.pszText	= (LPWSTR)(LPCTSTR)strDate;
	SetItem(&Item);
}


// Message map

int CDownloadedListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	LRESULT lRes = DefWindowProc();

	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP);

	CImageList ImageList;
	ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1);
	CBitmap bmpOK = AtlLoadBitmap(IDB_DLOK);
	CBitmap	bmpError = AtlLoadBitmap(IDB_DLERROR);
	ImageList.Add(bmpOK, RGB(255, 0, 255));
	ImageList.Add(bmpError, RGB(255, 0, 255));
	SetImageList(ImageList, LVSIL_SMALL);

	LPCTSTR strColumns[] = {
		_T("！"),
		_T("ファイル名"),
		_T("URL"),
		_T("ファイルパス"),
		_T("日付")
	};

	int nColumnWidths[] = {
		20, 160, 300, 300, 120
	};

	int nCount = sizeof (strColumns) / sizeof (strColumns[0]);
	for (int i = 0; i < nCount; ++i) {
		InsertColumn(i, strColumns[i], LVCFMT_LEFT, nColumnWidths[i]);
		SetColumnSortType(i, LVCOLSORT_TEXT);
	}

	// 設定を読み込む
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileI pr(strIniFile, _T("DownloadedListView"));
	for (int i = 0; i < nCount; ++i) {
		CString strWidth = _T("ColumnWidth");
		strWidth.Append(i);
		
		SetColumnWidth(i, pr.GetValue(strWidth, nColumnWidths[i]));
	}

	int *pColumnOrder = new int[nCount];
	for (int i = 0; i < nCount; i++) {
		CString strColumnOrder = _T("ColumnOrder");
		strColumnOrder.Append(i);
		pColumnOrder[i] = pr.GetValue(strColumnOrder, i);
	}
	SetColumnOrderArray(nCount, pColumnOrder);
	delete[] pColumnOrder;

	return lRes;
}

void CDownloadedListView::OnDestroy()
{
	int nCount = GetColumnCount();
	for (; nCount > 0; --nCount) {
		DeleteColumn(0);
	}

	// 設定を保存する
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileO pr(strIniFile, _T("DownloadedListView"));
	
	int nColumnCount = GetColumnCount();
	int *pColumnOrder = new int[nColumnCount];
	GetColumnOrderArray(nColumnCount, pColumnOrder);
	pr.SetValue(nColumnCount, _T("ColumnCount"));
	for (int i = 0; i < nColumnCount; ++i) {
		CString strColumnOrder = _T("ColumnOrder");
		strColumnOrder.Append(i);
		pr.SetValue(pColumnOrder[i], strColumnOrder);
	}
	delete[] pColumnOrder;

	for (int i = 0; i < nColumnCount; ++i) {
		int nWidth = GetColumnWidth(i);
		CString strWidth = _T("ColumnWidth");
		strWidth.Append(i);
		pr.SetValue(nWidth, strWidth);
	}

}


void CDownloadedListView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nIndex = HitTest(point, NULL);
	if (nIndex != -1) {
		::ShellExecute(NULL, NULL, m_vecpDLItem[nIndex]->strFilePath, NULL, NULL, SW_SHOWNORMAL);
	}
}

// 右クリック
LRESULT CDownloadedListView::OnListRClick(LPNMHDR pnmh)
{
	LPNMITEMACTIVATE	lp = (LPNMITEMACTIVATE)pnmh;
	if (lp->iItem != -1) {
		int nCount = GetItemCount();
		for (int i = 0; i < nCount; ++i) {
			SetItemState(i, 0, LVIS_SELECTED);
		}
		SetItemState(lp->iItem, LVIS_SELECTED, LVIS_SELECTED);

		LPITEMIDLIST pidl = ILCreateFromPath(m_vecpDLItem[lp->iItem]->strFilePath);
		if (pidl) {
			CComPtr<IShellItem>	spShellItem;
			::SHCreateShellItem(NULL, NULL, pidl, &spShellItem);
			if (spShellItem) {
				CComPtr<IContextMenu>	spContextMenu;
				spShellItem->BindToHandler(NULL, BHID_SFUIObject, IID_IContextMenu, (void**)&spContextMenu);
				if (spContextMenu) {
					CMenu menu;
					menu.CreatePopupMenu();
					HRESULT hr = spContextMenu->QueryContextMenu(menu, 0, 1, 0xFFFF, CMF_NORMAL);
					if (SUCCEEDED(hr)) {
						CPoint pt;
						::GetCursorPos(&pt);
						int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
						if (nCmd) {
							CMINVOKECOMMANDINFO   InvokeInfo= { sizeof (CMINVOKECOMMANDINFO) };
							InvokeInfo.hwnd 	= m_hWnd;
							InvokeInfo.lpVerb	= (LPCSTR) MAKEINTRESOURCE(nCmd - 1);
							InvokeInfo.nShow	= SW_SHOWNORMAL;
							hr	= spContextMenu->InvokeCommand(&InvokeInfo);
						}
					}
				}
			}
		}
	}
	return 0;
}



























