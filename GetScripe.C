#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
#include <sstream>
#include "GetScripeFunctions.h"
/*
root -b -q GetScripe.C
*/
using namespace std;
const int cpumax = 64;
const double Memmax = 110.e+9;//Byte
const string Opmode = "31";
// 1 = 0b00001, 3 = 0b00011, 7 = 0b00111, 15 = 0b01111, 31 = 0b11111
const string BashFile = "FileListBMCaliTest.sh";
// const string OutputPath = "./Feb2025/";
const string OutputPath = "/data8/ZDC/EMCal/BeamTest/Feb25Sort/BMCali/Test/";
const string Cases[4] = {"LYSO","Monitor1","Monitor2","PbWO4"};
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
  // "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25LYSOOnly/Run*MeV*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*"
  // "/data8/ZDC/EMCal/BeamTest/20250215/BeamMoniter/20250216/Run118*_07*"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Co60_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Cs137_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/BG_HV16_FV_290_320_LG_x10*"
  // "/data8/ZDC/EMCal/BeamTest/AS_NoiseTest/Run*"
  "/data8/ZDC/EMCal/BeamTest/Feb25Sort/BMCali/Na22*"
};
const string VSDirs[NumDirs]={
  "/data8/ZDC/EMCal/BeamTest/Feb25Sort/BMCali/"
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
  ""
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
  ""
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
  "auto"  
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
  for(int i=0;i<NumDirs;i++){
    // system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VDirs[i].data()));
    VNMD1[i] = SetPathAuto(VNMD1[i],Cases[0],VDirs[i]);
    VNT1 [i] = SetPathAuto(VNT1 [i],Cases[1],VDirs[i]);
    VNT2 [i] = SetPathAuto(VNT2 [i],Cases[2],VDirs[i]);
    VNMD2[i] = SetPathAuto(VNMD2[i],Cases[3],VDirs[i]);
    string dirfsMD1 = Form("%sdirfsMD1%d.dat",OutputPath.data(),i);
    string dirfsT1 = Form("%sdirfsT1%d.dat",OutputPath.data(),i);
    string dirfsT2 = Form("%sdirfsT2%d.dat",OutputPath.data(),i);
    string dirfsMD2 = Form("%sdirfsMD2%d.dat",OutputPath.data(),i);
    
    string VNs[4]={VNMD1[i], VNT1 [i], VNT2 [i], VNMD2[i]};
    string dirfss[4]={dirfsMD1, dirfsT1, dirfsT2, dirfsMD2};
    for(int ic = 0;ic<4;ic++){
      system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNs[ic].data()));
      system(Form("ls -1 %s*.bin>>%s",VNs[ic].data(),dirfss[ic].data()));
    }
    map< pair<string, string> , vector<string> > FileClass = classifyDatas(Cases,dirfss);
    string SavePath = VSDirs[i];
    string strtmp, fileroot, cmdline;
    double DataSizes = 0, TotalSize = 0;
    int DataNum = 0;
    // while(inMD1>>strtmp){
    for (const auto& entry : FileClass) {
      const auto& key = entry.first;        // pair<string,string>
      const auto& values = entry.second;
      fileroot = SavePath+values[0].substr(values[0].rfind("/")+1,values[0].size()-(values[0].rfind("/")+1));
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
      if((DataSizes+TotalSize)*75.>Memmax||(icpu + DataNum)>=cpumax){
        if((DataSizes+TotalSize)*75.>Memmax)
          outbash<<"echo Mem: "<<(DataSizes+TotalSize)*75.<<">"<<Memmax;
        else 
          outbash<<"echo CPU: "<<(icpu + DataNum)<<">="<<cpumax;
        outbash<<endl;
        icpu = 0;
        TotalSize = 0;
        outbash<<"wait"<<endl;
      }
      icpu += DataNum+1;
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
  
  // throw;
  
  
  const int NScripes = 9;
  string scripeNames[NScripes] = {"BinToHex","SelectScidata","Decoding","DecodingV2","DecodeSciRaw","ReConstruct","DrawTriggerLinesCenter","DrawADCRec","DrawADC"};
  if(true){
    string cmdline = "";
    for(int i=0; i <NScripes;i++)
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