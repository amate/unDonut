/**
 *	@file	MDIChildUserMessanger.h
 */

#pragma once

// IWebBrowserEvents2Impl - Windows' MDI messages are not enough
//
/////////////////////////////////////////////////////////////////////////////

DECLARE_REGISTERED_MESSAGE(Mtl_MDIChild_User)



enum EMdiChild_User {
	MDICHILD_USER_CREATED		= 0x00000001,
	MDICHILD_USER_CLOSING		= 0x00000002,
	MDICHILD_USER_DESTROYED 	= 0x00000004,
	MDICHILD_USER_ACTIVATED 	= 0x00000008,
	MDICHILD_USER_DEACTIVATED	= 0x00000010,
	MDICHILD_USER_VISUALIZED	= 0x00000020,
	MDICHILD_USER_TITLECHANGED	= 0x00000040,
	MDICHILD_USER_ALLCLOSED 	= 0x00000080,
	MDICHILD_USER_FIRSTCREATED	= 0x00000100,
};



#define MSG_WM_USER_MDICHILD(func)							   \
	if ( uMsg == GET_REGISTERED_MESSAGE(Mtl_MDIChild_User) ) { \
		SetMsgHandled(TRUE);								   \
		func( (HWND) wParam, (UINT) lParam );				   \
		lResult = 0;										   \
		if ( IsMsgHandled() )								   \
			return TRUE;									   \
	}



template <class T>
class CMDIChildUserMessenger {
public:
	// Constructor
	CMDIChildUserMessenger()
		: m_dwMDIChildUserMessengerStyle(MDICUMS_ONMDIACTIVATE | MDICHILD_USER_ACTIVATED)
	{
	}


	// Attributes
	void ModifyMDIChildUserMessengerStyle(DWORD dwRemove, DWORD dwAdd)
	{
		m_dwMDIChildUserMessengerStyle = (m_dwMDIChildUserMessengerStyle & ~dwRemove) | dwAdd;
	}


	// Message map and handlers
	BEGIN_MSG_MAP(CMDIChildUserMessenger)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SETTEXT(OnSetText)
		MSG_WM_MDIACTIVATE(OnMDIActivate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()


private:
	// MDI child user messenger style flags
	enum EMdiCUMS {
		MDICUMS_ONCREATE		=	0x00000001,
		MDICUMS_ONSETTEXT		=	0x00000002,
		MDICUMS_ONMDIACTIVATE	=	0x00000004,
		MDICUMS_ONCLOSE 		=	0x00000008,
		MDICUMS_ONDESTROY		=	0x00000010,
	};

	DWORD	m_dwMDIChildUserMessengerStyle;


private:
	LRESULT OnCreate(LPCREATESTRUCT)
	{
		SetMsgHandled(FALSE);

		if (m_dwMDIChildUserMessengerStyle & MDICUMS_ONCREATE) {
			T *pT = static_cast<T *>(this);
			::SendMessage(pT->GetMDIFrame(), GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
						 (WPARAM) pT->m_hWnd, (LPARAM) MDICHILD_USER_CREATED);
		}

		return 0;
	}


	LRESULT OnSetText(LPCTSTR lpszText)
	{
		SetMsgHandled(FALSE);

		if (m_dwMDIChildUserMessengerStyle & MDICUMS_ONSETTEXT) {
			T *pT = static_cast<T *>(this);
			::SendMessage(pT->GetMDIFrame(), GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
						  (WPARAM) pT->m_hWnd, (LPARAM) MDICHILD_USER_TITLECHANGED);
		}

		return FALSE;
	}


	void OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct)
	{
		SetMsgHandled(FALSE);

		if ( !(m_dwMDIChildUserMessengerStyle & MDICUMS_ONMDIACTIVATE) )
			return;

		T *pT = static_cast<T *>(this);

		if (hwndChildAct == NULL) { 		// all the child windows closed
			ATLTRACE2( atlTraceGeneral, 4, _T("CMDIChildUserMessenger(%d, %d, %d)\n"),
					  GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					  ::RegisterWindowMessage( _T("Mtl_MDIChild_User") ),
					  ::RegisterWindowMessage( _T("Mtl_Update_CmdUI_Message") ) );

			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) NULL,
					(LPARAM) MDICHILD_USER_ALLCLOSED);
			return;
		}

		if (hwndChildDeact == NULL) {		// I'm the first child
			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) pT->m_hWnd,
					(LPARAM) MDICHILD_USER_FIRSTCREATED);
		}

		if (hwndChildAct == pT->m_hWnd) {	// I'm activated
			ATLASSERT( hwndChildAct == pT->MDIGetActive() );
			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) pT->m_hWnd,
					(LPARAM) MDICHILD_USER_ACTIVATED);
		}

		if (hwndChildDeact == pT->m_hWnd) { // I'm deactivated
			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) pT->m_hWnd,
					(LPARAM) MDICHILD_USER_DEACTIVATED);
		}
	}


	void OnClose()
	{
		SetMsgHandled(FALSE);

		if (m_dwMDIChildUserMessengerStyle & MDICUMS_ONCLOSE) {
			T * 	pT = static_cast<T *>(this);
			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) pT->m_hWnd,
					(LPARAM) MDICHILD_USER_CLOSING );
		}
	}


	void OnDestroy()
	{
		SetMsgHandled(FALSE);

		if (m_dwMDIChildUserMessengerStyle & MDICUMS_ONDESTROY) {
			T * 	pT = static_cast<T *>(this);
			::SendMessage(
					pT->GetMDIFrame(),
					GET_REGISTERED_MESSAGE(Mtl_MDIChild_User),
					(WPARAM) pT->m_hWnd,
					(LPARAM) MDICHILD_USER_DESTROYED );
		}
	}
};

