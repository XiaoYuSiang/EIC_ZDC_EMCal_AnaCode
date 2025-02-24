#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace std;
/* Extruct the Science data from Hex file*/
void SelectScidata(string sourceName, string outputName){
  cout<<"Start to taking the science data from Hex raw data package!\n";
  ifstream fin(sourceName.data(), ifstream::in | ifstream::binary);
  ofstream fot(outputName.data());
  
  char chartmp; int valtmp; string strtmp;
  int i=0, frame=0;
  int   seq = 0, GTMID = 0; //Master == 55, Slave == AA
  int   length = 1103, crc8tmp;
  
  string headerLast, scilast;
  while(getline(fin,strtmp)){ // Get one line package
    if(strtmp=="BadLine"){
      fot<<"BadLine"<<endl;
      continue;
    }
    string packageData = strtmp; float Plength = strtmp.size()/3.;
    // cout<<"Size = "<<Plength<<" bytes"<<endl;
    if(Plength!=1127){
      cout<<"Error: length of a package is not 1127 Bytes!!!!"<<endl;
      cout<<"Now, Package length is : "<<Plength<<" frame "<<frame<<endl;
      fot<<"BadLine"<<endl;
      continue;
      // throw;
    }
    stringstream issread(strtmp.data());
    stringstream header;
    for(int i=0;i<15;i++){
      issread>>strtmp;
      header<<strtmp;
    }
    //#0-14 SpaceWire RMAP HEAD = "0xFE016000FE00000000000000000456"
    
    issread>>strtmp; header<<strtmp;//#15 - CRC8#0-#14
    sscanf(strtmp.data(), "%2x", &crc8tmp);
    if(crc8tmp!=0x17){
      cout<<"Warning: L97, crc8!=0x17, crc8 = "<<strtmp<<endl;
      // cout<<"Header = "<<header.str()<<endl;
      // cout<<"Header last  = "<<headerLast.data()<<endl;
      // cout<<"Science last = "<<scilast.data()<<endl;
      // cout<<"\nPackage data:\n";
      fot<<"BadLine"<<endl;
      continue;
      throw;
    }
      // cout<<packageData<<endl;
    headerLast = header.str();
    issread>>strtmp; //#16 - 0x88 Data head
    issread>>strtmp; //#17 - GTM Master=0x55, Slave=0xAA
    sscanf(strtmp.data(), "%2x", &GTMID);
    if(GTMID%85==0) GTMID /= 85 ; //M=0x55=85=Master, M=0xAA=170=Slave
    else {
      cout<<"Error:L~99:  The data is not correct for GTM case :M=55,S=AA: "<<strtmp<<endl;
      fot<<"BadLine"<<endl;
      continue;
      // GTMID = 0;
      throw;
    }
    issread>>strtmp; //#18 - CRC8 value of previous Science data packet
    issread>>strtmp; //#19 - Sequence count (0 – 255)
    sscanf(strtmp.data(), "%2x", &seq);
    issread>>strtmp; //#20 - 0x04 Packet length = 1103 %256
    sscanf(strtmp.data(), "%2x", &length);
    issread>>strtmp; //#21 - 0x4F Packet length = 1103 /256
    sscanf(strtmp.data(), "%2x", &valtmp);
    length = length*256+valtmp;//Get full Packet length
    if(length!=1103){
      cout<<"Warning: L113, length!=1103, length = "<<length<<" frame "<<frame<<endl;
      fot<<"BadLine"<<endl;
      continue;
      // throw;
    }
    stringstream iss;
    for(int i=22;i<=22+length;i++){ //Read fix length science data
      issread>>strtmp;
      iss<<strtmp<<" ";
    } iss<<endl;
    fot<<iss.str(); //Output 1 science data line
    scilast = iss.str();
    // cout<<iss.str();
    frame++; //1 package = 1 frame
    // if(frame==3) break; //test break line
    
  }
  fot.close();
  cout<<"Finish to taking the science data from Hex raw data package!\n";
    
} 
void SelectScidata(){
  cout<<"Finished compiling of SelectScidata.C+"<<endl;
}