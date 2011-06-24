/**
 *	@file	uxtheme_Wrap.h
 *	@brief	uxtheme api wrapper for no "uxtheme.dll"
 *	@date	2008
 *	@note
 *		+++ uxtheme.dll がない環境で、機能しないけど、少なくともハングしないようにするためのラッパー.
 *
 *		+++ このファイル自体は好きに使ってください.
 */
#ifndef __UXTHEME_WRAP_H__
#define __UXTHEME_WRAP_H__

#pragma once


#include <uxtheme.h>


#if _WIN_VER >= 0x0600	// (vista以降)

#define UxTheme_Wrap

#else	// _WIN_VER < 0x0600(xp)

#ifndef WM_THEMECHANGED
 #define WM_THEMECHANGED 	0x031A
#endif

#ifndef _WTL_NEW_UXTHEME
 #define _WTL_NEW_UXTHEME
#endif


// HPAINTBUFFER
typedef HANDLE HPAINTBUFFER;  // handle to a buffered paint context

// BP_BUFFERFORMAT
typedef enum _BP_BUFFERFORMAT
{
    BPBF_COMPATIBLEBITMAP,    // Compatible bitmap
    BPBF_DIB,                 // Device-independent bitmap
    BPBF_TOPDOWNDIB,          // Top-down device-independent bitmap
    BPBF_TOPDOWNMONODIB       // Top-down monochrome device-independent bitmap
} BP_BUFFERFORMAT;

#define BPBF_COMPOSITED BPBF_TOPDOWNDIB


// BP_ANIMATIONSTYLE
typedef enum _BP_ANIMATIONSTYLE
{
    BPAS_NONE,                // No animation
    BPAS_LINEAR,              // Linear fade animation
    BPAS_CUBIC,               // Cubic fade animation
    BPAS_SINE                 // Sinusoid fade animation
} BP_ANIMATIONSTYLE;


// BP_ANIMATIONPARAMS
typedef struct _BP_ANIMATIONPARAMS
{
    DWORD               cbSize;
    DWORD               dwFlags; // BPAF_ flags
    BP_ANIMATIONSTYLE   style;
    DWORD               dwDuration;
} BP_ANIMATIONPARAMS, *PBP_ANIMATIONPARAMS;

#define BPPF_ERASE               0x0001 // Empty the buffer during BeginBufferedPaint()
#define BPPF_NOCLIP              0x0002 // Don't apply the target DC's clip region to the double buffer
#define BPPF_NONCLIENT           0x0004 // Using a non-client DC

                                        
// BP_PAINTPARAMS
typedef struct _BP_PAINTPARAMS
{
    DWORD                       cbSize;
    DWORD                       dwFlags; // BPPF_ flags
    const RECT *                prcExclude;
    const BLENDFUNCTION *       pBlendFunction;
} BP_PAINTPARAMS, *PBP_PAINTPARAMS;

typedef HANDLE HANIMATIONBUFFER;  // handle to a buffered paint animation

enum WINDOWTHEMEATTRIBUTETYPE
{
    WTA_NONCLIENT = 1
};

typedef struct _WTA_OPTIONS
{
    DWORD dwFlags;          // values for each style option specified in the bitmask
    DWORD dwMask;           // bitmask for flags that are changing
                            // valid options are: WTNCA_NODRAWCAPTION, WTNCA_NODRAWICON, WTNCA_NOSYSMENU
} WTA_OPTIONS, *PWTA_OPTIONS;

#define WTNCA_NODRAWCAPTION       0x00000001    // don't draw the window caption
#define WTNCA_NODRAWICON          0x00000002    // don't draw the system icon
#define WTNCA_NOSYSMENU           0x00000004    // don't expose the system menu icon functionality
#define WTNCA_NOMIRRORHELP        0x00000008    // don't mirror the question mark, even in RTL layout
#define WTNCA_VALIDBITS           (WTNCA_NODRAWCAPTION | \
                                   WTNCA_NODRAWICON | \
                                   WTNCA_NOSYSMENU | \
                                   WTNCA_NOMIRRORHELP)

//namespace UxTheme_Wrap 	//+++ やっぱり、class版にしとく.
class UxTheme_Wrap {
	static HMODULE					s_hThemeDLL;
	static UxTheme_Wrap				s_uxtheme_wrap_init_;

	UxTheme_Wrap() {
		ATLASSERT(s_hThemeDLL == HMODULE(-1LL));
	}

	static HMODULE get_hThemeDLL() {
		if (s_hThemeDLL == HMODULE(-1LL))
			s_hThemeDLL = ::LoadLibrary(_T("uxtheme.dll"));
		return s_hThemeDLL;
	}

public:

	~UxTheme_Wrap() {
		if (s_hThemeDLL && s_hThemeDLL != HMODULE(-1LL)) {
			::FreeLibrary(s_hThemeDLL);
			//s_hThemeDLL = HMODULE(-1LL);
		}
	}

	#undef UXTHEME_WRAP_MAC
	#define UXTHEME_WRAP_MAC(RetType, name, arg, arg2)								\
		static RetType name arg {													\
			HMODULE 	hThemeDLL = get_hThemeDLL();								\
			if (hThemeDLL == 0)														\
				return 0;															\
			typedef RetType (WINAPI *fn_t_##name) arg;								\
			static fn_t_##name	fn_##name = 0;										\
			if (fn_##name == 0)														\
				fn_##name = (fn_t_##name)::GetProcAddress( hThemeDLL, #name );		\
			if (fn_##name)															\
				return fn_##name arg2;												\
			return 0;																\
		}

	UXTHEME_WRAP_MAC( HTHEME			, OpenThemeData, 				(HWND hwnd, LPCWSTR pszClassList), (hwnd, pszClassList))
	UXTHEME_WRAP_MAC( HTHEME			, OpenThemeDataEx, 				(HWND hwnd, LPCWSTR pszClassList, DWORD dwFlags), (hwnd, pszClassList, dwFlags))
	UXTHEME_WRAP_MAC( HRESULT 			, CloseThemeData, 				(HTHEME hTheme), (hTheme))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeBackground, 			(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect), (hTheme, hdc, iPartId, iStateId, pRect, pClipRect) )
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeBackgroundEx, 		(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS * pOptions), (hTheme, hdc, iPartId, iStateId, pRect, pOptions))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeText, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect), (hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeBackgroundContentRect,(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect), (hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeBackgroundExtent, 	(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect), (hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeBackgroundRegion, 	(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN * pRegion), (hTheme, hdc, iPartId, iStateId, pRect, pRegion))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemePartSize, 			(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE * psz), (hTheme, hdc, iPartId, iStateId, prc, eSize, psz))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeTextExtent, 			(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect), (hTheme, hdc, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeTextMetrics, 			(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, TEXTMETRICW * ptm), (hTheme, hdc, iPartId, iStateId, ptm))
	UXTHEME_WRAP_MAC( HRESULT 			, HitTestThemeBackground, 		(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD * pwHitTestCode), (hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeEdge, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect), (hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeIcon, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex), (hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex))
	UXTHEME_WRAP_MAC( BOOL				, IsThemePartDefined, 			(HTHEME hTheme, int iPartId, int iStateId), (hTheme, iPartId, iStateId))
	UXTHEME_WRAP_MAC( BOOL				, IsThemeBackgroundPartiallyTransparent, (HTHEME hTheme, int iPartId, int iStateId), (hTheme, iPartId, iStateId))
	UXTHEME_WRAP_MAC( HRESULT 	   		, GetThemeColor, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF * pColor), (hTheme, iPartId, iStateId, iPropId, pColor))
	UXTHEME_WRAP_MAC( HRESULT 	   		, GetThemeMetric, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal), (hTheme, hdc, iPartId, iStateId, iPropId, piVal))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeString, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars), (hTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeBool, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, BOOL * pfVal), (hTheme, iPartId, iStateId, iPropId, pfVal))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeInt, 					(HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal), (hTheme, iPartId, iStateId, iPropId, piVal))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeEnumValue, 			(HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal), (hTheme, iPartId, iStateId, iPropId, piVal))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemePosition, 			(HTHEME hTheme, int iPartId, int iStateId, int iPropId, POINT * pPoint), (hTheme, iPartId, iStateId, iPropId, pPoint))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeFont, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW * pFont), (hTheme, hdc, iPartId, iStateId, iPropId, pFont))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeRect, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPRECT pRect), (hTheme, iPartId, iStateId, iPropId, pRect))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeMargins, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS * pMargins), (hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeIntList, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, INTLIST * pIntList), (hTheme, iPartId, iStateId, iPropId, pIntList))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemePropertyOrigin, 		(HTHEME hTheme, int iPartId, int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin), (hTheme, iPartId, iStateId, iPropId, pOrigin))
	UXTHEME_WRAP_MAC( HRESULT 			, SetWindowTheme, 				(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList), (hwnd, pszSubAppName, pszSubIdList))
	UXTHEME_WRAP_MAC( HRESULT 			, SetWindowThemeAttribute, 		(HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute), (hwnd, eAttribute, pvAttribute, cbAttribute))
	UXTHEME_WRAP_MAC( HRESULT 	   		, GetThemeFilename, 			(HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars), (hTheme, iPartId, iStateId, iPropId, pszThemeFileName, cchMaxBuffChars))
	UXTHEME_WRAP_MAC( COLORREF			, GetThemeSysColor, 			(HTHEME hTheme, int iColorId), (hTheme, iColorId))
	UXTHEME_WRAP_MAC( HBRUSH			, GetThemeSysColorBrush, 		(HTHEME hTheme, int iColorId), (hTheme, iColorId))
	UXTHEME_WRAP_MAC( BOOL				, GetThemeSysBool, 				(HTHEME hTheme, int iBoolId), (hTheme, iBoolId))
	UXTHEME_WRAP_MAC( int			 	, GetThemeSysSize, 				(HTHEME hTheme, int iSizeId), (hTheme, iSizeId))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeSysFont, 				(HTHEME hTheme, int iFontId, LOGFONTW * plf), (hTheme, iFontId, plf))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeSysString, 			(HTHEME hTheme, int iStringId, LPWSTR pszStringBuff, int cchMaxStringChars), (hTheme, iStringId, pszStringBuff, cchMaxStringChars))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeSysInt, 				(HTHEME hTheme, int iIntId, int *piValue), (hTheme, iIntId, piValue))
	UXTHEME_WRAP_MAC( BOOL				, IsThemeActive, 				(), ())
	UXTHEME_WRAP_MAC( BOOL				, IsAppThemed, 					(), ())
	UXTHEME_WRAP_MAC( HTHEME			, GetWindowTheme, 				(HWND hwnd), (hwnd))
	UXTHEME_WRAP_MAC( HRESULT	    	, EnableThemeDialogTexture, 	(HWND hwnd, DWORD dwFlags), (hwnd, dwFlags))
	UXTHEME_WRAP_MAC( BOOL 				, IsThemeDialogTextureEnabled, 	(HWND hwnd), (hwnd))
	UXTHEME_WRAP_MAC( DWORD 			, GetThemeAppProperties, 		(), ())
	UXTHEME_WRAP_MAC( int/*void*/		, SetThemeAppProperties, 		(DWORD dwFlags), (dwFlags))
	UXTHEME_WRAP_MAC( HRESULT 			, GetCurrentThemeName, 			(LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars), (pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeDocumentationProperty, (LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars), (pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeParentBackground, 	(HWND hwnd, HDC hdc, const RECT * prc), (hwnd, hdc, prc))
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeParentBackgroundEx, 	(HWND hwnd, HDC hdc, DWORD dwFlags, const RECT * prc), (hwnd, hdc, dwFlags, prc))
	UXTHEME_WRAP_MAC( HRESULT 			, EnableTheming, 				(BOOL fEnable), (fEnable))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeBitmap, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, ULONG dwFlags, HBITMAP * phBitmap), (hTheme, iPartId, iStateId, iPropId, dwFlags, phBitmap))
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeStream, 				(HTHEME hTheme, int iPartId, int iStateId, int iPropId, VOID ** ppvStream, DWORD * pcbStream, HINSTANCE hInst), (hTheme, iPartId, iStateId, iPropId, ppvStream, pcbStream, hInst))
  #if 1 //_MSC_VER >= 1400	//+++ とりあえず手抜きで、VC2005以上で無い場合は生成なし
	UXTHEME_WRAP_MAC( HRESULT 			, DrawThemeTextEx, 				(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions), (hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions))
	UXTHEME_WRAP_MAC( HRESULT 			, BufferedPaintInit, 			(), ())
	UXTHEME_WRAP_MAC( HRESULT 			, BufferedPaintUnInit, 			(), ())
	UXTHEME_WRAP_MAC( HPAINTBUFFER		, BeginBufferedPaint, 			(HDC hdcTarget, const RECT * prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS * pPaintParams, HDC * phdc), (hdcTarget, prcTarget, dwFormat, pPaintParams, phdc))
	UXTHEME_WRAP_MAC( HRESULT    		, EndBufferedPaint, 			(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget), (hBufferedPaint, fUpdateTarget))
	UXTHEME_WRAP_MAC( HRESULT    		, GetBufferedPaintTargetRect, 	(HPAINTBUFFER hBufferedPaint, RECT * prc), (hBufferedPaint, prc))
	UXTHEME_WRAP_MAC( HDC			 	, GetBufferedPaintTargetDC, 	(HPAINTBUFFER hBufferedPaint), (hBufferedPaint))
	UXTHEME_WRAP_MAC( HDC 				, GetBufferedPaintDC, 			(HPAINTBUFFER hBufferedPaint), (hBufferedPaint))
	UXTHEME_WRAP_MAC( HRESULT    		, GetBufferedPaintBits, 		(HPAINTBUFFER hBufferedPaint, RGBQUAD ** ppbBuffer, int *pcxRow), (hBufferedPaint, ppbBuffer, pcxRow))
	UXTHEME_WRAP_MAC( HRESULT    		, BufferedPaintClear, 			(HPAINTBUFFER hBufferedPaint, const RECT * prc), (hBufferedPaint, prc))
	UXTHEME_WRAP_MAC( HRESULT    		, BufferedPaintSetAlpha, 		(HPAINTBUFFER hBufferedPaint, const RECT * prc, BYTE alpha), (hBufferedPaint, prc, alpha))
	UXTHEME_WRAP_MAC( HRESULT    		, BufferedPaintStopAllAnimations, (HWND hwnd), (hwnd))
	UXTHEME_WRAP_MAC( HANIMATIONBUFFER	, BeginBufferedAnimation, 		(HWND hwnd, HDC hdcTarget, const RECT * prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS * pPaintParams, BP_ANIMATIONPARAMS * pAnimationParams, HDC * phdcFrom, HDC * phdcTo), (hwnd, hdcTarget, prcTarget, dwFormat, pPaintParams, pAnimationParams,  phdcFrom, phdcTo))
	UXTHEME_WRAP_MAC( HRESULT    		, EndBufferedAnimation, 		(HANIMATIONBUFFER hbpAnimation, BOOL fUpdateTarget), (hbpAnimation, fUpdateTarget))
	UXTHEME_WRAP_MAC( BOOL				, BufferedPaintRenderAnimation, (HWND hwnd, HDC hdcTarget), (hwnd, hdcTarget))
  #endif
	UXTHEME_WRAP_MAC( BOOL				, IsCompositionActive, 			(), () )
	UXTHEME_WRAP_MAC( HRESULT 			, GetThemeTransitionDuration, 	(HTHEME hTheme, int iPartId, int iStateIdFrom, int iStateIdTo, int iPropId, DWORD * pdwDuration), (hTheme, iPartId, iStateIdFrom, iStateIdTo, iPropId, pdwDuration) )

	#undef UXTHEME_WRAP_MAC


  #if 1 //
	static __inline HRESULT SetWindowThemeNonClientAttributes(HWND hwnd, DWORD dwMask, DWORD dwAttributes) {
		WTA_OPTIONS 	wta;
		wta.dwFlags = dwAttributes,  wta.dwMask  = dwMask;
		return UxTheme_Wrap::SetWindowThemeAttribute(hwnd, WTA_NONCLIENT, (void *) &(wta), sizeof(wta));
	}
  #endif

};	// UxTheme_Wrap

__declspec(selectany) HMODULE		UxTheme_Wrap::s_hThemeDLL	= HMODULE(-1LL);
__declspec(selectany) UxTheme_Wrap	UxTheme_Wrap::s_uxtheme_wrap_init_;


#endif	// _WIN_VER

#endif	// __UXTHEME_WRAP_H__
