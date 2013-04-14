

CDonutCommandBar::Impl::Impl() : 
	m_nHotIndex(-1), 
	m_nPressedIndex(-1), 
	m_ChevronState(ChvNormal), 
	m_bFocusSelf(false), 
	m_bNowAltKeyDown(false), 
	m_hWndRestoreFocus(NULL),
	m_bAltPrefixFailed(false)
{
	WTL::CLogFont lf;
	lf.SetMenuFont();
	m_font = lf.CreateFontIndirect();
}

CDonutCommandBar::Impl::~Impl()
{
}


HWND CDonutCommandBar::Impl::Create(HWND hWndParent)
{
	enum { kWindowStyle =  WS_CHILD | WS_VISIBLE };
	return __super::Create(hWndParent, rcDefault, NULL, kWindowStyle, 0, ATL_IDW_COMMAND_BAR);
}

void	CDonutCommandBar::Impl::SetFont(HFONT hFont)
{
	m_font = hFont;
	WTL::CLogFont	lf;
	m_font.GetLogFont(&lf);
	LONG fontHeight = lf.GetHeight();
	fontHeight += kTopBottomPadding * 2;
	CRect rc;
	GetClientRect(&rc);
	SetWindowPos(NULL, -1, -1, rc.right, fontHeight, SWP_NOMOVE | SWP_NOZORDER);
	//MoveWindow(0, 0, rc.right, fontHeight);

	_RefreshBandInfo();
	_UpdateItemPosition();

	OnSize(0, rc.BottomRight());
}


// Overrides

void CDonutCommandBar::Impl::DoPaint(CDCHandle dc)
{
	HWND	hWnd = GetParent();
	CPoint	pt;
	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC)dc.m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
	::SetWindowOrgEx((HDC)dc.m_hDC, 0, 0, NULL);

	CRect rcClient;
	GetClientRect(&rcClient);

	dc.SetBkMode(TRANSPARENT);
	HFONT hFontPrev = dc.SelectFont(m_font);

	DWORD	dwTextFlags = DT_SINGLELINE | DT_VCENTER;
	if (m_bFocusSelf == false)
		dwTextFlags |= DT_HIDEPREFIX;

	int nCount = static_cast<int>(m_vecCommandButton.size());
	for (int i = 0; i < nCount; ++i) {
		const CommandButton& item = m_vecCommandButton[i];

		if (((i == nCount -1) && rcClient.right < item.rect.right) ||
			((i != nCount -1) &&rcClient.right < (item.rect.right + m_ChevronSize.cx)) ) {		// Chevron
			CRect rcChevronPos;
			rcChevronPos.top	= 0;
			rcChevronPos.right	= rcClient.right;
			rcChevronPos.left	= rcClient.right - m_ChevronSize.cx;
			rcChevronPos.bottom	= rcClient.bottom;
			if (IsThemeNull() == false)
				m_themeRebar.DrawThemeBackground(dc, RP_CHEVRON, m_ChevronState, &rcChevronPos);
			else
				dc.DrawText(_T("≫"), -1, rcChevronPos, DT_SINGLELINE);
			break;
		}

		CRect rcText = item.rect;
		rcText.left		+= kTextSideMargin;
		rcText.right	-= kTextSideMargin;

		if (IsThemeNull() == false) {
			int nState = TS_NORMAL;
			if (item.state == item.kHot)
				nState = TS_HOT;
			else if (item.state == item.kPressed)
				nState = TS_PRESSED;

			DrawThemeBackground(dc, TP_BUTTON, nState, &item.rect);

			DrawThemeText(dc, TP_BUTTON, nState, item.name, item.name.GetLength(), dwTextFlags, 0, &rcText);
		} else {
			CRect rcEdge = item.rect;
			rcEdge.DeflateRect(1, 1);

			//bool bDisabled = ((lpTBCustomDraw->nmcd.uItemState & CDIS_DISABLED) == CDIS_DISABLED);
			if (item.state == item.kHot || item.state == item.kPressed) {
				::FillRect(dc, &rcEdge, ::GetSysColorBrush(COLOR_MENUHILIGHT));
				::FrameRect(dc, &rcEdge, ::GetSysColorBrush(COLOR_HIGHLIGHT));
				//lpTBCustomDraw->clrText = ::GetSysColor(m_bParentActive ? COLOR_HIGHLIGHTTEXT : COLOR_GRAYTEXT);
				dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			dc.DrawText(item.name, item.name.GetLength(), &rcText, dwTextFlags);
			dc.SetTextColor(::GetSysColor(COLOR_MENUTEXT));
		}
	}
	dc.SelectFont(hFontPrev);
}

void CDonutCommandBar::Impl::OnTrackMouseMove(UINT nFlags, CPoint pt)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	bool bHitChevron = _HitTestChevron(pt);
	if (m_ChevronState != ChvPressed)
		_ChevronStateChange(bHitChevron ? ChvHot : ChvNormal);

	int nIndex = _HitTest(pt);
	if (nIndex != -1) {
		if (s_pSubMenu) {
			if (bHitChevron) {
				if (m_ChevronState != ChvPressed) {
					_PopupChevronMenu();
					_ChevronStateChange(ChvPressed);
				}
				return ;
			}
			if (_GetChevronIndex() != -1 && _GetChevronIndex() <= nIndex) {	// ChevronFolder
				_HotItem(-1);
				return ;
			}
			
			_HotItem(-1);	// Popupしてるフォルダを切り替える
			if (nIndex != m_nPressedIndex) {
				_DoPopupSubMenu(nIndex);
			} else {
				_PressItem(nIndex);
			}
		} else {
			_HotItem(nIndex);
		}
	} else {
		_HotItem(-1);
	}
}

void CDonutCommandBar::Impl::OnTrackMouseLeave()
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	if (s_pSubMenu == nullptr)
		_HotItem(-1);

	if (m_ChevronState != ChvPressed)
		_ChevronStateChange(ChvNormal);
}


BOOL CDonutCommandBar::Impl::PreTranslateMessage(MSG* pMsg)
{
	// Altキー
	if (pMsg->message == WM_SYSKEYDOWN) {
		if (pMsg->wParam == VK_MENU) {
			if (m_bFocusSelf == false && s_pSubMenu == nullptr) {	// メニュー選択モードにする
				m_bFocusSelf = true;
				m_bNowAltKeyDown = true;
				Invalidate(FALSE);
				m_hWndRestoreFocus = GetFocus();
				SetFocus();
			} else {						// メニュー選択をやめる
				if (m_bNowAltKeyDown == false) {
					m_bFocusSelf = false;
					_CloseSubMenu();
					_PressItem(-1);
					Invalidate(FALSE);
					::SetFocus(m_hWndRestoreFocus);
					m_hWndRestoreFocus = NULL;
				}
			}
			return TRUE;
		} else if (m_bFocusSelf && m_bAltPrefixFailed == false) {	// Alt + prefix
			OnKeyDown((UINT)pMsg->wParam, 0, 0);
			if (s_pSubMenu)	{	// メニューが表示された
				m_bNowAltKeyDown = false;
				return TRUE;
			} else {
				m_bAltPrefixFailed = true;
			}
		}
	} else if ((pMsg->message == WM_SYSKEYUP || pMsg->message == WM_KEYUP) && pMsg->wParam == VK_MENU && m_bFocusSelf) {
		m_bNowAltKeyDown = false;
		_PressItem(0);
		if (m_bAltPrefixFailed) {
			m_bAltPrefixFailed = false;

			m_bFocusSelf = false;
			_PressItem(-1);
			Invalidate(FALSE);
			::SetFocus(m_hWndRestoreFocus);
			m_hWndRestoreFocus = NULL;
		}
		return TRUE;
	}

	// キーを処理する
	if (pMsg->hwnd == m_hWnd) {
		if (pMsg->message == WM_KEYDOWN) {
			UINT nKey = (UINT)pMsg->wParam;
			if (s_pSubMenu && s_pSubMenu->PreTranslateMessage(pMsg)) {
				if (!(pMsg->wParam == VK_RIGHT && pMsg->lParam == -1)) {
					if (pMsg->wParam != VK_LEFT) {
						if (pMsg->wParam == VK_ESCAPE && s_pSubMenu) {
							int nPressedIndex = m_nPressedIndex;
							_CloseSubMenu();
							_PressItem(nPressedIndex);
						}
						return TRUE;
					}
				}
			}

			if (nKey == VK_RIGHT || nKey == VK_LEFT || nKey == VK_DOWN || nKey == VK_UP) {
				int nNewPressIndex = m_nPressedIndex;			
				if (nNewPressIndex == -1) {
					nNewPressIndex = 0;
				} else {
					if (nKey == VK_RIGHT) {
						++nNewPressIndex;
						if ((int)m_vecCommandButton.size() <= nNewPressIndex)
							nNewPressIndex = 0;
					} else if (nKey == VK_LEFT) {
						--nNewPressIndex;
						if (nNewPressIndex == -1) 
							nNewPressIndex = m_vecCommandButton.size() - 1;
					}
				}
				if (s_pSubMenu == nullptr && (nKey == VK_RIGHT || nKey == VK_LEFT)) {
					_PressItem(nNewPressIndex);
				} else {
					_DoPopupSubMenu(nNewPressIndex);
				}
				return TRUE;

			} else if (nKey == VK_RETURN && m_nPressedIndex != -1) {
				if (s_pSubMenu) {
					_CloseSubMenu();
				} else {
					_DoPopupSubMenu(m_nPressedIndex);
				}
				return TRUE;

			} else if (nKey == VK_ESCAPE) {		// メニューを閉じる
				m_bFocusSelf = false;
				_CloseSubMenu();
				_PressItem(-1);
				Invalidate(FALSE);
				::SetFocus(m_hWndRestoreFocus);
				m_hWndRestoreFocus = NULL;
				return TRUE;
			}

		} else if (pMsg->message == WM_KEYUP && s_pSubMenu && s_pSubMenu->PreTranslateMessage(pMsg)) {
			return TRUE;
		}
	}

	return FALSE;
}


// Message map

int  CDonutCommandBar::Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	OpenThemeData(VSCLASS_TOOLBAR);

	m_themeRebar.OpenThemeData(m_hWnd, VSCLASS_REBAR);
	m_ChevronSize;
	if (m_themeRebar.IsThemeNull() == false)
		m_themeRebar.GetThemePartSize(CClientDC(NULL), RP_CHEVRON, CHEVS_NORMAL, NULL,TS_TRUE, &m_ChevronSize);
	else
		m_ChevronSize.SetSize(17, 24);

	m_menu.LoadMenu(IDR_MAINFRAME);
	_UpdateItemPosition();

	// お気に入りを読み込み
	CRootFavoritePopupMenu::LoadFavoriteBookmark();

	// お気に入りグループを読み込み
	CRootFavoriteGroupPopupMenu::LoadFavoriteGroup();

	// cssメニュー
	enum { kToolPos = 4, kCssPos = 13 };
	CMenuHandle menuCss = m_menu.GetSubMenu(kToolPos).GetSubMenu(kCssPos);
	std::vector<CString>	vecFileName;
	MtlForEachFileSort(Misc::GetExeDirectory() + _T("css"), [this, &vecFileName](const CString& filePath) {
		CString ext = Misc::GetFileExt(filePath);
		ext.MakeLower();
		if (ext != _T("css"))
			return ;
		vecFileName.push_back(Misc::GetFileBaseName(filePath));
	});
	int nID = ID_INSERTPOINT_CSSMENU + 1;
	for (auto it = vecFileName.begin(); it != vecFileName.end(); ++it) {
		menuCss.InsertMenu(ID_INSERTPOINT_CSSMENU, MF_BYCOMMAND, nID, (LPCTSTR)*it);
		++nID;
	}
	if (vecFileName.size() > 0) {
		menuCss.DeleteMenu(ID_INSERTPOINT_CSSMENU, MF_BYCOMMAND);
	} else {
		menuCss.ModifyMenu(ID_INSERTPOINT_CSSMENU, MF_BYCOMMAND | MF_DISABLED, ID_INSERTPOINT_CSSMENU, _T("(なし)"));
	}

	s_hWnd = m_hWnd;

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

void CDonutCommandBar::Impl::OnDestroy()
{
	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);

	CRootFavoritePopupMenu::JoinSaveBookmarkThread();
}

void CDonutCommandBar::Impl::OnSize(UINT nType, CSize size)
 {
	 DefWindowProc();
	 _RefreshBandInfo();

	 //LinkFolderPtr pFolder = m_BookmarkList.back()->pFolder;
	 //for (;;) {
		// // ChevronFolderが見えてしまうので、ChevronFolderからアイテム補充
		// if (m_BookmarkList.back()->rcItem.left < size.cx) {
		//	 if (pFolder->size() > 0) {				 
		//		 m_BookmarkList.insert(m_BookmarkList.begin() +  m_BookmarkList.size() - 1, 
		//			 std::move(pFolder->front()));
		//		 pFolder->erase(pFolder->begin());
		//		 _UpdateItemPosition();
		//		 if (pFolder->size() == 0)	// なくなったので終了
		//			 break;
		//		 continue;
		//	 }			 
		// }
		// break;
	 //}
	 //for (;;) {
		//if (m_BookmarkList.size() >= 2) {
		//	unique_ptr<LinkItem>& pRightItem = m_BookmarkList[m_BookmarkList.size() - 2];
		//	int LinkBarWidth = size.cx;
		//	if (pFolder->size() > 0)
		//		LinkBarWidth -= m_ChevronSize.cx;
		//	if (LinkBarWidth < pRightItem->rcItem.right) {	// アイテムが欠けるのでChevronFolderに入れる
		//		pFolder->insert(pFolder->begin(), std::move(pRightItem));
		//		m_BookmarkList.erase(m_BookmarkList.begin() + (m_BookmarkList.size() - 2));
		//		_UpdateItemPosition();
		//		continue;
		//	}
		//}
		//break;
	 //}
 }

LRESULT CDonutCommandBar::Impl::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	HWND hWnd = ::WindowFromPoint(pt);
	if (hWnd && s_pSubMenu) {
		bool bChildWnd = false;
		IBasePopupMenu* pSubMenu = s_pSubMenu;
		if (pSubMenu->GetHWND() == hWnd) {
			bChildWnd = true;
		} else {
			CString className;
			GetClassName(hWnd, className.GetBuffer(128), 128);
			className.ReleaseBuffer();
			if (className == _T("DonutBasePopupMenu") || className == _T("DonutLinkPopupMenu"))
				bChildWnd = true;
		}
		if (bChildWnd) {
			::SendMessage(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		}
	}
	return 0;
}

void CDonutCommandBar::Impl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	if (_HitTestChevron(point)) {
		if (m_ChevronState != ChvPressed) {
			_ChevronStateChange(ChvPressed);
			_PopupChevronMenu();
			return ;
		} else {
			_ChevronStateChange(ChvHot);
			_CloseSubMenu();
		}
	}

	int nIndex = _HitTest(point);
	if (nIndex != -1 && (nIndex < _GetChevronIndex() || _GetChevronIndex() == -1)) {
		if (s_pSubMenu) {
			_CloseSubMenu();
		} else {
			_DoPopupSubMenu(nIndex);
		}
	} else {
		_CloseSubMenu();
	}
}

void CDonutCommandBar::Impl::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CDonutCommandBar::Impl::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CDonutCommandBar::Impl::OnMButtonDown(UINT nFlags, CPoint point)
{
}


LRESULT CDonutCommandBar::Impl::OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	GetWindowRect(&rc);
	CPoint	pt;
	GetCursorPos(&pt);
	//if (rc.PtInRect(pt)) {
		//ScreenToClient(&pt);
		//OnLButtonDown(0, pt);
	//} else {
		_CloseSubMenu();
		_HotItem(-1);
		_PressItem(-1);
	//}
		//_UpdateItemPosition();
	return 0;
}


void CDonutCommandBar::Impl::OnKillFocus(CWindow wndFocus)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu == false) {
		m_bFocusSelf = false;
		_PressItem(-1);
		Invalidate(FALSE);

		if (s_pSubMenu) 
			_CloseSubMenu();
	}
}

void CDonutCommandBar::Impl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bFocusSelf) {
		int nCount = (int)m_vecCommandButton.size();
		for (int i = 0; i < nCount; ++i) {
			if (m_vecCommandButton[i].prefix == nChar) {
				_DoPopupSubMenu(i);
				Invalidate(FALSE);
				break;
			}
		}
	}
}

int		CDonutCommandBar::Impl::_GetBandHeight()
{
	WTL::CLogFont	lf;
	m_font.GetLogFont(&lf);
	int fontHeight = (int)lf.GetHeight();
	fontHeight += kTopBottomPadding * 2;
	return fontHeight;
}

//-----------------------------------
/// リバーを更新する
void	CDonutCommandBar::Impl::_RefreshBandInfo()
{
	HWND		  hWndReBar = GetParent();
	CReBarCtrl	  rebar(hWndReBar);

	static UINT wID = 0;
	if (wID == 0) {
		REBARBANDINFO rb = { sizeof (REBARBANDINFO) };
		rb.fMask	= RBBIM_CHILD | RBBIM_ID;
		int nCount	= rebar.GetBandCount();
		for (int i = 0; i < nCount; ++i) {
			rebar.GetBandInfo(i, &rb);
			if (rb.hwndChild == m_hWnd) {
				wID = rb.wID;
				break;
			}
		}
		return;
	}

	int	nIndex = rebar.IdToIndex( wID );
	if ( nIndex == -1 ) 
		return;				// 設定できないので帰る

	UINT fontHeight = _GetBandHeight();
	//int cxIdeal = 0;
	//if (m_BookmarkList.size() > 0)
	//	cxIdeal = m_BookmarkList[m_BookmarkList.size() - 1]->rcItem.right;
	REBARBANDINFO rbBand = { sizeof (REBARBANDINFO) };
	rbBand.fMask  = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;
	rebar.GetBandInfo(nIndex, &rbBand);
	if (rbBand.cyMinChild != fontHeight/* || rbBand.cxIdeal != cxIdeal*/) {
		// Calculate the size of the band
		rbBand.cyMinChild = fontHeight;
		rbBand.cxIdeal	= 0;
		rebar.SetBandInfo(nIndex, &rbBand);
	}
	if (nIndex == 0) {
		UINT oldcyMinChild = rbBand.cyMinChild;
		rbBand.cyMinChild = fontHeight;
		rebar.SetBandInfo(nIndex, &rbBand);

		REBARBANDINFO rbi = { sizeof(rbi) };
		rbi.fMask = RBBIM_STYLE;
		rebar.GetBandInfo(nIndex + 1, &rbBand);
		UINT rawHeight = rebar.GetRowHeight(0);
		if (rbBand.fStyle & RBBS_BREAK)	// コマンドバーだけで１行使ってるので上はなしに
			rawHeight = 0;

		rbBand.cyMinChild = std::max(rawHeight, fontHeight);
		rbBand.cxIdeal	= 0;
		rebar.SetBandInfo(nIndex, &rbBand);
		std::for_each(m_vecCommandButton.begin(), m_vecCommandButton.end(), [=](CommandButton& btn) {
			btn.rect.bottom = std::max(rawHeight, fontHeight);
		});
	}
}


void	CDonutCommandBar::Impl::_UpdateItemPosition()
{
	m_vecCommandButton.clear();

	int nLeftPos = 0;
	int nCount = m_menu.GetMenuItemCount();
	for (int i = 0; i < nCount; ++i) {
		CString name;
		m_menu.GetMenuString(i, name, MF_BYPOSITION);

		CRect	rcItem;
		int nTextWidth = MTL::MtlComputeWidthOfText(name, m_font);
		rcItem.top	= 0;
		rcItem.left	= nLeftPos;
		rcItem.bottom	= _GetBandHeight();//24;//rcClient.bottom;
		rcItem.right	= nLeftPos + nTextWidth + (kTextSideMargin * 2);
		nLeftPos = rcItem.right;
#if _MSC_VER >= 1700
		m_vecCommandButton.emplace_back(name, rcItem);
#else
		m_vecCommandButton.push_back(CommandButton(name, rcItem));
#endif
	}
}

int		CDonutCommandBar::Impl::_HitTest(const CPoint& pt)
{
	int nCount = static_cast<int>(m_vecCommandButton.size());
	for (int i = 0; i < nCount; ++i) {
		if (m_vecCommandButton[i].rect.PtInRect(pt))
			return i;
	}
	return -1;
}

void	CDonutCommandBar::Impl::_HotItem(int nIndex)
{
	// clean up
	if ( _IsValidIndex(m_nHotIndex) ) {
		CommandButton& item = m_vecCommandButton[m_nHotIndex];
		if ( item.ModifyState(CommandButton::kNormal) )
			InvalidateRect(item.rect);
	}

	m_nHotIndex = nIndex;

	if ( _IsValidIndex(m_nHotIndex) ) {
		CommandButton& item = m_vecCommandButton[m_nHotIndex];
		if ( item.ModifyState(CommandButton::kHot) )
			InvalidateRect(item.rect);
	}
}

void	CDonutCommandBar::Impl::_PressItem(int nIndex)
{
	// clean up
	if ( _IsValidIndex(m_nPressedIndex) ) {
		CommandButton& item = m_vecCommandButton[m_nPressedIndex];
		if ( item.ModifyState(CommandButton::kNormal) )
			InvalidateRect(item.rect);
	}

	m_nPressedIndex = nIndex;

	if ( _IsValidIndex(m_nPressedIndex) ) {
		CommandButton& item = m_vecCommandButton[m_nPressedIndex];
		if ( item.ModifyState(CommandButton::kPressed) )
			InvalidateRect(item.rect);
	}
}

void	CDonutCommandBar::Impl::_CloseSubMenu()
{
	if (s_pSubMenu) {
		if (m_ChevronState == ChvPressed)
			_ChevronStateChange(ChvNormal);

		s_pSubMenu->DestroyWindow();
		s_pSubMenu = nullptr;

		::UnhookWindowsHookEx(s_hHook);
		s_hHook = NULL;

		_PressItem(-1);
	}
}

void	CDonutCommandBar::Impl::_DoPopupSubMenu(int nIndex)
{
	_CloseSubMenu();

	SetFocus();

	if (nIndex == kFilePos) {
		s_pSubMenu = new CFilePopupMenu;
	} else if (nIndex == kFavoritePos) {
		s_pSubMenu = new CRootFavoritePopupMenu;
	} else {
		s_pSubMenu = new CBasePopupMenu;
	}
	CPoint pt(m_vecCommandButton[nIndex].rect.left, m_vecCommandButton[nIndex].rect.bottom);
	ClientToScreen(&pt);
	--pt.y;
	s_pSubMenu->DoTrackPopupMenu(m_menu.GetSubMenu(nIndex), pt, m_hWnd);

	ATLVERIFY(s_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, _Module.GetModuleInstance(), 0));

	_PressItem(nIndex);
}

bool	CDonutCommandBar::Impl::_HitTestChevron(const CPoint& pt)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if (rcClient.right < m_vecCommandButton.back().rect.right) {
		CRect rcChevronPos;
		rcChevronPos.top	= 0;
		rcChevronPos.right	= rcClient.right;
		rcChevronPos.left	= rcClient.right - m_ChevronSize.cx;
		rcChevronPos.bottom	= rcClient.bottom;
		if (rcChevronPos.PtInRect(pt))
			return true;
	}
	return false;
}

void	CDonutCommandBar::Impl::_ChevronStateChange(ChevronState state)
 {
	if (m_ChevronState != state) {
		m_ChevronState = state;

		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChevron(rcClient.right - m_ChevronSize.cx, 0, rcClient.right, rcClient.bottom);
		InvalidateRect(rcChevron, FALSE);
	}
 }

void	CDonutCommandBar::Impl::_PopupChevronMenu()
{
	_CloseSubMenu();

	SetFocus();

	CRect rcClient;
	GetClientRect(&rcClient);

	int nIndex = _GetChevronIndex();
	ATLASSERT( nIndex != -1 );
	s_pSubMenu = new CChevronPopupMenu(nIndex);
	
	CPoint pt(rcClient.right - m_ChevronSize.cx, rcClient.bottom);
	ClientToScreen(&pt);
	--pt.y;
	s_pSubMenu->DoTrackPopupMenu(m_menu.m_hMenu, pt, m_hWnd);

	ATLVERIFY(s_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, _Module.GetModuleInstance(), 0));
	
}

int		CDonutCommandBar::Impl::_GetChevronIndex()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int nCount = static_cast<int>(m_vecCommandButton.size());
	for (int i = 0; i < nCount; ++i) {
		CommandButton& item = m_vecCommandButton[i];
		if (((i == nCount -1) && rcClient.right < item.rect.right) ||
			((i != nCount -1) &&rcClient.right < (item.rect.right + m_ChevronSize.cx)) ) {
				return i;
		}
	}
	return -1;
}

LRESULT  CDonutCommandBar::Impl::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(s_hHook, nCode, wParam, lParam);
	if (nCode == HC_ACTION) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN: 
			{
				LPMSLLHOOKSTRUCT pllms = (LPMSLLHOOKSTRUCT)lParam;
				HWND hWnd = WindowFromPoint(pllms->pt);

				bool bChildWnd = false;
				IBasePopupMenu* pSubMenu = s_pSubMenu;
				if (pSubMenu->GetHWND() == hWnd) {
					bChildWnd = true;
				} else {
					CString className;
					GetClassName(hWnd, className.GetBuffer(128), 128);
					className.ReleaseBuffer();
					if (className == _T("DonutBasePopupMenu") || className == _T("DonutLinkPopupMenu"))
						bChildWnd = true;
				}

				if (CLinkPopupMenu::s_bNowShowRClickMenu)
					break;
				if (hWnd != s_hWnd && bChildWnd == false) {
					::SendMessage(s_hWnd, WM_CLOSEBASESUBMENU, 0, 0);
				} 
			}
			break;

		default:
			break;
		}
	}
	return CallNextHookEx(s_hHook, nCode, wParam, lParam);
}

