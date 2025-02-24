#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include "TChain.h"
#include "TFile.h"

using namespace std;
namespace fs = std::filesystem;

string TargetRun = 
" 203 219 222 209 226 227 214 234 235 240 241 245 246 252 "
;
// 讀取輸入並解析成數字陣列
std::vector<int> parseInput() {

  std::vector<int> numbers;
  std::stringstream ss(TargetRun);
  std::string token;
  
  while (ss>>token) {
    try {
      numbers.push_back(std::stoi(token));
    } catch (...) {
      std::cerr << "無法解析數字：" << token << std::endl;
      exit(1);
    }
  }
  return numbers;
}

// 在檔案池中尋找符合的檔案
void searchFiles(const std::vector<int>& runIDs, const std::string& baseDir,map<int,string> &UsableFile, const string Target) {
  std::set<int> runSet(runIDs.begin(), runIDs.end());

  for (const auto& entry : fs::recursive_directory_iterator(baseDir)) {
    if (entry.is_regular_file()) {
      std::string fileName = entry.path().filename().string();
      for (int runID : runIDs) {
        if (fileName.find("Run" + std::to_string(runID)) == 0) {
          if(int(fileName.find(Target))!=-1){
            std::cout << "找到檔案：" << entry.path() << std::endl;
            UsableFile[runID] = entry.path();
            break;
          }
        }
      }
    }
  }
}
int PraseData() {
  std::string baseDir = "/data8/ZDC/EMCal/BeamTest/";

  std::cout << "Please paste run number array (Format: fileA fileB fileC fileD)，and press: Enter：" << std::endl;
  std::vector<int> runIDs = parseInput();
  
  map<int,string> FileMap;
  searchFiles(runIDs, baseDir, FileMap,"ReCon");
  
  string SaveFileDir = "/data8/ZDC/EMCal/BeamTestResult/NHit/";
  system(Form("mkdir -p %s",SaveFileDir.data()));
  
  TCanvas *c = new TCanvas("c","c",800+4,800+28);
  c->cd();
  ofstream ofsNHits(Form("%sTableNHits.dat",SaveFileDir.data()));
  for(const auto & it:FileMap){
    cout<<it.first<<" "<<it.second;
    string Name = it.second;
    TChain t("t");
    t.Add(Name.data());
    TH1F h("h","Hits of LYSO",64,0,64);
    t.Draw("nHits2>>h");
    cout<<" "<<h.GetMean()<<endl;
    ofsNHits<<it.first<<"\t"<<h.GetMean()<<endl;
    c->Print(Form("%sEventRun%03.0f.png",SaveFileDir.data(),it.first*1.));
  }
  return 0;
}
