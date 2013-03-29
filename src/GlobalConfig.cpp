/**
*	@file	GlobalConfig.cpp
*	@brief	MainFrameとChildFrameで共有する設定
*/

#include "stdafx.h"
#include "GlobalConfig.h"
#include "option\MainOption.h"
#include "option\MouseDialog.h"
#include "option\MenuDialog.h"
#include "option\DLControlOption.h"
#include "option\SearchPropertyPage.h"
#include "option\AddressBarPropertyPage.h"
#include "option\UrlSecurityOption.h"
#include "Download\DownloadManager.h"
#include "Download\DownloadOptionDialog.h"

#define DONUTGLOBALCONFIGSHAREDNAME	_T("DonutGlobalConfigSharedData")

// MainFrameが作成/破棄する
void	CreateGlobalConfig(GlobalConfigManageData* pMangeData)
{
	DWORD dwProcessID = ::GetCurrentProcessId();
	CString sharedName;
	sharedName.Format(_T("%s0x%x"), DONUTGLOBALCONFIGSHAREDNAME, dwProcessID);

	HANDLE hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GlobalConfig), sharedName);
	ATLASSERT( hMap );
	pMangeData->hMap	= hMap;
	pMangeData->pGlobalConfig = (GlobalConfig*)::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	ATLASSERT( pMangeData->pGlobalConfig );

	pMangeData->pGlobalConfig->bHilightSwitch	= false;
	// ProxyComboBox
	pMangeData->pGlobalConfig->ProxyAddress[0] = '\0';
	pMangeData->pGlobalConfig->ProxyBypass[0] = '\0';

	pMangeData->pGlobalConfig->bMainFrameClosing = false;

}


void	SetGlobalConfig(GlobalConfig* pConfig)
{
	// CMainOption
	pConfig->dwMainExtendedStyle	= CMainOption::s_dwMainExtendedStyle;
	pConfig->dwMainExtendedStyle2	= CMainOption::s_dwMainExtendedStyle2;
	pConfig->AutoImageResizeType	= CMainOption::s_nAutoImageResizeType;
	pConfig->bMultiProcessMode		= CMainOption::s_BrowserOperatingMode == BROWSEROPERATINGMODE::kMultiProcessMode;

	// CMouseOption
	pConfig->bUseRightDragSearch = CMouseOption::s_bUseRightDragSearch;
	pConfig->bUseRect			 = CMouseOption::s_bUseRect;
	::wcscpy_s(pConfig->strREngine, CMouseOption::s_strREngine);
	::wcscpy_s(pConfig->strTEngine, CMouseOption::s_strTEngine);
	::wcscpy_s(pConfig->strLEngine, CMouseOption::s_strLEngine);
	::wcscpy_s(pConfig->strBEngine, CMouseOption::s_strBEngine);
	::wcscpy_s(pConfig->strCEngine, CMouseOption::s_strCEngine);
	pConfig->nDragDropCommandID	= CMouseOption::s_nDragDropCommandID;

	// CMenuOption
	pConfig->bNoCustomIEMenu	= CMenuOption::s_bNoCustomIEMenu;

	// CDLControlOption
	pConfig->dwDLControlFlags		= CDLControlOption::s_dwDLControlFlags;
	pConfig->dwExtendedStyleFlags	= CDLControlOption::s_dwExtendedStyleFlags;

	pConfig->bChangeUserAgent	= (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_USER_AGENT_FLAG) != 0;
	::wcscpy_s(pConfig->strUserAgent, CDLControlOption::s_szUserAgent);
	::wcscpy_s(pConfig->strUserAgentCurrent, CDLControlOption::s_szUserAgent_cur);

	// CSearchBarOption
	pConfig->bScrollCenter	= CSearchBarOption::s_bScrollCenter;
	pConfig->bSaveSearchWord	= CSearchBarOption::s_bSaveSearchWord;
	pConfig->bSaveSearchWordOrg = CSearchBarOption::s_bSaveSearchWord;

	// CAddressBarOptionｔ
	pConfig->bReplaceSpace	= CAddressBarOption::s_bReplaceSpace;

	// CUrlSecurityOption
	pConfig->bUrlSecurityValid = CUrlSecurityOption::s_bValid;

	// CDownloadManager
	pConfig->bUseDownloadManager = CDownloadManager::UseDownloadManager();
	// CDLOptions は メインフレームのOnCreateとDLオプション更新時に行われるでいらない
}

void	DestroyGlobalConfig(GlobalConfigManageData* pMangeData)
{
	::UnmapViewOfFile(pMangeData->pGlobalConfig);
	pMangeData->pGlobalConfig = nullptr;
	::CloseHandle(pMangeData->hMap);
	pMangeData->hMap = NULL;
}

// ChildFrameが取得/返却する
GlobalConfigManageData GetGlobalConfig(HWND hWndMainFrame)
{
	DWORD dwProcessID = 0;
	::GetWindowThreadProcessId(hWndMainFrame, &dwProcessID);
	CString sharedName;
	sharedName.Format(_T("%s0x%x"), DONUTGLOBALCONFIGSHAREDNAME, dwProcessID);

	GlobalConfigManageData	manageData;
	manageData.hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedName);
	ATLASSERT( manageData.hMap );
	manageData.pGlobalConfig = (GlobalConfig*)::MapViewOfFile(manageData.hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	ATLASSERT( manageData.pGlobalConfig );
	return manageData;
}

void		  CloseGlobalConfig(GlobalConfigManageData* pManageData)
{
	DestroyGlobalConfig(pManageData);
}








