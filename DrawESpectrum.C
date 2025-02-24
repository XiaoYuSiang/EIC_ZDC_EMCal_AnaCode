#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <iostream>
using namespace std;
void DrawESpectrum(const string FileName,const string SavePath){
  TFile *_file0 = TFile::Open(FileName.data());
  TTree *t = (TTree*) _file0->Get("t");
  TH1F *hE11 = new TH1F("hE11",";ADC(HG or 10x LG);count",20,0,110000);
  TH1F *hE33 = new TH1F("hE33",";ADC(HG or 10x LG);count",20,0,220000);
  TH1F *hE55 = new TH1F("hE55",";ADC(HG or 10x LG);count",20,0,220000);
  TCanvas *c1 = new TCanvas("c1","",600+4,600+28);
  c1->cd();
  t->Draw("GADC55>>hE55","GADC55<220000&&GADC55>=0","he");
  t->Draw("GADC33>>hE33","GADC55<220000&&GADC55>=0","ehsame");
  t->Draw("GADC11>>hE11","GADC55<220000&&GADC55>=0","hesame");
  hE11->SetLineColor(1);
  hE33->SetLineColor(2);
  hE55->SetLineColor(4);
  hE11->SetLineWidth(2);
  hE33->SetLineWidth(2);
  hE55->SetLineWidth(2);
  
  hE55->GetYaxis()->SetRangeUser(0,hE11->GetMaximum()*1.2);
  
  TLegend *L = new TLegend(0.7,0.7,1,1);
  L->AddEntry(hE11,"E_{1x1}","Le");
  L->AddEntry(hE33,"E_{3x3}","Le");
  L->AddEntry(hE55,"E_{5x5}","Le");
  L->Draw();
  c1->Print(Form("%sEspectrum.gif",SavePath.data());
}