#include "header.h"
#include "HttpRequest.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>


#pragma once


/*
フィールド情報
・ width：integer（フィールド横幅）
・ height：integer（フィールド縦幅）
・ points：integer 二次元配列（各マスの点数）
・ startedAtUnixTime：integer（試合が始まった Unix 時間）
・ turn：integer（ターン）
・ tiled：integer 二次元配列（タイル配置状況）
・ teams：オブジェクト配列（各チーム状況）
  ・ teamID：integer（チーム ID）
  ・ agents：オブジェクト配列，（各エージェント状況）
    ・agentID：integer（エージェント ID）
    ・x：integer（ｘ座標）
    ・y：integer（ｙ座標
  ・ tilePoint：integer（タイルポイント）
  ・ areaPoint：integer（領域ポイント）
・ actions：オブジェクト配列（各行動履歴）
  ・ agentID：integer（エージェント ID）
  ・ type：string（行動の種類，“move”：移動, “remove”：除去, “stay”：停留）
  ・ dx：integer（行動のｘ方向の向き，-1：左，0：中，1：右）
  ・ dy：integer（行動のｙ方向の向き，-1：上，0：中，1：下）
  ・ turn：integer（行動ターン）
  ・ apply：integer（行動の適応状況，-1：無効，0：競合，1：有効）
*/


class JsonManagement{
  boost::property_tree::ptree pt;
  HttpRequest communication;
  // jsonファイルの名前と番号
  string game_filename;
  string state_filename;
  int game_filenum;

  // int型１つ読み込む
  int get_int(const boost::property_tree::ptree& info, string str);

  // string型１つ読み込む
  string get_string(const boost::property_tree::ptree& info, string str);

public:
  bool ng;
  // game info
  int id, intervalMillis, teamID, turnMillis, turns;
  string matchTo;
  // state_info
  int width, height, playerNum, startedAtUnixTime, turn;
  vector<int> number, color;
  TeamInfo team[2];

  // 初期化
  JsonManagement();

  // 初期化
  JsonManagement(string httphost, unsigned short port, string tokenname, int matches_id);

  // 通信を切断する
  void disconnect_server();

  // 試合事前情報を表示する
  void print_game_info();

  // ボード情報を表示する
  void print_state_info();

  // 試合事前情報取得
  void read_game_info();

  // ボード情報を読み込む
  void read_state_info(int state_turn);

  // 行動を送信する
  void post_action_info(int a, int b);

  // pingを送信する
  void get_ping();
};
