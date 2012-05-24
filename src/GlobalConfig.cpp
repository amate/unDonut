/**
*	@file	GlobalConfig.cpp
*	@brief	MainFrame‚ÆChildFrame‚Å‹¤—L‚·‚éÝ’è
*/

#include "stdafx.h"
#include "GlobalConfig.h"
#include "option\MainOption.h"
#include "option\MouseDialog.h"
#include "option\MenuDialog.h"
#include "option\DLControlOption.h"
#include "option\SearchPropertyPage.h"

#define DONUTGLOBALCONFIGSHAREDNAME	_T("DonutGlobalConfigSharedData")

// MainFrame‚ªì¬/”jŠü‚·‚é
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
}

void	SetGlobalConfig(GlobalConfig* pConfig)
{
	// CMainOption
	pConfig->AutoImageResizeType	= CMainOption::s_nAutoImageResizeType;

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
	pConfig->dwExtendedStyleFlags	= CDLControlOption::s_dwExtendedStyleFlags;

	// CSearchBarOption
	pConfig->bScrollCenter	= CSearchBarOption::s_bScrollCenter;

}

void	DestroyGlobalConfig(GlobalConfigManageData* pMangeData)
{
	::UnmapViewOfFile(pMangeData->pGlobalConfig);
	::CloseHandle(pMangeData->hMap);
}

// ChildFrame‚ªŽæ“¾/•Ô‹p‚·‚é
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








