#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
#include <sstream>
#include <cmath>
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
using namespace std;
void DrawADC(
  const string dirAnaPath = "/data4/YuSiang/LYSOReadOut/Datas/125621.311_HV56/",
  const string FileName = "125621.311_HV56"
){
  string dirGraph = dirAnaPath;
  system(Form("mkdir -p %s",dirGraph.data()));
  string SourceFileNameEvents = dirAnaPath+FileName+"_Sci.root";
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  TTree *t = (TTree*) file->Get("t");
  
  TH3D *h3 = new TH3D("h3","h3",8,0,8,8,0,8,1400,-3000,11000);
  TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", 6400+4, 6400+28);
  t->Draw("ADC:iY:iX>>h3","1","goff");
  // canvas->Print((dirAnaPath+"TH3F.png").data());
  canvas->Divide(8, 8); // 8x8 排列
  TCanvas *canvas3 = new TCanvas("canvas3", "TH3F Slices", 6400+4, 6400+28);
  canvas3->Divide(8, 8); // 8x8 排列

  // 迴圈處理每個 XBIN 和 YBIN
  for (int xbin = 1; xbin <= 8; ++xbin) {
    for (int ybin = 1; ybin <= 8; ++ybin) {
      // 計算子畫布索引
      int padIndex = (ybin - 1) * 8 + xbin;
      canvas->cd(padIndex);

      // 投影到 Z 軸
      TH1D *hProjZ = h3->ProjectionZ(TString::Format("projZ_%d_%d", xbin, ybin), xbin, xbin, ybin, ybin,"cutg");
      // 設定投影直方圖標題和繪製
      hProjZ->SetTitle(TString::Format("XBIN=%d, YBIN=%d", xbin-1, ybin-1));
      hProjZ->GetXaxis()->SetTitle("Z Axis Range");
      hProjZ->GetYaxis()->SetTitle("Counts");
      hProjZ->SetMarkerSize(2);
      hProjZ->SetMarkerStyle(22);
      canvas->cd(padIndex);
      hProjZ->Draw("eh");
      double mean = hProjZ->GetMean(), rms = hProjZ->GetRMS();
      TH1D *hProjZRange = (TH1D*) hProjZ->Clone();
      hProjZRange->GetXaxis()->SetRangeUser(mean-5*rms,mean+5*rms);
      canvas3->cd(padIndex);
      hProjZRange->Draw("eh");
    }
  }

  // 儲存畫布為圖像
  canvas->Print((dirAnaPath+"ADCVSChannel.png").data());
  canvas3->Print((dirAnaPath+"ADCVSChannelGauge.png").data());
  TCanvas *canvas2 = new TCanvas("canvas2", "",800+4, 800+28);
  
  canvas2->cd();
  TH2D *h2 = new TH2D("h2","h2",64,0,64,1400,-3000,11000);
  t->Draw("(HGMode? ADC/10. : ADC):channel+ROCID*32>>h2","1.*(HGMode? 1./10. : 1.)","colz");
  canvas2->Print((dirAnaPath+"ADCVSGch.png").data());
  canvas2->cd();
  TH1D *h1 = new TH1D("h1","h1",64,0,64);
  t->Draw("channel+ROCID*32>>h1","1","colz");
      h1->SetMarkerSize(2);
      h1->SetMarkerStyle(22);
      h1->Draw("eh");
  canvas2->Print((dirAnaPath+"NevGch.png").data());

  canvas2->cd();
  TH2D *h2Pos = new TH2D("h2Pos","h2Pos",8,0,8,8,0,8);
  h2Pos->SetStats(0);
  t->Draw("iY:iX>>h2Pos","","colztext");
  canvas2->Print((dirAnaPath+"HitVSPos.png").data());
  // 關閉文件
  file->Close();
}