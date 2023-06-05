#include "Board.h"
#include "header.h"

class PlayerRandom{
public:
  Board board;
  int role, n;
  PlayerRandom(int ROLE, int MAX_TURN, int N);  // 初期化
  void get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG = 0);  // 行動取得
};

class PlayerGreedy{
public:
  Board board;
  int role, width, n;
  PlayerGreedy(int ROLE, int MAX_TURN, int N);  // 初期化
  void get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG = 0);  // 行動取得
};

class PlayerHuman{
public:
  Board board;
  int role, n;
  PlayerHuman(int ROLE, int MAX_TURN, int N);  // 初期化
  void get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG = 0);  // 行動取得
};


class PlayerMCTS{
public:
  struct Child{int play_cnt;double win_cnt;vector<P> hand;PlayerMCTS *child;};
  Board board;
  int role, width, n, max_turn, play_cnt, *isEnd;
  double c;
  vector<Child*> children;
  PlayerMCTS(int ROLE, int MAX_TURN, int N);  // 初期化
  double get_score(Board &baord_, int Color);
  double simulation(Board &board_, int Color);
  double mcts(Board &board_, int Color, vector<P> mov);
  void dump();
  void get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG = 0);  // 行動取得
};


// typedef unordered_map<Board, int, Board::Hash> Map;
class PlayerAlphaBeta{
public:
  Map table;
  Board board;
  int role, width, n, max_depth, last, max_turn, prev_score, *isEnd;
  int leaf_total, add_total, add_min, add_max;
  vector<P> best;
  PlayerAlphaBeta(int ROLE, int MAX_TURN, int N);  // 初期化
  int get_score(int f);
  int alphabeta(int alpha, int beta, int color, int depth, vector<P> mov);
  void get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG = 0);  // 行動取得
};
