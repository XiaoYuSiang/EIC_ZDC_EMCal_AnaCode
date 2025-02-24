root -b -q -l CompilarV2.C
wait
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run059_HV17_VF_290_290__x60_BG_164849.127PbWO4.bin\"","\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run059_HV17_VF_290_290__x60_BG_164849.127PbWO4.bin\"","\"\"","\"\"","\"\"",29\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run060_HV18_VF_330_330__x60_BG_165731.985PbWO4.bin\"","\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run060_HV18_VF_330_330__x60_BG_165731.985PbWO4.bin\"","\"\"","\"\"","\"\"",29\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run061_HV18_VF_330_330__x60_Co60_170255.130PbWO4.bin\"","\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run061_HV18_VF_330_330__x60_Co60_170255.130PbWO4.bin\"","\"\"","\"\"","\"\"",29\) &
sleep 0 &&root -l -b -q Read.C\("\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run062_HV17_VF_290_290__x60_Co60_170546.651PbWO4.bin\"","\"/data8/ZDC/EMCal/PbWO4SiPM/250210/Run062_HV17_VF_290_290__x60_Co60_170546.651PbWO4.bin\"","\"\"","\"\"","\"\"",29\) &
wait
echo "Finish the scripe"

