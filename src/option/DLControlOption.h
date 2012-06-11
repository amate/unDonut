/**
 *	@file	DLControlOption.h
 *	brief	donutのオプション : "ブラウザ"
 */

#pragma once

#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDLControlOption

class CDLControlOption {
	friend class CDLControlPropertyPage;
public:
	// DL Control Flags
	enum { DLCTL_DEFAULT	= (DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS) };
	enum { GPURENDER_NONE, GPURENDER_CASH, GPURENDER_ALWAYS };

	static DWORD	s_dwDLControlFlags; 			// default flags		//+++ ref by MainFrame.cpp
	static DWORD	s_dwExtendedStyleFlags; 								//+++ ref by DonutView.cpp
	static bool		s_bUseDLManager;
	static int		s_nGPURenderStyle;

	static void		SetUserAgent();

	static TCHAR	s_szUserAgent[MAX_PATH];			// UDT DGSTR			//+++ ref by MainFrame.cpp
	static TCHAR	s_szUserAgent_cur[MAX_PATH];		//+++

public:
	static void 	GetProfile();
	static void 	WriteProfile();
};


/////////////////////////////////////////////////////////////////////////////
// CDLControlPropertyPage


class CDLControlPropertyPage
	: public CPropertyPageImpl<CDLControlPropertyPage>
	, public CWinDataExchange<CDLControlPropertyPage>
	, protected CDLControlOption
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_DLCONTROL };

private:
	// Data members
	int 		m_nBGSounds;
	int 		m_nVideos;
	int 		m_nDLImages;
	int 		m_nRunActiveXCtls;
	int 		m_nDLActiveXCtls;
	int 		m_nScripts;
	int 		m_nJava;
	int 		m_nNaviLock;
	int 		m_nNoCloseNL;			//+++
	int 		m_nFlatView;			// UDT DGSTR ( dai
	int			m_bUserAgent;			//+++
	CString 	m_strUserAgent; 		// UDT DGSTR
	CEdit		m_edit; 				// UDT DGSTR

	// UH
	HWND		m_hMainWnd;
	BOOL		m_bMsgFilter;
	BOOL		m_bMouseGesture;
	BOOL		m_bBlockMailto;
	BOOL		m_bUseDLManager;

	bool		m_bInit;
public:

	// DDX map
	BEGIN_DDX_MAP( CDLControlPropertyPage )
		DDX_CHECK( IDC_CHECK_DLCTL_BGSOUNDS 	, m_nBGSounds		)
		DDX_CHECK( IDC_CHECK_DLCTL_VIDEOS		, m_nVideos 		)
		DDX_CHECK( IDC_CHECK_DLCTL_DLIMAGES 	, m_nDLImages		)
		DDX_CHECK( IDC_CHECK_DLCTL_RUNACTIVEXCTLS, m_nRunActiveXCtls)
		DDX_CHECK( IDC_CHECK_DLCTL_DLACTIVEXCTLS, m_nDLActiveXCtls	)
		DDX_CHECK( IDC_CHECK_DLCTL_SCRIPTS		, m_nScripts		)
		DDX_CHECK( IDC_CHECK_DLCTL_JAVA 		, m_nJava			)
		DDX_CHECK( IDC_CHECK_NAVILOCK			, m_nNaviLock		)
		DDX_CHECK( IDC_CHECK_NOCLOSE_NAVILOCK	, m_nNoCloseNL		)	//+++
		DDX_CHECK( IDC_CHECK_SCROLLBAR			, m_nFlatView		)	// UDT DGSTR ( dai

		// UH
		DDX_CHECK( IDC_CHK_MSG_FILTER			, m_bMsgFilter		)
		DDX_CHECK( IDC_CHK_MOUSE_GESTURE		, m_bMouseGesture	)
		DDX_CHECK( IDC_CHK_BLOCK_MAILTO 		, m_bBlockMailto	)
		DDX_CHECK( IDC_CHK_USER_AGENT	 		, m_bUserAgent		)	//+++
		DDX_CHECK( IDC_CHK_DLMANAGER			, m_bUseDLManager	)
		DDX_TEXT_LEN( IDC_EDIT_USER_AGENT, m_strUserAgent, MAX_PATH )	// UDT DGSTR
		DDX_RADIO( IDC_RADIO_NONE, s_nGPURenderStyle )
	END_DDX_MAP()

	// Constructor
	CDLControlPropertyPage( HWND  hMainWnd );

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map and handlers
	BEGIN_MSG_MAP( CDLControlPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CDLControlPropertyPage> )
	END_MSG_MAP()

	// Implementation
private:
	void	_GetData();
	void	_SetData();
};

