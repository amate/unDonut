ミニット氏配布の PluginMaking のソースを、デバッグ ＆ unDonut+mod 用
に修正したものです。

LinkExtractBar.dll	リンク抽出ダイアログ(ツールバー)
TmpFavGroup.dll		お気に入りグループのスナップショット (エクスプローラバー)
Memo.dll		メモ帳(エクスプローラバー)
TabMoveBar.dll		タブ移動ボタン(ツールバー)


コンパイルは、
*_7.10.sln /.vcproj 	vs2003用   マルチバイト(MB)文字版専用
*_8.00.sln /.vcproj 	vs2005用
*_9.00.sln /.vcproj 	vs2008用

で、主に 9.00 / Visual Sturido 2008 で行っています。
MB版に関しては、可能ならば7.10でコンパイルしてます。
8.00 / vs2005 は2008のsln,vcprojのバージョン部分を書き換えてるだけです。
現状、コンパイルチェックしていません。

そうはいっても、ソース的には以前の版でも問題ないハズ?なので、
vc6のdswをvs?で読み込んで自分の環境に設定しなおせばコンパイル
できるかもしれません。

通常dllはExecuteEnv\plugin(64) に生成します。
ただし7.10/vs2003用と Release-MB/Debug-MBの場合は
ExecuteEnv-MB\plugin(64)に生成します。
※LinkExtractBar.dllは面倒で、TabMoveBar.dllは必要ないので、
　unicode化してません。

一応、ソースは、展開してできたPluginMaking/フォルダが、unDonut+modの
ソース環境のsrc_plugin/フォルダ直下に置いてある状態として、
.dllの出力先をExecuteEnv/plugin(64)し、参照するwtl80/フォルダや
unDonut.exeの位置を相対パスで指定しています。
※LinkExtractBarで必要になるboostに関しては、私の環境での絶対パス
このへん、各自の環境で適時修正してください。


修正内用については、各サブフォルダのreadme.txtをみてください。
また、ミニット氏の元々の*.txtのほうも使い方やソース解説等があり
ますのでみておいてください。
(ミニット氏の元々のreadme.txtはファイルを適当に変名しています。)

※私が弄ったあたりには//+++ で始まるコメントがつけてあります。
が、名前の置換等の場合は、何もしるしをつけていない場合もありますし、
しるしを付け忘れている場合もあるので...



2008-07-25 +mod
