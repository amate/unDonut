// BingTranslatorMenu.cpp

#include "stdafx.h"
#include "BingTranslatorMenu.h"
#include <boost/thread.hpp>
#include <WinInet.h>
#include "ChildFrame.h"
#include "MtlMisc.h"
#include "IniFile.h"
#include "DonutDefine.h"

LPCTSTR appID = _T("6CB08565F99A903FB046716AA865A256A122E24C");
LPCTSTR kUserAgent = _T("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

typedef CWinTraits<WS_THICKFRAME | WS_SYSMENU | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW | WS_EX_TOPMOST/*WS_EX_CLIENTEDGE*/> CDicWindowTraits;

///////////////////////////////////////////////////
// CDictionaryWindow

class CDictionaryWindow : public CWindowImpl<CDictionaryWindow, CWindow, CDicWindowTraits>
{
public:
	CDictionaryWindow()
	{
		CIniFileI	pr(g_szIniFileName, _T("DictionaryWindow"));
		m_Size.cx	= (int)pr.GetValue(_T("Width"), 250);
		m_Size.cy	= (int)pr.GetValue(_T("Height"), 270);
	}

	void	SetWord(const CString& strWord) 
	{
		if (m_strWord != strWord) {
			m_strWord = strWord;
			m_strTranslated.Empty();

			auto funcDictionary = [this] () {
				HINTERNET hInternet = NULL;
				HINTERNET hFile		= NULL;
				try {
					hInternet = ::InternetOpen(kUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
					if (hInternet == NULL) 
						throw _T("InternetOpenに失敗");

					CString strUrl;
					strUrl.Format(_T("http://www.microsofttranslator.com/dictionary.ashx?from=en&to=ja&text=%s"), m_strWord);
					hFile = ::InternetOpenUrl(hInternet, strUrl, NULL, 0, 0, 0);
					if (hFile == NULL)
						throw _T("InternetOpenUrlに失敗");

					std::string strTempBuff;
					while (1) {
						enum { BuffSize = 512 };
						char Buff[BuffSize + 1] = "\0";
						DWORD ReadSize = 0;
						if (::InternetReadFile(hFile, (LPVOID)Buff, BuffSize, &ReadSize) && ReadSize == 0)
							break;

						Buff[ReadSize] = '\0';
						strTempBuff += Buff;
					}
					CString strBody = Misc::utf8_to_CString(strTempBuff.c_str());
					int nBegin = strBody.Find(_T('"'));
					int nEnd   = strBody.ReverseFind(_T('"'));
					if (nBegin == -1 || nEnd == -1 || nEnd == nBegin + 1)
						throw _T("該当なし");

					strBody = strBody.Mid(nBegin + 1, nEnd - nBegin - 1);
					CString strTranslated = Misc::urlstr_decodeJpn(strBody, 3);
					strTranslated.Replace(_T("<br />"), _T("\r\n"));
					strTranslated.Replace(_T("<hr />"), _T("―――――――――――――\r\n"));
					strTranslated.Replace(_T("<span class=\"dictI\">"), _T(""));
					strTranslated.Replace(_T("</span>"), _T(""));

					m_strTranslated = strTranslated;
					if (m_Edit.IsWindow()) {
						m_Edit.SetWindowText(_T(""));
						m_Edit.AppendText(strTranslated, TRUE);
						m_Edit.SetSelNone(TRUE);
					}
				}
				catch (LPCTSTR strError) {
					m_strTranslated = strError;
					if (m_Edit.IsWindow()) {
						m_Edit.SetWindowText(_T(""));
						m_Edit.AppendText(strError, TRUE);
					}
					TRACEIN(strError);
				}
				if (hFile)		::InternetCloseHandle(hFile);
				if (hInternet)	::InternetCloseHandle(hInternet);

			};
		

			boost::thread td(funcDictionary);
		}

	}
	void	KeepShow() 
	{
		MoveWindow(m_rcDefault);
		ShowWindow(TRUE);
	}

	CRect	ColcWindowPos(const CRect& rcMenu)
	{
		CPoint pt;
		::GetCursorPos(&pt);
		HMONITOR	hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		MONITORINFO moniInfo = { sizeof (MONITORINFO) };
		::GetMonitorInfo(hMonitor, &moniInfo);

		enum { cxMargin = -2, cyMargin = -20 };

		CRect rcWindow;
		rcWindow.top	= rcMenu.top + cyMargin;
		rcWindow.left	= rcMenu.right + cxMargin;
		rcWindow.right	= rcWindow.left + m_Size.cx;
		rcWindow.bottom	= rcWindow.top + m_Size.cy;
			
		if (rcWindow.right > moniInfo.rcWork.right) {	// モニターの幅を超えてる
			rcWindow.left	= rcMenu.left - cxMargin - m_Size.cx;
			rcWindow.right	= rcWindow.left + m_Size.cx;
		}
		if (rcWindow.bottom	> moniInfo.rcWork.bottom) {	// モニターの下幅を超えてる
			int nButtomMargin = rcWindow.bottom	- moniInfo.rcWork.bottom;
			rcWindow.MoveToY(rcWindow.top - nButtomMargin);
		}
		m_rcDefault = rcWindow;
		return rcWindow;
	}

	BEGIN_MSG_MAP( x )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_SIZE( OnSize )
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		SetWindowText(m_strWord);

		WTL::CLogFont	lf;
		lf.SetMenuFont();
		m_Edit.Create(m_hWnd, 0, NULL, WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);
		m_Edit.SetFont(lf.CreateFontIndirect());
		//m_Edit.MoveWindow(0, 0, 250, 270);
		m_Edit.ShowWindow(SW_SHOWNOACTIVATE);
		m_Edit.AppendText(m_strTranslated, TRUE);
		return 0;
	}

	void OnDestroy()
	{
		ModifyStyle(WS_BORDER, 0);

		if (m_Edit.IsWindow())
			m_Edit.DestroyWindow();
		m_Edit.m_hWnd = NULL;

		CIniFileO	pr(g_szIniFileName, _T("DictionaryWindow"));
		pr.SetValue(m_Size.cx, _T("Width"));
		pr.SetValue(m_Size.cy, _T("Height"));
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
	CString m_strTranslated;
	CRect	m_rcDefault;
	CSize	m_Size;
};

/////////////////////////////////////////////////////////////////////
// CBingTranslatorMenu

// Constructor
CBingTranslatorMenu::CBingTranslatorMenu(CChildFrame* p)
	: m_pChildFrame(p)
	, m_pDicWindow(new CDictionaryWindow)
{
	m_menu.CreatePopupMenu();
	m_menu.AppendMenu(0, (UINT_PTR)0, _T("..."));
	//m_menu.AppendMenu(MF_SEPARATOR, (UINT_PTR)0);
}

// Destructor
CBingTranslatorMenu::~CBingTranslatorMenu()
{
	delete m_pDicWindow;
}

void CBingTranslatorMenu::OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu)
{
	if (menuPopup.m_hMenu == m_menu.m_hMenu) {
		CString strSelected = m_pChildFrame->GetSelectedTextLine();
		if (m_strSelectedText == strSelected)
			return;

		m_strSelectedText = strSelected;

		while (m_menu.GetMenuItemCount() > 1) 
			m_menu.DeleteMenu(1, MF_BYPOSITION);
		m_menu.ModifyMenu(0, MF_BYPOSITION, (UINT_PTR)0, _T("..."));

		if (strSelected.IsEmpty())
			return;

		if (strSelected.GetLength() > 512) {
			m_menu.ModifyMenu(0, MF_BYPOSITION, (UINT_PTR)0, _T("文字数が多すぎます"));
			return;
		}

		auto funcTranslate = [this, strSelected] () {
			HINTERNET hInternet = NULL;
			HINTERNET hFile		= NULL;
			try {
				hInternet = ::InternetOpen(_T("Mozilla/5.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			if (hInternet == NULL) 
				throw _T("InternetOpenに失敗");

			CString strUrl;
			strUrl.Format(_T("http://api.microsofttranslator.com/v2/Http.svc/Translate?appId=%s&text=%s&from=en&to=ja"), appID, strSelected);
			hFile = ::InternetOpenUrl(hInternet, strUrl, NULL, 0, 0, 0);
			if (hFile == NULL)
				throw _T("InternetOpenUrlに失敗");

			std::string strTempBuff;
			while (1) {
				enum { BuffSize = 512 };
				char Buff[BuffSize] = "\0";
				DWORD ReadSize = 0;
				if (::InternetReadFile(hFile, (LPVOID)Buff, BuffSize, &ReadSize) && ReadSize == 0)
					break;

				Buff[ReadSize] = '\0';
				strTempBuff += Buff;
			}
			CString strBody = Misc::utf8_to_CString(strTempBuff.c_str());
			int nBegin = strBody.Find(_T('>'));
			int nEnd   = strBody.ReverseFind(_T('<'));
			if (nBegin == -1 || nEnd == -1 || nEnd < nBegin)
				throw _T("Empty");

			CDC	dc(::GetDC(NULL));
			WTL::CLogFont	lf;
			lf.SetMenuFont();
			WTL::CFont font;
			font.CreateFontIndirect(&lf);
			HFONT hOldFont = dc.SelectFont(font);

			std::vector<CString>	vecStr;
			CString str = m_strTranslated = strBody.Mid(nBegin + 1, nEnd - nBegin - 1);
			int nLength = str.GetLength();
			for (int i = 0; i < nLength; ++i) {
				CRect rc;
				const CString& strColc = str.Left(i);
				dc.DrawText(strColc, i, &rc, DT_CALCRECT);
				if (rc.right > 300) {
					vecStr.push_back(strColc);
					str = str.Mid(i);
					nLength -= i;
					i = 0;
				}
			}
			vecStr.push_back(str);
			dc.SelectFont(hOldFont);

			int nCount = (int)vecStr.size();
			for (int i = 0; i < nCount; ++i) {
				if (i == 0)
					m_menu.ModifyMenu(0, MF_BYPOSITION, ID_BINGTRANSLATE, vecStr[0]);
				else
					m_menu.AppendMenu(0, ID_BINGTRANSLATE, vecStr[i]);
			}
			
			m_menu.AppendMenu(MF_SEPARATOR, (UINT_PTR)0);

			CString strWord = strSelected;
			strWord.TrimLeft();
			strWord.TrimRight();
			int nSpace = strWord.Find(_T(' '));
			if (nSpace == -1) {
				m_menu.AppendMenu(0, ID_A_DICTIONARY, _T("辞書を引く"));
				m_menu.AppendMenu(MF_SEPARATOR, (UINT_PTR)0);
				m_pDicWindow->SetWord(strWord);
			}


			m_menu.AppendMenu(0, ID_COPYTRANSLATERESULT, _T("翻訳結果をコピー"));
			}
			catch (LPCTSTR strError) {
				strError;
				TRACEIN(strError);
			}

			if (hFile)		::InternetCloseHandle(hFile);
			if (hInternet)	::InternetCloseHandle(hInternet);	
		};
		boost::thread td(funcTranslate);
		//funcTranslate();
		return;
	} else {
		int nCount = menuPopup.GetMenuItemCount();
		for (int i = 0; i < nCount; ++i) {
			UINT nID = menuPopup.GetMenuItemID(i);
			if (nID == ID_BINGTRANSLATOR_MENU) {	// サブメニューとして追加する
				m_RootMenu = menuPopup;
				CMenuItemInfo mii;
				mii.fMask  = MIIM_SUBMENU;
				mii.hSubMenu = m_menu;
				m_RootMenu.SetMenuItemInfo(i, TRUE, &mii);
				break;
			}
		}
	}

	SetMsgHandled(FALSE);
}

void CBingTranslatorMenu::OnMenuSelect(UINT nItemID, UINT nFlags, CMenuHandle menu)
{
	auto funcIsCursorOnDicWindow = [this]() -> bool {
		CPoint pt;
		::GetCursorPos(&pt);
		CRect rcWindow;
		m_pDicWindow->GetWindowRect(&rcWindow);
		return rcWindow.PtInRect(pt) != 0;
	};

	if (menu.m_hMenu == m_menu.m_hMenu) {
		if (nItemID == ID_A_DICTIONARY) {		
			CRect rc;
			menu.GetMenuItemRect(NULL, 1, &rc);

			if (m_pDicWindow->IsWindow() == FALSE)
				m_pDicWindow->Create(NULL);
			m_pDicWindow->MoveWindow(m_pDicWindow->ColcWindowPos(rc));
			m_pDicWindow->ShowWindow(SW_SHOWNOACTIVATE);
		} else {
			if (m_pDicWindow->IsWindow())
				m_pDicWindow->ShowWindow(SW_HIDE);
		}
		return ;
	} else {
		if (m_pDicWindow->IsWindow()) {
			if (funcIsCursorOnDicWindow() == false)
				m_pDicWindow->ShowWindow(SW_HIDE);
		}
	}
	if (menu.IsNull()) {
		if (m_pDicWindow->IsWindow()) {
			if (funcIsCursorOnDicWindow() == false)
				m_pDicWindow->DestroyWindow();
		}
	}
	SetMsgHandled(FALSE);
}

// BingTranslatorで翻訳する
void CBingTranslatorMenu::OnBingTranslate(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strUrl;
	strUrl.Format(_T("http://www.microsofttranslator.com/?ref=JAIME&from=en&to=ja&text=%s"), m_strSelectedText);

	COPYDATASTRUCT cds;
	cds.dwData	= kNewDonutLink;
	cds.lpData	= static_cast<LPVOID>(strUrl.GetBuffer(0));
	cds.cbData	= ((strUrl.GetLength() + 1) * sizeof(WCHAR));
	CWindow(m_pChildFrame->GetHwnd()).GetTopLevelWindow().SendMessage(WM_COPYDATA, NULL, (LPARAM)&cds);
}

// 翻訳結果をコピー
void CBingTranslatorMenu::OnCopyTranslateResult(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_strTranslated.IsEmpty() == FALSE)
		MtlSetClipboardText(m_strTranslated, NULL);
}

// 辞書ウィンドウを表示したままにする
void CBingTranslatorMenu::OnShowDicWindow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_pDicWindow->Create(NULL);
	m_pDicWindow->KeepShow();
}











