/**
 *	@file	PluginDialog.h
 *	@brief	donutのオプション : プラグイン
 */

#pragma once

#include "../resource.h"
#include "../Include/PluginInfo.h"



class CPluginPropertyPage
	: public CPropertyPageImpl<CPluginPropertyPage>
	, public CWinDataExchange<CPluginPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_PLUGIN };

private:
	CSimpleMap<CString, PLUGININFO *> m_mapPlaginInfo;

	// Data members
	CCheckListViewCtrl				  m_listview;
	CString 						  m_strPluginName;
	CString 						  m_strPluginKind;
	CString 						  m_strPluginVer;
	CString 						  m_strPluginDate;
	CString 						  m_strPluginComment;
	CString 						  m_strAuthuorName;
	CString 						  m_strAuthuorURL;
	CString 						  m_strAuthuorEMail;

public:
	// DDX map
	BEGIN_DDX_MAP(CPluginPropertyPage)
		DDX_TEXT( IDC_ED_PLUGIN_NAME,	m_strPluginName 	);
		DDX_TEXT( IDC_ED_PLUGIN_KIND,	m_strPluginKind 	);
		DDX_TEXT( IDC_ED_PLUGIN_VER,	m_strPluginVer		);
		DDX_TEXT( IDC_ED_PLUGIN_DATE,	m_strPluginDate 	);
		DDX_TEXT( IDC_ED_PLUGIN_COMMENT,m_strPluginComment	);
		DDX_TEXT( IDC_ED_AUTHOR_NAME,	m_strAuthuorName	);
		DDX_TEXT( IDC_ED_AUTHOR_URL,	m_strAuthuorURL 	);
		DDX_TEXT( IDC_ED_AUTHOR_EMAIL,	m_strAuthuorEMail	);
	END_DDX_MAP()

	// Constructor
	CPluginPropertyPage();
	~CPluginPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Message map and handlers
	BEGIN_MSG_MAP( CPluginPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BTN_SETTING, OnBtnSetting )
		NOTIFY_HANDLER_EX( IDC_LIST_PLUGIN, LVN_ITEMCHANGED, OnListPluginSelectchange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CPluginPropertyPage> )
	END_MSG_MAP()

private:
	void	OnBtnSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// Implementation
	void	_GetData();
	void	_SetData();

	LRESULT OnListPluginSelectchange(LPNMHDR);

	void	InitListItem(const CString& strDirectory);
	void	DoCheckListItem();
};

