/**
*	@file	LinkPopupMenu.cpp
*	@brief	リンクバー用のポップアップメニューウィンドウ
*/

#include "stdafx.h"
#include "LinkPopupMenu.h"
#include "MtlMisc.h"
#include <regex>
#include <boost/thread.hpp>
#include <atldlgs.h>
#include "GdiplusUtil.h"
#include "Donut.h"
#include "HlinkDataObject.h"
#include "FaviconManager.h"
#include "MainFrame.h"
#include "ExStyle.h"
#include "option\LinkBarPropertyPage.h"

//////////////////////////////////////////////////////
// CLinkItemDataObject

#pragma region CLinkItemDataObject

namespace {
const UINT CF_FILENAME		= ::RegisterClipboardFormat(CFSTR_FILENAME);
//const UINT CF_SHELLURLW		= ::RegisterClipboardFormat(CFSTR_INETURLW);
};

const UINT CF_DONUTLINKITEM	= ::RegisterClipboardFormat(_T("DonutLinkItem"));


CComPtr<IDataObject>	CLinkItemDataObject::CreateInstance(LinkFolderPtr pFolder, int nIndex)
{
	CComPtr<IDataObject>	spDataObject;
	CLinkItemDataObject::_CreatorClass::CreateInstance(NULL, IID_IDataObject, (void**)&spDataObject);
	CLinkItemDataObject*	pLinkItem;
	spDataObject->QueryInterface(IID_NULL, (void**)&pLinkItem);
	pLinkItem->m_pBaseFolder	= pFolder;
	pLinkItem->m_nIndex			= nIndex;
	pLinkItem->m_bItemIsFolder	= pFolder->at(nIndex)->pFolder != nullptr;
	return spDataObject;
}

// Constructor
CLinkItemDataObject::CLinkItemDataObject() : 
	m_bInitialized(false), m_rgde(NULL), m_cde(0)
{
}

CLinkItemDataObject::~CLinkItemDataObject()
{
    for (int ide = 0; ide < m_cde; ide++) {
        CoTaskMemFree(m_rgde[ide].fe.ptd);
        ReleaseStgMedium(&m_rgde[ide].stgm);
    }
    CoTaskMemFree(m_rgde);
}

bool	CLinkItemDataObject::IsAcceptDrag(IDataObject* pDataObject)
{
	return MtlIsDataAvailable(pDataObject, CF_SHELLURLW);
}

std::pair<CString, CString> CLinkItemDataObject::GetNameAndURL(IDataObject* pDataObject)
{
	CString name;
	CString url;

	// タブから
	FORMATETC formatetc = { CF_DONUTURLLIST, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (SUCCEEDED(pDataObject->QueryGetData(&formatetc))) {
		CHlinkDataObject*	pHlink;
		pDataObject->QueryInterface(IID_NULL, (void**)&pHlink);
		if (pHlink) {
			if (pHlink->m_arrNameAndUrl.GetSize() > 0) {
				name = pHlink->m_arrNameAndUrl[0].first;
				url	= pHlink->m_arrNameAndUrl[0].second;
			}
		}
		return std::make_pair(name, url);
	} 

	// IEのリンクをドロップ
	formatetc.cfFormat	= ::RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
	formatetc.lindex	= 0;
	formatetc.tymed	= TYMED_HGLOBAL;
	STGMEDIUM   stgMedium;
	HRESULT hr = pDataObject->GetData(&formatetc, &stgMedium);
	if (SUCCEEDED(hr)) {
		FILEGROUPDESCRIPTOR* pFgdis = (FILEGROUPDESCRIPTOR*)::GlobalLock(stgMedium.hGlobal);
		name = Misc::GetFileBaseNoExt(pFgdis->fgd[0].cFileName);
		MtlGetHGlobalText(pDataObject, url, CF_SHELLURLW);
		::GlobalUnlock(stgMedium.hGlobal);
		::ReleaseStgMedium(&stgMedium);
		return std::make_pair(name, url);
	}

	// Chromiumのリンクをドロップ
	formatetc.cfFormat	= ::RegisterClipboardFormat(_T("text/html"));
	formatetc.lindex	= -1;
	hr = pDataObject->GetData(&formatetc, &stgMedium);
	if (SUCCEEDED(hr) && stgMedium.hGlobal) {
		LPCSTR str = (LPCSTR)::GlobalLock(stgMedium.hGlobal);
		CString text = Misc::utf8_to_CString(str);
		
		std::wregex	rx(L"<a href=\"([^\"]+)\">([^<]+)</a>");
		std::wcmatch	result;
		if (std::regex_search((LPCWSTR)text, result, rx)) {
			url  = result[1].str().c_str();
			name = result[2].str().c_str();
		}
		::GlobalUnlock(stgMedium.hGlobal);
		::ReleaseStgMedium(&stgMedium);
	}

	return std::make_pair(name, url);
}

// Overrides

/// pformatetcIn であらわされるオブジェクトを渡す
HRESULT CLinkItemDataObject::IDataObject_GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	if (pformatetcIn == NULL || pmedium == NULL)
		return E_POINTER;

	::memset( pmedium, 0, sizeof (STGMEDIUM) );

	LPDATAENTRY pde;
	HRESULT hres = FindFORMATETC(pformatetcIn, &pde, FALSE);
	if (SUCCEEDED(hres)) {
		hres = AddRefStgMedium(&pde->stgm, pmedium, FALSE);
		if (SUCCEEDED(hres))
			return hres;
	}

	if ( (pformatetcIn->tymed & TYMED_HGLOBAL) == 0 )
		return DATA_E_FORMATETC;

	if (m_bItemIsFolder)
		return DATA_E_FORMATETC;

	pmedium->tymed			= TYMED_HGLOBAL;
	pmedium->pUnkForRelease = NULL;

	HGLOBAL hGlobal = NULL;
	const UINT format = pformatetcIn->cfFormat;
	if (format == CF_SHELLURLW || format == CF_FILENAME) {
		hGlobal = _CreateText();
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
		return DATA_E_FORMATETC;
}

// IDataObject
STDMETHODIMP	CLinkItemDataObject::QueryGetData(FORMATETC* pformatetc)
{
	if (pformatetc == NULL)
		return E_POINTER;

    LPDATAENTRY pde;
	if (   (pformatetc->cfFormat == CF_HDROP && !m_bItemIsFolder)
		|| (pformatetc->cfFormat == CF_SHELLURLW && !m_bItemIsFolder)
		|| (pformatetc->cfFormat == CF_FILENAME && !m_bItemIsFolder)
		|| pformatetc->cfFormat == CF_DONUTLINKITEM
		|| SUCCEEDED(FindFORMATETC(pformatetc, &pde, FALSE))
	   )
		return S_OK;
	else
		return DATA_E_FORMATETC;
}

STDMETHODIMP	CLinkItemDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{
	if (ppenumFormatEtc == NULL)
		return E_POINTER;

	if (dwDirection == DATADIR_SET)
		return S_FALSE;

	if (m_bItemIsFolder)
		return E_FAIL;

	typedef CComEnum< IEnumFORMATETC, &IID_IEnumFORMATETC, FORMATETC, _Copy<FORMATETC> >  CEnumFormatEtc;

	CComObject<CEnumFormatEtc>* pEnumFormatEtc;
	HRESULT hr = CComObject<CEnumFormatEtc>::CreateInstance(&pEnumFormatEtc);
	if ( FAILED(hr) ) {
		return S_FALSE;
	}

	pEnumFormatEtc->AddRef();	// preparing for the failure of the following QueryInterface

	{
		FORMATETC	formatetcs[] = {
			{ CF_HDROP		, NULL, DVASPECT_CONTENT,	-1,   TYMED_HGLOBAL },
			{ CF_SHELLURLW	, NULL, DVASPECT_CONTENT,	-1,   0 },
			{ CF_FILENAME	, NULL, DVASPECT_CONTENT,	-1,   TYMED_HGLOBAL },
		};

		hr = pEnumFormatEtc->Init(formatetcs, formatetcs + _countof(formatetcs), NULL, AtlFlagCopy);
		hr = pEnumFormatEtc->QueryInterface(IID_IEnumFORMATETC, (void **) ppenumFormatEtc);
	}

	pEnumFormatEtc->Release();

	return hr;
}

IUnknown* GetCanonicalIUnknown(IUnknown *punk)
{
    IUnknown *punkCanonical;
    if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown,
                                               (LPVOID*)&punkCanonical))) {
        punkCanonical->Release();
    } else {
        punkCanonical = punk;
    }
    return punkCanonical;
}


STDMETHODIMP	CLinkItemDataObject::SetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
    if (!fRelease) return E_NOTIMPL;

	FORMATETC* pfe = pFormatetc;
	STGMEDIUM* pstgm = pmedium;

    LPDATAENTRY pde;
    HRESULT hres = FindFORMATETC(pfe, &pde, TRUE);
    if (SUCCEEDED(hres)) {
        if (pde->stgm.tymed) {
            ReleaseStgMedium(&pde->stgm);
            ZeroMemory(&pde->stgm, sizeof(STGMEDIUM));
        }

        if (fRelease) {
            pde->stgm = *pstgm;
            hres = S_OK;
        } else {
            hres = AddRefStgMedium(pstgm, &pde->stgm, TRUE);
        }
        pde->fe.tymed = pde->stgm.tymed;    /* 同期で維持 */

        /* あいまい発生!  循環参照ループを抜ける  */
        if (GetCanonicalIUnknown(pde->stgm.pUnkForRelease) ==
            GetCanonicalIUnknown(static_cast<IDataObject*>(this))) {
            pde->stgm.pUnkForRelease->Release();
            pde->stgm.pUnkForRelease = NULL;
        }
    }
    return hres;
}


std::pair<LinkFolderPtr, int>	CLinkItemDataObject::GetFolderAndIndex()
{
	return std::pair<LinkFolderPtr, int>(m_pBaseFolder, m_nIndex);
}

// Implementation

HRESULT CLinkItemDataObject::FindFORMATETC(FORMATETC *pfe, LPDATAENTRY *ppde, BOOL fAdd)
{
    *ppde = NULL;

    /* 2 つの DVTARGETDEVICE 構造体の比較は難しいので行わない  */
    if (pfe->ptd != NULL) 
		return DV_E_DVTARGETDEVICE;

    /* リストに含まれているかどうかを確認 */
	int ide = 0;
    for (; ide < m_cde; ide++) {
        if (m_rgde[ide].fe.cfFormat == pfe->cfFormat &&
            m_rgde[ide].fe.dwAspect == pfe->dwAspect &&
            m_rgde[ide].fe.lindex == pfe->lindex) {
            if (fAdd || (m_rgde[ide].fe.tymed & pfe->tymed)) {
                *ppde = &m_rgde[ide];
                return S_OK;
            } else {
                return DV_E_TYMED;
            }
        }
    }

    if (!fAdd) 
		return DV_E_FORMATETC;

    LPDATAENTRY pdeT = (LPDATAENTRY)CoTaskMemRealloc(m_rgde,
                                        sizeof(DATAENTRY) * (m_cde+1));
    if (pdeT) {
        m_rgde = pdeT;
        m_cde++;
        m_rgde[ide].fe = *pfe;
        ZeroMemory(&pdeT[ide].stgm, sizeof(STGMEDIUM));
        *ppde = &m_rgde[ide];
        return S_OK;
    } else {
        return E_OUTOFMEMORY;
    }
}

HGLOBAL GlobalClone(HGLOBAL hglobIn)
{
    HGLOBAL hglobOut = NULL;

    LPVOID pvIn = GlobalLock(hglobIn);
    if (pvIn) {
        SIZE_T cb = GlobalSize(hglobIn);
        HGLOBAL hglobOut = GlobalAlloc(GMEM_FIXED, cb);
        if (hglobOut) {
            CopyMemory(hglobOut, pvIn, cb);
        }
        GlobalUnlock(hglobIn);
    }

    return hglobOut;
}

HRESULT CLinkItemDataObject::AddRefStgMedium(STGMEDIUM *pstgmIn, STGMEDIUM *pstgmOut, BOOL fCopyIn)
{
    HRESULT hres = S_OK;
    STGMEDIUM stgmOut = *pstgmIn;

    if (pstgmIn->pUnkForRelease == NULL &&
        !(pstgmIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE))) {
        if (fCopyIn) {
            /* オブジェクトのコピーが必要  */
            if (pstgmIn->tymed == TYMED_HGLOBAL) {
                stgmOut.hGlobal = GlobalClone(pstgmIn->hGlobal);
                if (!stgmOut.hGlobal) {
                    hres = E_OUTOFMEMORY;
                }
            } else {
                hres = DV_E_TYMED;      /* GDI オブジェクトのコピー方法がわからない */
            }
        } else {
            stgmOut.pUnkForRelease = static_cast<IDataObject*>(this);
        }
    }

    if (SUCCEEDED(hres)) {
        switch (stgmOut.tymed) {
        case TYMED_ISTREAM:
            stgmOut.pstm->AddRef();
            break;
        case TYMED_ISTORAGE:
            stgmOut.pstg->AddRef();
            break;
        }
        if (stgmOut.pUnkForRelease) {
            stgmOut.pUnkForRelease->AddRef();
        }

        *pstgmOut = stgmOut;
    }

    return hres;
}


/// ショートカットファイルを作成する
void CLinkItemDataObject::_InitFileNamesArrayForHDrop()
{	// on demmand
	if (m_bInitialized)
		return;

	ATLASSERT(m_bItemIsFolder == false);

	CString strTempPath;
	if (GetDonutTempPath(strTempPath) == false) {
		MessageBox(NULL, _T("一時フォルダの作成に失敗"), NULL, MB_ICONERROR);
		return ;
	}

	// Tempフォルダを掃除
	MtlForEachFile(strTempPath, [](const CString& strFile) {
		if (MtlIsExt(strFile, _T(".url")))
			::DeleteFile(strFile);
	});

	CSimpleArray<CString> arrTmpFiles;

	CString strName = m_pBaseFolder->at(m_nIndex)->strName;
	MtlValidateFileName(strName);
	CString strUrl	= m_pBaseFolder->at(m_nIndex)->strUrl;

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

	m_bInitialized = true;
}


CString CLinkItemDataObject::_CompactFileName(const CString &strDir, const CString &strFile, const CString &strExt)
{
	int nRest = MAX_PATH - strDir.GetLength() - strExt.GetLength() - 5; // it's enough about 5

	ATLASSERT( nRest > 0 && _T("Your Application path is too deep") );
	return strDir + strFile.Left(nRest) + strExt;
}

HGLOBAL CLinkItemDataObject::_CreateText()
{
	const CString&	strText  = m_pBaseFolder->at(m_nIndex)->strUrl;
	DWORD	size = (strText.GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL	hMem = ::GlobalAlloc( GHND, size );
	if (hMem == NULL)
		return NULL;

	LPTSTR	lpszDest = (LPTSTR) ::GlobalLock(hMem);
	::lstrcpyn(lpszDest, strText, size);
	::GlobalUnlock(hMem);

	return hMem;
}


bool CLinkItemDataObject::_CreateInternetShortcutFile(const CString &strFileName, const CString &strUrl)
{
	ATLASSERT(strFileName.GetLength() <= MAX_PATH);
	return MtlCreateInternetShortcutFile(strFileName, strUrl);

	// Note. I guess this function does'nt support Synchronization.
	// return ::WritePrivateProfileString(_T("InternetShortcut"), _T("URL"), strUrl, strFileName)
}

/// arrFileNamesに入ってるファイル名と被らないファイル名を返す
CString CLinkItemDataObject::_UniqueFileName(CSimpleArray<CString> &arrFileNames, const CString &strFileName)
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

#pragma endregion


/////////////////////////////////////////////////////////////////
/// ファイル名変更ダイアログ

namespace {

class CRenameDialog : public CDialogImpl<CRenameDialog>
{
public:
	enum { IDD = IDD_RENAMEDIALOG };
	
	// Constructor
	CRenameDialog(LinkItem& item) : m_rLinkItem(item)
	{
	}

	BEGIN_MSG_MAP( CRenameDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk)
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		GetDlgItem(IDC_EDIT).SetWindowText(m_rLinkItem.strName);
		return 0;
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString name = MtlGetWindowText(GetDlgItem(IDC_EDIT));
		if (name.IsEmpty()) {
			MessageBox(_T("名前を入力してください。"), NULL, MB_ICONERROR);
			return ;
		}
		m_rLinkItem.strName	= name;
		EndDialog(nID);
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(nID);
	}


private:
	LinkItem&	m_rLinkItem;
};

}	// namespace

/////////////////////////////////////////////////////////////////
/// リンク編集ダイアログ

class CLinkEditDialog : public CDialogImpl<CLinkEditDialog>
{
public:
	enum { IDD = IDD_LINKEDITDIALOG };
	
	// Constructor
	CLinkEditDialog(LinkItem& item, LinkFolderPtr pFolder, HWND hwnd) : 
		m_rLinkItem(item), m_pFolder(pFolder), m_hwnd(hwnd), m_bUseCustomIcon(false)
	{
	}

	BEGIN_MSG_MAP( CRenameDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk)
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( ID_GETFAVICONFROMFILE, OnGetFaviconFromFile )		
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		GetDlgItem(IDC_EDIT_NAME).SetWindowText(m_rLinkItem.strName);
		GetDlgItem(IDC_EDIT_URL).SetWindowText(m_rLinkItem.strUrl);
		SetIcon(m_rLinkItem.icon, FALSE);
		return 0;
	}

	void OnOk(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString name = MtlGetWindowText(GetDlgItem(IDC_EDIT_NAME));
		CString url	 = MtlGetWindowText(GetDlgItem(IDC_EDIT_URL));
		if (name.IsEmpty() || url.IsEmpty()) {
			CString msg;
			msg.Format(_T("%s を入力してください。"), name.IsEmpty() ? _T("名前") : _T("URL"));
			MessageBox(msg, NULL, MB_ICONERROR);
			return ;
		}
		if (m_bUseCustomIcon == false
			&& (url != m_rLinkItem.strUrl || m_rLinkItem.icon == NULL)) {
				CLinkPopupMenu::GetFaviconToLinkItem(url, m_pFolder, &m_rLinkItem, m_hwnd);
		}
		m_rLinkItem.strName	= name;
		m_rLinkItem.strUrl	= url;
		EndDialog(nID);
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(nID);
	}

	void OnGetFaviconFromFile(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CFileDialog dlg(TRUE, _T(""), NULL, OFN_PATHMUSTEXIST | OFN_EXPLORER,
    _T("画像 ファイル\0*.ico;*.png;*.jpg;*.jpeg;*.bmp\0すべてのファイル (*.*)\0*.*\0\0"));
		if (dlg.DoModal(m_hWnd) == IDOK) {
			unique_ptr<Gdiplus::Bitmap> img(Gdiplus::Bitmap::FromFile(dlg.m_szFileName));
			if (img) {
				Gdiplus::Graphics	graphics(m_hWnd);
				unique_ptr<Gdiplus::Bitmap> img16x16(new Gdiplus::Bitmap(16, 16, &graphics));
				Gdiplus::Graphics	graphicsTarget(img16x16.get());
				graphicsTarget.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);
				graphicsTarget.DrawImage(img.get(), 0, 0, 16, 16);

				HICON hIcon = NULL;
				img16x16->GetHICON(&hIcon);
				if (hIcon) {
					SetIcon(hIcon, FALSE);
					m_rLinkItem.icon = hIcon;
					m_bUseCustomIcon = true;
				}
			}
		}
	}


private:
	LinkItem&	m_rLinkItem;
	LinkFolderPtr m_pFolder;
	HWND		m_hwnd;
	bool	m_bUseCustomIcon;
};



///////////////////////////////////////////////////////////
// CLinkPopupMenu

HWND	CLinkPopupMenu::s_wndLinkBar = NULL;
bool	CLinkPopupMenu::s_bNoCloseBaseSubMenu	= false;

CIcon	CLinkPopupMenu::s_iconFolder;
CIcon	CLinkPopupMenu::s_iconLink;


CLinkPopupMenu::CLinkPopupMenu(LinkFolderPtr pFolder, int nInheritIndex /*= -1*/) : 
	m_pFolder(pFolder), 
	m_nHotIndex(-1),
	m_pSubMenu(nullptr),
	m_nInheritFolderIndex(nInheritIndex),
	m_bDrawDisableHot(false)
{
	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_font = lf.CreateFontIndirect();
}

HWND	CLinkPopupMenu::SetLinkBarHWND(HWND hWnd)
{
	HWND oldHwnd = s_wndLinkBar;
	s_wndLinkBar = hWnd;
	return oldHwnd;
}

int	CLinkPopupMenu::ComputeWindowWidth()
{
	int nMaxTextWidth = 0;
	for (auto it = m_pFolder->cbegin(); it != m_pFolder->cend(); ++it) {
		int nNameWidth = MTL::MtlComputeWidthOfText(it->get()->strName, m_font);
		if (nMaxTextWidth < nNameWidth)
			nMaxTextWidth = nNameWidth;
	}
	if (nMaxTextWidth == 0)
		nMaxTextWidth = kNoneTextWidth;
	if (kMaxMenuTextWidth < nMaxTextWidth)
		nMaxTextWidth = kMaxMenuTextWidth;
	nMaxTextWidth += kLeftTextPos + kTextMargin + kArrowWidth + (kBoundMargin * 2);
	return nMaxTextWidth;
}

int CLinkPopupMenu::ComputeWindowHeight()
{
	int nHeight = (kBoundMargin * 2) + (kBoundBorder * 2);
	if (m_pFolder->size() > 0)
		nHeight += kItemHeight * (int)m_pFolder->size();
	else
		nHeight += kItemHeight;
	return nHeight;
}

void	CLinkPopupMenu::OpenLink(const LinkItem& item, DWORD openFlag)
{
	if (item.bExPropEnable) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, 
			CDLControlOption::s_dwExtendedStyleFlags, 0, 
			item.dwExProp.get(), item.dwExPropOpt.get());
		int DLCtrl	= ExProp.GetDLControlFlags();
		int ExStyle	= ExProp.GetExtendedStyleFlags();
		int AutoRefresh = ExProp.GetAutoRefreshFlag();
		g_pMainWnd->UserOpenFile(item.strUrl, openFlag, DLCtrl, ExStyle, AutoRefresh);

	} else {
		g_pMainWnd->UserOpenFile(item.strUrl, openFlag);
	}
}

void	CLinkPopupMenu::ShowRClickMenuAndExecCommand(LinkFolderPtr pFolder, LinkItem* pLinkItem, HWND hwnd)
{
	s_bNowShowRClickMenu = true;

	CMenu	menuRoot;
	menuRoot.LoadMenu(IDM_LINKBAR);
	int nPos;
	if (pLinkItem == nullptr)
		nPos = 2;
	else if (pLinkItem->pFolder)
		nPos = 1;
	else
		nPos = 0;
	CMenu	menu =  menuRoot.GetSubMenu(nPos);

	CPoint pt;
	::GetCursorPos(&pt);
	int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, hwnd);
	switch (nCmd) {
	case ID_OPENLINK: 
		OpenLink(*pLinkItem, D_OPENFILE_NOCREATE);
		break;

	case ID_OPENLINKONNEWTAB:
		OpenLink(*pLinkItem, D_OPENFILE_CREATETAB | DonutGetStdOpenActivateFlag());
		break;

	case ID_EDITLINKITEM:
		{
			CLinkEditDialog	dlg(*pLinkItem, pFolder, hwnd);
			if (dlg.DoModal(hwnd) == IDOK)
				SaveLinkBookmark();
		}
		break;

	case ID_DELETELINKITEM:
		for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
			if (it->get() == pLinkItem) {
				if (pLinkItem->pFolder) {
					std::function<void (LinkFolderPtr)>	funcDelFolder;
					funcDelFolder	= [&funcDelFolder] (LinkFolderPtr pFolder) {
						for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
							if (it->get()->pFolder) {
								funcDelFolder(it->get()->pFolder);
								delete it->get()->pFolder;
							}
						}
					};
					funcDelFolder(pLinkItem->pFolder);
				}
				pFolder->erase(it);
				SaveLinkBookmark();
				break;
			}
		}
		break;

	case ID_EDITEXPROPERTY:
		{
			CExPropertyDialog	dlg(pLinkItem->strUrl, pLinkItem->bExPropEnable);
			if (pLinkItem->dwExProp && pLinkItem->dwExPropOpt)
				dlg.Init(pLinkItem->dwExProp.get(), pLinkItem->dwExPropOpt.get());
			if (dlg.DoModal(hwnd) == IDOK) {
				pLinkItem->dwExProp	= dlg.GetExProp();
				pLinkItem->dwExPropOpt = dlg.GetExPropOpt();
				pLinkItem->bExPropEnable = dlg.GetExProp() ? true : false;
				pLinkItem->strUrl = dlg.GetURL();
				SaveLinkBookmark();
			}
		}
		break;

	case ID_ADDLINK:
		{
			//unique_ptr<LinkItem>	pItem(new LinkItem);
			//CChildFrame* pChild = g_pMainWnd->GetActiveChildFrame();
			//if (pChild) {
			//	pItem->strName= pChild->GetTitle();
			//	pItem->strUrl = pChild->GetLocationURL();
			//}
			//CLinkEditDialog	dlg(*pItem, pFolder, hwnd);
			//if (dlg.DoModal(hwnd) == IDOK) {
			//	if (pLinkItem && pLinkItem->pFolder)
			//		pFolder = pLinkItem->pFolder;
			//	if (pFolder->size() > 0 && pFolder->back()->strName == _T("ChevronFolder")) {
			//		pFolder->insert(pFolder->begin() + (pFolder->size() - 1), std::move(pItem));
			//	} else {
			//		pFolder->push_back(std::move(pItem));
			//	}
			//	SaveLinkBookmark();
			//}
		}
		break;

	case ID_ADDFOLDER:
		{
			unique_ptr<LinkItem>	pItem(new LinkItem);
			pItem->strName	= _T("新しいフォルダ");
			CRenameDialog	dlg(*pItem);
			if (dlg.DoModal(hwnd) == IDOK) {
				pItem->pFolder	= new LinkFolder;
				pItem->icon.LoadIcon(IDI_FOLDER);
				if (pLinkItem && pLinkItem->pFolder)
					pFolder = pLinkItem->pFolder;
				if (pFolder->size() > 0 && pFolder->back()->strName == _T("ChevronFolder")) {
					pFolder->insert(pFolder->begin() + (pFolder->size() - 1), std::move(pItem));
				} else {
					pFolder->push_back(std::move(pItem));
				}
				SaveLinkBookmark();
			}
		}
		break;

	case ID_SORTBYNAME:
		if (pLinkItem == nullptr) {
			if ((CWindow(s_wndLinkBar).GetStyle() & WS_CHILD) == 0) { // メインメニューのお気に入り
				SortByName(pFolder);

			} else {
				int nChevronIndex = (int)pFolder->size() - 1;
				LinkFolderPtr pChevronFolder = pFolder->back()->pFolder;
				for (auto it = pChevronFolder->begin(); it != pChevronFolder->end(); ++it) {
					pFolder->push_back(std::move(*it));
				}
				delete pChevronFolder;
				pFolder->erase(pFolder->begin() + nChevronIndex);

				SortByName(pFolder);

				unique_ptr<LinkItem>	pItem(new LinkItem);
				pItem->strName	= _T("ChevronFolder");
				pItem->pFolder	= new LinkFolder;
				pFolder->push_back(std::move(pItem));
			}
		} else {
			ATLASSERT(pLinkItem->pFolder);
			SortByName(pLinkItem->pFolder);
		}
		SaveLinkBookmark();
		break;

	case ID_OPENALLLINKINFOLDER:
		ATLASSERT(pLinkItem->pFolder);
		for (auto it = pLinkItem->pFolder->begin(); it != pLinkItem->pFolder->end(); ++it) {
			LinkItem* pItem = it->get();
			if (pItem->pFolder)
				continue;
			OpenLink(*pItem, D_OPENFILE_CREATETAB);
		}
		break;

	case ID_EDITFOLDERNAME:
		{
			CRenameDialog	dlg(*pLinkItem);
			if (dlg.DoModal(hwnd) == IDOK)
				SaveLinkBookmark();
		}
		break;
	}
	s_bNowShowRClickMenu = false;
}

bool	CLinkPopupMenu::s_bNowShowRClickMenu = false;

void CLinkPopupMenu::SaveLinkBookmark()
{
	::SendMessage(s_wndLinkBar, WM_SAVELINKBOOKMARK, 0, 0);
}

void CLinkPopupMenu::SortByName(LinkFolderPtr pFolder)
{
	std::sort(pFolder->begin(), pFolder->end(), 
		[](const unique_ptr<LinkItem>& pItem1, const unique_ptr<LinkItem>& pItem2)-> bool {
			if ( (pItem1->pFolder && pItem2->pFolder)
				|| (pItem1->pFolder == nullptr && pItem2->pFolder == nullptr) ) {
				return ::StrCmpLogicalW(pItem1->strName, pItem2->strName) <= 0;
			} else {
				return  pItem1->pFolder != nullptr;
			}
	});
	for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
		if (it->get()->pFolder && it->get()->pFolder->size() > 0)
			SortByName(it->get()->pFolder);
	}
}

void	CLinkPopupMenu::GetFaviconToLinkItem(const CString& url, LinkFolderPtr pFolder, LinkItem* pItem, HWND hwnd)
{
	CString url2 = url;
	LinkFolderPtr pFolder2 = pFolder;
	LinkItem* pItem2 = pItem;
	HWND hwnd2 = hwnd;
	boost::thread td([=]() {
		HICON hIcon = CFaviconManager::GetFaviconFromURL(url2);
		if (hIcon) {
			for (auto it = pFolder2->begin(); it != pFolder2->end(); ++it) {
				if (it->get() == pItem2) {
					pItem2->icon = hIcon;
					::InvalidateRect(hwnd2, NULL, FALSE);
					SaveLinkBookmark();
					break;
				}
			}
		}
	});
	td.detach();
}

void CLinkPopupMenu::DoPaint(CDCHandle dc)
{
	CPoint ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom	+= ptOffset.y;

	CMemoryDC	memDC(dc, rcClient);

	HFONT hFontPrev = memDC.SelectFont(m_font);
	memDC.SetBkMode(TRANSPARENT);

	if (IsThemeNull() == false) {
		DrawThemeBackground(memDC, MENU_POPUPBACKGROUND, 0, &rcClient);
		CRect rcGutter;
		rcGutter.left = kBoundMargin + kIconWidth;
		rcGutter.right= rcGutter.left + kVerticalLineWidth;
		rcGutter.bottom= rcClient.bottom;
		DrawThemeBackground(memDC, MENU_POPUPGUTTER, 0, &rcGutter);

		for (auto it = m_pFolder->cbegin(); it != m_pFolder->cend(); ++it) {
			LinkItem& item = *it->get();
			int iState = MPI_NORMAL;
			if (item.state == item.kItemHot)
				iState = MPI_HOT;
			DrawThemeBackground(memDC, MENU_POPUPITEM, iState, &item.rcItem);

			POINT ptIcon;
			ptIcon.x	= kLeftIconMargin;
			ptIcon.y	= item.rcItem.top + kTopIconMargin;
			CIconHandle icon;
			if (item.pFolder) {
				icon = CLinkPopupMenu::s_iconFolder;
			} else {
				icon = item.icon;
				if (icon == NULL)
					icon = CLinkPopupMenu::s_iconLink;
			}
			icon.DrawIconEx(memDC, ptIcon, CSize(kcxIcon, kcyIcon));

			CRect	rcText = item.rcItem;
			rcText.left	= kLeftTextPos;
			rcText.right	-= kArrowWidth + kBoundMargin;
			DrawThemeText(memDC, MENU_POPUPITEM, MPI_NORMAL, item.strName, item.strName.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, 0, &rcText);
			if (item.pFolder) {
				CRect rcArrow = rcText;
				rcArrow.left	= rcText.right;
				rcArrow.right	= rcClient.right;
				DrawThemeBackground(memDC, MENU_POPUPSUBMENU, MSM_NORMAL, &rcArrow);
			}
		}
		if (m_bDrawDisableHot) {
			CRect rcNone = rcClient;
			rcNone.DeflateRect(kBoundMargin, kBoundMargin);
			DrawThemeBackground(memDC, MENU_POPUPITEM, MPI_DISABLEDHOT, &rcNone);
		}


		if (m_pFolder->size() == 0) {
			CRect	rcText;
			rcText.top	= kBoundMargin;
			rcText.left	= kLeftTextPos;
			rcText.right	= rcClient.right;
			rcText.bottom	= rcText.top + kItemHeight;

			LPCTSTR strNone = _T("(なし)");
			DrawThemeText(memDC, MENU_POPUPITEM, MPI_DISABLED, strNone, ::lstrlen(strNone), DT_SINGLELINE | DT_VCENTER, 0, &rcText);
		}
	} else {
		memDC.FillRect(rcClient, COLOR_3DFACE);
		for (auto it = m_pFolder->cbegin(); it != m_pFolder->cend(); ++it) {
			LinkItem& item = *it->get();
			if (item.state == item.kItemHot)
				memDC.FillRect(item.rcItem, COLOR_HIGHLIGHT);

			POINT ptIcon;
			ptIcon.x	= kLeftIconMargin;
			ptIcon.y	= item.rcItem.top + kTopIconMargin;
			CIconHandle icon;
			if (item.pFolder) {
				icon = CLinkPopupMenu::s_iconFolder;
			} else {
				icon = item.icon;
				if (icon == NULL)
					icon = CLinkPopupMenu::s_iconLink;
			}
			icon.DrawIconEx(memDC, ptIcon, CSize(kcxIcon, kcyIcon));

			CRect	rcText = item.rcItem;
			rcText.left	= kLeftTextPos;
			rcText.right	-= kArrowWidth + kBoundMargin;
			memDC.SetTextColor(item.state == item.kItemHot ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_MENUTEXT));
			memDC.DrawText(item.strName, item.strName.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
			if (item.pFolder) {
				CRect rcArrow = rcText;
				rcArrow.left	= rcText.right;
				rcArrow.right	= rcClient.right;
				enum { kcxArrow = 12, kcyArrow = 12 };
				CDC	memDC2 = ::CreateCompatibleDC(memDC);
				
				CBitmap bmpArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
				HBITMAP hbmpPrev = memDC2.SelectBitmap(bmpArrow);
				UINT nState = DFCS_MENUARROW;
				memDC2.DrawFrameControl(CRect(0, 0, kcxArrow, kcyArrow), DFC_MENU, DFCS_MENUARROW);
				if (item.state == item.kItemHot) {
					CBitmap bmpWiteArrow = ::CreateCompatibleBitmap(memDC, kcxArrow, kcyArrow);
					CDC	memDC3 = ::CreateCompatibleDC(memDC);
					HBITMAP hbmpPrev2 = memDC3.SelectBitmap(bmpWiteArrow);
					memDC3.BitBlt(0, 0, kcxArrow, kcyArrow, memDC2, 0, 0, NOTSRCCOPY);

					memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC3, 0, 0, kcxArrow, kcyArrow, RGB(0, 0, 0));
					memDC3.SelectBitmap(hbmpPrev2);
				} else {
					memDC.TransparentBlt(rcClient.right - kcxArrow - 4, rcText.top + ((rcText.Height() - kcyArrow) / 2), kcxArrow, kcyArrow, memDC2, 0, 0, kcxArrow, kcyArrow, RGB(0xFF, 0xFF, 0xFF));
				}
				memDC2.SelectBitmap(hbmpPrev);
			}
		}
		if (m_pFolder->size() == 0) {
			CRect	rcText;
			rcText.top	= kBoundMargin;
			rcText.left	= kLeftTextPos;
			rcText.right	= rcClient.right;
			rcText.bottom	= rcText.top + kItemHeight;

			LPCTSTR strNone = _T("(なし)");
			memDC.DrawText(strNone, ::lstrlen(strNone), rcText, DT_SINGLELINE | DT_VCENTER);
		}
	}


	memDC.SelectFont(hFontPrev);
}

void	CLinkPopupMenu::_DrawDragImage(CDCHandle dc, const LinkItem& item)
{
	POINT ptIcon;
	ptIcon.x	= kLeftIconMargin;
	ptIcon.y	= item.rcItem.top + kTopIconMargin;
	CIconHandle icon;
	if (item.pFolder) {
		icon = CLinkPopupMenu::s_iconFolder;
	} else {
		icon = item.icon;
		if (icon == NULL)
			icon = CLinkPopupMenu::s_iconLink;
	}
	icon.DrawIconEx(dc, ptIcon, CSize(kcxIcon, kcyIcon));

	CRect	rcText = item.rcItem;
	rcText.left	= kLeftTextPos - 2;
	rcText.right-= kArrowWidth + kBoundMargin;
	rcText.top	+= 2;
	Gdiplus::Graphics	graphics(dc);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	Gdiplus::Font	font(dc, m_font);
	Gdiplus::SolidBrush	fontColor(Gdiplus::Color(255, 0, 0, 0));
	graphics.DrawString(item.strName, item.strName.GetLength(), &font, Gdiplus::PointF(Gdiplus::REAL(rcText.left), Gdiplus::REAL(rcText.top)), NULL, &fontColor);
}

void CLinkPopupMenu::DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll, int cxySizePage, int cxySizeLine)
{
	_CloseSubMenu();
	__super::DoScroll(nType, nScrollCode, cxyOffset, cxySizeAll, cxySizePage, cxySizeLine);
}

/// カーソル上のアイテムをHOT状態にする
void CLinkPopupMenu::OnTrackMouseMove(UINT nFlags, CPoint pt)
{
	if (s_bNowShowRClickMenu)
		return ;

	int nIndex = -1;
	if (auto value = _HitTest(pt)) 
		nIndex = value.get();

	if ((m_nHotIndex == -1 || m_nHotIndex != nIndex)) {
		m_tip.Activate(FALSE);
		m_tip.Activate(TRUE);
	}

	if (nIndex != -1) {
		if (nIndex != m_nHotIndex) {
			_HotItem(nIndex);
			SetTimer(kSubMenuPopupTimerID, kSubMenuPopupTime);
		}
	}
}

/// アイテムのHOT状態を解除する
void CLinkPopupMenu::OnTrackMouseLeave()
{
	if (s_bNowShowRClickMenu)
		return ;

	KillTimer(kSubMenuPopupTimerID);
	if (m_pSubMenu) {	// カーソルが出て行ったときにサブメニューを閉じる
		CPoint	pt;
		GetCursorPos(&pt);
		HWND hWnd = WindowFromPoint(pt);
		if (hWnd != m_pSubMenu->m_hWnd) {
			SetTimer(kSubMenuPopupCloseTimerID, kSubMenuPopupCloseTime);
		} else {
			_HotItem(m_pSubMenu->m_nInheritFolderIndex);
			return ;
		}
	}
	_HotItem(-1);
}

// IDropTargetImpl

DROPEFFECT CLinkPopupMenu::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	m_bAcceptDrag = m_bDragItemIsLinkFile = CLinkItemDataObject::IsAcceptDrag(pDataObject);

	FORMATETC fmt = { 0 };
	fmt.cfFormat	= CF_DONUTLINKITEM;
	if (SUCCEEDED(pDataObject->QueryGetData(&fmt))) {
		CLinkItemDataObject*	pLinkItem;
		pDataObject->QueryInterface(IID_NULL, (void**)&pLinkItem);
		m_DragItemData = pLinkItem->GetFolderAndIndex();
		m_bAcceptDrag = true;
		m_bDragItemIsLinkFile = false;
		return _MtlStandardDropEffect(dwKeyState);
	}

	return DROPEFFECT_NONE;
}

DROPEFFECT CLinkPopupMenu::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (m_bAcceptDrag == false)
		return DROPEFFECT_NONE;

	_ClearInsertionEdge();

	_HotItem(-1);

	hitTestFlag htflag;
	int nIndex = _HitTestOnDragging(point, htflag);
	if (nIndex == -1) {
		_CloseSubMenu();
		return DROPEFFECT_NONE;
	}

	if (m_pFolder == m_DragItemData.first && nIndex == m_DragItemData.second) {
		s_bNoCloseBaseSubMenu = true;
		return DROPEFFECT_NONE;
	}

	if (htflag == htFolder) {
		_HotItem(nIndex);
		_DoExec(point);

	} else if (htflag == htItemNone) {
		if (m_bDrawDisableHot == false) {
			m_bDrawDisableHot = true;
			Invalidate(FALSE);
		}

	} else {
		LinkItem& item = *m_pFolder->at(nIndex);
		_CloseSubMenu();

		CRect rcEdge;
		rcEdge.left = item.rcItem.left;
		rcEdge.right= item.rcItem.right;
		switch (htflag) {
		case htItemTop:		rcEdge.top = item.rcItem.top;		break;
		case htItemBottom:	rcEdge.top = item.rcItem.bottom - 2;	break;
		}

		_DrawInsertEdge(_GetClientItemRect(rcEdge));

		if (m_rcInvalidateOnDrawingInsertionEdge != rcEdge) {
			_ClearInsertionEdge();
			m_rcInvalidateOnDrawingInsertionEdge = rcEdge;
		}
	}
	return DROPEFFECT_MOVE;

}

DROPEFFECT CLinkPopupMenu::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	if (m_bAcceptDrag == false)
		return DROPEFFECT_NONE;

	_ClearInsertionEdge();

	hitTestFlag htflag;
	int nIndex = _HitTestOnDragging(point, htflag);
	if (nIndex == -1)
		return DROPEFFECT_NONE;

	auto nameUrl = CLinkItemDataObject::GetNameAndURL(pDataObject);

	LinkItem* pLinkItemForFavicon = nullptr;
	unique_ptr<LinkItem> pItem;
	if (m_bDragItemIsLinkFile) {
		pItem.reset(new LinkItem);
		pItem->strName	= nameUrl.first;
		pItem->strUrl	= nameUrl.second;
		pLinkItemForFavicon = pItem.get();
		//pItem->icon = CFaviconManager::GetFaviconFromURL(pItem->strUrl);
	} else {
		pItem.reset(m_DragItemData.first->at(m_DragItemData.second).release());
	}
	if (htflag == htFolder) {		
		LinkItem& item = *m_pFolder->at(nIndex);
		ATLASSERT(item.pFolder);
		item.pFolder->push_back(std::move(pItem));
		if (m_bDragItemIsLinkFile == false)
			m_DragItemData.first->erase(m_DragItemData.first->begin() + m_DragItemData.second);
		else
			CLinkPopupMenu::GetFaviconToLinkItem(pLinkItemForFavicon->strUrl, item.pFolder, pLinkItemForFavicon, m_hWnd);

		::SendMessage(s_wndLinkBar, WM_UPDATESUBMENUITEMPOS, 0, 0);
		//_UpdateItemPosition();
		s_bNoCloseBaseSubMenu = true;
		_CloseSubMenu();
		_HotItem(-1);
		// メニューを閉じる
		//_CloseBaseSubMenu();

	} else if (htflag == htItemNone) {
		m_pFolder->insert(m_pFolder->begin(), std::move(pItem));
		if (m_bDragItemIsLinkFile == false)
			m_DragItemData.first->erase(m_DragItemData.first->begin() + m_DragItemData.second);
		else
			CLinkPopupMenu::GetFaviconToLinkItem(pLinkItemForFavicon->strUrl, m_pFolder, pLinkItemForFavicon, m_hWnd);
		::InvalidateRect(s_wndLinkBar, NULL, FALSE);

		// メニューを閉じる
		_CloseBaseSubMenu();

	} else {
		_CloseSubMenu();
		if (htflag == htItemBottom)
			++nIndex;
		// 同じサブメニュー内のアイテムの移動
		if (m_pFolder == m_DragItemData.first) {
			if (m_DragItemData.second < nIndex) {	// ドラッグしてるアイテムの右側に挿入される
				m_pFolder->insert(m_pFolder->begin() + nIndex, std::move(pItem));
				m_pFolder->erase(m_pFolder->begin() + m_DragItemData.second);
			} else {
				m_pFolder->insert(m_pFolder->begin() + nIndex, std::move(pItem));
				m_pFolder->erase(m_pFolder->begin() + m_DragItemData.second + 1);	// 一つ増えたのでずらす
			}
			_UpdateItemPosition();
			s_bNoCloseBaseSubMenu = true;

		} else {
			m_pFolder->insert(m_pFolder->begin() + nIndex, std::move(pItem));
			if (m_bDragItemIsLinkFile == false)
				m_DragItemData.first->erase(m_DragItemData.first->begin() + m_DragItemData.second);
			else
				CLinkPopupMenu::GetFaviconToLinkItem(pLinkItemForFavicon->strUrl, m_pFolder, pLinkItemForFavicon, m_hWnd);

			CRect rcWork = Misc::GetMonitorWorkArea(m_hWnd);
			CRect rcWindow;
			GetWindowRect(&rcWindow);
			bool	bScrollBarVisible = false;
			if (rcWindow.right == rcWork.right)
				bScrollBarVisible = true;
			rcWindow.bottom	= rcWindow.top	+ ComputeWindowHeight();
			rcWindow.right	= rcWindow.left	+ ComputeWindowWidth();
			
			if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
				rcWindow.bottom	= rcWork.bottom;
				if (bScrollBarVisible == false)
					rcWindow.right += ::GetSystemMetrics(SM_CXVSCROLL);
			}
			if (rcWork.right < rcWindow.right) {	// 右にはみ出る
				int nWidth = rcWindow.Width();
				rcWindow.right = rcWork.right;
			}
			if ((GetStyle() & WS_CHILD) == 0)
				MoveWindow(&rcWindow);
			enum { kBorderMargin = 2 };
			SetScrollSize(rcWindow.Width() - kBorderMargin, ComputeWindowHeight() - kBorderMargin);
			SetScrollLine(1, kItemHeight);

			::InvalidateRect(s_wndLinkBar, NULL, FALSE);

			::SendMessage(s_wndLinkBar, WM_UPDATESUBMENUITEMPOS, 0, 0);
			//_UpdateItemPosition();
			s_bNoCloseBaseSubMenu = true;
			// メニューを閉じる
			//_CloseBaseSubMenu();
		}
	}
	SaveLinkBookmark();

	return DROPEFFECT_MOVE;
}

void	CLinkPopupMenu::OnDragLeave()
{
	_ClearInsertionEdge();

	if (m_pSubMenu) {
		CPoint pt;
		::GetCursorPos(&pt);
		HWND hWnd = ::WindowFromPoint(pt);
		if (m_pSubMenu->m_hWnd == hWnd)
			return ;
	}

	_HotItem(-1);

	if (m_bDrawDisableHot) {
		Invalidate(FALSE);
		m_bDrawDisableHot = false;
	}
}




int CLinkPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	OpenThemeData(VSCLASS_MENU);

	enum { kBorderMargin = 2 };
	SetScrollSize(ComputeWindowWidth() - kBorderMargin, ComputeWindowHeight() - kBorderMargin);
    SetScrollLine(1, kItemHeight);

	_InitTooltip();

	_UpdateItemPosition();

	ATLVERIFY(SUCCEEDED(RegisterDragDrop()));

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

void CLinkPopupMenu::OnDestroy()
{
	//std::for_each(m_pFolder->begin(), m_pFolder->end(), [this](unique_ptr<LinkItem>& pItem) {
	//	pItem->state = pItem->kItemNormal;
	//});

	RevokeDragDrop();

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
}

void CLinkPopupMenu::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kSubMenuPopupCloseTimerID) {
		KillTimer(nIDEvent);
		if (m_pSubMenu) {
			m_pSubMenu->DestroyWindow();
			m_pSubMenu = nullptr;
		}
	} else if (nIDEvent == kSubMenuPopupTimerID) {
		KillTimer(nIDEvent);
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		_DoExec(pt);
	}
}


void CLinkPopupMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE) {
		TRACEIN(_T("OnKeyDown : VK_ESCAPE"));
	}
}

void CLinkPopupMenu::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (s_bNowShowRClickMenu)
		return ;

	if (auto value = _HitTest(point)) {
		if ( PreDoDragDrop(m_hWnd) ) {
			int nIndex = value.get();
			_HotItem(nIndex);
			_CloseSubMenu();

			CComPtr<IDataObject> spDataObject = CLinkItemDataObject::CreateInstance(m_pFolder, nIndex);
			if ( spDataObject ) {
				const LinkItem& item = *m_pFolder->at(nIndex);
				if (CLinkBarOption::s_bNoShowDragImage == false) {
					CBitmapHandle bmp;
					static const COLORREF	whiteColor = RGB(0xFF, 0xFF, 0xFF);
					static const COLORREF	blackColor = RGB(0, 0, 0);
					static const COLORREF	boundColor = RGB(212, 219, 237);
					{
						CClientDC	wndDC(m_hWnd);
						CDC dc = ::CreateCompatibleDC(wndDC);
						bmp = CreateCompatibleBitmap(wndDC, item.rcItem.Width(), item.rcItem.Height());
						dc.SetBkMode(TRANSPARENT);
						HFONT hFontPrev = dc.SelectFont(m_font);
						HBITMAP hPrevBMP = dc.SelectBitmap(bmp);
						::OffsetWindowOrgEx(dc, item.rcItem.left, item.rcItem.top, NULL);
						CBrush brs = ::CreateSolidBrush(boundColor);
						dc.FillRect(item.rcItem, brs);

						_DrawDragImage(dc.m_hDC, item);
					}
					if (bmp) {
						SHDRAGIMAGE	sdimg;
						sdimg.hbmpDragImage = bmp;
						sdimg.crColorKey = boundColor;
						sdimg.sizeDragImage = CSize(item.rcItem.Width(), item.rcItem.Height());
						sdimg.ptOffset	= CPoint(point.x - item.rcItem.left, point.y  - item.rcItem.top );
						if (FAILED(m_spDragSourceHelper->InitializeFromBitmap(&sdimg, spDataObject)))
							bmp.DeleteObject();
					}
				}

				s_bNoCloseBaseSubMenu = false;
				//m_nNowDragItemIndex = nIndex;
				//m_bDragFromItself = true;
				DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
				//m_bDragFromItself = false;
				//m_nNowDragItemIndex = -1;
				if (s_bNoCloseBaseSubMenu == false)
					_CloseBaseSubMenu();
			}
			//_PressItem(-1);

		} else {	// Drag操作をしていなかったのでアイテムを開く
			_DoExec(point, true);
		}
	}
}

void CLinkPopupMenu::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (s_bNowShowRClickMenu)
		return ;

	_CloseSubMenu();
	int nCount = (int)m_pFolder->size();
	if (auto value = _HitTest(point)) {
		int nIndex = value.get();
		CLinkPopupMenu::ShowRClickMenuAndExecCommand(m_pFolder, m_pFolder->at(nIndex).get(), m_hWnd);
	} else {
		CLinkPopupMenu::ShowRClickMenuAndExecCommand(m_pFolder, nullptr, m_hWnd);
	}
	if (nCount != (int)m_pFolder->size())
		_CloseBaseSubMenu();
	else
		_UpdateItemPosition();
}

void CLinkPopupMenu::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (s_bNowShowRClickMenu)
		return ;

	if (auto value = _HitTest(point)) {
		LinkItem& item = *m_pFolder->at(value.get());
		if (item.pFolder)
			return ;
		OpenLink(item, D_OPENFILE_CREATETAB | DonutGetStdOpenActivateFlag());
	}
}


LRESULT CLinkPopupMenu::OnUpdateSubMenuItemPosition(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	_UpdateItemPosition();
	if (m_pSubMenu)
		m_pSubMenu->SendMessage(WM_UPDATESUBMENUITEMPOS);
	return 0;
}


LRESULT CLinkPopupMenu::OnTooltipGetDispInfo(LPNMHDR pnmh)
{
	LPNMTTDISPINFO pntdi = (LPNMTTDISPINFO)pnmh;
	m_strTipText.Empty();
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return 0;
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);
	if (auto value = _HitTest(pt)) {
		LinkItem& item = *m_pFolder->at(value.get());
		if (item.pFolder) 
			return 0;

		m_strTipText = item.strName + _T("\r\n") + item.strUrl;		
		pntdi->lpszText = m_strTipText.GetBuffer(0);
	}
	return 0;
}



void	CLinkPopupMenu::_InitTooltip()
{
	m_tip.Create(m_hWnd, 0, NULL, TTS_ALWAYSTIP, WS_EX_TRANSPARENT);
	m_tip.Activate(TRUE);
	CToolInfo ti(TTF_SUBCLASS | TTF_TRANSPARENT, m_hWnd);
	ti.hwnd	 = m_hWnd;
	if (RunTimeHelper::IsCommCtrl6() == false)
		ti.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	ATLVERIFY(m_tip.AddTool(&ti));

	m_tip.SetMaxTipWidth(SHRT_MAX);
	m_tip.SetDelayTime(TTDT_AUTOPOP, 30 * 1000);
}


void	CLinkPopupMenu::_UpdateItemPosition()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int nTop = kBoundMargin;
	for (auto it = m_pFolder->begin(); it != m_pFolder->end(); ++it) {
		LinkItem& item = *it->get();
		item.rcItem.top	= nTop;
		item.rcItem.left	= kBoundMargin;
		item.rcItem.right	= rcClient.right - kBoundMargin;
		item.rcItem.bottom	= nTop + kItemHeight;
		nTop	+= kItemHeight;

		item.ModifyState(item.kItemNormal);
	}

	Invalidate(FALSE);
}


bool	CLinkPopupMenu::_IsValidIndex(int nIndex) const { 
	return 0 <= nIndex && nIndex < (int)m_pFolder->size();
}

CRect	CLinkPopupMenu::_GetClientItemRect(const CRect& rcItem)
{
	POINT ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcClientRect = rcItem;
	rcClientRect.top	-= ptOffset.y;
	rcClientRect.bottom	-= ptOffset.y;
	return rcClientRect;
}

boost::optional<int>	CLinkPopupMenu::_HitTest(const CPoint& point)
{
	POINT ptOffset;
	GetScrollOffset(ptOffset);
	int nCount = (int)m_pFolder->size();
	for (int i = 0; i < nCount; ++i) {
		CRect rcItem = m_pFolder->at(i)->rcItem;
		rcItem.top	-= ptOffset.y;
		rcItem.bottom	-= ptOffset.y;
		if (rcItem.PtInRect(point))
			return i;
	}
	return boost::none;
}

void	CLinkPopupMenu::_HotItem(int nNewHotIndex)
{
	// clean up
	if ( _IsValidIndex(m_nHotIndex) ) {
		LinkItem& item = *m_pFolder->at(m_nHotIndex);
		item.state	= item.kItemNormal;
		//if ( item.ModifyState(item.kItemHot, 0) )
			InvalidateRect(_GetClientItemRect(item.rcItem));
	}

	m_nHotIndex = nNewHotIndex;

	if ( _IsValidIndex(m_nHotIndex) ) {
		LinkItem& item = *m_pFolder->at(m_nHotIndex);
		item.state	= item.kItemHot;
		//if ( item.ModifyState(0, item.kItemHot) )
			InvalidateRect(_GetClientItemRect(item.rcItem));
	}
}

void	CLinkPopupMenu::_DoExec(const CPoint& pt, bool bLButtonDown /*= false*/)
{
	if (auto value = _HitTest(pt)) {
		LinkItem& item = *m_pFolder->at(value.get());
		if (m_pSubMenu) {
			if (m_pSubMenu->m_nInheritFolderIndex == value.get())
				return ;	// 派生元フォルダなら何もしない
			m_pSubMenu->DestroyWindow();
			m_pSubMenu = nullptr;
		}

		// サブメニューをポップアップさせる
		if (item.pFolder) {
			CRect rcWork = Misc::GetMonitorWorkArea(m_hWnd);
			m_pSubMenu = new CLinkPopupMenu(item.pFolder, value.get());
			CRect rcWindow;
			CRect rcClientItem = _GetClientItemRect(item.rcItem);
			int nTop	= rcClientItem.top - (kBoundBorder + kBoundMargin);
			int nLeft	= rcClientItem.right	- kBiteWidth;
			rcWindow.right	= m_pSubMenu->ComputeWindowWidth();
			rcWindow.bottom	= m_pSubMenu->ComputeWindowHeight();
			rcWindow.MoveToXY(nLeft, nTop);
			ClientToScreen(&rcWindow);
			if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
				int nOverHeight = rcWindow.bottom - rcWork.bottom;
				rcWindow.MoveToY( rcWindow.top - nOverHeight );
				if (rcWindow.top < rcWork.top)
					rcWindow.top = rcWork.top;
				rcWindow.bottom	= rcWork.bottom;
				static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
				rcWindow.right += s_nVScrollWidth;
			}
			if (rcWork.right < rcWindow.right) {	// 右にはみ出る
				int nWidth = rcWindow.Width();
				ClientToScreen(&rcClientItem);
				//rcClientItem.left += kBiteWidth;
				rcWindow.MoveToX(rcClientItem.left - nWidth);	// 反対側に出す
			}

			m_pSubMenu->Create(m_hWnd, rcWindow, nullptr, /*WS_VISIBLE| */WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);
			m_pSubMenu->ShowWindow(SW_SHOWNOACTIVATE);

		} else if (bLButtonDown) {
			// メニューを閉じる
			_CloseBaseSubMenu();

			// アイテムを実行...
			OpenLink(item, DonutGetStdOpenFlag());
		}
	}
}


void	CLinkPopupMenu::_CloseSubMenu()
{
	if (m_pSubMenu) {
		m_pSubMenu->DestroyWindow();
		m_pSubMenu = nullptr;
	}
}

/// 大元のメニューを閉じる
void	CLinkPopupMenu::_CloseBaseSubMenu()
{
	::PostMessage(s_wndLinkBar, WM_CLOSEBASESUBMENU, 0, 0);
}


void	CLinkPopupMenu::_DrawInsertEdge(const CRect& rcEdge)
{
	UpdateWindow();	// 事前にHotItemの処理を終わらせる
	CClientDC	 dc(m_hWnd);
	CBrush		 hbr;
	hbr.CreateSolidBrush( ::GetSysColor(COLOR_3DDKSHADOW) );
	//dc.SetBrushOrg(pt.x, 0);
	HBRUSH hbrOld = dc.SelectBrush(hbr);

	POINT pts[] = { 
		{ rcEdge.left , rcEdge.top }, 
		{ rcEdge.left , rcEdge.top + 1 }, 
		{ rcEdge.right - 1, rcEdge.top + 1 },
		{ rcEdge.right - 1, rcEdge.top },
	};
	dc.Polygon( pts, _countof(pts) );

	dc.SelectBrush(hbrOld);
}

void	CLinkPopupMenu::_ClearInsertionEdge()
{
	if (m_rcInvalidateOnDrawingInsertionEdge != CRect()) {
		CRect rc = m_rcInvalidateOnDrawingInsertionEdge;
		rc.bottom= rc.top + 2;
		rc = _GetClientItemRect(rc);
		InvalidateRect(&rc);
		UpdateWindow();
		m_rcInvalidateOnDrawingInsertionEdge.SetRectEmpty();
	}
}


int		CLinkPopupMenu::_HitTestOnDragging(const CPoint& pt, hitTestFlag& htflag)
{
	int nCount = (int)m_pFolder->size();
	if (nCount == 0) {
		htflag = htItemNone;
		return 0;
	}
	for (int i = 0; i < nCount; ++i) {
		const CRect& rcOrg = m_pFolder->at(i)->rcItem;
		CRect rcItem = _GetClientItemRect(rcOrg);
		rcItem.left = 0;
		rcItem.right += kBoundMargin;

		if (i == 0) {
			CRect rcTop = rcItem;
			rcTop.top = 0;
			rcTop.bottom = kBoundMargin;
			if (rcTop.PtInRect(pt)) {
				htflag = htItemTop;
				return i;
			}
		}
		if (i == nCount - 1) {
			CRect rcBottom = rcItem;
			rcBottom.top	= rcBottom.bottom;
			rcBottom.bottom	= rcBottom.top + kBoundMargin;
			if (rcBottom.PtInRect(pt)) {
				htflag = htItemBottom;
				return i;
			}
		}

		if (rcItem.PtInRect(pt)) {
			if (m_pFolder->at(i)->pFolder) {
				rcItem.top += kDragInsertHitWidthOnFolder;
				rcItem.bottom -= kDragInsertHitWidthOnFolder;
				if (rcItem.PtInRect(pt)) {
					htflag = htFolder;
				} else {
					rcItem.top = rcOrg.top;
					rcItem.bottom = rcItem.top + kDragInsertHitWidthOnFolder;
					if (rcItem.PtInRect(pt)) {
						htflag = htItemTop;
					} else {
						rcItem.top = rcOrg.bottom - kDragInsertHitWidthOnFolder;
						rcItem.bottom = rcOrg.bottom;
						if (rcItem.PtInRect(pt)) {
							htflag = htItemBottom;
						}
					}
				}
			} else {
				rcItem.bottom -= rcItem.Height() / 2;
				if (rcItem.PtInRect(pt)) {
					htflag = htItemTop;
				} else {
					htflag = htItemBottom;
				}

			}
			return i;
		}
	}
	htflag = htItemNone;
	return -1;
}

















































