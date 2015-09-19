/* =========================================
		unDonut
 =========================================== */

■はじめに
IEコンポーネントを利用したウェブブラウザです

■免責
作者(原著者＆改変者)は、このソフトによって生じた如何なる損害にも、
修正や更新も、責任を負わないこととします。
使用にあたっては、自己責任でお願いします。
 
何かあれば下記のURLにあるメールフォームにお願いします。
http://www31.atwiki.jp/lafe/pages/33.html


画像の一部に「VS2010ImageLibrary」の一部を使用しています。


■ビルドについて
Visual Studio 2013 が必要です
Express版なら別途ATLが必要です、WDKなどから手に入れてください

boost(1.54~)が必要です
serializationとthreadを利用しています

 Boostライブラリのビルド方法
 https://sites.google.com/site/boostjp/howtobuild
コマンドライン
// x86
b2.exe install -j4 --prefix=lib toolset=msvc-14.0 runtime-link=static --with-thread --with-date_time --with-serialization
// x64
b2.exe install -j4 --prefix=lib64 toolset=msvc-14.0 runtime-link=static address-model=64 --with-thread --with-date_time --with-serialization


