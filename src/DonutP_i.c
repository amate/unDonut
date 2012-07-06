

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0594 */
/* at Thu Jul 05 11:25:38 2012
 */
/* Compiler settings for DonutP.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0594 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IAPI,0x1413C5B5,0x0352,0x47AA,0x85,0xC6,0x92,0x19,0xF2,0x43,0xF5,0xE8);


MIDL_DEFINE_GUID(IID, IID_IAPI2,0xA7B5128A,0xE147,0x41d2,0x81,0x55,0x4B,0x9B,0xAE,0x44,0x9D,0xF4);


MIDL_DEFINE_GUID(IID, IID_IAPI3,0x738A9354,0xD4E7,0x4dd9,0x86,0x1B,0x15,0xD7,0x9F,0x2C,0x46,0xF5);


MIDL_DEFINE_GUID(IID, IID_IAPI4,0xB437832F,0xA3A8,0x4948,0x9D,0x42,0x64,0xD1,0xB9,0x1C,0x60,0x4E);


MIDL_DEFINE_GUID(IID, LIBID_DONUTPLib,0x2BCC2287,0xAAEF,0x4F0E,0xA8,0x6B,0xC2,0x06,0xEC,0xAB,0xF4,0x4D);


MIDL_DEFINE_GUID(IID, DIID_IDonutPEvents,0xCFF8CB23,0x2580,0x4BE4,0xBB,0x49,0x4A,0x2C,0x1B,0x64,0x51,0x64);


MIDL_DEFINE_GUID(CLSID, CLSID_API,0xDFA77729,0x9333,0x4E33,0x8D,0x52,0x7D,0x46,0x84,0xCF,0x98,0x67);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



