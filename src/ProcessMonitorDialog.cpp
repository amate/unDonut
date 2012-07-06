/**
*	@file	ProcessMonitorDialog.cpp
*	@brief	プロセス モニター
*/

#include "stdafx.h"
#include "ProcessMonitorDialog.h"
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
//#include <pdh.h>
//#pragma comment (lib, "pdh.lib")
#include "IniFile.h"
#include "Misc.h"
#include "ChildFrameCommandUIUpdater.h"
#include "LinkPopupMenu.h"
#include "FaviconManager.h"
#include "DonutDefine.h"

//////////////////////////////////////////////////////////////////////////////////
// CProcessMonitorDialog

CProcessMonitorDialog::CProcessMonitorDialog(function<void (function<void (HWND)>) > tabbarForEach, bool bMode)
	: m_funcTabBarForEach(tabbarForEach), m_bMultiProcessMode(bMode)
{
}


BOOL CProcessMonitorDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_listview = GetDlgItem(IDC_LIST_PROCESS);
	m_listview.SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

	m_listview.InsertColumn(0, _T("ページ")	, LVCFMT_LEFT, 300);
	m_listview.InsertColumn(1, _T("メモリ")	, LVCFMT_RIGHT, 70);
	//m_listview.InsertColumn(2, _T("CPU")	, LVCFMT_RIGHT, 40);

	m_imagelist.Create(16, 16, ILC_MASK | ILC_COLOR32, 60, 1);
	m_listview.SetImageList(m_imagelist, 0);

	OnTimer(kRefreshTimerID);
#if 0
	m_funcTabBarForEach([this](HWND hWnd) {
		auto pUIData = CChildFrameCommandUIUpdater::GetChildFrameUIData(hWnd);
		int nIndex = m_listview.InsertItem(m_listview.GetItemCount(), pUIData->strTitle);
		
		DWORD dwProcessId = 0;
		::GetWindowThreadProcessId(hWnd, &dwProcessId);
		CHandle hProcess;
		hProcess.Attach(::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId));
		PROCESS_MEMORY_COUNTERS pmcs = { sizeof(PROCESS_MEMORY_COUNTERS) };
		::GetProcessMemoryInfo(hProcess, &pmcs, sizeof(PROCESS_MEMORY_COUNTERS));

		WCHAR memorysize[24] = L"";
		::StrFormatByteSizeW(pmcs.WorkingSetSize, memorysize, 24);
		m_listview.AddItem(nIndex, 1, memorysize);
	});
#endif
#if 0
	// インスタンスリスト用の領域として必要な大きさを求める
	LPTSTR	mszCounterList = NULL;
	DWORD	dwCounterListSize = 0;
	LPTSTR	pmszInstanceList = NULL;
	DWORD	dwInstanceListSize = 0;
	::PdhEnumObjectItems(NULL, NULL, _T("Process"), mszCounterList, &dwCounterListSize, pmszInstanceList, &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);

	dwCounterListSize = 0;
	pmszInstanceList = new TCHAR[dwInstanceListSize];
	::PdhEnumObjectItems(NULL, NULL, _T("Process"), mszCounterList, &dwCounterListSize, pmszInstanceList, &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);
	std::map<std::wstring, int> mapProcess;
	for (LPTSTR strInstance = pmszInstanceList; *strInstance != 0; strInstance += lstrlen(strInstance) + 1) {
		HQUERY	hQuery = NULL;
		::PdhOpenQuery(NULL, 0, &hQuery);

		DWORD dwInstanceIndex = 0;
		auto it = mapProcess.find(std::wstring(strInstance));
		if (it != mapProcess.end()) {
			++it->second;
			dwInstanceIndex = it->second;
		} else {
			mapProcess[std::wstring(strInstance)] = 0;
		}
		PDH_COUNTER_PATH_ELEMENTS cpe = { 0 };
		cpe.szObjectName	= _T("Process");
		cpe.szInstanceName	= strInstance;
		cpe.dwInstanceIndex	= dwInstanceIndex;
		cpe.szCounterName	= _T("ID Process");
		TCHAR fullCounterPath[1024] = _T("");
		DWORD dwBufferSize = _countof(fullCounterPath);
		::PdhMakeCounterPath(&cpe, fullCounterPath, &dwBufferSize, 0);

		HCOUNTER hCounter = NULL;
		::PdhAddCounter(hQuery, fullCounterPath, 0, &hCounter);
		::PdhCollectQueryData(hQuery);
		PDH_FMT_COUNTERVALUE	fmtValue;
		::PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, NULL, &fmtValue);
		fmtValue.longValue;
		::PdhRemoveCounter(hCounter);
		::PdhCloseQuery(hQuery);
	}
#endif

	DlgResize_Init();

#if 0
	HRESULT hr = ::CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	CComPtr<IWbemLocator>	spLocator;	
	spLocator.CoCreateInstance(CLSID_WbemLocator);
	CComPtr<IWbemServices> spNamespace;
	hr = spLocator->ConnectServer(CComBSTR(L"root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &spNamespace);

	hr = ::CoSetProxyBlanket(spNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	CComPtr<IWbemRefresher> spRefresher;
	spRefresher.CoCreateInstance(CLSID_WbemRefresher);
	CComQIPtr<IWbemConfigureRefresher> spConfig = spRefresher;
	
	CComPtr<IWbemHiPerfEnum> spEnum;
	hr = spConfig->AddEnum(spNamespace, L"Win32_PerfRawData_PerfProc_Process", 0, NULL, &spEnum, NULL);

	ULONG nCount;
	hr = spEnum->GetObjects(0, 0, nullptr, &nCount);
#endif

	CIniFileI pr(g_szIniFileName, _T("ProcessMonitorDialog"));
	CRect rc;
	rc.top		= pr.GetValuei(_T("top"));
	rc.left		= pr.GetValuei(_T("left"));
	rc.right	= pr.GetValuei(_T("right"));
	rc.bottom	= pr.GetValuei(_T("bottom"));
	if (rc != CRect()) {
		SetWindowPos(NULL, &rc, SWP_NOZORDER);
	} else {
		CenterWindow(GetParent());
	}

	SetTimer(kRefreshTimerID, kRefreshInterval);
	PostMessage(WM_CHANGESIZE);
	return TRUE;
}

void CProcessMonitorDialog::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	PostMessage(WM_CHANGESIZE);
}

LRESULT CProcessMonitorDialog::OnChangeSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nMemoryColWidth = m_listview.GetColumnWidth(1);
	CRect rc;
	m_listview.GetClientRect(&rc);
	m_listview.SetColumnWidth(0, rc.right - nMemoryColWidth - 2);
	return 0;
}

void CProcessMonitorDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kRefreshTimerID) {
		//m_listview.SetRedraw(FALSE);
		int nSelIndex = m_listview.GetSelectedIndex();
		m_mapItem.clear();
		m_listview.DeleteAllItems();
		m_imagelist.RemoveAll();
		m_imagelist.AddIcon(CLinkPopupMenu::s_iconLink);
		CIcon	mainIcon;
		mainIcon.LoadIcon(IDR_MAINFRAME);
		m_imagelist.AddIcon(mainIcon);
		m_funcTabBarForEach([this](HWND hWnd) {
			auto pUIData = CChildFrameCommandUIUpdater::GetChildFrameUIData(hWnd);
			int nImageIndex = 0;
			if (pUIData->strFaviconURL[0] != 0) {
				HICON hIcon = CFaviconManager::GetFavicon(pUIData->strFaviconURL);
				if (hIcon)
					nImageIndex = m_imagelist.AddIcon(hIcon);
			}

			DWORD dwProcessId = 0;
			::GetWindowThreadProcessId(hWnd, &dwProcessId);

			m_mapItem[dwProcessId].push_back(CProcessMonitorDialog::ItemData(dwProcessId, hWnd, nImageIndex, pUIData->strTitle));
		});
		SIZE_T totalWorkingSet = 0;
		auto funcGetProcessWorkingSet = [&](DWORD dwProcessId) -> CString {
			CHandle hProcess;
			hProcess.Attach(::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId));
			PROCESS_MEMORY_COUNTERS pmcs = { sizeof(PROCESS_MEMORY_COUNTERS) };
			::GetProcessMemoryInfo(hProcess, &pmcs, sizeof(PROCESS_MEMORY_COUNTERS));
			totalWorkingSet += pmcs.WorkingSetSize;
			CString memorysize;
			::StrFormatByteSizeW(pmcs.WorkingSetSize, memorysize.GetBuffer(24), 24);
			memorysize.ReleaseBuffer();
			return memorysize;
		};
		for (auto it = m_mapItem.begin(); it != m_mapItem.end(); ++it) {
			for (auto itItem = it->second.begin(); itItem != it->second.end(); ++itItem) {
				int nIndex = m_listview.InsertItem(m_listview.GetItemCount(), itItem->strTitle, itItem->iconIndex);	
				m_listview.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(&(*itItem)));

				if (m_bMultiProcessMode && itItem == it->second.begin()) {
					m_listview.AddItem(nIndex, 1, funcGetProcessWorkingSet(itItem->processId));
				}
				if (it->second.size() > 1) {
					auto itEndPrev = std::prev(it->second.end());
					if (itItem != itEndPrev)
						itItem->bDrawSep = false;
				}
			}
		}
		if (m_bMultiProcessMode == false)
			totalWorkingSet = 0;
		DWORD dwMainFrameProcessId = ::GetCurrentProcessId();
		m_mapItem[0].push_back(ItemData(0, GetParent(), 1, L"メインフレーム"));
		m_mapItem[0].front().bDrawSep = false;
		m_listview.InsertItem(0, L"メインフレーム", 1);
		m_listview.AddItem(0, 1, funcGetProcessWorkingSet(dwMainFrameProcessId));
		m_listview.SetItemData(0, reinterpret_cast<DWORD_PTR>(&(*m_mapItem[0].begin())));

		WCHAR totalmemsize[24] = L"";
		::StrFormatByteSizeW(totalWorkingSet, totalmemsize, 24);
		CString title;
		title.Format(_T("プロセス モニター - 総メモリ使用量 %s"), totalmemsize);
		SetWindowText(title);

		m_listview.SelectItem(nSelIndex);
		//m_listview.SetRedraw(TRUE);
		//m_listview.Invalidate();
	}
}

void CProcessMonitorDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DWORD processId = -1;
	int nSelIndex = m_listview.GetSelectedIndex();
	if (nSelIndex != -1) 
		processId = reinterpret_cast<ItemData*>(m_listview.GetItemData(nSelIndex))->processId;

	CDCHandle dc = lpDrawItemStruct->hDC;
	dc.SaveDC();
	{
		dc.SetBkMode(TRANSPARENT);
		ItemData* pItem = reinterpret_cast<ItemData*>(lpDrawItemStruct->itemData);
		//--lpDrawItemStruct->rcItem.top;
		if (lpDrawItemStruct->itemState & ODS_SELECTED || processId == pItem->processId) {
			if (GetFocus() != m_listview) {
				dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(240, 240, 240));
				dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			} else {
				dc.FillRect(&lpDrawItemStruct->rcItem, COLOR_HIGHLIGHT);
				dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
		} else {
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255, 255, 255));
			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		}		

		CRect rcIcon;
		m_listview.GetItemRect(lpDrawItemStruct->itemID, &rcIcon, LVIR_ICON);
		rcIcon.top += (rcIcon.Height() - 16) / 2;
		m_imagelist.Draw(dc, pItem->iconIndex, rcIcon.left, rcIcon.top, ILD_TRANSPARENT);

		CString name;
		m_listview.GetItemText(lpDrawItemStruct->itemID, 0, name);
		CRect rcItem;
		m_listview.GetItemRect(lpDrawItemStruct->itemID, &rcItem, LVIR_LABEL);
		rcItem.left = rcIcon.left + 20;
		dc.DrawText(name, name.GetLength(), &rcItem, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

		CString memory;
		m_listview.GetItemText(lpDrawItemStruct->itemID, 1, memory);
		CRect rcMemory;
		m_listview.GetSubItemRect(lpDrawItemStruct->itemID, 1, LVIR_LABEL, &rcMemory);
		dc.DrawText(memory, memory.GetLength(), &rcMemory, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		CPen pen(::CreatePen(PS_SOLID, 1, RGB(208, 208, 208)));
		HPEN hPenPrev = dc.SelectPen(pen);
		if (pItem->bDrawSep) {
			POINT arrpt[] = {
				{ lpDrawItemStruct->rcItem.left	, lpDrawItemStruct->rcItem.bottom - 1	},
				{ lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom - 1	},
				{ lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.top	},
			};
			dc.Polyline(arrpt, _countof(arrpt));
			
		} else {
			POINT arrpt[] = {
				{ lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom - 1	},
				{ lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.top	},
			};
			dc.Polyline(arrpt, _countof(arrpt));
		}
		dc.SelectPen(hPenPrev);
	}
	dc.RestoreDC(-1);
}

LRESULT CProcessMonitorDialog::OnListViewItemChanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW pnmlv = reinterpret_cast<LPNMLISTVIEW>(pnmh);
	if (pnmlv->iItem == -1)
		return 0;
	ItemData* pData = reinterpret_cast<ItemData*>(m_listview.GetItemData(pnmlv->iItem));
	if (pData == nullptr)
		return 0;

	DWORD processId = pData->processId;
	if (processId == 0)	// メインフレームは下の処理やらない
		return 0;
	auto funcSelSameProcessId = [&, this](int StartIndex, bool bDown) {
		int nIndex = StartIndex;
		for (;;) {
			int nNextIndex = m_listview.GetNextItem(nIndex, bDown ? LVNI_BELOW : LVNI_ABOVE);
			if (nNextIndex == -1 || nNextIndex == nIndex)
				break;
			ItemData* pNextData = reinterpret_cast<ItemData*>(m_listview.GetItemData(nNextIndex));
			if (pNextData->processId == processId) {
				CRect rcItem;
				m_listview.GetItemRect(nNextIndex, &rcItem, LVIR_BOUNDS);
				m_listview.InvalidateRect(&rcItem);
				//m_listview.SetItemState(nNextIndex, LVIS_SELECTED, LVIS_SELECTED);
				nIndex = nNextIndex;
			} else
				break;
		}
	};
	funcSelSameProcessId(pnmlv->iItem, true);
	funcSelSameProcessId(pnmlv->iItem, false);

	return 0;
}


void CProcessMonitorDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CIniFileIO pr(g_szIniFileName, _T("ProcessMonitorDialog"));
	CRect rc;
	GetWindowRect(&rc);
	pr.SetValue(rc.top	, _T("top"));
	pr.SetValue(rc.left	, _T("left"));
	pr.SetValue(rc.right	, _T("right"));
	pr.SetValue(rc.bottom	, _T("bottom"));

	m_imagelist.Destroy();

	GetParent().PostMessage(WM_RELEASE_PROCESSMONITOR_PTR);

	DestroyWindow();
}

void CProcessMonitorDialog::OnKillProcess(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nSelIndex = m_listview.GetSelectedIndex();
	if (nSelIndex == -1) 
		return ;

	if (nSelIndex == 0) {
		if (MessageBox(L"メインフレームを終了します。よろしいですか？", L"確認", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) {
			return ;
		} else {
			GetParent().PostMessage(WM_CLOSE);
			return ;
		}
	}
	ItemData* pData = reinterpret_cast<ItemData*>(m_listview.GetItemData(nSelIndex));
	auto& vecTab = m_mapItem[pData->processId];
	for (auto it = vecTab.rbegin(); it != vecTab.rend(); ++it) {
		::PostMessage(it->hWnd, WM_CLOSE, 0, 0);
	}
}








