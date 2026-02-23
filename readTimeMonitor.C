#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>         // 新增：用於儲存監控數據
#include <sys/resource.h> // 新增：用於監控記憶體峰值
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

// --- 新增：資源監控輔助函數 ---
double GetPeakRSS() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024.0; // 轉換為 MB
}
// ----------------------------

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
int iDetArr[4] = {4,2,3,-1};
string SymbolFile[4] = {"D2","T1","T2","D1"};

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
  ifstream f(NAME+string(".bin"));
  if (!f.good()) {
    cout << "File: "<<NAME<<".bin is NOT found!\n";
    f.close();
    return false;
  }
  return true;
}

// 輔助函式：處理字串引號 (保留原名 sb 以相容舊程式碼)
string sb(string str){return Form("\\\"%s\\\"",str.data());};

// 主函式名稱更改為 readTimeMonitor
void readTimeMonitor(
  string NameRoot = defaultNameRoot,
  string NameRootMD1 = defaultNameRootMD1,
  string NameRootT1 = defaultNameRootT1,
  string NameRootT2 = defaultNameRootT2,
  string NameRootMD2 = defaultNameRootMD2,
  int process = processID//ex: 0b1111=all process, 0b0011 = last two process
){
  
  // --- 開始全域計時 ---
  TStopwatch totalSw;
  totalSw.Start();
  vector<pair<string, double>> timeLogs; // 儲存每個步驟的時間
  TStopwatch stepSw;

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
  
  // throw;
  /*
  gSystem->SetBuildDir("./tmpdir/", kTRUE);
  vector<string> scripeNames = {
    "BinToHex","SelectScidata","DecodeSciRaw","Decoding",
    "ReConstruct","OverwriteBranch",
    "DrawADC","DrawADCRec","DrawADCRecCalv2",
    "DrawTriggerLinesCenterCal","DrawTriggerLinesCenter"
  };
  if(false){
    string cmdline = "";
    for(int i=0; i <scripeNames.size();i++)
      cmdline+=string(Form("\(root -l -b -q %s.C+)&",scripeNames[i].data()));
    cmdline +="wait";
    system(cmdline.data());
    cout<<cmdline<<endl;
  }*/
  ofstream offInfo((dirAnaPath+"The_Run_Infos_file.dat").data());
  int posRun = int(NameRoot.rfind("Run")), runIndex;
  string RunName;
  if( posRun!=-1 ){
    RunName = NameRoot.substr(posRun+3,NameRoot.find("_",posRun)-posRun-3);
    runIndex = stoi(RunName);
    RunName = Form("Run%06.0f",runIndex*1.);
    offInfo<<"Run: "<<runIndex<<endl;
    offInfo.close();
  }else{
    cout<<"The File without the run number!!!"<<endl;
    cout<<"Fail to do the analysis!!!"<<endl;
    throw;
  }
  
  // --- Process 4: BinToHex ---
  if((process>>4)%2){
    stepSw.Start();
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) cmdline+=string(Form("\(root -l -b -q BinToHex.C+\\(%s,%s\\))&",
                                    sb(SourceFileNameBin[i]).data(),sb(SourceFileNameHex[i]).data()));
    }
    cmdline +="wait";
    system(cmdline.data());
    timeLogs.push_back({"BinToHex Conversion", stepSw.RealTime()});
  }

  // --- Process 3: SelectScidata ---
  if((process>>3)%2){
    stepSw.Start();
    // SelectScidata(SourceFileNameHex.data(),HexRawSciFileName.data());
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) 
        cmdline+=string(Form("root -l -b -q SelectScidata.C+\\(%s,%s\\)&",
                        sb(SourceFileNameHex[i]).data(),sb(HexRawSciFileName[i]).data()));
    }
    cmdline +="wait";
    system(cmdline.data());
    timeLogs.push_back({"SelectScidata Extraction", stepSw.RealTime()});
  }

  // --- Process 2: Decoding ---
  if((process>>2)%2){
    stepSw.Start();
    // Load sensor mapping (1) = quiet. (0) shows mapping
    string cmdline = "";
    for(int i=0; i <4;i++){
      if(FileCheck[i]) 
        cmdline+=string(Form("root -l -b -q Decoding.C+\\(%s,%s,%s,%s,%d\\)&",
                        sb(HexRawSciFileName[i]).data(),sb(ReadableRawSciFileNameT[i]).data(),
                        sb(ReadableRawSciFileNameR[i]).data(),sb(NameRoot).data(),iDetArr[i]));
    }
    cmdline +="wait";
    // cout<<cmdline.data()<<endl; throw;
    system(cmdline.data());
    int posRun = int(NameRoot.rfind("Run"));
    if( posRun!=-1 ){
      string RunName = NameRoot.substr(posRun+3,NameRoot.find("_",posRun)-posRun-3);
      int runIndex = stoi(RunName);
      for(int i=0; i <4;i++){
        if(FileCheck[i]){
          system(Form("ln -s %s/%s_Sci.root %s/Run%s_%s_Sci.root",
                          NameRoot.data(),SymbolFile[i].data(),
                          NameRoot.data(),RunName.data(),SymbolFile[i].data()));
        }
      }
    }
    timeLogs.push_back({"Decoding Process", stepSw.RealTime()});
  }

  // --- Process 1: ReConstruct ---
  if((process>>1)%2){
    stepSw.Start();
    system(Form("root -l -b -q ReConstruct.C+\\(%s,%s,%d\\)",sb(dirAnaPath).data(),sb(fileName).data(),true));
    timeLogs.push_back({"ReConstruct", stepSw.RealTime()});
  }

  // --- Process 0: Drawing & Analysis (Sequential & Monitored) ---
  if((process>>0)%2){
    // cout<<185<<endl;
    // 原本有 & 的部分現在改為順序執行，並分別計時
    /*
    cout << ">>> Running Task: DrawADC..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawADC.C+\\(%s\\)",sb(dirAnaPath).data()));
    timeLogs.push_back({"DrawADC", stepSw.RealTime()});

    cout << ">>> Running Task: DrawMonitering..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawMonitering.C+\\(%s\\)",sb(dirAnaPath).data()));
    timeLogs.push_back({"DrawMonitering", stepSw.RealTime()});
*/
    cout << ">>> Running Task: DrawADCRec..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawADCRec.C+\\(%s,%s\\)",
                sb(dirAnaPath).data(),sb(fileName).data()));
    timeLogs.push_back({"DrawADCRec", stepSw.RealTime()});
/*
    cout << ">>> Running Task: OverwriteBranch..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q OverwriteBranch.C+\\(%s,%s,%d\\)",
                sb(dirAnaPath).data(),sb(fileName+"_ReCon").data(),4));
    timeLogs.push_back({"OverwriteBranch", stepSw.RealTime()});

    cout << ">>> Running Task: SortCalData..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q SortCalData.C+\\(%s,%s\\)",
                sb(dirAnaPath).data(),sb(fileName+"_ReCon").data()));
    timeLogs.push_back({"SortCalData", stepSw.RealTime()});

    cout << ">>> Running Task: DrawTriggerLinesCenterCal..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawTriggerLinesCenterCal.C+\\(%s,%s\\)",
                sb(dirAnaPath).data(),sb(fileName).data()));
    timeLogs.push_back({"DrawTriggerLinesCenterCal", stepSw.RealTime()});

    cout << ">>> Running Task: DrawTriggerLinesCenter..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawTriggerLinesCenter.C+\\(%s,%s,%d\\)",
                sb(dirAnaPath).data(),sb(fileName).data(),4));
    timeLogs.push_back({"DrawTriggerLinesCenter", stepSw.RealTime()});

    cout << ">>> Running Task: DrawADCRecCalv2..." << endl;
    stepSw.Start();
    system(Form("root -l -b -q DrawADCRecCalv2.C+\\(%s,%s\\)",
                sb(dirAnaPath).data(),sb(fileName).data()));
    timeLogs.push_back({"DrawADCRecCalv2", stepSw.RealTime()});
                
    // system(Form("python stack_graphs.py %s/HitVSPos_D%d.png %s/HitVSPos_D%d.png %s/HitVSPos_D%d.png %s/BeamProfile_All.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
    // system(Form("python /data4/YuSiang/personalLib/Graph/SortGraph.py %s/Emax_D%d.png %s/Emax_D%d.png %s/Emax_D%d.png 3 1 %s/Emax_All.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
    // cout<<(Form("python stack_graphs.py %sHitVSPos_D%d.png %sHitVSPos_D%d.png %sHitVSPos_D%d.png %soutput.png",dirAnaPath.data(),1,dirAnaPath.data(),2,dirAnaPath.data(),3,dirAnaPath.data()));
    
    */
  }
  // throw;
  
  // --- Final Export Tasks ---
  cout << ">>> Running Task: Copy HTML..." << endl;
  stepSw.Start();
  system(Form("cp ./RunDisplay.html %s",dirAnaPath.data()));
  timeLogs.push_back({"Copy RunDisplay.html", stepSw.RealTime()});

  cout << ">>> Running Task: ExportToMHTML..." << endl;
  stepSw.Start();
  system(Form("root -l -b -q ExportToMHTML.C\\(%s,%s\\)",
    sb(dirAnaPath+"/RunDisplay.html").data(),sb(dirAnaPath+"/RunDisplayMonofile.html").data()));
  timeLogs.push_back({"ExportToMHTML", stepSw.RealTime()});
  
  system(Form("ln -s %s/RunDisplayMonofile.html %s/RunDisplay_%s.html"
                      ,NameRoot.data(),NameRoot.data(),RunName.data()));
  system(Form("ln -s %s/RunDisplayMonofile.html %s/../RunDisplay_%s.html"
                      ,NameRoot.data(),NameRoot.data(),RunName.data()));

  system(Form("chmod 777 %s/*",dirAnaPath.data()));
  system(Form("chmod 777 %s",dirAnaPath.data()));
  // system("wait\n echo \"Finish all graph\""); // 已改為單線程，不需要 wait
  cout<<endl<<"finish all processes!! for setting "<<Form("%i\n",process);

  // --- Resource Summary Output ---
  totalSw.Stop();
  double peakMem = GetPeakRSS();

  cout << "\n" << string(65, '=') << endl;
  cout << "             RESOURCE CONSUMPTION SUMMARY" << endl;
  cout << string(65, '-') << endl;
  cout << left << setw(40) << "  Task / Script Item" << " | " << "Time (RealTime)" << endl;
  cout << string(65, '-') << endl;

  for (const auto& log : timeLogs) {
      cout << "  " << left << setw(38) << log.first << " | " 
           << fixed << setprecision(3) << log.second << " s" << endl;
  }

  cout << string(65, '-') << endl;
  cout << left << setw(40) << "  TOTAL WALL TIME" << " | " << totalSw.RealTime() << " s" << endl;
  cout << left << setw(40) << "  PEAK MEMORY USAGE (RSS)" << " | " << peakMem << " MB" << endl;
  cout << string(65, '=') << endl;
}

// for(i=0;i<4;i++) bufferID |= (Word>>(18+i)&1) << i;