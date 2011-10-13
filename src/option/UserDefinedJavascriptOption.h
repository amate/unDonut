/**
*	@file	UserDefinedJavascriptOption.h
*	@brief	ユーザー定義Javascript
*/

#pragma once



#include <boost/optional.hpp>
#include "../resource.h"
#include "../FileNotification.h"

struct UserDefinedJsData {
	CString strUrl;
	CString strJsPath;
};


class CUserDefinedJsOption
{
	friend bool _PrivateInit();
public:
	static boost::optional<CString>	FindURL(const CString& strUrl);

protected:
	static void	LoadUserJsConfig();
	static void SaveUserJsConfig();

	static vector<unique_ptr<UserDefinedJsData> >	s_vecpData;

};




class CUserDefinedJsPropertyPage
	: public CPropertyPageImpl<CUserDefinedJsPropertyPage>
	, public CWinDataExchange<CUserDefinedJsPropertyPage>
	, public CDragListNotifyImpl<CUserDefinedJsPropertyPage>
	, public COwnerDraw<CUserDefinedJsPropertyPage>
	, protected CUserDefinedJsOption
{
public:
	enum { IDD = IDD_PROPPAGE_USERDEFINEDJAVASCRIPT };

	CUserDefinedJsPropertyPage(const CString& strUrl);

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

	BEGIN_DDX_MAP( CUserDefinedJsPropertyPage )
		DDX_CONTROL_HANDLE( IDC_LIST_URL, m_ListUrl )
		DDX_CONTROL_HANDLE( IDC_EDIT_URL, m_editUrl )
		DDX_CONTROL_HANDLE( IDC_COMBO_JS, m_cmbJs )		
	END_DDX_MAP()

	BEGIN_MSG_MAP( CUserDefinedJsPropertyPage )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD	, OnAdd )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_DELETE, OnDelete )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_CHANGEAPPLY, OnChangeApply )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_JSEDIT, OnJsEdit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_OPENJSFOLDER, OnOpenJavascriptFolder )
		COMMAND_HANDLER_EX( IDC_LIST_URL, LBN_SELCHANGE	, OnListSelChange )
		CHAIN_MSG_MAP( CPropertyPageImpl<CUserDefinedJsPropertyPage> )
		CHAIN_MSG_MAP( CDragListNotifyImpl<CUserDefinedJsPropertyPage> )
		CHAIN_MSG_MAP( COwnerDraw<CUserDefinedJsPropertyPage> )
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnChangeApply(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnJsEdit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnOpenJavascriptFolder(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnListSelChange(UINT uNotifyCode, int nID, CWindow wndCtl);


private:
	void	_initComboBoxJs();

	// Data members
	CString			m_strUrl;
	CDragListBox	m_ListUrl;
	CEdit			m_editUrl;
	CComboBox		m_cmbJs;
	CFileNotification	m_fileNotification;
};



































