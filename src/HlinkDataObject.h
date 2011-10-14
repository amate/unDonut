/**
 *	@file	HlinkDataObject.h
 *	@brief	ハイパーリンク・データ・オブジェクト
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
// TextDataObject.h: Last updated: February 20, 2001
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <atlmisc.h>
#include "MtlCom.h"
#include "MtlDragDrop.h"
#include "DonutPFunc.h"

namespace MTL {

const UINT	CF_SHELLURLW	= ::RegisterClipboardFormat(CFSTR_INETURLW);
const UINT	CF_DONUTURLLIST	= ::RegisterClipboardFormat(_T("DonutURLList"));

// for debug
#ifdef _DEBUG
	const bool _Mtl_HlinkDataObject_traceOn = false;
	#define HLDTRACE	if (_Mtl_HlinkDataObject_traceOn) ATLTRACE
#else
	#define HLDTRACE
#endif


//-------------------------------------------------
/// pDataObjectがリンクを示すものであればtrueを返す
inline bool _MtlIsHlinkDataObject(IDataObject *pDataObject)
{
	return (  MtlIsDataAvailable(pDataObject, CF_HDROP)
		   || MtlIsDataAvailable(pDataObject, MTL_CF_TEXT)		//+++ UNICODE対策(MTL_CF_TEXT)
		   || MtlIsDataAvailable(pDataObject, CF_SHELLURLW)
		   || MtlIsDataAvailable(pDataObject, CF_DONUTURLLIST) );
}

//-------------------------------------------------
/// pDataObjectからDonutURLListを返す
inline bool GetDonutURLList(IDataObject* pDataObject, std::vector<CString>&	vecUrl) 
{
	bool	bResult = false;
	FORMATETC formatetc = { CF_DONUTURLLIST, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmedium = { 0 };
	if ( SUCCEEDED(pDataObject->GetData(&formatetc, &stgmedium)) ) {
		if (stgmedium.hGlobal) {
			LPWSTR strList = reinterpret_cast<LPWSTR>(stgmedium.hGlobal);
			while (*strList) {
				CString strUrl = strList;
				vecUrl.push_back(strUrl);
				strList += strUrl.GetLength() + 1;				
			}
			bResult = true;
			delete (LPWSTR)stgmedium.hGlobal;
		}		
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CHlinkDataObject - リンクデータ用のIDataObject

class ATL_NO_VTABLE CHlinkDataObject
	: public CComCoClass<CHlinkDataObject, &CLSID_NULL>
	, public CComObjectRootEx<CComSingleThreadModel>
	, public IDataObjectImpl<CHlinkDataObject>
{
public:
	DECLARE_NO_REGISTRY()
private:
	DECLARE_NOT_AGGREGATABLE(CHlinkDataObject)

	BEGIN_COM_MAP(CHlinkDataObject)
		COM_INTERFACE_ENTRY(IDataObject)
		COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)
	END_COM_MAP()


public:
	// Data members
	CComPtr<IDataAdviseHolder>					m_spDataAdviseHolder;	// expected by IDataObjectImpl
	CSimpleArray< std::pair<CString, CString> > m_arrNameAndUrl;


	// Constructor
	CHlinkDataObject() : m_bInitialized(false)
	{
		HLDTRACE( _T("CHlinkDataObject::CHlinkDataObject\n") );
	}


	// Overrides
	HRESULT IDataObject_GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
	{
		HLDTRACE( _T("CHlinkDataObject::IDataObject_GetData\n") );

		if (pformatetcIn == NULL || pmedium == NULL)
			return E_POINTER;

		::memset( pmedium, 0, sizeof (STGMEDIUM) );

		if ( (pformatetcIn->tymed & TYMED_HGLOBAL) == 0 )
			return DATA_E_FORMATETC;

		pmedium->tymed			= TYMED_HGLOBAL;
		pmedium->pUnkForRelease = NULL;

		HGLOBAL hGlobal = NULL;
		static const UINT CF_FILENAME  = ::RegisterClipboardFormat(CFSTR_FILENAME);
		const UINT format = pformatetcIn->cfFormat;
		if (format == MTL_CF_TEXT || format == CF_SHELLURLW || format == CF_FILENAME) {
			hGlobal = _CreateText();
		} else if (format == CF_DONUTURLLIST) {
			hGlobal = _CreateDonutURLList();
		} else if (format == CF_HDROP) {
			// next, create shortcuts
			_InitFileNamesArrayForHDrop();
			// third, create dropfile
			hGlobal = (HGLOBAL) MtlCreateDropFile(m_arrFileNames);
			ATLASSERT(hGlobal != NULL);
		}

		if (hGlobal != NULL) {
			pmedium->hGlobal = hGlobal;
			return S_OK;
		} else
			return S_FALSE;
	}


private:
	// IDataObject
	STDMETHOD	(QueryGetData) (FORMATETC* pformatetc)
	{
		if (pformatetc == NULL)
			return E_POINTER;

		if (   pformatetc->cfFormat == CF_HDROP 
			|| pformatetc->cfFormat == MTL_CF_TEXT	//+++ UNICODE修正(MTL_CF_TEXT)
		    || pformatetc->cfFormat == CF_SHELLURLW 
			|| pformatetc->cfFormat == CF_DONUTURLLIST
			|| pformatetc->cfFormat == ::RegisterClipboardFormat(CFSTR_FILENAME) )
			return S_OK;
		else
			return DATA_E_FORMATETC;
	}

	STDMETHOD	(EnumFormatEtc) (DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
	{
		HLDTRACE( _T("CHlinkDataObject::EnumFormatEtc\n") );

		if (ppenumFormatEtc == NULL)
			return E_POINTER;

		if (dwDirection == DATADIR_SET)
			return S_FALSE;

		typedef CComEnum< IEnumFORMATETC, &IID_IEnumFORMATETC, FORMATETC, _Copy<FORMATETC> >  CEnumFormatEtc;

		CComObject<CEnumFormatEtc>* pEnumFormatEtc;
		HRESULT hr = CComObject<CEnumFormatEtc>::CreateInstance(&pEnumFormatEtc);
		if ( FAILED(hr) ) {
			HLDTRACE( _T(" CComObject<CEnumFormatEtc>::CreateInstance failed\n") );
			return S_FALSE;
		}

		pEnumFormatEtc->AddRef();	// preparing for the failure of the following QueryInterface

		{
			FORMATETC formatetcs[] = {
				{ CF_HDROP, 								 NULL,		DVASPECT_CONTENT,	-1,   TYMED_HGLOBAL },
				{ MTL_CF_TEXT,								 NULL,		DVASPECT_CONTENT,	-1,   TYMED_HGLOBAL },		//+++ UNICODE対策(MTL_CF_TEXT)
				{ CF_SHELLURLW,								 NULL,		DVASPECT_CONTENT,	-1,   0 			},
				{ CF_DONUTURLLIST,							 NULL,		DVASPECT_CONTENT,	-1,   0 			},
				{ ::RegisterClipboardFormat(CFSTR_FILENAME), NULL,		DVASPECT_CONTENT,	-1,   0 			},
			};
			hr = pEnumFormatEtc->Init(formatetcs, formatetcs + _countof(formatetcs), NULL, AtlFlagCopy);
			hr = pEnumFormatEtc->QueryInterface(IID_IEnumFORMATETC, (void **) ppenumFormatEtc);
		}

		pEnumFormatEtc->Release();

		return hr;
	}

	// Implementation

	/// ショートカットファイルを作成する
	void _InitFileNamesArrayForHDrop()
	{	// on demmand
		if (m_bInitialized)
			return;

		ATLASSERT(m_arrFileNames.GetSize() == 0);
		HLDTRACE( _T("CHlinkDataObject::_InitFileNamesArrayForHDrop\n") );

		CString strTempPath;
		if (GetDonutTempPath(strTempPath) == false) {
			MessageBox(NULL, _T("一時フォルダの作成に失敗"), NULL, MB_ICONERROR);
			return ;
		}

		MtlForEachFile(strTempPath, [](const CString& strFile) {
			if (MtlIsExt(strFile, _T(".url")))
				::DeleteFile(strFile);
		});

		CSimpleArray<CString> arrTmpFiles;

		for (int i = 0; i < m_arrNameAndUrl.GetSize(); ++i) {
			CString strName = m_arrNameAndUrl[i].first;
			MtlValidateFileName(strName);
			CString strUrl	= m_arrNameAndUrl[i].second;
			HLDTRACE(_T(" (%s, %s)\n"), strName, strUrl);

			if ( !strName.IsEmpty() ) { 		// not a local file
				strName = _CompactFileName(strTempPath, strName, _T(".url"));
				strName = _UniqueFileName(arrTmpFiles, strName);

				if ( _CreateInternetShortcutFile(strName, strUrl) )
					arrTmpFiles.Add(strName);
				else
					ATLASSERT(FALSE);

				m_arrFileNames.Add(strName);
			} else {							// local file
				m_arrFileNames.Add(strUrl);
			}
		}

		m_bInitialized = true;
	}


	static CString _CompactFileName(const CString &strDir, const CString &strFile, const CString &strExt)
	{
		int nRest = MAX_PATH - strDir.GetLength() - strExt.GetLength() - 5; // it's enough about 5

		ATLASSERT( nRest > 0 && _T("Your Application path is too deep") );
		return strDir + strFile.Left(nRest) + strExt;
	}

	HGLOBAL _CreateText()
	{
		if (m_arrNameAndUrl.GetSize() == 0)
			return NULL;

		const CString&	strText  = m_arrNameAndUrl[0].second;
		DWORD	size = (strText.GetLength() + 1) * sizeof(TCHAR);
		HGLOBAL	hMem = ::GlobalAlloc( GHND, size );
		if (hMem == NULL)
			return NULL;

		LPTSTR	lpszDest = (LPTSTR) ::GlobalLock(hMem);
		::lstrcpyn(lpszDest, strText, size);
		::GlobalUnlock(hMem);

		return hMem;
	}

	HGLOBAL	_CreateDonutURLList()
	{
		if (m_arrNameAndUrl.GetSize() == 0)
			return NULL;
		
		DWORD	dwSize = 0;
		int nCount = m_arrNameAndUrl.GetSize();
		for (int i = 0; i < nCount; ++i) {
			dwSize += (m_arrNameAndUrl[i].second.GetLength() + 1) * sizeof(WCHAR);
		}
		dwSize += sizeof(WCHAR);

		
		LPWSTR	lpszDest = new WCHAR[dwSize];
		HGLOBAL hMem = (HGLOBAL)lpszDest;
		LPWSTR	pEnd = lpszDest + dwSize;
		for (int i = 0; i < nCount; ++i) {
			const CString& strUrl = m_arrNameAndUrl[i].second;
			::_tcscpy_s(lpszDest, (pEnd -  lpszDest) / sizeof(WCHAR), strUrl);
			lpszDest += strUrl.GetLength() + 1;
		}
		*lpszDest = L'\0';
		::GlobalUnlock(hMem);
		return hMem;
	}


	bool _CreateInternetShortcutFile(const CString &strFileName, const CString &strUrl)
	{
		ATLASSERT(strFileName.GetLength() <= MAX_PATH);
		HLDTRACE(_T(" _CreateInternetShortcutFile(%s)\n"), strFileName);

		return MtlCreateInternetShortcutFile(strFileName, strUrl);

		// Note. I guess this function does'nt support Synchronization.
		// return ::WritePrivateProfileString(_T("InternetShortcut"), _T("URL"), strUrl, strFileName)
	}

	/// arrFileNamesに入ってるファイル名と被らないファイル名を返す
	CString _UniqueFileName(CSimpleArray<CString> &arrFileNames, const CString &strFileName)
	{
		CString strNewName = strFileName;
		CString strTmp;
		int 	i		   = 0;

		while (arrFileNames.Find(strNewName) != -1) {
			strTmp.Format(_T("%s[%d].%s"), Misc::GetFileBaseNoExt(strFileName), i, Misc::GetFileExt(strFileName));
			++i;
		}

		return strNewName;
	}


	// Data members
	bool					m_bInitialized;
	CSimpleArray<CString>	m_arrFileNames;
};


////////////////////////////////////////////////////////////////////////////



}		//namespace MTL



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE
