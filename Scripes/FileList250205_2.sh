root -b -q -l CompilarV2.C
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/01_CosmicRay_HV408V_FV280_300_LG_x60_020735.778LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/01_CosmicRay_HV408V_FV280_300_LG_x60_020735.778LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/01_CosmicRay_HV30V_FV320_020735.778Monitor1.bin\"","\"\"","\"\"",2\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/02_CosmicRay_HV200V_VF290_330_LG_x1_123858.773LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/02_CosmicRay_HV200V_VF290_330_LG_x1_123858.773LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/02_CosmicRay_HV30V_VF290_LG_x4_123858.773Monitor1.bin\"","\"\"","\"\"",2\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/03_CosmicRay_HV210V_VF310_360_LG_x1_160946.486LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/03_CosmicRay_HV210V_VF310_360_LG_x1_160946.486LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/03_CosmicRay_HV30V_VF280_LG_x4_160946.486Monitor1.bin\"","\"\"","\"\"",2\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/04_CosmicRay_HV388V_FV280_300_LG_x20_191512.683LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/04_CosmicRay_HV388V_FV280_300_LG_x20_191512.683LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/04_CosmicRay_HV30V_FV320_191512.683Monitor1.bin\"","\"\"","\"\"",2\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/05_CosmicRay_HV388V_FV250_270_LG_x4_222515.710LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/05_CosmicRay_HV388V_FV250_270_LG_x4_222515.710LYSO.bin\"","\"/data8/ZDC/EMCal/LYSO2ABMCosmic/250205/05_CosmicRay_HV30V_FV320_222515.710Monitor1.bin\"","\"\"","\"\"",2\) &
wait
echo "Finish the scripe"

