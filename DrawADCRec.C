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
void DrawADCRec( const string dirAnaPath,  const string FileName,  const int iDetector = 1){
  map< int, Sensor > sensorMap = LoadSensorMap(iDetector);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadLeftMargin(0.13);

  string dirGraph = dirAnaPath;
  system(Form("mkdir -p %s",dirGraph.data()));
  string SourceFileNameEvents = dirAnaPath+FileName+"_ReCon.root";
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  TTree *t = (TTree*) file->Get("t");
  
  TH3D *h3 = new TH3D("h3","h3",NumIX,0,NumIX,NumIY,0,NumIY,1400,-3000,11000);
  TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", NumIX*800+4, NumIY*800+28);
  if(iDetector == 2|| iDetector ==3)
    t->Draw("ADC:int(GID/8):GID%8>>h3",Form("DID==%d",iDetector),"goff");
  else 
    t->Draw("ADC:iY:iX>>h3",Form("DID==%d",iDetector),"goff");
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
  canvas->Print(Form("%sADCVSCh_Track.png",dirAnaPath.data()));
  canvas3->Print(Form("%sADCVSChGauge_Track.png",dirAnaPath.data()));

  TCanvas *canvas2 = new TCanvas("canvas2", "",800+4, 800+28);
  
  canvas2->cd();
  TH2D *h2 = new TH2D("h2","h2",NumAllChs,0,NumAllChs,1400,-3000,11000);
  t->Draw("(HGMode? ADC/10. : ADC):channel+ROCID*32>>h2",Form("DID==%d*(HGMode? 1./10. : 1.)",iDetector),"colz");
  canvas2->Print(Form("%sGADCVSGch_Track.png",dirAnaPath.data()));

  canvas2->cd();
  TH1D *h1ChNeV = new TH1D("h1ChNeV","h1Chfre",NumAllChs,0,NumAllChs);
  t->Draw("channel+ROCID*32>>h1ChNeV",Form("DID==%d",iDetector),"colz");
  h1ChNeV->SetMarkerSize(2);
  h1ChNeV->SetMarkerStyle(22);
  h1ChNeV->Draw("eh");
  canvas2->Print(Form("%sNevGch_D%d.png",dirAnaPath.data(),iDetector));

  TCanvas *canvas6 = new TCanvas("canvas6", "",800+4, 800+28);
  canvas6->cd()->SetLogy(1);
  TH1F *h1All = new TH1F("h1All","ADC spectrum; GADC; count",90,0,110000);
  h1All->SetStats(0);
  h1All->SetLineColor(1);
  // if(iDetector == 1) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ LYSO;ADC;count"));
  // if(iDetector == 4) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ PbWO4;ADC;count"));
  // if(iDetector == 2) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ B.M.1;ADC;count"));
  // if(iDetector == 3) h1All->SetTitle(Form("ADC spectrum(nHits>1) @ B.M.2;ADC;count"));
  if(iDetector == 1) h1All->SetTitle(Form("ADC spectrum@ LYSO;ADC;count"));
  if(iDetector == 4) h1All->SetTitle(Form("ADC spectrum@ PbWO4;ADC;count"));
  if(iDetector == 2) h1All->SetTitle(Form("ADC spectrum@ B.M.1;ADC;count"));
  if(iDetector == 3) h1All->SetTitle(Form("ADC spectrum@ B.M.2;ADC;count"));
  t->Draw("GADC>>h1All",Form("DID==%d",4),"he");
  TH1F *h1max = new TH1F("h1max","h1max",90,0,110000);
  h1max->SetLineColor(2);
  t->Draw("GADC11>>h1max","","hesame");
  TH1F *h1sec = new TH1F("h1sec","h1sec",90,0,110000);
  h1sec->SetLineColor(4);
  TH1F *h1thr = new TH1F("h1thr","h1thr",90,0,110000);
  h1thr->SetLineColor(6);
  t->Draw("GADC[PeakID+1]>>h1sec","nHits2>1","hesame");
  t->Draw("GADC[PeakID+2]>>h1thr","nHits2>2","hesame");
  TLegend *LSpect = new TLegend(0.75,0.75,0.99,0.99);
  LSpect->AddEntry(h1All,Form("all GADC, Sta:%.1e",h1All->Integral()),"le");
  LSpect->AddEntry(h1max,Form("GADC_{max}, Sta:%.1e",h1max->Integral()),"le");
  LSpect->AddEntry(h1sec,Form("GADC_{rd}, Sta:%.1e",h1sec->Integral()),"le");
  LSpect->AddEntry(h1thr,Form("GADC_{th}, Sta:%.1e",h1thr->Integral()),"le");
  LSpect->Draw();
  canvas6->Print(Form("%sEmax_Track.png",dirAnaPath.data()));
  
  TH1F *h13x3 = new TH1F("h13x3","h13x3",270,0,300000);
  h13x3->SetLineColor(1);
  TH1F *h15x5 = new TH1F("h15x5","h15x5",270,0,300000);
  h15x5->SetLineColor(4);
  TH1F *h1SUM = new TH1F("h1SUM","Clustring;GADC;count",270,0,300000);
  h1SUM->SetLineColor(6);
  t->Draw("GADCAL>>h1SUM","","he");
  t->Draw("GADC33>>h13x3","","hesame");
  t->Draw("GADC55>>h15x5","","hesame");
  t->Draw("GADC11>>h1max","","hesame");
  h1SUM->GetYaxis()->SetRangeUser(1,h1SUM->GetMaximum()*10);
  TLegend *LCRing = new TLegend(0.75,0.75,0.99,0.99);
  LCRing->AddEntry(h1max,Form("GADC_{max}"),"le");
  LCRing->AddEntry(h13x3,Form("GADC_{3x3}"),"le");
  LCRing->AddEntry(h15x5,Form("GADC_{5x5}"),"le");
  LCRing->AddEntry(h1SUM,Form("GADC_{all}"),"le");
  LCRing->Draw();
  canvas6->Print(Form("%sClustering_Track_Spectrum.png",dirAnaPath.data()));

  canvas6->SetLogy(0);
  TH1F *h11b3 = new TH1F("h11b3","Clustering ratio;Clustering Ratio;count",100,0,1.01);
  TH1F *h11b5 = new TH1F("h11b5","h11b5",100,0,1.01);
  TH1F *h11bA = new TH1F("h11bA","h11bA",100,0,1.01);
  t->Draw("GADC11/GADC33>>h11b3","","he");
  t->Draw("GADC11/GADC55>>h11b5","","hesame");
  t->Draw("GADC11/GADCAL>>h11bA","","hesame");
  h11b3->SetLineColor(1);
  h11b5->SetLineColor(2);
  h11bA->SetLineColor(4);
  TLegend *LShowProfile = new TLegend(0.75,0.75,0.99,0.99);
  LShowProfile->AddEntry(h11b3,Form("GADC_{max}/GADC_{3x3}"),"le");
  LShowProfile->AddEntry(h11b3,Form("#mu = %2.2f",h11b3->GetMean()),"T");
  LShowProfile->AddEntry(h11b5,Form("GADC_{max}/GADC_{5x5}"),"le");
  LShowProfile->AddEntry(h11b5,Form("#mu = %2.2f",h11b5->GetMean()),"T");
  LShowProfile->AddEntry(h11bA,Form("GADC_{max}/GADC_{all}"),"le");
  LShowProfile->AddEntry(h11bA,Form("#mu = %2.2f",h11bA->GetMean()),"T");
  LShowProfile->Draw();
  canvas6->Print(Form("%sClustering_Track_Ratio.png",dirAnaPath.data()));
  
  ofstream ofs(Form("%sEmax_Track_sta.dat",dirAnaPath.data()));
  ofs<<h1max->Integral()<<"\t"<<h1max->GetMean()<<"\t"<<h1max->GetRMS()<<endl;
  ofs<<"#Sta\tMean\tSig\tR13\tR15\tR1A"<<endl;
  
  
  ofs.close();
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
    canvas4->Print(Form("%sHitVSPos_Track.png",dirAnaPath.data()));
  }else{
    TCanvas *canvas4 = new TCanvas("canvas4", "",900+4, 800+28);
    canvas4->cd();
    TH2D *h2Pos = new TH2D("h2Pos","h2Pos",NumIX,0,NumIX,NumIY,0,NumIY);
    t->Draw("iY:iX>>h2Pos",Form("DID==%d",iDetector),"colz");
    if(iDetector==4) h2Pos->SetTitle("Hit of Position, Z=5,LYSO;index Y;count");

    else h2Pos->SetTitle("Hit of Position;index X;index Y;count");
    h2Pos->SetStats(0);
    canvas4->Print(Form("%sHitVSPos_Track.png",dirAnaPath.data()));
  }
  // 關閉文件
  
  file->Close();
}
void DrawADCRec() {
  cout<<"Finished compiling of DrawADCRec.C+"<<endl;

}
