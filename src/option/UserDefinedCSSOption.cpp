/**
*	@file	UserDefinedCSSOption.cpp
*	@brief	ユーザー定義CSS
*/

#include "stdafx.h"
#include "UserDefinedCSSOption.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "../Misc.h"
#include "../MtlMisc.h"
#include "../MtlFile.h"
#include "../MtlWin.h"


/////////////////////////////////////////////////////////
// CUserDefinedCSSOption

// 定義
vector<unique_ptr<UserDefinedCssData> >	CUserDefinedCSSOption::s_vecpData;

/// マッチしたらcssのパスを返す
boost::optional<CString>	CUserDefinedCSSOption::FindURL(const CString& strUrl)
{
	CString strUrl2 = strUrl;
	strUrl2.MakeLower();
	for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
		UserDefinedCssData& data = *(*it);
		if (strUrl2.GetLength() < data.strUrl.GetLength())
			continue;
		if (strUrl2.Left(data.strUrl.GetLength()) == data.strUrl)
			return data.strCSSPath;
	}
	return boost::none;
}

void	CUserDefinedCSSOption::LoadUserCSSConfig()
{
	s_vecpData.clear();
	s_vecpData.reserve(10);
	try {
		using boost::property_tree::wptree;
		using namespace boost::property_tree::xml_parser;

		CString strUserDefinedCSSConfig = Misc::GetFullPath_ForExe(_T("UserDefinedCSSConfig.xml"));

		FILE* fp = nullptr;
		if (_wfopen_s(&fp, strUserDefinedCSSConfig, L"r, ccs=UTF-8") != 0) {
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

		CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
		if (auto optChild = pt.get_child_optional(L"UserDefinedCSS")) {
			wptree ptChild = optChild.get();
			for (auto it = ptChild.begin(); it != ptChild.end(); ++it) {
				wptree& ptItem = it->second;
				unique_ptr<UserDefinedCssData>	pData(new UserDefinedCssData);
				pData->strUrl = ptItem.get<std::wstring>(L"<xmlattr>.url").c_str();
				pData->strCSSPath = strCssDir + ptItem.get<std::wstring>(L"<xmlattr>.csspath").c_str();
				s_vecpData.push_back(std::move(pData));
			}
		}
	} catch (...) {
		MessageBox(NULL, _T("UserDefinedCSSConfig.xmlの読み込みに失敗"), NULL, MB_ICONERROR);
	}
}

void CUserDefinedCSSOption::SaveUserCSSConfig()
{
	try {
		using boost::property_tree::wptree;
		using namespace boost::property_tree::xml_parser;

		CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
		wptree pt;
		for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
			UserDefinedCssData& data = *(*it);
			wptree& ptItem = pt.add(L"UserDefinedCSS.item", L"");
			ptItem.put(L"<xmlattr>.url", (LPCTSTR)data.strUrl);
			ptItem.put(L"<xmlattr>.csspath", (LPCTSTR)data.strCSSPath.Mid(strCssDir.GetLength()));
		}
		CString strUserDefinedCSSConfig = Misc::GetFullPath_ForExe(_T("UserDefinedCSSConfig.xml"));
		std::wstringstream	strstream;
		write_xml(strstream, pt, xml_writer_make_settings(L' ', 2, widen<wchar_t>("UTF-8")));

		FILE* fp = nullptr;
		if (_wfopen_s(&fp, strUserDefinedCSSConfig, L"w, ccs=UTF-8") != 0) 
			throw "error";
		CString str = strstream.str().c_str();
		fwrite(str, sizeof(wchar_t), str.GetLength(), fp);
		fclose(fp);
	} catch (...) {
		MessageBox(NULL, _T("UserDefinedCSSConfig.xmlの書き込みに失敗"), NULL, MB_ICONERROR);
	}
}


/////////////////////////////////////////////////////////
// CUserDefinedCSSPropertyPage

CUserDefinedCSSPropertyPage::CUserDefinedCSSPropertyPage(const CString& strUrl) : m_strUrl(strUrl)
{	}

// Overrides
BOOL	CUserDefinedCSSPropertyPage::OnSetActive()
{
	return TRUE;
}

BOOL	CUserDefinedCSSPropertyPage::OnKillActive()
{
	return TRUE;
}

BOOL	CUserDefinedCSSPropertyPage::OnApply()
{
	SaveUserCSSConfig();
	return TRUE;
}


BOOL	CUserDefinedCSSPropertyPage::OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	//m_ListUrl.DrawInsert(m_ListUrl.LBItemFromPt(ptCursor));
	return TRUE;
}

void	CUserDefinedCSSPropertyPage::OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(-1);
}

int		CUserDefinedCSSPropertyPage::OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(m_ListUrl.LBItemFromPt(ptCursor));
	return 0;
}

void	CUserDefinedCSSPropertyPage::OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
{
	m_ListUrl.DrawInsert(-1);

	int nSrcIndex = m_ListUrl.GetCurSel();              // 移動元のインデックス
	int nDestIndex = m_ListUrl.LBItemFromPt(ptCursor);  // 移動先のインデックス
	if(nSrcIndex == -1 || nDestIndex == -1 || nDestIndex == nSrcIndex)
		return;

	unique_ptr<UserDefinedCssData> pData(std::move(s_vecpData[nSrcIndex].release()));
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

void	CUserDefinedCSSPropertyPage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
			static CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
			CString strCssName = s_vecpData.at(lpDrawItemStruct->itemData)->strCSSPath.Mid(strCssDir.GetLength());
			dc.DrawText(strCssName, strCssName.GetLength(), &rcItem, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
		} catch(std::out_of_range e) {
			e;
		}
		// Reset the background color and the text color back to their
		// original values.
		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);
	}
}

void	CUserDefinedCSSPropertyPage::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlID == IDC_LIST_URL) {
		enum { cyMargin = 2, kHeight = 10 };
		SIZE size;
		size.cy = kHeight;	// Notoolbar+とか用
		size.cy += cyMargin*2;
		lpMeasureItemStruct->itemHeight	= size.cy;
	}
}



BOOL CUserDefinedCSSPropertyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// コントロールを設定
	DoDataExchange(DDX_LOAD);

	m_ListUrl.MakeDragList();

	m_fileNotification.SetFileNotifyFunc(boost::bind(&CUserDefinedCSSPropertyPage::_initComboBoxCSS, this));
	m_fileNotification.SetUpFileNotification(Misc::GetExeDirectory() + _T("css\\"));

	m_editUrl.SetWindowText(m_strUrl);

	_initComboBoxCSS();

	for (auto it = s_vecpData.begin(); it != s_vecpData.end(); ++it) {
		UserDefinedCssData& data = *(*it);
		int nIndex = m_ListUrl.AddString(data.strUrl);
		m_ListUrl.SetItemData(nIndex, nIndex);
	}

	return 0;
}

void CUserDefinedCSSPropertyPage::OnAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strUrl = MtlGetWindowText(m_editUrl);
	int nIndex = m_cmbCSS.GetCurSel();
	if (strUrl.IsEmpty() || nIndex == -1)
		return ;
	strUrl.MakeLower();
	unique_ptr<UserDefinedCssData>	pData(new UserDefinedCssData);
	pData->strUrl = MtlGetWindowText(m_editUrl);
	CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
	CString strCssName;
	m_cmbCSS.GetLBText(nIndex, strCssName);
	pData->strCSSPath	= strCssDir + strCssName;
	int nNewIndex = m_ListUrl.AddString(pData->strUrl);
	m_ListUrl.SetItemData(nNewIndex, nNewIndex);
	m_ListUrl.SetCurSel(nNewIndex);
	s_vecpData.push_back(std::move(pData));
	
}

void CUserDefinedCSSPropertyPage::OnDelete(UINT uNotifyCode, int nID, CWindow wndCtl)
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

void CUserDefinedCSSPropertyPage::OnChangeApply(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strUrl = MtlGetWindowText(m_editUrl);
	int nIndex = m_ListUrl.GetCurSel();
	int nCmbIndex = m_cmbCSS.GetCurSel();
	if (strUrl.IsEmpty() || nIndex == -1 || nCmbIndex == -1)
		return ;
	strUrl.MakeLower();

	s_vecpData[nIndex]->strUrl	= strUrl;
	CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
	CString strCssName;
	m_cmbCSS.GetLBText(nCmbIndex, strCssName);
	s_vecpData[nIndex]->strCSSPath	= strCssDir + strCssName;

	m_ListUrl.DeleteString(nIndex);
	m_ListUrl.InsertString(nIndex, strUrl);
	m_ListUrl.SetItemData(nIndex, nIndex);
	m_ListUrl.SetCurSel(nIndex);
}

/// コンボボックスで選択されているcssファイルを開く
void CUserDefinedCSSPropertyPage::OnCSSEdit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_cmbCSS.GetCurSel();
	if (nIndex == -1)
		return ;
	CString strCssName;
	m_cmbCSS.GetLBText(nIndex, strCssName);
	CString strCssFile;
	strCssFile.Format(_T("%s%s%s"), Misc::GetExeDirectory(), _T("css\\"), strCssName);
	::ShellExecute(NULL, NULL, strCssFile, NULL, NULL, SW_NORMAL);
}

/// unDonutのあるフォルダ/css フォルダを開く
void CUserDefinedCSSPropertyPage::OnOpenCSSFolder(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::ShellExecute(NULL, NULL, Misc::GetExeDirectory() + _T("css\\"), NULL, NULL, SW_NORMAL);
}

void CUserDefinedCSSPropertyPage::OnListSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_ListUrl.GetCurSel();
	if (nIndex == -1)
		return ;
	CString strCssDir = Misc::GetExeDirectory() + _T("css\\");
	UserDefinedCssData& data = *s_vecpData[nIndex];
	m_editUrl.SetWindowText(data.strUrl);
	CString strCssName = data.strCSSPath.Mid(strCssDir.GetLength());
	m_cmbCSS.SelectString(0, strCssName);
}

/* コンボボックスにcssフォルダの*.cssを登録 */
void	CUserDefinedCSSPropertyPage::_initComboBoxCSS()
{
	while (m_cmbCSS.GetCount())
		m_cmbCSS.DeleteString(0);
	
	CString strCSsDir = Misc::GetExeDirectory() + _T("css\\");
	MtlForEachFileSort(strCSsDir, [strCSsDir, this](const CString& strFilePath) {
		if (MtlIsExt(strFilePath, _T(".css"))) {
			CString strCSSName = strFilePath.Mid(strCSsDir.GetLength());
			m_cmbCSS.AddString(strCSSName);
		}
	});
}








