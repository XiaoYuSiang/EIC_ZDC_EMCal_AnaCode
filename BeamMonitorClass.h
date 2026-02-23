
#include "TVector3.h"
struct BMHit {
  double x, y, z, w;
public:
  BMHit(double px=-9999,double py=-9999,double pz = 0,double wt = 0){
    x = px; y = py; z = pz; w = wt;
  }
  void Show(){
    cout<<x<<" "<< y<<" "<< z<<" "<< w<<endl;
  }
  TVector3 Vec() const {
    return TVector3(x, y, z);
  }
};

vector<BMHit> GetCombination(
    vector<BMHit> pXZ,
    vector<BMHit> pYZ
) {
    vector<BMHit> results;
    for (const auto& posX : pXZ) {
        for (const auto& posY : pYZ) {
            double x = posX.x;
            double z1 = posX.z;
            double y = posY.y;
            double z2 = posY.z;
            double z_avg = (z1 + z2) / 2.0;
            double wt = posX.w * posY.w;
            results.push_back({x, y, z_avg,wt});
        }
    }
    return results;
}

double Distance(const BMHit& p1, const BMHit& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    double dz = p1.z - p2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

void CheckDataRM(vector<BMHit>& data, const BMHit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 8 || abs(data[i].y - center.y) > 8 || abs(data[i].z - center.z) > 8) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
void CheckDataRMZDC1(vector<BMHit>& data, const BMHit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 25 || abs(data[i].y - center.y) > 25) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
void CheckDataRMZDC2(vector<BMHit>& data, const BMHit& center) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (abs(data[i].x - center.x) > 50 || abs(data[i].y - center.y) > 50) {
          data.erase(data.begin() + i);
          i -= 2;
      }
    }
}
/*
data.erase(remove_if(data.begin(), data.end(),
                     [&](const BMHit& hit) { return shouldRemove(hit, center); }),
           data.end());
*/
bool shouldRemove(const BMHit& data, const BMHit& center) {
    return (abs(data.x - center.x) > 8 || abs(data.y - center.y) > 8 || abs(data.z - center.z) > 8);
}

double AverageDistance(const vector<BMHit>& points,const BMHit& center) {
    size_t n = points.size();
    if (n < 2) {
        return 0;
    }
    double totalDistance = 0.0;
    size_t count = 0;
    for (const auto& p : points) {
        totalDistance += pow(Distance(p, center),2);
        ++count;
    }
    return sqrt(totalDistance / count);
}

BMHit GetCenter(const vector<BMHit>& points) {
    size_t n = points.size();
    if (n == 0) {
        return {0, 0, 0};
    }
    BMHit result = {0, 0, 0};
    double SumW = 0;
    for (const auto& p : points) {
      result.x += p.x * p.w;
      result.y += p.y * p.w;
      result.z += p.z * p.w;
      SumW+=p.w;
    }
    result.x /= SumW;
    result.y /= SumW;
    result.z /= SumW;
    return result;
}
