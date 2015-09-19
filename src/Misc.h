/**
 *	@file	Misc.h
 *	@biref	undonut�Ɉˑ����Ȃ����Ɣėp�I�ȎG���ȃ��[�`���Q
 */
#ifndef MISC_H
#define MISC_H

#pragma once

#include <stdarg.h>
#include <io.h>
#include <vector>
//#include <atlmisc.h>

namespace Misc {


// ==========================================================================

///+++ printf�����w��ŕ���������
const CString StrFmt(const TCHAR* fmt, ...);
const CString StrVFmt(const TCHAR* fmt, va_list app);

///+++ �󔒂ŋ�؂�ꂽ�ŏ��̕������Ԃ�.
const CString GetStrWord(const TCHAR* str, const TCHAR** ppNext=0);

///+++ �󔒂ŋ�؂�ꂽ�����P��𕪉�����
void SeptTextToWords(std::vector<CString>& strs, const TCHAR* line);

///+++ ����������s�����ŕ���. (��s�͖���)
void SeptTextToLines(std::vector<CString>& strs, const TCHAR* text);


///+++ �ȈՂȁA���p�������Z�ł̕��������ǂ��J�E���g�B
unsigned	eseHankakuStrLen(const TCHAR* s);

///+++ ���񔼊p�w��Ŏw�肳�ꂽ�������܂ł̕������Ԃ�.
const CString eseHankakuStrLeft(const CString& str, int len);


// ==========================================================================
// +++ �����R�[�h�ϊ�.

const std::vector<char> 	sjis_to_sjis (const char*    pSjis);	//+++
const std::vector<wchar_t> 	wcs_to_wcs   (const wchar_t* pWcs );	//+++

const std::vector<char> 	wcs_to_sjis  (const wchar_t* pWcs);		//+++
const std::vector<wchar_t> 	sjis_to_wcs  (const char*    pSjis);	//+++

const std::vector<char> 	sjis_to_utf8 (const char*    pSjis);	//+++
const std::vector<char> 	utf8_to_sjis (const char*    pUtf8);	//+++

const std::vector<char> 	wcs_to_utf8  (const wchar_t* pWcs);		//+++
const std::vector<wchar_t> 	utf8_to_wcs  (const char*    pUtf8);	//+++

const std::vector<char> 	sjis_to_eucjp(const char*    pSjis);	//+++
const std::vector<char> 	eucjp_to_sjis(const char*    pEucjp);	//+++

const std::vector<char> 	wcs_to_eucjp (const wchar_t* pWcs);		//+++
const std::vector<wchar_t> 	eucjp_to_wcs (const char*    pEucjp);	//+++

//+++
#ifdef UNICODE	// Unicode�����x�[�X�ŃR���p�C�������ꍇ
__inline const std::vector<char>    tcs_to_sjis (const TCHAR*   pTcs  ) { return wcs_to_sjis (pTcs); }
__inline const std::vector<char>    tcs_to_utf8 (const TCHAR*   pTcs  ) { return wcs_to_utf8 (pTcs); }
__inline const std::vector<char>    tcs_to_eucjp(const TCHAR*   pTcs  ) { return wcs_to_eucjp(pTcs); }
__inline const std::vector<wchar_t> tcs_to_wcs  (const TCHAR*   pTcs  ) { return wcs_to_wcs  (pTcs); }

__inline const std::vector<TCHAR>   sjis_to_tcs (const char*    pSjis ) { return sjis_to_wcs (pSjis ); }
__inline const std::vector<TCHAR>   utf8_to_tcs (const char*    pUtf8 ) { return utf8_to_wcs (pUtf8 ); }
__inline const std::vector<TCHAR>   eucjp_to_tcs(const char*    pEucjp) { return eucjp_to_wcs(pEucjp); }
__inline const std::vector<TCHAR>   wcs_to_tcs  (const wchar_t* pWcs  ) { return wcs_to_wcs  (pWcs); }
#else			// MultiByte(sjis)�x�[�X�ŃR���p�C�������ꍇ.
__inline const std::vector<char>    tcs_to_sjis (const TCHAR*   pTcs  ) { return sjis_to_sjis (pTcs); }
__inline const std::vector<char>    tcs_to_eucjp(const TCHAR*   pTcs  ) { return sjis_to_eucjp(pTcs); }
__inline const std::vector<char>    tcs_to_utf8 (const TCHAR*   pTcs  ) { return sjis_to_utf8 (pTcs); }
__inline const std::vector<wchar_t> tcs_to_wcs  (const TCHAR*   pTcs  ) { return sjis_to_wcs  (pTcs); }

__inline const std::vector<TCHAR>   sjis_to_tcs (const char*    pSjis ) { return sjis_to_sjis (pSjis ); }
__inline const std::vector<TCHAR>   utf8_to_tcs (const char*    pUtf8 ) { return utf8_to_sjis (pUtf8 ); }
__inline const std::vector<TCHAR>   eucjp_to_tcs(const char*    pEucjp) { return eucjp_to_sjis(pEucjp); }
__inline const std::vector<TCHAR>   wcs_to_tcs  (const wchar_t* pWcs  ) { return wcs_to_sjis  (pWcs  ); }
#endif

__inline const CString sjis_to_CString(const char* pSjis)   { return CString( pSjis ); }
__inline const CString utf8_to_CString(const char* pUtf8)   { return CString(& utf8_to_tcs (pUtf8 )[0]); }
__inline const CString eucjp_to_CString(const char* pEucjp) { return CString(& eucjp_to_tcs(pEucjp)[0]); }

__inline const CString sjis_to_CString(const std::vector<char>& sjis)   { return CString( &sjis[0] ); }
__inline const CString utf8_to_CString(const std::vector<char>& utf8)   { return CString(& utf8_to_tcs (&utf8[0])[0]); }
__inline const CString eucjp_to_CString(const std::vector<char>& eucjp) { return CString(& eucjp_to_tcs(&eucjp[0])[0]); }


CString	UnknownToCString(const std::vector<char>& src);

//---------------------------------------------------
/// str��Shift-JIS(CF������Ȃ�true��Ԃ�
bool	IsShiftJIS(LPCSTR str, int nCount);


// ==========================================================================

///+++ %20 �����g��ꂽurl��ʏ�̕�����ɕϊ�.
const std::vector<char>		urlstr_decode(const TCHAR* url);
const CString				urlstr_decodeJpn(const TCHAR* url, int dfltCode=0);

///+++ �ʏ�̕������ %20 �����g��ꂽurl�ɕϊ�.
const CString				urlstr_encode(const char* str);
__inline const CString		urlstr_encode(const std::vector<char>& str) { return urlstr_encode(&str[0]); }

///+++ �����񒆂ɁASJIS�ɕϊ��ł��Ȃ�UNICODE�������������Ă����ꍇ�A������S�̂�urlstr�ɕϊ�����.
const CString 				urlstr_encodeWhenNeedUnicode(const CString& str);

///+++ �����񂪔��p�p���L���݂̂ł���%���������Ă���ꍇ�Autf8���G���R�[�h�������̂Ƃ��ăf�R�[�h����.
const CString 				urlstr_decodeWhenASC(const CString& str);

///+++ UNICODE������������Ȃ�SJIS�ɕϊ��ł��邩?
bool IsEnableUnicodeToSJIS(LPCWSTR pStr);

// ==========================================================================

//+++ �t�@�C���p�X�����A�t�@�C�������擾
const CString	GetFileBaseName(const CString& strFileName);

//+++ �t�@�C���p�X�����A�f�B���N�g�������擾. �Ō��'\\'�͊܂܂Ȃ�.
const CString	GetDirName(const CString& strFileName);

///+++ �t�@�C�����̊g���q�̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileExt(const CString& strFileName);

///+++ �t�H���_���g���q�����̃t�@�C�����̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileBaseNoExt(const CString& strFileName);

///+++ �g���q�����̃t�@�C�����̎擾. �� ���ʂ̕�����ɂ�'.'�͊܂܂�Ȃ�.
const CString	GetFileNameNoExt(const CString& strFileName);

//+++ ttp://��h�𑫂�����A���[�̋󔒂��폜�����肷��(SearchBar.h�̊֐����番��������������)
void	StrToNormalUrl(CString& strUrl);

/// ���Ȃ��t�@�C���p�X�ɂ��ĕԂ�
int	GetUniqueFilePath(CString& filepath, int nStart = 1);


// ==========================================================================

inline bool IsExistFile(LPCTSTR fname) { return ::GetFileAttributes(fname) != 0xFFFFFFFF; }

//+++	strFile�� .bak �������t�@�C���ɕϖ�. �Â� .bak ������΂���͍폜.
void	MoveToBackupFile(const CString& strFileName);

//	strFile�� .bak �������t�@�C���ɃR�s�[. �Â� .bak ������΂���͍폜.
void	CopyToBackupFile(const CString& strFileName);

///+++ �J�����g�f�B���N�g���̎擾.
const CString GetCurDirectory();

//+++
int SetCurDirectory(const CString& strDir);


///+++ �蔲���ȃt���p�X��. ���͂̋�؂��'/'.
const CString MakeFullPath(const CString& strDefaultDir, const CString& strFileName);

///+++
void 	StrBkSl2Sl(CString& str);

///+++ �e�L�X�g�t�@�C���𕶎���Ƃ��Ď擾.
const CString FileLoadTextString(LPCTSTR strFile);

///+++ �t�@�C���T�C�Y��Ԃ�.
size_t			GetFileSize(const TCHAR* fname);

///+++ �t�@�C���̃��[�h.
void*			FileLoad(const TCHAR* fname, void* mem, size_t size);

///+++ �t�@�C���̃��[�h.
template<class CHAR_VECTOR>
size_t	FileLoad(const TCHAR* fname, CHAR_VECTOR& vec) {
	enum { CHAR_SIZE = sizeof(vec[0]) };
	vec.clear();
	size_t	l = GetFileSize(fname);
	if (l > 0x1000000)		//+++ �Ƃ肠�����A�K���� 16M�o�C�g�őł��~�߂ɂ��Ƃ�
		l = 0x1000000;
	size_t	n = l / CHAR_SIZE;
	if (n > 0) {
		vec.resize(n);
		FileLoad(fname, &vec[0], n * CHAR_SIZE);
	}
	return n;
}



// ==========================================================================

///+++ undonut.exe�̃t���p�X����Ԃ�.  (MtlGetModuleFileName�ƈꏏ������...)
const CString 	GetExeFileName();

///+++ exe(dll)�̂���t�H���_��Ԃ�. �Ō��'\\'���t��
const CString 	GetExeDirectory();

///+++ exe(dll)�̂���t�H���_��Ԃ�. �Ō��'\\'���t���Ȃ�
const CString 	GetExeDirName();

//+++ .manifest�̑��݃`�F�b�N	�� CThemeDLLLoader ����Ɨ� & ������ƃ��l�[��
bool  			IsExistManifestFile();

///+++ �蔲���ȃt���p�X��. �f�B���N�g���̎w�肪�Ȃ���΁Aundonut�t�H���_���ƂȂ�.
const CString 	GetFullPath_ForExe(const CString& strFileName);



// ==========================================================================

//+++ �G���[���O�o��.
void ErrorLogPrintf(const TCHAR* fmt, ...);


// ==========================================================================

BOOL	IsWow64(); 					//+++

enum RenderingMode { 
	kIE7mode = 7,
	kIE8mode = 8,
	kIE9mode = 9,
	kIE10mode = 10,
	kIE11mode = 11,
};

std::pair<RenderingMode, bool>	GetRenderingModeAndForce();

unsigned getIEMejourVersion();		//+++
bool	IsGpuRendering();
bool	IsVistalater();
bool	Is10later();


// ==========================================================================

#if _ATL_VER >= 0x700
typedef ATL::CRegKey  CRegKey;
#else	//+++ wtl7�ȍ~ �d�l���ς�����悤�Ȃ̂ŁAatl3�̏ꍇ�̓��b�v���Ď����Ă���.
class CRegKey : public ATL::CRegKey {
public:
	CRegKey() throw() {;}
	// CRegKey( CRegKey& key ) throw() : ATL::CRegKey(key) { ; }
	// explicit CRegKey(HKEY hKey) throw() : ATL::CRegKey(hKey) {;}

	//+++	ATL3�n�ŃR���p�C���ł���悤�ɂ��邽�߂̃��b�p�[�֐�
	HRESULT 	QueryStringValue(const TCHAR* key, TCHAR* dst, DWORD* pCount) { return this->QueryValue(dst, key, pCount); }
	HRESULT 	SetStringValue(const TCHAR* value, const TCHAR* key) { return this->SetValue(value ? value : _T(""), key); }
	HRESULT 	QueryDWORDValue(const TCHAR* key, DWORD& value) { return this->QueryValue(value, key); }
	HRESULT 	SetDWORDValue(const TCHAR* key, DWORD value) { return this->SetValue(value, key); }
};
#endif


bool setHeapAllocLowFlagmentationMode();
bool mallocHeapCompact();

//------------------------------------------------------
CRect	GetMonitorWorkArea(HWND hWnd);


}	// namespace Misc


//using namespace Misc;			//+++ �蔲��
using Misc::ErrorLogPrintf;		//+++ �f�o�b�O�p�Ȃ�ł���y��...

#endif
