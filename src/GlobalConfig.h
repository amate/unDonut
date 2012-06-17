/**
*	@file	GlobalConfig.h
*	@brief	MainFrameÇ∆ChildFrameÇ≈ã§óLÇ∑ÇÈê›íË
*/

#pragma once


struct GlobalConfig
{
	//CMainOption
	int		AutoImageResizeType;

	// CMouseOption
	bool	bUseRightDragSearch;
	bool	bUseRect;
	WCHAR	strREngine[256];
	WCHAR	strTEngine[256];
	WCHAR	strLEngine[256];
	WCHAR	strBEngine[256];
	WCHAR	strCEngine[256];
	int		nDragDropCommandID;

	// CMenuOption
	bool	bNoCustomIEMenu;

	// CDLControlOption
	DWORD	dwDLControlFlags;
	DWORD	dwExtendedStyleFlags;

	// CSearchBarOption
	bool	bScrollCenter;
	
	// SerachBar
	bool	bHilightSwitch;

	// CAddressBarOption
	bool	bReplaceSpace;

	// CUrlSecurityOption
	bool	bUrlSecurityValid;
	
	// CDownloadManager
	bool	bUseDownloadManager;
	// CDLOptions
	bool	bShowDLManagerOnDL;
	WCHAR	strDefaultDLFolder[MAX_PATH];
	WCHAR	strImageDLFolder[MAX_PATH];
	DWORD	dwDLImageExStyle;

};

struct GlobalConfigManageData
{
	HANDLE hMap;
	GlobalConfig* pGlobalConfig;
};

// MainFrameÇ™çÏê¨/îjä¸Ç∑ÇÈ
void	CreateGlobalConfig(GlobalConfigManageData* pMangeData);
void	SetGlobalConfig(GlobalConfig* pConfig);
void	DestroyGlobalConfig(GlobalConfigManageData* pManageData);

// ChildFrameÇ™éÊìæ/ï‘ãpÇ∑ÇÈ
GlobalConfigManageData GetGlobalConfig(HWND hWndMainFrame);
void	CloseGlobalConfig(GlobalConfigManageData* pMangeData);





















