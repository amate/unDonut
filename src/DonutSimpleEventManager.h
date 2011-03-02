/**
 *	@file	DonutSimpleEventManager.h
 *	@brief	特定のイベントが発生した場合、iniの指定のキーに書き込んだファイルを実行させるためのクラス
 */

#pragma once


enum EDonutSimpleEvent {
	EVENT_PROCESS_START 		= 1,
	EVENT_INITIALIZE_COMPLETE	= 2,
	EVENT_CLOSING_WINDOW		= 3,
	EVENT_PROCESS_END			= 4,
};


/*
	特定のイベントが発生した場合、iniの指定のキーに書き込んだファイルを実行させるためのクラス
 */
class CDonutSimpleEventManager {

	static CString ReadEventData(CString strKey) {
		CIniFileI	pr( g_szIniFileName, _T("Event") );
		return pr.GetString(strKey);
	}

public:
	static void RaiseEvent(EDonutSimpleEvent nEventID) {
		CString 	strFile;
		switch (nEventID) {
		case EVENT_PROCESS_START:		strFile = ReadEventData( _T("StartProcess") ); break;
		case EVENT_INITIALIZE_COMPLETE: strFile = ReadEventData( _T("InitComplete") ); break;
		case EVENT_CLOSING_WINDOW:		strFile = ReadEventData( _T("CloseWindow")	); break;
		case EVENT_PROCESS_END: 		strFile = ReadEventData( _T("EndProcess")	); break;
		default:						ATLASSERT(FALSE);
		}

		if (!strFile.IsEmpty() && ::GetFileAttributes(strFile) != 0xFFFFFFFF)
			::ShellExecute(NULL, NULL, strFile, NULL, NULL, SW_SHOWNORMAL);
	}
};

