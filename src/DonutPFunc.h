/**
 *	@file DonutPFunc.h
 *	@brief	DonutP の汎用関数.
 */

#ifndef __DONUTPFUNC_H__
#define __DONUTPFUNC_H__

#pragma once

#include "MtlMisc.h"
#include "IniFile.h"
#include "Misc.h"		//+++


extern const TCHAR	g_cSeparater[];


/*+++ 使用ファイル１つなのでそちらに移動.
//#define PI						3.141592653589793238462643383279
//#define ANGLE_TO_RADIAN( a )	( (a) * PI / 180.0 )
//#define RADIAN_TO_ANGLE( a )	( (a) / PI * 180.0 )
*/

// アイテムＩＤリストからアイコンを作る
HICON	CreateIconFromIDList(PCIDLIST_ABSOLUTE pidl);

// アイコンからビットマップを作る
HBITMAP	CreateBitmapFromHICON(HICON hIcon);


double	_GetAngle(CPoint pt1, CPoint pt2);


BOOL	_CheckOsVersion_98Later();
BOOL	_CheckOsVersion_MELater();
BOOL	_CheckOsVersion_2000Later();
BOOL	_CheckOsVersion_XPLater();
BOOL	_CheckOsVersion_VistaLater();


/// undonut.exeと同じディレクトリに strFile があるものとしてフルパスを返す.
__inline CString _GetFilePath(const CString& strFile) { return Misc::GetExeDirectory() + strFile; }

CString _GetSkinDir();
CString _GetRecentCloseFile();
bool	_LoadToolTipText(int nCmdID, CString &strText);


// メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等	//+++ DonutRAPT よりパクってきたルーチン
void	RtlSetMinProcWorkingSetSize();

// strText の nFirst 文字目から、nCount 文字数を取得して返す。マルチバイト文字対応版
CString RtlMbbStrMid(CString strText, const int &nFirst = 0, const int &nCount = 1);

CString RtlGetSelectedText(const WTL::CEdit &edit);


//EM_GETSELがVisualStyle適用時にUNICODE的動作をする(UNICODEを定義しない場合でも)のでなんとかする関数
CString _GetSelectTextWtoA(CEdit &edit);
CString _GetSelectText(CEdit &edit);
CString _GetSelectText_OnCursor(CEdit &edit);		//+++

BOOL	_AddSimpleReBarBandCtrl(HWND hWndReBar,
								HWND hWndBand,
								int nID 				= 0,
								LPTSTR lpstrTitle		= NULL,
								BOOL bNewRow			= FALSE,
								int cxWidth 			= 0,
								BOOL bFullWidthAlways	= FALSE);
bool	FileWriteString(const CString& strFile, std::list<CString>* pString);
bool	FileReadString(const CString& strFile, std::list<CString>* pString);

BOOL	_SetCombboxCategory(CComboBox &cmb, HMENU hMenu);
BOOL	_DontUseID(UINT uID);
void	_PickUpCommandSub(HMENU hMenuSub, CComboBox &cmbCmd);
void	_PickUpCommand(HMENU hMenu, int nPopup, CComboBox &cmbCmd);


//minit
BOOL	_ReplaceImageList(CString strBmpFile, CImageList &imgs, DWORD dfltRes=0);
BOOL	_QueryColorString(CIniFileI &pr, COLORREF &col, LPCTSTR lpstrKey);


//+++ .manifestの存在チェック	※ CThemeDLLLoader から独立 & ちょっとリネーム
#if (defined UNICODE) && (defined USE_INNER_MANIFEST)
inline bool IsExistManifestFile() { return 1; }
#else
inline bool IsExistManifestFile() { return (::GetFileAttributes(Misc::GetExeFileName() + _T(".manifest")) != 0xFFFFFFFF); }
#endif

//+++ -----------------------------------
CString 	GetFavoriteLinksFolder_For_Vista();
int 		GetFontHeight(HFONT hFont);
int 		ForceMessageLoop(HWND hWnd=0);





#endif	// __DONUTPFUNC_H__

