#include "ReadJson.h"
// using namespace boost::property_tree;


// int型１つ読み込む
int ReadJson::get_int(const boost::property_tree::ptree& info, string str){
  int res;
  if (boost::optional<int> value = info.get_optional<int>(str)) {
    res = value.value();
  }else ng = true;
  return res;
}

// 初期化
ReadJson::ReadJson(){ng = false;}

// ボード情報を表示する
void ReadJson::print_info(){
  // board info
  cout << "width: " << width << endl;
  cout << "height: " << height << endl;
  cout << "playerNum: " << playerNum << endl;
  cout << "startedAtUnixTime: " << startedAtUnixTime << endl;
  cout << "turn: " << turn << endl;
  cout << endl;
  
  // points
  rep(i,height){
    rep(j,width)cout << number[i*width+j] << " ";
    cout << endl;
  }
  
  // tiled
  cout << endl;
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


// ボード情報を読み込む
void ReadJson::read_info(string file_name){
  read_json(file_name, pt);
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
      int id = get_int(info2, "agentID");
      int x = get_int(info2, "x")-1;
      int y = get_int(info2, "y")-1;
      team[i].agent_id.push_back(id);
      team[i].pos.push_back(y*width+x);
    }
    i++;
  }
  playerNum = team[0].pos.size();
  
  if(ng == true)cout << "program couldn't read the field data correctly" << endl;
  ng = true;
}



