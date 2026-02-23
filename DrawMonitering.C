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
// vector<string> NameDet = {"LYSO+SiPM60035","LYSO+APD","B.M.2","B.M.1","PbWO_{4}+SiPM"};
vector<string> NameDet = {"LYSO+SiPM60035","LYSO","B.M.1","B.M.2","PbWO"};
int pcnt, fcnt;
int MinPCNT0, MaxPCNT1;
string dirGraph = "";
void AnalyzeOutliers(TTree *t, const int iDet, int &min, int &max) {
  vector<int> vpcnt;
  for(Long64_t i=0;i<t->GetEntries();i++){
    t->GetEntry(i);
    vpcnt.push_back(pcnt);
  }
  if (vpcnt.empty()) return;
  // 使用 ROOT 的 TMath 計算平均值與標準差
  double mean = TMath::Mean(vpcnt.begin(), vpcnt.end());
  double rms  = TMath::RMS(vpcnt.begin(), vpcnt.end());
  double threshold = 5.0 * rms;
  vector<int> NewData;
  for (double x : vpcnt) {
      if (std::abs(x - mean) > threshold) {
        continue;
      }else{
        NewData.push_back(x);
      }
  }
  max = *std::max_element(NewData.begin(), NewData.end());
  min = *std::min_element(NewData.begin(), NewData.end());
}
void DrawMonitering( TTree *t, const int iDet){
  map< int, Sensor > sensorMap = LoadSensorMap(iDet);
  setTDRStyle();
  
  int pcnt0 = MinPCNT0, pcnt1 = MaxPCNT1;
  int DeltaT = pcnt1-pcnt0;
  
  TH2D *h2tX = new TH2D("h2tX",Form(";time,pcnt[s], pcnt_{0} = %d;index iX",pcnt0),DeltaT,0,DeltaT+1,NumIX,0,NumIX);
  TH2D *h2tY = new TH2D("h2tY",Form(";time,pcnt[s], pcnt_{0} = %d;index iY",pcnt0),DeltaT,0,DeltaT+1,NumIY,0,NumIY);
  h2tX->GetYaxis()->CenterTitle(1);
  h2tY->GetYaxis()->CenterTitle(1);
  TCanvas* c1 = new TCanvas("c1", "Position Resolution E-Scan", 1600+4, 900*2+28);
  c1->Divide(1,2,.001,.001);
  c1->cd(1);
  if((iDet==2||iDet==3)) t->Draw(Form("iX:pcnt-%d>>h2tX",pcnt0),"iY==0","colz");
  else                   t->Draw(Form("iX:pcnt-%d>>h2tX",pcnt0),"","colz");
  c1->cd(2);
  if((iDet==2||iDet==3)) t->Draw(Form("iY:pcnt-%d>>h2tY",pcnt0),"iX==0","colz");
  else                   t->Draw(Form("iY:pcnt-%d>>h2tY",pcnt0),"","colz");
  c1->Print(Form("%sTimeVSiXiY_D%d.gif",dirGraph.data(),iDet));
  h2tX->GetYaxis()->SetTitle("Relative Position X[mm]");
  h2tY->GetYaxis()->SetTitle("Relative Position Y[mm]");
  h2tX->SetBins(DeltaT,0,DeltaT+1,NumIX,0,NumIX* ChGapX[iDet]);
  h2tY->SetBins(DeltaT,0,DeltaT+1,NumIY,0,NumIY* ChGapY[iDet]);
  c1->Update();
  c1->Print(Form("%sTimeVSiXiY_mm_D%d.gif",dirGraph.data(),iDet));
  CleanupHistogramsAndCanvases();
}

void DrawMonitering( const string dirAnaPath ){

  // cout<<364<<endl;
  dirGraph = dirAnaPath+string("/graphRaw/");
  // cout<<366<<"  "<<dirGraph<<endl;
  system(Form("mkdir -p %s",dirGraph.data()));
  TFile *file[AnaDets.size()];
  TTree *t[AnaDets.size()];
  vector<int> pcnt0s(AnaDets.size()),pcnt1s(AnaDets.size());
  for(size_t iDet = 0;iDet<AnaDets.size();iDet++){
    string SourceFileNameEvents = dirAnaPath+SymbolFile[AnaDets[iDet]]+"_Sci.root";
    file[iDet] = (TFile*) TFile::Open(SourceFileNameEvents.data());
    t[iDet] = (TTree*) file[iDet]->Get("t");
    t[iDet]->SetBranchAddress("pcnt",&pcnt);
    t[iDet]->SetBranchAddress("fcnt",&fcnt);
    if (!t[iDet]) {
        std::cerr << "Error: TTree pointer is null." << std::endl;
        std::cerr << "  DrawMonitering.C("<<dirAnaPath<<","<<SymbolFile[AnaDets[iDet]]<<")"<< std::endl;
        return;
    }
    AnalyzeOutliers(t[iDet], iDet, pcnt0s[iDet], pcnt1s[iDet]);
  }
  MinPCNT0 = *std::max_element(pcnt0s.begin(), pcnt0s.end());
  MaxPCNT1 = *std::min_element(pcnt1s.begin(), pcnt1s.end());
  cout<<MinPCNT0<<" "<<MaxPCNT1<<endl;
  for(size_t iDet = 0;iDet<AnaDets.size();iDet++){
    DrawMonitering( t[iDet],AnaDets[iDet]);
    file[iDet]->Close();
  }
}
void DrawMonitering() {
  cout<<"Finished compiling of DrawMonitering.C+"<<endl;
  // DrawMonitering("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/");
}
