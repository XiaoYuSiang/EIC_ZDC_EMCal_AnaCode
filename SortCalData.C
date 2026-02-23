#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "BeamMonitorClass.h" 
#include "GIDMapping.h" 
using namespace std;

extern vector<TString> WtNames;
extern vector<TString> sfWtNames;
/**
 * DetectorReader 類別
 * 用於讀取並管理探測器數據 Tree
 */
class DetectorReader {
public:
    // --- 數據成員 (Data Members) ---
    // 基本型別
    Long64_t eventID;
    Int_t pcnt, fcnt, nHits, nHits0, nROC0, nHits1, nROC1, nHits2, nROC2, nHits3, nROC3, dfcnt;
    Int_t iX0, iY0, PeakID, Cortex, cCortex, nHEXs, nHE33, nHE55, PeakiX, PeakiY;
    Int_t BM1nPixs, BM2nPixs, BM1nPixX, BM1nPixY, BM2nPixX, BM2nPixY;
    
    Float_t CenpX, CenpY, BeamPosX, BeamPosY, BeamPosZ, BeamEnergy, TempEnv;
    Float_t LiveTimeA, LiveTimeB, BufferLTA, BufferLTB, RotateAng, cCenpX, cCenpY, PeakpX, PeakpY;
    
    Double_t GADC11, GADC33, GADC55, GADCAL, GADCXs, cGADC11, cGADC33, cGADC55, cGADCAL, cGADCXs;
    Double_t BMCpX0, BMCpY0, BMCpZ0, BMCpX1, BMCpY1, BMCpZ1, BMDXDZ, BMDYDZ, BMDXDZd, BMDYDZd, BMTheta, BMPhi;
    Double_t CCpdZDC1x, CCpdZDC1y, CCpdZDC2x, CCpdZDC2y;
    
    Bool_t bGoodZDC;

    // 向量型別 (必須初始化為 nullptr)
    std::vector<int> *iHit = nullptr;
    std::vector<int> *DID = nullptr;
    std::vector<int> *ROCID = nullptr;
    std::vector<int> *channel = nullptr;
    std::vector<int> *Fire = nullptr;
    std::vector<int> *ADC = nullptr;
    std::vector<int> *GADC = nullptr;
    std::vector<int> *GID = nullptr;
    std::vector<int> *iX = nullptr;
    std::vector<int> *iY = nullptr;
    std::vector<int> *iZ = nullptr;
    std::vector<int> *DiX = nullptr;
    std::vector<int> *DiY = nullptr;
    std::vector<int> *cFire = nullptr;
    
    std::vector<float> *pX = nullptr;
    std::vector<float> *pY = nullptr;
    std::vector<float> *pZ = nullptr;
    
    std::vector<bool> *HGMode = nullptr;
    std::vector<double> *EqGADC = nullptr;

private:
    TTree *fTree = nullptr; // 指向被讀取的 Tree

public:
    // 建構子
    DetectorReader(TTree *tree = nullptr) {
        if (tree) Init(tree);
    }
    // 析構子 (清理記憶體)
    virtual ~DetectorReader() {
        if (!fTree) return;
        delete fTree->GetCurrentFile();
    }
    /**
     * 初始化：連結 Tree 的分支地址
     */
    void Init(TTree *tree) {
        if (!tree) return;
        fTree = tree;

        // --- 設定基本型別分支地址 ---
        fTree->SetBranchAddress("eventID", &eventID);
        fTree->SetBranchAddress("BeamEnergy", &BeamEnergy);
        fTree->SetBranchAddress("nHits",   &nHits);
        fTree->SetBranchAddress("nHits0",  &nHits0);
        fTree->SetBranchAddress("nHits1",  &nHits1);
        fTree->SetBranchAddress("nHits2",  &nHits2);
        fTree->SetBranchAddress("nHits3",  &nHits3);

        fTree->SetBranchAddress("DID",     &DID);
        fTree->SetBranchAddress("Fire",    &Fire);
        fTree->SetBranchAddress("iX",      &iX);
        fTree->SetBranchAddress("iY",      &iY);
        fTree->SetBranchAddress("iZ",      &iZ);
        fTree->SetBranchAddress("iHit",    &iHit);
        fTree->SetBranchAddress("cFire",   &cFire);
        
        // fTree->SetBranchAddress("DiX",     &DiX);
        // fTree->SetBranchAddress("DiY",     &DiY);
        fTree->SetBranchAddress("pX",      &pX);
        fTree->SetBranchAddress("pY",      &pY);
        fTree->SetBranchAddress("pZ",      &pZ);
        
        fTree->SetBranchAddress("HGMode",  &HGMode);
        fTree->SetBranchAddress("EqGADC",  &EqGADC);

        std::cout << "DetectorReader: Branches Successfully Linked." << std::endl;
    }

    /**
     * 讀取特定 Entry
     */
    Int_t GetEntry(Long64_t entry) {
        if (!fTree) return 0;
        return fTree->GetEntry(entry);
    }

    /**
     * 獲取總 Entry 數
     */
    Long64_t GetEntries() const {
        return fTree ? fTree->GetEntries() : 0;
    }
};
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
    bool bGoodZDC;
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
    bGoodZDC = false;
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
    bGoodZDC = true;
  }
}
class CrystalData{
public:
  int DiX, DiY;
  TVector3 iPos;
  TVector3 Pos;
  double E, cADC;
  int nChs;
  CrystalData(int ix =0,int iy =0,int iz =0,double px =0,double py =0,double pz =0, double e =0, double ADC =0){
    iPos = TVector3(ix, iy, iz);
    Pos = TVector3(px,py,pz);
    E = e; cADC = ADC;
    nChs = 1;
  }
  void CalByCent(int iX0, int iY0){
    DiX = (int)(iPos.X() - iX0);
    DiY = (int)(iPos.Y() - iY0);
    // cout<<DiX<<" "<<DiY<<" "<<cADC<<" "<<endl;
  }
  int iX() const { return (int)iPos.X(); }
  int iY() const { return (int)iPos.Y(); }
  int iZ() const { return (int)iPos.Z(); }
  int pX() const { return (int)Pos.X(); }
  int pY() const { return (int)Pos.Y(); }
  int pZ() const { return (int)Pos.Z(); }
};
void FillbyPos(std::vector<CrystalData>& dataVec) {
  if (dataVec.empty()) return;

  // 使用 map 進行座標聚合，Key 為 {iX, iY}
  std::map<std::pair<int, int>, CrystalData> mergedMap;

  for (const auto& item : dataVec) {
    std::pair<int, int> coord = { item.iX(), item.iY() };
    
    if (mergedMap.find(coord) == mergedMap.end()) {
      // 第一次發現該座標
      mergedMap[coord] = item;
      // cout<<item.iX()<<" "<<item.iY()<<" "<<item.E<<endl;
    } else {
      // 座標重複，累加能量
      mergedMap[coord].E += item.E;
      mergedMap[coord].cADC += item.cADC;
      mergedMap[coord].nChs += 1;
      // cout<<item.iX()<<" "<<item.iY()<<"+"<<item.E<<endl;
    }
  }

  // 將合併後的結果放回原 vector
  dataVec.clear();
  for (auto const& item : mergedMap) {
    dataVec.push_back(item.second);
  }
}

/**
 * 按照能量 (E) 從高到低排序 (0 = 最高)
 */
void SortbyEng(std::vector<CrystalData>& dataVec) {
  std::sort(dataVec.begin(), dataVec.end(), [](const CrystalData& a, const CrystalData& b) {
    return a.E > b.E; // 降序排序
  });
}
TVector3 CalCent(const std::vector<CrystalData>& dataVec){
  TVector3 centroid(0, 0, 0);
  double totalE = 0;
  for (const auto& data : dataVec) {
    // 只有正能量參與重心計算
    int dx = data.DiX;
    int dy = data.DiY;
    if (data.E > 0 && (abs(dx)<2&&abs(dy)<2) ) {
      centroid += data.E * data.Pos;
      totalE += data.E;
    }
  }
  // 安全檢查：若總能量為 0，回傳無效值
  if (totalE <= 0) {
    return TVector3(-9999, -9999, -9999);
  }
  return (1.0 / totalE) * centroid;
}
TVector3 CalCentLog(const std::vector<CrystalData>& dataVec, double W0 = 4.5) {
    TVector3 centroid(0, 0, 0);
    double totalW = 0;
    double totalE = 0;
    for (const auto& d : dataVec) if (d.E > 0) totalE += d.E;

    for (const auto& data : dataVec) {
        if (data.E <= 0) continue;
        // 計算對數權重
        double w = W0 + TMath::Log(data.E / totalE);
        if (w > 0) {
            centroid += w * data.Pos;
            totalW += w;
        }
    }
    return (totalW > 0) ? (1.0 / totalW) * centroid : TVector3(-9999,-9999,-9999);
}
/**
 * 計算 ZDC 不同範圍內的能量總和
 * 包含固定對稱範圍以及自動尋找最佳位移的矩形範圍
 * * 返回 vector 順序：
 * [0]:1x1, [1]:3x3, [2]:5x5, [3]:All, 
 * [4]:1x2, [5]:1x3, [6]:2x3, [7]:2x1, [8]:3x1, [9]:3x2
 */
std::vector<double> CalECollection(const std::vector<CrystalData>& dataVec,char mode) {
  double e1x1 = 0, e3x3 = 0, e5x5 = 0, eAll = 0;
  // 1. 計算固定對稱範圍與總能量
  for (const auto& data : dataVec) {
    int dx = data.DiX;
    int dy = data.DiY;
    double energy = (mode == 'E') ? data.E : data.cADC;
    
    if (energy <= 0) continue;
    // cout<<dx<<" "<<dy<<" "<<energy<<endl;
    eAll += energy;
    if (abs(dx) <= 0 && abs(dy) <= 0) e1x1 += energy;
    if (abs(dx) <= 1 && abs(dy) <= 1) e3x3 += energy;
    if (abs(dx) <= 2 && abs(dy) <= 2) e5x5 += energy;
  }
  // 2. 定義一個 Lambda 函數，用於尋找 WxH 矩形在包含 (0,0) 的所有位移中的最大能量
  auto GetMaxRectE = [&](int width, int height) {
    double maxE = 0;
    // 對於 WxH 的矩形，(0,0) 可能位在該矩形內的任一位置 (ox, oy)
    // ox 範圍 0 ~ width-1, oy 範圍 0 ~ height-1
    for (int ox = 0; ox < width; ++ox) {
      for (int oy = 0; oy < height; ++oy) {
        double currentSum = 0;
        // 此位移下的座標邊界
        int xMin = -ox;
        int xMax = width - 1 - ox;
        int yMin = -oy;
        int yMax = height - 1 - oy;

        for (const auto& data : dataVec) {
          if (data.DiX >= xMin && data.DiX <= xMax &&
            data.DiY >= yMin && data.DiY <= yMax) {
            currentSum += (mode == 'E') ? data.E : data.cADC;
          }
        }
        if (currentSum > maxE) maxE = currentSum;
      }
    }
    return maxE;
  };
  // 3. 計算所有指定的矩形範圍
  double e1x2 = GetMaxRectE(1, 2);
  double e1x3 = GetMaxRectE(1, 3);
  double e2x3 = GetMaxRectE(2, 3);
  double e2x1 = GetMaxRectE(2, 1);
  double e3x1 = GetMaxRectE(3, 1);
  double e3x2 = GetMaxRectE(3, 2);
  // cout<<e1x1<<" "<< e3x3<<" "<< e5x5<<" "<< eAll<<" "<< e1x2<<" "<< e1x3<<" "<< e2x3<<" "<< e2x1<<" "<< e3x1<<" "<< e3x2<<endl;
  return {e1x1, e3x3, e5x5, eAll, e1x2, e1x3, e2x3, e2x1, e3x1, e3x2};
}
// string SetDefaultFilePath = "/data8/ZDC/EMCal/ShareScript/LYSOBT_HVScan/HV405V/";
// string SetDefaultFileName = "HV210Eng706MeV_ReCon";
// int SetDefaultMDDID = 1;
string SetDefaultFilePath = "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/";
string SetDefaultFileName = "Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4_ReCon";
// int SetDefaultMDDID = 4;
void SortCalData(
  // string FilePath = SetDefaultFilePath,  string FileName = SetDefaultFileName
  string FilePath ,  string FileName
) {
  map<int , double> CrystalZPos = LoadCrystalZMap();
  TFile *oldFile = new TFile((FilePath+FileName+string(Form("Cal.root"))).data(),"read");
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
  DetectorReader reader(oldTree);
  std::cout << "TTree loaded successfully!" << std::endl;
  string NemFileNameFull = FilePath+FileName+string(Form("CalSort.root"));
  // string NemFileNameGlob = FilePath+FileName+string(Form("CalSort.root"));
  TFile *newFile = new TFile(NemFileNameFull.data(), "RECREATE");
  std::cout << "New File: "<<NemFileNameFull << std::endl;
  
  TTree *tOut = new TTree("t", "Beam Monitoring Output Tree");

  double cGADC11 = 0, cGADC33 = 0, cGADCXs = 0, cGADC55 = 0, cGADCAL = 0;
  float  CenpX, CenpY; int Cortex;
  // bool bGoodZDC;
  tOut->Branch("eventID", &reader.eventID, "eventID/L");
  // tOut->Branch("bGoodZDC", &reader.bGoodZDC, "bGoodZDC/O");

  // 能量包含度 (Energy Containment / Clustering Ratio)
  // tOut->Branch("cGADC11", &reader.cGADC11, "cGADC11/D");
  // tOut->Branch("cGADC33", &reader.cGADC33, "cGADC33/D");
  // tOut->Branch("cGADC55", &reader.cGADC55, "cGADC55/D");
  // tOut->Branch("cGADCAL", &reader.cGADCAL, "cGADCAL/D");

  // 向量數據 (注意：對於 Vector，ROOT 需要指針的地址)
  // 這裡我們直接利用 reader 類別中已經存在的 vector 指針
  
  
  // 假設 CrystalDets, WtNames 等變數已在外部定義
  const size_t NCry = CrystalDets.size();
  const size_t NTrk = TrackerDets.size();
  const size_t Nwt  = sfWtNames.size();
  
  double BeamEnergy;
  map<int , int> effnHits;
  map<int , int> PeakID;
  std::vector<int> DetID;
  std::vector<double> Edep, cADC ;
  std::vector<int> DiX, DiY  ;
  std::vector<int> iX, iY, nChs;
  std::vector<double> pX, pY, pZ;
  map<int ,int> bGoodZDC;
  tOut->Branch("BeamEnergy", &BeamEnergy);
  tOut->Branch("DID",     &DetID);
  tOut->Branch("cADC",    &cADC );
  tOut->Branch("Edep",    &Edep );
  tOut->Branch("DiX",     &DiX  );
  tOut->Branch("DiY",     &DiY  );
  tOut->Branch("iX",     &iX  );
  tOut->Branch("iY",     &iY  );
  tOut->Branch("nChs",   &nChs  );
  tOut->Branch("pX",     &pX  );
  tOut->Branch("pY",     &pY  );
  tOut->Branch("pZ",     &pZ  );
  // 定義 Branch 用的變數 (使用固定大小的 vector 確保地址穩定)
  vector<int>    NPixsX(Nwt, 0), NPixsY(Nwt, 0);
  vector<double> BMDXDZ(Nwt, -9999), BMDYDZ(Nwt, -9999);
  vector<double> BMDXDZd(Nwt, -9999), BMDYDZd(Nwt, -9999);
  vector<double> BMTheta(Nwt, -9999), BMPhi(Nwt, -9999);
  vector<int>    bGoodBMT(Nwt,false);
  map<int , vector<TVector3>> BMTracks;
  map<int , vector<TVector3>> CCpdZDCs;
  map<int , TVector3> CentPs, CentLPs;
  map<int, TVector3> Posi0s, Pos0s;
  map<int, vector<double> > eCol, aCol;
  TVector3 nullVec(-9999, -9999, -9999);
  const int Nclus  = ClusNameE.size();
  for(size_t iZDC = 0; iZDC < NCry; iZDC++){
    int DID = CrystalDets[iZDC];
    CCpdZDCs[DID].assign(Nwt, nullVec);
    eCol[DID].assign(Nclus, 0);
    aCol[DID].assign(Nclus, 0);
    CentPs[DID] = nullVec;
    CentLPs[DID] = nullVec;
    Posi0s[DID] = nullVec;
    Pos0s[DID]  = nullVec;
    effnHits[DID] = 0;
    PeakID[DID] = 0;
    bGoodZDC[DID] = 0;
  }
  for(size_t iZDC = 0; iZDC < NCry; iZDC++){
    int DID = CrystalDets[iZDC];
    tOut->Branch(Form("nHits%d",DID),   &effnHits[DID]);
    tOut->Branch(Form("PeakID%d",DID),   &PeakID[DID]);
    tOut->Branch(Form("CentP%d",DID),   &CentPs[DID]  );
    tOut->Branch(Form("CentLP%d",DID),   &CentLPs[DID]  );
    tOut->Branch(Form("Posi0_%d",DID),  &Posi0s[DID]  );
    tOut->Branch(Form("Pos0_%d",DID),   &Pos0s[DID]   );
    tOut->Branch(Form("bGoodZDC%d",DID), &bGoodZDC[DID] );
    
    for(size_t iclus = 0; iclus < (size_t)Nclus; iclus++){
      tOut->Branch(Form("%s_%d",ClusNameE[iclus].data(),DID), &eCol[DID][iclus] );
      tOut->Branch(Form("%s_%d",ClusNameA[iclus].data(),DID), &aCol[DID][iclus] );
    }
  }

  for(size_t iTrk = 0; iTrk < (size_t)NTrk; iTrk++){
    BMTracks[TrackerDets[iTrk]].assign(Nwt, nullVec);
  }
  // 針對不同的 Weight 設定建立分支
  for (size_t iw = 0; iw < Nwt; iw++) {
    TString wn = sfWtNames[iw];
    tOut->Branch(Form("NPixsX_%s", wn.Data()), &NPixsX[iw]);
    tOut->Branch(Form("NPixsY_%s", wn.Data()), &NPixsY[iw]);
    tOut->Branch(Form("BMDXDZ_%s", wn.Data()), &BMDXDZ[iw]);
    tOut->Branch(Form("BMDYDZ_%s", wn.Data()), &BMDYDZ[iw]);
    tOut->Branch(Form("BMTheta_%s", wn.Data()), &BMTheta[iw]);
    tOut->Branch(Form("BMPhi_%s", wn.Data()),   &BMPhi[iw]);
    tOut->Branch(Form("bGoodBMT_%s", wn.Data()),&bGoodBMT[iw]);
    for(size_t iZDC = 0; iZDC<NCry;iZDC++){
      int DID = CrystalDets[iZDC];
      tOut->Branch(
        Form("CCpdZDC%d_%s",DID, wn.Data()), &CCpdZDCs[DID][iw]);
    }
    for(size_t iTrk = 0; iTrk<(size_t)NTrk;iTrk++){
      int DID = TrackerDets[iTrk];
      tOut->Branch(
        Form("BMTrack%d_%s",DID, wn.Data()), &BMTracks[DID][iw]);
    }
  }
  // 2. 數據處理主迴圈
  Long64_t nentries = reader.GetEntries();
  TGraph *grX = new TGraph(),*grY = new TGraph();
  // for (Long64_t i = 0; i < 10; i++) {
  map< int, vector <double> > CGFs = CrystalGainFactors;
  for (Long64_t i = 0; i < nentries; i++) {
    reader.GetEntry(i);
    BeamEnergy = (double)reader.BeamEnergy;
    // cout<<i<<"/"<<nentries<<endl;
    // --- 恢復 ZDC 資訊讀入 ---
    // 這裡將所有屬於 CrystalDets 的 Hit 存入 ZDCs map
    map<int, vector<CrystalData>> ZDCs;
    int nHits = reader.pX->size(); 
    for (int iH = 0; iH < nHits; iH++) {
      int DID = reader.DID->at(iH);
      // 檢查該 DID 是否在我們關心的晶體列表中
      if (!bCrystal[DID]) continue; 
      // cout<<"Read: "<<iH<<"/"<<nHits<<endl;

      double px = reader.pX->at(iH);
      double py = reader.pY->at(iH);
      double pz = reader.pZ->at(iH);
      int iX = reader.iX->at(iH);
      int iY = reader.iY->at(iH);
      int iZ = reader.iZ->at(iH);
      double Weight = reader.EqGADC->at(iH); // 使用校正後的 ADC 作為權重
      // cout<<px<<" "<< py<<" "<< pz<<" "<< Weight<<endl;
      // 存入對應的 Detector Sequence ID 中，供後續運算使用
      double energy = 0;
      for(size_t i_gf=0;i_gf<CGFs[DID].size();i_gf++){
        // cout<<energy<<"+"<<pow(Weight,i)*CGFs[DID][i]<<endl;
        energy+=pow(Weight,i_gf)*CGFs[DID][i_gf];
      }
      ZDCs[DID].push_back( CrystalData(iX, iY, iZ, px, py, pz, energy,Weight) );
    }
    for(size_t iZDC = 0; iZDC<NCry;iZDC++){
      int DID = CrystalDets[iZDC];
      PeakID[DID] = -1;
      Posi0s[DID] = nullVec;
      Pos0s[DID]  = nullVec;
      std::fill(eCol[DID].begin(), eCol[DID].end(), 0);
      std::fill(aCol[DID].begin(), aCol[DID].end(), 0);
      FillbyPos(ZDCs[DID]);
      SortbyEng(ZDCs[DID]);
      // int iX0 = ZDCs[DID][0].iX(), iY0 = ZDCs[DID][0].iY();
      // CentPs[DID] = CalCent(ZDCs[DID]);
      // auto seed = ZDCs[DID][0];
      // ZDCs[DID].erase(std::remove_if(ZDCs[DID].begin(), ZDCs[DID].end(), [&](const CrystalData& d){
          // return (abs(d.iX() - seed.iX()) > 2 || abs(d.iY() - seed.iY()) > 2);
      // }), ZDCs[DID].end());
      if(ZDCs[DID].size()!=0){
        PeakID[DID] = int(DetID.size());
        Posi0s[DID] = ZDCs[DID][0].iPos;
        Pos0s[DID]  = ZDCs[DID][0].Pos;
      }
      int iX0 = ZDCs[DID].size() > 0 ? ZDCs[DID][0].iX() : -9999;
      int iY0 = ZDCs[DID].size() > 0 ? ZDCs[DID][0].iY() : -9999;
      for( auto &ZDCCh : ZDCs[DID] ){
        ZDCCh.CalByCent(iX0, iY0); 
        DetID.push_back(DID);
        DiX.push_back(ZDCCh.DiX);
        DiY.push_back(ZDCCh.DiY);
        iX.push_back(ZDCCh.iX());
        iY.push_back(ZDCCh.iY());
        nChs.push_back(ZDCCh.nChs);
        pX.push_back(ZDCCh.pX());
        pY.push_back(ZDCCh.pY());
        pZ.push_back(ZDCCh.pZ());
        Edep.push_back(ZDCCh.E);
        cADC.push_back(ZDCCh.cADC);
      }
      CentPs[DID] = CalCent(ZDCs[DID]);
      CentLPs[DID]= CalCentLog(ZDCs[DID]);
      effnHits[DID] = ZDCs[DID].size();
      bGoodZDC[DID] = !(CentPs[DID].X()==-9999);
      auto resultE = CalECollection(ZDCs[DID], 'E');
      auto resultA = CalECollection(ZDCs[DID], 'A');
      for(size_t k=0; k<resultE.size(); ++k) eCol[DID][k] = resultE[k];
      for(size_t k=0; k<resultA.size(); ++k) aCol[DID][k] = resultA[k];
    }
    // 每次 Entry 開始前重置數值
    std::fill(NPixsX.begin(), NPixsX.end(), 0);
    std::fill(NPixsY.begin(), NPixsY.end(), 0);
    std::fill(bGoodBMT.begin(), bGoodBMT.end(), false);
    std::fill(BMDXDZ.begin(), BMDXDZ.end(), -9999);
    std::fill(BMDYDZ.begin(), BMDYDZ.end(), -9999);
    std::fill(BMDXDZd.begin(), BMDXDZd.end(), -9999);
    std::fill(BMDYDZd.begin(), BMDYDZd.end(), -9999);
    std::fill(BMTheta.begin(), BMTheta.end(), -9999);
    std::fill(BMPhi.begin(), BMPhi.end(), -9999);
    for (size_t i_zdc = 0; i_zdc < NCry; i_zdc++) {
      std::fill(CCpdZDCs[CrystalDets[i_zdc]].begin(), CCpdZDCs[CrystalDets[i_zdc]].end(), nullVec);
    }
    for (size_t i_trk = 0; i_trk < NTrk; i_trk++) {
      std::fill(BMTracks[TrackerDets[i_trk]].begin(), BMTracks[TrackerDets[i_trk]].end(), nullVec);
    }
    // ... 其他重置 ...

    // 遍歷不同的 Weight 設定 (例如：1, GADC, 單一 Hit 等)
    for (size_t iWt = 0; iWt < Nwt; iWt++) {
      // cout<<"iWt: "<<iWt<<"/"<<Nwt<<endl;
      map<int, vector<BMHit>> TriggerPosX, TriggerPosY;
      
      // 蒐集符合條件的 Trigger Hits
      for (int iH = 0; iH < nHits; iH++) {
        int DID = reader.DID->at(iH);
        if (!bTrigger[DID]) continue;
        // cout<<"Read: "<<iH<<"/"<<nHits<<endl;
        if(iWt==2&&reader.iHit->at(iH)!=0) continue;
        double px = reader.pX->at(iH), py = reader.pY->at(iH), pz = reader.pZ->at(iH);
        int ix = reader.iX->at(iH), iy = reader.iY->at(iH);
        double Weight = (iWt == 1) ? 1.0 : reader.EqGADC->at(iH);
        // cout<<px<<" "<< py<<" "<< pz<<" "<< Weight<<endl;

        if (ix == 0) TriggerPosX[DID].push_back(BMHit(0, py, pz, Weight));
        if (iy == 0) TriggerPosY[DID].push_back(BMHit(px, 0, pz, Weight));
      }
      // cout<<472<<endl;
      // 計算中心點並進行擬合
      grX->Set(0); // 清空 Graph
      grY->Set(0);
      map < int , BMHit > CenterTx, CenterTy;
      int iXYZPT = 0;
      
      for(auto TriggerX :TriggerPosX){
        CheckDataRM(TriggerX.second,GetCenter(TriggerX.second));
        auto TriXtmp = TriggerX.second;
        CenterTx[TriggerX.first] = GetCenter(TriXtmp);
        auto cTx = CenterTx[TriggerX.first];
        double AVGDisTx = AverageDistance(TriXtmp,cTx);
        // CenterTx[TriggerX.first].Show();
        NPixsX[iWt] = TriXtmp.size();
        for(auto TriggerY :TriggerPosY){
          if(TriggerY.first == TriggerX.first){
            CheckDataRM(TriggerY.second,GetCenter(TriggerY.second));
            auto TriYtmp = TriggerY.second;
            CenterTy[TriggerY.first] = GetCenter(TriYtmp);
            auto cTy = CenterTy[TriggerY.first];
            double AVGDisTy = AverageDistance(TriYtmp,cTy);
            // CenterTy[TriggerY.first].Show();
            NPixsY[iWt] = TriYtmp.size();
            BMHit CenterTs = BMHit(
              CenterTy[TriggerX.first].x, CenterTx[TriggerY.first].y,
              (CenterTx[TriggerY.first].z+CenterTy[TriggerX.first].z)/2.
            );
            BMTracks[TriggerX.first][iWt] = TVector3( CenterTs.x , CenterTs.y , CenterTs.z );
            grX->SetPoint(iXYZPT, CenterTs.z, CenterTs.x);
            grY->SetPoint(iXYZPT, CenterTs.z, CenterTs.y);
            // cout<<CenterTs.x<<" "<< CenterTs.y<<" "<< CenterTs.z<<endl;
            iXYZPT++;
            bGoodBMT[iWt] = !(AVGDisTx>8||AVGDisTy>8);
          }
        }
      }
      // cout<<496<<endl;
      // 執行擬合提取斜率 (BMDXDZ, BMDYDZ)
      grX->Fit("pol1", "Q");
      grY->Fit("pol1", "Q");
      
      TF1 *fitX = grX->GetFunction("pol1");
      TF1 *fitY = grY->GetFunction("pol1");
      // cout<<503<<endl;

      if (fitX && fitY) {
        double slopeX = fitX->GetParameter(1);
        double slopeY = fitY->GetParameter(1);
        double intcX  = fitX->GetParameter(0);
        double intcY  = fitY->GetParameter(0);
        // cout<<510<<endl;

        BMDXDZ[iWt] = slopeX;
        BMDYDZ[iWt] = slopeY;
        BMDXDZd[iWt] = TMath::ATan(slopeX) * TMath::RadToDeg();
        BMDYDZd[iWt] = TMath::ATan(slopeY) * TMath::RadToDeg();

        // cout<<516<<endl;
        // 方向向量與角度
        TVector3 dir(slopeX, slopeY, 1.0);
        BMTheta[iWt] = dir.Theta();
        BMPhi[iWt]   = dir.Phi();

        // cout<<522<<endl;
        // 3. 幾何外推 (針對每個檢測器平面)
        // 僅在特定 Weight (例如 iWt == 0) 下計算外推，避免重複
        for (size_t iZ = 0; iZ < NCry; iZ++) {
          int DID = CrystalDets[iZ];
          double targetZ = CrystalZPos[DID]; // 假設有各層的 Z 位置
          CCpdZDCs[DID][iWt] = TVector3(intcX + slopeX * targetZ, intcY + slopeY * targetZ, targetZ);
        }
        // cout<<530<<endl;
      }
    } // end weight loop
    tOut->Fill();
    Edep.clear();
    cADC.clear();
    DetID.clear();
    DiX.clear();
    DiY.clear();
    iX.clear();
    iY.clear();
    nChs.clear();
    pX.clear();
    pY.clear();
    pZ.clear();
  } // end event loop

  
  tOut->Write();
  newFile->Close();
  oldFile->Close();
  
  std::cout << "New File: "<<NemFileNameFull<<" has been created!" << std::endl;
  // system(Form("ln %s %s",NemFileNameFull.data(),NemFileNameGlob.data()));
}

void SortCalData(bool non){
  cout<<"FInished complie of SortCalData"<<endl;
}
void SortCalData(){
  cout<<"FInished complie of SortCalData"<<endl;
}