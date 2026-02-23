// Test.C
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <iostream>

// 這是你的 layout class header
#include "/data8/ZDC/EMCal/ShareScript/tdrstyle.h"
#include "/data4/YuSiang/personalLib/RPU/DBMLayouts.h"

void Test() {
    // 隨機種子
    TRandom3 rand(0);

    // === 產生第一個 2D Gaussian histogram ===
    int nx = 100;
    int ny = 100;
    double x_min = -5, x_max = 5;
    double y_min = -5, y_max = 5;

    TH2D* h2Diff2C = new TH2D("h2Diff2C", "Random 2D Gaussian;X;Y", nx, x_min, x_max, ny, y_min, y_max);

    // 填入隨機 2D Gaussian
   double meanX = rand.Uniform(-2, 2);
   double meanY = rand.Uniform(-2, 2);
   double SigX = rand.Uniform(0, 1.5);
   double SigY = rand.Uniform(0, 1.5);
    for (int i=0; i<10000; i++) {
        double x = rand.Gaus(meanX, SigX); // 平均0，sigma=1
        double y = rand.Gaus(meanY, SigY); // 平均0，sigma=1.5
        h2Diff2C->Fill(x, y);
    }

    // === 建立 LayoutProfile2DObj ===
    LayoutProfile2DObj layoutD2C(h2Diff2C, 1080, "Colz", "Fit(Gaus)", "Fit(Gaus)");

    // === 輸出 GIF ===
    layoutD2C.canvas->Print("./Test.gif");

    std::cout << "Finished drawing Test.gif" << std::endl;
}
