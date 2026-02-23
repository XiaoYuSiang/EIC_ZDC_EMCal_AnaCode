#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <TRandom3.h>
#include <iostream>
#include <vector>

using namespace std;

/**
 * 建立測試數據：生成符合實體結構的模擬數據
 * 結構包含 DID, iX, iY 三個 vector<int> 分支
 */
void CreateTestData(const char* filename, Long64_t nEntries = 1000000) {
    cout << ">>> 正在生成測試數據至 " << filename << " (總事件數: " << nEntries << ")..." << endl;
    TFile *f = new TFile(filename, "RECREATE");
    TTree *t = new TTree("t", "Mock detector data for speed test");

    vector<int> vDID, viX, viY;
    t->Branch("DID", &vDID);
    t->Branch("iX",  &viX);
    t->Branch("iY",  &viY);

    TRandom3 r(0);
    for (Long64_t i = 0; i < nEntries; ++i) {
        vDID.clear(); viX.clear(); viY.clear();
        
        // 模擬每個 Event 有 10~50 個 Hit
        int nHits = r.Integer(40) + 10;
        for (int h = 0; h < nHits; ++h) {
            vDID.push_back(r.Integer(10)); // DID 0~9
            viX.push_back(r.Integer(10));
            viY.push_back(r.Integer(10));
        }
        t->Fill();
    }
    t->Write();
    f->Close();
    delete f;
    cout << ">>> 測試數據生成完畢。\n" << endl;
}

/**
 * ROOT_Speed_Test
 * 測試目標：從 vector 分支中篩選 DID == 4 的 Hit 並填充直方圖
 */
void ROOT_Speed_Test(const char* filename = "Mock_Speed_Test_Data.root") {
    // 如果檔案不存在，則自動生成測試數據
    if (gSystem->AccessPathName(filename)) {
        CreateTestData(filename, 1000000); // 預設生成 1 萬筆
    }

    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        cout << "Error: 找不到測試檔案 " << filename << endl;
        return;
    }
    TTree *t = (TTree*)f->Get("t");
    if (!t) return;

    Long64_t nEvents = t->GetEntries();
    cout << ">>> 檔案讀取成功。總事件數: " << nEvents << endl;

    TStopwatch timer;
    double timeDraw, timeManual, timeOptimized;

    // --- 方法 1: TTree::Draw (底層優化) ---
    cout << "\n1. 測試 TTree::Draw (iX:iY, DID==4)..." << endl;
    TH2F *h1 = new TH2F("h1", "Draw", 10, 0, 10, 10, 0, 10);
    timer.Start();
    t->Draw("iY:iX>>h1", "DID==4", "goff");
    timer.Stop();
    timeDraw = timer.RealTime();
    cout << "   實時 (RealTime): " << timeDraw << "s" << endl;

    // --- 方法 2: 手動循環 (未優化：開啟所有分支) ---
    cout << "2. 測試手動循環 (未優化：預設讀取所有分支數據)..." << endl;
    TH2F *h2 = new TH2F("h2", "Manual", 10, 0, 10, 10, 0, 10);
    
    vector<int> *vDID = nullptr;
    vector<int> *viX = nullptr;
    vector<int> *viY = nullptr;

    t->SetBranchStatus("*", 1); 
    t->SetBranchAddress("DID", &vDID);
    t->SetBranchAddress("iX", &viX);
    t->SetBranchAddress("iY", &viY);

    timer.Start();
    for (Long64_t i = 0; i < nEvents; ++i) {
        t->GetEntry(i); 
        for (size_t h = 0; h < vDID->size(); ++h) {
            if ((*vDID)[h] == 4) {
                h2->Fill((*viX)[h], (*viY)[h]);
            }
        }
    }
    timer.Stop();
    timeManual = timer.RealTime();
    cout << "   實時 (RealTime): " << timeManual << "s" << endl;

    // --- 方法 3: 手動循環 (優化：SetBranchStatus) ---
    cout << "3. 測試手動循環 (優化：只讀取 DID, iX, iY 三個分支)..." << endl;
    TH2F *h3 = new TH2F("h3", "Optimized", 10, 0, 10, 10, 0, 10);
    
    t->SetBranchStatus("*", 0);
    t->SetBranchStatus("DID", 1);
    t->SetBranchStatus("iX", 1);
    t->SetBranchStatus("iY", 1);

    timer.Start();
    for (Long64_t i = 0; i < nEvents; ++i) {
        t->GetEntry(i); 
        for (size_t h = 0; h < vDID->size(); ++h) {
            if ((*vDID)[h] == 4) {
                h3->Fill((*viX)[h], (*viY)[h]);
            }
        }
    }
    timer.Stop();
    timeOptimized = timer.RealTime();
    cout << "   實時 (RealTime): " << timeOptimized << "s" << endl;

    // --- 結論匯整 ---
    cout << "\n==========================================" << endl;
    cout << "           效能對比總結 (Speed Summary)     " << endl;
    cout << "==========================================" << endl;
    printf("TTree::Draw           : %.4f s (100%%)\n", timeDraw);
    printf("手動循環 (未優化)      : %.4f s (%.1f%% slower)\n", timeManual, (timeManual/timeDraw)*100-100);
    printf("手動循環 (分支優化)    : %.4f s (%.1f%% relative to Draw)\n", timeOptimized, (timeOptimized/timeDraw)*100);
    cout << "==========================================" << endl;
    
    t->ResetBranchAddresses();
    f->Close();
}