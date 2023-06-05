#include "header.h"
#include "CreateBoard.h"
#include "ReadJson.h"

/*
8 11:-2 1 0 1 2 0 2 1 0 1 -2:1 3 2 -2 0 1 0 -2 2 3 1:1 3 2 1 0 -2 0 1 2 3 1:2 1 1 2 2 3 2 2 1 1 2:2 1 1 2 2 3 2 2 1 1 2:1 3 2 1 0 -2 0 1 2 3 1:1 3 2 -2 0 1 0 -2 2 3 1:-2 1 0 1 2 0 2 1 0 1 -2:2 2:7 10:
*/
/*
12 12:-2 5 13 0 11 7 7 11 0 13 5 -2:0 16 6 1 9 7 7 9 1 6 16 0:1 -15 -11 9 3 6 6 3 9 -11 -15 1:12 14 15 8 0 12 12 0 8 15 14 12:12 1 -8 13 5 -15 -15 5 13 -8 1 12:13 -14 7 3 -12 4 4 -12 3 7 -14 13:6 1 7 0 5 -5 -5 5 0 7 1 6:13 -1 0 11 -10 -5 -5 -10 11 0 -1 13:10 14 6 -6 12 1 1 12 -6 6 14 10:15 14 7 4 6 8 8 6 4 7 14 15:13 11 -7 -6 2 -12 -12 2 -6 -7 11 13:11 0 6 0 3 11 11 3 0 6 0 11:4 2:6 1:
*/
/*
12 12:-2 5 13 0 11 7 7 11 0 13 5 -2:0 16 6 1 9 7 7 9 1 6 16 0:1 -15 -11 9 3 6 6 3 9 -11 -15 1:12 14 15 8 0 12 12 0 8 15 14 12:12 1 -8 13 5 -15 -15 5 13 -8 1 12:13 -14 7 3 -12 4 4 -12 3 7 -14 13:6 1 7 0 5 -5 -5 5 0 7 1 6:13 -1 0 11 -10 -5 -5 -10 11 0 -1 13:10 14 6 -6 12 1 1 12 -6 6 14 10:15 14 7 4 6 8 8 6 4 7 14 15:13 11 -7 -6 2 -12 -12 2 -6 -7 11 13:11 0 6 0 3 11 11 3 0 6 0 11:4 2:6 1: [3 10:5 11]
*/

// ボードを作る
// 引数(k = 0): サンプルボード
// 引数(k = 1): ランダムボード
// 引数(k = 2): 文字列インプット
BoardInfo create::get_board(int k, int deg,int col, int red_id, int blue_id, string file_name){
  srand((unsigned int)time(NULL));
  
  // 変数宣言
  BoardInfo info;
  int h, w, n;
  vi pos, number, color;
  
  // // サンプルボード
  // if(k == 0){
  //   h = 8, w = 11, n = 2;
  //   vi Pos{(2-1)*w+(10-1), (7-1)*w+(2-1), (2-1)*w+(2-1), (7-1)*w+(10-1)};
  //   vector<int> Number{
  //   -2, 1, 0, 1, 2, 0, 2, 1, 0, 1, -2,
  //   1, 3, 2, -2, 0, 1, 0, -2, 2, 3, 1,
  //   1, 3, 2, 1, 0, -2, 0, 1, 2, 3, 1,
  //   2, 1, 1, 2, 2, 3, 2, 2, 1, 1, 2,
  //   2, 1, 1, 2, 2, 3, 2, 2, 1, 1, 2,
  //   1, 3, 2, 1, 0, -2, 0, 1, 2, 3, 1,
  //   1, 3, 2, -2, 0, 1, 0, -2, 2, 3, 1,
  //   -2, 1, 0, 1, 2, 0, 2, 1, 0, 1, -2};
  //   pos = Pos;
  //   number = Number;
  //   color.resize(h*w);
  //   rep(i,n){
  //     color[pos[i]] = RED;
  //     color[pos[i+n]] = BLUE;
  //   }
  // }
  
  
  
  
  // サンプルボード
  if(k == 0){
    h = 5, w = 5, n = 4;
    vi Pos{(2-1)*w+(3-1), (4-1)*w+(3-1), (1-1)*w+(1-1), (1-1)*w+(2-1),
           (3-1)*w+(2-1), (3-1)*w+(4-1), (1-1)*w+(3-1), (1-1)*w+(4-1)};
    vector<int> Number{
      0, 0, 0, 1, 0,
      0, 5, 0, 0, 1,
      0, 0, 0, 0, 0,
      1, 0, 0, 5, 0,
      0, 1, 0, 0, 0
    };
    pos = Pos;
    number = Number;
    color.resize(h*w);
    rep(i,n){
      color[pos[i]] = RED;
      color[pos[i+n]] = BLUE;
    }
  }
  
  
  
  // ランダムボード
  if(k == 1){
    h = rand()%11+10;   // [10, 20]
    w = rand()%11+10;   // [10, 20]
    n = (h*w+80 + 60-1) / 60;   // [3, 8]
    
    rep(i,h*w){
      int ran = rand()%(16*3+1), num;
      if(ran <= 16)num = ran-16;
      else num = (ran-16)/2;
      number.push_back(num);
    }
    color.resize(h*w);
    pos.resize(2*n);
    
    int symmetry = rand()%3;    // 0: y軸対称, 1: x軸対称, 2: 点対称
    rep(i,n){
      int y1, x1, y_, x_, y2, x2;
      while(true){
        y1 = rand()%h, x1 = rand()%w;
        if(h%2 == 1 and y1 == h/2)continue;
        if(w%2 == 1 and x1 == w/2)continue;
        if(color[y1*w+x1] != EMPTY)continue;
        y_ = (h-1)-(y1), x_ = (w-1)-(x1);   // y1, x1 に対称な座標
        if(symmetry == 0){
          y2 = y_;x2 = x1;
          rep(y,h)rep(x,w)number[y*w+x] = number[((h-1)-y)*w+x];
        }else if(symmetry == 1){
          y2 = y1;x2 = x_;
          rep(y,h)rep(x,w)number[y*w+x] = number[(y)*w+(w-1)-x];
        }else if(symmetry == 2){
          y2 = y_;x2 = x_;
          rep(y,h)rep(x,w)number[y*w+x] = number[((h-1)-y)*w+(w-1)-x];
        }
        break;
      }
      pos[i] = y1*w+x1;color[y1*w+x1] = RED;
      pos[i+n] = y2*w+x2;color[y2*w+x2] = BLUE;
    }
  }
  
  // JSONファイル入力
  if(k == 2){
    ReadJson info;
  	info.read_info(file_name);
    h = info.height, w = info.width, n = info.playerNum;
    number = info.number;
    
    // colorとposの処理
    color = info.color;
    rep(i,n)pos.push_back(info.team[0].pos[i]);
    rep(i,n)pos.push_back(info.team[1].pos[i]);
    if(color[pos[0]] == blue_id){
      rep(i,n)pos[i] = info.team[1].pos[i];
      rep(i,n)pos[i] = info.team[0].pos[i];
      swap(red_id, blue_id);
    }
    rep(i,h*w){
      if(color[i] == red_id)color[i] = RED;
      else if(color[i] == blue_id)color[i] = BLUE;
    }
    
  }
  
  // 盤面回転
  int cnt_r = (deg+360*4)%360/90;
  rep(i,cnt_r)rotate_board(h, w, pos, number, color);
  
  // 変数セット
  info.h = h, info.w = w, info.n = n;
  info.pos = pos;
  info.number = number;
  info.color = color;
  return info;
}



void create::rotate_board(int &h, int &w, vi &pos, vi &number, vi &color){
  vi num, col;
  rep(j,w)drep(i,h-1,0)num.push_back(number[i*w+j]);
  rep(j,w)drep(i,h-1,0)col.push_back(color[i*w+j]);
  number = num;color = col;
  rep(i,pos.size()){
    int y = pos[i]/w, x = pos[i]%w;
    pos[i] = (x)*h+(h-y-1);
  }
  swap(h, w);
}

