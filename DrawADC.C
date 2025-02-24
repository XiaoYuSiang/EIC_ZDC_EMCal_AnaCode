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
#include "./GIDMapping.h"
using namespace std;
void DrawADC( const string dirAnaPath,  const string FileName,  const int iDetector = 0){
  map< int, Sensor > sensorMap = LoadSensorMap(iDetector);

  string dirGraph = dirAnaPath;
  system(Form("mkdir -p %s",dirGraph.data()));
  string SourceFileNameEvents = dirAnaPath+FileName+"_Sci.root";
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  TTree *t = (TTree*) file->Get("t");
  
  TH3D *h3 = new TH3D("h3","h3",NumIX,0,NumIX,NumIY,0,NumIY,1400,-3000,11000);
  TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", NumIX*800+4, NumIY*800+28);
  if(iDetector == 2|| iDetector ==3)
    t->Draw("ADC:int(GID/8):GID%8>>h3","1","goff");
  else 
    t->Draw("ADC:iY:iX>>h3","1","goff");
  // canvas->Print((dirAnaPath+"TH3F.png").data());
  canvas->Divide(NumIX, NumIY); // 8x8 排列
  TCanvas *canvas3 = new TCanvas("canvas3", "TH3F Slices", NumIX*800+4, NumIY*800+28);
  canvas3->Divide(NumIX, NumIY); // 8x8 排列

  // 迴圈處理每個 XBIN 和 YBIN
  for (int xbin = 1; xbin <= NumIX; ++xbin) {
    for (int ybin = 1; ybin <= NumIY; ++ybin) {
      // 計算子畫布索引
      int padIndex = (ybin - 1) * NumIX + xbin;
      canvas->cd(padIndex);

      // 投影到 Z 軸
      TH1D *hProjZ = h3->ProjectionZ(TString::Format("projZ_%d_%d", xbin, ybin), xbin, xbin, ybin, ybin,"cutg");
      // 設定投影直方圖標題和繪製
      if(iDetector == 2|| iDetector ==3)
        hProjZ->SetTitle(Form("GID = %d", xbin-1+ 8*(ybin-1)));
      else
        hProjZ->SetTitle(Form("XBIN=%d, YBIN=%d", xbin-1, ybin-1));
      hProjZ->GetXaxis()->SetTitle("ADC");
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
  canvas->Print(Form("%sADCVSCh_D%d.png",dirAnaPath.data(),iDetector));
  canvas3->Print(Form("%sADCVSChGauge_D%d.png",dirAnaPath.data(),iDetector));

  TCanvas *canvas2 = new TCanvas("canvas2", "",800+4, 800+28);
  
  canvas2->cd();
  TH2D *h2 = new TH2D("h2","h2",NumAllChs,0,NumAllChs,1400,-3000,11000);
  t->Draw("(HGMode? ADC/10. : ADC):channel+ROCID*32>>h2","1.*(HGMode? 1./10. : 1.)","colz");
  canvas2->Print(Form("%sGADCVSGch_D%d.png",dirAnaPath.data(),iDetector));

  canvas2->cd();
  TH1D *h1 = new TH1D("h1","h1",NumAllChs,0,NumAllChs);
  t->Draw("channel+ROCID*32>>h1","1","colz");
      h1->SetMarkerSize(2);
      h1->SetMarkerStyle(22);
      h1->Draw("eh");
  canvas2->Print(Form("%sNevGch_D%d.png",dirAnaPath.data(),iDetector));

  canvas2->cd();
  gStyle->SetPadRightMargin(0.2);
  gStyle->SetPadLeftMargin(0.1);
  if(iDetector == 2|| iDetector ==3){
    TCanvas *canvas4 = new TCanvas("canvas4", "",900+4, 400+28);
    canvas4->cd();
    canvas4->Divide(2,1);
    TH2D *h2PosX = new TH2D("h2PosX","h2PosX",NumChs_T2,0,NumChs_T2,1,0,1);
    TH2D *h2PosY = new TH2D("h2PosY","h2PosY",1,0,1,NumChs_T2,0,NumChs_T2);
    canvas4->cd(1);
    t->Draw("iY:iX-1 >>h2PosX","iY==0","colz");
    h2PosX->SetStats(0);
    if(iDetector==2) h2PosX->SetTitle("Hit of Position, Z=1,B.M.2X;index X;count");
    if(iDetector==3) h2PosX->SetTitle("Hit of Position, Z=3,B.M.1X;index Y;count");
    // h2Pos->GetYaxis()->SetBinLabel(1,"Y bars");
    h2PosX->GetYaxis()->SetBinLabel(1,"-");
    canvas4->cd(2);
    t->Draw("iY-1:iX >>h2PosY","iX==0","colz");
    h2PosY->SetStats(0);
    if(iDetector==2) h2PosY->SetTitle("Hit of Position, Z=2,B.M.2Y;index X;index Y;count");
    if(iDetector==3) h2PosY->SetTitle("Hit of Position, Z=4,B.M.1Y;index Y;count");
    // h2Pos->GetYaxis()->SetBinLabel(1,"Y bars");
    h2PosY->GetXaxis()->SetBinLabel(1,"-");
    // h2Pos->GetYaxis()->SetBinLabel(1,"Y bars");
    // h2Pos->GetYaxis()->SetBinLabel(2,"X bars");
    // t->Draw("ROCID:(iY==0?iX : iY)-1 >>h2Pos","","colz");
    // h2Pos->GetYaxis()->SetBinLabel(1,"Y bars");
    // h2Pos->GetYaxis()->SetBinLabel(2,"X bars");
    canvas4->Print(Form("%sHitVSPos_D%d.png",dirAnaPath.data(),iDetector));
  }else{
    TCanvas *canvas4 = new TCanvas("canvas4", "",900+4, 800+28);
    canvas4->cd();
    TH2D *h2Pos = new TH2D("h2Pos","h2Pos",NumIX,0,NumIX,NumIY,0,NumIY);
    t->Draw("iY:iX>>h2Pos","","colz");
    if(iDetector==1) h2Pos->SetTitle("Hit of Position, Z=5,LYSO;index Y;count");

    else h2Pos->SetTitle("Hit of Position;index X;index Y;count");
    h2Pos->SetStats(0);
    canvas4->Print(Form("%sHitVSPos_D%d.png",dirAnaPath.data(),iDetector));
  }
  // 關閉文件
  
  TCanvas *canvas6 = new TCanvas("canvas6", "",800+4, 800+28);
  canvas6->cd();
  TH1F *h1All = new TH1F("h1All","h1All",45,0,4500);
  if(iDetector == 1) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ LYSO;ADC;count"));
  if(iDetector == 2) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ B.M.1;ADC;count"));
  if(iDetector == 3) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ B.M.2;ADC;count"));
  t->Draw("GADC+2500>>h1All","nHits>1","he");
  TH1F *h1max = new TH1F("h1max","h1max",45,0,4500);
  h1max->SetLineColor(2);
  t->Draw("ADCmax+2500>>h1max","nHits>1","hesame");
  TLegend *LSpect = new TLegend(0.75,0.99,0.75,0.99);
  LSpect->AddEntry(h1All,"all GADC","le");
  LSpect->AddEntry(h1max,"GADC_{max}","le");
  canvas6->Print(Form("%sEmax_D%d.png",dirAnaPath.data(),iDetector));
  file->Close();
}
void DrawADC() {
  cout<<"Finished compiling of DrawADC.C+"<<endl;

}
