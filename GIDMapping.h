#pragma once

#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <map>
#include <TString.h>
#include <TH2.h>
/*LYSO 2 CosmicTest*/
/*D0*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_LYSO2CosTest/GIDMapping_Cosmic_LYSOV2.h"
// /*T1*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_LYSO2CosTest/GIDMapping_Cosmic_TriggerTop.h"
// /*T2*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_LYSO2CosTest/GIDMapping_Cosmic_TriggerBottom.h"
/*Exp: Normal setup(beam->T2->T1->LYSO->PWO)*/
/*D1*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_Normal/GIDMapping_LYSOAPDMap.h"
/*T1*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_Normal/GIDMapping_Monitor1.h"
/*T2*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_Normal/GIDMapping_Monitor2.h"
// /*D2*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_Normal/GIDMapping_PWOSIPMMap.h"
/*Exp: PWO first setup(beam->T2->T1->PWO)*/
/*D2*/#include "/data8/ZDC/EMCal/BMAndZDCMapping/Ex_PWOFirst/GIDMapping_PWOSIPMMap.h"

using namespace std;
vector<int> TotDetArr = {0,1,2,3,4};//Sequence of detectors, if no detector: -1
int iDetArr[4] = {1,2,3,4};//Sequence of detectors, if no detector: -1
vector<int> CorDets = {2,3,4};
vector<int> AnaDets = {2,3,4};
vector<int> TrackerDets = {2,3};
vector<int> CrystalDets = {4};
/*y = #Sigma_{i=0}^{size} { p_i*x^{i} }*/
map< int, vector <double> > CrystalGainFactors= {
  /*(Data-771)/(MCx281.72)=1 -> */{4,{-771./281.72, 1./281.72}}
};
map< int, vector <int> > CrystalMainCh= {
  /*(Data-771)/(MCx281.72)=1 -> */{1,{35}},{4,{34,40}}
};
const bool bTrigger[5]={0,0,1,1,0};
const bool bCrystal[5]={1,1,0,0,1};
string SymbolFile[5] = {"D0","D1","T1","T2","D2"};
TString ZImf[5] = {"Z not use","Z=5","Z=3~4","Z=1~2","Z=5"};
map< TString , int > ZDetName = {{"BM 2X",1},{"BM 2Y",2},{"BM 1X",3},{"BM 1Y",4},{"PbWO",5}};
TString DName[5] = {"LYSO","LYSO","BM 1","BM 2","PbWO"};
double ChGapX[5] = {0.72,10.2,2.1,2.1,21.0};
double ChGapY[5] = {0.72,10.2,2.1,2.1,21.0};
TString sChGapX[5] = {"0.72","10.2","2.1","2.1","21.0"};
TString sChGapY[5] = {"0.72","10.2","2.1","2.1","21.0"};
const int MainDID = 4;
const int MainDZ  = 2;
const int DetSeq[5]={-1,-1,1,0,2};
vector<TString> WtNames = {"ADC Wt.","Eq. Wt.","ADC_{Max}"};
vector<TString> sfWtNames = {"ADCW","UNW","ADCM"};


vector<string> ClusNameE = {"E1x1", "E3x3", "E5x5", "E1x2", "E1x3", "E2x3", "E2x1", "E3x1", "E3x2", "EAll"};
vector<string> ClusNameA = {"A1x1", "A3x3", "A5x5", "A1x2", "A1x3", "A2x3", "A2x1", "A3x1", "A3x2", "AAll"};
vector<int> ClusColor = {2, 4, 6, kOrange+7, kViolet-3, kCyan+2, kAzure+1, kPink+1, kTeal-3, 1};


extern const int NumAllChs_D0,NumAllChs_D1,NumAllChs_T1, NumAllChs_T2,NumAllChs_D2;
extern const int NumChs_D0,NumChs_D1,NumChs_T1, NumChs_T2,NumChs_D2;
extern const int NumROCs_D0,NumROCs_D1,NumROCs_T1, NumROCs_T2,NumROCs_D2;
extern const int NumiX_D0,NumiX_D1,NumiX_T1, NumiX_T2,NumiX_D2;
extern const int NumiY_D0,NumiY_D1,NumiY_T1, NumiY_T2,NumiY_D2;
int NumAllChs, NumChs, NumROCs;
int NumIX, NumIY;
extern int proGID_D0[NumAllChs_D0],proGID_D1[NumAllChs_D1],proGID_T1[NumAllChs_T1],proGID_T2[NumAllChs_T2],proGID_D2[NumAllChs_D2];
extern int proiX_D0[NumAllChs_D0],proiX_D1[NumAllChs_D1],proiX_T1[NumAllChs_T1],proiX_T2[NumAllChs_T2],proiX_D2[NumAllChs_D2];
extern int proiY_D0[NumAllChs_D0],proiY_D1[NumAllChs_D1],proiY_T1[NumAllChs_T1],proiY_T2[NumAllChs_T2],proiY_D2[NumAllChs_D2];
extern int proiZ_D0[NumAllChs_D0],proiZ_D1[NumAllChs_D1],proiZ_T1[NumAllChs_T1],proiZ_T2[NumAllChs_T2],proiZ_D2[NumAllChs_D2];
extern float propX_D0[NumAllChs_D0],propX_D1[NumAllChs_D1],propX_T1[NumAllChs_T1],propX_T2[NumAllChs_T2],propX_D2[NumAllChs_D2];
extern float propY_D0[NumAllChs_D0],propY_D1[NumAllChs_D1],propY_T1[NumAllChs_T1],propY_T2[NumAllChs_T2],propY_D2[NumAllChs_D2];
extern float propZ_D0[NumAllChs_D0],propZ_D1[NumAllChs_D1],propZ_T1[NumAllChs_T1],propZ_T2[NumAllChs_T2],propZ_D2[NumAllChs_D2];
class Sensor{//class for 1 sensor imformation
public:
  int   GID, RID, CID, DID;
  int   iX , iY , iZ ;
  float pX , pY , pZ ;
  Sensor(int inGID = -1){
    GID = inGID; RID = -1; CID = -1;
    iX  = 0; iY  = 0; iZ  = 0;
    pX  = 0; pY  = 0; pZ  = 0;
  };
  void SetSensorID(int inGID, int inRID, int inCID){
    GID = inGID; RID = inRID; CID = inCID;
  };
  void SetSensorIXYZ(int iniX, int iniY, int iniZ){
    iX = iniX; iY = iniY; iZ = iniZ;
  };
  void SetSensorPXYZ(float inpX, float inpY, float inpZ){
    pX = inpX; pY = inpY; pZ = inpZ;
  };
  void GetPXYZ(float inpX, float inpY, float inpZ){
    pX = inpX; pY = inpY; pZ = inpZ;
  };
  string Show() const {
    string out = 
      Form(
        "%d , %d , %d , %d , %d , %d , %f , %f , %f;\n",
        GID, RID, CID,  iX , iY , iZ, pX , pY , pZ
      );
    return out;
  }
};


// Load the sensors by the setup imformation into the map
map< int, Sensor > LoadSensorMap(int iDet, bool quiet = true){
  cout<<"Start to load the sensor position vs ID mapping"<<endl;
  map< int, Sensor > sensorMap;
  if(iDet==0){
    for(int ich = 0; ich < NumAllChs_D0 ; ich++ ){
      Sensor TmpSensor(-1);
      TmpSensor.SetSensorID(proGID_D0[ich],proGID_D0[ich]/NumChs_D0,proGID_D0[ich]%NumChs_D0);
      TmpSensor.SetSensorIXYZ(proiX_D0[ich],proiY_D0[ich],proiZ_D0[ich]);
      TmpSensor.SetSensorPXYZ(propX_D0[ich],propY_D0[ich],propZ_D0[ich]);
      sensorMap[TmpSensor.GID] = TmpSensor;
    }
    NumAllChs = NumAllChs_D0;
    NumChs = NumChs_D0;
    NumROCs = NumROCs_D0;
    NumIX = NumiX_D0;
    NumIY = NumiY_D0;
  }
  else if(iDet==1){
    for(int ich = 0; ich < NumAllChs_D1 ; ich++ ){
      Sensor TmpSensor(-1);
      TmpSensor.SetSensorID(proGID_D1[ich],proGID_D1[ich]/NumChs_D1,proGID_D1[ich]%NumChs_D1);
      TmpSensor.SetSensorIXYZ(proiX_D1[ich],proiY_D1[ich],proiZ_D1[ich]);
      TmpSensor.SetSensorPXYZ(propX_D1[ich],propY_D1[ich],propZ_D1[ich]);
      sensorMap[TmpSensor.GID] = TmpSensor;
    }
    NumAllChs = NumAllChs_D1;
    NumROCs = NumROCs_D1;
    NumChs = NumChs_D1;
    NumIX = NumiX_D1;
    NumIY = NumiY_D1;
  }
  else if(iDet==2){
    for(int ich = 0; ich < NumAllChs_T1 ; ich++ ){
      Sensor TmpSensor(-1);
      TmpSensor.SetSensorID(proGID_T1[ich],proGID_T1[ich]/NumChs_T1,proGID_T1[ich]%NumChs_T1);
      TmpSensor.SetSensorIXYZ(proiX_T1[ich],proiY_T1[ich],proiZ_T1[ich]);
      TmpSensor.SetSensorPXYZ(propX_T1[ich],propY_T1[ich],propZ_T1[ich]);
      sensorMap[TmpSensor.GID] = TmpSensor;
    }
    NumAllChs = NumAllChs_T1;
    NumROCs = NumROCs_T1;
    NumChs = NumChs_T1;
    NumIX = NumiX_T1;
    NumIY = NumiY_T1;
  }
  else if(iDet==3){
    for(int ich = 0; ich < NumAllChs_T2 ; ich++ ){
      Sensor TmpSensor(-1);
      TmpSensor.SetSensorID(proGID_T2[ich],proGID_T2[ich]/NumChs_T2,proGID_T2[ich]%NumChs_T2);
      TmpSensor.SetSensorIXYZ(proiX_T2[ich],proiY_T2[ich],proiZ_T2[ich]);
      TmpSensor.SetSensorPXYZ(propX_T2[ich],propY_T2[ich],propZ_T2[ich]);
      sensorMap[TmpSensor.GID] = TmpSensor;
    }
    NumROCs =  NumROCs_T2;
    NumAllChs =  NumAllChs_T2;
    NumChs =  NumChs_T2;
    NumIX = NumiX_T2;
    NumIY = NumiY_T2;
  }
  else if(iDet==4){
    for(int ich = 0; ich < NumAllChs_T2 ; ich++ ){
      Sensor TmpSensor(-1);
      TmpSensor.SetSensorID(proGID_D2[ich],proGID_D2[ich]/NumChs_D2,proGID_D2[ich]%NumChs_D2);
      TmpSensor.SetSensorIXYZ(proiX_D2[ich],proiY_D2[ich],proiZ_D2[ich]);
      TmpSensor.SetSensorPXYZ(propX_D2[ich],propY_D2[ich],propZ_D2[ich]);
      sensorMap[TmpSensor.GID] = TmpSensor;
    }
    NumROCs = NumROCs_D2;
    NumAllChs = NumAllChs_D2;
    NumChs = NumChs_D2;
    NumIX = NumiX_D2;
    NumIY = NumiY_D2;
  }
  else{
    cout<<"Error: GIDMapping couldn't fing the detector index: "<<iDet<<endl;
    throw;
  }
  
  if(!quiet){
    cout<<"Shows the mapping :"<<endl;
    cout<<"GID, RID, CID,  iX , iY , iZ, pX , pY , pZ"<<endl;
    for (std::map<int, Sensor>::const_iterator it = sensorMap.begin(); it != sensorMap.end(); ++it) {
      it->second.Show(); // 訪問並執行每個 Sensor 的 show()
    }
  }
  cout<<"Finish to load the sensor position vs ID mapping"<<endl;
  return sensorMap;
}

map<int , double> LoadCrystalZMap(){
  map<int , double> CrystalZPos;
  for (int iZ = 0; iZ < int(CrystalDets.size()); iZ++) {
    map< int, Sensor > sensorMap = LoadSensorMap(CrystalDets[iZ]);
    CrystalZPos[CrystalDets[iZ]] = propZ_D1[1];
  }
  return CrystalZPos;
};
TH2F* CreateSensorGrid(
  std::map<int, Sensor>& sensorMap,
   const char* hName = "hSensorGrid", 
   const char* hTitle = ";X position [mm];Y position [mm]",
   bool bFill = true, int numIX = NumIX , int numIY = NumIY
){

    if (sensorMap.empty()) {
        std::cerr << "Error: Sensor map is empty!" << std::endl;
        return nullptr;
    }

    // 1. 找出物理座標的極值
    float minX = 1e9, maxX = -1e9;
    float minY = 1e9, maxY = -1e9;

    for (auto const& [gid, sensor] : sensorMap) {
        if (sensor.pX < minX) minX = sensor.pX;
        if (sensor.pX > maxX) maxX = sensor.pX;
        if (sensor.pY < minY) minY = sensor.pY;
        if (sensor.pY > maxY) maxY = sensor.pY;
    }

    // 2. 計算晶體間距 (Pitch)
    // 假設晶體是均勻分佈的矩形網格
    float dx = (numIX > 1) ? (maxX - minX) / (numIX - 1) : 20.0; // 預設 20mm
    float dy = (numIY > 1) ? (maxY - minY) / (numIY - 1) : 20.0;

    // 3. 設定直方圖的邊界 (Edges)
    // 邊界應為 中心點 +/- 半個間距
    float xLow  = minX - dx/2.0;
    float xHigh = maxX + dx/2.0;
    float yLow  = minY - dy/2.0;
    float yHigh = maxY + dy/2.0;

    // 4. 建立 TH2F
    // 使用物理座標作為軸，Bin 數量對應晶體數量
    TH2F *h2 = new TH2F(hName, hTitle, numIX, xLow, xHigh, numIY, yLow, yHigh);
    if(bFill){
      for(int iXtmp = 1; iXtmp<=numIX;iXtmp++)
        for(int iYtmp = 1; iYtmp<=numIY;iYtmp++)
          h2->SetBinContent(iXtmp,iYtmp,1);
      h2->SetLineColor(2);
      h2->SetLineWidth(2);
      h2->GetZaxis()->SetRangeUser(0,1);
    }
    // 5. 可選：驗證網格是否正確對齊
    // 我們可以填入 GID 或 iX 資訊來檢查
    /*
    for (auto const& [gid, sensor] : sensorMap) {
        h2->Fill(sensor.pX, sensor.pY, gid); 
    }
    */

    std::cout << "TH2F Grid Created: " << hName << std::endl;
    std::cout << "X Range: [" << xLow << ", " << xHigh << "] (Bins: " << numIX << ")" << std::endl;
    std::cout << "Y Range: [" << yLow << ", " << yHigh << "] (Bins: " << numIY << ")" << std::endl;

    return h2;
}