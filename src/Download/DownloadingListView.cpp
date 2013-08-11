// DownloadingListView.cpp

#include "stdafx.h"
#include "DownloadingListView.h"
#include <WinInet.h>
#include "DownloadedListView.h"
#include "DownloadOptionDialog.h"
#include "../MtlWin.h"
#include "../MtlFile.h"
#include "../MtlCom.h"
#include "../HlinkDataObject.h"
#include "../DonutPFunc.h"
#include "../SharedMemoryUtil.h"
#include "DLListWindow.h"
#include "../MainFrame.h"
#include "../dialog/RenameFileDialog.h"



////////////////////////////////////////////////////////////////
// CDownloadingListView

// Constructor/Destructor
CDownloadingListView::CDownloadingListView() : 
	m_bTimer(false),
	m_dwLastTime(0)
{ }

CDownloadingListView::~CDownloadingListView()
{
	m_ImageList.Destroy();
	m_ImgStop.Destroy();
}


void	CDownloadingListView::DoPaint(CDCHandle dc)
{
	CPoint ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.MoveToY(ptOffset.y);
	//rcClient.bottom	+= ptOffset.y;

	CMemoryDC	memDC(dc, rcClient);
	HFONT hOldFont = memDC.SelectFont(m_Font);

	// 背景を描画
	memDC.FillSolidRect(rcClient, RGB(255, 255, 255));

	std::size_t	size = m_vecDLItemInfo.size();
	for (auto it = m_vecDLItemInfo.begin(); it != m_vecDLItemInfo.end(); ++it) {
		const DLItem& DLItem = *it->pDLItem;
		CRect rcItem = it->rcItem;
		rcItem.right = rcClient.right;
		if (memDC.RectVisible(&rcItem) == FALSE)
			continue;

		memDC.SetBkMode(TRANSPARENT);
		if (it->dwState & DLITEMSTATE_SELECTED) {
			static COLORREF SelectColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			memDC.SetTextColor(SelectColor);
		} else {
			memDC.SetTextColor(0);
		}

		// 選択列描画
		if (it->dwState & DLITEMSTATE_SELECTED){
			memDC.FillRect(&rcItem, COLOR_HIGHLIGHT);
		}

		// アイコン描画
		CRect rcImage = rcItem;
		CPoint ptImg(cxImageMargin, rcImage.top + cyImageMargin);
		m_ImageList.Draw(memDC, it->nImgIndex, ptImg, ILD_NORMAL);

		// ファイル名を描画
		CRect rcFileName = rcItem;
		rcFileName.left = cxFileNameMargin;
		rcFileName.top += cyFileNameMargin;
		memDC.DrawText(DLItem.strFileName, -1, rcFileName, DT_SINGLELINE);

		// progress
		CRect rcProgress(cxFileNameMargin, rcItem.top + cyProgressMargin, rcItem.right - cleftProgressMargin, rcItem.top + cyProgressMargin + ProgressHeight);
		if (IsThemeNull() == false) {
			static int PROGRESSBAR  = _CheckOsVersion_VistaLater() ? PP_TRANSPARENTBAR : PP_BAR;
			static int PROGRESSBODY = _CheckOsVersion_VistaLater() ? PP_FILL		   : PP_CHUNK;
			if (IsThemeBackgroundPartiallyTransparent(PROGRESSBAR, 0))
				DrawThemeParentBackground(memDC, rcProgress);
			DrawThemeBackground(memDC, PROGRESSBAR, 0, rcProgress);
			CRect rcContent;
			GetThemeBackgroundContentRect(memDC, PROGRESSBAR, 0, rcProgress, rcContent);

			double Propotion = double(DLItem.nProgress) / double(DLItem.nProgressMax);
			int nPos = (int)((double)rcContent.Width() * Propotion);
			rcContent.right = rcContent.left + nPos;
			DrawThemeBackground(memDC, PROGRESSBODY, 0, rcContent);
		} else {
			memDC.DrawEdge(rcProgress, EDGE_RAISED, BF_ADJUST | BF_MONO | BF_RECT | BF_MIDDLE);
			double Propotion = double(DLItem.nProgress) / double(DLItem.nProgressMax);
			int nPos = (int)((double)rcProgress.Width() * Propotion);
			rcProgress.right = rcProgress.left + nPos;
			memDC.FillSolidRect(rcProgress, RGB(0, 255, 0));
		}

		// 説明描画
		CRect rcDiscribe = rcItem;
		rcDiscribe.top += cyProgressMargin + ProgressHeight;
		rcDiscribe.left = cxFileNameMargin;
		memDC.DrawText(it->strText, -1, rcDiscribe, DT_SINGLELINE);

		// 停止アイコン描画
		CPoint ptStop(rcClient.right - cxStopLeftSpace, rcItem.top + cyStopTopMargin);
		m_ImgStop.Draw(memDC, 0, ptStop, ILD_NORMAL);

		// 下にラインを引く
		static COLORREF BorderColor = ::GetSysColor(COLOR_3DLIGHT);
		HPEN hPen = ::CreatePen(PS_SOLID, 1, BorderColor);
		HPEN hOldPen = memDC.SelectPen(hPen);
		memDC.MoveTo(CPoint(rcItem.left, rcItem.bottom));
		memDC.LineTo(rcItem.right, rcItem.bottom);
		memDC.SelectPen(hOldPen);
		::DeleteObject(hPen);
	}

	dc.SelectFont(hOldFont);
}


DROPEFFECT CDownloadingListView::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	if (_MtlIsHlinkDataObject(pDataObject))
		return DROPEFFECT_LINK | DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
}


DROPEFFECT CDownloadingListView::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (_MtlIsHlinkDataObject(pDataObject))
		return DROPEFFECT_LINK | DROPEFFECT_COPY;
	return DROPEFFECT_NONE;
}


DROPEFFECT CDownloadingListView::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	vector<CString>	vecUrl;
	if (GetDonutURLList(pDataObject, vecUrl) == false) {
		CString strText;
		if (   MtlGetHGlobalText(pDataObject, strText)
			|| MtlGetHGlobalText(pDataObject, strText, CF_SHELLURLW))
		{
			vecUrl.push_back(strText);
		}
	}

	if (vecUrl.empty() == false) {
		CDLListWindow* pwndDL = new CDLListWindow;
		pwndDL->Create(m_hWnd);
		pwndDL->SetDLList(vecUrl);
	}
	return DROPEFFECT_LINK | DROPEFFECT_COPY;
}



// Message map

int CDownloadingListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 水平スクロールバーを削除
	ModifyStyle(WS_HSCROLL, 0);

	// 画面更新用のタイマーを起動
	//SetTimer(1, 1000);

	static bool bInit = false;
	if (bInit)
		return 0;
	bInit = true;

	SetScrollSize(10, 10);
    SetScrollLine(10, 10);
    //SetScrollPage(100, 100);

	// イメージリスト作成
	m_ImageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 20, 1);

	// デフォルトのアイコンを読み込み
	SHFILEINFO sfinfo;
	::SHGetFileInfo(_T("*.*"), FILE_ATTRIBUTE_NORMAL, &sfinfo, sizeof(SHFILEINFO)
		, SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
	int nImgIndex = m_ImageList.AddIcon(sfinfo.hIcon);	/* 0 */
	::DestroyIcon(sfinfo.hIcon);

	// 停止アイコンを読み込む
	m_ImgStop.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	HICON hStopIcon = AtlLoadIcon(IDI_DLSTOP);
	m_ImgStop.AddIcon(hStopIcon);
	::DestroyIcon(hStopIcon);

	// フォントを設定
	WTL::CLogFont	logfont;
	logfont.SetMenuFont();
	m_Font = logfont.CreateFontIndirectW();

	// ツールチップを設定
	m_ToolTip.Create(m_hWnd);
	m_ToolTip.ModifyStyle(0, TTS_ALWAYSTIP);
	CToolInfo ti(TTF_SUBCLASS, m_hWnd);
	ti.hwnd = m_hWnd;
	m_ToolTip.AddTool(ti);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 30 * 1000);
	m_ToolTip.SetMaxTipWidth(600);

	OpenThemeData(L"PROGRESS");

	RegisterDragDrop();

    return 0;
}

void CDownloadingListView::OnDestroy()
{
	if (m_bTimer)
		KillTimer(1);

	RevokeDragDrop();
}


void CDownloadingListView::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	if (size != CSize(0, 0)) {
		std::for_each(m_vecDLItemInfo.begin(), m_vecDLItemInfo.end(), [size](DLItemInfomation& Item) {
			Item.rcItem.right = size.cx;
		});
		CSize	scrollsize;
		GetScrollSize(scrollsize);
		scrollsize.cx	= size.cx ? size.cx : 1;
		m_sizeClient.cx = scrollsize.cx;
		SetScrollSize(scrollsize, FALSE, false);
		SetScrollLine(10, 10);
	}
}

void CDownloadingListView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcClient;
	GetClientRect(rcClient);

	int nIndex = _HitTest(point);

	int nCount = (int)m_vecDLItemInfo.size();
	for (int i = 0; i < nCount; ++i) {
		DLItemInfomation& item = m_vecDLItemInfo[i];
		if (item.dwState & DLITEMSTATE_SELECTED) 
			item.dwState &= ~DLITEMSTATE_SELECTED;
		if (i == nIndex) {
			CRect rcItem = _GetItemClientRect(i);
			CRect rcStop(CPoint(rcClient.right - cxStopLeftSpace, rcItem.top + cyStopTopMargin), CSize(cxStop, cyStop));
			if (rcStop.PtInRect(point)) {
				m_vecDLItemInfo[i].pDLItem->bAbort = true;	// DLを停止する
				TRACEIN(_T("DLをキャンセルしました！: %s"), m_vecDLItemInfo[i].pDLItem->strFileName);
			} else {
				item.dwState |= DLITEMSTATE_SELECTED;
			}
		}
	}
	Invalidate(FALSE);
}

//------------------------------------
/// 右クリックメニューを表示する
void CDownloadingListView::OnRButtonUp(UINT nFlags, CPoint point)
{
	int nIndex = _HitTest(point);
	if (nIndex == -1)
		return;

	int nCount = (int)m_vecDLItemInfo.size();
	for (int i = 0; i < nCount; ++i) {
		DLItemInfomation& item = m_vecDLItemInfo[i];
		if (item.dwState & DLITEMSTATE_SELECTED) 
			item.dwState &= ~DLITEMSTATE_SELECTED;
		if (i == nIndex) 
			item.dwState |= DLITEMSTATE_SELECTED;
	}
	Invalidate(FALSE);

	CMenu	menu;
	menu.LoadMenu(IDM_DOWNLOADINGLISTVIEW);
	CMenu	submenu = menu.GetSubMenu(0);

	m_DLItemForPopup = boost::in_place(*m_vecDLItemInfo[nIndex].pDLItem);
	CPoint pt;
	::GetCursorPos(&pt);
	int nCmd = submenu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
	if (nCmd)
		SendMessage(WM_COMMAND, nCmd);
	m_DLItemForPopup = boost::none;
}


void CDownloadingListView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		DWORD dwNowTime = ::timeGetTime();
		DWORD dwTimeMargin = dwNowTime - m_dwLastTime;
		if (dwTimeMargin <= 0) {
			return;
		} else {
			m_dwLastTime = dwNowTime;
		}

		int	nMaxTotalSecondTime = 0;
		for (auto it = m_vecDLItemInfo.begin(); it != m_vecDLItemInfo.end(); ++it) {
			DLItemInfomation& itemInfo = *it;
			DLItem& DLItem = *itemInfo.pDLItem;
			if (DLItem.bTextRefreshStop)
				continue;

			// (1.2 MB/sec)
			CString strTransferRate;
			int nProgressMargin = DLItem.nProgress - itemInfo.nOldProgress;
			itemInfo.nOldProgress = DLItem.nProgress;

			if (itemInfo.deqProgressAndTime.size() >= 10)
				itemInfo.deqProgressAndTime.pop_front();
			itemInfo.deqProgressAndTime.push_back(make_pair(nProgressMargin, (int)dwTimeMargin));
			nProgressMargin = 0;
			int nTotalTime = 0;
			for (auto itq = itemInfo.deqProgressAndTime.cbegin(); itq != itemInfo.deqProgressAndTime.cend(); ++itq) {
				nProgressMargin += itq->first;
				nTotalTime		+= itq->second;
			}

			double dKbTransferRate = (double)nProgressMargin / (double)nTotalTime;	// kbyte / second
			double MbTransferRate = dKbTransferRate / 1000.0;
			if (MbTransferRate > 1) {
				::swprintf(strTransferRate.GetBuffer(30), _T(" (%.1lf MB/sec)"), MbTransferRate);
				strTransferRate.ReleaseBuffer();
			} else {
				::swprintf(strTransferRate.GetBuffer(30), _T(" (%.1lf KB/sec)"), dKbTransferRate);
				strTransferRate.ReleaseBuffer();
			}
			

			// 残り 4 分
			int nRestByte = DLItem.nProgressMax - DLItem.nProgress;
			if (nRestByte <= 0) {
				CString strDownloaded;
				double dMByte = (double)DLItem.nProgress / (1000.0 * 1000.0);
				if (dMByte >= 1) {
					::swprintf(strDownloaded.GetBuffer(30), _T("%.1lf MB ダウンロード"), dMByte);
					strDownloaded.ReleaseBuffer();
				} else {
					double dKByte = (double)DLItem.nProgress / 1000.0;
					::swprintf(strDownloaded.GetBuffer(30), _T("%.1lf KB ダウンロード"), dKByte);
					strDownloaded.ReleaseBuffer();
				}
				itemInfo.strText.Format(_T("残り ??? 時間 ― %s%s ― %s"), strDownloaded, strTransferRate, DLItem.strDomain);
			} else {
				if (dKbTransferRate > 0) {
					int nTotalSecondTime = static_cast<int>((nRestByte / 1000) / dKbTransferRate);	// 残り時間(秒)
					if (nMaxTotalSecondTime < nTotalSecondTime)
						nMaxTotalSecondTime = nTotalSecondTime;
					int nhourTime	= nTotalSecondTime / (60 * 60);									// 時間
					int nMinTime	= (nTotalSecondTime - (nhourTime * (60 * 60))) / 60;			// 分
					int nSecondTime = nTotalSecondTime - (nhourTime * (60 * 60)) - (nMinTime * 60);	// 秒
					itemInfo.strText = _T("残り ");
					if (nhourTime > 0) {
						itemInfo.strText.Append(nhourTime) += _T(" 時間 ");
					}
					if (nMinTime > 0) {
						itemInfo.strText.Append(nMinTime) += _T(" 分 ");
					}
					itemInfo.strText.Append(nSecondTime) += _T(" 秒 ― ");


					// 10.5 / 288 MB
					CString strDownloaded;
					double dMByte = (double)DLItem.nProgressMax / (1000.0 * 1000.0);
					if (dMByte >= 1) {
						double DownloadMByte = (double)DLItem.nProgress / (1000.0 * 1000.0);
						::swprintf(strDownloaded.GetBuffer(30), _T("%.1lf / %.1lf MB"), DownloadMByte, dMByte);
						strDownloaded.ReleaseBuffer();
					} else {
						double dKByte = (double)DLItem.nProgressMax / 1000.0;
						double DownloadKByte = (double)DLItem.nProgress / 1000.0;
						::swprintf(strDownloaded.GetBuffer(30), _T("%.1lf / %.1lf KB"), DownloadKByte, dKByte);
						strDownloaded.ReleaseBuffer();
					}
					itemInfo.strText += strDownloaded + strTransferRate + _T(" ― ") + DLItem.strDomain;
				}
			}
		}
		/* タイトルバーに情報を表示する */
		if (nMaxTotalSecondTime > 0) {
			CString strTitle;
			strTitle.Format(_T("DLアイテム数 %d - 全ファイルのDL終了まで残り "), (int)m_vecDLItemInfo.size());
			int nhourTime	= nMaxTotalSecondTime / (60 * 60);									// 時間
			int nMinTime	= (nMaxTotalSecondTime - (nhourTime * (60 * 60))) / 60;			// 分
			int nSecondTime = nMaxTotalSecondTime - (nhourTime * (60 * 60)) - (nMinTime * 60);	// 秒
			if (nhourTime > 0) {
				strTitle.Append(nhourTime) += _T(" 時間 ");
			}
			if (nMinTime > 0) {
				strTitle.Append(nMinTime) += _T(" 分 ");
			}
			strTitle.Append(nSecondTime) += _T(" 秒");
			GetTopLevelWindow().SetWindowText(strTitle);
		} else {
			GetTopLevelWindow().SetWindowText(_T("\0"));
		}
		Invalidate(FALSE);
	}
}

BOOL	CDownloadingListView::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (pCopyDataStruct->dwData == kDownloadingFileExists) {
		CString strNewFilePath = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
		for (auto it = m_vecDLItemInfo.cbegin(); it != m_vecDLItemInfo.cend(); ++it) {
			if (::_wcsicmp(it->pDLItem->strFilePath, strNewFilePath) == 0) {
				TRACEIN(_T("ダウンロード中のファイルと保存先パスがマッチしました"));
				return TRUE;
			}
		}
	} else if (pCopyDataStruct->dwData == kDownloadingURLExists) {
		CString strNewDLURL = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
		for (auto it = m_vecDLItemInfo.cbegin(); it != m_vecDLItemInfo.cend(); ++it) {
			if (::_wcsicmp(it->pDLItem->strURL, strNewDLURL) == 0) {
				TRACEIN(_T("ダウンロードしようとするURLとダウンロード中のURLがマッチしました"));
				return TRUE;
			}
		}
	}
	return 0;
}

//----------------------------------------------------
/// リストから削除する
LRESULT CDownloadingListView::OnRemoveFromList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uintptr_t unique = (uintptr_t)wParam;

	enum { kMaxRetryResumeCount = 7 };
	// vectorから削除する
	DLItem* pDLItem = nullptr;
	HANDLE hMap = NULL;
	int nCount = (int)m_vecDLItemInfo.size();
	for (int i = 0; i < nCount; ++i) {
		if (m_vecDLItemInfo[i].pDLItem->unique == unique) {
			pDLItem = m_vecDLItemInfo[i].pDLItem;
			hMap = m_vecDLItemInfo[i].hMap;
			if (pDLItem->bAbort == false && 0 < pDLItem->nDLFailedCount && pDLItem->nDLFailedCount < kMaxRetryResumeCount) {
				m_vecDLItemInfo[i].strText = _T("DLに失敗。レジュームを試みます...");
			} else {
				m_vecDLItemInfo.erase(m_vecDLItemInfo.begin() + i);
			}
			break;
		}
	}

	// レジュームを試みる
	if (pDLItem->bAbort == false && 0 < pDLItem->nDLFailedCount) {
		ATLASSERT( pDLItem );
		if (pDLItem->nDLFailedCount < kMaxRetryResumeCount) {
			_DoResume(pDLItem);
			return 0;
		} else {
			// 最大試行回数を超えているので中止する
			pDLItem->bAbort = true;
			::DeleteFile(pDLItem->strIncompleteFilePath);
			::SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, pDLItem->strIncompleteFilePath, nullptr);
			TRACEIN(_T("レジュームの最大試行回数を超えて失敗しました。\n不完全ファイルを削除します。: %s"), pDLItem->strIncompleteFilePath);
		}
	}

	// リストには追加されていないアイテム
	CSharedMemory sharedMem;
	if (pDLItem == nullptr) {
		CString sharedMemName;
		sharedMemName.Format(_T("%s%#x_%#x"), DLITEMSHAREDMEMNAME, g_pMainWnd->GetHWND(), unique);		
		pDLItem = static_cast<DLItem*>(sharedMem.OpenSharedMemory(sharedMemName, false));
	}
	ATLASSERT( pDLItem );
	TRACEIN(_T("OnRemoveFromList() : ダウンロード終了 「%s」"), pDLItem->strFileName);
	DWORD dwThreadId = pDLItem->dwThreadId;
	if (dwThreadId) {
		::PostThreadMessage(dwThreadId, WM_DECREMENTTHREADREFCOUNT, 0, 0);
	}

	//if (pItem->bAbort == false)
	//	delete pbscb;
	//pbscb->Release();

	_RefreshList();

	// DLedViewに追加
	m_funcAddDownloadedItem(pDLItem);

	// 使ったハンドルを片付ける
	::UnmapViewOfFile(static_cast<LPCVOID>(pDLItem));
	::CloseHandle(hMap);

	/* 全ファイルのDL終了 */
	if (m_vecDLItemInfo.size() == 0) {
		KillTimer(1);		// 画面更新タイマーを停止
		m_bTimer = false;
		GetTopLevelWindow().SetWindowText(_T("\0"));
		if (CDLOptions::bCloseAfterAllDL) {	
			::PostMessage(GetTopLevelParent(), WM_CLOSE, 0, 0);	// 全てのDLが終わったので閉じる
		}
	}
	// DLの完了を知らせる
	CDLOptions::PlaySoundDLFinish();

	return 0;
}

//----------------------------------------------------------------------
/// 名前を付けて保存ダイアログを使うかどうか返す
LRESULT CDownloadingListView::OnUseSaveFileDialog(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CDLOptions::bUseSaveFileDialog;
}


//----------------------------------------------------------------------
/// リストに追加する
LRESULT CDownloadingListView::OnAddToList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bTimer == false) {	
		SetTimer(1, 1000);		// 画面更新タイマーを開始
		m_dwLastTime = ::timeGetTime();
		m_bTimer = true;
	}

	uintptr_t unique = (uintptr_t)wParam;
	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x_%#x"), DLITEMSHAREDMEMNAME, g_pMainWnd->GetHWND(), unique);
	CSharedMemoryHandle sharedMem;
	DLItem* pDLItem = static_cast<DLItem*>(sharedMem.OpenSharedMemory(sharedMemName, false));
	ATLASSERT( pDLItem );
	DLItemInfomation item(pDLItem, sharedMem.Handle());

	TRACEIN(_T("OnAddToList() : ダウンロード開始 「%s」"), pDLItem->strFileName);

	// アイコンを追加
	_AddIcon(item);

	// 先頭にアイテムを追加
	m_vecDLItemInfo.insert(m_vecDLItemInfo.begin(), item);

	_RefreshList();
	return 0;
}

LRESULT CDownloadingListView::OnTooltipGetDispInfo(LPNMHDR pnmh)
{
    LPNMTTDISPINFO pntdi = (LPNMTTDISPINFO)pnmh;
    if (pntdi->uFlags & TTF_IDISHWND) {
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		int nIndex = _HitTest(pt);
		if (nIndex != -1) {
			CRect rcItem = _GetItemClientRect(nIndex);
			CRect rcFileName = rcItem;
			rcFileName.left = cxFileNameMargin;
			rcFileName.top += cyFileNameMargin;
			rcFileName.bottom	-= (ItemHeight + UnderLineHeight) - cyProgressMargin;
			if (rcFileName.PtInRect(pt)) {
				pntdi->lpszText = m_vecDLItemInfo[nIndex].pDLItem->strFileName;
			} else {
				CRect	rcStop(CPoint(rcItem.right - cxStopLeftSpace, rcItem.top + cyStopTopMargin), CSize(cxStop, cyStop));
				if (rcStop.PtInRect(pt)) {
					::wcscpy_s(pntdi->szText, L"キャンセル");
				} else {
					pntdi->lpszText = m_vecDLItemInfo[nIndex].pDLItem->strURL;
				}
			}
		} else {
			pntdi->lpszText = NULL;
		}
    }

    return 0;
}

void CDownloadingListView::OnMouseMove(UINT nFlags, CPoint pt)
{
	SetMsgHandled(FALSE);

	enum HoldPosition {
		HOLD_NONE = 0,
		HOLD_NAME = 1,
		HOLD_STOP = 2,
	};
	static HoldPosition OldPosition = HOLD_NONE;
	static int nOldIndex = -1;
	HoldPosition	Position = HOLD_NONE;
	int nIndex = _HitTest(pt);
	if (nIndex != -1) {
		CRect rcItem = _GetItemClientRect(nIndex);
		CRect rcFileName = rcItem;
		rcFileName.left = cxFileNameMargin;
		rcFileName.top += cyFileNameMargin;
		rcFileName.bottom	-= (ItemHeight + UnderLineHeight) - cyProgressMargin;
		if (rcFileName.PtInRect(pt)) {
			Position = HOLD_NAME;
		} else {
			CRect	rcStop(CPoint(rcItem.right - cxStopLeftSpace, rcItem.top + cyStopTopMargin), CSize(cxStop, cyStop));
			if (rcStop.PtInRect(pt)) {
				Position = HOLD_STOP;
			} else {
				
			}
		}
	}

	if (nIndex != nOldIndex || Position != OldPosition) {
		nOldIndex	= nIndex;
		OldPosition	= Position;
		m_ToolTip.Activate(FALSE);
		m_ToolTip.Activate(TRUE);
	}
}

//---------------------------------------------
/// DL中のアイテムの名前を変更する
void	CDownloadingListView::OnRenameDLItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT( m_DLItemForPopup );

	CRenameDialog	dlg(m_DLItemForPopup->strFileName, m_DLItemForPopup->strFilePath, false);
	if (dlg.DoModal(m_hWnd) != IDOK)
		return;

	CString strOldFilePath = m_DLItemForPopup->strFilePath;
	int nCount = (int)m_vecDLItemInfo.size();
	for (int i = 0; i < nCount; ++i) {
		if (m_DLItemForPopup->unique == m_vecDLItemInfo[i].pDLItem->unique) {	// まだDL中
			::wcscpy_s(m_vecDLItemInfo[i].pDLItem->strFileName, dlg.GetNewFileName());
			::wcscpy_s(m_vecDLItemInfo[i].pDLItem->strFilePath, dlg.GetNewFilePath());
			InvalidateRect(_GetItemClientRect(i), FALSE);
			return ;
		}
	}
	
	// DLは終わっていたので普通にリネーム
	dlg.DoRename();	
}

//---------------------------------------------------------
/// 保存先のフォルダを開く
void	CDownloadingListView::OnOpenSaveFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT( m_DLItemForPopup );

	CString pathItem = m_DLItemForPopup->strIncompleteFilePath;
	boost::thread([pathItem]() {
		CoInitialize(NULL);
		OpenFolderAndSelectItem(pathItem);
		CoUninitialize();
	}).detach();
}

//-------------------------------------------------------------
/// ダウンロードしたページを表示する
void	CDownloadingListView::OnOpenReferer(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT( m_DLItemForPopup );

	if (m_DLItemForPopup->strReferer[0] == L'\0')
		return ;
	DonutOpenFile(m_DLItemForPopup->strReferer, D_OPENFILE_ACTIVATE);
}

//----------------------------------------------
/// アイテムの左端に置くアイコンを読み込む
void CDownloadingListView::_AddIcon(DLItemInfomation& item)
{
	CString strExt = Misc::GetFileExt(item.pDLItem->strFileName);
	if (strExt.IsEmpty()) {
		item.nImgIndex = 0;
	} else {
		auto it = m_mapImgIndex.find(strExt);
		if (it != m_mapImgIndex.end()) {	// 見つかった
			item.nImgIndex = it->second;
		} else {							// 見つからなかった
			SHFILEINFO sfinfo;
			CString strFind;
			strFind.Format(_T("*.%s"), strExt);
			// アイコンを取得
			::SHGetFileInfo(strFind, FILE_ATTRIBUTE_NORMAL, &sfinfo, sizeof(SHFILEINFO)
				, SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
			ATLASSERT(sfinfo.hIcon);
			// イメージリストにアイコンを追加
			int nImgIndex = m_ImageList.AddIcon(sfinfo.hIcon);
			// 拡張子とイメージリストのインデックスを関連付ける
			m_mapImgIndex.insert(std::pair<CString, int>(strExt, nImgIndex));
			::DestroyIcon(sfinfo.hIcon);

			item.nImgIndex = nImgIndex;
		}
	}
}


//-----------------------------------------------
/// リストを更新する
void	CDownloadingListView::_RefreshList()
{
	CRect rcClient;
	GetClientRect(rcClient);

	int cySize = 0;
	for (auto it = m_vecDLItemInfo.begin(); it != m_vecDLItemInfo.end(); ++it) {
		// アイテムの位置を設定
		it->rcItem.top = cySize;
		cySize += ItemHeight;
		it->rcItem.bottom = cySize;
		cySize += UnderLineHeight;

		it->rcItem.right = rcClient.right;
	}

	CSize	size;
	size.cx	= rcClient.right;
	size.cy	= m_vecDLItemInfo.size() ? cySize : 1;
	SetScrollSize(size, TRUE, false);
	SetScrollLine(10, 10);

	Invalidate(FALSE);
}
//----------------------------------------------
/// ptにあるDLアイテムのインデックスを返す
int		CDownloadingListView::_HitTest(CPoint pt)
{
	int nCount = (int)m_vecDLItemInfo.size();
	for (int i = 0; i < nCount; ++i) {
		//if (m_vecpDLItem[i]->rcItem.PtInRect(pt)) 
		if (_GetItemClientRect(i).PtInRect(pt))
			return i;
	}
	return -1;
}

//----------------------------------------------
/// nIndexのアイテムのクライアント座標での範囲を返す
CRect	CDownloadingListView::_GetItemClientRect(int nIndex)
{
	ATLASSERT(0 <= nIndex && nIndex < (int)m_vecDLItemInfo.size());

	CPoint	ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcItem = m_vecDLItemInfo[nIndex].rcItem;
	rcItem.top		-= ptOffset.y;
	rcItem.bottom	-= ptOffset.y;
	return rcItem;
}


/// レジュームを試みる
void	CDownloadingListView::_DoResume(DLItem* pDLItem)
{
	pDLItem->bTextRefreshStop = true;
	boost::thread([pDLItem, this]() {			
		// ファイルの末尾を削る
		HANDLE hFile = ::CreateFile(pDLItem->strIncompleteFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			ATLASSERT( FALSE );
			pDLItem->bAbort = true;
			/* DLリストから削除 */	
			::PostMessage(m_hWnd, WM_USER_REMOVEFROMDOWNLIST, (WPARAM)pDLItem->unique, 0);
			return ;
		}
		CoInitialize(NULL);
		DWORD filesize = ::GetFileSize(hFile, nullptr);
		enum { kTrimBytes = 4096 };
		if (0 <= filesize && filesize < kTrimBytes)
			filesize = 0;
		else
			filesize -= kTrimBytes;
		::SetFilePointer(hFile, filesize, nullptr, FILE_BEGIN);
		::SetEndOfFile(hFile);
		pDLItem->filesizeForResume = filesize;
		pDLItem->nProgress	= filesize;

		::Sleep(5 * 1000);
		pDLItem->bTextRefreshStop = false;

		char	szDefUserAgent[MAX_PATH] = "\0";
		DWORD	size = MAX_PATH;
		::ObtainUserAgentString(0 , szDefUserAgent , &size);

		struct InternetHandleCloser {
			void operator() (HINTERNET h) const {
				::InternetCloseHandle(h); 
			}
		};
		typedef unique_ptr<std::remove_pointer<HINTERNET>::type, InternetHandleCloser>	HINTERNETHANDLE;
		try {
			HINTERNETHANDLE hInternet(::InternetOpen(CString(szDefUserAgent), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0));
			if (hInternet.get() == NULL)
				throw _T("InternetOpenに失敗");

			URL_COMPONENTS url_components = { sizeof(URL_COMPONENTS) };
			WCHAR	host[INTERNET_MAX_HOST_NAME_LENGTH] = L"";
			url_components.lpszHostName	= host;
			url_components.dwHostNameLength	= INTERNET_MAX_HOST_NAME_LENGTH;
			WCHAR	urlpath[INTERNET_MAX_PATH_LENGTH] = L"";
			url_components.lpszUrlPath	= urlpath;
			url_components.dwUrlPathLength	= INTERNET_MAX_PATH_LENGTH;
			InternetCrackUrl(pDLItem->strURL, 0, ICU_ESCAPE, &url_components);
			HINTERNETHANDLE hSession(::InternetConnect(hInternet.get(), host, url_components.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0)); 
			if (hSession.get() == NULL)
				throw _T("InternetConnectに失敗");
			HINTERNETHANDLE hRequest(::HttpOpenRequest(hSession.get(), _T("GET"), urlpath, _T("HTTP/1.1"), pDLItem->strReferer, nullptr, INTERNET_FLAG_NO_CACHE_WRITE, 0));
			if (hRequest.get() == NULL)
				throw _T("HttpOpenRequestに失敗");
			CString strHeader;
			strHeader.Format(_T("Range: bytes=%d-"), pDLItem->filesizeForResume);
			if (::HttpSendRequest(hRequest.get(), strHeader, strHeader.GetLength(), nullptr, 0) == FALSE)
				throw _T("HttpSendRequestに失敗");
			DWORD	statuscode = 0;
			DWORD	buffsize = sizeof(DWORD);
			if (::HttpQueryInfo(hRequest.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, static_cast<LPVOID>(&statuscode), &buffsize, nullptr) == FALSE)
				throw _T("HttpQueryInfoに失敗");
			if (statuscode != 206)
				throw _T("レジュームできません");

			for (;;) {
				enum { kBuffSize = 5120 };
				char buff[kBuffSize + 1];
				DWORD	dwReadSize = 0;
				BOOL bRet = ::InternetReadFile(hRequest.get(), buff, kBuffSize, &dwReadSize);
				if (bRet == FALSE || dwReadSize == 0 || pDLItem->bAbort)
					break;
				DWORD dwWritten = 0;
				::WriteFile(hFile, (LPCVOID)buff, dwReadSize, &dwWritten, NULL);
				ATLASSERT(dwWritten == dwReadSize);
				pDLItem->nProgress	+= dwReadSize;
			}
			if (pDLItem->bAbort == false && pDLItem->nProgress != pDLItem->nProgressMax)
				throw _T("サイズが一致しません");

			::CloseHandle(hFile);
			if (pDLItem->bAbort) {
				::DeleteFile(pDLItem->strIncompleteFilePath);
				::SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, pDLItem->strIncompleteFilePath, nullptr);
				TRACEIN(_T("レジュームはユーザーによってキャンセルされました。\n不完全ファイルを削除します。: %s"), pDLItem->strIncompleteFilePath);
			} else {
				TRACEIN(_T("レジューム正常終了しました(%s)"), pDLItem->strFileName);
				::MoveFileEx(pDLItem->strIncompleteFilePath, pDLItem->strFilePath, MOVEFILE_REPLACE_EXISTING);
				/* エクスプローラーにファイルの変更通知 */
				::SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATH, pDLItem->strIncompleteFilePath, pDLItem->strFilePath);
				pDLItem->nDLFailedCount = 0;
			}

		} catch (LPCTSTR strError) {
			::CloseHandle(hFile);
			++pDLItem->nDLFailedCount;
			TRACEIN(_T("レジュームに失敗[failcount(%d)] : %s"), pDLItem->nDLFailedCount, strError);
		}

		CoUninitialize();

		/* DLリストから削除 */	
		::PostMessage(m_hWnd, WM_USER_REMOVEFROMDOWNLIST, (WPARAM)pDLItem->unique, 0);
	}).detach();
}



