#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TString.h"
#include "./GIDMapping.h"
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"

/**
 * 讀取 .dat 檔案並提取解析度數據
 * 返回 map: <Cluster名稱, <解析度, 誤差>>
 * 注意：會移除 key 的第一個字元 (如 A1x1 -> 1x1) 以便對齊比較
 */
string dirGraph = "";
std::map<std::string, std::pair<double, double>> ReadResData(std::string fileName) {
    std::map<std::string, std::pair<double, double>> data;
    std::ifstream ifs(fileName.c_str());
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open file " << fileName << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty() || line[0] == '#') continue; // 跳過註解和空行
        
        std::stringstream ss(line);
        std::string key;
        double mean, me, sig, se, res, rese;
        
        // 依照格式讀取: Type Mean MeanErr Sigma SigmaErr Resolution ResErr
        if (ss >> key >> mean >> me >> sig >> se >> res >> rese) {
            // 移除第一個字元 (例如 'A' 或 'E') 以便直接使用相同 Key 進行比較
            if (key.size() > 1) {
                key = key.substr(1);
            }
            data[key] = std::make_pair(res, rese);
        }
    }
    ifs.close();
    return data;
}

/**
 * 畫出 ADC (A) 與 Edep (E) 方法的解析度比較長條圖
 */
void DrawEResolution(int DID) {
    setTDRStyle();
    // 1. 讀取數據
    std::string fileA = Form("%sEmax_Track_sta_A_D%d.dat", dirGraph.c_str(), DID);
    std::string fileE = Form("%sEmax_Track_sta_E_D%d.dat", dirGraph.c_str(), DID);
    
    auto resA = ReadResData(fileA);
    auto resE = ReadResData(fileE);

    if (resA.empty() || resE.empty()) {
        std::cerr << "Error: Data map is empty. Check your .dat files." << std::endl;
        return;
    }

    // 2. 準備直方圖 (作為長條圖)
    // 取得所有的 Key (Clustering 方法名稱，此時已不含 A/E 前綴)
    std::vector<std::string> keys;
    for (auto const& [name, val] : resA) {
        keys.push_back(name);
    }
    int nKeys = keys.size();

    TH1F *hResA = new TH1F(Form("hResA%d", DID), "Resolution Comparison;Clustering Method;Resolution (#sigma/#mu)%", nKeys, 0, nKeys);
    TH1F *hResE = new TH1F(Form("hResE%d", DID), "", nKeys, 0, nKeys);

    // 填充數據與標籤
    for (int i = 0; i < nKeys; i++) {
        std::string k = keys[i];
        
        // A Method
        if (resA.count(k)) {
            hResA->SetBinContent(i + 1, resA[k].first*100.);
            hResA->SetBinError(i + 1, resA[k].second);
        }
        hResA->GetXaxis()->SetBinLabel(i + 1, k.c_str());
        
        // E Method
        if (resE.count(k)) {
            hResE->SetBinContent(i + 1, resE[k].first*100.);
            hResE->SetBinError(i + 1, resE[k].second);
        }
    }

    // 3. 設定樣式 (長條圖風格)
    gStyle->SetOptStat(0);
    hResA->SetFillColor(kAzure + 7);
    hResA->SetBarWidth(0.4);
    hResA->SetBarOffset(0.1);
    hResA->SetLineColor(kBlack);

    hResE->SetFillColor(kOrange + 1);
    hResE->SetBarWidth(0.4);
    hResE->SetBarOffset(0.5);
    hResE->SetLineColor(kBlack);

    // 4. 繪製
    TCanvas *cRes = new TCanvas("cRes", "Resolution Comparison", 1000, 700);
    cRes->SetGridy();
    cRes->SetBottomMargin(0.15);

    hResA->GetYaxis()->SetRangeUser(0, hResA->GetMaximum() * 1.5);
    hResA->GetXaxis()->SetLabelSize(0.05);
    hResA->Draw("bar2 error"); // bar2 繪製長條, error 繪製誤差線
    hResE->Draw("bar2 error same");

    // 5. 圖例
    TLegend *leg = new TLegend(0.65, 0.75, 0.98, 0.95);
    leg->AddEntry(hResA, "Method A (ADC-based)", "f");
    leg->AddEntry(hResE, "Method E (Edep-based)", "f");
    leg->SetBorderSize(1);
    leg->Draw();

    // 6. 輸出
    cRes->Print(Form("%sResolution_Comp_A_vs_E_D%d.gif", dirGraph.data(), DID));
}
void DrawEResolution( const string dirAnaPath ){
  dirGraph = dirAnaPath+string("graphCal/");
  for(size_t iDet = 0;iDet<CrystalDets.size();iDet++){
    DrawEResolution(CrystalDets[iDet]);
  }
}
void DrawEResolution() {
  cout<<"Finished compiling of DrawEResolution.C+"<<endl;
  // DrawEResolution("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/");
}