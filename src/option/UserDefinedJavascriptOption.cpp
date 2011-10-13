/**
*	@file	UserDefinedJavascriptOption.cpp
*	@brief	ユーザー定義Javascript
*/

#include "stdafx.h"
#include "UserDefinedJavascriptOption.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "../Misc.h"
#include "../MtlMisc.h"
#include "../MtlFile.h"
#include "../MtlWin.h"


/////////////////////////////////////////////////////////
// CUserDefinedJsOption

// 定義
vector<unique_ptr<UserDefinedJsData> >	CUserDefinedJsOption::s_vecpData;

/// マッチしたらjavascriptのパスを返す
boost::optional<CString>	CUserDefinedJsOption::FindURL(const CString& strUrl)
{
	CString strUrl2 = strUrl;
	strUrl2.MakeLower();
	for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
		UserDefinedJsData& data = *(*it);
		if (strUrl2.GetLength() < data.strUrl.GetLength())
			continue;
		if (strUrl2.Left(data.strUrl.GetLength()) == data.strUrl)
			return data.strJsPath;
	}
	return boost::none;
}

void	CUserDefinedJsOption::LoadUserJsConfig()
{
	s_vecpData.clear();
	s_vecpData.reserve(10);
	try {
		using boost::property_tree::wptree;
		using namespace boost::property_tree::xml_parser;

		CString strUserDefinedJsConfig = Misc::GetFullPath_ForExe(_T("UserDefinedJavascriptConfig.xml"));

		FILE* fp = nullptr;
		if (_wfopen_s(&fp, strUserDefinedJsConfig, L"r, ccs=UTF-8") != 0) {
			return ;
		}
		std::wstringstream	strstream;
		enum { kBuffSize = 512 };
		wchar_t	temp[kBuffSize + 1];
		while (!feof(fp)) {
			size_t n = fread(temp, sizeof(wchar_t), kBuffSize, fp);
			temp[n] = L'\0';
			strstream << temp;
		}
		fclose(fp);
		wptree	pt;
		read_xml(strstream, pt);

		CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
		wptree ptChild = pt.get_child(L"UserDefinedJavascript");
		for (auto it = ptChild.begin(); it != ptChild.end(); ++it) {
			wptree& ptItem = it->second;
			unique_ptr<UserDefinedJsData>	pData(new UserDefinedJsData);
			pData->strUrl = ptItem.get<std::wstring>(L"<xmlattr>.url").c_str();
			pData->strJsPath = strJsDir + ptItem.get<std::wstring>(L"<xmlattr>.jspath").c_str();
			s_vecpData.push_back(std::move(pData));
		}

	} catch (...) {
		MessageBox(NULL, _T("UserDefinedJavascriptConfig.xmlの読み込みに失敗"), NULL, MB_ICONERROR);
	}
}

void CUserDefinedJsOption::SaveUserJsConfig()
{
	try {
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
		CString strUserDefinedJsConfig = Misc::GetFullPath_ForExe(_T("UserDefinedJavascriptConfig.xml"));
		std::wstringstream	strstream;
		write_xml(strstream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));

		FILE* fp = nullptr;
		if (_wfopen_s(&fp, strUserDefinedJsConfig, L"w, ccs=UTF-8") != 0) 
			throw "error";
		CString str = strstream.str().c_str();
		fwrite(str, sizeof(wchar_t), str.GetLength(), fp);
		fclose(fp);
	} catch (...) {
		MessageBox(NULL, _T("UserDefinedJavascriptConfig.xmlの書き込みに失敗"), NULL, MB_ICONERROR);
	}
}


/////////////////////////////////////////////////////////
// CUserDefinedJsPropertyPage

CUserDefinedJsPropertyPage::CUserDefinedJsPropertyPage(const CString& strUrl) : m_strUrl(strUrl)
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


BOOL	CUserDefinedJsPropertyPage::OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	//m_ListUrl.DrawInsert(m_ListUrl.LBItemFromPt(ptCursor));
	return TRUE;
}

void	CUserDefinedJsPropertyPage::OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(-1);
}

int		CUserDefinedJsPropertyPage::OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(m_ListUrl.LBItemFromPt(ptCursor));
	return 0;
}

void	CUserDefinedJsPropertyPage::OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(-1);

	int nSrcIndex = m_ListUrl.GetCurSel();              // 移動元のインデックス
	int nDestIndex = m_ListUrl.LBItemFromPt(ptCursor);  // 移動先のインデックス
	if(nSrcIndex == -1 || nDestIndex == -1 || nDestIndex == nSrcIndex)
		return;

	unique_ptr<UserDefinedJsData> pData(std::move(s_vecpData[nSrcIndex].release()));
	s_vecpData.erase(s_vecpData.begin() + nSrcIndex);
	s_vecpData.insert(s_vecpData.begin() + nDestIndex, std::move(pData));

	CString strText;
	m_ListUrl.GetText(nSrcIndex, strText);
	m_ListUrl.DeleteString(nSrcIndex);

	m_ListUrl.InsertString(nDestIndex, strText);
	int nCount = m_ListUrl.GetCount();
	for (int i = 0; i < nCount; ++i) {
		m_ListUrl.SetItemData(i, i);
	}
	m_ListUrl.SetCurSel(nDestIndex);
}

void	CUserDefinedJsPropertyPage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlID == IDC_LIST_URL) {
		CDCHandle dc = lpDrawItemStruct->hDC;

		// Save these value to restore them when done drawing.
		COLORREF crOldTextColor = dc.GetTextColor();
		COLORREF crOldBkColor = dc.GetBkColor();

		// If this item is selected, set the background color 
		// and the text color to appropriate values. Also, erase
		// rect by filling it with the background color.
		if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
			(lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
				::GetSysColor(COLOR_HIGHLIGHT));
		} else
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
#if 0
		// If this item has the focus, draw a red frame around the
		// item's rect.
		if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
			(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			CBrush br;
			br.CreateSolidBrush(RGB(255, 0, 0));
			dc.FrameRect(&lpDrawItemStruct->rcItem, br);
		}
#endif
#if 0
		IconListData* pData = (IconListData*)lpDrawItemStruct->itemData;
		if (pData) {
			CIconHandle icon = m_imgList.GetIcon(pData->nIndex);
			if (icon.m_hIcon)
				icon.DrawIconEx(dc, lpDrawItemStruct->rcItem.left + cxMargin, lpDrawItemStruct->rcItem.top + cyMargin, m_iconSize.cx, m_iconSize.cy);

			lpDrawItemStruct->rcItem.left += m_iconSize.cx + cxMargin + IconTextMargin;
			// Draw the text.
			dc.DrawText(
				pData->strText,
				pData->strText.GetLength(),
				&lpDrawItemStruct->rcItem,
				DT_SINGLELINE | DT_VCENTER);
		}
#endif
		try {
			enum { kLeftMargin = 2, kUrlWidth = 270, kBorder = 1, kBorderMargin = 3, };
			RECT rcItem  = lpDrawItemStruct->rcItem;
			rcItem.left	 = kLeftMargin;
			rcItem.right = kLeftMargin + kUrlWidth;
			const CString& strUrl = s_vecpData.at(lpDrawItemStruct->itemData)->strUrl;
			dc.DrawText(strUrl, strUrl.GetLength(), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			
			static COLORREF BorderColor = ::GetSysColor(COLOR_3DLIGHT);
			CPen hPen = ::CreatePen(PS_SOLID, 1, BorderColor);
			HPEN hOldPen = dc.SelectPen(hPen);
			rcItem.right += kBorderMargin;
			dc.MoveTo(rcItem.right, rcItem.top);
			dc.LineTo(rcItem.right, rcItem.bottom);
			dc.SelectPen(hOldPen);

			rcItem.left = rcItem.right + kBorder + kBorderMargin;
			rcItem.right= lpDrawItemStruct->rcItem.right;
			static CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
			CString strJsName = s_vecpData.at(lpDrawItemStruct->itemData)->strJsPath.Mid(strJsDir.GetLength());
			dc.DrawText(strJsName, strJsName.GetLength(), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
		} catch(std::out_of_range e) {
			e;
		}
		// Reset the background color and the text color back to their
		// original values.
		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);
	}
}

void	CUserDefinedJsPropertyPage::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlID == IDC_LIST_URL) {
		enum { cyMargin = 2, kHeight = 10 };
		SIZE size;
		size.cy = kHeight;	// Notoolbar+とか用
		size.cy += cyMargin*2;
		lpMeasureItemStruct->itemHeight	= size.cy;
	}
}



BOOL CUserDefinedJsPropertyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// コントロールを設定
	DoDataExchange(DDX_LOAD);

	m_ListUrl.MakeDragList();

	m_fileNotification.SetFileNotifyFunc(boost::bind(&CUserDefinedJsPropertyPage::_initComboBoxJs, this));
	m_fileNotification.SetUpFileNotification(Misc::GetExeDirectory() + _T("javascript\\"));

	m_editUrl.SetWindowText(m_strUrl);

	_initComboBoxJs();

	for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
		UserDefinedJsData& data = *(*it);
		int nIndex = m_ListUrl.AddString(data.strUrl);
		m_ListUrl.SetItemData(nIndex, nIndex);
	}

	return 0;
}

void CUserDefinedJsPropertyPage::OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strUrl = MtlGetWindowText(m_editUrl);
	int nIndex = m_cmbJs.GetCurSel();
	if (strUrl.IsEmpty() || nIndex == -1)
		return ;
	strUrl.MakeLower();
	unique_ptr<UserDefinedJsData>	pData(new UserDefinedJsData);
	pData->strUrl = MtlGetWindowText(m_editUrl);
	CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
	CString strJsName;
	m_cmbJs.GetLBText(nIndex, strJsName);
	pData->strJsPath	= strJsDir + strJsName;
	int nNewIndex = m_ListUrl.AddString(pData->strUrl);
	m_ListUrl.SetItemData(nNewIndex, nNewIndex);
	m_ListUrl.SetCurSel(nNewIndex);
	s_vecpData.push_back(std::move(pData));
	
}

void CUserDefinedJsPropertyPage::OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_ListUrl.GetCurSel();
	if (nIndex == -1)
		return ;

	s_vecpData.erase(s_vecpData.begin() + nIndex);
	m_ListUrl.DeleteString(nIndex);
	if (nIndex == m_ListUrl.GetCount() - 1)
		--nIndex;
	int nCount = m_ListUrl.GetCount();
	for (int i = 0; i < nCount; ++i) {
		m_ListUrl.SetItemData(i, i);
	}
	m_ListUrl.SetCurSel(nIndex);
}

void CUserDefinedJsPropertyPage::OnChangeApply(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strUrl = MtlGetWindowText(m_editUrl);
	int nIndex = m_ListUrl.GetCurSel();
	int nCmbIndex = m_cmbJs.GetCurSel();
	if (strUrl.IsEmpty() || nIndex == -1 || nCmbIndex == -1)
		return ;
	strUrl.MakeLower();

	s_vecpData[nIndex]->strUrl	= strUrl;
	CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
	CString strJsName;
	m_cmbJs.GetLBText(nCmbIndex, strJsName);
	s_vecpData[nIndex]->strJsPath	= strJsDir + strJsName;

	m_ListUrl.DeleteString(nIndex);
	m_ListUrl.InsertString(nIndex, strUrl);
	m_ListUrl.SetItemData(nIndex, nIndex);
	m_ListUrl.SetCurSel(nIndex);
}

/// コンボボックスで選択されているjavascriptファイルを開く
void CUserDefinedJsPropertyPage::OnJsEdit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_cmbJs.GetCurSel();
	if (nIndex == -1)
		return ;
	CString strJsName;
	m_cmbJs.GetLBText(nIndex, strJsName);
	CString strJsFile;
	strJsFile.Format(_T("%s%s%s"), Misc::GetExeDirectory(), _T("javascript\\"), strJsName);
	::ShellExecute(NULL, NULL, strJsFile, NULL, NULL, SW_NORMAL);
}

/// unDonutのあるフォルダ/javascript フォルダを開く
void CUserDefinedJsPropertyPage::OnOpenJavascriptFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::ShellExecute(NULL, NULL, Misc::GetExeDirectory() + _T("javascript\\"), NULL, NULL, SW_NORMAL);
}

void CUserDefinedJsPropertyPage::OnListSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_ListUrl.GetCurSel();
	if (nIndex == -1)
		return ;
	CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
	UserDefinedJsData& data = *s_vecpData[nIndex];
	m_editUrl.SetWindowText(data.strUrl);
	CString strJsName = data.strJsPath.Mid(strJsDir.GetLength());
	m_cmbJs.SelectString(0, strJsName);
}

/* コンボボックスにjavascriptフォルダの*.jsを登録 */
void	CUserDefinedJsPropertyPage::_initComboBoxJs()
{
	while (m_cmbJs.GetCount())
		m_cmbJs.DeleteString(0);
	
	CString strJsDir = Misc::GetExeDirectory() + _T("javascript\\");
	MtlForEachFileSort(strJsDir, [strJsDir, this](const CString& strFilePath) {
		if (MtlIsExt(strFilePath, _T(".js"))) {
			CString strJsName = strFilePath.Mid(strJsDir.GetLength());
			m_cmbJs.AddString(strJsName);
		}
	});
}


























