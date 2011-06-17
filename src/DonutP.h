

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Jun 18 02:53:55 2011
 */
/* Compiler settings for DonutP.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

#ifndef __DonutP_h__
#define __DonutP_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAPI_FWD_DEFINED__
#define __IAPI_FWD_DEFINED__
typedef interface IAPI IAPI;
#endif 	/* __IAPI_FWD_DEFINED__ */


#ifndef __IAPI2_FWD_DEFINED__
#define __IAPI2_FWD_DEFINED__
typedef interface IAPI2 IAPI2;
#endif 	/* __IAPI2_FWD_DEFINED__ */


#ifndef __IAPI3_FWD_DEFINED__
#define __IAPI3_FWD_DEFINED__
typedef interface IAPI3 IAPI3;
#endif 	/* __IAPI3_FWD_DEFINED__ */


#ifndef __IAPI4_FWD_DEFINED__
#define __IAPI4_FWD_DEFINED__
typedef interface IAPI4 IAPI4;
#endif 	/* __IAPI4_FWD_DEFINED__ */


#ifndef __IDonutPEvents_FWD_DEFINED__
#define __IDonutPEvents_FWD_DEFINED__
typedef interface IDonutPEvents IDonutPEvents;
#endif 	/* __IDonutPEvents_FWD_DEFINED__ */


#ifndef __API_FWD_DEFINED__
#define __API_FWD_DEFINED__

#ifdef __cplusplus
typedef class API API;
#else
typedef struct API API;
#endif /* __cplusplus */

#endif 	/* __API_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IAPI_INTERFACE_DEFINED__
#define __IAPI_INTERFACE_DEFINED__

/* interface IAPI */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IAPI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1413C5B5-0352-47AA-85C6-9219F243F5E8")
    IAPI : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            int nIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDocumentObject( 
            int nIndex,
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWindowObject( 
            int nIndex,
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWebBrowserObject( 
            int nIndex,
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TabIndex( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TabIndex( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTabCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveToTab( 
            WORD wBefor,
            WORD wAfter) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NewWindow( 
            BSTR bstrURL,
            BOOL bActive,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MessageBox( 
            BSTR bstrText,
            BSTR bstrCaption,
            UINT uType,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowPanelBar( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPanelDocumentObject( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPanelWindowObject( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPanelWebBrowserObject( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTabState( 
            int nIndex,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAPIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAPI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAPI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAPI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAPI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAPI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAPI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAPI * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAPI * This,
            int nIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentObject )( 
            IAPI * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindowObject )( 
            IAPI * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWebBrowserObject )( 
            IAPI * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TabIndex )( 
            IAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TabIndex )( 
            IAPI * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabCount )( 
            IAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveToTab )( 
            IAPI * This,
            WORD wBefor,
            WORD wAfter);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow )( 
            IAPI * This,
            BSTR bstrURL,
            BOOL bActive,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MessageBox )( 
            IAPI * This,
            BSTR bstrText,
            BSTR bstrCaption,
            UINT uType,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowPanelBar )( 
            IAPI * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelDocumentObject )( 
            IAPI * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWindowObject )( 
            IAPI * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWebBrowserObject )( 
            IAPI * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabState )( 
            IAPI * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IAPIVtbl;

    interface IAPI
    {
        CONST_VTBL struct IAPIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAPI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAPI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAPI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAPI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAPI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAPI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAPI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAPI_Close(This,nIndex)	\
    ( (This)->lpVtbl -> Close(This,nIndex) ) 

#define IAPI_GetDocumentObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetDocumentObject(This,nIndex,pVal) ) 

#define IAPI_GetWindowObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWindowObject(This,nIndex,pVal) ) 

#define IAPI_GetWebBrowserObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWebBrowserObject(This,nIndex,pVal) ) 

#define IAPI_get_TabIndex(This,pVal)	\
    ( (This)->lpVtbl -> get_TabIndex(This,pVal) ) 

#define IAPI_put_TabIndex(This,newVal)	\
    ( (This)->lpVtbl -> put_TabIndex(This,newVal) ) 

#define IAPI_GetTabCount(This,pVal)	\
    ( (This)->lpVtbl -> GetTabCount(This,pVal) ) 

#define IAPI_MoveToTab(This,wBefor,wAfter)	\
    ( (This)->lpVtbl -> MoveToTab(This,wBefor,wAfter) ) 

#define IAPI_NewWindow(This,bstrURL,bActive,pVal)	\
    ( (This)->lpVtbl -> NewWindow(This,bstrURL,bActive,pVal) ) 

#define IAPI_MessageBox(This,bstrText,bstrCaption,uType,pVal)	\
    ( (This)->lpVtbl -> MessageBox(This,bstrText,bstrCaption,uType,pVal) ) 

#define IAPI_ShowPanelBar(This)	\
    ( (This)->lpVtbl -> ShowPanelBar(This) ) 

#define IAPI_GetPanelDocumentObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelDocumentObject(This,pVal) ) 

#define IAPI_GetPanelWindowObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWindowObject(This,pVal) ) 

#define IAPI_GetPanelWebBrowserObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWebBrowserObject(This,pVal) ) 

#define IAPI_GetTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetTabState(This,nIndex,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAPI_INTERFACE_DEFINED__ */


#ifndef __IAPI2_INTERFACE_DEFINED__
#define __IAPI2_INTERFACE_DEFINED__

/* interface IAPI2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IAPI2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A7B5128A-E147-41d2-8155-4B9BAE449DF4")
    IAPI2 : public IAPI
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteCommand( 
            int nCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSearchText( 
            /* [retval][out] */ BSTR *pbstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSearchText( 
            BSTR bstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAddressText( 
            /* [retval][out] */ BSTR *pbstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAddressText( 
            BSTR bstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExtendedTabState( 
            int nIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetExtendedTabState( 
            int nIndex,
            long nState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyState( 
            int nKey,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProfileInt( 
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nDefault,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteProfileInt( 
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProfileString( 
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrDefault,
            /* [retval][out] */ BSTR *pbstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteProfileString( 
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScriptFolder( 
            /* [retval][out] */ BSTR *pbstrFolder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCSSFolder( 
            /* [retval][out] */ BSTR *pbstrFolder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBaseFolder( 
            /* [retval][out] */ BSTR *pbstrFolder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExePath( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetStyleSheet( 
            int nIndex,
            BSTR bstrStyleSheet,
            BOOL bOff) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAPI2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAPI2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAPI2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAPI2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAPI2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAPI2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAPI2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAPI2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAPI2 * This,
            int nIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentObject )( 
            IAPI2 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindowObject )( 
            IAPI2 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWebBrowserObject )( 
            IAPI2 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TabIndex )( 
            IAPI2 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TabIndex )( 
            IAPI2 * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabCount )( 
            IAPI2 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveToTab )( 
            IAPI2 * This,
            WORD wBefor,
            WORD wAfter);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow )( 
            IAPI2 * This,
            BSTR bstrURL,
            BOOL bActive,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MessageBox )( 
            IAPI2 * This,
            BSTR bstrText,
            BSTR bstrCaption,
            UINT uType,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowPanelBar )( 
            IAPI2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelDocumentObject )( 
            IAPI2 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWindowObject )( 
            IAPI2 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWebBrowserObject )( 
            IAPI2 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabState )( 
            IAPI2 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteCommand )( 
            IAPI2 * This,
            int nCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchText )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSearchText )( 
            IAPI2 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAddressText )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAddressText )( 
            IAPI2 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExtendedTabState )( 
            IAPI2 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetExtendedTabState )( 
            IAPI2 * This,
            int nIndex,
            long nState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyState )( 
            IAPI2 * This,
            int nKey,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileInt )( 
            IAPI2 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nDefault,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileInt )( 
            IAPI2 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileString )( 
            IAPI2 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrDefault,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileString )( 
            IAPI2 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScriptFolder )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCSSFolder )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBaseFolder )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExePath )( 
            IAPI2 * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetStyleSheet )( 
            IAPI2 * This,
            int nIndex,
            BSTR bstrStyleSheet,
            BOOL bOff);
        
        END_INTERFACE
    } IAPI2Vtbl;

    interface IAPI2
    {
        CONST_VTBL struct IAPI2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAPI2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAPI2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAPI2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAPI2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAPI2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAPI2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAPI2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAPI2_Close(This,nIndex)	\
    ( (This)->lpVtbl -> Close(This,nIndex) ) 

#define IAPI2_GetDocumentObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetDocumentObject(This,nIndex,pVal) ) 

#define IAPI2_GetWindowObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWindowObject(This,nIndex,pVal) ) 

#define IAPI2_GetWebBrowserObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWebBrowserObject(This,nIndex,pVal) ) 

#define IAPI2_get_TabIndex(This,pVal)	\
    ( (This)->lpVtbl -> get_TabIndex(This,pVal) ) 

#define IAPI2_put_TabIndex(This,newVal)	\
    ( (This)->lpVtbl -> put_TabIndex(This,newVal) ) 

#define IAPI2_GetTabCount(This,pVal)	\
    ( (This)->lpVtbl -> GetTabCount(This,pVal) ) 

#define IAPI2_MoveToTab(This,wBefor,wAfter)	\
    ( (This)->lpVtbl -> MoveToTab(This,wBefor,wAfter) ) 

#define IAPI2_NewWindow(This,bstrURL,bActive,pVal)	\
    ( (This)->lpVtbl -> NewWindow(This,bstrURL,bActive,pVal) ) 

#define IAPI2_MessageBox(This,bstrText,bstrCaption,uType,pVal)	\
    ( (This)->lpVtbl -> MessageBox(This,bstrText,bstrCaption,uType,pVal) ) 

#define IAPI2_ShowPanelBar(This)	\
    ( (This)->lpVtbl -> ShowPanelBar(This) ) 

#define IAPI2_GetPanelDocumentObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelDocumentObject(This,pVal) ) 

#define IAPI2_GetPanelWindowObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWindowObject(This,pVal) ) 

#define IAPI2_GetPanelWebBrowserObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWebBrowserObject(This,pVal) ) 

#define IAPI2_GetTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetTabState(This,nIndex,pVal) ) 


#define IAPI2_ExecuteCommand(This,nCommand)	\
    ( (This)->lpVtbl -> ExecuteCommand(This,nCommand) ) 

#define IAPI2_GetSearchText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetSearchText(This,pbstrText) ) 

#define IAPI2_SetSearchText(This,bstrText)	\
    ( (This)->lpVtbl -> SetSearchText(This,bstrText) ) 

#define IAPI2_GetAddressText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetAddressText(This,pbstrText) ) 

#define IAPI2_SetAddressText(This,bstrText)	\
    ( (This)->lpVtbl -> SetAddressText(This,bstrText) ) 

#define IAPI2_GetExtendedTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetExtendedTabState(This,nIndex,pVal) ) 

#define IAPI2_SetExtendedTabState(This,nIndex,nState)	\
    ( (This)->lpVtbl -> SetExtendedTabState(This,nIndex,nState) ) 

#define IAPI2_GetKeyState(This,nKey,pVal)	\
    ( (This)->lpVtbl -> GetKeyState(This,nKey,pVal) ) 

#define IAPI2_GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal)	\
    ( (This)->lpVtbl -> GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal) ) 

#define IAPI2_WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue)	\
    ( (This)->lpVtbl -> WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue) ) 

#define IAPI2_GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText)	\
    ( (This)->lpVtbl -> GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText) ) 

#define IAPI2_WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText)	\
    ( (This)->lpVtbl -> WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText) ) 

#define IAPI2_GetScriptFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetScriptFolder(This,pbstrFolder) ) 

#define IAPI2_GetCSSFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetCSSFolder(This,pbstrFolder) ) 

#define IAPI2_GetBaseFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetBaseFolder(This,pbstrFolder) ) 

#define IAPI2_GetExePath(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetExePath(This,pbstrPath) ) 

#define IAPI2_SetStyleSheet(This,nIndex,bstrStyleSheet,bOff)	\
    ( (This)->lpVtbl -> SetStyleSheet(This,nIndex,bstrStyleSheet,bOff) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAPI2_INTERFACE_DEFINED__ */


#ifndef __IAPI3_INTERFACE_DEFINED__
#define __IAPI3_INTERFACE_DEFINED__

/* interface IAPI3 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IAPI3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("738A9354-D4E7-4dd9-861B-15D79F2C46F5")
    IAPI3 : public IAPI2
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveGroup( 
            BSTR bstrGroupFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadGroup( 
            BSTR bstrGroupFile,
            BOOL bClose) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EncryptString( 
            BSTR bstrString,
            BSTR bstrPass,
            /* [retval][out] */ BSTR *bstrRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DecryptString( 
            BSTR bstrString,
            /* [retval][out] */ BSTR *bstrRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InputBox( 
            BSTR bstrTitle,
            BSTR bstrDescript,
            BSTR bstrDefault,
            int nFlag,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NewWindow3( 
            BSTR bstrURL,
            BOOL bActive,
            long ExStyle,
            /* [in] */ const int *pHistInfo,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddGroupItem( 
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteGroupItem( 
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAPI3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAPI3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAPI3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAPI3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAPI3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAPI3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAPI3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAPI3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAPI3 * This,
            int nIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentObject )( 
            IAPI3 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindowObject )( 
            IAPI3 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWebBrowserObject )( 
            IAPI3 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TabIndex )( 
            IAPI3 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TabIndex )( 
            IAPI3 * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabCount )( 
            IAPI3 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveToTab )( 
            IAPI3 * This,
            WORD wBefor,
            WORD wAfter);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow )( 
            IAPI3 * This,
            BSTR bstrURL,
            BOOL bActive,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MessageBox )( 
            IAPI3 * This,
            BSTR bstrText,
            BSTR bstrCaption,
            UINT uType,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowPanelBar )( 
            IAPI3 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelDocumentObject )( 
            IAPI3 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWindowObject )( 
            IAPI3 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWebBrowserObject )( 
            IAPI3 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabState )( 
            IAPI3 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteCommand )( 
            IAPI3 * This,
            int nCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchText )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSearchText )( 
            IAPI3 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAddressText )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAddressText )( 
            IAPI3 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExtendedTabState )( 
            IAPI3 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetExtendedTabState )( 
            IAPI3 * This,
            int nIndex,
            long nState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyState )( 
            IAPI3 * This,
            int nKey,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileInt )( 
            IAPI3 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nDefault,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileInt )( 
            IAPI3 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileString )( 
            IAPI3 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrDefault,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileString )( 
            IAPI3 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScriptFolder )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCSSFolder )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBaseFolder )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExePath )( 
            IAPI3 * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetStyleSheet )( 
            IAPI3 * This,
            int nIndex,
            BSTR bstrStyleSheet,
            BOOL bOff);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveGroup )( 
            IAPI3 * This,
            BSTR bstrGroupFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadGroup )( 
            IAPI3 * This,
            BSTR bstrGroupFile,
            BOOL bClose);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EncryptString )( 
            IAPI3 * This,
            BSTR bstrString,
            BSTR bstrPass,
            /* [retval][out] */ BSTR *bstrRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DecryptString )( 
            IAPI3 * This,
            BSTR bstrString,
            /* [retval][out] */ BSTR *bstrRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InputBox )( 
            IAPI3 * This,
            BSTR bstrTitle,
            BSTR bstrDescript,
            BSTR bstrDefault,
            int nFlag,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow3 )( 
            IAPI3 * This,
            BSTR bstrURL,
            BOOL bActive,
            long ExStyle,
            /* [in] */ const int *pHistInfo,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddGroupItem )( 
            IAPI3 * This,
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteGroupItem )( 
            IAPI3 * This,
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IAPI3Vtbl;

    interface IAPI3
    {
        CONST_VTBL struct IAPI3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAPI3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAPI3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAPI3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAPI3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAPI3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAPI3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAPI3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAPI3_Close(This,nIndex)	\
    ( (This)->lpVtbl -> Close(This,nIndex) ) 

#define IAPI3_GetDocumentObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetDocumentObject(This,nIndex,pVal) ) 

#define IAPI3_GetWindowObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWindowObject(This,nIndex,pVal) ) 

#define IAPI3_GetWebBrowserObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWebBrowserObject(This,nIndex,pVal) ) 

#define IAPI3_get_TabIndex(This,pVal)	\
    ( (This)->lpVtbl -> get_TabIndex(This,pVal) ) 

#define IAPI3_put_TabIndex(This,newVal)	\
    ( (This)->lpVtbl -> put_TabIndex(This,newVal) ) 

#define IAPI3_GetTabCount(This,pVal)	\
    ( (This)->lpVtbl -> GetTabCount(This,pVal) ) 

#define IAPI3_MoveToTab(This,wBefor,wAfter)	\
    ( (This)->lpVtbl -> MoveToTab(This,wBefor,wAfter) ) 

#define IAPI3_NewWindow(This,bstrURL,bActive,pVal)	\
    ( (This)->lpVtbl -> NewWindow(This,bstrURL,bActive,pVal) ) 

#define IAPI3_MessageBox(This,bstrText,bstrCaption,uType,pVal)	\
    ( (This)->lpVtbl -> MessageBox(This,bstrText,bstrCaption,uType,pVal) ) 

#define IAPI3_ShowPanelBar(This)	\
    ( (This)->lpVtbl -> ShowPanelBar(This) ) 

#define IAPI3_GetPanelDocumentObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelDocumentObject(This,pVal) ) 

#define IAPI3_GetPanelWindowObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWindowObject(This,pVal) ) 

#define IAPI3_GetPanelWebBrowserObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWebBrowserObject(This,pVal) ) 

#define IAPI3_GetTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetTabState(This,nIndex,pVal) ) 


#define IAPI3_ExecuteCommand(This,nCommand)	\
    ( (This)->lpVtbl -> ExecuteCommand(This,nCommand) ) 

#define IAPI3_GetSearchText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetSearchText(This,pbstrText) ) 

#define IAPI3_SetSearchText(This,bstrText)	\
    ( (This)->lpVtbl -> SetSearchText(This,bstrText) ) 

#define IAPI3_GetAddressText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetAddressText(This,pbstrText) ) 

#define IAPI3_SetAddressText(This,bstrText)	\
    ( (This)->lpVtbl -> SetAddressText(This,bstrText) ) 

#define IAPI3_GetExtendedTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetExtendedTabState(This,nIndex,pVal) ) 

#define IAPI3_SetExtendedTabState(This,nIndex,nState)	\
    ( (This)->lpVtbl -> SetExtendedTabState(This,nIndex,nState) ) 

#define IAPI3_GetKeyState(This,nKey,pVal)	\
    ( (This)->lpVtbl -> GetKeyState(This,nKey,pVal) ) 

#define IAPI3_GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal)	\
    ( (This)->lpVtbl -> GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal) ) 

#define IAPI3_WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue)	\
    ( (This)->lpVtbl -> WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue) ) 

#define IAPI3_GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText)	\
    ( (This)->lpVtbl -> GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText) ) 

#define IAPI3_WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText)	\
    ( (This)->lpVtbl -> WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText) ) 

#define IAPI3_GetScriptFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetScriptFolder(This,pbstrFolder) ) 

#define IAPI3_GetCSSFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetCSSFolder(This,pbstrFolder) ) 

#define IAPI3_GetBaseFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetBaseFolder(This,pbstrFolder) ) 

#define IAPI3_GetExePath(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetExePath(This,pbstrPath) ) 

#define IAPI3_SetStyleSheet(This,nIndex,bstrStyleSheet,bOff)	\
    ( (This)->lpVtbl -> SetStyleSheet(This,nIndex,bstrStyleSheet,bOff) ) 


#define IAPI3_SaveGroup(This,bstrGroupFile)	\
    ( (This)->lpVtbl -> SaveGroup(This,bstrGroupFile) ) 

#define IAPI3_LoadGroup(This,bstrGroupFile,bClose)	\
    ( (This)->lpVtbl -> LoadGroup(This,bstrGroupFile,bClose) ) 

#define IAPI3_EncryptString(This,bstrString,bstrPass,bstrRet)	\
    ( (This)->lpVtbl -> EncryptString(This,bstrString,bstrPass,bstrRet) ) 

#define IAPI3_DecryptString(This,bstrString,bstrRet)	\
    ( (This)->lpVtbl -> DecryptString(This,bstrString,bstrRet) ) 

#define IAPI3_InputBox(This,bstrTitle,bstrDescript,bstrDefault,nFlag,pVal)	\
    ( (This)->lpVtbl -> InputBox(This,bstrTitle,bstrDescript,bstrDefault,nFlag,pVal) ) 

#define IAPI3_NewWindow3(This,bstrURL,bActive,ExStyle,pHistInfo,pVal)	\
    ( (This)->lpVtbl -> NewWindow3(This,bstrURL,bActive,ExStyle,pHistInfo,pVal) ) 

#define IAPI3_AddGroupItem(This,bstrGroupFile,nIndex,pVal)	\
    ( (This)->lpVtbl -> AddGroupItem(This,bstrGroupFile,nIndex,pVal) ) 

#define IAPI3_DeleteGroupItem(This,bstrGroupFile,nIndex,pVal)	\
    ( (This)->lpVtbl -> DeleteGroupItem(This,bstrGroupFile,nIndex,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAPI3_INTERFACE_DEFINED__ */


#ifndef __IAPI4_INTERFACE_DEFINED__
#define __IAPI4_INTERFACE_DEFINED__

/* interface IAPI4 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IAPI4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B437832F-A3A8-4948-9D42-64D1B91C604E")
    IAPI4 : public IAPI3
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHWND( 
            long nType,
            /* [retval][out] */ LONG_PTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAPI4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAPI4 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAPI4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAPI4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAPI4 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAPI4 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAPI4 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAPI4 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAPI4 * This,
            int nIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentObject )( 
            IAPI4 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWindowObject )( 
            IAPI4 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWebBrowserObject )( 
            IAPI4 * This,
            int nIndex,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TabIndex )( 
            IAPI4 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TabIndex )( 
            IAPI4 * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabCount )( 
            IAPI4 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveToTab )( 
            IAPI4 * This,
            WORD wBefor,
            WORD wAfter);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow )( 
            IAPI4 * This,
            BSTR bstrURL,
            BOOL bActive,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MessageBox )( 
            IAPI4 * This,
            BSTR bstrText,
            BSTR bstrCaption,
            UINT uType,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowPanelBar )( 
            IAPI4 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelDocumentObject )( 
            IAPI4 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWindowObject )( 
            IAPI4 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPanelWebBrowserObject )( 
            IAPI4 * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTabState )( 
            IAPI4 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteCommand )( 
            IAPI4 * This,
            int nCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchText )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSearchText )( 
            IAPI4 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAddressText )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAddressText )( 
            IAPI4 * This,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExtendedTabState )( 
            IAPI4 * This,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetExtendedTabState )( 
            IAPI4 * This,
            int nIndex,
            long nState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyState )( 
            IAPI4 * This,
            int nKey,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileInt )( 
            IAPI4 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nDefault,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileInt )( 
            IAPI4 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            int nValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileString )( 
            IAPI4 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrDefault,
            /* [retval][out] */ BSTR *pbstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteProfileString )( 
            IAPI4 * This,
            BSTR bstrFile,
            BSTR bstrSection,
            BSTR bstrKey,
            BSTR bstrText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScriptFolder )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCSSFolder )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBaseFolder )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrFolder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExePath )( 
            IAPI4 * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetStyleSheet )( 
            IAPI4 * This,
            int nIndex,
            BSTR bstrStyleSheet,
            BOOL bOff);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveGroup )( 
            IAPI4 * This,
            BSTR bstrGroupFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadGroup )( 
            IAPI4 * This,
            BSTR bstrGroupFile,
            BOOL bClose);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EncryptString )( 
            IAPI4 * This,
            BSTR bstrString,
            BSTR bstrPass,
            /* [retval][out] */ BSTR *bstrRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DecryptString )( 
            IAPI4 * This,
            BSTR bstrString,
            /* [retval][out] */ BSTR *bstrRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InputBox )( 
            IAPI4 * This,
            BSTR bstrTitle,
            BSTR bstrDescript,
            BSTR bstrDefault,
            int nFlag,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewWindow3 )( 
            IAPI4 * This,
            BSTR bstrURL,
            BOOL bActive,
            long ExStyle,
            /* [in] */ const int *pHistInfo,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddGroupItem )( 
            IAPI4 * This,
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteGroupItem )( 
            IAPI4 * This,
            BSTR bstrGroupFile,
            int nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHWND )( 
            IAPI4 * This,
            long nType,
            /* [retval][out] */ LONG_PTR *pVal);
        
        END_INTERFACE
    } IAPI4Vtbl;

    interface IAPI4
    {
        CONST_VTBL struct IAPI4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAPI4_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAPI4_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAPI4_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAPI4_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAPI4_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAPI4_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAPI4_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAPI4_Close(This,nIndex)	\
    ( (This)->lpVtbl -> Close(This,nIndex) ) 

#define IAPI4_GetDocumentObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetDocumentObject(This,nIndex,pVal) ) 

#define IAPI4_GetWindowObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWindowObject(This,nIndex,pVal) ) 

#define IAPI4_GetWebBrowserObject(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetWebBrowserObject(This,nIndex,pVal) ) 

#define IAPI4_get_TabIndex(This,pVal)	\
    ( (This)->lpVtbl -> get_TabIndex(This,pVal) ) 

#define IAPI4_put_TabIndex(This,newVal)	\
    ( (This)->lpVtbl -> put_TabIndex(This,newVal) ) 

#define IAPI4_GetTabCount(This,pVal)	\
    ( (This)->lpVtbl -> GetTabCount(This,pVal) ) 

#define IAPI4_MoveToTab(This,wBefor,wAfter)	\
    ( (This)->lpVtbl -> MoveToTab(This,wBefor,wAfter) ) 

#define IAPI4_NewWindow(This,bstrURL,bActive,pVal)	\
    ( (This)->lpVtbl -> NewWindow(This,bstrURL,bActive,pVal) ) 

#define IAPI4_MessageBox(This,bstrText,bstrCaption,uType,pVal)	\
    ( (This)->lpVtbl -> MessageBox(This,bstrText,bstrCaption,uType,pVal) ) 

#define IAPI4_ShowPanelBar(This)	\
    ( (This)->lpVtbl -> ShowPanelBar(This) ) 

#define IAPI4_GetPanelDocumentObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelDocumentObject(This,pVal) ) 

#define IAPI4_GetPanelWindowObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWindowObject(This,pVal) ) 

#define IAPI4_GetPanelWebBrowserObject(This,pVal)	\
    ( (This)->lpVtbl -> GetPanelWebBrowserObject(This,pVal) ) 

#define IAPI4_GetTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetTabState(This,nIndex,pVal) ) 


#define IAPI4_ExecuteCommand(This,nCommand)	\
    ( (This)->lpVtbl -> ExecuteCommand(This,nCommand) ) 

#define IAPI4_GetSearchText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetSearchText(This,pbstrText) ) 

#define IAPI4_SetSearchText(This,bstrText)	\
    ( (This)->lpVtbl -> SetSearchText(This,bstrText) ) 

#define IAPI4_GetAddressText(This,pbstrText)	\
    ( (This)->lpVtbl -> GetAddressText(This,pbstrText) ) 

#define IAPI4_SetAddressText(This,bstrText)	\
    ( (This)->lpVtbl -> SetAddressText(This,bstrText) ) 

#define IAPI4_GetExtendedTabState(This,nIndex,pVal)	\
    ( (This)->lpVtbl -> GetExtendedTabState(This,nIndex,pVal) ) 

#define IAPI4_SetExtendedTabState(This,nIndex,nState)	\
    ( (This)->lpVtbl -> SetExtendedTabState(This,nIndex,nState) ) 

#define IAPI4_GetKeyState(This,nKey,pVal)	\
    ( (This)->lpVtbl -> GetKeyState(This,nKey,pVal) ) 

#define IAPI4_GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal)	\
    ( (This)->lpVtbl -> GetProfileInt(This,bstrFile,bstrSection,bstrKey,nDefault,pVal) ) 

#define IAPI4_WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue)	\
    ( (This)->lpVtbl -> WriteProfileInt(This,bstrFile,bstrSection,bstrKey,nValue) ) 

#define IAPI4_GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText)	\
    ( (This)->lpVtbl -> GetProfileString(This,bstrFile,bstrSection,bstrKey,bstrDefault,pbstrText) ) 

#define IAPI4_WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText)	\
    ( (This)->lpVtbl -> WriteProfileString(This,bstrFile,bstrSection,bstrKey,bstrText) ) 

#define IAPI4_GetScriptFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetScriptFolder(This,pbstrFolder) ) 

#define IAPI4_GetCSSFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetCSSFolder(This,pbstrFolder) ) 

#define IAPI4_GetBaseFolder(This,pbstrFolder)	\
    ( (This)->lpVtbl -> GetBaseFolder(This,pbstrFolder) ) 

#define IAPI4_GetExePath(This,pbstrPath)	\
    ( (This)->lpVtbl -> GetExePath(This,pbstrPath) ) 

#define IAPI4_SetStyleSheet(This,nIndex,bstrStyleSheet,bOff)	\
    ( (This)->lpVtbl -> SetStyleSheet(This,nIndex,bstrStyleSheet,bOff) ) 


#define IAPI4_SaveGroup(This,bstrGroupFile)	\
    ( (This)->lpVtbl -> SaveGroup(This,bstrGroupFile) ) 

#define IAPI4_LoadGroup(This,bstrGroupFile,bClose)	\
    ( (This)->lpVtbl -> LoadGroup(This,bstrGroupFile,bClose) ) 

#define IAPI4_EncryptString(This,bstrString,bstrPass,bstrRet)	\
    ( (This)->lpVtbl -> EncryptString(This,bstrString,bstrPass,bstrRet) ) 

#define IAPI4_DecryptString(This,bstrString,bstrRet)	\
    ( (This)->lpVtbl -> DecryptString(This,bstrString,bstrRet) ) 

#define IAPI4_InputBox(This,bstrTitle,bstrDescript,bstrDefault,nFlag,pVal)	\
    ( (This)->lpVtbl -> InputBox(This,bstrTitle,bstrDescript,bstrDefault,nFlag,pVal) ) 

#define IAPI4_NewWindow3(This,bstrURL,bActive,ExStyle,pHistInfo,pVal)	\
    ( (This)->lpVtbl -> NewWindow3(This,bstrURL,bActive,ExStyle,pHistInfo,pVal) ) 

#define IAPI4_AddGroupItem(This,bstrGroupFile,nIndex,pVal)	\
    ( (This)->lpVtbl -> AddGroupItem(This,bstrGroupFile,nIndex,pVal) ) 

#define IAPI4_DeleteGroupItem(This,bstrGroupFile,nIndex,pVal)	\
    ( (This)->lpVtbl -> DeleteGroupItem(This,bstrGroupFile,nIndex,pVal) ) 


#define IAPI4_GetHWND(This,nType,pVal)	\
    ( (This)->lpVtbl -> GetHWND(This,nType,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAPI4_INTERFACE_DEFINED__ */



#ifndef __DONUTPLib_LIBRARY_DEFINED__
#define __DONUTPLib_LIBRARY_DEFINED__

/* library DONUTPLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DONUTPLib;

#ifndef __IDonutPEvents_DISPINTERFACE_DEFINED__
#define __IDonutPEvents_DISPINTERFACE_DEFINED__

/* dispinterface IDonutPEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_IDonutPEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("CFF8CB23-2580-4BE4-BB49-4A2C1B645164")
    IDonutPEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDonutPEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDonutPEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDonutPEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDonutPEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDonutPEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDonutPEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDonutPEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDonutPEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDonutPEventsVtbl;

    interface IDonutPEvents
    {
        CONST_VTBL struct IDonutPEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDonutPEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDonutPEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDonutPEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDonutPEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDonutPEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDonutPEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDonutPEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDonutPEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_API;

#ifdef __cplusplus

class DECLSPEC_UUID("DFA77729-9333-4E33-8D52-7D4684CF9867")
API;
#endif
#endif /* __DONUTPLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


