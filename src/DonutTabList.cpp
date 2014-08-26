#include "stdafx.h"
#include "DonutTabList.h"
#include <fstream>
#include <codecvt>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include "option\DLControlOption.h"
#include "Misc.h"

using boost::property_tree::wptree;

///////////////////////////////////////////////////////////////////////////
// CDonutTabList

CDonutTabList::CDonutTabList() : m_nActiveIndex(-1)
{
}

CDonutTabList::CDonutTabList(CDonutTabList&& tabList) : 
	m_vecpChildFrameData(std::move(tabList.m_vecpChildFrameData)), m_nActiveIndex(tabList.m_nActiveIndex)
{
}


bool	CDonutTabList::Load(const CString& filePath)
{
	try {
		std::wifstream	filestream(filePath);
		if (!filestream) {
			return false;
		}
		filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

		wptree	pt;
		boost::property_tree::read_xml(filestream, pt);

		auto SetTravelLog	= [](wptree& ptLog, vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = ptLog.begin(); it != ptLog.end(); ++it) {
				wptree& item = it->second;
				vecTravelLog.push_back(std::pair<CString, CString>(
					item.get(L"<xmlattr>.title", L"").c_str(), 
					item.get(L"<xmlattr>.url", L"").c_str()));
			}
		};
		
		m_vecpChildFrameData.clear();

		wptree&	ptChild = pt.get_child(L"TabList");
		auto it = ptChild.begin();
		m_nActiveIndex = it->second.get(L"ActiveIndex", -1);
		++it;
		for (; it != ptChild.end(); ++it) {
			wptree& ptItem = it->second;
			std::unique_ptr<ChildFrameDataOnClose>	pdata(new ChildFrameDataOnClose);
			pdata->strTitle	= ptItem.get(L"<xmlattr>.title", L"").c_str();
			pdata->strURL	= ptItem.get(L"<xmlattr>.url", L"").c_str();
			pdata->dwDLCtrl	= ptItem.get<DWORD>(L"<xmlattr>.DLCtrlFlags", CDLControlOption::s_dwDLControlFlags);
			pdata->dwExStyle= ptItem.get<DWORD>(L"<xmlattr>.ExStyle",	CDLControlOption::s_dwExtendedStyleFlags);
			pdata->dwAutoRefreshStyle	= ptItem.get<DWORD>(L"<xmlattr>.AutoRefreshStyle", 0);
			SetTravelLog(ptItem.get_child(L"TravelLog.Back"), pdata->TravelLogBack);
			SetTravelLog(ptItem.get_child(L"TravelLog.Fore"), pdata->TravelLogFore);

			m_vecpChildFrameData.push_back(std::move(pdata));
		}
	} catch (...) {
		m_vecpChildFrameData.clear();
		m_nActiveIndex = -1;
		return false;
	}

	return true;
}

bool	CDonutTabList::Save(const CString& filePath, bool bBackup /*= true*/)
{
	try {
		auto AddTravelLog = [](wptree& ptLog, const vector<std::pair<CString, CString> >& vecTravelLog) {
			for (auto it = vecTravelLog.cbegin(); it != vecTravelLog.cend(); ++it) {
				wptree& ptItem = ptLog.add(L"item", L"");
				ptItem.put(L"<xmlattr>.title", (LPCTSTR)it->first);
				ptItem.put(L"<xmlattr>.url"	 , (LPCTSTR)it->second);
			}
		};
		wptree	pt;
		wptree&	ptTabList = pt.add(L"TabList", L"");
		ptTabList.add(L"<xmlattr>.ActiveIndex", m_nActiveIndex);
		for (auto it = m_vecpChildFrameData.cbegin(); it != m_vecpChildFrameData.cend(); ++it) {
			ChildFrameDataOnClose& data = *(*it);
			wptree& ptItem = ptTabList.add(L"Tab", L"");
			ptItem.put(L"<xmlattr>.title", (LPCTSTR)data.strTitle);
			ptItem.put(L"<xmlattr>.url"	 , (LPCTSTR)data.strURL);
			ptItem.put(L"<xmlattr>.DLCtrlFlags", data.dwDLCtrl);
			ptItem.put(L"<xmlattr>.ExStyle",	data.dwExStyle);
			ptItem.put(L"<xmlattr>.AutoRefreshStyle", data.dwAutoRefreshStyle);
			AddTravelLog(ptItem.add(L"TravelLog.Back", L""), data.TravelLogBack);
			AddTravelLog(ptItem.add(L"TravelLog.Fore", L""), data.TravelLogFore);
		}
		using namespace boost::property_tree::xml_parser;

		CString fileExt = Misc::GetFileExt(filePath);
		CString fileBaseNoExt = Misc::GetFileNameNoExt(filePath);

		CString strTempTabList = fileBaseNoExt + _T(".temp.") + fileExt;
		std::wofstream filestream(strTempTabList);
		if (!filestream)
			throw "error";
		filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
		write_xml(filestream, pt, xml_writer_make_settings<std::wstring>(L' ', 2, widen<std::wstring>("UTF-8")));

		// .bak. ファイルに元ファイルを残しておく
		if (bBackup) {
			if (::PathFileExists(filePath)) {
				CString strBakFile = Misc::GetFileNameNoExt(filePath) + _T(".bak.") + fileExt;
				::MoveFileEx(filePath, strBakFile, MOVEFILE_REPLACE_EXISTING);
			}
		}
		filestream.close();
		::MoveFileEx(strTempTabList, filePath, MOVEFILE_REPLACE_EXISTING);

	} catch (...) {
		return false;
	}
	return true;
}

