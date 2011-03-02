/**
 * @file	ItemIDList.cpp
 *	@brief	アイテムＩＤリスト
 */

#include "stdafx.h"
#include "ItemIDList.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




// Constructors
CItemIDList::CItemIDList()
	: m_pidl(NULL)
{
}



CItemIDList::CItemIDList(const CItemIDList &idl)
{
	m_pidl = _CopyItemIDList(idl);
}



CItemIDList::CItemIDList(LPCITEMIDLIST pidl)
{
	m_pidl = _CopyItemIDList(pidl);
}



CItemIDList::CItemIDList(LPCTSTR lpszPath) // Note. this is late
{
	m_pidl = NULL;

	HRESULT hr = ::SHILCreateFromPath(lpszPath, &m_pidl, NULL);
	// ATLASSERT(SUCCEEDED(hr));
	if ( FAILED(hr) ) {
		return;
	}
#if 0
	ULONG				  chEaten;

	USES_CONVERSION;

	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return;

	spDesktopFolder->ParseDisplayName(NULL, NULL, T2OLE( (LPTSTR) lpszPath ),
									  &chEaten, &m_pidl, NULL);
#endif
	_INCREMENT_COUNT();
}



// Destructor
CItemIDList::~CItemIDList()
{
	_Release(m_pidl);
}



void CItemIDList::Release()
{
	_Release(m_pidl);
	m_pidl = NULL;
}



void CItemIDList::FreeIDList(LPITEMIDLIST pidl)
{
	_Release(pidl);
}



// Attributes
bool CItemIDList::IsNull()
{
	return (m_pidl == NULL);
}



CString CItemIDList::GetPath()
{
	TCHAR szPath[MAX_PATH] = _T("\0");
	if ( ::SHGetPathFromIDList(m_pidl, szPath) )
		return szPath;
	return CString();
}



void CItemIDList::Attach(LPITEMIDLIST pidl)
{
	ATLASSERT(m_pidl == NULL);
	m_pidl = pidl;
}



LPITEMIDLIST CItemIDList::Detach()
{
	LPITEMIDLIST idl = m_pidl;

	m_pidl = NULL;
	return idl;
}



CItemIDList::operator LPITEMIDLIST() const
{
	return m_pidl;
}



LPITEMIDLIST *CItemIDList::operator &()
{
	ATLASSERT(m_pidl == NULL);
	_INCREMENT_COUNT();
	return &m_pidl;
}



UINT CItemIDList::GetSize()
{
	return _GetSize(m_pidl);
}



void CItemIDList::operator +=(const CItemIDList &idl)
{
	LPITEMIDLIST pidlNew = _ConcatPidls(m_pidl, idl);

	Release();
	m_pidl = pidlNew;
}



void CItemIDList::operator -=(const CItemIDList &idl)
{
	LPITEMIDLIST pidlNew = _RelativePidls(m_pidl, idl);

	Release();
	m_pidl = pidlNew;
}



bool CItemIDList::Find(LPCTSTR lpszText)
{
	int 		nLen = ::lstrlen(lpszText);

	if (nLen == 0)
		return false;

	int 		nSrc = _GetSize(m_pidl);

	if (nSrc < nLen)
		return false;

	const BYTE *p	 = (BYTE *) m_pidl;

	for (int i = 0; i < nSrc - nLen; ++i) {
		if (::memcmp( p + i, lpszText, nLen * sizeof (TCHAR) ) == 0) {
			return true;
		}
	}

	return false;
}



// Operators
CItemIDList &CItemIDList::operator =(const CItemIDList &idlSrc)
{
	Release();
	m_pidl = _CopyItemIDList(idlSrc);
	return *this;
}



CItemIDList &CItemIDList::operator =(LPCTSTR lpszPath)
{
	Release();

	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return *this;

	ULONG				  chEaten;

	USES_CONVERSION;

	hr = spDesktopFolder->ParseDisplayName(NULL, NULL, T2OLE( (LPTSTR) lpszPath ), &chEaten, &m_pidl, NULL);

	_INCREMENT_COUNT();
	return *this;
}



CItemIDList CItemIDList::operator +(const CItemIDList &idlSrc) const
{
	CItemIDList idl;

	idl.m_pidl = _ConcatPidls(m_pidl, idlSrc);
	return idl;
}



CItemIDList CItemIDList::operator -(const CItemIDList &idlSrc) const
{
	CItemIDList idl;

	idl.m_pidl = _RelativePidls(m_pidl, idlSrc);
	return idl;
}



#if 1	//+++ お試し....無意味だった...
CString CItemIDList::GetUrl()
{
	//IID_IContextMenu
  #if 1	//+++
	if (m_pidl == NULL)
		return CString();
	CComPtr<IShellFolder> spDesktop;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktop);
	if ( FAILED(hr) )
		return CString();
	CString 			  strName;
	if ( !MtlGetDisplayName(spDesktop, m_pidl, strName, SHGDN_FORADDRESSBAR/*|SHGDN_INFOLDER|SHGDN_FORPARSING*/) )
		return CString();

	return strName;
  #elif 1
	TCHAR szPath[MAX_PATH];

	szPath[0] = 0;
	if ( ::SHGetPathFromIDList(m_pidl, szPath) )
		return szPath;
	return CString();
  #elif 1
	if (m_pidl == NULL)
		return CString();
	CComPtr<IShellFolder> spDesktop;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktop);
	if ( FAILED(hr) )
		return CString();

	char	localName[ _MAX_PATH * 4 ];
	char	remoteName[ _MAX_PATH * 4 ];
	char	comment[ _MAX_PATH * 4 ];
	char	provider[ _MAX_PATH * 4 ];
	localName[0] = 0;
	remoteName[0] = 0;
	comment[0] = 0;
	provider[0] = 0;
	NETRESOURCE	netRes = {0};
    netRes.dwScope		 = RESOURCE_GLOBALNET;
    netRes.dwType		 = RESOURCETYPE_ANY;
    netRes.dwDisplayType = RESOURCEDISPLAYTYPE_NETWORK;
    netRes.dwUsage		 = RESOURCEUSAGE_ALL; //RESOURCEUSAGE_NOLOCALDEVICE;
    netRes.lpLocalName   = localName;
    netRes.lpRemoteName  = remoteName;
    netRes.lpComment     = comment;
    netRes.lpProvider    = provider;
	hr = ::SHGetDataFromIDList(spDesktop, m_pidl, SHGDFIL_NETRESOURCE, &netRes, sizeof netRes);
	return CString(netRes.lpLocalName);
  #elif 1
	SHFILEINFO	sfi = {0};
	// タイトルの拾得
	LPITEMIDLIST pCpy = _CopyItemIDList(m_pidl);
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktopFolder);
	if ( SUCCEEDED(hr) )
		MtlSetDisplayName(spDesktopFolder ,pCpy, "", NULL);
	DWORD 	rc = ::SHGetFileInfo( LPCTSTR(pCpy), 0, &sfi, sizeof(SHFILEINFO FAR), SHGFI_PIDL | SHGFI_TYPENAME | SHGFI_DISPLAYNAME );
	_Release(pCpy);
	CString str = sfi.szDisplayName;
	return str;
  #else
	CString			str;
	UINT 			cbTotal = 0;
	LPITEMIDLIST	pidl 	= m_pidl;
	if (pidl) {
		cbTotal += sizeof (pidl->mkid.cb);
		// Null terminator
		while (pidl->mkid.cb) {
			cbTotal += pidl->mkid.cb;
			str		+= "";//pidl
			pidl	 = _Next(pidl);
		}
	}
	return str;
  #endif
}
#endif



// Helpers
LPITEMIDLIST CItemIDList::_Create(UINT cbSize)
{
	LPITEMIDLIST pidl = (LPITEMIDLIST) ::CoTaskMemAlloc(cbSize);

	if (pidl == NULL)
		return NULL;

	_INCREMENT_COUNT();

	if (pidl)
		::memset(pidl, 0, cbSize);

				// zero-init for external task	 alloc

	return pidl;
}



void CItemIDList::_Release(LPITEMIDLIST pidl)
{
  #ifdef _DEBUG
	if (pidl != NULL) {
		_DECREMENT_COUNT();
	}
  #endif
	::CoTaskMemFree(pidl);
}



UINT CItemIDList::_GetSize(LPCITEMIDLIST pidl)
{				// include null terminator
	UINT cbTotal = 0;

	if (pidl) {
		cbTotal += sizeof (pidl->mkid.cb);

				// Null terminator
		while (pidl->mkid.cb) {
			cbTotal += pidl->mkid.cb;
			pidl	 = _Next(pidl);
		}
	}

	return cbTotal;
}



LPITEMIDLIST CItemIDList::_Next(LPCITEMIDLIST pidl)
{
	ATLASSERT(pidl != NULL);

	LPSTR lpMem = (LPSTR) pidl;
	lpMem += pidl->mkid.cb;
	return (LPITEMIDLIST) lpMem;
}



LPITEMIDLIST CItemIDList::_ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	UINT		 cb1;

	if (pidl1)	//May be NULL
		cb1 = _GetSize(pidl1) - sizeof (pidl1->mkid.cb);
	else
		cb1 = 0;

	UINT		 cb2	 = _GetSize(pidl2);

	LPITEMIDLIST pidlNew = _Create(cb1 + cb2);

	if (pidlNew) {
		if (pidl1)
			::memcpy(pidlNew, pidl1, cb1);

		::memcpy( ( (LPSTR) pidlNew ) + cb1, pidl2, cb2 );
		ATLASSERT(_GetSize(pidlNew) == cb1 + cb2);
	}

	return pidlNew;
}



LPITEMIDLIST CItemIDList::_RelativePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	//		ATLASSERT(pidl1 != NULL);
	if (pidl1 == NULL)
		return NULL;

	UINT		 cb1	 = _GetSize(pidl1);
	UINT		 cb2;

	if (pidl2)	//May be NULL
		cb2 = _GetSize(pidl2) - sizeof (pidl2->mkid.cb);
	else
		cb2 = 0;

	if (cb1 <= cb2)
		return NULL;

	LPITEMIDLIST pidlNew = _Create(cb1 - cb2);

	if (pidlNew) {
		if (pidl1)
			::memcpy(pidlNew, ( (LPSTR) pidl1 ) + cb2, cb1 - cb2);

		ATLASSERT(_GetSize(pidlNew) == cb1 - cb2);
	}

	return pidlNew;
}



LPITEMIDLIST CItemIDList::_CopyItemIDList(LPCITEMIDLIST lpi)
{
	if (lpi == NULL)
		return NULL;

	UINT		 cb 	 = _GetSize(lpi);
	LPITEMIDLIST lpiTemp = (LPITEMIDLIST) ::CoTaskMemAlloc(cb);

	if (lpiTemp == NULL)
		return NULL;

	_INCREMENT_COUNT();

	::memcpy(lpiTemp, lpi, cb);

	return lpiTemp;
}



bool CItemIDList::_CmpItemID(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	if (pidl1 == NULL && pidl2 != NULL)
		return false;

	if (pidl1 != NULL && pidl2 == NULL)
		return false;

	UINT cb1 = _GetSize(pidl2);
	UINT cb2 = _GetSize(pidl1);

	if (cb1 != cb2)
		return false;

	return ::memcmp(pidl1, pidl2, cb1) == 0;
}



void CItemIDList::_Dump(LPCITEMIDLIST pidlSrc)
{
	if	(pidlSrc == NULL)
		return;

	LPITEMIDLIST pidl = (LPITEMIDLIST) pidlSrc;

	while (pidl->mkid.cb) {
		idlTRACE( _T(" ") );

		for (int i = sizeof (pidl->mkid.cb); i < pidl->mkid.cb; ++i)
			idlTRACE( _T("%c"), *( (LPBYTE) pidl + i ) );

		pidl = _Next(pidl);
	}

	idlTRACE( _T("\n") );
}



#ifdef _DEBUG
int CItemIDList::d_m_nCount = 0;
#endif



// ============================================================================

bool MtlGetDisplayName(IShellFolder *pFolder, LPCITEMIDLIST lpi, CString &rString, DWORD uFlags)
{
	ATLASSERT(pFolder != NULL);
	ATLASSERT(lpi != NULL);

	STRRET	str;
	HRESULT hr = pFolder->GetDisplayNameOf(lpi, uFlags, &str);

	if ( FAILED(hr) )
		return false;

	USES_CONVERSION;

	switch (str.uType) {
	case STRRET_WSTR:
		rString = W2T(str.pOleStr);
		break;

	case STRRET_OFFSET:
		rString = (LPSTR) lpi + str.uOffset;
		break;

	case STRRET_CSTR:
		rString = (LPSTR) str.cStr;
		break;

	default:
		return false;
	}

	return true;
}



CString MtlGetDisplayName(LPCITEMIDLIST pidlRelative)
{ // The desktop folder can give a name to you even if a relative idl
	if (pidlRelative == NULL)
		return CString();

	CComPtr<IShellFolder> spDesktop;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktop);

	if ( FAILED(hr) )
		return CString();

	CString 			  strName;

	if ( !MtlGetDisplayName(spDesktop, pidlRelative, strName) )
		return CString();

	return strName;
}



CItemIDList MtlSetDisplayName(IShellFolder *pFolder, LPCITEMIDLIST pidl,
							  LPCTSTR lpszName, HWND hWnd, DWORD uFlags)
{
	CItemIDList idl;

	USES_CONVERSION;

	HRESULT 	hr = pFolder->SetNameOf(hWnd, pidl, T2COLE(lpszName), uFlags, &idl);

	if ( SUCCEEDED(hr) )
		return idl;

	return idl;
}



bool MtlGetShellFolder(LPCITEMIDLIST pidl, IShellFolder **ppFolder)
{
	ATLASSERT(pidl != NULL);

	CComPtr<IShellFolder> spDesktop;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktop);

	if ( FAILED(hr) )
		return false;

	hr = spDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (void **) ppFolder);

	if ( FAILED(hr) )
		return false;

	return true;
}



// Note. It would fail if not the ordinary path.
CItemIDList MtlGetFullItemIDList(IShellFolder *pFolder, LPCITEMIDLIST lpi)
{
	CString 			  strName;

	if ( !MtlGetDisplayName(pFolder, lpi, strName, SHGDN_FORPARSING) )
		return CItemIDList();

	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return CItemIDList();

	ULONG				  chEaten;
	CItemIDList 		  idlFolder;

	USES_CONVERSION;

	hr = spDesktopFolder->ParseDisplayName(NULL, NULL, T2OLE( (LPTSTR) (LPCTSTR) strName ), &chEaten, &idlFolder, NULL);
	if ( FAILED(hr) )
		return CItemIDList();

	return idlFolder;
}



int MtlGetSystemIconIndex(LPCITEMIDLIST lpi, UINT uFlags)
{
	ATLASSERT(lpi != NULL);
	//+++
	if (lpi == NULL)
		return -1;

	uFlags |= SHGFI_PIDL | SHGFI_SYSICONINDEX;

	SHFILEINFO sfi;
	DWORD_PTR	   dwResult = ::SHGetFileInfo( (LPCTSTR) lpi, 0, &sfi, sizeof (SHFILEINFO), uFlags );

	if (dwResult == 0)
		return -1;

	return sfi.iIcon;
}



bool MtlGetAttributesOf(LPCITEMIDLIST pidlFull, ULONG &dwAttributes)
{
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr	  = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return false;

	LPCITEMIDLIST		  pidls[] = { pidlFull };
	hr = spDesktopFolder->GetAttributesOf(1, pidls, &dwAttributes);

	if ( FAILED(hr) )
		return false;

	return true;
}



// Note. It is possible to make browsing slow.
bool MtlHasSubObject(const CItemIDList &idlFolder, bool bIncludeHidden)
{
	ATLASSERT(idlFolder.m_pidl != NULL);
	// get desktop
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr	   = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return false;

	// get the folder
	CComPtr<IShellFolder> spFolder;
	hr = spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);

	if ( FAILED(hr) )
		return false;

	DWORD				  grfFlags = bIncludeHidden ? SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN
								   :				  SHCONTF_FOLDERS | SHCONTF_NONFOLDERS ;

	CComPtr<IEnumIDList>  spEnum;
	hr = spFolder->EnumObjects(NULL, grfFlags, &spEnum);

	if ( FAILED(hr) )
		return false;

	LPITEMIDLIST		  pidl;
	ULONG				  celtFetched;

	while (spEnum->Next(1, &pidl, &celtFetched) == S_OK) {
		ATLASSERT(pidl != NULL);
		CItemIDList idl;
		idl.Attach(pidl);
		return true;
	}

	return false;
}



// Note. this is still not precise.
bool MtlIsRealFolder(const CItemIDList &idlFolder)
{
	ATLASSERT(idlFolder.m_pidl != NULL);
	// get desktop
	CComPtr<IShellFolder> spDesktopFolder;
	HRESULT 			  hr		   = ::SHGetDesktopFolder(&spDesktopFolder);

	if ( FAILED(hr) )
		return false;

	ULONG				  dwAttributes = SFGAO_FOLDER;
	LPCITEMIDLIST		  pidls[]	   = { idlFolder.m_pidl };
	hr = spDesktopFolder->GetAttributesOf(1, pidls, &dwAttributes);

	if (dwAttributes & SFGAO_FOLDER)
		return true;
	else
		return false;
}



bool MtlShellExecute(HWND hWnd, LPCITEMIDLIST pidl)
{
	ATLASSERT(pidl != NULL);

	SHELLEXECUTEINFO sei = {
		sizeof (SHELLEXECUTEINFO),
		SEE_MASK_INVOKEIDLIST,
		hWnd,
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		SW_SHOWNORMAL,
		_Module.GetModuleInstance(),
		(LPVOID) pidl,
		NULL,
		0,
		0,
		NULL
	};

	return ::ShellExecuteEx(&sei) == TRUE;
}



bool MtlIsTooSlowIDList(LPCITEMIDLIST pidl)
{
	TCHAR	szPath[MAX_PATH];
	szPath[0] = _T('\0');

	::SHGetPathFromIDList(pidl, szPath);
	CString 	strExt = CString(szPath).Right(4);
	return (strExt.CompareNoCase( _T(".url") ) == 0);
}




