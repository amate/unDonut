/**
 *	@file	DonutSecurityZone.h
 *	@brief	クッキーの扱いに関するコマンドメッセージをハンドリングするクラス
 */
#pragma once

#include "resource.h"
#include "MtlUpdateCmdUI.h"
#include "MtlWeb.h"


/*
	CDonutSecurityZone
	クッキーの扱いに関するコマンドメッセージをハンドリングするクラス

	クッキーの使用可否、セッションごとのクッキーの使用可否、
	それら両方の状態変更の三種のコマンドに反応する

	メニューでいうところの[ツール]-[クッキー]
 */
class CDonutSecurityZone {
public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutSecurityZone)
		COMMAND_ID_HANDLER_EX(ID_URLACTION_COOKIES_CHG	  , OnUrlActionCookiesChg	 )
		COMMAND_ID_HANDLER_EX(ID_URLACTION_COOKIES		  , OnUrlActionCookies		 )
		COMMAND_ID_HANDLER_EX(ID_URLACTION_COOKIES_SESSION, OnUrlActionCookiesSession)
	END_MSG_MAP()

private:
	void OnUrlActionCookies(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnUrlActionCookiesSession(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// UH
	void OnUrlActionCookiesChg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

public:
	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP(CDonutSecurityZone)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_URLACTION_COOKIES,		 !MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES) )
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_URLACTION_COOKIES_SESSION, !MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES_SESSION) )
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_URLACTION_COOKIES_CHG,	 !MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES) && !MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES_SESSION) )
	END_UPDATE_COMMAND_UI_MAP()
};
