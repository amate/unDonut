/**
*	@file	UserDefinedCSSOption.h
*	@brief	ユーザー定義CSS
*/

#pragma once

#include <boost/optional.hpp>
#include "../resource.h"
#include "../FileNotification.h"

struct UserDefinedCssData {
	CString strUrl;
	CString strCSSPath;
};


class CUserDefinedCSSOption
{
	friend bool _PrivateInit();
public:
	static boost::optional<CString>	FindURL(const CString& strUrl);

protected:
	static void	LoadUserCSSConfig();
	static void SaveUserCSSConfig();

	static vector<unique_ptr<UserDefinedCssData> >	s_vecpData;

};




class CUserDefinedCSSPropertyPage
	: public CPropertyPageImpl<CUserDefinedCSSPropertyPage>
	, public CWinDataExchange<CUserDefinedCSSPropertyPage>
	, public CDragListNotifyImpl<CUserDefinedCSSPropertyPage>
	, public COwnerDraw<CUserDefinedCSSPropertyPage>
	, protected CUserDefinedCSSOption
{
public:
	enum { IDD = IDD_PROPPAGE_USERDEFINEDCSS };

	CUserDefinedCSSPropertyPage(const CString& strUrl);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	BOOL	OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/);
	void	OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/);
	int		OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/);
	void	OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/);

	void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	BEGIN_DDX_MAP( CUserDefinedCSSPropertyPage )
		DDX_CONTROL_HANDLE( IDC_LIST_URL, m_ListUrl )
		DDX_CONTROL_HANDLE( IDC_EDIT_URL, m_editUrl )
		DDX_CONTROL_HANDLE( IDC_COMBO_CSS, m_cmbCSS )		
	END_DDX_MAP()

	BEGIN_MSG_MAP( CUserDefinedCSSPropertyPage )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD	, OnAdd )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_DELETE, OnDelete )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_CHANGEAPPLY, OnChangeApply )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_CSSEDIT, OnCSSEdit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_OPENCSSFOLDER, OnOpenCSSFolder )
		COMMAND_HANDLER_EX( IDC_LIST_URL, LBN_SELCHANGE	, OnListSelChange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CUserDefinedCSSPropertyPage> )
		CHAIN_MSG_MAP( CDragListNotifyImpl<CUserDefinedCSSPropertyPage> )
		CHAIN_MSG_MAP( COwnerDraw<CUserDefinedCSSPropertyPage> )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnChangeApply(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCSSEdit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnOpenCSSFolder(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnListSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);


private:
	void	_initComboBoxCSS();

	// Data members
	CString			m_strUrl;
	CDragListBox	m_ListUrl;
	CEdit			m_editUrl;
	CComboBox		m_cmbCSS;
	CFileNotification	m_fileNotification;
};



















