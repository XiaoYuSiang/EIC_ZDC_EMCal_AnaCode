#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

void CompilarV2(){
  gSystem->SetBuildDir("./tmpdir/", kTRUE);
  system("cp ./GIDMapping.h ./tmpdir/GIDMapping.h");
  const int NScripes = 5;
  string scripeNames[NScripes] = {"BinToHex","SelectScidata","DecodeSciRaw","ReConstruct","DrawADC"};
  if(false){
    string cmdline = "";
    for(int i=0; i <NScripes;i++)
      cmdline+=string(Form("\(root -l -b -q %s.C+)&",scripeNames[i].data()));
    cmdline +="wait";
    system(cmdline.data());
  }
  // gSystem->CompileMacro("BinToHex.C", "k");
  // gSystem->CompileMacro("SelectScidata.C", "k");
  // gSystem->CompileMacro("DecodeSciRaw.C", "k");
  // gSystem->CompileMacro("ReConstruct.C", "k");
  // gSystem->CompileMacro("HLGScale.C", "k");
  // gSystem->CompileMacro("ReCombineADC.C", "k");
  // gSystem->CompileMacro("FitPeakCode.C", "k");
  // gSystem->CompileMacro("ADCShow.C", "k");
  // gSystem->CompileMacro("DrawADC.C", "k");
  // gSystem->CompileMacro("PositionEnergy.C", "k");
  /* gSystem->CompileMacro("FindFirstBin.C", "k");*/
  // gSystem->CompileMacro("DrawEnerCollRatio.C", "k");
  // gSystem->CompileMacro("DrawEnerCollRatio2.C", "k");
  // gSystem->CompileMacro("match.C", "k");
  // gSystem->CompileMacro("ShowEdepVsPeakChannel.C", "k");
  /* gSystem->CompileMacro("PackageTest.C", "k");*/
  // gSystem->CompileMacro("PackageTestReEnergy.C", "k");
  cout<<"Finish to Compile all scripes"<<endl;
  throw;
}
