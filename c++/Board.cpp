#include "Board.h"
#include "header.h"

// [0, (2^32)-1] の一様分布整数を生成
uint32_t get_rand() {
    static mt19937 mt32((uint)time(NULL));
    return mt32();
}

// 初期化
Board::Board(){
  // pass
}

// 初期化
Board::Board(int H, int W, vi Pos, vi num, vi Color){
  init(H, W, Pos, num, Color);
}

// 初期化処理
void Board::init(int H, int W, vi Pos, vi num, vi Color){
  h = H;w = W;n = Pos.size()/2;used.resize(h*w);random_table.resize(2+2*n);
  pos = Pos;number = num;color = Color;
  eigenvalue = 0;rand_max = 1<<(32-3);    // eigenvalueに何手目%8という情報を付け足してseedになる
  zip.assign(2*7, 0);
  rep(i,2+2*n){rep(j,h*w)random_table[i].push_back(get_rand()%rand_max);}
  // 盤面情報初期化
  score.assign(2, 0);
  rep(i,H*W){
    set_zip_bit(i, color[i]);
    if(color[i] == RED)score[0] += number[i];
    if(color[i] == BLUE)score[1] += number[i];
  }
  com.update(pos, zip, eigenvalue+history.size()%8*rand_max);
  // 変換変数初期化
  rep(i,n)color4[i] = RED;
  rep(i,n)color4[i+n] = BLUE;
  color2[RED] = 0;color2[BLUE] = 1;
  color3[EMPTY] = 0;color3[RED] = 1;color3[BLUE] = 2;
  vector<int> DV4{-W, -1, 1, W}, DV9{-W-1, -W, -W+1, -1, 0, 1, W-1, W, W+1};
  dv4 = DV4;dv9 = DV9;
  ave_num_abs = 0, ave_num_pos = 0;
  rep(i,h*w){
    ave_num_abs += abs(number[i]);
    if(number[i] > 0)ave_num_pos += number[i];
  }
  ave_num_abs = ave_num_abs / (h*w);
  ave_num_pos = ave_num_pos / (h*w);
  cout << "ave_num_abs = " << ave_num_abs << endl;
  cout << "ave_num_pos = " << ave_num_pos << endl;
}

// ボードの表示
void Board::print_board(){
  vector<int> disp = color;
  rep(i,n)disp[pos[i]] = PLAYER_RED;
  rep(i,n)disp[pos[i+n]] = PLAYER_BLUE;
  rep(i,h){
    rep(i,w)printf("------");printf("\n");
    rep(j,w){
      printf("%3d|", number[i*w+j]);
      if(disp[i*w+j] == RED)printf("■");
      else if(disp[i*w+j] == BLUE)printf("□");
      else if(disp[i*w+j] == PLAYER_RED)printf("●");
      else if(disp[i*w+j] == PLAYER_BLUE)printf("〇");
      else printf("　");
    }
    printf("\n");
  }
  rep(i,w)printf("------");printf("\n");
  int point[2] = {calc_score(RED), calc_score(BLUE)};
  int area[2] = {point[0]-score[0], point[1]-score[1]};
  printf("■RED■:%d (%d), ", point[0], area[0]);
  printf("□BLUE□:%d (%d), ", point[1], area[1]);
  printf("DIFF:%d (%d)\n", abs(point[0]-point[1]), abs(area[0]-area[1]));
  rep(i,2*n)printf("player%d (%d, %d)\n", i+1, pos[i]/w+1,pos[i]%w+1);
  printf("\n");
}

// bitの表示
void Board::print_zip(){
  vector<int> v;
  vector<ll> tmp = zip;
  rep(i,7)rep(j,3*w){
    v.push_back(tmp[i]%2);
    tmp[i] = tmp[i]/2;
  }
  rep(y,h){
    rep(x,w)printf("%d", v[y*w+x]);
    printf("\n");
  }
}

// 現在のターンを返す
int Board::get_turn(){
  return (int)history.size();
}

// zipを更新する
void Board::set_zip_bit(int Pos, int Color){
  int id = (Pos/w)/3, shift = Pos-w*(id*3);
  ll p = 1LL<<shift;
  if(Color == RED)zip[id] += p;
  else if(Color == BLUE)zip[7+id] += p;
  else if(Color == EMPTY){
    if(zip[id] & p)zip[id] -= p;
    if(zip[7+id] & p)zip[7+id] -= p;
  }
}

// マスに色をセットする
void Board::set_color(int Pos, int Color){
  // 色を付ける場合は無色にしておく
  if(Color == RED and color[Pos] != RED){
    eigenvalue ^= random_table[0][Pos];
    score[0] += number[Pos];
    set_zip_bit(Pos, RED);
  }else if(Color == BLUE and color[Pos] != BLUE){
    eigenvalue ^= random_table[1][Pos];
    score[1] += number[Pos];
    set_zip_bit(Pos, BLUE);
  }else if(Color == EMPTY and color[Pos] != EMPTY){
    if(color[Pos] == RED){
      eigenvalue ^= random_table[0][Pos];
      score[0] -= number[Pos];
    }else{
      eigenvalue ^= random_table[1][Pos];
      score[1] -= number[Pos];
    }
    set_zip_bit(Pos, EMPTY);
  }
  color[Pos] = Color;
}

// 領域ポイントを数える
int Board::dfs(int now){
  if(used[now])return 0;
  used[now] = 1;
  int sum = 0, flag = 0;
  rep(i,4){
    int nxt = now+dv4[i];
    if(nxt < 0 or h*w <= nxt or abs(nxt%w-now%w) > 1){flag = 1;continue;}
    int res = dfs(nxt);
    if(res == -1)flag = 1;
    sum += res;
  }
  if(flag)return -1;
  return sum+abs(number[now]);
}

// 得点を数える
int Board::calc_score(int Color){
  int add = 0;
  fill(used.begin(), used.end(), 0);
  rep(i,h*w){if(color[i] == Color)used[i] = 1;}
  rep(i,h*w){if(!used[i])add += max(0, dfs(i));}
  int point = (Color == RED ? score[0] : score[1]);
  return point+add;
}

// 移動先周りのプレイヤーを考慮する
double Board::adjacent(int nxt, int Color){
  double value = 0;
  // 相手チームのプレイヤーとの関係
  int base = (Color == RED ? n : 0);
  rep(i,n){
    int dx = abs(nxt%w-pos[base+i]%w);
    int dy = abs(nxt/w-pos[base+i]/w);
    if(dx == 0 and dy == 0){    // 移動先に相手がいたとき
      int my_color_cnt = 0;
      rep(j,9){
        int nxt2 = nxt+dv9[i];
        if(nxt2 < 0 or h*w <= nxt2 or abs(nxt2%w-nxt%w) > 1)continue;
        if(color[nxt2] == Color)my_color_cnt++;
      }
      if(my_color_cnt == 1)value += -5;      // その相手の周りに自分カラーが1つのとき
      else value += -40;      // その相手のまわりに自分カラーが2つ以上あったとき
    }else if(max(dx, dy) <= 1)value += 2;   // 移動先近くに相手がいたら加点
  }
  // 自分チームのプレイヤーとの関係
  base = (Color == RED ? 0 : n);
  rep(i,n){
    int dx = abs(nxt%w-pos[base+i]%w);
    int dy = abs(nxt/w-pos[base+i]/w);
    if(max(dx, dy) <= 2)value += -3;   // 移動先近くに仲間がいたら減点
    if(max(dx, dy) <= 1)value += -3;   // 移動先近くに仲間がいたら減点
    if(dx == 0 and dy == 0)value +=  -30;   // 移動先に仲間がいたら減点
  }
  return value;   // 移動先近くに誰もいなかったら加点なし
}

// 移動先周りの得点を考慮する
double Board::calc_value(int nxt, int Color){
  double value = 0;   // 移動先周辺の正の値の合計が入る
  rep(i,9){
    int nxt2 = nxt+dv9[i];
    if(nxt2 < 0 or h*w <= nxt2 or abs(nxt2%w-nxt%w) > 1)value += -3;    // 盤面外なら減点
    else if(nxt2 == nxt)continue;
    else if(color[nxt2] == Color)value += -1;   // 自分の陣地だったら減点
    else if(number[nxt2] > 0)value += number[nxt2]*0.7;
    else if(number[nxt2] < 0)value += number[nxt2]/5.0;
  }
  return value/5;
}

// 次の1手をソートする
vector<vector<P>> Board::move_sort(int width, int Color, int isBeam){
  typedef pair<double, vector<int>> Pv;
  typedef pair<double, int> Pd;
  priority_queue<Pv> q;
  vector<vector<Pd>> mov(n, vector<Pd>(9));     // mov[i][j] = i人目のj番目の動きの価値
  vector<vector<P>> res;       // res[i][j] = i番目に良い指し手のj人目の動き
  vector<int> now;
  rep(i,n)now.push_back(pos[color2[Color]*n+i]);
  // 価値代入
  rep(i,n)rep(j,9){
    int b1 = (Color == RED ? 0 : n), b2 = (-Color == RED ? 0 : n);
    int nxt = now[i]+dv9[j], id = b1+i;double value = 0;
    if(nxt < 0 or h*w <= nxt or abs(nxt%w-now[i]%w) > 1){   // 移動先が盤面外のとき
      mov[i][j] = Pd(-100, j);continue;
    }
    if(nxt != now[i]){
      value += adjacent(nxt, Color);      // 移動先周りのプレイヤーを考慮する
      value += calc_value(nxt, Color);    // 移動先周りの得点を考慮する
    }
    // rep(k,n)if(isBeam and nxt == pos[b2+k])value += -20;
    if(color[nxt] == -Color)value += number[nxt]+3;      // 移動先が相手の色のとき
    else if(color[nxt] == EMPTY)value += number[nxt];       // 移動先がEMPTYのとき
    else if(now[i] == nxt)value += -15;          // 動かないとき
    else if(number[nxt] < 0)value += -number[nxt]+5;        // 自分の色かつ負のとき
    else value += -2;        // 自分の色かつ負でないとき
    mov[i][j] = Pd(value, j);
  }
  // 価値の降順で取り出す
  rep(i,n)sort(mov[i].begin(), mov[i].end(), greater<Pd>());
  double value0 = 0;vi index0(9, 0);
  rep(i,n)value0 += mov[i][0].fi;
  q.push(Pv{value0, index0});
  // 価値の降順で width 個取り出す
  while(res.size() < width){
    Pv p = q.top();q.pop();
    int value = p.fi;vi index = p.se;
    vi nxt(n), rev(n);          // nxt: 次の移動方向, rev: ひっくり返すかどうか
    unordered_map<int, vector<int>> same_nxt;
    bool add_flag = true;
    // 次行くところが誰ともかぶっていなかったらresに追加する
    rep(i,n){
      int v = mov[i][index[i]].se;
      nxt[i] = now[i]+dv9[v];
      same_nxt[nxt[i]].push_back(i);
      if(same_nxt[nxt[i]].size() > 1)add_flag = false;
      rev[i] = (color[nxt[i]] == Color and number[nxt[i]] < 0);
      if(-ave_num_pos < (double)number[nxt[i]])rev[i] = 0;
    }
    
    // 仲間同士での衝突があったときに衝突をさけてpriority_queueに入れる
    if(add_flag == false){
      vector<vector<int>> sel;vi base;
      int mx = 1;
      // 衝突を分解する
      for(auto x: same_nxt){
        if(x.se.size() == 1)continue;
        sel.push_back(x.se);
        base.push_back(x.se.size());
        mx *= x.se.size();
      }
      rep(bit,mx){
        vi index_cpy = index;
        int bit_cpy = bit, b = 1;
        double value_nxt = value;
        // 衝突のグループがsel.size()個あったときに、それぞれの衝突について1人だけ要望を通してやる
        rep(i,sel.size()){
          int idx = (bit_cpy%(b*base[i]))/b;
          int element_fixed = sel[i][idx];
          // 選ばれた1人以外は次に良い手を候補にする
          rep(j,base[i]){
            if(sel[i][j] == element_fixed)continue;
            int element_mov = sel[i][j];
            value_nxt -= mov[element_mov][index[element_mov]].fi;
            value_nxt += mov[element_mov][index[element_mov]+1].fi;
            index_cpy[element_mov]++;
          }
          bit_cpy -= b*idx;
          b *= base[i];
        }
        q.push(Pv{value_nxt, index_cpy});
      }
    }
    // 仲間同士での衝突がなかったときは普通に遷移する
    if(add_flag == true){
      vector<P> res_i;
      rep(i,n)res_i.push_back(P(mov[i][index[i]].se, rev[i]));
      res.push_back(res_i);
      // 次の候補をpriority_queueに入れる
      rep(i,n){
        if(index[i] == 8)continue;
        double value_nxt = value;
        value_nxt -= mov[i][index[i]].fi;
        value_nxt += mov[i][index[i]+1].fi;
        index[i]++;
        q.push(Pv{value_nxt, index});
        index[i]--;
      }
    }
    
  }
  return res;       // vector<vector<P>> res;
}

// 行動
void Board::move(const vector<P> &vec){
  History his{eigenvalue, pos, score};
  vi nxt(2*n), rev(2*n, 0);
  unordered_map<int, int> cnt;
  rep(i,2*n){ // 合法な動きか判定してひっくり返すかどうか見る
    nxt[i] = pos[i]+dv9[vec[i].fi];
    if(nxt[i] < 0 or h*w <= nxt[i])nxt[i] = pos[i];
    if(abs(nxt[i]%w-pos[i]%w) > 1)nxt[i] = pos[i];
    cnt[nxt[i]]++;
    if(color[nxt[i]] == -color4[i])rev[i] = 1;
    if(color[nxt[i]] != EMPTY and vec[i].se)rev[i] = 1;
    his.past.push_back(P(pos[i], color[pos[i]]));
    his.past.push_back(P(nxt[i], color[nxt[i]]));
  }
  history.push(his);
  
  // nxtが同じ位置の人が多い人から順に処理していく
  vector<P> index;
  rep(i,2*n)index.push_back(P(cnt[nxt[i]]+rev[i], i));
  sort(index.begin(), index.end(), greater<P>());
  
  rep(i,2*n){ // 移動できない人たちの処理
    int idx = index[i].se;
    if(cnt[nxt[idx]] <= 1 and rev[idx])set_color(nxt[idx], EMPTY);
    if(cnt[nxt[idx]] > 1 or rev[idx])nxt[idx] = pos[idx];
    cnt[nxt[idx]]++;
  }
  rep(i,2*n){ // 移動
    if(pos[i] != nxt[i]){
      eigenvalue ^= random_table[i+2][pos[i]];
      eigenvalue ^= random_table[i+2][nxt[i]];
    }
    pos[i] = nxt[i];
    set_color(nxt[i], color4[i]);
  }
  
  com.update(pos, zip, eigenvalue+history.size()%8*rand_max);
  return;
}


// 2つのvectorを結合したものを返す
vector<P> Board::unite_vec(vector<P> &vec1, vector<P> &vec2, bool rev){
  vector<P> vec;
  int n = vec1.size();
  if(rev == false){
    rep(i,n)vec.push_back(vec1[i]);
    rep(i,n)vec.push_back(vec2[i]);
  }else{
    rep(i,n)vec.push_back(vec2[i]);
    rep(i,n)vec.push_back(vec1[i]);
  }
  return vec;
}


// p手戻る
void Board::undo(int p){
  int size = min((int)history.size(), p);
  if(size == 0)return;
  rep(i,size){
    History his = history.top();history.pop();
    rep(j,2*2*n){
      set_color(his.past[j].fi, his.past[j].se);    // (赤,青)*(pos,nxt)
    }
    if(i == size-1){
      pos = his.pos, score = his.score;
      eigenvalue = his.eigenvalue;
    }
  }
  com.update(pos, zip, eigenvalue+history.size()%8*rand_max);
}


void Board::brain(vi vec[2]){
  int cntup = 0;
}




// [FromPython]: 0 h w n number[0~h*w-1] color[0~h*w-1] pos[0~2*n-1]   初期化
// [FromPython]: 1 color[0~h*w-1] pos[0~2*n-1] score[0] score[1]        ゲーム進行
// [FromPython]: 2 v[0~2*n-1]     最善手(only first)予測

// 初期盤面の文字列を返す
string Board::get_init_str(){
  string str = "[FromPython]: 0 ";
  str += to_string(h) + " ";
  str += to_string(w) + " ";
  str += to_string(n) + " ";
  rep(i,h*w)str += to_string(number[i]) + " ";
  rep(i,h*w)str += to_string(color3[color[i]]) + " ";
  rep(i,2*n)str += to_string(pos[i]/w) + " " + to_string(pos[i]%w) + " ";
  return str;
}

// それぞれのマスの色とプレイヤーの位置を返す
string Board::get_state_str(){
  string str = "[FromPython]: 1 ";
  rep(i,h*w)str += to_string(color3[color[i]]) + " ";
  rep(i,2*n)str += to_string(pos[i]/w) + " " + to_string(pos[i]%w) + " ";
  str += to_string(calc_score(RED)) + " ";
  str += to_string(calc_score(BLUE)) + " ";
  return str;
}

// プレイヤーのpredictの文字列を返す
string Board::get_predict_str(const vector<P> vec[2]){
  string str = "[FromPython]: 2 ";
  rep(i,2)rep(j,n){
    str += to_string(vec[i][j].fi) + " ";
    str += to_string(vec[i][j].se) + " ";
  }
  return str;
}

// Unityからの文字列を受け取って行動する
void Board::move_str(string str){
  int p = 15;
  vector<P> mov;
  rep(i,2*n){
    int vec = str[p]-'0';p += 2;
    int rev = str[p]-'0';p += 2;
    mov.push_back(P(vec, rev));
  }
  move(mov);
}







