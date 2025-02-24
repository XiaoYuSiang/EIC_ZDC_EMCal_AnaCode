#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
#include <sstream>
/*
root -b -q GetScripe.C
*/

using namespace std;
const int    NumDirs =1;
const int cpumax = 10;
const string Opmode = "31";
// 1 = 0b00001, 3 = 0b00011, 7 = 0b00111, 15 = 0b01111, 31 = 0b11111
const string BashFile = "FileList250219_PWO.sh";
const string OutputPath = "./TBBO_PWO/";
const string Cases[4] = {"LYSO","Monitor1","Monitor2","PbWO4"};
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
  "/data8/ZDC/EMCal/BeamTest/BeamOn0220/Run{3009,3184,3216,3263,3273,3274,3304,3305}*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0220/Run20{8[8-9],9[0-6]}*"
  // "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run212*"
  // "/data8/ZDC/EMCal/BeamTest/20250215/BeamMoniter/20250216/Run118*_07*"
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Co60_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/Cs137_HV16_FV_290_320_LG_x10*",
  // "/data8/ZDC/EMCal/PbWO4SiPM/250204/BG_HV16_FV_290_320_LG_x10*"
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
  // ""  
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
string SetPathAuto(string vStrCase,string Case,string vStrMain){
  if(vStrCase=="auto") return vStrMain+Case;
  return vStrCase;
}

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
    system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNMD1[i].data()));
    system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNMD2[i].data()));
    system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNT1[i].data()));
    system(Form("sh /data8/ZDC/EMCal/APDLYSO/AnaCode/rename_bin.sh %s",VNT2[i].data()));
    
    
    string dirfsMD1 = Form("%sdirfsMD1%d.dat",OutputPath.data(),i);
    system(Form("ls -1 %s*.bin>>%s",VNMD1[i].data(),dirfsMD1.data()));
    ifstream inMD1(dirfsMD1.data());
    
    bool SubDetOn[3] = {0};
    string dirfsT1 = Form("%sdirfsT1%d.dat",OutputPath.data(),i);
    if(VNT1[i]!="") SubDetOn[0] = true;
    system(Form("ls -1 %s*.bin>>%s",VNT1[i].data(),dirfsT1.data()));
    ifstream inT1(dirfsT1.data());
    
    string dirfsT2 = Form("%sdirfsT2%d.dat",OutputPath.data(),i);
    if(VNT2[i]!="") SubDetOn[1] = true;
    system(Form("ls -1 %s*.bin>>%s",VNT2[i].data(),dirfsT2.data()));
    ifstream inT2(dirfsT2.data());
    
    string dirfsMD2 = Form("%sdirfsMD2%d.dat",OutputPath.data(),i);
    if(VNMD2[i]!="") SubDetOn[2] = true;
    system(Form("ls -1 %s*.bin>>%s",VNMD2[i].data(),dirfsMD2.data()));
    ifstream inMD2(dirfsMD2.data());
    
    string strtmp, fileroot, cmdline;
    while(inMD1>>strtmp){
      fileroot = strtmp;
      int delay = ifiles ==0? 0 : 0;
      cmdline = Form("sleep %d &&root -l -b -q Read.C\\(",delay);
      cmdline+= "\"\\\""+fileroot+"\\\"\""+",";
      cmdline+= "\"\\\""+fileroot+"\\\"\""+",";
      
      if(SubDetOn[0]){
        inT1>>strtmp;
        cmdline+= "\"\\\""+strtmp+"\\\"\""+",";
      }else{
        cmdline+= "\"\\\""+string("")+"\\\"\""+",";
      }
      
      if(SubDetOn[1]){
        inT2>>strtmp;
        cmdline+= "\"\\\""+strtmp+"\\\"\""+",";
      }else{
        cmdline+= "\"\\\""+string("")+"\\\"\""+",";
      }

      if(SubDetOn[2]){
        inMD2>>strtmp;
        cmdline+= "\"\\\""+strtmp+"\\\"\""+",";
      }else{
        cmdline+= "\"\\\""+string("")+"\\\"\""+",";
      }

      cmdline+= Opmode+"\\) &";
      cout<<cmdline<<endl;
      outbash<<cmdline<<endl;
      ifiles++;
      icpu++;
      if(icpu>=cpumax){
        icpu = 0;
        outbash<<"wait"<<endl;
      }
    }
    system(("rm "+dirfsMD1).data());
    system(("rm "+dirfsT1).data());
    system(("rm "+dirfsT2).data());
    system(("rm "+dirfsMD2).data());
    inMD1.close();
    inT1.close();
    inT2.close();
    inMD2.close();
  }
  // throw;
  outbash<<"wait"<<endl;
  outbash<<"echo \"Finish the scripe\"\n"<<endl;
  cout<<"finish: "<<(OutputPath+BashFile).data()<<endl;
  
  
  
  const int NScripes = 8;
  string scripeNames[NScripes] = {"BinToHex","SelectScidata","Decoding","DecodeSciRaw","ReConstruct","DrawTriggerLinesCenter","DrawADCRec","DrawADC"};
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