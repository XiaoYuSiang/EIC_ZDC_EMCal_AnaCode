#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
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
#include "./GIDMapping.h"
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"
#include "/data4/YuSiang/personalLib/RPU/DBMLayouts.h"
#include "/data4/YuSiang/personalLib/EFFTool/MemoryClear.h"

using namespace std;

string defRFM = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/CosmicRay_Full_System_ReCon.root";
string defSP  = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/graphRec/";
string SavePath = "";

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

void DrawTriggerLinesCenter( TTree *t , const int iDet ,int iwt = 0 ) {
  string BackStr = sfWtNames[iwt].Data(),TitStr = WtNames[iwt].Data();
    setTDRStyle();
    TGaxis::SetMaxDigits(3);
    gStyle->SetTitleYOffset(1.250);

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
    TH2F *h2D2 = new TH2F("h2D2","PWO Z=-7.55mm;X position (mm);Y position (mm);ADC",6,-40.65-10.25,64.35+10.25,6,68.92-10.25,173.92+10.25);
    // boxh3->SetStats(0);
    h2T1->SetStats(0);
    h2T2->SetStats(0);
    h2D1->SetStats(0);
    h2D2->SetStats(0);
    
    h2BD0->SetStats(0);
    
    TCanvas* c3 = new TCanvas("c3", "3D Scatter Plot with Color", 600*2+3, 600*2+28);
    c3->cd()->Divide(2,2);
    TH2F *h2DiffB1 = new TH2F("h2DiffB1","",71,-74.55,+74.55,71,-74.55,+74.55);
    h2DiffB1->SetTitle(Form("Position Residual%s @LYSO;#Delta X (mm);#Delta Y position (mm);ADC",TitStr.data()));
    TH2F *h2DiffB2 = new TH2F("h2DiffB2","",71,-74.55,+74.55,71,-74.55,+74.55);
    h2DiffB2->SetTitle(Form("Position Residual%s@PbWO4;#Delta X (mm);#Delta Y position (mm);ADC",TitStr.data()));
    TH2F *h2Diff1 = new TH2F("h2Diff1","",71,-74.55,+74.55,71,-74.55,+74.55);
    h2Diff1->SetTitle(Form("Position Residual%s@LYSO;#Delta X (mm);#Delta Y position (mm);ADC",TitStr.data()));
    TH2F *h2Diff2 = new TH2F("h2Diff2","",71,-74.55,+74.55,71,-74.55,+74.55);
    h2Diff2->SetTitle(Form("Position Residual%s@PbWO4;#Delta X (mm);#Delta Y (mm);ADC",TitStr.data()));
    TH2F *h2Diff2C = new TH2F("h2Diff2C","",71,-74.55,+74.55,71,-74.55,+74.55);
    h2Diff2C->SetTitle(Form("Position Residual(Combination)%s@PbWO4;#Delta X (mm);#Delta Y (mm);ADC",TitStr.data()));
    TH1F *h1DiffR = new TH1F("h1DiffR","",84,0,120);
    h1DiffR->SetTitle(Form("R@PbWO4%s;R=#sqrt{#DeltaX^{2}+#DeltaY^{2}} (mm);count",TitStr.data()));
    h1DiffR->SetStats(1);
    TH1F *h1DiffR_PC = new TH1F("h1DiffR_PC","",84,0,120);
    h1DiffR_PC->SetTitle(Form("R(Cali.)@PbWO4%s;R=#sqrt{(#DeltaX-#mu#DeltaX)^{2}+(#DeltaY-#mu#DeltaY)^{2}} [mm];count",TitStr.data()));
    h1DiffR_PC->SetStats(1);
    TH2F *h2Dir = new TH2F("h2Dir","",71,-1,1,71,-1,1);
    h2Dir->SetTitle(Form("Direction%s;dX/dZ;dY/dZ;count",TitStr.data()));
    TH2F *hThetaPhi = new TH2F("hThetaPhi","",100, -TMath::Pi(), TMath::Pi(),100, 0, TMath::Pi()/2.);
    hThetaPhi->SetTitle(Form("Direction%s;#phi(rad);#theta(rad);count",TitStr.data()));
    h2Dir->SetStats(1);
    hThetaPhi->SetStats(1);

    vector< pair<double,double> > vDatas;
    for (Long64_t i = 0; i < nEntries; i++) {
      t->GetEntry(i); 
      int NowHit = 0;
      vector<Hit> Trigger1PosX, Trigger1PosY, Trigger2PosX, Trigger2PosY;
      vector<Hit> ZDC2;
      // cout<<nHits<<endl;
      if(nHits2<=0) continue;
      if(nHits0>20&&nHits1>20) continue;
      for(int iH = 0; iH<nHits;iH++){
        double px = pX->at(iH), py = pY->at(iH), pz = pZ->at(iH);
        int iz = iZ->at(iH);
        int ihit = iHit->at(iH);
        double Weight = GADC->at(iH)+1200;
        // if(iwt==0) Weight = GADC->at(iH)+1200;
        if(iwt==1) Weight = 1;
        if(iz<5){
          if(iwt==2&&ihit!=0) continue;
          if(iz==1) Trigger2PosX.push_back(Hit(px,0,pz,Weight));
          else if(iz==2) Trigger2PosY.push_back(Hit(0,py,pz,Weight));
          else if(iz==3) Trigger1PosX.push_back(Hit(px,0,pz,Weight));
          else if(iz==4) Trigger1PosY.push_back(Hit(0,py,pz,Weight));
          // if(i<20) cout<<iz<<" "<<px<<" "<<py<<" "<<pz<<" "<<Weight<<endl;
        }else if(iz==5){
          h2D2->Fill(px,py,Weight);
          ZDC2.push_back(Hit(px,py,pz,Weight));
        }

      }
      vector<Hit> 
        Trigger1 = GetCombination(Trigger1PosX,Trigger1PosY),
        Trigger2 = GetCombination(Trigger2PosX,Trigger2PosY);
      
      
      
      Hit CenterT1 = GetCenter(Trigger1);
      Hit CenterT2 = GetCenter(Trigger2);
      Hit CenterD2 = GetCenter(ZDC2);
      double AVGDisT1 = AverageDistance(Trigger1,CenterT1);
      double AVGDisT2 = AverageDistance(Trigger2,CenterT2);
      if(AVGDisT1>8||AVGDisT2>8) continue;
      TGraph* PointDiff2 = new TGraph();
      TGraph* PointDiffC2 = new TGraph();
      TGraph* PITC1 = new TGraph();
      TGraph* PITC2 = new TGraph();
      TGraph* PointCZDC2 = new TGraph();
      TGraph* PointCT1 = new TGraph();
      TGraph* PointCT2 = new TGraph();
      TGraph* PITC1CTZT = new TGraph();
      TGraph* PITC2CTZT = new TGraph();
      CheckDataRM(Trigger1,CenterT1);
      CheckDataRM(Trigger2,CenterT2);
      CheckDataRMZDC2(ZDC2,CenterD2);
      CenterT1 = GetCenter(Trigger1);
      CenterT2 = GetCenter(Trigger2);
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
      const auto [cxD2, cyD2, czD2, cwD2] = CenterD2 ;
      for (const auto& [xi2, yi2, zi2, wi2] : Trigger1){
        for (const auto& [xi1, yi1, zi1, wi1] : Trigger2){
          // const auto& [cxD1, cyD1, czD1, cwD1] : CenterD1 ;
          double t = (-7.51 - zi1) / (zi2 - zi1);
          double xtmp = xi1 + t * (xi2 - xi1);
          double ytmp = yi1 + t * (yi2 - yi1);
          PITC1->SetPoint(indexAll,xtmp,ytmp);
          // cout<<cxD1<<" "<<cyD1<<endl;
          // cout<<DifX<<" "<<DifY<<" "<<1./(index1*index2)<<endl;
          
        // const auto& [cxD2, cyD2, czD2, cwD2] : CenterD2 ;
        
          t = (-7.55 - zi1) / (zi2 - zi1);
          xtmp = xi1 + t * (xi2 - xi1);
          ytmp = yi1 + t * (yi2 - yi1);
          PITC2->SetPoint(indexAll,xtmp,ytmp);
          double DifX = cxD2-xtmp, DifY = cyD2-ytmp;
          PointDiff2->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
          h2Diff2C->Fill( DifX, DifY,1./(index1*index2));
        
          indexAll++;
        }
      }
      PointCZDC2->SetPoint(0, cxD2, cyD2); // Z 值將自動對應顏色
      PointCZDC2->SetMarkerStyle(20);
      PointCZDC2->SetMarkerColorAlpha(2,0.9);
      // PointCZDC2->SetPoint(0, CenpX, CenpY); // Z 值將自動對應顏色
      // PointCZDC2->SetMarkerStyle(20);
      // PointCZDC2->SetMarkerColorAlpha(2,0.9);
      double CT1x = CenterT1.x, CT2x =CenterT2.x, CT1y =CenterT1.y, CT2y =CenterT2.y;
      double ttmp1 = (-7.51 - CenterT1.z) / (CenterT2.z - CenterT1.z);
      double CTIx1 = CT1x + ttmp1 * (CT2x - CT1x), CTIy1 = CT1y + ttmp1 * (CT2y - CT1y);
      double ttmp2 = (-7.55 - CenterT1.z) / (CenterT2.z - CenterT1.z);
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
      PointDiff2->SetMarkerStyle(20);
      PointDiff2->SetMarkerColorAlpha(kBlack,0.6);
      PointDiffC2->SetPoint(0, cxD2-CTIx2, cyD2-CTIy2);
      PointDiffC2->SetMarkerStyle(29);
      PointDiffC2->SetMarkerSize(2);
      PointDiffC2->SetMarkerColorAlpha(kRed,0.6);
      vDatas.push_back({cxD2-CTIx2, cyD2-CTIy2});
      h2Diff2->Fill( cxD2-CTIx2, cyD2-CTIy2 );
      h1DiffR->Fill( sqrt( pow(cxD2-CTIx2,2)+ pow(cyD2-CTIy2,2) ) );
      if(i<20){
        c3->cd(1);
        h2T2->Draw("col");
        PointCT2->Draw("psame");
        c3->cd(2);
        h2T1->Draw("col");
        PointCT1->Draw("psame");
        c3->cd(3);
        h2D2->Draw("ColzText"); // "PCOL" 表示按 Z 值上色的散點圖
        PointCZDC2->Draw("psame");
        // PITC2->Draw("psame");
        PITC2CTZT->Draw("psame");
        c3->cd(4);
        h2DiffB2->Draw("col");
        // PointDiff2->Draw("psame");
        PointDiffC2->Draw("psame");
        c3->Print(Form("%s/C_%05.0f_D%d_%s.gif",(SavePath+BackStr).data(),i*1.,iDet,BackStr.data()));
      }
      h2D0->Reset();
      h2D1->Reset();
      h2D2->Reset();
      h2T1->Reset();
      h2T2->Reset();

      // 根據需求，進行進一步的數據處理
    }
    setTDRStyle();
    LayoutProfile2DObj layoutD2(h2Diff2, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2.canvas->Print(Form("%s/RSD_PosBMPDandZDCD%d_%s.gif",SavePath.data(),iDet,BackStr.data()));
    layoutD2.Layout2x1D(600,"hep","hep");
    layoutD2.Print2x1D(Form("%s/RSD_PosBMPDandZDCD%d_2x1D_%s.gif",SavePath.data(),iDet,BackStr.data()));
    setTDRStyle();
    LayoutProfile2DObj layoutD2C(h2Diff2C, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2C.canvas->Print(Form("%s/RSD_PosCBMPDandZDCD%d_%s.gif",SavePath.data(),iDet,BackStr.data()));
    layoutD2C.Layout2x1D(600,"hep","hep");
    layoutD2C.Print2x1D(Form("%s/RSD_PosCBMPDandZDCD%d_2x1D_%s.gif",SavePath.data(),iDet,BackStr.data()));
    
    
    setTDRStyle();
    gStyle->SetOptFit(0);
    TF1 *fRay = new TF1("fRay", "[0] * x / ([1]*[1]) * exp(-x*x / (2*[1]*[1]))",0,h1DiffR->GetBinCenter(h1DiffR->GetMaximumBin())+h1DiffR->GetRMS()*2);
    fRay->SetTitle("Rayleigh");
    fRay->SetParNames("Norm","#sigma");
    fRay->SetLineColor(kRed);
    fRay->SetLineWidth(2);
    TCanvas *c0 = new TCanvas("c0","c0",640+4,640+28); c0->cd();
    double MaxY = h1DiffR->GetMaximum();
    h1DiffR->Scale(1/MaxY);
    h1DiffR->GetYaxis()->SetTitle(Form("Ratio(Max=%.1e)",MaxY));
    h1DiffR->GetYaxis()->SetRangeUser(0,1.2);
    h1DiffR->Draw("he");
    fRay->SetParameters(h1DiffR->GetMaximum(), h1DiffR->GetRMS()); // reasonable initial values
    fRay->SetParLimits(1, 0, 120);
    h1DiffR->Fit(fRay,"R");   // R = respect fit range
    fRay->Draw("lsame");
    DrawFitInfo(fRay, 0.50,0.8,0.05,0b11011,4);
    c0->Print(Form("%s/RSD_Pos_R_%s_D%d.gif",SavePath.data(),BackStr.data(),iDet));  

    double MeanDx = h2Diff2->GetMean(1);
    double MeanDy = h2Diff2->GetMean(2);
    for(size_t i=0;i<vDatas.size();i++)
      h1DiffR_PC->Fill(sqrt(pow(vDatas[i].first-MeanDx,2)+pow(vDatas[i].second-MeanDy,2)));
    
    double MaxYPC = h1DiffR_PC->GetMaximum();
    h1DiffR_PC->Scale(1/MaxYPC);
    TF1 *fRay_PC = (TF1*) fRay->Clone("fRay_PC");
    fRay_PC->SetParameters(h1DiffR_PC->GetMaximum(), h1DiffR_PC->GetRMS()); // reasonable initial values
    fRay_PC->SetParLimits(1, 0, 120);
    h1DiffR_PC->GetYaxis()->SetTitle(Form("Ratio(Max=%.1e)",MaxYPC));
    h1DiffR_PC->GetYaxis()->SetRangeUser(0,1.2);
    h1DiffR_PC->Fit(fRay_PC,"R");
    
    h1DiffR_PC->Draw("colz");
    DrawFitInfo(fRay_PC, 0.50,0.8,0.05,0b11011,4);
    fRay_PC->Draw("lsame");
    c0->Print(Form("%s/RSD_Pos_R_PC_%s_D%d.gif",SavePath.data(),BackStr.data(),iDet));  
    
    
    ofstream ofS(Form("%s/RSD_DR_ZDC_%s_D%d.dat",SavePath.data(),BackStr.data(),iDet));
    ofS<<"SigmaR\tESigmaR\tSigmaRPCal\tESigmaRPCal\n";
    ofS<<fRay->GetParameter(1)<<"\t"<<fRay->GetParError(1)<<"\t";
    ofS<<fRay_PC->GetParameter(1)<<"\t"<<fRay_PC->GetParError(1)<<endl;
    ofS.close();
    
    gStyle->SetOptStat(1);
    GSStatsPadNDC(0.7,0.7,.98,.95);
    h2Dir->SetStats(1);
    hThetaPhi->SetStats(1);
    TCanvas *c1 = new TCanvas("c1","c1",640*2+4,640+28);
    c1->Divide(2,1,0.001,0.001);
    c1->cd(1);
    h2Dir->Draw("colz");
    c1->cd(2);
    hThetaPhi->Draw("colz");
    c1->Print(Form("%s/Direction_%s.gif",SavePath.data(),BackStr.data()));
    
    LayoutProfile2DObj layoutD2Slope(h2Dir, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2Slope.canvas->Print(Form("%s/Direction_S_%s.gif",SavePath.data(),BackStr.data()));
    layoutD2Slope.Layout2x1D(600,"hep","hep");
    layoutD2Slope.Print2x1D(Form("%s/Direction_S_2x1D_%s.gif",SavePath.data(),BackStr.data()));
    
    LayoutProfile2DObj layoutD2TF(hThetaPhi, 1080, "Colz", "", "");
    layoutD2TF.canvas->Print(Form("%s/Direction_TF_%s.gif",SavePath.data(),BackStr.data()));
    layoutD2TF.Layout2x1D(600,"hep","hep");
    layoutD2TF.Print2x1D(Form("%s/Direction_TF_2x1D_%s.gif",SavePath.data(),BackStr.data()));
    
    TCanvas *c2 = new TCanvas("c2","c2",640*1+4,640*2+28);
    c2->Divide(1,2,0.001,0.001);
    c2->cd(1);
    h2Dir->Draw("colz");
    c2->cd(2);
    hThetaPhi->Draw("colz");
    c2->Print(Form("%s/Direction_%s_Transpose.gif",SavePath.data(),BackStr.data()));
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    CleanupHistogramsAndCanvases();
}

void DrawTriggerLinesCenter( const string dirAnaPath , const string FileName, const int iDet ){
  string SourceFileNameEvents = dirAnaPath+FileName+"_ReCon.root";
  
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  TTree *t = (TTree*) file->Get("t");
  if (!t) {
      std::cerr << "Error: TTree pointer is null." << std::endl;
      std::cerr << "  DrawTriggerLinesCenter.C("<<dirAnaPath<<","<<FileName<<")"<< std::endl;
      return;
  }
  SavePath = dirAnaPath+string("/graphRec/");
  system(Form("mkdir -p %s",SavePath.data()));
  // iDet = TargetDet;
  for(int i=0;i<3;i++){
    system(Form("mkdir -p %s",(TString(SavePath)+sfWtNames[i]).Data()));
    DrawTriggerLinesCenter(t,iDet,i);
  }
  file->Close();
}
void DrawTriggerLinesCenter() {
  cout<< "Finished compile: DrawTriggerLinesCenter"<<endl;
  // DrawTriggerLinesCenter("/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/","Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4",4);

  // DrawTriggerLinesCenter("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2018_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_234606.131PbWO4/","Run2018_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_234606.131PbWO4",4);
}