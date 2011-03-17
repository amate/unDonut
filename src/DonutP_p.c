

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Mar 17 21:18:54 2011
 */
/* Compiler settings for DonutP.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

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

#define TYPE_FORMAT_STRING_SIZE   91                                
#define PROC_FORMAT_STRING_SIZE   1829                              
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

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
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
/*  8 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentObject */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	NdrFcShort( 0x8 ),	/* 8 */
/* 52 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 54 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 64 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 66 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 68 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 70 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 72 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 74 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 76 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 78 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWindowObject */

/* 82 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 84 */	NdrFcLong( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x9 ),	/* 9 */
/* 90 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 92 */	NdrFcShort( 0x8 ),	/* 8 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 98 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 110 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 114 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 116 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 118 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 122 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWebBrowserObject */

/* 126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0xa ),	/* 10 */
/* 134 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 142 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 152 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 154 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 158 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 160 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 162 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 166 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_TabIndex */

/* 170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0xb ),	/* 11 */
/* 178 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x24 ),	/* 36 */
/* 184 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 186 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 196 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 198 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 202 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 204 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_TabIndex */

/* 208 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 210 */	NdrFcLong( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0xc ),	/* 12 */
/* 216 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 222 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 224 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter newVal */

/* 234 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 236 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 240 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 242 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTabCount */

/* 246 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 248 */	NdrFcLong( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0xd ),	/* 13 */
/* 254 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x24 ),	/* 36 */
/* 260 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 262 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 272 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 274 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 280 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MoveToTab */

/* 284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0xe ),	/* 14 */
/* 292 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 294 */	NdrFcShort( 0xc ),	/* 12 */
/* 296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 298 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 300 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter wBefor */

/* 310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 312 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 314 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter wAfter */

/* 316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 318 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 320 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NewWindow */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0xf ),	/* 15 */
/* 336 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	NdrFcShort( 0x24 ),	/* 36 */
/* 342 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 344 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x1 ),	/* 1 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrURL */

/* 354 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 356 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 358 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bActive */

/* 360 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 362 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 366 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 368 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MessageBox */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x10 ),	/* 16 */
/* 386 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 388 */	NdrFcShort( 0x8 ),	/* 8 */
/* 390 */	NdrFcShort( 0x24 ),	/* 36 */
/* 392 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 394 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x1 ),	/* 1 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 404 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 406 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 408 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrCaption */

/* 410 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 412 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 414 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter uType */

/* 416 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 418 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 422 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 424 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 426 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 428 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 430 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowPanelBar */

/* 434 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 436 */	NdrFcLong( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x11 ),	/* 17 */
/* 442 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0x8 ),	/* 8 */
/* 448 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 450 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 458 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 460 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 462 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelDocumentObject */

/* 466 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 468 */	NdrFcLong( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x12 ),	/* 18 */
/* 474 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x8 ),	/* 8 */
/* 480 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 482 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 492 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 494 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 496 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 498 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 500 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelWindowObject */

/* 504 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x13 ),	/* 19 */
/* 512 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x8 ),	/* 8 */
/* 518 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 520 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 530 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 532 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 534 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 538 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPanelWebBrowserObject */

/* 542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x14 ),	/* 20 */
/* 550 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 556 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 558 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 568 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 570 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 572 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 574 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 576 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 578 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetTabState */

/* 580 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 582 */	NdrFcLong( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x15 ),	/* 21 */
/* 588 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 592 */	NdrFcShort( 0x24 ),	/* 36 */
/* 594 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 596 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 598 */	NdrFcShort( 0x0 ),	/* 0 */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 608 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 612 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 614 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 618 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 620 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExecuteCommand */

/* 624 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 626 */	NdrFcLong( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x16 ),	/* 22 */
/* 632 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 634 */	NdrFcShort( 0x8 ),	/* 8 */
/* 636 */	NdrFcShort( 0x8 ),	/* 8 */
/* 638 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 640 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nCommand */

/* 650 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 652 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 656 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 658 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSearchText */

/* 662 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 664 */	NdrFcLong( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0x17 ),	/* 23 */
/* 670 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 674 */	NdrFcShort( 0x8 ),	/* 8 */
/* 676 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 678 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrText */

/* 688 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 690 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 692 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 694 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 696 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 698 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSearchText */

/* 700 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 702 */	NdrFcLong( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x18 ),	/* 24 */
/* 708 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 714 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 716 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x1 ),	/* 1 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 726 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 728 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 730 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 732 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 734 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAddressText */

/* 738 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 740 */	NdrFcLong( 0x0 ),	/* 0 */
/* 744 */	NdrFcShort( 0x19 ),	/* 25 */
/* 746 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0x8 ),	/* 8 */
/* 752 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 754 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 756 */	NdrFcShort( 0x1 ),	/* 1 */
/* 758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrText */

/* 764 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 766 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 768 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 770 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 772 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 774 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAddressText */

/* 776 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 778 */	NdrFcLong( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x1a ),	/* 26 */
/* 784 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x8 ),	/* 8 */
/* 790 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 792 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x1 ),	/* 1 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrText */

/* 802 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 804 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 806 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 808 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 810 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 812 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtendedTabState */

/* 814 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 816 */	NdrFcLong( 0x0 ),	/* 0 */
/* 820 */	NdrFcShort( 0x1b ),	/* 27 */
/* 822 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 826 */	NdrFcShort( 0x24 ),	/* 36 */
/* 828 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 830 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 840 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 842 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 846 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 848 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 854 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetExtendedTabState */

/* 858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x1c ),	/* 28 */
/* 866 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 868 */	NdrFcShort( 0x10 ),	/* 16 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 874 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */
/* 882 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 884 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 886 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nState */

/* 890 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 892 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 898 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetKeyState */

/* 902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 908 */	NdrFcShort( 0x1d ),	/* 29 */
/* 910 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 912 */	NdrFcShort( 0x8 ),	/* 8 */
/* 914 */	NdrFcShort( 0x24 ),	/* 36 */
/* 916 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 918 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nKey */

/* 928 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 930 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 932 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 934 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 936 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 938 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 940 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 942 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProfileInt */

/* 946 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 948 */	NdrFcLong( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x1e ),	/* 30 */
/* 954 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 958 */	NdrFcShort( 0x24 ),	/* 36 */
/* 960 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 962 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */
/* 966 */	NdrFcShort( 0x1 ),	/* 1 */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 972 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 974 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 976 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 978 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 980 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 982 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 984 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 986 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 988 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nDefault */

/* 990 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 992 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 994 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 996 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 998 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1002 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1004 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteProfileInt */

/* 1008 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1010 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1016 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1022 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1024 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 1034 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1036 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1038 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 1040 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1042 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1044 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 1046 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1048 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1050 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nValue */

/* 1052 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1054 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1058 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1060 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProfileString */

/* 1064 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1066 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1072 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1078 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1080 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1082 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1084 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 1090 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1092 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1094 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 1096 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1098 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1100 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 1102 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1104 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1106 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDefault */

/* 1108 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1110 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1112 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter pbstrText */

/* 1114 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1116 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1118 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1122 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteProfileString */

/* 1126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1132 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1134 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1140 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1142 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1146 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrFile */

/* 1152 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1154 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1156 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrSection */

/* 1158 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1160 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1162 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrKey */

/* 1164 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1166 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1168 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrText */

/* 1170 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1172 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1174 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 1176 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1178 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScriptFolder */

/* 1182 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1184 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1190 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1196 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1198 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1200 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1208 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1210 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1212 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1216 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCSSFolder */

/* 1220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1226 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1228 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1234 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1236 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1238 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1246 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1248 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1250 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1254 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBaseFolder */

/* 1258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1264 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1266 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1272 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1274 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1276 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrFolder */

/* 1284 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1286 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1288 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1292 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExePath */

/* 1296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1304 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1310 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1312 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1314 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pbstrPath */

/* 1322 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1324 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1326 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1330 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStyleSheet */

/* 1334 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1342 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1344 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1348 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1350 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1354 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nIndex */

/* 1360 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1362 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter bstrStyleSheet */

/* 1366 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1368 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1370 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bOff */

/* 1372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1374 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1380 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SaveGroup */

/* 1384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x27 ),	/* 39 */
/* 1392 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1398 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1400 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1410 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1412 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1414 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Return value */

/* 1416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1418 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadGroup */

/* 1422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1428 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1430 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1436 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1438 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1448 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1450 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1452 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bClose */

/* 1454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1456 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1460 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1462 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1464 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EncryptString */

/* 1466 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1468 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1472 */	NdrFcShort( 0x29 ),	/* 41 */
/* 1474 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1480 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1482 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1484 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1486 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1490 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrString */

/* 1492 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1494 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1496 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrPass */

/* 1498 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1500 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1502 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrRet */

/* 1504 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1506 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1508 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1512 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DecryptString */

/* 1516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x2a ),	/* 42 */
/* 1524 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1530 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1532 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1534 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1536 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1540 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrString */

/* 1542 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1544 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1546 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrRet */

/* 1548 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 1550 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1552 */	NdrFcShort( 0x48 ),	/* Type Offset=72 */

	/* Return value */

/* 1554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1556 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InputBox */

/* 1560 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1566 */	NdrFcShort( 0x2b ),	/* 43 */
/* 1568 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 1570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1572 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1574 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1576 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1580 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrTitle */

/* 1586 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1588 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1590 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDescript */

/* 1592 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1594 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1596 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bstrDefault */

/* 1598 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1600 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1602 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nFlag */

/* 1604 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1606 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1610 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1612 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1616 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1618 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NewWindow3 */

/* 1622 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1630 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 1632 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1634 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1636 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1638 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1642 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1646 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrURL */

/* 1648 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1650 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1652 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter bActive */

/* 1654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1656 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ExStyle */

/* 1660 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1662 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pHistInfo */

/* 1666 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 1668 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1670 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1672 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1674 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1680 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddGroupItem */

/* 1684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1690 */	NdrFcShort( 0x2d ),	/* 45 */
/* 1692 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1696 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1698 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1700 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1704 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1708 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1710 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1712 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1714 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nIndex */

/* 1716 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1718 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1720 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1722 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1724 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1728 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1730 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1732 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteGroupItem */

/* 1734 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1736 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1740 */	NdrFcShort( 0x2e ),	/* 46 */
/* 1742 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1746 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1748 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1750 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1754 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrGroupFile */

/* 1760 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1762 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1764 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter nIndex */

/* 1766 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1768 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1770 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1772 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1774 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1778 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1780 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHWND */

/* 1784 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1786 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1790 */	NdrFcShort( 0x2f ),	/* 47 */
/* 1792 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1794 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1796 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1798 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1800 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1808 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nType */

/* 1810 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1812 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pVal */

/* 1816 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1818 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1820 */	0xb8,		/* FC_INT3264 */
			0x0,		/* 0 */

	/* Return value */

/* 1822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1824 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1826 */	0x8,		/* FC_LONG */
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
/* 58 */	NdrFcShort( 0x8 ),	/* 8 */
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
/* 76 */	NdrFcShort( 0x8 ),	/* 8 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (68) */
/* 82 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 84 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 86 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 88 */	0xb8,		/* FC_INT3264 */
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
    38,
    82,
    126,
    170,
    208,
    246,
    284,
    328,
    378,
    434,
    466,
    504,
    542,
    580
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
    38,
    82,
    126,
    170,
    208,
    246,
    284,
    328,
    378,
    434,
    466,
    504,
    542,
    580,
    624,
    662,
    700,
    738,
    776,
    814,
    858,
    902,
    946,
    1008,
    1064,
    1126,
    1182,
    1220,
    1258,
    1296,
    1334
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
    38,
    82,
    126,
    170,
    208,
    246,
    284,
    328,
    378,
    434,
    466,
    504,
    542,
    580,
    624,
    662,
    700,
    738,
    776,
    814,
    858,
    902,
    946,
    1008,
    1064,
    1126,
    1182,
    1220,
    1258,
    1296,
    1334,
    1384,
    1422,
    1466,
    1516,
    1560,
    1622,
    1684,
    1734
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
    38,
    82,
    126,
    170,
    208,
    246,
    284,
    328,
    378,
    434,
    466,
    504,
    542,
    580,
    624,
    662,
    700,
    738,
    776,
    814,
    858,
    902,
    946,
    1008,
    1064,
    1126,
    1182,
    1220,
    1258,
    1296,
    1334,
    1384,
    1422,
    1466,
    1516,
    1560,
    1622,
    1684,
    1734,
    1784
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
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

