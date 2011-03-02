/**
 *	@file	CmbboxPrxy.h
 *	@brief	コンボボックス：プロキシ
 */
#ifndef __CMBBOXPRXY_H__
#define __CMBBOXPRXY_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <time.h>

/*
	CComboBoxPrxy
 */


#include "FlatComboBox.h"

//CComboBoxPrxy ReWrite ver
class CComboBoxPrxyR : public CFlatComboBox {
private:

	UINT_PTR	m_nIDEvent;
	bool		m_bUseIE;

public:
	CComboBoxPrxyR();

	BEGIN_MSG_MAP(CComboBoxPrxyR)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)

		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CFlatComboBox)
	END_MSG_MAP()

	bool	IsUseIE() const { return m_bUseIE; }
	void	ResetProxyList();

private:
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void	OnTimer(UINT_PTR wTimerID, TIMERPROC dmy=0);
	void	OnSelectChange();

	void	ChangeProxy(int nIndex);
	void	SetProxy();
	void	ResetTimer();

	// IEのを使う
	bool	UseIE();

	// バイパスを得る
	CString GetBypass();
};

#endif
