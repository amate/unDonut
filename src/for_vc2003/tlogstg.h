/**
 * @file tlogstg.h
 * @note
 * http://lamoo.s53.xrea.com/develop/wtl/wtl.html
 * で配布されていたwtl7.0修正版・セットに入っているtlogstg.h を抜き出して、
 * enum型チェックを緩めてint化したものです。(+++)
 */
// tlogstg.h
#pragma once

// なぜか tlogstg.h がどこにも見当たらない（Googleでも！）ため、自作。
// しかも、MSDN の インタフェースの説明は、vtable 順に並んでいない！
// 参考：http://www.euromind.com/iedelphi/ietravellog.htm
// まさか Delphi から逆輸入するとは思わなかった……。

enum TLENUMF
{
	TLEF_RELATIVE_INCLUDE_CURRENT = 0x00000001,
	TLEF_RELATIVE_BACK            = 0x00000010,
	TLEF_RELATIVE_FORE            = 0x00000020,
	TLEF_INCLUDE_UNINVOKEABLE     = 0x00000040,
	TLEF_ABSOLUTE                 = 0x00000031,
};

[ uuid("7EBFDD87-AD18-11d3-A4C5-00C04F72D6B8") ]
__interface ITravelLogEntry : IUnknown
{
	STDMETHOD(GetTitle)(LPOLESTR* ppTitle);
	STDMETHOD(GetURL)(LPOLESTR* ppURL);
};

const GUID IID_ITravelLogEntry = __uuidof(ITravelLogEntry);

[ uuid("7EBFDD85-AD18-11d3-A4C5-00C04F72D6B8") ]
__interface IEnumTravelLogEntry : IUnknown
{
	STDMETHOD(Next)(ULONG num, ITravelLogEntry** ppEntry, ULONG* fetched);
	STDMETHOD(Skip)(ULONG num);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumTravelLogEntry** ppClone);
};

const GUID IID_IEnumTravelLogEntry = __uuidof(IEnumTravelLogEntry);

[ uuid("7EBFDD80-AD18-11d3-A4C5-00C04F72D6B8") ]
__interface ITravelLogStg : IUnknown
{
	STDMETHOD(CreateEntry)(LPCOLESTR pszUrl, LPCOLESTR pszTitle, ITravelLogEntry* ptleRelativeTo, BOOL fPrepend, ITravelLogEntry** pptle);
	STDMETHOD(TravelTo)(ITravelLogEntry* ptle);
	STDMETHOD(EnumEntries)(int/*TLENUMF*/ flags, IEnumTravelLogEntry** ppEnum);
	STDMETHOD(FindEntries)(DWORD flags, LPOLESTR* ppURL, IEnumTravelLogEntry** ppEnum);
	STDMETHOD(GetCount)(int/*TLENUMF*/ flags, DWORD* pcEntries);
	STDMETHOD(RemoveEntry)(ITravelLogEntry* ptle);
	STDMETHOD(GetRelativeEntry)(int iOffset, ITravelLogEntry** ptle);
};

const GUID IID_ITravelLogStg    = __uuidof(ITravelLogStg);
const GUID SID_STravelLogCursor = __uuidof(ITravelLogStg);
