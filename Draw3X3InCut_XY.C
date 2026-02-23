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
#include <TH2.h>
#include <TH3.h>
#include <TString.h>
#include <TROOT.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "./GIDMapping.h"
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"
#include "/data4/YuSiang/personalLib/RPU/DBMLayouts.h"
#include "/data8/ZDC/EMCal/ShareScript/GrystalBallFitLibs.h"
#include "/data4/YuSiang/personalLib/EFFTool/MemoryClear.h"


string dirGraph = "";
void DrawRSD(TTree *t, int type, int DID, double &AlignX, double &AlignY){
  TH2D *hRSD2 = new TH2D("hRSD2",";#DeltaX[mm];#DeltaY[mm]",121,-50,50,121,-50,50);
  TString strTarget = "CCpdZDC@D_@w.Y()-CentP@D.Y():CCpdZDC@D_@w.X()-CentP@D.X()>>@s";
  strTarget.ReplaceAll("@D",Form("%d",DID));
  strTarget.ReplaceAll("@w",sfWtNames[type]);
  strTarget.ReplaceAll("@s",hRSD2->GetName());
  t->Draw(strTarget.Data(),Form("bGoodZDC%d",DID),"goff");
  LayoutProfile2DObj layout(hRSD2, 1080,"colz","cTFit(Gaus)","cTFit(Gaus)");
  layout.Print(Form("%sRSD_%s_D%d.gif",dirGraph.data(),sfWtNames[type].Data(),DID));
  layout.Layout2x1D(600,"hep","hep");
  layout.Print(Form("%sRSD_%s_2X1D_D%d.gif",dirGraph.data(),sfWtNames[type].Data(),DID));
  AlignX = layout.fX->GetParameter(1);
  AlignY = layout.fY->GetParameter(1);
}
const int NScans = 4;
double startRangSize = 1.0;
double xoff = 0, yoff = 0;
double Step = 2.5;
vector< vector<double> > offsetSet={{0,0},{10.25,0},{10.25,10.25}};
void Draw3X3InCut_XY( TTree *t,int DID){
  map< int, Sensor > sensorMap = LoadSensorMap(DID);
  int mainch = CrystalMainCh[DID][0];
  double pXmain = sensorMap[mainch].pX, pYmain = sensorMap[mainch].pY;
  GSPadMargins(0.150,0.130,0.03,0.050);
  gStyle->SetTitleXOffset(1.50);
  gStyle->SetTitleYOffset(1.50);
  
  double MeanX[2], MeanY[2];
  DrawRSD(t,0,4,MeanX[0], MeanY[0]);
  DrawRSD(t,1,4,MeanX[1], MeanY[1]);
  cout<<MeanX[0]<<"\t"<<MeanY[0]<<endl;
  cout<<MeanX[1]<<"\t"<<MeanY[1]<<endl;
  double muX=(MeanX[0]+MeanX[1])/2., muY=(MeanY[0]+MeanY[1])/2.;
  
  TH3D *h3 = new TH3D("h3",";iX;iY;Edep[MeV]",NumIX,0,NumIX,NumIY,0,NumIY,30,0,960);
  TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", 3*1080+4, 2*1080+28);
  TCanvas *canvas55 = new TCanvas("canvas55", "55 Slices", 1080+4, 1080+28);
  canvas55->SetGrid(1,1);
  canvas->Divide(3,2,1e-5,1e-5); // 8x8 排列
  canvas->cd(1);
  t->Draw("Edep:iY:iX>>h3",Form("DID==%d",DID),"box2z");
  h3->GetZaxis()->SetTitleOffset(1.6);
  h3->GetXaxis()->CenterTitle(1);
  h3->GetYaxis()->CenterTitle(1);
  h3->GetXaxis()->SetRangeUser(0,5);
  h3->GetYaxis()->SetRangeUser(0,5);
  // TH2D *hCentZDC2 
    // = new TH2D("hCentZDC2",";Position X[mm];Position Y[mm]",121,-50,50,121,-50,50);
  TH2F *GridZDC0   = CreateSensorGrid(sensorMap,"GridZDC0","");
  TH2F *GridZDC1   = CreateSensorGrid(sensorMap,"GridZDC1","");
  TH2F *GridZDC2   = CreateSensorGrid(sensorMap,"GridZDC2","");
  TH2F *GridZDC3   = CreateSensorGrid(sensorMap,"GridZDC3","");
  TH2F *CenterZDC = CreateSensorGrid(sensorMap,"CenterZDC","CM ZDC;X position [mm];Y position [mm]",false,51,51);
  TH2F *Centeriq0 = CreateSensorGrid(sensorMap,"Centeriq0","CM peak crystal;X position [mm];Y position [mm]",false,51,51);
  TH2F *BMPredict = CreateSensorGrid(sensorMap,"BMPredict","BM Predict on ZDC;X position [mm];Y position [mm]",false,51,51);
  TH2F *BMPreAlig = CreateSensorGrid(sensorMap,"BMPreAlig","BM Predict(offset);X position [mm];Y position [mm]",false,51,51);
  
  double ChGX = ChGapX[DID];
  double ChGY = ChGapY[DID];
  TH2F *GridZDCRing = new TH2F("GridZDCRing","",3,-ChGX*1.5,+ChGX*1.5,3,-ChGY*1.5,+ChGY*1.5);
  for(int iXtmp = 1; iXtmp<=3;iXtmp++)
    for(int iYtmp = 1; iYtmp<=3;iYtmp++)
      GridZDCRing->SetBinContent(iXtmp,iYtmp,1);
  GridZDCRing->GetZaxis()->SetRangeUser(0,1);
  GridZDCRing->SetLineColor(2);
  GridZDCRing->SetLineWidth(2);
  TH2F *CutInRing = new TH2F("CutInRing","Effective event range;PredictX(Cali.)-TargerX [mm];PredictX(Cali.)-TargerX [mm]",51,-25.5,+25.5,51,-25.5,+25.5);
  
  TGraph *g = new TGraph();
  g->SetPoint(0,pXmain+xoff,pYmain+yoff);
  g->SetPoint(1,xoff,yoff);
  // cout<<
  g->SetMarkerColor(2);
  g->SetMarkerSize(2);
  g->SetMarkerStyle(39);
  
  canvas->cd(2);
  TString strTarget = "CentP@D.Y():CentP@D.X()>>CenterZDC";
  strTarget.ReplaceAll("@D",Form("%d",DID));
  t->Draw(strTarget.Data(),Form("bGoodZDC%d",DID),"colz");
  GridZDC0->Scale(CenterZDC->GetMaximum()*1.2);GridZDC0->Draw("boxsame");
  g->Draw("psame");

  canvas->cd(3);
  TString strTarget2 = "Pos0_@D.Y():Pos0_@D.X()>>Centeriq0";
  strTarget2.ReplaceAll("@D",Form("%d",DID));
  t->Draw(strTarget2.Data(),Form("bGoodZDC%d",DID),"colz");
  GridZDC1->Scale(Centeriq0->GetMaximum()*1.2);GridZDC1->Draw("boxsame");
  g->Draw("psame");
  
  canvas->cd(4);
  TString strTarget3 = "CCpdZDC@D_@w.Y():CCpdZDC@D_@w.X()>>BMPredict";
  strTarget3.ReplaceAll("@D",Form("%d",DID));
  strTarget3.ReplaceAll("@w",sfWtNames[1]);
  t->Draw(strTarget3.Data(),Form("bGoodZDC%d",DID),"colz");
  GridZDC2->Scale(BMPredict->GetMaximum()*1.2);GridZDC2->Draw("boxsame");
  g->Draw("psame");
  
  canvas->cd(5);
  TString strTarget4 = "CCpdZDC@D_@w.Y()+Mx:CCpdZDC@D_@w.X()+My>>BMPreAlig";
  strTarget4.ReplaceAll("@D",Form("%d",DID));
  strTarget4.ReplaceAll("@w",sfWtNames[1]);
  strTarget4.ReplaceAll("Mx",Form("%f",muX));
  strTarget4.ReplaceAll("My",Form("%f",muY));
  t->Draw(strTarget4.Data(),Form("bGoodZDC%d",DID),"colz");
  GridZDC3->Scale(BMPreAlig->GetMaximum()*1.2);GridZDC3->Draw("boxsame");
  g->Draw("psame");
  
  TH3D *h3C = (TH3D*) h3->Clone("h3C");
  vector< map<int , map<int , TH1D *> > >h1s(NScans);
  canvas->cd(6);
  TString strTarget5 = "CCpdZDC@D_@w.Y()-My-MainY:CCpdZDC@D_@w.X()-Mx-MainX>>CutInRing";
  strTarget5.ReplaceAll("MainX",Form("%f",pXmain));
  strTarget5.ReplaceAll("MainY",Form("%f",pYmain));
  strTarget5.ReplaceAll("@D",Form("%d",DID));
  strTarget5.ReplaceAll("@w",sfWtNames[1]);
  strTarget5.ReplaceAll("Mx",Form("%f",muX));
  strTarget5.ReplaceAll("My",Form("%f",muY));
  t->Draw(strTarget5.Data(),Form("bGoodZDC%d",DID),"colz");
  GridZDCRing->Scale(CutInRing->GetMaximum()*1.2); GridZDCRing->Draw("boxsame");
  g->Draw("psame");

  canvas->Print(Form("%sADCVSDiXDiY_Track_WOcut.gif",dirGraph .data()));  
  
  TCanvas *cProj = new TCanvas("cProj", "Edep Slices Comparison", 1080+4, 1080+28);
  map<int , map< int , TLegend *> > leg;
  for(int iX = 0; iX<=4;iX++){
    for(int iY = 0; iY<=4;iY++){
      leg[iX][iY] = new TLegend(0.7, 0.5, .999, 0.95);
      leg[iX][iY]->SetHeader(Form("Cut: Radius < R @D%d", DID));
    }
  }
  TLegend *leg5x5 = new TLegend(0.7, 0.5, .999, 0.95);
  leg5x5->SetHeader(Form("Cut: Radius < R @D%d", DID));
  // 2. 進入循環，針對不同半徑進行切片
  vector<TH1F *>hE5x5s(NScans);
  for (int i = 0; i < NScans; i++) {
    map<int , map<int , TH1D *> > mh1s;
    double currentR = startRangSize + i*Step; // 半徑隨循環增加
    h3C->Reset(); // 每次填充前清空 3D 直方圖，確保數據對應目前的半徑
    canvas->cd(1);
    // 建立篩選條件字串 (RSDcdts)
    
    // TString RSDcdts = 
      // "bGoodZDC@D&(sqrt(pow(CCpdZDC@D_@w.Y()-My-Pos0_@D.Y()-yoffset,2)+pow(CCpdZDC@D_@w.X()-Mx-Pos0_@D.X()-xoffset,2))<@r)";
    // RSDcdts.ReplaceAll("xoffset",Form("%f",xoff));
    // RSDcdts.ReplaceAll("yoffset",Form("%f",yoff));
    // RSDcdts.ReplaceAll("Mx",Form("%f",muX));
    // RSDcdts.ReplaceAll("My",Form("%f",muY));
    // RSDcdts.ReplaceAll("@D",Form("%d",DID));
    // RSDcdts.ReplaceAll("@w",sfWtNames[1]);
    // RSDcdts.ReplaceAll("@r",Form("%f",currentR));
    
    TString RSDcdts = 
      "bGoodZDC@D&(sqrt(pow(CCpdZDC@D_@w.Y()-My-MainY-yoffset,2)+pow(CCpdZDC@D_@w.X()-Mx-MainX-xoffset,2))<@r)";
    RSDcdts.ReplaceAll("xoffset",Form("%f",xoff));
    RSDcdts.ReplaceAll("yoffset",Form("%f",yoff));
    RSDcdts.ReplaceAll("MainX",Form("%f",pXmain));
    RSDcdts.ReplaceAll("MainY",Form("%f",pYmain));
    RSDcdts.ReplaceAll("Mx",Form("%f",muX));
    RSDcdts.ReplaceAll("My",Form("%f",muY));
    RSDcdts.ReplaceAll("@D",Form("%d",DID));
    RSDcdts.ReplaceAll("@w",sfWtNames[1]);
    RSDcdts.ReplaceAll("@r",Form("%f",currentR));
    // cout<<RSDcdts<<endl;
    // 填充 3D 直方圖 (goff 代表不即時畫出)
    t->Draw("Edep:iY:iX>>h3C",RSDcdts.Data(),"box2z");
    h3C->GetZaxis()->SetTitleOffset(1.6);
    h3C->GetXaxis()->CenterTitle(1);
    h3C->GetYaxis()->CenterTitle(1);
    h3C->GetXaxis()->SetRangeUser(0,5);
    h3C->GetYaxis()->SetRangeUser(0,5);
    cout<<RSDcdts<<endl;
    
    canvas->cd(2);
    t->Draw(strTarget.Data(),RSDcdts.Data(),"colz");
    GridZDC0->Scale(CenterZDC->GetMaximum()*1.2);GridZDC0->Draw("boxsame");
    g->Draw("psame");
    canvas->cd(3);
    t->Draw(strTarget2.Data(),RSDcdts.Data(),"colz");
    GridZDC1->Scale(Centeriq0->GetMaximum()*1.2);GridZDC1->Draw("boxsame");
    g->Draw("psame");
    canvas->cd(4);
    t->Draw(strTarget3.Data(),RSDcdts.Data(),"colz");
    GridZDC2->Scale(BMPredict->GetMaximum()*1.2);GridZDC2->Draw("boxsame");
    g->Draw("psame");

    canvas->cd(5);
    t->Draw(strTarget4.Data(),RSDcdts.Data(),"colz");
    GridZDC3->Scale(BMPreAlig->GetMaximum()*1.2);GridZDC3->Draw("boxsame");
    g->Draw("psame");
    canvas->cd(6);
    t->Draw(strTarget5.Data(),RSDcdts.Data(),"colz");
    GridZDCRing->GetZaxis()->SetRangeUser(0,1); GridZDCRing->Draw("boxsame");
    g->Draw("psame");
    canvas->Print(Form("%sADCVS_iXiY_Track_WIcutRin%02.0f.gif",dirGraph .data(),currentR));
    canvas55->cd();
    hE5x5s[i] = new TH1F(Form("hE55%d",i),";E_{5x5}[MeV];count ratio, max = 1",30,0,960);
    t->Draw(Form("E5x5_%d>>%s",DID,hE5x5s[i]->GetName()),RSDcdts.Data(),"goff");
    leg5x5->AddEntry(hE5x5s[i], Form("R < %.1f mm, #mu=%.1f", currentR,hE5x5s[i]->GetMean()), "lp");
    // 找到 DiX=0, DiY=0 對應的 Bin 編號
    for(int iX = 0; iX<=4;iX++){
      for(int iY = 0; iY<=4;iY++){
        int binX = h3C->GetXaxis()->FindBin(iX*1.);
        int binY = h3C->GetYaxis()->FindBin(iY*1.);
        // 3. 核心動作：提取 Z 軸切片 (ProjectionZ)
        // 參數說明：名稱, X軸Bin起點, X軸Bin終點, Y軸Bin起點, Y軸Bin終點
        TString stmp = Form("h1s_%d_%d%d", i,iX,iY);
        stmp.ReplaceAll("-","N");
        mh1s[iX][iY] = h3C->ProjectionZ(stmp, binX, binX, binY, binY);
        auto h1 = mh1s[iX][iY];
        // 設定樣式
        h1->SetTitle(Form("Edep Distribution (DiX=%d, DiY=%d);Edep [MeV];count",iX,iY));
        h1->SetLineColor(TColor::GetColorPalette(i * (255/NScans))); // 使用調色盤自動著色
        h1->SetMarkerColor(TColor::GetColorPalette(i * (255/NScans))); // 使用調色盤自動著色
        h1->SetLineWidth(2);
        h1->SetMarkerSize(2);
        h1->SetMarkerStyle(22);
        h1->SetStats(0); // 隱藏統計盒以便疊圖
        leg[iX][iY]->AddEntry(h1, Form("R < %.1f mm, #mu=%.1f", currentR,h1->GetMean()), "lp");
        
      }
    }
    h1s[i] = mh1s;
  }
  canvas55->cd()->SetLogy(1);
  double maxVal5x5 = 0;
  for(int i=0; i<NScans; i++) 
    if(hE5x5s[i]->GetMaximum() > maxVal5x5)
      maxVal5x5 = hE5x5s[i]->GetMaximum();
  for(int i=0; i<NScans; i++){
    hE5x5s[i]->SetLineColor(TColor::GetColorPalette(i * (255/NScans))); // 使用調色盤自動著色
    hE5x5s[i]->SetMarkerColor(TColor::GetColorPalette(i * (255/NScans))); // 使用調色盤自動著色
    hE5x5s[i]->SetLineWidth(2);
    hE5x5s[i]->SetMarkerSize(2);
    hE5x5s[i]->SetMarkerStyle(22);
    hE5x5s[i]->Scale(1./maxVal5x5);
    hE5x5s[i]->GetYaxis()->SetRangeUser(3e-4,10);
    hE5x5s[i]->Draw((i==0?"he":"hesame"));
  }
  leg5x5->Draw();
  canvas55->Print(Form("%sE5x5_iXiY_TrackCutScan.gif",dirGraph.data()));

  // 4. 疊圖繪製
  // cProj->SetLogy(1); // 通常能量分佈看 Log 比較清楚
  
  double maxVal = 0;
  for(int i=0; i<NScans; i++)
    for(int iX = 0; iX<=4;iX++)
      for(int iY = 0; iY<=4;iY++)
        if(h1s[i][iX][iY]->GetMaximum() > maxVal)
          maxVal = h1s[i][iX][iY]->GetMaximum();
  TCanvas *canvas3 = new TCanvas("canvas3", "TH3F Slices", 5*600+4, 5*600+28);
  canvas3->Divide(5,5,0.0,0.0); // 8x8 排列
  for(int iX = 0; iX<=4;iX++){
    for(int iY = 0; iY<=4;iY++){
      for (int i = NScans-1; i >= 0; i--) {
        auto hProjZ = (TH1D*) h1s[i][iX][iY]->Clone(Form("%s_c",h1s[i][iX][iY]->GetName()));
        if(iX==0&&iY==0){
          cProj->cd();
          if(i==NScans-1) hProjZ->Draw("eh");
          else     hProjZ->Draw("ehsame");
        }
        int padIndex = (iX+1)+(4-iY)*5;
        // cout<<" "<<158<<" "<<iX<<" "<<iY<<" "<<padIndex<<endl;
        canvas3->cd(padIndex)->SetGrid(1,1);
        canvas3->cd(padIndex)->SetLogy(1);
        double CSFX = 
          canvas3->GetPad(padIndex)->GetAbsWNDC() / canvas3->GetPad(25)->GetAbsWNDC();
        double CSFY = 
          canvas3->GetPad(padIndex)->GetAbsHNDC() / canvas3->GetPad(25)->GetAbsHNDC();
        hProjZ->Scale(1./maxVal);
        hProjZ->GetYaxis()->SetMaxDigits(1);
        hProjZ->GetYaxis()->SetTitleSize(0.09/CSFY);
        hProjZ->GetYaxis()->SetLabelSize(0.07/CSFY);
        hProjZ->GetYaxis()->SetTitleOffset(0.95*CSFY);
        hProjZ->GetXaxis()->SetTitleSize(0.1/CSFX);
        hProjZ->GetXaxis()->SetLabelSize(0.1/CSFX);
        hProjZ->GetXaxis()->SetTitleOffset(0.75*CSFX);
        hProjZ->GetXaxis()->SetNdivisions(-505);
        hProjZ->GetYaxis()->SetNdivisions(-505);
        hProjZ->GetYaxis()->SetRangeUser(3e-4,5);
        if(i==NScans-1) hProjZ->Draw("eh");
        else     hProjZ->Draw("ehsame");
      }
      leg[iX][iY]->Draw();
    }
  
  }
  cProj->cd(); leg[0][0]->Draw();
  TString GraphName = Form("EnergySlices_iXiY_5x5_D%d_X%03.2fY%03.2f", DID, xoff, yoff);
  GraphName.ReplaceAll("-","N");   
  GraphName.ReplaceAll(".","O");
  // canvas3->Print(GraphName.Data());
  canvas3->Print(Form("%s%s.gif", dirGraph.data(),GraphName.Data()));
  cProj->Print(Form("%sEnergySlices_iXiY_Overlap_D%d.gif", dirGraph.data(), DID));

  for (int i = 0; i < NScans; i++) {
    // cout<<h1s[i][0][0]->GetMaximum()<<endl;
    h1s[i][0][0]->Scale(1./h1s[i][0][0]->GetMaximum());
    h1s[i][0][0]->GetYaxis()->SetTitle("Ratio, max = 1");
  }
  for (int i = 0; i < NScans; i++) {
    h1s[i][0][0]->GetYaxis()->SetRangeUser(0,1.2);
    if (i == 0) {
      h1s[i][0][0]->Draw("hist");
    } else {
      h1s[i][0][0]->Draw("hist same");
    }
    // cout<<h1s[i][0][0]->GetMaximum()<<endl;
  }
  
  leg[0][0]->Draw();
  cProj->Print(Form("%sEnergySlices_iXiY_Overlap_log_D%d.gif", dirGraph.data(), DID));
    CleanupHistogramsAndCanvases();
}
void Draw3X3InCut_XY( const string dirAnaPath ,  const string FileName ){
  string SourceFileNameEvents = dirAnaPath+FileName+"_ReConCalSort.root";
  TFile *file  = TFile::Open(SourceFileNameEvents.data());
  dirGraph = dirAnaPath+string("/graphCal/");
  
  TTree *t = (TTree*) file->Get("t");
  if (!t) {
      std::cerr << "Error: TTree pointer is null." << std::endl;
      std::cerr << "  Draw3X3InCut_XY.C("<<dirAnaPath<<","<<FileName<<")"<< std::endl;
      return;
  }else{
    cout << "  Draw3X3InCut_XY.C("<<file->GetName()<< std::endl;
  }
  
  for(size_t iZDC = 0; iZDC < CrystalDets.size(); iZDC++){
    int DID = CrystalDets[iZDC];
    for(int iSet=0;iSet<int(offsetSet.size());iSet++){
      xoff = offsetSet[iSet][0]; yoff = offsetSet[iSet][1];
      Draw3X3InCut_XY(t, DID);
    }
  }
}
void Draw3X3InCut_XY() {
  cout<<"Finished compiling of Draw3X3InCut_XY.C+"<<endl;
  Draw3X3InCut_XY("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/", "Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4");
}
