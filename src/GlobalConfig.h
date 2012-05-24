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
	DWORD	dwExtendedStyleFlags;

	// CSearchBarOption
	bool	bScrollCenter;
	
	// SerachBar
	bool	bHilightSwitch;

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





















