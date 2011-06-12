// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include "MyPluginData.h"

//--------------------------------------------
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//===========================================================================
// interface

//---------------------------------------------
/// unDonutがプラグインの情報を必要とするときに呼び出されます。
extern "C" void WINAPI GetPluginInfo(PLUGININFO* pstPluginInfo)
{
	memset( pstPluginInfo, 0, sizeof(PLUGININFO) );
	pstPluginInfo->type = MYPLUGINTYPE;

	strcpy_s( pstPluginInfo->name			, MYPLUGIN_NAME );
	strcpy_s( pstPluginInfo->version		, MYPLUGIN_VERSION );
	strcpy_s( pstPluginInfo->versionDate	, "" );
	strcpy_s( pstPluginInfo->authorName		, MYPLUGIN_AUTHORNAME );
	strcpy_s( pstPluginInfo->authorUrl		, "-----" );
	strcpy_s( pstPluginInfo->authorEmail	, "-----" );
	strcpy_s( pstPluginInfo->comment		, MYPLUGIN_COMMENT );
}


//----------------------------------------------
/// unDonutに表示するツールバーを作成します
extern "C" HWND WINAPI CreateToolBar(HWND hWndParent, UINT nID)
{
	return NULL;
}


//----------------------------------------------
extern "C" BOOL WINAPI PreTranslateMessage(MSG* pMsg)
{
	// DLLにはメッセージループがないので自分で必要なところに配送する

	return FALSE;
}


//----------------------------------------------
/// unDonutで発生したイベントを処理します
extern "C" int  WINAPI PluginEvent(UINT uMsg, FPARAM fParam, SPARAM sParam)
{
	return 0;
}
