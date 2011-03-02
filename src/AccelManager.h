/**
 *	@file	AccelManager.h
 *	@brief	キーアクセラレータ(ショートカットキー)に関する処理を受け持つクラス群
 *	@note
 *	CKeyHelper
 *	CAccelerManager
 *	CEditAccel
 *	を有する
 */


#ifndef __ACCELMANAGER_H__
#define __ACCELMANAGER_H__

#if _MSC_VER > 1000
 #pragma once
#endif	// _MSC_VER > 1000

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>
#include <atlctrls.h>



/*
	CKeyHelper
	アクセラレータキーの情報からショートカットキー文字列を生成するヘルパクラス

	CKeyHelperは1ショートカットキーの状態から"Ctrl+Shift+R","F8"のような文字列を
	生成するショートカットキーを扱うための補助クラスです。

	CKeyHelper(lpAccel);またはSetAccelerator(lpAccel)を呼び出して
	文字列を取得したいアクセラレータを設定します。
	続いてFormat関数を呼び出すと引数の参照を介して文字列を得ることが出来ます。
 */
class CKeyHelper {
protected:
	//メンバ変数
	LPACCEL 	m_lpAccel;									//アクセラレータのポインタ

	//内部関数
	//void		AddVirtKeyStr(CString &str, UINT uiVirtKey, BOOL bLast = FALSE) const;
	bool		AddVirtKeyStr(CString &str, UINT uiVirtKey, LPCTSTR xname=NULL, BOOL bLast = FALSE) const;

public:
	//初期化・破棄
	CKeyHelper();											//デフォルトコンストラクタ
	CKeyHelper(LPACCEL lpAccel);							//パラメータ付きコンストラクタ
	virtual ~CKeyHelper();									//デストラクタ

	//操作
	bool		Format(CString &str) const; 				//アクセラレータの設定による書式化

	//属性
	void		SetAccelerator(LPACCEL lpAccel);			//クラスで取り扱うアクセラレータの指定
};



/*
	CAccelerManager
	アクセラレータテーブルと個別のアクセラレータに関するメソッドを有する
	アクセラレータ管理クラス

	アクセラレータテーブルに対してコマンドIDによる検索、重複検索、
	アクセラレータの追加や削除といった機能を有します。また初期化ファイルの
	ロード・セーブ機能によってキーアクセラレータの情報を保存・再生できます。

	コンストラクタを呼び出してキーアクセラレータのハンドルを設定します。
	あとは各種関数を呼び出します。
 */
class CAccelerManager {
private:
	//メンバ変数
	HACCEL		m_hAccel;									//アクセラレータテーブルのハンドル
	LPACCEL 	m_lpAccel;									//アクセラレータ集合のポインタ
	int 		m_nAccelSize;								//アクセラレータの数

public:
	CAccelerManager(HACCEL hAccel); 						//コンストラクタ
	virtual ~CAccelerManager(); 							//デストラクタ

	int 		GetCount() const;							//テーブル内のアクセラレータの数を取得
	LPACCEL 	GetAt(int index);							//指定した番号のアクセラレータを取得(0origin)
	BOOL		FindAccelerator(UINT uiCmd, CString &str);	//指定したコマンドIDを持つアクセラレータを検索

	//戻り値は見つかったかどうか。見つかった場合には
	//引数strにキーの組み合わせを示す文字列が返される
	UINT		FindCommandID(ACCEL *pAccel);				//指定したアクセラレータとキーが一致するもののコマンドIDを返す

	//見つからない場合は0を返す
	HACCEL		DeleteAccelerator(UINT uCmd);				//指定したコマンドIDを持つアクセラレータを削除する
	HACCEL		AddAccelerator(ACCEL *lpAccel); 			//指定したアクセラレータをテーブルに追加する
	HACCEL		LoadAccelaratorState(HACCEL hAccel);		//ファイルからアクセラレータの情報をロードする

	//(minit)引数はファイルが読めない場合の代替アクセラレータテーブル?
	BOOL		SaveAccelaratorState(); 					//ファイルにアクセラレータテーブルの状態を記録する
};



/*
	CEditAccel
	ホットキーコントロールのようなテキストボックスを実現するクラス

	キーボードメッセージを解釈して入力に応じて対応したキー文字列を表示する
	テキストボックスを作成します。

 ##コメント追加途中##

 */
class CEditAccel : public CContainedWindowT < CEdit > {
private:
	BOOL	   m_bKeyDefined;
	CKeyHelper m_Helper;
	ACCEL	   m_Accel;

	void		SetAccelFlag(BYTE bFlag, BOOL bOn);

public:
	CEditAccel();

	ACCEL const *GetAccel() const;
	void		ResetKey();
	BOOL		OnTranslateAccelerator(MSG *pMsg);
};



#endif
