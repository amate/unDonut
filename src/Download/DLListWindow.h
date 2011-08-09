/**
*	@file	DLListWindow.h
*	@brief	タブからDropされたURLを表示/DLの実行をするウィンドウ
*/

#pragma once

#include "../resource.h"
#include "../MtlDragDrop.h"

/////////////////////////////////////////////////////////
// CDLListWindow

class CDLListWindow : 
	public CDialogImpl<CDLListWindow>,
	public CWinDataExchange<CDLListWindow>,
	public CDialogResize<CDLListWindow>,
	public CMessageFilter,
	public IDropTargetImpl<CDLListWindow>
{
public:
	enum { IDD = IDD_DLLIST };

	//void SetReferer(LPCTSTR strReferer) { m_strReferer = strReferer; }
	void SetDLList(const std::vector<CString>& vecURL);

	// Overrides
	void OnFinalMessage(_In_ HWND /*hWnd*/) { delete this; }
	BOOL PreTranslateMessage(MSG* pMsg);

	DROPEFFECT OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);

	// ダイアログリサイズマップ
    BEGIN_DLGRESIZE_MAP( CDLListWindow )
		DLGRESIZE_CONTROL(IDC_COMBO_DLFOLDER	, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_GETDLFOLDER		, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LIST_DL			, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

	// DDX map
	BEGIN_DDX_MAP( CDLListWindow )
		DDX_CONTROL_HANDLE(IDC_COMBO_DLFOLDER	, m_cmbDLFolder)
		DDX_CONTROL_HANDLE(IDC_COMBO_PARALLELDL	, m_cmbParallelDL)
		DDX_CONTROL_HANDLE(IDC_CMB_DLLISTOPTION	, m_cmbDLOption)
		DDX_CONTROL_HANDLE(IDC_LIST_DL			, m_DLList)
	END_DDX_MAP()

	// Message map
	BEGIN_MSG_MAP( CDLListWindow )
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_CLOSE	 ( OnClose )
		COMMAND_ID_HANDLER_EX( IDC_GETDLFOLDER	, OnGetDLFolder )
		COMMAND_ID_HANDLER_EX( IDC_DLSTART		, OnDLStart	)
		COMMAND_ID_HANDLER_EX( IDCANCEL			, OnDLFinish )
		COMMAND_ID_HANDLER_EX( IDC_DLFINISH		, OnDLFinish )
		COMMAND_HANDLER_EX( IDC_COMBO_PARALLELDL, CBN_SELCHANGE, OnSelChangeParallelDL )
		MESSAGE_HANDLER_EX( WM_DLCOMPLETE, OnDLComplete )
		CHAIN_MSG_MAP( CDialogResize<CDLListWindow> )    
	END_MSG_MAP()

	BOOL	OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void	OnClose();
	void	OnGetDLFolder(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnDLStart(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnDLFinish(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	OnSelChangeParallelDL(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnDLComplete(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void	_DLStart();
	void	_SetTitle();

	// Data members
	CComboBox	m_cmbDLFolder;
	CEdit		m_editDLFolder;
	CComboBox	m_cmbParallelDL;
	CComboBox	m_cmbDLOption;
	CListViewCtrl m_DLList;

	CString		m_strIniFile;
	CString		m_strDLFolder;
	UINT		WM_DLCOMPLETE;
	int			m_nDownloading;
	//CString		m_strReferer;
};





























