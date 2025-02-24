#pragma once

#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <map>
#include <TString.h>
/*LYSO 2 CosmicTest*/
/*D0*/#include "./Ex_LYSO2CosTest/GIDMapping_Cosmic_LYSOV2.h"
// /*T1*/#include "./Ex_LYSO2CosTest/GIDMapping_Cosmic_TriggerTop.h"
// /*T2*/#include "./Ex_LYSO2CosTest/GIDMapping_Cosmic_TriggerBottom.h"
/*Exp: Normal setup(beam->T2->T1->LYSO->PWO)*/
/*D1*/#include "./Ex_Normal/GIDMapping_LYSOAPDMap.h"
/*T1*/#include "./Ex_Normal/GIDMapping_Monitor1.h"
/*T2*/#include "./Ex_Normal/GIDMapping_Monitor2.h"
/*D2*/#include "./Ex_Normal/GIDMapping_PWOSIPMMap.h"
/*Exp: PWO first setup(beam->T2->T1->PWO)*/
// /*D2*/#include "./Ex_PWOFirst/GIDMapping_PWOSIPMMap.h"

using namespace std;
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