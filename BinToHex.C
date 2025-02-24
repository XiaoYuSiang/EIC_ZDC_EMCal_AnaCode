#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "TFile.h"
using namespace std;
const int Err_lessPackageLen = 1000;
Long64_t BinToHex(string sourceNameBin, string sourceNameHex){
  cout<<"Start to convert raw bin data to be hex format\n";
  std::ifstream inputFile(sourceNameBin.data(), std::ios::binary); // 打開二進制文件
  if (!inputFile.is_open()) {
      std::cerr << "Can't Open file!!!" << std::endl;
      return 1;
  }
  ofstream of(sourceNameHex.data()); //輸出檔案流
  unsigned char byte;
  Long64_t i=0, iline=0; //計數器
  const char header[45] = {"FE 01 60 00 FE 00 00 00 00 00 00 00 00 04 56"};
  //header of a package 
  string tmpData;
  while (
    inputFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))
    // && i<=30*1127
  ) {
    string strtmp = Form("%02X ",byte); //Get byte by 02X = hex number
    tmpData+=strtmp; //string the bytes
    if(i>(1127+14)){
      //string larger than 1 standard package size(1127Byte) + next header(14Byte)
      int posNext = int(tmpData.find(header,Err_lessPackageLen*3));
      //search header after in string index for observation lessest len. of a package *3=0x\b
      if(posNext>0) { //Find the data with header
        string StrThisLine = tmpData.substr(0,posNext);
        // Get this package as a line(from 1st to 2nd header)
        string StrNextLine = tmpData.substr(posNext,tmpData.size()-posNext);
        // Get next package as a line(from 2nd header)
        tmpData = StrNextLine; // Obtain the next line in the tmp data.
        if(int(StrThisLine.size())<1127*3-1){
          //if len of package line < a standard line, skip this data!
          cout<<"Warning:\n LineID = "<<iline
            <<", length = "<<(StrThisLine.size()+1)/3
            <<" , FN = "<< sourceNameBin <<endl;
          of<<StrThisLine.data()<<endl;
        }else{//Save data
          of<<StrThisLine.data()<<endl;
        }
        //i reset to the last index of next line
        iline++; i = (StrThisLine.size())/3;
      }
    }
    i++;
    // if(i==1128) throw;;
  }
  inputFile.close(); // 關閉文件
  of.close(); // 關閉文件
  cout<<"Finish to convert raw bin data to be hex format!!\n";
  return i;
}
void BinToHex(){
  cout<<"Finished compiling of BinToHex.C+"<<endl;
}