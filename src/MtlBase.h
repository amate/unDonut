/**
 *	@file	MtlBase.h
 *	@brief	MTL ： 基本
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlBase.h: Last updated: March 17, 2001
////////////////////////////////////////////////////////////////////////////

#ifndef __MTLBASE_H__
#define __MTLBASE_H__

#pragma once

#include <functional>


extern CServerAppModule 	_Module;


namespace MTL {


////////////////////////////////////////////////////////////////////////////
// ATL forgot
#ifdef _DEBUG
	#define MTLVERIFY(f)	if ( !(f) ) ATLASSERT(FALSE)
#else
	#define MTLVERIFY(f)	( (void) (f) )
#endif


//#define USES_PT		T	*pT = static_cast<T *>(this)


// for ATL array
#ifndef _countof
	#define _countof(array) 	( sizeof (array) / sizeof (array[0]) )
#endif

#define _begin(x)		(x.GetSize() == 0 ? NULL : &x[0])
#define _end(x) 		(x.GetSize() == 0 ? NULL : &x[0] + x.GetSize())



template <class T>
inline BOOL MtlSetSimpleArraySize(CSimpleArray<T> &arrSrc, int nSize)
{
	if (nSize > arrSrc.m_nAllocSize) {
		T * aT;
		int nNewAllocSize = nSize;
		aT					= (T *) ::realloc( arrSrc.m_aT, nNewAllocSize * sizeof (T) );

		if (aT == NULL)
			return FALSE;

		arrSrc.m_nAllocSize = nNewAllocSize;
		arrSrc.m_aT 		= aT;
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// Tips
template <class _Type1, class _Type2>
inline bool _check_flag(_Type1 __flag, _Type2 __flags)
{
	return (__flags & __flag) != 0;
}


template <class _VersionalBySize>
class CVersional : public _VersionalBySize {
public:
	CVersional()
	{
		memset(this, 0, sizeof(_VersionalBySize));	//+++ 仮想関数のいない構造体のみの指定だろう、で.
		cbSize = sizeof (_VersionalBySize);
	}
};


template <>
class CVersional<WINDOWPLACEMENT> : public WINDOWPLACEMENT {
public:
	CVersional()
	{
		memset(this, 0, sizeof(WINDOWPLACEMENT));	//+++ 仮想関数のいない構造体のみの指定だろう、で.
		length = sizeof (WINDOWPLACEMENT);
	}
};


struct CLockStaticDataInit {
	CLockStaticDataInit() { ::EnterCriticalSection( &_Module.m_csStaticDataInit); }

	~CLockStaticDataInit() { ::LeaveCriticalSection(&_Module.m_csStaticDataInit); }
};


struct CLockWindowCreate {
	CLockWindowCreate() { ::EnterCriticalSection( &_Module.m_csWindowCreate); }

	~CLockWindowCreate() { ::LeaveCriticalSection(&_Module.m_csWindowCreate); }
};


// Menu animation flags
#ifndef TPM_VERPOSANIMATION
	#define TPM_VERPOSANIMATION 	0x1000L
#endif
#ifndef TPM_NOANIMATION
	#define TPM_NOANIMATION 		0x4000L
#endif




/////////////////////////////////////////////////////////////////////////////
// Alternates std::bind1st(std::mem_fun(...), this)
// 1. HP's STL doesn't have std::mem_fun.
// 2. All the STL don't have the mem_fun1_t specialization to avoid "reference to reference".


template <class _Ret, class _Tp, class _Arg>
class __bind_mem_fun_t : public std::unary_function<_Arg, _Ret> {
public:
	explicit __bind_mem_fun_t(_Ret (_Tp::*__pf)(_Arg), _Tp *__po) : _M_f(__pf), _M_o(__po) { }
	_Ret operator ()(_Arg __x) { return (_M_o->*_M_f)(__x); }

private:
	_Ret (_Tp::* _M_f)(_Arg);
	_Tp *_M_o;
};


template <class _Ret, class _Tp, class _Arg>
inline __bind_mem_fun_t<_Ret, _Tp, _Arg> Mtl_bind_mem_fun(_Ret (_Tp::*__pf)(_Arg), _Tp *__po)
{
	return __bind_mem_fun_t<_Ret, _Tp, _Arg>(__pf, __po);
}


template <class _Tp, class _Arg>
class __bind_mem_fun_t_void : public std::unary_function<_Arg, void> {
public:
	explicit __bind_mem_fun_t_void(void (_Tp::*__pf)(_Arg), _Tp *__po) : _M_f(__pf), _M_o(__po) { }
	void operator ()(_Arg __x) { (_M_o->*_M_f)(__x); }

private:
	void (_Tp::* _M_f)(_Arg);
	_Tp *_M_o;
};


template <class _Tp, class _Arg>
inline __bind_mem_fun_t_void<_Tp, _Arg> Mtl_bind_mem_fun_void(void (_Tp::*__pf)(_Arg), _Tp *__po)
{
	return __bind_mem_fun_t_void<_Tp, _Arg>(__pf, __po);
}



////////////////////////////////////////////////////////////////////////////


}		// namespace MTL;



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE


#endif	// __MTLBASE_H__
