// RecentDocumentListFixed.cpp

#include "stdafx.h"
#include "RecentDocumentListFixed.h"
#include "option/MainOption.h"


class CRecentDocumentListFixed::Impl
{
private:
	enum {
		m_cchMaxItemLen_Min = 6,
		m_cchMaxItemLen_Max = INTERNET_MAX_PATH_LENGTH
	};

public:
	// Constructor
	Impl();

	// Attributes
	HMENU GetMenuHandle() const { return m_hMenu; }
	void  SetMenuHandle(HMENU hMenu);

	int  GetMaxEntries() const { return m_nMaxEntries; }
	void SetMaxEntries(int nMaxEntries);

	int  GetMaxItemLength() const { return m_cchMaxItemLen; }
	void SetMaxItemLength(int cchMaxLen);	// メニューの最大文字列

	int  GetMenuType() const { return m_nMenuType; }
	void SetMenuType(int nMode) { m_nMenuType = nMode; }

	int  GetRecentCount() const { return m_arrDocs.GetSize(); }

	// Operations
	BOOL AddToList(
			 LPCTSTR									lpstrDocName,
			std::vector<std::pair<CString, CString> >*	parrFore = NULL,
			std::vector<std::pair<CString, CString> >*	parrBack = NULL,
			const CString								strTitle = _T("") );
	BOOL GetFromList(int nItemID, RecentDocEntry* pEntry);
	BOOL RemoveFromList(int nItemID);


	BOOL ReadFromIniFile();
	BOOL WriteToIniFile();


	void DeleteIniKeys();

	// Implementation
	BOOL UpdateMenu();
	void ResetMenu();


private:

	// Overrideable
	// override to provide a different method of compacting document names
	static bool CompactDocumentName(LPTSTR lpstrOut, LPCTSTR lpstrIn, int cchLen)
	{
		return AtlCompactPathFixed(lpstrOut, lpstrIn, cchLen);
	}

	CString _GetRecentCloseFile() { return _GetFilePath("RecentClose.ini"); }


	// Data members
	typedef CRecentDocumentListFixed::RecentDocEntry _DocEntry;
	typedef _DocEntry	RecentDocEntry;
	CSimpleArray<_DocEntry> m_arrDocs;

	int 					m_nMaxEntries;		// default is 4
	HMENU					m_hMenu;

	TCHAR					m_szNoEntries[INTERNET_MAX_PATH_LENGTH];

	int 					m_cchMaxItemLen;

	int 					m_nMinID;
	int 					m_nMaxID;

	int 					m_nMenuType;
};


// =====================================================================

// Constructor
CRecentDocumentListFixed::Impl::Impl()
	: m_hMenu(NULL)
	, m_nMaxEntries(4)
	, m_cchMaxItemLen(100)
	, m_nMenuType(RECENTDOC_MENUTYPE_URL)
{
	m_nMinID = ID_RECENTDOCUMENT_FIRST;
	m_nMaxID = ID_RECENTDOCUMENT_LAST;
	m_szNoEntries[0] = 0;					//+++
	ATLASSERT(INTERNET_MAX_PATH_LENGTH > m_cchMaxItemLen_Min);
}

void  CRecentDocumentListFixed::Impl::SetMenuHandle(HMENU hMenu)
{
	ATLASSERT( hMenu == NULL || ::IsMenu(hMenu) );
	m_hMenu = hMenu;

	if ( m_hMenu == NULL || (::GetMenuString(m_hMenu, m_nMinID, m_szNoEntries, INTERNET_MAX_PATH_LENGTH, MF_BYCOMMAND) == 0) )
		lstrcpy( m_szNoEntries, _T("(empty)") );
}


void CRecentDocumentListFixed::Impl::SetMaxEntries(int nMaxEntries)
{
	ATLASSERT(m_nMaxEntries_Min < nMaxEntries && nMaxEntries < m_nMaxEntries_Max);

	if (nMaxEntries < m_nMaxEntries_Min)
		nMaxEntries = m_nMaxEntries_Min;
	else if (nMaxEntries > m_nMaxEntries_Max)
		nMaxEntries = m_nMaxEntries_Max;

	m_nMaxEntries = nMaxEntries;

	// 最大値より超えた分を消す
	if ( nMaxEntries < m_arrDocs.GetSize() ) {
		int nCount = m_arrDocs.GetSize() - nMaxEntries;

		for (int i = 0; i < nCount; i++)
			m_arrDocs.RemoveAt(nMaxEntries);
	}
}

// メニューの最大文字列を設定する
void CRecentDocumentListFixed::Impl::SetMaxItemLength(int cchMaxLen)
{
	ATLASSERT( (cchMaxLen >= m_cchMaxItemLen_Min && cchMaxLen <= m_cchMaxItemLen_Max) || cchMaxLen == -1 );

	if (cchMaxLen != -1) {
		if (cchMaxLen < m_cchMaxItemLen_Min)
			cchMaxLen = m_cchMaxItemLen_Min;
		else if (cchMaxLen > m_cchMaxItemLen_Max)
			cchMaxLen = m_cchMaxItemLen_Max;
	}

	m_cchMaxItemLen = cchMaxLen;
	UpdateMenu();
}



// Oparation
BOOL CRecentDocumentListFixed::Impl::AddToList(
			LPCTSTR									lpstrDocName,
		std::vector<std::pair<CString, CString> >*	parrFore,
		std::vector<std::pair<CString, CString> >*	parrBack,
		const CString								strTitle)
{
	_DocEntry	de;

	if (lstrcpy(de.szDocName, lpstrDocName) == NULL)
		return FALSE;

	for (int i = 0; i < m_arrDocs.GetSize(); i++) {
		if (lstrcmpi(m_arrDocs[i].szDocName, lpstrDocName) == 0) {
			m_arrDocs.RemoveAt(i);
			break;
		}
	}

	if (m_arrDocs.GetSize() == m_nMaxEntries && m_nMaxEntries)	//minit
		m_arrDocs.RemoveAt(0);

	if (parrFore)
		de.arrFore = *parrFore;

	if (parrBack)
		de.arrBack = *parrBack;

	de.strTitle = strTitle;

	BOOL bRet = m_arrDocs.Add(de);
	if (bRet) 
		bRet = UpdateMenu();

	return bRet;
}

BOOL CRecentDocumentListFixed::Impl::GetFromList(int nItemID, RecentDocEntry* pEntry)
{
	int nIndex = m_arrDocs.GetSize() - (nItemID - m_nMinID) - 1;

	if ( nIndex < 0 || nIndex >= m_arrDocs.GetSize() )
		return FALSE;

	*pEntry = m_arrDocs[nIndex];
	return TRUE;
}

BOOL CRecentDocumentListFixed::Impl::RemoveFromList(int nItemID)
{
	int  nIndex = m_arrDocs.GetSize() - (nItemID - m_nMinID) - 1;
	BOOL bRet	= m_arrDocs.RemoveAt(nIndex);

	if (bRet) 
		bRet = UpdateMenu();

	return bRet;
}

// メニューを更新する
BOOL CRecentDocumentListFixed::Impl::UpdateMenu()
{
	if (m_hMenu == NULL)
		return FALSE;

	ATLASSERT( ::IsMenu(m_hMenu) );

	int 	nItems = ::GetMenuItemCount(m_hMenu);
	int 	nInsertPoint;

	for (nInsertPoint = 0; nInsertPoint < nItems; nInsertPoint++) {
		MENUITEMINFO mi;
		mi.cbSize = sizeof (MENUITEMINFO);
		mi.fMask  = MIIM_ID;
		::GetMenuItemInfo(m_hMenu, nInsertPoint, TRUE, &mi);

		if (mi.wID == m_nMinID)
			break;
	}

	ATLASSERT(nInsertPoint < nItems && "You need a menu item with an ID = m_nMinID");

	int 	nItem;
	for (nItem = m_nMinID; nItem < m_nMinID + m_nMaxEntries; nItem++) {
		// keep the first one as an insertion point
		if (nItem != m_nMinID)
			::DeleteMenu(m_hMenu, nItem, MF_BYCOMMAND);
	}

	CString 	strOut;
	int 		nSize  = m_arrDocs.GetSize();

	nItem	= 0;
	if (nSize > 0) {
		for (nItem = 0; nItem < nSize; nItem++) {
			char		cShortcut = 0;
			CString 	strShortcut;

			if (nItem < 10)
				cShortcut = '0' + nItem;
			else if (nItem < 36)
				cShortcut = 'a' + nItem - 10;
			else if (nItem < 62)
				cShortcut = 'A' + nItem - 36;

			CString 	strIn;

			if (m_arrDocs[nSize - 1 - nItem].strTitle.IsEmpty() || m_nMenuType == RECENTDOC_MENUTYPE_URL) {
				//URL表示
				strIn = m_arrDocs[nSize - 1 - nItem].szDocName;

				if (m_cchMaxItemLen != -1) {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH];
					szBuff[0]	= 0;	//+++
					bool  bRet 	= CompactDocumentName(szBuff, strIn, m_cchMaxItemLen);
					strIn 		= szBuff;
				}

			} else if (m_nMenuType == RECENTDOC_MENUTYPE_TITLE) {
				//タイトル表示
				strIn = m_arrDocs[nSize - 1 - nItem].strTitle;

				if (m_cchMaxItemLen != -1) {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH];
					szBuff[0]	= 0;	//+++
					bool  bRet 	= CompactDocumentName(szBuff, strIn, m_cchMaxItemLen);
					strIn 		= szBuff;
				}

			} else if (m_nMenuType == RECENTDOC_MENUTYPE_DOUBLE) {
				//両方表示
				if (m_cchMaxItemLen == -1) {
					strIn = m_arrDocs[nSize - 1 - nItem].strTitle + _T(" - ") + m_arrDocs[nSize - 1 - nItem].szDocName;
				} else {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH];
					szBuff[0]	= 0;	//+++
					bool  bRet 	= CompactDocumentName(szBuff, m_arrDocs[nSize - 1 - nItem].strTitle, m_cchMaxItemLen / 2);
					strIn		= szBuff;
					strIn		+= _T(" - ");
					bRet		= CompactDocumentName(szBuff, m_arrDocs[nSize - 1 - nItem].szDocName, m_cchMaxItemLen / 2);
					strIn		+= szBuff;
				}

			} else {
				ATLASSERT(FALSE);
			}

			{
				strIn.Replace( _T("&"), _T("&&") );
				int nLen = strIn.GetLength();
				wsprintf(strOut.GetBufferSetLength(nLen + 8), _T("&%c %s"), cShortcut, strIn);
			}

			strOut.ReleaseBuffer();
			::InsertMenu(m_hMenu, nInsertPoint + nItem, MF_BYPOSITION | MF_STRING, m_nMinID + nItem, strOut);
		}
	} else {  // empty
		::InsertMenu(m_hMenu, nInsertPoint, MF_BYPOSITION | MF_STRING, m_nMinID, m_szNoEntries);
		::EnableMenuItem(m_hMenu, m_nMinID, MF_GRAYED);
		nItem++;
	}

	::DeleteMenu(m_hMenu, nInsertPoint + nItem, MF_BYPOSITION);

	return TRUE;
}


void CRecentDocumentListFixed::Impl::ResetMenu()
{
	for (int nItem = m_nMinID + 1; nItem < m_nMinID + m_nMaxEntries; ++nItem) {
		::DeleteMenu(m_hMenu, nItem, MF_BYCOMMAND);
	}
}


void CRecentDocumentListFixed::Impl::DeleteIniKeys()
{
	//+++ 念のため、バックアップ化
	//x DeleteFile( _GetFilePath( _T("RecentClose.ini") ) );
	Misc::MoveToBackupFile( _GetFilePath( _T("RecentClose.ini") ) );
}


BOOL CRecentDocumentListFixed::Impl::ReadFromIniFile()
{
	LONG			lRet1;
	LONG			lRet2;
	CString 		strIni = _GetRecentCloseFile();

	//標準ini(g_szIniFileName)から読み取り
	DWORD			dwRet1 = 10;
	DWORD			dwRet2 = 0;

	{
		CIniFileI	pr( g_szIniFileName, _T("Recent Document List") );
		lRet1		= pr.QueryValue( dwRet1, _T("DocumentCount") );
		lRet2		= pr.QueryValue( dwRet2, _T("MenuType") );
	}

	if (lRet1 != ERROR_SUCCESS || lRet2 != ERROR_SUCCESS) {
		//読み取り失敗 - RecentClose.iniにあるかも
		CIniFileI	pr( strIni, _T("Main") );

		if (lRet1 != ERROR_SUCCESS)
			lRet1 = pr.QueryValue( dwRet1, _T("DocumentCount") );

		if (lRet2 != ERROR_SUCCESS)
			lRet2 = pr.QueryValue( dwRet2, _T("MenuType") );
	}

	//読み取った値のチェック
	if ( lRet1 == ERROR_SUCCESS && ( dwRet1 > 0 && (int) dwRet1 <= (m_nMaxID - m_nMinID + 1) ) )
		m_nMaxEntries = dwRet1;

	m_nMenuType = dwRet2;

	m_arrDocs.RemoveAll();

	for (int nItem = m_nMaxEntries; nItem > 0; nItem--) {
		_DocEntry		de;
		CString 		strSection;
		strSection.Format(_T("Document%d"), nItem);
		CIniFileI		pr(strIni, strSection);

		CString 	strKey;
		if (CMainOption::s_bTravelLogClose) {
			for (unsigned j = 0; j < 0x7FFFFFFF; ++j) { 	//+++ 意味をなさないけど念のため回数限界記述.
				strKey.Format(_T("Fore_URL%d"), j);
				CString 	strURL = pr.GetString( strKey );
				if (strURL.IsEmpty())
					break;
				strKey.Format(_T("Fore_Title%d"), j);
				CString 	strTitle = pr.GetStringUW( strKey );
				if (strTitle.IsEmpty())
					break;
				de.arrFore.push_back( std::make_pair(strTitle, strURL) );
			}

			for (unsigned j = 0; j < 0x7FFFFFFF; ++j) { 	//+++ 意味をなさないけど念のため回数限界記述.
				strKey.Format(_T("Back_URL%d"), j);
				CString 	strURL = pr.GetString( strKey );
				if (strURL.IsEmpty())
					break;
				strKey.Format(_T("Back_Title%d"), j);
				CString 	strTitle = pr.GetStringUW( strKey );
				if (strTitle.IsEmpty())
					break;
				de.arrFore.push_back( std::make_pair(strTitle, strURL) );
			}
		}

		strKey.Format(_T("Title%i"), nItem);				// %iなんて必要ないけど他に合わせるということで
		de.strTitle = pr.GetStringUW( strKey );
		strKey.Format(_T("Document%i"), nItem); 			//本当は%iなんていらないけれど
		CString 	strRet = pr.GetString( strKey );
		if (strRet.IsEmpty() == 0) {
			if (lstrcpy(de.szDocName, strRet) != 0)
				m_arrDocs.Add(de);
		}

		pr.Close();
	}

	return UpdateMenu();
}


BOOL CRecentDocumentListFixed::Impl::WriteToIniFile()
{
	CString 	strIni = _GetRecentCloseFile();

	{
		CIniFileO	pr( g_szIniFileName, _T("Recent Document List") );
		pr.SetValue( m_nMenuType, _T("MenuType") );
		pr.SetValue( m_nMaxEntries, _T("DocumentCount") );
	}

	// set new values
	int 		nItem;
	CIniFileO	pr(strIni, _T("Document"));	//+++ セクション名は適当.
	for (nItem = m_arrDocs.GetSize(); nItem > 0; nItem--) {
		CString 	   strSection;
		strSection.Format(_T("Document%d"), nItem);
		//消してから書く（当たり前なんだけど忘れてて迷った）
		pr.ChangeSectionName( strSection );
		pr.DeleteSection();

		TCHAR		   szBuff[64];
		wsprintf(szBuff, _T("Document%i"), nItem);
		RecentDocEntry Entry;
		GetFromList(m_nMinID + nItem - 1, &Entry);
		LONG		   lRet = pr.SetString(Entry.szDocName, strSection);
		ATLASSERT(lRet == ERROR_SUCCESS);

		CString 	   strKey;
		strKey.Format(_T("Title%i"), nItem);
		pr.SetStringUW(Entry.strTitle, strKey);

		int 		   i;

		for (i = 0; i < (int) Entry.arrFore.size(); i++) {
			strKey.Format(_T("Fore_Title%d"), i);
			pr.SetStringUW(Entry.arrFore[i].first, strKey);

			strKey.Format(_T("Fore_URL%d"), i);
			pr.SetString(Entry.arrFore[i].second, strKey);
		}

		for (i = 0; i < (int) Entry.arrBack.size(); i++) {
			strKey.Format(_T("Back_Title%d"), i);
			pr.SetStringUW(Entry.arrBack[i].first, strKey);

			strKey.Format(_T("Back_URL%d"), i);
			pr.SetString(Entry.arrBack[i].second, strKey);
		}
	}

	// delete unused keys
	for (nItem = m_arrDocs.GetSize() + 1; nItem < (m_nMaxID - m_nMinID + 1); nItem++) {
		TCHAR szBuff[64/*11*/];
		wsprintf(szBuff, _T("Document%i"), nItem);
		pr.ChangeSectionName(szBuff);
		pr.DeleteSection();
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////
// CRecentDocumentListFixed


CRecentDocumentListFixed::CRecentDocumentListFixed()
	: pImpl(new Impl)
{
}

CRecentDocumentListFixed::~CRecentDocumentListFixed()
{
	delete pImpl;
}


// Attributes
HMENU CRecentDocumentListFixed::GetMenuHandle() const
{
	return pImpl->GetMenuHandle();
}
void  CRecentDocumentListFixed::SetMenuHandle(HMENU hMenu)
{
	pImpl->SetMenuHandle(hMenu);
}

int  CRecentDocumentListFixed::GetMaxEntries() const
{
	return pImpl->GetMaxEntries();
}
void CRecentDocumentListFixed::SetMaxEntries(int nMaxEntries)
{
	pImpl->SetMaxEntries(nMaxEntries);
}

int  CRecentDocumentListFixed::GetMaxItemLength() const
{
	return pImpl->GetMaxItemLength();
}
void CRecentDocumentListFixed::SetMaxItemLength(int cchMaxLen)	// メニューの最大文字列
{
	pImpl->SetMaxItemLength(cchMaxLen);
}

int  CRecentDocumentListFixed::GetMenuType() const
{
	return pImpl->GetMenuType();
}
void CRecentDocumentListFixed::SetMenuType(int nMode)
{
	pImpl->SetMenuType(nMode);
}

int  CRecentDocumentListFixed::GetRecentCount() const
{
	return pImpl->GetRecentCount();
}

// Operations
BOOL CRecentDocumentListFixed::AddToList(
			LPCTSTR									lpstrDocName,
		std::vector<std::pair<CString, CString> >*	parrFore,
		std::vector<std::pair<CString, CString> >*	parrBack,
		const CString								strTitle)
{
	return pImpl->AddToList(lpstrDocName, parrFore, parrBack, strTitle);
}
BOOL CRecentDocumentListFixed::GetFromList(int nItemID, RecentDocEntry* pEntry)
{
	return pImpl->GetFromList(nItemID, pEntry);
}
BOOL CRecentDocumentListFixed::RemoveFromList(int nItemID)
{
	return pImpl->RemoveFromList(nItemID);
}


BOOL CRecentDocumentListFixed::ReadFromIniFile()
{
	return pImpl->ReadFromIniFile();
}
BOOL CRecentDocumentListFixed::WriteToIniFile()
{
	return pImpl->WriteToIniFile();
}


void CRecentDocumentListFixed::DeleteIniKeys()
{
	pImpl->DeleteIniKeys();
}

// Implementation
BOOL CRecentDocumentListFixed::UpdateMenu()
{
	return pImpl->UpdateMenu();
}
void CRecentDocumentListFixed::ResetMenu()
{
	pImpl->ResetMenu();
}











