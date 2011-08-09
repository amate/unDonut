// DownloadedListView.cpp

#include "stdafx.h"
#include "DownloadedListView.h"
#include "DownloadingListView.h"
#include "IniFile.h"
#include "../resource.h"
#include "../MainFrame.h"
#include "../DonutPFunc.h"


void	CDownloadedListView::AddDownloadedItem(DLItem* pItem)
{
	m_vecpDLItem.insert(m_vecpDLItem.begin(), std::unique_ptr<DLItem>(pItem));

	InsertItem(LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM, 0, _T(""), 0, 0, pItem->bAbort ? 1 : 0,  (LPARAM)pItem);

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
	strDate.Format(_T("%d/%d %02d:%02d:%02d"), date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
	Item.iSubItem	= 4;
	Item.pszText	= (LPWSTR)(LPCTSTR)strDate;
	SetItem(&Item);

	CString strFileSize;
	int nMByte = pItem->nProgressMax / (1000 * 1000);
	if (nMByte > 0) {
		strFileSize.Format(_T("%d MB"), nMByte);
	} else {
		int nKByte = pItem->nProgressMax / 1000;
		if (nKByte > 0) {
			strFileSize.Format(_T("%d KB"), nKByte);
		} else {
			strFileSize.Format(_T("%d Byte"), pItem->nProgressMax);
		}
	}
	Item.iSubItem	= 5;
	Item.pszText	= (LPWSTR)(LPCTSTR)strFileSize;
	SetItem(&Item);
}


// Overrides
int CDownloadedListView::CompareItemsCustom(LVCompareParam* pItem1, LVCompareParam* pItem2, int iSortCol)
{
	if (iSortCol == 0) {
		DLItem*	p1 = (DLItem*)pItem1->dwItemData;
		DLItem* p2 = (DLItem*)pItem2->dwItemData;
		if (p1->bAbort == p2->bAbort)
			return 0;
		else if (p1->bAbort)
			return 1;
		else
			return -1;

	} else if (iSortCol == 5) {
		int n1 = ((DLItem*)pItem1->dwItemData)->nProgressMax;
		int n2 = ((DLItem*)pItem2->dwItemData)->nProgressMax;
		int nRet = 0;
		if (n1 > n2)
			nRet = 1;
		else if (n1 < n2)
			nRet = -1;
		return nRet;
	}
	return 0;
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

	struct Column {
		LPCTSTR	columnName;
		int		columnType;
		int		columnFormat;
		int		defaultcolumnWidth;
	};
	Column	clm[] = {
		{ _T("！")			, LVCOLSORT_CUSTOM	, LVCFMT_LEFT	,  20 },
		{ _T("ファイル名")	, LVCOLSORT_TEXT	, LVCFMT_LEFT	, 160 },
		{ _T("URL")			, LVCOLSORT_TEXT	, LVCFMT_LEFT	, 300 },
		{ _T("ファイルパス"), LVCOLSORT_TEXT	, LVCFMT_LEFT	, 300 },
		{ _T("日付")		, LVCOLSORT_TEXT	, LVCFMT_LEFT	, 120 },
		{ _T("サイズ")		, LVCOLSORT_CUSTOM	, LVCFMT_RIGHT	, 100 },
	};

	int nCount = _countof(clm);
	for (int i = 0; i < nCount; ++i) {
		InsertColumn(i, clm[i].columnName, clm[i].columnFormat, clm[i].defaultcolumnWidth);
		SetColumnSortType(i, clm[i].columnType);
	}

	// 設定を読み込む
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileI pr(strIniFile, _T("DownloadedListView"));
	for (int i = 0; i < nCount; ++i) {
		CString strWidth = _T("ColumnWidth");
		strWidth.Append(i);
		
		SetColumnWidth(i, pr.GetValue(strWidth, clm[i].defaultcolumnWidth));
	}

	unique_ptr<int[]> pColumnOrder(new int[nCount]);
	for (int i = 0; i < nCount; i++) {
		CString strColumnOrder = _T("ColumnOrder");
		strColumnOrder.Append(i);
		pColumnOrder[i] = pr.GetValue(strColumnOrder, i);
	}
	SetColumnOrderArray(nCount, pColumnOrder.get());

	return 0;
}

void CDownloadedListView::OnDestroy()
{
	// 設定を保存する
	CString strIniFile = Misc::GetFullPath_ForExe(_T("Download.ini"));
	CIniFileO pr(strIniFile, _T("DownloadedListView"));
	
	int nColumnCount = GetColumnCount();
	unique_ptr<int[]> pColumnOrder(new int[nColumnCount]);
	GetColumnOrderArray(nColumnCount, pColumnOrder.get());
	for (int i = 0; i < nColumnCount; ++i) {
		CString strColumnOrder = _T("ColumnOrder");
		strColumnOrder.Append(i);
		pr.SetValue(pColumnOrder[i], strColumnOrder);
	}

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
		const CString& path = ((DLItem*)GetItemData(nIndex))->strFilePath;
		::ShellExecute(NULL, NULL, path, NULL, NULL, SW_SHOWNORMAL);
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

		DLItem*	pItem = ((DLItem*)GetItemData(lp->iItem));
		const CString& path = pItem->strFilePath;
		const CString& strReferer = pItem->strReferer;

		CPoint pt;
		::GetCursorPos(&pt);
		CMenu menu;
		menu.CreatePopupMenu();
		enum { ID_OPEN_ITEMSAVEFOLDER = 1, ID_OPEN_REFERER_ = 2, ID_LAST };

		menu.InsertMenu( 0, MF_BYPOSITION | MF_ENABLED, ID_OPEN_ITEMSAVEFOLDER, _T("保存先フォルダを開く") );
		menu.InsertMenu( 1, MF_BYPOSITION | MF_ENABLED, ID_OPEN_REFERER_	  , _T("ダウンロードしたページを表示する") );

		auto funcExeCommand = [=](int commandID) -> bool {
			if (commandID == ID_OPEN_ITEMSAVEFOLDER) {
				OpenFolderAndSelectItem(path);

			} else if(commandID == ID_OPEN_REFERER_) {
				if (strReferer.IsEmpty())
					return true;
				DonutOpenFile(g_pMainWnd->m_hWnd, strReferer, D_OPENFILE_ACTIVATE);

			} else {
				return false;
			}
			return true;
		};
		if (pItem->bAbort || ::PathFileExists(path) == FALSE) {
			int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
			funcExeCommand(nCmd);
			return 0;
		}

		LPITEMIDLIST pidl = ILCreateFromPath(path);
		if (pidl) {
			CComPtr<IShellItem>	spShellItem;
			::SHCreateShellItem(NULL, NULL, pidl, &spShellItem);
			if (spShellItem) {
				CComPtr<IContextMenu>	spContextMenu;
				spShellItem->BindToHandler(NULL, BHID_SFUIObject, IID_IContextMenu, (void**)&spContextMenu);
				if (spContextMenu) {
					menu.AppendMenu(MF_BYPOSITION | MF_SEPARATOR, UINT_PTR(0), _T("\0"));
					HRESULT hr = spContextMenu->QueryContextMenu(menu, menu.GetMenuItemCount(), ID_LAST, 0xFFFF, CMF_NORMAL);
					if (SUCCEEDED(hr)) {
						int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
						if (nCmd) {
							if (funcExeCommand(nCmd) == false) {
								CMINVOKECOMMANDINFO   InvokeInfo= { sizeof (CMINVOKECOMMANDINFO) };
								InvokeInfo.hwnd 	= m_hWnd;
								InvokeInfo.lpVerb	= (LPCSTR) MAKEINTRESOURCE(nCmd - ID_LAST);
								InvokeInfo.nShow	= SW_SHOWNORMAL;
								hr	= spContextMenu->InvokeCommand(&InvokeInfo);
							}
						}
					}
				}
			}
		}
	}
	return 0;
}



























