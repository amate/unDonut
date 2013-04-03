

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Apr 02 20:28:50 2013
 */
/* Compiler settings for DonutP.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "DonutP.h"

#define TYPE_FORMAT_STRING_SIZE   87                                
#define PROC_FORMAT_STRING_SIZE   1747                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _DonutP_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } DonutP_MIDL_TYPE_FORMAT_STRING;

typedef struct _DonutP_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } DonutP_MIDL_PROC_FORMAT_STRING;

typedef struct _DonutP_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } DonutP_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const DonutP_MIDL_TYPE_FORMAT_STRING DonutP__MIDL_TypeFormatString;
extern const DonutP_MIDL_PROC_FORMAT_STRING DonutP__MIDL_ProcFormatString;
extern const DonutP_MIDL_EXPR_FORMAT_STRING DonutP__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAPI_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAPI_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAPI2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAPI2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAPI3_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAPI3_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IAPI4_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IAPI4_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const DonutP_MIDL_PROC_FORMAT_STRING DonutP__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Close */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentObject */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 60 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 66 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 74 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindowObject */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x9 ),	/* 9 */
/* 86 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 94 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 102 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 104 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 108 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 110 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 112 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWebBrowserObject */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0xa ),	/* 10 */
/* 128 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 130 */	NdrFcShort( 0x8 ),	/* 8 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 136 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 146 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 150 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 154 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 158 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TabIndex */

/* 162 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0xb ),	/* 11 */
/* 170 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x24 ),	/* 36 */
/* 176 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 178 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 186 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 188 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 190 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 192 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 194 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TabIndex */

/* 198 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 200 */	NdrFcLong( 0x0 ),	/* 0 */
/* 204 */	NdrFcShort( 0xc ),	/* 12 */
/* 206 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 212 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 214 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 222 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 224 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTabCount */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0xd ),	/* 13 */
/* 242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x24 ),	/* 36 */
/* 248 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 250 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 258 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 260 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 266 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MoveToTab */

/* 270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0xe ),	/* 14 */
/* 278 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 280 */	NdrFcShort( 0xc ),	/* 12 */
/* 282 */	NdrFcShort( 0x8 ),	/* 8 */
/* 284 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 286 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 292 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter wBefor */

/* 294 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 296 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 298 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter wAfter */

/* 300 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 304 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 308 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NewWindow */

/* 312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0xf ),	/* 15 */
/* 320 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	NdrFcShort( 0x24 ),	/* 36 */
/* 326 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 328 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x1 ),	/* 1 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrURL */

/* 336 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 340 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bActive */

/* 342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 348 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 356 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MessageBox */

/* 360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x10 ),	/* 16 */
/* 368 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 372 */	NdrFcShort( 0x24 ),	/* 36 */
/* 374 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 376 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x1 ),	/* 1 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 384 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 388 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrCaption */

/* 390 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 394 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter uType */

/* 396 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 402 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 404 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 408 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 410 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowPanelBar */

/* 414 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0x11 ),	/* 17 */
/* 422 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x8 ),	/* 8 */
/* 428 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 430 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 438 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelDocumentObject */

/* 444 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 446 */	NdrFcLong( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x12 ),	/* 18 */
/* 452 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x8 ),	/* 8 */
/* 458 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 460 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 468 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 470 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 472 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 474 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 476 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelWindowObject */

/* 480 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 482 */	NdrFcLong( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x13 ),	/* 19 */
/* 488 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 494 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 496 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 504 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 506 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 508 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 512 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelWebBrowserObject */

/* 516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x14 ),	/* 20 */
/* 524 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 530 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 532 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 540 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 542 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 544 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 546 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 548 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTabState */

/* 552 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 554 */	NdrFcLong( 0x0 ),	/* 0 */
/* 558 */	NdrFcShort( 0x15 ),	/* 21 */
/* 560 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 562 */	NdrFcShort( 0x8 ),	/* 8 */
/* 564 */	NdrFcShort( 0x24 ),	/* 36 */
/* 566 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 568 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 576 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 578 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 582 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 584 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 588 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 590 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExecuteCommand */

/* 594 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 596 */	NdrFcLong( 0x0 ),	/* 0 */
/* 600 */	NdrFcShort( 0x16 ),	/* 22 */
/* 602 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 606 */	NdrFcShort( 0x8 ),	/* 8 */
/* 608 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 610 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x0 ),	/* 0 */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nCommand */

/* 618 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 620 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 626 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSearchText */

/* 630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 636 */	NdrFcShort( 0x17 ),	/* 23 */
/* 638 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 646 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 648 */	NdrFcShort( 0x1 ),	/* 1 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrText */

/* 654 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 656 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 658 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 660 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 662 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSearchText */

/* 666 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 668 */	NdrFcLong( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x18 ),	/* 24 */
/* 674 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 680 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 682 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x1 ),	/* 1 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 690 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 694 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 696 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddressText */

/* 702 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 704 */	NdrFcLong( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0x19 ),	/* 25 */
/* 710 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 712 */	NdrFcShort( 0x0 ),	/* 0 */
/* 714 */	NdrFcShort( 0x8 ),	/* 8 */
/* 716 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 718 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 720 */	NdrFcShort( 0x1 ),	/* 1 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrText */

/* 726 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 728 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 730 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 732 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 734 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAddressText */

/* 738 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 740 */	NdrFcLong( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x1a ),	/* 26 */
/* 746 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0x8 ),	/* 8 */
/* 752 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 754 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 758 */	NdrFcShort( 0x1 ),	/* 1 */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 762 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 764 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 766 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 770 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtendedTabState */

/* 774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x1b ),	/* 27 */
/* 782 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 784 */	NdrFcShort( 0x8 ),	/* 8 */
/* 786 */	NdrFcShort( 0x24 ),	/* 36 */
/* 788 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 790 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 798 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 800 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 804 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 806 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 808 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 810 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 812 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetExtendedTabState */

/* 816 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 818 */	NdrFcLong( 0x0 ),	/* 0 */
/* 822 */	NdrFcShort( 0x1c ),	/* 28 */
/* 824 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 826 */	NdrFcShort( 0x10 ),	/* 16 */
/* 828 */	NdrFcShort( 0x8 ),	/* 8 */
/* 830 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 832 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 840 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 842 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nState */

/* 846 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 848 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 854 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyState */

/* 858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x1d ),	/* 29 */
/* 866 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 870 */	NdrFcShort( 0x24 ),	/* 36 */
/* 872 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 874 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nKey */

/* 882 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 884 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 888 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 890 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 896 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProfileInt */

/* 900 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x1e ),	/* 30 */
/* 908 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 910 */	NdrFcShort( 0x8 ),	/* 8 */
/* 912 */	NdrFcShort( 0x24 ),	/* 36 */
/* 914 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 916 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x1 ),	/* 1 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 924 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 928 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 930 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 932 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 934 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 936 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 938 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 940 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nDefault */

/* 942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 944 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 948 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 950 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 954 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 956 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteProfileInt */

/* 960 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 962 */	NdrFcLong( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0x1f ),	/* 31 */
/* 968 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 970 */	NdrFcShort( 0x8 ),	/* 8 */
/* 972 */	NdrFcShort( 0x8 ),	/* 8 */
/* 974 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 976 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	NdrFcShort( 0x1 ),	/* 1 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 984 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 986 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 988 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 990 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 992 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 994 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 996 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 998 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1000 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nValue */

/* 1002 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1004 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1008 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1010 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProfileString */

/* 1014 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1016 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1022 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1028 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1030 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1032 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1034 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1036 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 1038 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1040 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1042 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 1044 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1046 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1048 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 1050 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1052 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1054 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDefault */

/* 1056 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1058 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1060 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter pbstrText */

/* 1062 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1064 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1066 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1070 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteProfileString */

/* 1074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1082 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1088 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1090 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1092 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1094 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1096 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 1098 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1100 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1102 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 1104 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1106 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1108 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 1110 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1112 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1114 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrText */

/* 1116 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1118 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1120 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 1122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1124 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScriptFolder */

/* 1128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1142 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1144 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1146 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1152 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1154 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1156 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1160 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCSSFolder */

/* 1164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1172 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1176 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1178 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1180 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1182 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1188 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1190 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1192 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1194 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1196 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBaseFolder */

/* 1200 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1202 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1214 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1216 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1218 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1220 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1224 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1226 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1228 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1230 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1232 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExePath */

/* 1236 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1238 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1242 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1244 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1250 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1252 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1254 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrPath */

/* 1260 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1262 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1264 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1268 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStyleSheet */

/* 1272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1278 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1280 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1282 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1286 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1288 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1294 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 1296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1298 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter bstrStyleSheet */

/* 1302 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1304 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1306 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bOff */

/* 1308 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1310 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1314 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1316 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveGroup */

/* 1320 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1322 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1326 */	NdrFcShort( 0x27 ),	/* 39 */
/* 1328 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1334 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1336 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1344 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1346 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1348 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 1350 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1352 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadGroup */

/* 1356 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1364 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1370 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1372 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1376 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1380 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1382 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1384 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bClose */

/* 1386 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1388 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1392 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1394 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EncryptString */

/* 1398 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1400 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x29 ),	/* 41 */
/* 1406 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1410 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1412 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1414 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1416 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1418 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1420 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrString */

/* 1422 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1424 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1426 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrPass */

/* 1428 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1430 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1432 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrRet */

/* 1434 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1436 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1438 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1442 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DecryptString */

/* 1446 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1452 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1454 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1458 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1460 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1462 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1464 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1466 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1468 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrString */

/* 1470 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1472 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1474 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrRet */

/* 1476 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1478 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1480 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1482 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1484 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InputBox */

/* 1488 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1496 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1500 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1502 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1504 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1508 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1510 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrTitle */

/* 1512 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1514 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1516 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDescript */

/* 1518 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1520 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1522 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDefault */

/* 1524 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1526 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1528 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nFlag */

/* 1530 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1532 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1536 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1538 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1542 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1544 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NewWindow3 */

/* 1548 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1550 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1554 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1556 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1558 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1560 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1562 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1564 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1568 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1570 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrURL */

/* 1572 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1574 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1576 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bActive */

/* 1578 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1580 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ExStyle */

/* 1584 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1586 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pHistInfo */

/* 1590 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 1592 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1594 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1596 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1598 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1602 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1604 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddGroupItem */

/* 1608 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1614 */	NdrFcShort( 0x2d ),	/* 45 */
/* 1616 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1620 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1622 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1624 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1630 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1632 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1634 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1636 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nIndex */

/* 1638 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1640 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1644 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1646 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1648 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1652 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteGroupItem */

/* 1656 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x2e ),	/* 46 */
/* 1664 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1666 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1668 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1670 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1672 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1676 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1680 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1682 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1684 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nIndex */

/* 1686 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1688 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1692 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1694 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1700 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHWND */

/* 1704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1710 */	NdrFcShort( 0x2f ),	/* 47 */
/* 1712 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1714 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1716 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1718 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1720 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1726 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nType */

/* 1728 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1730 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1734 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1736 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1740 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1742 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const DonutP_MIDL_TYPE_FORMAT_STRING DonutP__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  8 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 24 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 26 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 28 */	
			0x12, 0x0,	/* FC_UP */
/* 30 */	NdrFcShort( 0xe ),	/* Offset= 14 (44) */
/* 32 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 34 */	NdrFcShort( 0x2 ),	/* 2 */
/* 36 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 38 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 40 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 42 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 44 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (32) */
/* 50 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 52 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 54 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x4 ),	/* 4 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0xffde ),	/* Offset= -34 (28) */
/* 64 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 66 */	NdrFcShort( 0x6 ),	/* Offset= 6 (72) */
/* 68 */	
			0x13, 0x0,	/* FC_OP */
/* 70 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (44) */
/* 72 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x4 ),	/* 4 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (68) */
/* 82 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 84 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IAPI, ver. 0.0,
   GUID={0x1413C5B5,0x0352,0x47AA,{0x85,0xC6,0x92,0x19,0xF2,0x43,0xF5,0xE8}} */

#pragma code_seg(".orpc")
static const unsigned short IAPI_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    120,
    162,
    198,
    234,
    270,
    312,
    360,
    414,
    444,
    480,
    516,
    552
    };

static const MIDL_STUBLESS_PROXY_INFO IAPI_ProxyInfo =
    {
    &Object_StubDesc,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IAPI_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(22) _IAPIProxyVtbl = 
{
    &IAPI_ProxyInfo,
    &IID_IAPI,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IAPI::Close */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::get_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::put_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabCount */ ,
    (void *) (INT_PTR) -1 /* IAPI::MoveToTab */ ,
    (void *) (INT_PTR) -1 /* IAPI::NewWindow */ ,
    (void *) (INT_PTR) -1 /* IAPI::MessageBox */ ,
    (void *) (INT_PTR) -1 /* IAPI::ShowPanelBar */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabState */
};


static const PRPC_STUB_FUNCTION IAPI_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IAPIStubVtbl =
{
    &IID_IAPI,
    &IAPI_ServerInfo,
    22,
    &IAPI_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IAPI2, ver. 0.0,
   GUID={0xA7B5128A,0xE147,0x41d2,{0x81,0x55,0x4B,0x9B,0xAE,0x44,0x9D,0xF4}} */

#pragma code_seg(".orpc")
static const unsigned short IAPI2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    120,
    162,
    198,
    234,
    270,
    312,
    360,
    414,
    444,
    480,
    516,
    552,
    594,
    630,
    666,
    702,
    738,
    774,
    816,
    858,
    900,
    960,
    1014,
    1074,
    1128,
    1164,
    1200,
    1236,
    1272
    };

static const MIDL_STUBLESS_PROXY_INFO IAPI2_ProxyInfo =
    {
    &Object_StubDesc,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IAPI2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(39) _IAPI2ProxyVtbl = 
{
    &IAPI2_ProxyInfo,
    &IID_IAPI2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IAPI::Close */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::get_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::put_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabCount */ ,
    (void *) (INT_PTR) -1 /* IAPI::MoveToTab */ ,
    (void *) (INT_PTR) -1 /* IAPI::NewWindow */ ,
    (void *) (INT_PTR) -1 /* IAPI::MessageBox */ ,
    (void *) (INT_PTR) -1 /* IAPI::ShowPanelBar */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::ExecuteCommand */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetKeyState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetScriptFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetCSSFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetBaseFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExePath */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetStyleSheet */
};


static const PRPC_STUB_FUNCTION IAPI2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IAPI2StubVtbl =
{
    &IID_IAPI2,
    &IAPI2_ServerInfo,
    39,
    &IAPI2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IAPI3, ver. 0.0,
   GUID={0x738A9354,0xD4E7,0x4dd9,{0x86,0x1B,0x15,0xD7,0x9F,0x2C,0x46,0xF5}} */

#pragma code_seg(".orpc")
static const unsigned short IAPI3_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    120,
    162,
    198,
    234,
    270,
    312,
    360,
    414,
    444,
    480,
    516,
    552,
    594,
    630,
    666,
    702,
    738,
    774,
    816,
    858,
    900,
    960,
    1014,
    1074,
    1128,
    1164,
    1200,
    1236,
    1272,
    1320,
    1356,
    1398,
    1446,
    1488,
    1548,
    1608,
    1656
    };

static const MIDL_STUBLESS_PROXY_INFO IAPI3_ProxyInfo =
    {
    &Object_StubDesc,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI3_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IAPI3_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI3_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(47) _IAPI3ProxyVtbl = 
{
    &IAPI3_ProxyInfo,
    &IID_IAPI3,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IAPI::Close */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::get_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::put_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabCount */ ,
    (void *) (INT_PTR) -1 /* IAPI::MoveToTab */ ,
    (void *) (INT_PTR) -1 /* IAPI::NewWindow */ ,
    (void *) (INT_PTR) -1 /* IAPI::MessageBox */ ,
    (void *) (INT_PTR) -1 /* IAPI::ShowPanelBar */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::ExecuteCommand */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetKeyState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetScriptFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetCSSFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetBaseFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExePath */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetStyleSheet */ ,
    (void *) (INT_PTR) -1 /* IAPI3::SaveGroup */ ,
    (void *) (INT_PTR) -1 /* IAPI3::LoadGroup */ ,
    (void *) (INT_PTR) -1 /* IAPI3::EncryptString */ ,
    (void *) (INT_PTR) -1 /* IAPI3::DecryptString */ ,
    (void *) (INT_PTR) -1 /* IAPI3::InputBox */ ,
    (void *) (INT_PTR) -1 /* IAPI3::NewWindow3 */ ,
    (void *) (INT_PTR) -1 /* IAPI3::AddGroupItem */ ,
    (void *) (INT_PTR) -1 /* IAPI3::DeleteGroupItem */
};


static const PRPC_STUB_FUNCTION IAPI3_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IAPI3StubVtbl =
{
    &IID_IAPI3,
    &IAPI3_ServerInfo,
    47,
    &IAPI3_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IAPI4, ver. 0.0,
   GUID={0xB437832F,0xA3A8,0x4948,{0x9D,0x42,0x64,0xD1,0xB9,0x1C,0x60,0x4E}} */

#pragma code_seg(".orpc")
static const unsigned short IAPI4_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    120,
    162,
    198,
    234,
    270,
    312,
    360,
    414,
    444,
    480,
    516,
    552,
    594,
    630,
    666,
    702,
    738,
    774,
    816,
    858,
    900,
    960,
    1014,
    1074,
    1128,
    1164,
    1200,
    1236,
    1272,
    1320,
    1356,
    1398,
    1446,
    1488,
    1548,
    1608,
    1656,
    1704
    };

static const MIDL_STUBLESS_PROXY_INFO IAPI4_ProxyInfo =
    {
    &Object_StubDesc,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI4_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IAPI4_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    DonutP__MIDL_ProcFormatString.Format,
    &IAPI4_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(48) _IAPI4ProxyVtbl = 
{
    &IAPI4_ProxyInfo,
    &IID_IAPI4,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IAPI::Close */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::get_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::put_TabIndex */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabCount */ ,
    (void *) (INT_PTR) -1 /* IAPI::MoveToTab */ ,
    (void *) (INT_PTR) -1 /* IAPI::NewWindow */ ,
    (void *) (INT_PTR) -1 /* IAPI::MessageBox */ ,
    (void *) (INT_PTR) -1 /* IAPI::ShowPanelBar */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelDocumentObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWindowObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetPanelWebBrowserObject */ ,
    (void *) (INT_PTR) -1 /* IAPI::GetTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::ExecuteCommand */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetSearchText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetAddressText */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetExtendedTabState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetKeyState */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileInt */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::WriteProfileString */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetScriptFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetCSSFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetBaseFolder */ ,
    (void *) (INT_PTR) -1 /* IAPI2::GetExePath */ ,
    (void *) (INT_PTR) -1 /* IAPI2::SetStyleSheet */ ,
    (void *) (INT_PTR) -1 /* IAPI3::SaveGroup */ ,
    (void *) (INT_PTR) -1 /* IAPI3::LoadGroup */ ,
    (void *) (INT_PTR) -1 /* IAPI3::EncryptString */ ,
    (void *) (INT_PTR) -1 /* IAPI3::DecryptString */ ,
    (void *) (INT_PTR) -1 /* IAPI3::InputBox */ ,
    (void *) (INT_PTR) -1 /* IAPI3::NewWindow3 */ ,
    (void *) (INT_PTR) -1 /* IAPI3::AddGroupItem */ ,
    (void *) (INT_PTR) -1 /* IAPI3::DeleteGroupItem */ ,
    (void *) (INT_PTR) -1 /* IAPI4::GetHWND */
};


static const PRPC_STUB_FUNCTION IAPI4_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IAPI4StubVtbl =
{
    &IID_IAPI4,
    &IAPI4_ServerInfo,
    48,
    &IAPI4_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    DonutP__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _DonutP_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IAPI4ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAPI3ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAPI2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IAPIProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _DonutP_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IAPI4StubVtbl,
    ( CInterfaceStubVtbl *) &_IAPI3StubVtbl,
    ( CInterfaceStubVtbl *) &_IAPI2StubVtbl,
    ( CInterfaceStubVtbl *) &_IAPIStubVtbl,
    0
};

PCInterfaceName const _DonutP_InterfaceNamesList[] = 
{
    "IAPI4",
    "IAPI3",
    "IAPI2",
    "IAPI",
    0
};

const IID *  const _DonutP_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _DonutP_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _DonutP, pIID, n)

int __stdcall _DonutP_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _DonutP, 4, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _DonutP, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _DonutP, 4, *pIndex )
    
}

const ExtendedProxyFileInfo DonutP_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _DonutP_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _DonutP_StubVtblList,
    (const PCInterfaceName * ) & _DonutP_InterfaceNamesList,
    (const IID ** ) & _DonutP_BaseIIDList,
    & _DonutP_IID_Lookup, 
    4,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

