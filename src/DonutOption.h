/**
 *	@file	DonutOption.h
 *	@brief	ツリービュー形式でオプションを管理するためのダイアログ
 *			リソーステンプレートに依存した処理があるため汎用性は非常に低くなっています。
 */

#ifndef __PROPERTY_SHEET_H_
#define __PROPERTY_SHEET_H_


////////////////////////////////////////////////////////////////////////////////
// CTreeViewPropertySheet : Donutのオプション

class CTreeViewPropertySheet 
	: public CDialogImpl<CTreeViewPropertySheet>
{
public:
	enum { IDD = IDD_PROPSHEET };

private:
	enum {
		TITLELENGTH		= 200,
		SPACE			= 2,
		TOOLTIPTIMERID	= 1,
		TOOLTIPDELAY	= 250,	// 250ms
	};

	struct PAGEINFO {
		PROPSHEETPAGE	psp;
		TCHAR			title[TITLELENGTH];
		HWND			hWnd;
		int 			nIndent;
	};

	CSimpleArray<PAGEINFO *>		m_aryPage;
	CString 						m_Title;
	CTreeViewCtrl					m_TreeView;
	HWND							m_hActiveWnd;

	int 							m_nWidth;
	int 							m_nHeight;
	bool							m_bInitPos;
	int								m_nCurSel;

	CToolTipCtrl					m_ToolTip;
	CButton							m_btnToolTip;
	CButton							m_btnRestore;

	UINT_PTR						m_ToolTipTimer;
	int								m_MoveCount;
	HWND							m_hWndCtrl;

	static HHOOK					s_hHook;
	static CTreeViewPropertySheet*	s_pThis;

	bool							m_bUseTheme;

public:
	// Constructor
	CTreeViewPropertySheet(LPCTSTR title = NULL);

	BOOL	AddPage(PROPSHEETPAGE *pPage, int nIndent = -1);

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL);

private:

	struct	DLGTEMPLATEEX_HALF {
		WORD	dlgVer;
		WORD	signature;
		DWORD	helpID;
		DWORD	exStyle;
		DWORD	style;
		WORD	cDlgItems;
		short	x;
		short	y;
		short	cx;
		short	cy;
	};	//DLGTEMPLATEEXの前半分

	void	_SeekUnicode(WCHAR **wppstr);
	void	_ReadyDialogResources();
	void	_InitTreeView();
	HWND	_LoadDialog(int nIndex);

	void	_ReleasePages();
	void	_BeChild(HWND hWnd, HWND hParent);
	void	_SetTitle(int index);
	BOOL	_NotifyToDialog(HWND hWnd, int code);
	void	_MoveChild(HWND hWnd);
	void	_SetToolTip(int nIndex);
	void	_ShowToolTip();
	HWND	_GetActivePage();

	BOOL	_ApplyToDialogs();

	void	_SelectItemFromIni(const CString& strLastSelectedItem);

private:
	static LRESULT CALLBACK MsgFilterProc(int code, WPARAM wParam, LPARAM lParam);

public:
	// Message map
	BEGIN_MSG_MAP(CTreeViewPropertySheet)
		MESSAGE_HANDLER( WM_INITDIALOG , OnInitDialog)
		MESSAGE_HANDLER( WM_DESTROY    , OnDestroy	 )
		MESSAGE_HANDLER( WM_SIZE	   , OnSize 	 )
		MSG_WM_SETCURSOR	( OnSetCursor		)
		MSG_WM_TIMER		( OnTimer			)
		MSG_WM_MOUSEACTIVATE( OnMouseActivate	)
		COMMAND_ID_HANDLER( IDOK	   , OnOk			)
		COMMAND_ID_HANDLER( IDCANCEL   , OnCancel		)
		COMMAND_ID_HANDLER( IDC_APPLY  , OnApply		)
		COMMAND_ID_HANDLER( IDC_TOOLTIP, OnCheckToolTip )
		//NOTIFY_HANDLER(IDC_TREEVIEW,TVN_SELCHANGED,OnSelChanged)
		NOTIFY_HANDLER(IDC_TREEVIEW, TVN_SELCHANGING, OnSelChanging)
		//+++ お試し...失敗... CHAIN_MSG_MAP(CDialogImpl<CTreeViewPropertySheet>)
	END_MSG_MAP()


	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	BOOL	OnSetCursor(CWindow wnd, UINT nHitTest, UINT message);
	void	OnTimer(UINT_PTR nIDEvent);
	int		OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message);

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnApply(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnCheckToolTip(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT OnSelChanging(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

};


__declspec(selectany) HHOOK 					CTreeViewPropertySheet::s_hHook		= NULL;
__declspec(selectany) CTreeViewPropertySheet*	CTreeViewPropertySheet::s_pThis		= NULL;


#endif //__PROPERTY_SHEET_H_
