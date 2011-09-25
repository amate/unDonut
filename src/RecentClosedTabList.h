/**
 *	@file	RecentClosedTabList.h
 *	@brief	最近閉じたタブのリスト.
 */

#pragma once

#include "resource.h"

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
	HMENU GetMenuHandle() const;
	void  SetMenuHandle(HMENU hMenu);

	int  GetMaxEntries() const;
	void SetMaxEntries(int nMaxEntries);

	int  GetMaxItemLength() const;
	void SetMaxItemLength(int cchMaxLen);

	int  GetMenuType() const;
	void SetMenuType(int nMode);

	int  GetRecentCount() const;

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
	class Impl;
	Impl*	pImpl;
};


