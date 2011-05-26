/**
 *	@file	ExMenu.h
 *	@brief	拡張メニュー : ユーザー定義メニューのこと
 */
#pragma once

#include "SimpleTree.h"
#include <vector>
#include "resource.h"
#include "DonutPFunc.h" 	//+++


class CExMenuItem {
public:
	CString 	strText;
	CString 	strCommand; 	//コマンド文字列
	CString 	strArg; 		//コマンド引数
	int 		nID;			//項目のコマンドID
	int 		nStyle; 		//メニュー項目のスタイル 下記定数参照
};


#define COMMAND_EXMENU_RANGE(min, max)												 \
	{																				 \
		if (uMsg == WM_COMMAND && LOWORD(wParam) >= min  && LOWORD(wParam) <= max) { \
			BOOL bRet = CExMenuManager::ExecCommand( (int) LOWORD(wParam) );		 \
			if (bRet)																 \
				return 0;															 \
		}																			 \
	}



typedef CSimpleTree<CExMenuItem>::NODEPTR  HMTREENODE;


///////////////////////////////////////////////////////
// CExMenuCtrl

class CExMenuCtrl 
{
private:
	CSimpleTree<CExMenuItem> *		m_pTree;

	//データ行の要素数
	enum EData_Count {
		DATA_COUNT_PARENT		= 2,
		DATA_COUNT_SEPARATOR	= 1,
		DATA_COUNT_ITEM 		= 4,
		DATA_COUNT_LEVEL		= 1,
	};


public:

	enum EExMenu_Style {
		EXMENU_STYLE_NORMAL 	= 0,
		EXMENU_STYLE_PARENT 	= 1,
		EXMENU_STYLE_SEPARATOR	= 2,
	};


	CExMenuCtrl(CSimpleTree<CExMenuItem> *pTree = NULL)
		: m_pTree(pTree)
	{
	}


	void	SetTree(CSimpleTree<CExMenuItem> *pTree) { m_pTree = pTree; }
	CSimpleTree<CExMenuItem>*	GetTree() { return m_pTree; }


	//ツリービューからデータ読み込み
	BOOL LoadFromTreeView(CTreeViewCtrl &TreeC)
	{
		ATLASSERT(m_pTree);

		if ( !::IsWindow(TreeC.m_hWnd) )
			return FALSE;

		m_pTree->Clear();

		HTREEITEM hRoot = TreeC.GetRootItem();

		if (!hRoot)
			return TRUE;

		LoadFromTreeViewSub(NULL, hRoot, TreeC);

		return TRUE;
	}


private:
	BOOL LoadFromTreeViewSub(HMTREENODE hNode, HTREEITEM hItem, CTreeViewCtrl& TreeC)
	{
		//hItemの子ノードをpNodeの子ノードとして登録する

		HTREEITEM hChild = TreeC.GetChildItem(hItem);
		while(hChild){
			//データはツリービューのアプリ定義から持ってくる
			CExMenuItem *pExMenuItem = (CExMenuItem *)TreeC.GetItemData(hChild);
			if(pExMenuItem){
				HMTREENODE hNewNode = m_pTree->Insert(hNode,*pExMenuItem);
				LoadFromTreeViewSub(hNewNode,hChild,TreeC);
			}

			hChild = TreeC.GetNextSiblingItem(hChild);
		}

		return TRUE;
	}


public:
	//データからツリービューを構築
	BOOL MakeTreeView(CTreeViewCtrl &TreeC)
	{
		ATLASSERT(m_pTree);

		if ( !::IsWindow(TreeC.m_hWnd) )
			return FALSE;

		TreeC.DeleteAllItems();

		HTREEITEM hRoot = TreeC.GetRootItem();
		//if(!hRoot) return TRUE;

		MakeTreeViewSub(NULL, hRoot, TreeC);

		return TRUE;
	}


private:
	BOOL MakeTreeViewSub(HMTREENODE hNode, HTREEITEM hItem, CTreeViewCtrl &TreeC)
	{
		HMTREENODE hChildNode = m_pTree->GetFirstChild(hNode);

		while (hChildNode) {
			CExMenuItem *pExMenuItem = (CExMenuItem *) m_pTree->GetData(hChildNode);

			if (pExMenuItem) {
				HTREEITEM hNewItem = TreeC.InsertItem(pExMenuItem->strText, hItem, TVI_LAST);

				if (hNewItem) {
					TreeC.SetItemData(hNewItem, (DWORD_PTR) pExMenuItem);
					MakeTreeViewSub(hChildNode, hNewItem, TreeC);
				}
			}

			hChildNode = m_pTree->GetNext(hChildNode);
		}

		return TRUE;
	}


public:
	//メニュー構築
	BOOL MakeMenu(HMENU& hMenuRoot)
	{
		ATLASSERT(m_pTree);

		if (!::IsMenu(hMenuRoot)) return FALSE;

		MakeMenuSub(NULL, hMenuRoot);

		return TRUE;
	}


private:
	BOOL MakeMenuSub(HMTREENODE hNode, HMENU hMenu)
	{
		HMTREENODE hChildNode = m_pTree->GetFirstChild(hNode);
		int 	   nIndex	  = 0;

		while (hChildNode) {
			CExMenuItem*	pItem = m_pTree->GetData(hChildNode);
			MENUITEMINFO	info  = { sizeof (MENUITEMINFO) };

			if (pItem->nStyle == EXMENU_STYLE_NORMAL) {
				//普通のメニュー項目
				info.fMask		= MIIM_DATA | MIIM_ID | MIIM_TYPE;
				info.cch		= pItem->strText.GetLength() + 1;
				info.dwTypeData = pItem->strText.GetBuffer(0);
				info.wID		= pItem->nID;
				info.dwItemData = (DWORD_PTR) pItem;
				info.fType		= MFT_STRING;

				if ( !InsertMenuItem(hMenu, nIndex, TRUE, &info) )
					return FALSE;

			} else if (pItem->nStyle == EXMENU_STYLE_PARENT) {
				//子階層を持つメニュー項目
				info.fMask		= MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
				info.cch		= pItem->strText.GetLength() + 1;
				info.dwTypeData = pItem->strText.GetBuffer(0);
				info.wID		= pItem->nID;
				info.dwItemData = (DWORD_PTR) pItem;
				info.fType		= MFT_STRING;
				//サブメニューの作成
				HMENU hSubMenu = ::CreatePopupMenu();
				info.hSubMenu	= hSubMenu;

				if ( !InsertMenuItem(hMenu, nIndex, TRUE, &info) )
					return FALSE;

				//子階層の構築
				if ( !MakeMenuSub(hChildNode, hSubMenu) )
					return FALSE;

			} else if (pItem->nStyle == EXMENU_STYLE_SEPARATOR) {
				//セパレータ
				info.fMask = MIIM_TYPE;
				info.fType = MFT_SEPARATOR;

				if ( !InsertMenuItem(hMenu, nIndex, TRUE, &info) )
					return FALSE;
			}

			hChildNode = m_pTree->GetNext(hChildNode);
			nIndex++;
		}

		return TRUE;
	}


public:
	//ファイルからデータ読み込み
	BOOL ReadData(CString &strFile)
	{
		ATLASSERT(m_pTree);
		m_pTree->Clear();

		FILE *fp   = _tfopen( strFile, _T("r") );
		if (fp == NULL)
			return FALSE;

		BOOL  bRet = ReadDataSub(fp, NULL);
		fclose(fp);
		return bRet;
	}


private:
	BOOL ReadDataSub(FILE *fp, HMTREENODE hNode)
	{
		//まず下位階層への移動記号"→"を探す
		std::vector<CString> aryValue;
		CString 			 strBuf;
		BOOL				 bStarted = FALSE;
		int 				 nCount;

		while (1) {
			if ( !ReadString(fp, strBuf) )
				return FALSE;

			strBuf.TrimRight();

			//空行とコメントは飛ばす
			if ( strBuf.IsEmpty() || strBuf.Left(2) == _T("//") )
				continue;

			if (!bStarted) {
				nCount	 = ParseDataString(strBuf, aryValue);

				if ( nCount != DATA_COUNT_LEVEL || aryValue[0] != _T("→") )
					continue;

				bStarted = TRUE;
			}

			if (bStarted) {
				nCount = ParseDataString(strBuf, aryValue);

				int		aryValue0 = _ttoi(aryValue[0]);		//+++ UNICODE対応
				if (nCount == DATA_COUNT_LEVEL /*DATA_COUNT_SEPARATOR*/) {
					//上位階層への移動記号"←"を見つけたら終わる
					if ( aryValue[0] == _T("←") )
						break;

					//セパレータである場合
					//+++ if (atoi(aryValue[0]) == CExMenuCtrl::EXMENU_STYLE_SEPARATOR)
					if (aryValue0 == CExMenuCtrl::EXMENU_STYLE_SEPARATOR)	//+++ 	UNICODE対応
					{
						CExMenuItem item;
						item.nStyle  = aryValue0;	//+++ item.nStyle  = atoi(aryValue[0]);	UNICODE対応.
						HMTREENODE	hItem = m_pTree->Insert(hNode, item);

						if (!hItem)
							return FALSE;
					}
				} else if (nCount == DATA_COUNT_PARENT) {
					//親メニューとその下位項目を登録する

					CExMenuItem item;
					item.nStyle  = aryValue0;	//+++ item.nStyle  = atoi(aryValue[0]);	UNICODE対応.
					item.strText = aryValue[1];
					HMTREENODE	hParent = m_pTree->Insert(hNode, item);

					if (!hParent)
						return FALSE;

					if ( !ReadDataSub(fp, hParent) )
						return FALSE;

				} else if (nCount >= DATA_COUNT_ITEM) {
					//板情報を登録する 上のコピペ
					CExMenuItem item;
					item.nStyle		= aryValue0;					//+++ item.nStyle  = atoi(aryValue[0]);	UNICODE対応.
					item.nID		= _ttoi(aryValue[1]);			//+++ item.nID		= atoi(aryValue[1]);
					item.strText	= aryValue[2];
					item.strCommand = aryValue[3];

					if (aryValue.size() > DATA_COUNT_ITEM) {
						for (int i = 4; i < (int) aryValue.size(); i++) {
							item.strArg += aryValue[i] + _T(" ");
						}
					}

					HMTREENODE	hItem = m_pTree->Insert(hNode, item);

					if (!hItem)
						return FALSE;
				} else {
					return FALSE;
				}
			}
		}

		return TRUE;
	}


	BOOL ReadString(FILE *fp, CString &strBuf)
	{
		char buf[0x4000+2];
		buf[0] = 0;	//+++
		if (fgets(buf, 0x4000, fp) == NULL) {
			return FALSE;
		}
		strBuf = buf;
		return TRUE;
	}


	//文字をタブ、カンマ等区切りでvectorに分割する
	int ParseDataString(const CString &strLine, std::vector<CString> &vecText)
	{
		vecText.clear();

		int 	nCount = 0;
		CString strBuf, strText = strLine;
		strText.TrimLeft( _T(" \t,　") );
		strText.TrimRight( _T(" \t,　") );

		while ( !strText.IsEmpty() ) {
			int nPos = strText.FindOneOf( _T("\t,") );

			if (nPos == -1) {
				if ( !strText.IsEmpty() ) {
					vecText.push_back(strText);
					strText.Empty();
				}
			} else {
				strBuf	= strText.Left(nPos);
				strText = strText.Mid(nPos);
				vecText.push_back(strBuf);
			}

			nCount++;
			strText.TrimLeft(_T(" \t,　"));
		}

		return nCount;
	}


public:
	//ファイルへデータ保存
	BOOL SaveData(CString &strFile)
	{
		ATLASSERT(m_pTree);
		FILE *fp   = _tfopen( strFile, _T("w") );
		if (fp == NULL)
			return FALSE;

		BOOL  bRet = SaveSub(fp, NULL, -1);
		fclose(fp);
		return bRet;
	}


private:
	BOOL SaveSub(FILE *fp, HMTREENODE hNode, int nDepth)
	{
		SaveLevelMark(fp, TRUE, nDepth + 1);
		BOOL bRet = PutOutData(fp, hNode, nDepth + 1);
		SaveLevelMark(fp, FALSE, nDepth + 1);
		return bRet;
	}


	BOOL PutOutData(FILE *fp, HMTREENODE hNode, int nDepth)
	{
		HMTREENODE hChild = m_pTree->GetFirstChild(hNode);

		while (hChild) {
			if ( m_pTree->GetFirstChild(hChild) ) {
				//子ノードを持つので自らの情報を書き出した後子ノードの情報を出力する
				PutOutParentItemData(fp, *m_pTree->GetData(hChild), nDepth);
				SaveSub(fp, hChild, nDepth);
			} else {
				//子ノードを持たないメニュー情報をもつノード
				PutOutItemData(fp, *m_pTree->GetData(hChild), nDepth);
			}

			hChild = m_pTree->GetNext(hChild);
		}

		return TRUE;
	}


	BOOL SaveLevelMark(FILE *fp, BOOL bDown, int nDepth)
	{
		CString		strOut;

		if (bDown) {
			strOut = _T("→");
		} else {
			strOut = _T("←");
		}

		strOut += _T("\n");

		CString 	strWhite;

		for (int i = 0; i < nDepth; i++)
			strWhite += _T("\t");

		strOut	= strWhite + strOut;

		return _fputts(strOut, fp) != EOF;		//+++ return fputs(strOut, fp) != EOF /*? TRUE : FALSE*/;	UNICODE修正
	}


	BOOL PutOutParentItemData(FILE *fp, CExMenuItem &item, int nDepth)
	{
		CString 	strBuf;

		strBuf.Format(_T("%d\t%s\n"), item.nStyle, item.strText);

		CString strWhite;

		for (int i = 0; i < nDepth; i++)
			strWhite += _T("\t");

		strBuf = strWhite + strBuf;

	  #if 1	//+++ UNICODE修正
		return _fputts(strBuf, fp) != EOF;
	  #else
		if (fputs(strBuf, fp) == EOF)
			return FALSE;

		return TRUE;
	  #endif
	}


	BOOL PutOutItemData(FILE *fp, CExMenuItem &item, int nDepth)
	{
		CString 	strBuf;

		strBuf.Format(_T("%d\t%d\t%s\t%s\n"), item.nStyle, item.nID, item.strText, item.strCommand);

		CString 	strWhite;

		for (int i = 0; i < nDepth; i++)
			strWhite += _T("\t");

		strBuf = strWhite + strBuf;

		if (_fputts(strBuf, fp) == EOF)
			return FALSE;

		return TRUE;
	}

};



class CExMenuManager {
public:
	enum EExMenu {
		EXMENU_ID_FIRST 		= 0,
		//EXMENU_ID_ADDRESSBAR	= 1,
		EXMENU_COUNT			= 1,
	};

	static CSimpleArray<CSimpleTree<CExMenuItem> *> *	s_aryTree;

	static void Initialize()
	{
		static LPCTSTR	EXMENU_FILENAME[]	= { _T("usermenu.txt")		  , NULL };


		ATLASSERT(!s_aryTree);
		s_aryTree = new CSimpleArray<CSimpleTree<CExMenuItem> *>();

		CExMenuCtrl menuC;

		for (int i = 0; i < EXMENU_COUNT; i++) {
			CSimpleTree<CExMenuItem> *pTree 	  = new CSimpleTree<CExMenuItem>();
			menuC.SetTree(pTree);

			if (EXMENU_FILENAME[i] == NULL)
				break;

			CString 	strFileName = _GetFilePath( _T("menu\\") ) + EXMENU_FILENAME[i];
			menuC.ReadData(strFileName);
			s_aryTree->Add(pTree);
		}
	}


	static void Terminate()
	{
		ATLASSERT(s_aryTree);
		if (s_aryTree == 0) 	//+++ 念のため弾く.
			return;
		int nCount = s_aryTree->GetSize();

		for (int i = 0; i < nCount; i++) {
			delete (*s_aryTree)[i];
		}

		delete s_aryTree;
		s_aryTree = NULL;		//+++ 削除後は強制クリアしとく.
	}


	//フルメニュー（メインメニュー+特殊メニュー+拡張メニュー）をロードする
	//受けたHMENUは必ずDestroyMenuすること
	static HMENU LoadFullMenu()
	{
		static LPCTSTR	EXMENU_NAME[]		= { _T("ユーザー定義メニュー"), NULL };
		CMenuHandle 	menu, menuSpc;

		menu.LoadMenu(IDR_MAINFRAME);

		//特殊メニューの追加
		menuSpc.LoadMenu(IDR_SPECIAL);
		menu.AppendMenu( MF_POPUP, (UINT_PTR) menuSpc.m_hMenu, _T("特殊") );

		//拡張メニューの追加
		int 		nExMenuCount = s_aryTree->GetSize();
		CExMenuCtrl menuC;
		CMenuHandle menuEx;

		for (int i = 0; i < nExMenuCount; i++) {
			menuC.SetTree( (*s_aryTree)[i] );
			menuEx.CreateMenu();

			if ( menuC.MakeMenu(menuEx.m_hMenu) ) {
				menu.AppendMenu(MF_POPUP, (UINT_PTR) menuEx.m_hMenu, EXMENU_NAME[i]);
			}
		}

		return menu.m_hMenu;
	}


	static HMENU LoadExMenu(int nID)
	{
		CMenuHandle menu;
		CExMenuCtrl menuC( (*s_aryTree)[nID] );

		menu.CreatePopupMenu();
		menuC.MakeMenu(menu.m_hMenu);
		return menu.m_hMenu;
	}


	static HMTREENODE FindFromID(int nCmdID, CSimpleTree<CExMenuItem> *pTree)
	{
		return FindFromIDSub(NULL, nCmdID, pTree);
	}


	static HMTREENODE FindFromIDSub(HMTREENODE hParent, int nCmdID, CSimpleTree<CExMenuItem> *pTree)
	{
		HMTREENODE hChild = pTree->GetFirstChild(hParent);

		while (hChild) {
			CExMenuItem *pItem = pTree->GetData(hChild);

			if (pItem) {
				if (pItem->nStyle == CExMenuCtrl::EXMENU_STYLE_NORMAL && pItem->nID == nCmdID) {
					return hChild;
				} else if (pItem->nStyle == CExMenuCtrl::EXMENU_STYLE_PARENT) {
					HMTREENODE hTarget = FindFromIDSub(hChild, nCmdID, pTree);

					if (hTarget)
						return hTarget;
				}
			}

			hChild = pTree->GetNext(hChild);
		}

		return NULL;
	}


	static CString GetToolTip(int nCmdID)
	{
		if (EXMENU_USERDEFINE_MIN <= nCmdID && nCmdID <= EXMENU_USERDEFINE_MAX) {
			CSimpleTree<CExMenuItem> *pTree = (*s_aryTree)[EXMENU_ID_FIRST];
			HMTREENODE				  hNode = FindFromID(nCmdID, pTree);

			if (hNode) {
				CExMenuItem *pItem = pTree->GetData(hNode);
				return pItem->strText;
			}
		}

		return CString();
	}


	static BOOL ExecCommand(int nID)
	{
	  #if 1 //*+++ 未初期化変数だった...が0だとマズイ? とりあえず、お試しで-1
		int 	nIndex = -1;
	  #endif

		if (EXMENU_USERDEFINE_MIN <= nID && nID <= EXMENU_USERDEFINE_MAX) {
			nIndex = EXMENU_ID_FIRST;
		}

		if (nIndex < 0 || EXMENU_COUNT <= nIndex)
			return FALSE;

		CSimpleTree<CExMenuItem>*	pTree  = (*s_aryTree)[nIndex];
		if (!pTree)
			return FALSE;

		HMTREENODE		hNode  = FindFromID(nID, pTree);
		if (!hNode)
			return FALSE;

		CExMenuItem *	pItem  = pTree->GetData(hNode);
		if (!pItem)
			return FALSE;
		if (pItem->strCommand.Left(6).CompareNoCase(_T("script")) == 0) 
			pItem->strCommand.Insert(0, Misc::GetExeDirectory());	//\\+
		::ShellExecute(NULL, _T("open"), pItem->strCommand, pItem->strArg, Misc::GetExeDirectory(), SW_SHOWNORMAL);
		return TRUE;
	}
};



__declspec(selectany) CSimpleArray<CSimpleTree<CExMenuItem> *> *	CExMenuManager::s_aryTree = NULL;
