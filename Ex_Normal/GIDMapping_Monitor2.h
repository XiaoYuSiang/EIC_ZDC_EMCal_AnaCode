#pragma once

#include <typeinfo>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <map>
#include <TString.h>
using namespace std;
const int NumAllChs_T2 = 64, NumChs_T2    = 32, NumROCs_T2   = 2;
const int NumiX_T2 = 8; const int NumiY_T2 = 8;
int proGID_T2[NumAllChs_T2] ={//general Index of channels in ROC A/B
15,
16,
14,
17,
13,
18,
12,
19,
11,
20,
10,
21,
9,
22,
8,
23,
7,
24,
6,
25,
5,
26,
4,
27,
3,
28,
2,
29,
1,
30,
0,
31,
  48, 47, 49, 46, 50, 45, 51, 44, 52, 43, 53, 42, 54, 41, 55, 40, 56, 39, 57, 38, 58, 37, 59, 36, 60, 35, 61, 34, 62, 33, 63, 32
};
int proiX_T2[NumAllChs_T2] ={//index of X at channel 
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
};
int proiY_T2[NumAllChs_T2] ={//index of Y at channel 
32,
31,
30,
29,
28,
27,
26,
25,
24,
23,
22,
21,
20,
19,
18,
17,
16,
15,
14,
13,
12,
11,
10,
9,
8,
7,
6,
5,
4,
3,
2,
1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
 };
int proiZ_T2[NumAllChs_T2] ={//index of Y at channel 
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
 };
float propX_T2[NumAllChs_T2] ={//position of Y at channel 
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
18.17,
  -11.318,-9.219,-7.12,-5.021,-2.922,-0.823,1.276,3.375,5.474,7.573,9.672,11.771,13.87,15.969,18.068,20.167,22.266,24.365,26.464,28.563,30.662,32.761,34.86,36.959,39.058,41.157,43.256,45.355,47.454,49.553,51.652,53.751

};
float propY_T2[NumAllChs_T2] ={//position of Y at channel 
165.501,
163.402,
161.303,
159.204,
157.105,
155.006,
152.907,
150.808,
148.709,
146.61,
144.511,
142.412,
140.313,
138.214,
136.115,
134.016,
131.917,
129.818,
127.719,
125.62,
123.521,
121.422,
119.323,
117.224,
115.125,
113.026,
110.927,
108.828,
106.729,
104.63,
102.531,
100.432,
  136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02, 136.02
};

float propZ_T2[NumAllChs_T2] ={//position of Y at channel 
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
143.4,
  145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,145.8,
};