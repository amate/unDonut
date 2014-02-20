/**
*	@file	SerachBarHelper.h
*/

#pragma once

#include <vector>
#include <fstream>
#include <boost\optional.hpp>
#include "DonutPFunc.h"


class CSearchBarHelper
{
public:
	static void LoadData()
	{
		try {
			CString configPath = GetConfigFilePath(_T("SearchBarHelper.txt"));
			std::ifstream fs(configPath);
			if (!fs)
				throw std::exception();

			std::string line;
			while (std::getline(fs, line)) {
				std::size_t tabPos = line.find('\t');
				if (tabPos == std::string::npos)
					continue;
				CString url = line.substr(0, tabPos).c_str();
				url.MakeLower();
				CString name = line.substr(tabPos + 1).c_str();
				s_vecURL_name.emplace_back(url, name);
				if (fs.eof())
					break;
			}
		}
		catch (const std::exception& e) {
			e;
		}
	}

	static boost::optional<CString>	FindName(const CString& URL)
	{
		CString strURL = URL;
		strURL.MakeLower();

		for (auto& pair : s_vecURL_name) {
			if (strURL.GetLength() < pair.first.GetLength())
				continue;
			if (strURL.Left(pair.first.GetLength()) == pair.first)
				return pair.second;
		}
		return boost::none;
	}


private:
	// Data members
	static std::vector<std::pair<CString, CString>>	s_vecURL_name;

};

__declspec(selectany) std::vector<std::pair<CString, CString>>	CSearchBarHelper::s_vecURL_name;

























