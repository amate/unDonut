/**
*	@file	ChildFrame.inl
*	@brief	CChildFrame::Impl �̒�`
*/

#pragma once


CChildFrame::Impl::Impl(CChildFrame* pChild) : 
	m_pParentChild(pChild), 
	m_pThreadRefCount(nullptr), 
	m_view(m_UIChange),
	m_bNowActive(false),
	m_bSaveSearchWordflg(false),
	m_nPainBookmark(0),
	m_bNowHilight(false),
	m_bAutoHilight(false),
	m_bExecutedNewWindow(false),
	m_bCancelRButtonUp(false),
	m_dwMarshalDLCtrlFlags(0),
	m_bInitTravelLog(false),
	m_nImgScl(100),
	m_nImgSclSav(100),
	m_nImgSclSw(0),
	m_bImagePage(false),
	m_bReload(false),
	m_bNowNavigate(false),
	m_bClosing(false),
	m_pPageBitmap(nullptr)
{	}


void	CChildFrame::Impl::SetExStyle(DWORD dwStyle)
{
	m_view.SetExStyle(dwStyle);

	bool bNavigateLock = (dwStyle & DVS_EX_OPENNEWWIN) != 0;
	//m_MDITab.NavigateLockTab(m_hWnd, bNavigateLock);
	//m_bExPropLock					= (bNavigateLock && bExProp) /*? true : false*/;

}


void 	CChildFrame::Impl::SetSearchWordAutoHilight(const CString& str, bool bAutoHilight)
{
	m_strSearchWord = str;
	m_bAutoHilight	= bAutoHilight;
}

/// �I��͈͂̕�������擾(�t���[���Ή���)
CString CChildFrame::Impl::GetSelectedText()
{
	CString 	strSelText;
	MtlForEachHTMLDocument2( m_spBrowser, [&strSelText](IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>		spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( SUCCEEDED( hr ) ) {
			CComPtr<IDispatch>				spDisp;
			hr	   = spSelection->createRange(&spDisp);
			if ( SUCCEEDED( hr ) ) {
				CComQIPtr<IHTMLTxtRange>	spTxtRange = spDisp;
				if (spTxtRange != NULL) {
					CComBSTR				bstrText;
					hr	   = spTxtRange->get_text(&bstrText);
					if (SUCCEEDED(hr) && !!bstrText)
						strSelText = bstrText;
				}
			}
		}
	});
#if 0	//:::
	if (strSelText.IsEmpty()) {
		//+++ �I��͈͂��Ȃ��ꍇ�A�E�N���b�N���j���[�o�R�̂��Ƃ�����̂ŁA����΍��������...
		CCustomContextMenu* pMenu = CCustomContextMenu::GetInstance();
		if (pMenu && pMenu->GetContextMenuMode() == CONTEXT_MENU_ANCHOR && pMenu->GetAnchorUrl().IsEmpty() == 0) {
			//bstrText		= pMenu->GetAnchorUrl();
			//bstrLocationUrl = pMenu->GetAnchorUrl();
			strSelText = CString(pMenu->GetAnchorUrl());
		}
	}
#endif
	return strSelText;
}



//+++ _OpenSelectedText()��蕪��.
CString CChildFrame::Impl::GetSelectedTextLine()
{
	CString str = GetSelectedText();
	if (str.IsEmpty() == 0) {
		int 	n	= str.Find(_T("\r"));
		int 	n2	= str.Find(_T("\n"));
		if ((n < 0 || n > n2) && n2 >= 0)
			n = n2;
		if (n >= 0) {
			str = str.Left(n);
		}
	}
	return str;
}


// Event handlers
void	CChildFrame::Impl::OnBeforeNavigate2(IDispatch*		pDisp,
										const CString&		strURL,
										DWORD				nFlags,
										const CString&		strTargetFrameName,
										CSimpleArray<BYTE>&	baPostedData,
										const CString&		strHeaders,
										bool&				bCancel )
{
	bool bTopWindow = IsPageIWebBrowser(pDisp);

	{
		//�v���O�C���C�x���g - �i�r�Q�[�g�O
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		int 	nRet		 = CPluginManager::ChainCast_PluginEvent(DEVT_TAB_BEFORENAVIGATE, stn.nIndex, (SPARAM) &stn);
		if (nRet == -1) {
			//�i�r�Q�[�g�L�����Z��
			bCancel = true;
			return;
		} else if (nRet == -2) {
			//�N���[�Y
			bCancel 	  = true;
			//m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.
			return;
		}
	}

	// mailto: ����
	if (m_view.GetExStyle() & DVS_EX_BLOCK_MAILTO) {
		if (strURL.Left(7).CompareNoCase( _T("mailto:") ) == 0) {
			bCancel = true;
			return;
		}
	}

	// Navigate������javescript����n�܂�E�B���h�E�̓i�r�Q�[�g���Ȃ�
	if (m_bNowNavigate && 
		strURL.Left(15).CompareNoCase(_T("javascript:void")) == 0 || strURL.CompareNoCase(_T("javascript:;")) == 0) 
	{
		bCancel = true;
		return;
	}

	if (bTopWindow) {
		if (m_dwMarshalDLCtrlFlags) {
			m_view.PutDLControlFlags(m_dwMarshalDLCtrlFlags);
			m_dwMarshalDLCtrlFlags = 0;
		}

		if (m_view.m_bLightRefresh) {
			m_view.m_bLightRefresh = false;	// �蓮�ŃZ�L�����e�B��ݒ肵���̂ŉ������Ȃ�
		} else	{	
			// URL�ʃZ�L�����e�B�̐ݒ�
			DWORD exopts	= 0xFFFFFFFF;
			DWORD dlCtlFlg	= 0xFFFFFFFF;
			DWORD exstyle	= 0xFFFFFFFF;
			DWORD autoRefresh = 0xFFFFFFFF;
			DWORD dwExPropOpt = 8;
			if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
				m_view.PutDLControlFlags(CDLControlOption::s_dwDLControlFlags);
				SetExStyle(CDLControlOption::s_dwExtendedStyleFlags);
			}
			if (CUrlSecurityOption::FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
				CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
				dlCtlFlg	= ExProp.GetDLControlFlags();
				exstyle		= ExProp.GetExtendedStyleFlags();
				autoRefresh = ExProp.GetAutoRefreshFlag();
			}

			//+++ url�ʊg���v���p�e�B�̏���....
			//+++	�߂�E�i�ނł̊g���v���p�e�B���̕ł��Ƃ̕ۑ����ł��Ă��Ȃ��̂ŁA�j�]����...
			//+++	���A�o�O���Ă����f����邱�Ƃ̂ق����Ӗ����肻���Ȃ̂ŗ��p
			if (CUrlSecurityOption::s_bValid) {
				if (exopts != 0xFFFFFFFF && CUrlSecurityOption::activePageToo()) {
					m_view.PutDLControlFlags( dlCtlFlg );
					m_view.SetAutoRefreshStyle( autoRefresh );
					SetExStyle( exstyle );	//+++����: �}�E�X���{�^���N���b�N�ł́A�����N�ʃ^�u�\���̏ꍇ�A�܂��^�u�ʒu������̂��ߐݒ�ł��Ȃ�...
				}
			}
		}

		/* Favicon�𔒎��ɐݒ� */
		//if (strURL.Left(11).CompareNoCase(_T("javascript:")) != 0) {
			//m_UIChange.SetLocationURL(strURL);
		//	_SetFavicon(strURL);
		//}

	}
	
	m_bNowNavigate = true;	// Navigate���ł���

	{
		//�C�x���g���� - �i�r�Q�[�g
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_NAVIGATE, stn.nIndex, (SPARAM) &stn);
	}
}

void	CChildFrame::Impl::OnDownloadComplete()
{
	m_bNowNavigate = false;	// Navigate�I��
}

void	CChildFrame::Impl::OnTitleChange(const CString& strTitle)
{
	SetWindowText(strTitle);
	m_UIChange.SetTitle(strTitle);

	m_UIChange.SetLocationURL(GetLocationURL());
}

void	CChildFrame::Impl::OnProgressChange(long progress, long progressMax)
{
	m_UIChange.SetProgress(progress, progressMax);
}

void	CChildFrame::Impl::OnStatusTextChange(const CString& strText)
{
	m_strStatusText = strText;
	m_UIChange.SetStatusText(strText);
}

void	CChildFrame::Impl::OnSetSecureLockIcon(long nSecureLockIcon)
{
	m_UIChange.SetSecureLockIcon((int)nSecureLockIcon);
}

void	CChildFrame::Impl::OnPrivacyImpactedStateChange(bool bPrivacyImpacted)
{
	m_UIChange.SetPrivacyImpacted(bPrivacyImpacted);
}

void	CChildFrame::Impl::OnStateConnecting()
{
	READYSTATE	state;
	HRESULT hr = m_spBrowser->get_ReadyState(&state);
	if (hr == S_OK && state == READYSTATE_COMPLETE)
		return;

	//m_MDITab.SetConnecting(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMECONNECTING, (WPARAM)m_hWnd);
	//m_bPrivacyImpacted = TRUE;	// �y�[�W�ǂݍ��݌��OnStateConnecting���Ă΂��y�[�W������̂�
								// �N�b�L�[�����A�C�R�����\������Ȃ��y�[�W������
	//m_nSecureLockIcon = secureLockIconUnsecure;
}

void	CChildFrame::Impl::OnStateDownloading()
{
	READYSTATE	state;
	HRESULT hr = m_spBrowser->get_ReadyState(&state);
	if (hr == S_OK && state == READYSTATE_COMPLETE)
		return;

	//m_MDITab.SetDownloading(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMEDOWNLOADING, (WPARAM)m_hWnd);
}

void	CChildFrame::Impl::OnStateCompleted()
{
	//m_MDITab.SetComplete(m_hWnd);
	GetTopLevelWindow().PostMessage(WM_CHILDFRAMECOMPLETE, (WPARAM)m_hWnd);

	if (m_bReload) {
		CString strUrl = GetLocationURL();
		CComQIPtr<IDispatch>	spDisp = m_spBrowser;
		OnDocumentComplete(spDisp, strUrl);
		m_bReload = false;
	}
}

/// document������ł���悤�ɂȂ���
void	CChildFrame::Impl::OnDocumentComplete(IDispatch *pDisp, const CString& strURL)
{
	if ( IsPageIWebBrowser(pDisp) ) {
		// �������T�C�Y�̐ݒ��������
		m_bImagePage	= false;
		m_nImgSclSw		= (CMainOption::s_nAutoImageResizeType == AUTO_IMAGE_RESIZE_FIRSTON);
		m_ImageSize.SetSize(0, 0);

		/* �y�[�W�������̏������Z�b�g */
		m_nPainBookmark = 0;
		m_strBookmark	= LPCOLESTR(NULL);

		_InitTravelLog();	// �g���x�����O��ݒ�

		_AutoImageResize(true);	// �摜�̎������T�C�Y

		_SetFavicon(strURL);

		/* ���[�U�[��`Javascript */
		if (auto value = CUserDefinedJsOption::FindURL(strURL)) {
			FILE* fp = nullptr;
			if (_wfopen_s(&fp, value.get(), L"r, ccs=UTF-8") != 0)
				goto ADDJSFINISH;
			std::wstringstream	strstream;
			enum { kBuffSize = 512 };
			wchar_t	temp[kBuffSize + 1];
			while (!feof(fp)) {
				size_t n = fread(temp, sizeof(wchar_t), kBuffSize, fp);
				temp[n] = L'\0';
				strstream << temp;
			}
			fclose(fp);

			CComPtr<IDispatch>	spDisp;
			m_spBrowser->get_Document(&spDisp);
			CComQIPtr<IHTMLDocument2> spDoc = spDisp;
			if (spDoc) {
				CComPtr<IHTMLElement>	spScriptElm;
				spDoc->createElement(CComBSTR(L"script"), &spScriptElm);
				if (spScriptElm == nullptr)
					goto ADDJSFINISH;
 
				CComQIPtr<IHTMLScriptElement>	spScript = spScriptElm;
				spScript->put_type(CComBSTR(L"text/javascript"));
				spScript->put_text(CComBSTR(strstream.str().c_str()));

				CComPtr<IHTMLElement>	spBodyElm;
				spDoc->get_body(&spBodyElm);
				CComQIPtr<IHTMLDOMNode>	spBodyNode = spBodyElm;
				if (spBodyNode == nullptr)
					goto ADDJSFINISH;
				CComQIPtr<IHTMLDOMNode>	spScriptNode = spScript;
				CComPtr<IHTMLDOMNode>	sptempNode;
				spBodyNode->appendChild(spScriptNode, &sptempNode);
			}
		}
		ADDJSFINISH:
		/* ���[�U�[��`CSS */
		if (auto value = CUserDefinedCSSOption::FindURL(strURL)) {
			CComBSTR strCssPath = value.get();
			MtlForEachHTMLDocument2g(m_spBrowser, [strCssPath](IHTMLDocument2* pDoc) {
				CComPtr<IHTMLStyleSheet>	spStyleSheet;
				pDoc->createStyleSheet(strCssPath, -1, &spStyleSheet);
			});
		}

		bool bHilight = m_bAutoHilight || CDonutSearchBar::GetInstance()->GetHilightSw();
		if (bHilight && m_strSearchWord.IsEmpty() == FALSE) {
			if (m_bNowActive)
				GetTopLevelWindow().SendMessage(WM_SETSEARCHTEXT, (WPARAM)(LPCTSTR)m_strSearchWord, true);
			CString strWords = m_strSearchWord;
			DeleteMinimumLengthWord(strWords);
			m_bNowHilight = true;
			_HilightOnce(pDisp, strWords);
		}
	}

	{
		//�v���O�C���C�x���g - ���[�h����
		CComQIPtr<IWebBrowser2> 	pWB2 = pDisp;

		if (pWB2) {
			DEVTS_TAB_DOCUMENTCOMPLETE	dc;
			dc.lpstrURL   = (LPCTSTR) strURL;

			CComBSTR	bstrTitle;
			pWB2->get_LocationName(&bstrTitle);

			CString 	strTitle = bstrTitle;
			dc.lpstrTitle = (LPCTSTR) strTitle;
			dc.nIndex	  = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
			dc.bMainDoc   = IsPageIWebBrowser(pDisp);
			dc.pDispBrowser	= pDisp;
			CPluginManager::BroadCast_PluginEvent(DEVT_TAB_DOCUMENTCOMPLETE, dc.nIndex, (DWORD_PTR) &dc);
		}
	}
}

/// �u���E�U�̃R�}���h�̏�Ԃ��ω�����(�߂�A�i�ނȂǂ�)
void	CChildFrame::Impl::OnCommandStateChange(long Command, bool bEnable)
{
	if (Command == CSC_NAVIGATEBACK)
		m_UIChange.SetNavigateBack(bEnable);
	else if (Command == CSC_NAVIGATEFORWARD)
		m_UIChange.SetNavigateForward(bEnable);
}

void	CChildFrame::Impl::OnNewWindow2(IDispatch **ppDisp, bool& bCancel)
{
#if 1
	m_bExecutedNewWindow = true;

	CChildFrame*	pChild = new CChildFrame;
	pChild->pImpl->SetThreadRefCount(m_pThreadRefCount);
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
		dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	}
	pChild->pImpl->m_view.SetDefaultFlags(dwDLCtrl, dwExStyle, 0);
	HWND hWnd = pChild->CreateEx(GetParent());
	ATLASSERT( ::IsWindow(hWnd) );

	pChild->pImpl->m_spBrowser->get_Application(ppDisp);
	ATLASSERT( ppDisp && *ppDisp );

	pChild->pImpl->SetSearchWordAutoHilight(m_strSearchWord, m_bNowHilight);

	GetTopLevelWindow().PostMessage(WM_TABCREATE, (WPARAM)pChild->pImpl->m_hWnd, TAB_LINK);
#endif
}

void	CChildFrame::Impl::OnNewWindow3(IDispatch **ppDisp, bool& bCancel, DWORD dwFlags, BSTR bstrUrlContext,  BSTR bstrUrl)
{
#if 0
	bCancel	= true;
	m_bExecutedNewWindow = true;

	NewChildFrameData	data(GetParent());
	data.strURL	= bstrUrl;
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
		dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	}
	data.dwDLCtrl	= dwDLCtrl;
	data.dwExStyle	= dwExStyle;
	data.bLink	= true;
	CString strSearchWord = m_strSearchWord;
	bool	bNowHilight	= m_bNowHilight;
	data.funcCallAfterCreated	= [strSearchWord, bNowHilight](CChildFrame* pChild) {
		pChild->SetSearchWordAutoHilight(strSearchWord, bNowHilight);
	};
	CChildFrame::AsyncCreate(data);
#endif
}

void	CChildFrame::Impl::OnWindowClosing(bool IsChildWindow, bool& bCancel)
{
	PostMessage(WM_CLOSE);
}


BOOL	CChildFrame::Impl::OnMButtonHook(MSG* pMsg)
{
	CIniFileI	 pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD	dwLinkOpenBtnM = pr.GetValue(_T("LinkOpenBtnM"), 0);
	if (dwLinkOpenBtnM == 0)	// �{�^�����ݒ肳��Ă��Ȃ�������A�I���.
		return FALSE;

	//+++ �����F�f�t�H���g�ł� dwLinkOpenBtmM = IDM_FOLLOWLINKN
	// ATLASSERT(dwLinkOpenBtnM == IDM_FOLLOWLINKN);

	//+++ �J�[�\���������N�������Ă�����statusBar�p�̃��b�Z�[�W�����邱�Ƃ𗘗p.
	bool	bLink = false;
	if (   m_strStatusText.IsEmpty() == FALSE
		&& m_strStatusText != _T("�y�[�W���\������܂���") 
		&& m_strStatusText != _T("����") )			//+++ �����N�̂Ȃ��y�[�W�ł�"�y�[�W���\������܂���"�Ƃ������b�Z�[�W���ݒ肳��Ă���̂ŏ��O.
		bLink = true;

	::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 				//+++ ���܂��Ȃ�.
	//int 	nTabCnt = m_MDITab.GetItemCount();									//+++ �����N���N���b�N�������ǂ����̃`�F�b�N�p.
	//pChild->m_bAllowNewWindow = true;
	m_bExecutedNewWindow = false;
	::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);					//+++ �����N��V�������ɂЂ炭
	if (m_bExecutedNewWindow)
		return TRUE;
	//pChild->m_bAllowNewWindow = false;
	//int 	nTabCnt2 = m_MDITab.GetItemCount();
	//if (nTabCnt != nTabCnt2 || bLink)											//+++ �����N���b�Z�[�W�����邩�A�^�u�������Ă�����A�����N���N���b�N�����Ƃ���.
	///	return TRUE;															//+++ true��Ԃ��Ē��{�^���N���b�N�̏�������������Ƃɂ���.

	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ �����N�ȊO���N���b�N�����ꍇ���܂��Ȃ��̍��N���b�N�������I�����Ă���.
	return FALSE;																//+++ false��Ԃ����ƂŁAIE�R���|�[�l���g�ɃE�B�[���N���b�N�̏�����C����.
}

static DWORD GetMouseButtonCommand(const MSG& msg)
{
	CString 	strKey;
	switch (msg.message) {
	case WM_LBUTTONUP:	strKey = _T("LButtonUp");					break;
	case WM_MBUTTONUP:	strKey = _T("MButtonUp");					break;
	case WM_XBUTTONUP:	strKey.Format(_T("XButtonUp%d"), GET_XBUTTON_WPARAM(msg.wParam)); break;
	case WM_MOUSEWHEEL:
		short zDelta = (short)HIWORD(msg.wParam);
		if (zDelta > 0)
			strKey = _T("WHEEL_UP");
		else
			strKey = _T("WHEEL_DOWN");
		break;
	}

	CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	return pr.GetValue(strKey, 0);;
}

static int PointDistance(const CPoint& pt1, const CPoint& pt2)
{
	return (int)sqrt( pow(float (pt1.x - pt2.x), 2.0f) + pow(float (pt1.y - pt2.y), 2.0f) );
}

BOOL CChildFrame::Impl::OnRButtonHook(MSG* pMsg)
{
	if ( !(m_view.GetExStyle() & DVS_EX_MOUSE_GESTURE) )
		return FALSE;

	SetCapture();

	CPoint	ptDown(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
	::ClientToScreen(pMsg->hwnd, &ptDown);
	CPoint	ptLast = ptDown;

	bool bCursorOnSelectedText = CMouseOption::s_bUseRightDragSearch && _CursorOnSelectedText();
	HMODULE	hModule	= ::LoadLibrary(_T("ole32.dll"));
	CCursor cursor	= ::LoadCursor(hModule, MAKEINTRESOURCE(3));
	CString	strSearchEngine;
	CString strLastMark;
	CString strMove;
	DWORD	dwTime = 0;
	int		nDistance = 0;
	bool	bNoting = true;	// �������Ȃ�����
	MSG msg = { 0 };
	do {
		BOOL nRet = GetMessage(&msg, NULL, 0, 0);
		if (nRet == 0 || nRet == -1 || GetCapture() != m_hWnd)
			break;

		DWORD dwCommand = 0;
		switch (msg.message) {
		case WM_MOUSEWHEEL:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			dwCommand = GetMouseButtonCommand(msg);
			break;

		case WM_MOUSEMOVE: {
			if (bNoting == false)	// ���̃R�}���h�����s�ς�
				break;
			CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			::ClientToScreen(msg.hwnd, &ptNow);
			if (bCursorOnSelectedText) {
				if (nDistance < 10) {
					nDistance = PointDistance(ptDown, ptNow);	// ���������߂�
					if (nDistance < 10)
						break;
				}
				SetCursor(cursor);	// �J�[�\����ύX����

				CString strMark;
				if (CMouseOption::s_bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// �p�x�����߂�
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine = CMouseOption::s_strREngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = CMouseOption::s_strTEngine;	
						strMark = _T("[��] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = CMouseOption::s_strLEngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = CMouseOption::s_strBEngine;
						strMark = _T("[��] ");
					}
				} else {
					strSearchEngine = CMouseOption::s_strCEngine;
				}
				if (strSearchEngine.IsEmpty() == FALSE) {
					CString strMsg;
					strMsg.Format(_T("���� %s: %s"), strMark, strSearchEngine);
					m_UIChange.SetStatusText(strMsg);
				} else {
					m_UIChange.SetStatusText(_T(""));
				}
			} else {
				nDistance = PointDistance(ptLast, ptNow);	// ���������߂�
				if (nDistance < 10)
					break;
				
				CString strMark1;
				int nAng1  = (int) _GetAngle(ptLast, ptNow);	// �p�x�����߂�
				if		(nAng1 <  45 || nAng1 >  315)
					strMark1 = _T("��");
				else if (nAng1 >= 45 && nAng1 <= 135)
					strMark1 = _T("��");
				else if (nAng1 > 135 && nAng1 <  225)
					strMark1 = _T("��");
				else if (nAng1 >= 225 && nAng1 <= 315)
					strMark1 = _T("��");

				if (strMark1 == strLastMark) {					// ���������ɓ������āA����300ms�ȏ�o�����Ȃ�L��		
					DWORD dwTimeNow = ::GetTickCount();
					if ( (dwTimeNow - dwTime) > 300 ) {
						strLastMark = _T("");
						dwTime	 = dwTimeNow;
					}
				}
				if (strMark1 != strLastMark) {
					strMove	+= strMark1;	// ������ǉ�
					strLastMark = strMark1;	

					CString strCmdName;
					CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
					DWORD	dwCommand = pr.GetValue(strMove);
					if (dwCommand) {
						// ���v����R�}���h������Ε\��
						CString strTemp;
						CToolTipManager::LoadToolTipText(dwCommand, strTemp);
						strCmdName.Format(_T("[ %s ]"), strTemp);
					}

					// �X�e�[�^�X�o�[�ɕ\��
					CString 	strMsg;
					strMsg.Format(_T("�W�F�X�`���[ : %s %s"), strMove, strCmdName);
					m_UIChange.SetStatusText(strMsg);
				}
				dwTime = ::GetTickCount();
			}
			ptLast = ptNow;
			break;
						   }

		case WM_LBUTTONDOWN:
			if (bCursorOnSelectedText && nDistance >= 10) {	// �E�{�^���h���b�O���L�����Z������
				m_bCancelRButtonUp = true;
				msg.message = WM_RBUTTONUP;
				m_UIChange.SetStatusText(_T(""));
			}
			break;
			
		default:
			::DispatchMessage(&msg);
			break;
		}	// switch

		switch (dwCommand) {
		case 0:	
			break;
			
		case ID_FILE_CLOSE:
			PostMessage(WM_CLOSE);	// ���ꂾ�����Ⴝ�Ԃ�_��
			msg.message = WM_RBUTTONUP;
			//::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
			//::PostMessage(hWnd, WM_CLOSE, 0, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;

		case ID_GET_OUT:				// �ޔ�
		case ID_VIEW_FULLSCREEN:		// �S�̕\��
		case ID_VIEW_UP:				// ���
		case ID_VIEW_BACK:				// �O�ɖ߂�
		case ID_VIEW_FORWARD:			// ���ɐi��
		case ID_VIEW_STOP_ALL:			// ���ׂĒ��~
		case ID_VIEW_REFRESH_ALL:		// ���ׂčX�V
		case ID_WINDOW_CLOSE_ALL:		// ���ׂĕ���
		case ID_WINDOW_CLOSE_EXCEPT:	// ����ȊO����
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;

		default:
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;
		}

	} while (msg.message != WM_RBUTTONUP);

	ReleaseCapture();
	::FreeLibrary(hModule);
	if (bCursorOnSelectedText) {
		SetCursor(::LoadCursor(NULL, IDC_ARROW));	// �J�[�\�������ɖ߂�
		if (m_bCancelRButtonUp)
			return TRUE;
		if (strSearchEngine.IsEmpty() == FALSE) {	// �E�{�^���h���b�O���s
			m_UIChange.SetStatusText(_T(""));
			CString str = GetSelectedTextLine();
			CDonutSearchBar::GetInstance()->SearchWebWithEngine(str, strSearchEngine);
			bNoting = false;
		}
	}

	/* �}�E�X�W�F�X�`���[�R�}���h���s */
	if (bNoting) {
		ptLast.SetPoint(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		::ClientToScreen(msg.hwnd, &ptLast);

		m_UIChange.SetStatusText(_T(""));

		CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		DWORD dwCommand = pr.GetValue(strMove);
		if (dwCommand) {
			::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting = false;
		} else if (dwCommand == -1)
			return TRUE;
	}

	if ( bNoting && strMove.IsEmpty() ) {	// �E�N���b�N���j���[���o��
		::ScreenToClient(pMsg->hwnd, &ptLast);
		pMsg->lParam = MAKELONG(ptLast.x, ptLast.y);

		::PostMessage(pMsg->hwnd, WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
	}
	return !bNoting;
}

BOOL CChildFrame::Impl::OnXButtonUp(WORD wKeys, WORD wButton)
{
	CString 	strKey;
	switch (wButton) {
	case XBUTTON1: strKey = _T("Side1"); break;
	case XBUTTON2: strKey = _T("Side2"); break;
	}

	CIniFileI pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD	dwCommand = pr.GetValue(strKey);
	if (dwCommand == 0)
		return FALSE;

	switch (dwCommand) {
	case ID_VIEW_BACK:		GoBack();		break;
	case ID_VIEW_FORWARD:	GoForward();	break;
	default:
		GetTopLevelWindow().SendMessage(WM_COMMAND, dwCommand);
	}
	return TRUE;
}

// Overrides

BOOL CChildFrame::Impl::PreTranslateMessage(MSG* pMsg)
{
	if (m_bNowActive == false)
		return FALSE;

	// �������T�C�Y�̃g�O��
	if (   m_bImagePage 
		&& pMsg->message == WM_LBUTTONDOWN 
		&& CMainOption::s_nAutoImageResizeType != AUTO_IMAGE_RESIZE_NONE)
	{
		CPoint	pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		CRect	rc;
		GetClientRect(&rc);
		rc.right -= GetSystemMetrics(SM_CXVSCROLL);
		rc.bottom-= GetSystemMetrics(SM_CYHSCROLL);
		if (rc.PtInRect(pt))
			OnHtmlZoom(0, ID_HTMLZOOM_100TOGLE, 0);
		return FALSE;
	}

	// �~�h���N���b�N
	if ( pMsg->message == WM_MBUTTONDOWN && IsChild(pMsg->hwnd) && OnMButtonHook(pMsg) )
		return TRUE;

	// �E�h���b�O�L�����Z���p
	if ( pMsg->message == WM_RBUTTONUP && m_bCancelRButtonUp ) {
		m_bCancelRButtonUp = false;
		return TRUE;
	}
	// �}�E�X�W�F�X�`���[��
	if ( pMsg->message == WM_RBUTTONDOWN && OnRButtonHook(pMsg) )
		return TRUE;

	// �T�C�h�{�^��
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam)) )
			return TRUE;
	}

	// �A�N�Z�����[�^�L�[
	if (g_pMainWnd->m_hAccel != NULL && ::TranslateAccelerator(m_hWnd, g_pMainWnd->m_hAccel, pMsg))
			return TRUE;
	return m_view.PreTranslateMessage(pMsg);
}

/// �I�𒆂̕����������(�A�h���X�o�[��Ctrl+Enter�����Ƃ��̌����G���W�����g����)
void CChildFrame::Impl::searchEngines(const CString& strKeyWord)
{
	CString 	strSearchWord = strKeyWord;

	if (CAddressBarOption::s_bReplaceSpace)
		strSearchWord.Replace( _T("�@"), _T(" ") );

	//_ReplaceCRLF(strSearchWord,CString(_T(" ")));
	//strSearchWord.Replace('\n',' ');
	//strSearchWord.Remove('\r');
	strSearchWord.Replace( _T("\r\n"), _T("") );

	CIniFileI	pr( g_szIniFileName, _T("AddressBar") );
	CString 		strEngin = pr.GetStringUW( _T("EnterCtrlEngin"), NULL, 256 );

	::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) strSearchWord, (LPARAM) (LPCTSTR) strEngin);
}

static int _Pack(int hi, int low)
{
	if ( !( ( ('0' <= low && low <= '9') || ('A' <= low && low <= 'F') || ('a' <= low && low <= 'f') )
		  && ( ('0' <= hi && hi  <= '9') || ('A' <= hi	&& hi  <= 'F') || ('a' <= hi  && hi  <= 'f') ) ) )
		return 0;	//���l�ł͂Ȃ�

	int nlow = ('0' <= low && low <= '9') ? low - '0'
			 : ('A' <= low && low <= 'F') ? low - 'A' + 0xA
			 :								low - 'a' + 0xA ;
	int nhi  = ('0' <= hi  && hi  <= '9') ? hi	- '0'
			 : ('A' <= hi  && hi  <= 'F') ? hi	- 'A' + 0xA
			 :								hi	- 'a' + 0xA ;

	return (nhi << 4) + nlow;
}

#if 1
void	CChildFrame::Impl::DoPaint(CDCHandle dc)
{
	RECT rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect, RGB(255, 255, 255));
#if 0
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc) {
		CComVariant	vBgColor;
		spDoc->get_bgColor(&vBgColor);
		if (vBgColor.bstrVal) {
			CString strCol = vBgColor.bstrVal;

			COLORREF col = RGB(_Pack(strCol[1], strCol[2]) ,
							   _Pack(strCol[3], strCol[4]) ,
							   _Pack(strCol[5], strCol[6]) );
			dc.FillSolidRect(&rect, col);
			goto FINISH_FILL;
		}
	}
	dc.FillSolidRect(&rect, RGB(255, 255, 255));
FINISH_FILL:
#endif
	if (   m_bClosing 
		|| m_spBrowser == nullptr 
		|| Misc::IsGpuRendering() == false 
		|| CDLControlOption::s_nGPURenderStyle == CDLControlOption::GPURENDER_NONE)
		return ;

	if (m_pPageBitmap) {
		CMemoryDC	memDC(dc, rect);
		memDC.SelectBitmap(*m_pPageBitmap);
	}
#if 0
	CComQIPtr<IViewObject>	spViewObject = m_spBrowser;
	if (spViewObject == nullptr)
		return ;

	spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, dc, (RECTL*)&rect, (RECTL*)&rect, NULL, NULL);
#endif

#if 0
	if (m_bmpPage.IsNull())
		return ;
	CMemoryDC	memDC(dc, rect);
	memDC.SelectBitmap(m_bmpPage);
#endif
}
#endif

// Message map

int		CChildFrame::Impl::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	DefWindowProc();

	++(*m_pThreadRefCount);

	CChildFrameCommandUIUpdater::AddCommandUIMap(m_hWnd);
	m_UIChange.SetChildFrame(m_hWnd);

	RECT rc;
	GetClientRect(&rc);
	m_view.Create(m_hWnd, rc, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ATLASSERT( m_view.IsWindow() );
	m_view.SetDlgCtrlID(ID_DONUTVIEW);

	m_view.QueryControl(IID_IWebBrowser2, (void **)&m_spBrowser);

	WebBrowserEvents2Advise();

	SetRegisterAsBrowser(true);
	SetVisible(true);

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

void	CChildFrame::Impl::OnDestroy()
{
	m_bClosing = true;
	HRESULT hr = m_spBrowser->Stop();

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);

	SetRegisterAsBrowser(false);

	WebBrowserEvents2Unadvise();

	m_spBrowser.Release();

	m_view.DestroyWindow();

	--(*m_pThreadRefCount);
	if (*m_pThreadRefCount == 0) {
		TRACEIN(_T("ChildFreame�X���b�h�̔j��"));
		PostQuitMessage(0);
	}

	CChildFrameCommandUIUpdater::RemoveCommandUIMap(m_hWnd);
}

void	CChildFrame::Impl::OnClose()
{
	SetMsgHandled(FALSE);

	CWindow	wndMain = GetTopLevelWindow();

	ChildFrameDataOnClose*	pClosedTabData = new ChildFrameDataOnClose;
	_CollectDataOnClose(*pClosedTabData);
	wndMain.PostMessage(WM_ADDRECENTCLOSEDTAB, (WPARAM)pClosedTabData);

	{
		int nIndex = (int)GetTopLevelWindow().SendMessage(WM_GETTABINDEX, (WPARAM)m_hWnd);
		//�v���O�C���C�x���g - �N���[�Y
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CLOSE, nIndex, 0);
	}

	wndMain.SendMessage(WM_TABDESTROY, (WPARAM)m_hWnd);
}

void	CChildFrame::Impl::OnSize(UINT nType, CSize size)
{
	DefWindowProc();
	if (m_view.IsWindow())
		m_view.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER);

	_AutoImageResize(false);
}

void	CChildFrame::Impl::OnChildFrameActivate(HWND hWndAct, HWND hWndDeact)
{
	if (hWndAct == m_hWnd) {
		m_bNowActive = true;
		if (MtlIsApplicationActive(m_hWnd) && m_view.IsWindow())
			m_view.SetFocus();

		if (CSearchBarOption::s_bSaveSearchWord) {
			CDonutSearchBar::GetInstance()->SetSearchStr(m_strSearchWord); //\\ �ۑ����Ă�����������������o�[�ɖ߂�
			CDonutSearchBar::GetInstance()->ForceSetHilightBtnOn(m_bNowHilight);
		}

	} else if (hWndDeact == m_hWnd) {
		m_bNowActive = false;
		// _KillFocusToHTML
		HRESULT	hr = E_FAIL;
		CComQIPtr<IOleInPlaceObject> spIOleInPlaceObject = m_spBrowser;
		if(spIOleInPlaceObject) {
			hr = spIOleInPlaceObject->UIDeactivate(); // IE��UI�𖳌���
		}

		if( m_bSaveSearchWordflg ){	//\\ ���݁A�����o�[�ɂ��镶���������Ă���
			CDonutSearchBar::GetInstance()->GetEditCtrl().GetWindowText(m_strSearchWord.GetBuffer(1024), 1024);
			m_strSearchWord.ReleaseBuffer();
		} else {
			m_bSaveSearchWordflg = true;
		}
#if 0
		{
			CComQIPtr<IViewObject>	spViewObject = m_spBrowser;
			if (spViewObject == nullptr)
				return ;

			RECT rc;
			GetClientRect(&rc);
			if (m_bmpPage.IsNull() == false)
				m_bmpPage.DeleteObject();

			CDC dc = GetDC();
			m_bmpPage.CreateCompatibleBitmap(dc, rc.right, rc.bottom);
			CDC	memDC = CreateCompatibleDC(dc);
			CBitmap bmpPrev = memDC.SelectBitmap(m_bmpPage);
			spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, memDC, (RECTL*)&rc, (RECTL*)&rc, NULL, NULL);
		}
#endif
	}
}

/// ���ݑI������Ă���e�L�X�g��Ԃ�
void	CChildFrame::Impl::OnGetSelectedText(LPCTSTR* ppStr)
{
	CString strSelectedText = GetSelectedText();
	LPTSTR	strtemp = new TCHAR[strSelectedText.GetLength() + 1];
	::wcscpy_s(strtemp, strSelectedText.GetLength() + 1, strSelectedText);
	*ppStr = strtemp;
}

/// strScriptText�ɏ�����Ă���X�N���v�g�����s����
void	CChildFrame::Impl::OnExecuteUserJavascript(CString* pstrScriptText)
{
	if (m_spBrowser == nullptr)
		return ;

	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return ;
	CComPtr<IHTMLWindow2>	spWindow;
	spDoc->get_parentWindow(&spWindow);
	if (spWindow == nullptr)
		return ;
	CComVariant	vRet;
	spWindow->execScript(CComBSTR(*pstrScriptText), CComBSTR(L"javascript"), &vRet);
	TRACEIN(_T("OnExecuteUserJavascript() : ���s(%s)"), CString(*pstrScriptText).Left(45));
	delete pstrScriptText;
}

void	CChildFrame::Impl::OnDrawChildFramePage(CDCHandle dc)
{
	if (m_bClosing)
		return ;
	CComQIPtr<IViewObject>	spViewObject = m_spBrowser;
	if (spViewObject == nullptr)
		return ;

	RECT rect;
	GetClientRect(&rect);
	spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, dc, (RECTL*)&rect, (RECTL*)&rect, NULL, NULL);
}


/// �t�@�C�������
void 	CChildFrame::Impl::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//+++ �^�u���b�N�΍�... OnClose���̂����ő��v�̂悤������ǁA�Ƃ肠����.
	if (   //::: s_bMainframeClose == false											//+++ unDonut�I�����ȊO��
		/*&&*/ _check_flag(m_view.GetExStyle(), DVS_EX_OPENNEWWIN)					//+++ �i�r�Q�[�g���b�N�̃y�[�W��
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK) )	//+++ �i�r�Q�[�g���b�N�̃y�[�W����Ȃ��A�̎w�肪�����
	{
		return; 																//+++ �����ɋA��
	}

	PostMessage(WM_CLOSE);
}

// �����o�[����

/// �n�C���C�g
LRESULT CChildFrame::Impl::OnHilight(CString strKeyWord)
{
	bool bHilightSw = CDonutSearchBar::GetInstance()->GetHilightSw();
	//if (m_bNowHilight != bHilightSw)
	{
		m_bNowHilight	= bHilightSw;
		if (bHilightSw) {
			m_strSearchWord = strKeyWord;
			DeleteMinimumLengthWord(strKeyWord);
		} else {
			m_strSearchWord.Empty();
		}
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(strKeyWord, bHilightSw));
	}
	return TRUE;
}

/// �y�[�W������
LRESULT CChildFrame::Impl::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown, long Flags /*= 0*/)
{
	if (!m_spBrowser)
		return 0;

	CComPtr<IDispatch>	spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDocument = spDisp;	// html�̎擾
	if (!spDocument)
		return 0;

	CString strKeyword = lpszKeyWord;
	strKeyword.Replace(_T('�'), _T('�J'));

	// ����
	BOOL	bSts = _FindKeyWordOne(spDocument, strKeyword, bFindDown, Flags);
	if (bSts)
		return TRUE;

	// �t���[���E�B���h�E�̎擾
	CComPtr<IHTMLFramesCollection2> 	spFrames;
	hr = spDocument->get_frames(&spFrames);
	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) )
		return 0;

	// �t���[�����E�B���h�E�̐����擾
	LONG	nCount	   = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return 0;

	BOOL	bFindIt    = FALSE;
	if (bFindDown) {	// �y�[�W������ - ��
		for (LONG ii = m_nPainBookmark; ii < nCount; ii++) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// �t���[�����̃E�B���h�E���擾
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
			if ( FAILED(hr) ) {
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					continue;
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					continue;
				spDocumentFr = spDisp;
				if (!spDocument)
					continue;
			}

			// ����
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, bFindDown, Flags);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}

	} else {			// �y�[�W������ - ��
		if (m_nPainBookmark == 0 && !m_strBookmark)
			m_nPainBookmark = nCount - 1;

		for (LONG ii = m_nPainBookmark; ii >= 0; ii--) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// �E�B���h�E�̎擾
			hr		= spFrames->item(&varItem, &varResult);
			if ( FAILED(hr) )
				continue;

			CComQIPtr<IHTMLWindow2> spWindow = varResult.pdispVal;
			if (!spWindow)
				continue;

			CComPtr<IHTMLDocument2> spDocumentFr;
			hr		= spWindow->get_document(&spDocumentFr);
			if ( FAILED(hr) ) {
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					continue;
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					continue;
				spDocumentFr = spDisp;
				if (!spDocument)
					continue;
			}

			// ����
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, bFindDown, Flags);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}
	}

	return bFindIt;
}

static void _RemoveHighlight(IHTMLDocument3* pDoc3)
{
	CComQIPtr<IHTMLDocument2> spDoc2 = pDoc3;
	_MtlForEachHTMLDocument2(spDoc2, [&](IHTMLDocument2* pDoc) {
		CComPtr<IHTMLSelectionObject> spSelection;	/* �e�L�X�g�I������ɂ��� */
		pDoc->get_selection(&spSelection);
		if (spSelection.p)
			spSelection->empty();

		vector<CComPtr<IHTMLElement> > vecElm;
		CComPtr<IHTMLElementCollection>	spCol;
		CComQIPtr<IHTMLDocument3> spDoc3 = pDoc;
		spDoc3->getElementsByTagName(CComBSTR(L"span"), &spCol);
		ForEachHtmlElement(spCol, [&vecElm](IDispatch* pDisp) -> bool {
			CComQIPtr<IHTMLElement>	spElm = pDisp;
			if (spElm.p) {
				CComBSTR strID;
				spElm->get_id(&strID);
				if (strID && strID == L"udfbh")
					vecElm.push_back(spElm);
			}
			return true;
		});
		for (auto it = vecElm.rbegin(); it != vecElm.rend(); ++it) {
			CComBSTR str;
			(*it)->get_innerText(&str);
			(*it)->put_outerHTML(str);
		}
	});
}

int		CChildFrame::Impl::OnHilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags)
{
	CString strKeyword = strText;
	CComBSTR	strChar(L"Character");
	CComBSTR	strTextedit(L"Textedit");
	CComBSTR	strBackColor(L"BackColor");
	CComBSTR	strColor(L"greenyellow");

	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return 0;

	/* �O�̃n�C���C�g�\�������� */
	CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc;
	if (bEraseOld)
		_RemoveHighlight(spDoc3);

	/* �P����n�C���C�g���� */
	int nMatchCount = 0;
	if (strKeyword.GetLength() > 0) {
		_MtlForEachHTMLDocument2(spDoc, [&](IHTMLDocument2* pDoc) {
			CComPtr<IHTMLSelectionObject> spSelection;	/* �e�L�X�g�I������ɂ��� */
			pDoc->get_selection(&spSelection);
			if (spSelection.p)
				spSelection->empty();

			CComPtr<IHTMLElement>	spElm;
			pDoc->get_body(&spElm);
			CComQIPtr<IHTMLBodyElement>	spBody = spElm;
			if (spBody.p == nullptr)
				return;

			CComPtr<IHTMLTxtRange>	spTxtRange;
			spBody->createTextRange(&spTxtRange);
			if (spTxtRange.p == nullptr)
				return;

			//long nMove;
			//spTxtRange->moveStart(strTextedit, -1, &nMove);
			//spTxtRange->moveEnd(strTextedit, 1, &nMove);
			VARIANT_BOOL	vResult;
			CComBSTR	strWord = strKeyword;
			while (spTxtRange->findText(strWord, 1, Flags, &vResult), vResult == VARIANT_TRUE) {
				CComPtr<IHTMLElement> spParentElement;
				spTxtRange->parentElement(&spParentElement);
				CComBSTR	bstrParentTag;
				spParentElement->get_tagName(&bstrParentTag);
				if (   bstrParentTag != _T("SCRIPT")
					&& bstrParentTag != _T("NOSCRIPT")
					&& bstrParentTag != _T("TEXTAREA")
					&& bstrParentTag != _T("STYLE")) 
				{
					if (bNoHighlight == false) {
						CComBSTR strInnerText;
						spTxtRange->get_text(&strInnerText);
						//VARIANT_BOOL	vRet;
						//spTxtRange->execCommand(strBackColor, VARIANT_FALSE, CComVariant(strColor), &vRet);
						CComBSTR strValue(L"<span id=\"udfbh\" style=\"color:black;background:greenyellow\">");//#00FFFF
						strValue += strInnerText;
						strValue += _T("</span>");
						spTxtRange->pasteHTML(strValue);
					}
					++nMatchCount;
				}
				spTxtRange->collapse(VARIANT_FALSE);
			}
		});
	}
	return nMatchCount;
}

void	CChildFrame::Impl::OnRemoveHilight()
{
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3> spDoc3 = spDisp;
	if (spDoc3)
		_RemoveHighlight(spDoc3);
}


/// �|�b�v�A�b�v�Y�[�����j���[���J��
void	CChildFrame::Impl::OnHtmlZoomMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::SetForegroundWindow(m_hWnd);

	CMenu	menu0;
	menu0.LoadMenu(IDR_ZOOM_MENU);
	ATLASSERT(menu0.IsMenu());
	CMenuHandle menu = menu0.GetSubMenu(0);
	ATLASSERT(menu.IsMenu());

	// �|�b�v�A�b�v���j���[���J��.
	POINT 	pt;
	::GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
}

void	CChildFrame::Impl::OnSpecialKeys(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCode = 0;
	switch (nID) {
	case ID_SPECIAL_HOME:		nCode = VK_HOME;	break;
	case ID_SPECIAL_END:		nCode = VK_END; 	break;
	case ID_SPECIAL_PAGEUP: 	nCode = VK_PRIOR;	break;
	case ID_SPECIAL_PAGEDOWN:	nCode = VK_NEXT;	break;
	case ID_SPECIAL_UP: 		nCode = VK_UP;		break;
	case ID_SPECIAL_DOWN:		nCode = VK_DOWN;	break;
	default:	
		ATLASSERT(FALSE);
		return ;
	}

	if (nID == ID_SPECIAL_HOME || ID_SPECIAL_END) {
		auto funcGetHTMLWindowOnCursorPos = [](CPoint& pt, IHTMLDocument3* pDoc) -> CComPtr<IHTMLWindow2> {
			auto funcGetIFrameAbsolutePosition = [](CComQIPtr<IHTMLElement>	spIFrame) -> CRect {
				CRect rc;
				spIFrame->get_offsetHeight(&rc.bottom);
				spIFrame->get_offsetWidth(&rc.right);
				CComPtr<IHTMLElement>	spCurElement = spIFrame;
				do {
					CPoint temp;
					spCurElement->get_offsetTop(&temp.y);
					spCurElement->get_offsetLeft(&temp.x);
					rc += temp;
					CComPtr<IHTMLElement>	spTemp;
					spCurElement->get_offsetParent(&spTemp);
					spCurElement.Release();
					spCurElement = spTemp;
				} while (spCurElement.p);
				
				return rc;
			};
			auto funcGetScrollPosition = [](CComQIPtr<IHTMLDocument2> spDoc2) -> CPoint {
				CPoint ptScroll;
				CComPtr<IHTMLElement>	spBody;
				spDoc2->get_body(&spBody);
				CComQIPtr<IHTMLElement2>	spBody2 = spBody;
				spBody2->get_scrollTop(&ptScroll.y);
				spBody2->get_scrollLeft(&ptScroll.x);
				if (ptScroll == CPoint(0, 0)) {
					CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc2;
					CComPtr<IHTMLElement>	spDocumentElement;
					spDoc3->get_documentElement(&spDocumentElement);
					CComQIPtr<IHTMLElement2>	spDocumentElement2 = spDocumentElement;
					spDocumentElement2->get_scrollTop(&ptScroll.y);
					spDocumentElement2->get_scrollLeft(&ptScroll.x);
				}
				return ptScroll;
			};

			HRESULT hr = S_OK;
			CComQIPtr<IHTMLDocument2>	spDoc2 = pDoc;

			CComPtr<IHTMLFramesCollection2>	spFrames;
			spDoc2->get_frames(&spFrames);
			CComPtr<IHTMLElementCollection>	spIFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"iframe"), &spIFrameCol);
			CComPtr<IHTMLElementCollection>	spFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"frame"), &spFrameCol);
			
			long frameslength = 0, iframelength = 0, framelength = 0;
			spFrames->get_length(&frameslength);
			spIFrameCol->get_length(&iframelength);
			spFrameCol->get_length(&framelength);
			ATLASSERT(frameslength == iframelength || frameslength == framelength);

			if (frameslength == iframelength && spIFrameCol.p && spFrames.p) {
				for (long i = 0; i < iframelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spIFrameCol->item(vIndex, vIndex, &spDisp2);
					CRect rcAbsolute = funcGetIFrameAbsolutePosition(spDisp2.p);
					CPoint ptScroll = funcGetScrollPosition(spDoc2);
					CRect rc = rcAbsolute - ptScroll;
					if (rc.PtInRect(pt)) {
						CComVariant vResult;
						spFrames->item(&vIndex, &vResult);
						CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
						return spWindow;
					}
				}
			}

			if (frameslength == framelength && spFrameCol.p && spFrames.p) {
				for (long i = 0; i < framelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spFrameCol->item(vIndex, vIndex, &spDisp2);
					CComQIPtr<IHTMLElement>	spFrame = spDisp2;
					if (spFrame.p) {
						CRect rc;
						spFrame->get_offsetLeft(&rc.left);
						spFrame->get_offsetTop(&rc.top);
						long temp;
						spFrame->get_offsetWidth(&temp);
						rc.right += rc.left + temp;
						spFrame->get_offsetHeight(&temp);
						rc.bottom+= rc.top + temp;
						if (rc.PtInRect(pt)) {
							CComVariant vResult;
							spFrames->item(&vIndex, &vResult);
							CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
							return spWindow;
						}
					}
				}
			}
			return nullptr;
		};

		CComPtr<IDispatch>	spDisp;
		HRESULT hr = m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument3>	spDocument = spDisp;
		if (spDocument == nullptr)
			return ;
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		CComPtr<IHTMLWindow2> spWindow = funcGetHTMLWindowOnCursorPos(pt, spDocument);
		if (spWindow) {
			CComQIPtr<IHTMLDocument2>	spFrameDocument;
			hr = spWindow->get_document(&spFrameDocument);
			if ( FAILED(hr) ) {
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					return ;
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					return ;
				spFrameDocument = spDisp;
				if (!spFrameDocument)
					return ;
				spWindow.Release();
				spFrameDocument->get_parentWindow(&spWindow);
				if (!spWindow)
					return ;
			}

			if (nID == ID_SPECIAL_HOME)
				spWindow->scrollBy(0, -300000);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +300000);
		} else {
			CComQIPtr<IHTMLDocument2>	spDocument2 = spDocument;
			spDocument2->get_parentWindow(&spWindow);
			ATLASSERT(spWindow);
			if (nID == ID_SPECIAL_HOME)
				spWindow->scrollBy(0, -300000);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +300000);
		}
	} else {
		m_view.SetFocus();

		INPUT	inputs[2] = { 0 };
		inputs[0].type		= INPUT_KEYBOARD;
		inputs[0].ki.wVk	= nCode;
		inputs[0].ki.wScan	= ::MapVirtualKey(nCode, 0);
		inputs[0].ki.dwFlags= 0;
		inputs[1].type		= INPUT_KEYBOARD;
		inputs[1].ki.wVk	= nCode;
		inputs[1].ki.wScan	= ::MapVirtualKey(nCode, 0);
		inputs[1].ki.dwFlags= KEYEVENTF_KEYUP;
		SendInput(2, inputs, sizeof(INPUT));

		//PostMessage(WM_KEYDOWN, nCode, 0);
	}
}

/// �摜��ۑ�����
void	CChildFrame::Impl::OnSaveImage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto funcGetUrl = [](CComQIPtr<IDispatch> spDisp) -> LPCTSTR {
		CComBSTR strUrl;
		CComQIPtr<IHTMLImgElement>	spImage =  spDisp;
		if (spImage != NULL) {
			spImage->get_href(&strUrl);
		} else {
			CComQIPtr<IHTMLInputElement>	spInput = spDisp;
			if (spInput != NULL) {
				spInput->get_src(&strUrl);
			} else {
				CComQIPtr<IHTMLAreaElement>	spArea = spDisp;
				if (spArea != NULL) {
					spArea->get_href(&strUrl);
				}
			}
		}
		return strUrl;
	};

	CString strUrl = m_view.GetAnchorURL();
	if (strUrl.IsEmpty() == FALSE) {
		CDownloadManager::GetInstance()->DownloadStart(strUrl, NULL, NULL, DLO_SAVEIMAGE);
	} else {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		ATLASSERT(spDocument);
		CPoint	pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		CComPtr<IHTMLElement>	spHitElement;
		spDocument->elementFromPoint(pt.x, pt.y, &spHitElement);
		// Hit�悪�摜�̏ꍇ
		CComQIPtr<IHTMLImgElement>	spImg = spHitElement;
		if (spImg == NULL) {
			// �t���[���̉\��
			CComQIPtr<IHTMLFrameElement3>	spFrame = spHitElement;
			if (spFrame) {
				LONG x, y;
				spHitElement->get_offsetTop(&y);
				spHitElement->get_offsetLeft(&x);
				pt.Offset(-x, -y);
				CComPtr<IDispatch>	spFrameDisp;
				spFrame->get_contentDocument(&spFrameDisp);
				CComQIPtr<IHTMLDocument2>	spFrameDocument = spFrameDisp;
				if (spFrameDocument == NULL) 
					return;	// �ʃh���C��
				ATLASSERT(spFrameDocument);
				CComPtr<IHTMLElement>	spHitFrameElement;
				spFrameDocument->elementFromPoint(pt.x, pt.y, &spHitFrameElement);
				spImg = spHitFrameElement;
			}
		}
		if (spImg) {
			CString strUrl = funcGetUrl(spImg.p);
			if (strUrl.IsEmpty() == FALSE)
				CDownloadManager::GetInstance()->DownloadStart(strUrl, NULL, NULL, DLO_SAVEIMAGE);
		}
	}
}


void	CChildFrame::Impl::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_bReload = true;
	if (::GetAsyncKeyState(VK_CONTROL) < 0) 	// Inspired by DOGSTORE, Thanks
		Refresh2(REFRESH_COMPLETELY);
	else
		Refresh();
}

void	CChildFrame::Impl::OnHtmlZoom(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	auto SetBodyStyleZoom	= [this](int addSub, int scl, bool bWheel) {
		if (Misc::getIEMejourVersion() >= 7 && bWheel) {
			CComVariant vZoom;
			if (addSub) {
				m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, NULL, &vZoom);
				scl =  static_cast<int>(vZoom.lVal);
				scl += addSub;
			}
			if (scl < 5)
				scl = 5;
			else if (scl > 500)
				scl = 500;

			vZoom.lVal = scl;
			m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoom, NULL); 

		} else {
			CComPtr<IDispatch>	spDisp;
			m_spBrowser->get_Document(&spDisp);
			CComQIPtr<IHTMLDocument2> 	pDoc = spDisp;
			if ( pDoc.p == nullptr )
				return;

			CComPtr<IHTMLElement>	spHTMLElement;
			if ( FAILED( pDoc->get_body(&spHTMLElement) ) && !spHTMLElement )
				return;
			CComQIPtr<IHTMLElement2>  spHTMLElement2 = spHTMLElement;
			if (!spHTMLElement2)
				return;

			CComPtr<IHTMLStyle>		pHtmlStyle;
			if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) && !pHtmlStyle )
				return;

			CComVariant variantVal;
			CComBSTR	bstrZoom( L"zoom" );
			if (addSub) {
				if ( FAILED(pHtmlStyle->getAttribute(bstrZoom, 1, &variantVal)) )
					return;
				CComBSTR	bstrTmp(variantVal.bstrVal);
				scl =  !bstrTmp.m_str ? 100 : ::wcstol((wchar_t*)LPCWSTR(bstrTmp.m_str), 0, 10);
				scl += addSub;
			}
			if (scl < 5)
				scl = 5;
			else if (scl > 500)
				scl = 500;

			m_nImgScl = scl;

			wchar_t	wbuf[128] = L"\0";
			swprintf_s(wbuf, L"%d%%", scl);
			variantVal = CComVariant(wbuf);
			pHtmlStyle->setAttribute(bstrZoom, variantVal, 1);
			if (scl != 100 && m_ImageSize != CSize(0, 0))		// �X�N���[���o�[���B��
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));
			else 
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"auto"));
		}
	};	// lamda

	switch (nID) {
	case ID_HTMLZOOM_ADD:	SetBodyStyleZoom(+10, 0, wndCtl == 0);	break;
	case ID_HTMLZOOM_SUB:	SetBodyStyleZoom(-10, 0, wndCtl == 0);	break;

	case ID_HTMLZOOM_100TOGLE:
		if (m_nImgScl == 100) {	// 100% �Ȃ猳�̊g�嗦��
			m_nImgScl	 = m_nImgSclSav;
			m_nImgSclSw	 = 1;
		} else {				// ����ȊO�Ȃ� 100% ��
			m_nImgSclSav = m_nImgScl;
			m_nImgScl    = 100;
			m_nImgSclSw	 = 0;
		}
		SetBodyStyleZoom(0, m_nImgScl, false);
		break;

	default: {
		ATLASSERT(ID_HTMLZOOM_400 <= nID && nID <= ID_HTMLZOOM_050);
		static const int scls[] = { 400, 200, 150, 125, 100, 75, 50 };
		int  n = nID - ID_HTMLZOOM_400;
		if (n < 0 || n > ID_HTMLZOOM_050 - ID_HTMLZOOM_400)
			return;
		SetBodyStyleZoom(0, scls[n], false);
		}
		break;
	}
}


/// �I��͈͂̃����N���J��
void 	CChildFrame::Impl::OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CSimpleArray<CString> arrUrls;
	bool bNoAddFromMenu = false;
	MtlForEachHTMLDocument2( m_spBrowser, [&arrUrls, &bNoAddFromMenu, this] (IHTMLDocument2 *pDocument) {
		CComPtr<IHTMLSelectionObject>	spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>				spDisp;
		hr = spSelection->createRange(&spDisp);
		if ( FAILED(hr) )
			return;

		CComQIPtr<IHTMLTxtRange>		spTxtRange = spDisp;
		if (!spTxtRange)
			return;

		CComBSTR						bstrLocationUrl;
		CComBSTR						bstrText;
		hr = spTxtRange->get_htmlText(&bstrText);
		if (FAILED(hr) || !bstrText) {	//+++
			if (bNoAddFromMenu == false) {	// �E�N���b�N���j���[�������Ă���
				bNoAddFromMenu = true;
				CString strURL = m_view.GetAnchorURL();
				if (strURL.IsEmpty() == FALSE)
					arrUrls.Add(strURL);
			}
		} else {
			hr = pDocument->get_URL(&bstrLocationUrl);
			if ( FAILED(hr) )
				return;

			//BASE�^�O�ɑΏ����� minit
			CComPtr<IHTMLElementCollection> spAllClct;
			hr = pDocument->get_all(&spAllClct);
			if ( SUCCEEDED(hr) ) {
				CComQIPtr<IHTMLElementCollection> spBaseClct;
				CComVariant 		val = _T("BASE");
				CComPtr<IDispatch>	spDisp;
				hr = spAllClct->tags(val, &spDisp);
				spBaseClct	= spDisp;
				if ( SUCCEEDED(hr) && spBaseClct ) {
					long	length;
					hr = spBaseClct->get_length(&length);
					if (length > 0) {
						CComPtr<IHTMLElement> spElem;
						CComVariant 		  val1( (int) 0 ), val2( (int) 0 );
						hr = spBaseClct->item(val1, val2, (IDispatch **) &spElem);
						if ( SUCCEEDED(hr) ) {
							CComPtr<IHTMLBaseElement> spBase;
							hr = spElem->QueryInterface(&spBase);
							if ( SUCCEEDED(hr) ) {
								CComBSTR bstrBaseUrl;
								hr = spBase->get_href(&bstrBaseUrl);
								if ( SUCCEEDED(hr) && !(!bstrBaseUrl) )
									bstrLocationUrl = bstrBaseUrl;
							}
						}
					}
				}
			}

			MtlCreateHrefUrlArray( arrUrls, WTL::CString(bstrText), WTL::CString(bstrLocationUrl) );
			if (arrUrls.GetSize() > 0)
				bNoAddFromMenu = true;	// �I��͈͂��烊���N�����������̂�
		}
	});
#if 1	//:::
	//m_MDITab.SetLinkState(LINKSTATE_B_ON);
	int size = arrUrls.GetSize();
	for (int i = 0; i < size; ++i) {
		DWORD dwOpenFlags = D_OPENFILE_CREATETAB;
		if (  i == (size - 1) 
			&& !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN))
			dwOpenFlags |= D_OPENFILE_ACTIVATE;	// �����N���J���Ƃ��A�N�e�B�u�ɂ��Ȃ��ɏ]��
		DonutOpenFile(arrUrls[i], dwOpenFlags);
	}
	//m_MDITab.SetLinkState(LINKSTATE_OFF);
#endif
}

/// URL�e�L�X�g���J��
void 	CChildFrame::Impl::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	std::wstring strText = GetSelectedText();
	if (strText.empty())
		return;

	std::vector<CString>	vecUrls;
	std::wregex rx(L"(http(?:s|)://|)((?:[a-zA-Z0-9_\\-]+\\.)+\\w+(?::\\d+|)(?:/[a-zA-Z0-9./_\\-?#%&=+��-��A-���-݈�-�]*|))");
	std::wsmatch result;
	auto itbegin = strText.cbegin();
	auto itend	 = strText.cend();
	while (std::regex_search(itbegin, itend, result, rx)) {
		CString strUrl;
		if (result[1].str().empty())
			strUrl = _T("http://");
		strUrl += result[2].str().c_str();
		vecUrls.push_back(strUrl);
		itbegin = result[0].second;
	}
#if 0
	std::vector<CString> lines;
	lines.reserve(20);
	Misc::SeptTextToLines(lines, strText);
	size_t	size = lines.size();
	//+++ �I�𕶎��񒆂�url�ۂ����̂��Ȃ�������A�����񌟍��ɂ��Ă݂�.
	bool	f	= 0;
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		f |= (strUrl.Find(_T(':')) >= 0) || (strUrl.Find(_T('/')) >= 0) || (strUrl.Find(_T('.')) >= 0) ||  (strUrl.Find(_T('\\')) >= 0);
		if (f)
			break;
	}
	if (f == 0) {	// url�ۂ������񂪂Ȃ�����...
		CDonutSearchBar*	pSearchBar = CDonutSearchBar::GetInstance();
		if (pSearchBar) {
			CString str	= lines[0];
			LPCTSTR	strExcept  = _T(" \t\"\r\n�@");
			str.TrimLeft(strExcept);
			str.TrimRight(strExcept);

			pSearchBar->SearchWeb(str);
			return;
		}
	}
#endif
	size_t size = vecUrls.size();
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = vecUrls[i];
		//Misc::StrToNormalUrl(strUrl);		//+++ �֐���
		DWORD dwOpenFlags = D_OPENFILE_CREATETAB;
		if (  i == (size - 1) 
			&& !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN))
			dwOpenFlags |= D_OPENFILE_ACTIVATE;	// �����N���J���Ƃ��A�N�e�B�u�ɂ��Ȃ��ɏ]��
		DonutOpenFile(strUrl, dwOpenFlags);
	}
}

/// �|�b�v�A�b�v�}�~�ɒǉ����ĕ��܂��B
void 	CChildFrame::Impl::OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::Add( GetLocationURL() );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// �^�C�g���}�~�ɒǉ�
void 	CChildFrame::Impl::OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// n�y�[�W�߂�
void	CChildFrame::Impl::OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_BACK1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoBack();
}

/// n�y�[�W�i��
void	CChildFrame::Impl::OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_FORWARD1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoForward();
}

// �ҏW

void	CChildFrame::Impl::OnEditFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	if (CMainOption::s_bUseCustomFindBar) {
		CString strText = GetSelectedText();
		GetTopLevelWindow().SendMessage(WM_OPENFINDBARWITHTEXT, (WPARAM)(LPCTSTR)strText);
	} else {
		__super::OnEditFind(0, 0, NULL);
	}
}

/// �^�C�g�����N���b�v�{�[�h�ɃR�s�[���܂��B
void	CChildFrame::Impl::OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(MtlGetWindowText(m_hWnd), m_hWnd);
}

/// �A�h���X���N���b�v�{�[�h�ɃR�s�[���܂��B
void	CChildFrame::Impl::OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(GetLocationURL(), m_hWnd);
}

/// �^�C�g���ƃA�h���X���N���b�v�{�[�h�ɃR�s�[���܂��B
void	CChildFrame::Impl::OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strText;
	strText.Format(_T("%s\r\n%s"), MtlGetWindowText(m_hWnd), GetLocationURL());
	MtlSetClipboardText(strText, m_hWnd);
}

// �\��

void	CChildFrame::Impl::OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	Stop();

	m_nDownloadCounter = 0;
	OnStateCompleted();
}


/// �p������DL�R���g���[���𓾂�
DWORD	CChildFrame::Impl::_GetInheritedDLCtrlFlags()
{
	DWORD dwDLFlags = CDLControlOption::s_dwDLControlFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) )
		dwDLFlags = m_view.GetDLControlFlags();
	return dwDLFlags;
}

/// �p������ExStyle�𓾂�
DWORD	CChildFrame::Impl::_GetInheritedExStyleFlags()
{
	DWORD dwExFlags = CDLControlOption::s_dwExtendedStyleFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) ) {
		dwExFlags = m_view.GetExStyle();
	  #if 1	//+++ �i�r�Q�[�g���b�N�Ɋւ��Ă͌p�����Ȃ�....
		dwExFlags &= ~DVS_EX_OPENNEWWIN;											//+++ off
		dwExFlags |= CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_OPENNEWWIN;	//+++ ���ǁA�f�t�H���g�ݒ肪����΁A����𔽉f.
	  #endif
	}
	return dwExFlags;
}

bool	CChildFrame::Impl::_CursorOnSelectedText()
{
	try {
		HRESULT hr = S_OK;

		CPoint	pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		CComPtr<IDispatch>	spDisp;
		hr = m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		if (spDocument == NULL)
			AtlThrow(hr);


		auto funcGetHTMLWindowOnCursorPos = [](CPoint& pt, IHTMLDocument3* pDoc) -> CComPtr<IHTMLWindow2> {
			auto funcGetIFrameAbsolutePosition = [](CComQIPtr<IHTMLElement>	spIFrame) -> CRect {
				CRect rc;
				spIFrame->get_offsetHeight(&rc.bottom);
				spIFrame->get_offsetWidth(&rc.right);
				CComPtr<IHTMLElement>	spCurElement = spIFrame;
				do {
					CPoint temp;
					spCurElement->get_offsetTop(&temp.y);
					spCurElement->get_offsetLeft(&temp.x);
					rc += temp;
					CComPtr<IHTMLElement>	spTemp;
					spCurElement->get_offsetParent(&spTemp);
					spCurElement.Release();
					spCurElement = spTemp;
				} while (spCurElement.p);
				
				return rc;
			};
			auto funcGetScrollPosition = [](CComQIPtr<IHTMLDocument2> spDoc2) -> CPoint {
				CPoint ptScroll;
				CComPtr<IHTMLElement>	spBody;
				spDoc2->get_body(&spBody);
				CComQIPtr<IHTMLElement2>	spBody2 = spBody;
				spBody2->get_scrollTop(&ptScroll.y);
				spBody2->get_scrollLeft(&ptScroll.x);
				if (ptScroll == CPoint(0, 0)) {
					CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc2;
					CComPtr<IHTMLElement>	spDocumentElement;
					spDoc3->get_documentElement(&spDocumentElement);
					CComQIPtr<IHTMLElement2>	spDocumentElement2 = spDocumentElement;
					spDocumentElement2->get_scrollTop(&ptScroll.y);
					spDocumentElement2->get_scrollLeft(&ptScroll.x);
				}
				return ptScroll;
			};

			HRESULT hr = S_OK;
			CComQIPtr<IHTMLDocument2>	spDoc2 = pDoc;

			CComPtr<IHTMLFramesCollection2>	spFrames;
			spDoc2->get_frames(&spFrames);
			CComPtr<IHTMLElementCollection>	spIFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"iframe"), &spIFrameCol);
			CComPtr<IHTMLElementCollection>	spFrameCol;
			pDoc->getElementsByTagName(CComBSTR(L"frame"), &spFrameCol);
			
			long frameslength = 0, iframelength = 0, framelength = 0;
			spFrames->get_length(&frameslength);
			spIFrameCol->get_length(&iframelength);
			spFrameCol->get_length(&framelength);
			ATLASSERT(frameslength == iframelength || frameslength == framelength);

			if (frameslength == iframelength && spIFrameCol.p && spFrames.p) {	// �C�����C���t���[��
				for (long i = 0; i < iframelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spIFrameCol->item(vIndex, vIndex, &spDisp2);
					CRect rcAbsolute = funcGetIFrameAbsolutePosition(spDisp2.p);
					CPoint ptScroll = funcGetScrollPosition(spDoc2);
					CRect rc = rcAbsolute - ptScroll;
					if (rc.PtInRect(pt)) {
						pt.x	-= rc.left;
						pt.y	-= rc.top;
						CComVariant vResult;
						spFrames->item(&vIndex, &vResult);
						CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
						return spWindow;
					}
				}
			}

			if (frameslength == framelength && spFrameCol.p && spFrames.p) {	// ���ʂ̃t���[��
				for (long i = 0; i < framelength; ++i) {
					CComVariant vIndex(i);
					CComPtr<IDispatch>	spDisp2;
					spFrameCol->item(vIndex, vIndex, &spDisp2);
					CComQIPtr<IHTMLElement>	spFrame = spDisp2;
					if (spFrame.p) {
						CRect rc = funcGetIFrameAbsolutePosition(spFrame);
						//spFrame->get_offsetLeft(&rc.left);
						//spFrame->get_offsetTop(&rc.top);
						//long temp;
						//spFrame->get_offsetWidth(&temp);
						//rc.right += rc.left + temp;
						//spFrame->get_offsetHeight(&temp);
						//rc.bottom+= rc.top + temp;
						if (rc.PtInRect(pt)) {
							pt.x	-= rc.left;
							pt.y	-= rc.top;
							CComVariant vResult;
							spFrames->item(&vIndex, &vResult);
							CComQIPtr<IHTMLWindow2> spWindow = vResult.pdispVal;
							return spWindow;
						}
					}
				}
			}
			return nullptr;
		};	// funcGetHTMLWindowOnCursorPos

		CComQIPtr<IHTMLDocument3>	spDocument3 = spDocument;
		CComPtr<IHTMLWindow2> spWindow = funcGetHTMLWindowOnCursorPos(pt, spDocument3);
		if (spWindow) {
			spDocument.Release();
			CComQIPtr<IHTMLDocument2>	spFrameDocument;
			hr = spWindow->get_document(&spFrameDocument);
			if ( FAILED(hr) ) {	// �ʃh���C��
				CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
				ATLASSERT(spServiceProvider);
				CComPtr<IWebBrowser2>	spBrowser;
				hr = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
				if (!spBrowser)
					AtlThrow(hr);
				CComPtr<IDispatch>	spDisp;
				hr = spBrowser->get_Document(&spDisp);
				if (!spDisp)
					AtlThrow(hr);
				spDocument = spDisp;
			} else {
				spDocument = spFrameDocument;
			}
		}

		CComPtr<IDispatch>	spTargetDisp;
		auto funcGetRangeDisp = [&spTargetDisp](CPoint pt, IHTMLDocument2 *pDocument) {
			CComPtr<IHTMLSelectionObject>		spSelection;
			HRESULT 	hr	= pDocument->get_selection(&spSelection);
			if ( SUCCEEDED(hr) ) {
				CComPtr<IDispatch>				spDisp;
				hr	   = spSelection->createRange(&spDisp);
				if ( SUCCEEDED(hr) ) {
					CComQIPtr<IHTMLTxtRange>	spTxtRange = spDisp;
					if (spTxtRange != NULL) {
						CComBSTR				bstrText;
						hr	   = spTxtRange->get_text(&bstrText);
						if (SUCCEEDED(hr) && !!bstrText) {
							spTargetDisp = spDisp;
						}
					}
				}
			}
		};

		funcGetRangeDisp(pt, spDocument);
		if (spTargetDisp == NULL)
			return false;

		CComQIPtr<IHTMLTextRangeMetrics2>	spMetrics = spTargetDisp;
		ATLASSERT(spMetrics);
		CComPtr<IHTMLRect>	spRect;
		hr = spMetrics->getBoundingClientRect(&spRect);
		if (FAILED(hr))
			AtlThrow(hr);

		CRect rc;
		spRect->get_top(&rc.top);
		spRect->get_left(&rc.left);
		spRect->get_right(&rc.right);
		spRect->get_bottom(&rc.bottom);
		if (rc.PtInRect(pt)) {
			return true;
		}


#if 0
		CComPtr<IHTMLRectCollection>	spRcCollection;
		hr = spMetrics->getClientRects(&spRcCollection);
		if (FAILED(hr))
			AtlThrow(hr);

		long l;
		spRcCollection->get_length(&l);
		for (long i = 0; i < l; ++i) {
			VARIANT vIndex;
			vIndex.vt	= VT_I4;
			vIndex.lVal	= i;
			VARIANT vResult;
			if (spRcCollection->item(&vIndex, &vResult) == S_OK) {
				CComQIPtr<IHTMLRect>	spRect = vResult.pdispVal;
				ATLASSERT(spRect);
				CRect rc;
				spRect->get_top(&rc.top);
				spRect->get_left(&rc.left);
				spRect->get_right(&rc.right);
				spRect->get_bottom(&rc.bottom);
				if (rc.PtInRect(pt)) {
					return true;
				}
			}
		}
#endif

	}
	catch (const CAtlException& e) {
		e;
	}
	return false;
}

/// �g���x�����O������������
void	CChildFrame::Impl::_InitTravelLog()
{
	if (m_bInitTravelLog == false) {
		m_bInitTravelLog = true;
		if (m_TravelLogFore.empty() && m_TravelLogBack.empty())
			return ;
		auto LoadTravelLog = [this](vector<std::pair<CString, CString> >& arrLog, bool bFore) -> BOOL {
			HRESULT 					 hr;
			CComPtr<IEnumTravelLogEntry> pTLEnum;
			CComPtr<ITravelLogEntry>	 pTLEntryBase;
			int		nDir = bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK;

			CComQIPtr<IServiceProvider>	 pISP = m_spBrowser;
			if (pISP == NULL)
				return FALSE;

			CComPtr<ITravelLogStg>		 pTLStg;
			hr	   = pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
			if (FAILED(hr) || pTLStg == NULL)
				return FALSE;

			hr	   = pTLStg->EnumEntries(nDir, &pTLEnum);
			if (FAILED(hr) || pTLEnum == NULL)
				return FALSE;

			int		nLast	= ( 10 > arrLog.size() ) ? (int) arrLog.size() : 10;
			for (int i = 0; i < nLast; i++) {
				CComPtr<ITravelLogEntry> pTLEntry;

			  #if 1	//+++ UNICODE �Ή�.
				std::vector<wchar_t>	title = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].first ) );
				std::vector<wchar_t>	url   = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].second) );
				// CreateEntry �̑�l������TRUE���ƑO�ɒǉ������
				hr			 = pTLStg->CreateEntry(&url[0], &title[0], pTLEntryBase, !bFore, &pTLEntry);
			  #else
				CString 	strTitle	= arrLog[i].first;
				CString 	strURL		= arrLog[i].second;

				LPOLESTR				 pszwURL   = _ConvertString(strURL, strURL.GetLength() + 1);		//new
				LPOLESTR				 pszwTitle = _ConvertString(strTitle, strTitle.GetLength() + 1);	//new

				hr			 = pTLStg->CreateEntry(pszwURL, pszwTitle, pTLEntryBase, !bFore, &pTLEntry);
				delete[] pszwURL;																			//Don't forget!
				delete[] pszwTitle;
			  #endif

				if (FAILED(hr) || pTLEntry == NULL)
					return FALSE;

				if (pTLEntryBase.p)
					pTLEntryBase.Release();

				pTLEntryBase = pTLEntry;
			}

			return TRUE;
		};	// lamda

		LoadTravelLog(m_TravelLogFore, true);
		LoadTravelLog(m_TravelLogBack, false);
		m_TravelLogFore.clear();
		m_TravelLogBack.clear();
	}
}

/// �ŋߕ����^�u�p�̃f�[�^���W�߂�
void	CChildFrame::Impl::_CollectDataOnClose(ChildFrameDataOnClose& data)
{	
	data.strTitle	= GetLocationName();
	data.strTitle.Replace(_T('\"'), _T('_'));
	data.strURL		= GetLocationURL();
	data.dwDLCtrl	= m_view.GetDLControlFlags();
	data.dwExStyle	= m_view.GetExStyle();
	data.dwAutoRefreshStyle	= m_view.GetAutoRefreshStyle();

	HRESULT hr;
	CComQIPtr<IServiceProvider>	 pISP = m_spBrowser;
	if (pISP == NULL)
		return ;

	CComPtr<ITravelLogStg>		 pTLStg;
	hr	= pISP->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void **) &pTLStg);
	if (FAILED(hr) || pTLStg == NULL)
		return ;

	auto GetTravelLog	= [&](vector<std::pair<CString, CString> >& vecLog, bool bFore) {
		DWORD	dwCount = 0;
		hr	= pTLStg->GetCount(bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK, &dwCount);
		if (FAILED(hr) || dwCount == 0)	// �P�ɗ������Ȃ������̏�ԂȂ�true.
			return ;

		CComPtr<IEnumTravelLogEntry> pTLEnum;
		hr	= pTLStg->EnumEntries(bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK, &pTLEnum);
		if (FAILED(hr) || pTLEnum == NULL)
			return ;

		vecLog.reserve(10);

		int 	count = 0;
		for (int j = 0; j < (int)dwCount; ++j) {
			CComPtr<ITravelLogEntry>  pTLEntry	= NULL;
			LPOLESTR				  szURL 	= NULL;
			LPOLESTR				  szTitle	= NULL;
			DWORD	dummy = 0;
			hr = pTLEnum->Next(1, &pTLEntry, &dummy);
			if (pTLEntry == NULL || FAILED(hr))
				break;

			if (   SUCCEEDED( pTLEntry->GetTitle(&szTitle) ) && szTitle
				&& SUCCEEDED( pTLEntry->GetURL  (&szURL  ) ) && szURL  )
			{
				CString strTitle = szTitle;
				strTitle.Replace(_T('\"'), _T('_'));
				vecLog.push_back(std::make_pair<CString, CString>(szTitle, szURL));
				++count;
			}
			if (szTitle) ::CoTaskMemFree( szTitle );
			if (szURL)	 ::CoTaskMemFree( szURL );

			if (count >= 10)
				break;
		}
	};	// lamda

	GetTravelLog(data.TravelLogBack, false);
	GetTravelLog(data.TravelLogFore, true);
}

/// �����摜���T�C�Y
void	CChildFrame::Impl::_AutoImageResize(bool bFirst)
{
	if (CMainOption::s_nAutoImageResizeType == AUTO_IMAGE_RESIZE_NONE)
		return ;

	if (bFirst) {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
		if (spDoc == nullptr)
			return ;
		CComBSTR	strmineType;
		spDoc->get_mimeType(&strmineType);
		if (strmineType == nullptr)
			return ;

		CString strExt = CString(strmineType).Left(3);
		if (strExt != _T("JPG") && strExt != _T("PNG") && strExt != _T("GIF"))
			return ;
	
		m_bImagePage = true;
	}
	if (m_bImagePage == false)
		return ;

	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
	if (spDoc == nullptr)
		return ;

	if (bFirst) {	// �摜�̃T�C�Y�𓾂�
		CComPtr<IHTMLElementCollection> 	pElemClct;
		if ( FAILED( spDoc->get_images(&pElemClct) ) )
			return;
		long	length = 0;
		pElemClct->get_length(&length);

		CComPtr<IDispatch>			pDisp;
		CComVariant 				varName ( 0L );
		CComVariant 				varIndex( 0L );
		if ( FAILED( pElemClct->item(varName, varIndex, &pDisp) ) )
			return;
		CComQIPtr<IHTMLImgElement>	pImg = pDisp;
		if ( pImg == nullptr )
			return;
		
		long	Width; 
		long	Height;
		if ( FAILED( pImg->get_width(&Width) ) )
			return;
		if ( FAILED( pImg->get_height(&Height) ) )
			return;
		m_ImageSize.SetSize((int)Width, (int)Height);
	}

	CComPtr<IHTMLElement>	spHTMLElement;
	spDoc->get_body(&spHTMLElement);
	CComQIPtr<IHTMLElement2>  spHTMLElement2 = spHTMLElement;
	if (!spHTMLElement2)
		return;

	RECT	rcClient;
	GetClientRect(&rcClient);

	enum { kMargen = 10/*32*/ };
	long 	scWidth  = (rcClient.right  - kMargen < 0) ? 1 : rcClient.right  - kMargen;
	long 	scHeight = (rcClient.bottom - kMargen < 0) ? 1 : rcClient.bottom - kMargen;

	double  sclW = scWidth  * 100.0 / m_ImageSize.cx;
	double  sclH = scHeight * 100.0 / m_ImageSize.cy;
	double  scl  = sclW < sclH ? sclW : sclH;
	if (scl < 1) {
		scl = 1;
	} else if (scl >= 100) {
		scl			= 100;
		m_nImgSclSav = 100;
	}
	if (m_nImgSclSw == 0) {
		m_nImgSclSav = int(scl);
		scl			 = 100;
	}
	m_nImgScl = int( scl );
	wchar_t		wbuf[128];
	swprintf_s(wbuf, L"%d%%", int(scl));
	CComVariant variantVal(wbuf);
	CComBSTR	bstrZoom( L"zoom" );

	CComPtr<IHTMLStyle>		pHtmlStyle;
	if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) || !pHtmlStyle )
		return;
	pHtmlStyle->setAttribute(bstrZoom, variantVal);
	if (m_nImgSclSw)
		pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));

}

/// �^�u�Ȃǂ�Favicon��ݒ�
void	CChildFrame::Impl::_SetFavicon(const CString& strURL)
{
	CString strFaviconURL;
	HRESULT hr = S_OK;
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3>	spDocument = spDisp;
	if (spDocument) {
		CComPtr<IHTMLElementCollection>	spCol;
		spDocument->getElementsByTagName(CComBSTR(L"link"), &spCol);
		if (spCol) {
			ForEachHtmlElement(spCol, [&](IDispatch* pDisp) -> bool {
				CComQIPtr<IHTMLLinkElement>	spLink = pDisp;
				if (spLink.p) {
					CComBSTR strrel;
					spLink->get_rel(&strrel);
					CComBSTR strhref;
					spLink->get_href(&strhref);
					strrel.ToLower();
					if (strrel == _T("shortcut icon") || strrel == _T("icon")) {
						DWORD	dwSize = INTERNET_MAX_URL_LENGTH;
						hr = ::UrlCombine(strURL, strhref, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), &dwSize, 0);
						strFaviconURL.ReleaseBuffer();
						if (SUCCEEDED(hr))
							return false;
					}
				}
				return true;
			});
		}
	}
	if (strFaviconURL.IsEmpty()) {	// ���[�g�ɂ���Favicon�̃A�h���X�𓾂�
		DWORD cchResult = INTERNET_MAX_URL_LENGTH;
		if (::CoInternetParseUrl(strURL, PARSE_ROOTDOCUMENT, 0, strFaviconURL.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH, &cchResult, 0) == S_OK) {
			strFaviconURL.ReleaseBuffer();
			strFaviconURL += _T("/favicon.ico");
		}
	}

	m_UIChange.SetFaviconURL(strFaviconURL);
	CFaviconManager::SetFavicon(m_hWnd, strFaviconURL);
}

void	CChildFrame::Impl::_HilightOnce(IDispatch *pDisp, LPCTSTR lpszKeyWord)
{
	CComQIPtr<IWebBrowser2> 	pWebBrowser = pDisp;
	if (pWebBrowser) {
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_SelectEmpt());
		MtlForEachHTMLDocument2g(pWebBrowser, _Function_Hilight2(lpszKeyWord, TRUE));
	}
}


	
BOOL CChildFrame::Impl::_FindKeyWordOne(IHTMLDocument2* pDocument, const CString& strKeyword, BOOL bFindDown, long Flags /*= 0*/)
{
	// �h�L�������g��NULL�Ȃ�I��
	if (!pDocument)
		return FALSE;

	HRESULT	hr = S_OK;
	// �L�[���[�h�����擾
	//x strKeyWord = strtok( (LPSTR) strKeyWord.GetBuffer(0), " " );
	CString 	strKeyWord = strKeyword;//\\Misc::GetStrWord( strKeyword );
	LPCTSTR		strExcept  = _T(" \t\"\r\n�@");
	strKeyWord.TrimLeft(strExcept);
	strKeyWord.TrimRight(strExcept);

	// <body>���擾
	CComPtr<IHTMLElement>		spHTMLElement;
	pDocument->get_body(&spHTMLElement);
	CComQIPtr<IHTMLBodyElement> spHTMLBody = spHTMLElement;
	if (!spHTMLBody)
		return FALSE;

	// �e�L�X�g�����W���擾
	CComPtr<IHTMLTxtRange>		spTxtRange;
	spHTMLBody->createTextRange(&spTxtRange);
	if (!spTxtRange)
		return FALSE;

	if (m_strBookmark && m_strOldKeyword == strKeyword) {
		VARIANT_BOOL vMoveBookmark = VARIANT_FALSE;
		spTxtRange->moveToBookmark(m_strBookmark, &vMoveBookmark);
		if (vMoveBookmark == TRUE) {
			long lActual;
			if (bFindDown) {
				spTxtRange->collapse(false);	// Caret�̈ʒu��I�������e�L�X�g�̈�ԉ���
				if (Misc::getIEMejourVersion() < 9)
					spTxtRange->moveStart(CComBSTR(L"character"), 1, &lActual);
				//spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
			} else {
				spTxtRange->collapse(true);	// Caret�̈ʒu��I�������e�L�X�g�̈�ԏ��
				//if (Misc::getIEMejourVersion() < 9) 
				//	spTxtRange->moveEnd  (CComBSTR(L"character"), -10, &lActual);			
			}
		}
	} else {	// �����͈͂�S�̂ɂ���
		long lActual;
		spTxtRange->moveStart(CComBSTR("Textedit"), -1, &lActual);
		spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
	}
	m_strOldKeyword = strKeyword;

	CComBSTR		bstrText(strKeyWord);
	BOOL			bSts  = FALSE;
	VARIANT_BOOL	vBool = VARIANT_FALSE;
	int	nSearchCount = 0;
	while (spTxtRange->findText(bstrText, (bFindDown) ? 1 : -1, Flags, &vBool), vBool == VARIANT_TRUE) {

		auto funcMove = [&spTxtRange, bFindDown] () {	// �����͈͂�ύX����֐�
			long lActual = 0;
			if (bFindDown)
				spTxtRange->collapse(false);
				//spTxtRange->moveStart(CComBSTR(L"character"), 1, &lActual);
			else
				spTxtRange->collapse(true);
				//spTxtRange->moveEnd  (CComBSTR(L"character"), -10, &lActual);
		};
		
		CComPtr<IHTMLElement>	spFirstParentElement;

		bool	bVisible = true;
		CComQIPtr<IHTMLElement> spParentElement;
		spTxtRange->parentElement(&spParentElement);
		spFirstParentElement = spParentElement;
		while (spParentElement) {
			CComQIPtr<IHTMLElement2>	spParentElm2 = spParentElement;
			CComPtr<IHTMLCurrentStyle>	spStyle;
			spParentElm2->get_currentStyle(&spStyle);
			if (spStyle) {
				CComBSTR	strdisplay;
				spStyle->get_display(&strdisplay);
				if (strdisplay && strdisplay == _T("none")) {	// �\������Ă��Ȃ��ꍇ�̓X�L�b�v
					funcMove();
					bVisible = false;
					break;
				}
			}
			CComPtr<IHTMLElement>	spPPElm;
			spParentElement->get_parentElement(&spPPElm);
			spParentElement = spPPElm;
		}
		if (bVisible == false)
			continue;

		CComBSTR	bstrParentTag;
		spFirstParentElement->get_tagName(&bstrParentTag);
		if (   bstrParentTag != _T("SCRIPT")
			&& bstrParentTag != _T("NOSCRIPT")
			&& bstrParentTag != _T("TEXTAREA")) 
			break;	/* �I��� */

		//++nSearchCount;
		//if (nSearchCount > 5)	// 5�ȏ�őł��~��
		//	break;

		funcMove();
	}

	auto funcScrollBy = [](IHTMLDocument2 *pDoc2) {
		CComPtr<IHTMLDocument3> 	   pDoc3;
		HRESULT 	hr = pDoc2->QueryInterface(&pDoc3);
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLElement>		   pElem;
		hr	= pDoc3->get_documentElement(&pElem);
		if ( FAILED(hr) )
			return;

		long		height = 0;
		hr	= pElem->get_offsetHeight(&height); 	// HTML�\���̈�̍���
		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLSelectionObject>  pSel;
		hr	= pDoc2->get_selection(&pSel);
		if ( FAILED(hr) )
			return;

		CComPtr<IDispatch>			   pDisp;
		hr	= pSel->createRange(&pDisp);

		if ( FAILED(hr) )
			return;

		CComPtr<IHTMLTextRangeMetrics> pTxtRM;
		hr	= pDisp->QueryInterface(&pTxtRM);
		if ( FAILED(hr) )
			return;

		long		y = 0;
		hr	= pTxtRM->get_offsetTop(&y);		// �I�𕔕��̉�ʏォ���y���W
		if ( FAILED(hr) )
			return;

		long scy = y - height / 2;				// ��ʒ����܂ł̋���

		// �������\��������1/4���傫����΃X�N���[��������
		if ( (scy > height / 4) || (scy < -height / 4) ) {
			CComPtr<IHTMLWindow2> pWnd;
			hr = pDoc2->get_parentWindow(&pWnd);

			if ( FAILED(hr) )
				return;

			pWnd->scrollBy(0, scy);
		}
	};

	if (vBool == VARIANT_FALSE) {
		CComPtr<IHTMLSelectionObject> spSelection;
		pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
	} else {
		if (spTxtRange->getBookmark(&m_strBookmark) != S_OK)
			m_strBookmark = LPCOLESTR(NULL);

		spTxtRange->select();
		spTxtRange->scrollIntoView(VARIANT_TRUE);

		bSts = TRUE;

		if (CSearchBarOption::s_bScrollCenter)
			funcScrollBy(pDocument);
	}

	return bSts;
}