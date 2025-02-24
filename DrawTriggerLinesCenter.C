#include <vector>
#include <iostream>
#include <sstream>
#include <TMath.h>       // ROOT 檔案讀取
#include <TFile.h>       // ROOT 檔案讀取
#include <TTree.h>       // ROOT TTree
#include <TChain.h>       // ROOT TTree
#include <TH2F.h>        // 2D 直方圖
#include <TGraph.h>     // 畫布
#include <TCanvas.h>     // 畫布
#include <TStyle.h>      // ROOT 風格設定
#include <vector>        // C++ 動態陣列
#include <iostream>      // 標準輸出
#include <cmath>         // 數學函式，如 sqrt、pow
#include <cstdlib>       // system() 函式
#include <string>        // C++ 字串處理
#include <TStyle.h>  // 用於 gStyle
#include <TROOT.h>   // ROOT 全域變數（包含 gStyle）
#include <TH2F.h>  // 用於 TH2F 2D 直方圖

using namespace std;

string defRFM = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/CosmicRay_Full_System_ReCon.root";
string defSP  = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/Graph/";

struct Hit {
  double x, y, z, w;
public:
  Hit(double px,double py,double pz = 0,double wt = 0){
    x = px; y = py; z = pz; w = wt;
  }
};

vector<Hit> GetCombination(
    vector<Hit> pXZ,
    vector<Hit> pYZ
) {
    vector<Hit> results;
    for (const auto& posX : pXZ) {
        for (const auto& posY : pYZ) {
            double x = posX.x;
            double z1 = posX.z;
            double y = posY.y;
            double z2 = posY.z;
            double z_avg = (z1 + z2) / 2.0;
            double wt = posX.w * posY.w;
            results.push_back({x, y, z_avg,wt});
        }
    }
    return results;
}

double Distance(const Hit& p1, const Hit& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    double dz = p1.z - p2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

void CheckDataRM(vector<Hit>& data, const Hit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 8 || abs(data[i].y - center.y) > 8 || abs(data[i].z - center.z) > 8) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
void CheckDataRMZDC1(vector<Hit>& data, const Hit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 25 || abs(data[i].y - center.y) > 25) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
void CheckDataRMZDC2(vector<Hit>& data, const Hit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 50 || abs(data[i].y - center.y) > 50) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
/*
data.erase(remove_if(data.begin(), data.end(),
                     [&](const Hit& hit) { return shouldRemove(hit, center); }),
           data.end());
*/
bool shouldRemove(const Hit& data, const Hit& center) {
    return (abs(data.x - center.x) > 8 || abs(data.y - center.y) > 8 || abs(data.z - center.z) > 8);
}

double AverageDistance(const vector<Hit>& points,const Hit& center) {
    size_t n = points.size();
    if (n < 2) {
        return 0;
    }
    double totalDistance = 0.0;
    size_t count = 0;
    for (const auto& p : points) {
        totalDistance += pow(Distance(p, center),2);
        ++count;
    }
    return sqrt(totalDistance / count);
}

Hit GetCenter(const vector<Hit>& points) {
    size_t n = points.size();
    if (n == 0) {
        return {0, 0, 0};
    }
    Hit result = {0, 0, 0};
    double SumW = 0;
    for (const auto& p : points) {
      result.x += p.x * p.w;
      result.y += p.y * p.w;
      result.z += p.z * p.w;
      SumW+=p.w;
    }
    result.x /= SumW;
    result.y /= SumW;
    result.z /= SumW;
    return result;
}

void DrawTriggerLinesCenter() {
  cout<< "Finish compile: DrawTriggerLinesCenter"<<endl;
}
// void DrawTriggerLinesCenter(const string  RootFileName = defRFM,const string SavePath = defSP) {
void DrawTriggerLinesCenter(const string  RootFileName ,const string SavePath ) {
    std::vector<std::string> vRFN;
    std::stringstream ss(RootFileName);
    std::string token;
    while (std::getline(ss, token, ',')) vRFN.push_back(token);
    // 打開 ROOT 文件
    system(Form("mkdir -p %s",SavePath.data()));
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.15);
    // TFile *file = TFile::Open("/data8/ZDC/EMCal/LYSO2ABMCosmic/250118/CosRay_FV430_30V_FV320_x40_test2/CosRay_FV430_30V_FV320_x40_test2_ReCon.root");
    TChain *t = new TChain("t");
    for (const auto& word : vRFN) {
      TFile *file = TFile::Open(word.data());
      if (!file || file->IsZombie()) {
          cerr << "無法打開文件！" << endl;
          return;
      }
      file->Close();
      delete file;
      t->Add(word.data());
      std::cout << word << std::endl;
    }

    // 獲取 TTree
    // TTree *t = (TTree*)file->Get("t");
    // if (!t) {
        // cerr << "無法找到 TTree 't'！" << endl;
        // return;
    // }

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
    double OffT1[2] = {18,130};
    double OffT2[2] = {18,130};
    TH2F *h2T1 = new TH2F("h2T1","T1 Z=109.6mm;X position (mm);Y position (mm);-",32,-11.318-1.05,53.751+1.05,32,100.432-1.05,165.501+1.05);
    TH2F *h2T2 = new TH2F("h2T2","T2 Z=144.6mm;X position (mm);Y position (mm);-",32,-11.318-1.05,53.751+1.05,32,100.432-1.05,165.501+1.05);
    TH2F *h2BD0 = new TH2F("h2BD0","ZDC Z=5.3mm;X position (mm);Y position (mm);ADC",32,-SideSize-OffT2[0],SideSize-OffT2[0],32,-SideSize-OffT2[1],SideSize-OffT2[1]);
    TH2F *h2D0 = new TH2F("h2D0","",8,-28.8,28.8,8,-28.8,28.8);
    TH2F *h2D1 = new TH2F("h2D1","LYSO Z=-7.51mm;X position (mm);Y position (mm);ADC",8,-17.5,63.95,8,90.12,171.72);
    float centLYSOY = (90.12+171.72)/2.;
    TH2F *h2D2 = new TH2F("h2D2","PWO Z=-137.35mm;X position (mm);Y position (mm);ADC",6,-40.65-10.25,64.35+10.25,6,68.92-10.25,173.92+10.25);
    // boxh3->SetStats(0);
    h2T1->SetStats(0);
    h2T2->SetStats(0);
    h2D1->SetStats(0);
    h2D2->SetStats(0);
    // h2T1->GetXaxis()->SetRangeUser(-50,50);
    // h2T1->GetYaxis()->SetRangeUser(-50,50);
    // h2T2->GetXaxis()->SetRangeUser(-50,50);
    // h2T2->GetYaxis()->SetRangeUser(-50,50);
    // h2BD0->GetXaxis()->SetRangeUser(-50,50);
    // h2BD0->GetYaxis()->SetRangeUser(-50,50);
    h2BD0->SetStats(0);
    
    TCanvas* c3 = new TCanvas("c3", "3D Scatter Plot with Color", 600*4+4, 600*2+28);
    c3->cd()->Divide(4,2);
    TH2F *h2DiffB = new TH2F("h2DiffB","Position Difference;#Delta X (mm);#Delta Y position (mm);ADC",240,-120,120,240,-120,120);
    TH2F *h2Diff1 = new TH2F("h2Diff1","Position Difference @LYSO;#Delta X (mm);#Delta Y position (mm);ADC",240,-120,120,240,-120,120);
    TH2F *h2Diff2 = new TH2F("h2Diff2","Position Difference @PbWO4;#Delta X (mm);#Delta Y position (mm);ADC",240,-120,120,240,-120,120);
    TH2F *h2Dir = new TH2F("h2Dir","Direction;dX/dZ;dY/dZ;count",120,-1,1,120,-1,1);
    TH2F *hThetaPhi = new TH2F("hThetaPhi","Direction;#phi;#theta;count",100, -TMath::Pi(), TMath::Pi(),100, 0, TMath::Pi()/2.);

    for (Long64_t i = 0; i < nEntries; i++) {
      t->GetEntry(i); // 加載第 i 個 entry
      // 打印數據
      // cout << "Entry " << i << ":" << endl;
      // cout << "  eventID: " << eventID << endl;
      // cout << "  pcnt: " << pcnt << ", fcnt: " << fcnt << endl;
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
      vector<Hit> Trigger1PosX, Trigger1PosY, Trigger2PosX, Trigger2PosY;
      vector<Hit> ZDC1, ZDC2;
      // cout<<nHits<<endl;
      if(nHits2<=0) continue;
      if(nHits0>20&&nHits1>20) continue;
      for(int iH = 0; iH<nHits;iH++){
        double px = pX->at(iH), py = pY->at(iH), pz = pZ->at(iH);
        int iz = iZ->at(iH);
        double Weight = GADC->at(iH);
        if(iz==1) Trigger2PosX.push_back(Hit(px,0,pz,Weight+1200));
        else if(iz==2) Trigger2PosY.push_back(Hit(0,py,pz,Weight+1200));
        else if(iz==3) Trigger1PosX.push_back(Hit(px,0,pz,Weight+1200));
        else if(iz==4) Trigger1PosY.push_back(Hit(0,py,pz,Weight+1200));
        else if(iz==5){
          h2D1->Fill(px,py,Weight);
          ZDC1.push_back(Hit(px,py,pz,Weight));
        }else if(iz==6){
          h2D2->Fill(px,py,Weight);
          ZDC2.push_back(Hit(px,py,pz,Weight));
        } 

      }
      vector<Hit> 
        Trigger1 = GetCombination(Trigger1PosX,Trigger1PosY),
        Trigger2 = GetCombination(Trigger2PosX,Trigger2PosY);
      
      
      
      Hit CenterT1 = GetCenter(Trigger1);
      Hit CenterT2 = GetCenter(Trigger2);
      Hit CenterD1 = GetCenter(ZDC1);
      Hit CenterD2 = GetCenter(ZDC2);
      double AVGDisT1 = AverageDistance(Trigger1,CenterT1);
      double AVGDisT2 = AverageDistance(Trigger2,CenterT2);
      if(AVGDisT1>8||AVGDisT2>8) continue;
      TGraph* PointDiff1 = new TGraph();
      TGraph* PointDiff2 = new TGraph();
      TGraph* PointDiffC1 = new TGraph();
      TGraph* PointDiffC2 = new TGraph();
      TGraph* PITC1 = new TGraph();
      TGraph* PITC2 = new TGraph();
      TGraph* PointCZDC1 = new TGraph();
      TGraph* PointCZDC2 = new TGraph();
      TGraph* PointCT1 = new TGraph();
      TGraph* PointCT2 = new TGraph();
      TGraph* PITC1CTZT = new TGraph();
      TGraph* PITC2CTZT = new TGraph();
      CheckDataRM(Trigger1,CenterT1);
      CheckDataRM(Trigger2,CenterT2);
      CheckDataRMZDC1(ZDC1,CenterD1);
      CheckDataRMZDC2(ZDC2,CenterD2);
      CenterT1 = GetCenter(Trigger1);
      CenterT2 = GetCenter(Trigger2);
      CenterD1 = GetCenter(ZDC1);
      CenterD2 = GetCenter(ZDC2);
      // 塞入數據
      int indexAll = 0,index1 = 0,index2 = 0;
      for (const auto& [xi, yi, zi, wi] : Trigger1){
          // PointT1->SetPoint(index1++, xi, yi); // Z 值將自動對應顏色
          h2T1->Fill( xi, yi, wi); // Z 值將自動對應顏色 
          index1++;
      }
      for (const auto& [xi, yi, zi, wi] : Trigger2){
          h2T2->Fill( xi, yi, wi); // Z 值將自動對應顏色
          index2++;
      }
      const auto [cxD1, cyD1, czD1, cwD1] = CenterD1;
      const auto [cxD2, cyD2, czD2, cwD2] = CenterD2 ;
      for (const auto& [xi2, yi2, zi2, wi2] : Trigger1){
        for (const auto& [xi1, yi1, zi1, wi1] : Trigger2){
          // const auto& [cxD1, cyD1, czD1, cwD1] : CenterD1 ;
          double t = (-7.51 - zi1) / (zi2 - zi1);
          double xtmp = xi1 + t * (xi2 - xi1);
          double ytmp = yi1 + t * (yi2 - yi1);
          PITC1->SetPoint(indexAll,xtmp,ytmp);
          double DifX = cxD1-xtmp, DifY = cyD1-ytmp;
          PointDiff1->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
          h2Diff1->Fill( DifX, DifY,1./(index1*index2));
          // cout<<cxD1<<" "<<cyD1<<endl;
          // cout<<DifX<<" "<<DifY<<" "<<1./(index1*index2)<<endl;
          
        // const auto& [cxD2, cyD2, czD2, cwD2] : CenterD2 ;
        
          t = (-137.35 - zi1) / (zi2 - zi1);
          xtmp = xi1 + t * (xi2 - xi1);
          ytmp = yi1 + t * (yi2 - yi1);
          PITC2->SetPoint(indexAll,xtmp,ytmp);
          DifX = cxD2-xtmp, DifY = cyD2-ytmp;
          if(nHits3!=0) PointDiff2->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
          if(nHits3!=0) h2Diff2->Fill( DifX, DifY,1./(index1*index2));
        
          indexAll++;
        }
      }
      PointCZDC1->SetPoint(0, cxD1, cyD1); // Z 值將自動對應顏色
      PointCZDC1->SetMarkerStyle(20);
      PointCZDC1->SetMarkerColorAlpha(2,0.9);
      PointCZDC2->SetPoint(0, cxD2, cyD2); // Z 值將自動對應顏色
      PointCZDC2->SetMarkerStyle(20);
      PointCZDC2->SetMarkerColorAlpha(2,0.9);
      // PointCZDC2->SetPoint(0, CenpX, CenpY); // Z 值將自動對應顏色
      // PointCZDC2->SetMarkerStyle(20);
      // PointCZDC2->SetMarkerColorAlpha(2,0.9);
      double CT1x = CenterT1.x, CT2x =CenterT2.x, CT1y =CenterT1.y, CT2y =CenterT2.y;
      double ttmp1 = (-7.51 - CenterT1.z) / (CenterT2.z - CenterT1.z);
      double CTIx1 = CT1x + ttmp1 * (CT2x - CT1x), CTIy1 = CT1y + ttmp1 * (CT2y - CT1y);
      double ttmp2 = (-137.35 - CenterT1.z) / (CenterT2.z - CenterT1.z);
      double CTIx2 = CT1x + ttmp2 * (CT2x - CT1x), CTIy2 = CT1y + ttmp2 * (CT2y - CT1y);
      double slopex = (CT2x-CT1x)/(CenterT2.z-CenterT1.z), slopey = (CT2y-CT1y)/(CenterT2.z-CenterT1.z);
      h2Dir->Fill(slopex,slopey);
      hThetaPhi->Fill( TMath::ATan2(slopey, slopex), TMath::ATan2(sqrt(slopex*slopex + slopey*slopey), 1.0));
      PITC1CTZT->SetPoint(0,CTIx1,CTIy1);
      PITC1CTZT->SetMarkerStyle(29);
      PITC1CTZT->SetMarkerSize(2);
      PITC1CTZT->SetMarkerColorAlpha(2,0.9);
      PITC2CTZT->SetPoint(0,CTIx2,CTIy2);
      PITC2CTZT->SetMarkerStyle(29);
      PITC2CTZT->SetMarkerSize(2);
      PITC2CTZT->SetMarkerColorAlpha(2,0.9);
      PointCT1->SetPoint(0, CT1x, CT1y); // Z 值將自動對應顏色
      PointCT1->SetMarkerStyle(29);
      PointCT1->SetMarkerSize(2);
      PointCT1->SetMarkerColorAlpha(2,0.9);
      PointCT2->SetPoint(0, CT2x, CT2y); // Z 值將自動對應顏色
      PointCT2->SetMarkerStyle(29);
      PointCT2->SetMarkerSize(2);
      PointCT2->SetMarkerColorAlpha(2,0.9);
      PITC1->SetMarkerStyle(20);
      PITC1->SetMarkerColorAlpha(kGreen,0.6);
      PITC2->SetMarkerStyle(20);
      PITC2->SetMarkerColorAlpha(kGreen,0.6);
      PointDiff1->SetMarkerStyle(20);
      PointDiff1->SetMarkerColorAlpha(kBlack,0.6);
      PointDiff2->SetMarkerStyle(20);
      PointDiff2->SetMarkerColorAlpha(kBlack,0.6);
      PointDiffC1->SetPoint(0, cxD1-CTIx1, cyD1-CTIy1);
      PointDiffC1->SetMarkerStyle(29);
      PointDiffC1->SetMarkerSize(2);
      PointDiffC1->SetMarkerColorAlpha(kRed,0.6);
      if(nHits3!=0){
        PointDiffC2->SetPoint(0, cxD2-CTIx2, cyD2-CTIy2);
        PointDiffC2->SetMarkerStyle(29);
        PointDiffC2->SetMarkerSize(2);
        PointDiffC2->SetMarkerColorAlpha(kRed,0.6);
      }
      if(i<20){
        c3->cd(1);
        h2T2->Draw("col");
        PointCT2->Draw("psame");
        c3->cd(2);
        h2T1->Draw("col");
        PointCT1->Draw("psame");
        c3->cd(3);
        
        h2D1->Draw("ColzText"); // "PCOL" 表示按 Z 值上色的散點圖
        PointCZDC1->Draw("psame");
        PITC1->Draw("psame");
        PITC1CTZT->Draw("psame");
        c3->cd(4);
        h2D2->Draw("ColzText"); // "PCOL" 表示按 Z 值上色的散點圖
        PointCZDC2->Draw("psame");
        PITC2->Draw("psame");
        PITC2CTZT->Draw("psame");
        c3->cd(5);
        h2DiffB->Draw("col");
        PointDiff1->Draw("psame");
        PointDiffC1->Draw("psame");
        c3->cd(6);
        h2DiffB->Draw("col");
        PointDiff2->Draw("psame");
        PointDiffC2->Draw("psame");
        
        c3->Print(Form("%s/C_%05.0f.gif",SavePath.data(),i*1.));
      }
      h2D0->Reset();
      h2D1->Reset();
      h2D2->Reset();
      h2T1->Reset();
      h2T2->Reset();

      // 根據需求，進行進一步的數據處理
    }
    TCanvas *c1 = new TCanvas("c1","c1",640*2+4,640+28);
    c1->Divide(2,1);
    c1->cd(1);
    h2Diff1->Draw("colz");
    c1->cd(2);
    h2Diff2->Draw("colz");
      
    c1->Print(Form("%s/Diff.gif",SavePath.data()));
    
    c1->cd(1);
    h2Dir->Draw("colz");
    c1->cd(2);
    hThetaPhi->Draw("colz");
    c1->Print(Form("%s/Direction.gif",SavePath.data()));
    // 關閉文件
}
