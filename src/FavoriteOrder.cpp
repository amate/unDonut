/**
 *	@file	FavoriteOrder.cpp
 *	@brief	レジストリにあるお気に入り情報を操作して並び順を書き換えるクラス
 */
#include "stdafx.h"
#include "FavoriteOrder.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/// レジストリからデータを読み込む
int CFavoriteOrderHandler::ReadData(const CString &strDirPath)
{
	if (Misc::IsExistFile( strDirPath) == 0)
		return FALSE;
	CString 		strKeyName = GetOrderKeyName(strDirPath);

	if ( strKeyName.IsEmpty() )
		return FALSE;

	m_strRegKey  = strKeyName;
	m_strDirPath = strDirPath;

	//レジストリからバイナリデータを取得
	Misc::CRegKey 	rkOrder;
	LONG			lRet	   = rkOrder.Open(HKEY_CURRENT_USER, strKeyName);

	if (lRet != ERROR_SUCCESS) {
		if ( ReConstructData(strDirPath) )
			return -1;	//TRUE;
		return FALSE;
	}

	DWORD			dwSize	   = 0;
	DWORD			dwType	   = REG_BINARY;
	lRet		 = ::RegQueryValueEx(rkOrder, _T("Order"), NULL, &dwType, NULL, &dwSize);

	if (lRet == ERROR_FILE_NOT_FOUND) {
		if ( ReConstructData(strDirPath) )
			return TRUE;
	}

	if ( lRet != ERROR_SUCCESS || dwSize < sizeof (OrderDataHeader) )
		return FALSE;

	BYTE *			pByte	   = (BYTE *) _alloca( (dwSize + 10) * sizeof (BYTE) );
	::memset(pByte, 0, dwSize + 10);

	lRet		 = ::RegQueryValueEx(rkOrder, _T("Order"), NULL, &dwType, pByte, &dwSize);

	if (lRet != ERROR_SUCCESS)
		return FALSE;

	rkOrder.Close();

	//データを配列m_aryDataに格納する
	m_aryData.clear();
	OrderDataHeader header	   = *(OrderDataHeader *) pByte;
	pByte		+= sizeof (OrderDataHeader);
	LPBYTE			pEnd	   = pByte + ( header.dwFullSizem8 + 8 - sizeof (OrderDataHeader) );
	int 			nCount	   = header.dwItemCount;

	for (int i = 0; i < nCount; i++) {
		OrderItemTemplate*	pItem = (OrderItemTemplate *) pByte;			//こっちから
		OrderItemData		data;											//こっちへデータをコピー
		data.dwSize 	= pItem->dwSize;
		data.dwPosition = pItem->dwPosition;

		CItemIDList 	idl(&pItem->idlName);
		data.idlName	= idl;

		if (data.idlName.GetSize() & 1)
			data.bPadding = TRUE;
		else
			data.bPadding = FALSE;

		data.dwFlag 	= *(DWORD *) ( pByte + pItem->dwSize - sizeof (DWORD) );
		data.strName	= MtlGetFileName( data.idlName.GetPath() );

		m_aryData.push_back(data);
		pByte		   += pItem->dwSize;

		if (pByte > pEnd)
			break;
	}

	ReConstructData(m_strDirPath);

	return TRUE;
}




/// データを追加する.
/// m_strDirPath からの相対パスで指定.
BOOL CFavoriteOrderHandler::AddData(CString strSrcFileName, DWORD dwPosition)
{
	if (dwPosition == -1)
		dwPosition = m_aryData.size();

	if (dwPosition < 0 || m_aryData.size() < dwPosition)
		return FALSE;

	if ( strSrcFileName.IsEmpty() )
		return FALSE;

	//ファイル名はフルパスに直す
	CString 					strDir	  = m_strDirPath;
	MtlMakeSureTrailingBackSlash(strDir);

	if (strSrcFileName.Find('\\') == -1)
		strSrcFileName = strDir + strSrcFileName;

	//データを埋める
	OrderItemData	item;
	item.dwPosition = dwPosition;
  #if 1	//+++
	CString dirName = Misc::GetDirName(strSrcFileName);
	CString	baseName= Misc::GetFileBaseName(strSrcFileName);
	CString	dstName = m_strDirPath + _T('\\') + baseName;
	if (dirName.CompareNoCase(m_strDirPath) == 0) {
		if ( !MtlIsValidPath(strSrcFileName) )
			return FALSE;

		item.idlName	= strSrcFileName;
	} else {
		if ( !MtlIsValidPath(dstName) )
			return FALSE;

		//::MoveFile( strSrcFileName, dstName);
		item.idlName	= dstName;
	}
  #else
	if ( !MtlIsValidPath(strSrcFileName) )
		return FALSE;
	item.idlName	= strFileName;
  #endif
	CItemIDList 	idlFolder = m_strDirPath;
	item.idlName   -= idlFolder;

	int 	nSize	= item.idlName.GetSize();
	item.bPadding	= (nSize & 1);		//+++ ? TRUE : FALSE;
	item.dwSize 	= nSize + s_itemsize_without_idl + (item.bPadding /*+++ ? 1 : 0*/ );
	item.strName	= baseName;			//+++ MtlGetFileName(strSrcFileName);

	//既存の同名の項目が存在すれば削除する
	CString 					strPath   = item.idlName.GetPath();
	FavoriteDataArray::iterator it		  = m_aryData.begin();
	while (1) {
		if ( it == m_aryData.end() )
			break;

		if (it->idlName.GetPath() == strPath) {
			it = m_aryData.erase(it);
		} else {
			++it;
		}
	}

	//フォルダか否かを判定
	//if ( MtlIsValidPath(strFileName) )
	if ( MtlIsValidPath(dstName) )
	{
		CString strFavDir;
		MtlGetFavoritesFolder(strFavDir);

		if (strFavDir != m_strDirPath) {
			//if ( MtlIsDirectory(strSrcFileName) )
			if ( MtlIsDirectory(dstName) )
				item.dwFlag = IE_MENUORDER_FOLDER;
			else
				item.dwFlag = IE_MENUORDER_ITEM;
		} else {
			item.dwFlag = IE_MENUORDER_ZERO;
		}
	} else {
		return FALSE;
	}

  #if 0	//+++
	//ポジションをずらす
	for (it = m_aryData.begin(); it != m_aryData.end(); it++) {
		if (dwPosition <= it->dwPosition)
			++it->dwPosition;
	}

   #if 1	//+++
	m_aryData.insert(m_aryData.begin()+dwPosition, item);
   #else
	m_aryData.push_back(item);

	// dwPotisionでソートする
	_FavoritesMenuPosCompare	f(m_strDirPath);
	std::stable_sort(m_aryData.begin(), m_aryData.end(), f);
   #endif
  #else
//ErrorLogPrintf("%s  %s  %d\n", LPCTSTR(m_strDirPath), LPCTSTR(dstName), dwPosition);
	//ポジションをずらす
	for (it = m_aryData.begin(); it != m_aryData.end(); it++) {
		if (dwPosition <= it->dwPosition)
			++it->dwPosition;
	}

	item.bRef = true;
	m_aryData.push_back(item);
   #if 0	//+++
	ReConstructData(m_strDirPath);
   #else
	//文字列でソートする
	_FavoritesMenuNameCompare	f(m_strDirPath);
	std::sort(m_aryData.begin(), m_aryData.end(), f);
   #endif
  #endif

	return TRUE;
}



BOOL	CFavoriteOrderHandler::MoveData(CFavoriteOrderHandler& srcOrder, const CString &srcPath, DWORD dstPos)
{
	CString srcDir   = Misc::GetDirName(srcPath);
	CString name     = Misc::GetFileBaseName(srcPath);
	CString& dstDir	 = m_strDirPath;
	CString dstName  = dstDir + _T('\\') + name;
	bool	bCopy	 = false;
	if (srcDir.CompareNoCase(dstDir) == 0) {
		DWORD srcPos = FindNameToPosition( name/*srcPath*/ );
		return MoveData(srcPos, dstPos);
	}
	if (srcOrder.FindName(srcPath) < 0) {
		ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、ソースフォルダが不正(%s)\n"), LPCTSTR(srcPath));
		return FALSE;
	}
	if (::MoveFile( srcPath, dstName ) == 0) {
		ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、ファイル移動失敗(%s)\n"), LPCTSTR(name));
		return FALSE;
	} else {
		bCopy = ::CopyFile(dstName, srcPath, TRUE) != 0;	//+++ 項目の削除時の実体として、ダミーを作成
	}
	if (srcOrder.DeleteData( srcPath, true ) == 0) {
		ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、元フォルダ側で削除がされなかった(%s)\n"), LPCTSTR(name));
	}
	if (bCopy)	//+++ もし何か失敗していた場合のための削除.
		::DeleteFile(srcPath);
	if (AddData(srcPath, dstPos) == 0) {
		ErrorLogPrintf(_T("お気に入りメニューでの、フォルダをまたいだ移動で、行き先への追加失敗(%s)\n"), LPCTSTR(name));
		return FALSE;
	}
	return TRUE;
}



BOOL CFavoriteOrderHandler::MoveData(DWORD dwSrcPos, DWORD dwDstPos)
{
	if (dwSrcPos == dwDstPos)
		return TRUE;

	if (dwSrcPos < 0 || dwDstPos < 0)
		return FALSE;

	int 						nSrcIndex = FindPosition(dwSrcPos);

	if (nSrcIndex == -1)
		return FALSE;

	FavoriteDataArray::iterator 	it;

	if (dwSrcPos < dwDstPos) {
		//dwSrcPos以上dwDstPos以下のポジションを一つ下げる
		for (it = m_aryData.begin(); it != m_aryData.end(); ++it) {
			if (dwSrcPos <= it->dwPosition && it->dwPosition <= dwDstPos)
				it->dwPosition--;
		}
	} else {
		//dwDstPos以上dwSrcPos以下のポジションを一つ上げる
		for (it = m_aryData.begin(); it != m_aryData.end(); ++it) {
			if (dwDstPos <= it->dwPosition && it->dwPosition <= dwSrcPos)
				it->dwPosition++;
		}
	}

	m_aryData[nSrcIndex].dwPosition = dwDstPos;
	return TRUE;
}



BOOL CFavoriteOrderHandler::DeleteData(CString strFileName, bool bSw)
{
	if ( strFileName.IsEmpty() )
		return FALSE;

	//ファイル名はフルパスに直す
	CString 					strDir = m_strDirPath;
	MtlMakeSureTrailingBackSlash(strDir);

	if (strFileName.Find('\\') == -1) {
		strFileName = strDir + strFileName;
	} else {
	}

	if ( !MtlIsValidPath(strFileName)  && bSw == 0)
		return FALSE;

	CItemIDList 				idl 	  = strFileName;
	CItemIDList 				idlFolder = m_strDirPath;
	idl -= idlFolder;

	//同じファイル名を持つ項目を削除
	CString 					strPath   = idl.GetPath();
	FavoriteDataArray::iterator it		  = m_aryData.begin();

	while (1) {
		if ( it == m_aryData.end() )
			break;

		if (it->idlName.GetPath() == strPath) {
			DWORD dwPosition = it->dwPosition;
			m_aryData.erase(it);

			//他の項目の番号を付け直す
			for (FavoriteDataArray::iterator it2 = m_aryData.begin(); it2 != m_aryData.end(); ++it2) {
				DWORD dwPos2 = it2->dwPosition;
				if (dwPosition < dwPos2)
					--it2->dwPosition;
				else if (dwPosition == dwPos2)
					ErrorLogPrintf(_T("お気に入りメニューで%sと同じ位置番号(%d)のデータがある(%s)\n"), LPCTSTR(strPath), dwPos2, LPCTSTR(it2->strName));
			}

			return TRUE;
		} else {
			++it;
		}
	}

	return FALSE;
}



///データをレジストリに書き込む
BOOL CFavoriteOrderHandler::SaveData()
{
  #if 0
	if ( !_check_flag(CFavoritesMenuOption::GetStyle(), EMS_CUSTOMIZE_ORDER) )
		return FALSE;
  #endif
	//まずサイズを確定させる
	DWORD			 dwFullSize = 0;

	for (FavoriteDataArray::iterator it = m_aryData.begin(); it != m_aryData.end(); ++it) {
		dwFullSize += it->dwSize;
	}

	dwFullSize			 += sizeof (OrderDataHeader); //ヘッダ分を足す

	//メモリを確保
	LPBYTE			 lpMem		= (LPBYTE) new BYTE[dwFullSize];

	if (!lpMem)
		return FALSE;

	LPBYTE			 lpData 	= lpMem;

	//ヘッダデータ格納
	OrderDataHeader *pHeader	= (OrderDataHeader *) lpData;
	pHeader->dwSize8	  = 8;
	pHeader->dwFullSizem8 = dwFullSize - pHeader->dwSize8;
	pHeader->dwUnknown	  = IE_MENUORDER_UNKNOWN1;
	pHeader->dwUnknown2   = IE_MENUORDER_UNKNOWN2;
	pHeader->dwItemCount  = m_aryData.size();
	lpData				 += sizeof (OrderDataHeader);

	//アイテムデータをコピー
	int 			 nCount 	= m_aryData.size();

	for (int i = 0; i < nCount; i++) {
		LPBYTE		   lpStart = lpData;
		OrderItemData *pItem   = &m_aryData[i];
		*(DWORD *) lpData = pItem->dwSize;
		lpData			 += 4;
		*(DWORD *) lpData = pItem->dwPosition;
		lpData			 += 4;
		int 		   nSize   = pItem->idlName.GetSize();
		::memcpy(lpData, (LPITEMIDLIST) pItem->idlName, nSize);
		lpData			 += nSize;

		if (pItem->bPadding) {
			*(BYTE *) lpData = 0;
			lpData			+= 1;
		}

		*(DWORD *) lpData = pItem->dwFlag;
		lpData			 += 4;

		if (lpData - pItem->dwSize != lpStart)
			goto error;
	}

	//データを書き込む
	{
		Misc::CRegKey	rkOrder;
		LONG			lRet = rkOrder.Open(HKEY_CURRENT_USER, m_strRegKey);

		if (lRet != ERROR_SUCCESS) {
			lRet = rkOrder.Create(HKEY_CURRENT_USER, m_strRegKey);
			if (lRet != ERROR_SUCCESS)
				goto error;
		}

		lRet = ::RegSetValueEx(rkOrder, _T("Order"), 0, REG_BINARY, lpMem, dwFullSize);
		rkOrder.Close();

		if (lRet != ERROR_SUCCESS)
			goto error;
	}

	//メモリ開放
	delete[] lpMem;
	return TRUE;

  error:
	delete[] lpMem;
	return FALSE;
}



/// データを構築する
BOOL CFavoriteOrderHandler::ReConstructData(CString strDirPath)
{
	//フォルダのファイルを列挙してデータに含まれていないものを探して追加
	int 							nFirstCount = m_aryData.size();
	FavoriteDataArray::iterator 	it;

	for (it = m_aryData.begin(); it != m_aryData.end(); ++it)
		it->bRef = FALSE;

	_FavoriteItemCollector		funcClct(strDirPath, m_aryData);
	MtlForEachObject_OldShell(strDirPath, funcClct);
	int 						nValidCount = 0;
	it = m_aryData.begin();

	while ( it != m_aryData.end() ) {
		if (!it->bRef) {
			for (FavoriteDataArray::iterator it2 = m_aryData.begin(); it2 != m_aryData.end(); ++it2)
				if ( (int) it2->dwPosition > (int) it->dwPosition )
					it2->dwPosition--;

			it = m_aryData.erase(it);
		} else {
			if (it->dwPosition != FAVORITESORDER_NOTFOUND)
				nValidCount++;

			++it;
		}
	}

	//ソートする
	_FavoritesMenuNameCompare	funcComp(strDirPath);
	std::sort(m_aryData.begin(), m_aryData.end(), funcComp);

	//追加したデータに番号をつける
	int 						j			= nValidCount;
	int 						nCount		= m_aryData.size();

	for (int i = 0; i < nCount; i++) {
		if (m_aryData[i].dwPosition == FAVORITESORDER_NOTFOUND) {
			m_aryData[i].dwPosition = j;
			j++;
		}
	}

	return TRUE;
}



///指定した名前を持つ項目の番号を返す
int CFavoriteOrderHandler::FindName(const CString &strFileName)
{
	CString 	strName = MtlGetFileName(strFileName);
	int 		nCount	= m_aryData.size();

	for (int i = 0; i < nCount; i++) {
		if (strName == m_aryData[i].strName)
			return i;
	}

	return -1;
}



/// 指定した位置にある項目の番号を返す
int CFavoriteOrderHandler::FindPosition(DWORD dwPosition)
{
	int 	nCount = m_aryData.size();

	for (int i = 0; i < nCount; i++) {
		if (dwPosition == m_aryData[i].dwPosition)
			return i;
	}

	return -1;
}



CString CFavoriteOrderHandler::GetOrderKeyName(const CString &strDirPath)
{
	CString 	strFavRoot;

	MtlGetFavoritesFolder(strFavRoot);
	MtlMakeSureTrailingBackSlash(strFavRoot);

	CString 	strDir		= strDirPath;
	MtlMakeSureTrailingBackSlash(strDir);

	if (strDir.Find(strFavRoot) == -1)
		return _T("");

	CString 	strKeyName	= REGKEY_IE_MENUORDER_ROOT;
	CString 	strRelative = strDirPath.Right( strDirPath.GetLength() - strFavRoot.GetLength() );

	if ( !strRelative.IsEmpty() )
		strKeyName += _T('\\') + strRelative;

	MtlRemoveTrailingBackSlash(strKeyName);

	return strKeyName;
}

