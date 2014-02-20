/**
*	@file	GlobalConfig.h
*	@brief	MainFrameとChildFrameで共有する設定
*/

#pragma once


struct GlobalConfig
{
	//CMainOption
	DWORD	dwMainExtendedStyle;
	DWORD	dwMainExtendedStyle2;
	int		AutoImageResizeType;
	bool	bMultiProcessMode;

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

	bool	bChangeUserAgent;
	WCHAR	strUserAgent[256];
	WCHAR	strUserAgentCurrent[256];

	// CSearchBarOption
	bool	bScrollCenter;
	bool	bSaveSearchWord;
	bool	bSaveSearchWordOrg;
	HWND	SearchEditHWND;
	int		nMimimulHilightTextLength;
	
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

	// ProxyComboBox
	char	ProxyAddress[512];
	char	ProxyBypass[512];

	bool	bMainFrameClosing;

};

struct GlobalConfigManageData
{
	HANDLE hMap;
	GlobalConfig* pGlobalConfig;
};

// MainFrameが作成/破棄する
void	CreateGlobalConfig(GlobalConfigManageData* pMangeData);
void	SetGlobalConfig(GlobalConfig* pConfig);
void	DestroyGlobalConfig(GlobalConfigManageData* pManageData);

// ChildFrameが取得/返却する
GlobalConfigManageData GetGlobalConfig(HWND hWndMainFrame);
void	CloseGlobalConfig(GlobalConfigManageData* pMangeData);


// ====================================

#include <functional>
#include <unordered_map>

enum ObserverClass {
	kSupressPopupOption,
	kLoginDataManager,
	kCustomContextMenuOption,
	kUrlSecurityOption,
	kAcceleratorOption,
};

/// 子プロセス側が利用するクラス
/// このクラスに更新の通知を依頼する

class CSharedDataChangeSubject
{
public:
	static void	AddObserver(ObserverClass obclass, std::function<void (HWND)> callback);
	static void	RemoveObserver(ObserverClass obclass);

	static void NotifyFromMainFrame(ObserverClass obclass, HWND hWndMainFrame);

private:
	// Data members
	static std::unordered_map<ObserverClass, std::function<void (HWND)>> s_mapOBClassAndCallBack;
};

/// メインフレーム側が利用するクラス
/// CSharedDataChangeSubject 側と裏側で繋がっていて 各子プロセスに更新の通知が行われる

class CSharedDataChangeNotify
{
public:
	static void	NotifyObserver(ObserverClass obclass);
};














