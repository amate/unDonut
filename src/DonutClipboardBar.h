/**
 *	@file	DonutClipbordBar.h
 *	@brief	クリップボード・バー
 */

#pragma once

// for debug
#ifdef _DEBUG
	const bool _Mtl_ClipboardBar_traceOn = false;
	#define clbTRACE	if (_Mtl_ClipboardBar_traceOn)	ATLTRACE
#else
	#define clbTRACE
#endif


#include "resource.h"
#include "Mtl.h"
#include "MtlCtrls.h"
#include "Donut.h"


// Note. When I made dialogs, I've encountered a strange kernel32 exception.
//		 If a form view has a check box, kernel32 exception occurs when an app unloaded.
//		 I can't get the reason.


class CDonutClipboardBar
	: public CDialogImpl<CDonutClipboardBar>
	, public CDialogResize<CDonutClipboardBar>
	, public CClipboardViewer<CDonutClipboardBar>
	, public CWinDataExchange<CDonutClipboardBar>
{
public:
	enum { IDD = IDD_CLIPBAR_FORM };

private:
	enum EClpv_Ex {
		CLPV_EX_ON		=	0x00000001L,
		CLPV_EX_DIRECT	=	0x00000002L,
		CLPV_EX_FLUSH	=	0x00000004L,
	};

	// Data members
	CString 					m_strExts;
	DWORD						m_dwExStyle;
	int 						m_nOn;
	int 						m_nDirect;
	int 						m_nFlush;

	// Windows
	CEdit						m_edit;
	CContainedWindowT<CListBox> m_box;

public:
	// Ctor
	CDonutClipboardBar();

	// Overrides
	void	OnUpdateClipboard();

	// Methods

	void	OpenClipboardUrl();
	BYTE	PreTranslateMessage(MSG *pMsg);

public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutClipboardBar)
		MESSAGE_HANDLER(WM_INITDIALOG				, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY					, OnDestroy)

		COMMAND_ID_HANDLER(IDC_CHECK_CB_ON			, OnCheckCommand)
		COMMAND_ID_HANDLER(IDC_CHECK_CB_DIRECT		, OnCheckCommand)
		COMMAND_ID_HANDLER(IDC_CHECK_CB_FLUSH		, OnCheckCommand)

		COMMAND_ID_HANDLER(IDC_BUTTON_OPEN			, OnButtonOpen)
		COMMAND_ID_HANDLER(IDC_BUTTON_CB_PASTE		, OnButtonPaste)
		COMMAND_ID_HANDLER(IDC_BUTTON_DELETE		, OnButtonDelete)
		COMMAND_ID_HANDLER(IDC_BUTTON_DELETEALL 	, OnButtonDeleteAll)
		COMMAND_ID_HANDLER(IDC_BUTTON_CB_PASTE_DONUT, OnButtonPasteDonut)
		COMMAND_ID_HANDLER(IDOK 					, OnIdOk)

		CHAIN_MSG_MAP( CClipboardViewer<CDonutClipboardBar> )
		CHAIN_MSG_MAP( CDialogResize<CDonutClipboardBar>	)
	ALT_MSG_MAP(1) // list box
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK			, OnLBLButtonDblClk)
	END_MSG_MAP()

public:
	// Dlg resize map
	BEGIN_DLGRESIZE_MAP( CDonutClipboardBar )
		DLGRESIZE_CONTROL( IDC_CHECK_CB_ON, 	DLSZ_SIZE_X )
		DLGRESIZE_CONTROL( IDC_LIST_CB_CONT,	DLSZ_SIZE_X | DLSZ_SIZE_Y )
		DLGRESIZE_CONTROL( IDC_BUTTON_OPEN, 	DLSZ_MOVE_Y | DLSZ_SIZE_X )

		BEGIN_DLGRESIZE_GROUP()
			DLGRESIZE_CONTROL( IDC_BUTTON_CB_PASTE, 	DLSZ_SIZE_X )
			DLGRESIZE_CONTROL( IDC_BUTTON_DELETE,		DLSZ_SIZE_X )
			DLGRESIZE_CONTROL( IDC_BUTTON_DELETEALL,	DLSZ_SIZE_X )
		END_DLGRESIZE_GROUP()

		DLGRESIZE_CONTROL( IDC_BUTTON_CB_PASTE_DONUT,	DLSZ_SIZE_X | DLSZ_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_CHECK_CB_DIRECT, 		DLSZ_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_CHECK_CB_FLUSH,			DLSZ_MOVE_Y )

		DLGRESIZE_CONTROL( IDC_STATIC_CB_EXT,	DLSZ_MOVE_Y )
		DLGRESIZE_CONTROL( IDC_EDIT_CB_EXT, 	DLSZ_MOVE_Y | DLSZ_SIZE_X )
	END_DLGRESIZE_MAP()

	// DDX map
	BEGIN_DDX_MAP( CDonutClipboardBar )
		DDX_TEXT ( IDC_EDIT_CB_EXT, 	m_strExts	)
		DDX_CHECK( IDC_CHECK_CB_ON, 	m_nOn		)
		DDX_CHECK( IDC_CHECK_CB_DIRECT, m_nDirect	)
		DDX_CHECK( IDC_CHECK_CB_FLUSH,	m_nFlush	)
	END_DDX_MAP()


private:
	LRESULT 	OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT 	OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT 	OnCheckCommand(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnButtonDelete(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnButtonOpen(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnButtonPaste(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnButtonPasteDonut(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnButtonDeleteAll(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnIdOk(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT 	OnLBLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/);

	CString 	GetExtsList();

	// Implementation
	void	_GetData();
	void	_SetData();
	void	_GetProfile();
	void	_WriteProfile();

	void	_OnItemOpen(int nIndex);


	template <class _Function>
	bool	_MtlForEachListBoxSelectedItem(CListBox box, _Function __f);


	struct _Function_DeleteString {
		void operator ()(CListBox box, int nIndex, int nCount)
		{
			box.DeleteString(nIndex);
		}
	};


	struct _Function_Open {
		void operator ()(CListBox box, int nIndex, int nCount)
		{
			CString str;

			MtlListBoxGetText(box.m_hWnd, nIndex, str);

			if ( str.IsEmpty() )
				return;

			DonutOpenFile(str, 0);
		}
	};

};



template <class _Function>
bool CDonutClipboardBar::_MtlForEachListBoxSelectedItem(CListBox box, _Function __f)
{
	int 	nCount	= box.GetSelCount();
	if (nCount == 0)
		return false;

	LPINT	rgIndex = (LPINT) _alloca(sizeof (INT) * nCount);

	nCount			= box.GetSelItems(nCount, rgIndex);
	if (nCount == LB_ERR)
		return false;

	CSimpleArray<int>	arrIndex;

	for (int i = 0; i < nCount; ++i) {
		arrIndex.Add(rgIndex[i]);
	}

	std::sort( _begin(arrIndex), _end(arrIndex) );

	for (int j = arrIndex.GetSize() - 1; j >= 0; --j)  // from the tail, you can delete it
		__f(box.m_hWnd, arrIndex[j], nCount);

	return true;
}

