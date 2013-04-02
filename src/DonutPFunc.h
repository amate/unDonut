/**
 *	@file DonutPFunc.h
 *	@brief	DonutP の汎用関数.
 */

#ifndef __DONUTPFUNC_H__
#define __DONUTPFUNC_H__

#pragma once

#include "Misc.h"		//+++

extern const TCHAR	g_cSeparater[];

//////////////////////////////////////////////////////

//---------------------------------------
/// lengthを持ちitemに3つの引数を取るコレクション専用
template<class collection>
void	ForEachHtmlElement(collection col, function<bool (IDispatch*)> func)
{
	if (col) {
		long length = 0;
		col->get_length(&length);
		for (long i = 0; i < length; ++i) {
			CComVariant	vIndex(i);
			CComPtr<IDispatch>	spDisp;
			col->item(vIndex, vIndex, &spDisp);
			if (func(spDisp) == false)
				break;
		}
	}
}


//-----------------------------------------
/// アイテムＩＤリストからアイコンを作る
HICON	CreateIconFromIDList(PCIDLIST_ABSOLUTE pidl);

//-----------------------------------------
/// アイコンからビットマップを作る
HBITMAP	CreateBitmapFromHICON(HICON hIcon);


double	_GetAngle(CPoint pt1, CPoint pt2);


BOOL	_CheckOsVersion_XPLater();
BOOL	_CheckOsVersion_VistaLater();

//----------------------------------------

/// iniファイルパスを初期化する
void	InitDonutConfigFilePath(LPTSTR iniFilePath, int sizeInWord);

/// filenameをConfigフォルダ以下のパスにして返す
CString GetConfigFilePath(const CString& filename);

/// undonut.exeと同じディレクトリに strFile があるものとしてフルパスを返す.
inline CString _GetFilePath(const CString& strFile) { return Misc::GetExeDirectory() + strFile; }

//----------------------------------------
/// 設定されたスキンフォルダのパスを返す(最後に'\\'がつく)
CString _GetSkinDir();

//----------------------------------------
/// コマンドに関連付けられた文字列を取得する
bool	_LoadToolTipText(int nCmdID, CString &strText);

//----------------------------------------
/// メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等	//+++ DonutRAPT よりパクってきたルーチン
void	RtlSetMinProcWorkingSetSize();


//-------------------------------------------
/// エディットコントロールで選択されている文字列を返す
CString _GetSelectText(const CEdit &edit);

//-------------------------------------------
///+++ エディットの、選択テキストの取得. 未選択の場合は、カーソル位置(キャレットのこと？)の単語を取得.(ページ内検索用)
CString _GetSelectText_OnCursor(const CEdit &edit);		//+++


BOOL	_AddSimpleReBarBandCtrl(HWND hWndReBar,
								HWND hWndBand,
								int nID 				= 0,
								LPTSTR lpstrTitle		= NULL,
								BOOL bNewRow			= FALSE,
								int cxWidth 			= 0,
								BOOL bFullWidthAlways	= FALSE);


//----------------------------------------------
/// strFile に Stringsを改行しつつ書き込む
bool	FileWriteString(const CString& strFile, const std::list<CString>& Strings);

//----------------------------------------------
/// strFile から 1行ごとにStringsに入れていく(※1行につき4096文字以上読み込むと分割される)
bool	FileReadString(const CString& strFile, std::list<CString>& Strings);


//---------------------------------------------
/// メニューの文字列を順次コンボボックスに追加する
BOOL	_SetCombboxCategory(CComboBox &cmb, HMENU hMenu);
BOOL	_DontUseID(UINT uID);
//----------------------------------------------
/// メニューのコマンドに割り当てられた文字列をコンボボックス/リストボックスに追加する
template <class boxT>
void	PickUpCommandSub(CMenuHandle menuSub, boxT box)
{
	int nPopStartIndex = box.AddString(g_cSeparater);
	int nAddCnt = 0;

	int nCount = menuSub.GetMenuItemCount();
	for (int ii = 0; ii < nCount; ++ii) {
		HMENU	hMenuSub2 = menuSub.GetSubMenu(ii);
		if (hMenuSub2)
			PickUpCommandSub(hMenuSub2, box);

		UINT	nCmdID	  = menuSub.GetMenuItemID(ii);
		if ( _DontUseID(nCmdID) )
			break;

		CString strMenu;
		CToolTipManager::LoadToolTipText(nCmdID, strMenu);

		if ( strMenu.IsEmpty() )
			continue;

		int 	nIndex	  = box.AddString(strMenu);
		box.SetItemData(nIndex, nCmdID);
		nAddCnt++;
	}

	if (nAddCnt != 0)
		box.AddString(g_cSeparater);
	else
		box.DeleteString(nPopStartIndex);
}
//-------------------------------------------------
/// hMenuのnPopup番目のサブメニューのコマンドに割り当てられた文字列をコンボボックスに追加する
template <class boxT>
void	PickUpCommand(CMenuHandle menuRoot, int nPopup, boxT box)
{
	CMenu	menu = menuRoot.GetSubMenu(nPopup);
	box.ResetContent();

	int nCount = menu.GetMenuItemCount();
	for (int ii = 0; ii < nCount; ++ii) {
		HMENU	hMenuSub = menu.GetSubMenu(ii);
		if (hMenuSub)
			PickUpCommandSub(hMenuSub, box);

		UINT	nCmdID	 = menu.GetMenuItemID(ii);

		if ( _DontUseID(nCmdID) )
			break;

		CString strMenu;
		CToolTipManager::LoadToolTipText(nCmdID, strMenu);

		if ( strMenu.IsEmpty() )
			continue;

		int nIndex = box.AddString(strMenu);
		box.SetItemData(nIndex, nCmdID);
	}

	menu.Detach();

	//不要なセパレータの削除
	int   nCountSep = 0;
	int   nCountCmb = box.GetCount();

	for (int i = 0; i < nCountCmb - 1; i++) {
		if (box.GetItemData(i) == 0) {
			nCountSep++;

			if (box.GetItemData(i + 1) == 0) {
				box.DeleteString(i);
				nCountCmb--;
				i--;
			}
		}
	}

	if (nCountSep > 2) {
		if (box.GetItemData(0) == 0)
			box.DeleteString(0);

		int nIndexLast = box.GetCount() - 1;

		if (box.GetItemData(nIndexLast) == 0)
			box.DeleteString(nIndexLast);
	}
}

//-------------------------------------------------
/// イメージリストのイメージをstrBmpFileのビットマップで置換する
BOOL	_ReplaceImageList(const CString& strBmpFile, CImageList& imgs, DWORD defaultResID = 0);


//+++ .manifestの存在チェック	※ CThemeDLLLoader から独立 & ちょっとリネーム
#if (defined UNICODE) && (defined USE_INNER_MANIFEST)
inline bool IsExistManifestFile() { return 1; }
#else
inline bool IsExistManifestFile() { return ::PathFileExists(Misc::GetExeFileName() + _T(".manifest")) != 0; }
#endif

//------------------------------------------
/// フォントの高さを返す
int		GetFontHeight(HFONT hFont);


//------------------------------------------
///強制的に、その場でメッセージをさばく...
int 	ForceMessageLoop(HWND hWnd = NULL);


//------------------------------------------
/// エラーコードに対応するエラーメッセージ文字列を返す
CString	GetLastErrorString(HRESULT hr = -1);


//------------------------------------------
/// 一時ファイル置き場のパスを返す
bool	GetDonutTempPath(CString& strTempPath);

//------------------------------------------
/// エクスプローラーを開いてアイテムを選択する
void	OpenFolderAndSelectItem(const CString& strPath);

///------------------------------------------
/// vecTextを "テキスト１\0テキスト２\0テキスト３\0\0" といった形式に変換して返す
std::pair<std::unique_ptr<WCHAR[]>, int>	CreateMultiText(const std::vector<CString>& vecText);

///------------------------------------------
/// "テキスト１\0テキスト２\0テキスト３\0\0" といった文字列を vector<CString> にして返す
std::vector<CString>	GetMultiText(LPCWSTR multiText);

#endif	// __DONUTPFUNC_H__

