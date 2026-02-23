#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "BeamMonitorClass.h" 
using namespace std;
const string CalibrationFile[4]={
  "/data8/ZDC/EMCal/ShareScript/ChannelCalTableLYSO.dat",
  "/data8/ZDC/EMCal/ShareScript/ChannelCalTableBMT1.dat",
  "/data8/ZDC/EMCal/ShareScript/ChannelCalTableBMT2.dat",
  "/data8/ZDC/EMCal/ShareScript/ChannelCalTablePbWO.dat"
};
map< int , vector<double> > LoadChCalTable(double &mainSlope,double &mainIntercept, int DID){
  string FileIN = "";
  if(DID==1) FileIN = CalibrationFile[0];
  else if(DID==2) FileIN = CalibrationFile[1];
  else if(DID==3) FileIN = CalibrationFile[2];
  else if(DID==4) FileIN = CalibrationFile[3];
  else FileIN = "/data8/ZDC/EMCal/ShareScript/ChannelCalTableNAN.dat";
  ifstream in(FileIN.data());
  int chid, bMain;
  double  Slope, Intercept;
  map< int , vector<double> > table;
  string title;
  getline(in,title);
  // cout<<title<<endl;
  for(int i=0;i<=64;i++){
    table[i].resize(2, -1);
  }
  while(in>>chid>>bMain>>Slope>>Intercept){
    if(bMain){
      mainSlope = Slope;
      mainIntercept = Intercept;
    }
    table[chid][0]=(Slope);
    table[chid][1]=(Intercept);
  }
  for(const auto & it:table){

    cout<<it.first<<" "<<it.second[0]<<" "<<it.second[1]<<endl;
  }
  // throw;
  return table;
}
class EventSorter {
public:
    std::vector<int> *iHit, *ROCID, *channel, *DID, *Fire, *ADC, *GADC, *GID, *iX, *iY, *iZ;
    std::vector<bool> *HGMode;
    std::vector<float> *pX, *pY, *pZ;
    std::vector<double> *EqGADC;
    std::vector<int> *cFire;
    int MainDID, nHits;
    int peakID, nHEXs, nHE33, nHE55;
    int peakiX, peakiY;
    float peakpX, peakpY;
    bool bGoodEV;
    void Ini(){
      HGMode=nullptr;
      channel=nullptr; Fire=nullptr; ADC=nullptr; ROCID=nullptr; GID=nullptr; iHit=nullptr;
      iX=nullptr; iY=nullptr; iZ=nullptr; DID=nullptr; nHits = 0;
      nHEXs = 0; nHE33 = 0; nHE55 = 0;
      pX=nullptr; pY=nullptr; pZ=nullptr; GADC=nullptr;
      EqGADC = new vector<double>();
      cFire = new vector<int>();
    }
    EventSorter(int mainDID, TTree* t) : MainDID(mainDID){
        Ini();
        BindTree(t);
    }
    void Clear(){
      EqGADC->clear();
      cFire->clear();
      nHE33 = 0;
      nHE55 = 0;
      peakiX = -9999;
      peakiY = -9999;
      peakpX = -9999;
      peakpY = -9999;
    }
    void BindTree(TTree *tree) {
        cout << "Binding branches..." << std::endl;
        if (!tree){
          std::cout << "Error: Branches bound not successfully!" << std::endl;
          throw;
        } 
        // cout<<73<<endl;
        tree->SetBranchAddress("nHits", &nHits);
        tree->SetBranchAddress("iHit", &iHit);
        tree->SetBranchAddress("DID", &DID);
        tree->SetBranchAddress("ROCID", &ROCID);
        // cout<<76<<endl;
        tree->SetBranchAddress("channel", &channel);
        tree->SetBranchAddress("Fire", &Fire);
        tree->SetBranchAddress("HGMode", &HGMode);
        // cout<<81<<endl;
        tree->SetBranchAddress("ADC", &ADC);
        tree->SetBranchAddress("GADC", &GADC);
        tree->SetBranchAddress("GID", &GID);
        // cout<<85<<endl;
        tree->SetBranchAddress("iX", &iX);
        tree->SetBranchAddress("iY", &iY);
        tree->SetBranchAddress("iZ", &iZ);
        // cout<<89<<endl;
        tree->SetBranchAddress("pX", &pX);
        tree->SetBranchAddress("pY", &pY);
        tree->SetBranchAddress("pZ", &pZ);
        cout << "Branches bound successfully!" << std::endl;
        // if (!iHit || !ROCID || !channel || !DID || !Fire || !ADC || !GADC || !GID || !iX || !iY || !iZ || !pX || !pY || !pZ) {
            // std::cout << "Error: One or more branches are not bound correctly!" << std::endl;
            // throw std::runtime_error("Branches binding failed");
        // }
    }

    void SortAndRank();
    void CalAnaVars(int NHitsMD, double &GADC11, double &GADC33, double &GADCXs, double &GADC55, double &GADCAL,float &CenpX, float &CenpY, int &Cortex);
    int GetPeakCry() const { return peakID; }
};
void EventSorter::SortAndRank() {
    if (!iZ || iZ->empty()) return;  // 確保指標有效且不為空

    size_t n = iZ->size();
    std::vector<size_t> indices(n);
    for (size_t i = 0; i < n; ++i) indices[i] = i;

    std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        if ((*iZ)[a] == (*iZ)[b]) return (*EqGADC)[a] > (*EqGADC)[b];
        return (*iZ)[a] < (*iZ)[b];
    });

    // 修正 `reorder` lambda
    auto reorder = [&indices](auto *vec) {
        if (!vec || vec->empty()) return;
        using T = typename std::remove_reference_t<decltype(*vec)>::value_type;
        std::vector<T> temp(vec->size());
        
        for (size_t i = 0; i < indices.size(); ++i) temp[i] = (*vec)[indices[i]];

        // 特殊處理 vector<bool>
        if constexpr (std::is_same_v<T, bool>) {
            for (size_t i = 0; i < indices.size(); ++i) (*vec)[i] = temp[i];
        } else {
            *vec = std::move(temp);
        }
    };

    reorder(iHit);
    reorder(ROCID);
    reorder(channel);
    reorder(DID);
    reorder(EqGADC);
    reorder(cFire);
    reorder(Fire);
    reorder(HGMode);  // vector<bool>，需要特殊處理
    reorder(ADC);
    reorder(GADC);
    reorder(GID);
    reorder(iX);
    reorder(iY);
    reorder(iZ);
    reorder(pX);
    reorder(pY);
    reorder(pZ);

    // 計算排名
    int currentRank = 0;
    peakID = -1;
    for (size_t i = 0; i < n; ++i) {
        if (i > 0 && (*iZ)[i] != (*iZ)[i - 1]) {
            currentRank = 0;
            if ((*DID)[i] == MainDID) peakID = i;
        }
        (*iHit)[i] = currentRank++;
    }
    // cout<<169<<peakID<<endl;
}
void EventSorter::CalAnaVars(int NHitsMD, double &GADC11, double &GADC33, double &GADCXs, double &GADC55, double &GADCAL,float &CenpX, float &CenpY, int &Cortex) {
  // cout<<171<<" "<<peakID<<"~"<<peakID+NHitsMD<<endl;
  GADC11 = 0,GADC33 = 0,GADCXs = 0,GADC55 = 0,GADCAL = 0;
  CenpX = 0; CenpY = 0;
  nHEXs=0; nHE33=0; nHE55=0;

  peakiX = iX->at(peakID);
  peakiY = iY->at(peakID);
  // cout<<"origin: "<<PeakiX<<" "<<PeakiY<<" "<<PeakID<<endl;
  // cout<<177<<" "<<peakID<<"~"<<peakID+NHitsMD<<endl;
  map< pair<int,int> , bool > HitPosXs, HitPos33, HitPos55;
  for(int iH=peakID;iH<peakID+NHitsMD;iH++){
    // cout<<iH<<" "<<peakID<<"~"<<peakID+NHitsMD<<endl;
    // cout<<iXEV[iH]<<" "<<iYEV[iH]<<endl;
    // CenpX += Esav->pX->at(i)*Esav->GADC->at(i);
    // CenpY += Esav->pY->at(i)*Esav->GADC->at(i);
    int TmpiX = iX->at(iH);
    int TmpiY = iY->at(iH);
    // cout<<"tmpos: "<<TmpiX<<" "<<TmpiY<<endl;
    double GADCTMP = EqGADC->at(iH);
    if(GADCTMP<0) GADCTMP = 0;
    GADCAL+=GADCTMP;
    if(abs(peakiX-TmpiX)<3&&abs(peakiY-TmpiY)<3){
      // HitPos55[peakiX-TmpiX]
      // nHE55++;
      HitPos55[{peakiX-TmpiX,peakiY-TmpiY}] = 1;
      GADC55 += GADCTMP;
      if(abs(peakiX-TmpiX)<2&&abs(peakiY-TmpiY)<2){
        GADC33 += GADCTMP;
        // nHE33++;
        HitPos33[{peakiX-TmpiX,peakiY-TmpiY}] = 1;
        if((abs(peakiX-TmpiX) +abs(peakiY-TmpiY))<2){
          GADCXs += GADCTMP;
          HitPosXs[{peakiX-TmpiX,peakiY-TmpiY}] = 1;
          if((abs(peakiX-TmpiX) +abs(peakiY-TmpiY))==0){
            GADC11 += GADCTMP;
          }
        }
      }
      CenpX += pX->at(iH)*GADCTMP;
      CenpY += pY->at(iH)*GADCTMP;
    }
  }
  nHEXs = HitPosXs.size();
  nHE33 = HitPos33.size();
  nHE55 = HitPos55.size();
  // cout<<GADC33<<" "<<GADC55<<endl;
      
  if(GADC11 <= 0){
    CenpX = -9999;
    CenpY = -9999;
    Cortex = -9999;
    bGoodEV = false;
    peakiX = -9999;
    peakiY = -9999;
  }else{
    peakpX = pX->at(peakID);
    peakpY = pY->at(peakID);
    CenpX /= GADC55;
    CenpY /= GADC55;
    Cortex = abs(peakiX-3.5)>abs(peakiY-3.5)
      ? abs(peakiX-3.5)-0.5 
      : abs(peakiY-3.5)-0.5;
    bGoodEV = true;
  }
}
// string SetDefaultFilePath = "/data8/ZDC/EMCal/ShareScript/LYSOBT_HVScan/HV405V/";
// string SetDefaultFileName = "HV210Eng706MeV_ReCon";
// int SetDefaultMDDID = 1;
string SetDefaultFilePath = "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/";
string SetDefaultFileName = "Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4_ReCon";
int SetDefaultMDDID = 4;
void OverwriteBranch(
  // string FilePath = SetDefaultFilePath,  string FileName = SetDefaultFileName,  int MDDID = SetDefaultMDDID
  string FilePath ,  string FileName ,  int MDDID 
) {
  
  TFile *oldFile = new TFile((FilePath+FileName+string(Form("_MDID%d.root",MDDID))).data(),"read");
  if (!oldFile->IsOpen()) {
      std::cerr << "Error: File could not be opened!" << std::endl;
      return;  // 退出或处理错误
  }
  std::cout << "File opened successfully!" << std::endl;
  std::cout << oldFile->GetName() << std::endl;

  TTree *oldTree = (TTree*)oldFile->Get("t");
  if (!oldTree) {
      std::cerr << "Error: TTree 't' not found in the file!" << std::endl;
      return;  // 退出或处理错误
  }
  std::cout << "TTree loaded successfully!" << std::endl;
  string NemFileNameFull = FilePath+FileName+string(Form("Cal_MDID%d.root",MDDID));
  string NemFileNameGlob = FilePath+FileName+string(Form("Cal.root"));
  TFile *newFile = new TFile(NemFileNameFull.data(), "RECREATE");
  std::cout << "New File: "<<NemFileNameFull << std::endl;

  map< int,double> mainSlope, mainIntercept;
  map< int, map< int , vector<double> > > ChTable;
  ChTable[1] = LoadChCalTable(mainSlope[1], mainIntercept[1],1);
  ChTable[2] = LoadChCalTable(mainSlope[2], mainIntercept[2],2);
  ChTable[3] = LoadChCalTable(mainSlope[3], mainIntercept[3],3);
  ChTable[4] = LoadChCalTable(mainSlope[4], mainIntercept[4],4);
  TTree *newTree = oldTree->CloneTree(0); // 創建空的副本

  int PeakID = 0,nHits = 0,nHit0 = 0,nHit1 = 0,nHit2 = 0,nHit3 = 0;
  EventSorter Event(MDDID,oldTree);
  oldTree->SetBranchAddress("nHits", &nHits);
  oldTree->SetBranchAddress("nHits0", &nHit0);
  oldTree->SetBranchAddress("nHits1", &nHit1);
  oldTree->SetBranchAddress("nHits2", &nHit2);
  oldTree->SetBranchAddress("nHits3", &nHit3);
  newTree->Branch("EqGADC", &Event.EqGADC);
  newTree->Branch("cFire", &Event.cFire);
  newTree->Branch("bGoodEV",&Event.bGoodEV);
  double cGADC11 = 0, cGADC33 = 0, cGADCXs = 0, cGADC55 = 0, cGADCAL = 0;
  float  CenpX, CenpY; int Cortex;
  bool bGoodEV;
  newTree->Branch("cGADC11",&cGADC11);
  newTree->Branch("cGADC33",&cGADC33);
  newTree->Branch("cGADC55",&cGADC55);
  newTree->Branch("cGADCAL",&cGADCAL);
  newTree->Branch("cGADCXs",&cGADCXs);
  newTree->Branch("cCenpX",&CenpX);
  newTree->Branch("cCenpY",&CenpY);
  newTree->Branch("cCortex",&Cortex);
  newTree->Branch("nHEXs",&Event.nHEXs);
  newTree->Branch("nHE33",&Event.nHE33);
  newTree->Branch("nHE55",&Event.nHE55);
  newTree->Branch("PeakpX",&Event.peakpX);
  newTree->Branch("PeakpY",&Event.peakpY);
  newTree->Branch("PeakiX",&Event.peakiX);
  newTree->Branch("PeakiY",&Event.peakiY);
  
  double BMCpX0,BMCpY0,BMCpZ0;
  double BMCpX1,BMCpY1,BMCpZ1;
  double BMDXDZ,BMDYDZ,BMDXDZd,BMDYDZd,BMTheta,BMPhi;
  newTree->Branch("BMCpX0",&BMCpX0);
  newTree->Branch("BMCpY0",&BMCpY0);
  newTree->Branch("BMCpZ0",&BMCpZ0);
  newTree->Branch("BMCpX1",&BMCpX1);
  newTree->Branch("BMCpY1",&BMCpY1);
  newTree->Branch("BMCpZ1",&BMCpZ1);
  newTree->Branch("BMDXDZ",&BMDXDZ);
  newTree->Branch("BMDYDZ",&BMDYDZ);
  newTree->Branch("BMDXDZd",&BMDXDZd);
  newTree->Branch("BMDYDZd",&BMDYDZd);
  newTree->Branch("BMTheta",&BMTheta);
  newTree->Branch("BMPhi",&BMPhi);
  int BM1nPixs, BM2nPixs;
  int BM1nPixX, BM2nPixX;
  int BM1nPixY, BM2nPixY;
  newTree->Branch("BM1nPixs",&BM1nPixs);
  newTree->Branch("BM2nPixs",&BM2nPixs);
  newTree->Branch("BM1nPixX",&BM1nPixX);
  newTree->Branch("BM1nPixY",&BM1nPixY);
  newTree->Branch("BM2nPixX",&BM2nPixX);
  newTree->Branch("BM2nPixY",&BM2nPixY);
  double CCpdZDC1x, CCpdZDC1y, CCpdZDC2x, CCpdZDC2y;
  newTree->Branch("CCpdZDC1x",&CCpdZDC1x);
  newTree->Branch("CCpdZDC1y",&CCpdZDC1y);
  newTree->Branch("CCpdZDC2x",&CCpdZDC2x);
  newTree->Branch("CCpdZDC2y",&CCpdZDC2y);
  
  // cout<<172<<endl;
  Long64_t nentries = oldTree->GetEntries();
  // cout<<174<<" "<<nentries<<endl;
  
  int gid = -1, gadc = -1, adc = -1;
  double eqgadc = 0;
    
  for (Long64_t i = 0; i < nentries; i++) {
    // cout<<214<<" "<<i<<endl;
    oldTree->GetEntry(i);
    // try {
        // oldTree->GetEntry(i);
    // } catch (std::exception& e) {
        // std::cerr << "Error reading entry " << i << ": " << e.what() << std::endl;
        // break;
    // }
    // cout<<222<<endl;
    vector<BMHit> Trigger1PosX, Trigger1PosY, Trigger2PosX, Trigger2PosY;
    vector<BMHit> ZDC1, ZDC2;
    // cout<<nHits<<endl;
    if(nHit2<=0) continue;
    if(nHit0>20&&nHit1>20) continue;
    for(int iH = 0; iH<nHits;iH++){
      double px = Event.pX->at(iH), py = Event.pY->at(iH), pz = Event.pZ->at(iH);
      int iz = Event.iZ->at(iH);
      double Weight = 1;
      if(iz==1) Trigger2PosX.push_back(BMHit(px,0,pz,Weight));
      else if(iz==2) Trigger2PosY.push_back(BMHit(0,py,pz,Weight));
      else if(iz==3) Trigger1PosX.push_back(BMHit(px,0,pz,Weight));
      else if(iz==4) Trigger1PosY.push_back(BMHit(0,py,pz,Weight));
      else if(iz==5){
        // h2D1->Fill(px,py,Weight);
        ZDC1.push_back(BMHit(px,py,pz,Weight));
      }else if(iz==6){
        // h2D2->Fill(px,py,Weight);
        ZDC2.push_back(BMHit(px,py,pz,Weight));
      } 

    }
    // vector<BMHit> 
      // Trigger1 = GetCombination(Trigger1PosX,Trigger1PosY),
      // Trigger2 = GetCombination(Trigger2PosX,Trigger2PosY);
    // BMHit CenterT1 = GetCenter(Trigger1);
    // BMHit CenterT2 = GetCenter(Trigger2);
    BMHit CenterT1x = GetCenter(Trigger1PosX);
    BMHit CenterT1y = GetCenter(Trigger1PosY);
    BMHit CenterT2x = GetCenter(Trigger2PosX);
    BMHit CenterT2y = GetCenter(Trigger2PosY);
    // BMHit CenterD1 = GetCenter(ZDC1);
    // BMHit CenterD2 = GetCenter(ZDC2);
    BMHit CenterT1(CenterT1x.x,CenterT1y.y,(CenterT1x.z+CenterT1y.z)/2.);
    BMHit CenterT2(CenterT2x.x,CenterT2y.y,(CenterT2x.z+CenterT2y.z)/2.);

    // double AVGDisT1 = AverageDistance(Trigger1,CenterT1);
    // double AVGDisT2 = AverageDistance(Trigger2,CenterT2);
    // if(AVGDisT1>8||AVGDisT2>8) continue;
    // CheckDataRM(Trigger1,CenterT1);
    // CheckDataRM(Trigger2,CenterT2);
    // CheckDataRMZDC1(ZDC1,CenterD1);
    // CheckDataRMZDC2(ZDC2,CenterD2);
    // CenterT1 = GetCenter(Trigger1);
    // CenterT2 = GetCenter(Trigger2);
    BM1nPixs = Trigger1PosX.size()*Trigger1PosY.size();
    BM2nPixs = Trigger2PosX.size()*Trigger2PosY.size();
    BM1nPixX = Trigger1PosX.size();
    BM1nPixY = Trigger1PosY.size();
    BM2nPixX = Trigger2PosX.size();
    BM2nPixY = Trigger2PosY.size();
    // CenterD1 = GetCenter(ZDC1);
    // CenterD2 = GetCenter(ZDC2);
  
    // const auto [cxD1, cyD1, czD1, cwD1] = CenterD1;
    // const auto [cxD2, cyD2, czD2, cwD2] = CenterD2 ;
    const auto [cxD1, cyD1, czD1, cwD1] = CenterT1;
    const auto [cxD2, cyD2, czD2, cwD2] = CenterT2 ;
    BMCpX0 = cxD1;
    BMCpY0 = cyD1;
    BMCpZ0 = czD1;
    BMCpX1 = cxD2;
    BMCpY1 = cyD2;
    BMCpZ1 = czD2;
    if(Event.DID->at(nHit0+nHit1)!=MDDID) Event.MainDID = Event.DID->at(nHit0+nHit1);
    double posZ1 = ( Event.MainDID==1 ? -7.51 : -7.55 );
    double posZ2 = ( Event.MainDID==1 ? -137.35 : -9999 );
    double CT1x = CenterT1.x, CT2x =CenterT2.x, CT1y =CenterT1.y, CT2y =CenterT2.y;
    double ttmp1 = (posZ1 - CenterT1.z) / (CenterT2.z - CenterT1.z);
    double CTIx1 = CT1x + ttmp1 * (CT2x - CT1x), CTIy1 = CT1y + ttmp1 * (CT2y - CT1y);
    double ttmp2 = (posZ2 - CenterT1.z) / (CenterT2.z - CenterT1.z);
    double CTIx2 = CT1x + ttmp2 * (CT2x - CT1x), CTIy2 = CT1y + ttmp2 * (CT2y - CT1y);
    double slopex = (CT2x-CT1x)/(CenterT2.z-CenterT1.z), slopey = (CT2y-CT1y)/(CenterT2.z-CenterT1.z);
    CCpdZDC1x = CTIx1, CCpdZDC1y = CTIy1;
    if(Event.MainDID==1){ CCpdZDC2x = CTIx2; CCpdZDC2y = CTIy2;}
    else {CCpdZDC2x = -9999; CCpdZDC2y = -9999;}
    BMDXDZ = slopex;
    BMDYDZ = slopey;
    BMDXDZd = atan(BMDXDZ)/3.1415926*180.;
    BMDYDZd = atan(BMDYDZ)/3.1415926*180.;
    BMPhi = TMath::ATan2(slopey, slopex);
    BMTheta = TMath::ATan2(sqrt(slopex*slopex + slopey*slopey), 1.0);
    /*
    for (const auto& [xi2, yi2, zi2, wi2] : Trigger1){
      for (const auto& [xi1, yi1, zi1, wi1] : Trigger2){
        // const auto& [cxD1, cyD1, czD1, cwD1] : CenterD1 ;
        double t = (-7.51 - zi1) / (zi2 - zi1);
        double xtmp = xi1 + t * (xi2 - xi1);
        double ytmp = yi1 + t * (yi2 - yi1);
        // PITC1->SetPoint(indexAll,xtmp,ytmp);
        double DifX = cxD1-xtmp, DifY = cyD1-ytmp;
        // PointDiff1->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
        // h2Diff1->Fill( DifX, DifY,1./(index1*index2));
        // cout<<cxD1<<" "<<cyD1<<endl;
        // cout<<DifX<<" "<<DifY<<" "<<1./(index1*index2)<<endl;
        
      // const auto& [cxD2, cyD2, czD2, cwD2] : CenterD2 ;
      
        t = (-137.35 - zi1) / (zi2 - zi1);
        xtmp = xi1 + t * (xi2 - xi1);
        ytmp = yi1 + t * (yi2 - yi1);
        // PITC2->SetPoint(indexAll,xtmp,ytmp);
        DifX = cxD2-xtmp, DifY = cyD2-ytmp;
        // if(nHits3!=0) PointDiff2->SetPoint(indexAll, DifX, DifY); // Z 值將自動對應顏色
        // if(nHits3!=0) h2Diff2->Fill( DifX, DifY,1./(index1*index2));
      
        indexAll++;
      }
    }*/
    for(int iH=0;iH<nHits;iH++){
      // cout<<183<<" "<<iH<<" / "<<nHit2<<" / "<<nHits<<" / "<<endl;
      // cout<<56<<" "<<PeakID<<endl;
      // cout<<Event.iHit->at(iH)<<endl;
      
      int did = Event.DID->at(iH); //cout<<226<<endl;
      gid = Event.GID->at(iH);
      gadc = Event.GADC->at(iH); //cout<<228<<endl;
      // cout<<60<<" "<<gid<<" "<<gadc<<endl;
      if(ChTable[did][gid][0]!=-1){
        // cout<<62<<" "<<ChTable[gid][0]<<" "<<ChTable[gid][1]<<endl;

        if(did==2||did==3) eqgadc = (Event.ADC->at(iH)-ChTable[did][gid][1])/ChTable[did][gid][0]*mainSlope[did];
        else eqgadc = (gadc-ChTable[did][gid][1])/ChTable[did][gid][0]*mainSlope[did];//*(MDDID==4 ? 3./117.85 : 1);
        Event.EqGADC->push_back(eqgadc);

        // cout<<65<<" "<<gid<<" "<<eqgadc<<endl;
      }else{
        // cout<<67<<" "<<-1<<endl;
        Event.EqGADC->push_back(-1);
      } //cout<<238<<endl;
      if(eqgadc<0) Event.cFire->push_back(0);
      else Event.cFire->push_back(1);
      // if(Event.GetPeakCry()!=-1){
      // }
      // cout<<241<<endl;
    }
    
    
    // cout<<243<<endl;
    Event.SortAndRank();
    Event.CalAnaVars(nHit2, cGADC11, cGADC33, cGADCXs, cGADC55, cGADCAL, CenpX, CenpY, Cortex);
    newTree->Fill();
    Event.Clear();
    cGADC11 = 0, cGADC33 = 0, cGADCXs = 0, cGADC55 = 0, cGADCAL = 0;
    // cout<<243<<endl;
  }
  
  newTree->Write();
  newFile->Close();
  oldFile->Close();
  
  std::cout << "New File: "<<NemFileNameFull<<" has been created!" << std::endl;
  system(Form("ln %s %s",NemFileNameFull.data(),NemFileNameGlob.data()));
}

void OverwriteBranch(bool non){
  cout<<"FInished complie of OverwriteBranchLYSO "<<endl;
}
void OverwriteBranch(){
  cout<<"FInished complie of OverwriteBranchLYSO "<<endl;
}