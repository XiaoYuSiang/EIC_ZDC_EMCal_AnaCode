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
stringstream GetLastSyncData(ifstream &in, int &Cosume,int &lost,int &TotalSync){
  std::vector<string> buffer;
  string strtmp;
  stringstream ss;
  cout<<"searching fine sync data"<<endl;
  int losttmp =0;
  while(in>>strtmp){
    buffer.push_back(strtmp); Cosume++;
    if(strtmp=="CA"){ lost++; TotalSync++; losttmp++;}
    // cout<<strtmp.data()<<endl;
    // if(buffer.size()>45) buffer.pop_front();
    if(buffer.size()>=45&&buffer[buffer.size()-45]=="CA"&&(buffer[buffer.size()-3]+buffer[buffer.size()-2]+buffer[buffer.size()-1])=="F2F5FA"){
      for(size_t i =buffer.size()-45;i<buffer.size();i++){
        ss<<buffer[i].data()<<" ";
        // cout<<buffer[i].data()<<endl;
      }
      lost--;
      return ss;
    }
  }
  // cout<<ss.str()<<endl;
  cout<<"searching fine sync data : loss \"CA\" x "<<losttmp<<endl;
  ss<<"NAN/NAN";
  return ss;
}
stringstream GetLastSyncDataCA(ifstream &in,string &Str1,string &Str2,string &Str3, int &Cosume,int &lost,int &TotalSync){
  vector<string> buffer;
  string strtmp;
  stringstream ss;
  // cout<<"re searching fine sync data"<<endl;
  if(in>>Str2>>Str3) {
    ss<<"END";
    return ss;
  }
  // cout<<Str1<<" "<<Str2<<" "<<Str3<<" ";
  buffer.push_back(Str1);
  buffer.push_back(Str2);
  buffer.push_back(Str3);
  for(int i=0;i<42&&in>>strtmp;i++){
    if(!(in>>strtmp)){
      ss<<"END";
      return ss;
    }
    buffer.push_back(strtmp);
    Cosume++;
    if((buffer[buffer.size()-3]+buffer[buffer.size()-2]+buffer[buffer.size()-1])=="F2F5FA"){
      break;
    }
    // cout<<"Add "<<strtmp<<endl;
  }
  if(buffer.size()==45&&(buffer[42]+buffer[43]+buffer[44])=="F2F5FA"){
    for(size_t i =0;i<45;i++){
      ss<<buffer[i].data()<<" ";
      // cout<<buffer[i].data()<<endl;
    }
    lost--;
    ss>>Str1>>Str2>>Str3;
    return ss;
  }else{ 
    cout<<"Error: Find Sync data package not qulify! Check re search!"<<endl;
    ss = GetLastSyncData(in,Cosume,lost,TotalSync);
    if(ss.str()!=string("NAN")) ss>>Str1>>Str2>>Str3;
  }
  return ss;
}
void DecodeSciRaw(
 const string HexRawSciFileName,
 const string ReadableRawSciFileNameT,
 const string ReadableRawSciFileNameR,
 const string NameRoot,
 const int iDetector = 0
){
  map< int, Sensor > sensorMap = LoadSensorMap(iDetector);
  cout<<"Start to decode the science data\n";
  
  ifstream inRawSci(HexRawSciFileName.data()); //open Sci hex data
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
  
  
  stringstream ofiss;
  // while(inRawSci>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]&&iData<198){ // test break line 
  int ByteIndex = 0, TotalSync = 0, lost = 0;
  bool SetSyncMode = true;
  lost++;
  stringstream strSyncData = GetLastSyncData(inRawSci,ByteIndex,lost,TotalSync);
  bool TrashData[2] = {false,false};
  if(strSyncData.str()!=string("NAN")){
    // cout<<strSyncData.str()<<endl;
    while(inRawSci>>Hitdata[0]){ // Get 3 byte data for #Word 0      
      if(Hitdata[0]=="BadLine"){
        strSyncData = GetLastSyncData(inRawSci,ByteIndex,lost,TotalSync);
        SetSyncMode = true;
      }
      // if (!SetSyncMode) {
        // ByteIndex+=3;
        // cout<<Hitdata[0]<<" "<<Hitdata[1]<<" "<<Hitdata[2]<<endl;
      // }
      // transform 3 bytes data to be 1 word value(int)
      if(Hitdata[0]=="CA"){
        //Header (0xCA) - Get Header of Sync. data 
        TotalSync++; lost++;
        int bID0 = ByteIndex;
        stringstream ssSync = GetLastSyncDataCA(inRawSci,Hitdata[0],Hitdata[1],Hitdata[2],ByteIndex,lost,TotalSync);
        if(ssSync.str()==string("NAN")){
          TrashData[1] = true;
          break;
        }else if(ssSync.str()==string("END")){
          break;
        }
        Word = GetWordVal(Hitdata);
        // lost+=-(ByteIndex-bID0)+42+1;
        if(SetSyncMode){
          ssSync.str(strSyncData.str());
          SetSyncMode = false;
        }
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
        ByteIndex+=3;
        Word = GetWordVal(Hitdata); 
        SeqSAD = GetBits(Word,16,23);// #23-#16 Sequence number from CMD-SAD
        UTCDay = GetBits(Word,0,7)*256+GetBits(Word,8,15);//Guess value now!!
        // #15-#8, #7-#0 Sequence number from CMD-SAD 
        // cout<<Hitdata[0]<<Hitdata[1]<<Hitdata[2]<<" ";
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 2
        ByteIndex+=3;
        Word = GetWordVal(Hitdata);
        UTCHour = GetBits(Word,16,23);//#23-16 UTC-Hour
        UTCMin = GetBits(Word,8,15);//#15-8 UTC-Minute
        UTCSec = GetBits(Word,0,7);//15-8 UTC-Second
        // cout<<Hitdata[0]<<Hitdata[1]<<Hitdata[2]<<" ";
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 3
        ByteIndex+=3;
        Word = GetWordVal(Hitdata);
        UTCSecS = GetBits(Word,16,23);//#23-16 UTC-Subsecond
        
        BeamPosX = GetBits(Word,0,15);//#Woed 3 15-0 > 
        // if(BeamPosX-65536/2>0) BeamPosX -= 65536;
        ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
        ByteIndex+=2;
        BeamPosY = GetBits(Word,0,15);//#Word 4 23-8
        // if(BeamPosY-65536/2>0) BeamPosY -= 65536;
        
        ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
        ByteIndex+=2;
        BeamPosZ = GetBits(Word,0,15);//#Word 4 7-0 5 23-16
        // if(BeamPosZ-65536/2>0) BeamPosZ -= 65536;
        
        ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
        ByteIndex+=2;
        BeamEnergy = GetBits(Word,0,15,5);//MeV Word 5 15-0
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]; Word=GetWordVal(Hitdata); 
        ByteIndex+=3;
        HighVROCA = GetBits(Word,16,23);// Word 6 23-16
        HighVROCB = GetBits(Word, 8,15);//Word 6 15-8
        SW2VROCA  = GetBits(Word, 0, 7);//Word 6 7-0
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2]; Word=GetWordVal(Hitdata); 
        ByteIndex+=3;
        SW2VROCB  = GetBits(Word,16,23);// Word 7 23-16
        VFROCA    = GetBits(Word, 8,15);//Word 7 15-8
        VFROCB    = GetBits(Word, 0, 7);//Word 7 7-0
        
        ssSync>>Hitdata[0]>>Hitdata[1]; Word=Get2BVal(Hitdata); 
        ByteIndex+=2;
        TempEnv = GetBits(Word,0,15,8);//Word 8 23-8
        
        for(int iByte=0;iByte<6;iByte++) ssSync>>Hitdata[0];
        ByteIndex+=6;
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];
        ByteIndex+=3;
        Word = GetWordVal(Hitdata);
        LiveTimeA = GetBits(Word,12,23,5);//#Word 10 07-0 -> Word 11 23-20
        LiveTimeB = GetBits(Word, 0,11,5);//#Word 11 19-8
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//#Word 
        ByteIndex+=3;
        Word = GetWordVal(Hitdata);
        BufferLTA = GetBits(Word,12,23,5);//#Word 11 07-0 -> Word 12 23-20
        BufferLTB = GetBits(Word, 0,11,5);//#Word 12 19-8
        
        ssSync>>Hitdata[0];
        ByteIndex++;
        Word = Word=Get1BVal(Hitdata); 
        RotateAng = GetBits(Word, 0, 7);//#Word 12 7-0
        
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//skip #Word 13.1->13.3
        ByteIndex+=3;
        ssSync>>Hitdata[0]>>Hitdata[1]>>Hitdata[2];//skip #Word 13.1->13.3
        ByteIndex+=3;
        
        // bool GoodSyncData = false;
        // string Tail = Hitdata[0]+Hitdata[1]+Hitdata[2];
        // if(Tail!="F2F5FA"){
          // for(int itail = 0;itail<3; itail++){
            // Hitdata[0] = Hitdata[1]; Hitdata[1] = Hitdata[2];
            // inRawSci>>Hitdata[2];
            // ByteIndex++;
            // Tail = Hitdata[0]+Hitdata[1]+Hitdata[2];
            // if(Tail!="F2F5FA"){
              // if(itail==2) GoodSyncData = 1;
              // break;
            // } 
          // }
        // }
        // if(GoodSyncData){
          ofSyncData<<"\n A Sync. data:        --------------\n";
          ofSyncData<<"pcnt:GTMID:SeqSAD:  "<<pcnt<<":"<<GTMID<<":"<<SeqSAD<<"\n";
          ofSyncData<<"Day,Hour:Min:Sec.SubSec:      "<<UTCDay<<","<<UTCHour<<":"<<UTCMin<<":"<<UTCSec<<"."<<UTCSecS<<"\n";
          ofSyncData<<"BeamEnergy     = "<<BeamEnergy<<" MeV"<<endl;
          ofSyncData<<"BeamPos(x,y,z) = ("<<BeamPosX<<","<<BeamPosY<<","<<BeamPosZ<<") mm\n";
          ofSyncData<<"Rotation       = "<<RotateAng<<" degree\n";
          ofSyncData<<"TempEnv        = "<<TempEnv<<" degree C\n";
          ofSyncData<<"HV-SW2-VF, LTROC-LTBuff  A = "<<HighVROCA<<"-"<<SW2VROCA<<"-"<<VFROCA<<", "<<LiveTimeA<<"%-"<<BufferLTA<<"%\n";
          ofSyncData<<"HV-SW2-VF, LTROC-LTBuff  B = "<<HighVROCB<<"-"<<SW2VROCB<<"-"<<VFROCB<<", "<<LiveTimeB<<"%-"<<BufferLTB<<"%\n";
        // }
        // else{
          // ofiss<<"Error: Can't find Tail of Sync data!!! Problem last ByteIndex:  "<<ByteIndex<<"  "<<Tail.data()<<endl;
          // cout<<"Error: Can't find Tail of Sync data!!! Problem last ByteIndex:  "<<ByteIndex<<"  "<<Tail.data()<<endl;
          // strSyncData = GetLastSyncData(inRawSci);
        // }
        
      }else{
        if(!(inRawSci>>Hitdata[1]>>Hitdata[2])) break;
        Word = GetWordVal(Hitdata);
        if     (((Word>>23)&1) == 0){
          // cout<<"<---Find Header \"0\" for Event-Hit data"<<endl;
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
        }else if(((Word>>22)&1) == 0){
          // cout<<"<---Find Header \"10\" for Event-Time data"<<endl;
          // for(i=0;i<4;i++) bufferID |= (Word>>(18+i)&1) << i;
          bufferID = (Word%(1<<22))/(1<<18);//Bit21-18, buffer ID
          fcnt = Word%(1<<18);//Bit17-0, Fine counter value (LSb = 3.84 µsec)
          dfcnt = fcnt - fcnt0[2];
          // if(dfcnt<0) pcnt++;
          dfcntR[CITIROCID] = fcnt - fcnt0[CITIROCID];
          dfcntR[2] = dfcntR[CITIROCID];
          // cout<<"   bufferID = "<<bufferID<<" , pcnt = "<<pcnt<<" , Fine_count_value = "<< fcnt <<endl;
          ofReadab<<"#event time:\t"<<bufferID<<"\t"<<pcnt<<"\t"<<fcnt<<endl;
          // cout<<"#event time:\t"<<bufferID<<"\t"<<pcnt<<"\t"<<fcnt<<endl;
          
          //Save datas and clear the data and vec.
          
          // int maxGADC = 0;
          // vector<int> GADCPlusiHit;
          // for(int iH = 0; iH<nHits;iH++){
            // GADC_[iH]=GADC_[iH]*100;//+iH;
            // cout<<GADC_[iH]<<endl;
            // GADCPlusiHit.push_back(GADC_*100+iHit);
          // }
          
          sort(vADCSor.begin(), vADCSor.end());
          for(size_t i=0;i<vADCSor.size();i++){
            iHit[i] = vADCSor[i]%100;
          }
          // int   *tmp_bHit_      = bHit_.data();
          // bool  *tmp_HitGTMID_  = HitGTMID_.data();
          // bool  *tmp_HGMode_    = HGMode_.data();
          // int   *tmp_ROCID_     = ROCID_.data();
          // int   *tmp_channel_   = channel_.data();
          // int   *tmp_ADC_       = ADC_.data();
          // int   *tmp_GADC_      = GADC_.data();
          // int   *tmp_GID        = GID.data();
          // int   *tmp_iX         = iX.data();
          // int   *tmp_iY         = iY.data();
          // int   *tmp_iZ         = iZ.data();
          // float *tmp_pX         = pX.data();
          // float *tmp_pY         = pY.data();
          // float *tmp_pZ         = pZ.data();

          // for(int iH = 0; iH<nHits;iH++){
            // int pos = iHit[iH];
            // bHit_[pos] = tmp_bHit_[pos];
            // HitGTMID_[pos] = tmp_HitGTMID_[pos];
            // HGMode_[pos] = tmp_HGMode_[pos];
            // ROCID_[pos] = tmp_ROCID_[pos];
            // channel_[pos] = tmp_channel_[pos];
            // ADC_[pos] = tmp_ADC_[pos];
            // GADC_[pos] = tmp_GADC_[pos];
            // GID[pos] = tmp_GID[pos];
            // iX[pos] = tmp_iX[pos];
            // iY[pos] = tmp_iY[pos];
            // iZ[pos] = tmp_iZ[pos];
            // pX[pos] = tmp_pX[pos];
            // pY[pos] = tmp_pY[pos];
            // pZ[pos] = tmp_pZ[pos];
          // }
          
          
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
        }else {
          // lost+=2;
          cout<<"Error: Can't find Header!!! Problem header: "<<ByteIndex<<"  "<<Hitdata[0].data()<<endl;
          cout<<"  Try: consume 1 bytes data to revise!"<<endl;
          ofiss<<"Error: Can't find Header!!! Problem header:  "<<ByteIndex<<"  "<<Hitdata[0].data()<<endl;
          ofiss<<"  Try: consume 1 bytes data to revise!"<<endl;
          // inRawSci>>Hitdata[0]>>Hitdata[1];
          // ByteIndex+=2;
          // throw;
        }
      }
      
    }
    if(pcnt0!=-1) t->Fill();
    f->Write();// save the TTree into the tfile
    f->Close();// tfile closeed
  }else{
    TrashData[0] = true;
    // lost = ByteIndex; 
    f->Close();
  }
  cout<<"Finish to decode the science data\n";
  ofstream ott(Form("%s/ConvertProblem_DID%d.dat",NameRoot.data(),iDetector) );
  if(TrashData[0]) {
    ott<<lost<<"/"<<TotalSync<<" : "<<(lost*100.)/(TotalSync)<<"%"<<endl;
    cout<<"Error: "<<HexRawSciFileName<<" is a trash pre data!!!"<<endl;
    cout<<"No sync data is found!"<<endl;
    ott<<"Error: "<<HexRawSciFileName<<" is a trash pre data!!!"<<endl;
    ott<<"No sync data is found!"<<endl;
  }else if(TrashData[1]) {
    ott<<lost<<"/"<<TotalSync<<" : "<<(lost*100.)/(TotalSync)<<"%"<<endl;
    cout<<"Error: "<<HexRawSciFileName<<" is a trash sec data!!!"<<endl;
    cout<<"No sync data is found!"<<endl;
    ott<<"Error: "<<HexRawSciFileName<<" is a trash sec data!!!"<<endl;
    ott<<"No sync data is found!"<<endl;
  }else{
    ott<<lost<<"/"<<TotalSync<<" : "<<(lost*100.)/(TotalSync)<<"%"<<endl;
    cout<<lost<<"/"<<TotalSync<<" : "<<(lost*100.)/(TotalSync)<<"%"<<endl;
    ott<<"-----------------------------\n"<<endl;
    ott<<HexRawSciFileName.data()<<endl;
    ott<<ofiss.str()<<endl;
    ott<<"-----------------------------\n"<<endl;
  }
  ott.close();
  // cout<<(Form("root -l -b -q Read.C+\\(\\\"%s\\\"\\)",NameRoot.data()))<<endl;
}
void DecodeSciRaw(){
  cout<<"Finished compiling of DecodeSciRaw.C+"<<endl;
}