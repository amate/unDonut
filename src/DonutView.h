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
	public IDispatchImpl<IDispatch>
{
public:
	// Declaration
	DECLARE_WND_SUPERCLASS( NULL, CAxWindow::GetWndClassName() )

	// Constants
	enum {
	  #if 1	//+++ xp未満ではフラットスクロールバーを設定、xp以後ではthemeを適用.
		DOCHOSTUIFLAG_FLATVIEW		= (DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
		DOCHOSTUIFLAG_THEME_VIEW    = (DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
		DOCHOSTUIFLAG_NOT_FLATVIEW	= ( /*DOCHOSTUIFLAG_NO3DBORDER*/ DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),
	  #elif 1	// unDonut+
		DOCHOSTUIFLAG_FLATVIEW		= (DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
		DOCHOSTUIFLAG_NOT_FLATVIEW	= ( /*DOCHOSTUIFLAG_NO3DBORDER*/ DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),
	  #else
		//docHostUIFlagDEFAULT		= (docHostUIFlagFLAT_SCROLLBAR | docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE |DOCHOSTUIFLAG_THEME),
		//docHostUIFlagNotFlatView	= (docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE),		// UDT DGSTR ( added by dai
		// DonutRAPT(1.26)の値.
		//docHostUIFlagDEFAULT 		= (docHostUIFlagFLAT_SCROLLBAR | docHostUIFlagNO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
		//docHostUIFlagNotFlatScrBar= (docHostUIFlagNO3DBORDER     | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME),
	  #endif
	};
	CDonutViewOption<CDonutView>		m_ViewOption;
	

public:
	// Constructor
	CDonutView(DWORD dwDefaultDLControlFlags, DWORD dwDefaultExtendedStyleFlags);

	// Methods
	DWORD	GetDLControlFlags();
	void	PutDLControlFlags(DWORD dwDLControlFlags);
	void	SetIeMenuNoCstm(int nStatus);

	//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
	void	SetOperateDragDrop(BOOL bOn, int nCommand);
	void	SetDefaultDropTarget(IDropTarget* pTarget) { m_spDefaultDropTarget = pTarget; }

	// Overrides
	BOOL	PreTranslateMessage(MSG *pMsg);

	DWORD	_GetDLControlFlags();
	DWORD	_GetExtendedStypeFlags();

	//bool	OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);

	void	InitDLControlFlags();

	void	put_DLControlFlags(DWORD dwFlags);
	void	get_DLControlFlags(DWORD* pdwFlags);

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
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);


	// Message map and handlers
	BEGIN_MSG_MAP( CDonutView )
		MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MSG_WM_DESTROY( OnDestroy )
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
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &);
	void	OnDestroy();

	void	OnMultiBgsounds(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiVideos(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMultiDlImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurRunactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurDlactivexctls(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurScritps(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurJava(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// Implementation
	//protected:
	void	_OnFlag(DWORD dwFlag);
	void	_OffFlag(DWORD dwFlag);
	bool	_ToggleFlag(WORD wID, DWORD dwFlag, BOOL bReverse = FALSE);
	void	_AddFlag(DWORD dwFlag);
	void	_RemoveFlag(DWORD dwFlag);
	void	_LightRefresh();

	void		OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_DLIMAGES(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_RUNACTIVEXCTLS(CCmdUI *pCmdUI);
	void		OnUpdateDocHostUIOpenNewWinUI(CCmdUI *pCmdUI);

	// Overrides
	//STDMETHOD	(GetHostInfo) (DWORD  * pdwFlags, DWORD  * pdwDoubleClick);
	//STDMETHOD	(GetDropTarget) (IUnknown	* pDropTarget, IUnknown  * *ppDropTarget);
	void		_DrawDragEffect(bool bRemove);

private:
	// Data members
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

};




