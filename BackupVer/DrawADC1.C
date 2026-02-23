#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <map>
#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TText.h>
#include <TFile.h>
#include <TPad.h>
#include <TTree.h>
#include <TColor.h>
#include <TStyle.h>
#include <TPDF.h>
#include <TPaveStats.h>
#include <TH2D.h>
#include <TString.h>
#include <TROOT.h>
#include <iostream>
#include <vector>
#include <TROOT.h>
#include <algorithm>

#include "./GIDMapping.h"
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"
#include "/data4/YuSiang/personalLib/RPU/DBMLayouts.h"
#include "/data8/ZDC/EMCal/ShareScript/GrystalBallFitLibs.h"
#include "/data4/YuSiang/personalLib/EFFTool/MemoryClear.h"

using namespace std;

std::vector<Int_t> *iX = nullptr;
std::vector<Int_t> *iY = nullptr;
std::vector<Int_t> *fHit_DID = nullptr;
vector<string> WeightStr = {"1", "EqGADC", "(iHit==0)"};
string dirGraph = "";

const int nB_BM_ADC = 100;
const double BM_ADCRang[2] = {-200, 200};
vector<string> NameDet = {"LYSO+SiPM60035", "LYSO", "B.M.1", "B.M.2", "PbWO"};

void DrawADC(TTree *t, const int iDet) {
  gROOT->ForceStyle();
  map<int, Sensor> sensorMap = LoadSensorMap(iDet);
  gStyle->SetTitleYOffset(1.25);
  TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
  setTDRStyle();
  gStyle->Copy(*oldStyle);
  TGaxis::SetMaxDigits(3);
  gStyle->SetTitleYOffset(1.00);
  bool bBMs = iDet == 2|| iDet ==3;
  GSPadMargins(0.15, 0.15, 0.005, 0.005);
  int NumCX = (bBMs) ? 16 : NumIX, NumCY = (bBMs) ? NumChs_T2*NumROCs_T2/16 : NumIY;
  // ==========================================================
  // [加速優化] I/O 白名單設定
  // ==========================================================
  t->SetBranchStatus("*", 0); // 關閉所有分支
  t->SetBranchStatus("ADC", 1);
  t->SetBranchStatus("iX", 1);
  t->SetBranchStatus("iY", 1);
  t->SetBranchStatus("GID", 1);
  t->SetBranchStatus("Fire", 1);
  t->SetBranchStatus("nHits", 1);
  t->SetBranchStatus("ADCmax", 1);
  t->SetBranchStatus("HGMode", 1);
  t->SetBranchStatus("channel", 1);
  t->SetBranchStatus("ROCID", 1);

  // 定義對應變數
  // --- 修正後的型別定義 ---
  std::vector<int>  *v_ADC = nullptr;
  std::vector<int>  *v_iX  = nullptr;
  std::vector<int>  *v_iY  = nullptr;
  std::vector<int>  *v_GID = nullptr;
  std::vector<int>  *v_Fire = nullptr;    // 檔案中 Fire 是 vector<int>
  std::vector<bool> *v_HGMode = nullptr;  // 檔案中 HGMode 是 vector<bool>
  std::vector<int>  *v_channel = nullptr;
  std::vector<int>  *v_ROCID = nullptr;
  
  Int_t nHits_val;    // nHits 是 Int_t (scalar)
  Int_t ADCmax_val;   // ADCmax 是 Int_t (scalar)

  // 重新設定 Branch 位址
  t->SetBranchAddress("ADC", &v_ADC);
  t->SetBranchAddress("iX", &v_iX);
  t->SetBranchAddress("iY", &v_iY);
  t->SetBranchAddress("GID", &v_GID);
  t->SetBranchAddress("Fire", &v_Fire);
  t->SetBranchAddress("HGMode", &v_HGMode);
  t->SetBranchAddress("channel", &v_channel);
  t->SetBranchAddress("ROCID", &v_ROCID);
  t->SetBranchAddress("nHits", &nHits_val);
  t->SetBranchAddress("ADCmax", &ADCmax_val);


  // ==========================================================
  // [加速優化] 使用單次迴圈填充所有直方圖 (取代 t->Draw)
  // ==========================================================
  TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", NumCX * 540 + 4, NumCY * 540 + 28);
  TH3D *h3;
  if (bBMs) h3 = new TH3D("h3", "h3", NumCX, 1, NumCX+1, NumCY, 0, NumCY, nB_BM_ADC, BM_ADCRang[0], BM_ADCRang[1]);
  else h3 = new TH3D("h3", "h3", NumCX, 0, NumCX, NumCY, 0, NumCY, 120, -1000, 11000);

  TH2D *hGADCVSGCh;
  if (iDet == 2 || iDet == 3) hGADCVSGCh = new TH2D("hGADCVSGCh", "", NumAllChs, 0, NumAllChs, nB_BM_ADC, BM_ADCRang[0], BM_ADCRang[1]);
  else hGADCVSGCh = new TH2D("hGADCVSGCh", "", NumAllChs, 0, NumAllChs, 1400, -3000, 11000);

  TH1D *hCtVsGCh = new TH1D("hCtVsGCh", "", NumAllChs, 0, NumAllChs);
  TH1F *h1All = (iDet == 2 || iDet == 3) ? new TH1F("h1All", "h1All", nB_BM_ADC / 2, BM_ADCRang[0] / 1000., BM_ADCRang[1] / 1000.) : new TH1F("h1All", "h1All", 110, 0, 11);
  TH1F *h1max = (TH1F*)h1All->Clone("h1max");

// --- 修正後的 Entry Loop (處理向量迭代) ---
  Long64_t nEntries = t->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    t->GetEntry(i);
    
    // 因為 ADC 是 vector，必須遍歷該事件內的所有 Hit
    for (size_t iH = 0; iH < v_ADC->size(); ++iH) {
        int cur_ADC = (*v_ADC)[iH];
        int cur_GID = (*v_GID)[iH];
        int ix = v_iX->at(iH);
        int iy = v_iY->at(iH);
    
        if (bBMs) {
          if (ix == 0) h3->Fill((iy-1)%16+1, (iy-1)/16+2, cur_ADC);
          if (iy == 0) h3->Fill((ix-1)%16+1, (ix-1)/16, cur_ADC);
        } else {
          h3->Fill(ix, iy, cur_ADC);
          // if(ix==3&&iy==1)cout<<"\t"<<ix<<"\t"<< iy<<"\t"<< summed_val<<endl;
        }
        double fADC = ((*v_HGMode)[iH] ? cur_ADC / 10. : cur_ADC);
        hGADCVSGCh->Fill((*v_channel)[iH] + (*v_ROCID)[iH] * 32, fADC);
        hCtVsGCh->Fill((*v_channel)[iH] + (*v_ROCID)[iH] * 32);
    }

    if (nHits_val > 1) {
      h1All->Fill(ADCmax_val / 1000.0);
      h1max->Fill(ADCmax_val / 10000.0);
    }
  }

  h3->Sumw2();
  double MaxIn3D = h3->GetMaximum() * 1.2;
  int Digit_3D = (MaxIn3D > 0) ? int(log10(MaxIn3D)) : 0;
  h3->Scale(1 / pow(10, Digit_3D));
  MaxIn3D = h3->GetMaximum() * 1.2;

  canvas->Divide(NumCX, NumCY, 0, 0);
  TCanvas *canvas3 = new TCanvas("canvas3", "TH3F Slices", NumCX * 540 + 4, NumCY * 540 + 28);
  canvas3->Divide(NumCX, NumCY, 0.001, 0.001);
  
  TH2D *h2ByH3ReduceZ = new TH2D("h2ByH3ReduceZ", "iX VS iY VS #muADC;index X;index Y;Mean ADC", NumCX, 0, NumCX, NumCY, 0, NumCY);

  for (int xbin = 1; xbin <= NumCX; ++xbin) {
    for (int ybin = 1; ybin <= NumCY; ++ybin) {
      int rootY = NumCY - ybin + 1;
      int rootPadIndex = (rootY - 1) * NumCX + xbin;
      
      double CSFX = canvas->GetPad(rootPadIndex)->GetAbsWNDC() / canvas->GetPad(NumCX*NumCY)->GetAbsWNDC();
      double CSFY = canvas->GetPad(rootPadIndex)->GetAbsHNDC() / canvas->GetPad(NumCX*NumCY)->GetAbsHNDC();

      TH1D *hProjZ = h3->ProjectionZ(TString::Format("projZ_%d_%d", xbin, ybin), xbin, xbin, ybin, ybin, "cutg");
      
      if (iDet == 2 || iDet == 3) hProjZ->SetTitle(Form("GID = %d;ADC;Counts #scale[0.8]{x10^{%d}}", xbin - 1 + 8 * (ybin - 1), Digit_3D));
      else hProjZ->SetTitle(Form(";kADC;Counts #scale[0.8]{x10^{%d}}", Digit_3D));

      hProjZ->SetMarkerSize(2);
      hProjZ->SetMarkerStyle(22);
      hProjZ->GetYaxis()->SetMaxDigits(1);
      hProjZ->GetYaxis()->SetTitleSize(0.09 / CSFY);
      hProjZ->GetYaxis()->SetLabelSize(0.07 / CSFY);
      hProjZ->GetYaxis()->SetTitleOffset(0.95 * CSFY);
      hProjZ->GetXaxis()->SetTitleSize(0.1 / CSFX);
      hProjZ->GetXaxis()->SetLabelSize(0.1 / CSFX);
      hProjZ->GetXaxis()->SetTitleOffset(0.75 * CSFX);
      hProjZ->GetXaxis()->SetNdivisions(-505);
      hProjZ->GetYaxis()->SetNdivisions(-505);

      canvas->cd(rootPadIndex);
      hProjZ->GetYaxis()->SetRangeUser(0, MaxIn3D);
      hProjZ->Draw("eh");

      double mean = hProjZ->GetMean(), rms = hProjZ->GetRMS();
      TH1D *hProjZRange = (TH1D*)hProjZ->Clone();
      hProjZRange->GetXaxis()->SetRangeUser(mean - 5 * rms, mean + 5 * rms);
      hProjZRange->GetXaxis()->SetTitle("ADC");
      hProjZRange->GetYaxis()->SetTitleOffset(0.9);
      hProjZRange->GetXaxis()->SetTitleSize(0.1);
      hProjZRange->GetXaxis()->SetLabelSize(0.1);
      hProjZRange->GetXaxis()->SetTitleOffset(0.75);
      hProjZRange->GetYaxis()->SetTitleSize(0.08);
      hProjZRange->GetYaxis()->SetLabelSize(0.06);
      hProjZRange->GetXaxis()->SetNdivisions(-505);
      hProjZRange->GetYaxis()->SetNdivisions(-505);

      canvas3->cd(rootPadIndex);
      hProjZRange->Draw("eh");
      h2ByH3ReduceZ->SetBinContent(xbin, ybin, hProjZRange->GetMean());
    }
  }

  canvas->Print(Form("%sADCVSCh_D%d.gif", dirGraph.data(), iDet));
  canvas3->Print(Form("%sADCVSChGauge_D%d.gif", dirGraph.data(), iDet));

  // 繪製 2D Mean 圖
  GSPadMargins(0.130, 0.130, 0.005, 0.050);
  TCanvas *canvas2D3D = new TCanvas("canvas2D3D", "", 800 + 4, 800 + 28);
  canvas2D3D->SetGrid(1, 1);
  h2ByH3ReduceZ->SetMarkerColor(2);
  h2ByH3ReduceZ->GetXaxis()->CenterLabels(1);
  h2ByH3ReduceZ->GetYaxis()->CenterLabels(1);
  h2ByH3ReduceZ->GetXaxis()->CenterTitle(1);
  h2ByH3ReduceZ->GetYaxis()->CenterTitle(1);
  h2ByH3ReduceZ->Draw("colztext");
  canvas2D3D->Print(Form("%sADCMeanVSiXiY_D%d.gif", dirGraph.data(), iDet));

  // 繪製 GADC vs GCh
  GSPadMargins(0.150,0.130,0.05,0.050);
  TCanvas *canvas2 = new TCanvas("canvas2", "", 800 + 4, 800 + 28);
  canvas2->cd();
  hGADCVSGCh->SetTitle(Form("GADC VS GID @%s; ch-GID;GADC", NameDet[iDet].data()));
  hGADCVSGCh->Draw("colz");
  canvas2->Print(Form("%sGADCVSGch_D%d.gif", dirGraph.data(), iDet));

  hCtVsGCh->SetTitle(Form("Hit VS GID @%s; ch-GID;count", NameDet[iDet].data()));
  hCtVsGCh->GetXaxis()->CenterTitle(1);
  hCtVsGCh->Draw("colz");
  hCtVsGCh->SetMarkerSize(2);
  hCtVsGCh->SetMarkerStyle(22);
  hCtVsGCh->Draw("eh");
  canvas2->Print(Form("%sNevGch_D%d.gif", dirGraph.data(), iDet));
  
  GSPadMargins(0.150,0.130,0.05,0.050);
  if(iDet==2||iDet==3){
    TCanvas *cIPVG = new TCanvas("cIPVG","",640*2+4,640*1+28);
    cIPVG->Divide(2,1);
    
    TH2D *hIxVSGCh= new TH2D("hIxVSGCh","",
                            NumAllChs/2+0.5,0,NumAllChs/2+0.5,
                            nB_BM_ADC,BM_ADCRang[0],BM_ADCRang[1]);
    hIxVSGCh->SetTitle(Form("GADC VS iX @%s; iX;GADC",NameDet[iDet].data()));
    TH2D *hIyVSGCh= new TH2D("hIyVSGCh","",
                            NumAllChs/2+0.5,0,NumAllChs/2+0.5,
                            nB_BM_ADC,BM_ADCRang[0],BM_ADCRang[1]);
    hIyVSGCh->SetTitle(Form("GADC VS iY @%s; iY;GADC",NameDet[iDet].data()));
    hIxVSGCh->GetXaxis()->CenterTitle(1);
    hIxVSGCh->GetYaxis()->CenterTitle(1);
    hIyVSGCh->GetXaxis()->CenterTitle(1);
    hIyVSGCh->GetYaxis()->CenterTitle(1);
    cIPVG->cd(1);
    t->Draw("(HGMode? ADC/10. : ADC):iX>>hIxVSGCh","1.*(iX!=0)","colz");
    cIPVG->cd(2);
    t->Draw("(HGMode? ADC/10. : ADC):iY>>hIyVSGCh","1.*(iY!=0)","colz");
    cIPVG->Print(Form("%siXiYVSGch_D%d.gif",dirGraph.data(),iDet));
    cIPVG->Close();
  }
  
  
  canvas2->cd();
  
  GSPadMargins(0.150,0.130,0.15,0.050);
  if(iDet == 2|| iDet ==3){
    TCanvas *canvas4 = new TCanvas("canvas4", "",900+4, 400+28);
    canvas4->cd();
    canvas4->Divide(2,1);
    TH2D *h2PosX = new TH2D("h2PosX","h2PosX",NumChs_T2,0,NumChs_T2,1,0,1);
    TH2D *h2PosY = new TH2D("h2PosY","h2PosY",1,0,1,NumChs_T2,0,NumChs_T2);
    h2PosX->GetXaxis()->CenterLabels(1);
    h2PosY->GetYaxis()->CenterLabels(1);
    canvas4->cd(1);
    t->Draw("iY:iX-1 >>h2PosX","iY==0","colz");
    h2PosX->SetStats(0);
    h2PosX->SetTitle(Form("Hit Position, Z=%d,%sX;index X;;",ZDetName[DName[iDet]+"X"],+DName[iDet].Data()));
    // h2Pos->GetYaxis()->SetBinLabel(1,"Y bars");
    h2PosX->GetYaxis()->SetBinLabel(1,"-");
    canvas4->cd(2);
    t->Draw("iY-1:iX >>h2PosY","iX==0","colz");
    h2PosY->SetStats(0);
    h2PosY->SetTitle(Form("Hit Position, Z=%d,%sY;;index Y;",ZDetName[DName[iDet]+"Y"],+DName[iDet].Data()));
    h2PosY->GetXaxis()->SetBinLabel(1,"-");
    canvas4->Print(Form("%sHitVSPos_D%d.gif",dirGraph.data(),iDet));
    canvas4->Close();
    
    TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(1);
    
    GSPadMargins(0.150,0.130,0.5,0.050);
    GSStatsPadNDC(0.7,0.7,1.0,1.0);
    
    gStyle->SetPadRightMargin(0.05);
    TCanvas *canvas5 = new TCanvas("canvas5", "",640*2+4, 640+28);
    canvas5->cd();
    canvas5->Divide(2,1,0.001,0.001);
    
    TH1D *h1PosX = new TH1D("h1PosX","",NumChs_T2,0,NumChs_T2);
    TH1D *h1PosY = new TH1D("h1PosY","",NumChs_T2,0,NumChs_T2);
    h1PosX->SetTitle(Form("Z=%d,%s X;index X;count",ZDetName[DName[iDet]+"X"],+DName[iDet].Data()));
    h1PosY->SetTitle(Form("Z=%d,%s Y;index Y;count",ZDetName[DName[iDet]+"Y"],+DName[iDet].Data()));
    
    canvas5->cd(1);  t->Draw("(iX-1) >>h1PosX","iY==0","he");
    canvas5->cd(2);  t->Draw("(iY-1) >>h1PosY","iX==0","he");
    canvas5->Print(Form("%sHitVSPosH1_D%d.gif",dirGraph.data(),iDet));
    h1PosX->SetTitle(Form("Z=%d,%s X;relative X[mm];count",ZDetName[DName[iDet]+"X"],+DName[iDet].Data()));
    h1PosY->SetTitle(Form("Z=%d,%s Y;relative Y[mm];count",ZDetName[DName[iDet]+"Y"],+DName[iDet].Data()));
    h1PosX->SetBins(NumChs_T2,0, NumChs_T2 * ChGapX[iDet]);
    h1PosY->SetBins(NumChs_T2,0, NumChs_T2 * ChGapY[iDet]);
    h1PosX->ResetStats();
    h1PosY->ResetStats();
    canvas5->cd(1);  h1PosX->Draw("he");
    canvas5->cd(2);  h1PosY->Draw("he");
    canvas5->Print(Form("%sHitVSPosH1_mm_D%d.gif",dirGraph.data(),iDet));
    
    TH1D *h1nHitX = new TH1D("h1nHitX","h1nHitX",NumChs_T2,0.5,NumChs_T2+0.5);
    TH1D *h1nHitY = new TH1D("h1nHitY","h1nHitY",NumChs_T2,0.5,NumChs_T2+0.5);
    h1nHitX->SetTitle(Form("Z=%d,%s X;Number of hit;count",ZDetName[DName[iDet]+"X"],+DName[iDet].Data()));
    h1nHitY->SetTitle(Form("Z=%d,%s Y;Number of hit;count",ZDetName[DName[iDet]+"Y"],+DName[iDet].Data()));
    h1nHitX->SetStats(1);  h1nHitY->SetStats(1);
    canvas5->cd(1)->SetLogy(1);
    t->Draw("Sum$( (iY==0)?1:0 ) >>h1nHitX","","he");
    canvas5->cd(2)->SetLogy(1);
    t->Draw("Sum$( (iX==0)?1:0 ) >>h1nHitY","","he");
    canvas5->Print(Form("%sHitVSnHit_Sep_D%d.gif",dirGraph.data(),iDet));
    
    TCanvas *canvas6 = new TCanvas("canvas6", "Overlay Plot", 800, 800);
    canvas6->SetLogy(1); // 通常 Hit 數分佈建議用 Log 軸
    canvas6->SetGrid();

    h1nHitX->SetLineColor(kBlack);
    h1nHitX->SetLineWidth(2);
    h1nHitX->SetTitle(Form("X & Y Hit Comparison, D%d;Number of hits;count", iDet));
    // 3. 設定第二個圖形 (Y) - 改為紅色
    h1nHitY->SetLineColor(kRed);
    h1nHitY->SetMarkerColor(kRed);
    h1nHitY->SetLineWidth(2);

    // 4. 開始繪圖
    // 第一張圖正常畫
    h1nHitX->Draw("he"); 
    h1nHitY->Draw("hesames");
    canvas6->Update();
    TPaveStats *st1 = (TPaveStats*)h1nHitX->FindObject("stats");
    if (st1) {
        st1->SetX1NDC(0.70); st1->SetX2NDC(0.95);
        st1->SetY1NDC(0.75); st1->SetY2NDC(0.90);
    }
    TPaveStats *st2 = (TPaveStats*)h1nHitY->FindObject("stats");
    if (st2) {
        st2->SetTextColor(kRed);
        st2->SetLineColor(kRed);
        st2->SetX1NDC(0.70); st2->SetX2NDC(0.95);
        st2->SetY1NDC(0.55); st2->SetY2NDC(0.70);
        canvas6->Modified(); 
    }
    canvas6->Print(Form("%sHitVSnHit_D%d.gif", dirGraph.data(), iDet));
    gStyle->Copy(*oldStyle);
    delete oldStyle;
  }else{
    GSStatsPadNDC(0.8,0.8,1.0,1.0);
    TH2D *h2Pos = new TH2D("h2Pos","",NumCX,0,NumIX,NumCY,0,NumCY);
    h2Pos->SetTitle(Form("Z=5,%s;Index iX;Index iY;",DName[iDet].Data()));
    t->Draw("iY:iX>>h2Pos","","goff");
    
    GSPadMargins(0.150,0.130,0.05,0.050);
    LayoutProfile2DObj layoutHPZDC(h2Pos, 1080, "colz","cTFit(Gaus)","cTFit(Gaus)");
    layoutHPZDC.canvas->Print(Form("%sHitVSPos_D%d.gif",dirGraph.data(),iDet));
    layoutHPZDC.canvas->cd();
    layoutHPZDC.h2->Draw("colz");
    layoutHPZDC.Print(Form("%sHitVSPos_2D_D%d.gif",dirGraph.data(),iDet));
    layoutHPZDC.Layout2x1D(600,"hep","hep");
    layoutHPZDC.Print2x1D(Form("%sHitVSPosH1_1D_D%d.gif",dirGraph.data(),iDet));
    
    h2Pos->SetTitle(Form("%s,%s;relative X[mm];relative Y[mm];",ZImf[iDet].Data(),DName[iDet].Data()));
    LayoutProfile2DObj layoutHPZDC_mm(h2Pos, 1080, Form("colz scale(%f,%f)",ChGapX[iDet],ChGapY[iDet]),"cTFit(Gaus)","cTFit(Gaus)");
    layoutHPZDC_mm.canvas->Print(Form("%sHitVSPos_mm_D%d.gif",dirGraph.data(),iDet));
    layoutHPZDC_mm.canvas->cd();
    layoutHPZDC_mm.h2->Draw("colz");
    layoutHPZDC_mm.canvas->Print(Form("%sHitVSPos_2D_mm_D%d.gif",dirGraph.data(),iDet));
    layoutHPZDC_mm.Layout2x1D(600,"hep","hep");
    layoutHPZDC_mm.Print2x1D(Form("%sHitVSPosH1_1D_mm_D%d.gif",dirGraph.data(),iDet));
    
  
    TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(1);
    GSPadMargins(0.150,0.130,0.05,0.050);
    GSStatsPadNDC(0.7,0.7,1.0,1.0);
    
    TCanvas *canvas5 = new TCanvas("canvas5", "",640+4, 640+28);
    canvas5->cd()->SetLogy(1);
    TH1D *h1nHit = new TH1D("h1nHit","",NumIX*NumCY,0.5,NumIX*NumCY+0.5);
    if(iDet==1) h1nHit->SetTitle("Z=5,LYSO;Number of hit;count");
    else h1nHit->SetTitle("Z=5,PbWO4;Number of hit;count;");
    h1nHit->SetStats(1);
    t->Draw("Sum$( (Fire) ) >>h1nHit","","he");
    canvas5->Print(Form("%sHitVSnHit_D%d.gif",dirGraph.data(),iDet));
    
    gStyle->Copy(*oldStyle);
    delete oldStyle;
  }
  // 關閉文件
  gStyle->SetOptStat(0);
  GSPadMargins(0.150,0.130,0.05,0.050);
  TCanvas *canvas6 = new TCanvas("canvas6", "", 800 + 4, 800 + 28);
  h1All->SetTitle("ADC spectrum@ " + DName[iDet] + ";kADC;ratio(max=1)");
  h1max->SetLineColor(2);
  h1max->SetMarkerColor(2);
  TLegend *LSpect = new TLegend(0.8, 0.8, 0.999, 0.999);
  LSpect->AddEntry(h1All, "All ADC", "le");
  LSpect->AddEntry(h1max, "ADC_{max}", "le");
  if (h1All->GetMaximum() > 0) h1All->Scale(1. / h1All->GetMaximum());
  if (h1max->GetMaximum() > 0) h1max->Scale(1. / h1max->GetMaximum());
  h1All->Draw("pe");
  h1max->Draw("pesame");
  LSpect->Draw();
  canvas6->Print(Form("%sEmax_D%d.gif", dirGraph.data(), iDet));
  gStyle->Copy(*oldStyle);
  CleanupHistogramsAndCanvases();
}

void DrawADC(const string dirAnaPath) {
  dirGraph = dirAnaPath + string("/graphRaw/");
  system(Form("mkdir -p %s", dirGraph.data()));
  for (size_t iDet = 0; iDet < AnaDets.size(); iDet++) {
    string SourceFileNameEvents = dirAnaPath + SymbolFile[AnaDets[iDet]] + "_Sci.root";
    TFile *file = TFile::Open(SourceFileNameEvents.data());
    TTree *t = (TTree*) file->Get("t");
    if (!t) {
        std::cerr << "Error: TTree pointer is null." << std::endl;
        std::cerr << "  DrawADC.C("<<dirAnaPath<<","<<SymbolFile[AnaDets[iDet]]<<")"<< std::endl;
        return;
    }
    DrawADC(t, AnaDets[iDet]);
    file->Close();
  }
}
void DrawADC() {
  cout<<"Finished compiling of DrawADC.C+"<<endl;
  // DrawADC("/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/");
}
