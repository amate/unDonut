/**
 *	@file	MtlCom.h
 *	@brief	MTL : COMä÷åW
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlCom.h: Last updated: February 12, 2001
////////////////////////////////////////////////////////////////////////////

#ifndef __MTLCOM_H__
#define __MTLCOM_H__

#pragma once

#include "MtlBase.h"
#include <shlobj.h>
#include "MtlMisc.h"

#define HANDLE_MENU_MESSAGE_CONTEXTMENU(x)											 \
	if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM || uMsg == WM_MEASUREITEM) { \
		if (x != NULL) {															 \
			bHandled = TRUE;														 \
			lResult  = x->HandleMenuMsg(uMsg, wParam, lParam);						 \
		} else {																	 \
			bHandled = FALSE;														 \
		}																			 \
		if (bHandled)																 \
			return TRUE;															 \
	}



namespace MTL {


inline void MtlCheckError(SCODE sc)
{
	if ( FAILED(sc) ) {
		if (sc == E_OUTOFMEMORY)
			ATLTRACE2(atlTraceCOM, 0, "MtlCheckError Error : OOM\n");
		else
			ATLTRACE2(atlTraceCOM, 0, "MtlCheckError Error : MISC\n");
	}
}


// cf. "ATL Internals"
__inline HRESULT WINAPI _This(void *pv, REFIID iid, void **ppvObject, DWORD_PTR)
{
	ATLASSERT(iid == IID_NULL);
	*ppvObject = pv;
	return S_OK;
}


// Helper for creating default FORMATETC from cfFormat
LPFORMATETC _MtlFillFormatEtc(LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill);
bool		MtlIsDataAvailable(IDataObject *pDataObject, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);
bool		MtlGetDropFileName(IDataObject *pDataObject, CSimpleArray<CString> &arrFileNames);


// Implementation
//+++ bool	MtlGetHGlobalText(IDataObject *pDataObject, CString &strText, CLIPFORMAT cfFormat = CF_TEXT);
bool		MtlGetHGlobalText(IDataObject *pDataObject, CString &strText, CLIPFORMAT cfFormat = MTL_CF_TEXT);	//+++ UNICODEëŒçÙ



// Thanks to www.nakka.com
// Even IE creates a shortcut file for every link, what the hell is OLE.
template <class _InputStringIter>
HDROP MtlCreateDropFile(_InputStringIter __first, _InputStringIter __last)
{
	if (__first == __last)
		return NULL;

	//filename\0...\0filename\0...\0filename\0\0
	int 	nLen  = 0;

	_InputStringIter it;

	for (it = __first; it != __last; ++it) {
		nLen += ::lstrlen(*it);
		nLen += 1;					// for '\0' separator
	}

	nLen	+= 1;					// for the last '\0'

	HDROP	hDrop = (HDROP) ::GlobalAlloc( GHND, sizeof (DROPFILES) + nLen * sizeof (TCHAR) );
	if (hDrop == NULL)
		return NULL;

	LPDROPFILES 	lpDropFiles;
	lpDropFiles 		= (LPDROPFILES) ::GlobalLock(hDrop);
	lpDropFiles->pFiles = sizeof (DROPFILES);
	lpDropFiles->pt.x	= 0;
	lpDropFiles->pt.y	= 0;
	lpDropFiles->fNC	= FALSE;

  #ifdef _UNICODE
	lpDropFiles->fWide	= TRUE;
  #else
	lpDropFiles->fWide	= FALSE;
  #endif

	TCHAR * 		 psz   = (TCHAR *) (lpDropFiles + 1);

	for (it = __first; it != __last; ++it) {
		::lstrcpy( psz, (*it) );
		psz += ::lstrlen(*it) + 1;	// skip a '\0' separator
	}

	::GlobalUnlock(hDrop);
	return hDrop;
}


HDROP	MtlCreateDropFile(CSimpleArray<CString> &arrFiles);


/////////////////////////////////////////////////////////////////////////////
// CSafeArray class

typedef const SAFEARRAY   *LPCSAFEARRAY;
typedef const VARIANT	  *LPCVARIANT;


/////////////////////////////////////////////////////////////////////////////
// Helper for initializing CComSafeArray

bool	_MtlCompareSafeArrays(SAFEARRAY *parray1, SAFEARRAY *parray2);


/////////////////////////////////////////////////////////////////////////////
// CComSafeArray class

class CComSafeArray : public tagVARIANT {
	// Data members
	// Cache info to make element access (operator []) faster
	DWORD	m_dwElementSize;
	DWORD	m_dwDims;

public:
	//Constructors
	CComSafeArray();
	~CComSafeArray();
	CComSafeArray(const SAFEARRAY &saSrc, VARTYPE vtSrc);
	CComSafeArray(LPCSAFEARRAY pSrc, VARTYPE vtSrc);
	CComSafeArray(const CComSafeArray &saSrc);
	CComSafeArray(const VARIANT &varSrc);
	CComSafeArray(LPCVARIANT pSrc);

	operator LPVARIANT();
	operator LPCVARIANT() const;

	void	CreateOneDim(VARTYPE vtSrc, DWORD dwElements, const void *pvSrcData = NULL, long nLBound = 0);
	DWORD	GetOneDimSize();
	void	AccessData(void **ppvData);
	void	UnaccessData();

private:
	void	_SafeArrayInit(CComSafeArray *psa);
	void	Clear();

	DWORD	GetDim();
	DWORD	GetElemSize();

	// Operations
	void	Attach(VARIANT &varSrc);

	VARIANT Detach();

	// Assignment operators
	CComSafeArray &operator =(const CComSafeArray &saSrc);
	CComSafeArray &operator =(const VARIANT &varSrc);
	CComSafeArray &operator =(LPCVARIANT pSrc);
	CComSafeArray &operator =(const CComVariant &varSrc);


	// Comparison operators
	bool	operator==(const SAFEARRAY &saSrc) const;
	bool	operator==(LPCSAFEARRAY pSrc) const;
	bool	operator==(const CComSafeArray &saSrc) const;
	bool	operator==(const VARIANT &varSrc) const;
	bool	operator==(LPCVARIANT pSrc) const;
	bool	operator==(const CComVariant &varSrc) const;

	void	ResizeOneDim(DWORD dwElements);
	void	Create(VARTYPE vtSrc, DWORD dwDims, DWORD *rgElements);
	void	Create(VARTYPE vtSrc, DWORD dwDims, SAFEARRAYBOUND *rgsabound);
	void	AllocData();
	void	AllocDescriptor(DWORD dwDims);
	void	Copy(LPSAFEARRAY *ppsa);
	void	GetLBound(DWORD dwDim, long *pLbound);
	void	GetUBound(DWORD dwDim, long *pUbound);
	void	GetElement(long *rgIndices, void *pvData);
	void	PtrOfIndex(long *rgIndices, void **ppvData);
	void	PutElement(long *rgIndices, void *pvData);
	void	Redim(SAFEARRAYBOUND *psaboundNew);
	void	Lock();
	void	Unlock();
	void	Destroy();
	void	DestroyData();
	void	DestroyDescriptor();
};



/////////////////////////////////////////////////////////////////////////////
// Helper for iter to CComSafeArray

void	_MtlCreateOneDimArray(VARIANT &varSrc, DWORD dwSize);
void	_MtlCopyBinaryData(SAFEARRAY *parray, const void *pvSrc, DWORD dwSize);
void	MtlInitVariantFromArray(CComVariant &v, CSimpleArray<BYTE> &arrSrc);
void	MtlInitVariantFromItemIDList(CComVariant &v, LPCITEMIDLIST pidl);



////////////////////////////////////////////////////////////////////////////


}		// namespace MTL



#endif	// __MTLCOM_H__
