/**
 *	@file	GuidUtl.h
 *	@brief	文字列からGUID型変数を生成
 *	@author	minit
 *	@note
 *		+++ minit氏のToolBandContenaプラグインのソースよりコピペ.
 */
#pragma once

class CGuidUtil {
public:
	//文字列からGUID型変数を作成する関数とその補助関数
	//調べたけど見つからなかったのでとりあえず作った
	//すごい適当なつくりなので変な入力与えると落ちます
	GUID StringToUUID(LPCTSTR lpstr)
	{
		GUID			guid;
		LPCTSTR 		pUUIDstr= lpstr;
		unsigned char	buf[1024];
		memset(buf, 0, sizeof buf);
		unsigned char*	pstr  	= buf;
		int 			brace 	= (*pUUIDstr == '{');
		if (brace)
			++pUUIDstr;
		while (*pUUIDstr) {
			*pstr++ = AtoI(*pUUIDstr++);
		}
		pstr		  = buf;
		guid.Data1	  = Pack_DWORD( Pack_8bit( *(pstr + 0), *(pstr + 1) ), Pack_8bit( *(pstr + 2), *(pstr + 3) ),
								    Pack_8bit( *(pstr + 4), *(pstr + 5) ), Pack_8bit( *(pstr + 6), *(pstr + 7) ) );
		pstr		 += 8;
		++pstr;

		guid.Data2	  = Pack_WORD( Pack_8bit( *(pstr + 0), *(pstr + 1) ), Pack_8bit( *(pstr + 2), *(pstr + 3) ) );
		pstr		 += 4;
		++pstr;

		guid.Data3	  = Pack_WORD( Pack_8bit( *(pstr + 0), *(pstr + 1) ), Pack_8bit( *(pstr + 2), *(pstr + 3) ) );
		pstr		 += 4;
		++pstr;

		guid.Data4[0] = Pack_8bit( *(pstr + 0), *(pstr + 1) );
		guid.Data4[1] = Pack_8bit( *(pstr + 2), *(pstr + 3) );
		pstr		 += 4;
		++pstr;

		for (int i = 2; i < 8; i++) {
			guid.Data4[i] = Pack_8bit( *pstr, *(pstr + 1) );
			pstr		 += 2;
		}

		return guid;
	}


private:
	int AtoI(TCHAR c)
	{
		if ('0' <= c && c <= '9')
			return c - '0';
		else if ('A' <= c && c <= 'F')
			return 10 + c - 'A';
		else if ('a' <= c && c <= 'f')
			return 10 + c - 'a';
		else
			return 0;
	}


	int Pack_8bit(unsigned char hi, unsigned char lo)
	{
		return (hi << 4) | lo;
	}


	DWORD Pack_DWORD(unsigned char n1, unsigned char n2, unsigned char n3, unsigned char n4)
	{
		return (n1 << 24) | (n2 << 16) | (n3 << 8) | n4;
	}


	WORD Pack_WORD(unsigned char n1, unsigned char n2)
	{
		return (n1 << 8) | n2;
	}
};
