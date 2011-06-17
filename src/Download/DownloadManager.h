#pragma once

#include <regex>
#include <boost\thread.hpp>
#include <downloadmgr.h>
#include "../MtlWin.h"

#include "DownloadFrame.h"
#include "DownloadOptionDialog.h"

// Forward Declaration
class CMainFrame;
class CChildFrame;
class CCustomBindStatusCallBack;

///////////////////////////////////////////////////////////////
// CDownloadManager

class CDownloadManager : 
	public CWindowImpl<CDownloadManager>,
	public IDownloadManager
{
public:
	// Constructor
	CDownloadManager();

	void	SetParent(HWND hWnd) { m_hWndParent = hWnd; }
	static CDownloadManager* GetInstance() { return s_pThis; }
	static bool UseDownloadManager();
	static void	SetReferer(LPCTSTR strReferer) { s_strReferer = strReferer; }

	void	DownloadStart(LPCTSTR strURL, LPCTSTR strDLFolder = NULL, HWND hWnd = NULL, DWORD dwDLOption = DLO_OVERWRITEPROMPT );
	int		GetDownloadingCount() const;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject);
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release(){ return 1; }

	// IDownloadManager
	STDMETHODIMP Download(
		IMoniker* pmk,  
		IBindCtx* pbc,  
		DWORD	  dwBindVerb,  
		LONG	  grfBINDF,  
		BINDINFO* pBindInfo,  
		LPCOLESTR pszHeaders,  
		LPCOLESTR pszRedir,  
		UINT	  uiCP );


	// Message map and Handler
	BEGIN_MSG_MAP_EX(CDownloadManager)
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		COMMAND_ID_HANDLER_EX( ID_SHOW_DLMANAGER, OnShowDLManager )
		MESSAGE_HANDLER_EX( WM_GETDEFAULTDLFOLDER, OnDefaultDLFolder )
		MESSAGE_HANDLER_EX( WM_STARTDOWNLOAD	 , OnStartDownload )
	END_MSG_MAP()


	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnShowDLManager(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnDefaultDLFolder(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnStartDownload(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void	_DLStart(CString* pstrURL, IBindStatusCallback* bscb);
	CCustomBindStatusCallBack*	_CreateCustomBindStatusCallBack();

	// Data members
	static CDownloadManager*	s_pThis;
	static CString				s_strReferer;
	CDownloadFrame m_wndDownload;
	HWND			m_hWndParent;

	UINT	WM_GETDEFAULTDLFOLDER;
	UINT	WM_STARTDOWNLOAD;

};


///////////////////////////////////////////////////////////////////
// 