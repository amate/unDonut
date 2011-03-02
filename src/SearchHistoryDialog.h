/**
 *	@file	SearchHistoryDialog.h
 *	@brief	履歴検索ダイアログ.
 */
#pragma once

#include "resource.h"
#include "MtlWin.h"



class CSearchHistoryDialog : public CDialogImpl<CSearchHistoryDialog> 
{
public:
	enum { IDD = IDD_DIALOG_SEARCHHISTORY };

	int 		m_bUseHiFunction;
	CString 	m_strKeyWord;
	SYSTEMTIME	m_sysTimeStart;
	SYSTEMTIME	m_sysTimeEnd;
	BOOL		m_bCheckDate;

private:
	int 		m_nRetCode;

public:
	CSearchHistoryDialog() : m_nRetCode(0)
	{
		//日付指定コントロールを試用するための初期化
		INITCOMMONCONTROLSEX	 icex;

		icex.dwSize = sizeof ( INITCOMMONCONTROLSEX );
		icex.dwICC	= ICC_DATE_CLASSES ;
		InitCommonControlsEx(&icex);

		//IE5.5以降の履歴用インターフェイスが使えるかチェック
		CComPtr<IUrlHistoryStg2> pHistory;
		HRESULT 				 hr = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg2, (void **) &pHistory);
		if ( FAILED(hr) ) {
			m_bUseHiFunction = FALSE;
		} else {
			m_bUseHiFunction = TRUE;
		}
	}


public:
	

	BEGIN_MSG_MAP(CSearchHistoryDialog)
		MESSAGE_HANDLER 	( WM_INITDIALOG , OnInitDialog )
		COMMAND_ID_HANDLER	( IDC_BTN_SEARCH, OnSearch )
		COMMAND_ID_HANDLER	( IDC_BTN_CANCEL, OnCancel )
	END_MSG_MAP()


private:
	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_GetData();
		m_nRetCode = 1;
		EndDialog(m_nRetCode);
		return 0;
	}


	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		m_nRetCode = 0;
		EndDialog(m_nRetCode);
		return 0;
	}


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		if (m_bUseHiFunction == FALSE) {
			CDateTimePickerCtrl 	dateStart = GetDlgItem(IDC_DATETIMEPICKER1);
			CDateTimePickerCtrl 	dateEnd   = GetDlgItem(IDC_DATETIMEPICKER2);
			CButton 				btnCheck  = GetDlgItem(IDC_CHECK_DATE);
			dateStart.EnableWindow(FALSE);
			dateEnd.EnableWindow(FALSE);
			btnCheck.EnableWindow(FALSE);
		}

		return 0;
	}


	void _GetData()
	{
		CDateTimePickerCtrl dateStart = GetDlgItem(IDC_DATETIMEPICKER1);
		CDateTimePickerCtrl dateEnd   = GetDlgItem(IDC_DATETIMEPICKER2);
		CButton 			btnCheck  = GetDlgItem(IDC_CHECK_DATE);
		CEdit				edit	  = GetDlgItem(IDC_EDIT_KEYWORD);

		dateStart.GetSystemTime(&m_sysTimeStart);
		dateStart.GetSystemTime(&m_sysTimeEnd);
		m_bCheckDate =	btnCheck.GetCheck() != 0/*? TRUE : FALSE*/;
		m_strKeyWord = MtlGetWindowText(edit.m_hWnd);
	}

};
