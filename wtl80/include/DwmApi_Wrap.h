/**
 *	@file	DwmApi_Wrap.h
 *	@brief	dwmapi wrapper for no "dwmapi.dll"
 *	@date	2008
 *	@note
 *		+++ dwmapi.dll がない環境で、機能しないけど、少なくともハングしないようにするためのラッパー.
 *
 *		+++ このファイル自体は好きに使ってください.
 */
#ifndef __DWMAPI_WRAP_H__
#define __DWMAPI_WRAP_H__

#pragma once


#include <dwmapi.h>

#if _WIN32_WINNT >= 0x0600	// (vista)

#define DwmApi_Wrap

#else	// _WIN_VER < 0x0600(vista)

#define WM_DWMCOMPOSITIONCHANGED			0x031E
#define WM_DWMNCRENDERINGCHANGED			0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED		0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE			0x0321

//namespace DwmApi_Wrap		//+++ やっぱり class版にする.
class DwmApi_Wrap {
	static HMODULE		s_hDwmApiDLL;
	static DwmApi_Wrap	s_dwmApi_wrap_init_;

	DwmApi_Wrap() {
		ATLASSERT(s_hDwmApiDLL == HMODULE(-1LL));
	}

	static HMODULE get_hDwmApiDLL() {
		if (s_hDwmApiDLL == HMODULE(-1LL))
			s_hDwmApiDLL = ::LoadLibrary(_T("dwmapi.dll"));
		return s_hDwmApiDLL;
	}

public:

	~DwmApi_Wrap() {
		if (s_hDwmApiDLL && s_hDwmApiDLL != HMODULE(-1LL) ) {
			::FreeLibrary(s_hDwmApiDLL);
			//s_hDwmApiDLL = HMODULE(-1LL);
		}
	}

	#undef  DWMAPI_WRAP_MAC
   #if 1
	#define DWMAPI_WRAP_MAC(RetType, name, arg, arg2)								\
		static RetType name arg {													\
			HMODULE	 hDwmApiDLL = get_hDwmApiDLL();									\
			if (hDwmApiDLL == 0)													\
				return 0;															\
			typedef RetType (WINAPI *fn_t_##name) arg;								\
			static fn_t_##name	fn_##name = 0;										\
			if (fn_##name == 0)														\
				fn_##name = (fn_t_##name)::GetProcAddress( hDwmApiDLL, #name );		\
			if (fn_##name)															\
				return fn_##name arg2;												\
			return 0;																\
		}
   #else	//
	#define DWMAPI_WRAP_MAC(RetType, name, arg, arg2)									\
		static RetType name arg {														\
			HMODULE	hThemeDLL = ::LoadLibrary(_T("dwmapi.dll"));						\
			if (hThemeDLL == 0)															\
				return 0;																\
			typedef RetType (WINAPI *fn_t_##name) arg;									\
			fn_t_##name fn_##name = (fn_t_##name)::GetProcAddress( hThemeDLL, #name );	\
			RetType rc = 0;																\
			if (fn_##name)																\
				rc = fn_##name arg2;													\
			if (hThemeDLL)																\
				::FreeLibrary(hThemeDLL);												\
			return rc;																	\
		}
   #endif

	DWMAPI_WRAP_MAC(	BOOL	, DwmDefWindowProc					, (HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam,LRESULT *plResult)	, (hWnd,msg,wParam,lParam,plResult)				)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmEnableBlurBehindWindow			, (HWND hWnd, const DWM_BLURBEHIND* pBlurBehind)						, (hWnd, pBlurBehind)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmEnableComposition				, (UINT uCompositionAction)												, (uCompositionAction)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmEnableMMCSS					, (BOOL fEnableMMCSS)													, (fEnableMMCSS)								)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmExtendFrameIntoClientArea		, (HWND hWnd,const MARGINS* pMarInset)									, (hWnd,pMarInset)								)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetColorizationColor			, (DWORD* pcrColorization,BOOL* pfOpaqueBlend)							, (pcrColorization,pfOpaqueBlend)				)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetCompositionTimingInfo		, (HWND hwnd,DWM_TIMING_INFO* pTimingInfo)								, (hwnd,pTimingInfo)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetWindowAttribute				, (HWND hwnd,DWORD dwAttribute,PVOID pvAttribute, DWORD cbAttribute)	, (hwnd,dwAttribute,pvAttribute, cbAttribute)	)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmIsCompositionEnabled			, (BOOL* pfEnabled)														, (pfEnabled)									)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmModifyPreviousDxFrameDuration	, (HWND hwnd, INT cRefreshes,BOOL fRelative)							, (hwnd, cRefreshes,fRelative)					)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmQueryThumbnailSourceSize		, (HTHUMBNAIL hThumbnail, PSIZE pSize)									, (hThumbnail, pSize)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmRegisterThumbnail				, (HWND hwndDestination, HWND hwndSource, PHTHUMBNAIL phThumbnailId)	, (hwndDestination, hwndSource, phThumbnailId)	)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmSetDxFrameDuration				, (HWND hwnd, INT cRefreshes)											, (hwnd, cRefreshes)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmSetPresentParameters			, (HWND hwnd,DWM_PRESENT_PARAMETERS* pPresentParams)					, (hwnd,pPresentParams)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmSetWindowAttribute				, (HWND hwnd,DWORD dwAttribute,LPCVOID pvAttribute, DWORD cbAttribute)	, (hwnd,dwAttribute,pvAttribute, cbAttribute)	)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmUnregisterThumbnail			, (HTHUMBNAIL hThumbnailId)												, (hThumbnailId)								)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmUpdateThumbnailProperties		, (HTHUMBNAIL hThumbnailId, DWM_THUMBNAIL_PROPERTIES* ptnProperties)	, (hThumbnailId, ptnProperties)					)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmAttachMilContent				, (HWND hwnd)															, (hwnd)										)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmDetachMilContent				, (HWND hwnd)															, (hwnd)										)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmFlush							, ()																	, ()											)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetGraphicsStreamTransformHint	, (UINT uIndex,MIL_MATRIX3X2D *pTransform)								, (uIndex,pTransform)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetGraphicsStreamClient		, (UINT uIndex,UUID *pClientUuid)										, (uIndex,pClientUuid)							)
	DWMAPI_WRAP_MAC(	HRESULT	, DwmGetTransportAttributes			, (BOOL *pfIsRemoting,BOOL *pfIsConnected,DWORD *pDwGeneration)			, (pfIsRemoting,pfIsConnected,pDwGeneration)	)

	#undef DWMAPI_WRAP_MAC

};	// DwmApi_Wrap

__declspec(selectany) HMODULE		DwmApi_Wrap::s_hDwmApiDLL	= HMODULE(-1LL);
__declspec(selectany) DwmApi_Wrap	DwmApi_Wrap::s_dwmApi_wrap_init_;

#endif	// _WIN_VER


#endif	// __DWMAPI_WRAP_H__
