/**
 *	@file	Misc.cpp
 *	@biref	undonut�Ɉˑ����Ȃ����Ɣėp�I�ȎG���ȃ��[�`���Q
 *	@note
 *
 */
#pragma once

#include "stdafx.h"
#include "Misc.h"
#include <stdarg.h>
#include <io.h>
#include <winerror.h>


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace Misc {


// ==========================================================================

///+++ printf�����̕������Ԃ�
const CString StrFmt(const TCHAR* fmt, ...)
{
	va_list app;
	va_start(app, fmt);
	CString 	str;
	str.FormatV(fmt, app);
	va_end(app);
	return str;
}


///+++ printf�����̕������Ԃ�
const CString StrVFmt(const TCHAR* fmt, va_list app)
{
	CString 	str;
	str.FormatV(fmt, app);
	va_end(app);
	return str;
}



///+++ �󔒂ŋ�؂�ꂽ�ŏ��̕������Ԃ�.
const CString GetStrWord(const TCHAR* str, const TCHAR** ppNext)
{
	#define IS_SPACE(c) 	(0 <  (c) && (c) <= _T(' '))
	#define IS_SPACE_EOS(c) (0 <= (c) && (c) <= _T(' '))
	const TCHAR*	p = (const TCHAR*)str;
	int 			c;
	do {
		c = *p++;
	} while (IS_SPACE(c));
	--p;
	const TCHAR*	bgn = p;
	do {
		c = *p++;
	} while (IS_SPACE_EOS(c) == 0);
	--p;
	const TCHAR*	end = p;
	if (ppNext)
		*ppNext = p;
	size_t l = end - bgn;
	return CString(bgn, (int)l);
	#undef IS_SPACE
	#undef IS_SPACE_EOS
}



///+++ �󔒂ŋ�؂�ꂽ�����P��𕪉�����
void SeptTextToWords(std::vector<CString>& strs, const TCHAR* line)
{
	const TCHAR*	p = line;
	while (*p) {
		CString str = GetStrWord(p, &p);
		if (str.IsEmpty())
			break;
		strs.push_back(str);
	}
}


///+++ ����������s�����ŕ���. (��s�͖���)
void SeptTextToLines(std::vector<CString>& strs, const TCHAR* text)
{
	const TCHAR*	t = text;
	const TCHAR*	p = t;
	for (;;) {
		int c = *p++;
		if (c == '\r' || c == '\n' || c == '\0') {
			ptrdiff_t l = p - t - 1;
			if (l > 0)
				strs.push_back(CString(t, (int)l));
			if (c == 0)
				break;
			if (c == '\r' && *p == '\n')
				++p;
			t = p;
		}
	}
}


/** �ȈՂȁA���p�������J�E���g�B
 *	UNICODE�̏ꍇ�A�P���ȕ����R�[�h����ŁA�\�������̔��p�������Z�̒l�ŕ�����T�C�Y��Ԃ�.
 *  ���p�S�p�̔��ʂ͎蔲���K���B���F�t�H���g����Ȃ̂ŁA���\�R�ɂȂ邪�ASJIS��ł�����ۂ��Ȃ�悤��...���炢.
 *  ��؂̊T�Z�����߂�ꍇ�p��.
 */
unsigned	eseHankakuStrLen(const TCHAR* s)
{
  #ifdef UNICODE
	unsigned l = 0;
	while (*s) {
		unsigned c = *s++;
		if (c < 0x200 || (c >= 0xFF61 && c <= 0xFF9F))
			++l;
		else
			l += 2;
	}
	return l;
  #else	//+++ SJIS�Ȃ炻�̂܂ܕ������ł�����
	return strlen(s);
  #endif
}


/** ���񔼊p�w��Ŏw�肳�ꂽ�������܂ł̕������Ԃ�.
 */
const CString eseHankakuStrLeft(const CString& str, int len)
{
  #ifdef UNICODE
	LPCTSTR		t = LPCTSTR(str);
	LPCTSTR		s = t;
	int		 l = 0;
	unsigned b = 0;
	while (*s) {
		unsigned c = *s;
		if (c < 0x200 || (c >= 0xFF61 && c <= 0xFF9F))
			++l;
		else
			l += 2;
		if (l > len) {
			l = b;
			break;
		}
		b = l;
		++s;
	}
	l = static_cast<int>(s - t);
	return str.Left(l);
  #else	//+++ SJIS�Ȃ炻�̂܂ܕ������ł�����
	return str.Left(len);
  #endif
}




// ==========================================================================
//+++ mlang.dll ���g�����ق����悢���낤�Ȃ�... ���̂���.
//+++ ���� UNICODE��EUC-JP�ϊ��ň�U sjis ���o�R���邽�ߖ{���Ȃ�ϊ��\��(��O��l����)��������ϊ����Ă��܂�.


enum { CP_SJIS = 932 };


/** +++ jis �R�[�h�� sjis�ɕϊ�
 */
static int jis_to_sjis(int c)
{
	if (c <= 0xff)
		return c;
	c -= 0x2121;
	if (c & 0x100)
		c += 0x9e;
	else
		c += 0x40;
	if ((BYTE)c >= 0x7f)
		c++;
	c = (((c >> (8+1)) + 0x81)<<8) | ((BYTE)c);
	if (c >= 0xA000)
		c += 0x4000;
	return c;
}



/** +++ sjis �� jis �ɕϊ�
 */
static int sjis_to_jis(int c)
{
	if (c <= 0xff)
		return c;
	if (c >= 0xE000)
		c -= 0x4000;
	c = (((c>>8) - 0x81)<<9) | (UCHAR)c;
	if ((UCHAR)c >= 0x80)
		c -= 1;
	if ((UCHAR)c >= 0x9e)
		c += 0x62;
	else
		c -= 0x40;
	c += 0x2121;
	return c;
}

static inline int eucjp_to_sjis(int c) { return jis_to_sjis(c & ~0x8080); }	///< +++ euc-jp�R�[�h��sjis�ɕϊ�
static inline int sjis_to_eucjp(int c) { return sjis_to_jis(c) | 0x8080 ; }	///< +++ sjis�R�[�h��euc-jp�ɕϊ�


/** +++ sjis������� euc-jp������ɕϊ�
 */
const std::vector<char> sjis_to_eucjp(const char* pSjis)
{
	std::vector<char> vecEucjp;
	if (pSjis == NULL)
		return vecEucjp;
	const unsigned char* s = (const unsigned char*)pSjis;
	int				 l = 0;
	int					 c;
	while ((c = *s++) != 0) {
		++l;
		if (c >= 0xA0 && c <= 0xDF) {	// ���p�J�i��2�o�C�g�ɂȂ�.
			++l;
		}
	}
	if (l == 0)
		return vecEucjp;
	vecEucjp.resize( l + 1 );
	unsigned char* d = (unsigned char*) &vecEucjp[0];
	s				 = (const unsigned char*)pSjis;
	do {
		c = *s++;
		if (c <= 0x80 || c == 0xFF /* c >= 0xfc*/) {
			*d++ = c;
		} else if (c <= 0x9F || c >= 0xE0) {	// �S�p
			if (*s) {
				c = (c << 8) | *s;
				++s;
				c = sjis_to_eucjp(c);
				*d++ = c >> 8;
				*d++ = (unsigned char)c;
			}
		} else {	// ���p�J�i 0xA0-0xDF
			*d++ = 0x8E;
			*d++ = (unsigned char)c;
		}
	} while (c);
	l = static_cast<int>((char*)d - (char*)&vecEucjp[0]);
	vecEucjp.resize(l);
	return vecEucjp;
}



/** +++ euc-jp������� sjis������ɕϊ�
 */
const std::vector<char>	eucjp_to_sjis(const char* pEucjp)
{
	std::vector<char>	vecSjis;
	if (pEucjp== NULL)
		return vecSjis;
	unsigned l = (unsigned)strlen(pEucjp);
	if (l == 0)
		return vecSjis;
	vecSjis.resize( l + 1 );
	int c;
	const unsigned char* s = (const unsigned char*)pEucjp;
	unsigned char*		 d = (unsigned char*)&vecSjis[0];
	do {
		c = *s++;
		if (c < 0x80) {
			*d++ = c;
		} else if (c == 0x8e) {
			if (*s) {
				c    = *s++;
				*d++ = c;
			}
		} else if (c == 0x8f) {		// ���Ή�...
		} else {	// �S�p
			if (*s & 0x80) {
				c = (c << 8) | *s;
				++s;
				c = eucjp_to_sjis(c);
				*d++ = c >> 8;
				*d++ = (unsigned char)c;
			}
		}
	} while (c);
	l = static_cast<unsigned>((char*)d - (char*)&vecSjis[0]);
	if (l != vecSjis.size())
		vecSjis.resize(l);
	return vecSjis;
}



/** +++ wcs(utf16le)������ �� sjis �ɕϊ�.
 */
const std::vector<char> wcs_to_sjis(const wchar_t* pWcs)
{
	std::vector<char> vecSjis;
	unsigned bytes = ::WideCharToMultiByte(CP_SJIS, 0, pWcs,-1, NULL, 0, NULL, NULL);
	if (bytes) {
		vecSjis.resize( bytes );
		bytes = ::WideCharToMultiByte(CP_SJIS, 0, pWcs, -1, &vecSjis[0], bytes, NULL, NULL);
		if (bytes == 0)
			vecSjis.clear();
	}
	return vecSjis;
}


/** +++ sjis�������wcs(utf16le)������ɕϊ�.
 */
const std::vector<wchar_t> sjis_to_wcs(const char* pSjis)
{
	std::vector<wchar_t> vecWcs;
	unsigned chrs = ::MultiByteToWideChar(CP_SJIS, 0, pSjis,-1, NULL, 0);
	if (chrs) {
		vecWcs.resize( chrs );
		chrs = ::MultiByteToWideChar(CP_SJIS, 0, pSjis, -1, &vecWcs[0], chrs);
		if (chrs == 0)
			vecWcs.clear();
	}
	return vecWcs;
}


/** +++ wcs(utf16le)������ �� utf8 �ɕϊ�.
 */
const std::vector<char> wcs_to_utf8(const wchar_t* pWcs)
{
	std::vector<char> vecUtf8;
	unsigned bytes = ::WideCharToMultiByte(CP_UTF8, 0, pWcs,-1, NULL, 0, NULL, NULL);
	if (bytes) {
		vecUtf8.resize( bytes );
		bytes = ::WideCharToMultiByte(CP_UTF8, 0, pWcs, -1, &vecUtf8[0], bytes, NULL, NULL);
		if (bytes == 0)
			vecUtf8.clear();
	}
	return vecUtf8;
}



/** +++ utf8�������wcs(utf16le)������ɕϊ�.
 */
const std::vector<wchar_t> utf8_to_wcs(const char* pUtf8)
{
	std::vector<wchar_t> vecWcs;
	unsigned chrs = ::MultiByteToWideChar(CP_UTF8, 0, pUtf8,-1, NULL, 0);
	if (chrs) {
		vecWcs.resize( chrs );
		chrs = ::MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, &vecWcs[0], chrs);
		if (chrs == 0)
			vecWcs.clear();
	}
	return vecWcs;
}



/** +++ sjis�������utf8������ɕϊ�
 */
const std::vector<char> sjis_to_utf8(const char* pSjis)
{
	return wcs_to_utf8( &sjis_to_wcs( pSjis )[0] );
}



/** +++ utf8�������sjis������ɕϊ�.
 */
const std::vector<char> utf8_to_sjis(const char* pUtf8)
{
	return wcs_to_sjis( &utf8_to_wcs( pUtf8 )[0] );
}



/** +++ wcs(utf16)�������euc-jp������ɕϊ�
 */
const std::vector<char> wcs_to_eucjp(const wchar_t* pWcs)
{
	return sjis_to_eucjp( &wcs_to_sjis( pWcs )[0] );
}



/** +++ euc-jp�������wcs(utf16)������ɕϊ�.
 */
const std::vector<wchar_t> eucjp_to_wcs(const char* pEucjp)
{
	return sjis_to_wcs( &eucjp_to_sjis( pEucjp )[0] );
}



/** +++ sjis�������vector<char>�ɃR�s�[����
 */
const std::vector<char> sjis_to_sjis(const char* pSjis)
{
	std::vector<char> vecSjis;
	if (pSjis && pSjis[0]) {
		unsigned l = static_cast<unsigned>(strlen(pSjis) + 1);
		vecSjis.resize( l );
		memcpy(&vecSjis[0], pSjis, l);
	}
	return vecSjis;
}


/** wchar_t�������vector<wchar_t>�ɃR�s�[����
 */
const std::vector<wchar_t> wcs_to_wcs(const wchar_t* pWcs)
{
	std::vector<wchar_t> vecWcs;
	if (pWcs && pWcs[0]) {
		unsigned l = (unsigned)wcslen(pWcs) + 1;
		vecWcs.resize( l );
		memcpy(&vecWcs[0], pWcs, l * 2);
	}
	return vecWcs;
}



// ==========================================================================


///+++ utf-8�����񂩂��`�F�b�N. 0:�����  1:ascii(<=0x7f)�̂�. 3:utf8������.
int  checkUtf8(const char* src)
{
	const unsigned char* s = (unsigned char*)src;
	bool		bAsc  = false;
	bool		bBad  = false;
	bool		bZen  = false;
	int			c;
	while ((c = *s++) != 0) {
		if  (c <= 0x7f) {
			bAsc = true;
		} else {
			if (c < 0xC0) {
				bBad = true;
				break;
			}
			int c2 = *s++;
			if ((c2 & 0xC0) != 0x80) {
				bBad = true;
				break;
			}
			c2 &= 0x3f;
			if (c < 0xE0) {
				c = ((c & 0x1F) << 6) | c2;
				if (c <= 0x7F) {
					bBad = true;
					break;
				}
				bZen = true;
			} else {
				int c3 = *s++;
				if ((c3 & 0xC0) != 0x80) {
					bBad = true;
					break;
				}
				c3 &= 0x3f;
				if (c < 0xF0) {
					c = ((c & 0xF) << 12) | (c2 << 6) | c3;
					if (c <= 0x7FF) {
						bBad = true;
						break;
					}
					bZen = true;
				} else {
					int c4 = *s++;
					if ((c4 & 0xC0) != 0x80) {
						bBad = true;
						break;
					}
					c4 &= 0x3f;
					if (c < 0xF8) {
						c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
						if (c <= 0xFFFF) {
							bBad = true;
							break;
						}
						bZen = true;
					} else {
						int c5 = *s++;
						if ((c5 & 0xC0) != 0x80) {
							bBad = true;
							break;
						}
						c5 &= 0x3f;
						if (c < 0xFC) {
							c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
							if (c <= 0x1fFFFF) {
								bBad = true;
								break;
							}
							bZen = true;
						} else {
							int c6 = *s++;
							if ((c6 & 0xC0) != 0x80) {
								bBad = true;
								break;
							}
							c6 &= 0x3f;
							c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
							if (c <= 0x3ffFFFF) {
								bBad = true;
								break;
							}
							bZen = true;
						}
					}
				}
			}
		}
	}
	if (bBad)
		return 0;
	if (bZen)
		return 3;
	if (bAsc)
		return 1;
	return 0;
}


///+++ euc-jp�����񂩂��`�F�b�N. 0:�����  1:ascii(<=0x7f)�̂�. 2:���p�J�i(0x8e),�g��0x8f���g���Ă���.  3:euc-jp
int  checkEucjp(const char* src)
{
	const unsigned char* s = (unsigned char*)src;
	bool		bAsc  = false;
	bool		bKata = false;
	bool		bBad  = false;
	bool		bZen  = false;
	int			c;
	while ((c = *s++) != 0) {
		if  (c <= 0x7f) {
			bAsc = true;

		} else if (c >= 0xA0 && c <= 0xfe) {
			c = *s;
			if (c) {
				++s;
				if (c >= 0xA0 && c <= 0xfe) {
					bZen = true;
				} else {
					bBad = true;
					break;
				}
			} else {
				bBad = true;
				break;
			}
		} else if (c == 0x8e) {		//+++ ���p�J�i
			c = *s;
			if (c) {
				++s;
				if (c >= 0xA0 && c <= 0xDF) {
					bKata = true;
				} else {
					bBad  = true;
					break;
				}
			} else {
				bBad = true;
				break;
			}

		} else if (c == 0x8f) {		//+++ ���󖢑Ή�
			;
		} else {
			bBad = true;
			break;
		}
	}
	if (bBad)
		return 0;
	if (bKata)
		return 2;
	if (bZen)
		return 3;
	if (bAsc)
		return 1;
	return 0;
}


///+++ sjis�����񂩂��`�F�b�N. 0:�����  1:ascii(<=0x7f)�̂�. 2:���p�J�i���g���Ă���.  3:sjis
int  checkSjis(const char* src)
{
	const unsigned char* s = (unsigned char*)src;
	bool		bAsc  = false;
	bool		bKata = false;
	bool		bBad  = false;
	bool		bZen  = false;
	int			c;
	while ((c = *s++) != 0) {
		if  (c <= 0x7f) {
			bAsc = true;
		} else if (c >= 0x81 && c <= 0xfe) {
			c = *s;
			if (c) {
				++s;
				if (c >= 0x40 && c <= 0xfe && c != 0x7f) {
					bZen = true;
				} else {
					bBad = true;
					break;
				}
			} else {
				bBad = true;
				break;
			}
		} else if (c >= 0xA0 && c <= 0xDF) {
			bKata = true;
		} else {
			bBad = true;
			break;
		}
	}
	if (bBad)
		return 0;
	if (bKata)
		return 2;
	if (bZen)
		return 3;
	if (bAsc)
		return 1;
	return 0;
}



///+++ ������̕����R�[�h���A1:sjis,2:euc,3:utf8���`�F�b�N����.
int checkSjisEucUtf8(const char* src, int dfltCode)
{
	int  utf8 = checkUtf8(src);
	int  euc  = checkEucjp(src);
	int  sjis = checkSjis(src);

	if (utf8 > 0) {
		if (utf8 > euc && utf8 > sjis)
			return 3;
		if ((dfltCode == 3 || dfltCode == 0) && utf8 >= euc && utf8 >= sjis)
			return 3;
	}
	if (euc > 0) {		//+++ �l�b�g��̃e�L�X�g�Ȃ̂�sjis��肩�͑�����?�Ƃ���sjis����ɂ�������
		if (euc > utf8 && euc > sjis)
			return 2;
		if ((dfltCode == 2 || dfltCode == 0) && euc >= utf8 && euc >= sjis)
			return 2;
	}
	if (sjis > 0) {
		if (sjis > utf8 && sjis > euc)
			return 1;
		if ((dfltCode == 1 || dfltCode == 0) && sjis >= utf8 && sjis >= euc)
			return 1;
	}

	if (utf8 > 0) {
		if (utf8 >= euc && utf8 >= sjis)
			return 3;
	}
	if (euc > 0) {
		if (euc >= utf8 && euc >= sjis)
			return 2;
	}
	if (sjis > 0) {
		if (sjis >= utf8 && sjis >= euc)
			return 1;
	}

	return dfltCode;
}



// ==========================================================================

///+++ �ʏ�̕������ %20 �����g��ꂽurl�ɕϊ�.
const CString	urlstr_encode(const char* str)
{
	unsigned				l = 0;
	const unsigned char*	s = (unsigned char*)str;
	if (s) {
		int		 c;
		while ((c = *s++) != 0) {
			if (   (c >= 0x30 && c <= 0x39)							// 0-9
				|| (c >= 0x40 && c <= 0x5A)							// @A-Z
				|| (c >= 0x61 && c <= 0x7A)							// a-z
				|| c == 0x2A || c == 0x2D || c == 0x2E || c == 0x5F	// *-._
			){
				++l;
			} else {
				l += 3;
			}
		}
	}
	if (l == 0)
		return CString();

	++l;
	std::vector<TCHAR>		buf;
	buf.resize(l);
	s 		 		= (unsigned char*)str;
	TCHAR* 		  d = &buf[0];
	int			  c;
	while ((c = *s++) != 0) {
		if (   (c >= 0x30 && c <= 0x39)							// 0-9
			|| (c >= 0x40 && c <= 0x5A)							// @A-Z
			|| (c >= 0x61 && c <= 0x7A)							// a-z
			|| c == 0x2A || c == 0x2D || c == 0x2E || c == 0x5F	// *-._
		){
			*d++ = c;
		} else {
			static TCHAR xdigit[] = _T("0123456789abcdef");
			*d++ = _T('%');
			*d++ = xdigit[ c >> 4 ];
			*d++ = xdigit[ c & 15 ];
		}
	}
	*d = _T('\0');
	return CString(&buf[0]);
}



///+++ %20 �����g��ꂽurl��ʏ�̕�����ɕϊ�. ���ɑS�p�������������Ă��Ȃ�����!
const std::vector<char>	urlstr_decode(const TCHAR* url)
{
  #ifdef UNICODE
	typedef wchar_t			char_t;
  #else
	typedef unsigned char	char_t;
  #endif
	std::vector<char>		buf;
	if (url == 0)
		return buf;
	const char_t* 	s = (const char_t*)url;
	unsigned 		l = 0;
	int				c;
	while ((c = *s++) != 0) {
		++l;
		if (c == '%') {
			if (*s) {
				++s;
				if (*s)
					++s;
			}
		}
	}
	++l;
	buf.resize(l);
	s 				  = (const char_t*)url;
	unsigned char*  d = (unsigned char*) &buf[0];
	while ((c = *s++) != 0) {
		if (c == '%') {
			c = *s++;
			if (c) {
				if      (c >= '0' && c <= '9') c = c - '0';
				else if (c >= 'A' && c <= 'F') c = 10 + c - 'A';
				else if (c >= 'a' && c <= 'f') c = 10 + c - 'a';
				else                           c = -1;
				int k = *s++;
				if (k && c >= 0) {
					if      (k >= '0' && k <= '9') k = k - '0';
					else if (k >= 'A' && k <= 'F') k = 10 + k - 'A';
					else if (k >= 'a' && k <= 'f') k = 10 + k - 'a';
					else                           k = -1;
					if (k >= 0) {
						c    = (c << 4) | k;
						*d++ = c;
					}
				}
			}
		} else {
			*d++ = char(c);
		}
	}
	*d = '\0';
	return buf;
}


///+++ %20 �����g��ꂽurl����{�ꕶ���R�[�h�Ƃ��Ă��������Ȃ��Ȃ�ʏ�̕�����ɕϊ�.
const CString	urlstr_decodeJpn(const TCHAR* url, int dfltCode)
{
	std::string	buf = urlstr_decode(url).data();

	int jpnCharSet = checkSjisEucUtf8(buf.c_str(), dfltCode);
	switch (jpnCharSet) {
	case 1:	// sjis
		return CString( buf.c_str() );
	case 2:	// euc-jp
		return CString( eucjp_to_tcs(buf.c_str()).data() );
	case 3:	// utf8
		return CString( utf8_to_tcs(buf.c_str()).data() );
	default:
		;
	}
	return CString(url);
}



///+++ �����񒆂ɁASJIS�ɕϊ��ł��Ȃ�UNICODE�������������Ă����ꍇ�A������S�̂�urlstr�ɕϊ�����.
const CString urlstr_encodeWhenNeedUnicode(const CString& str)
{
	BOOL	 flag  = 0;

	if (str.Find(_T('%')) >= 0) {	//+++ % ������Β����Ώ�.
	  #ifdef UNICODE
	  	const wchar_t*		 s = LPCTSTR(str);
	  #else
	  	const unsigned char* s = (unsigned char*) LPCTSTR(str);
	  #endif
		while (*s) {
			int c = *s++;
			if (c == _T('%')) {	//+++ %�������� %?? �̌`�Ȃ�A�ϊ��Ώ�.
				if (isxdigit(*s) && isxdigit(s[1])) {
					flag = 1;
					break;
				}
			}
		}
	}

  #ifdef UNICODE
	if (flag == 0)
		/*unsigned bytes =*/ ::WideCharToMultiByte(CP_SJIS, 0, str, -1, NULL, 0, NULL, &flag);
  #endif

	if (flag) {
		std::vector<char> buf = tcs_to_utf8(str);
		return urlstr_encode(&buf[0]);
	}
	return str;
}



///+++ �����񂪔��p�p���L���݂̂ł���%���������Ă���ꍇ�Autf8���G���R�[�h�������̂Ƃ��ăf�R�[�h����.
const CString urlstr_decodeWhenASC(const CString& str)
{
	if ((int)str.Find(_T('%')) >= 0) {	//+++ % ������Β����Ώ�.
	  #ifdef UNICODE
	  	const wchar_t*		 s = LPCTSTR(str);
	  #else
	  	const unsigned char* s = (unsigned char*) LPCTSTR(str);
	  #endif
		bool flag = 1;
		while (*s) {
			int c = *s++;
			if ((unsigned)c >= 0x80) {	//+++ 0x80�ȏオ�����sjis�┼�p�J�i���낤�ŁA�ϊ����Ȃ�.
				flag = 0;
				break;
			}
			if (c == _T('%')) {	//+++ %�������� %?? �̌`�łȂ��Ȃ�A�ϊ����Ȃ�
				if (!isxdigit(*s) || !isxdigit(s[1])) {
					flag = 0;
					break;
				}
			}
		}
		if (flag) {	//+++ �p���L���݂̂ł���%??���������Ă��鎞
			return utf8_to_CString( urlstr_decode( str ) );
		}
	}
	return str;
}



bool IsEnableUnicodeToSJIS(LPCWSTR pStr)
{
	enum { CP_SJIS = 932 };
	BOOL flag = 0;
	::WideCharToMultiByte(CP_SJIS, 0, pStr, -1, NULL, 0, NULL, &flag);
	return flag == 0;
}

#include <MLang.h>
/// �����R�[�h�s���ȕ������CString�ɂ��ĕԂ�
CString	UnknownToCString(const std::vector<char>& src)
{
	CString str;
	UINT nCodePage = CP_UTF8;
	CComPtr<IMultiLanguage2>	spMultiLang;
	HRESULT hr = spMultiLang.CoCreateInstance(CLSID_CMultiLanguage);
	if (SUCCEEDED(hr)) {
		int Scores = 5; // �擾������̐�
		DetectEncodingInfo Encoding[5] = { 0 };
		int nSize = (int)src.size();
		hr = spMultiLang->DetectInputCodepage(MLDETECTCP_DBCS, 0, (char*)src.data(), &nSize, Encoding, &Scores);
		if (SUCCEEDED(hr)) {
			DWORD dwMode = 0;
			UINT nSrcSize = static_cast<UINT>(nSize);
			UINT nDestSize = 0;
			hr = spMultiLang->ConvertStringToUnicode(&dwMode, Encoding[0].nCodePage, (char*)src.data(), &nSrcSize, str.GetBuffer(0), &nDestSize);
			if (SUCCEEDED(hr)) {
				hr = spMultiLang->ConvertStringToUnicode(&dwMode, Encoding[0].nCodePage, (char*)src.data(), &nSrcSize, str.GetBuffer(nDestSize), &nDestSize);
				str.ReleaseBuffer();
				return str;
			}
		}
	}
	str = src.data();
	return str;
}

//---------------------------------------------------
/// str��Shift-JIS������Ȃ�true��Ԃ�
bool	IsShiftJIS(LPCSTR str, int nCount)
{
	UINT nCodePage = CP_SJIS;
	CComPtr<IMultiLanguage2>	spMultiLang;
	HRESULT hr = spMultiLang.CoCreateInstance(CLSID_CMultiLanguage);
	if (SUCCEEDED(hr)) {
		int Scores = 5; // �擾������̐�
		DetectEncodingInfo Encoding[5] = { 0 };
		int nSize = nCount;
		hr = spMultiLang->DetectInputCodepage(MLDETECTCP_DBCS, 0, const_cast<LPSTR>(str), &nSize, Encoding, &Scores);
		if (SUCCEEDED(hr)) {
			if (Encoding[0].nCodePage == CP_SJIS)
				return true;
		}
	}
	return false;
}


// ==========================================================================

//+++ �t�@�C���p�X�����A�t�@�C�������擾
TCHAR *GetFileBaseName(const TCHAR* adr)
{
	const TCHAR *p;
	for (p = adr; *p != _T('\0'); ++p) {
		if (*p == _T(':') || *p == _T('/') || *p == _T('\\')) {
			adr = p + 1;
		}
	  #ifndef UNICODE
		if (_ismbblead((*(unsigned char *)p)) && *(p+1) )
			++p;
	  #endif
	}
	return (TCHAR*)adr;
}


//+++ �t�@�C���p�X�����A�t�@�C�������擾
const CString	GetFileBaseName(const CString& strFileName)
{
  #if 1
	return GetFileBaseName(LPCTSTR(strFileName));
  #else
	int 	n	= strFileName.ReverseFind( _T('\\') );
	int 	m	= strFileName.ReverseFind( _T('/') );
	if (n < 0 && m < 0) {
		return strFileName;
	}
	n = std::max(n, m);
	return	strFileName.Mid(n + 1);
  #endif
}


//+++ �t�@�C���p�X�����A�f�B���N�g�������擾. �Ō��'\\'�͊܂܂Ȃ�.
const CString	GetDirName(const CString& strFileName)
{
	int 	n	= strFileName.ReverseFind( _T('\\') );
	if (n < 0)
		return _T(".");	//strFileName;
	return	strFileName.Left(n);
}



///+++ �t�@�C�����̊g���q�̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
TCHAR *GetFileExt(const TCHAR *name)
{
	name			= GetFileBaseName(name);
	const TCHAR*  p = _tcsrchr(name, _T('.'));
	if (p) {
		return (TCHAR*)(p+1);
	}
	return (TCHAR*)name + _tcslen(name);
}



///+++ �t�@�C�����̊g���q�̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileExt(const CString& strFileName)
{
  #if 1
	return GetFileExt(LPCTSTR(strFileName));
  #else
	const CString strBaseName = GetFileBaseName(strFileName);
	int 	n	= strBaseName.ReverseFind( _T('.') );
	if (n < 0)
		return CString();
	return strBaseName.Mid(n + 1);
  #endif
}


///+++ �t�H���_���g���q�����̃t�@�C�����̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileBaseNoExt(const CString& strFileName)
{
	const CString strBaseName = GetFileBaseName(strFileName);
	int 	n	= strBaseName.ReverseFind( _T('.') );
	if (n < 0)
		return strBaseName;
	return strBaseName.Left(n);
}



///+++ �g���q�����̃t�@�C�����̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileNameNoExt(const CString& strFileName)
{
  #if 1
	const TCHAR*	name	= LPCTSTR(strFileName);
	const TCHAR*	baseName= GetFileBaseName(name);
	const TCHAR*	p 		= _tcsrchr(baseName, _T('.'));
	if (p) {
		if (p == baseName)
			return CString();
		return CString(name, static_cast<int>(p - name));
	}
	return strFileName;
  #else
	const CString strBaseName = GetFileBaseName(strFileName);
	int 	n	= strBaseName.ReverseFind( _T('.') );
	if (n < 0)
		return strFileName;

	int 	nd	= strFileName.ReverseFind( _T('\\') );
	if (nd >= 0)
		return strFileName.Left(nd + 1) + strBaseName.Left(n);
	return strBaseName.Left(n);
  #endif
}


//+++ ttp://��h�𑫂�����A���[�̋󔒂��폜�����肷��(SearchBar.h�̊֐����番��������������)
void	StrToNormalUrl(CString& strUrl)
{
	//+++ ���E�̋󔒂��폜
	const TCHAR* spcs  = _T(" \t\r\n�@\"");
	strUrl.TrimLeft(spcs);
	strUrl.TrimRight(spcs);
	//
	int nFind = strUrl.Find(_T("p://"));
	if (nFind >= 0) {
		CString strProt = strUrl.Left(nFind + 4);
		if (strProt == _T("ttp://") || strProt == _T("tp://"))
			strUrl = _T("http://") + strUrl.Mid(nFind + 4);
	}
}


/// ���Ȃ��t�@�C���p�X�ɂ��ĕԂ�
int	GetUniqueFilePath(CString& filepath, int nStart /*= 1*/)
{
	if (::PathFileExists(filepath) == FALSE)
		return 0;

	CString basepath;
	CString ext;
	int nExt = filepath.ReverseFind(_T('.'));
	if (nExt != -1) {
		basepath = filepath.Left(nExt);
		ext		= filepath.Mid(nExt);
	} else {
		basepath = filepath;
	}
	for (;;) {
		filepath.Format(_T("%s_[%d]%s"), basepath, nStart, ext);
		if (::PathFileExists(filepath) == FALSE)
			return nStart;
		++nStart;
	}
}



// ==========================================================================

//bool IsExistFile(LPCTSTR fname)
//{
//	return ::GetFileAttributes(fname) != 0xFFFFFFFF;
//}


//+++	strFile�� .bak �������t�@�C���ɕϖ�. �Â� .bak ������΂���͍폜.
void	MoveToBackupFile(const CString& strFileName)
{
	CString 	strBakName = strFileName + _T(".bak");
	if (::PathFileExists(strFileName))
		::MoveFileEx(strFileName, strBakName, MOVEFILE_REPLACE_EXISTING);		// �����̃t�@�C�����o�b�N�A�b�v�t�@�C���ɂ���.
}

//	strFile�� .bak �������t�@�C���ɃR�s�[. �Â� .bak ������΂���͍폜.
void	CopyToBackupFile(const CString& strFileName)
{
	CString 	strBakName = strFileName + _T(".bak");
	if (::PathFileExists(strFileName)) 
		::CopyFile(strFileName, strBakName, FALSE);		// �����̃t�@�C�����o�b�N�A�b�v�t�@�C���ɂ���.
}


/// .exe�̂���f�B���N�g���̃t���p�X��Ԃ�. �Ō��'\\'���܂�.
//   �J�����g�f�B���N�g���̎擾.
const CString GetCurDirectory()
{
	TCHAR dir[MAX_PATH+2];
	dir[0] = 0;
	::GetCurrentDirectory(MAX_PATH, dir);
	return CString(dir);
}


//+++
int SetCurDirectory(const CString& strDir)
{
	return ::SetCurrentDirectory(LPCTSTR(strDir));
}



///+++ �蔲���ȃt���p�X��. ���͂̋�؂��'/'.
const CString MakeFullPath(const CString& strDefaultDir, const CString& strFileName)
{
	CString		dir = strDefaultDir;
	StrBkSl2Sl(dir);
	if (dir.Left(7) != _T("file://"))
		return strFileName;
	CString		fnm = dir.Mid(5);
	int         net = 1;
	if (fnm[2] == _T('/') && fnm[4] == _T(':')) {
		fnm = fnm.Mid(3);
		net = 0;
	}
	DWORD		atr = ::GetFileAttributes(LPCTSTR(fnm));
	if (atr == (DWORD)-1) {
		if (net == 0)
			return strFileName;
		// �l�b�g���[�N�ȃp�X��������΁A�f�B���N�g���������Ƃ�.

	} else if (atr & FILE_ATTRIBUTE_DIRECTORY) {
		;

	} else {
		// �f�B���N�g���łȂ��t�@�C���Ȃ�΁A�t�@�C�����ƂƂ��ăf�B���N�g�����擾
		int r = dir.ReverseFind(_T('/'));
		if (r >= 0)
			dir = dir.Left(r);
	}

	CString		src = strFileName;
	StrBkSl2Sl(src);
	const TCHAR* p	= LPCTSTR(src);
	if (p == 0 || *p == 0) {
		return dir;
	}
	unsigned	c  = *p;
	unsigned	c1 = p[1];
	//unsigned	c2 = 0;
	if (c == _T('/') || (c1 == _T(':') && ( p[2] == _T('/'))) ) {
		return strFileName; // �t���p�X���낤�ł��̂܂ܕԂ�.

	} else {
		int l = dir.GetLength();
		if (l > 0 && dir[l-1] != _T('/'))
			dir += _T('/');

		l			= src.GetLength();
		int   n     = 0;
		while (n < l && src[n]) {
			if (src[n] == '.' && n+1 < l) {
				if (src[n+1] == _T('/')) {
					n += 2;
				} else if (n+1 < l && src[n+1] == '.'
						&& (n+2 == l || (n+3 < l && src[n+2] == _T('/')))
				) {
					n += 2 + (n+2 < l);
					int r = dir.GetLength();
					if (r > 0 && dir[r-1] == _T('/'))
						dir = dir.Left(r-1);
					r = dir.ReverseFind(_T('/'));
					if (r >= 0)
						dir = dir.Left(r+1);
				} else {
					goto DIRECT;
				}
			} else {
		  DIRECT:
				while (n < l && src[n] != 0) {
					TCHAR c = src[n++];
					dir += c;
					if (c == _T('/'))
						break;
				}
			}
		}
	}
	return dir;
}



//+++
void 	StrBkSl2Sl(CString& str)
{
  #ifdef UNICODE
	str.Replace(_T('\\'), _T('/'));
  #else
	unsigned l = str.GetLength();
	TCHAR*	 s = (TCHAR*) LPCTSTR( str );
	for (unsigned i = 0; i < l; ++i) {
		int c = (unsigned char)s[i];
		if (c == 0)
			break;
		if (_ismbblead(c) && i+1 < l)
			++i;
		else if (c == '\\')
			s[i] = _T('/');
	}
  #endif
}



///+++ �e�L�X�g�t�@�C���𕶎���Ƃ��Ď擾.
const CString FileLoadTextString(LPCTSTR strFile)
{
	FILE *	fp = _tfopen(strFile, _T("rb"));
	if (fp == NULL)
		return CString(_T(""));
	size_t 	l = _filelength(_fileno(fp));
	if (l == 0)
		return CString(_T(""));
	if (l > 0x1000000)		//+++ �Ƃ肠�����A�K���� 16M�o�C�g�őł��~�߂ɂ��Ƃ�
		return CString(_T(""));
	//�蔲���� TCHAR �łȂ� char�̂܂܏���
	char*	buf = new char[ l + 4 ];
	memset(buf, 0, l+4);
	size_t r = fread(buf, 1, l, fp);
	fclose(fp);
	ATLASSERT(r == l);
	if (r > l)
		r = l;
	buf[r] = '\0';
	CString str( buf );
	delete[] buf;
	return str;
}



///+++ �w�肵���t�@�C�����̃t�@�C���T�C�Y��Ԃ�.
size_t	GetFileSize(const TCHAR* fname)
{
	size_t	l  = 0;
	FILE *	fp = _tfopen(fname, _T("rb"));
	if (fp) {
		l = _filelength(_fileno(fp));
		fclose(fp);
	}
	return l;
}



///+++ �t�@�C�������[�h.
void*	FileLoad(const TCHAR* fname, void* mem, size_t size)
{
	FILE *	fp = _tfopen(fname, _T("rb"));
	if (fp) {
		fread(mem, 1, size, fp);
		if (ferror(fp))
			mem = NULL;
		fclose(fp);
	}
	return mem;
}



// ==========================================================================

///+++ undonut.exe�̃t���p�X����Ԃ�.  (MtlGetModuleFileName�ƈꏏ������...)
const CString 	GetExeFileName()
{
	TCHAR	buf[MAX_PATH];
	buf[0] = 0;
	::GetModuleFileName(_Module.GetModuleInstance(), buf, MAX_PATH);
	return CString(buf);
}


///+++ exe(dll)�̂���t�H���_��Ԃ�. �Ō��'\\'���t��
const CString GetExeDirectory()
{
	CString str = GetExeFileName();
	int 	n	= str.ReverseFind( _T('\\') );
	return	str.Left(n+1);
}


///+++ exe(dll)�̂���t�H���_��Ԃ�. �Ō��'\\'���t���Ȃ�
const CString GetExeDirName()
{
	CString str = GetExeFileName();
	int 	n	= str.ReverseFind( _T('\\') );
	return	str.Left(n);
}


///+++ �蔲���ȃt���p�X��. �f�B���N�g���̎w�肪�Ȃ���΁Aundonut�t�H���_���ƂȂ�.
const CString GetFullPath_ForExe(const CString& strFileName)
{
	const TCHAR* p = LPCTSTR(strFileName);
	if (p == 0 || *p == 0) {
		return GetExeDirectory();
	}
	unsigned	c  = *p;
	unsigned	c1 = p[1];
	unsigned	c2 = 0;
	if (c == '\\' || c == '/' || (c1 == ':' && ((c2 = p[2]) == '\\' || c2 == '/')) ) {
		return strFileName; // �t���p�X���낤�ł��̂܂ܕԂ�.
	} else if (c1 == ':' || (c == '.' && (c1 == '\\' || c1 == '/' || (c1 == '.' && ((c2 = p[2]) == '\\' || c2 == '/'))) ) ) {
		// ���΃p�X�ۂ�������A�ʏ�̃t���p�X�������Ă�... �蔲���ŃJ�����g�𒲐�.
		CString sav = GetCurDirectory();
		SetCurDirectory( GetExeDirectory() );
		TCHAR	buf[ MAX_PATH + 4 ];
		buf[0]	= 0;	//+++
		::_tfullpath(buf, strFileName, MAX_PATH);
		SetCurDirectory( sav );
		return CString(buf);
	} else {	// �����łȂ��ꍇ��undonut�̃t�H���_���̎w��Ƃ��Ĉ���.
		return GetExeDirectory() + strFileName;
	}
}



// ==========================================================================

//-------------------------------------------
//+++ �G���[���O�o��.
void ErrorLogPrintf(const TCHAR* fmt, ...)
{
	FILE* fp = _tfopen(GetExeDirectory() + _T("errorlog.txt"), _T("a"));
	if (fp) {
		va_list app;
		va_start(app, fmt);
	  #if 1 //def UNICODE	//+++ unicode�΍�
		CString str;
		str.FormatV(fmt, app);
		fputs(& wcs_to_sjis(str)[0], fp);
	  #else
		_vftprintf(fp, fmt, app);
	  #endif
		va_end(app);
		fclose(fp);
	}
}


// ==========================================================================

//-----------------------------------------------------
/// Wow64(Win64�ł�32�r�b�g���[�h)�œ����Ă��邩�ǂ���
BOOL IsWow64()
{
	typedef BOOL (WINAPI   * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
	BOOL				bIsWow64		 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(GetModuleHandle( TEXT("kernel32") ), "IsWow64Process");
	if (fnIsWow64Process) {
		if ( !fnIsWow64Process(::GetCurrentProcess(), &bIsWow64) ) {
			;	// handle error
		}
	}

	return bIsWow64;
}


std::pair<RenderingMode, bool>	GetRenderingModeAndForce()
{
	CString exeFileName = Misc::GetFileBaseName(Misc::GetExeFileName());

	// http://msdn.microsoft.com/en-us/library/ee330730(v=vs.85).aspx#browser_emulation
	RenderingMode mode = kIE7mode;
	bool bForce = false;

	auto funcGetRenderingModeFromRegistory = [&] (HKEY hk) {
		static LPCTSTR BROWSEREMULATIONKEY	= _T("SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");
		ATL::CRegKey rk;
		LONG result = rk.Open(hk, BROWSEREMULATIONKEY, KEY_QUERY_VALUE);
		if (result == ERROR_SUCCESS) {
			DWORD dwMode = 0;
			rk.QueryDWORDValue(exeFileName, dwMode);
			switch (dwMode) {
			// IE7mode
			case 7000:
			default:
				mode = kIE7mode;
				break;

			// IE8mode
			case 8888:
				bForce = true;
			case 8000:
				mode = kIE8mode;
				break;

			// IE9mode
			case 9999:
				bForce = true;
			case 9000:
				mode = kIE9mode;
				break;

			// IE10mode
			case 10001:
				bForce = true;
			case 10000:			
				mode = kIE10mode;
				break;

			// IE11mode
			case 11001:
				bForce = true;
			case 11000:
				mode = kIE11mode;
			}
		}
	};

	funcGetRenderingModeFromRegistory(HKEY_LOCAL_MACHINE);
	funcGetRenderingModeFromRegistory(HKEY_CURRENT_USER);
	return std::make_pair(mode, bForce);
}


//+++
unsigned getIEMejourVersion()
{
	static unsigned ver = -1;
	if (ver == -1) {
		//���W�X�g������IE�̃o�[�W�������擾
		TCHAR			buf[MAX_PATH+2];
		DWORD			dwCount = MAX_PATH;
		Misc::CRegKey 	reg;

		memset(buf, 0, sizeof buf);
		LRESULT rc = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"), KEY_QUERY_VALUE );
		if (rc == ERROR_SUCCESS) {
			if (reg.QueryStringValue(_T("svcVersion"), buf, &dwCount) != ERROR_SUCCESS) {
				dwCount = MAX_PATH;
				rc = reg.QueryStringValue(_T("Version"), buf, &dwCount);
			}
			reg.Close();
		}

		if (rc != ERROR_SUCCESS) {
			rc = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector"), KEY_QUERY_VALUE );
			if (rc == ERROR_SUCCESS) {
				rc = reg.QueryStringValue(_T("IE"), buf, &dwCount);
				//DWORD dw = 0;
				//LRESULT rc = reg.QueryDWORDValue(_T("IE"), dw);
				//sprintf(buf, _T("%d"), dw);
				reg.Close();
			}
		}

		ver = _tcstol(buf, NULL, 10);
	}
	return ver;
}


bool	IsGpuRendering()
{
	static BOOL	bEnable = -1;
	if (bEnable == -1) {
		bEnable = FALSE;
		{
			CRegKey	rk;
			if (rk.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_GPU_RENDERING"), KEY_READ) == ERROR_SUCCESS) {
				CString strExe = GetFileBaseName(GetExeFileName());
				DWORD	dwValue = 0;
				if (rk.QueryDWORDValue(strExe, dwValue) == ERROR_SUCCESS)
					bEnable = dwValue != 0;
			}
		}
		{
			CRegKey	rk;
			if (rk.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_GPU_RENDERING"), KEY_READ) == ERROR_SUCCESS) {
				CString strExe = GetFileBaseName(GetExeFileName());
				DWORD	dwValue = 0;
				if (rk.QueryDWORDValue(strExe, dwValue) == ERROR_SUCCESS)
					bEnable = dwValue != 0;
			}
		}
	}
	return bEnable != 0;
}

bool	IsVistalater()
{
	OSVERSIONINFO osvi = { sizeof (OSVERSIONINFO) };
	::GetVersionEx(&osvi);
	return osvi.dwMajorVersion >= 6;
}

bool	Is10later()
{
	OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
	::GetVersionEx(&osvi);
	return osvi.dwMajorVersion >= 10;
}



/** +++ XP�ȍ~�Ŏg����Aos�q�[�v�擾�ŁA�t���O�����e�[�V���������Ȃ����郂�[�h�ւ̐ݒ�.
 *  �� �ꕔ w2k�ł�MS�̃p�b�`�ɂ���Ă͎g����炵��?
 *     ������Vista���ƁA���x�A0���A���Ă���̂ŁA�g���ĂȂ��͗lorz
 */
bool setHeapAllocLowFlagmentationMode()
{
	BOOL rc = 0;
  #if 1 //+++ w2k sp4���Ǝg���邩��?�Ȃ�ŁA���݃`�F�b�N����`�ɕύX.
	HINSTANCE hLib = ::LoadLibrary( _T( "kernel32.dll" ) );
	if ( hLib ) {
		typedef BOOL ( WINAPI *HeapSetInformation_t)( HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T );
		HeapSetInformation_t fnHeapSetInformation = reinterpret_cast< HeapSetInformation_t >( ::GetProcAddress( hLib, "HeapSetInformation" ) );
		if ( fnHeapSetInformation) {
			ULONG 				ulEnableLFH = 2;
			HANDLE hProcessHeap = GetProcessHeap();
			rc 	= fnHeapSetInformation(hProcessHeap, HeapCompatibilityInformation, &ulEnableLFH, sizeof(ulEnableLFH));
			ulEnableLFH = 2;
			HANDLE/*INT_PTR*/	hCrtHeap 	= (HANDLE)_get_heap_handle();
			BOOL rc2 = fnHeapSetInformation(hCrtHeap, HeapCompatibilityInformation, &ulEnableLFH, sizeof(ulEnableLFH));
			int er = ::GetLastError();
			rc &= rc2;
		}
		::FreeLibrary( hLib );
	}
  #else
	if (_CheckOsVersion_XPLater()) {
		ULONG 				ulEnableLFH = 2;
		HANDLE hProcessHeap = GetProcessHeap();
		rc 	= ::HeapSetInformation(hProcessHeap, HeapCompatibilityInformation, &ulEnableLFH, sizeof(ulEnableLFH));
		int er = ::GetLastError();
		ulEnableLFH = 2;
		HANDLE/*INT_PTR*/	hCrtHeap 	= (HANDLE)_get_heap_handle();
		rc 	&= HeapSetInformation(hCrtHeap, HeapCompatibilityInformation, &ulEnableLFH, sizeof(ulEnableLFH));
	}
  #endif
	return rc != 0;
}


/** +++
 * �q�[�v���R���p�N�g�ɂ���(?�炵��...)
 */
bool mallocHeapCompact()
{
  #if 1	//
	INT_PTR 	hCrtHeap 	= _get_heap_handle();
	SIZE_T		rc			= ::HeapCompact((PVOID)hCrtHeap, 0);
	HANDLE hProcessHeap		= GetProcessHeap();
	SIZE_T		rc2			= ::HeapCompact(hProcessHeap, 0);
	return (rc | rc2) > 0;
  #endif
	return false;
}

//------------------------------------------------------
CRect	GetMonitorWorkArea(HWND hWnd)
{
	HMONITOR hMoni = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO	moniInfo = { sizeof(MONITORINFO) };
	GetMonitorInfo(hMoni, &moniInfo);
	return moniInfo.rcWork;
}


}	// namespace Misc
