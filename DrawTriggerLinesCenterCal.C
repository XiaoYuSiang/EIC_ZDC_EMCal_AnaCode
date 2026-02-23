#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
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
string defSP  = "/data8/ZDC/EMCal/LYSOPWOBMCosmic/CosmicRay_Full_System/graphCal/";
extern vector<TString> WtNames;
extern vector<TString> sfWtNames;
string dirGraph = "";

TFile *fSaveHist;
void SaveOrUpdate(TH1* h,string Tag = "") {
  if (!fSaveHist || !h) return;
  fSaveHist->cd(); // 切換到存檔目錄
  h->Write(Form("%s%s",h->GetName(),Tag.data()), TObject::kOverwrite);
}
// void DrawTriggerLinesCenterCal(const string  RootFileName = defRFM,const string dirGraph = defSP) {
void DrawTriggerLinesCenterCal(TTree *t,const int iDet,const int iwt) {
    string wt = sfWtNames[iwt].Data();
    string wTitle =  WtNames[iwt].Data();
    setTDRStyle();
    TGaxis::SetMaxDigits(3);
    gStyle->SetTitleYOffset(1.250);
    // 打開 ROOT 文件
    system(Form("mkdir -p %s",dirGraph.data()));
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.15);


    TH2F *hRSD2 = new TH2F("hRSD2","",71,-74.55,+74.55,71,-74.55,+74.55);
    hRSD2->SetTitle(
            Form("Position Residual%s@%s;#Delta X (mm);#Delta Y (mm);count",wTitle.data(),DName[iDet].Data()));
    TH1F *h1RSDR = new TH1F("h1RSDR","",84,0,120);
    h1RSDR->SetTitle(Form("R@PbWO4%s;R=#sqrt{#DeltaX^{2}+#DeltaY^{2}} (mm);count",wTitle.data()));
    h1RSDR->SetStats(0);
    TString strCDT = Form("bGoodZDC%d&&bGoodBMT_%s",iDet, wt.data());
    TString strTarget = "CentP@D.Y()-CCpdZDC@D_@w.Y():CentP@D.X()-CCpdZDC@D_@w.X()>>@s";
    strTarget.ReplaceAll("@D",Form("%d",iDet));  strTarget.ReplaceAll("@w",sfWtNames[iwt]);
    strTarget.ReplaceAll("@s",hRSD2->GetName());
    t->Draw(strTarget.Data(),strCDT.Data(),"goff");
    TString strTargetR = "sqrt(pow(CCpdZDC@D_@w.Y()-CentP@D.Y(),2)+pow(CCpdZDC@D_@w.X()-CentP@D.X(),2))>>@s";
    strTargetR.ReplaceAll("@D",Form("%d",iDet));  strTargetR.ReplaceAll("@w",sfWtNames[iwt]);
    strTargetR.ReplaceAll("@s",h1RSDR->GetName());

    LayoutProfile2DObj layoutD2(hRSD2, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2.canvas->Print(Form("%s/RSD_PosBMPDandZDCD%d_%s.gif",dirGraph.data(),iDet,wt.data()));
    layoutD2.Layout2x1D(600,"hep","hep");
    layoutD2.Print2x1D(Form("%s/RSD_PosBMPDandZDCD%d_2x1D_%s.gif",dirGraph.data(),iDet,wt.data()));
    SaveOrUpdate(layoutD2.hX);
    SaveOrUpdate(layoutD2.hY);
    
    gStyle->SetOptFit(0);
    TCanvas *c0 = new TCanvas("c0","c0",640+4,640+28); c0->cd();
    t->Draw(strTargetR.Data(),strCDT.Data(),"he");
    TF1 *fRay = new TF1("fRay", "[0] * x / ([1]*[1]) * exp(-x*x / (2*[1]*[1]))",0,h1RSDR->GetBinCenter(h1RSDR->GetMaximumBin())+h1RSDR->GetRMS()*2);
    fRay->SetTitle("Rayleigh");
    fRay->SetParNames("Norm","#sigma");
    fRay->SetLineColor(kRed);
    fRay->SetLineWidth(2);
    fRay->SetParameters(h1RSDR->GetMaximum(), h1RSDR->GetRMS()); // reasonable initial values
    fRay->SetParLimits(1, 0, 120);
    h1RSDR->Fit(fRay,"R");   // R = respect fit range
    fRay->Draw("lsame");
    DrawFitInfo(fRay, 0.50,0.8,0.05,0b11011,4);
    c0->Print(Form("%s/RSD_Pos_R_%s_D%d.gif",dirGraph.data(),wt.data(),iDet));  
    // c0->Clear();
    double AlignX = layoutD2.h2->GetMean(1);
    double AlignY = layoutD2.h2->GetMean(2);
    
    TH2F *hRSD2_PC = new TH2F("hRSD2_PC","",71,-74.55,+74.55,71,-74.55,+74.55);
    hRSD2_PC->SetTitle(
          Form("Position Residual (Cali.)%s@%s;#Delta X (mm);#Delta Y (mm);count",wTitle.data(),DName[iDet].Data()));
            
    TH1F *h1RSDR_PC = new TH1F("h1RSDR_PC","",84,0,120);
    h1RSDR_PC->SetTitle(Form("R@PbWO4%s(Cali.);R=#sqrt{#DeltaX^{2}+#DeltaY^{2}} (mm);count",wTitle.data()));
    h1RSDR_PC->SetStats(0);
    
    TString strTargetA = "CentP@D.Y()-CCpdZDC@D_@w.Y()-@AY:CentP@D.X()-CCpdZDC@D_@w.X()-@AX>>@s";
    strTargetA.ReplaceAll("@D",Form("%d",iDet)); strTargetA.ReplaceAll("@w",sfWtNames[iwt]);
    strTargetA.ReplaceAll("@s",hRSD2_PC->GetName());
    strTargetA.ReplaceAll("@AX",Form("%f",AlignX)); strTargetA.ReplaceAll("@AY",Form("%f",AlignY));
    TString strTargetAR = "sqrt(pow(CCpdZDC@D_@w.Y()-CentP@D.Y()-@AY,2)+pow(CCpdZDC@D_@w.X()-CentP@D.X()-@AX,2))>>@s";
    strTargetAR.ReplaceAll("@D",Form("%d",iDet));  strTargetAR.ReplaceAll("@w",sfWtNames[iwt]);
    strTargetAR.ReplaceAll("@s",h1RSDR_PC->GetName());
    strTargetAR.ReplaceAll("@AX",Form("(%f)",AlignX)); strTargetAR.ReplaceAll("@AY",Form("(%f)",AlignY));
    t->Draw(strTargetA.Data(),strCDT.Data(),"goff");
    LayoutProfile2DObj layoutD2_PC(hRSD2_PC, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2_PC.canvas->Print(Form("%s/RSD_PosBMPDandZDCD%d_PC_%s.gif",dirGraph.data(),iDet,wt.data()));
    layoutD2_PC.Layout2x1D(600,"hep","hep");
    layoutD2_PC.Print2x1D(Form("%s/RSD_PosBMPDandZDCD%d_PC_2x1D_%s.gif",dirGraph.data(),iDet,wt.data()));
    SaveOrUpdate(layoutD2_PC.hX,"_OffsetCal");
    SaveOrUpdate(layoutD2_PC.hY,"_OffsetCal");
    // cout<<strTargetA<<endl; throw;
    c0->cd();
    cout<<t->Draw(strTargetAR.Data(),strCDT.Data(),"he")<<endl;;
    cout<<110<<" "<<strTargetAR<<endl; 
    
    TF1 *fRay_PC = (TF1*) fRay->Clone("fRay_PC");
    fRay_PC->SetParameters(h1RSDR_PC->GetMaximum(), h1RSDR_PC->GetRMS()); // reasonable initial values
    fRay_PC->SetParLimits(1, 0, 120);
    h1RSDR_PC->Draw("he");
    h1RSDR_PC->Fit(fRay_PC,"R");   // R = respect fit range
    fRay_PC->Draw("lsame");
    DrawFitInfo(fRay_PC, 0.50,0.8,0.05,0b11011,4);
    c0->Print(Form("%s/RSD_Pos_R_PC_%s_D%d.gif",dirGraph.data(),wt.data(),iDet));  
    
    // throw;
    
    setTDRStyle();    
    GSStatsPadNDC(0.6,0.6,0.95,1);
    gStyle->SetStatFont(42);       // 使用標準字體 (Precision 2)
    gStyle->SetStatFontSize(0.04);
    gStyle->SetOptStat(1);
    // gStyle->SetSta->SetStats(1);
    // hThetaPhi->SetStats(1);
    TH2F *h2Dir = new TH2F("h2Dir","Direction;dX/dZ;dY/dZ;count",61,-1,1,61,-1,1);
    TH2F *hThetaPhi = new TH2F("hThetaPhi","Direction;#phi(rad);#theta(rad);count",100, -TMath::Pi(), TMath::Pi(),100, 0, TMath::Pi()/2.);
    h2Dir->GetXaxis()->CenterTitle(1);
    h2Dir->GetYaxis()->CenterTitle(1);
    hThetaPhi->GetXaxis()->CenterTitle(1);
    hThetaPhi->GetYaxis()->CenterTitle(1);
    // h2Dir->SetStats(1);
    // hThetaPhi->SetStats(1);
    TCanvas *c1 = new TCanvas("c1","c1",640*2+4,640+28);
    c1->Divide(2,1,.0001,.0001);
    c1->cd(1);
    t->Draw(Form("BMDYDZ_%s:BMDXDZ_%s>>h2Dir",wt.data(),wt.data()),"","colz");
    c1->cd(2);
    t->Draw(Form("BMTheta_%s:BMPhi_%s>>hThetaPhi",wt.data(),wt.data()),"","colz");
    c1->Print(Form("%s/Direction_%s.gif",dirGraph.data(),wt.data()));
    TCanvas *c2 = new TCanvas("c2","c2",640*1+4,640*2+28);
    c2->Divide(1,2,0.001,0.001);
    c2->cd(1);
    h2Dir->Draw("colz");
    c2->cd(2);
    hThetaPhi->Draw("colz");
    c2->Print(Form("%s/Direction_%s_Transpose.gif",dirGraph.data(),wt.data()));
    // cout<<153<<endl;
    LayoutProfile2DObj layoutD2Slope(h2Dir, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");
    layoutD2Slope.canvas->Print(Form("%s/Direction_%s_S.gif",dirGraph.data(),wt.data()));
    layoutD2Slope.Layout2x1D(600,"hep","hep");
    layoutD2Slope.Print2x1D(Form("%s/Direction_2x1D_%s_S.gif",dirGraph.data(),wt.data()));
    SaveOrUpdate(layoutD2Slope.hX,"_dxdz");
    SaveOrUpdate(layoutD2Slope.hY,"_dydz");
    // cout<<158<<endl;
    LayoutProfile2DObj layoutD2TF(hThetaPhi, 1080, "Colz","he","he");
    // cout<<160<<endl;
    layoutD2TF.canvas->Print(Form("%s/Direction_%s_TF.gif",dirGraph.data(),wt.data()));
    // cout<<162<<endl;
    layoutD2TF.Layout2x1D(600,"hep","hep");
    SaveOrUpdate(layoutD2TF.hX,"_Theta");
    SaveOrUpdate(layoutD2TF.hY,"_Phi");
    // cout<<164<<endl;
    layoutD2TF.Print2x1D(Form("%s/Direction_2x1D_%s_TF.gif",dirGraph.data(),wt.data()));
    // cout<<166<<endl;
    // 關閉文件
    CleanupHistogramsAndCanvases();
}

void DrawTriggerLinesCenterCal( const string dirAnaPath ,  const string FileName ){
  
  string SourceFileNameEvents = dirAnaPath+FileName+"_ReConCalSort.root";
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  dirGraph = dirAnaPath+string("/graphCal/");
  string outHistName = dirAnaPath + FileName + "_ReConCal_Hist.root";
  fSaveHist = new TFile(outHistName.c_str(), "UPDATE");
  
  TTree *t = (TTree*) file->Get("t");
  if (!t) {
      std::cerr << "Error: TTree pointer is null." << std::endl;
      std::cerr << "  DrawTriggerLinesCenterCal.C("<<dirAnaPath<<","<<FileName<<")"<< std::endl;
      return;
  }
  system(Form("mkdir -p %s",dirGraph.data()));
  
  for(size_t iwt = 0; iwt < sfWtNames.size(); iwt++){
    for(size_t iZDC = 0; iZDC < CrystalDets.size(); iZDC++){
      int DID = CrystalDets[iZDC];
      DrawTriggerLinesCenterCal(t, DID, iwt);
    }
  }
  if (fSaveHist) {
    fSaveHist->Close();
    delete fSaveHist;
    fSaveHist = nullptr;
  }
  if (file)  file->Close();
}
void DrawTriggerLinesCenterCal() {
  cout<< "Finish compile: DrawTriggerLinesCenterCal"<<endl;
  // DrawTriggerLinesCenterCal("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/","Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4");

}