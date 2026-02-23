#ifndef FAST_BIN_DECODING_C
#define FAST_BIN_DECODING_C

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "TSystem.h"
#include "Decoding.C" 

using namespace std;

void FastBinDecoding(string binFileName, string rootFileName, string txtFileName, int iDetector = 0) {
    ifstream inputFile(binFileName, ios::binary);
    if (!inputFile.is_open()) return;

    string tmpHexName = "final_aligned_output.hex";
    ofstream outputFile(tmpHexName);
    
    char hexMap[256][3];
    for (int i = 0; i < 256; i++) sprintf(hexMap[i], "%02X", i);

    unsigned char byte;
    vector<unsigned char> buffer;
    const unsigned char spwHead[] = {0xFE, 0x01, 0x60, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x56};
    
    while (inputFile.read((char*)&byte, 1)) {
        buffer.push_back(byte);
        
        if (buffer.size() == 15) {
            bool match = true;
            for (int i = 0; i < 15; i++) {
                if (buffer[i] != spwHead[i]) { match = false; break; }
            }

            if (match) {
                // 1. 跳過前 22 bytes 中的前 15 bytes (已讀取)
                // 2. 讀取剩下的 1112 bytes (1127 - 15)
                unsigned char fullPayload[1112];
                if (inputFile.read((char*)fullPayload, 1112)) {
                    // 3. 核心修正：從第 22 byte 開始 (payload[6])
                    // 必須取到最後一個 byte (payload[1111])，不能因為 F2 F5 FA 就停止
                    for (int k = 7; k < 1111; k++) {
                        outputFile << hexMap[fullPayload[k]] << " ";

                    }
                    outputFile << "\n"; // 保持一封包一行
                }
                buffer.clear();
            } else {
                buffer.erase(buffer.begin());
            }
        }
    }
    
    inputFile.close();
    outputFile.close();

    cout << ">>> Full-Length Payload Extracted. Syncing with Decoding.C..." << endl;
    Decoding(tmpHexName, txtFileName, rootFileName, "FastResult", iDetector);
    // gSystem->Unlink(tmpHexName.c_str());
}

void FastBinDecoding(){
  cout<<"Finished compiling of FastBinDecoding.C+"<<endl;
}
#endif