## 概要
第30回高専プロコン競技部門「踊って舞って回って」の府大高専のソルバーです。  

## 環境構築
動く保証はないです。

下記2つのツールをインストールする
- [MinGW-w64](http://text.baldanders.info/remark/2018/03/mingw-w64/) (threadのため)
- [GnuWin32](https://qiita.com/tokikaze0604/items/e13c04192762f8d4ec85) (makeのため)  

下記3つを順に実行する  
1. `c++/server.cpp`を`g++ -o server.exe server.cpp -lws2_32`でコンパイル・実行  
2. `Environment/Visualizer.exe`をダブルクリックで実行  
3. `c++/Interface.cpp`を`make`で実行   

※文字化けしてたら`chcp 65001`  
※QRコードのサンプルはQR_sampleにあります  
※c++版ではプレイヤーの選択は`c++/Interface.cpp`からしかできません
他設定は`c++/Interface.cpp`開いて環境設定の欄をいじってください  
あとはVisualizerで操作するだけ  

## 使用しているアルゴリズムについて  
前回はアルファベータ探索を主に使っていたが今回は探索範囲が増えたこともありアルゴリズムをモンテカルロ木探索に変更した  
モンテカルロ木探索は主に以下の３つのタイプが当てはまる分野に対して有効である  
・ランダムに打っていっても一定の手数で決着がつく  
・途中の局面での静的な評価関数を作るのが困難  
・着手数が多くて探索が困難  
この３つに今回のゲームは当てはまっており、他には囲碁などでよく用いられている  
本プログラムはプレイアウトと呼ばれるランダムにプレイする部分を特にこだわった  

## フォルダ・ファイルの説明  
- Environment: Visualizerの実行ファイルと実行するために必要なファイル  
- Visualizer: Visualizerの開発環境  
- c++: ソルバーのソースコード  
  - get_file: 運営サーバーから取ってきたjsonファイルを保存する場所  
  - post_file: 運営サーバーに送るためのファイルを保存する場所  
  - Board.cpp: ボードの管理（ボードの初期化、表示など）をする  
  - Board.h: Board.cppのヘッダーファイル  
  - CreateBoard.cpp: ゲームの最初にボードを作る（「テンプレボード」、「ランダム生成」、「jsonファイルから生成」の3つを指定できる）  
  - CreateBoard.h: CreateBoard.cppのヘッダーファイル  
  - HttpRequest.cpp: 運営サーバーと通信する  
  - HttpRequest.h: HttpRequest.cppのヘッダーファイル  
  - Interface.cpp: ゲーム全体を管理する・main関数がある  
  - JsonManagement.cpp: 「HttpRequest.cppで使うJsonファイル」と「本プログラムで使っている変数」の変換  
  - JsonManagement.h: JsonManagement.cppのヘッダーファイル  
  - Makefile: 複数のファイルを一気にコンパイル実行  
  - Player.cpp: ゲームをプレイするプレイヤー  
  - Player.h: Player.cppのヘッダーファイル  
  - ReadJson.cpp: 最新バージョンでは使っていない  
  - ReadJson.h: 最新バージョンでは使っていない  
  - client.cpp: Visualizerサーバーのソルバー側のクライアント（Visualizer側のクライアントはUnityの方で記述している）  
  - client.h: client.cppのヘッダーファイル  
  - header.h: 複数のファイル共通のヘッダーファイル  
  - server.cpp: Visualizerとソルバを通信するためのサーバー（ここでは運営サーバーとの区別のためにVisualizerサーバーと名づける）  
