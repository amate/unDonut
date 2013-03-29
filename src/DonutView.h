/**
 *	@file	DonutView.h
 *	@brief	View.
 */

#pragma once

#include <DownloadMgr.h>
#include "DocHostUIHandlerDispatch.h"
#include "MtlBrowser.h"
#include "HLinkDataObject.h"


enum EDvs_AutoRefresh {
	DVS_AUTOREFRESH_15SEC	= 0x00000001L,
	DVS_AUTOREFRESH_30SEC	= 0x00000002L,
	DVS_AUTOREFRESH_1MIN	= 0x00000004L,
	DVS_AUTOREFRESH_2MIN	= 0x00000008L,
	DVS_AUTOREFRESH_5MIN	= 0x00000010L,
	DVS_AUTOREFRESH_USER	= 0x00000020L,	// UDT DGSTR ( dai

	DVS_AUTOREFRESH_OR		= (    DVS_AUTOREFRESH_15SEC | DVS_AUTOREFRESH_30SEC
								 | DVS_AUTOREFRESH_1MIN  | DVS_AUTOREFRESH_2MIN | DVS_AUTOREFRESH_5MIN
								 | DVS_AUTOREFRESH_USER
							  ),
};

// 前方宣言
class CChildFrameUIStateChange;
struct GlobalConfig;

//////////////////////////////////////////////////////////////////////
// CDonutView

class CDonutView :
	public CWindowImpl<CDonutView, CAxWindow>,
	public CWebBrowser2,
	public IServiceProvider,
	public IDropTarget,
	public IDispatch,
	public IDownloadManager
{
public:
	// Declaration
	DECLARE_WND_SUPERCLASS( _T("DonutView"), CAxWindow::GetWndClassName() )

	// Constructor
	CDonutView(CChildFrameUIStateChange& UI);

	void	SetGlobalConfig(GlobalConfig* pConfig) { m_pGlobalConfig = pConfig; }

	void	SetDefaultFlags(DWORD dwDefaultDLCtrl, DWORD dwDefaultExStyle, DWORD dwAutoRefresh);
	void	SetAutoRefreshStyle(DWORD dwStyle);
	DWORD	GetAutoRefreshStyle() const { return m_dwAutoRefreshStyle; }

	// Methods
	DWORD	GetDLControlFlags() const { return m_dwDLControlFlags; }
	void	PutDLControlFlags(DWORD dwDLControlFlags);

	DWORD	GetExStyle() const { return m_dwExStyle; }
	void	SetExStyle(DWORD dwExStyle);
	void	SetIeMenuNoCstm(int nStatus);
	CString GetAnchorURL() const { return m_ExternalUIDispatch.GetAnchorURL(); }
	CPoint	GetMenuPoint() const { return m_ExternalUIDispatch.GetPoint(); }
	void	InitDLControlFlags() { _InitDLControlFlags(); }
	void	LightRefresh();

	//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
	void	SetOperateDragDrop(BOOL bOn);

	bool	UseDownloadManager() const;	// for DocHostUIHandlerDispatch
	void	StartTheDownload(LPCTSTR strURL, bool bSaveImage = false);

	// Overrides
	BOOL	PreTranslateMessage(MSG *pMsg);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject);
	STDMETHODIMP_(ULONG) AddRef() { return 1; };
	STDMETHODIMP_(ULONG) Release(){ return 1; };

	// IServiceProvider
	STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void** ppv);

	// IDropTarget
    STDMETHODIMP DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    STDMETHODIMP DragLeave();
    STDMETHODIMP Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	// IDispatch
	STDMETHODIMP	GetTypeInfoCount(UINT * pctinfo) { return E_NOTIMPL; }
	STDMETHODIMP	GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo * *pptinfo) { return E_NOTIMPL; }
	STDMETHODIMP	GetIDsOfNames(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid) { return E_NOTIMPL; }

	STDMETHODIMP	Invoke(
			DISPID			dispidMember,
			REFIID			riid,
			LCID			lcid,
			WORD			wFlags,
			DISPPARAMS *	pdispparams,
			VARIANT *		pvarResult,
			EXCEPINFO * 	pexcepinfo,
			UINT *			puArgErr);

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


	// Message map and handlers
	BEGIN_MSG_MAP( CDonutView )
		MSG_WM_CREATE	( OnCreate )
		MSG_WM_DESTROY	( OnDestroy )
		MSG_WM_TIMER( OnTimer )
		if (uMsg == WM_INITMENUPOPUP || uMsg == WM_MENUSELECT) {
			GetParent().SendMessage(uMsg, wParam, lParam);
			return TRUE;
		}
		COMMAND_ID_HANDLER_EX( ID_DLCTL_BGSOUNDS		, OnMultiBgsounds		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_VIDEOS			, OnMultiVideos 		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_DLIMAGES		, OnMultiDlImages		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_RUNACTIVEXCTLS	, OnSecurRunactivexctls )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_DLACTIVEXCTLS	, OnSecurDlactivexctls	)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_SCRIPTS 		, OnSecurScritps		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_JAVA			, OnSecurJava			)

		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_MULTI		, OnMultiChg			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_SECU		, OnSecuChg 			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_MULTI	, OnAllOnOff			)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_SECU 	, OnAllOnOff			)

		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_NONE , OnAutoRefreshNone )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_15SEC, OnAutoRefresh15sec)
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_30SEC, OnAutoRefresh30sec)
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_1MIN , OnAutoRefresh1min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_2MIN , OnAutoRefresh2min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_5MIN , OnAutoRefresh5min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_USER , OnAutoRefreshUser )

		COMMAND_ID_HANDLER_EX( ID_DOCHOSTUI_OPENNEWWIN, OnOpenNewWin )
		COMMAND_ID_HANDLER_EX( ID_MESSAGE_FILTER  , OnMessageFilter   )
		COMMAND_ID_HANDLER_EX( ID_MOUSE_GESTURE   , OnMouseGesture	  )
		COMMAND_ID_HANDLER_EX( ID_BLOCK_MAILTO	  , OnBlockMailto	  )
	END_MSG_MAP()


	int		OnCreate(LPCREATESTRUCT /*lpCreateStruct*/);
	void	OnDestroy();
	void	OnTimer(UINT_PTR nIDEvent);
	void	OnMultiChg(WORD, WORD, HWND);
	void	OnSecuChg(WORD, WORD, HWND);
	void	OnAllOnOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);

	void	OnMultiBgsounds(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiVideos(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiDlImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurRunactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurDlactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurScritps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurJava(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void	OnBlockMailto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMouseGesture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnOpenNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMessageFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnDocHostUIOpenNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefreshNone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefresh15sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefresh30sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefresh1min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefresh2min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefresh5min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnAutoRefreshUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

private:
	void	_InitDLControlFlags() { PutDLControlFlags(m_dwDefaultDLControlFlags); }
	bool	_ToggleFlag(WORD wID, DWORD dwFlag, BOOL bReverse = FALSE);
	void	_AddFlag(DWORD dwFlag);
	void	_RemoveFlag(DWORD dwFlag);
	void	_SetTimer();

	enum { AutoRefreshTimerID = 1 };

	// Data members
	CComQIPtr<IAxWinHostWindow>					m_spHost;
	CComQIPtr<IAxWinAmbientDispatchEx>			m_spAxAmbient;

	DWORD						m_dwDefaultDLControlFlags;
	//DWORD						m_dwDefaultExtendedStyleFlags;	//+++

	CDocHostUIHandlerDispatch	m_ExternalUIDispatch;
	//CAmbientDispatch			m_ExternalAmbientDispatch;

	bool						m_bUseCustomDropTarget;
	bool						m_bDragAccept;
	bool						m_bExternalDrag;
	CComPtr<IDropTargetHelper>	m_spDropTargetHelper;
	DWORD						m_dwDLControlFlags;
	DWORD		m_dwAutoRefreshStyle;
	DWORD		m_dwExStyle;
	DWORD		m_dwCurrentThreadId;

	CChildFrameUIStateChange&	m_UIChange;
	GlobalConfig*				m_pGlobalConfig;
public:
	bool	m_bLightRefresh;
};




