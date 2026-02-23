#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <iostream>

using namespace std;

string RootFileName = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/CosmicRay_Full_System_ReCon.root";
string SavePath = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/Graph/";
vector<tuple<double, double, double>> GetCombination(
  vector<pair<double, double>> pXZ,
  vector<pair<double, double>> pYZ
){
  vector<tuple<double, double, double>> results;
  // 列舉所有組合
  for (const auto& posX : pXZ) {
    for (const auto& posY : pYZ) {
      // 提取 XZ 和 YZ 的座標
      double x = posX.first;  // X 值
      double z1 = posX.second; // XZ 平面的 Z 值
      double y = posY.first;  // Y 值
      double z2 = posY.second; // YZ 平面的 Z 值

      // 計算平均 Z 值
      double z_avg = (z1 + z2) / 2.0;

      // 儲存結果
      results.emplace_back(x, y, z_avg);
    }
  }
  // cout << "All combinations of pXZ and pYZ with average Z:" << endl;
  for (const auto& result : results) {
    double x, y, z;
    tie(x, y, z) = result;
    // cout << "(X: " << x << ", Y: " << y << ", Z_avg: " << z << ")" << endl;
  }
  return results;
};

double Distance(const tuple<double, double, double>& p1, const tuple<double, double, double>& p2) {
    double dx = get<0>(p1) - get<0>(p2);
    double dy = get<1>(p1) - get<1>(p2);
    double dz = get<2>(p1) - get<2>(p2);
    return sqrt(dx * dx + dy * dy + dz * dz);
}
void CheckDataRM(vector<tuple<double, double, double>> data,const tuple<double, double, double>& center){
  double cx, cy, cz; tie(cx, cy, cz) = center;
  for (size_t i = 0; i < data.size(); ++i) {
    double x, y, z; tie(x, y, z) = data[i];
    
    if (abs(x-cx) > 8 || abs(y-cy) > 8 || abs(z-cz) > 8){
      data.erase(data.begin() + i);
      i-=2;
    } // 例如，移除任意負數的數據
      

  }
}

bool shouldRemove(const tuple<double, double, double>& data,const tuple<double, double, double>& center) {
  double x, y, z; tie(x, y, z) = data;
  double cx, cy, cz; tie(cx, cy, cz) = center;

  // 如果 x, y, z 不符合條件，返回 true 表示移除
  return (abs(x-cx) > 8 || abs(y-cy) > 8 || abs(z-cz) > 8); // 例如，移除任意負數的數據
}
// 計算所有點的平均距離
double AverageDistance(const vector<tuple<double, double, double>>& points) {
    size_t n = points.size();
    if (n < 2) {
        return 0;
    }

    double totalDistance = 0.0;
    size_t count = 0;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            totalDistance += Distance(points[i], points[j]);
            ++count;
        }
    }

    return totalDistance / count;
}
tuple<double, double, double> GetCenter(const vector<tuple<double, double, double>>& points) {
    size_t n = points.size();
    if (n < 2) {
      return points[0];
    }
    tuple<double, double, double> result = {0,0,0};
    size_t count = 0;

    for (size_t i = 0; i < n; ++i) {
      get<0>(result) +=get<0>(points[i]);
      get<1>(result) +=get<1>(points[i]);
      get<2>(result) +=get<2>(points[i]);
      // totalDistance += Distance(points[i], points[j]);
      ++count;
    }
    get<1>(result) /= (count * 1.0);
    get<2>(result) /= (count * 1.0);
    get<2>(result) /= (count * 1.0);
    return result;
}
void DrawTriggerLines() {
    // 打開 ROOT 文件
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.15);
    // TFile *file = TFile::Open("/data8/ZDC/EMCal/LYSO2ABMCosmic/250118/CosRay_FV430_30V_FV320_x40_test2/CosRay_FV430_30V_FV320_x40_test2_ReCon.root");
    TFile *file = TFile::Open(RootFileName.data());
    if (!file || file->IsZombie()) {
        cerr << "無法打開文件！" << endl;
        return;
    }

    // 獲取 TTree
    TTree *t = (TTree*)file->Get("t");
    if (!t) {
        cerr << "無法找到 TTree 't'！" << endl;
        return;
    }

    // 定義變數來存取所有 branch
    Long64_t eventID;
    int pcnt, fcnt, nHits0, nROC0, nHits1, nROC1, nHits2, nROC2, nHits3, nROC3, nHits;
    vector<int> *iHit = nullptr, *DID = nullptr, *ROCID = nullptr, *channel = nullptr, *Fire = nullptr, *ADC = nullptr;
    vector<bool> *HGMode = nullptr;
    vector<int> *GADC = nullptr, *GID = nullptr, *iX = nullptr, *iY = nullptr, *iZ = nullptr;
    vector<float> *pX = nullptr, *pY = nullptr, *pZ = nullptr;
    double GADC11, GADC33, GADC55, GADCAL, GADCXs;
    int PeakID, Cortex;
    float CenpX, CenpY;

    // 將變數與 branch 綁定
    t->SetBranchAddress("eventID", &eventID);
    t->SetBranchAddress("pcnt", &pcnt);
    t->SetBranchAddress("fcnt", &fcnt);
    t->SetBranchAddress("nHits", &nHits);
    t->SetBranchAddress("nHits0", &nHits0);
    t->SetBranchAddress("nROC0", &nROC0);
    t->SetBranchAddress("nHits1", &nHits1);
    t->SetBranchAddress("nROC1", &nROC1);
    t->SetBranchAddress("nHits2", &nHits2);
    t->SetBranchAddress("nROC2", &nROC2);
    t->SetBranchAddress("nHits3", &nHits3);
    t->SetBranchAddress("nROC3", &nROC3);
    t->SetBranchAddress("iHit", &iHit);
    t->SetBranchAddress("DID", &DID);
    t->SetBranchAddress("ROCID", &ROCID);
    t->SetBranchAddress("channel", &channel);
    t->SetBranchAddress("Fire", &Fire);
    t->SetBranchAddress("HGMode", &HGMode);
    t->SetBranchAddress("ADC", &ADC);
    t->SetBranchAddress("GADC", &GADC);
    t->SetBranchAddress("GID", &GID);
    t->SetBranchAddress("iX", &iX);
    t->SetBranchAddress("iY", &iY);
    t->SetBranchAddress("iZ", &iZ);
    t->SetBranchAddress("pX", &pX);
    t->SetBranchAddress("pY", &pY);
    t->SetBranchAddress("pZ", &pZ);
    t->SetBranchAddress("GADC11", &GADC11);
    t->SetBranchAddress("GADC33", &GADC33);
    t->SetBranchAddress("GADC55", &GADC55);
    t->SetBranchAddress("GADCAL", &GADCAL);
    t->SetBranchAddress("GADCXs", &GADCXs);
    t->SetBranchAddress("PeakID", &PeakID);
    t->SetBranchAddress("CenpX", &CenpX);
    t->SetBranchAddress("CenpY", &CenpY);
    t->SetBranchAddress("Cortex", &Cortex);

    // 遍歷所有 entries
    Long64_t nEntries = t->GetEntries();
    // TH3F *boxh3 = new TH3F("boxh3",";X position (mm);Y position (mm);Z position (mm)",260,-130,130,260,-130,130,100,-150,150);
    double TriggerSize = 67.2;
    double SideSize = TriggerSize/2.;
    double OffT1[2] = {-0.018,0.018};
    double OffT2[2] = {2.482,0.018};
    TH2F *h2T1 = new TH2F("h2T1","T1 Z=112.12mm;X position (mm);Y position (mm);-",32,-SideSize-OffT1[0],SideSize-OffT1[0],32,-SideSize-OffT1[1],SideSize-OffT1[1]);
    TH2F *h2T2 = new TH2F("h2T2","T2 Z=-77.68mm;X position (mm);Y position (mm);-",32,-SideSize-OffT2[0],SideSize-OffT2[0],32,-SideSize-OffT2[1],SideSize-OffT2[1]);
    TH2F *h2BD0 = new TH2F("h2BD0","ZDC Z=5.3mm;X position (mm);Y position (mm);ADC",32,-SideSize-OffT2[0],SideSize-OffT2[0],32,-SideSize-OffT2[1],SideSize-OffT2[1]);
    TH2F *h2D0 = new TH2F("h2D0","",8,-28.8,28.8,8,-28.8,28.8);
    // boxh3->SetStats(0);
    h2T1->SetStats(0);
    h2T2->SetStats(0);
    // h2T1->GetXaxis()->SetRangeUser(-50,50);
    // h2T1->GetYaxis()->SetRangeUser(-50,50);
    // h2T2->GetXaxis()->SetRangeUser(-50,50);
    // h2T2->GetYaxis()->SetRangeUser(-50,50);
    // h2BD0->GetXaxis()->SetRangeUser(-50,50);
    // h2BD0->GetYaxis()->SetRangeUser(-50,50);
    h2BD0->SetStats(0);
    
    TCanvas* c3 = new TCanvas("c3", "3D Scatter Plot with Color", 600*4+4, 600+28);
    c3->cd()->Divide(4,1);
    TH2F *h2DiffB = new TH2F("h2DiffB","Position Difference;#Delta X (mm);#Delta Y position (mm);ADC",360,-60,60,360,-60,60);
    TH2F *h2Diff = new TH2F("h2Diff","Position Difference;#Delta X (mm);#Delta Y position (mm);ADC",150,-3.6-7*7.2,3.6+7*7.2,150,-3.6-7*7.2,3.6+7*7.2);

    for (Long64_t i = 0; i < nEntries; i++) {
      t->GetEntry(i); // 加載第 i 個 entry
      // 打印數據
      cout << "Entry " << i << ":" << endl;
      cout << "  eventID: " << eventID << endl;
      cout << "  pcnt: " << pcnt << ", fcnt: " << fcnt << endl;
      // cout << "  nHits0: " << nHits0 << ", nROC0: " << nROC0 << endl;
      // cout << "  nHits1: " << nHits1 << ", nROC1: " << nROC1 << endl;
      // cout << "  nHits2: " << nHits2 << ", nROC2: " << nROC2 << endl;
      // cout << "  nHits3: " << nHits3 << ", nROC3: " << nROC3 << endl;
      // cout << "  iHit: ";
      // for (auto &hit : *iHit) cout << hit << " ";
      // cout << endl;
      // cout << "  pX: ";
      // for (auto &px : *pX) cout << px << " ";
      // cout << endl;
      // cout << "  pY: ";
      // for (auto &py : *pY) cout << py << " ";
      // cout << endl;
      // cout << "  GADC11: " << GADC11 << ", GADC33: " << GADC33 << endl;
      // cout << "  GADC55: " << GADC55 << ", GADCAL: " << GADCAL << endl;
      // cout << "  GADCXs: " << GADCXs << endl;
      // cout << "  PeakID: " << PeakID << ", Cortex: " << Cortex << endl;
      // cout << "  CenpX: " << CenpX << ", CenpY: " << CenpY << endl;
      // cout << "  ADC: ";
      // for (auto &adc : *ADC) cout << adc << " ";
      // cout << endl;
      int NowHit = 0;
      vector<pair<double, double>> Trigger1PosX, Trigger1PosY, Trigger2PosX, Trigger2PosY;
      cout<<nHits<<endl;
      for(int iH = 0; iH<nHits;iH++){
        double px = pX->at(iH), py = pY->at(iH), pz = pZ->at(iH);
        int iz = iZ->at(iH);
        if(iz==5) Trigger2PosX.push_back(pair<double, double>(px,pz));
        else if(iz==4) Trigger2PosY.push_back(pair<double, double>(py,pz));
        else if(iz==1) Trigger1PosX.push_back(pair<double, double>(px,pz));
        else if(iz==2) Trigger1PosY.push_back(pair<double, double>(py,pz));
        else if(iz==3){
          h2D0->Fill(px,py,GADC->at(iH));
        } 

      }
      vector<tuple<double, double, double>> 
        Trigger1 = GetCombination(Trigger1PosX,Trigger1PosY),
        Trigger2 = GetCombination(Trigger2PosX,Trigger2PosY);
      
      tuple<double, double, double> CenterT1 = GetCenter(Trigger1);
      CheckDataRM(Trigger1,CenterT1);

      // double AVGDisT1 = AverageDistance(Trigger1);
      // if(AVGDisT1>6) continue;
      TGraph* PointDiff = new TGraph();
      TGraph* PointIntercept = new TGraph();
      TGraph* PointCZDC = new TGraph();

      // 塞入數據
      int indexAll = 0,index1 = 0,index2 = 0;
      for (const auto& [xi, yi, zi] : Trigger1){
          // PointT1->SetPoint(index1++, xi, yi); // Z 值將自動對應顏色
          h2T1->Fill( xi, yi); // Z 值將自動對應顏色 
          index1++;
      }
      for (const auto& [xi, yi, zi] : Trigger2){
          h2T2->Fill( xi, yi); // Z 值將自動對應顏色
          index2++;
      }
      for (const auto& [xi1, yi1, zi1] : Trigger1){
        for (const auto& [xi2, yi2, zi2] : Trigger2){
          double t = (5.3 - zi1) / (zi2 - zi1);
          double xtmp = xi1 + t * (xi2 - xi1);
          double ytmp = yi1 + t * (yi2 - yi1);
          PointIntercept->SetPoint(indexAll,xtmp,ytmp);
          double DifX = -CenpX-xtmp, DifY = CenpY-ytmp;
          PointDiff->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
          indexAll++;
          h2Diff->Fill( DifX, DifY,1./(index1*index2));
        }
      }
      PointCZDC->SetPoint(0, CenpX, CenpY); // Z 值將自動對應顏色
      // PointT1->SetMarkerStyle(21);
      // PointT2->SetMarkerStyle(21);
      PointCZDC->SetMarkerStyle(20);
      PointCZDC->SetMarkerColorAlpha(2,0.9);
      PointIntercept->SetMarkerStyle(20);
      PointIntercept->SetMarkerColorAlpha(kGreen,0.6);
      PointDiff->SetMarkerStyle(20);
      PointDiff->SetMarkerColorAlpha(kBlack,0.6);
      if(i<10){
        c3->cd(1);
        h2T1->Draw("col");
        c3->cd(2);
        h2BD0->Draw("");
        h2BD0->GetZaxis()->SetRangeUser(0,h2D0->GetMaximum());
        h2D0->Draw("ColzTextsame"); // "PCOL" 表示按 Z 值上色的散點圖
        PointCZDC->Draw("psame");
        PointIntercept->Draw("psame");
        c3->cd(3);
        h2T2->Draw("col");
        c3->cd(4);
        h2DiffB->Draw("col");
        PointDiff->Draw("psame");
        c3->Print(Form("%s/C_%05.0f.gif",SavePath.data(),i*1.));
      }
      h2D0->Reset();
      h2T1->Reset();
      h2T2->Reset();

      // 根據需求，進行進一步的數據處理
    }
    TCanvas *c1 = new TCanvas("c1","c1",640+4,640+28);
    c1->cd();
    h2Diff->Draw("colz");
      
    c1->Print(Form("%s/Diff.gif",SavePath.data()));
    // 關閉文件
    file->Close();
    delete file;
}
