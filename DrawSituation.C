#include "/data8/ZDC/EMCal/APDLYSO/AnaCode/DrawTriggerLinesCenter.C"  
// const int NFiles = 16;
// string StepTitle = "Run000-Run015 HV205 x60";
// string Step = "Step1";
// string files[NFiles] = {
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run000_HV205_VF_268_308__x60_CosmicRay_194726.362LYSO/Run000_HV205_VF_268_308__x60_CosmicRay_194726.362LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run001_HV205_VF_268_308__x60_CosmicRay_214552.845LYSO/Run001_HV205_VF_268_308__x60_CosmicRay_214552.845LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run002_HV205_VF_268_308__x60_CosmicRay_225236.820LYSO/Run002_HV205_VF_268_308__x60_CosmicRay_225236.820LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run003_HV205_VF_268_308__x60_CosmicRay_000025.339LYSO/Run003_HV205_VF_268_308__x60_CosmicRay_000025.339LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run004_HV205_VF_268_308__x60_CosmicRay_010810.022LYSO/Run004_HV205_VF_268_308__x60_CosmicRay_010810.022LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run005_HV205_VF_268_308__x60_CosmicRay_021558.733LYSO/Run005_HV205_VF_268_308__x60_CosmicRay_021558.733LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run006_HV205_VF_268_308__x60_CosmicRay_032348.918LYSO/Run006_HV205_VF_268_308__x60_CosmicRay_032348.918LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run007_HV205_VF_268_308__x60_CosmicRay_043139.161LYSO/Run007_HV205_VF_268_308__x60_CosmicRay_043139.161LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run008_HV205_VF_268_308__x60_CosmicRay_053931.907LYSO/Run008_HV205_VF_268_308__x60_CosmicRay_053931.907LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run009_HV205_VF_268_308__x60_CosmicRay_064723.953LYSO/Run009_HV205_VF_268_308__x60_CosmicRay_064723.953LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run010_HV205_VF_268_308__x60_CosmicRay_075511.893LYSO/Run010_HV205_VF_268_308__x60_CosmicRay_075511.893LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run011_HV205_VF_268_308__x60_CosmicRay_090306.383LYSO/Run011_HV205_VF_268_308__x60_CosmicRay_090306.383LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run012_HV205_VF_268_308__x60_CosmicRay_101058.259LYSO/Run012_HV205_VF_268_308__x60_CosmicRay_101058.259LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run013_HV205_VF_268_308__x60_CosmicRay_111821.741LYSO/Run013_HV205_VF_268_308__x60_CosmicRay_111821.741LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run014_HV205_VF_268_308__x60_CosmicRay_122347.431LYSO/Run014_HV205_VF_268_308__x60_CosmicRay_122347.431LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/Run015_HV205_VF_268_308__x60_CosmicRay_132908.661LYSO/Run015_HV205_VF_268_308__x60_CosmicRay_132908.661LYSO_ReCon"
// };
const int NFiles = 1;
string StepTitle = "Run079 HV210 x60";
string Step = "StepSwitch";
string files[NFiles] = {
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run069_HV210_VF_220_244__x10_CosmicRay_000022.081LYSO/Run069_HV210_VF_220_244__x10_CosmicRay_000022.081LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run070_HV210_VF_220_244__x10_CosmicRay_010519.876LYSO/Run070_HV210_VF_220_244__x10_CosmicRay_010519.876LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run071_HV210_VF_220_244__x10_CosmicRay_020601.891LYSO/Run071_HV210_VF_220_244__x10_CosmicRay_020601.891LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run072_HV210_VF_220_244__x10_CosmicRay_030641.331LYSO/Run072_HV210_VF_220_244__x10_CosmicRay_030641.331LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run073_HV210_VF_220_244__x10_CosmicRay_040723.041LYSO/Run073_HV210_VF_220_244__x10_CosmicRay_040723.041LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run074_HV210_VF_220_244__x10_CosmicRay_050803.903LYSO/Run074_HV210_VF_220_244__x10_CosmicRay_050803.903LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run075_HV210_VF_220_244__x10_CosmicRay_060844.124LYSO/Run075_HV210_VF_220_244__x10_CosmicRay_060844.124LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run076_HV210_VF_240_270__x60_CosmicRay_152955.665LYSO/Run076_HV210_VF_240_270__x60_CosmicRay_152955.665LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run077_HV210_VF_300_330__x60_CosmicRay_165253.372LYSO/Run077_HV210_VF_300_330__x60_CosmicRay_165253.372LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run078_HV210_VF_300_328__x60_181725.565LYSO/Run078_HV210_VF_300_328__x60_181725.565LYSO_ReCon",
"/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run079_HV210_VF_270_270__x60_190137.651LYSO/Run079_HV210_VF_270_270__x60_190137.651LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080_HV210_VF_270_270__x60_200228.595LYSO/Run080_HV210_VF_270_270__x60_200228.595LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run084_HV210_VF_270_270__x60_000805.323LYSO/Run084_HV210_VF_270_270__x60_000805.323LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run085_HV210_VF_270_270__x60_011017.280LYSO/Run085_HV210_VF_270_270__x60_011017.280LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run086_HV210_VF_270_270__x60_021236.935LYSO/Run086_HV210_VF_270_270__x60_021236.935LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run087_HV210_VF_270_270__x60_031451.490LYSO/Run087_HV210_VF_270_270__x60_031451.490LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run088_HV210_VF_270_270__x60_041707.120LYSO/Run088_HV210_VF_270_270__x60_041707.120LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run089_HV210_VF_270_270__x60_051922.058LYSO/Run089_HV210_VF_270_270__x60_051922.058LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run090_HV210_VF_270_270__x60_062136.207LYSO/Run090_HV210_VF_270_270__x60_062136.207LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run091_HV210_VF_270_270__x60_072350.707LYSO/Run091_HV210_VF_270_270__x60_072350.707LYSO_ReCon"
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run092_HV210_VF_270_270__x60_082608.584LYSO/Run092_HV210_VF_270_270__x60_082608.584LYSO_ReCon"
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run092_HV210_VF_270_270__x60_082608/Run092_HV210_VF_270_270__x60_082608_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run093_HV210_VF_270_270__x60_092821.648LYSO/Run093_HV210_VF_270_270__x60_092821.648LYSO_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run093_HV210_VF_270_270__x60_092821/Run093_HV210_VF_270_270__x60_092821_ReCon",
// "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run094_HV210_VF_270_270__x60_103041.578LYSO/Run094_HV210_VF_270_270__x60_103041.578LYSO_ReCon"
};
// const int NFiles = 8;
// string StepTitle = "Run016-Run023 HV210 x60";
// string Step = "Step2";
// string files[NFiles] = {
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run016_HV210_VF_220_245__x60_CosmicRay_161415.200LYSO/Run016_HV210_VF_220_245__x60_CosmicRay_161415.200LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run017_HV210_VF_220_245__x60_CosmicRay_171416.783LYSO/Run017_HV210_VF_220_245__x60_CosmicRay_171416.783LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run018_HV210_VF_220_245__x60_CosmicRay_181433.535LYSO/Run018_HV210_VF_220_245__x60_CosmicRay_181433.535LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run019_HV210_VF_220_245__x60_CosmicRay_191437.814LYSO/Run019_HV210_VF_220_245__x60_CosmicRay_191437.814LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run020_HV210_VF_220_245__x60_CosmicRay_201439.896LYSO/Run020_HV210_VF_220_245__x60_CosmicRay_201439.896LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run021_HV210_VF_220_245__x60_CosmicRay_211441.736LYSO/Run021_HV210_VF_220_245__x60_CosmicRay_211441.736LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run022_HV210_VF_220_245__x60_CosmicRay_221443.960LYSO/Run022_HV210_VF_220_245__x60_CosmicRay_221443.960LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run023_HV210_VF_220_245__x60_CosmicRay_231446.418LYSO/Run023_HV210_VF_220_245__x60_CosmicRay_231446.418LYSO_ReCon"
// };

// const int NFiles = 14;
// string StepTitle = "Run045-Run058 HV210 x10";
// string Step = "Step3";
// string files[NFiles] = {
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run045_HV210_VF_220_245__x10_CosmicRay_003931.571LYSO/Run045_HV210_VF_220_245__x10_CosmicRay_003931.571LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run046_HV210_VF_220_245__x10_CosmicRay_014710.461LYSO/Run046_HV210_VF_220_245__x10_CosmicRay_014710.461LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run047_HV210_VF_220_245__x10_CosmicRay_025431.298LYSO/Run047_HV210_VF_220_245__x10_CosmicRay_025431.298LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run048_HV210_VF_220_245__x10_CosmicRay_040218.011LYSO/Run048_HV210_VF_220_245__x10_CosmicRay_040218.011LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run049_HV210_VF_220_245__x10_CosmicRay_051019.578LYSO/Run049_HV210_VF_220_245__x10_CosmicRay_051019.578LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run050_HV210_VF_220_245__x10_CosmicRay_061757.583LYSO/Run050_HV210_VF_220_245__x10_CosmicRay_061757.583LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run051_HV210_VF_220_245__x10_CosmicRay_072515.811LYSO/Run051_HV210_VF_220_245__x10_CosmicRay_072515.811LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run052_HV210_VF_220_245__x10_CosmicRay_083246.712LYSO/Run052_HV210_VF_220_245__x10_CosmicRay_083246.712LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run053_HV210_VF_220_245__x10_CosmicRay_093904.727LYSO/Run053_HV210_VF_220_245__x10_CosmicRay_093904.727LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run054_HV210_VF_220_245__x10_CosmicRay_104530.890LYSO/Run054_HV210_VF_220_245__x10_CosmicRay_104530.890LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run055_HV210_VF_220_245__x10_CosmicRay_115201.457LYSO/Run055_HV210_VF_220_245__x10_CosmicRay_115201.457LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run056_HV210_VF_220_245__x10_CosmicRay_125849.887LYSO/Run056_HV210_VF_220_245__x10_CosmicRay_125849.887LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run057_HV210_VF_220_245__x10_CosmicRay_140531.687LYSO/Run057_HV210_VF_220_245__x10_CosmicRay_140531.687LYSO_ReCon",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/Run058_HV210_VF_220_245__x10_CosmicRay_151310.434LYSO/Run058_HV210_VF_220_245__x10_CosmicRay_151310.434LYSO_ReCon"
// };
void DrawSituation(string STR,string CDT,string Titles,string LABEL){
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  TChain *t= new TChain("t");
  TChain *tMD4= new TChain("tMD4");
  for (const string file : files)  t->Add(Form("%s%s.root",file.data(),STR.data())); 
  TCanvas *c = new TCanvas("c","c",640*3+4,640*2+28);
  c->Divide(3,2);
  c->cd(1);
  TH1D *hNH0 = new TH1D("hNH0","# of Hits; # of Hits; count",30,0,30);
  TH1D *hNH1 = new TH1D("hNH1","# Trigger 2""# Hits",30,0,30);
  TH1D *hNH2 = new TH1D("hNH2","# LYSO ZDC""# Hits",30,0,30);
  TH1D *hNH3 = new TH1D("hNH3","# PbWO4 ZDC""# Hits",30,0,30);
  t->Draw("nHits0>>hNH0","DID==2","h");
  t->Draw("nHits1>>hNH1","DID==3","hsame");
  t->Draw("nHits2>>hNH2","DID==1","hsame");
  t->Draw("nHits3>>hNH3","DID==4","hsame");

  hNH0->SetStats(0);
  hNH1->SetStats(0);
  hNH2->SetStats(0);
  hNH3->SetStats(0);
  hNH0->SetLineColor(1);
  hNH1->SetLineColor(2);
  hNH2->SetLineColor(4);
  hNH3->SetLineColor(6);
  hNH0->SetLineWidth(2);
  hNH1->SetLineWidth(2);
  hNH2->SetLineWidth(2);
  hNH3->SetLineWidth(2);
  double maxNHY = hNH0->GetMaximum();
  if(hNH1->GetMaximum()>maxNHY) maxNHY = hNH1->GetMaximum();
  if(hNH2->GetMaximum()>maxNHY) maxNHY = hNH2->GetMaximum();
  if(hNH3->GetMaximum()>maxNHY) maxNHY = hNH3->GetMaximum();
  hNH0->GetYaxis()->SetRangeUser(0,maxNHY*1.2);
  c->cd(2);
  TH1D *hADC0 = new TH1D("hADC0","ADC of Hits; ADC; count",80,-500,11000);
  TH1D *hADC1 = new TH1D("hADC1","ADC of Hits; ADC; count",80,-500,11000);
  TH1D *hADC2 = new TH1D("hADC2","ADC of Hits; ADC; count",40,-500,11000);
  TH1D *hADC3 = new TH1D("hADC3","ADC of Hits; ADC; count",40,-500,11000);
  t->Draw("GADC+1000>>hADC0",Form("DID==2%s",CDT.data()),"h");
  t->Draw("GADC+1000>>hADC1",Form("DID==3%s",CDT.data()),"hsame");
  t->Draw("GADC>>hADC2",Form("DID==1%s",CDT.data()),"hsame");
  t->Draw("GADC>>hADC3",Form("DID==4%s",CDT.data()),"hsame");
  hADC0->SetStats(0);
  hADC1->SetStats(0);
  hADC2->SetStats(0);
  hADC3->SetStats(0);
  hADC0->SetLineColor(1);
  hADC1->SetLineColor(2);
  hADC2->SetLineColor(4);
  hADC3->SetLineColor(6);
  hADC0->SetLineWidth(2);
  hADC1->SetLineWidth(2);
  hADC2->SetLineWidth(2);
  hADC3->SetLineWidth(2);
  double maxADCY = hADC0->GetMaximum();
  if(hADC1->GetMaximum()>maxADCY) maxADCY = hADC1->GetMaximum();
  if(hADC2->GetMaximum()>maxADCY) maxADCY = hADC2->GetMaximum();
  if(hADC3->GetMaximum()>maxADCY) maxADCY = hADC3->GetMaximum();
  hADC0->GetYaxis()->SetRangeUser(0,maxADCY*1.2);
  cout<<maxADCY*1.2<<endl;
  c->cd(3);
  TLegend *l1 = new TLegend(0.1,0.1,0.9,0.9);
  l1->AddEntry(hNH1,"Trigger 2(Z=+144.6mm)","l");
  l1->AddEntry(hNH0,"Trigger 1(Z=+109.6mm)","l");
  l1->AddEntry(hNH2,"LYSO ZDC (Z=-7.51mm)","l");
  l1->AddEntry(hNH3,"PbWO4 ZDC(Z=-137.4mm)","l");
  l1->Draw();
  
  c->cd(4);
  TH1D *hEventGap = new TH1D("hEventGap","Event window; window size(#mus); count",70,0,1);
  // TH1D *hADC1 = new TH1D("hADC1","ADC of Hits; ADC; count",70,-500,3000);
  // TH1D *hADC2 = new TH1D("hADC2","ADC of Hits; ADC; count",70,-500,3000);
  // t->Draw("dfcnt*1.E+6>>hEventGap");
  // hADC0->SetStats(0);
  // hADC1->SetStats(0);
  // hADC2->SetStats(0);
  // hEventGap->SetLineColor(1);
  // hADC1->SetLineColor(2);
  // hADC2->SetLineColor(4);
  // hEventGap->SetLineWidth(2);
  // hADC1->SetLineWidth(2);
  // hADC2->SetLineWidth(2);
  c->cd(5);
  TChain *ts[NFiles];
  TH1D *hDateSta0 = new TH1D("hDateSta0","File event;Run number;Reconstruct event",NFiles,0,NFiles);
  TH1D *hDateSta1 = new TH1D("hDateSta1","File event;Run number;Reconstruct event",NFiles,0,NFiles);
  TH1D *hDateSta2 = new TH1D("hDateSta2","File event;Run number;Reconstruct event",NFiles,0,NFiles);
  TH1D *hDateSta3 = new TH1D("hDateSta3","File event;Run number;Reconstruct event",NFiles,0,NFiles);
  for (int i = 0; i<NFiles;i++){
    ts[i] = new TChain("t");
    ts[i]->Add(Form("%s%s.root",files[i].data(),STR.data())); 

    string FN = files[i];
    string Label = FN.substr(FN.find("Run")+4,FN.find("_")-FN.find("Run")-4);
    hDateSta0->SetBinContent(i+1,ts[i]->GetEntries("nHits0"));
    hDateSta1->SetBinContent(i+1,ts[i]->GetEntries("nHits1"));
    hDateSta2->SetBinContent(i+1,ts[i]->GetEntries("nHits2"));
    hDateSta3->SetBinContent(i+1,ts[i]->GetEntries("nHits3"));
    if(i==0||i==NFiles-1||i%5==0) hDateSta0->GetXaxis()->SetBinLabel(i+1,Label.data());
  }
  hDateSta0->SetLineColor(1);
  hDateSta1->SetLineColor(2);
  hDateSta2->SetLineColor(4);
  hDateSta3->SetLineColor(6);
  hDateSta0->SetLineWidth(2);
  hDateSta1->SetLineWidth(2);
  hDateSta2->SetLineWidth(2);
  hDateSta3->SetLineWidth(2);
  hDateSta0->SetStats(0);
  hDateSta0->Draw("he");
  hDateSta1->Draw("hesame");
  hDateSta2->Draw("hesame");
  hDateSta3->Draw("hesame");
  hDateSta0->GetYaxis()->SetRangeUser(0,hDateSta0->GetMaximum()*1.2);

  
  c->cd(4);
  TH1D *hADCMax2 = new TH1D("hADCMax2","ADCmax; ADC; count",40,-500,11000);
  TH1D *hADCMax3 = new TH1D("hADCMax3","ADCmax; ADC; count",40,-500,11000);
  t->Draw("GADC[nHits0+nHits1]>>hADCMax2","(nHits2>1)","h");
  t->Draw("GADC[nHits0+nHits1+nHits2]>>hADCMax3","(nHits3>1)","hsame");
  hADCMax2->SetStats(0);
  hADCMax2->SetLineColor(4);
  hADCMax3->SetLineColor(6);
  hADCMax2->SetLineWidth(2);
  hADCMax3->SetLineWidth(2);
  // c->cd(4);
  // TLegend *lTitle = new TLegend(0.1,0.1,0.9,0.9);
  // lTitle->AddEntry(hDateSta1,Titles.data(),"T");
  // lTitle->Draw();
  c->cd(6);
  TLegend *l2 = new TLegend(0.1,0.1,0.9,0.9);
  l2->AddEntry(hDateSta1,"Pass 2 B.M.","l");
  l2->AddEntry(hDateSta2,"Hit 2 B.M. + LYSO ZDC","l");
  l2->AddEntry(hDateSta3,"Hit 2 B.M. + PWO ZDC","l");
  l2->Draw();
  
  c->Print(Form("AnaResult%s%s.gif",STR.data(),LABEL.data()));
  // gStyle->SetPadLeftMargin(0.08);
  // gStyle->SetPadRightMargin(0.12);
  TCanvas *c1 = new TCanvas("c1","c1",640+4,640+28);
  c1->cd();
  TH2D *hDIDROC = new TH2D("hDIDROC","Detector ID VS ROC ID;D ID(1:LYSO,2/3:BM-T1/T2,4:PbWO); ROCID(0=A,1=B); count",4,1,5,2,0,2);
  hDIDROC->SetStats(0);
  t->Draw("ROCID:DID>>hDIDROC ","","coltexte");
  c1->Print(Form("hDIDVSROC%s%s.gif",STR.data(),LABEL.data()));
  
  
}
void DrawSituation(){
  DrawSituation("_MDID1","&&(nHits2>0)",StepTitle.data(),Step.data());
  DrawSituation("_MDID4","&&(nHits3>0)",StepTitle.data(),Step.data());
  string FilesMDID1 ,FilesMDID4;
  for (const string file : files) FilesMDID1+=file+"_MDID1.root,"; 
  cout<<FilesMDID1<<endl;
  DrawTriggerLinesCenter(FilesMDID1,"./Graphs/");
}
