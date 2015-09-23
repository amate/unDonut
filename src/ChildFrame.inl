/**
*	@file	ChildFrame.inl
*	@brief	CChildFrame::Impl �̒�`
*/


CChildFrame::Impl::Impl(CChildFrame* pChild) : 
	m_pParentChild(pChild), 
	m_pThreadRefCount(nullptr), 
	m_view(m_UIChange),
	m_BingTranslatorMenu(pChild),
	m_bNowActive(false),
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
	m_pPageBitmap(nullptr),
	m_hMapChildFrameData(NULL),
	m_dwThreadIdFromNewWindow(0),
	m_bFirstNavigate(true),
	m_bWaitNavigateLock(true),
	m_nAutoLoginPrevIndex(-1),
	m_bMClickFail(false)
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
		return true;
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


/// �I�����ꂽ�e�L�X�g����s�����Ԃ�
CString CChildFrame::Impl::GetSelectedTextLine()
{
	CString str = GetSelectedText();
	if (str.GetLength() > 0) {
		str.TrimLeft();
		str.TrimRight();
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

	// mailto: ����
	if (m_view.GetExStyle() & DVS_EX_BLOCK_MAILTO) {
		if (strURL.Left(7).CompareNoCase( _T("mailto:") ) == 0) {
			bCancel = true;
			return;
		}
	}

	// Navigate������javescript����n�܂�E�B���h�E�̓i�r�Q�[�g���Ȃ�
	if (//m_bNowNavigate && // msdn�Ŗ���������
		strURL.Left(15).CompareNoCase(_T("javascript:void(0)")) == 0 || strURL.CompareNoCase(_T("javascript:;")) == 0) 
	{
		bCancel = true;
		return;
	}

	// �|�b�v�A�b�v���L�����Z�������Ƃ��i�r�Q�[�g����Ă��܂��̂�h��
	if (m_bMClickFail) {
		bCancel = true;
		return ;
	}

	// �|�b�v�A�b�v�}�~
	if (m_bFirstNavigate && strURL != _T("about:blank") && m_strNewWindowURL.GetLength() > 0) {
		m_bFirstNavigate = false;
		if (CSupressPopupOption::SearchURLString(strURL)) {
			bCancel = true;
			PostMessage(WM_CLOSE);
			return ;
		}
		if (m_strNewWindowURL == _T("about:blank"))
			m_strNewWindowURL = strURL;
	}

	// ���[�U�[�X�N���v�g���C���X�g�[�����邩�ǂ���
	//if (strURL.Right(8).CompareNoCase(_T(".user.js")) == 0) {
	//	if (CUserDefinedJsOption::UserDefinedScriptInstall(strURL, m_hWnd)) {
	//		bCancel = true;

	//		CString strLocation = GetLocationURL();
	//		if (strLocation.IsEmpty())
	//			PostMessage(WM_CLOSE);
	//		return ;
	//	}
	//}

	if (bTopWindow) {
		if (m_dwMarshalDLCtrlFlags) {
			m_view.PutDLControlFlags(m_dwMarshalDLCtrlFlags);
			m_dwMarshalDLCtrlFlags = 0;
		}
		// Navigate lock
		if ( _check_flag(DVS_EX_OPENNEWWIN, m_view.GetExStyle()) &&
			 m_bWaitNavigateLock == false &&
		     IsRefreshBeforeNavigate2(pDisp) == false )
		{
			NewChildFrameData	data(GetParent());
			data.strURL	= strURL;
			DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
			DWORD	dwExStyle	= _GetInheritedExStyleFlags();
			if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
				dwDLCtrl	= m_pGlobalConfig->dwDLControlFlags;
				dwExStyle	= m_pGlobalConfig->dwExtendedStyleFlags;
			}
			data.dwDLCtrl	= dwDLCtrl;
			data.dwExStyle	= dwExStyle;
			data.bLink	= true;
			data.searchWord	= m_strSearchWord;
			data.bAutoHilight = m_bAutoHilight;
			data.dwThreadIdFromNewWindow = ::GetCurrentThreadId();
			data.strNewWindowURL	= strURL;

			CChildFrame::AsyncCreate(data);

			bCancel = true;
			return ;
		}

		m_view.PutDLControlFlags(m_view.GetDLControlFlags());

		if (m_view.m_bLightRefresh) {
			m_view.m_bLightRefresh = false;	// �蓮�ŃZ�L�����e�B��ݒ肵���̂ŉ������Ȃ�
		} else	{	
			// URL�ʃZ�L�����e�B�̐ݒ�
			if (m_pGlobalConfig->bUrlSecurityValid) {
				DWORD exopts	= 0xFFFFFFFF;
				DWORD dlCtlFlg	= 0xFFFFFFFF;
				DWORD exstyle	= 0xFFFFFFFF;
				DWORD autoRefresh = 0xFFFFFFFF;
				DWORD dwExPropOpt = 8;
				CString nowLocation = GetLocationURL();
				if (CUrlSecurityOption::IsUndoSecurity(nowLocation)) {
					m_view.PutDLControlFlags(m_pGlobalConfig->dwDLControlFlags);
					SetExStyle(m_pGlobalConfig->dwExtendedStyleFlags);
				}
				if (CUrlSecurityOption::FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
					CExProperty  ExProp(m_pGlobalConfig->dwDLControlFlags, m_pGlobalConfig->dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
					dlCtlFlg	= ExProp.GetDLControlFlags();
					exstyle		= ExProp.GetExtendedStyleFlags();
					autoRefresh = ExProp.GetAutoRefreshFlag();
				}

				//+++ url�ʊg���v���p�e�B�̏���....
				//+++	�߂�E�i�ނł̊g���v���p�e�B���̕ł��Ƃ̕ۑ����ł��Ă��Ȃ��̂ŁA�j�]����...
				//+++	���A�o�O���Ă����f����邱�Ƃ̂ق����Ӗ����肻���Ȃ̂ŗ��p
				if (exopts != 0xFFFFFFFF) {
					m_view.PutDLControlFlags( dlCtlFlg );
					m_view.SetAutoRefreshStyle( autoRefresh );
					SetExStyle( exstyle );	//+++����: �}�E�X���{�^���N���b�N�ł́A�����N�ʃ^�u�\���̏ꍇ�A�܂��^�u�ʒu������̂��ߐݒ�ł��Ȃ�...
				}
			}
		}

		/* Favicon�𔒎��ɐݒ� */
		if (strURL.Left(11).CompareNoCase(_T("javascript:")) != 0) {
			m_UIChange.SetLocationURL(strURL);
			_SetFavicon(strURL);
		}

		m_strLastScriptErrorMessage.Empty();

	}
	
	m_bNowNavigate = true;	// Navigate���ł���
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

	if (CSupressPopupOption::SearchTitleString(strTitle))
		PostMessage(WM_CLOSE);
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
	if (hr == S_OK && (state == READYSTATE_COMPLETE || state == READYSTATE_INTERACTIVE))
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
	if (strURL.IsEmpty()) {
		READYSTATE	state;
		HRESULT hr = m_spBrowser->get_ReadyState(&state);
		OnStateCompleted();
	}

	if ( IsPageIWebBrowser(pDisp) || strURL.IsEmpty() ) {

		m_bWaitNavigateLock = false;

		// �������T�C�Y�̐ݒ��������
		m_bImagePage	= false;
		m_nImgSclSw		= (m_pGlobalConfig->AutoImageResizeType == AUTO_IMAGE_RESIZE_FIRSTON);
		m_ImageSize.SetSize(0, 0);

		/* �y�[�W�������̏������Z�b�g */
		m_SearchBarHilightData.Clear();
		m_FindBarHilightData.Clear();

		_InitTravelLog();	// �g���x�����O��ݒ�

		_AutoImageResize(true);	// �摜�̎������T�C�Y

		if (m_bNowActive && MtlIsApplicationActive(m_hWnd))
			_SetFocusToHTML();

		_SetFavicon(strURL);

		/* ���[�U�[��`Javascript */
		//if (auto value = CUserDefinedJsOption::FindURL(strURL)) {
		//	for (auto it = value->cbegin(); it != value->cend(); ++it) {
		//		CComPtr<IDispatch>	spDisp;
		//		m_spBrowser->get_Document(&spDisp);
		//		CComQIPtr<IHTMLDocument2> spDoc = spDisp;
		//		if (spDoc) {
		//			CComPtr<IHTMLElement>	spScriptElm;
		//			spDoc->createElement(CComBSTR(L"script"), &spScriptElm);
		//			if (spScriptElm == nullptr)
		//				goto ADDJSFINISH;
 
		//			CComQIPtr<IHTMLScriptElement>	spScript = spScriptElm;
		//			spScript->put_type(CComBSTR(L"text/javascript"));
		//			spScript->put_text(*(*it));

		//			CComPtr<IHTMLElement>	spBodyElm;
		//			spDoc->get_body(&spBodyElm);
		//			CComQIPtr<IHTMLDOMNode>	spBodyNode = spBodyElm;
		//			if (spBodyNode == nullptr)
		//				goto ADDJSFINISH;
		//			CComQIPtr<IHTMLDOMNode>	spScriptNode = spScript;
		//			CComPtr<IHTMLDOMNode>	sptempNode;
		//			spBodyNode->appendChild(spScriptNode, &sptempNode);
		//		}
		//		
		//	}
		//}
		//ADDJSFINISH:
		/* ���[�U�[��`CSS */
		//if (auto value = CUserDefinedCSSOption::FindURL(strURL)) {
		//	CComBSTR strCssPath = value.get();
		//	MtlForEachHTMLDocument2g(m_spBrowser, [strCssPath](IHTMLDocument2* pDoc) {
		//		CComPtr<IHTMLStyleSheet>	spStyleSheet;
		//		pDoc->createStyleSheet(strCssPath, -1, &spStyleSheet);
		//	});
		//}

		PostMessage(WM_DELAYDOCUMENTCOMPLETE);
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
#if 0
	m_bExecutedNewWindow = true;

	CChildFrame*	pChild = new CChildFrame;
	pChild->pImpl->SetThreadRefCount(m_pThreadRefCount);
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (m_UrlSecurity.IsUndoSecurity(GetLocationURL())) {	// ����URL�ł̂ݗL���Ȃ�߂�
		dwDLCtrl	= m_pGlobalConfig->dwDLControlFlags;
		dwExStyle	= m_pGlobalConfig->dwExtendedStyleFlags;
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
	if (CSupressPopupOption::SearchURLString(bstrUrl)) {
		bCancel = true;
		CString statusText;
		statusText.Format(_T("�|�b�v�A�b�v���u���b�N���܂����B: %s"), bstrUrl);
		m_UIChange.SetTitle(statusText);
		SetTimer(kReturnTitleTimerId, kReturnTitleInterval);
		return ;
	}

	/// window.open�ȃ����N���~�h���N���b�N�ŊJ�����Ƃ��A�󔒂ȃy�[�W���J���Ȃ��悤�ɂ���
	if (CString(bstrUrl).Left(11).CompareNoCase(_T("javascript:")) == 0) {
		bCancel = true;
		return ;
	}

	//bCancel	= true;
	m_bExecutedNewWindow = true;

	NewChildFrameData	data(GetParent());
	data.strURL	= _T("");
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (CUrlSecurityOption::IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= m_pGlobalConfig->dwDLControlFlags;
		dwExStyle	= m_pGlobalConfig->dwExtendedStyleFlags;
	}
	data.dwDLCtrl	= dwDLCtrl;
	data.dwExStyle	= dwExStyle;
	data.bLink	= true;
	data.searchWord	= m_strSearchWord;
	data.bAutoHilight = m_bAutoHilight;
	data.dwThreadIdFromNewWindow = ::GetCurrentThreadId();
	data.strNewWindowURL	= bstrUrl;

#if 1
	class CThreadObserver : public CMessageFilter
	{
	public:
		CThreadObserver(IDispatch** ppDisp, NewChildFrameData& data, bool bMode) 
			: m_ppDisp(ppDisp)
			, m_data(data)
			, m_bMultiProcessMode(bMode)
			, m_bGetMarshalIDispatchInterface(false)
		{	}

		virtual BOOL PreTranslateMessage(MSG* pMsg)
		{
			switch (pMsg->message) {
			case WM_EXECUTECHILDFRAMETHREADFROMNEWWINDOW2:
				{
					MultiThreadManager::CreateChildFrameThread(m_data, m_bMultiProcessMode);
					return TRUE;
				}

			case WM_GETMARSHALIDISPATCHINTERFACE:
				{
					IStream* pStream = (IStream*)pMsg->wParam;
					IDispatch* pDisp;
					ATLVERIFY(::CoGetInterfaceAndReleaseStream(pStream, IID_IDispatch, (void**)&pDisp) == S_OK);
					*m_ppDisp = pDisp;
					m_bGetMarshalIDispatchInterface = true;
					return TRUE;
				}				
			}
			return FALSE;
		}
	private:
		IDispatch**	m_ppDisp;
		NewChildFrameData& m_data;
		bool	m_bMultiProcessMode;
	public:
		bool	m_bGetMarshalIDispatchInterface;
	};

	CThreadObserver threadObserver(ppDisp, data, m_pGlobalConfig->bMultiProcessMode);
	
	PostThreadMessage(::GetCurrentThreadId(), WM_EXECUTECHILDFRAMETHREADFROMNEWWINDOW2, 0, 0);

	using namespace std::chrono;
	auto startTime = steady_clock::now();
	while (duration_cast<seconds>(steady_clock::now() - startTime) < seconds(30)) {
		MSG msg = {};
		BOOL bRet = ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (bRet) {
			//bRet = ::GetMessage(&msg, NULL, 0, 0);
			//if(bRet == -1) {
			//	ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
			//	continue;   // error, don't process
			//} else if(!bRet) {
			//	ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
			//	break;   // WM_QUIT, exit message loop
			//}
			
			if(!threadObserver.PreTranslateMessage(&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			} else {
				if (threadObserver.m_bGetMarshalIDispatchInterface)
					break;	// �|�C���^���󂯎�����̂Ń��[�v�𔲂���
			}
		} else {
			::Sleep(10);
		}
	}

#endif
#if 0
	{
		// ChildFrame�E�B���h�E�쐬
		CChildFrame* pChild = new CChildFrame;
		CWindow wnd = pChild->CreateChildFrame(data, m_pThreadRefCount);

		pChild->pImpl->m_spBrowser->get_Application(ppDisp);
		ATLASSERT( ppDisp && *ppDisp );

		DWORD dwOption = 0;
		if (data.bActive)
			dwOption |= TAB_ACTIVE;
		if (data.bLink)
			dwOption |= TAB_LINK;
		CWindow wndMainFrame = wnd.GetTopLevelWindow();
		wndMainFrame.SendMessage(WM_TABCREATE, (WPARAM)wnd.m_hWnd, (LPARAM)dwOption);
	}
#endif
}

void	CChildFrame::Impl::OnWindowClosing(bool IsChildWindow, bool& bCancel)
{
	bCancel = true;
	PostMessage(WM_CLOSE);
}


BOOL	CChildFrame::Impl::OnMButtonHook(MSG* pMsg)
{
	CIniFileI	 pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
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
	m_bMClickFail = true;
	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ �����N�ȊO���N���b�N�����ꍇ���܂��Ȃ��̍��N���b�N�������I�����Ă���.
	m_bMClickFail = false;
	return FALSE;																//+++ false��Ԃ����ƂŁAIE�R���|�[�l���g�ɃE�B�[���N���b�N�̏�����C����.
}


BOOL CChildFrame::Impl::OnRButtonHook(MSG* pMsg)
{
	if ( !(m_view.GetExStyle() & DVS_EX_MOUSE_GESTURE) )
		return FALSE;

	HWND hWndFocus = ::GetFocus();
	if (hWndFocus != m_hWnd && IsChild(hWndFocus) == FALSE)
		return FALSE;

	MouseGestureData	data;
	data.hwnd	= pMsg->hwnd;
	data.wParam	= pMsg->wParam;
	data.lParam	= pMsg->lParam;
	data.bCursorOnSelectedText	= m_pGlobalConfig->bUseRightDragSearch && _CursorOnSelectedText();
	data.strSelectedTextLine	= GetSelectedTextLine();

	CString strSharedMemName;
	strSharedMemName.Format(_T("%s0x%x"), MOUSEGESTUREDATASHAREDMEMNAME, m_hWnd);
	CSharedMemoryHandle	sharedMem;
	sharedMem.Serialize(data, strSharedMemName);

	GetTopLevelWindow().SetCapture();
	GetTopLevelWindow().PostMessage(WM_MOUSEGESTURE, (WPARAM)m_hWnd, (LPARAM)sharedMem.Handle());
	return TRUE;
#if 0
	SetCapture();

	CPoint	ptDown(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
	::ClientToScreen(pMsg->hwnd, &ptDown);
	CPoint	ptLast = ptDown;

	bool bCursorOnSelectedText
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
				if (m_pGlobalConfig->bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// �p�x�����߂�
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine =m_pGlobalConfig->strREngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = m_pGlobalConfig->strTEngine;	
						strMark = _T("[��] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = m_pGlobalConfig->strLEngine;	
						strMark = _T("[��] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = m_pGlobalConfig->strBEngine;
						strMark = _T("[��] ");
					}
				} else {
					strSearchEngine = m_pGlobalConfig->strCEngine;
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
					CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
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
			_SearchWebWithEngine(str, strSearchEngine);
			bNoting = false;
		}
	}

	/* �}�E�X�W�F�X�`���[�R�}���h���s */
	if (bNoting) {
		ptLast.SetPoint(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		::ClientToScreen(msg.hwnd, &ptLast);

		m_UIChange.SetStatusText(_T(""));

		CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
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
#endif
}

BOOL CChildFrame::Impl::OnXButtonUp(WORD wKeys, WORD wButton)
{
	CString 	strKey;
	switch (wButton) {
	case XBUTTON1: strKey = _T("Side1"); break;
	case XBUTTON2: strKey = _T("Side2"); break;
	}

	CIniFileI pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
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
	if (m_bNowActive == false) {
		return FALSE;
	}
#if 0	// http://digital-mixnews.com/archives/67603987.html �Ōł܂�o�O�C�������ǂ������youtube�ŃT���l�C���ǂݍ��܂Ȃ��Ȃ�...
	if (pMsg->message == (WM_APP + 3)) {
		WCHAR classname[32] = L"";
		::GetClassName(pMsg->hwnd, classname, 32);
		if (::lstrcmp(classname, L"Internet Explorer_Hidden") == 0)
			return TRUE;
	}
#endif

	// �������T�C�Y�̃g�O��
	if (   m_bImagePage
		&& pMsg->message == WM_LBUTTONDOWN
		&& m_pGlobalConfig->AutoImageResizeType != AUTO_IMAGE_RESIZE_NONE)
	{
		CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		CRect	rc;
		GetClientRect(&rc);
		if (m_nImgSclSw == 0) {
			rc.right -= GetSystemMetrics(SM_CXVSCROLL);
			rc.bottom-= GetSystemMetrics(SM_CYHSCROLL);
		}
		// �X�N���[���o�[���N���b�N�����Ƃ��͉������Ȃ�
		if (m_nImgScl == 100 && rc.PtInRect(pt) == false)
			return FALSE;

		CPoint ptCur;
		GetCursorPos(&ptCur);
		if (::DragDetect(m_hWnd, ptCur)) {
			if (m_nImgScl == 100) {
				/// �����摜���h���b�O�ŃX�N���[��
				SetCapture();
				CPoint ptFirst(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
				CComPtr<IDispatch> spDisp;
				m_spBrowser->get_Document(&spDisp);
				CComQIPtr<IHTMLDocument2> spDoc(spDisp);
				CComPtr<IHTMLWindow2> spWindow;
				spDoc->get_parentWindow(&spWindow);

				MSG msg = { 0 };
				for (;;) {
					BOOL nRet = GetMessage(&msg, NULL, 0, 0);
					if (nRet == 0 || nRet == -1 || GetCapture() != m_hWnd)
						break;

					if (msg.message == WM_MOUSEMOVE) {
						CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
						int xDiff = ptFirst.x - ptNow.x;
						int yDiff = ptFirst.y - ptNow.y;
						ptFirst = ptNow;
						spWindow->scrollBy(xDiff, yDiff);
					} else if (msg.message == WM_LBUTTONUP) {
						break;
					}
					::DispatchMessage(&msg);
				}
				ReleaseCapture();
				return TRUE;
			} else {
				return FALSE;
			}
		} else {
			// �g��/�����̃g�O���ؑ�
			if (rc.PtInRect(pt))
				OnHtmlZoom(0, ID_HTMLZOOM_100TOGLE, (HWND)true);
			return TRUE;
		}
	}

	// �~�h���N���b�N
	if ( pMsg->message == WM_MBUTTONDOWN && IsChild(pMsg->hwnd) && OnMButtonHook(pMsg) )
		return TRUE;

	// �E�h���b�O�L�����Z���p
	//if ( pMsg->message == WM_RBUTTONUP && m_bCancelRButtonUp ) {
	//	m_bCancelRButtonUp = false;
	//	return TRUE;
	//}
	// �}�E�X�W�F�X�`���[��
	if ( pMsg->message == WM_RBUTTONDOWN && OnRButtonHook(pMsg) )
		return TRUE;

	// �T�C�h�{�^��
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam)) )
			return TRUE;
	}

	// ���C���t���[���̃}�E�X�W�F�X�`���[�p�Ƀz�C�[����ʒm����
	if (pMsg->message == WM_MOUSEWHEEL) {
		CWindow wndMainFrame = GetTopLevelWindow();
		if (GetCapture() == wndMainFrame) {
			wndMainFrame.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		} else {
			UINT nFlags = (UINT)LOWORD(pMsg->wParam);
			int	 zDelta = (short)HIWORD(pMsg->wParam);
			if (nFlags == MK_CONTROL) {		// �����T�C�Y�ύX
				CComVariant	vEmpty;
				CComVariant vZoomSize;
				m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vEmpty, &vZoomSize); 
				if ( zDelta > 0 ){	
					vZoomSize.lVal += 1;
				} else {
					vZoomSize.lVal -= 1;
				}
				m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoomSize, &vEmpty);
			} else if (::GetKeyState(VK_MENU) < 0) {	// �g��/�k��
				if ( zDelta > 0 ){	
					OnHtmlZoom(0, ID_HTMLZOOM_ADD, NULL);
				} else {
					OnHtmlZoom(0, ID_HTMLZOOM_SUB, NULL);
				}
			} else {	// �J�[�\�����̎����̃E�B���h�E�Ƀz�C�[���X�N���[����ʒm����
				CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
				HWND hWndFound = WindowFromPoint(pt);
				if (hWndFound) {
					if (IsChild(hWndFound)) {
						::SendMessage(hWndFound, pMsg->message, pMsg->wParam, pMsg->lParam);
					} else if (wndMainFrame.m_hWnd == hWndFound || wndMainFrame.IsChild(hWndFound)) {
						// �z�C�[���Ń^�u�ؑ֗p
						wndMainFrame.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
					}
				}
			}
			return TRUE;
		}
	}

	// Ctrl + Tab
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB) && ::GetAsyncKeyState(VK_CONTROL) < 0) {
		GetTopLevelWindow().PostMessage(WM_COMMAND, ::GetAsyncKeyState(VK_SHIFT) < 0 ? ID_TAB_LEFT : ID_TAB_RIGHT);
		return TRUE;
	}

	// �����N��Alt + Click
	if (pMsg->message == WM_LBUTTONDOWN && ::GetAsyncKeyState(VK_MENU) < 0 && _ShowLinkTextSelectWindow(pMsg)) {
		return TRUE;
	}

#if 0
	// Alt�L�[
	if ((pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP) && pMsg->wParam == VK_MENU) {
		GetTopLevelWindow().PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}
#endif

	// �A�N�Z�����[�^�L�[
	if (CAcceleratorOption::TranslateAccelerator(m_hWnd, pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

/// �I�𒆂̕����������(�A�h���X�o�[��Ctrl+Enter�����Ƃ��̌����G���W�����g����)
void CChildFrame::Impl::searchEngines(const CString& strKeyWord)
{
	CString 	strSearchWord = strKeyWord;

	if (m_pGlobalConfig->bReplaceSpace)
		strSearchWord.Replace( _T("�@"), _T(" ") );

	//_ReplaceCRLF(strSearchWord,CString(_T(" ")));
	//strSearchWord.Replace('\n',' ');
	//strSearchWord.Remove('\r');
	strSearchWord.Replace( _T("\r\n"), _T("") );

	CIniFileI	pr( g_szIniFileName, _T("AddressBar") );
	CString 		strEngine = pr.GetStringUW( _T("EnterCtrlEngin"), NULL, 256 );
	_SearchWebWithEngine(strSearchWord, strEngine);
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

// Message map


int		CChildFrame::Impl::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	DefWindowProc();

	++(*m_pThreadRefCount);

	m_UIChange.SetChildFrame(m_hWnd);
	m_UIChange.AddCommandUIMap();

	// �O���[�o���ݒ�f�[�^���擾����
	CWindow wndMainFrame = GetTopLevelWindow();
	m_GlobalConfigManageData = GetGlobalConfig(wndMainFrame);
	m_pGlobalConfig	= m_GlobalConfigManageData.pGlobalConfig;
	m_view.SetGlobalConfig(m_pGlobalConfig);
	OnSetProxyToChildFrame(0, 0, 0);	// �v���N�V�ݒ�

	if (m_pGlobalConfig->bMultiProcessMode) {
		
		// ���[�U�[�G�[�W�F���g��ݒ�
		std::vector<char>	userAgent;
		if (m_pGlobalConfig->bChangeUserAgent) {
			userAgent = Misc::wcs_to_sjis( m_pGlobalConfig->strUserAgent );
		} else {
			userAgent = Misc::wcs_to_sjis( m_pGlobalConfig->strUserAgentCurrent );
		}
		::UrlMkSetSessionOption(URLMON_OPTION_USERAGENT, (void*)userAgent.data(), (int)userAgent.size(), 0);
	}

	RECT rc;
	GetClientRect(&rc);
	m_view.Create(m_hWnd, rc, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	ATLASSERT( m_view.IsWindow() );
	m_view.SetDlgCtrlID(ID_DONUTVIEW);

	m_view.QueryControl(IID_IWebBrowser2, (void **)&m_spBrowser);

	WebBrowserEvents2Advise();

	if (m_dwThreadIdFromNewWindow) {
		IDispatch* pDisp;
		ATLVERIFY(m_spBrowser->get_Application(&pDisp) == S_OK);
		IStream* pStream;
		ATLVERIFY(::CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pDisp, &pStream) == S_OK);
		::PostThreadMessage(m_dwThreadIdFromNewWindow, WM_GETMARSHALIDISPATCHINTERFACE, (WPARAM)pStream, 0);
	}
	if (m_pGlobalConfig->dwMainExtendedStyle & MAIN_EX_REGISTER_AS_BROWSER)
		SetRegisterAsBrowser(true);
	SetVisible(true);

	CString strKeyMessageSharedMemName;
	strKeyMessageSharedMemName.Format(_T("DonutKeyMessageSharedMemName%#x"), wndMainFrame.m_hWnd);
	m_sharedMemKeyMessage.OpenSharedMemory(strKeyMessageSharedMemName, true);

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

void	CChildFrame::Impl::OnDestroy()
{
	m_bClosing = true;
	HRESULT hr = m_spBrowser->Stop();

	m_sharedMemKeyMessage.CloseHandle();

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);

	SetRegisterAsBrowser(false);

	WebBrowserEvents2Unadvise();

	m_view.DestroyWindow();

	--(*m_pThreadRefCount);
	if (*m_pThreadRefCount == 0) {
		TRACEIN(_T("ChildFreame�X���b�h�̔j��"));
		PostQuitMessage(0);
	}

	m_UIChange.RemoveCommandUIMap();
	CloseGlobalConfig(&m_GlobalConfigManageData);

	m_spBrowser.Release();
}

void	CChildFrame::Impl::OnClose()
{
	// �^�u���b�N��ԂȂ���Ȃ�
	if ( m_pGlobalConfig->bMainFrameClosing == false &&							//+++ unDonut�I�����ȊO��
		 _check_flag(m_view.GetExStyle(), DVS_EX_OPENNEWWIN) &&					//+++ �i�r�Q�[�g���b�N�̃y�[�W��
		 (m_pGlobalConfig->dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK) )	//+++ �i�r�Q�[�g���b�N�̃y�[�W����Ȃ��A�̎w�肪�����
	{
		return; 																//+++ �����ɋA��
	}

	SetMsgHandled(FALSE);

	CWindow	wndMain = GetTopLevelWindow();

	// �ŋߕ����^�u�ɒǉ�����
	if (m_pGlobalConfig->bMainFrameClosing == false) {

		ChildFrameDataOnClose	ClosedTabData;
		_CollectDataOnClose(ClosedTabData);

		CString strSharedMemName;
		strSharedMemName.Format(_T("%s%#x"), CHILDFRAMEDATAONCLOSESHAREDMEMNAME, m_hWnd);
		CSharedMemory sharedMem;
		sharedMem.Serialize(ClosedTabData, strSharedMemName);

		wndMain.SendMessage(WM_ADDRECENTCLOSEDTAB, (WPARAM)m_hWnd);
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

BOOL	CChildFrame::Impl::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	switch (pCopyDataStruct->dwData) {
	case kHilightText:
		PostMessage(WM_DELAYHILIGHT, (WPARAM)new CString(static_cast<LPCTSTR>(pCopyDataStruct->lpData)));
		//OnHilight(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
		break;

	case kNavigateChildFrame:
		{
			NavigateChildFrame	data;
			std::wstringstream	ss;
			ss << static_cast<LPWSTR>(pCopyDataStruct->lpData);
			boost::archive::text_wiarchive	ar(ss);
			ar >> data;
		
			if (data.dwDLCtrl == -1) {
				data.dwDLCtrl = _GetInheritedDLCtrlFlags();
			}
			SetDLCtrl(data.dwDLCtrl);

			if (data.dwExStyle == -1) {
				data.dwExStyle = _GetInheritedExStyleFlags();
			}
			SetExStyle(data.dwExStyle);

			if (data.dwAutoRefresh) {
				SetAutoRefreshStyle(data.dwAutoRefresh);
			}
			Navigate2(data.strURL);
		}
		break;

	case kExecuteUserJavascript:
		{
			CString* pScript = new CString(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
			PostMessage(WM_EXECUTE_JAVASCRIPT, (WPARAM)pScript);
			//_ExecuteUserJavascript();
		}
		break;

	case kHilightFromFindBar:
		{
			PostMessage(WM_DELAYHILIGHT, (WPARAM)new CString(static_cast<LPCTSTR>(pCopyDataStruct->lpData)), (LPARAM)wnd.m_hWnd);
			//CString strData = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
			//CString strdw = strData[0];
			//CString strFlags = strData[1];
			//DWORD dw = _wtoi(strdw);
			//long flags = _wtoi(strFlags);
			//return _HilightFromFindBar(strData.Mid(2), (dw & 0x1) != 0, (dw & 0x2) != 0, flags);
		}
		break;

	case kSetSearchText:
		m_strSearchWord = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
		break;

	default:
		SetMsgHandled(FALSE);
	}
	return 0;
}


void	CChildFrame::Impl::OnSetFocus(CWindow wndOld)
{
	if (m_spBrowser == nullptr)
		return ;

	CComPtr<IDispatch>	spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	if ( FAILED(hr) )
		return;

	CComQIPtr<IHTMLDocument2> spDoc = spDisp;
	if (!spDoc) 								// document not initialized yet
		return;

	CComPtr<IHTMLWindow2>	spWnd;
	hr	= spDoc->get_parentWindow(&spWnd);
	if (!spWnd)
		return;

	//m_bPageFocusInitialized = true; 			// avoid the endless loop
	hr = spWnd->focus();	// makes mainframe active
	TRACEIN(L"ChildFrame::OnSetFocus : hr(%s)", GetLastErrorString(hr));
}

void	CChildFrame::Impl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kReturnTitleTimerId) {
		KillTimer(kReturnTitleTimerId);
		m_UIChange.SetTitle(MtlGetWindowText(m_hWnd));

	} else if (nIDEvent == kDelayHilightTimerId) {
		KillTimer(kDelayHilightTimerId);

		/* �y�[�W���n�C���C�g */
		bool bHilight = m_bAutoHilight || m_pGlobalConfig->bHilightSwitch;
		if (bHilight && m_strSearchWord.IsEmpty() == FALSE) {
			if (m_bNowActive) {
				COPYDATASTRUCT	cds;
				cds.dwData	= kSetSearchText;
				CString str;
				str.Format(_T("%d%s"), bHilight, m_strSearchWord);
				cds.lpData	= (LPVOID)str.GetBuffer(0);
				cds.cbData	= (str.GetLength() + 1) * sizeof(WCHAR);
				GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
			}
			CString strWords = m_strSearchWord;
			_DeleteMinimumHilightTextLengthWord(strWords);
			m_bAutoHilight = true;
			_HilightText(_T(""), false);
			_HilightText(strWords, true);
		}
	}
}

LRESULT CChildFrame::Impl::OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_view.SendMessage(WM_FORWARDMSG, 0, (LPARAM)m_sharedMemKeyMessage.GetPointer());
}

CString	CChildFrame::Impl::_SearchBarHelper(const CString& URL)
{
	if (auto optName = CSearchBarHelper::FindName(URL)) {
		CComPtr<IDispatch> spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDoc2 = spDisp;
		if (spDoc2 == nullptr)
			return CString();

		const CString& targetName = *optName;
		CString value;

		CComPtr<IHTMLElementCollection>	spCol;
		spDoc2->get_forms(&spCol);
		ForEachHtmlElement(spCol, [&targetName, &value](IDispatch* pDisp) -> bool {
			CComQIPtr<IHTMLFormElement>	spForm = pDisp;
			ForEachHtmlElement(spForm, [&targetName, &value](IDispatch* pDisp) -> bool {
				CComQIPtr<IHTMLInputElement>	spInput = pDisp;
				if (spInput == nullptr)
					return true;
				CComBSTR inputName;
				spInput->get_name(&inputName);
				if (inputName == nullptr || inputName != (LPCTSTR)targetName)
					return true;
				CComBSTR strValue;
				spInput->get_value(&strValue);
				if (strValue == nullptr)
					return true;
				value = strValue;
				return false;
			});
			if (value.GetLength() > 0)
				return false;
			else
				return true;
		});
		return value;
	}
	return CString();
}

void	CChildFrame::Impl::_DeleteMinimumHilightTextLengthWord(CString& strWord)
{
	if (1 < m_pGlobalConfig->nMimimulHilightTextLength && strWord.IsEmpty() == FALSE) {
		std::vector<CString>	strSearchWords;
		Misc::SeptTextToWords(strSearchWords, strWord);
		strWord = _T("");
		std::vector<CString>::iterator it = strSearchWords.begin();
		for (; it != strSearchWords.end(); ++it) {
			if (m_pGlobalConfig->nMimimulHilightTextLength <= it->GetLength()) {
				strWord += *it;
				strWord += _T(" ");
			}
		}
	}
}

LRESULT CChildFrame::Impl::OnDelayDocumentComplete(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CString strURL = GetLocationURL();
	strURL.MakeLower();

	CString formValue = _SearchBarHelper(strURL);
	if (formValue.GetLength() > 0) {
		COPYDATASTRUCT	cds;
		cds.dwData = kAddToSearchBoxUnique;
		cds.lpData = (LPVOID)formValue.GetBuffer(0);
		cds.cbData = (formValue.GetLength() + 1) * sizeof(WCHAR);
		GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

		m_strSearchWord = formValue;

		if (m_bNowActive) {
			COPYDATASTRUCT	cds;
			cds.dwData = kSetSearchText;
			CString str;
			str.Format(_T("%d%s"), m_bAutoHilight, m_strSearchWord);
			cds.lpData = (LPVOID)str.GetBuffer(0);
			cds.cbData = (str.GetLength() + 1) * sizeof(WCHAR);
			GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		}		
	}

	if (strURL.Left(30) == L"http://www.google.co.jp/search" || strURL.Left(31) == L"https://www.google.co.jp/search" ) {
		SetTimer(kDelayHilightTimerId, kDelayHilightInterval);
	} else {
		/* �y�[�W���n�C���C�g */
		bool bHilight = m_bAutoHilight || m_pGlobalConfig->bHilightSwitch;
		if (bHilight && m_strSearchWord.IsEmpty() == FALSE) {
			if (m_bNowActive) {
				COPYDATASTRUCT	cds;
				cds.dwData	= kSetSearchText;
				CString str;
				str.Format(_T("%d%s"), bHilight, m_strSearchWord);
				cds.lpData	= (LPVOID)str.GetBuffer(0);
				cds.cbData	= (str.GetLength() + 1) * sizeof(WCHAR);
				GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
			}
			CString strWords = m_strSearchWord;
			_DeleteMinimumHilightTextLengthWord(strWords);
			m_bAutoHilight = true;
			_HilightText(strWords, true);
		}
	}

	/* �������O�C�� */
	if (m_nAutoLoginPrevIndex == -1) {
		int nIndex = CLoginDataManager::Find(strURL);
		if (nIndex != -1) {
			bool bSuccess = CLoginDataManager::DoAutoLogin(nIndex, m_spBrowser);
			if (bSuccess)
				m_nAutoLoginPrevIndex = nIndex;
		}
	} else {
		m_nAutoLoginPrevIndex = -1;
	}

	return 0;
}

LRESULT CChildFrame::Impl::OnDelayHilight(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CString* pStrHilight = (CString*)wParam;
	ATLASSERT( pStrHilight );
	if (lParam) {
		CString strData = *pStrHilight;
		CString strdw = strData[0];
		CString strFlags = strData[1];
		DWORD dw = _wtoi(strdw);
		long flags = _wtoi(strFlags);
		int nMatchCount =  _HilightFromFindBar(strData.Mid(2), (dw & 0x1) != 0, (dw & 0x2) != 0, flags);
		::PostMessage((HWND)lParam, WM_RETURNMATCHCOUNT, (WPARAM)m_hWnd, nMatchCount);
	} else {
		OnHilight(*pStrHilight);
	}
	delete pStrHilight;
	return 0;
}

LRESULT CChildFrame::Impl::OnGetChildFrameActive(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bNowActive) {
		if (MtlIsApplicationActive(m_hWnd))
			return TRUE;
#if 0
		HWND hWndFocus = ::GetFocus();
		if (hWndFocus == m_hWnd || IsChild(hWndFocus))
			return TRUE;
#endif
	}
	return FALSE;
}


void	CChildFrame::Impl::OnChildFrameActivate(HWND hWndAct, HWND hWndDeact)
{
	if (hWndAct == m_hWnd) {
		m_bNowActive = true;
		if (MtlIsApplicationActive(m_hWnd)) {
			//SetForegroundWindow(m_hWnd);
			_SetFocusToHTML();
		}
		// �x���ǂݍ��݂��s��
		if (m_strDelayLoadURL.GetLength() > 0) {
			Navigate2(m_strDelayLoadURL);
			m_strDelayLoadURL.Empty();
		}

		// �^�u���ƂɌ����������ۑ�����
		if (m_pGlobalConfig->bSaveSearchWord) {
			CString str;
			str.Format(_T("%d%s"), m_bAutoHilight, m_strSearchWord);
			COPYDATASTRUCT cds;
			cds.dwData	= kSetSearchText;
			cds.lpData	= static_cast<LPVOID>(str.GetBuffer(0));
			cds.cbData	= (str.GetLength() + 1) * sizeof(WCHAR);
			GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);  //\\ �ۑ����Ă�����������������o�[�ɖ߂�
		} else {
			m_pGlobalConfig->bSaveSearchWord = m_pGlobalConfig->bSaveSearchWordOrg;
		}


		if (Misc::IsGpuRendering()) {
			GetParent().SetRedraw(FALSE);
			RECT rcClient;
			GetParent().GetClientRect(&rcClient);
			//::SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | *//*SWP_NOZORDER |*/ SWP_NOREDRAW);
			MoveWindow(&rcClient);
			::ShowWindow(m_hWnd, TRUE);
			//::BringWindowToTop(m_hWnd);

			if (hWndDeact) {
				::ShowWindow/*Async*/(hWndDeact, FALSE);
			}
			GetParent().SetRedraw(TRUE);
			m_view.GetWindow(GW_CHILD).RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

		} else {	// no GPU �Ő����I
			GetParent().SetRedraw(FALSE);
			RECT rcClient;
			GetParent().GetClientRect(&rcClient);
			::SetWindowPos(m_hWnd, NULL, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | *//*SWP_NOZORDER |*/ SWP_NOREDRAW);
			::ShowWindow(m_hWnd, TRUE);
			//::BringWindowToTop(m_hWnd);

			if (hWndDeact) {
				::ShowWindow/*Async*/(hWndDeact, FALSE);
			}
			GetParent().SetRedraw(TRUE);
			m_view.GetWindow(GW_CHILD).RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
		::PostMessage(hWndDeact, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndAct, (LPARAM)hWndDeact);

	} else if (hWndDeact == m_hWnd) {
		m_bNowActive = false;

		// _KillFocusToHTML
		HRESULT	hr = E_FAIL;
		CComQIPtr<IOleInPlaceObject> spIOleInPlaceObject = m_spBrowser;
		if(spIOleInPlaceObject) {
			hr = spIOleInPlaceObject->UIDeactivate(); // IE��UI�𖳌���
		}

#if 0	// ChildFrameClient���ł���悤�ɂ���
		if (m_pGlobalConfig->bSaveSearchWord){	//\\ ���݁A�����o�[�ɂ��镶���������Ă���
			::SendMessage(m_pGlobalConfig->SearchEditHWND, WM_GETTEXT, 1024, (LPARAM)m_strSearchWord.GetBuffer(1024));
			m_strSearchWord.ReleaseBuffer();
		}
#endif

		// �E�B���h�E�\����hWndAct�ɔC����
		if (hWndAct) {
			//::SendMessage(hWndAct, WM_CHILDFRAMEACTIVATE, (WPARAM)hWndAct, (LPARAM)hWndDeact);
			//ShowWindow(FALSE);
		} else {
			// �������Ō�̃^�u�Ȃ̂Ŏ����Ŕ�\���ɂ���
			ShowWindow(FALSE);
		}
	}
}

void	CChildFrame::Impl::OnGetChildFrameData(bool bCreateData)
{
	if (bCreateData == false) {
		::CloseHandle(m_hMapChildFrameData);
		m_hMapChildFrameData = NULL;
	} else {
		ChildFrameDataOnClose	ClosedTabData;
		_CollectDataOnClose(ClosedTabData);

		CString strSharedMemName;
		strSharedMemName.Format(_T("%s%#x"), NOWCHILDFRAMEDATAONCLOSESHAREDMEMNAME, m_hWnd);
		CSharedMemoryHandle sharedMem;
		sharedMem.Serialize(ClosedTabData, strSharedMemName);

		m_hMapChildFrameData = sharedMem.Handle();
	}
}

void	CChildFrame::Impl::OnCreateTravelLogMenu(bool bFore)
{
	m_sharedTravelLogMenu.CloseHandle();

	vector<CString> vecLog;

	CComQIPtr<IServiceProvider> spServiceProvider = m_spBrowser;
	CComPtr<ITravelLogStg>	spTLStg;
	spServiceProvider->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void**)&spTLStg);
	if (spTLStg) {
		CComPtr<IEnumTravelLogEntry> spEnum;
		spTLStg->EnumEntries(bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK, &spEnum);
		if (spEnum) {
			for (int i = 0; i < 9; ++i) {
				CComPtr<ITravelLogEntry> spEntry;
				if (spEnum->Next(1, &spEntry, nullptr) != S_OK)
					break;

				LPWSTR title = nullptr;
				spEntry->GetTitle(&title);
				ATLASSERT( title );
				vecLog.push_back(title);
				::CoTaskMemFree(title);
			}
		}
	}
	CString sharedMemName;
	sharedMemName.Format(_T("%s%#x"), CREATETRAVELLOGMENUSHAREDMEMNAME, m_hWnd);
	m_sharedTravelLogMenu.Serialize(vecLog, sharedMemName);
}

/// ���ݑI������Ă���e�L�X�g��Ԃ�
void	CChildFrame::Impl::OnGetSelectedText(LPCTSTR* ppStr)
{
	CString strSelectedText = GetSelectedText();
	LPTSTR	strtemp = new TCHAR[strSelectedText.GetLength() + 1];
	::wcscpy_s(strtemp, strSelectedText.GetLength() + 1, strSelectedText);
	*ppStr = strtemp;
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

/// �v���N�V��ݒ肷��
LRESULT CChildFrame::Impl::OnSetProxyToChildFrame(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pGlobalConfig->bMultiProcessMode == false)
		return 0;
	
	struct SInternet_Proxy_Info {
		DWORD	dwAccessType;
		LPCSTR	lpszProxy;
		LPCSTR	lpszProxyBypass;
	};
	SInternet_Proxy_Info	proxyinfo;
	if (m_pGlobalConfig->ProxyAddress[0] != '\0') {
		proxyinfo.dwAccessType	= INTERNET_OPEN_TYPE_PROXY;
		proxyinfo.lpszProxy		= m_pGlobalConfig->ProxyAddress;
		if (m_pGlobalConfig->ProxyBypass[0] == '\0') {
			proxyinfo.lpszProxyBypass = NULL;
		} else {
			proxyinfo.lpszProxyBypass = m_pGlobalConfig->ProxyBypass;
		}
	} else {
		proxyinfo.dwAccessType	  = INTERNET_OPEN_TYPE_DIRECT;
		proxyinfo.lpszProxy 	  = NULL;
		proxyinfo.lpszProxyBypass = NULL;
	}
	UrlMkSetSessionOption(INTERNET_OPTION_PROXY, &proxyinfo, sizeof (proxyinfo), 0);
	return 0;
}


LRESULT CChildFrame::Impl::OnDefaultRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	//ClientToScreen(&pt);
	CPoint pt;
	GetCursorPos(&pt);
	HWND hWndFound = WindowFromPoint(pt);
	if (hWndFound && IsChild(hWndFound)) {
		return ::SendMessage(hWndFound, WM_RBUTTONDOWN, wParam, lParam);
	}
	return SendMessage(WM_RBUTTONDOWN, wParam, lParam);
}
LRESULT CChildFrame::Impl::OnDefaultRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	//ClientToScreen(&pt);
	CPoint pt;
	GetCursorPos(&pt);
	HWND hWndFound = WindowFromPoint(pt);
	if (hWndFound && IsChild(hWndFound)) {
		return ::SendMessage(hWndFound, WM_RBUTTONUP, wParam, lParam);
	}
	return SendMessage(WM_RBUTTONUP, wParam, lParam);
}


LRESULT CChildFrame::Impl::OnExecuteJavascript(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CString* pScript = (CString*)wParam;
	_ExecuteUserJavascript(*pScript);
	delete pScript;
	return 0;
}


HANDLE	CChildFrame::Impl::OnGetLoginInfomation(HANDLE hMapForClose)
{
	if (hMapForClose == NULL) {
		LoginInfomation info;
		CComBSTR	strUrl;
		m_spBrowser->get_LocationURL(&strUrl);
		if (strUrl)
			info.strLoginUrl = strUrl;

		CComPtr<IDispatch> spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDoc2 = spDisp;
		if (spDoc2) {
			CComPtr<IHTMLElement>	spActiveElement;
			spDoc2->get_activeElement(&spActiveElement);

			CComPtr<IHTMLElementCollection>	spCol;
			spDoc2->get_forms(&spCol);
			ForEachHtmlElement(spCol, [&](IDispatch* pDisp) -> bool {
				LoginInfomation& info2 = info;
				CComQIPtr<IHTMLFormElement>	spForm = pDisp;
				if (spForm.p) {
					CComQIPtr<IHTMLElement>	spFormElm = spForm;
					VARIANT_BOOL	vbResult;
					spFormElm->contains(spActiveElement, &vbResult);
					if (vbResult == VARIANT_TRUE) {

						ForEachHtmlElement(spForm, [&](IDispatch* pDisp) -> bool {
							CComQIPtr<IHTMLInputElement>	spInput = pDisp;
							if (spInput.p) {
								CComBSTR	strType;
								spInput->get_type(&strType);
								if (strType == nullptr)
									return true;
								strType.ToLower();
								if (strType == L"text" || strType == L"email" || strType == L"password") {
									CComBSTR	strName;
									spInput->get_name(&strName);
									CComBSTR	strValue;
									spInput->get_value(&strValue);
									info2.mapNameValue.insert(std::make_pair(WTL::CString(strName), WTL::CString(strValue)));								
								} else if (strType == L"checkbox") {
									CComBSTR	strName;
									spInput->get_name(&strName);
									if (strName == nullptr) {
										CComQIPtr<IHTMLElement>	spInputElm = spInput;
										spInputElm->get_id(&strName);
									}
									VARIANT_BOOL	vChecked;
									spInput->get_checked(&vChecked);
									info2.mapCheckbox.insert(std::make_pair(WTL::CString(strName), bool(vChecked != VARIANT_FALSE)));
								}
							}
							return true;
						});
					}
				}
				return true;
			});
		}

		CString sharedMemName;
		sharedMemName.Format(_T("%s%#x"), GETLOGININFOMATIONSHAREDMEMNAME, m_hWnd);
		CSharedMemoryHandle sharedMem;
		sharedMem.Serialize(info, sharedMemName);		
		return sharedMem.Handle();
	} else {
		::CloseHandle(hMapForClose);
		return NULL;
	}
}


DWORD	CChildFrame::Impl::OnChangeChildFrameFlags(ChildFrameChangeFlag change, DWORD flags)
{
	switch (change) {
	case kChangeDLCtrl:
		if (flags == -1) {
			return m_view.GetDLControlFlags();
		} else {
			m_view.PutDLControlFlags(flags);
			m_view.LightRefresh();
		}
		break;

	case kChangeExStyle:
		if (flags == -1)
			return m_view.GetExStyle();
		else
			m_view.SetExStyle(flags);
		break;

	case kChangeAutoRefresh:
		if (flags == -1)
			return m_view.GetAutoRefreshStyle();
		else
			m_view.SetAutoRefreshStyle(flags);
		break;

	default:
		ATLASSERT( FALSE );
	}
	return 0;
}



/// �t�@�C�������
void 	CChildFrame::Impl::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	PostMessage(WM_CLOSE);
}

// �^�u�o�[����
void	CChildFrame::Impl::OnTabClone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString url = GetLocationURL();
	std::vector<CString> vecUrl;
	vecUrl.emplace_back(url);
	auto pair = CreateMultiText(vecUrl);
	OpenMultiUrl(pair, m_hWnd);
}

// �����o�[����

/// �n�C���C�g
LRESULT CChildFrame::Impl::OnHilight(const CString& strKeyWord)
{
	bool bHilightSw = m_pGlobalConfig->bHilightSwitch;
	CString strHilight = strKeyWord;
	m_bAutoHilight = bHilightSw;
	if (bHilightSw) {
		m_strSearchWord = strHilight;
		_DeleteMinimumHilightTextLengthWord(strHilight);
	} else {
		m_strSearchWord.Empty();
	}
	_HilightText(strHilight, bHilightSw);

	return TRUE;
}

void	CChildFrame::Impl::_HilightText(LPCTSTR lpszKeyWord, bool bHilight)
{
	if (bHilight) {
		if (m_SearchBarHilightData.strLastHilightText == lpszKeyWord)
			return ;

		m_SearchBarHilightData.strLastHilightText = lpszKeyWord;
	} else {
		m_SearchBarHilightData.strLastHilightText.Empty();
	}

	MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
	MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(lpszKeyWord, bHilight, [this, lpszKeyWord]() -> bool {
		// ���܂��Ă郁�b�Z�[�W����������
		MSG msg = {};
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd) {
				return false;
			}

			if (PreTranslateMessage(&msg)) {
				::GetMessage(&msg, NULL, 0, 0);
				continue ;
			}
			// �������g�̊֐����Ă΂ꂽ�璆�~����
			if (msg.message == WM_DELAYHILIGHT && msg.hwnd == m_hWnd && msg.lParam == 0) {
				CString* pStrHilight = (CString*)msg.wParam;
				ATLASSERT( pStrHilight );
				if (*pStrHilight != lpszKeyWord || m_pGlobalConfig->bHilightSwitch == false/* �n�C���C�g���~ */) {
					return false;
				} else {
					::GetMessage(&msg, NULL, 0, 0);
					continue ;	// �n�C���C�g���镶���������Ȃ̂Ńn���h������
				}
			}

			::GetMessage(&msg, NULL, 0, 0);
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			// �ʂ̃y�[�W�Ƀi�r�Q�[�g���J�n���ꂽ�̂Œ��~����
			if (m_bNowNavigate)
				return false;
		}
		return true;		
	}));
}



/// �y�[�W������
int		CChildFrame::Impl::OnFindKeyWord(HANDLE handle)
{
	if (!m_spBrowser)
		return 0;

	FindKeywordData	findKeywordData;

	CString strSharedMemName;
	strSharedMemName.Format(_T("%s%#x"), FINDKEYWORDATASHAREDMEMNAME, handle);
	CSharedMemory sharedMem;
	sharedMem.Deserialize(findKeywordData, strSharedMemName);

	CComPtr<IDispatch>	spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2>	spDocument = spDisp;	// html�̎擾
	if (!spDocument)
		return 0;

	CString strKeyword = findKeywordData.strKeyword;
	strKeyword.Replace(_T('�'), _T('�J'));

	FindHilightData*	pfindHilightData = nullptr;
	if (findKeywordData.Flags & 0x80)
		pfindHilightData = &m_FindBarHilightData;
	else
		pfindHilightData = &m_SearchBarHilightData;

	// ����
	BOOL	bSts = _FindKeyWordOne(spDocument, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
	if (bSts)
		return MAKELONG(TRUE, (WORD)pfindHilightData->nHitPos);

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
	if (findKeywordData.bFindDown) {	// �y�[�W������ - ��
		for (LONG ii = pfindHilightData->nPaneBookmark; ii < nCount; ii++) {
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
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
			if (bFindIt) {
				pfindHilightData->nPaneBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			pfindHilightData->nPaneBookmark = 0;
			pfindHilightData->strBookmark.Empty();
		}

	} else {			// �y�[�W������ - ��
		if (pfindHilightData->nPaneBookmark == 0 && !pfindHilightData->strBookmark)
			pfindHilightData->nPaneBookmark = nCount - 1;

		for (LONG ii = pfindHilightData->nPaneBookmark; ii >= 0; ii--) {
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
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
			if (bFindIt) {
				pfindHilightData->nPaneBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			pfindHilightData->nPaneBookmark = 0;
			pfindHilightData->strBookmark.Empty();
		}
	}
	if (bFindIt == false)
		pfindHilightData->nHitPos = 0;	// ���Z�b�g����

	return MAKELONG(bFindIt, (WORD)pfindHilightData->nHitPos);
}

static void _RemoveHighlight(IHTMLDocument3* pDoc3)
{
	CComQIPtr<IHTMLDocument2> spDoc2 = pDoc3;
	_MtlForEachHTMLDocument2(spDoc2, [&](IHTMLDocument2* pDoc) -> bool {
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
		return true;
	});
}


void	CChildFrame::Impl::OnRemoveHilight()
{
	m_FindBarHilightData.strLastHilightText.Empty();
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3> spDoc3 = spDisp;
	if (spDoc3)
		_RemoveHighlight(spDoc3);
}


void	CChildFrame::Impl::OnStatusBarDefaultPaneDblClk(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_strLastScriptErrorMessage.GetLength() > 0) {
		MessageBox(m_strLastScriptErrorMessage, _T("�X�N���v�g�G���["), MB_ICONWARNING);
		if (::GetKeyState(VK_CONTROL) < 0)
			MtlSetClipboardText(m_strLastScriptErrorMessage, NULL);
	}
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

	if (nID == ID_SPECIAL_HOME || nID == ID_SPECIAL_END) {
		auto funcGetHTMLWindowOnCursorPos = [](CPoint& pt, IHTMLDocument3* pDoc) -> CComPtr<IHTMLWindow2> {
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
					CRect rcAbsolute = GetIFrameAbsolutePosition(spDisp2.p);
					CPoint ptScroll = GetScrollPosition(spDoc2);
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
				spWindow->scrollBy(0, -1000000000L);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +1000000000L);
		} else {
			CComQIPtr<IHTMLDocument2>	spDocument2 = spDocument;
			spDocument2->get_parentWindow(&spWindow);
			ATLASSERT(spWindow);
			if (nID == ID_SPECIAL_HOME)
				spWindow->scrollBy(0, -1000000000L);
			else if (nID == ID_SPECIAL_END)
				spWindow->scrollBy(0, +1000000000L);
		}
	} else {
		m_view.SetFocus();

		//INPUT	inputs[2] = {};
		//inputs[0].type		= INPUT_KEYBOARD;
		//inputs[0].ki.wVk	= nCode;
		////inputs[0].ki.wScan	= ::MapVirtualKey(nCode, 0);
		//inputs[0].ki.dwFlags= KEYEVENTF_EXTENDEDKEY;
		//inputs[0].ki.dwExtraInfo = ::GetMessageExtraInfo();

		//inputs[1].type		= INPUT_KEYBOARD;
		//inputs[1].ki.wVk	= nCode;
		////inputs[1].ki.wScan	= ::MapVirtualKey(nCode, 0);
		//inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		//inputs[1].ki.dwExtraInfo = ::GetMessageExtraInfo();

		//SendInput(2, inputs, sizeof(INPUT));

		m_view.SendMessageToDescendants(WM_KEYDOWN, nCode, 0);
		m_view.SendMessageToDescendants(WM_KEYUP, nCode, 0);

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
	if (strUrl.IsEmpty()) {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDocument = spDisp;
		ATLASSERT(spDocument);
		CPoint	pt = m_view.GetMenuPoint();
		if (pt.x == -1 && pt.y == -1) {
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
		}
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
			strUrl = funcGetUrl(spImg.p);
		}
	}

	if (strUrl.GetLength() > 0)
		m_view.StartTheDownload(strUrl, true);
}

void	CChildFrame::Impl::OnDoAutoLogin(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strURL = GetLocationURL();
	int nIndex = CLoginDataManager::Find(strURL);
	if (nIndex != -1) {
		bool bSuccess = CLoginDataManager::DoAutoLogin(nIndex, m_spBrowser);
	}
}


void	CChildFrame::Impl::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_bReload = true;
	m_nAutoLoginPrevIndex = -1;
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

			vZoom = static_cast<long>(scl);
			HRESULT hr = m_spBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoom, NULL);
			m_nImgScl = scl;

		} else {
			// �����͉摜�̊g��/�k���ȊO�g��Ȃ��Ǝv��
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
			CComBSTR	bstrZoom(L"zoom");
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

			if (m_ImageSize != CSize(0, 0) && m_nImgSclSw == 0 && m_nImgScl == 100) {	// �摜��100%�ɖ߂�
				CSize	shrinkImageSize;
				enum { kImageMargin = 4, kImagePadding = 20 };
				shrinkImageSize.cx = static_cast<LONG>((m_ImageSize.cx + kImagePadding) * (double(m_nImgSclSav) / 100.0));
				shrinkImageSize.cy = static_cast<LONG>((m_ImageSize.cy + kImagePadding) * (double(m_nImgSclSav) / 100.0));
				CRect rcShrinkImage(0, 0, shrinkImageSize.cx, shrinkImageSize.cy);
				
				rcShrinkImage.MoveToXY(kImageMargin, kImageMargin);
				CPoint pt;
				::GetCursorPos(&pt);
				this->GetParent().ScreenToClient(&pt);
				if (rcShrinkImage.PtInRect(pt)) {
					CPoint pt100;
					pt100.x		= static_cast<LONG>(double(pt.x + kImageMargin) / (double(m_nImgSclSav) / 100.0));
					pt100.y		= static_cast<LONG>(double(pt.y + kImageMargin) / (double(m_nImgSclSav) / 100.0));
					int a = 0;
					CRect rcClient;
					this->GetClientRect(&rcClient);
					CPoint ptScroll;
					ptScroll.x	= pt100.x - (rcClient.Width() / 2);// + kImageMargin;
					ptScroll.y	= pt100.y - (rcClient.Height() / 2);// + kImageMargin;
					CComPtr<IHTMLWindow2> spWindow;
					pDoc->get_parentWindow(&spWindow);
					if (!spWindow)
						return ;
					spWindow->scrollBy(ptScroll.x, ptScroll.y);
				}
			} else {
				CComPtr<IHTMLWindow2> spWindow;
				pDoc->get_parentWindow(&spWindow);
				if (!spWindow)
					return;
				spWindow->scrollBy(-m_ImageSize.cx, -m_ImageSize.cy);
			}
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
		SetBodyStyleZoom(0, m_nImgScl, wndCtl == NULL);
		break;

	default: {
		ATLASSERT(ID_HTMLZOOM_400 <= nID && nID <= ID_HTMLZOOM_050);
		static const int scls[] = { 400, 200, 150, 125, 100, 75, 50 };
		int  n = nID - ID_HTMLZOOM_400;
		if (n < 0 || n > ID_HTMLZOOM_050 - ID_HTMLZOOM_400)
			return;
		SetBodyStyleZoom(0, scls[n], true);
		}
		break;
	}
}


/// �I��͈͂̃����N���J��
void 	CChildFrame::Impl::OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CSimpleArray<CString> arrUrls;
	bool bNoAddFromMenu = false;
	MtlForEachHTMLDocument2( m_spBrowser, [&arrUrls, &bNoAddFromMenu, this] (IHTMLDocument2 *pDocument) -> bool {
		CComPtr<IHTMLSelectionObject>	spSelection;
		HRESULT 	hr	= pDocument->get_selection(&spSelection);
		if ( FAILED(hr) )
			return true;

		CComPtr<IDispatch>				spDisp;
		hr = spSelection->createRange(&spDisp);
		if ( FAILED(hr) )
			return true;

		CComQIPtr<IHTMLTxtRange>		spTxtRange = spDisp;
		if (!spTxtRange)
			return true;

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
				return true;

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
		return true;
	});

	std::vector<CString> vecUrl(arrUrls.GetData(), arrUrls.GetData() + arrUrls.GetSize());
	auto pair = CreateMultiText(vecUrl);
	OpenMultiUrl(pair, m_hWnd);
#if 0	//:::
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

/// URL�e�L�X�g���J�� : �I�𕶎��񂩂�URL���ۂ�����������o���ĊJ��
void 	CChildFrame::Impl::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	std::wstring strText = GetSelectedText();
	if (strText.empty())
		return;

	std::vector<CString>	vecUrls;
	std::wregex rx(L"(?:(?:(?:(?:(?:h|)t|)t|)p(s)?|)://|)?((?:(?:[a-z0-9_-]+\\.)+)\\w+(?::\\d+)?(?:/\\S*)?)", std::regex_constants::icase);
	std::wsmatch result;
	auto itbegin = strText.cbegin();
	auto itend	 = strText.cend();
	while (std::regex_search(itbegin, itend, result, rx)) {
		CString strUrl = result[1].str().empty() ? _T("http://") : _T("https://");
		strUrl += result[2].str().c_str();
		vecUrls.push_back(strUrl);
		itbegin = result[0].second;
	}
	auto pair = CreateMultiText(vecUrls);
	OpenMultiUrl(pair, m_hWnd);
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
#if 0
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
#endif
}

/// �|�b�v�A�b�v�}�~�ɒǉ����ĕ��܂��B
void 	CChildFrame::Impl::OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
//	CIgnoredURLsOption::Add( GetLocationURL() );
	//m_bClosing = true;

	if (::GetKeyState(VK_CONTROL) < 0) {
		MtlSetClipboardText(m_strNewWindowURL, NULL);
		return ;
	}

	CString url = GetLocationURL();
	COPYDATASTRUCT cds = { 0 };
	cds.dwData	= kAddIgnoreURL;
	cds.lpData	= static_cast<LPVOID>(url.GetBuffer(0));
	cds.cbData	= (url.GetLength() + 1) * sizeof(TCHAR);
	GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

	PostMessage(WM_CLOSE);
}

/// �^�C�g���}�~�ɒǉ�
void 	CChildFrame::Impl::OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
//	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	//m_bClosing = true;
	CString title = MtlGetWindowText(m_hWnd);
	COPYDATASTRUCT cds = { 0 };
	cds.dwData	= kAddCloseTitle;
	cds.lpData	= static_cast<LPVOID>(title.GetBuffer(0));
	cds.cbData	= (title.GetLength() + 1) * sizeof(TCHAR);
	GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

	PostMessage(WM_CLOSE);
}

/// CSS ���j���[
void	CChildFrame::Impl::OnChangeCSS(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	bool bOff = false;
	CString cssName;
	CString cssPath;
	CComBSTR cssText;
	
	switch (nID) {
	case ID_STYLESHEET_BASE:
		break;

	case ID_STYLESHEET_OFF:
		bOff = true;
		break;

	default:
		{
			int nIndex = nID - (ID_INSERTPOINT_CSSMENU + 1);
			std::vector<CString>	vecFileName;
			MtlForEachFileSort(Misc::GetExeDirectory() + _T("css"), [this, &vecFileName](const CString& filePath) {
				CString ext = Misc::GetFileExt(filePath);
				ext.MakeLower();
				if (ext != _T("css"))
					return ;
				vecFileName.push_back(Misc::GetFileBaseName(filePath));
			});
			if (nIndex < 0 || (int)vecFileName.size() <= nIndex) {
				ATLASSERT( FALSE );
				return ;
			}
			cssName = vecFileName[nIndex];
			cssPath = Misc::GetExeDirectory() + _T("css\\") + cssName;
			std::ifstream fs(cssPath);
			if (!fs)
				return ;
			std::string tempBuff;
			while (fs.good()) {
				enum { kBuffSize = 5120 };
				char buff[kBuffSize + 1] = "";
				fs.read(buff, kBuffSize);
				tempBuff.append(buff, fs.gcount());
			}
			cssText = tempBuff.c_str();
		}
		break;
	}
	MtlForEachHTMLDocument2g(m_spBrowser, [=](IHTMLDocument2* pDocument) -> bool {
		// �X�^�C���V�[�g�ꗗ���擾
		CComPtr<IHTMLStyleSheetsCollection> spSheetsColl;
		pDocument->get_styleSheets(&spSheetsColl);
		if (!spSheetsColl)
			return true;

		bool	bFindTarSheet = false;
		long	lLength = 0;
		spSheetsColl->get_length(&lLength);
		for (long i = 0; i < lLength; i++) {
			VARIANT varIdx, varSheet;
			varIdx.vt		  = VT_I4;
			varIdx.lVal 	  = i;
			varSheet.vt 	  = VT_DISPATCH;
			varSheet.pdispVal = NULL;
			spSheetsColl->item(&varIdx, &varSheet);
			CComQIPtr<IHTMLStyleSheet> spSheet2 = varSheet.pdispVal;
			if (spSheet2) {
				CComBSTR	strTitle;
				spSheet2->get_title(&strTitle);				
				CString	strTarSheet;
				if (strTitle)
					strTarSheet = strTitle;
				if (strTarSheet == cssName && bOff == false) {
					spSheet2->put_disabled(VARIANT_FALSE);
					bFindTarSheet = true;
				} else {
					spSheet2->put_disabled(VARIANT_TRUE);
				}
			}
			if (varSheet.pdispVal)
				varSheet.pdispVal->Release();
		}

		if ( !bFindTarSheet && !bOff && !cssPath.IsEmpty() ) {
			CComPtr<IHTMLStyleSheet> spSheet;
			pDocument->createStyleSheet(NULL, -1, &spSheet);
			if (spSheet) {
				// ������ݒ�
				//long lPos;
				//spSheet->addImport( (BSTR) CComBSTR(cssPath), -1, &lPos );
				spSheet->put_cssText(cssText);
				spSheet->put_title( (BSTR) CComBSTR(cssName) );
				spSheet->put_disabled(VARIANT_FALSE);
			}
		}
		return true;
	});
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

LRESULT CChildFrame::Impl::OnEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString strText = GetSelectedTextLine();
	COPYDATASTRUCT	cds;
	cds.dwData	= kOpenFindBarWithText;
	cds.lpData	= static_cast<LPVOID>(strText.GetBuffer(0));
	cds.cbData	= (strText.GetLength() + 1) * sizeof(WCHAR);
	if (GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds)) {
		return true;
	} else {
		__super::OnEditFind(0, 0, NULL);
		return false;
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

/// ���̊K�w��
void	CChildFrame::Impl::OnViewUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strURL = GetLocationURL();

	if ( strURL.ReverseFind(_T('/')) == (strURL.GetLength() - 1) )
		strURL = strURL.Left( strURL.ReverseFind(_T('/')) );

	if (strURL.ReverseFind(_T('/')) != -1) {
		strURL = strURL.Left(strURL.ReverseFind(_T('/')) + 1);
	} else
		return;

	if (strURL[strURL.GetLength() - 2] == _T('/'))
		return;

	Navigate2(strURL);
}

/// ���[�g�K�w��
void	CChildFrame::Impl::OnViewRoot(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strURL = GetLocationURL();
	int nPos = strURL.Find(_T('/'), 9);
	if (nPos == -1)
		return ;
	if (strURL.ReverseFind(_T('/')) == nPos) {	// ���Ń��[�g�Ȃ̂ł���ɑk��
		std::wregex	rx(L"(http(?:s)?://)([^/]+/)");
		std::wsmatch result;
		std::wstring url = strURL;
		if (std::regex_match(url, result, rx)) {
			std::wstring prefix = result[1].str();
			std::wstring body = result[2].str();
			int nDotPos = body.find(L'.');
			if (nDotPos != -1) {
				strURL.Format(_T("%s%s"), prefix.c_str(), body.substr(nDotPos + 1).c_str());
				Navigate2(strURL);
				return ;
			}
		}
	}

	strURL = strURL.Left(nPos + 1);
	Navigate2(strURL);
}

/// �v���C�o�V�[���|�[�g
void	CChildFrame::Impl::OnPrivacyReport(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString 	strURL = GetLocationURL();
	CComBSTR	bstrURL(strURL);

	CComQIPtr<IServiceProvider>	spSP = m_spBrowser;
	if (spSP == NULL)
		return ;

	static const CLSID CLSID_IEnumPrivacyRecords = { 0x3050f844, 0x98b5, 0x11cf, { 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b } };

	CComPtr<IEnumPrivacyRecords>	spEnumRecords;
	spSP->QueryService(CLSID_IEnumPrivacyRecords, &spEnumRecords);
	if (spEnumRecords == NULL)
		return ;

	HINSTANCE		hInstDLL;
	typedef DWORD	(WINAPI* FuncDoPrivacyDlg)(HWND, LPOLESTR, IEnumPrivacyRecords*, BOOL);
	FuncDoPrivacyDlg pfnDoPrivacyDlg = NULL;

	if (Misc::getIEMejourVersion() >= 8 && _CheckOsVersion_VistaLater() == 0){//\\ XP+IE8�̏ꍇ
		hInstDLL = ::LoadLibrary( _T("ieframe.dll") );
	} else {//vista+IE8�̏ꍇ
		hInstDLL = ::LoadLibrary( _T("shdocvw.dll") );
	}
	if ( hInstDLL == NULL )
		return ;

	pfnDoPrivacyDlg	= (FuncDoPrivacyDlg)GetProcAddress( hInstDLL, "DoPrivacyDlg" );
	if (pfnDoPrivacyDlg == NULL) {
		::FreeLibrary(hInstDLL);
		return ;
	}

	BOOL	bPrivacyImpacted = FALSE;
	spEnumRecords->GetPrivacyImpacted(&bPrivacyImpacted);
	pfnDoPrivacyDlg(m_hWnd, bstrURL, spEnumRecords, !bPrivacyImpacted);

	::FreeLibrary( hInstDLL );
}


/// �p������DL�R���g���[���𓾂�
DWORD	CChildFrame::Impl::_GetInheritedDLCtrlFlags()
{
	DWORD dwDLFlags = m_pGlobalConfig->dwDLControlFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, m_pGlobalConfig->dwMainExtendedStyle) )
		dwDLFlags = m_view.GetDLControlFlags();
	return dwDLFlags;
}

/// �p������ExStyle�𓾂�
DWORD	CChildFrame::Impl::_GetInheritedExStyleFlags()
{
	DWORD dwExFlags = m_pGlobalConfig->dwExtendedStyleFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, m_pGlobalConfig->dwMainExtendedStyle) ) {
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
	data.strTitle	= MtlGetWindowText(m_hWnd);
	data.strTitle.Replace(_T('\"'), _T('_'));
	data.dwDLCtrl	= m_view.GetDLControlFlags();
	data.dwExStyle	= m_view.GetExStyle();
	data.dwAutoRefreshStyle	= m_view.GetAutoRefreshStyle();

	data.strURL		= GetLocationURL();
	if (data.strURL.IsEmpty()) {
		data.strURL	= m_strDelayLoadURL;
		data.TravelLogBack	= m_TravelLogBack;
		data.TravelLogFore	= m_TravelLogFore;
		return ;
	}

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
				vecLog.push_back(std::make_pair(WTL::CString(szTitle), WTL::CString(szURL)));
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
	if (m_spBrowser == nullptr)
		return ;
	if (m_pGlobalConfig->AutoImageResizeType == AUTO_IMAGE_RESIZE_NONE)
		return ;
	//TRACEIN(L"_AutoImageResize : bFirst(%d)", bFirst);
	if (bFirst) {
		CComPtr<IDispatch>	spDisp;
		m_spBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2>	spDoc = spDisp;
		if (spDoc == nullptr)
			return ;
		CComBSTR	strmineType;
		HRESULT hr = spDoc->get_mimeType(&strmineType);
		if (strmineType) {
			CString strExt = CString(strmineType).Left(3);
			//TRACEIN(L"  mineType == %s  Ext == %s", strmineType, strExt);
			if (strExt != _T("JPG") && strExt != _T("PNG") && strExt != _T("GIF") && strmineType != L"�r�b�g�}�b�v �C���[�W" && strExt != _T("JPE"))
				return ;
		} else {
			CString strUrl = GetLocationURL();
			if (GetLocationName() == strUrl) {
				CString strExt = strUrl.Right(4);
				strExt.MakeUpper();
				if (strExt != _T(".JPG") && strExt != _T(".PNG") && strExt != _T(".GIF") && strExt != _T(".BMP") && strExt != _T(".JPE"))
					return ;
			} else 
				return ;
		}
	
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

	enum { kMargen = 20/*32*/ };
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
	CComBSTR	bstrZoom(L"zoom");

	CComPtr<IHTMLStyle>		pHtmlStyle;
	if ( FAILED(spHTMLElement2->get_runtimeStyle(&pHtmlStyle)) || !pHtmlStyle )
		return;
	pHtmlStyle->setAttribute(bstrZoom, variantVal);
	if (m_nImgSclSw) {
		pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));

		CComPtr<IHTMLWindow2> spWindow;
		spDoc->get_parentWindow(&spWindow);
		if (!spWindow)
			return;
		spWindow->scrollBy(-m_ImageSize.cx, -m_ImageSize.cy);
	}
	TRACEIN(L"  ���T�C�Y����");
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
						if (strFaviconURL.Left(2) == _T("//"))
							strFaviconURL.Insert(0, _T("http:"));
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

	FindHilightData*	pfindHilightData = nullptr;
	if (Flags & 0x80) {
		Flags &= ~0x80;
		pfindHilightData = &m_FindBarHilightData;
	} else {
		pfindHilightData = &m_SearchBarHilightData;
	}

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

	// �O��̑������猟������
	if (pfindHilightData->strBookmark && pfindHilightData->strLastSearchText == strKeyword) {
		VARIANT_BOOL vMoveBookmark = VARIANT_FALSE;
		spTxtRange->moveToBookmark(pfindHilightData->strBookmark, &vMoveBookmark);
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
		pfindHilightData->nHitPos = 0;
		long lActual;
		spTxtRange->moveStart(CComBSTR("Textedit"), -1, &lActual);
		spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
	}
	pfindHilightData->strLastSearchText = strKeyword;

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
				CComBSTR	strvisibility;
				spStyle->get_display(&strdisplay);
				spStyle->get_visibility(&strvisibility);
				if ((strdisplay && strdisplay == _T("none")) ||
					(strvisibility && strvisibility == _T("hidden"))) {	// �\������Ă��Ȃ��ꍇ�̓X�L�b�v
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

		CComQIPtr<IHTMLElement2>	spElm2 = pElem;
		if (spElm2 == nullptr)
			return;

		long		height = 0;
		long		width = 0;
		hr = spElm2->get_clientHeight(&height); 	// HTML�\���̈�̍���
		hr = spElm2->get_clientWidth(&width);
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

		CComPtr<IHTMLWindow2> pWnd;
		hr = pDoc2->get_parentWindow(&pWnd);
		if ( FAILED(hr) )
			return;

		long		y = 0;
		hr	= pTxtRM->get_offsetTop(&y);		// �I�𕔕��̉�ʏォ���y���W
		if ( FAILED(hr) )
			return;

		long scy = y - height / 2;				// ��ʒ����܂ł̋���

		// �������\��������1/4���傫����΃X�N���[��������
		if ( (scy > height / 4) || (scy < -height / 4) ) {
			pWnd->scrollBy(0, scy);
		}

		long	x = 0;
		pTxtRM->get_offsetLeft(&x);
		if (x < (double)width * ((double)3 / (double)4)) {
			pWnd->scrollBy(-width, 0);
		} else {
			pWnd->scrollBy(width - x, 0);
		}

	};

	if (vBool == VARIANT_FALSE) {
		CComPtr<IHTMLSelectionObject> spSelection;
		pDocument->get_selection(&spSelection);
		if (spSelection)
			spSelection->empty();
	} else {
		if (spTxtRange->getBookmark(&pfindHilightData->strBookmark) != S_OK)
			pfindHilightData->strBookmark.Empty();

		spTxtRange->select();
		spTxtRange->scrollIntoView(VARIANT_TRUE);

		bSts = TRUE;

		if (m_pGlobalConfig->bScrollCenter)
			funcScrollBy(pDocument);
	}
	if (bSts) {			
		if (bFindDown) {
			++pfindHilightData->nHitPos;
		} else {
			--pfindHilightData->nHitPos;
		}
		if (pfindHilightData->nHitPos < 0 && pfindHilightData->nLastHilightCount > 0) {
			pfindHilightData->nHitPos = pfindHilightData->nLastHilightCount - (-pfindHilightData->nHitPos) + 1;
		}
	}

	return bSts;
}


void	CChildFrame::Impl::_SearchWebWithEngine(const CString& strText, const CString& strEngine)
{
	CString str;
	str.Format(_T("%s\n%s"), strEngine ,strText);
	COPYDATASTRUCT	cds;
	cds.dwData	= kSearchTextWithEngine;
	cds.lpData	= (LPVOID)str.GetBuffer(0);
	cds.cbData	= (str.GetLength() + 1) * sizeof(WCHAR);
	GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}


/// strScriptText�ɏ�����Ă���X�N���v�g�����s����
void	CChildFrame::Impl::_ExecuteUserJavascript(const CString& strScriptText)
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
	HRESULT hr = spWindow->execScript(CComBSTR(strScriptText), CComBSTR(L"javascript"), &vRet);
	TRACEIN(_T("_ExecuteUserJavascript() : ���s(%s)"), CString(strScriptText).Left(45));
}



int		CChildFrame::Impl::_HilightFromFindBar(LPCTSTR strText, bool bNoHighlight, bool bEraseOld, long Flags)
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

	if (m_FindBarHilightData.strLastHilightText == strKeyword && m_FindBarHilightData.flags == Flags)
		return m_FindBarHilightData.nLastHilightCount;

	m_FindBarHilightData.strLastHilightText = strKeyword;
	m_FindBarHilightData.flags = Flags;

	/* �O�̃n�C���C�g�\�������� */
	TIMERSTART();
	CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc;
	if (bEraseOld)
		_RemoveHighlight(spDoc3);
	TIMERSTOP(_T("_HilightFromFindBar, _RemoveHilight"));

	/* �P����n�C���C�g���� */
	TIMERSTART();
	int nMatchCount = 0;
	if (strKeyword.GetLength() > 0) {
		_MtlForEachHTMLDocument2(spDoc, [&](IHTMLDocument2* pDoc) -> bool {
			CComPtr<IHTMLSelectionObject> spSelection;	/* �e�L�X�g�I������ɂ��� */
			pDoc->get_selection(&spSelection);
			if (spSelection.p)
				spSelection->empty();

			CComPtr<IHTMLElement>	spElm;
			pDoc->get_body(&spElm);
			CComQIPtr<IHTMLBodyElement>	spBody = spElm;
			if (spBody.p == nullptr)
				return true;

			CComPtr<IHTMLTxtRange>	spTxtRange;
			spBody->createTextRange(&spTxtRange);
			if (spTxtRange.p == nullptr)
				return true;

			//long nMove;
			//spTxtRange->moveStart(strTextedit, -1, &nMove);
			//spTxtRange->moveEnd(strTextedit, 1, &nMove);
			VARIANT_BOOL	vResult;
			CComBSTR	strWord = strKeyword;
			while (spTxtRange->findText(strWord, 1, Flags, &vResult), vResult == VARIANT_TRUE) {
				CComPtr<IHTMLElement> spParentElement;
				spTxtRange->parentElement(&spParentElement);

				//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
				// �f�B�X�v���C�ɕ\������Ă��邩�ǂ���
				auto funcMove = [&spTxtRange] () {	// �����͈͂�ύX����֐�
					spTxtRange->collapse(false);
				};
		
				CComPtr<IHTMLElement>	spFirstParentElement;

				bool	bVisible = true;
//				CComQIPtr<IHTMLElement> spParentElement;
//				spTxtRange->parentElement(&spParentElement);
				spFirstParentElement = spParentElement;
				while (spParentElement != nullptr) {
					CComQIPtr<IHTMLElement2>	spParentElm2 = spParentElement;
					CComPtr<IHTMLCurrentStyle>	spStyle;
					spParentElm2->get_currentStyle(&spStyle);
					if (spStyle != nullptr) {
						CComBSTR	strdisplay;
						CComBSTR	strvisibility;
						spStyle->get_display(&strdisplay);
						spStyle->get_visibility(&strvisibility);
						if ((strdisplay && strdisplay == _T("none")) ||
							(strvisibility && strvisibility == _T("hidden"))) {	// �\������Ă��Ȃ��ꍇ�̓X�L�b�v
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
				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				spParentElement	= spFirstParentElement;

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

				// ���܂��Ă郁�b�Z�[�W����������
				MSG msg = {};
				while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
					if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd) {
						return false;
					}

					if (PreTranslateMessage(&msg)) {
						::GetMessage(&msg, NULL, 0, 0);
						continue ;
					}
					// �������g�̊֐����Ă΂ꂽ�璆�~����
					if (msg.message == WM_DELAYHILIGHT && msg.hwnd == m_hWnd && msg.lParam) {
						CString* pStrHilight = (CString*)msg.wParam;
						ATLASSERT( pStrHilight );
						CString strData = *pStrHilight;
						CString strDw	= strData[0];
						CString strFlags = strData[1];
						DWORD dw = _wtoi(strDw);
						long flags = _wtoi(strFlags);
						strData = strData.Mid(2);
						if (strData != strKeyword || m_FindBarHilightData.flags != flags) {
							_RemoveHighlight(spDoc3);
							return false;
						} else {
							::GetMessage(&msg, NULL, 0, 0);
							continue ;	// �n�C���C�g���镶���������Ȃ̂Ńn���h������
						}
					} else if (msg.message == WM_REMOVEHILIGHT && msg.hwnd == m_hWnd) {
						::GetMessage(&msg, NULL, 0, 0);
						m_FindBarHilightData.strLastHilightText.Empty();
						_RemoveHighlight(spDoc3);
						return false;
					}

					::GetMessage(&msg, NULL, 0, 0);
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);

					// �ʂ̃y�[�W�Ƀi�r�Q�[�g���J�n���ꂽ�̂Œ��~����
					if (m_bNowNavigate)
						return false;
				}
			}
			return true;
		});
	}
	TIMERSTOP(_T(" �n�C���C�g�ɂ�����������(%d ��hit)"), nMatchCount);
	m_FindBarHilightData.nLastHilightCount = nMatchCount;
	return nMatchCount;
}


void	CChildFrame::Impl::_SetFocusToHTML()
{
	CComQIPtr<IOleObject>	spOleObj = m_spBrowser;
	if (spOleObj) {
		CComPtr<IOleClientSite> spClientSite;
		spOleObj->GetClientSite(&spClientSite);
		RECT rcClient;
		GetClientRect(&rcClient);
		HRESULT hr = spOleObj->DoVerb(OLEIVERB_UIACTIVATE, NULL, spClientSite, 0, m_hWnd, &rcClient);
	}
}


bool	CChildFrame::Impl::_ShowLinkTextSelectWindow(MSG* pMsg)
{
	CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
	ATLASSERT( m_spBrowser );
	CComPtr<IDispatch> spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spDoc = spDisp;
	if (spDoc == nullptr)
		return false;

	CComPtr<IHTMLElement> spElmHit;
	spDoc->elementFromPoint(pt.x, pt.y, &spElmHit);
	if (spElmHit == nullptr)
		return false;

	CComPtr<IHTMLElement> spElmTemp = spElmHit;
	CComPtr<IHTMLElement> spElmAnchor;
	for (;;) {
		CComBSTR tagname;
		spElmTemp->get_tagName(&tagname);
		if (tagname == nullptr)
			return false;
		if (tagname == L"A") {
			spElmAnchor = spElmTemp;
			break;
		}
		CComPtr<IHTMLElement> spElmParent;
		spElmTemp->get_parentElement(&spElmParent);
		spElmTemp.Release();
		spElmTemp = spElmParent;
		if (spElmTemp == nullptr)
			return false;
	}
	ATLASSERT( spElmAnchor );

	CComBSTR text;
	spElmAnchor->get_innerText(&text);
	if (text.Length() == 0)
		return true;
	
	CRect rcAbsolute = GetIFrameAbsolutePosition(spElmAnchor.p);
	CPoint ptScroll = GetScrollPosition(spDoc);
	CRect rc = rcAbsolute - ptScroll;
	ClientToScreen(&rc);

	///////////////////////////////////////////////////
	// CTextWindow

	class CTextWindow : 
		public CWindowImpl<CTextWindow, CWindow, CWinTraits<WS_POPUP | WS_DLGFRAME | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW | WS_EX_TOPMOST> >,
		public CMessageFilter
	{
	public:
		CTextWindow(LPCWSTR text, CPoint pt, CMessageLoop* pLoop, bool bRightDragSearch) : m_strWord(text), m_ptTopLeft(pt), m_pLoop(pLoop), m_bUseRightDragSearch(bRightDragSearch)
		{ }

		void	OnFinalMessage(HWND /*hWnd*/) { delete this; }

		BOOL PreTranslateMessage(MSG* pMsg) override
		{
			// Ctrl + A �������ꂽ
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'A' && ::GetKeyState(VK_CONTROL) < 0) {
				m_Edit.SetSelAll(TRUE);
				return TRUE;
			}

			// RButtonHook
			if (pMsg->message == WM_RBUTTONDOWN) {
				MouseGestureData	data;
				data.hwnd	= pMsg->hwnd;
				data.wParam	= pMsg->wParam;
				data.lParam	= pMsg->lParam;
				CString selectedText;
				int start = 0, end = 0;
				m_Edit.GetSel(start, end);
				if (start < end)
					selectedText = MtlGetWindowText(m_Edit).Mid(start, end - start);
				data.bCursorOnSelectedText	= m_bUseRightDragSearch && selectedText.GetLength() > 0/* && _CursorOnSelectedText()*/;
				data.strSelectedTextLine	= selectedText;
				
				CString strSharedMemName;
				strSharedMemName.Format(_T("%s0x%x"), MOUSEGESTUREDATASHAREDMEMNAME, m_hWnd);
				CSharedMemoryHandle	sharedMem;
				sharedMem.Serialize(data, strSharedMemName);

				GetTopLevelWindow().SetCapture();
				GetTopLevelWindow().PostMessage(WM_MOUSEGESTURE, (WPARAM)m_hWnd, (LPARAM)sharedMem.Handle());
				return FALSE;
			}

			return FALSE;
		}

		BEGIN_MSG_MAP( CTextWindow )
			MSG_WM_CREATE( OnCreate )
			MSG_WM_DESTROY( OnDestroy )
			MSG_WM_ACTIVATE( OnActivate )
			MSG_WM_SIZE( OnSize )
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			// message loop
			m_pLoop->AddMessageFilter(this);

			WTL::CLogFont	lf;
			lf.SetMenuFont();
			m_Edit.Create(m_hWnd, 0, NULL, WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL /*| WS_VSCROLL*/);
			CFontHandle font = lf.CreateFontIndirect();
			m_Edit.SetFont(font);
			//m_Edit.MoveWindow(0, 0, 250, 270);
			m_Edit.ShowWindow(SW_SHOWNOACTIVATE);
			m_Edit.AppendText(m_strWord, TRUE);

			CRect rcText = MtlComputeRectOfText(m_strWord, font);
			BOOL b = ::AdjustWindowRectEx(&rcText, WS_POPUP | WS_DLGFRAME, FALSE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
			enum { kSpacing = 10 };

			CRect rcWindow;
			rcWindow.top	= m_ptTopLeft.y - rcText.Height() - kSpacing;
			rcWindow.left	= m_ptTopLeft.x;
			rcWindow.right	= rcWindow.left + rcText.Width() + ::GetSystemMetrics(SM_CXVSCROLL);
			rcWindow.bottom	= rcWindow.top + rcText.Height();
			
			MoveWindow(&rcWindow);
			
			return 0;
		}

		void OnDestroy()
		{
			m_pLoop->RemoveMessageFilter(this);

			m_Edit.DestroyWindow();
			PostQuitMessage(0);
		}

		void OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
		{
			if (nState == WA_INACTIVE)
				PostMessage(WM_CLOSE);
		}

		void OnSize(UINT nType, CSize size)
		{
			if (size.cx != 0 && size.cy !=0) {
				CRect rc;
				GetWindowRect(&rc);
				//AdjustWindowRectEx(&rc, WS_THICKFRAME, FALSE, WS_EX_TOOLWINDOW);
				m_Size.SetSize(rc.Width(), rc.Height());
			}
			if (m_Edit.IsWindow())
				m_Edit.MoveWindow(0, 0, size.cx, size.cy);
		}


	private:
		CEdit	m_Edit;
		CString m_strWord;
		CPoint	m_ptTopLeft;
		CSize	m_Size;
		CMessageLoop*	m_pLoop;
		bool	m_bUseRightDragSearch;
	};

	CMessageLoop loop;

	auto window = new CTextWindow(text, rc.TopLeft(), &loop, m_pGlobalConfig->bUseRightDragSearch);
	window->Create(m_hWnd);
	//window->MoveWindow(&rcWindow);
	window->ShowWindow(TRUE);

	loop.Run();

	return true;
}




