#include <typeinfo>
#include <deque>
#include <iostream>
#include <vector>
#include <algorithm>  // For std::max_element
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "TTree.h"
#include "TFile.h"
#include "./GIDMapping.h"
using namespace std;
// 定義封包結構
struct Packet {
    vector<string> data;
    bool isValid;
    bool bSync;
    string invalidReason;
};

//globel variable
const bool AllowNegADC = false;
bool sortByVGADC(const std::vector<int>& a, const std::vector<int>& b) {
    return a[0] > b[0];
}

/*Get bit data from range B-Start to B-Final*/
int GetBits(int Target, int StartB, int FinalB,int del = 0){
  int mask = (1 << (FinalB - StartB + 1)) - 1;
  // 提取所需位段並移位到最右端
  int bits = (Target >> StartB) & mask;
  // 根據 del 進行縮放
  return bits / pow(2, del);
}

/*Get a word value(int) in the science data*/
int GetWordVal(string *Hitdata){
  int value = 0;
  string Hitdatas = Hitdata[0] + Hitdata[1] + Hitdata[2];
  sscanf(Hitdatas.data(), "%6x", &value);
  return value;
}
int Get2BVal(string *Hitdata){
  int value = 0;
  string Hitdatas = Hitdata[0] + Hitdata[1];
  sscanf(Hitdatas.data(), "%6x", &value);
  return value;
}
int Get1BVal(string *Hitdata){
  int value = 0;
  sscanf(Hitdata[0].data(), "%6x", &value);
  return value;
}

// 解析 HEX 字串為陣列
vector<string> parseHexFile(const string& filename) {
    ifstream file(filename);
    vector<string> hexData;
    string word;
    
    while (file >> word) {
        hexData.push_back(word);
    }
    cout<<"L66: "<<filename<<" , hex array size: "<<hexData.size()<<endl;
    return hexData;
}

// 檢查是否為 SYNC 開頭
bool isSyncStart(const vector<string>& data, size_t index) {
    return (index + 1 < data.size() && data[index] == "CA");
}

// 檢查是否為 SYNC 結尾
bool isSyncEnd(const vector<string>& data, size_t index) {
    return  data[ index - 2 ] == "F2" &&
            data[ index - 1 ] == "F5" && data[index] == "FA";
}

// 解析封包
vector<Packet> extractPackets(const vector<string>& hexData) {
  vector<Packet> packets;
  size_t i = 0;
  vector<int> iyncsPack;
  while (i < hexData.size()) {
    Packet pkt;
    // cout<<46<<endl;
    while (i < hexData.size()) {
      // cout<<i<<endl;
      if (hexData[i] == "BadLine") {
        pkt.isValid = false;
        pkt.bSync   = false;
        pkt.invalidReason = "Contains BadLine";
        break;
      }
      // cout<<"L54"<<endl;
      pkt.data.push_back(hexData[i]);
      if(i >= 3&&isSyncEnd(hexData,i)){
      // cout<<"L68"<<endl;
        if (isSyncStart(hexData, i-44)){
          Packet pktSync;
          int pktsize = pkt.data.size();
          for(int isub=0;isub<45;isub++) pktSync.data.push_back(pkt.data[pktsize - 45 + isub]);

          for(int isub=0;isub<45;isub++) pkt.data.pop_back();
          pkt.isValid = true;
          pkt.bSync = false;
          packets.push_back(pkt);
          pktSync.isValid = true;
          pktSync.bSync = true;
          packets.push_back(pktSync);
          iyncsPack.push_back(packets.size());
          break;
        }else{
      // cout<<"L76"<<endl;
          pkt.isValid = false;
          pkt.bSync = true;
          pkt.invalidReason = "Invalid length ( ! 45 units)";
          packets.push_back(pkt);
          break;
        }
      }
      // cout<<"L78"<<endl;
      i++;
    }
    if(i == hexData.size()-1){
      pkt.isValid = false;
      pkt.bSync = false;
      pkt.invalidReason = "Tail of File";
      packets.push_back(pkt);
    }
    i++;
  }
  packets[0].isValid = false;
  packets[0].bSync = false;
  packets[0].invalidReason = "Head of File";
  // 檢查 SYNC 之間的 DATA 是否為 3 的倍數
  // int pcntlast = -1;
  // for (size_t j = 0; j < iyncsPack.size() ; j++) {
    // stringstream ssSync(packets[j].data[0]+packets[j].data[1]+packets[j].data[2]);
    // string Hitdata[3];
    // ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];
    // int Word = GetWordVal(Hitdata);
    // int pcnt   = Word%(1<<15);//#14-#0 PPS counts
    // if(j==0) pcntlast = pcnt;
    // if(pcnt-pcntlast != 1){
      // for (int k = iyncsPack[j]; k < iyncsPack[j+1]; k++) {
        // packets[k].isValid = false;
        // packets[j + 1].invalidReason = Form("Data pcnt lost: %d->%d",pcntlast,pcnt);
      // }
    // }
    // pcntlast = pcnt;
  // }
  for (size_t j = 0; j < packets.size() - 1; j++) {
    if(!packets[j].bSync){
      if(packets[j + 1].data.size()%3!=0){
        packets[j + 1].isValid = false;
        packets[j + 1].invalidReason = "Data length not a multiple of 3";
      }else{
        packets[j + 1].isValid = true;
      }
    }
  }
  
  cout<<"L166: packets array size: "<<packets.size()<<endl;
  return packets;
}
// 主程式

int Decoding(bool bUse) {
    string filename = "/data8/ZDC/EMCal/BeamTest/BeamOn0217/Run255_48MeV_HV210_VF240_268_x60_Pos0mm_0mm_0mm_193129.466LYSO/D1_Sci.hex";
    vector<string> hexData = parseHexFile(filename);
    vector<Packet> packets = extractPackets(hexData);
    double Eff = 0;
    for (const auto& pkt : packets) {
        // cout << "Packet: ";
        // for (const auto& word : pkt.data) {
            // cout << word << " ";
        // }
      if(!pkt.isValid){
        Eff+=1.;
        cout << "[Invalid] Reason: " << pkt.invalidReason << endl;
      }
      for (const auto& word : pkt.data) {
            cout << word << " ";
        }
    };
    Eff /= packets.size();
    cout<<100*Eff<<"%"<<endl;
    return 0;
}

void Decoding(
 const string HexRawSciFileName,
 const string ReadableRawSciFileNameT,
 const string ReadableRawSciFileNameR,
 const string NameRoot,
 const int iDetector = 0
){
  map< int, Sensor > sensorMap = LoadSensorMap(iDetector);
  cout<<"Start to decode the science data\n";
  
  // ifstream inRawSci(HexRawSciFileName.data()); //open Sci hex data
  ofstream ofReadab(ReadableRawSciFileNameT.data()); //open decode Sci txt data
  ofstream ofSyncData(Form("%s/SyncData_DID%d.dat",NameRoot.data(),iDetector)); //open decode Sci txt data
  TFile *f = new TFile(ReadableRawSciFileNameR.data(), "recreate");
  //open decode Sci root data
  
  //operation variable
  string strtmp; string Hitdata[3], WordData;
  
  //data variable
  int Word = 0, SeqSAD, UTCDay, UTCHour, UTCMin, UTCSec, UTCSecS;
  bool GTMID = false;
  int bufferID = -1, bufferID0 = -1, ChannelID, ADCVal, nHits=0;
  int fcnt = -1, dfcnt = -1, fcnt0[3] = {0,0,0}, dfcntR[3]={0,0,0};
  int pcnt = -1, pcnt0 = -1;
  Long64_t iData = 0, eventID;
  bool bHit, HitGTMID,  HGMode;  
  int CITIROCID = 0;
  int DID = iDetector;
  int ADCmax = -1;
  // cout<<DID<<endl;
  // throw;
  vector<bool>  HitGTMID_, HGMode_;
  vector<int>   fcnt_, channel_, bHit_, ADC_, GADC_,ROCID_, GID, iHit;
  vector<int>   iX, iY, iZ;
  vector<float> pX, pY, pZ;
  float Edep;//test variable !! Sum of all channel, 100~120ev/ADC
  vector<int>   vADCSor;
  TTree *t = new TTree("t", "frame data");
  t->Branch("eventID",   &eventID  );
  t->Branch("SeqSAD",    &SeqSAD   );
  t->Branch("UTCDay",    &UTCDay   );
  t->Branch("UTCHour",   &UTCHour  );
  t->Branch("UTCMin",    &UTCMin   );
  t->Branch("UTCSec",    &UTCSec   );
  t->Branch("UTCSecS",   &UTCSecS  );
  t->Branch("pcnt",      &pcnt0     );
  t->Branch("fcnt",      &fcnt0[2]  );
  t->Branch("dfcnt",     &dfcnt     );
  t->Branch("dfcntR",    &dfcntR[2] );
  
  
  t->Branch("nHits",     &nHits     );
  t->Branch("Edep",      &Edep      );
  t->Branch("iHit",      &iHit      );
  t->Branch("Fire",      &bHit_     ); 
  t->Branch("HGTMID",    &HitGTMID_);
  t->Branch("DID",       &DID    );
  t->Branch("ROCID",     &ROCID_    );
  t->Branch("channel",   &channel_ );
  t->Branch("HGMode",    &HGMode_  );
  t->Branch("ADC",        &ADC_     );
  t->Branch("GADC",       &GADC_    );
  t->Branch("ADCmax",       &ADCmax     );
  t->Branch("GID",        &GID       );
  t->Branch("iX",         &iX        );
  t->Branch("iY",         &iY        );
  t->Branch("iZ",         &iZ        );
  t->Branch("pX",         &pX        );
  t->Branch("pY",         &pY        );
  t->Branch("pZ",         &pZ        );
  
  //PPS data
  float BeamPosX = -999,BeamPosY = -999,BeamPosZ = -999,BeamEnergy = -1;
  int   HighVROCA= -1  , HighVROCB = -1, SW2VROCA = -1 , SW2VROCB = -1;
  int   VFROCA = -1, VFROCB = -1;
  float TempEnv = -1, LiveTimeA = -1, LiveTimeB = -1, BufferLTA = -1, BufferLTB = -1, RotateAng = -1;
  t->Branch("BeamPosX",    &BeamPosX   );
  t->Branch("BeamPosY",    &BeamPosY   );
  t->Branch("BeamPosZ",    &BeamPosZ   );
  t->Branch("BeamEnergy",  &BeamEnergy );
  t->Branch("HighVROCA",   &HighVROCA  );
  t->Branch("HighVROCB",   &HighVROCB  );
  t->Branch("SW2VROCA",    &SW2VROCA   );
  t->Branch("SW2VROCB",    &SW2VROCB   );
  t->Branch("VFROCA",      &VFROCA     );
  t->Branch("VFROCB",      &VFROCB     );
  t->Branch("TempEnv",     &TempEnv    );
  t->Branch("LiveTimeA",   &LiveTimeA  );
  t->Branch("LiveTimeB",   &LiveTimeB  );
  t->Branch("BufferLTA",   &BufferLTA  );
  t->Branch("BufferLTB",   &BufferLTB  );
  t->Branch("RotateAng",   &RotateAng  );
  
  vector<string> hexData = parseHexFile(HexRawSciFileName);
  vector<Packet> packets = extractPackets(hexData);
  cout<<"L290"<<endl;
  double Eff = 0, Tot = 0, Loss = 0;
  for (const auto& pkt : packets) {
      // cout << "Packet: ";
      // for (const auto& word : pkt.data) {
          // cout << word << " ";
      // }
    // cout<<pkt.bSync<<" "<<endl;
    Tot += pkt.data.size();
    if(!pkt.isValid){
      Loss+=pkt.data.size();
      cout << "[Invalid] Reason: " << pkt.invalidReason << endl;
    } 
  };
  Eff = ( Loss ) / Tot;
  cout<<"Loss rate : "<<100*Eff<<"%"<<endl;
  stringstream ofiss;
  // while(inRawSci>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]&&iData<198){ // test break line 
  // int ByteIndex = 0;
  
  for (const auto& pkt : packets){
    if(!pkt.isValid){
      ofiss << "[Invalid] Reason: " << pkt.invalidReason << endl;
      continue;
    }else if(pkt.bSync){
      stringstream ssSync;
      for (const auto& word : pkt.data) {
        ssSync << word <<" ";
      }
      // cout<<ssSync.str()<<endl;
      // throw;
      // cout<<pkt.data.size()<<endl;
      // cout<<ssSync.str()<<endl;
       // throw;
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 1
      Word = GetWordVal(Hitdata);
      // transform 3 bytes data to be 1 word value(int)
      //Header (0xCA) - Get Header of Sync. data 

      // cout<<" <--- Sync. Data"<<endl;
      pcnt   = Word%(1<<15);//#14-#0 PPS counts
      GTMID = ((Word>>15)&1); //#15 GTM Master=0, Slave=1
      if(GTMID==1){
        cout<<"Warning: Get the Slave GTM ID"<<endl;
        ofiss<<"Warning: Get the Slave GTM ID"<<endl;
        // throw; // test break line 
      }
      // cout<<"---------------Event time----------------\n";
      // cout<<Hitdata[0]<<Hitdata[1]<<Hitdata[2]<<" ";
      // cout<<"Find Header \"CA\" for Sync. data, GTMID = "<<GTMID<<", pcnt = "<< pcnt <<endl;
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 1
      // ByteIndex+=3;
      Word = GetWordVal(Hitdata); 
      SeqSAD = GetBits(Word,16,23);// #23-#16 Sequence number from CMD-SAD
      UTCDay = GetBits(Word,0,7)*256+GetBits(Word,8,15);//Guess value now!!
      // #15-#8, #7-#0 Sequence number from CMD-SAD 
      // cout<<Hitdata[0]<<Hitdata[1]<<Hitdata[2]<<" ";
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 2
      // ByteIndex+=3;
      Word = GetWordVal(Hitdata);
      UTCHour = GetBits(Word,16,23);//#23-16 UTC-Hour
      UTCMin = GetBits(Word,8,15);//#15-8 UTC-Minute
      UTCSec = GetBits(Word,0,7);//15-8 UTC-Second
      // cout<<Hitdata[0]<<Hitdata[1]<<Hitdata[2]<<" ";
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 3
      // ByteIndex+=3;
      Word = GetWordVal(Hitdata);
      UTCSecS = GetBits(Word,16,23);//#23-16 UTC-Subsecond
      
      BeamPosX = GetBits(Word,0,15);//#Woed 3 15-0 > 
      // if(BeamPosX-65536/2>0) BeamPosX -= 65536;
      ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
      // ByteIndex+=2;
      BeamPosY = GetBits(Word,0,15);//#Word 4 23-8
      // if(BeamPosY-65536/2>0) BeamPosY -= 65536;
      
      ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
      // ByteIndex+=2;
      BeamPosZ = GetBits(Word,0,15);//#Word 4 7-0 5 23-16
      // if(BeamPosZ-65536/2>0) BeamPosZ -= 65536;
      
      ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
      // ByteIndex+=2;
      BeamEnergy = GetBits(Word,0,15,5);//MeV Word 5 15-0
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]; Word=GetWordVal(Hitdata); 
      // ByteIndex+=3;
      HighVROCA = GetBits(Word,16,23);// Word 6 23-16
      HighVROCB = GetBits(Word, 8,15);//Word 6 15-8
      SW2VROCA  = GetBits(Word, 0, 7);//Word 6 7-0
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]; Word=GetWordVal(Hitdata); 
      // ByteIndex+=3;
      SW2VROCB  = GetBits(Word,16,23);// Word 7 23-16
      VFROCA    = GetBits(Word, 8,15);//Word 7 15-8
      VFROCB    = GetBits(Word, 0, 7);//Word 7 7-0
      
      ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
      // ByteIndex+=2;
      TempEnv = GetBits(Word,0,15,8);//Word 8 23-8
      
      for(int iByte=0;iByte<6;iByte++) ssSync>>Hitdata[0];
      // ByteIndex+=6;
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];
      // ByteIndex+=3;
      Word = GetWordVal(Hitdata);
      LiveTimeA = GetBits(Word,12,23,5);//#Word 10 07-0 -> Word 11 23-20
      LiveTimeB = GetBits(Word, 0,11,5);//#Word 11 19-8
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 
      // ByteIndex+=3;
      Word = GetWordVal(Hitdata);
      BufferLTA = GetBits(Word,12,23,5);//#Word 11 07-0 -> Word 12 23-20
      BufferLTB = GetBits(Word, 0,11,5);//#Word 12 19-8
      
      ssSync>>Hitdata[0];
      // ByteIndex++;
      Word = Word=Get1BVal(Hitdata); 
      RotateAng = GetBits(Word, 0, 7);//#Word 12 7-0
      
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//skip #Word 13.1->13.3
      // ByteIndex+=3;
      ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//skip #Word 13.1->13.3
      // ByteIndex+=3;
      
        ofSyncData<<"\n A Sync. data:        --------------\n";
        ofSyncData<<"pcnt:GTMID:SeqSAD:  "<<pcnt<<":"<<GTMID<<":"<<SeqSAD<<"\n";
        ofSyncData<<"Day,Hour:Min:Sec.SubSec:      "<<UTCDay<<","<<UTCHour<<":"<<UTCMin<<":"<<UTCSec<<"."<<UTCSecS<<"\n";
        ofSyncData<<"BeamEnergy     = "<<BeamEnergy<<" MeV"<<endl;
        ofSyncData<<"BeamPos(x,y,z) = ("<<BeamPosX<<","<<BeamPosY<<","<<BeamPosZ<<") mm\n";
        ofSyncData<<"Rotation       = "<<RotateAng<<" degree\n";
        ofSyncData<<"TempEnv        = "<<TempEnv<<" degree C\n";
        ofSyncData<<"HV-SW2-VF, LTROC-LTBuff  A = "<<HighVROCA<<"-"<<SW2VROCA<<"-"<<VFROCA<<", "<<LiveTimeA<<"%-"<<BufferLTA<<"%\n";
        ofSyncData<<"HV-SW2-VF, LTROC-LTBuff  B = "<<HighVROCB<<"-"<<SW2VROCB<<"-"<<VFROCB<<", "<<LiveTimeB<<"%-"<<BufferLTB<<"%\n";

    }else{
      stringstream ssData;
      for (const auto& word : pkt.data) {
        ssData << word <<" ";
      }
      // cout<<ssData.str()<<endl;
      // throw;
      while(ssData>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]){
        Word = GetWordVal(Hitdata);
        if     (((Word>>23)&1) == 0){// cout<<"<---Find Header \"0\" for Event-Hit data"<<endl;
          iHit.push_back(nHits);
          bHit = Word>>(22)&1; //Hit (1=Hit)
          HitGTMID = Word>>(21)&1; HitGTMID_.push_back(HitGTMID);//GTM ID (Master=0, Slave=1)
          CITIROCID = Word>>(20)&1; ROCID_.push_back(CITIROCID);//CITIROC ID (A=0, B=1)
          ChannelID = GetBits(Word,15,19); channel_.push_back(ChannelID);//Channel ID (0~31)
          HGMode    = Word>>(14)&1; HGMode_.push_back(HGMode);//LG=0/HG=1
          ADCVal    = Word%(1<<14); 
          if(ADCVal>11000) ADCVal = ADCVal-16384;
          // if(ADCVal>0||AllowNegADC){
            ADC_.push_back(ADCVal);
            bHit_.push_back(bHit&&ADCVal>=0);
            //ADC value (bit13-0)
            // -1=(1)”11111111111111”,         // 0=”00000000000000”,         // 1=”00000000000001”
            // If the ADC value > 11000(5500x2), it is a negative value.
            GADC_.push_back(HGMode?ADCVal:ADCVal*10);//ganeral ADC count cal. LG ADC to be HG ADC value
            vADCSor.push_back(GADC_[nHits]*100+nHits);
            nHits++;//total number of hits in 1 event time
            Edep+= GADC_[GADC_.size()-1]*100; 
            //Sum up all GADC*100eV!!!test only!!!
            int GIDtmp = NumChs*CITIROCID + ChannelID;
            GID.push_back(GIDtmp);
            iX .push_back(sensorMap[GIDtmp].iX);
            iY .push_back(sensorMap[GIDtmp].iY);
            iZ .push_back(sensorMap[GIDtmp].iZ);
            pX .push_back(sensorMap[GIDtmp].pX);
            pY .push_back(sensorMap[GIDtmp].pY);
            pZ .push_back(sensorMap[GIDtmp].pZ);
            
            // cout<<"Hit?, CiTiROC, GTMID, ch., HG?, ADC: \n";
            // cout<<bHit<<" "<<HitGTMID<<" "<<CITIROCID<<" "<<ChannelID<<" "<<HGMode<<" "<<ADCVal<<"\t"<<(HGMode?ADCVal:ADCVal*10)<<"\n";
            ofReadab<<"event  adc:\t"<<bHit<<";\t"<<HitGTMID<<"; "<<CITIROCID<<"; "<<ChannelID<<"; "<<HGMode<<";\t"<<ADCVal<<endl;//output to txt file 
          // }
        } else if(((Word>>22)&1) == 0){
          // cout<<"<---Find Header \"10\" for Event-Time data"<<endl;
          // for(i=0;i<4;i++) bufferID |= (Word>>(18+i)&1) << i;
          bufferID = (Word%(1<<22))/(1<<18);//Bit21-18, buffer ID
          fcnt = Word%(1<<18);//Bit17-0, Fine counter value (LSb = 3.84 µsec)
          dfcnt = fcnt - fcnt0[2];
          dfcntR[CITIROCID] = fcnt - fcnt0[CITIROCID];
          dfcntR[2] = dfcntR[CITIROCID];
          // cout<<"   bufferID = "<<bufferID<<" , pcnt = "<<pcnt<<" , Fine_count_value = "<< fcnt <<endl;
          ofReadab<<"#event time:\t"<<bufferID<<"\t"<<pcnt<<"\t"<<fcnt<<endl;
          // cout<<"#event time:\t"<<bufferID<<"\t"<<pcnt<<"\t"<<fcnt<<endl;
          
          sort(vADCSor.begin(), vADCSor.end());
          for(size_t i=0;i<vADCSor.size();i++){
            iHit[i] = vADCSor[i]%100;
          }
          
          // if(pcnt0!=-1) 
          if(nHits==0){
            cout<<"Warning: Find a 0 hit event ? at eventID = "<<iData<<endl;
            ofiss<<"Warning: Find a 0 hit event ? at eventID = "<<iData<<endl;
          }else if(iData!=0){
            eventID = iData-1;
            // t->Fill();
            if(pcnt0!=-1){
              auto max_it = max_element(GADC_.begin(), GADC_.end());
              ADCmax = double(*max_it);
              t->Fill();
            }// if((iDetector==2||iDetector==3)&&nHits>1) t->Fill();
          }else{
            cout<<"Warning: Get Start Filling the TTree from 1st event"<<endl;
            cout<<"And Clear the data before 1st event"<<endl;
            cout<<"Totally erase: "<<nHits<<" hits"<<endl;
            ofiss<<"Warning: Get Start Filling the TTree from 1st event"<<endl;
            ofiss<<"And Clear the data before 1st event"<<endl;
            ofiss<<"Totally erase: "<<nHits<<" hits"<<endl;
          }
          iData++;
          bHit_.clear();
          HitGTMID_.clear();
          channel_.clear();
          HGMode_.clear();
          ADC_.clear();
          GADC_.clear();
          ROCID_.clear();
          GID.clear();
          iX.clear();
          iY.clear();
          iZ.clear();
          pX.clear();
          pY.clear();
          pZ.clear();
          iHit.clear();
          vADCSor.clear();
          nHits = 0;
          Edep = 0;
          fcnt0[CITIROCID] = fcnt;
          fcnt0[2] = fcnt0[CITIROCID];
          pcnt0 = pcnt;
          // if(iData>200) break;
          // throw;
        }
      }
    }
    
  }
  packets.clear();
  hexData.clear();

  if(pcnt0!=-1) t->Fill();
  f->Write();// save the TTree into the tfile
  f->Close();// tfile closeed
  
  // lost = ByteIndex; 
  f->Close();
  delete t;

  cout<<"Finish to decode the science data\n";
  ofstream ott(Form("%s/ConvertProblem_DID%d.dat",NameRoot.data(),iDetector) );
  ofstream ott2(Form("%s/ConvertProblemNew_DID%d.dat",NameRoot.data(),iDetector) );
  ott<<Eff*100.<<"\t"<<Loss<<" "<<Tot<<endl;
  ott<<"Err : "<<"\t Loss / Tot"<<endl;
  ott<<Loss<<"/"<<Tot<<" : "<<(Loss*100.)/(Tot)<<"%"<<endl;
  ott<<"-----------------------------\n"<<endl;
  ott<<HexRawSciFileName.data()<<endl;
  ott<<ofiss.str()<<endl;
  ott<<"-----------------------------\n"<<endl;

  ott2<<Eff*100.<<"\t"<<Loss<<" "<<Tot<<endl;
  ott2<<"Err : "<<"\t Loss / Tot"<<endl;
  ott2<<Loss<<"/"<<Tot<<" : "<<(Loss*100.)/(Tot)<<"%"<<endl;
  ott2<<"-----------------------------\n"<<endl;
  ott2<<HexRawSciFileName.data()<<endl;
  ott2<<ofiss.str()<<endl;
  ott2<<"-----------------------------\n"<<endl;
  ott.close();
  ott2.close();
  ofReadab.close();
  ofSyncData.close();
  // cout<<(Form("root -l -b -q Read.C+\\(\\\"%s\\\"\\)",NameRoot.data()))<<endl;
}
void Decoding(){
  cout<<"Finished compiling of Decoding.C+"<<endl;
}