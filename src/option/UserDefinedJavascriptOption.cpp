/**
*	@file	UserDefinedJavascriptOption.cpp
*	@brief	ユーザー定義Javascript
*/

#include "stdafx.h"
#include "UserDefinedJavascriptOption.h"
#include <regex>
#include <codecvt>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/scope_exit.hpp>
#include "../Misc.h"
#include "../MtlMisc.h"
#include "../MtlFile.h"
#include "../MtlWin.h"
#include "../Donut.h"



///////////////////////////////////////////////////////////
/// スクリプトの詳細

class CScriptDitailDialog : public CDialogImpl<CScriptDitailDialog>
{
public:
	enum { IDD = IDD_SCRIPTDETAIL };

	CScriptDitailDialog(UserDefinedJsData* pData, bool bScriptInstallDialog = false) : 
		m_pData(pData), m_bScriptInstallDialog(bScriptInstallDialog), m_bReInstall(false)
	{	}

	// attributes
	bool	IsReInstall() const { return m_bReInstall; }
	CString GetNewJsFilePath() const { return m_strNewJsFilePath; }

	BEGIN_MSG_MAP( CScriptDitailDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOK )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnOK )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_INSTALL, OnInstall )
		NOTIFY_HANDLER_EX(IDC_SYSLINK_HP, NM_CLICK, OnLinkClick)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		CStatic		name	= GetDlgItem(IDC_STATIC_NAME);
		WTL::CLogFont	lfname;
		lfname.SetMenuFont();
		lfname.SetBold();
		//lfname.SetHeight(11);
		name.SetFont(lfname.CreateFontIndirect());
		CStatic		discription = GetDlgItem(IDC_STATIC_DISCRIPTION);
		CStatic		version	= GetDlgItem(IDC_STATIC_VERSION);
		CLinkCtrl	linkHP = GetDlgItem(IDC_SYSLINK_HP);
		CStatic		filename = GetDlgItem(IDC_STATIC_FILENAME);
		CEdit		include	= GetDlgItem(IDC_EDIT_INCLUDE);
		CEdit		exclude = GetDlgItem(IDC_EDIT_EXCLUDE);

		name.SetWindowText(m_pData->strName);
		discription.SetWindowText(m_pData->strDiscription);
		version.SetWindowText(m_pData->strVersion);
		CString strHP;
		strHP.Format(_T("<a>%s</a>"), m_pData->strHP);
		linkHP.SetWindowText(strHP);		
		filename.SetWindowText(Misc::GetFileBaseName(m_pData->strScriptFilePath));

		CString strinclude;
		for (auto it = m_pData->vecIncludeUrls.cbegin(); it != m_pData->vecIncludeUrls.cend(); ++it) {
			strinclude += *it;
			if (it != m_pData->vecIncludeUrls.cend())
				strinclude += _T("\r\n");
		}
		include.SetWindowText(strinclude);
		include.SetSel(0, 0, FALSE);

		CString strexclude;
		for (auto it = m_pData->vecExcludeUrls.cbegin(); it != m_pData->vecExcludeUrls.cend(); ++it) {
			strexclude += *it;
			if (it != m_pData->vecExcludeUrls.cend())
				strinclude += _T("\r\n");
		}
		exclude.SetWindowText(strexclude);
		exclude.SetSel(0, 0, FALSE);

		if (m_bScriptInstallDialog) {
			GetDlgItem(IDOK).SetWindowText(_T("キャンセル"));
			GetDlgItem(IDC_BUTTON_INSTALL).ShowWindow(TRUE);

			CString strJsFileName = Misc::GetFileBaseName(m_pData->strScriptFilePath);
			int nIndex = strJsFileName.ReverseFind(_T('['));
			if (nIndex != -1) {
				strJsFileName = strJsFileName.Left(nIndex) + _T(".js");
			}
			CString strJsFilePath;
			strJsFilePath.Format(_T("%sjavascript\\%s"), Misc::GetExeDirectory(), strJsFileName);
			if (::PathFileExists(strJsFilePath)) {
				m_bReInstall = true;
				GetDlgItem(IDC_BUTTON_INSTALL).SetWindowText(_T("再インストール"));
			}
			m_strNewJsFilePath = strJsFilePath;
		}
		CenterWindow();
		return 0;
	}

    void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
        EndDialog(nID);
    }

	void OnInstall(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(IDC_BUTTON_INSTALL);
	}


	LRESULT OnLinkClick(LPNMHDR pnmh)
	{
		DonutOpenFile(m_pData->strHP);
		return 0;
	}
private:
	UserDefinedJsData*	m_pData;
	bool	m_bScriptInstallDialog;
	bool	m_bReInstall;
	CString m_strNewJsFilePath;
};


/////////////////////////////////////////////////////////
// CUserDefinedJsOption

// 定義
vector<unique_ptr<UserDefinedJsData> >	CUserDefinedJsOption::s_vecpData;
CCriticalSection	CUserDefinedJsOption::s_criticalsection;

/// マッチしたらjavascriptのパスを返す
boost::optional<vector<BSTR*> >	CUserDefinedJsOption::FindURL(const CString& strUrl)
{
	s_criticalsection.Enter();
	BOOST_SCOPE_EXIT((&s_criticalsection)) {
		s_criticalsection.Leave();
	} BOOST_SCOPE_EXIT_END

	vector<BSTR*>	vecScripts;
	for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
		auto& IncludeUrlList = it->get()->vecIncludeUrls;
		for (auto itUrl = IncludeUrlList.begin(); itUrl != IncludeUrlList.end(); ++itUrl) {
			if (MtlStrMatchWildCard(*itUrl, strUrl)) {
				auto& ExcludeUrlList = it->get()->vecExcludeUrls;
				for (auto itExUrl = ExcludeUrlList.begin(); itExUrl != ExcludeUrlList.end(); ++itExUrl) {
					if (MtlStrMatchWildCard(*itExUrl, strUrl)) {
						goto EXCLUDEURLMATCH;
					}
				}
				vecScripts.push_back(&it->get()->strScript);
				EXCLUDEURLMATCH:;
			}
		}
	}
	if (vecScripts.empty())
		return boost::none;
	
	return vecScripts;
}

bool	CUserDefinedJsOption::UserDefinedScriptInstall(const CString& strUrl, HWND hWndParent)
{
	CString strJsFilePath;
	if (::URLDownloadToCacheFile(NULL, strUrl, strJsFilePath.GetBuffer(MAX_PATH), MAX_PATH, 0, NULL) == S_OK) {
		if (auto ptr = _CreateJsData(strJsFilePath)) {
			CScriptDitailDialog dialog(ptr.get(), true);
			if (dialog.DoModal(hWndParent) == IDC_BUTTON_INSTALL) {
				CString strNewJsFilePath = dialog.GetNewJsFilePath();
				if (::CopyFile(strJsFilePath, strNewJsFilePath, FALSE)) {
					s_criticalsection.Enter();
					BOOST_SCOPE_EXIT((&s_criticalsection)) {
						s_criticalsection.Leave();
					} BOOST_SCOPE_EXIT_END
					if (dialog.IsReInstall()) {
						for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
							if (it->get()->strScriptFilePath.CompareNoCase(strNewJsFilePath) == 0) {
								s_vecpData.erase(it);
								break;
							}
						}
					}
					if (auto ptr2 = _CreateJsData(strNewJsFilePath)) {
						s_vecpData.push_back(std::move(ptr2));
					}
				}
				return true;
			}
		}
	}
	return false;
}

void	CUserDefinedJsOption::LoadUserJsConfig()
{
	s_criticalsection.Enter();
	BOOST_SCOPE_EXIT((&s_criticalsection)) {
		s_criticalsection.Leave();
	} BOOST_SCOPE_EXIT_END

	s_vecpData.clear();
	s_vecpData.reserve(10);

	CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
	MtlForEachFileSort(strJsDir, [strJsDir](const CString& strFilePath) {
		if (MtlIsExt(strFilePath, _T(".js"))) {
			if (auto ptr = _CreateJsData(strFilePath))
				CUserDefinedJsOption::s_vecpData.push_back(std::move(ptr));
		}
	});

}

void CUserDefinedJsOption::SaveUserJsConfig()
{
	try {
#if 0
		using boost::property_tree::wptree;
		using namespace boost::property_tree::xml_parser;

		CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
		wptree pt;
		for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
			UserDefinedJsData& data = *(*it);
			wptree& ptItem = pt.add(L"UserDefinedJavascript.item", L"");
			ptItem.put(L"<xmlattr>.url", (LPCTSTR)data.strUrl);
			ptItem.put(L"<xmlattr>.jspath", (LPCTSTR)data.strJsPath.Mid(strJsDir.GetLength()));
		}
		CString strUserDefinedJsConfig = GetConfigFilePath(_T("UserDefinedJavascriptConfig.xml"));
		std::wstringstream	strstream;
		write_xml(strstream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));

		FILE* fp = nullptr;
		if (_wfopen_s(&fp, strUserDefinedJsConfig, L"w, ccs=UTF-8") != 0) 
			throw "error";
		CString str = strstream.str().c_str();
		fwrite(str, sizeof(wchar_t), str.GetLength(), fp);
		fclose(fp);
#endif
	} catch (...) {
		MessageBox(NULL, _T("UserDefinedJavascriptConfig.xmlの書き込みに失敗"), NULL, MB_ICONERROR);
	}
}

unique_ptr<UserDefinedJsData> CUserDefinedJsOption::_CreateJsData(const CString& strFilePath)
{
	std::wifstream	filestream(strFilePath);
	if (!filestream)
		return nullptr;
	filestream.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

	std::wstring buff;
	// ユーザースクリプトかどうか
	if (!std::getline(filestream, buff) != 0)
		return nullptr;
	if (buff.find(L"// ==UserScript==", 0) == -1)
		return nullptr;

	unique_ptr<UserDefinedJsData>	pData(new UserDefinedJsData);
	pData->strScriptFilePath	= strFilePath;

	std::wregex	rx(L"// @(\\w+)\\s+(.+)");
	std::wsmatch result;
	while (std::getline(filestream, buff) != 0) {
		if (buff.find(L"// ==/UserScript==", 0) != -1)
			break;
		if (std::regex_search(buff.cbegin(), buff.cend(), result, rx)) {
			CString strat = result.str(1).c_str();
			CString strsecond = result.str(2).c_str();
			if (strat == _T("name")) {
				pData->strName	= strsecond;

			} else if (strat == _T("description")) {
				pData->strDiscription = strsecond;

			} else if (strat == _T("version")) {
				pData->strVersion	= strsecond;

			} else if (strat == _T("namespace")) {
				pData->strHP = strsecond;

			} else if (strat == _T("include")) {
				pData->vecIncludeUrls.push_back(strsecond);

			} else if (strat == _T("exclude")) {
				pData->vecExcludeUrls.push_back(strsecond);

			}
		}
	}

	auto curPos = filestream.tellg();
	filestream.seekg(0, std::fstream::end);
	auto LastPos = filestream.tellg();
	filestream.clear();
	filestream.seekg(curPos);
	auto scriptSize = LastPos - curPos;
	CTempBuffer<wchar_t>	strtemp(static_cast<int>(scriptSize) + 1);
	filestream.get(strtemp, scriptSize, L'\0');
	pData->strScript = strtemp;

	return std::move(pData);
}


///////////////////////////////////////////////////////////
// CUserJsListView

void	CUserJsListView::DoPaint(CDCHandle dc)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcItem;
	rcItem.right = rcClient.right;
	rcItem.bottom= kItemHeight;

	HFONT hPrevFont = dc.SelectFont(m_font);
	dc.SetBkMode(TRANSPARENT);

	int nCount = (int)s_vecpData.size();
	for (int i = 0; i < nCount; ++i) {
		UserDefinedJsData* pData = s_vecpData[i].get();
		
		CRect rcName = rcItem;
		rcName.top += kTopNameMargin;
		rcName.bottom = rcName.top + kNameHeight;
		rcName.left	= kLeftMargin;
		CString strName;
		strName.Format(_T("%s　%s"), pData->strName, pData->strVersion);
		HFONT hNormalFont = dc.SelectFont(m_namefont);
		dc.DrawText(strName, strName.GetLength(), &rcName, DT_SINGLELINE);
		dc.SelectFont(hNormalFont);

		CRect rcDisc = rcItem;
		rcDisc.top	+= kTopDiscMargin;
		rcDisc.bottom	= rcDisc.top + kDiscHeight;
		rcDisc.left	= kLeftMargin;
		rcDisc.right= rcDisc.left + kDiscWidth;
		dc.DrawText(pData->strDiscription, pData->strDiscription.GetLength(), &rcDisc, DT_SINGLELINE | DT_END_ELLIPSIS);

		CRect rcDelete = rcItem;
		rcDelete.top += kTopButton;
		rcDelete.bottom = rcDelete.top + kButtonHeight;
		rcDelete.right -= kRightButtonMargin;
		rcDelete.left	= rcDelete.right - kButtonWidth;
		dc.DrawFrameControl(&rcDelete, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_FLAT);
		dc.DrawText(_T("削除"), -1, &rcDelete, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		CRect rcEdit = rcDelete;
		rcEdit.MoveToX(rcEdit.left - (kButtonMargin + kButtonWidth));
		dc.DrawFrameControl(&rcEdit, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_FLAT);
		dc.DrawText(_T("編集"), -1, &rcEdit, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		CRect rcDetail = rcEdit;
		rcDetail.MoveToX(rcDetail.left - (kButtonMargin + kButtonWidth));
		dc.DrawFrameControl(&rcDetail, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_FLAT);
		dc.DrawText(_T("詳細"), -1, &rcDetail, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		// 下にラインを引く
		static COLORREF BorderColor = ::GetSysColor(COLOR_3DLIGHT);
		CPen hPen = ::CreatePen(PS_SOLID, 1, BorderColor);
		HPEN hOldPen = dc.SelectPen(hPen);
		dc.MoveTo(CPoint(rcItem.left, rcItem.bottom));
		dc.LineTo(rcItem.right, rcItem.bottom);
		dc.SelectPen(hOldPen);

		rcItem.MoveToY(rcItem.top + kItemHeight);
	}

	dc.SelectFont(hPrevFont);
	//dc.DrawText(_T("test"), -1, CRect(10, 10, 200, 200), 0);
}


int CUserJsListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nScrollHegiht;
	if (s_vecpData.size() == 0) {
		nScrollHegiht = 1;
	} else {
		nScrollHegiht = static_cast<int>(s_vecpData.size()) * kItemHeight;
	}
	CRect rcClient;
	GetClientRect(&rcClient);
	CSize	scrollsize(rcClient.right, nScrollHegiht);
	SetScrollSize(scrollsize);

	WTL::CLogFont	lf;
	lf.SetMenuFont();
	m_font = lf.CreateFontIndirect();
	lf.SetBold();
	lf.SetHeight(12);
	m_namefont = lf.CreateFontIndirect();
	return 0;
}

void CUserJsListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	int nIndex = _HitTest(point);
	if (nIndex == -1)
		return ;

	CRect rcItem = _GetItemClientRect(nIndex);

	// 削除ボタン
	CRect rcDelete = rcItem;
	rcDelete.top += kTopButton;
	rcDelete.bottom = rcDelete.top + kButtonHeight;
	rcDelete.right -= kRightButtonMargin;
	rcDelete.left	= rcDelete.right - kButtonWidth;
	if (rcDelete.PtInRect(point)) {
		const CString& strPath = s_vecpData[nIndex]->strScriptFilePath;
		CTempBuffer<TCHAR>	buff;
		LPTSTR	strtemp = buff.Allocate(strPath.GetLength() + 2);
		_tcscpy_s(strtemp, strPath.GetLength() + 2, strPath);
		strtemp[strPath.GetLength() + 1] = 0;
		SHFILEOPSTRUCT	fop = {};
		fop.pFrom	= strtemp;
		fop.wFunc	= FO_DELETE;
		fop.fFlags	= FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		int bResult = SHFileOperation(&fop);
		s_vecpData.erase(s_vecpData.begin() + nIndex);
		Invalidate();
		return ;
	}

	// 編集ボタン
	CRect rcEdit = rcDelete;
	rcEdit.MoveToX(rcEdit.left - (kButtonMargin + kButtonWidth));
	if (rcEdit.PtInRect(point)) {
		::ShellExecute(NULL, _T("edit"), s_vecpData[nIndex]->strScriptFilePath, NULL, NULL, SW_NORMAL);
		return ;
	}

	// 詳細ボタン
	CRect rcDetail = rcEdit;
	rcDetail.MoveToX(rcDetail.left - (kButtonMargin + kButtonWidth));
	if (rcDetail.PtInRect(point)) {
		CScriptDitailDialog	dialog(s_vecpData[nIndex].get());
		dialog.DoModal(GetParent());
		return ;
	}
}


/// ptにあるDLアイテムのインデックスを返す
int		CUserJsListView::_HitTest(CPoint pt)
{
	int nCount = (int)s_vecpData.size();
	for (int i = 0; i < nCount; ++i) {
		if (_GetItemClientRect(i).PtInRect(pt))
			return i;
	}
	return -1;
}

/// nIndexのアイテムのクライアント座標での範囲を返す
CRect	CUserJsListView::_GetItemClientRect(int nIndex)
{
	ATLASSERT(0 <= nIndex && nIndex < (int)s_vecpData.size());

	CPoint	ptOffset;
	GetScrollOffset(ptOffset);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcItem;
	rcItem.top		= kItemHeight * nIndex;
	rcItem.right	= rcClient.right;
	rcItem.bottom	= rcItem.top + kItemHeight;

	rcItem.top		-= ptOffset.y;
	rcItem.bottom	-= ptOffset.y;
	return rcItem;
}


/////////////////////////////////////////////////////////
// CUserDefinedJsPropertyPage

CUserDefinedJsPropertyPage::CUserDefinedJsPropertyPage(const CString& strUrl)
{	}

// Overrides
BOOL	CUserDefinedJsPropertyPage::OnSetActive()
{
	return TRUE;
}

BOOL	CUserDefinedJsPropertyPage::OnKillActive()
{
	return TRUE;
}

BOOL	CUserDefinedJsPropertyPage::OnApply()
{
	SaveUserJsConfig();
	return TRUE;
}



BOOL CUserDefinedJsPropertyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// コントロールを設定
	DoDataExchange(DDX_LOAD);

	CRect rcJsList;
	CWindow wndDummy = GetDlgItem(IDC_STATIC_SCRIPTPOSITION);
	wndDummy.GetClientRect(&rcJsList);
	m_JsListView.Create(wndDummy, rcJsList, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER);
	//m_fileNotification.SetFileNotifyFunc(boost::bind(&CUserDefinedJsPropertyPage::_initComboBoxJs, this));
	//m_fileNotification.SetUpFileNotification(Misc::GetExeDirectory() + _T("javascript\\"));


	return 0;
}

/// リストを再読み込みする
void CUserDefinedJsPropertyPage::OnReload(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	LoadUserJsConfig();
	m_JsListView.Invalidate();
}


/// unDonutのあるフォルダ/javascript フォルダを開く
void CUserDefinedJsPropertyPage::OnOpenJavascriptFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::ShellExecute(NULL, NULL, Misc::GetExeDirectory() + _T("javascript\\"), NULL, NULL, SW_NORMAL);
}


























