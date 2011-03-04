/**
 *	@file	DonutSecurityZone.cpp
 *	@brief	クッキーの扱いに関するコマンドメッセージをハンドリングするクラス
 */
#include "stdafx.h"
#include "DonutSecurityZone.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




void CDonutSecurityZone::OnUrlActionCookies(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//クッキーの使用状態反転
	MtlSetInternetZoneActionPolicy(URLACTION_COOKIES,
			MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES) ? URLPOLICY_ALLOW : URLPOLICY_DISALLOW);
}



void CDonutSecurityZone::OnUrlActionCookiesSession(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//セッション毎のクッキー使用状態反転
	MtlSetInternetZoneActionPolicy(URLACTION_COOKIES_SESSION,
			MtlIsInternetZoneActionPolicyDisallow(URLACTION_COOKIES_SESSION) ? URLPOLICY_ALLOW : URLPOLICY_DISALLOW);
}



// UH
void CDonutSecurityZone::OnUrlActionCookiesChg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	//両方反転
	OnUrlActionCookies(0,0,0);
	OnUrlActionCookiesSession(0,0,0);
}

