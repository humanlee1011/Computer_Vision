#include <iostream>
#include <Eigen/Dense>
#include "CImg.h"
#include "hough.h"
#include "canny.h"
#include <vector>
#include <algorithm>
#include "perspectiveTransform.h"

using namespace cimg_library;
using namespace std;
using namespace Eigen;

void perspectiveTransform(CImg<float>& ori, CImg<float>& result, vector<pair<int, int> >& src) {
  vector<pair<int, int> > dst;
  
  result.assign(ori._width * 0.7, ori._height * 0.7, 1, 3);
  result.fill(255.0f);  

  dst.push_back(make_pair(1, 1));
  dst.push_back(make_pair(result._width - 1, 1));
  dst.push_back(make_pair(1, result._height - 1));
  dst.push_back(make_pair(result._width - 1, result._height - 1));
  sortPoint(src);
  cout << "Point: " << src[0].first << " " << src[0].second << endl;
  cout << "Point: " << src[1].first << " " << src[1].second << endl;
  cout << "Point: " << src[2].first << " " << src[2].second << endl;
  cout << "Point: " << src[3].first << " " << src[3].second << endl;
  _perspectiveTransform(ori, result, src, dst);
}


void _perspectiveTransform(CImg<float>& ori, CImg<float>& result, vector<pair<int, int> >& src, vector<pair<int, int> >& dst) {
  // 1. 通过src和dst将透视矩阵求出来
  // 2. 用透视矩阵乘以各点，得到变换后的点（注意转换后不为整数的点）
  // 3. 输出最终图像
  if (src.size() != 4 || dst.size() != 4)
    cout << "Error: there are no 4 points.\n";

  cout << "1 \n";
  // 创建UV矩阵
  // UV  = [u1, v1, u2, v2, u3, v3, u4, v4]
  MatrixXf UV(8, 1);
  UV << dst[0].first, dst[0].second,
        dst[1].first, dst[1].second,
        dst[2].first, dst[2].second,
        dst[3].first, dst[3].second;
  // 获得A矩阵
  MatrixXf A(8, 8);
 A << src[0].first, src[0].second, 1, 0, 0, 0, -src[0].first * dst[0].first, -src[0].second * dst[0].first,
    0, 0, 0, src[0].first, src[0].second, 1, -src[0].first * dst[0].second, -src[0].second * dst[0].second,
    src[1].first, src[1].second, 1, 0, 0, 0, -src[1].first * dst[1].first, -src[1].second * dst[1].first,
    0, 0, 0, src[1].first, src[1].second, 1, -src[1].first * dst[1].second, -src[1].second * dst[1].second,
    src[2].first, src[2].second, 1, 0, 0, 0, -src[2].first * dst[2].first, -src[2].second * dst[2].first,
    0, 0, 0, src[2].first, src[2].second, 1, -src[2].first * dst[2].second, -src[2].second * dst[2].second,
    src[3].first, src[3].second, 1, 0, 0, 0, -src[3].first * dst[3].first, -src[3].second * dst[3].first,
    0, 0, 0, src[3].first, src[3].second, 1, -src[3].first * dst[3].second, -src[3].second * dst[3].second;
  //矩阵求逆
  // cout << "A: " << A << endl;
  A = A.inverse();
  // cout << "A-1: " << A << endl;
  //求出透视矩阵参数
  MatrixXf M(8, 1);
  M = A * UV;
  //将参数转化成3*3的方阵
  MatrixXf param(3, 3);
  param << M(0, 0), M(1, 0), M(2, 0), M(3, 0),
          M(4, 0), M(5, 0), M(6, 0), M(7, 0), 1;
  // 透视矩阵求拟
  param = param.inverse();
  // cout << "parameter: \n" << param << endl;
  cimg_forXY(result, u, v) {
    MatrixXf xy(3, 1);
    MatrixXf uv(3, 1);
    uv << u, v, 1;
    xy = param * uv;
    float x = xy(0, 0) / xy(2, 0),
        y = xy(1, 0) / xy(2, 0);
    int i = (int)x, j = (int)y;
    float a = x - i, 
          b = y - j;

    // 判断出界
    if (x >= ori._width - 1 || x <= 0 || y >= ori._height - 1 || y <= 0)
      continue;
    // 双线性插值
    result(u, v, 0) = ori(i, j, 0) * (1 - a) * (1 - b) + 
              ori(i + 1, j, 0) * a * (1 - b) + 
              ori(i, j + 1, 0) * (1 - a) * b +
              ori(i + 1, j + 1, 0) * a * b;

    result(u, v, 1) = ori(i, j, 1) * (1 - a) * (1 - b) + 
              ori(i + 1, j, 1) * a * (1 - b) + 
              ori(i, j + 1, 1) * (1 - a) * b +
              ori(i + 1, j + 1, 1) * a * b;

    result(u, v, 2) = ori(i, j, 2) * (1 - a) * (1 - b) + 
            ori(i + 1, j, 2) * a * (1 - b) + 
            ori(i, j + 1, 2) * (1 - a) * b +
            ori(i + 1, j + 1, 2) * a * b;
    // if (x % 100 == 0 && y % 100 == 0)
    //   cout << "ii: " << ori(x, y, 0) << " " << ori(x, y, 1) << " " << ori(x, y, 2) << endl;

  }
  result.display("Morphing");
}

void sortPoint(vector<pair<int, int> >& point) {
  // pair<int, int> p = point[0];
  vector<pair<double, int> > dist;
  dist.push_back(make_pair(calDist(point[0], point[1]), 1));
  dist.push_back(make_pair(calDist(point[0], point[2]), 2));
  dist.push_back(make_pair(calDist(point[0], point[3]), 3));
  sort(dist.begin(), dist.end(), myComp);

  vector<pair<int, int> > res;
  //初顶点在右侧，向左倾斜
  if (point[dist[0].second].first < point[0].first) {
    res.push_back(point[dist[0].second]);
    res.push_back(point[0]);
    res.push_back(point[dist[2].second]);
    res.push_back(point[dist[1].second]);
  }
  else { // 初顶点在左侧，向右倾斜
    res.push_back(point[0]);
    res.push_back(point[dist[0].second]);
    res.push_back(point[dist[1].second]);
    res.push_back(point[dist[2].second]);
  }
  point = res;
}

bool myComp(pair<double, int>& p1, pair<double, int>& p2) {
  return p1.first < p2.first;
}

double calDist(pair<int, int>& p1, pair<int, int> p2) {
  return sqrt((p1.first - p2.first) * (p1.first - p2.first) + (p1.second - p2.second) * (p1.second - p2.second));
}

