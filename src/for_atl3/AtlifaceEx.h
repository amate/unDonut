

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Fri Aug 05 15:22:58 2005
 */
/* Compiler settings for AtlifaceEx.idl:
	Oicf, W1, Zp8, env=Win32 (32b run)
	protocol : dce , ms_ext, c_ext, robust
	error checks: allocation ref bounds_check enum stub_data
	VC __declspec() decoration level:
		 __declspec(uuid()), __declspec(selectany), __declspec(novtable)
		 DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(	)

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AtlifaceEx_h__
#define __AtlifaceEx_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __IAxWinAmbientDispatchEx_UNDONUT_FWD_DEFINED__
#define __IAxWinAmbientDispatchEx_UNDONUT_FWD_DEFINED__
typedef interface IAxWinAmbientDispatchEx_UNDONUT IAxWinAmbientDispatchEx_UNDONUT;
#endif	/* __IAxWinAmbientDispatchEx_UNDONUT_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * );

/* interface __MIDL_itf_AtlifaceEx_0000 */
/* [local] */

EXTERN_C const CLSID CLSID_Registrar;


extern RPC_IF_HANDLE __MIDL_itf_AtlifaceEx_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_AtlifaceEx_0000_v0_0_s_ifspec;

#ifndef __IAxWinAmbientDispatchEx_UNDONUT_INTERFACE_DEFINED__
#define __IAxWinAmbientDispatchEx_UNDONUT_INTERFACE_DEFINED__

/* interface IAxWinAmbientDispatchEx_UNDONUT */
/* [unique][helpstring][uuid][dual][object] */




EXTERN_C const IID	IID_IAxWinAmbientDispatchEx_UNDONUT;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("359E0200-54D2-11d4-8A9C-ABAEC73C4C73")
	IAxWinAmbientDispatchEx_UNDONUT : public IDispatch
	{
	public:
		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AllowWindowlessActivation(
			/* [in] */ VARIANT_BOOL bCanWindowlessActivate) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AllowWindowlessActivation(
			/* [retval][out] */ VARIANT_BOOL *pbCanWindowlessActivate) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor(
			/* [in] */ OLE_COLOR clrBackground) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor(
			/* [retval][out] */ OLE_COLOR *pclrBackground) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ForeColor(
			/* [in] */ OLE_COLOR clrForeground) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ForeColor(
			/* [retval][out] */ OLE_COLOR *pclrForeground) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LocaleID(
			/* [in] */ LCID lcidLocaleID) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LocaleID(
			/* [retval][out] */ LCID *plcidLocaleID) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UserMode(
			/* [in] */ VARIANT_BOOL bUserMode) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserMode(
			/* [retval][out] */ VARIANT_BOOL *pbUserMode) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DisplayAsDefault(
			/* [in] */ VARIANT_BOOL bDisplayAsDefault) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayAsDefault(
			/* [retval][out] */ VARIANT_BOOL *pbDisplayAsDefault) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Font(
			/* [in] */ IFontDisp *pFont) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Font(
			/* [retval][out] */ IFontDisp **pFont) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MessageReflect(
			/* [in] */ VARIANT_BOOL bMsgReflect) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MessageReflect(
			/* [retval][out] */ VARIANT_BOOL *pbMsgReflect) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShowGrabHandles(
			VARIANT_BOOL *pbShowGrabHandles) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShowHatching(
			VARIANT_BOOL *pbShowHatching) = 0;

		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DocHostFlags(
			/* [in] */ DWORD dwDocHostFlags) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DocHostFlags(
			/* [retval][out] */ DWORD *pdwDocHostFlags) = 0;

		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DocHostDoubleClickFlags(
			/* [in] */ DWORD dwDocHostDoubleClickFlags) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DocHostDoubleClickFlags(
			/* [retval][out] */ DWORD *pdwDocHostDoubleClickFlags) = 0;

		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AllowContextMenu(
			/* [in] */ VARIANT_BOOL bAllowContextMenu) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AllowContextMenu(
			/* [retval][out] */ VARIANT_BOOL *pbAllowContextMenu) = 0;

		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AllowShowUI(
			/* [in] */ VARIANT_BOOL bAllowShowUI) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AllowShowUI(
			/* [retval][out] */ VARIANT_BOOL *pbAllowShowUI) = 0;

		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_OptionKeyPath(
			/* [in] */ BSTR bstrOptionKeyPath) = 0;

		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_OptionKeyPath(
			/* [retval][out] */ BSTR *pbstrOptionKeyPath) = 0;

		virtual /* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DLControlFlags(
			/* [in] */ DWORD dwDLControltFlags) = 0;

		virtual /* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DLControlFlags(
			/* [retval][out] */ DWORD *dwDLControltFlags) = 0;

		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetIeMenuNoCstm(
			int nStatus) = 0;

	};

#else	/* C style interface */

	typedef struct IAxWinAmbientDispatchEx_UNDONUTVtbl
	{
		BEGIN_INTERFACE

		HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ void **ppvObject);

		ULONG ( STDMETHODCALLTYPE *AddRef )(
			IAxWinAmbientDispatchEx_UNDONUT * This);

		ULONG ( STDMETHODCALLTYPE *Release )(
			IAxWinAmbientDispatchEx_UNDONUT * This);

		HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [out] */ UINT *pctinfo);

		HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ UINT iTInfo,
			/* [in] */ LCID lcid,
			/* [out] */ ITypeInfo **ppTInfo);

		HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ REFIID riid,
			/* [size_is][in] */ LPOLESTR *rgszNames,
			/* [in] */ UINT cNames,
			/* [in] */ LCID lcid,
			/* [size_is][out] */ DISPID *rgDispId);

		/* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ DISPID dispIdMember,
			/* [in] */ REFIID riid,
			/* [in] */ LCID lcid,
			/* [in] */ WORD wFlags,
			/* [out][in] */ DISPPARAMS *pDispParams,
			/* [out] */ VARIANT *pVarResult,
			/* [out] */ EXCEPINFO *pExcepInfo,
			/* [out] */ UINT *puArgErr);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AllowWindowlessActivation )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bCanWindowlessActivate);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AllowWindowlessActivation )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbCanWindowlessActivate);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ OLE_COLOR clrBackground);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ OLE_COLOR *pclrBackground);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ForeColor )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ OLE_COLOR clrForeground);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForeColor )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ OLE_COLOR *pclrForeground);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LocaleID )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ LCID lcidLocaleID);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LocaleID )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ LCID *plcidLocaleID);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserMode )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bUserMode);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserMode )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbUserMode);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DisplayAsDefault )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bDisplayAsDefault);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DisplayAsDefault )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbDisplayAsDefault);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Font )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ IFontDisp *pFont);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Font )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ IFontDisp **pFont);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MessageReflect )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bMsgReflect);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MessageReflect )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbMsgReflect);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowGrabHandles )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			VARIANT_BOOL *pbShowGrabHandles);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowHatching )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			VARIANT_BOOL *pbShowHatching);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DocHostFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ DWORD dwDocHostFlags);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DocHostFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ DWORD *pdwDocHostFlags);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DocHostDoubleClickFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ DWORD dwDocHostDoubleClickFlags);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DocHostDoubleClickFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ DWORD *pdwDocHostDoubleClickFlags);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AllowContextMenu )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bAllowContextMenu);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AllowContextMenu )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbAllowContextMenu);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AllowShowUI )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ VARIANT_BOOL bAllowShowUI);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AllowShowUI )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ VARIANT_BOOL *pbAllowShowUI);

		/* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OptionKeyPath )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ BSTR bstrOptionKeyPath);

		/* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OptionKeyPath )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ BSTR *pbstrOptionKeyPath);

		/* [id][helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DLControlFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [in] */ DWORD dwDLControltFlags);

		/* [id][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DLControlFlags )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			/* [retval][out] */ DWORD *dwDLControltFlags);

		/* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetIeMenuNoCstm )(
			IAxWinAmbientDispatchEx_UNDONUT * This,
			int nStatus);

		END_INTERFACE
	} IAxWinAmbientDispatchEx_UNDONUTVtbl;

	interface IAxWinAmbientDispatchEx_UNDONUT
	{
		CONST_VTBL struct IAxWinAmbientDispatchEx_UNDONUTVtbl *lpVtbl;
	};



#ifdef COBJMACROS


#define IAxWinAmbientDispatchEx_UNDONUT_QueryInterface(This,riid,ppvObject) \
	(This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAxWinAmbientDispatchEx_UNDONUT_AddRef(This)	\
	(This)->lpVtbl -> AddRef(This)

#define IAxWinAmbientDispatchEx_UNDONUT_Release(This)	\
	(This)->lpVtbl -> Release(This)


#define IAxWinAmbientDispatchEx_UNDONUT_GetTypeInfoCount(This,pctinfo)	\
	(This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAxWinAmbientDispatchEx_UNDONUT_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
	(This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAxWinAmbientDispatchEx_UNDONUT_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) \
	(This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAxWinAmbientDispatchEx_UNDONUT_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
	(This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAxWinAmbientDispatchEx_UNDONUT_put_AllowWindowlessActivation(This,bCanWindowlessActivate)	\
	(This)->lpVtbl -> put_AllowWindowlessActivation(This,bCanWindowlessActivate)

#define IAxWinAmbientDispatchEx_UNDONUT_get_AllowWindowlessActivation(This,pbCanWindowlessActivate) \
	(This)->lpVtbl -> get_AllowWindowlessActivation(This,pbCanWindowlessActivate)

#define IAxWinAmbientDispatchEx_UNDONUT_put_BackColor(This,clrBackground)	\
	(This)->lpVtbl -> put_BackColor(This,clrBackground)

#define IAxWinAmbientDispatchEx_UNDONUT_get_BackColor(This,pclrBackground)	\
	(This)->lpVtbl -> get_BackColor(This,pclrBackground)

#define IAxWinAmbientDispatchEx_UNDONUT_put_ForeColor(This,clrForeground)	\
	(This)->lpVtbl -> put_ForeColor(This,clrForeground)

#define IAxWinAmbientDispatchEx_UNDONUT_get_ForeColor(This,pclrForeground)	\
	(This)->lpVtbl -> get_ForeColor(This,pclrForeground)

#define IAxWinAmbientDispatchEx_UNDONUT_put_LocaleID(This,lcidLocaleID) \
	(This)->lpVtbl -> put_LocaleID(This,lcidLocaleID)

#define IAxWinAmbientDispatchEx_UNDONUT_get_LocaleID(This,plcidLocaleID)	\
	(This)->lpVtbl -> get_LocaleID(This,plcidLocaleID)

#define IAxWinAmbientDispatchEx_UNDONUT_put_UserMode(This,bUserMode)	\
	(This)->lpVtbl -> put_UserMode(This,bUserMode)

#define IAxWinAmbientDispatchEx_UNDONUT_get_UserMode(This,pbUserMode)	\
	(This)->lpVtbl -> get_UserMode(This,pbUserMode)

#define IAxWinAmbientDispatchEx_UNDONUT_put_DisplayAsDefault(This,bDisplayAsDefault)	\
	(This)->lpVtbl -> put_DisplayAsDefault(This,bDisplayAsDefault)

#define IAxWinAmbientDispatchEx_UNDONUT_get_DisplayAsDefault(This,pbDisplayAsDefault)	\
	(This)->lpVtbl -> get_DisplayAsDefault(This,pbDisplayAsDefault)

#define IAxWinAmbientDispatchEx_UNDONUT_put_Font(This,pFont)	\
	(This)->lpVtbl -> put_Font(This,pFont)

#define IAxWinAmbientDispatchEx_UNDONUT_get_Font(This,pFont)	\
	(This)->lpVtbl -> get_Font(This,pFont)

#define IAxWinAmbientDispatchEx_UNDONUT_put_MessageReflect(This,bMsgReflect)	\
	(This)->lpVtbl -> put_MessageReflect(This,bMsgReflect)

#define IAxWinAmbientDispatchEx_UNDONUT_get_MessageReflect(This,pbMsgReflect)	\
	(This)->lpVtbl -> get_MessageReflect(This,pbMsgReflect)

#define IAxWinAmbientDispatchEx_UNDONUT_get_ShowGrabHandles(This,pbShowGrabHandles) \
	(This)->lpVtbl -> get_ShowGrabHandles(This,pbShowGrabHandles)

#define IAxWinAmbientDispatchEx_UNDONUT_get_ShowHatching(This,pbShowHatching)	\
	(This)->lpVtbl -> get_ShowHatching(This,pbShowHatching)

#define IAxWinAmbientDispatchEx_UNDONUT_put_DocHostFlags(This,dwDocHostFlags)	\
	(This)->lpVtbl -> put_DocHostFlags(This,dwDocHostFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_get_DocHostFlags(This,pdwDocHostFlags)	\
	(This)->lpVtbl -> get_DocHostFlags(This,pdwDocHostFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_put_DocHostDoubleClickFlags(This,dwDocHostDoubleClickFlags) \
	(This)->lpVtbl -> put_DocHostDoubleClickFlags(This,dwDocHostDoubleClickFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_get_DocHostDoubleClickFlags(This,pdwDocHostDoubleClickFlags)	\
	(This)->lpVtbl -> get_DocHostDoubleClickFlags(This,pdwDocHostDoubleClickFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_put_AllowContextMenu(This,bAllowContextMenu)	\
	(This)->lpVtbl -> put_AllowContextMenu(This,bAllowContextMenu)

#define IAxWinAmbientDispatchEx_UNDONUT_get_AllowContextMenu(This,pbAllowContextMenu)	\
	(This)->lpVtbl -> get_AllowContextMenu(This,pbAllowContextMenu)

#define IAxWinAmbientDispatchEx_UNDONUT_put_AllowShowUI(This,bAllowShowUI)	\
	(This)->lpVtbl -> put_AllowShowUI(This,bAllowShowUI)

#define IAxWinAmbientDispatchEx_UNDONUT_get_AllowShowUI(This,pbAllowShowUI) \
	(This)->lpVtbl -> get_AllowShowUI(This,pbAllowShowUI)

#define IAxWinAmbientDispatchEx_UNDONUT_put_OptionKeyPath(This,bstrOptionKeyPath)	\
	(This)->lpVtbl -> put_OptionKeyPath(This,bstrOptionKeyPath)

#define IAxWinAmbientDispatchEx_UNDONUT_get_OptionKeyPath(This,pbstrOptionKeyPath)	\
	(This)->lpVtbl -> get_OptionKeyPath(This,pbstrOptionKeyPath)

#define IAxWinAmbientDispatchEx_UNDONUT_put_DLControlFlags(This,dwDLControltFlags)	\
	(This)->lpVtbl -> put_DLControlFlags(This,dwDLControltFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_get_DLControlFlags(This,dwDLControltFlags)	\
	(This)->lpVtbl -> get_DLControlFlags(This,dwDLControltFlags)

#define IAxWinAmbientDispatchEx_UNDONUT_SetIeMenuNoCstm(This,nStatus)	\
	(This)->lpVtbl -> SetIeMenuNoCstm(This,nStatus)

#endif /* COBJMACROS */


#endif	/* C style interface */



/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_AllowWindowlessActivation_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bCanWindowlessActivate);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_AllowWindowlessActivation_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_AllowWindowlessActivation_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbCanWindowlessActivate);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_AllowWindowlessActivation_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_BackColor_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ OLE_COLOR clrBackground);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_BackColor_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_BackColor_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ OLE_COLOR *pclrBackground);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_BackColor_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_ForeColor_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ OLE_COLOR clrForeground);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_ForeColor_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_ForeColor_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ OLE_COLOR *pclrForeground);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_ForeColor_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_LocaleID_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ LCID lcidLocaleID);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_LocaleID_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_LocaleID_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ LCID *plcidLocaleID);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_LocaleID_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_UserMode_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bUserMode);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_UserMode_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_UserMode_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbUserMode);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_UserMode_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_DisplayAsDefault_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bDisplayAsDefault);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_DisplayAsDefault_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_DisplayAsDefault_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbDisplayAsDefault);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_DisplayAsDefault_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_Font_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ IFontDisp *pFont);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_Font_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_Font_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ IFontDisp **pFont);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_Font_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_MessageReflect_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bMsgReflect);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_MessageReflect_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_MessageReflect_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbMsgReflect);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_MessageReflect_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_ShowGrabHandles_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	VARIANT_BOOL *pbShowGrabHandles);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_ShowGrabHandles_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_ShowHatching_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	VARIANT_BOOL *pbShowHatching);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_ShowHatching_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_DocHostFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ DWORD dwDocHostFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_DocHostFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_DocHostFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ DWORD *pdwDocHostFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_DocHostFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_DocHostDoubleClickFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ DWORD dwDocHostDoubleClickFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_DocHostDoubleClickFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_DocHostDoubleClickFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ DWORD *pdwDocHostDoubleClickFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_DocHostDoubleClickFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_AllowContextMenu_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bAllowContextMenu);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_AllowContextMenu_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_AllowContextMenu_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbAllowContextMenu);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_AllowContextMenu_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_AllowShowUI_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ VARIANT_BOOL bAllowShowUI);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_AllowShowUI_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_AllowShowUI_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ VARIANT_BOOL *pbAllowShowUI);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_AllowShowUI_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_OptionKeyPath_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ BSTR bstrOptionKeyPath);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_OptionKeyPath_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_OptionKeyPath_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ BSTR *pbstrOptionKeyPath);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_OptionKeyPath_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propput] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_put_DLControlFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [in] */ DWORD dwDLControltFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_put_DLControlFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [id][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_get_DLControlFlags_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	/* [retval][out] */ DWORD *dwDLControltFlags);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_get_DLControlFlags_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAxWinAmbientDispatchEx_UNDONUT_SetIeMenuNoCstm_Proxy(
	IAxWinAmbientDispatchEx_UNDONUT * This,
	int nStatus);


void __RPC_STUB IAxWinAmbientDispatchEx_UNDONUT_SetIeMenuNoCstm_Stub(
	IRpcStubBuffer *This,
	IRpcChannelBuffer *_pRpcChannelBuffer,
	PRPC_MESSAGE _pRpcMessage,
	DWORD *_pdwStubPhase);



#endif	/* __IAxWinAmbientDispatchEx_UNDONUT_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long			  __RPC_USER  BSTR_UserSize(	 unsigned long *, unsigned long 		   , BSTR * );
unsigned char * __RPC_USER	BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * );
unsigned char * __RPC_USER	BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * );
void					  __RPC_USER  BSTR_UserFree(	 unsigned long *, BSTR * );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


