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
#include <TStopwatch.h> // 加入計時器

#include "./GIDMapping.h"
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"
#include "/data4/YuSiang/personalLib/RPU/DBMLayouts.h"
#include "/data8/ZDC/EMCal/ShareScript/GrystalBallFitLibs.h"
#include "/data4/YuSiang/personalLib/EFFTool/MemoryClear.h"

using namespace std;

// 全域變數保留
vector<string> WeightStr = {"GADC","1","(iHit==0)"};
extern vector<TString> WtNames;
extern vector<TString> sfWtNames;
string dirGraph = "";
string outTimeFile = "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/TCADR.dat";

void SetHistColor(TH1* h, int color){
    h->SetLineColor(color);
    h->SetMarkerColor(color);
    h->SetLineWidth(2);
}
void SetFlineColor(TF1* f, int color){
    f->SetLineColor(color);
    f->SetLineWidth(2);
    f->SetLineStyle(2);
}

TFile *fSaveHist;
void SaveOrUpdate(TH1* h,string Tag = "") {
  if (!fSaveHist || !h) return;
  fSaveHist->cd(); // 切換到存檔目錄
  h->Write(Form("%s%s",h->GetName(),Tag.data()), TObject::kOverwrite);
}
void DrawADCRec(TTree *t, const int iDet){
    ofstream ofssw(outTimeFile, std::ios::app);
    TStopwatch sw;
    sw.Start();
    // [加速關鍵 1] 暫時關閉圖形顯示，只在記憶體中繪圖 (Batch Mode)
    bool wasBatch = gROOT->IsBatch();
    gROOT->SetBatch(kTRUE); 

    // 防止大量暫存的 Projection 直方圖自動加入目錄，造成變慢
    bool oldAddDir = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);

    map< int, Sensor > sensorMap = LoadSensorMap(iDet);
    setTDRStyle();
    TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    gStyle->Copy(*oldStyle);
    TGaxis::SetMaxDigits(3);
    gStyle->SetTitleYOffset(1.00);
    
    GSPadMargins(0.15,0.15,0.005,0.005);

    // --- 準備直方圖 ---
    // 手動管理這些主要直方圖，讓它們可以被 Draw 存取
    bool bBMs = iDet == 2|| iDet ==3;
    TH3D *h3;
    int NumXPad = (bBMs) ? 16 : NumIX, NumYPad = (bBMs) ? NumChs_T2*NumROCs_T2/16 : NumIY;
    if(bBMs)  h3 = new TH3D("h3","h3",NumXPad,1,NumXPad+1,NumYPad,0,NumYPad,66,-120,540);
    else       h3 = new TH3D("h3","h3",NumXPad,0,NumXPad,NumYPad,0,NumYPad,120,-2000,11000*2);
    TH2D *h2 = new TH2D("h2","h2;GID;ADC",NumAllChs,0,NumAllChs,120,-2000,11000*2);
    TH1D *h1ChNeV = new TH1D("h1ChNeV","h1ChNeV;GID;count",NumAllChs,0,NumAllChs);
    std::vector<TH2D*> v_h2Pos; 
    for(size_t w=0; w<WeightStr.size(); ++w) {
        TH2D* h;
        if(bBMs) 
            h = new TH2D(Form("h2Pos_W%lu",w), "", NumIX, 0+0.5, NumIX+0.5, NumIY, 0+0.5, NumIY+0.5);
        else 
            h = new TH2D(Form("h2Pos_W%lu",w), "", NumIX, 0-0.5, NumIX-0.5, NumIY, 0-0.5, NumIY-0.5);
        
        h->SetDirectory(0);
        v_h2Pos.push_back(h);
    }
    
    TH1D *h1nHitX = new TH1D("h1nHitX",";Numbeer of hits;count",NumIX,0.5,NumIX+0.5);
    TH1D *h1nHitY = new TH1D("h1nHitY",";Numbeer of hits;count",NumIY,0.5,NumIY+0.5);
    TH1D *h1nHitC = new TH1D("h1nHitC",";Numbeer of hits;count",NumAllChs,0.5,NumAllChs+0.5);
    // 確保這些主要直方圖不會因為我們上面設了 AddDirectory(kFALSE) 而消失
    h3->SetDirectory(0);
    h2->SetDirectory(0);
    h1ChNeV->SetDirectory(0);
    h1nHitX->SetDirectory(0);
    h1nHitY->SetDirectory(0);
    h1nHitC->SetDirectory(0);

    // --- I/O 優化設定 ---
    t->SetBranchStatus("*", 0); 
    t->SetBranchStatus("ADC", 1);
    t->SetBranchStatus("iX", 1);
    t->SetBranchStatus("iY", 1);
    t->SetBranchStatus("iHit", 1);
    t->SetBranchStatus("DID", 1);
    t->SetBranchStatus("GID", 1);
    t->SetBranchStatus("HGMode", 1);
    t->SetBranchStatus("channel", 1);
    t->SetBranchStatus("ROCID", 1);

    std::vector<Double_t> *v_ADC = nullptr;
    std::vector<Int_t>    *v_iX = nullptr;
    std::vector<Int_t>    *v_iY = nullptr;
    std::vector<Int_t>    *v_iHit = nullptr;
    std::vector<Int_t>    *v_DID = nullptr;
    std::vector<Int_t>    *v_GID = nullptr;
    std::vector<Bool_t>   *v_HGMode = nullptr;
    std::vector<Int_t>    *v_channel = nullptr;
    std::vector<Int_t>    *v_ROCID = nullptr;

    t->SetBranchAddress("ADC", &v_ADC);
    t->SetBranchAddress("iX", &v_iX);
    t->SetBranchAddress("iY", &v_iY);
    t->SetBranchAddress("iHit",   &v_iHit   );
    t->SetBranchAddress("DID", &v_DID);
    t->SetBranchAddress("GID", &v_GID);
    t->SetBranchAddress("HGMode", &v_HGMode);
    t->SetBranchAddress("channel", &v_channel);
    t->SetBranchAddress("ROCID", &v_ROCID);

    // --- 迴圈讀取與填充 ---
    cout<<"Start : DrawADCRec (Loading Tree)"<<endl;
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (Loading Tree): " << sw.RealTime() << " s" << endl;
    sw.Start();
    Long64_t nEntries = t->GetEntries();
    for(Long64_t i=0; i<nEntries; ++i){
        t->GetEntry(i);
        if(!v_ADC) continue;
        size_t nHits = v_ADC->size();
        // 暫存 Beam Profile 計算用的變數 (針對每個 Weight)
        // 結構: [WeightIndex][0:SumX, 1:SumY, 2:SumW_X, 3:SumW_Y]
        double bp_stats[3][4] = {0}; 
        bool bp_hasSignal[3][2] = {false}; // [WeightIndex][0:X_valid, 1:Y_valid]
        map<pair<int,int>,double> cGADCSum;
        
        for(size_t h=0; h<nHits; ++h){
            if(v_DID->at(h) != iDet) continue; 
            double val = v_ADC->at(h);
            int gid = v_GID->at(h);
            int ix = v_iX->at(h);
            int iy = v_iY->at(h);
            int iHH = v_iHit->at(h);
            if(val>0) cGADCSum[{ix,iy}]+=val;
            
            // [警告修正] 這裡 h 已經是 size_t，比較安全
            double finalADC = val;
            if(v_HGMode && h < v_HGMode->size()) {
                if(v_HGMode->at(h)) finalADC /= 10.0;
            }
            
            int globalCh = v_channel->at(h) + v_ROCID->at(h) * 32;
            h2->Fill(globalCh, finalADC);
            h1ChNeV->Fill(globalCh);
            
            // 2. 累加 Beam Profile 統計量
            // 針對每一種 Weight 計算
            for(size_t w=0; w<WeightStr.size(); ++w) {
                double weight = 1.0;
                if(w == 0) weight = val; // GADC
                else if(w == 2) { if(iHH != 0) weight = 0; } // iHit==0 (僅首個 Hit)
                // w==1 為權重 1 (幾何中心)

                if(weight <= 0) continue;

                // X-plane (iY==0)
                
                if(bBMs){
                    bp_stats[w][0] += (ix) * weight * (iy==0); // Sum((iX-1)*W)
                    bp_stats[w][2] += weight * (iy==0);            // Sum(W)
                    bp_stats[w][1] += (iy) * weight * (ix==0); // Sum((iY-1)*W)
                    bp_stats[w][3] += weight * (ix==0);            // Sum(W)
                }else{
                    bp_stats[w][0] += (ix) * weight; // Sum((iX-1)*W)
                    bp_stats[w][2] += weight;            // Sum(W)
                    bp_stats[w][1] += (iy) * weight; // Sum((iY-1)*W)
                    bp_stats[w][3] += weight;            // Sum(W)
                }
            }
        } // End Hits Loop

        int NHITX = 0, NHITY = 0, NHITC = 0;
        for (auto const& [coord, summed_val] : cGADCSum) {
            int ix = coord.first;
            int iy = coord.second;
            if (bBMs) {
                if (ix == 0){
                  NHITY++;
                  h3->Fill((iy-1)%16+1, (iy-1)/16+2, summed_val);
                } 
                if (iy == 0) {
                  NHITX++;
                  h3->Fill((ix-1)%16+1, (ix-1)/16, summed_val);
                }
            } else {
                h3->Fill(ix, iy, summed_val);
                NHITC++;
                // if(ix==3&&iy==1)cout<<"\t"<<ix<<"\t"<< iy<<"\t"<< summed_val<<endl;
            }
        }
        if(bBMs){
          h1nHitX->Fill(NHITX);
          h1nHitY->Fill(NHITY);
        } 
        else 
          h1nHitC->Fill(NHITC);
        // 3. 填充 Beam Profile Histograms (每個 Event 一次)
        for(size_t w=0; w<WeightStr.size(); ++w) {
            // 只有當 X 和 Y 都有有效訊號時才填充
            double meanX = (bp_stats[w][2] > 0) ? bp_stats[w][0] / bp_stats[w][2] : -9999;
            double meanY = (bp_stats[w][3] > 0) ? bp_stats[w][1] / bp_stats[w][3] : -9999;
            // if( (bBMs && bp_hasSignal[w][0] && bp_hasSignal[w][1]) || (!bBMs))
            v_h2Pos[w]->Fill(meanX, meanY);
        }
    }
    
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (Loading Loop): " << sw.RealTime() << " s" << endl;
    cout<<"Finish : DrawADCRec (Loading Tree): "<< sw.RealTime() << " s"<<endl;
    sw.Start();
    cout<<"Start : DrawADCRec (Set NX*NY pads)"<<endl;
    t->ResetBranchAddresses();
    t->SetBranchStatus("*", 1);

    // --- 繪圖邏輯 ---
    h3->Sumw2();
    double MaxIn3D = h3->GetMaximum()*1.2;
    int Digit_3D = (MaxIn3D > 0) ? int(log10(MaxIn3D)) : 0;
    h3->Scale(1/pow(10,Digit_3D));
    MaxIn3D = h3->GetMaximum()*1.2;

    TCanvas *canvas = new TCanvas("canvas", "TH3F Slices", NumXPad*600+4, NumYPad*600+28);
    canvas->Divide(NumXPad, NumYPad,0,0);
    TCanvas *canvas3 = new TCanvas("canvas3", "TH3F Slices", NumXPad*600+4, NumYPad*600+28);
    canvas3->Divide(NumXPad, NumYPad,0.0001,0.0001);
    TH2D *h2ByH3ReduceZ = new TH2D("h2ByH3ReduceZ","iX VS iY VS #muADC;index X;index Y;Mean ADC",NumYPad,0,NumYPad,NumXPad,0,NumXPad);
    if(bBMs){
      h2ByH3ReduceZ->GetXaxis()->SetNdivisions(-2);
      h2ByH3ReduceZ->SetBins(NumYPad,0,NumYPad,NumXPad,0,NumXPad);
    }
    h2ByH3ReduceZ->SetDirectory(0); // 避免污染目錄

    // 用來收集迴圈中產生的暫存 Histogram，最後統一刪除以免記憶體洩漏
    std::vector<TH1*> tempHists;

    // 自動計算最後一個 Pad 的尺寸做為縮放基準
    int lastPadIdx = NumXPad * NumYPad;
    double baseW = (canvas->GetPad(lastPadIdx)) ? canvas->GetPad(lastPadIdx)->GetAbsWNDC() : 0.1;
    double baseH = (canvas->GetPad(lastPadIdx)) ? canvas->GetPad(lastPadIdx)->GetAbsHNDC() : 0.1;

    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (Set NX*NY pads): " << sw.RealTime() << " s" << endl;
    cout<<"Finish : DrawADCRec (Set NX*NY pads): "<< sw.RealTime() << " s"<<endl;
    sw.Start();
    for (int xbin = 1; xbin <= NumXPad; ++xbin) {
        for (int ybin = 1; ybin <= NumYPad; ++ybin) {
            int rootY = NumYPad - ybin + 1; 
            int rootPadIndex = (rootY - 1) * NumXPad + xbin;
            
            double CSFX = canvas->GetPad(rootPadIndex)->GetAbsWNDC() / baseW;
            double CSFY = canvas->GetPad(rootPadIndex)->GetAbsHNDC() / baseH;

            // 這裡會產生新的 TH1D，因為 AddDirectory(kFALSE)，需要手動管理
            TH1D *hProjZ = h3->ProjectionZ(TString::Format("projZ_%d_%d", xbin, ybin), xbin, xbin, ybin, ybin,"cutg");
            tempHists.push_back(hProjZ); // 加入垃圾回收清單

            if(bBMs)
                hProjZ->SetTitle(Form(";kADC;Counts #scale[0.8]{x10^{%d}}",Digit_3D));
            else
                hProjZ->SetTitle(Form(";kADC;Counts #scale[0.8]{x10^{%d}}",Digit_3D));
            
            hProjZ->SetMarkerSize(2);
            hProjZ->SetMarkerStyle(22);
            hProjZ->GetYaxis()->SetMaxDigits(1);
            hProjZ->GetYaxis()->SetTitleSize(0.09/CSFY);
            hProjZ->GetYaxis()->SetLabelSize(0.07/CSFY);
            hProjZ->GetYaxis()->SetTitleOffset((!bBMs ? 0.95:0.95)*CSFY);
            hProjZ->GetYaxis()->SetLabelOffset((!bBMs ? 0.01:0.01)*CSFY);
            hProjZ->GetXaxis()->SetTitleSize(0.1/CSFX);
            hProjZ->GetXaxis()->SetLabelSize(0.1/CSFX);
            hProjZ->GetXaxis()->SetTitleOffset((!bBMs ? 0.75:0.75)*CSFX);
            hProjZ->GetXaxis()->SetLabelOffset((!bBMs ? 0.01:0.01)*CSFX);
            hProjZ->GetXaxis()->SetNdivisions(-505);
            hProjZ->GetYaxis()->SetNdivisions(-505);
            
            canvas->cd(rootPadIndex)->SetGrid(1,1);
            hProjZ->GetYaxis()->SetRangeUser(0,MaxIn3D);
            hProjZ->Draw("eh");
            
            double mean = hProjZ->GetMean(), rms = hProjZ->GetRMS();
            TH1D *hProjZRange = (TH1D*) hProjZ->Clone();
            tempHists.push_back(hProjZRange); // 加入垃圾回收清單

            hProjZRange->GetXaxis()->SetRangeUser(mean-5*rms,mean+5*rms);
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
            if(!bBMs) h2ByH3ReduceZ->SetBinContent(ybin,xbin,hProjZRange->GetMean());
            else      h2ByH3ReduceZ->SetBinContent(ybin,xbin,hProjZRange->GetMean());
        }
    }

    // 存檔 (Batch Mode 下這裡非常快)
    
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (Finish drawing Loop): " << sw.RealTime() << " s" << endl;
    cout<<"Finish : DrawADCRec (Finish drawing Loop): "<< sw.RealTime() << " s"<<endl;
    sw.Start();
    canvas->Print(Form("%sADCVSCh_Track_D%d.gif",dirGraph.data(),iDet));
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (ADCVSCh_Track.gif): " << sw.RealTime() << " s" << endl;
    cout<<"Finish : DrawADCRec (Finish drawing Loop): "<< sw.RealTime() << " s"<<endl;
    sw.Start();
    canvas3->Print(Form("%sADCVSChGauge_Track_D%d.gif",dirGraph.data(),iDet));
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (ADCVSChGauge_Track.gif): " << sw.RealTime() << " s" << endl;
    cout<<"Finish : DrawADCRec (Drawing Loop): "<< sw.RealTime() << " s"<<endl;

    sw.Start();
    // 清理暫存的 Histograms (關鍵記憶體釋放)
    for(auto h : tempHists) delete h;
    tempHists.clear();

    gStyle->SetPadLeftMargin(0.13);
    gStyle->SetPadBottomMargin(0.10);
    GSPadMargins(0.150,0.130,0.05,0.050);
    
    TCanvas *canvas2D3D = new TCanvas("canvas2D3D", "",800+4, 800+28);
    canvas2D3D->SetGrid(1,1);
    h2ByH3ReduceZ->SetMarkerColor(2);
    h2ByH3ReduceZ->GetXaxis()->CenterLabels(1);
    if(!bBMs) h2ByH3ReduceZ->GetYaxis()->CenterLabels(1);
    else {
      h2ByH3ReduceZ->GetXaxis()->SetTitle("Direction of channel");
      h2ByH3ReduceZ->GetYaxis()->SetTitle("position index[2.1mm]");
      h2ByH3ReduceZ->GetXaxis()->SetBinLabel(1,"X01-16");
      h2ByH3ReduceZ->GetXaxis()->SetBinLabel(2,"X17-32");
      h2ByH3ReduceZ->GetXaxis()->SetBinLabel(3,"Y01-16");
      h2ByH3ReduceZ->GetXaxis()->SetBinLabel(4,"Y17-32");
    }
    h2ByH3ReduceZ->GetXaxis()->CenterTitle(1);
    h2ByH3ReduceZ->GetYaxis()->CenterTitle(1);
    gStyle->SetPaintTextFormat(".0f"); 

    h2ByH3ReduceZ->Draw("colztext");
    canvas2D3D->Print(Form("%sADCMeanVSiXiY_D%d.gif",dirGraph.data(),iDet));
    
    TCanvas *canvas2 = new TCanvas("canvas2", "",800+4, 800+28);
    canvas2->cd();
    h2->Draw("colz"); 
    canvas2->Print(Form("%sGADCVSGch_Track_D%d.gif",dirGraph.data(),iDet));

    canvas2->cd();
    h1ChNeV->SetMarkerSize(2);
    h1ChNeV->SetMarkerStyle(22);
    h1ChNeV->Draw("eh");
    canvas2->Print(Form("%sNevGch_D%d.gif",dirGraph.data(),iDet));

    // Beam Profile (Histogram creation)
    // 這裡同樣受益於 Batch Mode
    gStyle->SetPadRightMargin(0.2);
    gStyle->SetPadLeftMargin(0.1);
    
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec Beam profile: " << sw.RealTime() << " s" << endl;
    cout<<"Start : DrawADCRec Beam profile"<<endl;
    sw.Start();
    // ... [Beam Profile 繪圖部分保持原邏輯] ...
    // 因篇幅關係此處不重複，直接使用您原本的邏輯，
    // 因為 SetBatch(kTRUE) 仍然有效，所以這部分也會變快。
    // 定義要畫哪些 Weight
    vector<int> targetWeights;
    if(bBMs) { targetWeights = {0, 1, 2}; } // GADC, 1, iHit==0
    else { targetWeights = {0}; } // Only GADC

    for(int w : targetWeights) {
        TH2D* hPos = v_h2Pos[w];
        
        // 設定標題與軸
        if(bBMs)
            hPos->SetTitle("Beam profile("+WtNames[w]+"), "+ZImf[iDet]+","+DName[iDet]+";index X;index Y;count");
        else
            hPos->SetTitle("Beam profile(ADC Wt.), "+ZImf[iDet]+","+DName[iDet]+";index X;index Y;count");

        // 繪圖 (使用 LayoutProfile2DObj)
        LayoutProfile2DObj layout(hPos, 1080, "colz", "cTFit(Gaus)", "cTFit(Gaus)");
        
        // 檔名設定 (盡量維持原邏輯)
        TString nameSuffix = (bBMs) ? sfWtNames[w] : "ADCW";
        layout.Print(Form("%sHitVSPos_%s_D%d.gif", dirGraph.data(), nameSuffix.Data(), iDet));
        layout.Layout2x1D(600, "hep", "hep");
        layout.Print2x1D(Form("%sHitVSPos_%s_1D_D%d.gif", dirGraph.data(), nameSuffix.Data(), iDet));

        // 繪製 mm 單位圖 (Scale)
        hPos->SetTitle("Beam profile("+WtNames[w]+"), "+ZImf[iDet]+","+DName[iDet]+";X position[mm];Y position[mm];count");
        string drawOpt = Form("colz scale(%f,%f)", ChGapX[iDet], ChGapY[iDet]);
        LayoutProfile2DObj layout_mm(hPos, 1080, drawOpt, "cTFit(Gaus)", "cTFit(Gaus)");
        layout_mm.Print(Form("%sHitVSPos_%s_mm_D%d.gif", dirGraph.data(), sfWtNames[w].Data(), iDet));
        layout_mm.Layout2x1D(600, "hep", "hep");
        layout_mm.Print2x1D(Form("%sHitVSPos_%s_1D_mm_D%d.gif", dirGraph.data(), sfWtNames[w].Data(), iDet));
    }

    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec (Drawing beam profile): " << sw.RealTime() << " s" << endl;
    
    sw.Start();
    GSPadMargins(0.150,0.12,0.02,0.05);
    TCanvas *canvas5 = new TCanvas("canvas5", "",640+4, 640+28);
    canvas5->cd();
    canvas5->SetLogy(1);
    canvas5->SetGrid(1,1);
    if(bBMs){
      h1nHitX->SetTitle(Form("Z=%d,%s X",ZDetName[DName[iDet]+"X"],+DName[iDet].Data()));
      h1nHitY->SetTitle(Form("Z=%d,%s Y",ZDetName[DName[iDet]+"Y"],+DName[iDet].Data()));
      h1nHitX->SetStats(1);
      h1nHitY->SetStats(1);
      
      h1nHitX->SetLineColor(kBlack);
      h1nHitX->SetMarkerColor(kBlack);
      h1nHitX->SetLineWidth(2);
      h1nHitY->SetLineColor(kRed);
      h1nHitY->SetMarkerColor(kRed);
      h1nHitY->SetLineWidth(2);
      double upperR1 = h1nHitX->GetMean()+6*h1nHitX->GetRMS();
      double upperR2 = h1nHitY->GetMean()+6*h1nHitY->GetRMS();
      double upperR = upperR1>upperR2?upperR1:upperR2;
      double upperY = h1nHitX->GetMaximum()>h1nHitY->GetMaximum()
                      ?h1nHitY->GetMaximum():h1nHitX->GetMaximum();
      h1nHitX->GetXaxis()->SetRangeUser(0.5,upperR+0.5);
      h1nHitX->GetYaxis()->SetRangeUser(1,upperY*3);
      h1nHitX->Draw("he");
      canvas5->Update();
      TPaveStats *st1 = (TPaveStats*)h1nHitX->FindObject("stats");
      if (st1) {
          st1->SetX1NDC(0.70); st1->SetX2NDC(0.95);
          st1->SetY1NDC(0.75); st1->SetY2NDC(0.90);
          st1 = (TPaveStats*)st1->Clone("st1");
      }
      h1nHitY->Draw("he");
      canvas5->Update();
      TPaveStats *st2 = (TPaveStats*)h1nHitY->FindObject("stats");
      if (st2) {
          st2->SetTextColor(kRed);
          st2->SetLineColor(kRed);
          st2->SetX1NDC(0.70); st2->SetX2NDC(0.95);
          st2->SetY1NDC(0.55); st2->SetY2NDC(0.70);
          st2 = (TPaveStats*)st2->Clone("st2");
      }
      h1nHitX->Draw("he");
      h1nHitY->Draw("hesame");
      st1->Draw();
      st2->Draw();
      canvas5->Modified(); 
    }else{
      h1nHitC->SetTitle(Form("Z=%d,%s",ZDetName[DName[iDet]],+DName[iDet].Data()));
      h1nHitC->SetStats(1);
      
      h1nHitC->SetLineColor(kBlack);
      h1nHitC->SetMarkerColor(kBlack);
      h1nHitC->SetLineWidth(2);
      double upperR = h1nHitC->GetMean()+6*h1nHitC->GetRMS();
      h1nHitC->GetXaxis()->SetRangeUser(0.5,upperR+0.5);
      h1nHitC->GetYaxis()->SetRangeUser(1,h1nHitC->GetMaximum()*3);
      h1nHitC->Draw("he");
      TPaveStats *st1 = (TPaveStats*)h1nHitC->FindObject("stats");
      if (st1) {
          st1->SetX1NDC(0.70); st1->SetX2NDC(0.95);
          st1->SetY1NDC(0.75); st1->SetY2NDC(0.90);
          st1 = (TPaveStats*)st1->Clone("st1");
      }
      h1nHitC->Draw("he");
      st1->Draw();
      canvas5->Modified(); 
    }
    canvas5->Print(Form("%sHitVSnHit_D%d.gif", dirGraph.data(), iDet));
    
    // [加速關鍵 2] 恢復預設值
    TH1::AddDirectory(oldAddDir);
    gROOT->SetBatch(wasBatch); // 恢復原本的顯示模式
    
    // 清理手動管理的 Histograms
    delete h3;
    delete h2;
    delete h1ChNeV;
    delete h2ByH3ReduceZ;
    delete canvas;
    delete canvas3;
    delete canvas2;
    delete canvas2D3D;
    CleanupHistogramsAndCanvases();
    sw.Stop();
    if(ofssw.is_open()) ofssw << "DrawADCRec clear: " << sw.RealTime() << " s" << endl;
}
// 輔助函式保持原樣
// 保持 DrawEClusterRingAndShowerShape 不變，僅確保 BranchStatus
void DrawEClusterRingAndShowerShape(TTree *t){
    t->SetBranchStatus("*", 1); // 確保全開
    // ... [保留您原始的 DrawEClusterRingAndShowerShape 程式碼，完全不變] ...
    // 為節省篇幅，請直接使用您貼上的原始碼內容填入此處
    setTDRStyle();
    TGaxis::SetMaxDigits(3);
    GSPadMargins(0.150,0.130,0.03,0.050);
    gStyle->SetTitleYOffset(1.20);
    gStyle->SetOptFit(0);
    TCanvas *canvas6 = new TCanvas("canvas6", "",800+4, 800+28);
    canvas6->cd()->SetLogy(1);
    TH1F *h1All = new TH1F("h1All","ADC spectrum;10^{4}xGADC; count",90,0,11);
    h1All->SetStats(0);
    SetHistColor(h1All,1);
    h1All->SetTitle("ADC spectrum@ "+DName[MainDID]+";ADC;count");
    t->Draw("GADC/1e+4>>h1All",Form("DID==%d",4),"he");
    TH1F *h1Max = new TH1F("h1Max","h1max",90,0,22);
    SetHistColor(h1Max,2);
    t->Draw("GADC11/1e+4>>h1Max","","hesame");
    TH1F *h1sec = new TH1F("h1sec","h1sec",90,0,22);
    SetHistColor(h1sec,4);
    TH1F *h1thr = new TH1F("h1thr","h1thr",90,0,22);
    SetHistColor(h1thr,6);
    t->Draw("GADC[PeakID+1]/1e+4>>h1sec","nHits2>1","hesame");
    t->Draw("GADC[PeakID+2]/1e+4>>h1thr","nHits2>2","hesame");
    TLegend *LSpect = new TLegend(0.50,0.75,0.999,0.950);
    LSpect->SetNColumns(2);
    LSpect->AddEntry(h1All,Form("#splitline{all GADC}{Sta:%.1e}",h1All->Integral()),"le");
    LSpect->AddEntry(h1Max,Form("#splitline{GADC_{max}}{Sta:%.1e}",h1Max->Integral()),"le");
    LSpect->AddEntry(h1sec,Form("#splitline{GADC_{rd}}{Sta:%.1e}",h1sec->Integral()),"le");
    LSpect->AddEntry(h1thr,Form("#splitline{GADC_{th}}{Sta:%.1e}",h1thr->Integral()),"le");
    LSpect->Draw();
    h1All->GetYaxis()->SetRangeUser(1,h1All->GetMaximum()*30);
    canvas6->Print(Form("%sEmax_Track_D%d.gif",dirGraph .data(),MainDID));
    
    GSPadMargins(0.150,0.130,0.01,0.010);
    TCanvas *canvas7 = new TCanvas("canvas7", "",800+4, 800+28);
    canvas7->cd()->SetLogy(1);
    
    TH1F *h1TMP = new TH1F("h1TMP","h1TMP",3000,0,30*1e+4);
    int nBinsTMP; double xmin, xmax;
    t->Draw("GADC55>>h1TMP","","goff");
    GetBestBin(h1TMP, nBinsTMP, xmin, xmax, 300000, 10);
    TH1F *h1max = new TH1F("h1max",";10^{4}xGADC;count",nBinsTMP,xmin/1e+4,xmax/1e+4);
    TH1F *h13x3 = (TH1F*) h1max->Clone("h13x3");
    TH1F *h15x5 = (TH1F*) h1max->Clone("h15x5");
    TH1F *h1SUM = (TH1F*) h1max->Clone("h1SUM");

    SetHistColor(h1max,2);
    SetHistColor(h13x3,1);
    SetHistColor(h15x5,4);
    SetHistColor(h1SUM,6);
    t->Draw("GADCAL/1e+4>>h1SUM","","he");
    t->Draw("GADC33/1e+4>>h13x3","","hesame");
    t->Draw("GADC55/1e+4>>h15x5","","hesame");
    t->Draw("GADC11/1e+4>>h1max","","hesame");
    TF1 *f1max = GetBestTF1(h1max,"f1max");
    TF1 *f13x3 = GetBestTF1(h13x3,"f13x3");
    TF1 *f15x5 = GetBestTF1(h15x5,"f15x5");
    TF1 *f1SUM = GetBestTF1(h1SUM,"f1SUM");
    SetFlineColor(f1max,2);
    SetFlineColor(f13x3,1);
    SetFlineColor(f15x5,4);
    SetFlineColor(f1SUM,6);
    FitTH1byTF1(h1max,f1max,"Rq");
    FitTH1byTF1(h13x3,f13x3,"Rq");
    FitTH1byTF1(h15x5,f15x5,"Rq");
    FitTH1byTF1(h1SUM,f1SUM,"Rq");
    h1max->GetYaxis()->SetRangeUser(1,h1SUM->GetMaximum()*30);
    h13x3->GetYaxis()->SetRangeUser(1,h1SUM->GetMaximum()*30);
    h15x5->GetYaxis()->SetRangeUser(1,h1SUM->GetMaximum()*30);
    h1SUM->GetYaxis()->SetRangeUser(1,h1SUM->GetMaximum()*30);
    TLegend *LCRing = new TLegend(0.40,0.72,0.999,0.999);
    LCRing->Draw();
    DrawCBFitResultBox(h1max, f1max,"GADC_{max}", 0.41, 0.98, 0.7, 1);
    DrawCBFitResultBox(h13x3, f13x3,"GADC_{3x3}", 0.41, 0.84, 0.7, 1);
    DrawCBFitResultBox(h15x5, f15x5,"GADC_{5x5}", 0.71, 0.98, 0.7, 1);
    DrawCBFitResultBox(h1SUM, f1SUM,"GADC_{all}", 0.71, 0.84, 0.7, 1);
    canvas7->Print(Form("%sClustering_Track_Spectrum_D%d.gif",dirGraph .data(),MainDID));
    
    h1max->Draw("he");
    TLegend *LCRingAll = new TLegend(0.65,0.8,0.999,0.999);
    LCRingAll->Draw();
    DrawCBFitResultBox(h1max, f1max,"GADC_{max}", 0.660, 0.97, .9, 1);
    canvas7->Print(Form("%sClustering_Track_Spectrum_EMax_D%d.gif",dirGraph .data(),MainDID));
    h13x3->Draw("he");
    TLegend *LCRing3x3 = new TLegend(0.65,0.8,0.999,0.999);
    LCRing3x3->Draw();
    DrawCBFitResultBox(h13x3, f13x3,"GADC_{3x3}", 0.660, 0.97, .9, 1);
    canvas7->Print(Form("%sClustering_Track_Spectrum_E3x3_D%d.gif",dirGraph .data(),MainDID));
    h15x5->Draw("he");
    TLegend *LCRing5x5 = new TLegend(0.65,0.8,0.999,0.999);
    LCRing5x5->Draw();
    DrawCBFitResultBox(h15x5, f15x5,"GADC_{5x5}", 0.660, 0.97, .9, 1);
    canvas7->Print(Form("%sClustering_Track_Spectrum_E5x5_D%d.gif",dirGraph .data(),MainDID));
    h1SUM->Draw("he");
    TLegend *LCRingMax = new TLegend(0.65,0.8,0.999,0.999);
    LCRingMax->Draw();
    DrawCBFitResultBox(h1SUM, f1SUM,"GADC_{all}", 0.660, 0.97, .9, 1);
    canvas7->Print(Form("%sClustering_Track_Spectrum_EAll_D%d.gif",dirGraph .data(),MainDID));
    
    
    canvas7->Clear();
    canvas7->SetLogy(1);
    TH1F *h11b3 = new TH1F("h11b3", ";Energy Containment;count", 100, 0, 1.01);
    TH1F *h33b5 = new TH1F("h33b5", ";Energy Containment;count", 100, 0, 1.01);
    TH1F *h11b5 = new TH1F("h11b5", ";Energy Containment;count", 100, 0, 1.01);
    TH1F *h11bA = new TH1F("h11bA", ";Energy Containment;count", 100, 0, 1.01);
    t->Draw("GADC11/GADC33>>h11b3","","he");
    t->Draw("GADC11/GADC55>>h11b5","","hesame");
    t->Draw("GADC33/GADC55>>h33b5","","hesame");
    t->Draw("GADC11/GADCAL>>h11bA","","hesame");
    h11b3->SetStats(0);
    h33b5->SetStats(0);
    h11b5->SetStats(0);
    h11bA->SetStats(0);
    SetHistColor(h11b3,1);
    SetHistColor(h33b5,6);
    SetHistColor(h11b5,2);
    SetHistColor(h11bA,4);
    
    h11b3->GetYaxis()->SetRangeUser(1,h33b5->GetMaximum()*10);
    h33b5->GetYaxis()->SetRangeUser(1,h33b5->GetMaximum()*10);
    h11b5->GetYaxis()->SetRangeUser(1,h33b5->GetMaximum()*10);
    h11bA->GetYaxis()->SetRangeUser(1,h33b5->GetMaximum()*10);
    TLegend *LShowProfile = new TLegend(0.2,0.8,0.9,0.95);
    LShowProfile->SetNColumns(3);
    LShowProfile->SetLineColor(0);
    LShowProfile->SetFillStyle(3000);
    LShowProfile->AddEntry(h11b3,Form("#frac{Cal.GADC_{max}}{Cal.GADC_{3x3}}"),"le");
    LShowProfile->AddEntry(h11b5,Form("#frac{Cal.GADC_{max}}{Cal.GADC_{5x5}}"),"le");
    LShowProfile->AddEntry(h11bA,Form("#frac{Cal.GADC_{max}}{Cal.GADC_{all}}"),"le");
    LShowProfile->AddEntry(h11b3,Form("#color[%d]{#mu = %2.2f}",h11b3->GetLineColor(),h11b3->GetMean()),"T");
    LShowProfile->AddEntry(h11b5,Form("#color[%d]{#mu = %2.2f}",h11b5->GetLineColor(),h11b5->GetMean()),"T");
    LShowProfile->AddEntry(h11bA,Form("#color[%d]{#mu = %2.2f}",h11bA->GetLineColor(),h11bA->GetMean()),"T");
    LShowProfile->AddEntry(h33b5,Form("#frac{Cal.GADC_{3x3}}{Cal.GADC_{5x5}}"),"le");
    LShowProfile->AddEntry(h33b5,Form("#color[%d]{#mu = %2.2f}",h33b5->GetLineColor(),h33b5->GetMean()),"T");
    LShowProfile->Draw();
    canvas7->Print(Form("%sClustering_Track_Ratio_D%d.gif",dirGraph .data(),MainDID));
    
    h11b3->Draw("he");
    TLegend *LCRing1b3 = new TLegend(0.2,0.85,0.7,0.95);
    LCRing1b3->SetNColumns(2);
    LCRing1b3->SetLineColor(0);
    LCRing1b3->SetFillStyle(3000);
    LCRing1b3->AddEntry(h11b3,Form("#frac{GADC_{max}}{GADC_{3x3}}"),"le");
    LCRing1b3->AddEntry(h11b3,Form("#mu = %2.2f",h11b3->GetMean()),"T");
    LCRing1b3->Draw();
    canvas7->Print(Form("%sClustering_Track_Ratio_E11b33_D%d.gif",dirGraph .data(),MainDID));
    h33b5->Draw("he");
    TLegend *LCRing3b5 = new TLegend(0.2,0.85,0.7,0.95);
    LCRing3b5->SetNColumns(2);
    LCRing3b5->SetLineColor(0);
    LCRing3b5->SetFillStyle(3000);
    LCRing3b5->AddEntry(h33b5,Form("#frac{GADC_{3x3}}{GADC_{5x5}}"),"le");
    LCRing3b5->AddEntry(h33b5,Form("#mu = %2.2f",h33b5->GetMean()),"T");
    LCRing3b5->Draw();
    canvas7->Print(Form("%sClustering_Track_Ratio_E33b55_D%d.gif",dirGraph .data(),MainDID));
    h11b5->Draw("he");
    TLegend *LCRing1b5 = new TLegend(0.2,0.85,0.7,0.95);
    LCRing1b5->SetNColumns(2);
    LCRing1b5->SetLineColor(0);
    LCRing1b5->SetFillStyle(3000);
    LCRing1b5->AddEntry(h11b5,Form("#frac{GADC_{max}}{GADC_{5x5}}"),"le");
    LCRing1b5->AddEntry(h11b5,Form("#mu = %2.2f",h11b5->GetMean()),"T");
    LCRing1b5->Draw();
    canvas7->Print(Form("%sClustering_Track_Ratio_E11b55_D%d.gif",dirGraph .data(),MainDID));
    h11bA->Draw("he");
    TLegend *LCRing1bA = new TLegend(0.2,0.85,0.7,0.95);
    LCRing1bA->SetNColumns(2);
    LCRing1bA->SetLineColor(0);
    LCRing1bA->SetFillStyle(3000);
    LCRing1bA->AddEntry(h11bA,Form("#frac{GADC_{max}}{GADC_{All}}"),"le");
    LCRing1bA->AddEntry(h11bA,Form("#mu = %2.2f",h11bA->GetMean()),"T");
    LCRing1bA->Draw();
    canvas7->Print(Form("%sClustering_Track_Ratio_E11bAL_D%d.gif",dirGraph .data(),MainDID));
    
    ofstream ofs(Form("%sEmax_Track_sta.dat",dirGraph .data()));
    ofs<<"#Sta\tMean\tSig\tR13\tR15\tR1A"<<endl;
    ofs<<h1max->Integral()<<"\t"<<h1max->GetMean()<<"\t"<<h1max->GetRMS()<<"\t";
    ofs<<h11b3->GetMean()<<"\t"<<h11b5->GetMean()<<"\t"<<h11bA->GetMean()<<endl;
    
    ofs.close();
    CleanupHistogramsAndCanvases();
}

// 保持 DrawShowerProfile 不變
void DrawShowerProfile( TTree *t, int iDet){
    t->SetBranchStatus("*", 1);
    // ... [保留您原始的 DrawShowerProfile 程式碼] ...
    // 請複製您的原始碼填入
    map< int, Sensor > sensorMap = LoadSensorMap(iDet);
    setTDRStyle();
    GSPadMargins(0.144,0.144,0.16,0.02);
    // TGaxis::SetMaxDigits(3);
    gStyle->SetPaintTextFormat("0.4f");
    gStyle->SetTitleYOffset(1.20);
    gStyle->SetTitleXOffset(1.20);
    TCanvas *c = new TCanvas("c", "", 800+4, 640+28);
    c->cd()->SetLogz(1);
    TH2D *h2 = new TH2D("h2",";iX-iX_{max};iY-iY_{max}",
                        NumIX*2+1,-NumIX+.5,NumIX-.5,NumIY*2+1,-NumIY+.5,NumIY-.5);
    cout<<NumIX*2+1<<" "<<NumIX+.5<<" "<<NumIX-.5<<" "<<endl;
    t->Draw("DiY:DiX>>h2",Form("ADC*(DID==%d)",iDet),"goff");
    h2->Scale(1./h2->Integral());
    h2->GetZaxis()->SetRangeUser(1e-4,1e-0);
    h2->GetXaxis()->SetRangeUser(-1.5,1.5);
    h2->GetYaxis()->SetRangeUser(-1.5,1.5);
    h2->GetXaxis()->CenterTitle(1);
    h2->GetYaxis()->CenterTitle(1);
    h2->GetYaxis()->SetTitleOffset(1);
    h2->SetMinimum(1e-4); 
    h2->SetMarkerColor(2); 
    h2->Draw("coltextz");
    c->Print(Form("%sShowerProfile_2D_D%d.gif",dirGraph .data(),iDet));
    LayoutProfile2DObj layout(h2, 1080,"colztext","cTFit(Gaus)","cTFit(Gaus)");
    layout.pad_main->cd()->SetLogz(1);
    TGaxis::SetMaxDigits(3);
    gStyle->SetPaintTextFormat("1.3f");
    gStyle->SetPaintTextFormat("1.3f");
    layout.h2->SetMarkerSize(layout.h2->GetMarkerSize()*0.6); 
    layout.h2->SetMinimum(1e-4); 
    layout.h2->SetMarkerColor(2); 
    layout.h2->Draw("coltext");
    layout.Print(Form("%sShowerProfile_D%d.gif",dirGraph .data(),iDet));
    layout.Layout2x1D(600,"hep","hep");
    layout.Print2x1D(Form("%sShowerProfile_2x1D_D%d.gif",dirGraph .data(),iDet));
    
    h2->SetTitle(";X position[mm];Y position[mm];count");
    h2->SetBins(NumIX*2+1,(-NumIX+.5)*ChGapX[iDet],(NumIX-.5)*ChGapX[iDet],NumIY*2+1,(-NumIY+.5)*ChGapY[iDet],(NumIY-.5)*ChGapY[iDet]);
    h2->SetStats(0);
    h2->ResetStats();
    h2->Draw("coltextz");
    c->Print(Form("%sShowerProfile_2D_mm_D%d.gif",dirGraph .data(),iDet));
    
    LayoutProfile2DObj layout_mm(h2,1080,"colztext","cTFit(Gaus)","cTFit(Gaus)");
    layout_mm.pad_main->cd()->SetLogz(1);
    TGaxis::SetMaxDigits(3);
    gStyle->SetPaintTextFormat("1.3f");
    gStyle->SetPaintTextFormat("1.3f");
    layout_mm.h2->SetMarkerSize(layout_mm.h2->GetMarkerSize()*0.6); 
    layout_mm.h2->SetMinimum(1e-4); 
    layout_mm.h2->SetMarkerColor(2); 
    layout_mm.h2->Draw("coltext");
    layout_mm.Print(Form("%sShowerProfile_mm_D%d.gif",dirGraph .data(),iDet));
    layout_mm.Layout2x1D(600,"hep","hep");
    layout_mm.Print2x1D(Form("%sShowerProfile_2x1D_mm_D%d.gif",dirGraph .data(),iDet));
}

// ======================================================================================
// 2. 優化版 DrawDetPosCorrelation (關聯圖)
//    將 72 次 t->Draw 改為 1 次 GetEntry 迴圈
// ======================================================================================

void DrawDetPosCorrelation( 
  TTree *t ,  const string WeightOpt,  const int iDetA,  const int iDetB
){
  setTDRStyle();
  gStyle->SetTitleXOffset(1.00);
  gStyle->SetTitleYOffset(1.00);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadLeftMargin(0.13);
  // ==========================================================
  // 1. 定義四個平面的非加權平均頻道索引表達式
  //    我們假設 (iX-1) 或 (iY-1) 是頻道索引 (從0開始)
  //    且分母 Sum$(1*...) 是計算觸發的頻道數 (Hits)
  // ==========================================================
  // 2D 直方圖的邊界和分箱 (Binning)
  double minIdXY[4] = {0}, maxIdXY[4] = {0};
  // TString sZlay[4] = {};
  // P1: Z=1 (BM.2, X-plane) - 使用 DID=2 和 iY==0 選擇器
  // P2: Z=2 (BM.2, Y-plane) - 使用 DID=2 和 iX==0 選擇器
  // P3: Z=3 (BM.1, X-plane) - 使用 DID=3 和 iY==0 選擇器
  // P4: Z=4 (BM.1, Y-plane) - 使用 DID=3 和 iX==0 選擇器
  TString p1_X1, p2_Y2, p3_X3, p4_Y4;
  // double ScaleFactor;
  if(iDetA==2||iDetA==3){
    p1_X1 = "Sum$((iX-1)*(iY==0)*(DID==TargetDid)*Wei)/Sum$(Wei*(iY==0)*(DID==TargetDid))";
    p2_Y2 = "Sum$((iY-1)*(iX==0)*(DID==TargetDid)*Wei)/Sum$(Wei*(iX==0)*(DID==TargetDid))";
    p1_X1.ReplaceAll("Wei", TString(WeightOpt));
    p2_Y2.ReplaceAll("Wei", TString(WeightOpt));
    if(iDetA==2){ maxIdXY[0] = NumChs_T1; maxIdXY[1] = NumChs_T1; };
    if(iDetA==3){ maxIdXY[0] = NumChs_T2; maxIdXY[1] = NumChs_T2; };
  }else{
    p1_X1 = "Sum$((iX)*(DID==TargetDid)*GADC)/Sum$(GADC*(DID==TargetDid))";
    p2_Y2 = "Sum$((iY)*(DID==TargetDid)*GADC)/Sum$(GADC*(DID==TargetDid))";
    if(iDetA==1){ maxIdXY[0] = NumiX_D1; maxIdXY[1] = NumiY_D1; };
    if(iDetA==4){ maxIdXY[0] = NumiX_D2; maxIdXY[1] = NumiY_D2; };
  }
  if(iDetB==2||iDetB==3){
    p3_X3 = "Sum$((iX-1)*(iY==0)*(DID==TargetDid)*Wei)/Sum$(Wei*(iY==0)*(DID==TargetDid))";
    p4_Y4 = "Sum$((iY-1)*(iX==0)*(DID==TargetDid)*Wei)/Sum$(Wei*(iX==0)*(DID==TargetDid))";
    p3_X3.ReplaceAll("Wei", TString(WeightOpt));
    p4_Y4.ReplaceAll("Wei", TString(WeightOpt));
    if(iDetB==2){ maxIdXY[2] = NumChs_T1; maxIdXY[3] = NumChs_T1; };
    if(iDetB==3){ maxIdXY[2] = NumChs_T2; maxIdXY[3] = NumChs_T2; };
  }else{
    p3_X3 = "Sum$((iX)*(DID==TargetDid)*GADC)/Sum$(GADC*(DID==TargetDid))";
    p4_Y4 = "Sum$((iY)*(DID==TargetDid)*GADC)/Sum$(GADC*(DID==TargetDid))";
    if(iDetB==1){ maxIdXY[2] = NumiX_D1; maxIdXY[3] = NumiY_D1; };
    if(iDetB==4){ maxIdXY[2] = NumiX_D2; maxIdXY[3] = NumiY_D2; };
  }
  
  p1_X1.ReplaceAll("TargetDid", TString(Form("%d",iDetA)));
  p2_Y2.ReplaceAll("TargetDid", TString(Form("%d",iDetA)));
  p3_X3.ReplaceAll("TargetDid", TString(Form("%d",iDetB)));
  p4_Y4.ReplaceAll("TargetDid", TString(Form("%d",iDetB)));

  // 定義所有可能的 6 種關聯組合及其標籤
  struct CorrelationPlot {
      TString name; TString title; TString drawCmd;
      TString xExpr; TString yExpr;
      int iLayA; int iLayB;
  };
  CorrelationPlot plots[] = {
    // 1. BM.2 內部 X-Y 耦合
    {"h2_X1vsY2", DName[iDetA]+"X vs "+DName[iDetA]+"Y;"+DName[iDetA]+"X Index("+sChGapX[iDetA]+"mm);"+DName[iDetA]+"Y Index("+sChGapY[iDetA]+"mm);Count", "", p1_X1, p2_Y2,0,1},
    // 3. 相同平面 X 在不同 Z 上的發散度
    {"h2_X1vsX3", DName[iDetA]+"X vs "+DName[iDetB]+"X;"+DName[iDetA]+"X Index("+sChGapX[iDetA]+"mm);"+DName[iDetB]+"X Index("+sChGapX[iDetB]+"mm);Count", "", p1_X1, p3_X3,0,2},
    // 5. 跨平面/跨 Z 關聯 (X1 vs Y4)
    {"h2_X1vsY4", DName[iDetA]+"X vs "+DName[iDetB]+"Y;"+DName[iDetA]+"X Index("+sChGapX[iDetA]+"mm);"+DName[iDetB]+"Y Index("+sChGapY[iDetB]+"mm);Count", "", p1_X1, p4_Y4,0,3},
    // 2. BM.1 內部 X-Y 耦合
    {"h2_X3vsY4", DName[iDetB]+"X vs "+DName[iDetB]+"Y;"+DName[iDetB]+"X Index("+sChGapX[iDetB]+"mm);"+DName[iDetB]+"Y Index("+sChGapY[iDetB]+"mm);Count", "", p3_X3, p4_Y4,2,3},
    // 4. 相同平面 Y 在不同 Z 上的發散度
    {"h2_Y2vsY4", DName[iDetA]+"Y vs "+DName[iDetB]+"Y;"+DName[iDetA]+"Y Index("+sChGapY[iDetA]+"mm);"+DName[iDetB]+"Y Index("+sChGapY[iDetB]+"mm);Count", "", p2_Y2, p4_Y4,1,3},
    // 6. 跨平面/跨 Z 關聯 (Y2 vs X3)
    {"h2_Y2vsX3", DName[iDetA]+"Y vs "+DName[iDetB]+"X;"+DName[iDetA]+"Y Index("+sChGapY[iDetA]+"mm);"+DName[iDetB]+"X Index("+sChGapX[iDetB]+"mm);Count", "", p2_Y2, p3_X3,1,2},
  };

  TCanvas *c = new TCanvas("c", "TH3F Slices", 3*640+4, 2*640+28);
  c->Divide(3, 2); // 8x8 排列
  // 繪製所有的 2D 關聯圖
  TH2D *h2[6];
  const Double_t x1 = 0.70,y1 = 0.15,x2 = 0.95,y2 = 0.40;
  // 左邊界,下邊界,右邊界,上邊界
  gStyle->SetOptFit(1111); // 顯示所有擬合資訊
  gStyle->SetOptStat(0);   // 關閉直方圖內容的統計框 (方便我們控制 Fit Stats Box)
  // 遍歷 6 張圖
  for (int i = 0; i < 6; ++i) {
    c->cd(i+1)->SetGrid(1,1);
    
    // 組合 TTree::Draw() 命令: Y-expression : X-expression
    plots[i].drawCmd = plots[i].yExpr + ":" + plots[i].xExpr + " >> " + plots[i].name;
    std::cout << plots[i].drawCmd.Data() << std::endl;
    
    // 創建 TH2D 直方圖 (使用您原有的 Binning 邏輯)
    int iLayA = plots[i].iLayA, iLayB = plots[i].iLayB;
    h2[i] = new TH2D(plots[i].name, plots[i].title
         , maxIdXY[iLayA], minIdXY[iLayA], maxIdXY[iLayA], maxIdXY[iLayB], minIdXY[iLayB], maxIdXY[iLayB]);
    std::cout << maxIdXY[iLayA] << "\t" << minIdXY[iLayA] << "\t" << maxIdXY[iLayA] << "\t" 
              << maxIdXY[iLayB] << "\t" << minIdXY[iLayB] << "\t" << maxIdXY[iLayB] << std::endl;
    
    // 執行 TTree::Draw()
    // 必須先 Draw，否則 Fit 會出錯 (因為 Draw 填充了直方圖)
    // 使用 "goff" 參數 (graphics off) 避免重複繪圖
    Long64_t entries = t->Draw(plots[i].drawCmd, "", "colz goff");
    std::cout << plots[i].name.Data() << " 填充事件數: " << entries << std::endl;

    // 設置軸標題和標籤居中
    h2[i]->GetXaxis()->CenterTitle(1);
    h2[i]->GetYaxis()->CenterTitle(1);
    h2[i]->GetXaxis()->CenterLabels(1);
    h2[i]->GetYaxis()->CenterLabels(1);

    // 繪製直方圖 (此時沒有 Stats Box)
    h2[i]->Draw("colz");
    if(i+1==2||i+1==5)
      TFitResultPtr fitResult = h2[i]->Fit("pol1", "S"); // "S" 參數用於返回 TFitResultPtr
    if (gPad) { // 確保當前 TPad 存在
      gPad->Update(); 
      TPaveStats *st = (TPaveStats*)h2[i]->FindObject("stats");
      if (st) {
        // 1. 設置 Stats Box 的 NDC 座標
        st->SetX1NDC(0.13); st->SetY1NDC(0.65); 
        st->SetX2NDC(0.65); st->SetY2NDC(0.95); 
        st->Draw();
      }
    }
    c->Update();
  }
  TString outputName = TString::Format("%sCorrelation_D%dtoD%d_%s.gif", dirGraph.data(),iDetA,iDetB,(WeightOpt == "GADC" ? "Wt" : "Unw"));
  c->Print(outputName);
  delete c;
  CleanupHistogramsAndCanvases();
}

void DrawDetPosCorrelations( TTree *t ){
  for(size_t iCorA = 0;iCorA<CorDets.size();iCorA++){
    for(size_t iCorB = iCorA+1;iCorB<CorDets.size();iCorB++){
      DrawDetPosCorrelation(t,WeightStr[0],CorDets[iCorA],CorDets[iCorB]);
      DrawDetPosCorrelation(t,WeightStr[1],CorDets[iCorA],CorDets[iCorB]);
    }
  }
  // DrawBMAllCombCorrelations(t);
}
// ======================================================================================
// 3. 主程式 DrawADCRec (含計時器與寫檔)
// ======================================================================================
void DrawADCRec( const string dirAnaPath ,  const string FileName ){
    // 開啟計時記錄檔
    ofstream ofs(outTimeFile, std::ios::app);
    if(ofs.is_open()) ofs << "\n>>> Start Processing: " << FileName << " <<<" << endl;

    string SourceFileNameEvents = dirAnaPath+FileName+"_ReCon.root";
    TFile *file  = TFile::Open(SourceFileNameEvents.data());
    TTree *t = (TTree*) file->Get("t");
    if (!t) {
        cerr << "Error: TTree pointer is null. " << SourceFileNameEvents << endl;
        if(ofs.is_open()) ofs << "Error: TTree null" << endl;
        return;
    }
    
    dirGraph = dirAnaPath+string("/graphRec/");
    system(Form("mkdir -p %s",dirGraph.data()));
    
    TStopwatch sw;

    // --- 1. DrawADCRec Loop ---
    sw.Start();
    for(size_t iDet = 0;iDet<AnaDets.size();iDet++){
        DrawADCRec(t,AnaDets[iDet]);
    }
    sw.Stop();
    if(ofs.is_open()) ofs << "DrawADCRec (Loop): " << sw.RealTime() << " s" << endl;

    // --- 2. DrawEClusterRingAndShowerShape ---
    sw.Start();
    DrawEClusterRingAndShowerShape(t);
    sw.Stop();
    if(ofs.is_open()) ofs << "DrawEClusterRingAndShowerShape: " << sw.RealTime() << " s" << endl;

    // --- 3. DrawDetPosCorrelations ---
    sw.Start();
    DrawDetPosCorrelations(t);
    sw.Stop();
    if(ofs.is_open()) ofs << "DrawDetPosCorrelations (Total): " << sw.RealTime() << " s" << endl;

    // --- 4. DrawShowerProfile Loop ---
    sw.Start();
    for(size_t iDet = 0;iDet<CrystalDets.size();iDet++){
        DrawShowerProfile(t,CrystalDets[iDet]);
    }
    sw.Stop();
    if(ofs.is_open()) {
        ofs << "DrawShowerProfile (Loop): " << sw.RealTime() << " s" << endl;
        ofs << "--------------------------------------------------" << endl;
        ofs.close();
    }
}

void DrawADCRec() {
    cout<<"Finished compiling of DrawADCRec.C+"<<endl;
  // DrawADCRec("/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/", "Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4");
}