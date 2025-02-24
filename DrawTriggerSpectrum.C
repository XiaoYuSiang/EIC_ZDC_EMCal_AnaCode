void DrawTriggerSpectrum(){
  TFile *_fileT1 = TFile::Open("/data8/ZDC/EMCal/LYSO2ABMCosmic/250118/CosRay_FV430_30V_FV320_x40/T1_Sci.root");
  TFile *_fileT2 = TFile::Open("/data8/ZDC/EMCal/LYSO2ABMCosmic/250118/CosRay_FV430_30V_FV320_x40/T2_Sci.root");
  TTree *tT1 = (TTree*) _fileT1->Get("t");
  TTree *tT2 = (TTree*) _fileT2->Get("t");
  
  
  TCanvas *c1 = new TCanvas("c","c",840+4,840+28);
  c1->cd()->SetLogy(1);
  c1->cd()->SetGrid(1,0);
  TH1F *hT1R0 = new TH1F("hT1R0",";ADC;Count",87*5,-500,8200);
  TH1F *hT2R0 = new TH1F("hT2R0",";ADC;Count",87*5,-500,8200);
  TH1F *hT1R1 = new TH1F("hT1R1",";ADC;Count",87*5,-500,8200);
  TH1F *hT2R1 = new TH1F("hT2R1",";ADC;Count",87*5,-500,8200);
  hT1R0->SetLineColor(2);
  hT2R0->SetLineColor(4);
  hT1R1->SetLineColor(6);
  hT2R1->SetLineColor(8);
  hT1R0->SetLineWidth(2);
  hT2R0->SetLineWidth(2);
  hT1R1->SetLineWidth(2);
  hT2R1->SetLineWidth(2);
  tT1->Draw("ADC>>hT1R0","ROCID==0","e");
  tT2->Draw("ADC>>hT2R0","ROCID==0","esame");
  tT1->Draw("ADC>>hT1R1","ROCID==1","esame");
  tT2->Draw("ADC>>hT2R1","ROCID==1","esame");
  double max = 0;
  if(max<hT1R0->GetMaximum()) max = hT1R0->GetMaximum();
  if(max<hT2R0->GetMaximum()) max = hT2R0->GetMaximum();
  if(max<hT2R1->GetMaximum()) max = hT2R1->GetMaximum();
  if(max<hT1R1->GetMaximum()) max = hT1R1->GetMaximum();
  hT1R0->GetYaxis()->SetRangeUser(1,max*1.2);
  hT1R0->GetXaxis()->SetRangeUser(-500,3000);
  TLegend *L = new TLegend(0.7,0.7,1,1);
  L->AddEntry(hT1R0,"T. 1 ROC A","L");
  L->AddEntry(hT1R1,"T. 1 ROC B","L");
  L->AddEntry(hT2R0,"T. 2 ROC A","L");
  L->AddEntry(hT2R1,"T. 2 ROC B","L");
  L->Draw();
  
  c1->Print("/data8/ZDC/EMCal/LYSO2ABMCosmic/250118/CosRay_FV430_30V_FV320_x40/TriggerADC.gif");
}