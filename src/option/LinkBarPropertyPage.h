/**
 *	@file	LinkBarPropertyPage.h
 *	@brief	リンクバーのオプション設定.
 *	@note
 *		nogui.txt に書かれている Linkバーに関する設定をguiのdonutオプションで設定できるようにしたもの(+mod)
 */
#pragma once

class CMainFrame;


class CLinkBarPropertyPage
		: public CPropertyPageImpl < CLinkBarPropertyPage >
		, public CWinDataExchange < CLinkBarPropertyPage >
{
public:
	enum { IDD = IDD_PROPPAGE_LINKBAR };

private:
	CDonutLinksBarCtrl<CMainFrame>& 	m_LinkBar;
	CString 							m_strLinks;
	int 								m_nIconMode;
	BOOL								m_bInit;

public:
	// DDX map
	BEGIN_DDX_MAP( CLinkBarPropertyPage )
		DDX_RADIO( IDC_LINKBAR_SMALLICON , m_nIconMode		)
		DDX_TEXT_LEN( IDC_LINKBAR_EDIT_FOLDER, m_strLinks, MAX_PATH )
	END_DDX_MAP()


	// Constructor
	CLinkBarPropertyPage(CDonutLinksBarCtrl<CMainFrame>& linkBar)
		: m_LinkBar(linkBar)
	  #if 0
		, m_nLargeIcon(0)
		, m_nNoIcon(0)
	  #endif
	{
		_SetData();
	}


	// Message map and handlers
	BEGIN_MSG_MAP( CExplorerPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_LINKBAR_BUTTON_FOLDER, OnUserFolder )
		CHAIN_MSG_MAP( CPropertyPageImpl<CLinkBarPropertyPage> )
	END_MSG_MAP()


public: // Overrides

	BOOL OnSetActive()
	{
		if (!m_bInit) {
			m_bInit = TRUE;
		}
		SetModified(TRUE);
		return DoDataExchange(FALSE);
	}


	BOOL		OnKillActive()
	{
		return DoDataExchange(TRUE);
	}


	BOOL		OnApply()
	{
		if ( DoDataExchange(TRUE) ) {
			_GetData();
			return TRUE;
		} else
			return FALSE;
	}


	// Implementation
private:

	void	_GetData()
	{
		// update  flags
		DWORD	dwFlags = 0;
		switch (m_nIconMode) {
		case 0: dwFlags = 0				; break;
		case 1: dwFlags = LNK_LARGEICON	; break;
		case 2: dwFlags = LNK_NOICON   	; break;
		default: ATLASSERT(0);
		}
		CIniFileO pr(g_szIniFileName, _T("LinkBar") );
		pr.SetValue(dwFlags,  _T("ExtendedStyle") );
		pr.SetStringUW(m_strLinks, _T("RootPath") );
	}


	void	_SetData()
	{
		CIniFileI pr( g_szIniFileName, _T("LinkBar") );
		DWORD dwFlags = pr.GetValue( _T("ExtendedStyle"), 0 );
		m_nIconMode   = 0;
		if (dwFlags & LNK_NOICON)
			m_nIconMode = 2;
		else if (dwFlags & LNK_LARGEICON)
			m_nIconMode = 1;
		m_strLinks	  = pr.GetStringUW( _T("RootPath") );
	}


	void	OnUserFolder(UINT wNotifyCode, int wID, HWND hWndCtl)
	{
		CString strPath = BrowseForFolder();
		if ( !strPath.IsEmpty() ) {
			m_strLinks = strPath;
			DoDataExchange(FALSE);
		}
	}


	CString BrowseForFolder()
	{
		TCHAR		szDisplayName[MAX_PATH+1];
		ZeroMemory(szDisplayName, sizeof szDisplayName);	//+++

		BROWSEINFO	bi = {
			m_hWnd, 			  NULL, szDisplayName, _T("ユーザー定義のリンクバー・フォルダ"),
			BIF_RETURNONLYFSDIRS, NULL, 0,				0
		};

		CItemIDList idl;

		idl.Attach( ::SHBrowseForFolder(&bi) );
		return idl.GetPath();
	}
};

