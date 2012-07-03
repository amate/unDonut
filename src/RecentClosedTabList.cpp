/**
 *	@file	RecentClosedTabList.cpp
 *	@brief	最近閉じたタブのリスト.
 */

#include "stdafx.h"
#include "RecentClosedTabList.h"
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\thread.hpp>
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
	// Constructor/Destructor
	Impl();
	~Impl();

	// Attributes
	int  GetMaxEntries() const { return m_nMaxEntries; }
	void SetMaxEntries(int nMaxEntries);

	int  GetMaxItemLength() const { return m_cchMaxItemLen; }
	void SetMaxItemLength(int cchMaxLen);	// メニューの最大文字列

	int  GetMenuType() const { return m_nMenuType; }
	void SetMenuType(int nMode) { m_nMenuType = nMode; }

	int  GetRecentCount() const { return m_bLoading ? 0 : (int)m_vecpClosedTabData.size(); }

	// Operations
	BOOL AddToList(ChildFrameDataOnClose* pClosedTabData);
	BOOL GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData);
	BOOL RemoveFromList(int nItemID);


	BOOL ReadFromXmlFile();
	BOOL WriteToXmlFile();

	void DeleteRecentClosedTabFile();

private:

	// Overrideable
	// override to provide a different method of compacting document names
	static bool CompactDocumentName(LPTSTR lpstrOut, LPCTSTR lpstrIn, int cchLen)
	{
		return AtlCompactPathFixed(lpstrOut, lpstrIn, cchLen);
	}
	 
	CString _GetRecentCloseFile() { return GetConfigFilePath("RecentClosedTab.xml"); }


	// Data members
	vector<unique_ptr<ChildFrameDataOnClose> >	m_vecpClosedTabData;	// beginが最も最後に閉じたタブ
	int 	m_nMaxEntries;		// default is 4
	TCHAR	m_szNoEntries[INTERNET_MAX_PATH_LENGTH];
	int 	m_cchMaxItemLen;
	int 	m_nMinID;
	int 	m_nMaxID;
	int 	m_nMenuType;
	bool	m_bLoading;
	boost::thread m_thrdLoading;
	CCriticalSection	m_csLoading;
};


// =====================================================================

// Constructor
CRecentClosedTabList::Impl::Impl()
	: m_nMaxEntries(4)
	, m_cchMaxItemLen(100)
	, m_nMenuType(RECENTDOC_MENUTYPE_URL)
	, m_bLoading(false)
{
	m_nMinID = ID_RECENTDOCUMENT_FIRST;
	m_nMaxID = ID_RECENTDOCUMENT_LAST;
	m_szNoEntries[0] = 0;					//+++
	ATLASSERT(INTERNET_MAX_PATH_LENGTH > m_cchMaxItemLen_Min);
}

CRecentClosedTabList::Impl::~Impl()
{
	if (m_thrdLoading.joinable())
		m_thrdLoading.join();
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
}



// Oparation
BOOL CRecentClosedTabList::Impl::AddToList(ChildFrameDataOnClose* pClosedTabData)
{
	CCritSecLock	lock(m_csLoading);
	while ((int)m_vecpClosedTabData.size() > m_nMaxEntries)
		m_vecpClosedTabData.erase(m_vecpClosedTabData.begin() + m_nMaxEntries - 1);

	m_vecpClosedTabData.insert(m_vecpClosedTabData.begin(), unique_ptr<ChildFrameDataOnClose>(std::move(pClosedTabData)));

	return TRUE;
}

BOOL CRecentClosedTabList::Impl::GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData)
{
	CCritSecLock	lock(m_csLoading);
	int nIndex = nItemID - m_nMinID;
	if ( nIndex < 0 || nIndex >= (int)m_vecpClosedTabData.size() )
		return FALSE;

	*ppClosedTabData = 	m_vecpClosedTabData[nIndex].get();
	return TRUE;
}

BOOL CRecentClosedTabList::Impl::RemoveFromList(int nItemID)
{
	CCritSecLock	lock(m_csLoading);
	int  nIndex = nItemID - m_nMinID;
	m_vecpClosedTabData.erase(m_vecpClosedTabData.begin() + nIndex);

	return TRUE;
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
	m_bLoading = true;
	m_thrdLoading.swap(boost::thread([this]() {
		m_vecpClosedTabData.clear();
		try {
			std::wifstream	filestream(_GetRecentCloseFile());
			if (!filestream) {
				m_bLoading = false;
				return ;
			}
			filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
			TIMERSTART();
			using boost::property_tree::wptree;
			wptree	pt;
			boost::property_tree::read_xml(filestream, pt);
			
			auto SetTravelLog	= [](wptree& ptLog, vector<std::pair<CString, CString> >& vecTravelLog) {
				for (auto it = ptLog.begin(); it != ptLog.end(); ++it) {
					wptree& item = it->second;
					CString title = item.get(L"<xmlattr>.Title", L"").c_str();
					title.Replace(_T("&quot;"), _T("\""));
					CString url	  = item.get(L"<xmlattr>.URL", L"").c_str();
					url.Replace(_T("&quot;"), _T("\""));
					vecTravelLog.push_back(std::make_pair(title, url));
				}
			};
			CCritSecLock	lock(m_csLoading);
			m_vecpClosedTabData.reserve(m_nMaxEntries);
			if (auto opRoot = pt.get_child_optional(L"RecentCloseTab")) {
				wptree& ptChild = opRoot.get();
				for (auto it = ptChild.begin(); it != ptChild.end(); ++it) {
					unique_ptr<ChildFrameDataOnClose>	pdata(new ChildFrameDataOnClose);
					wptree& ptItem = it->second;
		
					pdata->strTitle	= ptItem.get(L"<xmlattr>.Title", L"").c_str();
					pdata->strTitle.Replace(_T("&quot;"), _T("\""));
					pdata->strURL	= ptItem.get(L"<xmlattr>.URL", L"").c_str();
					pdata->strURL.Replace(_T("&quot;"), _T("\""));
					pdata->dwDLCtrl	= ptItem.get<DWORD>(L"<xmlattr>.DLCtrl", CDLControlOption::s_dwDLControlFlags);
					SetTravelLog(ptItem.get_child(L"TravelLog.Back"), pdata->TravelLogBack);
					SetTravelLog(ptItem.get_child(L"TravelLog.Fore"), pdata->TravelLogFore);
					m_vecpClosedTabData.push_back(std::move(pdata));
				}
			}
		} catch (boost::property_tree::xml_parser_error e) {
			ATLTRACE(e.what());
		} catch (...) {
			ATLTRACE(_T("CRecentClosedTabList::Impl::ReadFromXmlFile で例外発生!\n"));
		}
		TIMERSTOP(L"RecentClosedTabファイル読み込み完了");
		m_bLoading = false;
	}));
	return TRUE;
}


BOOL CRecentClosedTabList::Impl::WriteToXmlFile()
{
	using boost::property_tree::wptree;

	try {
		auto AddTravelLog = [](wptree& ptLog, vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = vecTravelLog.begin(); it != vecTravelLog.end(); ++it) {
				wptree& ptItem = ptLog.add(L"item", L"");
				it->first.Replace(_T("\""), _T("&quot;"));
				it->second.Replace(_T("\""), _T("&quot;"));
				ptItem.put(L"<xmlattr>.Title", (LPCTSTR)it->first);
				ptItem.put(L"<xmlattr>.URL"	 , (LPCTSTR)it->second);
			}
		};
		wptree	pt;
		wptree& ptRecentCloseTab = pt.add(L"RecentCloseTab", L"");
		for (auto it = m_vecpClosedTabData.begin(); it != m_vecpClosedTabData.end(); ++it) {
			ChildFrameDataOnClose& data = *(*it);
			wptree& ptItem = ptRecentCloseTab.add(L"item", L"");
			data.strTitle.Replace(_T("\""), _T("&quot;"));
			ptItem.put(L"<xmlattr>.Title", (LPCTSTR)data.strTitle);
			data.strURL.Replace(_T("\""), _T("&quot;"));
			ptItem.put(L"<xmlattr>.URL"	 , (LPCTSTR)data.strURL);
			ptItem.put(L"<xmlattr>.DLCtrl", data.dwDLCtrl);
			AddTravelLog(ptItem.add(L"TravelLog.Back", L""), data.TravelLogBack);
			AddTravelLog(ptItem.add(L"TravelLog.Fore", L""), data.TravelLogFore);
		}

		
		using namespace boost::property_tree::xml_parser;

		std::wofstream filestream(_GetRecentCloseFile() + _T(".temp"));
		if (!filestream)
			return FALSE;
		filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
		write_xml(filestream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));
		filestream.close();

		::MoveFileEx(_GetRecentCloseFile() + _T(".temp"), _GetRecentCloseFile(), MOVEFILE_REPLACE_EXISTING);

	} catch (...) {
		ATLTRACE(_T("CRecentClosedTabList::Impl::WriteToXmlFile で例外発生!\n"));
		return FALSE;
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











