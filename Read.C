#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <TF1.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TText.h>
#include <TPDF.h>
#include "TH2.h"
#include "TProfile.h"
#include <TProfile2D.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TChain.h>
#include <TObject.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include "TTree.h"
#include "TFile.h"
// #include "/data4/YuSiang/personalLib/FileIO/FileIO.h"
// #include "BinToHex.C"
using namespace std;

/*Example + default setting*/
string defaultNameRoot   = "/data8/ZDC/EMCal/BeamTest/20250216Run118/LYSOAndPWO/Run118_HV200_VF350_420_LG_x60_073416.641LYSO";
string defaultNameRootMD1   = "/data8/ZDC/EMCal/BeamTest/20250216Run118/LYSOAndPWO/Run118_HV200_VF350_420_LG_x60_073416.641LYSO.bin";
string defaultNameRootT1   = "";
string defaultNameRootT2   = "";
string defaultNameRootMD2   = "";
// string defaultNameRootT1   = "/data8/ZDC/EMCal/BeamTest/20250216Run118/BeamMoniter/Run118_071447.296Monitor1.bin";
// string defaultNameRootT2   = "/data8/ZDC/EMCal/BeamTest/20250216Run118/BeamMoniter/Run118_071447.296Monitor2.bin";
// string defaultNameRootMD2   = "/data8/ZDC/EMCal/BeamTest/20250216Run118/LYSOAndPWO/Run118_HV16_VF348_420_LG_x60_073416.641PbWO4.bin";
int processID = 0b00100;
//^^^^^^^^^^^^change this path and the file name without extentio
// int iDetArr[4] = {0,2,3,-1};
int iDetArr[4] = {1,2,3,4};
string SymbolFile[4] = {"D1","T1","T2","D2"};

  /*Example of files path name*/
//The case file is path/CaseString_hhmmss.bin
string SourceFileNameBin[4] = {"unUseValShouldChange","","",""};
// Binary Raw data Ex: "path/CaseString_hhmmss.bin"
string SourceFileNameHex[4] = {"unUseValShouldChange","","",""};
// Convert to be Hex Raw data Ex: "path/CaseString_hhmmss.hex"
string HexRawSciFileName[4] = {"unUseValShouldChange","","",""};
// Save hex science data to be Hex Ex: "path/CaseString_hhmmss_Sci.hex"
string ReadableRawSciFileNameT[4] = {"unUseValShouldChange","","",""};
// Save hex science data to be Readable .txt Ex: "path/CaseString_hhmmss_Sci.dat"
string ReadableRawSciFileNameR[4] = {"unUseValShouldChange","","",""};
// Save hex science data to be Readable .root Ex: "path/CaseString_hhmmss_Sci.root"


/*main code*/
/*
1. Raw *.bin -> Raw *.hex
2. Raw *.hex -> Raw_Sci *.txt
3. Raw_Sci *.txt -> decode_Sci *.txt & decode_Sci *.root
4. Draw Some sample graphs
*/
bool ChechName(string &NAME){
  if(NAME=="") return false;
  if (NAME.size() >= 4 && NAME.substr(NAME.size() - 4) == ".bin") 
    NAME = NAME.substr(0, NAME.size() - 4);
  return true;
}
void Read(
  string NameRoot = defaultNameRoot,
  string NameRootMD1 = defaultNameRootMD1,
  string NameRootT1 = defaultNameRootT1,
  string NameRootT2 = defaultNameRootT2,
  string NameRootMD2 = defaultNameRootMD2,
  int process = processID//ex: 0b1111=all process, 0b0011 = last two process
){
  ChechName(NameRoot);
  int posNameFirst = NameRoot.rfind("/")+1; int posNameLast = NameRoot.size()-1; 
  string fileName = NameRoot.substr(posNameFirst,posNameLast-posNameFirst+1);
  cout<<fileName<<endl;
  
  string dirAnaPath = Form("%s/",NameRoot.data());
  system(Form("mkdir -p %s",dirAnaPath.data()));
  
  bool FileCheck[4]={
    ChechName(NameRootMD1), ChechName(NameRootT1), ChechName(NameRootT2), ChechName(NameRootMD2) 
  };
  string FileNames[4]={ NameRootMD1, NameRootT1, NameRootT2, NameRootMD2  };
  for(int i=0; i <4;i++){
    SourceFileNameBin[i] = FileNames[i]+".bin";
    // Binary Raw data
    SourceFileNameHex[i] = dirAnaPath+SymbolFile[i]+"_.hex";
    // Convert to be Hex Raw data
    HexRawSciFileName[i] =  dirAnaPath+SymbolFile[i]+"_Sci.hex";
    // Save hex science data to be Hex
    ReadableRawSciFileNameT[i] = dirAnaPath+SymbolFile[i]+"_Sci.dat";
    // Save hex science data to be Readable .txt
    ReadableRawSciFileNameR[i] = dirAnaPath+SymbolFile[i]+"_Sci.root";
    // Save hex science data to be Readable .root
    
    // Save hex science data to be Readable .root
  }
  cout<<"process code = "<<process<<"=";
  cout<<((process>>4)%2)<<((process>>3)%2)<<((process>>2)%2);
  cout<<((process>>1)%2)<<((process>>0)%2)<<endl;
  
  gSystem->SetBuildDir("./tmpdir/", kTRUE);
  // throw;
  const int NScripes = 5;
  string scripeNames[NScripes] = {"BinToHex","SelectScidata","DecodeSciRaw","ReConstruct","DrawADC"};
  if(false){
    string cmdline = "";
    for(int i=0; i <NScripes;i++)
      cmdline+=string(Form("\(root -l -b -q %s.C+)&",scripeNames[i].data()));
    cmdline +="wait";
    system(cmdline.data());
  }
  if((process>>4)%2){
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) cmdline+=string(Form("\(root -l -b -q BinToHex.C+\\(\\\"%s\\\",\\\"%s\\\"\\))&",SourceFileNameBin[i].data(),SourceFileNameHex[i].data()));
    }
    cmdline +="wait";
    system(cmdline.data());
  }
  if((process>>3)%2){
    // SelectScidata(SourceFileNameHex.data(),HexRawSciFileName.data());
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) 
        cmdline+=string(Form("root -l -b -q SelectScidata.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",SourceFileNameHex[i].data(),HexRawSciFileName[i].data()));
    }
    cmdline +="wait";
    system(cmdline.data());
  }
  if((process>>2)%2){
    // Load sensor mapping (1) = quiet. (0) shows mapping
    // DecodeSciRaw(HexRawSciFileName.data(),ReadableRawSciFileNameT.data(),ReadableRawSciFileNameR.data(),NameRoot.data());
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) 
        cmdline+=string(Form("root -l -b -q Decoding.C+\\(\\\"%s\\\",\\\"%s\\\",\\\"%s\\\",\\\"%s\\\",%d\\)&",HexRawSciFileName[i].data(),ReadableRawSciFileNameT[i].data(),ReadableRawSciFileNameR[i].data(),NameRoot.data(),iDetArr[i]));
      
    }
    cmdline +="wait";
    system(cmdline.data());
    if( int(NameRoot.rfind("Run"))!=-1 ){
      string RunName = NameRoot.substr(NameRoot.rfind("Run"),6);
      
      for(int i=0; i <4;i++){
        if(FileCheck[i]){
          system(Form("cp %s/%s_Sci.root %s/%s_%s_Sci.root",NameRoot.data(),SymbolFile[i].data(),NameRoot.data(),RunName.data(),SymbolFile[i].data()));

        }
      }
    }
    // ofstream ofsLog(Form("%s/SyncLog.dat",NameRoot.data()));
    // for(int i=0;i<4;i++){
      // if(FileCheck[i]){
        // ifstream in(Form("%s/ConvertProblem_DID%d",NameRoot.data(),iDetArr[i]));
        // string strtmp;
        // in>>strtmp;
        // cout<<Form("%s/ConvertProblem_DID%d.dat",NameRoot.data(),iDetArr[i])<<endl;
        // cout<<strtmp<<endl;
        // ofsLog<<strtmp<<endl;
      // }
    // }
    // ofsLog.close();
    // system(Form("sleep 0 &&root -l -b -q HLGScale.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",dirAnaPath.data(),fileName.data()));
    // throw;
  }
  if((process>>1)%2){
    system(Form("sleep 0 &&root -l -b -q ReConstruct.C+\\(\\\"%s\\\",\\\"%s\\\",%d\\)",dirAnaPath.data(),fileName.data(),0));
    // cout<<"Finish process>>1 == true"<<endl;
    // system(Form("sleep 0 &&root -l -b -q DrawADC.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",dirAnaPath.data(),fileName.data()));
    // system(Form("sleep 0 &&root -l -b -q HLGScale.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",dirAnaPath.data(),fileName.data()));
    // throw;  
    string reconData = dirAnaPath+fileName+"_ReCon.root";
    system(Form("root -l -b -q DrawTriggerLinesCenter.C+\\(\\\"%s\\\",\\\"%sgraph/\\\"\\)",reconData.data(),dirAnaPath.data()));
    // system(Form("root -l -b -q DrawESpectrum.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",reconData.data(),dirAnaPath.data()));

  }
  // ReCombineADC(dirAnaPath.data(),fileName.data());
  // system(Form("root -l -b -q PackageTestV2.C+\\(\\\"%s/\\\",\\\"%s\\\"\\)",NameRoot.data(),fileName.data()));
  // system(Form("root -l -b -q PackageTestReEnergy.C+\\(\\\"%s/\\\",\\\"%s\\\"\\)",NameRoot.data(),fileName.data()));
  // system(Form("root -l -b -q match.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",NameRoot.data(),fileName.data()));
  // system(Form("root -l -b -q ShowEdepVsPeakChannel.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",NameRoot.data(),fileName.data()));
  
  // DrawEnerCollRatio(dirAnaPath.data(),fileName.data());
  // DrawEnerCollRatio2(dirAnaPath.data(),fileName.data());
  
 // throw; // test break line
  // system(Form("sleep 0 &&root -l -b -q PositionEnergy.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",NameRoot.data(),fileName.data()));
  // system(Form("sleep 0 &&root -l -b -q PositionEnergy2.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",NameRoot.data(),fileName.data()));
  if((process>>0)%2){
    // system(Form("sleep 0 &&root -l -b -q FitPeakCode.C+\\(\\\"%s\\\"\\)&",dirAnaPath.data()));
    // system(Form("sleep 0 &&root -l -b -q ADCShow2.C+\\(\\\"%s\\\",\\\"%s\\\"\\)&",NameRoot.data(),fileName.data()));
    // system(Form("sleep 0 &&root -l -b -q FindFirstBin.C\\(\\\"%s\\\"\\)&",dirAnaPath.data()));
    
    string cmdlineDrawADC = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) 
        cmdlineDrawADC+=string(Form("(root -l -b -q DrawADC.C+\\(\\\"%s\\\",\\\"%s\\\",%d\\))&",dirAnaPath.data(),SymbolFile[i].data(),iDetArr[i]));
    }
    cmdlineDrawADC +="wait";
    system(cmdlineDrawADC.data());
    system(Form("(root -l -b -q DrawADCRec.C+\\(\\\"%s\\\",\\\"%s\\\",%d\\))&",dirAnaPath.data(),fileName.data(),1));
    system(Form("python stack_graphs.py %s/HitVSPos_D%d.png %s/HitVSPos_D%d.png %s/HitVSPos_D%d.png %s/BeamProfile_All.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
    system(Form("python /data4/YuSiang/personalLib/Graph/SortGraph.py %s/Emax_D%d.png %s/Emax_D%d.png %s/Emax_D%d.png 3 1 %s/Emax_All.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
    // cout<<(Form("python stack_graphs.py %sHitVSPos_D%d.png %sHitVSPos_D%d.png %sHitVSPos_D%d.png %soutput.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
  }
  // throw;
  system(Form("chmod 777 %s/*",dirAnaPath.data()));
  system(Form("chmod 777 %s",dirAnaPath.data()));
  system("wait\n echo \"Finish all graph\"");
  cout<<endl<<"finish all processes!! for setting "<<Form("%i\n",process);
}

// for(i=0;i<4;i++) bufferID |= (Word>>(18+i)&1) << i;
