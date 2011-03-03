/**
 *	@file	DocHostUIHandlerDispatch.h
 *	@brief	主に右クリックメニューのカスタム処理を行えるようにするためのクラス
 */
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <mshtmdid.h>

#include "MtlDragDrop.h"
#include "CustomContextMenu.h"
#include "ScriptErrorCommandTargetImpl.h"

// 前方宣言
class CDonutView;

/*
	CDocHostUIHandlerDispatch
	主に右クリックメニューのカスタム処理を行えるようにするためのクラス
	IEコンポーネントのUI処理を置き換えるIDocHostUIHandlerDispatchを実装する
	インスタンスを作成します。ホストウィンドウからクエリーしたIAxWinHostWindowを
	SetDefaultUIHandlerで設定します。続いて今度は逆にインスタンスのポインタを
	IAxWinHostWindow::SetExternalUIHandlerで設定します。

	もともとはatlhost.hを直接書き換えたatlhostEx.hを使うようになっていたのですが、
	ATL7になってからヘッダの依存関係が変わったのか、この方法が使いにくくなったため、
	書き換えていた部分を外部に移しIDocHostUIHandlerDispatchの実装としました。

	このクラスは本当に既存の処理を置き換えてしまうように定義されています(atlhost.h参照)
	しかし実際必要な関数はShowContextMenuだけで、それ以外はデフォルト動作するように
	なっていて欲しいのですが、なっていません。
	大体のメソッドは適当な戻り値を返せばデフォルトと同じ動作をしますが、
	そうでない幾つかのメソッドに対応するためにデフォルトのUIハンドラを所持し、
	一時的に当クラスを解除してデフォルト動作を行わせます(GetHostInfo,GetOptionKeyPath等参照)
 */
class CDocHostUIHandlerDispatch : 
	public IDocHostUIHandlerDispatch,
	public IScriptErrorCommandTargetImpl<CDocHostUIHandlerDispatch>
{
public: 
	// Constructor
	CDocHostUIHandlerDispatch(CDonutView* pView);

	void SetIEContextMenuCustom(bool bCustom) { m_bNoIECustom = !bCustom; }
	bool GetIEContextMenuCustom() { return !m_bNoIECustom; }

	void SetDefaultUIHandler(const CComPtr<IDocHostUIHandler> &pDefaultHandler) { m_pDefaultHandler = pDefaultHandler; }


public: 	
	// IUnknown atlhost.hを見る限り必要ではない
	STDMETHODIMP			QueryInterface (REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) 	AddRef()	{ return 1; }
	STDMETHODIMP_(ULONG) 	Release()	{ return 1; }
#if 0
	// IDocHostUIHandler
    STDMETHODIMP ShowContextMenu( 
        /* [in] */ DWORD dwID,
        /* [in] */ POINT *ppt,
        /* [in] */ IUnknown *pcmdtReserved,
        /* [in] */ IDispatch *pdispReserved);
        
    STDMETHODIMP GetHostInfo( 
        /* [out][in] */ DOCHOSTUIINFO *pInfo);
        
    STDMETHODIMP ShowUI( 
        /* [in] */ DWORD dwID,
        /* [in] */ IOleInPlaceActiveObject *pActiveObject,
        /* [in] */ IOleCommandTarget *pCommandTarget,
        /* [in] */ IOleInPlaceFrame *pFrame,
        /* [in] */ IOleInPlaceUIWindow *pDoc) { return S_OK; }
        
    STDMETHODIMP HideUI( void) { return S_OK; }
        
    STDMETHODIMP UpdateUI( void) { return S_OK; }
        
    STDMETHODIMP EnableModeless( 
        /* [in] */ BOOL fEnable) { return E_NOTIMPL; }
        
    STDMETHODIMP OnDocWindowActivate( 
        /* [in] */ BOOL fActivate) { return E_NOTIMPL; }
        
    STDMETHODIMP OnFrameWindowActivate( 
        /* [in] */ BOOL fActivate) { return E_NOTIMPL; }
        
    STDMETHODIMP ResizeBorder( 
        /* [in] */ LPCRECT prcBorder,
        /* [in] */ IOleInPlaceUIWindow *pUIWindow,
        /* [in] */ BOOL fRameWindow) { return E_NOTIMPL; }
        
    STDMETHODIMP TranslateAccelerator( 
        /* [in] */ LPMSG lpMsg,
        /* [in] */ const GUID *pguidCmdGroup,
        /* [in] */ DWORD nCmdID) { return S_FALSE; }
        
    STDMETHODIMP GetOptionKeyPath( 
        /* [out] */ 
        __out  LPOLESTR *pchKey,
        /* [in] */ DWORD dw) { return E_NOTIMPL; }
        
    STDMETHODIMP GetDropTarget( 
        /* [in] */ IDropTarget *pDropTarget,
        /* [out] */ IDropTarget **ppDropTarget);
        
    STDMETHODIMP GetExternal( 
        /* [out] */ IDispatch **ppDispatch) { return E_NOTIMPL; }
        
    STDMETHODIMP TranslateUrl( 
        /* [in] */ DWORD dwTranslate,
        /* [in] */ 
        __in __nullterminated  OLECHAR *pchURLIn,
        /* [out] */ 
        __out  OLECHAR **ppchURLOut) { return S_FALSE; }
        
    STDMETHODIMP FilterDataObject( 
        /* [in] */ IDataObject *pDO,
        /* [out] */ IDataObject **ppDORet) { return S_FALSE; }
#endif
private:
	bool					   m_bNoIECustom;
	CComPtr<IDocHostUIHandler> m_pDefaultHandler;
	CDonutView*				   m_pView;

#if 1
public: 	// IDispatch atlhost.hを見る限り必要ではない

	STDMETHOD	(GetTypeInfoCount) (UINT * pctinfo) { return E_NOTIMPL; }
	STDMETHOD	(GetTypeInfo) (UINT itinfo, LCID lcid, ITypeInfo * *pptinfo) { return E_NOTIMPL; }
	STDMETHOD	(GetIDsOfNames) (REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid)	{ return E_NOTIMPL; }

	STDMETHOD	(Invoke) (
			DISPID			dispidMember,
			REFIID			riid,
			LCID			lcid,
			WORD			/*wFlags*/,
			DISPPARAMS *	pdispparams,
			VARIANT *		pvarResult,
			EXCEPINFO * 	/*pexcepinfo*/,
			UINT *			/*puArgErr*/) { return E_NOTIMPL; }


public: 	
	// IDocHostUIHandlerDispatch
	//このインターフェイスは既存の動作を置き換える

	//カスタムコンテキストメニュー
	STDMETHOD(ShowContextMenu) (
		/* [in] */ DWORD				dwID,
		/* [in] */ DWORD				x,
		/* [in] */ DWORD				y,
		/* [in] */ IUnknown*			pcmdtReserved,
		/* [in] */ IDispatch*			pdispReserved,
		/* [retval][out] */ HRESULT*	dwRetVal);
	STDMETHOD(GetHostInfo) (
		/* [out][in] */ DWORD  * pdwFlags,
		/* [out][in] */ DWORD  * pdwDoubleClick);
	STDMETHOD(ShowUI) (
		/* [in] */ DWORD			 dwID,
		/* [in] */ IUnknown  *		 pActiveObject,
		/* [in] */ IUnknown  *		 pCommandTarget,
		/* [in] */ IUnknown  *		 pFrame,
		/* [in] */ IUnknown  *		 pDoc,
		/* [retval][out] */ HRESULT* dwRetVal) { return S_OK; }
	STDMETHOD(HideUI) (void) { return S_OK; }
	STDMETHOD(UpdateUI) (void) { return S_OK; }
	STDMETHOD(EnableModeless) (/* [in] */ VARIANT_BOOL fEnable) { return S_OK; }
	STDMETHOD(OnDocWindowActivate) ( /* [in] */ VARIANT_BOOL fActivate) { return S_OK; }
	STDMETHOD(OnFrameWindowActivate) ( /* [in] */ VARIANT_BOOL fActivate) { return S_OK; }
	STDMETHOD(ResizeBorder) (
		/* [in] */ long 			left,
		/* [in] */ long 			top,
		/* [in] */ long 			right,
		/* [in] */ long 			bottom,
		/* [in] */ IUnknown  *		pUIWindow,
		/* [in] */ VARIANT_BOOL 	fFrameWindow) { return S_OK; }
	STDMETHOD(TranslateAccelerator) (
		/* [in] */ DWORD_PTR		hWnd,
		/* [in] */ DWORD			nMessage,
		/* [in] */ DWORD_PTR		wParam,
		/* [in] */ DWORD_PTR		lParam,
		/* [in] */ BSTR 			bstrGuidCmdGroup,
		/* [in] */ DWORD			nCmdID,
		/* [retval][out] */HRESULT* dwRetVal);
	STDMETHOD(GetOptionKeyPath) (/* [out] */ BSTR* pbstrKey, /* [in] */ DWORD dw) { return E_NOTIMPL; }
	STDMETHOD(GetDropTarget) (/* [in] */ IUnknown* pDropTarget, /* [out] */ IUnknown** ppDropTarget);
	STDMETHOD(GetExternal) ( /* [out] */ IDispatch** ppDispatch) { return E_NOTIMPL; }
	STDMETHOD(TranslateUrl) (
		/* [in] */ DWORD		dwTranslate,
		/* [in] */ BSTR 		bstrURLIn,
		/* [out] */ BSTR  * 	pbstrURLOut) { return E_NOTIMPL; }
	STDMETHOD(FilterDataObject)(
		/* [in] */ IUnknown  *		pDO,
		/* [out] */ IUnknown  * *	ppDORet) { return E_NOTIMPL; }
#endif
};
