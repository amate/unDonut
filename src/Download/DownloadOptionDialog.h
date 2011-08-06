// OptionDialog.h

#pragma once

#include <atlddx.h>
#include "../IniFile.h"
#include "../MtlMisc.h"
#include "../resource.h"

#define REGISTERMESSAGE_GETDEFAULTDLFOLDER	_T("Donut_DLManager_GetDefaultDLFolder")
#define REGISTERMESSAGE_STARTDOWNLOAD		_T("Donut_DLManager_StartDownload")
#define REGISTERMESSAGE_SETREFERER			_T("Donut_DLManager_SetReferer")

struct DLStartItem {
	LPCWSTR strURL;
	LPCWSTR strDLFolder;
	HWND	hWnd;
	DWORD	dwOption;
};
enum EDLOption {
	DLO_OVERWRITEPROMPT = 0x0001,	// 上書きの確認をする
	DLO_USEUNIQUENUMBER = 0x0002,	// 連番を付ける
	DLO_SHOWWINDOW		= 0x0004,
	DLO_SAVEIMAGE		= 0x0008,	// 画像を保存
};

#define REGISTERMESSAGE_DLCOMPLETE			_T("Donut_DLManager_DLComplete")


#if !(NTDDI_VERSION >= NTDDI_LONGHORN)
typedef struct _COMDLG_FILTERSPEC
    {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
    } 	COMDLG_FILTERSPEC;

typedef /* [v1_enum] */ 
enum tagFDE_OVERWRITE_RESPONSE
    {	FDEOR_DEFAULT	= 0,
	FDEOR_ACCEPT	= 0x1,
	FDEOR_REFUSE	= 0x2
    } 	FDE_OVERWRITE_RESPONSE;

typedef /* [v1_enum] */ 
enum tagFDE_SHAREVIOLATION_RESPONSE
    {	FDESVR_DEFAULT	= 0,
	FDESVR_ACCEPT	= 0x1,
	FDESVR_REFUSE	= 0x2
    } 	FDE_SHAREVIOLATION_RESPONSE;

typedef /* [v1_enum] */ 
enum tagFDAP
    {	FDAP_BOTTOM	= 0,
	FDAP_TOP	= 0x1
    } 	FDAP;



#ifndef __IFileDialog_INTERFACE_DEFINED__
#define __IFileDialog_INTERFACE_DEFINED__

/* interface IFileDialog */
/* [unique][object][uuid] */ 


enum tagFILEOPENDIALOGOPTIONS
    {	FOS_OVERWRITEPROMPT	= 0x2,
	FOS_STRICTFILETYPES	= 0x4,
	FOS_NOCHANGEDIR	= 0x8,
	FOS_PICKFOLDERS	= 0x20,
	FOS_FORCEFILESYSTEM	= 0x40,
	FOS_ALLNONSTORAGEITEMS	= 0x80,
	FOS_NOVALIDATE	= 0x100,
	FOS_ALLOWMULTISELECT	= 0x200,
	FOS_PATHMUSTEXIST	= 0x800,
	FOS_FILEMUSTEXIST	= 0x1000,
	FOS_CREATEPROMPT	= 0x2000,
	FOS_SHAREAWARE	= 0x4000,
	FOS_NOREADONLYRETURN	= 0x8000,
	FOS_NOTESTFILECREATE	= 0x10000,
	FOS_HIDEMRUPLACES	= 0x20000,
	FOS_HIDEPINNEDPLACES	= 0x40000,
	FOS_NODEREFERENCELINKS	= 0x100000,
	FOS_DONTADDTORECENT	= 0x2000000,
	FOS_FORCESHOWHIDDEN	= 0x10000000,
	FOS_DEFAULTNOMINIMODE	= 0x20000000,
	FOS_FORCEPREVIEWPANEON	= 0x40000000
    } ;

EXTERN_C const IID IID_IFileDialog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
    IFileDialog : public IModalWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFileTypes( 
            /* [in] */ UINT cFileTypes,
            /* [size_is][in] */ __RPC__in_ecount_full(cFileTypes) const COMDLG_FILTERSPEC *rgFilterSpec) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex( 
            /* [in] */ UINT iFileType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex( 
            /* [out] */ __RPC__out UINT *piFileType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Advise( 
            /* [in] */ __RPC__in_opt IFileDialogEvents *pfde,
            /* [out] */ __RPC__out DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unadvise( 
            /* [in] */ DWORD dwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOptions( 
            /* [in] */ DWORD fos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOptions( 
            /* [out] */ __RPC__out DWORD *pfos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder( 
            /* [in] */ __RPC__in_opt IShellItem *psi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFolder( 
            /* [in] */ __RPC__in_opt IShellItem *psi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFolder( 
            /* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection( 
            /* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileName( 
            /* [string][in] */ __RPC__in LPCWSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTitle( 
            /* [string][in] */ __RPC__in LPCWSTR pszTitle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel( 
            /* [string][in] */ __RPC__in LPCWSTR pszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel( 
            /* [string][in] */ __RPC__in LPCWSTR pszLabel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResult( 
            /* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddPlace( 
            /* [in] */ __RPC__in_opt IShellItem *psi,
            /* [in] */ FDAP fdap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension( 
            /* [string][in] */ __RPC__in LPCWSTR pszDefaultExtension) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClientGuid( 
            /* [in] */ __RPC__in REFGUID guid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearClientData( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFilter( 
            /* [in] */ __RPC__in_opt IShellItemFilter *pFilter) = 0;
        
    };
    
#endif
#endif


#ifndef __IFileOpenDialog_INTERFACE_DEFINED__
#define __IFileOpenDialog_INTERFACE_DEFINED__

/* interface IFileOpenDialog */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IFileOpenDialog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d57c7288-d4ad-4768-be02-9d969532d960")
    IFileOpenDialog : public IFileDialog
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetResults( 
            /* [out] */ __RPC__deref_out_opt IShellItemArray **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSelectedItems( 
            /* [out] */ __RPC__deref_out_opt IShellItemArray **ppsai) = 0;
        
    };
    
#endif
#endif

#ifndef __IFileSaveDialog_INTERFACE_DEFINED__
#define __IFileSaveDialog_INTERFACE_DEFINED__

/* interface IFileSaveDialog */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IFileSaveDialog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("84bccd23-5fde-4cdb-aea4-af64b83d78ab")
    IFileSaveDialog : public IFileDialog
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem( 
            /* [in] */ __RPC__in_opt IShellItem *psi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProperties( 
            /* [in] */ __RPC__in_opt IPropertyStore *pStore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties( 
            /* [in] */ __RPC__in_opt IPropertyDescriptionList *pList,
            /* [in] */ BOOL fAppendDefault) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProperties( 
            /* [out] */ __RPC__deref_out_opt IPropertyStore **ppStore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyProperties( 
            /* [in] */ __RPC__in_opt IShellItem *psi,
            /* [in] */ __RPC__in_opt IPropertyStore *pStore,
            /* [unique][in] */ __RPC__in_opt HWND hwnd,
            /* [unique][in] */ __RPC__in_opt IFileOperationProgressSink *pSink) = 0;
        
    };
    
#endif
#endif

#ifndef __IFileDialogEvents_INTERFACE_DEFINED__
#define __IFileDialogEvents_INTERFACE_DEFINED__

/* interface IFileDialogEvents */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IFileDialogEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("973510db-7d7f-452b-8975-74a85828d354")
    IFileDialogEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnFileOk( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnFolderChanging( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd,
            /* [in] */ __RPC__in_opt IShellItem *psiFolder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnFolderChange( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSelectionChange( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnShareViolation( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd,
            /* [in] */ __RPC__in_opt IShellItem *psi,
            /* [out] */ __RPC__out FDE_SHAREVIOLATION_RESPONSE *pResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnTypeChange( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnOverwrite( 
            /* [in] */ __RPC__in_opt IFileDialog *pfd,
            /* [in] */ __RPC__in_opt IShellItem *psi,
            /* [out] */ __RPC__out FDE_OVERWRITE_RESPONSE *pResponse) = 0;
        
    };

#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// CShellFileDialogImpl - base class for CShellFileOpenDialogImpl and CShellFileSaveDialogImpl

template <class T>
class ATL_NO_VTABLE CShellFileDialogImpl : public IFileDialogEvents
{
public:
// Operations
	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		INT_PTR 	nRet = -1;

		T* 	pT = static_cast<T*>(this);
		if (pT->m_spFileDlg == NULL)
		{
			ATLASSERT(FALSE);
			return nRet;
		}

		DWORD 	dwCookie = 0;
		pT->_Advise(dwCookie);

		HRESULT hRet = pT->m_spFileDlg->Show(hWndParent);
		if (SUCCEEDED(hRet))
			nRet = IDOK;
		else if (hRet == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			nRet = IDCANCEL;
		else
			ATLASSERT(FALSE);   // error

		pT->_Unadvise(dwCookie);

		return nRet;
	}

	bool IsNull() const
	{
		const T* 	pT = static_cast<const T*>(this);
		return (pT->m_spFileDlg == NULL);
	}

 // Operations - get file path after dialog returns
	HRESULT GetFilePath(LPWSTR lpstrFilePath, int cchLength)
	{
		T* 			pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);

		ATL::CComPtr<IShellItem> 	spItem;
		HRESULT		hRet= pT->m_spFileDlg->GetResult(&spItem);

		if (SUCCEEDED(hRet))
			hRet = GetFileNameFromShellItem(spItem, SIGDN_FILESYSPATH, lpstrFilePath, cchLength);

		return hRet;
	}

	HRESULT GetFileTitle(LPWSTR lpstrFileTitle, int cchLength)
	{
		T* 		  pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);

		ATL::CComPtr<IShellItem> 	spItem;
		HRESULT   hRet 	= pT->m_spFileDlg->GetResult(&spItem);

		if (SUCCEEDED(hRet))
			hRet 	= GetFileNameFromShellItem(spItem, SIGDN_NORMALDISPLAY, lpstrFileTitle, cchLength);

		return hRet;
	}

  #if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	HRESULT GetFilePath(_CSTRING_NS::CString& strFilePath)
	{
		T* 	pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);

		ATL::CComPtr<IShellItem> spItem;
		HRESULT 	hRet = pT->m_spFileDlg->GetResult(&spItem);

		if (SUCCEEDED(hRet))
			hRet = GetFileNameFromShellItem(spItem, SIGDN_FILESYSPATH, strFilePath);

		return hRet;
	}

	HRESULT GetFileTitle(_CSTRING_NS::CString& strFileTitle)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);

		ATL::CComPtr<IShellItem> spItem;
		HRESULT 	hRet = pT->m_spFileDlg->GetResult(&spItem);

		if (SUCCEEDED(hRet))
			hRet = GetFileNameFromShellItem(spItem, SIGDN_NORMALDISPLAY, strFileTitle);

		return hRet;
	}
  #endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

 // Helpers for IShellItem
	static HRESULT GetFileNameFromShellItem(IShellItem* pShellItem, SIGDN type, LPWSTR lpstr, int cchLength)
	{
		ATLASSERT(pShellItem != NULL);

		LPWSTR 		lpstrName 	= NULL;
		HRESULT 	hRet 		= pShellItem->GetDisplayName(type, &lpstrName);

		if (SUCCEEDED(hRet))
		{
			if (lstrlenW(lpstrName) < cchLength)
			{
				SecureHelper::strcpyW_x(lpstr, cchLength, lpstrName);
			}
			else
			{
				ATLASSERT(FALSE);
				hRet = DISP_E_BUFFERTOOSMALL;
			}

			::CoTaskMemFree(lpstrName);
		}

		return hRet;
	}

 #if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	static HRESULT GetFileNameFromShellItem(IShellItem* pShellItem, SIGDN type, _CSTRING_NS::CString& str)
	{
		ATLASSERT(pShellItem != NULL);

		LPWSTR 		lpstrName 	= NULL;
		HRESULT 	hRet 		= pShellItem->GetDisplayName(type, &lpstrName);

		if (SUCCEEDED(hRet))
		{
			str = lpstrName;
			::CoTaskMemFree(lpstrName);
		}

		return hRet;
	}
 #endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

 // Implementation
	void _Advise(DWORD& dwCookie)
	{
		T* 		  pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);
		HRESULT   hRet 	= pT->m_spFileDlg->Advise((IFileDialogEvents*)this, &dwCookie);
		ATLVERIFY(SUCCEEDED(hRet));
	}

	void _Unadvise(DWORD dwCookie)
	{
		T* 		pT 	 = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);
		HRESULT hRet = pT->m_spFileDlg->Unadvise(dwCookie);
		ATLVERIFY(SUCCEEDED(hRet));
	}

	void _Init(LPCWSTR lpszFileName, DWORD dwOptions, LPCWSTR lpszDefExt, const COMDLG_FILTERSPEC* arrFilterSpec, UINT uFilterSpecCount)
	{
		T* 		pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg != NULL);

		HRESULT hRet = E_FAIL;

		if (lpszFileName != NULL)
		{
			hRet = pT->m_spFileDlg->SetFileName(lpszFileName);
			ATLASSERT(SUCCEEDED(hRet));
		}

		hRet 	= pT->m_spFileDlg->SetOptions(dwOptions);
		ATLASSERT(SUCCEEDED(hRet));

		if (lpszDefExt != NULL)
		{
			hRet = pT->m_spFileDlg->SetDefaultExtension(lpszDefExt);
			ATLASSERT(SUCCEEDED(hRet));
		}

		if (arrFilterSpec != NULL && uFilterSpecCount != 0U)
		{
			hRet = pT->m_spFileDlg->SetFileTypes(uFilterSpecCount, arrFilterSpec);
			ATLASSERT(SUCCEEDED(hRet));
		}
	}

 // Implementation - IUnknown interface
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
	{
		if (ppvObject == NULL)
			return E_POINTER;

		T* 	pT = static_cast<T*>(this);
		if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IFileDialogEvents))
		{
			*ppvObject = (IFileDialogEvents*)pT;
			// AddRef() not needed
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

 // Implementation - IFileDialogEvents interface
	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnFileOk(IFileDialog* pfd)
	{
		T* 	pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnFileOk();
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnFolderChanging(IFileDialog* pfd, IShellItem* psiFolder)
	{
		T* 	pT 	= static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnFolderChanging(psiFolder);
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnFolderChange(IFileDialog* pfd)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnFolderChange();
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnSelectionChange(IFileDialog* pfd)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnSelectionChange();
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnShareViolation(IFileDialog* pfd, IShellItem* psi, FDE_SHAREVIOLATION_RESPONSE* pResponse)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnShareViolation(psi, pResponse);
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnTypeChange(IFileDialog* pfd)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnTypeChange();
	}

	virtual HRESULT STDMETHODCALLTYPE IFileDialogEvents::OnOverwrite(IFileDialog* pfd, IShellItem* psi, FDE_OVERWRITE_RESPONSE* pResponse)
	{
		T* 	pT = static_cast<T*>(this);
		ATLASSERT(pT->m_spFileDlg.IsEqualObject(pfd));
		pfd;   // avoid level 4 warning
		return pT->OnOverwrite(psi, pResponse);
	}

 // Overrideables - Event handlers
	HRESULT OnFileOk()
	{
		return E_NOTIMPL;
	}

	HRESULT OnFolderChanging(IShellItem* /*psiFolder*/)
	{
		return E_NOTIMPL;
	}

	HRESULT OnFolderChange()
	{
		return E_NOTIMPL;
	}

	HRESULT OnSelectionChange()
	{
		return E_NOTIMPL;
	}

	HRESULT OnShareViolation(IShellItem* /*psi*/, FDE_SHAREVIOLATION_RESPONSE* /*pResponse*/)
	{
		return E_NOTIMPL;
	}

	HRESULT OnTypeChange()
	{
		return E_NOTIMPL;
	}

	HRESULT OnOverwrite(IShellItem* /*psi*/, FDE_OVERWRITE_RESPONSE* /*pResponse*/)
	{
		return E_NOTIMPL;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CShellFileOpenDialogImpl - implements new Shell File Open dialog

template <class T>
class ATL_NO_VTABLE CShellFileOpenDialogImpl : public CShellFileDialogImpl< T >
{
public:
	ATL::CComPtr<IFileOpenDialog> 	m_spFileDlg;

	CShellFileOpenDialogImpl(LPCWSTR 				  lpszFileName 	   = NULL,
	                         DWORD 					  dwOptions 	   = FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST,
	                         LPCWSTR 				  lpszDefExt 	   = NULL,
	                         const COMDLG_FILTERSPEC* arrFilterSpec    = NULL,
	                         UINT 					  uFilterSpecCount = 0U)
	{
		HRESULT hRet = m_spFileDlg.CoCreateInstance(CLSID_FileOpenDialog);

		if (SUCCEEDED(hRet))
			_Init(lpszFileName, dwOptions, lpszDefExt, arrFilterSpec, uFilterSpecCount);
	}

	IFileOpenDialog* GetPtr()
	{
		return m_spFileDlg;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CShellFileOpenDialog - new Shell File Open dialog without events

class CShellFileOpenDialog : public CShellFileOpenDialogImpl<CShellFileOpenDialog>
{
public:
	CShellFileOpenDialog(LPCWSTR lpszFileName = NULL,
	                     DWORD dwOptions = FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST,
	                     LPCWSTR lpszDefExt = NULL,
	                     const COMDLG_FILTERSPEC* arrFilterSpec = NULL,
	                     UINT uFilterSpecCount = 0U) : CShellFileOpenDialogImpl<CShellFileOpenDialog>(lpszFileName, dwOptions, lpszDefExt, arrFilterSpec, uFilterSpecCount)
	{ }

 // Implementation (remove _Advise/_Unadvise code using template magic)
	void _Advise(DWORD& /*dwCookie*/)
	{ }

	void _Unadvise(DWORD /*dwCookie*/)
	{ }
};


///////////////////////////////////////////////////////////////////////////////
// CShellFileSaveDialogImpl - implements new Shell File Save dialog

template <class T>
class ATL_NO_VTABLE CShellFileSaveDialogImpl : public CShellFileDialogImpl< T >
{
public:
	ATL::CComPtr<IFileSaveDialog> m_spFileDlg;

	CShellFileSaveDialogImpl(LPCWSTR 	lpszFileName 				= NULL,
	                         DWORD 		dwOptions 					= FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT,
	                         LPCWSTR 	lpszDefExt 					= NULL,
	                         const COMDLG_FILTERSPEC* arrFilterSpec = NULL,
	                         UINT 		uFilterSpecCount 			= 0U)
	{
		HRESULT hRet = m_spFileDlg.CoCreateInstance(CLSID_FileSaveDialog);

		if (SUCCEEDED(hRet))
			_Init(lpszFileName, dwOptions, lpszDefExt, arrFilterSpec, uFilterSpecCount);
	}

	IFileSaveDialog* GetPtr()
	{
		return m_spFileDlg;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CShellFileSaveDialog - new Shell File Save dialog without events

class CShellFileSaveDialog : public CShellFileSaveDialogImpl<CShellFileSaveDialog>
{
public:
	CShellFileSaveDialog(LPCWSTR 	lpszFileName 				= NULL,
	                     DWORD 		dwOptions 					= FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT,
	                     LPCWSTR 	lpszDefExt 					= NULL,
	                     const COMDLG_FILTERSPEC* arrFilterSpec = NULL,
	                     UINT 		uFilterSpecCount 			= 0U) : CShellFileSaveDialogImpl<CShellFileSaveDialog>(lpszFileName, dwOptions, lpszDefExt, arrFilterSpec, uFilterSpecCount)
	{ }

 // Implementation (remove _Advise/_Unadvise code using template magic)
	void _Advise(DWORD& /*dwCookie*/)
	{ }

	void _Unadvise(DWORD /*dwCookie*/)
	{ }
};

#endif	// !(NTDDI_VERSION >= NTDDI_LONGHORN)




//////////////////////////////////////////
// CDLOptions

struct CDLOptions
{
	static CString	s_DLIniFilePath;

	static CString	strDLFolderPath;
	static bool		bUseSaveFileDialog;
	static bool		bCloseAfterAllDL;
	static bool		bShowWindowOnDL;

	static CString	strImgDLFolderPath;
	static DWORD	dwImgExStyle;

	enum { kMaxHistory = 15 };
	static vector<CString>	s_vecDLFolderHistory;
	static vector<CString>	s_vecImageDLFolderHistory;

	static void	LoadProfile()
	{
		s_DLIniFilePath = Misc::GetFullPath_ForExe(_T("Download.ini"));
		CIniFileI	pr(s_DLIniFilePath, _T("Main"));
		strDLFolderPath		= pr.GetString(_T("DLFolder"), Misc::GetExeDirectory());
		bUseSaveFileDialog	= pr.GetValue(_T("UseSaveFileDialog"), bUseSaveFileDialog) != 0;
		bCloseAfterAllDL	= pr.GetValue(_T("CloseAfterAllDL"), bCloseAfterAllDL) != 0;
		bShowWindowOnDL		= pr.GetValue(_T("ShowWindowOnDL"), bShowWindowOnDL) != 0;

		strImgDLFolderPath  = pr.GetString(_T("ImgDLFolder"), strDLFolderPath);
		dwImgExStyle		= pr.GetValue(_T("ImgExStyle"), DLO_OVERWRITEPROMPT);

		s_vecDLFolderHistory.clear();
		pr.ChangeSectionName(_T("DLFolderHistory"));
		for (int i = 0; i < kMaxHistory; ++i) {
			CString strName;
			strName.Append(i);
			CString strFolder = pr.GetString(strName);
			if (strFolder.IsEmpty())
				break;
			s_vecDLFolderHistory.push_back(strFolder);
		}

		s_vecImageDLFolderHistory.clear();
		pr.ChangeSectionName(_T("ImageDLFolderHistory"));
		for (int i = 0; i < kMaxHistory; ++i) {
			CString strName;
			strName.Append(i);
			CString strFolder = pr.GetString(strName);
			if (strFolder.IsEmpty())
				break;
			s_vecImageDLFolderHistory.push_back(strFolder);
		}
	}

	static void	SaveProfile()
	{
		CIniFileO pr(s_DLIniFilePath, _T("Main"));
		MTL::MtlMakeSureTrailingBackSlash(strDLFolderPath);
		pr.SetString(strDLFolderPath, _T("DLFolder"));
		pr.SetValue(bUseSaveFileDialog, _T("UseSaveFileDialog"));
		pr.SetValue(bCloseAfterAllDL, _T("CloseAfterAllDL"));
		pr.SetValue(bShowWindowOnDL, _T("ShowWindowOnDL"));

		MTL::MtlMakeSureTrailingBackSlash(strImgDLFolderPath);
		pr.SetString(strImgDLFolderPath, _T("ImgDLFolder"));
		pr.SetValue(dwImgExStyle, _T("ImgExStyle"));

		{
			pr.ChangeSectionName(_T("DLFolderHistory"));
			int nCount = (int)s_vecDLFolderHistory.size();
			for (int i = 0; i < nCount; ++i) {
				CString strName;
				strName.Append(i);
				pr.SetString(s_vecDLFolderHistory[i], strName);
			}
		}
		{
			pr.ChangeSectionName(_T("ImageDLFolderHistory"));
			int nCount = (int)s_vecImageDLFolderHistory.size();
			for (int i = 0; i < nCount; ++i) {
				CString strName;
				strName.Append(i);
				pr.SetString(s_vecImageDLFolderHistory[i], strName);
			}
		}
	}

	static void _SavePathHistory(CString strPath, vector<CString>& vecPathHistory)
	{
		MTL::MtlMakeSureTrailingBackSlash(strPath);
		int nCount = (int)vecPathHistory.size();
		for (int i = 0; i < nCount; ++i) {
			if (strPath == vecPathHistory[i]) {	// 重複の削除
				vecPathHistory.erase(vecPathHistory.begin() + i);
				break;
			}
		}
		vecPathHistory.insert(vecPathHistory.begin(), strPath);
	}
};


__declspec(selectany) CString	CDLOptions::s_DLIniFilePath;

__declspec(selectany) CString	CDLOptions::strDLFolderPath;
__declspec(selectany) bool		CDLOptions::bUseSaveFileDialog = false;
__declspec(selectany) bool		CDLOptions::bCloseAfterAllDL = false;
__declspec(selectany) bool		CDLOptions::bShowWindowOnDL = false;

__declspec(selectany) CString	CDLOptions::strImgDLFolderPath;
__declspec(selectany) DWORD		CDLOptions::dwImgExStyle = DLO_OVERWRITEPROMPT;

__declspec(selectany) vector<CString>	CDLOptions::s_vecDLFolderHistory;
__declspec(selectany) vector<CString>	CDLOptions::s_vecImageDLFolderHistory;


///////////////////////////////////////////////////////
// COptionDialog

class CDLOptionDialog 
	: public CDialogImpl<CDLOptionDialog> 
	, public CWinDataExchange<CDLOptionDialog>
	, protected CDLOptions

{
public:
	enum { IDD = IDD_DLOPTION };

	// Constructor
	CDLOptionDialog()
		: m_nRadioImg(0)
	{ }

    // DDXマップ
    BEGIN_DDX_MAP(CDLOptionDialog)
        DDX_TEXT(IDC_CMB_DOWNLOADFOLDER			, strDLFolderPath)
		DDX_CHECK(IDC_CHECK_OPENDIALOG_BEFOREDL	, bUseSaveFileDialog)
		DDX_CHECK(IDC_CHECK_CLOSEAFTERALLDL		, bCloseAfterAllDL)
		DDX_CHECK(IDC_CHECK_SHOWWINDOW_ONDL		, bShowWindowOnDL)
		DDX_TEXT(IDC_CMB_IMGDOWNLOADFOLDER		, strImgDLFolderPath)
		DDX_RADIO(IDC_RADIO_OVERWRITEPROMPT		, m_nRadioImg)
    END_DDX_MAP()

	BEGIN_MSG_MAP(CDLOptionDialog)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX(IDOK		, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL	, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_FOLDERSELECT, OnFolderSelect )
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_IMGFOLDERSELECT, OnFolderSelect )
	END_MSG_MAP()


	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		CenterWindow();

		if (dwImgExStyle == DLO_USEUNIQUENUMBER)
			m_nRadioImg = 1;

		DoDataExchange(DDX_LOAD);

		/* 履歴の設定 */
		CComboBox cmbDLFolder = GetDlgItem(IDC_CMB_DOWNLOADFOLDER);
		std::for_each(s_vecDLFolderHistory.cbegin(), s_vecDLFolderHistory.cend(), [&cmbDLFolder](const CString& strFolder) {
			cmbDLFolder.AddString(strFolder);
		});

		CComboBox cmbImageDLFolder = GetDlgItem(IDC_CMB_IMGDOWNLOADFOLDER);
		std::for_each(s_vecImageDLFolderHistory.cbegin(), s_vecImageDLFolderHistory.cend(), [&cmbImageDLFolder](const CString& strFolder) {
			cmbImageDLFolder.AddString(strFolder);
		});
		

		return TRUE;
	}

	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString strDLFolder = strDLFolderPath;
		DoDataExchange(DDX_SAVE);
		if (strDLFolderPath.IsEmpty()) {
			MessageBox(_T("既定のDLフォルダが設定されていません。"), NULL, MB_ICONWARNING);
			strDLFolderPath = strDLFolder;
			DoDataExchange(DDX_LOAD, IDC_CMB_DOWNLOADFOLDER);
			return ;
		}

		/* 履歴の保存 */
		_SavePathHistory(strDLFolderPath, s_vecDLFolderHistory);
		_SavePathHistory(strImgDLFolderPath, s_vecImageDLFolderHistory);


		if (m_nRadioImg == 0)
			dwImgExStyle	= DLO_OVERWRITEPROMPT;
		else 
			dwImgExStyle	= DLO_USEUNIQUENUMBER;

		SaveProfile();	// 設定を保存

		EndDialog(nID);
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(nID);
	}


	void OnFolderSelect(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString strPath;
		CShellFileOpenDialog SHdlg(NULL, FOS_PICKFOLDERS);
		if (SHdlg.IsNull() == false) {
			if (SHdlg.DoModal(m_hWnd) == IDOK) {
				SHdlg.GetFilePath(strPath);
			}
		} else {
			CFolderDialog dlg(NULL, _T("フォルダを選択してください。"));
			if (dlg.DoModal(m_hWnd) == IDOK) {
				strPath = dlg.GetFolderPath();
			}
		}
		if (strPath.IsEmpty() == FALSE) {
			UINT	nTargetID;
			switch (nID) {
			case IDC_BUTTON_FOLDERSELECT:		
				nTargetID = IDC_CMB_DOWNLOADFOLDER; 
				strDLFolderPath	   = strPath;
				break;
			case IDC_BUTTON_IMGFOLDERSELECT:	
				nTargetID = IDC_CMB_IMGDOWNLOADFOLDER;
				strImgDLFolderPath = strPath;
				break;
			}
			DoDataExchange(DDX_LOAD, nTargetID);
		}
	}

private:
	int	m_nRadioImg;

};






