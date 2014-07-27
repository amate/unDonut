// OptionDialog.h

#pragma once

#include <atlddx.h>
#include "../IniFile.h"
#include "../MtlMisc.h"
#include "../ShellFileDialog.h"
#include "../resource.h"

#define REGISTERMESSAGE_GETDEFAULTDLFOLDER	_T("Donut_DLManager_GetDefaultDLFolder")
#define REGISTERMESSAGE_STARTDOWNLOAD		_T("Donut_DLManager_StartDownload")
#define REGISTERMESSAGE_SETREFERER			_T("Donut_DLManager_SetReferer")

struct DLStartItem {
	LPCWSTR strURL;
	LPCWSTR strDLFolder;
	HWND	hWnd;
	DWORD	dwOption;
};
enum EDLOption {
	DLO_OVERWRITEPROMPT = 0x0001,	// 上書きの確認をする
	DLO_USEUNIQUENUMBER = 0x0002,	// 連番を付ける
	DLO_SHOWWINDOW		= 0x0004,
	DLO_SAVEIMAGE		= 0x0008,	// 画像を保存
};

#define REGISTERMESSAGE_DLCOMPLETE			_T("Donut_DLManager_DLComplete")


//////////////////////////////////////////
// CDLOptions

struct CDLOptions
{
	static CString	s_DLIniFilePath;

	static CString	strDLFolderPath;
	static bool		bUseSaveFileDialog;
	static bool		bCloseAfterAllDL;
	static bool		bShowWindowOnDL;

	static CString	strImgDLFolderPath;
	static DWORD	dwImgExStyle;

	enum { kMaxHistory = 30 };
	static vector<CString>	s_vecDLFolderHistory;
	static vector<CString>	s_vecImageDLFolderHistory;

	static CString	s_strDLFinishSoundFilePath;

	static void	LoadProfile()
	{
		s_DLIniFilePath = GetConfigFilePath(_T("Download.ini"));
		CIniFileI	pr(s_DLIniFilePath, _T("Main"));
		strDLFolderPath		= pr.GetString(_T("DLFolder"), Misc::GetExeDirectory());
		bUseSaveFileDialog	= pr.GetValue(_T("UseSaveFileDialog"), bUseSaveFileDialog) != 0;
		bCloseAfterAllDL	= pr.GetValue(_T("CloseAfterAllDL"), bCloseAfterAllDL) != 0;
		bShowWindowOnDL		= pr.GetValue(_T("ShowWindowOnDL"), bShowWindowOnDL) != 0;

		strImgDLFolderPath  = pr.GetString(_T("ImgDLFolder"), strDLFolderPath);
		dwImgExStyle		= pr.GetValue(_T("ImgExStyle"), DLO_OVERWRITEPROMPT);

		s_strDLFinishSoundFilePath = pr.GetString(_T("DLFinishSoundFilePath"));

		s_vecDLFolderHistory.clear();
		pr.ChangeSectionName(_T("DLFolderHistory"));
		for (int i = 0; i < kMaxHistory; ++i) {
			CString strName;
			strName.Append(i);
			CString strFolder = pr.GetString(strName);
			if (strFolder.IsEmpty())
				break;
			s_vecDLFolderHistory.push_back(strFolder);
		}

		s_vecImageDLFolderHistory.clear();
		pr.ChangeSectionName(_T("ImageDLFolderHistory"));
		for (int i = 0; i < kMaxHistory; ++i) {
			CString strName;
			strName.Append(i);
			CString strFolder = pr.GetString(strName);
			if (strFolder.IsEmpty())
				break;
			s_vecImageDLFolderHistory.push_back(strFolder);
		}
	}

	static void	SaveProfile()
	{
		CIniFileO pr(s_DLIniFilePath, _T("Main"));
		MTL::MtlMakeSureTrailingBackSlash(strDLFolderPath);
		pr.SetString(strDLFolderPath, _T("DLFolder"));
		pr.SetValue(bUseSaveFileDialog, _T("UseSaveFileDialog"));
		pr.SetValue(bCloseAfterAllDL, _T("CloseAfterAllDL"));
		pr.SetValue(bShowWindowOnDL, _T("ShowWindowOnDL"));

		MTL::MtlMakeSureTrailingBackSlash(strImgDLFolderPath);
		pr.SetString(strImgDLFolderPath, _T("ImgDLFolder"));
		pr.SetValue(dwImgExStyle, _T("ImgExStyle"));

		pr.SetString(s_strDLFinishSoundFilePath, _T("DLFinishSoundFilePath"));
	}

	enum SaveTargetFolderHistory { kDLFolderHistory, kImageDLFolderHistory };

	static void SavePathToHistory(CString strPath, SaveTargetFolderHistory kTarget)
	{
		vector<CString>* pvecHistory = nullptr;
		LPCTSTR saveSectionName = nullptr;
		switch (kTarget) {
		case kDLFolderHistory:
			pvecHistory = &s_vecDLFolderHistory;
			saveSectionName = _T("DLFolderHistory");
			break;

		case kImageDLFolderHistory:
			pvecHistory = &s_vecImageDLFolderHistory;
			saveSectionName = _T("ImageDLFolderHistory");
			break;

		default:
			ATLASSERT(FALSE);
			return;
		}
		MTL::MtlMakeSureTrailingBackSlash(strPath);
		for (auto it = pvecHistory->begin(); it != pvecHistory->end(); ++it) {
			if (strPath == *it) {	// 重複の削除
				pvecHistory->erase(it);
				break;
			}
		}
		pvecHistory->insert(pvecHistory->begin(), strPath);

		CIniFileO pr(s_DLIniFilePath, saveSectionName);
		pr.DeleteSection();
		int nCount = (int)pvecHistory->size();
		for (int i = 0; i < nCount; ++i) {
			CString strName;
			strName.Append(i);
			pr.SetString(pvecHistory->at(i), strName);
		}
	}

	static void PlaySoundDLFinish()
	{
		if (::PathFileExists(s_strDLFinishSoundFilePath))
			::PlaySound(s_strDLFinishSoundFilePath, NULL, SND_ASYNC | SND_FILENAME);
	}
};


__declspec(selectany) CString	CDLOptions::s_DLIniFilePath;

__declspec(selectany) CString	CDLOptions::strDLFolderPath;
__declspec(selectany) bool		CDLOptions::bUseSaveFileDialog = false;
__declspec(selectany) bool		CDLOptions::bCloseAfterAllDL = false;
__declspec(selectany) bool		CDLOptions::bShowWindowOnDL = false;

__declspec(selectany) CString	CDLOptions::strImgDLFolderPath;
__declspec(selectany) DWORD		CDLOptions::dwImgExStyle = DLO_OVERWRITEPROMPT;

__declspec(selectany) vector<CString>	CDLOptions::s_vecDLFolderHistory;
__declspec(selectany) vector<CString>	CDLOptions::s_vecImageDLFolderHistory;

__declspec(selectany) CString	CDLOptions::s_strDLFinishSoundFilePath;

///////////////////////////////////////////////////////
// COptionDialog

class CDLOptionDialog 
	: public CDialogImpl<CDLOptionDialog> 
	, public CWinDataExchange<CDLOptionDialog>
	, protected CDLOptions

{
public:
	enum { IDD = IDD_DLOPTION };

	// Constructor
	CDLOptionDialog()
		: m_nRadioImg(0)
	{ }

    // DDXマップ
    BEGIN_DDX_MAP(CDLOptionDialog)
        DDX_TEXT(IDC_CMB_DOWNLOADFOLDER			, strDLFolderPath)
		DDX_CHECK(IDC_CHECK_OPENDIALOG_BEFOREDL	, bUseSaveFileDialog)
		DDX_CHECK(IDC_CHECK_CLOSEAFTERALLDL		, bCloseAfterAllDL)
		DDX_CHECK(IDC_CHECK_SHOWWINDOW_ONDL		, bShowWindowOnDL)
		DDX_TEXT(IDC_CMB_IMGDOWNLOADFOLDER		, strImgDLFolderPath)
		DDX_RADIO(IDC_RADIO_OVERWRITEPROMPT		, m_nRadioImg)
		DDX_TEXT(IDC_EDIT_SOUNDFILEPATH			, m_strDLFinishSoundFilePath)
    END_DDX_MAP()

	BEGIN_MSG_MAP(CDLOptionDialog)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX(IDOK		, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL	, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_FOLDERSELECT, OnFolderSelect )
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_IMGFOLDERSELECT, OnFolderSelect )
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_SOUNDFILESELECT, OnSoundFileSelect )
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_PLAYSOUND	, OnPlaySound )
	END_MSG_MAP()


	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		CenterWindow();

		if (dwImgExStyle == DLO_USEUNIQUENUMBER)
			m_nRadioImg = 1;

		m_strDLFinishSoundFilePath = s_strDLFinishSoundFilePath;

		DoDataExchange(DDX_LOAD);

		/* 履歴の設定 */
		CComboBox cmbDLFolder = GetDlgItem(IDC_CMB_DOWNLOADFOLDER);
		std::for_each(s_vecDLFolderHistory.cbegin(), s_vecDLFolderHistory.cend(), [&cmbDLFolder](const CString& strFolder) {
			cmbDLFolder.AddString(strFolder);
		});

		CComboBox cmbImageDLFolder = GetDlgItem(IDC_CMB_IMGDOWNLOADFOLDER);
		std::for_each(s_vecImageDLFolderHistory.cbegin(), s_vecImageDLFolderHistory.cend(), [&cmbImageDLFolder](const CString& strFolder) {
			cmbImageDLFolder.AddString(strFolder);
		});
		

		return TRUE;
	}

	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString strDLFolder = strDLFolderPath;
		DoDataExchange(DDX_SAVE);
		if (strDLFolderPath.IsEmpty()) {
			MessageBox(_T("既定のDLフォルダが設定されていません。"), NULL, MB_ICONWARNING);
			strDLFolderPath = strDLFolder;
			DoDataExchange(DDX_LOAD, IDC_CMB_DOWNLOADFOLDER);
			return ;
		}

		/* 履歴の保存 */
		SavePathToHistory(strDLFolderPath, kDLFolderHistory);
		SavePathToHistory(strImgDLFolderPath, kImageDLFolderHistory);


		if (m_nRadioImg == 0)
			dwImgExStyle	= DLO_OVERWRITEPROMPT;
		else 
			dwImgExStyle	= DLO_USEUNIQUENUMBER;

		s_strDLFinishSoundFilePath	= m_strDLFinishSoundFilePath;

		SaveProfile();	// 設定を保存

		EndDialog(nID);
	}

	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		EndDialog(nID);
	}


	void OnFolderSelect(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString strPath;
		CShellFileOpenDialog SHdlg(NULL, FOS_PICKFOLDERS);
		if (SHdlg.IsNull() == false) {
			if (SHdlg.DoModal(m_hWnd) == IDOK) {
				SHdlg.GetFilePath(strPath);
			}
		} else {
			CFolderDialog dlg(NULL, _T("フォルダを選択してください。"));
			if (dlg.DoModal(m_hWnd) == IDOK) {
				strPath = dlg.GetFolderPath();
			}
		}
		if (strPath.IsEmpty() == FALSE) {
			UINT	nTargetID;
			switch (nID) {
			case IDC_BUTTON_FOLDERSELECT:		
				nTargetID = IDC_CMB_DOWNLOADFOLDER; 
				strDLFolderPath	   = strPath;
				break;
			case IDC_BUTTON_IMGFOLDERSELECT:	
				nTargetID = IDC_CMB_IMGDOWNLOADFOLDER;
				strImgDLFolderPath = strPath;
				break;
			}
			DoDataExchange(DDX_LOAD, nTargetID);
		}
	}

	void OnSoundFileSelect(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CFileDialog	dlg(TRUE, _T("wav"), nullptr, 6UL, _T("wavファイル (*.wav)\0*.wav\0"));
		if (dlg.DoModal(m_hWnd) == IDOK) {
			m_strDLFinishSoundFilePath = dlg.m_szFileName;
			DoDataExchange(DDX_LOAD, IDC_EDIT_SOUNDFILEPATH);
		}
	}

	void OnPlaySound(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CString temp = s_strDLFinishSoundFilePath;
		s_strDLFinishSoundFilePath = m_strDLFinishSoundFilePath;
		PlaySoundDLFinish();
		s_strDLFinishSoundFilePath = temp;
	}

private:
	int	m_nRadioImg;
	CString	m_strDLFinishSoundFilePath;

};






