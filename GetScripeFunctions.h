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

map< pair<string, string> , vector<string> > classifyDatas(
  const string *Cases, string *Namess
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
        timestamp = timestamp.substr(0,5);
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
