/**
 *	@file	SimpleTree.h
 *	@brief	簡易ツリー構造を表現するためのテンプレート
 *	@note
 *	ノードの集合体としての木構造を、ノードクラスとその管理クラスで
 *	表現する。ノード毎に持つデータはテンプレートによって自由に決定できるので
 *	様々な木構造に応用出来る。
 *	ただし、機能はシンプルなので手間はかかる。
 */

#pragma once



/**
	CTreeNode<TNode>
	ノードとして必要な機能をラップするクラス。
	親ノードと前後のノード、それに最初の子と最後の子を把握する情報を持つ。
	そしてノード毎の情報をテンプレート引数によって持たせることが出来る。

	TNodeとして与える型の条件は、代入演算子によって問題なくデータをコピー
	しうるかどうかである。TNode型の中にポインタが存在する場合は
	代入演算子をオーバーロードして同じメモリを指すポインタが二つ存在しないよう
	注意する必要がある。コピーコンストラクタは使用しないはずだが、心配なら
	定義してもよい。
 */
template<class TNode>
class CTreeNode {
private:
public:
	TNode				Self;
	CTreeNode<TNode>*	pPrev,
					*	pNext,
					*	pParent,
					*	pFirstChild,
					*	pLastChild;

	CTreeNode() : Self()
	{
		pPrev = pNext = pParent = pFirstChild = pLastChild = NULL;
	}
};



/**
	CSimpleTree<TNode>
	ノードを管理して構造を操作するためのクラス

	TNode型の情報を持つノードを木構造として管理・操作することができる。
	内部ではCTreeNode<TNode>のポインタNODEPTRを介して操作する。
 */
template<class TNode>
class CSimpleTree {
	//型定義
	typedef CTreeNode<TNode>  NODE;
public:
	typedef NODE *			  NODEPTR;

private:
	//メンバ変数
	NODE	m_Root; 					///< ルート・親を持たない唯一のノード
	DWORD	m_dwNewCount;				///< メモリリーク監視用カウンタ

public:

	//コンストラクタ
	CSimpleTree() : m_Root() , m_dwNewCount(0) { }

	//デストラクタ
	~CSimpleTree() { Clear(); }

	//メソッド

public:
	NODEPTR Insert(NODEPTR pParent, TNode &NewItem)
	{
		if (!pParent)
			pParent = &m_Root;

		NODEPTR 	pNewNode = AllocNode();
		pNewNode->Self		 = NewItem; 	//中身のコピー
		pNewNode->pParent	 = pParent;

		if (!pParent->pFirstChild) {
			//最初の子ノードとして登録
			pParent->pFirstChild = pNewNode;
			pParent->pLastChild  = pNewNode;
		} else {
			//最後の子ノードの後ろにくっつける
			NODEPTR  pLastChild = pParent->pLastChild;
			pLastChild->pNext	= pNewNode;
			pNewNode->pPrev 	= pLastChild;
			pParent->pLastChild = pNewNode;
		}

		return pNewNode;
	}


	BOOL Clear()
	{
		if (Delete(&m_Root) && m_dwNewCount == 0)
			return TRUE;

		ATLASSERT(FALSE);				//メモリリークしている
		return FALSE;
	}


private:
	BOOL Delete(NODEPTR pNode)
	{
		ATLASSERT(pNode);

		//まず指定ノードの子を全て消す
		if (pNode->pFirstChild) {
			DeleteChildren(pNode);
		}

		//関連ノードのつながりを変更する
		if (pNode->pParent) {
			if (pNode->pPrev) {
				//前がある。すなわち後ろがあれば前後2つをつなぐだけでよい
				if (pNode->pNext) {
					pNode->pPrev->pNext = pNode->pNext;
					pNode->pNext->pPrev = pNode->pPrev;
				} else {
					pNode->pPrev->pNext = NULL;
					pNode->pLastChild	= pNode->pPrev;
				}
			} else {
				//これが一番先頭の子ノード。すなわち親との関係も変更する必要がある
				if (pNode->pNext) {
					pNode->pNext->pPrev 		= NULL;
					pNode->pParent->pFirstChild = pNode->pNext;
				} else {
					pNode->pParent->pFirstChild = NULL;
					pNode->pParent->pLastChild	= NULL;
				}
			}
		} else {
			//ルートであるので抜ける
			return TRUE;
		}

		//周りとのつながりが無くなったところで削除する
		FreeNode(pNode);
		return TRUE;
	}


private:
	BOOL DeleteChildren(NODEPTR pNode)
	{
		ATLASSERT(pNode);

		NODEPTR pChild = pNode->pFirstChild;

		if (!pChild)
			return TRUE;

		while (pChild->pNext) {
			pChild = pChild->pNext;
			Delete(pChild->pPrev);
		}

		Delete(pChild);

		pNode->pFirstChild = NULL;
		pNode->pLastChild  = NULL;
		return TRUE;
	}


public:
	TNode *GetData(NODEPTR pNode)
	{
		return (TNode *) pNode; //そのまんま
	}


private:
	NODEPTR GetRoot()
	{
		return &m_Root;
	}


public:
	//操作関数 直接ポインタいじったほうが早いけど
	NODEPTR GetFirstChild(NODEPTR pNode)
	{
		if (!pNode)
			pNode = &m_Root;

		return pNode->pFirstChild;
	}


private:
	NODEPTR GetLastChild(NODEPTR pNode)
	{
		if (!pNode)
			pNode = &m_Root;

		return pNode->pLastChild :
	}


	NODEPTR GetPrev(NODEPTR pNode)
	{
		ATLASSERT(pNode);
		return pNode->pPrev;
	}


public:
	NODEPTR GetNext(NODEPTR pNode)
	{
		ATLASSERT(pNode);
		return pNode->pNext;
	}


private:
	NODEPTR GetParent(NODEPTR pNode)
	{
		ATLASSERT(pNode);
		return pNode->pParent;
	}


private:
	NODEPTR AllocNode()
	{
		++m_dwNewCount;
		return new NODE();
	}


	void FreeNode(NODEPTR pNode)
	{
		if (pNode) {
			--m_dwNewCount;
			delete pNode;
		}
	}
};
