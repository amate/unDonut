/**
 *	@file	AboutDlg.h
 *	@brief	�o�[�W��������\������_�C�A���O
 */

#pragma once

#include "../resource.h"
#ifdef USE_AERO
#include "../Aero.h"
#endif

/// �o�[�W��������\������_�C�A���O�N���X
/// unDonut�{�̂�Windows�AIE�̃o�[�W�������擾���ĕ\�����A
/// �R�s�[���邱�Ƃ��ł��܂��B
#ifdef USE_AERO
class CAboutDlg : public CAeroDialogImpl<CAboutDlg>			//+++ Aero����.
#else
class CAboutDlg : public CDialogImpl<CAboutDlg>
#endif
{
public:
	enum { IDD = IDD_ABOUTBOX };

	// �R���X�g���N�^
	CAboutDlg();

	// ���b�Z�[�W�}�b�v
	BEGIN_MSG_MAP(CAboutDlg)
	  #ifdef USE_AERO
		CHAIN_MSG_MAP(CAeroDialogImpl<CAboutDlg>)			//+++ Aero.
	  #endif
		MESSAGE_HANDLER   ( WM_INITDIALOG, OnInitDialog)
		MSG_WM_CTLCOLORSTATIC( OnCtlColorStatic )
		COMMAND_ID_HANDLER( IDOK		 , OnCloseCmd  )
		COMMAND_ID_HANDLER( IDCANCEL	 , OnCloseCmd  )
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER   ( WM_LBUTTONUP , OnLButtonUp )
	END_MSG_MAP()


	//�R�}���h�n���h��
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	HBRUSH	OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);

	//���̑������֐�
	CString 	GetEnvironInfo();
	CString 	GetIEVersion();
	CString 	GetUpdateInfo();
	CString 	GetOSInfo();
	CString 	GetVersion();
	CString 	GetOSName();
	CString 	GetOSName_Version3(OSVERSIONINFO &osvi);	//NT3.51
	CString 	GetOSName_Version4(OSVERSIONINFO &osvi);	//95 98 Me NT4
	CString 	GetOSName_Version5(OSVERSIONINFO &osvi);	//2000 XP 2003Server
	CString 	GetOSName_Version6(OSVERSIONINFO &osvi);	//Vista 7 8 8.1
	CString 	GetOSName_Version10(OSVERSIONINFO &osvi);	//10

private:
	void	_subclassWindows(HWND hWndChild);

	// Data members
	CContainedWindow m_wndEdit;		//�e�L�X�g�{�b�N�X
	CAeroStatic	m_static[12];
	int	m_nSubclassPos;
	CAeroCtrlImpl<CStatic>	m_staticIcon;
	CAeroButton	m_btnOk;
};

/////////////////////////////////////////////////////////////////////////////
