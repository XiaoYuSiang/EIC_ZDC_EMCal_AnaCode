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
// string defaultNameRoot   = "path/CaseString_hhmmss";
string defaultNameRoot   = "/data8/ZDC/EMCal/APDLYSO/LEDTEST/LED_HV199_VF230_250_X50_cableSWAB_172457.459Ch1";

//^^^^^^^^^^^^change this path and the file name without extention^^^^^^^^^


  /*Example of files path name*/
//The case file is path/CaseString_hhmmss.bin
string SourceFileNameBin = "unUseValShouldChange";
// Binary Raw data Ex: "path/CaseString_hhmmss.bin"
string SourceFileNameHex = "unUseValShouldChange";
// Convert to be Hex Raw data Ex: "path/CaseString_hhmmss.hex"
string HexRawSciFileName = "unUseValShouldChange";
// Save hex science data to be Hex Ex: "path/CaseString_hhmmss_Sci.hex"
string ReadableRawSciFileNameT = "unUseValShouldChange";
// Save hex science data to be Readable .txt Ex: "path/CaseString_hhmmss_Sci.dat"
string ReadableRawSciFileNameR = "unUseValShouldChange";
// Save hex science data to be Readable .root Ex: "path/CaseString_hhmmss_Sci.root"


/*main code*/
/*
1. Raw *.bin -> Raw *.hex
2. Raw *.hex -> Raw_Sci *.txt
3. Raw_Sci *.txt -> decode_Sci *.txt & decode_Sci *.root
4. Draw Some sample graphs
*/

void Read(
  const string NameRoot = defaultNameRoot,
  const int process = 0b1111//ex: 0b1111=all process, 0b0011 = last two process
){
  int posNameFirst = NameRoot.rfind("/")+1; int posNameLast = NameRoot.size()-1; 
  string fileName = NameRoot.substr(posNameFirst,posNameLast-posNameFirst+1);
  cout<<fileName<<endl;
  
  string dirAnaPath = Form("%s/",NameRoot.data());
  system(Form("mkdir -p %s",dirAnaPath.data()));
  
  SourceFileNameBin = NameRoot+".bin";
  // Binary Raw data
  SourceFileNameHex = dirAnaPath+fileName+".hex";
  // Convert to be Hex Raw data
  HexRawSciFileName =  dirAnaPath+fileName+"_Sci.hex";
  // Save hex science data to be Hex
  ReadableRawSciFileNameT = dirAnaPath+fileName+"_Sci.dat";
  // Save hex science data to be Readable .txt
  ReadableRawSciFileNameR = dirAnaPath+fileName+"_Sci.root";
  // Save hex science data to be Readable .root
  
  cout<<"process code = "<<process<<"=";
  cout<<((process>>3)%2)<<((process>>2)%2);
  cout<<((process>>1)%2)<<((process>>0)%2)<<endl;
  
  gSystem->SetBuildDir("./tmpdir/", kTRUE);
  // throw;
  if((process>>3)%2)
    // BinToHex(SourceFileNameBin.data(),SourceFileNameHex.data());
    system(Form("sleep 0 &&root -l -b -q BinToHex.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",SourceFileNameBin.data(),SourceFileNameHex.data()));
    // throw; // test break line
  if((process>>2)%2)
    // SelectScidata(SourceFileNameHex.data(),HexRawSciFileName.data());
    system(Form("sleep 0 &&root -l -b -q SelectScidata.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",SourceFileNameHex.data(),HexRawSciFileName.data()));
  if((process>>1)%2){
    //Load sensor mapping (1) = quiet. (0) shows mapping
    // DecodeSciRaw(HexRawSciFileName.data(),ReadableRawSciFileNameT.data(),ReadableRawSciFileNameR.data(),NameRoot.data());
    system(Form("sleep 0 &&root -l -b -q DecodeSciRaw.C+\\(\\\"%s\\\",\\\"%s\\\",\\\"%s\\\",\\\"%s\\\"\\)",HexRawSciFileName.data(),ReadableRawSciFileNameT.data(),ReadableRawSciFileNameR.data(),NameRoot.data()));
    // system(Form("sleep 0 &&root -l -b -q DrawADC.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",dirAnaPath.data(),fileName.data()));
    // system(Form("sleep 0 &&root -l -b -q HLGScale.C+\\(\\\"%s\\\",\\\"%s\\\"\\)",dirAnaPath.data(),fileName.data()));
    throw;
  }
  // ReCombineADC(dirAnaPath.data(),fileName.data());
  system(Form("root -l -b -q PackageTestV2.C+\\(\\\"%s/\\\",\\\"%s\\\"\\)",NameRoot.data(),fileName.data()));
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
  }
  system("wait\n echo \"Finish all graph\"");
  cout<<"finish all processes!! for setting "<<Form("%i\n",process);
}

// for(i=0;i<4;i++) bufferID |= (Word>>(18+i)&1) << i;
