#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <vector>
#include <map>
#include <TH1F.h>

namespace fs = std::filesystem;
using namespace std;

// 讀取檔案的第一行第一個數字
double getFirstNumber(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) return -1;  // 如果打不開檔案，返回 -1

    double number;
    file >> number;
    return number;
}

int ScanErr() {
    string baseDir = "/data8/ZDC/EMCal/BeamTest/";
    ofstream ofs("./RunTable.csv");
    regex dirPattern(R"(BeamOn021[7-9]/Run(\d+)_.*LYSO)");
    regex filePattern(R"(ConvertProblem_DID([1-3])\.dat)");
    regex EffPattern(R"(EffCal.dat)");

    map<int, vector<double>> EfErData;  // 儲存 RunID 與對應的 Err1, Err2, Err3

    for (const auto& entry : fs::recursive_directory_iterator(baseDir)) {
        if (entry.is_regular_file()) {
            string path = entry.path().string();

            smatch dirMatch;
            if (regex_search(path, dirMatch, dirPattern)) {
                int runID = stoi(dirMatch[1].str());
                cout<<runID<<endl;
                smatch fileMatch;
                if (regex_search(path, fileMatch, filePattern)) {
                    int did = stoi(fileMatch[1].str());  // DID 是 1, 2, 3
                    double errValue = getFirstNumber(path);
                    EfErData[runID].resize(6, -1);  // 確保有 3 個欄位，預設 -1
                    EfErData[runID][did - 1] = errValue;
                cout<<runID<<endl;
                }
                smatch fileMatchEff;
                if (regex_search(path, fileMatchEff, EffPattern)) {
                  ifstream file(path);
                  if (!file.is_open()) return -1;  // 如果打不開檔案，返回 -1
                  double GoodEvCnt,TotalCnt;
                  file >> GoodEvCnt >> TotalCnt;
                  EfErData[runID][3] = GoodEvCnt;
                  EfErData[runID][4] = TotalCnt;
                  EfErData[runID][5] = GoodEvCnt/TotalCnt;
                }
            }
        }
    }

    // 輸出表格
    cout << "# RunID\tErr1\tErr2\tErr3\tGCnt\tTotCnt\tEff\n";
    ofs << "# RunID\tErr1\tErr2\tErr3\tGCnt\tTotCnt\tEff\n";
    for (const auto& [runID, EfErs] : EfErData) {
      ofs << Form("%d\t%.2f\t%.2f\t%.2f\t%.0f\t%.0f\t%.2f",runID,EfErs[0],EfErs[1],EfErs[2],EfErs[3],EfErs[4],EfErs[5])<<endl;;
      cout << Form("%d\t%.2f\t%.2f\t%.2f\t%.0f\t%.0f\t%.2f",runID,EfErs[0],EfErs[1],EfErs[2],EfErs[3],EfErs[4],EfErs[5])<<endl;;
    }

    return 0;
}
