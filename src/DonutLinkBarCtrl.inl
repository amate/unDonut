/**
 *	@file	DonutLinkBarCtrl.inl
 *	@brief	リンクバーの実装
 */


CDonutLinkBarCtrl::Impl::Impl() : 
	m_nHotIndex(-1), 
	m_nPressedIndex(-1),
	m_nNowDragItemIndex(-1),
	m_ChevronState(ChvNormal),
	m_bLoading(true)
{
	
	CLinkPopupMenu::s_iconFolder.LoadIcon(IDI_FOLDER);
	::ExtractIconEx(_T("url.dll"), 0, NULL, &CLinkPopupMenu::s_iconLink.m_hIcon, 1);
}

CDonutLinkBarCtrl::Impl::~Impl()
{
	if (m_thread_load.joinable())
		m_thread_load.join();
	if (m_thread_save.joinable())
		m_thread_save.join();
}

void	CDonutLinkBarCtrl::Impl::SetFont(HFONT hFont)
{
	m_font = hFont;
	if (m_bLoading)
		return ;
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

void	CDonutLinkBarCtrl::Impl::Refresh()
{
	_UpdateItemPosition();

	CRect rc;
	GetClientRect(&rc);
#if 0
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
		if (wID == 0)
			return;
	}

	int	nIndex = rebar.IdToIndex( wID );
	if (nIndex != -1) {

		REBARBANDINFO rbinfo = { sizeof(REBARBANDINFO) };
		rbinfo.fMask = RBBIM_SIZE;
		rebar.GetBandInfo(nIndex, &rbinfo);
		rc.right	= rbinfo.cx;
		if (rc.right == 0)
			return ;
	}
#endif
	OnSize(0, rc.BottomRight());
}

void	CDonutLinkBarCtrl::Impl::LinkImportFromFolder(LPCTSTR folder)
{
	ATLASSERT( ::PathIsDirectory(folder) );

	// 以前のリンクフォルダを削除する
	_ClearLinkBookmark();

	std::function<void (LPCTSTR, bool, LinkFolderPtr)> funcAddLink;
	funcAddLink = [&, this](LPCTSTR strPath, bool bDirectory, LinkFolderPtr pFolder) {
		unique_ptr<LinkItem> pItem(new LinkItem);
		if (bDirectory) {
			CString strDirPath = strPath;
			::PathRemoveBackslash(strDirPath.GetBuffer(MAX_PATH));
			strDirPath.ReleaseBuffer();
			pItem->strName = Misc::GetFileBaseName(strDirPath);
			pItem->pFolder	= new LinkFolder;
			MtlForEachObject_OldShell(strPath, std::bind(funcAddLink, std::placeholders::_1, std::placeholders::_2, pItem->pFolder));
		} else {
			if (Misc::GetFileExt(strPath).CompareNoCase(_T("url")) != 0)
				return ;

			pItem->strName	= Misc::GetFileBaseNoExt(strPath);
			pItem->strUrl	= MtlGetInternetShortcutUrl(strPath);
			DWORD dwExProp = 0;
			DWORD dwExPropOpt = 0;
			if (CExProperty::CheckExPropertyFlag(dwExProp, dwExPropOpt, strPath)) {
				pItem->bExPropEnable = true;
				pItem->dwExProp	= dwExProp;
				pItem->dwExPropOpt	= dwExPropOpt;
			}			
		}
		pFolder->push_back(std::move(pItem));
	};
	MtlForEachObject_OldShell(folder, std::bind(funcAddLink, std::placeholders::_1, std::placeholders::_2, &m_BookmarkList));

	// 名前順で並び替え
	CLinkPopupMenu::SortByName(&m_BookmarkList);

	unique_ptr<LinkItem> pItem(new LinkItem);
	LinkFolderPtr	pFolder = new LinkFolder;
	pItem->pFolder	= pFolder;
	pItem->strName = _T("ChevronFolder");
	m_BookmarkList.push_back(std::move(pItem));

	_SaveLinkBookmark();

	Refresh();
}

void	CDonutLinkBarCtrl::Impl::LinkExportToFolder(LPCTSTR folder)
{
	CString strBaseFolder = folder;
	MtlMakeSureTrailingBackSlash(strBaseFolder);

	std::function<void (LPCTSTR, LinkFolderPtr)>	funcAddLinkFile;
	funcAddLinkFile = [&](LPCTSTR folder, LinkFolderPtr pFolder) {
		for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
			LinkItem& item = *it->get();
			if (item.pFolder) {
				if (item.strName == _T("ChevronFolder")) {
					funcAddLinkFile(folder, item.pFolder);
				} else {
					CString strNewFolder = folder;
					strNewFolder += item.strName;
					MtlMakeSureTrailingBackSlash(strNewFolder);
					CreateDirectory(strNewFolder, NULL);
					funcAddLinkFile(strNewFolder, item.pFolder);
				}
			} else {
				CString LinkFilePath = folder;
				LinkFilePath += item.strName + _T(".url");
				int i = 0;
				while (::PathFileExists(LinkFilePath)) {
					LinkFilePath.Format(_T("%s%s(%d).url"), folder, item.strName, i);
					++i;
				}
				if (MtlCreateInternetShortcutFile(LinkFilePath, item.strUrl)) {
					if (item.bExPropEnable) {
						CIniFileO	pr(LinkFilePath, DONUT_SECTION);
						pr.SetValue(true, EXPROP_KEY_ENABLED);
						pr.SetValue(item.dwExProp.get(), EXPROP_KEY);
						pr.SetValue(item.dwExPropOpt.get(), EXPROP_OPTION);
					}
				}

			}
		}
	};
	funcAddLinkFile(strBaseFolder, &m_BookmarkList);
}


// Overrides
void CDonutLinkBarCtrl::Impl::DoPaint(CDCHandle dc)
{
	if (m_bLoading)
		return;

	HWND	hWnd = GetParent();
	CPoint	pt;
	MapWindowPoints(hWnd, &pt, 1);
	::OffsetWindowOrgEx( (HDC)dc.m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
	::SetWindowOrgEx((HDC)dc.m_hDC, 0, 0, NULL);

	dc.SetBkMode(TRANSPARENT);
	HFONT hFontPrev = dc.SelectFont(m_font);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	int nCount = (int)m_BookmarkList.size();
	for (int i = 0; i < nCount; ++i) {
		LinkItem& item = *m_BookmarkList[i].get();

		// ChevronFolder
		if (i == (nCount - 1)) {
			if (item.pFolder->size() > 0) {
				CRect rcChevronPos;
				rcChevronPos.top	= 0;
				rcChevronPos.right	= rcClient.right;
				rcChevronPos.left	= rcClient.right - m_ChevronSize.cx;
				rcChevronPos.bottom	= rcClient.bottom;
				if (m_themeRebar.IsThemeNull() == false)
					m_themeRebar.DrawThemeBackground(dc, RP_CHEVRON, m_ChevronState, &rcChevronPos);
				else
					dc.DrawText(_T("≫"), -1, rcChevronPos, DT_SINGLELINE);
			}
			break;
		}

		_DrawItem(dc, item);

	}
	//if (m_BookmarkList.empty())
	//	dc.DrawText(_T("none"), -1, CRect(0, kTopBottomPadding, 200, 20), DT_TOP);

	dc.SelectFont(hFontPrev);
}


void CDonutLinkBarCtrl::Impl::OnTrackMouseMove(UINT nFlags, CPoint pt)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu || m_bLoading)
		return ;

	int nIndex = -1;
	if (auto value = _HitTest(pt)) 
		nIndex = value.get();

	if ((m_nHotIndex == -1 || m_nHotIndex != nIndex) && s_pSubMenu == nullptr) {
		m_tip.Activate(FALSE);
		m_tip.Activate(TRUE);
	}

	bool bHitChevron = _HitTestChevron(pt);
	if (m_ChevronState != ChvPressed)
		_ChevronStateChange(bHitChevron ? ChvHot : ChvNormal);

	if (nIndex != -1) {
		if (s_pSubMenu) {
			LinkFolderPtr pFolder = m_BookmarkList[nIndex]->pFolder;
			if (bHitChevron) {
				if (m_ChevronState != ChvPressed) {
					if (pFolder != s_pSubMenu->GetLinkFolderPtr()) {
						_ChevronStateChange(ChvPressed);
						_PopupChevronMenu();
					}
				}
				return ;
			}
			if (nIndex == (int)m_BookmarkList.size() - 1) {	// ChevronFolder
				_HotItem(-1);
				return ;
			}
			
			if (pFolder) {	// Popupしてるフォルダを切り替える
				_HotItem(-1);
				if (pFolder != s_pSubMenu->GetLinkFolderPtr()) {
					_DoPopupSubMenu(nIndex);
				} else {
					_PressItem(nIndex);
				}
				return ;
			}
		}
		_HotItem(nIndex);
	} else {
		_HotItem(-1);
	}
}

void CDonutLinkBarCtrl::Impl::OnTrackMouseLeave()
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu || m_bLoading)
		return ;

	_HotItem(-1);
	if (m_ChevronState != ChvPressed)
		_ChevronStateChange(ChvNormal);
}


BOOL CDonutLinkBarCtrl::Impl::PreTranslateMessage(MSG* pMsg)
{
	// キーを処理する
	if (pMsg->hwnd == m_hWnd) {
		if (pMsg->message == WM_KEYDOWN) {
			UINT nKey = (UINT)pMsg->wParam;
			if (s_pSubMenu && s_pSubMenu->PreTranslateMessage(pMsg)) {
				// サブメニューがないアイテムでの '→'キーは自分が処理する
				if (!(pMsg->wParam == VK_RIGHT && pMsg->lParam == -1)) {
					// サブメニューがない状態での '←' キーは自分が処理する
					if (pMsg->wParam != VK_LEFT) {
						// サブメニューがある状態での 'Esc'キーはサブメニューを閉じる
						if (pMsg->wParam == VK_ESCAPE && s_pSubMenu) {
							int nPressedIndex = m_nPressedIndex;
							_CloseSubMenu();
							_PressItem(nPressedIndex);
						}
						return TRUE;
					}
				}
			}
			// 矢印キーの処理
			if (nKey == VK_RIGHT || nKey == VK_LEFT || nKey == VK_DOWN || nKey == VK_UP) {
				int nNewPressIndex = m_nPressedIndex;			
				if (nNewPressIndex == -1) {
					nNewPressIndex = 0;
				} else {
					if (nKey == VK_RIGHT) {
						++nNewPressIndex;
						if ((int)m_BookmarkList.size() <= nNewPressIndex)
							nNewPressIndex = 0;
					} else if (nKey == VK_LEFT) {
						--nNewPressIndex;
						if (nNewPressIndex == -1) 
							nNewPressIndex = m_BookmarkList.size() - 1;
					}
				}
				if (_DoPopupSubMenu(nNewPressIndex) == false)
					_PressItem(nNewPressIndex);
				return TRUE;

			} else if (nKey == VK_RETURN && m_nPressedIndex != -1) {
				if (s_pSubMenu) {
					_CloseSubMenu();
				} else {
					if (_DoPopupSubMenu(m_nPressedIndex) == false)
						CLinkPopupMenu::OpenLink(*m_BookmarkList[m_nPressedIndex], DonutGetStdOpenFlag());
				}
				return TRUE;

			} else if (nKey == VK_ESCAPE) {		// メニューを閉じる
				_PressItem(-1);
				Invalidate(FALSE);
				GetTopLevelWindow().SetFocus();
				return TRUE;
			}

		}
	}
	return FALSE;
}


// IDropTargetImpl

DROPEFFECT CDonutLinkBarCtrl::Impl::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	m_bAcceptDrag = m_bDragItemIsLinkFile = CLinkItemDataObject::IsAcceptDrag(pDataObject);

	FORMATETC fmt = { 0 };
	fmt.cfFormat	= CF_DONUTLINKITEM;
	if (pDataObject->QueryGetData(&fmt) == S_OK) {
		CLinkItemDataObject*	pLinkItem;
		pDataObject->QueryInterface(IID_NULL, (void**)&pLinkItem);
		if (pLinkItem) {
			m_DragItemData = pLinkItem->GetFolderAndIndex();
			m_bAcceptDrag = true;
			m_bDragItemIsLinkFile = false;
		}
	}
	if (m_bAcceptDrag)
		return _MtlStandardDropEffect(dwKeyState);
	return DROPEFFECT_NONE;
}

/// DragNone にするかどうかを返す
bool	CDonutLinkBarCtrl::Impl::_IsDragNone(int nOverIndex, const CPoint& point)
{
	if (m_nNowDragItemIndex == -1)
		return false;

	if (m_nNowDragItemIndex == nOverIndex)
		return true;

	const CRect& rcDragItem = m_BookmarkList[m_nNowDragItemIndex]->rcItem;
	CRect rcHit = rcDragItem;

	int nDragLeftIndex = m_nNowDragItemIndex - 1;
	if (_IsValidIndex(nDragLeftIndex)) {
		rcHit.right= rcDragItem.left;

		CRect rcitem = m_BookmarkList[nDragLeftIndex]->rcItem;
		if (m_BookmarkList[nDragLeftIndex]->pFolder) {
			rcHit.left = rcitem.right - kDragInsertHitWidthOnFolder;
		} else {
			rcHit.left = rcitem.right - rcitem.Width() / 2;
		}
		if (rcHit.PtInRect(point))
			return true;
	}
	int nDragRightIndex = m_nNowDragItemIndex + 1;
	if (_IsValidIndex(nDragRightIndex)) {
		rcHit.left	= rcDragItem.right;

		CRect rcitem = m_BookmarkList[nDragRightIndex]->rcItem;
		if (m_BookmarkList[nDragRightIndex]->pFolder) {
			rcHit.right = rcitem.left + kDragInsertHitWidthOnFolder;
		} else {
			rcHit.right = rcitem.left + rcitem.Width() / 2;
		}
		if (rcHit.PtInRect(point))
			return true;
	}

	return false;
}

// 挿入位置を返して、インサートマークを描画する
int		CDonutLinkBarCtrl::Impl::_HitTestOnDragging(const CPoint& point, hitTestDragCategory& htc)
{
	if (_HitTestChevron(point)) {
		htc = htChevron;
		return (int)m_BookmarkList.size() - 1;
	}

	int nIndex = -1;
	CPoint	DrawPoint;
	CRect	rcClient;
	GetClientRect(&rcClient);
	DrawPoint.y	= rcClient.bottom;
	auto value = _HitTest(point);
	if (value && value.get() != (m_BookmarkList.size() - 1)) {
		if (_IsDragNone(value.get(), point)) {
			htc	= htNone;
			return -1;
		}

		LinkItem& item = *m_BookmarkList[value.get()];
		CRect rcItem	= item.rcItem;
		if (item.pFolder) {	// カーソルはフォルダの上
			rcItem.right	= rcItem.left + kDragInsertHitWidthOnFolder;
			if (rcItem.PtInRect(point)) {
				// インサートマーク位置決定
				DrawPoint.x	= m_BookmarkList[value.get()]->rcItem.left;
				htc	= htInsert;
				nIndex = value.get();
			} else {
				rcItem.right	= item.rcItem.right;
				rcItem.left		= rcItem.right - kDragInsertHitWidthOnFolder;
				if (rcItem.PtInRect(point)) {
					// インサートマーク位置決定
					DrawPoint.x	= m_BookmarkList[value.get()]->rcItem.right;
					htc = htInsert;
					nIndex = value.get() + 1;
				} else {
					// フォルダ上
					_ClearInsertionEdge();

					htc = htFolder;
					return value.get();
				}
			}
		} else {	// カーソルはアイテムの上
			int nItemHalfWidth = rcItem.Width() / 2;
			rcItem.right	= rcItem.left + nItemHalfWidth;
		
			if (rcItem.PtInRect(point)) {	// アイテムの左側にポインタがある
				DrawPoint.x	= m_BookmarkList[value.get()]->rcItem.left;
				htc = htInsert;
				nIndex = value.get();
			} else {
				DrawPoint.x	= m_BookmarkList[value.get()]->rcItem.right;
				htc = htInsert;
				nIndex = value.get() + 1;
			}
		}
	} else {
		CRect rcLeft = rcClient;
		rcLeft.right = kLeftMargin;
		if (rcLeft.PtInRect(point)) {
			htc = htInsert;
			nIndex = 0;
		} else if (m_BookmarkList.size() > 0) {
			DrawPoint.x = m_BookmarkList.back()->rcItem.left;
			htc = htInsert;
			nIndex = (int)m_BookmarkList.size() - 1;
		} else {
			htc = htInsert;
			nIndex = 0;
		}
	}

	// インサートマーク描画
	_DrawInsertEdge(DrawPoint);

	if (m_ptInvalidateOnDrawingInsertionEdge != DrawPoint) {
		_ClearInsertionEdge();
		m_ptInvalidateOnDrawingInsertionEdge = DrawPoint;
	}
	return nIndex;
}

DROPEFFECT CDonutLinkBarCtrl::Impl::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
{
	if (m_bAcceptDrag == false)
		return DROPEFFECT_NONE;

	_ClearInsertionEdge();

	if (m_nNowDragItemIndex != -1) {
		_HotItem(m_nNowDragItemIndex);	// Drag中のアイテムを強調表示
	}
	hitTestDragCategory	htc;
	int nIndex = _HitTestOnDragging(point, htc);
	if (htc == htNone) {
		_PressItem(-1);
		_CloseSubMenu();
		return DROPEFFECT_NONE;

	} else if (htc == htFolder) {
		LinkItem& item = *m_BookmarkList[nIndex];
		if (s_pSubMenu.get() == nullptr 
		|| s_pSubMenu->GetLinkFolderPtr() != item.pFolder)
			_DoPopupSubMenu(nIndex, true);	// サブメニューを開く
		return DROPEFFECT_MOVE | DROPEFFECT_LINK;

	} else if (htc == htInsert) {
		_PressItem(-1);
		_CloseSubMenu();
		return DROPEFFECT_MOVE | DROPEFFECT_LINK;
	} else if (htc == htChevron) {
		LinkItem& item = *m_BookmarkList.back();
		if (s_pSubMenu.get() == nullptr 
		|| s_pSubMenu->GetLinkFolderPtr() != item.pFolder) {
			_ChevronStateChange(ChvPressed);
			_PopupChevronMenu();
		}
		return DROPEFFECT_MOVE | DROPEFFECT_LINK;
	}

	return DROPEFFECT_NONE;
#if 0
	_hitTestFlag flag;
	int nIndex = _HitTestOnDragging(flag, point);

	_DrawInsertionEdge(flag, nIndex);

	if ( flag == htItem && _IsSameIndexDropped(nIndex) )
		return DROPEFFECT_NONE;

	if (!m_bDragFromItself)
		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
#endif
	//return _MtlStandardDropEffect(dwKeyState);
}

DROPEFFECT CDonutLinkBarCtrl::Impl::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	if (m_bAcceptDrag == false)
		return DROPEFFECT_NONE;

	_ClearInsertionEdge();

	_CloseSubMenu();

	auto nameUrl = CLinkItemDataObject::GetNameAndURL(pDataObject);

	hitTestDragCategory	htc;
	int nIndex = _HitTestOnDragging(point, htc);
	if (htc == htNone) {		
		return DROPEFFECT_NONE;

	} else {
		auto funcUpdateChevronPos = [this]() {
			CRect rcClient;
			this->GetClientRect(&rcClient);
			OnSize(0, rcClient.BottomRight());
		};
		// LinkItem 作成
		LinkItem* pLinkItemForFavicon = nullptr;
		unique_ptr<LinkItem> pItem;
		if (m_bDragItemIsLinkFile) {
			pItem.reset(new LinkItem);			
			pItem->strName	= nameUrl.first;
			pItem->strUrl	= nameUrl.second;
			pLinkItemForFavicon = pItem.get();
			//pItem->icon = CFaviconManager::GetFaviconFromURL(pItem->strUrl);
		} else {
			pItem.swap(m_DragItemData.first->at(m_DragItemData.second));
		}
		

		if (htc == htFolder || htc == htChevron) {
			LinkItem& targetLinkItem = *m_BookmarkList[nIndex];	// Drop先
			if (m_bDragItemIsLinkFile) {
				targetLinkItem.pFolder->push_back(std::move(pItem));	//外部からフォルダへ
			} else {
				ATLASSERT(targetLinkItem.pFolder);
				targetLinkItem.pFolder->push_back(std::move(pItem));
				m_DragItemData.first->erase(m_DragItemData.first->begin() + m_DragItemData.second);
				_UpdateItemPosition();
				funcUpdateChevronPos();
			}
			if (m_bDragItemIsLinkFile)
				CLinkPopupMenu::GetFaviconToLinkItem(pLinkItemForFavicon->strUrl, targetLinkItem.pFolder, pLinkItemForFavicon, m_hWnd);
			_SaveLinkBookmark();
			return DROPEFFECT_MOVE | DROPEFFECT_LINK;

		} else if (htc == htInsert) {
			if (m_nNowDragItemIndex != -1) {
				if (m_nNowDragItemIndex < nIndex) {	// ドラッグしてるアイテムの右側に挿入される
					m_BookmarkList.insert(m_BookmarkList.begin() + nIndex, std::move(pItem));
					m_BookmarkList.erase(m_BookmarkList.begin() + m_nNowDragItemIndex);
				} else {
					m_BookmarkList.insert(m_BookmarkList.begin() + nIndex, std::move(pItem));
					m_BookmarkList.erase(m_BookmarkList.begin() + m_nNowDragItemIndex + 1);	// 一つ増えたのでずらす
				}
			} else {
				m_BookmarkList.insert(m_BookmarkList.begin() + nIndex, std::move(pItem));
				if (m_bDragItemIsLinkFile == false)
					m_DragItemData.first->erase(m_DragItemData.first->begin() + m_DragItemData.second);
				else
					CLinkPopupMenu::GetFaviconToLinkItem(pLinkItemForFavicon->strUrl, &m_BookmarkList, pLinkItemForFavicon, m_hWnd);
			}

			_UpdateItemPosition();
			funcUpdateChevronPos();
			_SaveLinkBookmark();
			return DROPEFFECT_MOVE | DROPEFFECT_LINK;
		}
	}

	return DROPEFFECT_NONE;
}

void	CDonutLinkBarCtrl::Impl::OnDragLeave()
{
	_ClearInsertionEdge();

	if (s_pSubMenu) {
		CPoint pt;
		::GetCursorPos(&pt);
		HWND hWndpt = ::WindowFromPoint(pt);
		if (hWndpt != s_pSubMenu->m_hWnd)
			_CloseSubMenu();	// サブメニューにカーソルを移すつもりがなければ閉じる
	}
}


 int CDonutLinkBarCtrl::Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	OpenThemeData(VSCLASS_TOOLBAR);

	ATLVERIFY(RegisterDragDrop());

	_InitTooltip();

	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_font.CreateFontIndirect(&lf);

	m_themeRebar.OpenThemeData(m_hWnd, VSCLASS_REBAR);
	m_ChevronSize;
	if (m_themeRebar.IsThemeNull() == false)
		m_themeRebar.GetThemePartSize(CClientDC(NULL), RP_CHEVRON, CHEVS_NORMAL, NULL,TS_TRUE, &m_ChevronSize);
	else
		m_ChevronSize.SetSize(17, 24);

	s_hWnd = m_hWnd;
	CLinkPopupMenu::SetLinkBarHWND(m_hWnd);

	_LoadLinkBookmark();

	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

 void	_AddFaviconData(wptree& ptItem, LinkItem& item)
 {
		wptree ptFavicon;

	ICONINFO	IconInfo;
	item.icon.GetIconInfo(&IconInfo);

	int	bRet = 0;

	CClientDC	dc(NULL);
	{	// hbmColor
		enum { kBitmapInfoSize = sizeof(BITMAPINFOHEADER) + 64 };
		unique_ptr<BITMAPINFO>	pbmpInfo(reinterpret_cast<BITMAPINFO*>(new BYTE[kBitmapInfoSize]));
		::SecureZeroMemory((PVOID)pbmpInfo.get(), kBitmapInfoSize);
		BITMAPINFO&	bmpInfo = *pbmpInfo;
		bmpInfo.bmiHeader.biSize	= sizeof (bmpInfo.bmiHeader);
		bRet = ::GetDIBits(dc, IconInfo.hbmColor, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
		CTempBuffer<BYTE>	pBits(bmpInfo.bmiHeader.biSizeImage);
		bRet = ::GetDIBits(dc, IconInfo.hbmColor, 0, bmpInfo.bmiHeader.biHeight, (LPVOID)(BYTE*)pBits, &bmpInfo, DIB_RGB_COLORS);

		{
			int nDestSize = Base64EncodeGetRequiredLength(sizeof(bmpInfo.bmiHeader), ATL_BASE64_FLAG_NOCRLF);
			CTempBuffer<char>	buff(nDestSize);
			Base64Encode((BYTE*)&bmpInfo.bmiHeader, sizeof(bmpInfo.bmiHeader), buff, &nDestSize, ATL_BASE64_FLAG_NOCRLF);
			CString strBuff((LPSTR)buff, nDestSize);
			ptFavicon.add(L"ColorBmpInfoHeader", (LPCTSTR)strBuff);
		}
		{
			int nDestSize = Base64EncodeGetRequiredLength(bmpInfo.bmiHeader.biSizeImage, ATL_BASE64_FLAG_NOCRLF);
			CTempBuffer<char>	buff(nDestSize);
			Base64Encode(pBits, bmpInfo.bmiHeader.biSizeImage, buff, &nDestSize, ATL_BASE64_FLAG_NOCRLF);
			CString strBuff((LPSTR)buff, nDestSize);
			ptFavicon.add(L"ColorBits", (LPCTSTR)strBuff);
		}
	}

	{	// hbmMask
		enum { kBitmapInfoSize = sizeof(BITMAPINFOHEADER) + 64 };
		unique_ptr<BITMAPINFO>	pbmpInfo(reinterpret_cast<BITMAPINFO*>(new BYTE[kBitmapInfoSize]));
		::SecureZeroMemory((PVOID)pbmpInfo.get(), kBitmapInfoSize);
		BITMAPINFO&	bmpInfo = *pbmpInfo;
		bmpInfo.bmiHeader.biSize	= sizeof (bmpInfo.bmiHeader);
		::GetDIBits(dc, IconInfo.hbmMask, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
		CTempBuffer<BYTE>	pBits(bmpInfo.bmiHeader.biSizeImage);
		::GetDIBits(dc, IconInfo.hbmMask, 0, bmpInfo.bmiHeader.biHeight, (LPVOID)(BYTE*)pBits, &bmpInfo, DIB_RGB_COLORS);

		{
			int nDestSize = Base64EncodeGetRequiredLength(sizeof(bmpInfo.bmiHeader), ATL_BASE64_FLAG_NOCRLF);
			CTempBuffer<char>	buff(nDestSize);
			Base64Encode((BYTE*)&bmpInfo.bmiHeader, sizeof(bmpInfo.bmiHeader), buff, &nDestSize, ATL_BASE64_FLAG_NOCRLF);
			CString strBuff((LPSTR)buff, nDestSize);
			ptFavicon.add(L"MaskBmpInfoHeader", (LPCTSTR)strBuff);
		}
		{
			int nDestSize = Base64EncodeGetRequiredLength(bmpInfo.bmiHeader.biSizeImage, ATL_BASE64_FLAG_NOCRLF);
			CTempBuffer<char>	buff(nDestSize);
			Base64Encode(pBits, bmpInfo.bmiHeader.biSizeImage, buff, &nDestSize, ATL_BASE64_FLAG_NOCRLF);
			CString strBuff((LPSTR)buff, nDestSize);
			ptFavicon.add(L"MaskBits", (LPCTSTR)strBuff);
		}
	}
	ptItem.add_child(L"favicon", ptFavicon);
 }

#if _MSC_VER >= 1700
 void	_AddPtree(wptree& ptFolder, LinkFolderPtr pLinkFolder, std::atomic<bool>* pbCancel)
#else
void	_AddPtree(wptree& ptFolder, LinkFolderPtr pLinkFolder, bool* pbCancel)
#endif
 {
	 for (auto it = pLinkFolder->begin(); it != pLinkFolder->end(); ++it) {
#if _MSC_VER >= 1700
		 if (pbCancel->load())
#else
		 if (*pbCancel)
#endif
			 return ;
		 LinkItem& item = *it->get();
		 if (item.pFolder) {
			 if (item.strName == _T("ChevronFolder")) {	// Chevron
				 _AddPtree(ptFolder, item.pFolder, pbCancel);
			 } else {
				 wptree ptChildFolder;
				 _AddPtree(ptChildFolder, item.pFolder, pbCancel);
				 ptFolder.add_child(L"Folder", ptChildFolder).put(L"<xmlattr>.name", (LPCTSTR)item.strName);
			 }
		 } else {
			 wptree ptItem;
			 ptItem.add(L"name", (LPCTSTR)item.strName);
			 ptItem.add(L"url", (LPCTSTR)item.strUrl);

			 if (item.bExPropEnable) {
				 ptItem.add(L"ExProp", item.dwExProp.get());
				 ptItem.add(L"ExPropOpt", item.dwExPropOpt.get());
			 }

			 if (item.icon)
				 _AddFaviconData(ptItem, item);

			 ptFolder.add_child(L"Link", ptItem);
			 ::Sleep(1);
		 }
	 }
 }

 void CDonutLinkBarCtrl::Impl::OnDestroy()
 {
	CMessageLoop *pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);

	RevokeDragDrop();

	_ClearLinkBookmark();
 }


 void CDonutLinkBarCtrl::Impl::OnSize(UINT nType, CSize size)
 {
	 DefWindowProc();
	 _RefreshBandInfo();
	 if (m_bLoading)
		 return ;
	 LinkFolderPtr pFolder = m_BookmarkList.back()->pFolder;
	 for (;;) {
		 // ChevronFolderが見えてしまうので、ChevronFolderからアイテム補充
		 if (m_BookmarkList.back()->rcItem.left < size.cx) {
			 if (pFolder->size() > 0) {				 
				 m_BookmarkList.insert(m_BookmarkList.begin() +  m_BookmarkList.size() - 1, 
					 std::move(pFolder->front()));
				 pFolder->erase(pFolder->begin());
				 _UpdateItemPosition();
				 if (pFolder->size() == 0)	// なくなったので終了
					 break;
				 continue;
			 }			 
		 }
		 break;
	 }
	 for (;;) {
		if (m_BookmarkList.size() >= 2) {
			unique_ptr<LinkItem>& pRightItem = m_BookmarkList[m_BookmarkList.size() - 2];
			int LinkBarWidth = size.cx;
			if (pFolder->size() > 0)
				LinkBarWidth -= m_ChevronSize.cx;
			if (LinkBarWidth < pRightItem->rcItem.right) {	// アイテムが欠けるのでChevronFolderに入れる
				pFolder->insert(pFolder->begin(), std::move(pRightItem));
				m_BookmarkList.erase(m_BookmarkList.begin() + (m_BookmarkList.size() - 2));
				_UpdateItemPosition();
				continue;
			}
		}
		break;
	 }
 }

LRESULT CDonutLinkBarCtrl::Impl::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	HWND hWnd = ::WindowFromPoint(pt);
	if (hWnd && s_pSubMenu) {
		bool bChildWnd = false;
		CLinkPopupMenu* pSubMenu = s_pSubMenu.get();
		do {
			if (pSubMenu->m_hWnd == hWnd) {
				bChildWnd = true;
				break;
			}
		} while (pSubMenu = pSubMenu->GetSubMenu());
		if (bChildWnd) {
			::SendMessage(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		}
	}
	return 0;
}


void CDonutLinkBarCtrl::Impl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu || m_bLoading)
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
	
	int nIndex = -1;
	if (auto value = _HitTest(point)) {
		if (value.get() == ((int)m_BookmarkList.size() - 1)) {
			_CloseSubMenu();
			return ;	// 見えないようにしてるチェブロンフォルダなので
		}
		if (m_BookmarkList[value.get()]->pFolder == nullptr)
			_CloseSubMenu();
		_DoDragDrop(point, nFlags, value.get());
	} else {
		_CloseSubMenu();
	}
}

void CDonutLinkBarCtrl::Impl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	if (s_pSubMenu)
		return ;

	_PressItem(-1);
	if (auto value = _HitTest(point))
		_HotItem(value.get());
}

/// 右クリックメニューを表示する
void CDonutLinkBarCtrl::Impl::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	_CloseSubMenu();
	auto value = _HitTest(point);
	if (value && value.get() != (m_BookmarkList.size() - 1)) {
		CLinkPopupMenu::ShowRClickMenuAndExecCommand(&m_BookmarkList, m_BookmarkList[value.get()].get(), m_hWnd);
	} else {	// ItemNone
		CLinkPopupMenu::ShowRClickMenuAndExecCommand(&m_BookmarkList, nullptr, m_hWnd);	
	}
	Refresh();
}

void CDonutLinkBarCtrl::Impl::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return ;

	if (auto value = _HitTest(point)) {
		LinkItem& item = *m_BookmarkList[value.get()];
		if (item.pFolder)
			return ;
		CLinkPopupMenu::OpenLink(item, D_OPENFILE_CREATETAB | DonutGetStdOpenActivateFlag());
	}
}

void CDonutLinkBarCtrl::Impl::OnKillFocus(CWindow wndFocus)
{
	if (CLinkPopupMenu::s_bNowShowRClickMenu == false) {
		_PressItem(-1);
		Invalidate(FALSE);

		if (s_pSubMenu) 
			_CloseSubMenu();
	}
}

LRESULT CDonutLinkBarCtrl::Impl::OnTooltipGetDispInfo(LPNMHDR pnmh)
{
	if (s_pSubMenu)
		return 0;

	LPNMTTDISPINFO pntdi = (LPNMTTDISPINFO)pnmh;
	m_strTipText.Empty();
	if (CLinkPopupMenu::s_bNowShowRClickMenu)
		return 0;
	CPoint pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);
	if (auto value = _HitTest(pt)) {
		LinkItem& item = *m_BookmarkList[value.get()];
		if (value.get() == (m_BookmarkList.size() - 1))
			return 0;	// チェブロンはツールチップを表示しない
		if (item.pFolder) {
			//CClientDC dc(NULL);
			//CRect rcExtent;
			//
			//if (IsThemeNull() == false) {
			//	HRESULT hr = GetThemeTextExtent(dc, TP_BUTTON, TS_NORMAL, item.strName, item.strName.GetLength(), DT_SINGLELINE, NULL, &rcExtent);
			//} else {
			//	HFONT hPrev = dc.SelectFont(m_font);
			//	CSize	size;
			//	dc.GetTextExtent(item.strName, item.strName.GetLength(), &size);
			//	dc.SelectFont(hPrev);
			//	rcExtent.right	= size.cx;
			//}
			if (CLinkBarOption::s_bShowIconOnly == false) {
				const int kMaxItemTextWidth = CLinkBarOption::s_nMaxTextWidth;
				int nTextWidth = MTL::MtlComputeWidthOfText(item.strName, m_font);
				if (nTextWidth < kMaxItemTextWidth)
					return 0;
			}
			m_strTipText = item.strName;
		} else {
			m_strTipText = item.strName + _T("\r\n") + item.strUrl;
		}
		pntdi->lpszText = m_strTipText.GetBuffer(0);
	}
	return 0;
}


LRESULT CDonutLinkBarCtrl::Impl::OnCloseBaseSubMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		_UpdateItemPosition();
	return 0;
}


void	CDonutLinkBarCtrl::Impl::_InitTooltip()
{
	bool bUseTheme = false;
	// comctl32のバージョンを取得
    HINSTANCE         hComCtl;
    HRESULT           hr;
    DLLGETVERSIONPROC pDllGetVersion;
	DLLVERSIONINFO    dvi = { sizeof(DLLVERSIONINFO) };
    hComCtl = LoadLibrary(_T("comctl32.dll"));
    if (hComCtl) {
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hComCtl, "DllGetVersion");
        if (pDllGetVersion) {
            hr = (*pDllGetVersion)(&dvi);
            if (SUCCEEDED(hr) && dvi.dwMajorVersion >= 6) {
				bUseTheme = true;
			}
        }
    }
    FreeLibrary(hComCtl);

	m_tip.Create(m_hWnd);
	m_tip.Activate(TRUE);
	CToolInfo ti(TTF_SUBCLASS, m_hWnd);
	ti.hwnd	 = m_hWnd;
	if (bUseTheme == false)
		ti.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	ATLVERIFY(m_tip.AddTool(&ti));

	m_tip.SetMaxTipWidth(SHRT_MAX);
	m_tip.SetDelayTime(TTDT_AUTOPOP, 30 * 1000);
}

 //-----------------------------------
/// リバーを更新する
void	CDonutLinkBarCtrl::Impl::_RefreshBandInfo()
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

	WTL::CLogFont	lf;
	m_font.GetLogFont(&lf);
	int fontHeight = (int)lf.GetHeight();
	fontHeight += kTopBottomPadding * 2;

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
}

/// ptItem にある faviconツリーからアイコンを作成する
void	_AddFaviconDataToLinkItem(wptree& ptItem, LinkItem* pLinkItem)
{
	if (auto value = ptItem.get_child_optional(L"favicon")) {
		wptree& ptFavicon = value.get();
		CIcon& icon = pLinkItem->icon;
		ICONINFO IconInfo = { 0 };
		IconInfo.fIcon	= TRUE;

		{
			unique_ptr<BITMAPINFO>	pBmpInfo;
			{
				std::wstring str = ptFavicon.get<std::wstring>(L"ColorBmpInfoHeader");
				CW2A	temp(str.c_str());
				int nDestSize = Base64DecodeGetRequiredLength((int)str.length());	
				pBmpInfo.reset((BITMAPINFO*)new BYTE[nDestSize + 24]);
				::SecureZeroMemory((PVOID)pBmpInfo.get(), nDestSize + 24);
				Base64Decode(temp, (int)str.length(), (BYTE*)pBmpInfo.get(), &nDestSize);
			}
			unique_ptr<BYTE[]>	pBits;
			{
				std::wstring str = ptFavicon.get<std::wstring>(L"ColorBits");
				CW2A temp = str.c_str();
				int nDestSize = Base64DecodeGetRequiredLength((int)str.length());
				pBits.reset(new BYTE[nDestSize]);
				Base64Decode(temp, (int)str.length(), pBits.get(), &nDestSize);
			}
			CBitmapHandle bmp;
			BITMAPINFOHEADER& bmpHeader = pBmpInfo->bmiHeader;
			bmpHeader.biCompression	= 0;
			LPVOID	pvBits;
			bmp.CreateDIBSection(NULL, pBmpInfo.get(), DIB_RGB_COLORS, &pvBits, NULL, 0);
			::memcpy(pvBits, pBits.get(), bmpHeader.biSizeImage);
			//bmp.CreateBitmap(bmpHeader.biWidth, -bmpHeader.biHeight, bmpHeader.biPlanes, bmpHeader.biBitCount, (void*)pBits.get());
			/*bmp.CreateDIBitmap(dc, &pBmpInfo->bmiHeader, 0, NULL, NULL, DIB_RGB_COLORS);
			int n = bmp.SetDIBits(dc, 0, pBmpInfo->bmiHeader.biHeight, (void*)pBits.get(), pBmpInfo.get(), DIB_RGB_COLORS);*/
			IconInfo.hbmColor	= bmp.Detach();
		}
		{
			unique_ptr<BITMAPINFO>	pBmpInfo;
			{
				std::wstring str = ptFavicon.get<std::wstring>(L"MaskBmpInfoHeader");
				CW2A temp = str.c_str();
				int nDestSize = Base64DecodeGetRequiredLength((int)str.length());
				pBmpInfo.reset((BITMAPINFO*)new BYTE[nDestSize + 24]);
				::SecureZeroMemory((PVOID)pBmpInfo.get(), nDestSize + 24);
				Base64Decode(temp, (int)str.length(), (BYTE*)pBmpInfo.get(), &nDestSize);
			}

			unique_ptr<BYTE[]>	pBits;
			{
				std::wstring str = ptFavicon.get<std::wstring>(L"MaskBits");
				CW2A	temp = str.c_str();
				int nDestSize = Base64DecodeGetRequiredLength((int)str.length());
				pBits.reset(new BYTE[nDestSize]);
				Base64Decode(temp, (int)str.length(), pBits.get(), &nDestSize);
			}
			CBitmapHandle bmp;
			BITMAPINFOHEADER& bmpHeader = pBmpInfo->bmiHeader;
			bmpHeader.biCompression	= 0;
			LPVOID	pvBits;
			bmp.CreateDIBSection(NULL, pBmpInfo.get(), DIB_RGB_COLORS, &pvBits, NULL, 0);
			::memcpy(pvBits, pBits.get(), bmpHeader.biSizeImage);
			//bmp.CreateBitmap(bmpHeader.biWidth, -bmpHeader.biHeight, bmpHeader.biPlanes, bmpHeader.biBitCount, (void*)pBits.get());
			/*bmp.CreateDIBitmap(dc, &pBmpInfo->bmiHeader, 0, NULL, NULL, 0);
			bmp.SetDIBits(dc, 0, pBmpInfo->bmiHeader.biHeight, (void*)pBits.get(), pBmpInfo.get(), DIB_RGB_COLORS);*/
			IconInfo.hbmMask	= bmp.Detach();
		}
		
		icon.CreateIconIndirect(&IconInfo);
		ATLASSERT(icon.m_hIcon);
		::DeleteObject(IconInfo.hbmColor);
		::DeleteObject(IconInfo.hbmMask);
	}
}

void	_AddLinkItem(LinkFolderPtr pFolder, wptree pt)
{
	for (auto it = pt.begin(); it != pt.end(); ++it) {
		if (it->first == L"<xmlattr>")
			continue;
		wptree& ptItem = it->second;
		unique_ptr<LinkItem>	pItem(new LinkItem);		
		if (it->first == L"Link") {
			pItem->strName	= ptItem.get<std::wstring>(L"name").c_str();
			pItem->strUrl	= ptItem.get<std::wstring>(L"url").c_str();
			if (auto value = ptItem.get_optional<DWORD>(L"ExProp")) {
				pItem->bExPropEnable = true;
				pItem->dwExProp	= value.get();
				pItem->dwExPropOpt	= ptItem.get<DWORD>(L"ExPropOpt");
			}
			_AddFaviconDataToLinkItem(ptItem, pItem.get());
			//::Sleep(1);
		} else if (it->first == L"Folder") {
			pItem->strName	= ptItem.get<std::wstring>(L"<xmlattr>.name").c_str();
			pItem->pFolder = new LinkFolder;		
			_AddLinkItem(pItem->pFolder, ptItem);
		}
		pFolder->push_back(std::move(pItem));
	}
}


void	CDonutLinkBarCtrl::Impl::_LoadLinkBookmark()
{
	m_bLoading = true;
	m_thread_load = boost::thread([this]() {
		CString LinkBookmarkFilePath = GetConfigFilePath(_T("LinkBookmark.xml"));

		if (::PathFileExists(LinkBookmarkFilePath) == FALSE) {
			CString LinkFolder;
			CIniFileIO	pr(g_szIniFileName, _T("LinkBar"));
			LinkFolder = pr.GetStringUW(_T("RootPath"));
			pr.DeleteValue(_T("RootPath"));
			if (LinkFolder.IsEmpty()) {
				MtlGetFavoriteLinksFolder(LinkFolder);
			}
			LinkImportFromFolder(LinkFolder);
			m_bLoading = false;
			//Refresh();
			::PostMessage(m_hWnd, WM_REFRESH, 0, 0);
			return ;
		}
		try {
			std::wifstream	filestream(LinkBookmarkFilePath);
			if (!filestream)
				return ;

			filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
			wptree	pt;
			read_xml(filestream, pt);

			if (auto optChild = pt.get_child_optional(L"LinkBookmark")) {
				wptree ptChild = optChild.get();
				_AddLinkItem(&m_BookmarkList, ptChild);
			}

		} catch (const boost::property_tree::ptree_error& error) {
			CString strError = _T("LinkBookmark.xmlの読み込みに失敗\n");
			strError += error.what();
			this->MessageBox(strError, NULL, MB_ICONERROR);
			::MoveFileEx(LinkBookmarkFilePath, LinkBookmarkFilePath + _T(".error"), MOVEFILE_REPLACE_EXISTING);
		}

		unique_ptr<LinkItem> pItem(new LinkItem);
		LinkFolderPtr	pFolder = new LinkFolder;
		pItem->pFolder	= pFolder;
		pItem->strName = _T("ChevronFolder");
		m_BookmarkList.push_back(std::move(pItem));
		m_bLoading = false;
		//Refresh();
		::PostMessage(m_hWnd, WM_REFRESH, 0, 0);
	});
}

void	CDonutLinkBarCtrl::Impl::_SaveLinkBookmark()
{
	static CCriticalSection	s_cs;
#if _MSC_VER >= 1700
	static std::atomic<bool> s_bCancel(false);
#else
	static bool	s_bCancel = false;
#endif
	m_thread_save.swap(boost::thread([this]() {
		for (;;) {
			if (s_cs.TryEnter()) {
				CString LinkBookmarkFilePath = GetConfigFilePath(_T("LinkBookmark.xml"));
				CString tempPath = GetConfigFilePath(_T("LinkBookmark.temp.xml"));
				try {
					std::wofstream	filestream(tempPath);
					if (!filestream) {
						s_cs.Leave();
#if _MSC_VER >= 1700
						s_bCancel.store(false);
#else
						s_bCancel = false;
#endif
						return ;
					}
		
					wptree	pt;
					_AddPtree(pt.add(L"LinkBookmark", L""), &m_BookmarkList, &s_bCancel);

					filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));
					write_xml(filestream, pt, xml_writer_make_settings<std::wstring>(L' ', 2, widen<std::wstring>("UTF-8")));
					filestream.close();
					::MoveFileEx(tempPath, LinkBookmarkFilePath, MOVEFILE_REPLACE_EXISTING);

				} catch (const boost::property_tree::ptree_error& error) {
					CString strError = _T("LinkBookmark.xmlへの書き込みに失敗\n");
					strError += error.what();
					this->MessageBox(strError, NULL, MB_ICONERROR);
				}
				s_cs.Leave();
#if _MSC_VER >= 1700
				s_bCancel.store(false);
#else
				s_bCancel = false;
#endif
				break;

			} else {
				// 他のスレッドが保存処理を実行中...
				TRACEIN(_T("_SaveLinkBookmark : TryEnter failed"));
#if _MSC_VER >= 1700
				s_bCancel.store(true);
#else
				s_bCancel = true;
#endif
				::Sleep(100);
				continue;
			}
		}
	}));
}

void	CDonutLinkBarCtrl::Impl::_UpdateItemPosition()
{
	const int kMaxItemTextWidth = CLinkBarOption::s_nMaxTextWidth;
	CRect rcClient;
	GetClientRect(&rcClient);
	int nLeftPos = kLeftMargin;
	for (auto it = m_BookmarkList.begin(); it != m_BookmarkList.end(); ++it) {
		int nTextWidth = MTL::MtlComputeWidthOfText(it->get()->strName, m_font);
		if (kMaxItemTextWidth < nTextWidth) {
			nTextWidth = kMaxItemTextWidth;			
		}
		CRect&	rcItem = it->get()->rcItem;
		rcItem.top	= 0;
		rcItem.left	= nLeftPos;
		rcItem.bottom	= rcClient.bottom;
		rcItem.right	= nLeftPos + kLeftTextPadding;
		if (CLinkBarOption::s_bShowIconOnly == false || it->get()->pFolder) 
			rcItem.right += nTextWidth + (kRightLeftPadding/* * 2*/);
		nLeftPos = rcItem.right;

		it->get()->ModifyState(it->get()->kItemNormal);
	}
	Invalidate(FALSE);
}

boost::optional<int>	CDonutLinkBarCtrl::Impl::_HitTest(const CPoint& point)
{
	int nCount = (int)m_BookmarkList.size();
	for (int i = 0; i < nCount; ++i) {
		if (m_BookmarkList[i]->rcItem.PtInRect(point))
			return i;
	}
	return boost::none;
}

void	CDonutLinkBarCtrl::Impl::_HotItem(int nNewHotIndex)
{
	// clean up
	if ( _IsValidIndex(m_nHotIndex) ) {
		LinkItem& item = *m_BookmarkList[m_nHotIndex];
		if ( item.ModifyState(item.kItemNormal) )
			InvalidateRect(item.rcItem);
	}

	m_nHotIndex = nNewHotIndex;

	if ( _IsValidIndex(m_nHotIndex) ) {
		LinkItem& item = *m_BookmarkList[m_nHotIndex];
		//if (item.state != item.kItemPressed) {
			if ( item.ModifyState(item.kItemHot) )
				InvalidateRect(item.rcItem);
		//}
	}
}

void	CDonutLinkBarCtrl::Impl::_PressItem(int nPressedIndex)
{
	// clean up prev
	if ( _IsValidIndex(m_nPressedIndex) ) {
		LinkItem& item = *m_BookmarkList[m_nPressedIndex];
		if ( item.ModifyState(item.kItemNormal) )
			InvalidateRect(item.rcItem);
	}

	m_nPressedIndex = nPressedIndex;

	if ( _IsValidIndex(m_nPressedIndex) ) {
		LinkItem& item = *m_BookmarkList[m_nPressedIndex];
		if ( item.ModifyState(item.kItemPressed) )
			InvalidateRect(item.rcItem);
	}
}

void	CDonutLinkBarCtrl::Impl::_CloseSubMenu()
{
	if (s_pSubMenu) {
		if (s_pSubMenu->GetLinkFolderPtr() == m_BookmarkList.back()->pFolder
			&& m_ChevronState == ChvPressed)
			_ChevronStateChange(ChvNormal);

		s_pSubMenu->DestroyWindow();
		s_pSubMenu.release();		// s_pSubMenuは勝手にdeleteされるので

		::UnhookWindowsHookEx(s_hHook);
		s_hHook = NULL;

		_PressItem(-1);
	}
}

/// nIndexがフォルダじゃなければ false を返す
bool	CDonutLinkBarCtrl::Impl::_DoPopupSubMenu(int nIndex, bool bPressItem /*= true*/)
{
	ATLASSERT(_IsValidIndex(nIndex));
	CLinkPopupMenu::SetLinkBarHWND(m_hWnd);
	_CloseSubMenu();

	LinkFolderPtr	pFolder = m_BookmarkList[nIndex]->pFolder;
	if (pFolder == nullptr)
		return false;

	SetFocus();
	
	s_pSubMenu.reset(new CLinkPopupMenu(pFolder));
	CRect	rcWindow;
	rcWindow.right	= s_pSubMenu->ComputeWindowWidth();
	rcWindow.bottom	= s_pSubMenu->ComputeWindowHeight();
	CRect	rcButton = m_BookmarkList[nIndex]->rcItem;
	ClientToScreen(&rcButton);
	rcWindow.MoveToXY(rcButton.left, rcButton.bottom);	
	CRect rcWork = Misc::GetMonitorWorkArea(m_hWnd);
	if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
		rcWindow.bottom	= rcWork.bottom;
		static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
		rcWindow.right += s_nVScrollWidth;
	}
	if (rcWork.right < rcWindow.right) {	// 右にはみ出る
		int nWidth = rcWindow.Width();
		rcWindow.MoveToX(rcWork.right - nWidth);
	}

	s_pSubMenu->Create(GetDesktopWindow(), rcWindow, NULL, WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);
	s_pSubMenu->ShowWindow(SW_SHOWNOACTIVATE);

	ATLVERIFY(s_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, _Module.GetModuleInstance(), 0));
	
	if (bPressItem)
		_PressItem(nIndex);

	return true;
}

void	CDonutLinkBarCtrl::Impl::_PopupChevronMenu()
{
	_CloseSubMenu();

	SetFocus();

	CRect rcClient;
	GetClientRect(&rcClient);

	s_pSubMenu.reset(new CLinkPopupMenu(m_BookmarkList.back()->pFolder));
	CRect	rcWindow;
	rcWindow.right	= s_pSubMenu->ComputeWindowWidth();
	rcWindow.bottom	= s_pSubMenu->ComputeWindowHeight();
	
	CPoint ptLeftBottom;
	ptLeftBottom.x = rcClient.right - rcWindow.right;
	ptLeftBottom.y = rcClient.bottom;
	ClientToScreen(&ptLeftBottom);
	rcWindow.MoveToXY(ptLeftBottom.x, ptLeftBottom.y);
	CRect rcWork = Misc::GetMonitorWorkArea(m_hWnd);
	if (rcWork.bottom < rcWindow.bottom) {	// 下にはみ出る
		rcWindow.bottom	= rcWork.bottom;
		static const int s_nVScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
		rcWindow.right += s_nVScrollWidth;
		rcWindow.MoveToX(ptLeftBottom.x - s_nVScrollWidth);
	}
	if (rcWork.right < rcWindow.right) {	// 右にはみ出る
		int nWidth = rcWindow.Width();
		rcWindow.MoveToX(rcWork.right - nWidth);
	}

	s_pSubMenu->Create(GetDesktopWindow(), rcWindow, NULL, /*WS_VISIBLE | */WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);
	s_pSubMenu->ShowWindow(SW_SHOWNOACTIVATE);

	ATLVERIFY(s_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, _Module.GetModuleInstance(), 0));
	
}


/// メニュー以外の場所をクリックしたときにメニューを閉じる
LRESULT CALLBACK CDonutLinkBarCtrl::Impl::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(s_hHook, nCode, wParam, lParam);
	if (nCode == HC_ACTION) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN: {
			LPMSLLHOOKSTRUCT pllms = (LPMSLLHOOKSTRUCT)lParam;
			HWND hWnd = WindowFromPoint(pllms->pt);

			bool bChildWnd = false;
			CLinkPopupMenu* pSubMenu = s_pSubMenu.get();
			do {
				if (pSubMenu->m_hWnd == hWnd) {
					bChildWnd = true;
					break;
				}
			} while (pSubMenu = pSubMenu->GetSubMenu());

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


void	CDonutLinkBarCtrl::Impl::_DoDragDrop(const CPoint& pt, UINT nFlags, int nIndex)
{
	_PressItem(nIndex);
	UpdateWindow();
	if ( PreDoDragDrop(m_hWnd) ) {
		_CloseSubMenu();
		_HotItem(nIndex);

		CComPtr<IDataObject> spDataObject = CLinkItemDataObject::CreateInstance(&m_BookmarkList, nIndex);
		if ( spDataObject ) {
			const LinkItem& item = *m_BookmarkList[nIndex];
			if (CLinkBarOption::s_bNoShowDragImage == false) {
				CBitmapHandle bmp;
				static const COLORREF	whiteColor = RGB(0xFF, 0xFF, 0xFF);
				static const COLORREF	blackColor = RGB(0, 0, 0);
				static const COLORREF	boundColor = RGB(212, 219, 237);
				{
					CClientDC	wndDC(m_hWnd);
					CDC dc = ::CreateCompatibleDC(wndDC);
					bmp = CreateCompatibleBitmap(wndDC, item.rcItem.Width(), item.rcItem.Height());
					dc.SetBkMode(TRANSPARENT);
					HFONT hFontPrev = dc.SelectFont(m_font);
					HBITMAP hPrevBMP = dc.SelectBitmap(bmp);
					::OffsetWindowOrgEx(dc, item.rcItem.left, item.rcItem.top, NULL);
					CBrush brs = ::CreateSolidBrush(boundColor);
					dc.FillRect(item.rcItem, brs);

					_DrawDragImage(dc.m_hDC, item);
					const CRect& rcItem = item.rcItem;
																								#if 0
				{
					Gdiplus::Graphics	graphics(dc);
					graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

					Gdiplus::GraphicsPath path;
					WTL::CLogFont	lf;
					m_font.GetLogFont(&lf);
					Gdiplus::FontFamily	fontFamily(lf.lfFaceName);
					Gdiplus::StringFormat	strFormat;

					CRect rcText = item.rcItem;
					rcText.left	+= kLeftTextPadding - 2;//+= kRightLeftPadding;
					//rcText.right-= kRightTextMargin;
					rcText.top	+= 3;
					path.AddString(item.strName, item.strName.GetLength(), &fontFamily, Gdiplus::FontStyleRegular, ::abs(lf.lfHeight), Gdiplus::Rect(rcText.left, rcText.top, rcText.Width(), rcText.Height()), &strFormat);

					Gdiplus::SolidBrush foreBrush(Gdiplus::Color(0, 0, 0));
					Gdiplus::SolidBrush borderBrush(Gdiplus::Color(212, 219, 237));
					Gdiplus::Pen pen(&borderBrush, 4.0F);
					pen.SetLineJoin(Gdiplus::LineJoinRound);
					graphics.DrawPath(&pen, &path);
					graphics.FillPath(&foreBrush, &path);

				}
				::SetWindowOrgEx(dc.m_hDC, 0, 0, NULL);
				dc.SelectBitmap(hPrevBMP);
				dc.SelectFont(hFontPrev);
	#endif
				}
				if (bmp) {
					SHDRAGIMAGE	sdimg;
					sdimg.hbmpDragImage = bmp;
					sdimg.crColorKey = boundColor;
					sdimg.sizeDragImage = CSize(item.rcItem.Width(), item.rcItem.Height());
					sdimg.ptOffset	= CPoint(pt.x - item.rcItem.left, pt.y  - item.rcItem.top );
					if (FAILED(m_spDragSourceHelper->InitializeFromBitmap(&sdimg, spDataObject)))
						bmp.DeleteObject();
				}
			}
			m_nBeforeBookmarkListSize = m_BookmarkList.size();
			m_nNowDragItemIndex = nIndex;
			//m_bDragFromItself = true;
			DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
			//m_bDragFromItself = false;
			m_nNowDragItemIndex = -1;
			// 外部に移動されたので更新する
			if (m_nBeforeBookmarkListSize != m_BookmarkList.size())
				_UpdateItemPosition();
			_CloseSubMenu();
		}
		_PressItem(-1);
		_HotItem(-1);

	} else {	// Drag操作をしていなかったのでアイテムを開く
		if (s_pSubMenu) {
			_CloseSubMenu();
		} else {
			if (_DoPopupSubMenu(nIndex) == false) {
				CLinkPopupMenu::OpenLink(*m_BookmarkList[nIndex], DonutGetStdOpenFlag());
			}
		}
	}
}


void	CDonutLinkBarCtrl::Impl::_DrawInsertEdge(const CPoint& ptRightBottom)
{
	const CPoint& pt = ptRightBottom;
	CClientDC	 dc(m_hWnd);
	CBrush		 hbr;
	hbr.CreateSolidBrush( ::GetSysColor(COLOR_3DDKSHADOW) );
	dc.SetBrushOrg(pt.x, 0);
	HBRUSH hbrOld = dc.SelectBrush(hbr);

	POINT pts[] = { 
		{ pt.x , 0 }, 
		{ pt.x , pt.y }, 
		{ pt.x + 1, pt.y },
		{ pt.x + 1, 0 },
	};
	dc.Polygon( pts, _countof(pts) );

	dc.SelectBrush(hbrOld);
}

void	CDonutLinkBarCtrl::Impl::_ClearInsertionEdge()
{
	if (m_ptInvalidateOnDrawingInsertionEdge != CPoint()) {
		CRect rc;
		rc.left	= m_ptInvalidateOnDrawingInsertionEdge.x;
		rc.right= m_ptInvalidateOnDrawingInsertionEdge.x + 2;
		rc.bottom= m_ptInvalidateOnDrawingInsertionEdge.y;
		InvalidateRect(&rc);
		UpdateWindow();
		m_ptInvalidateOnDrawingInsertionEdge.SetPoint(0, 0);
	}
}


 bool	CDonutLinkBarCtrl::Impl::_HitTestChevron(const CPoint& pt)
 {
	 CRect rcClient;
	 GetClientRect(&rcClient);

	 if (m_BookmarkList.back()->pFolder->size() > 0) {
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


 void	CDonutLinkBarCtrl::Impl::_ChevronStateChange(ChevronState state)
 {
	if (m_ChevronState != state) {
		m_ChevronState = state;

		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChevron(rcClient.right - m_ChevronSize.cx, 0, rcClient.right, rcClient.bottom);
		InvalidateRect(rcChevron, FALSE);
	}
 }



void	CDonutLinkBarCtrl::Impl::_DrawItem(CDCHandle dc, const LinkItem& item)
{
	const CRect& rcItem = item.rcItem;
	CRect rcText = item.rcItem;
	rcText.left	+= kLeftTextPadding;//+= kRightLeftPadding;
	rcText.right-= kRightTextMargin;

	static bool s_bVistaLater = _CheckOsVersion_VistaLater() != 0;

	auto funcDrawIcon = [&, this]() {
		POINT ptIcon;
		ptIcon.x	= rcItem.left + kLeftIconMargin;
		ptIcon.y	= rcItem.top  + kTopIconMargin;
		if (IsThemeNull() && item.state == item.kItemPressed) {
			ptIcon.x += 2;
			ptIcon.y += 2;
		}
		CIconHandle icon;
		if (item.pFolder) {
			icon = CLinkPopupMenu::s_iconFolder;
		} else {
			icon = item.icon;
			if (icon == NULL)
				icon = CLinkPopupMenu::s_iconLink;
		}
		icon.DrawIconEx(dc, ptIcon, CSize(kcxIcon, kcyIcon));
	};
	if (IsThemeNull() == false) {
		if (s_bVistaLater)
			funcDrawIcon();
		int nState = TS_NORMAL;
		if (item.state == item.kItemHot)
			nState = TS_HOT;
		else if (item.state == item.kItemPressed)
			nState = TS_PRESSED;

		DrawThemeBackground(dc, TP_BUTTON, nState, &rcItem);

		if (s_bVistaLater == false)
			funcDrawIcon();
		//DrawThemeText(dc, TP_BUTTON, nState, item.strName, item.strName.GetLength(), DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS, 0, &rcText);
	} else {
		CRect rcEdge = item.rcItem;
		rcEdge.DeflateRect(1, 1);
		if (item.state == item.kItemHot) {
			dc.Draw3dRect(rcEdge, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
		} else if (item.state == item.kItemPressed) {
			dc.Draw3dRect(rcEdge, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
			rcText.top += 2;
			rcText.left	+= 2;
		}
		funcDrawIcon();
	}



	dc.DrawText(item.strName, item.strName.GetLength(), &rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
}


void	CDonutLinkBarCtrl::Impl::_DrawDragImage(CDCHandle dc, const LinkItem& item)
{
	const CRect& rcItem = item.rcItem;
	POINT ptIcon;
	ptIcon.x	= rcItem.left + kLeftIconMargin;
	ptIcon.y	= rcItem.top  + kTopIconMargin;
	if (IsThemeNull() && item.state == item.kItemPressed) {
		ptIcon.x += 2;
		ptIcon.y += 2;
	}
	CIconHandle icon;
	if (item.pFolder) {
		icon = CLinkPopupMenu::s_iconFolder;
	} else {
		icon = item.icon;
		if (icon == NULL)
			icon = CLinkPopupMenu::s_iconLink;
	}
	icon.DrawIconEx(dc, ptIcon, CSize(kcxIcon, kcyIcon));

	CRect rcText = item.rcItem;
	rcText.left	+= kLeftTextPadding - 2;//+= kRightLeftPadding;
	rcText.right-= kRightTextMargin;
	rcText.top += 2;

	Gdiplus::Graphics	graphics(dc);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	Gdiplus::Font	font(dc, m_font);
	Gdiplus::SolidBrush	fontColor(Gdiplus::Color(255, 0, 0, 0));
	graphics.DrawString(item.strName, item.strName.GetLength(), &font, Gdiplus::PointF(Gdiplus::REAL(rcText.left), Gdiplus::REAL(rcText.top)), NULL, &fontColor);

	//dc.DrawText(item.strName, item.strName.GetLength(), &rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}


void	CDonutLinkBarCtrl::Impl::_ClearLinkBookmark()
{
	// 以前のリンクフォルダを削除する
	//std::function<void (LinkFolderPtr)>	funcDelFolder;
	//funcDelFolder	= [&funcDelFolder] (LinkFolderPtr pFolder) {
	//	for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
	//		if (it->get()->pFolder) {
	//			funcDelFolder(it->get()->pFolder);
	//			delete it->get()->pFolder;
	//		}
	//	}
	//};
	//funcDelFolder(&m_BookmarkList);
	m_BookmarkList.clear();
}






