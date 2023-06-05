#include "JsonManagement.h"
// using namespace boost::property_tree;


// int型１つ読み込む
int JsonManagement::get_int(const boost::property_tree::ptree& info, string str){
  int res;
  if (boost::optional<int> value = info.get_optional<int>(str)) {
    res = value.value();
  }else ng = true;
  return res;
}

// string型１つ読み込む
string JsonManagement::get_string(const boost::property_tree::ptree& info, string str){
  string res;
  if(boost::optional<string> value = info.get_optional<string>(str)){
    res = value.value();
  }else ng = true;
  return res;
}

// 初期化
JsonManagement::JsonManagement(){
}

// 初期化
JsonManagement::JsonManagement(string httphost, unsigned short port, string tokenname, int matches_id){
  communication = HttpRequest(httphost, port, tokenname);
  communication.connect_server();
  game_filename = "get_file/game_info";
  state_filename = "get_file/state_info";
  game_filenum = 1;
  id = matches_id;
  ng = false;
}

void JsonManagement::disconnect_server(){
  communication.disconnect_server();
}

// 試合事前情報を表示する
void JsonManagement::print_game_info(){
  cout << "id: " << id << endl;
  cout << "intervalMillis: " << intervalMillis << endl;
  cout << "matchTo: " << matchTo << endl;
  cout << "teamID: " << teamID << endl;
  cout << "turnMillis: " << turnMillis << endl;
  cout << "turns: " << turns << endl;
  cout << endl;
}


// ボード情報を表示する
void JsonManagement::print_state_info(){
  // board info
  cout << "width: " << width << endl;
  cout << "height: " << height << endl;
  cout << "playerNum: " << playerNum << endl;
  cout << "startedAtUnixTime: " << startedAtUnixTime << endl;
  cout << "turn: " << turn << endl;
  cout << endl;

  // points
  cout << "points: " << endl;
  rep(i,height){
    rep(j,width)cout << number[i*width+j] << " ";
    cout << endl;
  }

  // tiled
  cout << endl;
  cout << "tiled: " << endl;
  rep(i,height){
    rep(j,width)cout << color[i*width+j] << " ";
    cout << endl;
  }
  cout << endl;

  // team info
  rep(t,2){
    cout << "team_id: " << team[t].team_id << endl;
    cout << "tile_point: " << team[t].tile_point << endl;
    cout << "area_point: " << team[t].area_point << endl;
    cout << "point: " << team[t].point << endl;
    rep(i,playerNum){
      cout << "agent_id" << team[t].agent_id[i];
      cout << ": (" << team[t].pos[i]/width << ", ";
      cout << team[t].pos[i]%width << ")" << endl;
    }
    cout << endl;
  }

}


// 試合事前情報取得
void JsonManagement::read_game_info(){
  string file_name = game_filename + to_string(game_filenum) + ".json";
  game_filenum++;
  communication.get_game_info(file_name);
  read_json(file_name, pt);
  BOOST_FOREACH (const boost::property_tree::ptree::value_type& child2, pt.get_child("game_info")) {
    const boost::property_tree::ptree& info2 = child2.second;
    int tmp_id = get_int(info2, "id");
    intervalMillis = get_int(info2, "intervalMillis");
    matchTo = get_string(info2, "matchTo");
    teamID = get_int(info2, "teamID");
    turnMillis = get_int(info2, "turnMillis");
    turns = get_int(info2, "turns");
    if(tmp_id == id)break;
  }
  if(ng == true)cout << "program couldn't read the field data correctly" << endl;
}



// ボード情報を読み込む
void JsonManagement::read_state_info(int state_turn){
  string file_name = state_filename + to_string(state_turn) + ".json";
  communication.get_state_info(file_name, id);      // getして書き込み
  read_json(file_name, pt);         // ファイル読み込み
  // board info
  width = get_int(pt, "width");
  height = get_int(pt, "height");
  startedAtUnixTime = get_int(pt, "startedAtUnixTime");
  turn = get_int(pt, "turn");

  // points
  BOOST_FOREACH (const boost::property_tree::ptree::value_type& child, pt.get_child("points")) {
    BOOST_FOREACH (const boost::property_tree::ptree::value_type& child2, child.second) {
      string data = child2.second.data();
      int num = atoi(data.c_str());
      number.push_back(num);
    }
  }

  // tiled
  BOOST_FOREACH (const boost::property_tree::ptree::value_type& child, pt.get_child("tiled")) {
    BOOST_FOREACH (const boost::property_tree::ptree::value_type& child2, child.second) {
      string data = child2.second.data();
      int col = atoi(data.c_str());
      color.push_back(col);
    }
  }


  // team info
  int i = 0;
  BOOST_FOREACH (const boost::property_tree::ptree::value_type& child, pt.get_child("teams")) {
    const boost::property_tree::ptree& info = child.second;
    team[i].team_id = get_int(info, "teamID");
    team[i].tile_point = get_int(info, "tilePoint");
    team[i].area_point = get_int(info, "areaPoint");
    team[i].point = team[i].tile_point + team[i].area_point;
    BOOST_FOREACH (const boost::property_tree::ptree::value_type& child2, info.get_child("agents")) {
      const boost::property_tree::ptree& info2 = child2.second;
      int agent_id = get_int(info2, "agentID");
      int x = get_int(info2, "x")-1;
      int y = get_int(info2, "y")-1;
      team[i].agent_id.push_back(agent_id);
      team[i].pos.push_back(y*width+x);
    }
    i++;
  }
  playerNum = team[0].pos.size();

  if(ng == true){
    cout << "program couldn't read the field data correctly" << endl;
  }
}



// 行動を送信する
void JsonManagement::post_action_info(int a, int b){
  // ファイル名作成
  string file_name;
  file_name += "post_file/post_info";
  file_name += to_string(a+1) + "-" + to_string(b) + ".json";
  communication.post_action_info(id, file_name);
}

// pingを送信する
void JsonManagement::get_ping(){
  communication.get_ping();
}
