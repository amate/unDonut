
undonut+mod.1.48のソースファイルからWTL80を持ってきてパスを通してください

ver 26からboost::threadを使ってるので
boostを用意してください

コンパイルしたとき
>libboost~.libが見つからない
とか出たら
>bjam --toolset=vc100 link=static runtime-link=static debug release stage 
こんな感じで

もうすでにVS2010以外でコンパイル通らないと思います…
その他コンパイル通らなかったら言って


WDKとかについてるATLを使うときは
atlthunk.libがライブラリ指定されてないので、 
リンカの追加の依存ファイルに、atlthunk.libを付ける。 
もしくは↓を適当なヘッダに追加する 
#pragma comment(lib,"atlthunk.lib") 



リソースエディタで編集しても大丈夫になったはず
コメントとか消えちゃったけどresource_back.hに残してるから大丈夫かな

リソースエディタを使ってダイアログなどを編集したときは
///////////
// ICON
・・・
///////////
// Bitmap
・・・
とかに書き換わるので、この部分をコンパイルする前に戻してください
(32bit版を作るだけなら元に戻さなくてもいい）



このソースを使ったバイナリの公開とかは自由にしてください



--------------------------------------------------------------------

DebugWindowをいじってた時DebugWindowのエディットボックスで
Backspaceキーが使えなくてハマったのでメモ
どうもブラウザ(IEServer)がキーを食ってたみたいでそれがダメだったらしい
CMainFrame::PreTranslateMessage ...
hWnd = MDIGetActive();
if (bFocus == 0 && hWnd != NULL && ::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg) ) {
	return TRUE;
}
これのWM_FORWARDMSGの処理先で
if(spInPlaceActiveObject->TranslateAccelerator(lpMsg) == S_OK)
	return 1;
1が返されて
return TRUEを返したため::TraslateMessageと::DespatchMessageが呼ばれなかったのが原因
DebugWindowにフォーカスが当たっていた時もbFocus = trueとすることでこれを回避した


酷い罠に引っかかった
http://ameblo.jp/blueskyame/entry-10398978729.html

