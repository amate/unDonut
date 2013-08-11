/**
*	@file	AutoLogin.h
*	@brief	ログイン情報を管理します
*/

#pragma once

#include <map>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include "CustomSerializeClass.h"
#include "SharedMemoryUtil.h"


#define GETLOGININFOMATIONSHAREDMEMNAME	_T("DonutGetLoginInfomationSharedMemName")

typedef std::map<CString, CString>	NameValueMap;
typedef std::map<CString, bool>		CheckboxMap;

struct LoginInfomation
{
	CString			strLoginUrl;
	NameValueMap	mapNameValue;
	CheckboxMap		mapCheckbox;
	bool			bAutoFillOnly;

	LoginInfomation() : bAutoFillOnly(false) { }

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & strLoginUrl & mapNameValue & mapCheckbox & bAutoFillOnly;
	}
};

struct LoginInfoData
{
	bool					bEnableAutoLogin;
	vector<LoginInfomation>	vecLoginInfo;

	LoginInfoData() : bEnableAutoLogin(true) { }

private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & bEnableAutoLogin & vecLoginInfo;
	}
};

////////////////////////////////////////
/// ログイン情報の入出力と検索、管理を行う

class CLoginDataManager
{
public:
	// for MainFrame
	static void CreateOriginalLoginDataList(HWND hWndMainFrame);

	// for ChildFrame
	static void UpdateLoginDataList(HWND hWndMainFrame);

	static bool DoAutoLogin(int nIndex, IWebBrowser2* pBrowser);

	static int	Find(LPCTSTR strUrl);
	static void	GetNameValueMap(int nIndex, NameValueMap*& pmap);
	static void	GetCheckboxMap(int nIndex, CheckboxMap*& pmapCheckbox);

protected:
	static void	Load();
	static void	Save();
	static void	Import(LPCTSTR strPath);
	static void	Export(LPCTSTR strPath);

	// Data members
	static LoginInfoData	s_loginInfoData;

private:
	static bool	_ReadData(LPCTSTR strPath, vector<char>& vecData);
	static void	_DeSerializeLoginData(const std::wstring& strData);
	static void	_SerializeLoginData(std::wstring& strData);
	static bool	_WriteData(LPCTSTR strPath, BYTE* pData, DWORD size);

	static CSharedMemory	s_sharedMem;
};



#include "resource.h"

////////////////////////////////////////
/// ログイン情報編集ダイアログ

class CLoginInfoEditDialog : 
	public CDialogImpl<CLoginInfoEditDialog>,
	public CWinDataExchange<CLoginInfoEditDialog>,
	protected CLoginDataManager
{
public:
	enum { IDD = IDD_LOGININFOEDITOR };

	enum { s_cMaxNameValue = 3, s_cMaxCheckbox = 2 };

	CLoginInfoEditDialog(const LoginInfomation& info);

	void	SetAutoLoginfunc(function<void ()> func) { m_funcAutoLogin = func; }

	// DDX map
    BEGIN_DDX_MAP( CLoginInfoEditDialog )
        DDX_CONTROL_HANDLE(IDC_URL_LIST, m_UrlList)
		DDX_CHECK( IDC_CHECKBOX_ENABLE_AUTOLOGIN, m_bEnableAutoLogin )

		DDX_TEXT(IDC_EDIT_URL, m_Url)
		DDX_TEXT(IDC_NAME1	, m_Name[0])
		DDX_TEXT(IDC_NAME2	, m_Name[1])
		DDX_TEXT(IDC_NAME3	, m_Name[2])
		DDX_TEXT(IDC_VALUE1	, m_Value[0])
		DDX_TEXT(IDC_VALUE2	, m_Value[1])
		DDX_TEXT(IDC_VALUE3	, m_Value[2])
		DDX_TEXT(IDC_CHECKNAME1, m_CheckboxName[0])
		DDX_TEXT(IDC_CHECKNAME2, m_CheckboxName[1])
		DDX_CHECK(IDC_CHECKBOX1, m_bCheck[0])
		DDX_CHECK(IDC_CHECKBOX2, m_bCheck[1])
		DDX_CHECK(IDC_CHECKBOX_AUTOFILLONLY, m_bAutoFillOnly )
    END_DDX_MAP()

	// Message map
	BEGIN_MSG_MAP_EX( CLoginInfoEditDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDC_BTN_APPLY, OnApply )
		COMMAND_ID_HANDLER_EX( IDC_BTN_ADD	, OnAdd )
		COMMAND_ID_HANDLER_EX( IDC_BTN_DELETE, OnDelete )
		COMMAND_ID_HANDLER_EX( IDC_BTN_IMPORT, OnImport )
		COMMAND_ID_HANDLER_EX( IDC_BTN_EXPORT, OnExport )
		COMMAND_ID_HANDLER_EX( IDC_BTN_TEST	, OnTest )
		COMMAND_ID_HANDLER_EX( IDCANCEL	, OnFinish )
		COMMAND_HANDLER_EX(IDC_URL_LIST, CBN_SELCHANGE, OnUrlListChange )
	END_MSG_MAP()

	BOOL	OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void	OnApply(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnNew(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnImport(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnExport(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnTest(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnFinish(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnUrlListChange(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	void	_SetLoginInfoData();
	void	_SetCopyLoginInfo();

	// Data members
	LoginInfomation	m_info;

	CComboBox	m_UrlList;

	bool	m_bEnableAutoLogin;
	CString	m_Url;
	CString	m_Name[s_cMaxNameValue];
	CString	m_Value[s_cMaxNameValue];
	CString m_CheckboxName[s_cMaxCheckbox];
	bool	m_bCheck[s_cMaxCheckbox];
	bool	m_bAutoFillOnly;

	function<void ()>	m_funcAutoLogin;
	
};























