/**
*	@file	MultiThreadManager.h
*	@brief	CChildFrame用のスレッド作成管理
*/

#pragma once

// 前方宣言
class CChildFrame;
struct NewChildFrameData;


namespace MultiThreadManager {

int Run(LPTSTR lpstrCmdLine, int nCmdShow, bool bTray);

void	ExecuteChildFrameThread(CChildFrame* pChild, NewChildFrameData* pData);


};	// namespace MultiThreadManager





