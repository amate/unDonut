/**
*	@file	BeginMsgMap_decl.h
*	@brief	メッセージマップを .cpp実装するためのマクロ
*/

#pragma once


#define BEGIN_MSG_MAP_EX_decl(theClass) 				\
public:													\
	BOOL m_bMsgHandled; 								\
	/* "handled" management for cracked handlers */ 	\
	__inline BOOL IsMsgHandled() const {				\
		return m_bMsgHandled; 							\
	} 													\
	__inline void SetMsgHandled(BOOL bHandled) {		\
		m_bMsgHandled = bHandled; 						\
	} 													\
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0); \
	BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);



#define BEGIN_MSG_MAP_EX_impl(theClass) 				\
	BOOL theClass::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	{ 													\
		BOOL bOldMsgHandled = m_bMsgHandled; 			\
		BOOL bRet 			= _ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID); \
		m_bMsgHandled 		= bOldMsgHandled;			\
		return bRet;									\
	} 													\
	BOOL theClass::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	{ 													\
		BOOL bHandled = TRUE; 							\
		hWnd; 											\
		uMsg; 											\
		wParam; 										\
		lParam; 										\
		lResult; 										\
		bHandled; 										\
		switch(dwMsgMapID) 								\
		{ 												\
		case 0:


#define BEGIN_DDX_MAP_decl(theClass)
#define END_DDX_MAP_decl(theClass)

