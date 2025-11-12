#include <regex>

Long64_t FileSize(const char *CheckFileName, const char OPT ='-'){
  ifstream fin(CheckFileName, ifstream::in | ifstream::binary);
  fin.seekg(0, ios::end);
  Long64_t length = fin.tellg();
  fin.close();
  if(OPT!='Q'&&OPT!='q') cout << length << " bytes." << endl;
  return length;
}
string SetPathAuto(string vStrCase,string Case,string vStrMain){
  if(vStrCase=="auto") return vStrMain+Case;
  return vStrCase;
}
/*
map< pair<string, string> , vector<string> > classifyDatas(
  const string *Cases, string *Namess, const int TDigit = 5
){
  regex run_pattern("(?:[Rr][Uu][Nn])(\\d+)_");
  map< pair<string, string> , vector<string> > MapFiles;
  for (int i = 0; i < 4; ++i) {
    ifstream in(Namess[i]);
    string strtmp;
    cout << "Case of files: " << Cases[i] << endl;
    while(in>>strtmp){
      cout<<strtmp<<endl;
      smatch match;
      string time_pattern_str = "(\\d{6}\\.\\d{3})" + Cases[i] + "\\.bin";
      regex time_pattern(time_pattern_str);

      string run_number, timestamp;

      if (regex_search(strtmp, match, run_pattern)) {
        run_number = match[1];
      }

      if (regex_search(strtmp, match, time_pattern)) {
        timestamp = match[1];
        timestamp = timestamp.substr(0,TDigit);
      }

      cout << "Run number: " << run_number << "\t , \t";
      cout << "Timestamp: " << timestamp << endl;
      if(i==0){
        vector<string> vName = {strtmp,"","",""};
        MapFiles[{run_number,timestamp}] = vName;
      }else{
        if(MapFiles.count({run_number,timestamp}) == 0) continue;
        else MapFiles[{run_number,timestamp}][i] = strtmp;
      }
    }
    in.close();
  }
  return MapFiles;
}
*/
map< pair<string, string> , vector<string> > classifyDatas(
  const string *Cases, string *Namess, const double TTHR = 300
){
  regex run_pattern("(?:[Rr][Uu][Nn])(\\d+)_");
  map< pair<string, string> , vector<string> > MapFiles;

  struct Entry {
    string run_number;
    double timestamp;
    string timestamp_str;
    string filename;
    int case_index;
  };

  vector<Entry> entries;

  // === 先讀取所有資料 ===
  for (int i = 0; i < 4; ++i) {
    ifstream in(Namess[i]);
    string strtmp;
    cout << "Case of files: " << Cases[i] << endl;
    while(in >> strtmp){
      cout << strtmp << endl;
      smatch match;
      string time_pattern_str = "(\\d{6}\\.\\d{3})" + Cases[i] + "\\.bin";
      regex time_pattern(time_pattern_str);

      string run_number, timestamp_str;

      if (regex_search(strtmp, match, run_pattern)) {
        run_number = match[1];
      }

      if (regex_search(strtmp, match, time_pattern)) {
        timestamp_str = match[1].str();
        timestamp_str = timestamp_str.substr(0, 10);
      }

      if (run_number.empty() || timestamp_str.empty()) continue;

      // 將 HHMMSS.ms 轉為秒數方便比較
      double H = stod(timestamp_str.substr(0,2));
      double M = stod(timestamp_str.substr(2,2));
      double S = stod(timestamp_str.substr(4));
      double t_in_sec = H*3600 + M*60 + S;

      entries.push_back({run_number, t_in_sec, timestamp_str, strtmp, i});
    }
    in.close();
  }

  // === 依 run_number、timestamp 排序 ===
  sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b){
    if (a.run_number != b.run_number) return a.run_number < b.run_number;
    return a.timestamp < b.timestamp;
  });

  // === 按 TTHR 合併相近時間 ===
  for (size_t i = 0; i < entries.size(); ++i) {
    const auto &ent = entries[i];
    string run = ent.run_number;
    double base_time = ent.timestamp;
    string base_time_str = ent.timestamp_str;

    // 找是否可歸入已存在群組
    bool grouped = false;
    for (auto &kv : MapFiles) {
      if (kv.first.first == run) {
        double H = stod(kv.first.second.substr(0,2));
        double M = stod(kv.first.second.substr(2,2));
        double S = stod(kv.first.second.substr(4));
        double t_exist = H*3600 + M*60 + S;
        if (fabs(t_exist - base_time) < TTHR) {
          kv.second[ent.case_index] = ent.filename;
          grouped = true;
          break;
        }
      }
    }

    if (!grouped) {
      vector<string> vName(4, "");
      vName[ent.case_index] = ent.filename;
      MapFiles[{run, base_time_str}] = vName;
      // cout << "\n==== Matched run-time and corresponding files ====\n";
    }
  }
  for (const auto& kv : MapFiles) {
      const auto& key = kv.first;
      const auto& vName = kv.second;
      if(key.first!="")
        cout << "Run: " << key.first << " | Time: " << key.second << "\n";
      else continue;
      for (size_t i = 0; i < vName.size(); ++i) {
          if (!vName[i].empty())
              cout << "  [" << i << "] " << vName[i] << "\n";
      }
      cout << "-----------------------------------\n";
  }

  return MapFiles;
}
