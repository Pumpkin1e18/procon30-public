#include "HttpRequest.h"


// HttpRequest
// https://tomosoft.jp/design/?p=3636

// GETとPOST
// https://qiita.com/Sekky0905/items/dff3d0da059d6f5bfabf

// ファイル入出力
// https://qiita.com/NickTominaga/items/7e01b7eb0b67ac791ec6

// chcp 65001
// g++ HttpRequest.cpp -lws2_32



// 初期化
HttpRequest::HttpRequest(){

}

// 初期化
HttpRequest::HttpRequest(string httphost_, unsigned short port_, string tokenname_){
  /* IP アドレス、ポート番号、ソケット 出力ファイル名 */
  port = port_;
  httphost = httphost_;
  tokenname = tokenname_;
}

// サーバーに接続する
void HttpRequest::connect_server(){
  /* Windows 独自の設定 */
  WSADATA data;
  WSAStartup(MAKEWORD(2, 0), &data);

  /* sockaddr_in 構造体のセット */
  memset(&dstAddr, 0, sizeof(dstAddr));
  dstAddr.sin_port = htons(port);
  dstAddr.sin_family = AF_INET;
  dstAddr.sin_addr.s_addr = inet_addr(httphost.c_str());

  /* ソケット生成 */
  dstSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (dstSocket < 0){
      printf("%d\n", GetLastError());
      printf("ソケット生成エラー\n");
  }

  /* 接続 */
  result = connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr));
  if (result < 0){
      printf("%d\n", GetLastError());
      printf("バインドエラー\n");
  }
}

// サーバーの接続を切る
void HttpRequest::disconnect_server(){
  /* Windows 独自の設定 */
  closesocket(dstSocket);
  WSACleanup();
}



// jsonファイルに書き込む
void HttpRequest::write_json(string buf, string data_type){
  int n = buf.size();
  for(int i = 0;i < n;i++){
    // 最初のjsonに関係ないところを読み飛ばす
    if(start == false and (buf[i] != '[' and buf[i] != '{'))continue;
    start = true;
    // game_infoのときかっこ追加
    if(data_type == "game_info" and indent_cnt == 0){
      writing_file << "{" << endl;
      writing_file << "\t" << "\"game_info\": ";
      indent_cnt++;
    }
    // jsonのファイルに合うように改行とタブを適宜挿入する
    if(buf[i] == ']' or buf[i] == '}'){
      indent_cnt--;
      writing_file << endl;
      for(int i = 0;i < indent_cnt;i++)writing_file << "\t";
    }
    writing_file << buf[i];
    if(buf[i] == '[' or buf[i] == '{')indent_cnt++;
    // game_infoのときかっこ追加
    if(data_type == "game_info" and indent_cnt == 1){
      writing_file << endl << "}" << endl;
      indent_cnt--;
      return;
    }
    if(buf[i] != ',' and buf[i] != '[' and buf[i] != '{')continue;
    writing_file << endl;
    for(int i = 0;i < indent_cnt;i++)writing_file << "\t";
  }
}


// 試合事前情報を取得する
void HttpRequest::get_game_info(string file_name){
  string send_text = "";
  send_text += "GET /matches/ HTTP/1.1\n";
  send_text += "HOST: " + httphost + ":" + to_string(port) + "\n";
  send_text += "Authorization: " + tokenname + "\n\n";
  send(dstSocket, send_text.c_str(), send_text.size(), 0);

  //Dump HTTP response
  writing_file.open(file_name, ios::out);
  indent_cnt = 0;
  start = false;
  while(1){
      memset(buf, 0, sizeof(buf));
      read_size = recv(dstSocket, buf, BUF_LEN, 0);
      string buf_str = buf;
      cout << buf_str << endl;
      if (read_size > 0){
          write_json(buf_str, "game_info");
          if(indent_cnt == 0)break;
      }else break;
  }
}


// ボード情報を取得する
void HttpRequest::get_state_info(string file_name, int matches_id){
  // string send_text = "";
  // send_text += "GET /matches/" + to_string(matches_id) + "/ HTTP/1.1\n";
  // send_text += "HOST: " + httphost + ":" + to_string(port) + "\n";
  // send_text += "Authorization: " + tokenname + "\n\n";
  // // char *cmdline = "curl -H " + send_txt;
  // send(dstSocket, send_text.c_str(), send_text.size(), 0);


  string send_text = "curl -H \"Authorization: " + tokenname + "\" " + httphost + ":" + to_string(port) + "/matches/" + to_string(matches_id) + " > " + file_name;
  const char *cmdline = send_text.c_str();
  int res = system(cmdline);
  if(res != 0){
      cout << "Error" << endl;
  }


  // //Dump HTTP response
  // writing_file.open(file_name, ios::out);
  // indent_cnt = 0;
  // start = false;
  // bool line_test = false;
  // while (1){
  //   memset(buf, 0, sizeof(buf));
  //   read_size = recv(dstSocket, buf, BUF_LEN, 0);
  //   string buf_str = buf;
  //   cout << buf_str << endl;

  //   // jsonファイルに書き込む
  //   if (read_size > 0){
  //     for(int i = 0;i < buf_str.size();i++){
  //       // 最初のjsonに関係ないところを読み飛ばす
  //       if(start == false and (buf[i] != '[' and buf[i] != '{'))continue;
  //       start = true;
  //       if(buf[i] == '[' or buf[i] == '{')indent_cnt++;
  //       if(buf[i] == ']' or buf[i] == '}')indent_cnt--;
  //       writing_file << buf_str[i];
  //     }
  //     // writing_file << endl;   // バッファキャッシュに入ってるデータを吐き出す
  //     if(indent_cnt == 0)break;
  //   }else break;
  // }
  // writing_file << endl;   // バッファキャッシュに入ってるデータを吐き出す
}


// 行動を送信する
void HttpRequest::post_action_info(int matches_id, string file_name){
  // jsonファイルから行動の情報を取り出す
  ifstream reading_file;
  reading_file.open(file_name, ios::in);

  string action_info = "";
  while (!reading_file.eof()){
    string reading_line_buffer;
    getline(reading_file, reading_line_buffer);
    action_info += reading_line_buffer;
  }


  // // 送信する文字列連結
  // string send_text = "";
  // send_text += "POST /matches/" + to_string(matches_id) + "/action/ HTTP/1.1\n";
  // send_text += "HOST: " + httphost + ":" + to_string(port) + "\n";
  // send_text += "Authorization: " + tokenname + "\n";
  // send_text += "Content-Type: application/json\n";
  // send_text += "Content-Length: " + to_string(action_info.size()) + "\n\n";
  // send_text += action_info;

  // cout << send_text << endl;

  // // 送信
  // send(dstSocket, send_text.c_str(), send_text.size(), 0);



  /* 変更した部分 */
  string empty_file = "post_file/empty.json";
  string send_text = "curl -H \"Authorization: " + tokenname + "\" " + "-H \"Content-Type: application/json\" -X POST " + "http://" + httphost + ":" + to_string(port) + "/matches/" + to_string(matches_id) + "/action -d \"" + action_info + "\"" + " > " + empty_file;
  cout << send_text << endl;
  const char *postcmd = send_text.c_str();
  int res = system(postcmd);
  if(res != 0){
      cout << "Error" << endl;
  }
  /* 変更した部分 */




  // // 読み取ったものは捨てる
  // int bracket_cnt = 0;
  // while (1){
  //     memset(buf, 0, sizeof(buf));
  //     read_size = recv(dstSocket, buf, BUF_LEN, 0);
  //     string buf_str = buf;
  //     cout << "A" << endl;
  //     cout << buf_str << endl;
  //     cout << "B" << endl;
  //     if (read_size > 0){
  //       for(int i = 0;i < buf_str.size();i++){
  //         if(buf_str[i] == '[' or buf_str[i] == '{')bracket_cnt++;
  //         if(buf_str[i] == ']' or buf_str[i] == '}')bracket_cnt--;
  //       }
  //       if(bracket_cnt == 0)break;
  //     }else break;
  // }

}

void HttpRequest::get_ping(){
    string send_text = "";
    send_text += "GET /ping/ HTTP/1.1\n";
    send_text += "HOST: " + httphost + ":" + to_string(port) + "\n";
    send_text += "Authorization: " + tokenname + "\n\n";
    send(dstSocket, send_text.c_str(), send_text.size(), 0);

    memset(buf, 0, sizeof(buf));
    read_size = recv(dstSocket, buf, BUF_LEN, 0);
    string buf_str = buf;
    cout << buf_str << endl;
    cout << "ping ok" << endl;
}
