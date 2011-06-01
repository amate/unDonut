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
/// undonut.exeと同じディレクトリに strFile があるものとしてフルパスを返す.
__inline CString _GetFilePath(const CString& strFile) { return Misc::GetExeDirectory() + strFile; }

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
/// メニューのコマンドに割り当てられた文字列をコンボボックスに追加する
void	_PickUpCommandSub(HMENU hMenuSub, CComboBox &cmbCmd);
//-------------------------------------------------
/// hMenuのnPopup番目のサブメニューのコマンドに割り当てられた文字列をコンボボックスに追加する
void	_PickUpCommand(HMENU hMenu, int nPopup, CComboBox &cmbCmd);


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


#endif	// __DONUTPFUNC_H__

