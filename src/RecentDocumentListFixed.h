/**
 *	@file	RecentDocumentListFixed.h
 *	@brief	最近使ったドキュメントのリスト.
 */

#pragma once

#include "resource.h"


class CRecentDocumentListFixed
{
private:
	enum ERecentDoc {
		RECENTDOC_MENUTYPE_URL		= 0,
		RECENTDOC_MENUTYPE_TITLE	= 1,
		RECENTDOC_MENUTYPE_DOUBLE	= 2,
	};

	// Declarations
	struct _DocEntry {
		TCHAR										szDocName[INTERNET_MAX_PATH_LENGTH];
		std::vector<std::pair<CString, CString> >	arrFore;	//タイトル,URLのペアを持つ配列
		std::vector<std::pair<CString, CString> >	arrBack;
		CString 									strTitle;
		bool operator ==(const _DocEntry &de) const { return (lstrcmpi(szDocName, de.szDocName) == 0); }
	};

public:
	typedef _DocEntry  RecentDocEntry;

	enum {
		m_nMaxEntries_Min	= 2,
		m_nMaxEntries_Max	= ID_RECENTDOCUMENT_LAST - ID_RECENTDOCUMENT_FIRST + 1,
	};


	// Constructor/Destructor
	CRecentDocumentListFixed();
	~CRecentDocumentListFixed();

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
	class Impl;
	Impl*	pImpl;
};


