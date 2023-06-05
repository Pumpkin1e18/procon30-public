// Client
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <iostream>


namespace client{
  using namespace std;
  #define PORT 10021 //ポート番号
  // IP アドレス，ポート番号，ソケット，sockaddr_in 構造体
  char destination[32] = "127.0.0.1";
  int dstSocket;
  const int size = 1<<15;
  struct sockaddr_in dstAddr;
  
  // 受け取ったメッセージ, スレッド, 接続中かどうか
  char message[size] = "", message_get[size] = "";;
  thread th;
  bool isConnect = false;
  
  // サーバーからのメッセージの受け取り
  void receive_message(){
    isConnect = true;
    while(true){
      recv(dstSocket, message_get, sizeof(message_get), 0);
      if(message_get[0] == '\n' or message_get[0] == ' ' or message_get[0] == '\0'){
        continue;
      }
      strcpy(message, message_get);
      cout << message << "を受け取りました" << endl;
      if(message[13] == '1'){isConnect = false;return;}
    }
  }

  // サーバーと接続
  void connect_server(){
    // Windows の場合
    WSADATA data;
    WSAStartup(MAKEWORD(2,0), &data);
    // sockaddr_in 構造体のセット
    memset(&dstAddr, 0, sizeof(dstAddr));
    dstAddr.sin_port = htons(PORT);
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_addr.s_addr = inet_addr(destination);
    
  	dstSocket = socket(AF_INET, SOCK_STREAM, 0); // ソケットの生成

  	if(connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr))){ //接続
  		printf("%s に接続できませんでした\n",destination);
  		return;
  	}
    printf("%s に接続しました\n", destination);
    th = thread(receive_message);
  }

  // メッセージを送る
  void send_message(string str){
    char buffer[size];
    for(int i = 0;i < str.size();i++)buffer[i] = str[i];
    buffer[str.size()] = '\0';
    send(dstSocket, buffer, sizeof(buffer), 0);
  }

  // メッセージを受け取る
  string get_message(){
    string str = "";
    int size = strlen(message);
    for(int i = 0;i < size;i++)str.push_back(message[i]);
    message[0] = '\0';
    return str;
  }

  // サーバーとの接続を切る
  void disconnect_server(){
    th.join();
  	closesocket(dstSocket);  // Windows でのソケットの終了
  	WSACleanup();
  }
}