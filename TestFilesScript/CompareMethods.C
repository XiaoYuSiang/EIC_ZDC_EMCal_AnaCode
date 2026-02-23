#ifndef COMPARE_METHODS_C
#define COMPARE_METHODS_C

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sys/resource.h> // 測量記憶體所需的標頭檔

#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include "BinToHex.C"
#include "SelectScidata.C"
#include "FastBinDecoding.C"

using namespace std;

// 定義精準測量記憶體峰值的函數
void PrintMemoryPeak(string label) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    // Linux 下 ru_maxrss 單位是 KB，轉換為 MB
    double peakMB = usage.ru_maxrss / 1024.0;
    cout << "[Memory] Peak usage up to " << label << ": " << fixed << setprecision(2) << peakMB << " MB" << endl;
}

void CompareMethods(string sourceBin = "/data8/ZDC/EMCal/BeamTest/Feb25Sort/Feb25LYSOOnly/Run1142_584MeV_HV205_VF255_290_x60_Pos0mm_-346mm_0mm_Lifetime60_191715.267LYSO.bin") {
    string GREEN  = "\033[0;32m";
    string YELLOW = "\033[1;33m";
    string RED    = "\033[0;31m";
    string NC     = "\033[0m";

    if (gSystem->AccessPathName(sourceBin.c_str())) {
        cout << RED << "Error: Cannot find input file -> " << sourceBin << NC << endl;
        return;
    }

    string oldRoot = "OldResult.root";
    string newRoot = "NewResult.root";
    string oldTxt  = "OldResult.txt";
    string newTxt  = "NewResult.txt";
    TStopwatch sw;

    cout << "=============================================" << endl;
    cout << "   ZDC Decoding Performance Benchmark" << endl;
    cout << "=============================================" << endl;

    // --- OLD METHOD ---
    cout << ">>> Starting Old Method (Sequential)..." << endl;
    sw.Start();
    string hexFile = "tmp_step1.hex";
    BinToHex(sourceBin, hexFile);
    double timeStep1 = sw.RealTime();

    sw.Start();
    string sciFile = "tmp_step2.hex";
    SelectScidata(hexFile, sciFile);
    double timeStep2 = sw.RealTime();

    sw.Start();
    Decoding(sciFile, oldTxt, oldRoot, "ResultDir", 0);
    double timeStep3 = sw.RealTime();
    
    double totalOld = timeStep1 + timeStep2 + timeStep3;
    cout << " Done (" << totalOld << "s)" << endl;
    PrintMemoryPeak("Old Method End"); // 紀錄 Old 方法結束後的峰值

    // --- NEW METHOD ---
    cout << "\n>>> Starting New Method (FastBinDecoding)...";
    sw.Start();
    FastBinDecoding(sourceBin, newRoot, newTxt, 0);
    double totalNew = sw.RealTime();
    cout << " Done (" << totalNew << "s)" << endl;
    PrintMemoryPeak("New Method End"); // 紀錄 New 方法結束後的峰值

    // --- 比對 ROOT 檔案 ---
    TFile *fOld = TFile::Open(oldRoot.c_str());
    TFile *fNew = TFile::Open(newRoot.c_str());
    TTree *tOld = (TTree*)fOld->Get("t");
    TTree *tNew = (TTree*)fNew->Get("t");
    Long64_t entriesOld = (tOld) ? tOld->GetEntries() : 0;
    Long64_t entriesNew = (tNew) ? tNew->GetEntries() : 0;

    cout << "\n================ PERFORMANCE ANALYSIS ================" << endl;
    cout << left << setw(25) << "Process Step" << " | " << "Time (RealTime)" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << setw(25) << "OLD TOTAL (3-Step)"    << " | " << YELLOW << totalOld << " s" << NC << endl;
    cout << setw(25) << "NEW TOTAL (FastStream)" << " | " << GREEN  << totalNew << " s" << NC << endl;
    if (totalNew > 0) cout << ">> Speedup Ratio: " << fixed << setprecision(2) << totalOld / totalNew << "x faster" << endl;
    cout << ">> Event Counts:  Old=" << entriesOld << " vs New=" << entriesNew << endl;
    
    if (entriesOld != entriesNew) cout << RED << "[!!] WARNING: Event count mismatch!" << NC << endl;
    else cout << GREEN << "[OK] Event counts match." << NC << endl;
    cout << "======================================================" << endl;

    fOld->Close(); fNew->Close();
}
#endif