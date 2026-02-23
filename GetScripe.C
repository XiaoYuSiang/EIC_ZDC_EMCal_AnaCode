#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
#include <sstream>
#include <string>
#include "GetScripeFunctions.h"
/*
root -b -q GetScripe.C
*/
using namespace std;
const int cpumax = 60;
const double Memmax = 150.e+9;//Byte
// const string Opmode = "3";
const string Opmode = "1";
const int iSaveAtWhere = 0;
// 0 Class be run-time in VSDirs, 1 Class be run-time in VDirs
const double FileTimeStampGap = 1800; //(s)
// 1 = 0b00001, 3 = 0b00011, 7 = 0b00111, 15 = 0b01111, 31 = 0b11111
// const string BashFile = "TestRuns.sh";
const string BashFile = "FileListEScanTypicalRuns.sh";
const string OutputPath = "./Save/";
// const string OutputPath = "/data8/ZDC/EMCal/BTDataStreamTest/Save/";
const string Cases[4] = {"PbWO4","Monitor1","Monitor2","LYSO"};
const int NumDirs =1;
const string VDirs[NumDirs]={
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250127/*",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/*",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/*",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run205*",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run20[7,8,9]*",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run21[0,1]*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0219/Run838*",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0220/Run2041*"
  // "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/TestSize/Run2013*"
  "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run2013*"
  // "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run20{45,46,47,48}*"
  // "/data8/ZDC/EMCal/BTDataStreamTest/Simulated_Run005*"
  // "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25PWOOnly/Run20{13,18,35,21,24,38,41,27,32}*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0220/Run20{8[8-9],9[0-6]}*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*"
  // "/data8/ZDC/EMCal/BeamTest/20250215/BeamMoniter/20250216/Run118*_07*"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Co60_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Cs137_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/BG_HV16_FV_290_320_LG_x10*"
};
//#RunID Energy
// 2013,	796
// 2018,	739
// 2035,	706
// 2021,	584
// 2024,	492
// 2038,	395
// 2041,	297
// 2027,	197
// 2032,	98
// 2045,	47
//  存檔的根目錄
const string VSDirs[NumDirs]={
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4003/",
  // "/data8/ZDC/EMCal/BTDataStreamTest/",
  "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4005/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4006/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4007/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4008/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4009/",
  // "/data8/ZDC/EMCal/BothZDCABM/AnaCode2/CosTest/Run4010/"
};

// 4/10/20/60
string VNMD1[NumDirs]={
  // "auto",
  "auto"
  // ""
  // "/data8/ZDC/EMCal/PbWO4SiPM/250206/*PbWO4",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250127/*LYSO",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*LYSO"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/*LYSO",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/*LYSO",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080*LYSO"
  // "/data8/ZDC/EMCal/BeamTest/20250216Run118/Run118*_07*_08*LYSO",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run205*LYSO",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run20[7,8,9]*LYSO",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run21[0,1]*LYSO"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run2*LYSO"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*LYSO"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Co60_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Cs137_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/BG_HV16_FV_290_320_LG_x10*"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250127/*"
};
string VNT1[NumDirs]={
  // "auto",
  "auto"
  // ""
  // "",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250127/*Trigger1",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*Trigger1"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/*Monitor1",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/*Monitor1",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080*Monitor1"
  // "/data8/ZDC/EMCal/BeamTest/20250216Run118/Run118*_07*_08*Monitor1",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run205*Monitor1",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run20[7,8,9]*Monitor1",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run21[0,1]*Monitor1"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run2*Monitor1"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*Monitor1"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*Trigger1"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250127/*"
};
string VNT2[NumDirs]={
  // "auto",
  "auto"
  // ""
  // "",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250127/*Trigger2",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*Trigger2"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/*Monitor2",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/*Monitor2",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080*Monitor2"
  // "/data8/ZDC/EMCal/BeamTest/20250216Run118/Run118*_07*_08*Monitor2",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run205*Monitor2",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run20[7,8,9]*Monitor2",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run21[0,1]*Monitor2"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run2*Monitor2"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*Monitor2"
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/250128/*Trigger2"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250127/*"
};
string VNMD2[NumDirs]={
  // "" ,
  ""  
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25020708/*PbWO4",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/2502080910/*PbWO4",
  // "/data8/ZDC/EMCal/LYSOPWOBMCosmic/25021112/Run080*PbWO4"
  // "/data8/ZDC/EMCal/BeamTest/20250216Run118/Run118*_07*_08*PbWO4",
  // "/data8/ZDC/EMCal/BeamTest/20250216Run118/Run205*PbWO4"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run205*PbWO4",
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run20*PbWO4"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250127/*"
};
void GetScripe(){
  system(Form("mkdir -p %s",OutputPath.data()));
  
  ofstream outbash((OutputPath+BashFile).data());
  // outbash<<"root -b -q -l CompilarV2.C"<<endl;
  // outbash<<"wait"<<endl;
  int ifiles = 0, icpu = 0;
  bool AutoStart[4] = {false,false,false,false};
  for(int i=0;i<NumDirs;i++){
    vector<string> VNs={VNMD1[i], VNT1 [i], VNT2 [i], VNMD2[i]};
    for(int iCase = 0;iCase<4;iCase++){
      string strNewtmp = VNs[iCase];
      if(VNs[iCase]=="auto") AutoStart[iCase] = true;
      else if(VNs[iCase]==""){
        if(AutoStart[iCase]==true) strNewtmp = "auto";
        else strNewtmp = "";
      }
      VNs[iCase] = SetPathAuto(strNewtmp,Cases[iCase],VDirs[i]);
    }
    string dirfsMD1 = Form("%sdirfsMD1%d.dat",OutputPath.data(),i);
    string dirfsT1 = Form("%sdirfsT1%d.dat",OutputPath.data(),i);
    string dirfsT2 = Form("%sdirfsT2%d.dat",OutputPath.data(),i);
    string dirfsMD2 = Form("%sdirfsMD2%d.dat",OutputPath.data(),i);
    
    
    string dirfss[4]={dirfsMD1, dirfsT1, dirfsT2, dirfsMD2};
    for(int ic = 0;ic<4;ic++){
      system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNs[ic].data()));
      system(Form("> %s", dirfss[ic].data()));  
      system(Form("ls -1 %s*.bin>>%s",VNs[ic].data(),dirfss[ic].data()));
    }
    map< pair<string, string> , vector<string> > FileClass = classifyDatas(Cases,dirfss,FileTimeStampGap);
        
    // 1 Class be run-time in VSDirs, 2 Class be run-time in Main Det dir, 3. Save at Main Det .bins dir
    string SavePath = "", strtmp, fileroot, cmdline;
    if      (iSaveAtWhere == 0) SavePath = VSDirs[i];
    else if(iSaveAtWhere != 1){
      cout<<"Error: GetScripe.C ~L160: The val of iSaveAtWhere is illegal: "<<iSaveAtWhere<<endl;
      throw;
    }
    double DataSizes = 0, TotalSize = 0;
    int DataNum = 0;
    // throw;
    // while(inMD1>>strtmp){
    for (const auto& entry : FileClass) {
      const auto& key = entry.first;        // pair<string,string>
      const auto& values = entry.second;
      if (iSaveAtWhere == 0){
        string SaveByBinFile = "";
        for(int i=0;i<4;i++){
          if(values[i]!=""){
            SaveByBinFile = values[i];
            break;
          } 
        }
        fileroot = SavePath+SaveByBinFile.substr(SaveByBinFile.rfind("/")+1,SaveByBinFile.size()-(SaveByBinFile.rfind("/")+1));
      }
      else if (iSaveAtWhere == 1)
        fileroot = values[0];
      // else if (iSaveAtWhere == 2)
        // fileroot = values[0].substr(0,values[0].rfind("/")+1);
      cmdline = Form("root -l -b -q Read.C\\(");
      cmdline+= "\"\\\""+fileroot+"\\\"\""+",";
      for(int ic = 0;ic<4;ic++){
        if(values[ic]!="") {
          DataNum++;
          DataSizes+= FileSize(values[ic].data(),'q');
        }
        cmdline+= "\"\\\""+values[ic]+"\\\"\""+",";
      }

      cmdline+= Opmode+"\\) &";
      cout<<cmdline<<endl;
      ifiles++;
      if((DataSizes+TotalSize)*75.>Memmax||(icpu + DataNum)>cpumax){
        if((DataSizes+TotalSize)*75.>Memmax)
          outbash<<"echo Mem: \""<<(DataSizes+TotalSize)*75.<<">"<<Memmax<<"\"";
        else 
          outbash<<"echo CPU: \""<<(icpu + DataNum)<<">"<<cpumax<<"\"";
        outbash<<endl;
        icpu = 0;
        TotalSize = 0;
        outbash<<"wait"<<endl;
      }
      icpu += DataNum;
      TotalSize += DataSizes;
      DataSizes = 0;
      DataNum = 0;
      outbash<<cmdline<<endl;
    }
    for(int ic = 0;ic<4;ic++)
      system(("rm "+dirfss[ic]).data());
  }
  // throw;
  outbash<<"wait"<<endl;
  outbash<<"echo \"Finish the scripe\"\n"<<endl;
  cout<<"finish: "<<(OutputPath+BashFile).data()<<endl;
  
  
  vector<string> scripeNames = {
    "BinToHex","SelectScidata","DecodeSciRaw","Decoding","FastBinDecoding",
    "ReConstruct","DrawMonitering","OverwriteBranch",
    "DrawADC","DrawADCRec","DrawADCRecCalv2","SortCalData",
    "DrawTriggerLinesCenterCal","DrawTriggerLinesCenter", "DrawEResolution"
  };
  if(true){
    string cmdline = "";
    for(int i=0; i <scripeNames.size();i++)
      cmdline+=string(Form("\(root -l -b -q %s.C+)&",scripeNames[i].data()));
    cmdline +="wait";
    system(cmdline.data());
    cout<<cmdline<<endl;
  }
  cout<<"sh "<<(OutputPath+BashFile).data()<<endl;
  // throw;
  system(Form("sh %s%s",OutputPath.data(),BashFile.data()));
  outbash.close();
}