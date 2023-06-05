#include "client.h"
#include "Board.h"
#include "CreateBoard.h"
#include "Player.h"
#include "header.h"


// 自分用実行メモ
// chcp 65001
// make

// Procon29の仕様
// Unityとの通信のためにデータを変形する
// [FromPython]: 0 h w number[h*w] pos[4]         初期化
// [FromPython]: 1 color[h*w] pos[4] score[2]         ゲーム進行
// [FromPython]: 2 [[v1[i], b1[i]], ...]      最善手(only first)予測

// Procon30の仕様
// [FromPython]: 0 h w n number[0~h*w-1] color[0~h*w-1] pos[0~2*n-1]   初期化
// [FromPython]: 1 color[0~h*w-1] pos[0~2*n-1] score[0] score[1]        ゲーム進行
// [FromPython]: 2 v[0~2*n-1]     最善手(only first)予測
// 
// [FromUnity]: 0 v[i]    移動
// [FromUnity]: 1     終了
// いらない [FromUnity]: 2 color player    プレイヤーの選択
// [FromUnity]: 3     前のターンに戻る
// いらない [FromUnity]: 4 Y X     移動先変更

// 一時的に変更していることメモ
// CreateBoard.cppのhとwを地作している
// PlayerMCTSのsimulationをすべて最善手にしている
// ランダムプレイアウトにしている
// プレイアウト時、いい着手に重み付けをしている
// 勝率0％になったときにアルファベータ探索に切り替える
// MCTSのwidth, depthとかを調整してる
// Interfaceのdisplay、PlayerMCTSの表示

// ToDo
// 自分の色のマイナスを踏んだ時にひっくり返すかどうか検討
// Visualizerでヒートマップ実装
// JSONファイルで読み込んだときプレイできない

// トークン: 601a046d407fbf465cb88bbbbd440c3fe12592f654874d2176992634ea0e7f5a

// 変数宣言
int h, w, n;
vi pos, number, color;

// 関数宣言
void game(int CG, int MA, int MT, int B, int DEG, int COL, int RID, int BID);
void game_CUI(int MT);
void game_GUI(int MA, int MT);
void select_disp(vector<P> vec);


// 確認事項
// max_turn
// 線対象
// 自分の色 typedef(2), degree, color



// 環境設定
/*--------------------------------------------------------------------------*/
// プレイヤー選択
// PlayerRandom, PlayerRnaom, PlayerHuman, PlayerAlphaBeta
typedef PlayerMCTS PLAYER1;      // 赤チームのプレイヤー選択
typedef PlayerAlphaBeta PLAYER2;      // 青チームのプレイヤー選択

int red_win_cnt = 0, blue_win_cnt = 0;
bool display = true;

int main(){
  SetConsoleOutputCP( 65001 );  // UTF-8にセットする
  int CG = CUI;     // CUI:コマンドプロンプト, GUI;Unity
  int MA = AUTO;  // MANUAL:人が操作, AUTO:自動でゲーム進行
  int B = 2;        // ボード選択 0:デフォルト, 1:ランダム, 2:JSONファイル
  int MT = -1;      // max_turn(-1の時は(h*w+200+(10-1))/10)
  int DEG = 0;      // 盤面を時計回りに回転する [-90, 0, 90, 180] 赤:90, 青:-90
  int COL = BLUE;    // 自分の色選択 RED:赤, BLUE:青
  int RID = 1;      // REDチームのID
  int BID = 2;      // BLUEチームのID
  game(CG, MA, MT, B, DEG, COL, RID, BID);
  /*rrep(i,20){
    cout << red_win_cnt << " : " << blue_win_cnt;
    cout << " / " << i << endl;
  }*/
}
/*--------------------------------------------------------------------------*/




void game(int CG, int MA, int MT, int B, int DEG, int COL, int RID, int BID){
  // 変数代入
  BoardInfo info = create::get_board(B, DEG, COL, RID, BID, "Field/D-2.json");
  h = info.h, w = info.w, n = info.n;
  pos = info.pos;
  number = info.number;
  color = info.color;
  // 環境選択
  if(MT <= 0)MT = (h*w+200+(10-1))/10;
  if(CG == CUI)game_CUI(MT);
  if(CG == GUI)game_GUI(MA, MT);
}


// CUI
void game_CUI(int MT){
  // 初期化
  Board board(h, w, pos, number, color);
  const int max_turn = MT;
  PLAYER1 player1(RED, max_turn, n);
  PLAYER2 player2(BLUE, max_turn, n);
  printf("created (%d, %d) board\n", h, w);
  if(display)board.print_board();
  
  // ゲーム進行
  vector<int> isEnd(2);
  while(board.get_turn() < max_turn){
    vector<P> hand[2];
    player1.get_act(board, hand[0], isEnd[0]);
    player2.get_act(board, hand[1], isEnd[1]);
    printf("\nturn%d: ", board.get_turn()+1);
    vector<P> vec = hand[0];
    rep(i,hand[1].size())vec.push_back(hand[1][i]);
    cout << max_turn << endl;
    if(display)select_disp(vec);
    board.move(vec);
    if(display)board.print_board();
  }
  int point[2] = {board.calc_score(RED), board.calc_score(BLUE)};
  int score = point[0] - point[1];
  if(score > 0)red_win_cnt++;
  if(score < 0)blue_win_cnt++;
}



// GUI
void game_GUI(int MA, int MT){
  // 初期化
  Board board(h, w, pos, number, color);
  int max_turn = MT;
  PLAYER1 player1(RED, max_turn, n);
  PLAYER2 player2(BLUE, max_turn, n);
  printf("created (%d, %d) board\n", h, w);
  printf("max_turn = %d\n", max_turn);
  board.print_board();
  vector<P> hand[2];
  vector<int> isEnd(2);
  vector<thread> th(2);
  th[0] = thread(&PLAYER1::get_act, &player1, ref(board), ref(hand[0]), ref(isEnd[0]), 1);
  th[1] = thread(&PLAYER2::get_act, &player2, ref(board), ref(hand[1]), ref(isEnd[1]), 1);
  clock_t start = clock();
  
  // サーバーとの通信開始
  client::connect_server();
  rep(i,h*w)cout << board.number[i] << " ";cout << endl;
  rep(i,h*w)cout << board.color3[board.color[i]] << " ";cout << endl;
  rep(i,2*n)cout << pos[i]/w << " " << pos[i]%w << " ";cout << endl;
  cout << endl << endl;
  cout << board.get_init_str() << endl;
  client::send_message(board.get_init_str());
  Sleep(1000);
  client::send_message(board.get_state_str());
  struct ColorChange{bool exist;int player, y, x;};
  ColorChange Col;Col.exist = false;
  
  // ゲームループ
  while(true){
    if((int)(board.history.size()) == max_turn)break;
    if(MA == AUTO)printf("\nturn:%d\n", (int)(board.history.size()));
    Sleep(100);
    string str = client::get_message();
    // predict送信
    if(str == "" and MA == MANUAL){
      if(clock()-start < 1000)continue;
      start = clock();
      client::send_message(board.get_predict_str(hand));
      continue;
    }
    if(str != "")cout << str << endl;
    // 通信切断(ゲーム終了)
    if(str[13] == '1')break;
    
    // thread停止
    if(str[13] == '0' or str[13] == '3' or MA == AUTO){
      if(MA == MANUAL){rep(i,2)isEnd[i] = 1;}
      rep(i,2)th[i].join();
      rep(i,2)isEnd[i] = 0;
    }
    // 行動
    if(str[13] == '0' or MA == AUTO){
      cout << "step ok" << endl;
      vector<P> vec = hand[0];
      rep(i,hand[1].size())vec.push_back(hand[1][i]);
      if(MA == AUTO)board.move(vec);
      else board.move_str(str);
    }
    // 1手戻る
    if(str[13] == '3'){
      board.undo(1);
    }
    // thread再開
    if(str[13] == '0' or str[13] == '3' or MA == AUTO){
      board.print_board();
      client::send_message(board.get_state_str());
      th[0] = thread(&PLAYER1::get_act, &player1, ref(board), ref(hand[0]), ref(isEnd[0]), 1);
      th[1] = thread(&PLAYER2::get_act, &player2, ref(board), ref(hand[1]), ref(isEnd[1]), 1);
      start = clock();
    }
    
  }
  // 終了
  printf("disconnected\n");
  rep(i,2)isEnd[i] = 1;
  rep(i,2)th[i].join();
  client::disconnect_server();
}


// 選択表示
void select_disp(vector<P> vec){
  int n = vec.size()/2;
  printf("selected\n");
  printf("player ");
  rep(i,n){
    printf("%d:(%d %d)", i+1, vec[i].fi, vec[i].se);
    if(i == n-1)printf("\n");
    else printf(", ");
  }
  printf("player ");
  repi(i,n,2*n){
    printf("%d:(%d %d)", i+1, vec[i].fi, vec[i].se);
    if(i == 2*n-1)printf("\n");
    else printf(", ");
  }
}





