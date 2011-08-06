/**
 *	@file	MtlCom.cpp
 *	@brief	MTL : COMä÷åW
 */

#include "stdafx.h"
#include "MtlCom.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace MTL {



/// Helper for creating default FORMATETC from cfFormat
LPFORMATETC _MtlFillFormatEtc(LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill)
{
	ATLASSERT(lpFormatEtcFill != NULL);

	if (lpFormatEtc == NULL && cfFormat != 0) {
		lpFormatEtc 		  = lpFormatEtcFill;
		lpFormatEtc->cfFormat = cfFormat;
		lpFormatEtc->ptd	  = NULL;
		lpFormatEtc->dwAspect = DVASPECT_CONTENT;
		lpFormatEtc->lindex   = -1;
		lpFormatEtc->tymed	  = (DWORD) -1;
	}

	return lpFormatEtc;
}



bool MtlIsDataAvailable(IDataObject *pDataObject, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	ATLASSERT(pDataObject != NULL);

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	lpFormatEtc = MTL::_MtlFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);

	// attempt to get the data
	return pDataObject->QueryGetData(lpFormatEtc) == S_OK;
}



bool MtlGetDropFileName(IDataObject *pDataObject, CSimpleArray<CString> &arrFileNames)
{
	if ( !MTL::MtlIsDataAvailable(pDataObject, CF_HDROP) )
		return false;

	FORMATETC formatetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmedium;
	HRESULT   hr		= pDataObject->GetData(&formatetc, &stgmedium);

	if (FAILED(hr) || stgmedium.hGlobal == NULL) {
		return false;
	}

	HDROP	  hDropInfo = (HDROP) stgmedium.hGlobal;

	UINT	  nFiles	= ::DragQueryFile(hDropInfo, (UINT) -1, NULL, 0);

	for (UINT iFile = 0; iFile < nFiles; iFile++) {
		TCHAR szFileName[_MAX_PATH];
		szFileName[0]	= 0;
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		arrFileNames.Add( CString(szFileName) );
	}

	::DragFinish(hDropInfo); // required?

	::ReleaseStgMedium(&stgmedium);

	if (arrFileNames.GetSize() > 0)
		return true;
	else
		return false;
}



// Implementation
bool MtlGetHGlobalText(IDataObject *pDataObject, CString &strText, CLIPFORMAT cfFormat/* = MTL_CF_TEXT*/)
{
	bool	  bResult	= false;

	if ( !MTL::MtlIsDataAvailable(pDataObject, cfFormat) )
		return false;

	FORMATETC formatetc = { cfFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmedium = { 0 };
	HRESULT   hr		= pDataObject->GetData(&formatetc, &stgmedium);

	if ( SUCCEEDED(hr) ) {
		if (stgmedium.hGlobal != NULL) {
			HGLOBAL hText = stgmedium.hGlobal;
			strText = reinterpret_cast<LPTSTR>( ::GlobalLock(hText) );	//+++ LPSTR Å® LPTSTR
			::GlobalUnlock(hText);
			bResult = true;
		}

		::ReleaseStgMedium(&stgmedium);
	}

	return bResult;
}



HDROP MtlCreateDropFile(CSimpleArray<CString> &arrFiles)
{
	if (arrFiles.GetSize() == 0)
		return NULL;

	//filename\0...\0filename\0...\0filename\0\0
	int 	nLen  = 0;
	int 	i;
	for (i = 0; i < arrFiles.GetSize(); ++i) {
		nLen += arrFiles[i].GetLength();
		nLen += 1;							// for '\0' separator
	}

	nLen	+= 1;				// for the last '\0'

	HDROP	hDrop = (HDROP) ::GlobalAlloc( GHND, sizeof (DROPFILES) + nLen * sizeof (TCHAR) );
	if (hDrop == NULL)
		return NULL;

	LPDROPFILES lpDropFiles;
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

	TCHAR * 	psz   = (TCHAR *) (lpDropFiles + 1);

	for (i = 0; i < arrFiles.GetSize(); ++i) {
		::lstrcpy(psz, arrFiles[i]);
		psz += arrFiles[i].GetLength() + 1; 	// skip a '\0' separator
	}

	::GlobalUnlock(hDrop);
	return hDrop;
}



// Helper for initializing CComSafeArray
bool _MtlCompareSafeArrays(SAFEARRAY *parray1, SAFEARRAY *parray2)
{
	bool   bCompare 	  = false;

	// If one is NULL they must both be NULL to compare
	if (parray1 == NULL || parray2 == NULL) {
		return parray1 == parray2;
	}

	// Dimension must match and if 0, then arrays compare
	DWORD  	dwDim1		  = ::SafeArrayGetDim(parray1);
	DWORD  	dwDim2		  = ::SafeArrayGetDim(parray2);

	if (dwDim1 != dwDim2)
		return false;
	else if (dwDim1 == 0)
		return true;

	// Element size must match
	DWORD  	dwSize1		  = ::SafeArrayGetElemsize(parray1);
	DWORD  	dwSize2		  = ::SafeArrayGetElemsize(parray2);

	if (dwSize1 != dwSize2)
		return false;

	long* 	pLBound1 	  = NULL;
	long* 	pLBound2 	  = NULL;
	long* 	pUBound1 	  = NULL;
	long* 	pUBound2 	  = NULL;

	void* 	pData1		  = NULL;
	void* 	pData2		  = NULL;

	// Bounds must match
	ATLTRY(pLBound1 = new long[dwDim1]);
	ATLTRY(pLBound2 = new long[dwDim2]);
	ATLTRY(pUBound1 = new long[dwDim1]);
	ATLTRY(pUBound2 = new long[dwDim2]);

	size_t nTotalElements = 1;

	// Get and compare bounds
	for (DWORD dwIndex = 0; dwIndex < dwDim1; dwIndex++) {
		MtlCheckError( ::SafeArrayGetLBound(parray1, dwIndex + 1, &pLBound1[dwIndex]) );
		MtlCheckError( ::SafeArrayGetLBound(parray2, dwIndex + 1, &pLBound2[dwIndex]) );
		MtlCheckError( ::SafeArrayGetUBound(parray1, dwIndex + 1, &pUBound1[dwIndex]) );
		MtlCheckError( ::SafeArrayGetUBound(parray2, dwIndex + 1, &pUBound2[dwIndex]) );

		// Check the magnitude of each bound
		if (   pUBound1[dwIndex] - pLBound1[dwIndex]
			!= pUBound2[dwIndex] - pLBound2[dwIndex])
		{
			delete[] pLBound1;
			delete[] pLBound2;
			delete[] pUBound1;
			delete[] pUBound2;

			return false;
		}

		// Increment the element count
		nTotalElements *= pUBound1[dwIndex] - pLBound1[dwIndex] + 1;
	}

	// Access the data
	MtlCheckError( ::SafeArrayAccessData(parray1, &pData1) );
	MtlCheckError( ::SafeArrayAccessData(parray2, &pData2) );

	// Calculate the number of bytes of data and compare
	size_t nSize   = nTotalElements * dwSize1;
	int    nOffset = memcmp(pData1, pData2, nSize);
	bCompare = (nOffset == 0);

	// Release the array locks
	MtlCheckError( ::SafeArrayUnaccessData(parray1) );
	MtlCheckError( ::SafeArrayUnaccessData(parray2) );

	// Clean up bounds arrays
	delete[] pLBound1;
	delete[] pLBound2;
	delete[] pUBound1;
	delete[] pUBound2;

	return bCompare;
}



/////////////////////////////////////////////////////////////////////////////
// CComSafeArray class

///Constructors
CComSafeArray::CComSafeArray()
{
	_SafeArrayInit(this);
	vt = VT_EMPTY;
}



CComSafeArray::~CComSafeArray()
{
	Clear();
}



CComSafeArray::CComSafeArray(const SAFEARRAY &saSrc, VARTYPE vtSrc)
{
	_SafeArrayInit(this);
	vt				= (VARTYPE) (vtSrc | VT_ARRAY);
	MtlCheckError( ::SafeArrayCopy( (LPSAFEARRAY) &saSrc, &parray ) );
	m_dwDims		= GetDim();
	m_dwElementSize = GetElemSize();
}



CComSafeArray::CComSafeArray(LPCSAFEARRAY pSrc, VARTYPE vtSrc)
{
	_SafeArrayInit(this);
	vt				= (VARTYPE) (vtSrc | VT_ARRAY);
	MtlCheckError( ::SafeArrayCopy( (LPSAFEARRAY) pSrc, &parray ) );
	m_dwDims		= GetDim();
	m_dwElementSize = GetElemSize();
}



CComSafeArray::CComSafeArray(const CComSafeArray &saSrc)
{
	_SafeArrayInit(this);
	*this			= saSrc;
	m_dwDims		= GetDim();
	m_dwElementSize = GetElemSize();
}



CComSafeArray::CComSafeArray(const VARIANT &varSrc)
{
	_SafeArrayInit(this);
	*this			= varSrc;
	m_dwDims		= GetDim();
	m_dwElementSize = GetElemSize();
}



CComSafeArray::CComSafeArray(LPCVARIANT pSrc)
{
	_SafeArrayInit(this);
	*this			= pSrc;
	m_dwDims		= GetDim();
	m_dwElementSize = GetElemSize();
}



void CComSafeArray::_SafeArrayInit(CComSafeArray *psa)
{
	::memset( psa, 0, sizeof (*psa) );
}



void CComSafeArray::Clear()
{
	MTLVERIFY(::VariantClear(this) == NOERROR);
}



CComSafeArray::operator LPVARIANT()
{
	return this;
}



CComSafeArray::operator LPCVARIANT() const
{
	return this;
}



DWORD CComSafeArray::GetDim()
{
	return ::SafeArrayGetDim(parray);
}



DWORD CComSafeArray::GetElemSize()
{
	return ::SafeArrayGetElemsize(parray);
}



/// Operations
void CComSafeArray::Attach(VARIANT &varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	// Free up previous safe array if necessary
	Clear();

	// give control of data to CComSafeArray
	::memcpy( this, &varSrc, sizeof (varSrc) );
	varSrc.vt = VT_EMPTY;
}



VARIANT CComSafeArray::Detach()
{
	VARIANT varResult = *this;

	vt = VT_EMPTY;
	return varResult;
}



/// Assignment operators
CComSafeArray &CComSafeArray::operator =(const CComSafeArray &saSrc)
{
	ATLASSERT(saSrc.vt & VT_ARRAY);

	MtlCheckError( ::VariantCopy(this, (LPVARIANT) &saSrc) );
	return *this;
}



CComSafeArray &CComSafeArray::operator =(const VARIANT &varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	MtlCheckError( ::VariantCopy(this, (LPVARIANT) &varSrc) );
	return *this;
}



CComSafeArray &CComSafeArray::operator =(LPCVARIANT pSrc)
{
	ATLASSERT(pSrc->vt & VT_ARRAY);

	MtlCheckError( ::VariantCopy(this, (LPVARIANT) pSrc) );
	return *this;
}



CComSafeArray &CComSafeArray::operator =(const CComVariant &varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	MtlCheckError( ::VariantCopy(this, (LPVARIANT) &varSrc) );
	return *this;
}



// Comparison operators
bool CComSafeArray::operator ==(const SAFEARRAY &saSrc) const
{
	return _MtlCompareSafeArrays(parray, (LPSAFEARRAY) &saSrc);
}



bool CComSafeArray::operator ==(LPCSAFEARRAY pSrc) const
{
	return _MtlCompareSafeArrays(parray, (LPSAFEARRAY) pSrc);
}



bool CComSafeArray::operator ==(const CComSafeArray &saSrc) const
{
	if (vt != saSrc.vt)
		return false;

	return _MtlCompareSafeArrays(parray, saSrc.parray);
}



bool CComSafeArray::operator ==(const VARIANT &varSrc) const
{
	if (vt != varSrc.vt)
		return false;

	return _MtlCompareSafeArrays(parray, varSrc.parray);
}



bool CComSafeArray::operator ==(LPCVARIANT pSrc) const
{
	if (vt != pSrc->vt)
		return false;

	return _MtlCompareSafeArrays(parray, pSrc->parray);
}



bool CComSafeArray::operator ==(const CComVariant &varSrc) const
{
	if (vt != varSrc.vt)
		return false;

	return _MtlCompareSafeArrays(parray, varSrc.parray);
}



void CComSafeArray::CreateOneDim(VARTYPE vtSrc, DWORD dwElements, const void *pvSrcData, long nLBound)
{
	ATLASSERT(dwElements > 0);

	// Setup the bounds and create the array
	SAFEARRAYBOUND rgsabound;
	rgsabound.cElements = dwElements;
	rgsabound.lLbound	= nLBound;
	Create(vtSrc, 1, &rgsabound);

	// Copy over the data if neccessary
	if (pvSrcData != NULL) {
		void *pvDestData;
		AccessData(&pvDestData);
		memcpy(pvDestData, pvSrcData, GetElemSize() * dwElements);
		UnaccessData();
	}
}



DWORD CComSafeArray::GetOneDimSize()
{
	ATLASSERT(GetDim() == 1);

	long nUBound, nLBound;

	GetUBound(1, &nUBound);
	GetLBound(1, &nLBound);

	return nUBound + 1 - nLBound;
}



void CComSafeArray::ResizeOneDim(DWORD dwElements)
{
	ATLASSERT(GetDim() == 1);

	SAFEARRAYBOUND rgsabound;

	rgsabound.cElements = dwElements;
	rgsabound.lLbound	= 0;

	Redim(&rgsabound);
}



void CComSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, DWORD *rgElements)
{
	ATLASSERT(rgElements != NULL);

	// Allocate and fill proxy array of bounds (with lower bound of zero)
	SAFEARRAYBOUND *rgsaBounds = new SAFEARRAYBOUND[dwDims];

	for (DWORD dwIndex = 0; dwIndex < dwDims; dwIndex++) {
		// Assume lower bound is 0 and fill in element count
		rgsaBounds[dwIndex].lLbound   = 0;
		rgsaBounds[dwIndex].cElements = rgElements[dwIndex];
	}

	Create(vtSrc, dwDims, rgsaBounds);
	delete[] rgsaBounds;
}



void CComSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, SAFEARRAYBOUND *rgsabound)
{
	ATLASSERT(dwDims > 0);
	ATLASSERT(rgsabound != NULL);

	// Validate the VARTYPE for SafeArrayCreate call
	ATLASSERT( !(vtSrc & VT_ARRAY) );
	ATLASSERT( !(vtSrc & VT_BYREF) );
	ATLASSERT( !(vtSrc & VT_VECTOR) );
	ATLASSERT(vtSrc != VT_EMPTY);
	ATLASSERT(vtSrc != VT_NULL);

	// Free up old safe array if necessary
	Clear();

	ATLTRY( parray = ::SafeArrayCreate(vtSrc, dwDims, rgsabound) );

	if (parray == NULL) {
		ATLTRACE2(atlTraceDBProvider, 0, "CComSafeArray::Create Error : OOM\n");
		return;
	}

	vt				= unsigned short (vtSrc | VT_ARRAY);
	m_dwDims		= dwDims;
	m_dwElementSize = GetElemSize();
}



void CComSafeArray::AccessData(void **ppvData)
{
	MtlCheckError( ::SafeArrayAccessData(parray, ppvData) );
}



void CComSafeArray::UnaccessData()
{
	MtlCheckError( ::SafeArrayUnaccessData(parray) );
}



void CComSafeArray::AllocData()
{
	MtlCheckError( ::SafeArrayAllocData(parray) );
}



void CComSafeArray::AllocDescriptor(DWORD dwDims)
{
	MtlCheckError( ::SafeArrayAllocDescriptor(dwDims, &parray) );
}



void CComSafeArray::Copy(LPSAFEARRAY *ppsa)
{
	MtlCheckError( ::SafeArrayCopy(parray, ppsa) );
}



void CComSafeArray::GetLBound(DWORD dwDim, long *pLbound)
{
	MtlCheckError( ::SafeArrayGetLBound(parray, dwDim, pLbound) );
}



void CComSafeArray::GetUBound(DWORD dwDim, long *pUbound)
{
	MtlCheckError( ::SafeArrayGetUBound(parray, dwDim, pUbound) );
}



void CComSafeArray::GetElement(long *rgIndices, void *pvData)
{
	MtlCheckError( ::SafeArrayGetElement(parray, rgIndices, pvData) );
}



void CComSafeArray::PtrOfIndex(long *rgIndices, void **ppvData)
{
	MtlCheckError( ::SafeArrayPtrOfIndex(parray, rgIndices, ppvData) );
}



void CComSafeArray::PutElement(long *rgIndices, void *pvData)
{
	MtlCheckError( ::SafeArrayPutElement(parray, rgIndices, pvData) );
}



void CComSafeArray::Redim(SAFEARRAYBOUND *psaboundNew)
{
	MtlCheckError( ::SafeArrayRedim(parray, psaboundNew) );
}



void CComSafeArray::Lock()
{
	MtlCheckError( ::SafeArrayLock(parray) );
}



void CComSafeArray::Unlock()
{
	MtlCheckError( ::SafeArrayUnlock(parray) );
}



void CComSafeArray::Destroy()
{
	MtlCheckError( ::SafeArrayDestroy(parray) );
}



void CComSafeArray::DestroyData()
{
	MtlCheckError( ::SafeArrayDestroyData(parray) );
}



void CComSafeArray::DestroyDescriptor()
{
	MtlCheckError( ::SafeArrayDestroyDescriptor(parray) );
}



/////////////////////////////////////////////////////////////////////////////
// Helper for iter to CComSafeArray
void _MtlCreateOneDimArray(VARIANT &varSrc, DWORD dwSize)
{
	UINT nDim;

	// Clear VARIANT and re-create SafeArray if necessary
	if (varSrc.vt != (VT_UI1 | VT_ARRAY)
	   || ( nDim = ::SafeArrayGetDim(varSrc.parray) ) != 1)
	{
		MTLVERIFY(::VariantClear(&varSrc) == NOERROR);
		varSrc.vt		= VT_UI1 | VT_ARRAY;

		SAFEARRAYBOUND bound;
		bound.cElements = dwSize;
		bound.lLbound	= 0;
		ATLTRY( varSrc.parray = ::SafeArrayCreate(VT_UI1, 1, &bound) );

		if (varSrc.parray == NULL)
			ATLTRACE2(atlTraceDBProvider, 0, "MtlCheckError Error : OOM\n");

	} else {
		// Must redimension array if necessary
		long	lLower, lUpper;
		MtlCheckError( ::SafeArrayGetLBound(varSrc.parray, 1, &lLower) );
		MtlCheckError( ::SafeArrayGetUBound(varSrc.parray, 1, &lUpper) );

		// Upper bound should always be greater than lower bound
		long	lSize = lUpper - lLower;
		if (lSize < 0) {
			ATLASSERT(FALSE);
			lSize = 0;
		}

		if ( (DWORD) lSize != dwSize ) {
			SAFEARRAYBOUND	bound;
			bound.cElements = dwSize;
			bound.lLbound	= lLower;
			MtlCheckError( ::SafeArrayRedim(varSrc.parray, &bound) );
		}
	}
}



void _MtlCopyBinaryData(SAFEARRAY *parray, const void *pvSrc, DWORD dwSize)
{
	// Access the data, copy it and unaccess it.
	void *pDest;

	MtlCheckError( ::SafeArrayAccessData(parray, &pDest) );
	::memcpy(pDest, pvSrc, dwSize);
	MtlCheckError( ::SafeArrayUnaccessData(parray) );
}



void MtlInitVariantFromArray(CComVariant &v, CSimpleArray<BYTE> &arrSrc)
{
	int nSize = arrSrc.GetSize();

	// Set the correct type and make sure SafeArray can hold data
	_MtlCreateOneDimArray(v, (DWORD) nSize);

	// Copy the data into the SafeArray
	_MtlCopyBinaryData(v.parray, arrSrc.GetData(), (DWORD) nSize);
}



void MtlInitVariantFromItemIDList(CComVariant &v, LPCITEMIDLIST pidl)
{
	if (pidl != NULL) {
		// walk through entries in the list and accumulate their size

		UINT		  cbTotal	 = 0;
		SAFEARRAY *   psa		 = NULL;
		LPCITEMIDLIST pidlWalker = pidl;

		while (pidlWalker->mkid.cb) {
			cbTotal   += pidlWalker->mkid.cb;
			pidlWalker = (LPCITEMIDLIST) ( ( (LPBYTE) pidlWalker ) + pidlWalker->mkid.cb );
		}

		// add the base structure size
		cbTotal += sizeof (ITEMIDLIST);

		// get a safe array for them
		psa 	 = ::SafeArrayCreateVector(VT_UI1, 0, cbTotal);

		// copy it and set members
		if (psa != NULL) {
			::memcpy(psa->pvData, (LPBYTE) pidl, cbTotal);
			v.vt	 = VT_ARRAY | VT_UI1;
			v.parray = psa;
		}
	}
}


////////////////////////////////////////////////////////////////////////////


} // namespace MTL
