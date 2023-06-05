#include "header.h"
#include "JsonManagement.h"

// Memo
// ターン数(30~60)
// マスの数(100~400)
// 点数(-16~16)
// 1ターンの時間(5~15)
// テーブル(180cmx45cmx高さ70cm)
// 公開フィールド → 非公開フィールド


// ボード情報を格納する構造体
struct BoardInfo{int h, w, n;vi pos, number, color;};

namespace create{
  // ボードを作る
  // 引数(k = 0): サンプルボード
  // 引数(k = 1): ランダムボード
  // 引数(k = 2): JSONファイルから入力
  BoardInfo get_board(int k, int deg, int col, int rid, int bid, string hn, unsigned short p, string tn, int id, int turn);
  
  // 時計回りに90度回転する
  void rotate_board(int &h, int &w, vi &pos, vi &number, vi &color);
};




