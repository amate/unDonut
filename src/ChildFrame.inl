/**
*	@file	ChildFrame.inl
*	@brief	CChildFrame::Impl の定義
*/


CChildFrame::Impl::Impl(CChildFrame* pChild) : 
	m_pParentChild(pChild), 
	m_pThreadRefCount(nullptr), 
	m_view(m_UIChange),
	m_BingTranslatorMenu(pChild),
	m_bNowActive(false),
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
	m_pPageBitmap(nullptr),
	m_hMapChildFrameData(NULL),
	m_dwThreadIdFromNewWindow(0)
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

/// 選択範囲の文字列を取得(フレーム対応版)
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
		//+++ 選択範囲がない場合、右クリックメニュー経由のことがあるので、それ対策を強引に...
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


/// 選択されたテキストを一行だけ返す
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

	// mailto: 無効
	if (m_view.GetExStyle() & DVS_EX_BLOCK_MAILTO) {
		if (strURL.Left(7).CompareNoCase( _T("mailto:") ) == 0) {
			bCancel = true;
			return;
		}
	}

	// Navigate中かつjavescriptから始まるウィンドウはナビゲートしない
	if (m_bNowNavigate && 
		strURL.Left(15).CompareNoCase(_T("javascript:void")) == 0 || strURL.CompareNoCase(_T("javascript:;")) == 0) 
	{
		bCancel = true;
		return;
	}

	// ユーザースクリプトをインストールするかどうか
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

		if (m_view.m_bLightRefresh) {
			m_view.m_bLightRefresh = false;	// 手動でセキュリティを設定したので何もしない
		} else	{	
			// URL別セキュリティの設定
			if (m_pGlobalConfig->bUrlSecurityValid) {
				DWORD exopts	= 0xFFFFFFFF;
				DWORD dlCtlFlg	= 0xFFFFFFFF;
				DWORD exstyle	= 0xFFFFFFFF;
				DWORD autoRefresh = 0xFFFFFFFF;
				DWORD dwExPropOpt = 8;
				CString nowLocation = GetLocationURL();
				if (m_UrlSecurity.IsUndoSecurity(nowLocation)) {
					m_view.PutDLControlFlags(m_pGlobalConfig->dwDLControlFlags);
					SetExStyle(m_pGlobalConfig->dwExtendedStyleFlags);
				}
				if (m_UrlSecurity.FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
					CExProperty  ExProp(m_pGlobalConfig->dwDLControlFlags, m_pGlobalConfig->dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
					dlCtlFlg	= ExProp.GetDLControlFlags();
					exstyle		= ExProp.GetExtendedStyleFlags();
					autoRefresh = ExProp.GetAutoRefreshFlag();
				}

				//+++ url別拡張プロパティの処理....
				//+++	戻る・進むでの拡張プロパティ情報の頁ごとの保存ができていないので、破綻する...
				//+++	が、バグっても反映されることのほうが意味ありそうなので利用
				if (exopts != 0xFFFFFFFF) {
					m_view.PutDLControlFlags( dlCtlFlg );
					m_view.SetAutoRefreshStyle( autoRefresh );
					SetExStyle( exstyle );	//+++メモ: マウス中ボタンクリックでの、リンク別タブ表示の場合、まだタブ位置未決定のため設定できない...
				}
			}
		}

		/* Faviconを白紙に設定 */
		if (strURL.Left(11).CompareNoCase(_T("javascript:")) != 0) {
			m_UIChange.SetLocationURL(strURL);
			_SetFavicon(strURL);
		}

	}
	
	m_bNowNavigate = true;	// Navigate中である
}

void	CChildFrame::Impl::OnDownloadComplete()
{
	m_bNowNavigate = false;	// Navigate終了
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
	//m_bPrivacyImpacted = TRUE;	// ページ読み込み後もOnStateConnectingが呼ばれるページがあるので
								// クッキー制限アイコンが表示されないページがある
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

/// documentが操作できるようになった
void	CChildFrame::Impl::OnDocumentComplete(IDispatch *pDisp, const CString& strURL)
{
	if ( IsPageIWebBrowser(pDisp) ) {
		// 自動リサイズの設定を初期化
		m_bImagePage	= false;
		m_nImgSclSw		= (m_pGlobalConfig->AutoImageResizeType == AUTO_IMAGE_RESIZE_FIRSTON);
		m_ImageSize.SetSize(0, 0);

		/* ページ内検索の情報をリセット */
		m_nPainBookmark = 0;
		m_strBookmark	= LPCOLESTR(NULL);

		_InitTravelLog();	// トラベルログを設定

		_AutoImageResize(true);	// 画像の自動リサイズ

		if (m_bNowActive && MtlIsApplicationActive(m_hWnd))
			_SetFocusToHTML();

		_SetFavicon(strURL);

		/* ユーザー定義Javascript */
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
		/* ユーザー定義CSS */
		//if (auto value = CUserDefinedCSSOption::FindURL(strURL)) {
		//	CComBSTR strCssPath = value.get();
		//	MtlForEachHTMLDocument2g(m_spBrowser, [strCssPath](IHTMLDocument2* pDoc) {
		//		CComPtr<IHTMLStyleSheet>	spStyleSheet;
		//		pDoc->createStyleSheet(strCssPath, -1, &spStyleSheet);
		//	});
		//}

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
			//DeleteMinimumLengthWord(strWords);
			m_bNowHilight = true;
			_HilightOnce(pDisp, strWords);
		}
	}
}

/// ブラウザのコマンドの状態が変化した(戻る、進むなどの)
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
	if (m_UrlSecurity.IsUndoSecurity(GetLocationURL())) {	// このURLでのみ有効なら戻す
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
#if 1
	//bCancel	= true;
	m_bExecutedNewWindow = true;

	NewChildFrameData	data(GetParent());
	data.strURL	= _T("");
	DWORD	dwDLCtrl	= _GetInheritedDLCtrlFlags();
	DWORD	dwExStyle	= _GetInheritedExStyleFlags();
	if (m_UrlSecurity.IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= m_pGlobalConfig->dwDLControlFlags;
		dwExStyle	= m_pGlobalConfig->dwExtendedStyleFlags;
	}
	data.dwDLCtrl	= dwDLCtrl;
	data.dwExStyle	= dwExStyle;
	data.bLink	= true;
	data.searchWord	= m_strSearchWord;
	data.bAutoHilight= m_bNowHilight;
	data.dwThreadIdFromNewWindow = ::GetCurrentThreadId();

	class CThreadObserver : public CMessageFilter
	{
	public:
		CThreadObserver(IDispatch** ppDisp, NewChildFrameData& data, bool bMode) 
			: m_ppDisp(ppDisp)
			, m_data(data)
			, m_bMultiProcessMode(bMode)
		{	}

		virtual BOOL PreTranslateMessage(MSG* pMsg)
		{
			switch (pMsg->message) {
			case WM_EXECUTECHILDFRAMETHREADFROMNEWWINDOW2:
				{
					if (m_bMultiProcessMode)
						MultiThreadManager::AddChildThread(&m_data);
					else
						MultiThreadManager::ExecuteChildFrameThread(new CChildFrame, &m_data);
					return TRUE;
				}

			case WM_GETMARSHALIDISPATCHINTERFACE:
				{
					IStream* pStream = (IStream*)pMsg->wParam;
					IDispatch* pDisp;
					ATLVERIFY(::CoGetInterfaceAndReleaseStream(pStream, IID_IDispatch, (void**)&pDisp) == S_OK);
					*m_ppDisp = pDisp;
					PostQuitMessage(0);
					return TRUE;
				}				
			}
			return FALSE;
		}
	private:
		IDispatch**	m_ppDisp;
		NewChildFrameData& m_data;
		bool	m_bMultiProcessMode;
	};

	CMessageLoop loop;
	CThreadObserver threadObserver(ppDisp, data, m_pGlobalConfig->bMultiProcessMode);
	loop.AddMessageFilter(&threadObserver);
	
	PostThreadMessage(::GetCurrentThreadId(), WM_EXECUTECHILDFRAMETHREADFROMNEWWINDOW2, 0, 0);
	int nRet = loop.Run();

	loop.RemoveMessageFilter(&threadObserver);
	//CChildFrame::AsyncCreate(data);
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
	if (m_UrlSecurity.IsUndoSecurity(GetLocationURL())) {
		dwDLCtrl	= CDLControlOption::s_dwDLControlFlags;
		dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags;
	}
	data.dwDLCtrl	= dwDLCtrl;
	data.dwExStyle	= dwExStyle;
	data.bLink	= true;
	CString strSearchWord = m_strSearchWord;
	bool	bNowHilight	= m_bNowHilight;
	//data.funcCallAfterCreated	= [strSearchWord, bNowHilight](CChildFrame* pChild) {
	//	pChild->SetSearchWordAutoHilight(strSearchWord, bNowHilight);
	//};
	CChildFrame::AsyncCreate(data);
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
	if (dwLinkOpenBtnM == 0)	// ボタンが設定されていなかったら、終わり.
		return FALSE;

	//+++ メモ：デフォルトでは dwLinkOpenBtmM = IDM_FOLLOWLINKN
	// ATLASSERT(dwLinkOpenBtnM == IDM_FOLLOWLINKN);

	//+++ カーソルがリンクをさしていたらstatusBar用のメッセージがあることを利用.
	bool	bLink = false;
	if (   m_strStatusText.IsEmpty() == FALSE
		&& m_strStatusText != _T("ページが表示されました") 
		&& m_strStatusText != _T("完了") )			//+++ リンクのないページでは"ページが表示されました"というメッセージが設定されているので除外.
		bLink = true;

	::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 				//+++ おまじない.
	//int 	nTabCnt = m_MDITab.GetItemCount();									//+++ リンクをクリックしたかどうかのチェック用.
	//pChild->m_bAllowNewWindow = true;
	m_bExecutedNewWindow = false;
	::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);					//+++ リンクを新しい窓にひらく
	if (m_bExecutedNewWindow)
		return TRUE;
	//pChild->m_bAllowNewWindow = false;
	//int 	nTabCnt2 = m_MDITab.GetItemCount();
	//if (nTabCnt != nTabCnt2 || bLink)											//+++ リンクメッセージがあるか、タブが増えていたら、リンクをクリックしたとする.
	///	return TRUE;															//+++ trueを返して中ボタンクリックの処理をやったことにする.

	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ リンク以外をクリックした場合おまじないの左クリック押しを終了しておく.
	return FALSE;																//+++ falseを返すことで、IEコンポーネントにウィールクリックの処理を任せる.
}


BOOL CChildFrame::Impl::OnRButtonHook(MSG* pMsg)
{
	if ( !(m_view.GetExStyle() & DVS_EX_MOUSE_GESTURE) )
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
	bool	bNoting = true;	// 何もしなかった
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
			if (bNoting == false)	// 他のコマンドを実行済み
				break;
			CPoint	ptNow(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			::ClientToScreen(msg.hwnd, &ptNow);
			if (bCursorOnSelectedText) {
				if (nDistance < 10) {
					nDistance = PointDistance(ptDown, ptNow);	// 距離を求める
					if (nDistance < 10)
						break;
				}
				SetCursor(cursor);	// カーソルを変更する

				CString strMark;
				if (m_pGlobalConfig->bUseRect) {
					int nAng  = (int) _GetAngle(ptDown, ptNow);	// 角度を求める
					if		  (nAng <  45 || nAng >  315) {
						strSearchEngine =m_pGlobalConfig->strREngine;	
						strMark = _T("[→] ");
					} else if (nAng >= 45 && nAng <= 135) {
						strSearchEngine = m_pGlobalConfig->strTEngine;	
						strMark = _T("[↑] ");
					} else if (nAng > 135 && nAng <  225) {
						strSearchEngine = m_pGlobalConfig->strLEngine;	
						strMark = _T("[←] ");
					} else if (nAng >= 225 && nAng <= 315) {
						strSearchEngine = m_pGlobalConfig->strBEngine;
						strMark = _T("[↓] ");
					}
				} else {
					strSearchEngine = m_pGlobalConfig->strCEngine;
				}
				if (strSearchEngine.IsEmpty() == FALSE) {
					CString strMsg;
					strMsg.Format(_T("検索 %s: %s"), strMark, strSearchEngine);
					m_UIChange.SetStatusText(strMsg);
				} else {
					m_UIChange.SetStatusText(_T(""));
				}
			} else {
				nDistance = PointDistance(ptLast, ptNow);	// 距離を求める
				if (nDistance < 10)
					break;
				
				CString strMark1;
				int nAng1  = (int) _GetAngle(ptLast, ptNow);	// 角度を求める
				if		(nAng1 <  45 || nAng1 >  315)
					strMark1 = _T("→");
				else if (nAng1 >= 45 && nAng1 <= 135)
					strMark1 = _T("↑");
				else if (nAng1 > 135 && nAng1 <  225)
					strMark1 = _T("←");
				else if (nAng1 >= 225 && nAng1 <= 315)
					strMark1 = _T("↓");

				if (strMark1 == strLastMark) {					// 同じ方向に動かして、かつ300ms以上経ったなら有効		
					DWORD dwTimeNow = ::GetTickCount();
					if ( (dwTimeNow - dwTime) > 300 ) {
						strLastMark = _T("");
						dwTime	 = dwTimeNow;
					}
				}
				if (strMark1 != strLastMark) {
					strMove	+= strMark1;	// 方向を追加
					strLastMark = strMark1;	

					CString strCmdName;
					CIniFileI	pr( GetConfigFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
					DWORD	dwCommand = pr.GetValue(strMove);
					if (dwCommand) {
						// 合致するコマンドがあれば表示
						CString strTemp;
						CToolTipManager::LoadToolTipText(dwCommand, strTemp);
						strCmdName.Format(_T("[ %s ]"), strTemp);
					}

					// ステータスバーに表示
					CString 	strMsg;
					strMsg.Format(_T("ジェスチャー : %s %s"), strMove, strCmdName);
					m_UIChange.SetStatusText(strMsg);
				}
				dwTime = ::GetTickCount();
			}
			ptLast = ptNow;
			break;
						   }

		case WM_LBUTTONDOWN:
			if (bCursorOnSelectedText && nDistance >= 10) {	// 右ボタンドラッグをキャンセルする
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
			PostMessage(WM_CLOSE);	// これだけじゃたぶんダメ
			msg.message = WM_RBUTTONUP;
			//::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
			//::PostMessage(hWnd, WM_CLOSE, 0, 0);
			bNoting    = false;
			m_UIChange.SetStatusText(_T(""));
			break;

		case ID_GET_OUT:				// 退避
		case ID_VIEW_FULLSCREEN:		// 全体表示
		case ID_VIEW_UP:				// 上へ
		case ID_VIEW_BACK:				// 前に戻る
		case ID_VIEW_FORWARD:			// 次に進む
		case ID_VIEW_STOP_ALL:			// すべて中止
		case ID_VIEW_REFRESH_ALL:		// すべて更新
		case ID_WINDOW_CLOSE_ALL:		// すべて閉じる
		case ID_WINDOW_CLOSE_EXCEPT:	// これ以外閉じる
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
		SetCursor(::LoadCursor(NULL, IDC_ARROW));	// カーソルを元に戻す
		if (m_bCancelRButtonUp)
			return TRUE;
		if (strSearchEngine.IsEmpty() == FALSE) {	// 右ボタンドラッグ実行
			m_UIChange.SetStatusText(_T(""));
			CString str = GetSelectedTextLine();
			_SearchWebWithEngine(str, strSearchEngine);
			bNoting = false;
		}
	}

	/* マウスジェスチャーコマンド実行 */
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

	if ( bNoting && strMove.IsEmpty() ) {	// 右クリックメニューを出す
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
#if 0	// http://digital-mixnews.com/archives/67603987.html で固まるバグ修正だけどこれやるとyoutubeでサムネイル読み込まなくなる...
	if (pMsg->message == (WM_APP + 3)) {
		WCHAR classname[32] = L"";
		::GetClassName(pMsg->hwnd, classname, 32);
		if (::lstrcmp(classname, L"Internet Explorer_Hidden") == 0)
			return TRUE;
	}
#endif

	// 自動リサイズのトグル
	if (   m_bImagePage 
		&& pMsg->message == WM_LBUTTONDOWN 
		&& m_pGlobalConfig->AutoImageResizeType != AUTO_IMAGE_RESIZE_NONE)
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

	// ミドルクリック
	if ( pMsg->message == WM_MBUTTONDOWN && IsChild(pMsg->hwnd) && OnMButtonHook(pMsg) )
		return TRUE;

	// 右ドラッグキャンセル用
	//if ( pMsg->message == WM_RBUTTONUP && m_bCancelRButtonUp ) {
	//	m_bCancelRButtonUp = false;
	//	return TRUE;
	//}
	// マウスジェスチャーへ
	if ( pMsg->message == WM_RBUTTONDOWN && OnRButtonHook(pMsg) )
		return TRUE;

	// サイドボタン
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam)) )
			return TRUE;
	}

	// メインフレームのマウスジェスチャー用にホイールを通知する
	if (pMsg->message == WM_MOUSEWHEEL) {
		CWindow wndMainFrame = GetTopLevelWindow();
		if (GetCapture() == wndMainFrame) {
			wndMainFrame.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		} else {
			UINT nFlags = (UINT)LOWORD(pMsg->wParam);
			int	 zDelta = (short)HIWORD(pMsg->wParam);
			if (nFlags == MK_CONTROL) {		// 文字サイズ変更
				CComVariant	vEmpty;
				CComVariant vZoomSize;
				m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vEmpty, &vZoomSize); 
				if ( zDelta > 0 ){	
					vZoomSize.lVal += 1;
				} else {
					vZoomSize.lVal -= 1;
				}
				m_spBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoomSize, &vEmpty);
			} else if (::GetKeyState(VK_MENU) < 0) {	// 拡大/縮小
				if ( zDelta > 0 ){	
					OnHtmlZoom(0, ID_HTMLZOOM_ADD, NULL);
				} else {
					OnHtmlZoom(0, ID_HTMLZOOM_SUB, NULL);
				}
			} else {	// カーソル下の自分のウィンドウにホイールスクロールを通知する
				CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
				HWND hWndFound = WindowFromPoint(pt);
				if (hWndFound) {
					if (IsChild(hWndFound)) {
						::SendMessage(hWndFound, pMsg->message, pMsg->wParam, pMsg->lParam);
					} else if (wndMainFrame.m_hWnd == hWndFound || wndMainFrame.IsChild(hWndFound)) {
						// ホイールでタブ切替用
						wndMainFrame.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
					}
				}
			}
			return TRUE;
		}
	}

	// アクセラレータキー
	if (m_AcceleratorOption.TranslateAccelerator(m_hWnd, pMsg))
		return TRUE;
	//if (g_pMainWnd->m_hAccel != NULL && ::TranslateAccelerator(m_hWnd, g_pMainWnd->m_hAccel, pMsg))
	//		return TRUE;
	return m_view.PreTranslateMessage(pMsg);
}

/// 選択中の文字列を検索(アドレスバーでCtrl+Enterしたときの検索エンジンが使われる)
void CChildFrame::Impl::searchEngines(const CString& strKeyWord)
{
	CString 	strSearchWord = strKeyWord;

	if (m_pGlobalConfig->bReplaceSpace)
		strSearchWord.Replace( _T("　"), _T(" ") );

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
		return 0;	//数値ではない

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
#if 0
	if (m_pPageBitmap) {
		CMemoryDC	memDC(dc, rect);
		memDC.SelectBitmap(*m_pPageBitmap);
	}
#endif
#if 1
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

	m_UIChange.SetChildFrame(m_hWnd);
	m_UIChange.AddCommandUIMap();

	m_GlobalConfigManageData = GetGlobalConfig(GetTopLevelWindow());
	m_pGlobalConfig	= m_GlobalConfigManageData.pGlobalConfig;
	m_view.SetGlobalConfig(m_pGlobalConfig);
	OnSetProxyToChildFrame(0, 0, 0);	// プロクシ設定

	m_UrlSecurity.SetMainFrameHWND(GetTopLevelWindow());
	m_UrlSecurity.ReloadList();

	m_AcceleratorOption.ReloadAccelerator(GetTopLevelWindow());

	if (m_pGlobalConfig->bMultiProcessMode &&
		CCustomContextMenuOption::s_menuDefault.IsNull())
		CCustomContextMenuOption::ReloadCustomContextMenuList(GetTopLevelWindow());

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
		TRACEIN(_T("ChildFreameスレッドの破棄"));
		PostQuitMessage(0);
	}

	m_UIChange.RemoveCommandUIMap();
	CloseGlobalConfig(&m_GlobalConfigManageData);
}

void	CChildFrame::Impl::OnClose()
{
	SetMsgHandled(FALSE);

	CWindow	wndMain = GetTopLevelWindow();

	ChildFrameDataOnClose	ClosedTabData;
	_CollectDataOnClose(ClosedTabData);

	CString strSharedMemName;
	strSharedMemName.Format(_T("%s%#x"), CHILDFRAMEDATAONCLOSESHAREDMEMNAME, m_hWnd);
	CSharedMemory sharedMem;
	sharedMem.Serialize(ClosedTabData, strSharedMemName);

	wndMain.SendMessage(WM_ADDRECENTCLOSEDTAB, (WPARAM)m_hWnd);

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
		OnHilight(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
		break;

	case kNavigateChildFrame:
		{
			NavigateChildFrame	data;
			std::wstringstream	ss;
			ss << static_cast<LPWSTR>(pCopyDataStruct->lpData);
			boost::archive::text_wiarchive	ar(ss);
			ar >> data;
		
			if (data.dwDLCtrl != -1)
				SetDLCtrl(data.dwDLCtrl);
			if (data.dwExStyle != -1)
				SetExStyle(data.dwExStyle);
			if (data.dwAutoRefresh)
				SetAutoRefreshStyle(data.dwAutoRefresh);
			Navigate2(data.strURL);
		}
		break;

	case kExecuteUserJavascript:
		_ExecuteUserJavascript(static_cast<LPCTSTR>(pCopyDataStruct->lpData));
		break;

	case kHilightFromFindBar:
		{
			CString strData = static_cast<LPCTSTR>(pCopyDataStruct->lpData);
			CString strdw = strData[0];
			CString strFlags = strData[1];
			DWORD dw = _wtoi(strdw);
			long flags = _wtoi(strFlags);
			return _HilightFromFindBar(strData.Mid(2), (dw & 0x1) != 0, (dw & 0x2) != 0, flags);
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

void	CChildFrame::Impl::OnChildFrameActivate(HWND hWndAct, HWND hWndDeact)
{
	if (hWndAct == m_hWnd) {
		m_bNowActive = true;
		if (MtlIsApplicationActive(m_hWnd)) {
			//SetForegroundWindow(m_hWnd);
			_SetFocusToHTML();
		}

		if (m_pGlobalConfig->bSaveSearchWord) {
			CString str;
			str.Format(_T("%d%s"), m_bNowHilight, m_strSearchWord);
			COPYDATASTRUCT cds;
			cds.dwData	= kSetSearchText;
			cds.lpData	= static_cast<LPVOID>(str.GetBuffer(0));
			cds.cbData	= (str.GetLength() + 1) * sizeof(WCHAR);
			GetTopLevelWindow().SendMessage(WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
		//	//CDonutSearchBar::GetInstance()->SetSearchStr(m_strSearchWord); //\\ 保存しておいた文字列を検索バーに戻す
		//	//CDonutSearchBar::GetInstance()->ForceSetHilightBtnOn(m_bNowHilight);
		} else {
			m_pGlobalConfig->bSaveSearchWord = m_pGlobalConfig->bSaveSearchWordOrg;
		}


		if (Misc::IsGpuRendering()) {
			//SetRedraw(FALSE);
			RECT rcClient;
			GetParent().GetClientRect(&rcClient);
			::SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | *//*SWP_NOZORDER |*/ SWP_NOREDRAW);
			::ShowWindow(m_hWnd, TRUE);
			//::BringWindowToTop(m_hWnd);

			if (hWndDeact) {
				::ShowWindowAsync(hWndDeact, FALSE);
			}
			//m_view.GetWindow(GW_CHILD).RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
			//SetRedraw(TRUE);
			//PostMessage(WM_APP + 123);
			//m_view.GetWindow(GW_CHILD).RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
			//GetParent().RedrawWindow(NULL, NULL, /*RDW_FRAME | RDW_ERASE | RDW_ERASENOW | */RDW_INVALIDATE/* | RDW_UPDATENOW */| RDW_ALLCHILDREN);
		} else {	// no GPU で成功！
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
			//GetParent().RedrawWindow(NULL, NULL, /*RDW_FRAME | RDW_ERASE | RDW_ERASENOW | */RDW_INVALIDATE/* | RDW_UPDATENOW */| RDW_ALLCHILDREN);
		}
#if 0
		{	// アクティブ先のウィンドウが表示されない...
			GetParent().SetRedraw(FALSE);
			RECT rcClient;
			GetParent().GetClientRect(&rcClient);
			::SetWindowPos(m_hWnd, hWndDeact, 0, 0, rcClient.right, rcClient.bottom, /*SWP_ASYNCWINDOWPOS | *//*SWP_NOZORDER |*/ SWP_SHOWWINDOW/* | SWP_NOREDRAW*/);
			//::ShowWindow(m_hWnd, TRUE);
			::BringWindowToTop(m_hWnd);
			if (hWndDeact)
				::ShowWindow/*Async*/(hWndDeact, FALSE);
			GetParent().SetRedraw(TRUE);
			//GetParent().RedrawWindow(NULL, NULL, /*RDW_FRAME | RDW_ERASE | RDW_ERASENOW | */RDW_INVALIDATE/* | RDW_UPDATENOW */| RDW_ALLCHILDREN);
		}
#endif
	} else if (hWndDeact == m_hWnd) {
		m_bNowActive = false;
		// _KillFocusToHTML
		HRESULT	hr = E_FAIL;
		CComQIPtr<IOleInPlaceObject> spIOleInPlaceObject = m_spBrowser;
		if(spIOleInPlaceObject) {
			hr = spIOleInPlaceObject->UIDeactivate(); // IEのUIを無効化
		}

		if(m_pGlobalConfig->bSaveSearchWord){	//\\ 現在、検索バーにある文字列を取っておく
			::SendMessage(m_pGlobalConfig->SearchEditHWND, WM_GETTEXT, 1024, (LPARAM)m_strSearchWord.GetBuffer(1024));
			m_strSearchWord.ReleaseBuffer();
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


/// 現在選択されているテキストを返す
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

/// プロクシを設定する
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



/// ファイルを閉じる
void 	CChildFrame::Impl::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//+++ タブロック対策... OnClose側のだけで大丈夫のようだけれど、とりあえず.
	if (   //::: s_bMainframeClose == false											//+++ unDonut終了時以外で
		/*&&*/ _check_flag(m_view.GetExStyle(), DVS_EX_OPENNEWWIN)					//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK) )	//+++ ナビゲートロックのページを閉じない、の指定があれば
	{
		return; 																//+++ 閉じずに帰る
	}

	PostMessage(WM_CLOSE);
}

// 検索バーから

/// ハイライト
LRESULT CChildFrame::Impl::OnHilight(const CString& strKeyWord)
{
	bool bHilightSw = m_pGlobalConfig->bHilightSwitch;
	m_bNowHilight	= bHilightSw;
	if (bHilightSw) {
		m_strSearchWord = strKeyWord;
//		DeleteMinimumLengthWord(strKeyWord);
	} else {
		m_strSearchWord.Empty();
	}
	MtlForEachHTMLDocument2g(m_spBrowser, _Function_SelectEmpt());
	MtlForEachHTMLDocument2g(m_spBrowser, _Function_Hilight2(strKeyWord, bHilightSw));

	return TRUE;
}

/// ページ内検索
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
	CComQIPtr<IHTMLDocument2>	spDocument = spDisp;	// htmlの取得
	if (!spDocument)
		return 0;

	CString strKeyword = findKeywordData.strKeyword;
	strKeyword.Replace(_T('ﾞ'), _T('゛'));

	// 検索
	BOOL	bSts = _FindKeyWordOne(spDocument, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
	if (bSts)
		return TRUE;

	// フレームウィンドウの取得
	CComPtr<IHTMLFramesCollection2> 	spFrames;
	hr = spDocument->get_frames(&spFrames);
	// cf. Even if no frame, get_frames would succeed.
	if ( FAILED(hr) )
		return 0;

	// フレーム内ウィンドウの数を取得
	LONG	nCount	   = 0;
	hr = spFrames->get_length(&nCount);
	if ( FAILED(hr) )
		return 0;

	BOOL	bFindIt    = FALSE;
	if (findKeywordData.bFindDown) {	// ページ内検索 - 下
		for (LONG ii = m_nPainBookmark; ii < nCount; ii++) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// フレーム内のウィンドウを取得
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

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
			if (bFindIt) {
				m_nPainBookmark = ii;
				break;
			}
		}

		if (!bFindIt) {
			m_nPainBookmark = 0;
			m_strBookmark	= LPCOLESTR(NULL);
		}

	} else {			// ページ内検索 - 上
		if (m_nPainBookmark == 0 && !m_strBookmark)
			m_nPainBookmark = nCount - 1;

		for (LONG ii = m_nPainBookmark; ii >= 0; ii--) {
			CComVariant 			varItem(ii);
			CComVariant 			varResult;

			// ウィンドウの取得
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

			// 検索
			bFindIt = _FindKeyWordOne(spDocumentFr, strKeyword, findKeywordData.bFindDown, findKeywordData.Flags);
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
		CComPtr<IHTMLSelectionObject> spSelection;	/* テキスト選択を空にする */
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


void	CChildFrame::Impl::OnRemoveHilight()
{
	CComPtr<IDispatch>	spDisp;
	m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument3> spDoc3 = spDisp;
	if (spDoc3)
		_RemoveHighlight(spDoc3);
}


/// ポップアップズームメニューを開く
void	CChildFrame::Impl::OnHtmlZoomMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::SetForegroundWindow(m_hWnd);

	CMenu	menu0;
	menu0.LoadMenu(IDR_ZOOM_MENU);
	ATLASSERT(menu0.IsMenu());
	CMenuHandle menu = menu0.GetSubMenu(0);
	ATLASSERT(menu.IsMenu());

	// ポップアップメニューを開く.
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

/// 画像を保存する
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
		// Hit先が画像の場合
		CComQIPtr<IHTMLImgElement>	spImg = spHitElement;
		if (spImg == NULL) {
			// フレームの可能性
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
					return;	// 別ドメイン
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
			if (scl != 100 && m_ImageSize != CSize(0, 0))		// スクロールバーを隠す
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"hidden"));
			else 
				pHtmlStyle->setAttribute(CComBSTR(L"overflow"), CComVariant(L"auto"));
		}
	};	// lamda

	switch (nID) {
	case ID_HTMLZOOM_ADD:	SetBodyStyleZoom(+10, 0, wndCtl == 0);	break;
	case ID_HTMLZOOM_SUB:	SetBodyStyleZoom(-10, 0, wndCtl == 0);	break;

	case ID_HTMLZOOM_100TOGLE:
		if (m_nImgScl == 100) {	// 100% なら元の拡大率に
			m_nImgScl	 = m_nImgSclSav;
			m_nImgSclSw	 = 1;
		} else {				// それ以外なら 100% に
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


/// 選択範囲のリンクを開く
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
			if (bNoAddFromMenu == false) {	// 右クリックメニューから取ってくる
				bNoAddFromMenu = true;
				CString strURL = m_view.GetAnchorURL();
				if (strURL.IsEmpty() == FALSE)
					arrUrls.Add(strURL);
			}
		} else {
			hr = pDocument->get_URL(&bstrLocationUrl);
			if ( FAILED(hr) )
				return;

			//BASEタグに対処する minit
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
				bNoAddFromMenu = true;	// 選択範囲からリンクが見つかったので
		}
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
			dwOpenFlags |= D_OPENFILE_ACTIVATE;	// リンクを開くときアクティブにしないに従う

		DonutOpenFile(arrUrls[i], dwOpenFlags);
	}
	//m_MDITab.SetLinkState(LINKSTATE_OFF);
#endif
}

/// URLテキストを開く : 選択文字列からURLっぽい文字列を取り出して開く
void 	CChildFrame::Impl::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	std::wstring strText = GetSelectedText();
	if (strText.empty())
		return;

	std::vector<CString>	vecUrls;
	std::wregex rx(L"(http(?:s|)://|)((?:[a-zA-Z0-9_\\-]+\\.)+\\w+(?::\\d+|)(?:/[a-zA-Z0-9./_\\-?#%&=+あ-んア-ンｱ-ﾝ一-龠]*|))");
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
	auto pair = CreateMultiText(vecUrls);
	OpenMultiUrl(pair, m_hWnd);
#if 0
	std::vector<CString> lines;
	lines.reserve(20);
	Misc::SeptTextToLines(lines, strText);
	size_t	size = lines.size();
	//+++ 選択文字列中にurlぽいものがなかったら、文字列検索にしてみる.
	bool	f	= 0;
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		f |= (strUrl.Find(_T(':')) >= 0) || (strUrl.Find(_T('/')) >= 0) || (strUrl.Find(_T('.')) >= 0) ||  (strUrl.Find(_T('\\')) >= 0);
		if (f)
			break;
	}
	if (f == 0) {	// urlぽい文字列がなかった...
		CDonutSearchBar*	pSearchBar = CDonutSearchBar::GetInstance();
		if (pSearchBar) {
			CString str	= lines[0];
			LPCTSTR	strExcept  = _T(" \t\"\r\n　");
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
		//Misc::StrToNormalUrl(strUrl);		//+++ 関数化
		DWORD dwOpenFlags = D_OPENFILE_CREATETAB;
		if (  i == (size - 1) 
			&& !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN))
			dwOpenFlags |= D_OPENFILE_ACTIVATE;	// リンクを開くときアクティブにしないに従う
		DonutOpenFile(strUrl, dwOpenFlags);
	}
#endif
}

/// ポップアップ抑止に追加して閉じます。
void 	CChildFrame::Impl::OnAddClosePopupUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
//	CIgnoredURLsOption::Add( GetLocationURL() );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// タイトル抑止に追加
void 	CChildFrame::Impl::OnAddClosePopupTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
//	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	//m_bClosing = true;
	PostMessage(WM_CLOSE);
}

/// nページ戻る
void	CChildFrame::Impl::OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_BACK1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoBack();
}

/// nページ進む
void	CChildFrame::Impl::OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_FORWARD1) + 1;
	CLockRedraw lock(m_hWnd);

	for (int i = 0; i < nStep; ++i)
		GoForward();
}

// 編集

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

/// タイトルをクリップボードにコピーします。
void	CChildFrame::Impl::OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(MtlGetWindowText(m_hWnd), m_hWnd);
}

/// アドレスをクリップボードにコピーします。
void	CChildFrame::Impl::OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(GetLocationURL(), m_hWnd);
}

/// タイトルとアドレスをクリップボードにコピーします。
void	CChildFrame::Impl::OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strText;
	strText.Format(_T("%s\r\n%s"), MtlGetWindowText(m_hWnd), GetLocationURL());
	MtlSetClipboardText(strText, m_hWnd);
}

// 表示

void	CChildFrame::Impl::OnViewStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	Stop();

	m_nDownloadCounter = 0;
	OnStateCompleted();
}

/// 一つ上の階層へ
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

/// ルート階層へ
void	CChildFrame::Impl::OnViewRoot(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strURL = GetLocationURL();
	int nPos = strURL.Find(_T('/'), 9);
	if (nPos == -1)
		return ;
	if (strURL.ReverseFind(_T('/')) == nPos) {	// すでルートなのでさらに遡る
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

/// プライバシーレポート
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

	if (Misc::getIEMejourVersion() >= 8 && _CheckOsVersion_VistaLater() == 0){//\\ XP+IE8の場合
		hInstDLL = ::LoadLibrary( _T("ieframe.dll") );
	} else {//vista+IE8の場合
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


/// 継承するDLコントロールを得る
DWORD	CChildFrame::Impl::_GetInheritedDLCtrlFlags()
{
	DWORD dwDLFlags = m_pGlobalConfig->dwDLControlFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, m_pGlobalConfig->dwMainExtendedStyle) )
		dwDLFlags = m_view.GetDLControlFlags();
	return dwDLFlags;
}

/// 継承するExStyleを得る
DWORD	CChildFrame::Impl::_GetInheritedExStyleFlags()
{
	DWORD dwExFlags = m_pGlobalConfig->dwExtendedStyleFlags;
	if ( _check_flag(MAIN_EX_INHERIT_OPTIONS, m_pGlobalConfig->dwMainExtendedStyle) ) {
		dwExFlags = m_view.GetExStyle();
	  #if 1	//+++ ナビゲートロックに関しては継承しない....
		dwExFlags &= ~DVS_EX_OPENNEWWIN;											//+++ off
		dwExFlags |= CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_OPENNEWWIN;	//+++ けど、デフォルト設定があれば、それを反映.
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

			if (frameslength == iframelength && spIFrameCol.p && spFrames.p) {	// インラインフレーム
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

			if (frameslength == framelength && spFrameCol.p && spFrames.p) {	// 普通のフレーム
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
			if ( FAILED(hr) ) {	// 別ドメイン
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

/// トラベルログを初期化する
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

			  #if 1	//+++ UNICODE 対応.
				std::vector<wchar_t>	title = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].first ) );
				std::vector<wchar_t>	url   = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].second) );
				// CreateEntry の第四引数がTRUEだと前に追加される
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

/// 最近閉じたタブ用のデータを集める
void	CChildFrame::Impl::_CollectDataOnClose(ChildFrameDataOnClose& data)
{	
	data.strTitle	= MtlGetWindowText(m_hWnd);
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
		if (FAILED(hr) || dwCount == 0)	// 単に履歴がないだけの状態ならtrue.
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
				vecLog.push_back(std::make_pair<WTL::CString, WTL::CString>(szTitle, szURL));
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

/// 自動画像リサイズ
void	CChildFrame::Impl::_AutoImageResize(bool bFirst)
{
	if (m_pGlobalConfig->AutoImageResizeType == AUTO_IMAGE_RESIZE_NONE)
		return ;
	TRACEIN(L"_AutoImageResize : bFirst(%d)", bFirst);
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
			TRACEIN(L"  mineType == %s  Ext == %s", strmineType, strExt);
			if (strExt != _T("JPG") && strExt != _T("PNG") && strExt != _T("GIF"))
				return ;
		} else {
			CString strUrl = GetLocationURL();
			if (GetLocationName() == strUrl) {
				CString strExt = strUrl.Right(4);
				strExt.MakeUpper();
				if (strExt != _T(".JPG") && strExt != _T(".PNG") && strExt != _T(".GIF"))
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

	if (bFirst) {	// 画像のサイズを得る
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
	TRACEIN(L"  リサイズ成功");
}

/// タブなどにFaviconを設定
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
	if (strFaviconURL.IsEmpty()) {	// ルートにあるFaviconのアドレスを得る
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
	// ドキュメントがNULLなら終了
	if (!pDocument)
		return FALSE;

	HRESULT	hr = S_OK;
	// キーワードを一語取得
	//x strKeyWord = strtok( (LPSTR) strKeyWord.GetBuffer(0), " " );
	CString 	strKeyWord = strKeyword;//\\Misc::GetStrWord( strKeyword );
	LPCTSTR		strExcept  = _T(" \t\"\r\n　");
	strKeyWord.TrimLeft(strExcept);
	strKeyWord.TrimRight(strExcept);

	// <body>を取得
	CComPtr<IHTMLElement>		spHTMLElement;
	pDocument->get_body(&spHTMLElement);
	CComQIPtr<IHTMLBodyElement> spHTMLBody = spHTMLElement;
	if (!spHTMLBody)
		return FALSE;

	// テキストレンジを取得
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
				spTxtRange->collapse(false);	// Caretの位置を選択したテキストの一番下に
				if (Misc::getIEMejourVersion() < 9)
					spTxtRange->moveStart(CComBSTR(L"character"), 1, &lActual);
				//spTxtRange->moveEnd(CComBSTR("Textedit"), 1, &lActual);
			} else {
				spTxtRange->collapse(true);	// Caretの位置を選択したテキストの一番上に
				//if (Misc::getIEMejourVersion() < 9) 
				//	spTxtRange->moveEnd  (CComBSTR(L"character"), -10, &lActual);			
			}
		}
	} else {	// 検索範囲を全体にする
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

		auto funcMove = [&spTxtRange, bFindDown] () {	// 検索範囲を変更する関数
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
					(strvisibility && strvisibility == _T("hidden"))) {	// 表示されていない場合はスキップ
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
			break;	/* 終わり */

		//++nSearchCount;
		//if (nSearchCount > 5)	// 5以上で打ち止め
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
		long		width = 0;
		hr	= pElem->get_offsetHeight(&height); 	// HTML表示領域の高さ
		hr  = pElem->get_offsetWidth(&width);
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
		hr	= pTxtRM->get_offsetTop(&y);		// 選択部分の画面上からのy座標
		if ( FAILED(hr) )
			return;

		long scy = y - height / 2;				// 画面中央までの距離

		// 距離が表示部分の1/4より大きければスクロールさせる
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
		if (spTxtRange->getBookmark(&m_strBookmark) != S_OK)
			m_strBookmark = LPCOLESTR(NULL);

		spTxtRange->select();
		spTxtRange->scrollIntoView(VARIANT_TRUE);

		bSts = TRUE;

		if (m_pGlobalConfig->bScrollCenter)
			funcScrollBy(pDocument);
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


/// strScriptTextに書かれているスクリプトを実行する
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
	spWindow->execScript(CComBSTR(strScriptText), CComBSTR(L"javascript"), &vRet);
	TRACEIN(_T("_ExecuteUserJavascript() : 実行(%s)"), CString(strScriptText).Left(45));
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

	/* 前のハイライト表示を消す */
	TIMERSTART();
	CComQIPtr<IHTMLDocument3>	spDoc3 = spDoc;
	if (bEraseOld)
		_RemoveHighlight(spDoc3);
	TIMERSTOP(_T("_HilightFromFindBar, _RemoveHilight"));

	/* 単語をハイライトする */
	TIMERSTART();
	int nMatchCount = 0;
	if (strKeyword.GetLength() > 0) {
		_MtlForEachHTMLDocument2(spDoc, [&](IHTMLDocument2* pDoc) {
			CComPtr<IHTMLSelectionObject> spSelection;	/* テキスト選択を空にする */
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

				//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
				// ディスプレイに表示されているかどうか
				auto funcMove = [&spTxtRange] () {	// 検索範囲を変更する関数
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
							(strvisibility && strvisibility == _T("hidden"))) {	// 表示されていない場合はスキップ
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
			}
		});
	}
	TIMERSTOP(_T(" ハイライトにかかった時間(%d 個hit)"), nMatchCount);
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


