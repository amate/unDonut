/**
 *	@file	DonutView.h
 *	@brief	View.
 */

#pragma once

#include "DonutViewOption.h"
#include "DocHostUIHandlerDispatch.h"
#include "MtlBrowser.h"
#include "HLinkDataObject.h"


//////////////////////////////////////////////////////////////////////
// CDonutView

class CDonutView :
	public CWindowImpl<CDonutView, CAxWindow>,
	public CWebBrowser2,
	public IServiceProvider,
	public IDropTarget,
	public IScriptErrorCommandTargetImpl<CDonutView>,
	public IDispatch
{
public:
	// Declaration
	DECLARE_WND_SUPERCLASS( NULL, CAxWindow::GetWndClassName() )
	
	// Constructor
	CDonutView(DWORD dwDefaultDLControlFlags, DWORD dwDefaultExtendedStyleFlags);

	// Methods
	DWORD	GetDLControlFlags() const { return m_dwDLControlFlags; }
	void	PutDLControlFlags(DWORD dwDLControlFlags);
	void	SetIeMenuNoCstm(int nStatus);

	//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
	void	SetOperateDragDrop(BOOL bOn, int nCommand);
	void	SetDefaultDropTarget(IDropTarget* pTarget) { m_spDefaultDropTarget = pTarget; }

	// Overrides
	BOOL	PreTranslateMessage(MSG *pMsg);

	DWORD	_GetDLControlFlags();
	DWORD	_GetExtendedStypeFlags();

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


	// Message map and handlers
	BEGIN_MSG_MAP( CDonutView )
		MSG_WM_CREATE	( OnCreate )
		MSG_WM_DESTROY	( OnDestroy )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_BGSOUNDS		, OnMultiBgsounds		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_VIDEOS			, OnMultiVideos 		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_DLIMAGES		, OnMultiDlImages		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_RUNACTIVEXCTLS	, OnSecurRunactivexctls )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_DLACTIVEXCTLS	, OnSecurDlactivexctls	)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_SCRIPTS 		, OnSecurScritps		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_JAVA			, OnSecurJava			)
		CHAIN_MSG_MAP_MEMBER( m_ViewOption )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_MULTI		, OnMultiChg			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_SECU		, OnSecuChg 			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_MULTI	, OnAllOnOff			)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_SECU 	, OnAllOnOff			)
	END_MSG_MAP()


	BEGIN_UPDATE_COMMAND_UI_MAP( CDonutView )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ViewOption )
		UPDATE_COMMAND_UI( ID_DLCTL_DLIMAGES	  , OnUpdateDLCTL_DLIMAGES		 )		// with popup
		UPDATE_COMMAND_UI( ID_DLCTL_RUNACTIVEXCTLS, OnUpdateDLCTL_RUNACTIVEXCTLS )		// with popup

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_BGSOUNDS, DLCTL_BGSOUNDS, GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_VIDEOS, DLCTL_VIDEOS	 , GetDLControlFlags() )

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_DLACTIVEXCTLS, DLCTL_NO_DLACTIVEXCTLS, GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_SCRIPTS	  , DLCTL_NO_SCRIPTS	  , GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG_REV( ID_DLCTL_JAVA		  , DLCTL_NO_JAVA		  , GetDLControlFlags() )

		// UH
		UPDATE_COMMAND_UI( ID_DLCTL_CHG_MULTI, OnUpdateDLCTL_ChgMulti ) 				// with popup
		UPDATE_COMMAND_UI( ID_DLCTL_CHG_SECU , OnUpdateDLCTL_ChgSecu  ) 				// with popup

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_MULTI,
				( ( GetDLControlFlags() & (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS) )
				  == (DLCTL_DLIMAGES | DLCTL_BGSOUNDS | DLCTL_VIDEOS)
				) /*+++ ? 1 : 0*/ )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_SECU,
				( ( GetDLControlFlags() & (DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA)) == 0 )
				/*+++ ? 1 : 0*/ )
	END_UPDATE_COMMAND_UI_MAP()


private:

	// UDT DGSTR
	void	OnMultiChg(WORD, WORD, HWND);
	void	OnSecuChg(WORD, WORD, HWND);
	void	OnAllOnOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();

	void	OnMultiBgsounds(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiVideos(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiDlImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurRunactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurDlactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurScritps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurJava(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void		OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_DLIMAGES(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_RUNACTIVEXCTLS(CCmdUI *pCmdUI);
	void		OnUpdateDocHostUIOpenNewWinUI(CCmdUI *pCmdUI);


private:
	void	_InitDLControlFlags() { PutDLControlFlags(m_dwDefaultDLControlFlags); }
	bool	_ToggleFlag(WORD wID, DWORD dwFlag, BOOL bReverse = FALSE);
	void	_AddFlag(DWORD dwFlag);
	void	_RemoveFlag(DWORD dwFlag);
	void	_LightRefresh();

	// Data members
	CComQIPtr<IAxWinHostWindow>					m_spHost;
	CComQIPtr<IAxWinAmbientDispatchEx>			m_spAxAmbient;

	DWORD						m_dwDefaultDLControlFlags;
	//DWORD						m_dwDefaultExtendedStyleFlags;	//+++

	CDocHostUIHandlerDispatch	m_ExternalUIDispatch;
	//CAmbientDispatch			m_ExternalAmbientDispatch;

	int 						m_nDDCommand;
	CComPtr<IDropTarget>		m_spDefaultDropTarget;
	bool						m_bUseCustomDropTarget;
	bool						m_bTempUseDefaultDropTarget;

	DWORD						m_dwDLControlFlags;

public:
	CDonutViewOption<CDonutView>		m_ViewOption;
	bool	m_bLightRefresh;
};




