#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <tuple>
#if defined (__MAKECINT__) 
#pragma link C++ class vector<Long64_t>+; 
#endif
#include <chrono>  // For timing

using namespace std;
const std::string fileMark[5] = {"D0", "D1", "T1", "T2", "D2"};
const int DetSeq[5]={-1,2,1,0,3};
// const bool bTrigger[5]={0,0,1,1,0};
// const int MainDID = 4;
// const int MainDZ  = 1;
const double TestRate = 1;
const bool bTrigger[5]={0,0,1,1,0};
const int MainDID = 4;
const int MainDZ  = 3;
Long64_t  threshold = 1;
struct Event {
  vector<Long64_t> eNtryID;
  vector<Int_t> DID, pcnt, fcnt, ROCID;
};

bool checkFileExistence(const char* filename) {
    TFile* file = TFile::Open(filename, "READ");
    if (file && file->IsOpen()) {
        file->Close();
        return true;
    }
    return false;
}

// 排序函數，根據 iZEV 分組，然後按 GADCEV 排序，最後更新排名至 iHitEV
int SortAndRankByGADCEVandIZ(
  std::vector<int> &iHitEV,
  std::vector<int> &ROCIDEV,
  std::vector<int> &channelEV,
  std::vector<int> &DIDEV,
  std::vector<int> &FireEV,
  std::vector<bool> &HGModeEV,
  std::vector<int> &ADCEV,
  std::vector<int> &GADCEV,
  std::vector<int> &GIDEV,
  std::vector<int> &iXEV,
  std::vector<int> &iYEV,
  std::vector<int> &iZEV,
  std::vector<float> &pXEV,
  std::vector<float> &pYEV,
  std::vector<float> &pZEV
){
  // 確保所有向量長度一致
  size_t n = iZEV.size();
  if (n == 0) return -1;
  
  // 建立索引
  std::vector<size_t> indices(n);
  for (size_t i = 0; i < n; ++i) {
      indices[i] = i;
  }

  // 雙重排序：先按 iZEV 升序，再按 GADCEV 升序
  std::sort(indices.begin(), indices.end(), [&iZEV, &GADCEV](size_t a, size_t b) {
      if (iZEV[a] == iZEV[b]) {
          return GADCEV[a] > GADCEV[b];
      }
      return iZEV[a] < iZEV[b];
  });

  // 按排序後的索引重排所有向量
  auto reorder = [&indices](auto &vec) {
      std::vector<typename std::decay<decltype(vec)>::type::value_type> temp(vec.size());
      for (size_t i = 0; i < indices.size(); ++i) {
          temp[i] = vec[indices[i]];
      }
      vec = std::move(temp);
  };

  reorder(iHitEV);
  reorder(ROCIDEV);
  reorder(channelEV);
  reorder(DIDEV);
  reorder(FireEV);
  reorder(HGModeEV);
  reorder(ADCEV);
  reorder(GADCEV);
  reorder(GIDEV);
  reorder(iXEV);
  reorder(iYEV);
  reorder(iZEV);
  reorder(pXEV);
  reorder(pYEV);
  reorder(pZEV);

  // 計算排名：更新 iHitEV 為分組內的排名
  int currentRank = 0, peakCry = 0;
  for (size_t i = 0; i < n; ++i) {
    if (i > 0 && iZEV[i] != iZEV[i - 1]) {
      // 當 iZEV 改變時，重置排名
      currentRank = 0;
      if(DIDEV[i] == MainDID) peakCry = i;
    }
    iHitEV[i] = currentRank++;
  }
  return peakCry;
}

void processFiles(
  const std::string dirAnaPath,
  const std::string FileName,
  const bool quiet
){
  // File names
  std::vector<TFile*> files;
  std::vector<TTree*> trees;
  int totalFiles = 0;
  // Open files and check if they exist
  size_t NTrees = 0;
  bool ACTreep[5]={0};
  for (int i = 0; i < 5; i++) {
      std::string FilePathName = dirAnaPath + fileMark[i] + "_Sci.root";
      if (!checkFileExistence(FilePathName.data())) {
          std::cerr << "File " << FilePathName << " does not exist!" << std::endl;
          trees.push_back(NULL);
          continue;
      }
      TFile* file = TFile::Open(FilePathName.data(), "READ");
      files.push_back(file);
      TTree* tree = (TTree*)file->Get("t");
      trees.push_back(tree);
      NTrees++;
      ACTreep[i] = true;
  }
  if(NTrees == 0){
      std::cerr << "No tree could be reconstructed" << std::endl;
      throw;
  }

  // Variables to hold branches data
  Int_t pcnt, fcnt, DID, nHits = 0;
  // Long64_t eventID;
  // Use arrays to store branch data
  vector<Int_t> *ROCID=0;

  // Set branch addresses to pointers
  for (size_t i = 0; i < 5; ++i) {
    if(ACTreep[i]){
      // trees[i]->SetBranchAddress("eventID", &eventID);
      trees[i]->SetBranchAddress("pcnt", &pcnt);
      trees[i]->SetBranchAddress("fcnt", &fcnt);
      trees[i]->SetBranchAddress("DID", &DID);
      trees[i]->SetBranchAddress("nHits", &nHits);
      trees[i]->SetBranchAddress("ROCID", &ROCID);
    }
  }

  // Loop over the events and pair based on time differences
  map<Long64_t, Event> Events;
  for (size_t i = 0; i < 5; ++i) cout<<(ACTreep[i])<<" "<<dirAnaPath + fileMark[i] + "_Sci.root"<<endl;
  for (size_t i = 0; i < 5; ++i) {
    if(!ACTreep[i]) continue;
    Long64_t EVS = trees[i]->GetEntries();
    for (Long64_t iev = 0; iev < EVS ; ++iev) {
      if(iev%100000==0&&!quiet)
        cout<<"\rDet: "<<i<<"\t"<<(iev*100.)/(1.*EVS)<<"%"<<flush;
      // Read events from each file
      trees[i]->GetEntry(iev);
      Event currentEvent;
      if(nHits<= 0) continue;
      currentEvent.eNtryID.push_back( iev );
      currentEvent.DID.push_back( DID );
      currentEvent.pcnt.push_back( pcnt );
      currentEvent.fcnt.push_back( fcnt );
      currentEvent.ROCID.push_back(ROCID->at(0));
      
      // Merge the event
      Long64_t key = pcnt*pow(2,19)+fcnt;
      // cout<<iev<<"/"<<EVS<<" "<<key<<endl;
      if (Events.find(key) != Events.end()&&(iev!=0)){
        Events[key].eNtryID.push_back( iev );
        Events[key].DID.push_back( DID );
        Events[key].pcnt.push_back( pcnt );
        Events[key].fcnt.push_back( fcnt );
        Events[key].ROCID.push_back(ROCID->at(0));
        // cout<<iev<<" "<<DID<<" "<<pcnt<<" "<<fcnt<<" "<<ROCID->at(0)<<endl;
        size_t itot = Events[key].DID.size()-1;
        // cout<<Events[key].eventID[itot]<<" "<<Events[key].DID[itot]<<" "<<Events[key].pcnt[itot]<<" "<<Events[key].fcnt[itot]<<" "<<Events[key].ROCID[itot]<<endl;
      }else{
        Events[key] = currentEvent;
      }
      // cout<<" 193"<<endl;

    }
    // ofSave<<endl<<"Det: "<<i<<"  is finished!"<<endl;
  }
  // vector<double> rmEv;
  vector<vector<Long64_t>> groupedEIDs;
  vector<vector<int>> groupedDIDs,groupedRIDs;
  vector<vector<Long64_t>> groupedkeys;
  // 迴圈遍歷 map
  for (map<Long64_t, Event>::const_iterator it = Events.begin(); it != Events.end(); ++it) {
    vector<Long64_t> currentGroupEID;
    vector<int> currentGroupDID, currentGroupRID;
    vector<Long64_t> currentGroupKey;

    // 存儲當前 key 的資料
    for(size_t i = 0; i < it->second.ROCID.size(); i++) {
      currentGroupEID.push_back(it->second.eNtryID[i]);
      currentGroupDID.push_back(it->second.DID[i]);
      currentGroupRID.push_back(it->second.ROCID[i]);
      currentGroupKey.push_back(it->first);
      // cout<<it->second.DID[i]<<"\t"<<it->second.eventID[i]<<" ";
    }
    // cout<<endl;

    // 檢查下一個 key
    while (it != std::prev(Events.end())) {
      vector<int> GROCID;
      for (size_t i = 0; i < it->second.ROCID.size(); i++) {
        GROCID.push_back(it->second.ROCID[i] + it->second.DID[i] * 1000);
      }

      auto nextIt = std::next(it);

      if (std::abs(nextIt->first - it->first) < threshold) {
        bool nextIsDuplicate = false;

        for (size_t j = 0; j < nextIt->second.ROCID.size() && !nextIsDuplicate; j++) {
          for (size_t i = 0; i < GROCID.size(); i++) {
            if (nextIt->second.ROCID[j] + nextIt->second.DID[j] * 1000 == GROCID[i]) {
              nextIsDuplicate = true;
              break;
            }
          }
        }

        if (!nextIsDuplicate) {
          for (size_t j = 0; j < nextIt->second.ROCID.size(); j++) {
            if (j >= nextIt->second.eNtryID.size() || j >= nextIt->second.DID.size() || j >= nextIt->second.ROCID.size()) {
              // 安全性檢查，避免越界
              continue;
            }

            currentGroupEID.push_back(nextIt->second.eNtryID[j]);
            currentGroupDID.push_back(nextIt->second.DID[j]);
            currentGroupRID.push_back(nextIt->second.ROCID[j]);
            currentGroupKey.push_back(nextIt->first);
          }

          if (it != std::prev(Events.end())) {
            it++;
            continue;
          }
        }
      }
      break;
    }

    // 存入結果
    if (currentGroupEID.size() >= 4) {
      groupedEIDs.push_back(currentGroupEID);
      groupedDIDs.push_back(currentGroupDID);
      groupedRIDs.push_back(currentGroupRID);
      groupedkeys.push_back(currentGroupKey);
    }
  }
  
  
  
  Long64_t eventID;
  int SeqSAD, UTCDay, UTCHour, UTCMin, UTCSec, UTCSecS, dfcnt, HighVROCA, HighVROCB;
  int SW2VROCA, SW2VROCB, VFROCA, VFROCB;
  float Edep, BeamPosX, BeamPosY, BeamPosZ, BeamEnergy, TempEnv, LiveTimeA, LiveTimeB, BufferLTA, BufferLTB, RotateAng;
  std::vector<int> *iHit = nullptr, *Fire = nullptr, *ROCIDCh = nullptr, *channel = nullptr, *ADC = nullptr, *GADC = nullptr;
  std::vector<int> *GID = nullptr, *iX = nullptr, *iY = nullptr, *iZ = nullptr;
  std::vector<float> *pX = nullptr, *pY = nullptr, *pZ = nullptr;
  std::vector<bool> *HGTMID = nullptr, *HGMode = nullptr;
  
  for (size_t i = 0; i < 5; ++i) {
    if(ACTreep[i]){
      trees[i]->SetBranchAddress("eventID", &eventID);
      trees[i]->SetBranchAddress("SeqSAD", &SeqSAD);
      trees[i]->SetBranchAddress("UTCDay", &UTCDay);
      trees[i]->SetBranchAddress("UTCHour", &UTCHour);
      trees[i]->SetBranchAddress("UTCMin", &UTCMin);
      trees[i]->SetBranchAddress("UTCSec", &UTCSec);
      trees[i]->SetBranchAddress("UTCSecS", &UTCSecS);
      trees[i]->SetBranchAddress("pcnt", &pcnt);
      trees[i]->SetBranchAddress("fcnt", &fcnt);
      trees[i]->SetBranchAddress("dfcnt", &dfcnt);
      trees[i]->SetBranchAddress("nHits", &nHits);
      trees[i]->SetBranchAddress("Edep", &Edep);
      trees[i]->SetBranchAddress("iHit", &iHit);
      trees[i]->SetBranchAddress("Fire", &Fire);
      trees[i]->SetBranchAddress("HGTMID", &HGTMID);
      trees[i]->SetBranchAddress("DID", &DID);
      trees[i]->SetBranchAddress("ROCID", &ROCIDCh);
      trees[i]->SetBranchAddress("channel", &channel);
      trees[i]->SetBranchAddress("HGMode", &HGMode);
      trees[i]->SetBranchAddress("ADC", &ADC);
      trees[i]->SetBranchAddress("GADC", &GADC);
      trees[i]->SetBranchAddress("GID", &GID);
      trees[i]->SetBranchAddress("iX", &iX);
      trees[i]->SetBranchAddress("iY", &iY);
      trees[i]->SetBranchAddress("iZ", &iZ);
      trees[i]->SetBranchAddress("pX", &pX);
      trees[i]->SetBranchAddress("pY", &pY);
      trees[i]->SetBranchAddress("pZ", &pZ);
      trees[i]->SetBranchAddress("BeamPosX", &BeamPosX);
      trees[i]->SetBranchAddress("BeamPosY", &BeamPosY);
      trees[i]->SetBranchAddress("BeamPosZ", &BeamPosZ);
      trees[i]->SetBranchAddress("BeamEnergy", &BeamEnergy);
      trees[i]->SetBranchAddress("HighVROCA", &HighVROCA);
      trees[i]->SetBranchAddress("HighVROCB", &HighVROCB);
      trees[i]->SetBranchAddress("SW2VROCA", &SW2VROCA);
      trees[i]->SetBranchAddress("SW2VROCB", &SW2VROCB);
      trees[i]->SetBranchAddress("VFROCA", &VFROCA);
      trees[i]->SetBranchAddress("VFROCB", &VFROCB);
      trees[i]->SetBranchAddress("TempEnv", &TempEnv);
      trees[i]->SetBranchAddress("LiveTimeA", &LiveTimeA);
      trees[i]->SetBranchAddress("LiveTimeB", &LiveTimeB);
      trees[i]->SetBranchAddress("BufferLTA", &BufferLTA);
      trees[i]->SetBranchAddress("BufferLTB", &BufferLTB);
      trees[i]->SetBranchAddress("RotateAng", &RotateAng);
        

    }
  }
  Long64_t eID = 0;
  Int_t nROC[4]={0}; 
  int pcntEV, fcntEV, dfcntEV, nHitsEV[4]={0},nHitsAL= 0;
  vector<int>   ROCIDEV, channelEV, DIDEV, FireEV, iHitEV;
  vector<int>   ADCEV, GADCEV, GIDEV, iXEV, iYEV, iZEV;
  vector<float>  pXEV, pYEV, pZEV;
  vector<bool>  HGModeEV;
  double GADC11, GADC33, GADCXs, GADC55, GADCAL;
  float CenpX, CenpY; 
  int    Cortex; 
  int PeakID = -1;
  TFile *fSave = new TFile((dirAnaPath+FileName+"_ReCon.root").data(),"recreate");
  TTree *tSave = new TTree("t","Tree of all detector data");
  // tSave->SetAutoFlush(100000);

  tSave->Branch("eventID", &eID);
  tSave->Branch("pcnt", &pcntEV);
  tSave->Branch("fcnt", &fcntEV);
  tSave->Branch(Form("nHits"), &nHitsAL);
  for(int i=0;i<4;i++){
    tSave->Branch(Form("nHits%d",i), &nHitsEV[i]);
    tSave->Branch(Form("nROC%d",i), &nROC[i]);
  }
  tSave->Branch("iHit", &iHitEV);
  tSave->Branch("DID", &DIDEV);
  tSave->Branch("ROCID", &ROCIDEV);
  tSave->Branch("channel", &channelEV);
  tSave->Branch("Fire", &FireEV);
  tSave->Branch("HGMode", &HGModeEV);
  tSave->Branch("ADC", &ADCEV);
  tSave->Branch("dfcnt", &dfcntEV);
  // tSave->Branch("SeqSAD", &SeqSAD);
  // tSave->Branch("UTCDay", &UTCDay);
  // tSave->Branch("UTCHour", &UTCHour);
  // tSave->Branch("UTCMin", &UTCMin);
  // tSave->Branch("UTCSec", &UTCSec);
  // tSave->Branch("UTCSecS", &UTCSecS);
  // tSave->Branch("HGTMID", &HGTMID);
  tSave->Branch("GADC", &GADCEV);
  tSave->Branch("GID", &GIDEV);
  tSave->Branch("iX", &iXEV);
  tSave->Branch("iY", &iYEV);
  tSave->Branch("iZ", &iZEV);
  tSave->Branch("pX", &pXEV);
  tSave->Branch("pY", &pYEV);
  tSave->Branch("pZ", &pZEV);
  tSave->Branch("GADC11",&GADC11);
  tSave->Branch("GADC33",&GADC33);
  tSave->Branch("GADC55",&GADC55);
  tSave->Branch("GADCAL",&GADCAL);
  tSave->Branch("GADCXs",&GADCXs);
  tSave->Branch("PeakID",&PeakID);
  tSave->Branch("CenpX",&CenpX);
  tSave->Branch("CenpY",&CenpY);
  tSave->Branch("Cortex",&Cortex);
  tSave->Branch("BeamPosX", &BeamPosX);
  tSave->Branch("BeamPosY", &BeamPosY);
  tSave->Branch("BeamPosZ", &BeamPosZ);
  tSave->Branch("BeamEnergy", &BeamEnergy);
  tSave->Branch("HighVROCA", &HighVROCA);
  tSave->Branch("HighVROCB", &HighVROCB);
  tSave->Branch("SW2VROCA", &SW2VROCA);
  tSave->Branch("SW2VROCB", &SW2VROCB);
  tSave->Branch("VFROCA", &VFROCA);
  tSave->Branch("VFROCB", &VFROCB);
  tSave->Branch("TempEnv", &TempEnv);
  tSave->Branch("LiveTimeA", &LiveTimeA);
  tSave->Branch("LiveTimeB", &LiveTimeB);
  tSave->Branch("BufferLTA", &BufferLTA);
  tSave->Branch("BufferLTB", &BufferLTB);
  tSave->Branch("RotateAng", &RotateAng);
  
  const size_t gEVS = groupedEIDs.size();
  // ofSave<<gEVS<<endl;
  // throw;
  ofstream ofs(Form("%sLossCases.txt",dirAnaPath.data()));
  ofstream ofSave(Form("%sSaveCases.txt",dirAnaPath.data()));
  ofstream ofEFF(Form("%sEffCal.dat",dirAnaPath.data()));
  ofSave<<"SL"<<endl;
  int lossCnt = 0, TotalCnt = 0, GoodEvCnt = 0;
  for (size_t i=0;i<gEVS*TestRate;i++) {
  // for (size_t i=0;i<10;i++) {
    // auto start = std::chrono::high_resolution_clock::now();
    if(i%1000==0&&!quiet)
      cout<<"\rDet: "<<i<<"\t"<<(i*100.)/(1.*gEVS)<<"%"<<flush;
    size_t layerSize = groupedEIDs[i].size();
    if(layerSize<4 )continue;
    int passTrigger = 0;
    int passZDC = 0;
    for(size_t k = 0; k< layerSize; k++){
      if(bTrigger[groupedDIDs[i][k]]) passTrigger++;
      else passZDC++;
    }
    if(passTrigger<4) continue;
    TotalCnt++;
    if(passZDC<1){
      ofs<<"lossCnt: "<<++lossCnt<<" MD ZDC not catch, GieV= "<<i <<endl;
      // ofEFF<<eID<<"\t"<<0<<endl;
      continue;
    }
    // cout<<eID<<"\t"<<1<<endl;
    // ofEFF<<eID<<"\t"<<1<<endl;
    ofSave<<"--------------\nGieV = "<<i<<" / evs: "<<gEVS<<endl;
    // auto point1 = std::chrono::high_resolution_clock::now();
    std::vector<std::chrono::high_resolution_clock::time_point> time_points;
    
    for(size_t k = 0; k< layerSize; k++){
      int EID = groupedEIDs[i][k];
      DID = groupedDIDs[i][k];
      int RID = groupedRIDs[i][k];
      trees[DID]->GetEntry(EID);
      ofSave<<"Eid= "<<eventID<<"\t"<<"Did= "<<DID<<"\t"<<"Rid= "<<RID<<"\t";
      ofSave<<"nHits= "<<nHits<<endl;
      int Z = DetSeq[DID];
      nROC[Z]++;
      time_points.push_back(std::chrono::high_resolution_clock::now());
      ofSave<<Form("key= %lld\t%d\t%d\n",groupedkeys[i][k],pcnt,fcnt);
      if(k==0){
        pcntEV = pcnt;
        fcntEV = fcnt;
      }
      time_points.push_back(std::chrono::high_resolution_clock::now());
      for(int iH=0;iH<nHits;iH++){
        // if( (MainDID == DID)&& (GADC->at(iH)<=0) ){
        if( (MainDID == DID)&& (ADC->at(iH)+250<=0) ){
          ofSave<<"Erase: D/R: ADC "<<DID<<"/"<<ROCIDCh->at(iH)<<" : "<<GADC->at(iH)<<"<0"<<endl;
          continue;
        }
        nHitsEV[Z]++;
        nHitsAL++;
        iHitEV.push_back(iH);
        ROCIDEV.push_back(ROCIDCh->at(iH));
        channelEV.push_back(channel->at(iH));
        DIDEV.push_back(DID);
        FireEV.push_back(Fire->at(iH));
        HGModeEV.push_back(HGMode->at(iH));
        ADCEV.push_back(ADC->at(iH)+250);
        GADCEV.push_back(HGMode->at(iH)==1 ? (ADC->at(iH)+250) : (ADC->at(iH)+250)*10);
        GIDEV.push_back(GID->at(iH));
        iXEV.push_back(iX->at(iH));
        iYEV.push_back(iY->at(iH));
        // cout<<476<<"  "<<iX->at(iH)<<"\t"<<iY->at(iH)<<endl;
        // cout<<477<<"  "<<iXEV[iXEV.size()-1]<<"\t"<<iYEV[iYEV.size()-1]<<endl;
        // cout<<iX->at(iH)<<" "<<iY->at(iH)<<endl;
        iZEV.push_back(iZ->at(iH));
        pXEV.push_back(pX->at(iH));
        pYEV.push_back(pY->at(iH));
        pZEV.push_back(pZ->at(iH));
        
      }
      time_points.push_back(std::chrono::high_resolution_clock::now());

      // cout<<"ROC: "<<ROCID->at(0) <<"  "<<pcnt<<" , "<<fcnt*0.24E-6<<"  "<<"nH= "<<nHits<<endl;
      // auto point2 = std::chrono::high_resolution_clock::now();

    }
    
    GoodEvCnt++;
    // auto point3 = std::chrono::high_resolution_clock::now();
    ofSave<<endl;
    if(nHitsEV[MainDZ] != 0){
      PeakID = SortAndRankByGADCEVandIZ(iHitEV, ROCIDEV, channelEV, DIDEV, FireEV, HGModeEV, ADCEV, GADCEV, GIDEV, iXEV, iYEV, iZEV, pXEV, pYEV, pZEV);
    }else{
      PeakID = -1;
      ofs<<"lossCnt: "<<++lossCnt<<" MD ZDC empty, GieV= "<<i <<endl;
    }
    // cout<<498<<endl;
    if(PeakID==-1){
      // cout<<500<<endl;
      ofSave<<"PeakID == -1"<<endl;
      // for(size_t ihits=0;ihits<channelEV.size();ihits++){
        // ofSave<<iHitEV[ihits]<<" "<<DIDEV[ihits]<<" "<<ROCIDEV[ihits]<<" "<<GIDEV[ihits]<<" "<<FireEV[ihits]<<" "<<ADCEV[ihits]<<" "<<GADCEV[ihits]<<endl;
      // }
        
    }else{
      // cout<<503<<endl;
      GADC11 = GADCEV[PeakID],GADC33 = 0,GADCXs = 0,GADC55 = 0,GADCAL = 0;
      CenpX = 0; CenpY = 0;

      int PeakiX = iXEV[PeakID];
      int PeakiY = iYEV[PeakID];
      // cout<<"origin: "<<PeakiX<<" "<<PeakiY<<" "<<PeakID<<endl;
      for(int iH=PeakID;iH<PeakID+nHitsEV[MainDZ];iH++){
        // cout<<iXEV[iH]<<" "<<iYEV[iH]<<endl;
        // CenpX += Esav->pX->at(i)*Esav->GADC->at(i);
        // CenpY += Esav->pY->at(i)*Esav->GADC->at(i);
        int TmpiX = iXEV[iH];
        int TmpiY = iYEV[iH];
        // cout<<"tmpos: "<<TmpiX<<" "<<TmpiY<<endl;
        int GADCTMP = GADCEV[iH];
        GADCAL+=GADCTMP;
        if(abs(PeakiX-TmpiX)<3&&abs(PeakiY-TmpiY)<3){
          GADC55 += GADCTMP;
          if(abs(PeakiX-TmpiX)<2&&abs(PeakiY-TmpiY)<2){
            GADC33 += GADCTMP;
            if((abs(PeakiX-TmpiX) +abs(PeakiY-TmpiY))<2)
              GADCXs += GADCTMP;
          }
          CenpX += pXEV[iH]*GADCTMP;
          CenpY += pYEV[iH]*GADCTMP;
        }
      }
      // cout<<GADC33<<" "<<GADC55<<endl;
      CenpX /= GADC55;
      CenpY /= GADC55;
      Cortex = abs(PeakiX-3.5)>abs(PeakiY-3.5)
          ? abs(PeakiX-3.5)-0.5 
          : abs(PeakiY-3.5)-0.5;
          
      dfcntEV = (pcnt-pcntEV)*0.05+(fcnt-fcntEV)*0.24E-6;
      tSave->Fill();
      // tSave->FlushBaskets();
      // tSave->AutoSave("SaveSelf"); 
      ofSave<<eID<<","<<nHits<<endl;
      eID++;
      
    }
    // auto point4 = std::chrono::high_resolution_clock::now();

    nHitsAL = 0;
    for(int iLay=0;iLay<4;iLay++) nHitsEV[iLay] = 0;
    for(int iLay=0;iLay<4;iLay++) nROC[iLay] = 0;
    iHitEV.clear();
    ROCIDEV.clear();
    channelEV.clear();
    DIDEV.clear();
    FireEV.clear();
    HGModeEV.clear();
    ADCEV.clear();
    GADCEV.clear();
    GIDEV.clear();
    iXEV.clear();
    iYEV.clear();
    iZEV.clear();
    pXEV.clear();
    pYEV.clear();
    pZEV.clear();
    // auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(point1 - start).count();
    // auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(point3 - point1).count();
    // auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(point4 - point3).count();
    // auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(point4 - start).count();
    // cout<<total_duration <<": "<<duration1<<"+"<<duration2<<"+"<<duration4<<endl;
        // cout<<"i= "<<0<<" : "<<std::chrono::duration_cast<std::chrono::milliseconds>(
                            // time_points[0] - point1).count()<<endl;
    // for (size_t i = 1; i < time_points.size(); ++i) {
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                            // time_points[i] - time_points[i - 1])
                            // .count();
        // cout<<"i= "<<i<<" : "<<duration<<endl;
    // }
    // throw;
  }
  ofEFF<<GoodEvCnt<<"\t"<<TotalCnt<<endl;
  cout<<"Eff: Fine/Total"<<GoodEvCnt/TotalCnt*100<<"%\t"<<GoodEvCnt<<"\t"<<TotalCnt<<endl;
  tSave->Write();
  // fSave->Write();
  cout<<"Before erase: "<<Events.size()<<endl;
  // for(size_t i = 0;i<rmEv.size();i++) Events.erase(rmEv[i]);
  cout<<"After erase: "<<eID<<endl;
  ofs.close();
  // Close files
  for (TFile* file : files) {
      file->Close();
  }
  fSave->Close();
  string fsaveName = (dirAnaPath+FileName+"_ReCon.root");
  string fsaveNameSet = dirAnaPath+FileName+string(Form("_ReCon_MDID%d.root",MainDID));
  system(Form("cp %s %s",fsaveName.data(),fsaveNameSet.data()));
}

int ReConstruct(
  const std::string dirAnaPath,
  const std::string FileName,
  const bool quiet = false
) {
    processFiles(dirAnaPath, FileName,quiet);
    return 0;
}

void ReConstruct() {
  cout<<"Finished compiling of ReConstruct.C+"<<endl;

}
