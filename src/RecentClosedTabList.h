/**
 *	@file	RecentClosedTabList.h
 *	@brief	最近閉じたタブのリスト.
 */

#pragma once

#include "resource.h"
#include <memory>

// 前方宣言
struct ChildFrameDataOnClose;


///////////////////////////////////////
// CRecentClosedTabList

class CRecentClosedTabList
{
public:
	enum {
		kMaxEntries_Min	= 2,
		kMaxEntries_Max	= ID_RECENTDOCUMENT_LAST - ID_RECENTDOCUMENT_FIRST + 1,
	};

	// Constructor/Destructor
	CRecentClosedTabList();
	~CRecentClosedTabList();

	// Attributes
	int  GetMaxEntries() const;
	void SetMaxEntries(int nMaxEntries);

	int  GetMaxItemLength() const;
	void SetMaxItemLength(int cchMaxLen);

	int  GetMenuType() const;
	void SetMenuType(int nMode);

	int  GetRecentCount() const;

	// Operations
	BOOL AddToList(std::unique_ptr<ChildFrameDataOnClose>&& pClosedTabData);
	BOOL GetFromList(int nItemID, ChildFrameDataOnClose** ppClosedTabData);
	BOOL RemoveFromList(int nItemID);


	BOOL ReadFromXmlFile();
	BOOL WriteToXmlFile();

	void DeleteRecentClosedTabFile();
	
private:
	class Impl;
	Impl*	pImpl;
};


