/**
 *	@file	ItemIDList.h
 *	@brief	アイテムＩＤリスト
 */
#pragma once


// too easy implementation
// no ref count, no other goodness, forgive me.
class CItemIDList;


// for debug
#ifdef _DEBUG
	const bool _Mtl_ItemIDList_traceOn = false;
	#define idlTRACE if (_Mtl_ItemIDList_traceOn) ATLTRACE
#else
	#define idlTRACE
#endif



class CItemIDList {
public:
  #ifdef _DEBUG
	// for checking leaks
	static int	d_m_nCount;
	#define _INCREMENT_COUNT()	++CItemIDList::d_m_nCount; if (_Mtl_ItemIDList_traceOn) ATLTRACE(_T("CItemIDList(%d)\n"), CItemIDList::d_m_nCount)
	#define _DECREMENT_COUNT()	--CItemIDList::d_m_nCount; if (_Mtl_ItemIDList_traceOn) ATLTRACE(_T("CItemIDList(%d)\n"), CItemIDList::d_m_nCount)
  #else
	#define _INCREMENT_COUNT()
	#define _DECREMENT_COUNT()
  #endif

	// Data members
	LPITEMIDLIST	m_pidl;

public:
	// Constructors
	CItemIDList();
	CItemIDList(const CItemIDList &idl);
	CItemIDList(LPCITEMIDLIST pidl);
	CItemIDList(LPCTSTR lpszPath); // Note. this is late

	// Destructor
	~CItemIDList();

	static void 			FreeIDList(LPITEMIDLIST pidl);

	// Attributes
	bool					IsNull();
	CString 				GetPath();
	CString 				GetUrl();			//+++ urlてか、無理やりpath文字列を取得.
	void					Attach(LPITEMIDLIST pidl);
	LPITEMIDLIST			Detach();
	operator				LPITEMIDLIST() const;

	LPITEMIDLIST *operator &();
	UINT					GetSize();

	// Operations
	void operator			+=(const CItemIDList &idl);
	void operator			-=(const CItemIDList &idl);

	// Operators
	CItemIDList &operator	=(const CItemIDList &idlSrc);
	CItemIDList &operator	=(LPCTSTR lpszPath);

	CItemIDList operator	+(const CItemIDList &idlSrc) const;
	CItemIDList operator	-(const CItemIDList &idlSrc) const;

	// Helpers
	static bool 			_CmpItemID(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);

private:
	void					Release();
	bool					Find(LPCTSTR lpszText);

	// Helpers
	static LPITEMIDLIST 	_Create(UINT cbSize);
	static void 			_Release(LPITEMIDLIST pidl);
	static UINT 			_GetSize(LPCITEMIDLIST pidl);
	static LPITEMIDLIST 	_Next(LPCITEMIDLIST pidl);
	static LPITEMIDLIST 	_ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	static LPITEMIDLIST 	_RelativePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	static LPITEMIDLIST 	_CopyItemIDList(LPCITEMIDLIST lpi);
	static void 			_Dump(LPCITEMIDLIST pidlSrc);
};



// Compare helpers
inline bool operator ==(const CItemIDList &s1, const CItemIDList &s2)
{
	return CItemIDList::_CmpItemID(s1, s2);
}



inline bool operator ==(const CItemIDList &s1, LPCITEMIDLIST s2)
{
	return CItemIDList::_CmpItemID(s1, s2);
}



inline bool operator ==(LPCITEMIDLIST s1, const CItemIDList &s2)
{
	return CItemIDList::_CmpItemID(s1, s2);
}



inline bool operator !=(const CItemIDList &s1, const CItemIDList &s2)
{
	return !CItemIDList::_CmpItemID(s1, s2);
}



inline bool operator !=(const CItemIDList &s1, LPCITEMIDLIST s2)
{
	return !CItemIDList::_CmpItemID(s1, s2);
}



inline bool operator !=(LPCITEMIDLIST s1, const CItemIDList &s2)
{
	return !CItemIDList::_CmpItemID(s1, s2);
}



template <class _Function>
_Function MtlForEachObject(const CItemIDList &idlFolder, _Function __f, bool bIncludeHidden = false)
{
	// get desktop
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr	   = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return __f;

	// get the folder
	CComPtr<IShellFolder> spFolder;
	hr = spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);

	if ( FAILED(hr) ) {
		CItemIDList idl;
		hr = ::SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idl);

		if ( FAILED(hr) )
			return __f;

		if (idl == idlFolder)	// check if same idl or not
			spFolder = spDesktopFolder;
		else
			return __f;
	}

	DWORD	grfFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;
	if (bIncludeHidden)
		grfFlags |= SHCONTF_INCLUDEHIDDEN;

	CComPtr<IEnumIDList>  spEnum;
	hr = spFolder->EnumObjects(NULL, grfFlags, &spEnum);

	if ( FAILED(hr) )
		return __f;

	LPITEMIDLIST		  pidl;
	ULONG				  celtFetched;

	while (spEnum->Next(1, &pidl, &celtFetched) == S_OK) {
		_INCREMENT_COUNT();
		ATLASSERT(pidl != NULL);

		CItemIDList   idlFile;
		idlFile.Attach(pidl);


		ULONG		  dwAttributes =  SFGAO_FOLDER;
		LPCITEMIDLIST pidls[]	   = { idlFile.m_pidl };
		HRESULT 	  hr		   = spFolder->GetAttributesOf(1, pidls, &dwAttributes);

		if ( FAILED(hr) )
			continue;

		bool		  bFolder	   = _check_flag(SFGAO_FOLDER, dwAttributes);	// check

		__f(spFolder, idlFolder, idlFile, bFolder);
	}

	return __f;
}


template <class _Function>
_Function MtlForEachFolder(const CItemIDList &idlFolder, _Function __f, bool bIncludeHidden = false)
{
	// get desktop
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr	   = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return __f;

	// get the folder
	CComPtr<IShellFolder> spFolder;
	hr = spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);

	if ( FAILED(hr) )
		return __f;

	DWORD				  grfFlags = bIncludeHidden ? SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN : SHCONTF_FOLDERS;

	CComPtr<IEnumIDList>  spEnum;
	hr = spFolder->EnumObjects(NULL, grfFlags, &spEnum);

	if ( FAILED(hr) )
		return __f;

	LPITEMIDLIST		  pidl;
	ULONG				  celtFetched;

	while (spEnum->Next(1, &pidl, &celtFetched) == S_OK) {
		_INCREMENT_COUNT();
		ATLASSERT(pidl != NULL);

		CItemIDList   idlFile;
		idlFile.Attach(pidl);

		ULONG		  dwAttributes = SFGAO_FOLDER;
		LPCITEMIDLIST pidls[]	   = { idlFile.m_pidl };
		spFolder->GetAttributesOf(1, pidls, &dwAttributes);

		if (dwAttributes & SFGAO_FOLDER)		// required
			__f(spFolder, idlFolder, idlFile);
	}

	return __f;
}


template <class _Function>
_Function MtlForEachFile(const CItemIDList &idlFolder, _Function __f, bool bIncludeHidden = false)
{
	// get desktop
	CComPtr<IShellFolder>	spDesktopFolder;

	HRESULT 	hr = ::SHGetDesktopFolder(&spDesktopFolder);
	if ( FAILED(hr) )
		return __f;

	// get the folder
	CComPtr<IShellFolder>	spFolder;

	hr = spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);
	if ( FAILED(hr) )
		return __f;

	DWORD					grfFlags = bIncludeHidden ? SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN : SHCONTF_NONFOLDERS;
	CComPtr<IEnumIDList>	spEnum;

	hr = spFolder->EnumObjects(NULL, grfFlags, &spEnum);
	if ( FAILED(hr) )
		return __f;

	LPITEMIDLIST			pidl;
	ULONG					celtFetched;

	while (spEnum->Next(1, &pidl, &celtFetched) == S_OK) {
		_INCREMENT_COUNT();
		ATLASSERT(pidl != NULL);

		CItemIDList   idlFile;
		idlFile.Attach(pidl);

		ULONG		  dwAttributes = SFGAO_FOLDER;
		LPCITEMIDLIST pidls[]	   = { idlFile.m_pidl };
		spFolder->GetAttributesOf(1, pidls, &dwAttributes);

		if ( !(dwAttributes & SFGAO_FOLDER) )	// required
			__f(spFolder, idlFolder, idlFile);
	}

	return __f;
}



bool		MtlGetDisplayName(IShellFolder *pFolder, LPCITEMIDLIST lpi, CString &rString, DWORD uFlags = SHGDN_NORMAL);
CString 	MtlGetDisplayName(LPCITEMIDLIST pidlRelative);
CItemIDList MtlSetDisplayName(IShellFolder *pFolder, LPCITEMIDLIST pidl,  LPCTSTR lpszName, HWND hWnd, DWORD uFlags = SHGDN_NORMAL);
bool		MtlGetShellFolder(LPCITEMIDLIST pidl, IShellFolder **ppFolder);


// Note. It would fail if not the ordinary path.
CItemIDList MtlGetFullItemIDList(IShellFolder *pFolder, LPCITEMIDLIST lpi);
int 		MtlGetSystemIconIndex(LPCITEMIDLIST lpi, UINT uFlags = SHGFI_SMALLICON);
bool		MtlGetAttributesOf(LPCITEMIDLIST pidlFull, ULONG &dwAttributes);


// Note. It is possible to make browsing slow.
bool		MtlHasSubObject(const CItemIDList &idlFolder, bool bIncludeHidden = false);


// Note. this is still not precise.
bool		MtlIsRealFolder(const CItemIDList &idlFolder);
bool		MtlShellExecute(HWND hWnd, LPCITEMIDLIST pidl);
bool		MtlIsTooSlowIDList(LPCITEMIDLIST pidl);
