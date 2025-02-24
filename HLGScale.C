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
#include "/data4/YuSiang/personalLib/RPU/DBMTCanvas.h"
using namespace std;

#include "./GIDMapping.h"
TH1D* SmoothHistogramWithMovingAverage(TH1D* histogram, int windowSize) {
    // Create a temporary histogram to store smoothed values
    TH1D* smoothedHist = dynamic_cast<TH1D*>(histogram->Clone("smoothedHist"));

    // Apply moving average to smooth the histogram
    for (int i = 1; i <= histogram->GetNbinsX(); ++i) {
        double sum = 0.0;
        int count = 0;
        for (int j = i - windowSize; j <= i + windowSize; ++j) {
            if (j > 0 && j <= histogram->GetNbinsX()) {
                sum += histogram->GetBinContent(j);
                count++;
            }
        }
        double avg = sum / count;
        smoothedHist->SetBinContent(i, avg);
    }

    // Create a canvas and draw the smoothed histogram
    smoothedHist->SetLineColor(2);
    smoothedHist->SetName(histogram->GetName());
    // smoothedHist->Draw("hsame");
    return smoothedHist;
}

int findMinNonZeroBin(TH1* hist) {
  double smallval = 1E+7;
  int minNonZeroBin = -1;
  for (int i = 1; i <= hist->GetNbinsX(); ++i) {
    double val = hist->GetBinContent(i);
    // cout<<hist->GetBinCenter(i)<<" "<<val<<endl;
    if (val > 0&&val<smallval) {
      // cout<<val<<"<"<<smallval<<"@"<<i<<endl;
      smallval = val;
      minNonZeroBin = i;
      // break;
    }
  }
  return minNonZeroBin;
}

void HLGScale(
  const string dirAnaPath = "/data4/YuSiang/LYSOReadOut/Datas/125621.311_HV56/",
  const string FileName = "125621.311_HV56"
){
  string dirGraph = dirAnaPath;
  system(Form("mkdir -p %s",dirGraph.data()));
  string SourceFileNameEvents = dirAnaPath+FileName+"_Sci.root";
  string OutputRootFileName   = dirGraph+"ChannelADCDistribution.root";
  string OutputResultFileName = dirGraph+"RChHGDivLG.ptxt";
  string OutputReRootFileName = dirGraph+"RChHGDivLG.root";
  
  ifstream fin(OutputRootFileName.data(), ifstream::in | ifstream::binary);
  fin.seekg(0, ios::end); 
  Long64_t SizeOfRotSave = fin.tellg();
  fin.close(); cout<<SizeOfRotSave<<endl;
  bool  OptIsCreate = SizeOfRotSave > 5e+5 ? true : false;
  cout<<"OptIsCreate = "<<OptIsCreate<<"  !OptIsCreate = "<<!OptIsCreate<<endl;
  TFile *fileOpt = TFile::Open(OutputRootFileName.data());
  
  TFile *_file0  = TFile::Open(SourceFileNameEvents.data());
  gStyle->SetPadRightMargin(.15);
  TTree *t = (TTree*) _file0->Get("t");
  
  TCanvas *c1;
  TH2F *h2L, *h2H;
  TCanvas *c2;
  TH1F *h2Proj;
  TH1D *h2HProj[NumAllChs],*h2LProj[NumAllChs]; 
  string Name_h2HProj[NumAllChs], Name_h2LProj[NumAllChs];
  for(int i=0;i<NumAllChs;i++){
    Name_h2HProj[i] = Form("hChVHGADC=%02.f",i*1.);
    Name_h2LProj[i] = Form("hChVLGADC=%02.f",i*1.);
  }
  // cout<<61<<" "<<_file0->GetName()<<endl;

  if(!OptIsCreate){
  // if(1){
    cout<<"Craete the output file: "<<OutputRootFileName.data()<<endl;
    fileOpt = TFile::Open(OutputRootFileName.data(),"recreate"); 
    h2L = new TH2F("h2L","h2L",11000,0,11000,NumAllChs,0,NumAllChs);
    h2L->GetXaxis()->SetTitle("ADC Number: HG ADC or LG ADC");
    h2L->GetYaxis()->SetTitle("Channel ID + ROC ID *32");
    h2L->SetTitle("Channel VS ADC Number for self radiation");
    h2L->SetStats(0);
    h2L->GetZaxis()->SetTitle("Number of Hits");
    h2L->GetXaxis()->SetTitleOffset(1.4);
    h2L->GetZaxis()->SetTitleOffset(1.4);
    h2L->GetZaxis()->RotateTitle();
    h2H = new TH2F("h2H","h2H",1100,0,11000,NumAllChs,0,NumAllChs);
    h2H->GetXaxis()->SetTitle("ADC Number: HG ADC or LG ADC");
    h2H->GetYaxis()->SetTitle("Channel ID + ROC ID *32");
    h2H->SetTitle("Channel VS ADC Number for self radiation");
    h2H->SetStats(0);
    h2H->GetZaxis()->SetTitle("Number of Hits");
    h2H->GetXaxis()->SetTitleOffset(1.4);
    h2H->GetZaxis()->SetTitleOffset(1.4);
    h2H->GetZaxis()->RotateTitle();
    c1 = new TCanvas("c1","c1",640*2,480*2);
    c1->cd()->SetLogz(1);
    cout<<"Draw the H/L gain VS channel"<<endl;
    t->Draw("ROCID*32+channel:ADC>>h2H","HGMode","col2z");
    t->Draw("ROCID*32+channel:ADC>>h2L","!HGMode","col2z");
    cout<<"88"<<endl;
    fileOpt->WriteObject(h2H,h2H->GetName());
    fileOpt->WriteObject(h2L,h2L->GetName());
    gStyle->SetPadRightMargin(.1);
    c2 = EasyCanvas("c2",1600*4,1600*4,8,8,"false");
    h2Proj = new TH1F("h2Proj","",11000,0,11000);
    h2Proj->GetXaxis()->SetTitleOffset(1.4);
    h2Proj->GetYaxis()->SetTitleOffset(1.4);
    h2Proj->GetXaxis()->SetTitle("ADC Number: HG ADC or LG ADC");
    h2Proj->GetYaxis()->SetTitle("Ratio of Hits");
    h2Proj->SetStats(0);
    for(int i=0;i<NumAllChs;i++){
      cout<<"Draw the slice of gain VS channel: GID="<<i<<endl;
      TH1F *h2prohtmp = (TH1F*) h2Proj->Clone();
      h2prohtmp->SetTitle(Form("ROCID = %d, ChID = %02.f",i/32,(i%32)*1.));
      c2->cd(i+1);
      c2->cd(i+1)->SetLogy(1);
      
      h2HProj[i] = h2H->ProjectionX(Name_h2HProj[i].data(),i+1,i+1,"cutg");
      h2LProj[i] = h2L->ProjectionX(Name_h2LProj[i].data(),i+1,i+1,"cutg");
      h2HProj[i] ->SetLineColor(1);
      h2LProj[i] ->SetLineColor(2);
      h2HProj[i] = SmoothHistogramWithMovingAverage(h2HProj[i],6);
      h2LProj[i] = SmoothHistogramWithMovingAverage(h2LProj[i],6);
      
      h2prohtmp->Draw("hist");
      // h2HProj[i]->Draw("histsame");
      // h2LProj[i]->Draw("histsame");
      float TotalNum = h2HProj[i]->Integral(-1,-1);
      TotalNum     += h2LProj[i]->Integral(-1,-1);
      h2HProj[i]->Scale(1./TotalNum);
      h2LProj[i]->Scale(1./TotalNum);
      h2HProj[i]->Draw("histsame");
      h2LProj[i]->Draw("histsame");
      float maxVal = h2HProj[i]->GetMaximum();
      h2prohtmp->GetYaxis()->SetRangeUser(1./TotalNum,maxVal*3);
      fileOpt->WriteObject(h2HProj[i],h2HProj[i]->GetName());
      fileOpt->WriteObject(h2LProj[i],h2LProj[i]->GetName());
    }
    fileOpt->WriteObject(c2,c2->GetName());
    c2->Print(Form("%sChannelsVSHLADC.pdf",dirGraph.data()));
    c2->Print(Form("%sChannelsVSHLADC.png",dirGraph.data()));
    c2->Print(Form("%sChannelsVSHLADC.gif",dirGraph.data()));
    // fileOpt->Close();
    c2->Close();
  }else{
    cout<<"Load all ADC data by previously save data"<<endl;
    h2L = (TH2F*)     fileOpt->Get("h2L");
    h2H = (TH2F*)     fileOpt->Get("h2H");
    // c2  = (TCanvas*)  fileOpt->Get("c2");
    h2Proj = (TH1F*)  fileOpt->Get("c2");
    for(int i=0;i<NumAllChs;i++) h2HProj[i] = (TH1D*) fileOpt->Get(Name_h2HProj[i].data());
    for(int i=0;i<NumAllChs;i++) h2LProj[i] = (TH1D*) fileOpt->Get(Name_h2LProj[i].data());
    cout<<"Finish Loading"<<endl;

  }
  

/*
  for(int ich=0;ich<NumAllChs;ich++){
    c2->cd(ich+1)->Divide(1,2); 
    h2LDADC[ich] = new TH1D(Form("h2LDADCGID%02d",ich),"",5500,0,11000);
    h2HDADC[ich] = new TH1D(Form("h2HDADCGID%02d",ich),"", 550,0,11000);
    h2LDADC[ich]->SetTitle(Form("Channel GID:%02d VS LG-ADC / #DeltaADC",ich));
    h2HDADC[ich]->SetTitle(Form("Channel GID:%02d VS HG-ADC / #DeltaADC",ich));
    
    h2HDADC[ich]->SetLineColor(1);
    h2LDADC[ich]->SetLineColor(2);
    
    h2HDADC[ich]->GetXaxis()->SetTitle("ADC Number: HG ADC or LG ADC");
    h2HDADC[ich]->GetYaxis()->SetTitle("# of Hits");
    h2HDADC[ich]->SetStats(0);
    h2HDADC[ich]->GetXaxis()->SetTitleOffset(1.4);
    h2HDADC[ich]->GetYaxis()->SetTitleOffset(1.4);
    
    h2LDADC[ich]->GetXaxis()->SetTitle("ADC Number: HG ADC or LG ADC");
    h2LDADC[ich]->GetYaxis()->SetTitle("# of Hits");
    h2LDADC[ich]->SetStats(0);
    h2LDADC[ich]->GetXaxis()->SetTitleOffset(1.4);
    h2LDADC[ich]->GetYaxis()->SetTitleOffset(1.4);
    for(int ibin=1 ; ibin<550 ; ibin++){
      h2LDADC[ich]->SetBinContent(
        ibin,h2LProj[ich]->GetBinContent(ibin+1)-h2LProj[ich]->GetBinContent(ibin));
    }
    for(int ibin=1 ; ibin<5500 ; ibin++){
      h2HDADC[ich]->SetBinContent(
        ibin,h2HProj[ich]->GetBinContent(ibin+1)-h2HProj[ich]->GetBinContent(ibin));
    }
    h2LDADC[ich]->GetYaxis()->SetRangeUser(-0.002,0.002);
    h2HDADC[ich]->GetYaxis()->SetRangeUser(-0.002,0.002);
    c2->cd(ich+1)->cd(1)->SetLogy(0); 
    h2LDADC[ich]->Draw("hist");
    c2->cd(ich+1)->cd(2)->SetLogy(0); 
    h2HDADC[ich]->Draw("hist");
  }
  c2->Print(Form("%sChVSHLADCDADC.pdf",dirGraph.data()));
  c2->Print(Form("%sChVSHLADCDADC.png",dirGraph.data()));
  c2->Print(Form("%sChVSHLADCDADC.gif",dirGraph.data()));
  */
  TFile *fileReOpt = TFile::Open(OutputReRootFileName.data(),"recreate");

  ofstream out(OutputResultFileName.data());
  out<<"#GChID\tROCID\tChID\tBinShift\tHG/LG\tErrR"<<endl;

  TF1 *fitFunc = new TF1("fitFunc", "[0]*exp(-0.5*((x-[1])/[2])**2)*((x-[1])<0)+[0]*((x-[1])>=0)");
  fitFunc->SetParameter(0, 0.001);
  fitFunc->SetParameter(1, 800);
  fitFunc->SetParameter(2, 300);
  // 設定初始參數估計值
   // 可調整振幅初始值
  // TH1D *Htmp = new TH1D("Hfitgood","Hfitgood",80,100,900);
  // TH1D *HtmpN = new TH1D("HtmpNgood","HtmpNgood",80,100,900);
  

  for(int iN=0;iN<=0;iN++){
    gStyle->SetPadRightMargin(.1);
    c2 = EasyCanvas("c2",1600*4*2,1600*4*2,8,8,"false");
    int LGBin1st       [NumAllChs] = {0};
    int HGBinlast      [NumAllChs] = {0};
    double LGBinStart [NumAllChs] = {0};
    double HGLastVal[NumAllChs] = {0}, RLGDHG[NumAllChs] = {0};
    TH1D *hGADC[NumAllChs],*hHADC[NumAllChs],*hLADC[NumAllChs];

    for(int ich=0;ich<NumAllChs;ich++){
      cout<<"Find R for Gch==: "<<ich<<endl;
      
      // Htmp->Reset();
      // HtmpN->Reset();
      // vector<double> vX,vY;
      c2->cd(ich+1);
      // for(int i=100;i<900;i+=5){
        // TF1 *g0 = new TF1("g0", "fitFunc", 0, i);
        // g0->SetParameter(1, i);
        // h2LProj[ich]->Fit("g0", "Rq");
        // g0->SetLineColor(i+20); 
        // double val = (double)g0->GetChisquare()/(double)g0->GetNDF();
        // if(!isnan(val)) Htmp->Fill(g0->GetParameter(1),val*1.);
        // if(!isnan(val)) HtmpN->Fill(g0->GetParameter(1),1);
        // g0->Draw("lsame");
      // }
      // c2->cd(ich+2);
      // TCanvas *c23 = new TCanvas("c23","c23",640,480);
      // c23->cd();
      // Htmp->Draw("h");
      // HtmpN->Scale(2);
      // Htmp->Divide(HtmpN);
      // c2->Print("/data4/YuSiang/LYSOReadOut/Datas/20231217/160933.522_VFEC_HV56/aedrghd.gif");
      // c23->Print("/data4/YuSiang/LYSOReadOut/Datas/20231217/160933.522_VFEC_HV56/awrghajt.gif");
      // int ipos = findMinNonZeroBin(Htmp);
      // cout<<ipos<<"\t "<<Htmp->GetBinCenter(ipos)<<endl;
      // cout<<"LG start\t "<<LGBinStart[ich]<<endl;
      // throw;
      
      
      for(int ibin=820;ibin>0;ibin--){
        if( h2HProj[ich]->GetBinContent(ibin)>0){
          HGBinlast[ich] = ibin;
          HGLastVal[ich] = h2HProj[ich]->GetBinContent(ibin);
          break;
        }
      }
      if(h2LProj[ich]->Integral(-1,-1)*99<h2HProj[ich]->Integral(-1,-1)) RLGDHG[ich] = -1;
      for(int ibin=1;ibin<=11000;ibin++){
        if( h2LProj[ich]->GetBinContent(ibin)>0) LGBin1st[ich] = ibin;
        if( h2LProj[ich]->GetBinContent(ibin)>HGLastVal[ich]*0.7 ){
          LGBinStart[ich] = ibin+iN;
          break;
        }
      }
      
      // LGBinStart[ich] = h2LProj[ich]->FindBin(Htmp->GetBinCenter(ipos))+iN;
      // cout<<266<<endl;
      float StartBinVal = h2LProj[ich]->GetXaxis()->GetBinCenter(LGBinStart[ich]);
      float LastBinVal = h2LProj[ich]->GetXaxis()->GetBinCenter(LGBinStart[ich]-1);
      float NextBinVal = h2LProj[ich]->GetXaxis()->GetBinCenter(LGBinStart[ich]+1);
      RLGDHG[ich] = RLGDHG[ich]==-1 ? -1 : StartBinVal/8192.;
      double LastRLGDHG = RLGDHG[ich]==-1 ? 0 : LastBinVal/8192.;
      double NextRLGDHG = RLGDHG[ich]==-1 ? 0 : NextBinVal/8192.;
      // double Err1 = abs(LastRLGDHG-RLGDHG[ich]);
      // double Err1 = abs(LastRLGDHG-RLGDHG[ich]);

      
      vector<double> vBins;
      for(int ibin=0; ibin<=819; ibin++) vBins.push_back(ibin*10);
      vBins.push_back(8192);
      float tmpf32;
      for(int ibin=LGBinStart[ich]; ibin<=11000&&RLGDHG[ich]>0; ibin++){
        tmpf32 = h2LProj[ich]->GetXaxis()->GetBinCenter(ibin+1);
        vBins.push_back(tmpf32/RLGDHG[ich]);
      }
      // if(ich==48){
        // for(int ibin=0; ibin<int(vBins.size()); ibin++)
          // cout<<Form("%06.0f",vBins[ibin])<<" ";cout<<endl;
        // throw;
      // }
      cout<<"In Ch GID = "<<ich<<"\t";
      double InVRLGDHG = RLGDHG[ich]<0 ? -1 : 1./RLGDHG[ich];
      double InvLastRLGDHG = LastRLGDHG<=0 ? 0 : 1./LastRLGDHG;
      double InvNextRLGDHG = NextRLGDHG<=0 ? 0 : 1./NextRLGDHG;
      double ErrRLGDHG  = RLGDHG[ich]<0? 0 : sqrt(abs((InvLastRLGDHG-InVRLGDHG)*(InvNextRLGDHG-InVRLGDHG)));
      out<<Form("%d\t%d\t%d\t%d\t%f\t%f",ich,ich/NumChs,ich%NumChs,iN,InVRLGDHG,ErrRLGDHG)<<endl;
      cout<<InvLastRLGDHG<<"\t"<<InVRLGDHG<<"\t"<<InvNextRLGDHG<<endl;
      cout<<ErrRLGDHG<<"\n";

      // throw;
      cout<<"R(GL/GH) = "<<InVRLGDHG<<endl;
      hGADC[ich] 
        = new TH1D(Form("hGADC-GID%02.f",ich*1.),"",vBins.size()-1,vBins.data());
        
      hGADC[ich]->GetYaxis()->SetTitle("Hit Ratio");
      if(InVRLGDHG!=-1) 
        hGADC[ich]->SetTitle(Form("Generally ADC count(HG&LGx%2.2f)",InVRLGDHG));
      else 
        hGADC[ich]->SetTitle(Form("Generally ADC count(HG)"));
      // hGADC[ich]->GetXaxis()->SetTitleOffset(1.5);
      hGADC[ich]->GetYaxis()->SetTitleOffset(1.5);
      hGADC[ich]->GetXaxis()->SetTitleSize(hGADC[ich]->GetXaxis()->GetTitleSize()*1.5);
      hGADC[ich]->GetYaxis()->SetTitleSize(hGADC[ich]->GetYaxis()->GetTitleSize()*1.5);
      hGADC[ich]->GetXaxis()->CenterTitle(1);
      hGADC[ich]->GetYaxis()->CenterTitle(1);
      hGADC[ich]->GetXaxis()->SetLabelSize(0.02);
      hGADC[ich]->GetXaxis()->SetRangeUser(0,110000);
      // hGADC[ich]->GetYaxis()->SetRangeUser(0,5.0E-4);
      hGADC[ich]->GetXaxis()->SetNdivisions(606);
      hGADC[ich]->GetXaxis()->SetLabelSize(hGADC[ich]->GetXaxis()->GetLabelSize()*1.5);
      
      hLADC[ich] = (TH1D*) hGADC[ich]->Clone();
      hHADC[ich] = (TH1D*) hGADC[ich]->Clone();
      hGADC[ich]->SetLineColor(1);
      hLADC[ich]->SetName(Form("hLADC-GID%02.f",ich*1.));
      hHADC[ich]->SetName(Form("hHADC-GID%02.f",ich*1.));
      hGADC[ich]->SetTitle(Form("Hit Ratio VS ADC @GChID=%02.f",ich*1.));
      hHADC[ich]->SetTitle(Form("Hit Ratio VS HG ADC @GChID=%02.f",ich*1.));
      hLADC[ich]->SetTitle(Form("Hit Ratio VS LG ADC @GChID=%02.f",ich*1.));
      if(InVRLGDHG!=-1){
        hGADC[ich]->GetXaxis()->SetTitle(Form("ADC count(HG&LGx%2.2f)",InVRLGDHG));
        hLADC[ich]->GetXaxis()->SetTitle(Form("ADC count(LGx%2.2f)",InVRLGDHG));
      }else{
        hGADC[ich]->GetXaxis()->SetTitle(Form("ADC count(HG)"));
        hHADC[ich]->GetXaxis()->SetTitle("ADC count(HG)");
      }
      hHADC[ich]->SetLineColor(2);
      hLADC[ich]->SetLineColor(3);
      hGADC[ich]->SetMarkerColor(1);
      hHADC[ich]->SetMarkerColor(2);
      hLADC[ich]->SetMarkerColor(3);
      cout<<"Establish the TH1D-hGADC"<<endl;
      // c2->cd(ich+1)->Divide(1,3);
      float tmpx, tmpy;
      for(int ibin=1;ibin<=819;ibin++) {
        tmpx = ibin*10;
        tmpy = h2HProj[ich]->GetBinContent(ibin)/10.;
        hGADC[ich] ->Fill(tmpx,tmpy);
        hLADC[ich] ->Fill(tmpx,tmpy);
      }
      for(int ibin=LGBin1st[ich];ibin<=11000;ibin++) {
        tmpx = ibin/RLGDHG[ich];
        tmpy = h2LProj[ich]->GetBinContent(ibin)*RLGDHG[ich];
        hGADC[ich] ->Fill(tmpx,tmpy);
        hHADC[ich] ->Fill(tmpx,tmpy);
      }
      cout<<"printing"<<endl;
      c2->cd(ich+1)->SetLogy(0);
      // c2->cd(ich+1)->cd(1)->SetLogy(0);
      hGADC[ich]->Draw("l"); 
      // c2->cd(ich+1)->cd(2)->SetLogy(0);
      hLADC[ich]->Draw("lsame");
      // c2->cd(ich+1)->cd(3)->SetLogy(0);
      hHADC[ich]->Draw("lsame");
      hGADC[ich]->Draw("lsame"); 
      
      TLegend *L = new TLegend(0.78,0.575,0.98,0.775);
      L->AddEntry(hGADC[ich],"both ADC","l");
      L->AddEntry(hLADC[ich],"L-G  ADC","l");
      L->AddEntry(hHADC[ich],"H-G  ADC","l");
      
      L->Draw("same");
      // throw;
      fileReOpt->WriteObject(hGADC[ich],hGADC[ich]->GetName());
    }
    c2->Print(Form("%sChVSGADC_N=%d.gif",dirGraph.data(),iN));
    c2->Print(Form("%sChVSGADC_N=%d.pdf",dirGraph.data(),iN));
    c2->Print(Form("%sChVSGADC_N=%d.png",dirGraph.data(),iN));
  }
  
  
  fileReOpt->Close();
  out.close();
}