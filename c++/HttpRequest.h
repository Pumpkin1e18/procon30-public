#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>

#define BUF_LEN 1024                      /* バッファのサイズ */
using namespace std;



class HttpRequest{
  /* IP アドレス、ポート番号、ソケット 出力ファイル名 */
  unsigned short port;
  string httphost;
  string tokenname;
  string filename;
  ofstream writing_file;
  /* サーバー接続用変数 */
  int dstSocket;
  int result;          //戻り値
  struct sockaddr_in dstAddr;   // sockaddr_in 構造体
  /* 各種パラメータ */
  char toSendText[BUF_LEN];
  char buf[BUF_LEN];
  int read_size;
  /* データ出力用変数 */
  int indent_cnt;
  bool start;

public:
  // 初期化
  HttpRequest();

  // 初期化
  HttpRequest(string httphost_, unsigned short port_, string tokenname_);

  // サーバーに接続する
  void connect_server();

  // サーバーの接続を切る
  void disconnect_server();

  // jsonファイルに書き込む
  void write_json(string buf, string data_type);

  // 試合事前情報を取得する
  void get_game_info(string file_name);

  // ボード情報を取得する
  void get_state_info(string file_name, int matches_id);

  // 行動を送信する
  void post_action_info(int matches_id, string file_name);

  // pingを送信する
  void get_ping();
};
