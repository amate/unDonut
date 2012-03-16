/**
*
*
*/

#pragma once

#include <atldlgs.h>


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
