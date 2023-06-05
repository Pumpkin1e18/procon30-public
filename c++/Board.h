#include "header.h"

#ifndef PBOARD
#define PBOARD

// class for hash
// 盤面判定するだけなのにBoatdを使っていては重いので軽量化したクラスを定義する
// ハッシュの値が違うと==であっても別物とみなされる
class Compact{
public:
  uint seed;  // size%8も含めたseed値を渡すこと: 16depthの以上は対応してない
  vector<int> pos;
  // zip 0～(7-1): 赤のタイルがあるところのbitが立っている, 7～(14-1): 青のbitが立っている
  vector<ll> zip;
	Compact(){}
  void update(vi &Pos, vector<ll> &Zip, uint Seed){
    pos = Pos;
    zip = Zip;
    seed = Seed;
  }
  bool operator==(const Compact& right) const{      // 同じか判定（深さは見れない）
    const Compact &left = *this;
    rep(i,left.pos.size())if(left.pos[i] != right.pos[i])return false;
    rep(i,left.zip.size())if(left.zip[i] != right.zip[i])return false;
    return true;
  }
  struct Hash{        // 大雑把な盤面の固有値を返す
      typedef size_t result_type;
      size_t operator()(const Compact& key) const{
        return key.seed;
      }
  };
};

class Board{
public:
  struct History{uint eigenvalue;vi pos, score;vector<P> past;};
  // struct History{int pos[4], score[2], eigenvalue;P past[8];};
  stack<History> history;
  Compact com;
  int h, w, n;
  int start_turn;   // Playerの処理が呼ばれた時のターンを保存しとく用
  double ave_num_abs;    // 盤面のタイルの絶対値平均
  double ave_num_pos;    // (正のタイルの重み) / (h*w)
  uint eigenvalue, rand_max;        // eigenvalue: 局面に対する固有値
  vector<int> pos, number, color, used, dv4, dv9, score;
  vector<ll> zip;
  unordered_map<int, int> color2, color3, color4;
  // random_table 0: 赤のタイル, 1: 青のタイル, 2+i: i人目の位置 の 2+2*n の要素のランダム値を持つ
  vector<vector<uint>> random_table;
  
  Board();
  Board(int H, int W, vi Pos, vi num, vi Color);
  void init(int H, int W, vi Pos, vi num, vi Color);
  void print_board();
  void print_zip();
  int get_turn();
  
  void set_zip_bit(int Pos, int Color);
  void set_color(int Pos, int Color);
  int dfs(int now);
  int calc_score(int Color);
  double adjacent(int now, int Color);
  double calc_value(int nxt, int Color);
  vector<vector<P>> move_sort(int width, int Color, int isBeam = 0);
  void move(const vector<P> &vec);
  vector<P> unite_vec(vector<P> &vec1, vector<P> &vec2, bool rev);
  void undo(int p);
  
  // 計測用
  void brain(vi vec[2]);
  
  // Unityとの通信用
  string get_init_str();
  string get_state_str();
  string get_predict_str(const vector<P> vec[2]);
  void move_str(string str);
  
  bool operator==(const Board& right) const{
    const Board &left = *this;
    rep(i,2*n)if(left.pos[i] != right.pos[i])return false;
    rep(i,2*7)if(left.zip[i] != right.zip[i])return false;
    if(left.history.size() != right.history.size())return false;
    return true;
  }
  struct Hash{
      typedef size_t result_type;
      size_t operator()(const Board& key) const{
        uint seed = key.eigenvalue;
        seed += key.history.size()%8*key.rand_max;
        return seed;
      }
  };
};

// ハッシュの衝突があったときに前回の動きをメモっておく
struct Memo{int value = 0;vector<vector<P>> v[2];};
typedef unordered_map<Compact, Memo, Compact::Hash> Map;
// typedef unordered_map<Board, int, Board::Hash> Map;


#endif