#include "header.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>



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


class ReadJson{
  bool ng;
  boost::property_tree::ptree pt;
  
  // int型１つ読み込む
  int get_int(const boost::property_tree::ptree& info, string str);
  
public:
  int width, height, playerNum, startedAtUnixTime, turn;
  vector<int> number, color;
  TeamInfo team[2];
  
  // 初期化
  ReadJson();
  
  // ボード情報を表示する
  void print_info();
  
  // ボード情報を読み込む
  void read_info(string);
};




