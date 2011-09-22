/**
 *	@file	DonutViewOption.h
 *	@brief	Viewのオプション設定.
 */

#pragma once

#include "MtlUpdateCmdUI.h"
#include "option/DLControlOption.h"
#include "resource.h"


#if 1 //+++ .dfg セーブ用の情報
#include "MtlProfile.h"
#include "IniFile.h"


///+++ .dfg へ書き込むセーブ情報.
struct SDfgSaveInfo {
	CString 			m_section;
	CString 			m_title;
	CString 			m_location_URL;
	DWORD				m_DL_Control_Flags;
	DWORD				m_dwAutoRefreshStyle;
	CWindowPlacement	m_wndpl;
	DWORD				m_dwExStyle;

	//x typedef std::list<std::pair<CString, CString> > 	List;
	typedef std::vector<std::pair<CString, CString> >		List;
	List				m_listFore;
	List				m_listBack;

public:
	SDfgSaveInfo() : m_dwAutoRefreshStyle(0), m_dwExStyle(0) {;}

	void WriteProfile(CIniFileO& pr, bool bSaveTravelLog) {
		pr.ChangeSectionName( m_section );
		pr.SetStringUW( m_title				, _T("Title") ); //minit
		pr.SetString( m_location_URL 		, _T("Location_URL") );
		pr.SetValue ( m_DL_Control_Flags 	, _T("DL_Control_Flags") );
		pr.SetValue ( m_dwAutoRefreshStyle	, _T("Auto_Refresh_Style") );
		m_wndpl.WriteProfile(pr , _T("child."));
		pr.SetValue( m_dwExStyle, _T("Extended_Style") );

		if (bSaveTravelLog) {	// 進む戻る履歴を保存する場合.
			CString 		strTmp;
			CString 		strDir	= "Fore";
			unsigned		i		= 0;
			for (SDfgSaveInfo::List::iterator l = m_listFore.begin();
				l != m_listFore.end();
				++l)
			{
				strTmp.Format(_T("%s_Title%d"), strDir, i);
				pr.SetStringUW(l->first , strTmp);
				strTmp.Format(_T("%s_URL%d")  , strDir, i);
				pr.SetString(l->second, strTmp);
				++i;
			}
			strDir	= "Back";
			i		= 0;
			for (SDfgSaveInfo::List::iterator l = m_listBack.begin();
				l != m_listBack.end();
				++l)
			{
				strTmp.Format(_T("%s_Title%d"), strDir, i);
				pr.SetStringUW(l->first , strTmp);
				strTmp.Format(_T("%s_URL%d")  , strDir, i);
				pr.SetString(l->second, strTmp);
				++i;
			}
		}
	}
};
#endif


#if 0	//:::

//#define DVS_EX_OPENNEWWIN 	0x00000001L
//#define DVS_EX_MESSAGE_FILTER 0x00000002L



template <class _DonutView>
class CDonutViewOption {
private:
	CString 	m_strURL;


public:
	void	WriteProfile(const CString &strFileName, int nIndex);	//+++	未使用状態
	void	GetProfile(const CString &strFileName, int nIndex, bool bGetChildFrameState);
	void	_GetProfile(const CString &strFileName, const CString &strSection, bool bGetChildFrameState);


public:
	BOOL _OutPut_TravelLogs(SDfgSaveInfo::List& listFore, SDfgSaveInfo::List& listBack);

	BOOL GetDfgSaveInfo(SDfgSaveInfo & rDfgSaveInfo, bool bSaveFB);

};




#if 1	//+++ 未使用状態? 念のため残す...
template <class _DonutView>
void	CDonutViewOption<_DonutView>::WriteProfile(const CString &strFileName, int nIndex)
{
	bool			bSaveFB = false;
	SDfgSaveInfo	dfgSaveInfo;
	dfgSaveInfo.m_section.Format(_T("Window%d"), nIndex);
	GetDfgSaveInfo(dfgSaveInfo, bSaveFB);
	CIniFileO		pr( strFileName, _T("Window") );
	dfgSaveInfo.WriteProfile(pr, bSaveFB);
}
#endif



template <class _DonutView>
void	CDonutViewOption<_DonutView>::GetProfile(const CString &strFileName, int nIndex, bool bGetChildFrameState)
{
	CString strSection;

	strSection.Format(_T("Window%d"), nIndex);
	_GetProfile(strFileName, strSection, bGetChildFrameState);
}



template <class _DonutView>
void	CDonutViewOption<_DonutView>::_GetProfile(const CString &strFileName, const CString & strSection, bool bGetChildFrameState)
{
	CString 	strSectionName	 = strSection;

	CIniFileI	pr(strFileName, strSectionName);

	DWORD		dwDLControlFlags = CDLControlOption::DLCTL_DEFAULT;

	if (pr.QueryValue( dwDLControlFlags, _T("DL_Control_Flags") ) == ERROR_SUCCESS)
		__m_pDonutView->PutDLControlFlags(dwDLControlFlags);

	TCHAR		szUrl[INTERNET_MAX_PATH_LENGTH];
	szUrl[0]			= 0;
	DWORD		dwBytes = INTERNET_MAX_PATH_LENGTH;

	if (pr.QueryString(szUrl, _T("Location_URL"), &dwBytes) == ERROR_SUCCESS) {
		// avoid endless loop
		CString strURL(szUrl);
		bool	bMaybeEndless = ( strURL == _T("javascript:location.reload()") );

		if (!bMaybeEndless)
			__m_pDonutView->Navigate2(szUrl);
	}

	pr.QueryValue( m_dwAutoRefreshStyle, _T("Auto_Refresh_Style") );
	_SetTimer();

	if (bGetChildFrameState)
		MtlGetProfileChildFrameState( pr, __m_pDonutView->GetParent(), SW_SHOWNORMAL, true, _T("child.") );

	// load extended style
	pr.QueryValue( m_dwExStyle, _T("Extended_Style") ); 	// never give up yeah.
	pr.Close();

	if ( _check_flag(DVS_EX_OPENNEWWIN, m_dwExStyle) ) {
		MtlSendOnCommand(__m_pDonutView->GetParent(), ID_DOCHOSTUI_OPENNEWWIN);
	}
}



//+++ CComPtr<ITravelLogStg>pTLStg をフレームのある頁に使うとハングする... でいろいろテスト中
template <class _DonutView>
BOOL	CDonutViewOption<_DonutView>::_OutPut_TravelLogs(SDfgSaveInfo::List& listFore, SDfgSaveInfo::List& listBack)
{
	CComPtr<IWebBrowser2>		 pWB2 = 0;
	__m_pDonutView->QueryControl(IID_IWebBrowser2, (void **) &pWB2);
	if (pWB2 == NULL)
		return FALSE;

	CComPtr<IServiceProvider>	 pISP = 0;
	HRESULT 					 hr;
	hr	= pWB2->QueryInterface(IID_IServiceProvider, (void **) &pISP);
	if (FAILED(hr) || pISP == NULL)
		return FALSE;

	CComPtr<ITravelLogStg>		 pTLStg = 0;
	hr	= pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
	if (FAILED(hr) || pTLStg == NULL)
		return FALSE;

	DWORD	dwCount[2] = { 0, 0};
	try {
		hr	= pTLStg->GetCount(TLEF_RELATIVE_FORE/*|TLEF_RELATIVE_INCLUDE_CURRENT*/, &dwCount[0]);
		if (FAILED(hr))
			return FALSE;
		hr	= pTLStg->GetCount(TLEF_RELATIVE_BACK/*|TLEF_RELATIVE_INCLUDE_CURRENT*/, &dwCount[1]);
		if (FAILED(hr))
			return FALSE;
	} catch (...) {
		ErrorLogPrintf(_T("ERROR: _OutPut_TravelLog: ..\n"));
		return FALSE;
	}
	if (dwCount[0] == 0 && dwCount[1] == 0) // 単に履歴がないだけの状態ならtrue.
		return TRUE;
	//--dwCount[0];
	//--dwCount[1];

	CComPtr<IEnumTravelLogEntry> pTLEnum[2] = {0, 0};
	try {
		hr	= pTLStg->EnumEntries(TLEF_RELATIVE_FORE, &pTLEnum[0]);
		if (FAILED(hr) || pTLEnum[0] == NULL)
			return FALSE;
		hr	= pTLStg->EnumEntries(TLEF_RELATIVE_BACK, &pTLEnum[1]);
		if (FAILED(hr) || pTLEnum[1] == NULL)
			return FALSE;
	} catch (...) {
		ErrorLogPrintf(_T("ERROR: _OutPut_TravelLog: ...\n"));
		return FALSE;
	}
	listFore.reserve(10);
	listBack.reserve(10);
	for (unsigned i = 0; i < 2; ++i) {
		int 	count = 0;
		for (unsigned j = 0; j < dwCount[i]; ++j) {
			CComPtr<ITravelLogEntry>  pTLEntry	= NULL;
			LPOLESTR				  szURL 	= NULL;
			LPOLESTR				  szTitle	= NULL;
			try {
				DWORD	dummy = 0;
				hr = pTLEnum[i]->Next(1, &pTLEntry, &dummy);
			} catch (...) { //+++
				ErrorLogPrintf(_T("ERROR: _OutPut_TravelLog n=%d: ...\n"), count);
				hr		 = -1;
				pTLEntry = 0;
			}
			if (pTLEntry == NULL || FAILED(hr))
				break;

			if ( SUCCEEDED( pTLEntry->GetTitle(&szTitle) ) && szTitle
			  && SUCCEEDED( pTLEntry->GetURL  (&szURL  ) ) && szURL  )
			{
				SDfgSaveInfo::List& arrData = (i == 0) ? listFore : listBack;
				arrData.push_back( std::make_pair( CString(szTitle), CString(szURL) ) );
				++count;
			}

		  #if 1 //+++	開放忘れ( http://donut.blog.shinobi.jp/Entry/4/ RAPTのブログ メモリーリーク修正 を元に)
			if (szTitle)
				::CoTaskMemFree( szTitle );
			if (szURL)
				::CoTaskMemFree( szURL );
		  #endif

			if (count >= 10)
				break;
		}
	}

	return TRUE;
}



/// 自動バックアップでの、_WrilteProfileの代わり.
template <class _DonutView>
BOOL	CDonutViewOption<_DonutView>::GetDfgSaveInfo(SDfgSaveInfo & rDfgSaveInfo, bool bSaveFB)
{
	CComPtr<IHTMLDocument2> 	pDoc;
	__m_pDonutView->m_spBrowser->get_Document( (IDispatch **) &pDoc );

	rDfgSaveInfo.m_title				= __m_pDonutView->GetLocationName();
	rDfgSaveInfo.m_location_URL 		= __m_pDonutView->GetLocationURL();
	rDfgSaveInfo.m_DL_Control_Flags 	= __m_pDonutView->GetDLControlFlags();
	rDfgSaveInfo.m_dwAutoRefreshStyle	= m_dwAutoRefreshStyle;
	MTLVERIFY( ::GetWindowPlacement(__m_pDonutView->GetParent(), &rDfgSaveInfo.m_wndpl) );
	rDfgSaveInfo.m_dwExStyle			= m_dwExStyle;

	// 進む・戻る履歴を保存
	if (bSaveFB) {
		_OutPut_TravelLogs( rDfgSaveInfo.m_listFore, rDfgSaveInfo.m_listBack );
	}
	return TRUE;
}

// http://msdn2.microsoft.com/en-us/library/aa768369.aspx

#endif