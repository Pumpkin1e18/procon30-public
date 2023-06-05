#include "Board.h"
#include "Player.h"
#include "header.h"

// Random
/*--------------------------------------------------------------------------*/
PlayerRandom::PlayerRandom(int ROLE, int MAX_TURN, int N){  // 初期化
  role = ROLE;n = N;
}

void PlayerRandom::get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG){  // 行動取得
  srand((unsigned int)time(NULL));
  vector<P> v;
  rep(i,n)v.push_back(P(rand()%9, 0));
  vec = v;
  return;
}
/*--------------------------------------------------------------------------*/




// Greedy
/*--------------------------------------------------------------------------*/
PlayerGreedy::PlayerGreedy(int ROLE, int MAX_TURN, int N){  // 初期化
  role = ROLE;width = 5;n = N;
}

void PlayerGreedy::get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG){  // 行動取得
  srand((unsigned int)time(NULL));
  vector<vector<P>> v = BOARD.move_sort(width, role);
  rep(i,v.size()){
    printf("Greedyが考える%d番目の手 Player ", i+1);
    rep(j,v[i].size())printf("%d:(%d,%d), ", j+1, v[i][j].fi, v[i][j].se);
    printf("\n");
  }
  vec = v[rand()%width];      // ランダムで上位から１つ選ぶ
  return;
}
/*--------------------------------------------------------------------------*/



// Human
/*--------------------------------------------------------------------------*/
PlayerHuman::PlayerHuman(int ROLE, int MAX_TURN, int N){  // 初期化
  role = ROLE;n = N;
}

void PlayerHuman::get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG){ // 行動取得
  if(CG == GUI){vec = vector<P>(n, P(-1, -1));return;}
  string str;
  while(true){
    printf("input:");getline(cin, str);
    str += " ";
    vector<int> v;
    int num = 0;
    if(str.substr(0, 4) == "undo" or str.substr(0, 4) == "UNDO"){   // undo
      repi(i,4,str.size()){
        if(0 <= str[i]-'0' and str[i]-'0' <= 9)num = num*10+str[i]-'0';
        if(200 < num)break;
      }
      BOARD.undo(max(num, 1));
      BOARD.print_board();
      continue;
    }
    rep(i,str.size())if(0 <= str[i]-'0' and str[i]-'0' <= 9){   // put
      if(v.size() == 2*n)break;
      v.push_back(str[i]-'0');
    }
    bool ok = true;
    if(v.size() == n){
      rep(i,n){
        if(v[i] < 0 or 8 < v[i]){ok = false;break;}
        vec.push_back(P(v[i], 0));
      }
    }else if(v.size() == 2*n){
      rep(i,n)v[i*2+1] = (v[i*2+1] != 0);
      rep(i,n){
        if(v[i*2] < 0 or 8 < v[i*2]){ok = false;break;}
        vec.push_back(P(v[i*2], v[i*2+1]));
      }
    }else ok = false;
    if(ok)break;
    vec.resize(0);
    printf("invalid operation\n");
  }
  return;
  
}
/*--------------------------------------------------------------------------*/




// ctrl-cmd-h	highlight-selected:toggle	atom-text-editor

// MCTS
// 最適パラメータ
// 500 <= playout < 1000, playout = 500 確定
// 5 <= width <= 10, width = 5, 8
// 10 <= depth, depth = 20 ほぼ確定
/*--------------------------------------------------------------------------*/
PlayerMCTS::PlayerMCTS(int ROLE, int MAX_TURN, int N){  // 初期化
  role = ROLE;max_turn = MAX_TURN;
  width = 8;n = N;play_cnt = 0;
  c = 0.5;      // 定数
}

double PlayerMCTS::get_score(Board &board_, int Color){
  int point[2] = {board_.calc_score(Color), board_.calc_score(-Color)};
  int score = point[0] - point[1];
  if(score > 0)return 1.0;
  if(score == 0)return 0.5;
  if(score < 0)return 0;
  return 0;
}

double PlayerMCTS::simulation(Board &board_, int Color){
  while(board_.get_turn() < max_turn){
    int alpha = 0;
    if(role == RED)alpha = 20;
    if(role == BLUE)alpha = 20;
    if(board_.get_turn() >= board_.start_turn+alpha)break;
    int red_idx = 0, blue_idx = 0;
    vector<int> table{0};
    rrep(i,width)table.push_back(table[i-1]*2);   // 変更中
    int red_rand = rand()%((width)*(width+1)/2);
    int blue_rand = rand()%((width)*(width+1)/2);
    rep(i,width){if(red_rand < table[i+1])red_idx = i;}
    rep(i,width){if(blue_rand < table[i+1])blue_idx = i;}
    vector<P> hand1 = board_.move_sort(width, RED)[(width-1)-red_idx];
    vector<P> hand2 = board_.move_sort(width, BLUE)[(width-1)-blue_idx];
    vector<P> hand = board_.unite_vec(hand1, hand2, false);
    /*
    vector<P> hand;
    rep(i,2*n)hand.push_back(P(rand()%9, 0));
    */
    board_.move(hand);
  }
  return get_score(board_, Color);
}

double PlayerMCTS::mcts(Board &board_, int Color, vector<P> mov){
  double res;
  int idx;
  // Selection: 選択
  if(children.size() != 0){
    idx = -1;double mx = -inf;
    rep(i,children.size()){
      if(children[i]->play_cnt == 0){idx = i;break;}
      int p = children[i]->play_cnt, s = play_cnt;
      double w = children[i]->win_cnt;
      double ucb = w/p + c*sqrt(2.0*log(s)/p);
      if(ucb > mx){mx = ucb;idx = i;}
    }
    if(role == Color)res = children[idx]->child->mcts(board_, -Color, children[idx]->hand);
    else{
      vector<P> hand = board_.unite_vec(mov, children[idx]->hand, role == BLUE);
      board_.move(hand);
      if(board_.history.size() == max_turn)res = get_score(board_, Color);
      else res = children[idx]->child->mcts(board_, -Color, vector<P>(1, P(0, 0)));
    }
  }
  // Expantion: 展開
  if(children.size() == 0){
    int alpha = 0;    // 初手だけ探索の幅をalpha増やす
    if(board_.start_turn == board_.get_turn())alpha = 2;
    vector<vector<P>> hands = board_.move_sort(width+alpha, Color);
    rep(i,width+alpha){
      PlayerMCTS *player = new PlayerMCTS(role, max_turn, n);
      Child *child = new Child{0, 0.0, hands[i], player};
      children.push_back(child);
    }
    
    idx = 0;    // 強い手から探索
    if(Color == role)res = children[idx]->child->mcts(board_, -Color, hands[idx]);
    else{
      vector<P> hand = board_.unite_vec(mov, children[idx]->hand, role == BLUE);
      board_.move(hand);
      if(board_.history.size() == max_turn)res = get_score(board_, Color);
      else res = simulation(board_, Color);      // Simulation: シミュレーション
    }
  }
  // Backpropagation: 伝搬
  children[idx]->play_cnt++;
  children[idx]->win_cnt += res;
  play_cnt++;
  return 1.0-res;
}

void PlayerMCTS::dump(){
  if(children.empty())return;
  rep(i,children.size()){
    children[i]->child->dump();
    delete children[i]->child;children[i]->child = NULL;
    delete children[i];children[i] = NULL;
  }
  return;
}

void PlayerMCTS::get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG){  // 行動取得
  srand((unsigned int)time(NULL));
  vec = vector<P>(n, P(-1, -1));
  clock_t start = clock();
  rep(i,500){
    Board b = BOARD;
    b.start_turn = b.get_turn();
    mcts(b, role, vector<P>(1, P(0, 0)));
  }
  int mx = -1;vector<P> best;
  rep(i,children.size())if(mx < children[i]->play_cnt){
    best = children[i]->hand;
    mx = children[i]->play_cnt;
  }
  vec = best;
  // 表示
  rep(i,children.size()){
    printf("%3d回探索  ", children[i]->play_cnt);
    printf("勝率:%-8lf %%  ", 100.0*children[i]->win_cnt/children[i]->play_cnt);
    rep(j,n)printf("%d:(%d,%d), ", j+1, children[i]->hand[j].fi, children[i]->hand[j].se);
    printf("\n");
  }
  printf("MCTS finished in ");cout << clock()-start << "ms" << endl;
  cout << endl;
  dump();   // メモリ解放
  children.resize(0);
  return;
}
/*--------------------------------------------------------------------------*/



// AlphaBeta
/*--------------------------------------------------------------------------*/
PlayerAlphaBeta::PlayerAlphaBeta(int ROLE, int MAX_TURN, int N){  // 初期化
  role = ROLE;max_turn = MAX_TURN;n = N;
  prev_score = 0;
  width = 8;
  max_depth = 8;  // ここは実質関係ない
}

// ゲーム自体が終わっていたらf = 0, max_depthだったらf = 1
int PlayerAlphaBeta::get_score(int f){
  leaf_total++;
  int point[2] = {board.calc_score(role), board.calc_score(-role)};
  int res = point[0]-point[1];
  if(f == 0)return res;
  
  return res;
  
  // int add[2] = {}, dis[2] = {}, h = board.h, w = board.w, pos[4];
  // rep(i,4)pos[i] = board.pos[i];
  // rep(i,2){dis[i] += abs(pos[2*i]/w-pos[2*i+1]/w)+abs(pos[2*i]%w-pos[2*i+1]%w);}
  // // 周辺の状況を考慮する
  // rep(i,4)rep(y,3)rep(x,3){
  //   int Color = board.color4[i];
  //   int yy = (pos[i]/w)+(y-1), xx = (pos[i]%w)+(x-1);
  //   if(yy < 0 or h <= yy or xx < 0 or w <= xx){
  //     if(board.history.size() < board.h*board.w/6)add[i/2] += 1;
  //   }else if(board.color[yy*w+xx] == Color)add[i/2] += -2;
  //   else add[i/2] += 1;
  // }
  // add[role==BLUE] += (point[0]-prev_score)/2;
  // 
  // int Dis = (role == RED ? dis[0]-dis[1] : dis[1]-dis[0]);
  // int Add = (role == RED ? add[0]-add[1] : add[1]-add[0]);
  // int sup = (Add+Dis/2)/4;
  // add_min = min(add_min, sup);
  // add_max = max(add_max, sup);
  // // add_min = min(add_min, res);
  // // add_max = max(add_max, res);
  // 
  // add_total += sup;
  // return res+sup;
}

int PlayerAlphaBeta::alphabeta(int alpha, int beta, int color, int depth, vector<P> mov){
  if(color == role){
    if(table.find(board.com) != table.end()){
      int value = table[board.com].value;
      if(depth and value != inf and table[board.com].v[0].size())return value;
    }
    if((int)board.history.size() == max_turn)return table[board.com].value = get_score(0);
    if(depth == max_depth)return table[board.com].value = get_score(1);
  }
  
  int idx = 0;
  vector<vector<P>> v;
  Memo &memo = table[board.com];
  if(color == role and memo.v[0].size() == 0){
    memo.v[0] = board.move_sort(width, color);
    memo.v[1] = board.move_sort(width, -color);
  }
  v = memo.v[color==-role];
  
  if(color == role){  // 自分の手番
    rep(i,width){
      if(*isEnd)return alpha;
      int res = alphabeta(alpha, beta, -color, depth+1, v[i]);
      if(depth == 0 and alpha < res)best = v[i];
      if(alpha < res){alpha = res;idx = i;}
      if(alpha >= beta)break;
    }
    drep(j,idx,1)swap(memo.v[0][j-1], memo.v[0][j]);
    memo.value = alpha;
    if(depth == 0){
      /*
      rep(i,width){
        cout << i << "  ";
        rep(j,v[i].size())printf("%d:%d,  ", (role == RED ? j+1 : n+j+1), v[i][j].fi);
        cout << endl;
      }
      */
      printf("alpha = %d\n", alpha);
    }
    return alpha;
  }else{  // 相手の手番
    rep(i,width){
      if(*isEnd)return beta;
      vector<P> vec = board.unite_vec(mov, v[i], role == BLUE);
      board.move(vec);
      int res = alphabeta(alpha, beta, -color, depth+1, vector<P>(1, P(0, 0)));
      board.undo(1);
      if(beta > res){beta = res;idx = i;}
      if(alpha >= beta)break;
    }
    drep(j,idx,1)swap(memo.v[1][j-1], memo.v[1][j]);
    return beta;
  }
}

void PlayerAlphaBeta::get_act(Board &BOARD, vector<P> &vec, int &IsEnd, int CG){ // 行動取得
  board = BOARD;
  table.clear();
  isEnd = &IsEnd;
  leaf_total = add_total = 0;
  add_min = inf, add_max = -inf;
  vec = vector<P>(n, P(-1, -1));
  
  prev_score = board.calc_score(role);
  int alpha = -inf, beta = inf;
  clock_t start = clock();
  max_depth = 6;
  alphabeta(alpha, beta, role, 0, vector<P>(1, P(0, 0)));
  
  // for(auto it = table.begin();it != table.end();++it)it->se.value = inf;
  // max_depth = 8;
  // alphabeta(alpha, beta, role, 0, vector<P>(1, P(0, 0)));
  // 
  // for(auto it = table.begin();it != table.end();++it)it->se.value = inf;
  // max_depth = 10;
  // alphabeta(alpha, beta, role, 0, vector<P>(1, P(0, 0)));
  table.clear();
  vec = best;
  
  // 表示
  // printf("add_min: %d add_max: %d\n", add_min, add_max);
  // printf("add_average: %d\n", add_total/leaf_total);
  printf("AlphaBeta finished in ");cout << clock()-start << "ms" << endl;
  return;
}
/*--------------------------------------------------------------------------*/

