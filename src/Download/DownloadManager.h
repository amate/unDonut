#pragma once

#include <downloadmgr.h>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include "../MtlWin.h"
#include "../DonutDefine.h"
#include "DownloadFrame.h"
#include "DownloadOptionDialog.h"

// Forward Declaration
class CMainFrame;
class CChildFrame;
class CCustomBindStatusCallBack;
class CDonutView;


struct DownloadData {
	std::wstring	strURL;
	std::wstring	strReferer;
	uintptr_t		unique;
	std::wstring	strFolder;
	DWORD			dwImageExStyle;

	DownloadData() : unique(0), dwImageExStyle(0) { }

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar  & strURL & strReferer & unique & strFolder & dwImageExStyle;
	}
};

///////////////////////////////////////////////////////////////
// CDownloadManager

class CDownloadManager : 
	public CWindowImpl<CDownloadManager>
{
	friend class CDonutView;
public:
	// Constructor
	CDownloadManager();

	void	SetParent(HWND hWnd);
	static CDownloadManager* GetInstance() { return s_pThis; }
	static bool UseDownloadManager();
	static void	SetReferer(LPCTSTR strReferer) { s_strReferer = strReferer; }

	static CCustomBindStatusCallBack*	CreateCustomBindStatusCallBack(HWND hWndMainFrame, LPCTSTR defaultDLFolder);
	static void	StartTheDownload(LPCTSTR strURL, IBindStatusCallback* pcbsc);

	void	DownloadStart(LPCTSTR strURL, LPCTSTR strDLFolder = NULL, HWND hWnd = NULL, DWORD dwDLOption = -1);
	int		GetDownloadingCount() const;

	// Message map and Handler
	BEGIN_MSG_MAP_EX(CDownloadManager)
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		USER_MSG_WM_GETDOWNLOADINGVIEWHWND( OnGetDownloadingViewHWND )
		COMMAND_ID_HANDLER_EX( ID_SHOW_DLMANAGER, OnShowDLManager )
		MESSAGE_HANDLER_EX( WM_GETDEFAULTDLFOLDER, OnDefaultDLFolder )
		MESSAGE_HANDLER_EX( WM_STARTDOWNLOAD	 , OnStartDownload )
		MESSAGE_HANDLER_EX( WM_SETREFERER		 , OnSetReferer )
	END_MSG_MAP()


	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	HWND OnGetDownloadingViewHWND();
	void OnShowDLManager(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnDefaultDLFolder(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnStartDownload(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetReferer(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static void	_DLStart(CString* pstrURL, IBindStatusCallback* bscb);

	// Data members
	static CDownloadManager*	s_pThis;
	static CString				s_strReferer;
	CDownloadFrame m_wndDownload;
	HWND			m_hWndParent;

	UINT	WM_GETDEFAULTDLFOLDER;
	UINT	WM_STARTDOWNLOAD;
	UINT	WM_SETREFERER;

};


///////////////////////////////////////////////////////////////////
// 