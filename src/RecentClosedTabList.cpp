/**
 *	@file	RecentClosedTabList.cpp
 *	@brief	最近閉じたタブのリスト.
 */

#include "stdafx.h"
#include "RecentClosedTabList.h"
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include "MtlMisc.h"
#include "ChildFrame.h"
#include "option\MainOption.h"
#include "option\DLControlOption.h"

class CRecentClosedTabList::Impl
{
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

	int  GetRecentCount() const { return (int)m_vecpClosedTabData.size(); }

	// Operations
	BOOL AddToList(ChildFrameDataOnClose* pClosedTabData);
	BOOL GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData);
	BOOL RemoveFromList(int nItemID);


	BOOL ReadFromXmlFile();
	BOOL WriteToXmlFile();

	void DeleteRecentClosedTabFile();

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
	 
	CString _GetRecentCloseFile() { return _GetFilePath("RecentClosedTab.xml"); }


	// Data members
	vector<unique_ptr<ChildFrameDataOnClose> >	m_vecpClosedTabData;	// beginが最も最後に閉じたタブ

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
CRecentClosedTabList::Impl::Impl()
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

void  CRecentClosedTabList::Impl::SetMenuHandle(HMENU hMenu)
{
	ATLASSERT( hMenu == NULL || ::IsMenu(hMenu) );
	m_hMenu = hMenu;

	if ( m_hMenu == NULL || (::GetMenuString(m_hMenu, m_nMinID, m_szNoEntries, INTERNET_MAX_PATH_LENGTH, MF_BYCOMMAND) == 0) )
		_tcscpy_s( m_szNoEntries, _T("(empty)") );
}


void CRecentClosedTabList::Impl::SetMaxEntries(int nMaxEntries)
{
	if (nMaxEntries < CRecentClosedTabList::kMaxEntries_Min)
		nMaxEntries = CRecentClosedTabList::kMaxEntries_Min;
	else if (nMaxEntries > CRecentClosedTabList::kMaxEntries_Max)
		nMaxEntries = CRecentClosedTabList::kMaxEntries_Max;

	m_nMaxEntries = nMaxEntries;

	// 最大値より超えた分を消す
	if ( nMaxEntries < (int)m_vecpClosedTabData.size() ) {
		int nCount = (int)m_vecpClosedTabData.size() - nMaxEntries;
		for (int i = 0; i < nCount; ++i) 
			m_vecpClosedTabData.erase(m_vecpClosedTabData.begin() + nMaxEntries);
	}
}

// メニューの最大文字列を設定する
void CRecentClosedTabList::Impl::SetMaxItemLength(int cchMaxLen)
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
BOOL CRecentClosedTabList::Impl::AddToList(ChildFrameDataOnClose* pClosedTabData)
{
	if ((int)m_vecpClosedTabData.size() == m_nMaxEntries)
		m_vecpClosedTabData.erase(m_vecpClosedTabData.begin() + m_nMaxEntries - 1);

	m_vecpClosedTabData.insert(m_vecpClosedTabData.begin(), unique_ptr<ChildFrameDataOnClose>(std::move(pClosedTabData)));

	UpdateMenu();

	return TRUE;
}

BOOL CRecentClosedTabList::Impl::GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData)
{
	int nIndex = nItemID - m_nMinID;
	if ( nIndex < 0 || nIndex >= (int)m_vecpClosedTabData.size() )
		return FALSE;

	*ppClosedTabData = 	m_vecpClosedTabData[nIndex].get();
	return TRUE;
}

BOOL CRecentClosedTabList::Impl::RemoveFromList(int nItemID)
{
	int  nIndex = nItemID - m_nMinID;
	m_vecpClosedTabData.erase(m_vecpClosedTabData.begin() + nIndex);

	UpdateMenu();

	return TRUE;
}

// メニューを更新する
BOOL CRecentClosedTabList::Impl::UpdateMenu()
{
	if (m_hMenu == NULL)
		return FALSE;

	ATLASSERT( ::IsMenu(m_hMenu) );

	int 	nItems = ::GetMenuItemCount(m_hMenu);
	int 	nInsertPoint;

	for (nInsertPoint = 0; nInsertPoint < nItems; nInsertPoint++) {
		MENUITEMINFO mi = { sizeof (MENUITEMINFO) };
		mi.fMask  = MIIM_ID;
		::GetMenuItemInfo(m_hMenu, nInsertPoint, TRUE, &mi);
		if (mi.wID == m_nMinID)
			break;
	}

	ATLASSERT(nInsertPoint < nItems && "You need a menu item with an ID = m_nMinID");

	int nItem;
	for (nItem = m_nMinID; nItem < m_nMinID + m_nMaxEntries; nItem++) {
		// keep the first one as an insertion point
		if (nItem != m_nMinID)
			::DeleteMenu(m_hMenu, nItem, MF_BYCOMMAND);
	}

	nItem = 0;
	CString 	strOut;
	int nSize  = (int)m_vecpClosedTabData.size();
	if (nSize > 0) {
		for (nItem = 0; nItem < nSize; ++nItem) {
			ChildFrameDataOnClose& data = *m_vecpClosedTabData[nItem];
			char		cShortcut = 0;
			CString 	strShortcut;

			if (nItem < 10)
				cShortcut = '0' + nItem;
			else if (nItem < 36)
				cShortcut = 'a' + nItem - 10;
			else if (nItem < 62)
				cShortcut = 'A' + nItem - 36;

			CString 	strIn;

			if (data.strTitle.IsEmpty() || m_nMenuType == RECENTDOC_MENUTYPE_URL) {
				//URL表示
				strIn = data.strURL;

				if (m_cchMaxItemLen != -1) {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH] = _T("\0");
					bool  bRet 	= CompactDocumentName(szBuff, strIn, m_cchMaxItemLen);
					strIn 		= szBuff;
				}

			} else if (m_nMenuType == RECENTDOC_MENUTYPE_TITLE) {
				//タイトル表示
				strIn = data.strTitle;

				if (m_cchMaxItemLen != -1) {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH] = _T("\0");
					bool  bRet 	= CompactDocumentName(szBuff, strIn, m_cchMaxItemLen);
					strIn 		= szBuff;
				}

			} else if (m_nMenuType == RECENTDOC_MENUTYPE_DOUBLE) {
				//両方表示
				if (m_cchMaxItemLen == -1) {
					strIn = data.strTitle + _T(" - ") + data.strURL;
				} else {
					TCHAR szBuff[INTERNET_MAX_PATH_LENGTH] = _T("\0");
					bool  bRet 	= CompactDocumentName(szBuff, data.strTitle, m_cchMaxItemLen / 2);
					strIn		= szBuff;
					strIn		+= _T(" - ");
					bRet		= CompactDocumentName(szBuff, data.strURL, m_cchMaxItemLen / 2);
					strIn		+= szBuff;
				}

			} else {
				ATLASSERT(FALSE);
			}

			{
				strIn.Replace( _T("&"), _T("&&") );
				strOut.Format(_T("&%c %s"), cShortcut, strIn);
			}

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


void CRecentClosedTabList::Impl::ResetMenu()
{
	for (int nItem = m_nMinID + 1; nItem < m_nMinID + m_nMaxEntries; ++nItem) {
		::DeleteMenu(m_hMenu, nItem, MF_BYCOMMAND);
	}
}


void CRecentClosedTabList::Impl::DeleteRecentClosedTabFile()
{
	//+++ 念のため、バックアップ化
	//x DeleteFile( _GetFilePath( _T("RecentClose.ini") ) );
	//\\Misc::MoveToBackupFile( _GetFilePath( _T("RecentClose.ini") ) );
	DeleteFile(_GetRecentCloseFile());
}


BOOL CRecentClosedTabList::Impl::ReadFromXmlFile()
{
	using boost::property_tree::wptree;

	CString 		strFile = _GetRecentCloseFile();

	m_vecpClosedTabData.clear();

	std::wifstream	filestream;
	filestream.imbue(std::locale("japanese"));
	filestream.open(strFile);
	if (filestream.is_open() == false) {
		UpdateMenu();
		return FALSE;
	}
	try {
		wptree	pt;
		boost::property_tree::read_xml(filestream, pt);

		auto SetTravelLog	= [](wptree& ptLog, vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = ptLog.begin(); it != ptLog.end(); ++it) {
				wptree& item = it->second;
				vecTravelLog.push_back(std::pair<CString, CString>(
					item.get(L"<xmlattr>.Title", L"").c_str(), 
					item.get(L"<xmlattr>.URL", L"").c_str()));
			}
		};

		wptree	ptChild = pt.get_child(L"RecentCloseTab");
		for (auto it = ptChild.begin(); it != ptChild.end(); ++it) {
			unique_ptr<ChildFrameDataOnClose>	pdata(new ChildFrameDataOnClose);
			wptree& ptItem = it->second;
		
			pdata->strTitle	= ptItem.get(L"<xmlattr>.Title", L"").c_str();
			pdata->strURL	= ptItem.get(L"<xmlattr>.URL", L"").c_str();
			pdata->dwDLCtrl	= ptItem.get<DWORD>(L"<xmlattr>.DLCtrl", CDLControlOption::s_dwDLControlFlags);
			SetTravelLog(ptItem.get_child(L"TravelLog.Back"), pdata->TravelLogBack);
			SetTravelLog(ptItem.get_child(L"TravelLog.Fore"), pdata->TravelLogFore);

			m_vecpClosedTabData.push_back(std::move(pdata));
		}
	} catch (...) {
		ATLTRACE(_T("CRecentClosedTabList::Impl::ReadFromXmlFile で例外発生!\n"));
	}
	return UpdateMenu();
}


BOOL CRecentClosedTabList::Impl::WriteToXmlFile()
{
	using boost::property_tree::wptree;

	CString 	strSaveFilePath = _GetRecentCloseFile();

	try {
		auto AddTravelLog = [](wptree& ptLog, const vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = vecTravelLog.cbegin(); it != vecTravelLog.cend(); ++it) {
				wptree& ptItem = ptLog.add(L"item", L"");
				ptItem.put(L"<xmlattr>.Title", (LPCTSTR)it->first);
				ptItem.put(L"<xmlattr>.URL"	 , (LPCTSTR)it->second);
			}
		};
		wptree	pt;
		for (auto it = m_vecpClosedTabData.cbegin(); it != m_vecpClosedTabData.cend(); ++it) {
			ChildFrameDataOnClose& data = *(*it);
			wptree& ptItem = pt.add(L"RecentCloseTab.item", L"");
			ptItem.put(L"<xmlattr>.Title", (LPCTSTR)data.strTitle);
			ptItem.put(L"<xmlattr>.URL"	 , (LPCTSTR)data.strURL);
			ptItem.put(L"<xmlattr>.DLCtrl", data.dwDLCtrl);
			AddTravelLog(ptItem.add(L"TravelLog.Back", L""), data.TravelLogBack);
			AddTravelLog(ptItem.add(L"TravelLog.Fore", L""), data.TravelLogFore);
		}
		using namespace boost::property_tree::xml_parser;
		std::wofstream	filestream;
		filestream.imbue(std::locale("japanese"));
		filestream.open(strSaveFilePath, std::ios::out | std::ios::trunc);
		write_xml(filestream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("shift_jis")));
	} catch (...) {
		ATLTRACE(_T("CRecentClosedTabList::Impl::WriteToXmlFile で例外発生!\n"));
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////
// CRecentClosedTabList


CRecentClosedTabList::CRecentClosedTabList()
	: pImpl(new Impl)
{
}

CRecentClosedTabList::~CRecentClosedTabList()
{
	delete pImpl;
}


// Attributes
HMENU CRecentClosedTabList::GetMenuHandle() const
{
	return pImpl->GetMenuHandle();
}
void  CRecentClosedTabList::SetMenuHandle(HMENU hMenu)
{
	pImpl->SetMenuHandle(hMenu);
}

int  CRecentClosedTabList::GetMaxEntries() const
{
	return pImpl->GetMaxEntries();
}
void CRecentClosedTabList::SetMaxEntries(int nMaxEntries)
{
	pImpl->SetMaxEntries(nMaxEntries);
}

int  CRecentClosedTabList::GetMaxItemLength() const
{
	return pImpl->GetMaxItemLength();
}
void CRecentClosedTabList::SetMaxItemLength(int cchMaxLen)	// メニューの最大文字列
{
	pImpl->SetMaxItemLength(cchMaxLen);
}

int  CRecentClosedTabList::GetMenuType() const
{
	return pImpl->GetMenuType();
}
void CRecentClosedTabList::SetMenuType(int nMode)
{
	pImpl->SetMenuType(nMode);
}

int  CRecentClosedTabList::GetRecentCount() const
{
	return pImpl->GetRecentCount();
}

// Operations
BOOL CRecentClosedTabList::AddToList(ChildFrameDataOnClose* pClosedTabData)
{
	return pImpl->AddToList(pClosedTabData);
}
BOOL CRecentClosedTabList::GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData)
{
	return pImpl->GetFromList(nItemID, ppClosedTabData);
}
BOOL CRecentClosedTabList::RemoveFromList(int nItemID)
{
	return pImpl->RemoveFromList(nItemID);
}


BOOL CRecentClosedTabList::ReadFromXmlFile()
{
	return pImpl->ReadFromXmlFile();
}
BOOL CRecentClosedTabList::WriteToXmlFile()
{
	return pImpl->WriteToXmlFile();
}


void CRecentClosedTabList::DeleteRecentClosedTabFile()
{
	pImpl->DeleteRecentClosedTabFile();
}

// Implementation
BOOL CRecentClosedTabList::UpdateMenu()
{
	return pImpl->UpdateMenu();
}
void CRecentClosedTabList::ResetMenu()
{
	pImpl->ResetMenu();
}











