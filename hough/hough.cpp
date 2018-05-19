#include <iostream>
#include "CImg.h"
#include "canny.h"
#include "hough.h"
#include <cmath>
#include <cstring>

#define PI 3.1415926535
#define R_STEP 1
#define THETA_STEP 8
#define NEIGH_ZONE  4.0f
#define NEIGH_ZONE_CIRCLE 20.0f

using namespace cimg_library;
using namespace std;
//问题：
// 1. 垂直于x轴的直线检测不出来
// 2.

void HoughLine(CImg<float>& img, CImg<float>& edge, CImg<float>& HoughSpace, CImg<float>& result,
  float in_thresh, float out_thresh) {
  const int width = edge._width;
  const int height = edge._height;
  const float width2 = width * 0.5;
  const float height2 = height * 0.5;
  const float diagonal = sqrt(width * width + height * height);

  result.assign(width, height);
  result.fill(0.0f);
  int THETA_N = 180;
  float theta_step = PI / THETA_N;
  // Hough transform
  int OFFSET_N = (int)diagonal;

  HoughSpace.assign(THETA_N, OFFSET_N);
  HoughSpace.fill(0.0f);
  float maxvote = 0;
  cimg_forXY(edge, x, y) {
    if (edge(x, y) < in_thresh)
      continue;
    for (int i = 0; i < THETA_N; i++) {
      float theta = float(i * theta_step);
      float R = sin(theta) * (x - width2) + cos(theta) * (y - height2);
      int offset_int = (int)(OFFSET_N*(R/diagonal+0.5)); //四舍五入
      HoughSpace(i, offset_int)++;
      // 找到最大maxvote的值，用作筛选局部极值
      maxvote = max(maxvote, HoughSpace(i, offset_int));
    }
  }
  HoughSpace.display("Hough Space");

  cout << "find maxvote done: " << maxvote << endl;

  // 用non_maximum suppression 方法进行细化，进而找到peak point
  TVectorOfPairs nonmax;
  // parameter: thresh = 0.5f, zone_width2 = 4
  non_maximum_suppression(HoughSpace, nonmax, out_thresh * maxvote, NEIGH_ZONE);
  sort_nonmax(HoughSpace, nonmax);
  CImg<float> output2(img);
  cout << "suppression done: " << nonmax.size() << endl;
  // vector<pair<int, int> > node;
  /**************draw lines****************/
  CImg<float> count;
  count.assign(width, height);
  count.fill(0.0f);
  for (int i = 0; i < nonmax.size() && i < 4; i++) {
    float theta = nonmax[i].first * theta_step;
    float R = diagonal * (float(nonmax[i].second) / OFFSET_N - 0.5f);
    cout << "parameter: diagonal=" << nonmax[i].first << " nonmax[i].second" << nonmax[i].second << " offset_n" << OFFSET_N << endl;
    cout << "Line: theta=" << theta <<
      " offset=" << R << " strength=" << HoughSpace(nonmax[i].first, nonmax[i].second) << endl;
    // 存在两种情况可能超出边界
     if ((theta < PI / 4) || (theta > (PI * 3 / 4.0f))) {
      for (int x = 0; x < width; x++) {
        // 如果theta==0，则画出来是一个点
        if (theta == 0)
          theta = 0.1 * theta_step;
        int y = (int)((R - (x - width2) * sin(theta)) / cos(theta) + height2);
        if (y < height && y >= 0) {
          output2(x, y, 0) = 255.0f;
          output2(x, y, 1) = 255.0f;
          output2(x, y, 2) = 0.0f;
          count(x, y)++;
        }
      }
      // 超出x轴范围
    } else {
      for (int y = 0; y < height; y++) {

        int x = (int)((R - (y - height2) * cos(theta)) / sin(theta) + width2);
        if (x < width && x >= 0) {
          output2(x, y, 0) = 255.0f;
          output2(x, y, 1) = 0.0f;
          output2(x, y, 2) = 255.0f;
          count(x, y)++;
        }
      }
    }
  }
  cout << "find local maxima done\n";
  // detect point
  cimg_forXY(count, i, j) {
      if (count(i, j) > 1) {
        float red[3] = {255.0f, 0.0f, 0.0f};
        output2.draw_circle(i, j, 5, red);
      }
    }
  output2.display();
}

void HoughCircle(CImg<float>& img, CImg<float>& edge, CImg<float>& HoughSpace, CImg<float>& result,
  float in_thresh, float out_thresh, float r_low, float r_high) {
  // float halfwidth;
  // cout << "halfwidth: ";
  // cin >> halfwidth;
  const int width = edge._width;
  const int height = edge._height;
  const float width2 = width * 0.5;
  const float height2 = height * 0.5;
  const float diagonal = sqrt(width * width + height * height);
  int OFFSET_N = (int) diagonal / 2;


  HoughSpace.assign(width, height, OFFSET_N);
  HoughSpace.fill(0.0f);
  float maxvote = 0.0f;
  cimg_forXY(edge, x, y) {
    //cout << "x: " << x << "y: " << y << endl;
    if (edge(x, y) < in_thresh)
      continue;
    for (int radius = r_low; radius < r_high; radius += R_STEP) {
      for (int theta = 0; theta < 360; theta += THETA_STEP) {
        //cout << "radius: " << radius << " theta: " << theta << endl;
        float a = x - radius * cos(theta * PI / 180); //polar coordinate for center
        float b = y - radius * sin(theta * PI / 180);  //polar coordinate for center
        //cout << "a: " << a << " b: " << b << endl;
        HoughSpace((int)a, (int)b, radius)++; //voting
        maxvote = max(maxvote, HoughSpace((int)a, (int)b, radius));
      }
    }
  }
  cout << "Voting done\n";
  HoughSpace.display("HoughSpace");
  cout << "find maxvote done: " << maxvote << endl;
  CImg<float> output(img);
  TVectorOf3D nonmax;
  non_maximum_suppression3d(HoughSpace, nonmax, out_thresh * maxvote, NEIGH_ZONE_CIRCLE);
  sort_nonmax3d(HoughSpace, nonmax);
  remove_neigh(HoughSpace, nonmax, r_low);
  cout << "Suppression done: " << nonmax.size() << endl;
  // draw lines*******************************
  for (int i = 0; i < nonmax.size() && i < 8; i++) {
    float a = nonmax[i].x;
    float b = nonmax[i].y;
    float r = nonmax[i].z;


    float red[3] = {255.0f, 0.0f, 0.0f};
    //draw outter circle
    output.draw_circle(a, b, r + 10, red, 1, 1);
    //draw inner circle
    output.draw_circle(a, b, r - 10, red, 0, 1);
  }
  cout << "find local maxima done\n";
  output.display();
}

void non_maximum_suppression (CImg<float>& img, TVectorOfPairs& nonmax,
                float thresh, int halfwidth)
{
  nonmax.clear();
  cimg_forXY(img, x, y) {
      float value = img(x,y);
      if (value < thresh)
       continue;

      bool ismax = true;
      for (int ny = y - halfwidth; ny <= y + halfwidth; ny++) {
        for (int nx = x - halfwidth; nx <= x + halfwidth; nx++) {
          if (ny < 0 || ny >= img._height || nx < 0 || nx >= img._width)
            continue;
          ismax = ismax && (img(nx,ny) <= value);
        }}
      if (!ismax) continue;

      nonmax.push_back(make_pair(x,y));
  }
}


void non_maximum_suppression3d(CImg<float>& img, TVectorOf3D& nonmax,
                float thresh, int halfwidth)
{
  nonmax.clear();
  cimg_forXYZ(img, x, y, z){
      float value = img(x, y, z);
      if (value<thresh)  continue;
      bool ismax = true;
      for (int ny = y - halfwidth; ny <= y + halfwidth; ny++) {
        for (int nx = x - halfwidth; nx <= x + halfwidth; nx++) {
          for (int nz = z - halfwidth; nz <= z + halfwidth; nz++) {
            if (ny < 0 || ny >= img._height || nx < 0 || nx >= img._width
             || nz < 0 || nz >= img._depth)
              continue;
            ismax = ismax && (img(nx, ny, nz) <= value);
        }}}
      if (!ismax) continue;

      nonmax.push_back(nonmax_3d(x, y, z));
  }
  // 去除圆心相近的点
}

/*
  计算两个圆心之间的欧几里得距离
  */
float EucliDist(nonmax_3d& n1, nonmax_3d& n2) {
  return sqrt((n1.x - n2.x) * (n1.x - n2.x) + (n1.y - n2.y) * (n1.y - n2.y));
}


/* 去除圆心相近的局部极值*/
void remove_neigh(CImg<float>& hough, TVectorOf3D& nonmax, float r_low) {
  // 此时的nonmax是降序的
  float thresh = 1.8 * r_low;
  vector<int> to_delete;
  for (int i = 0; i < nonmax.size(); i++)
    for (int j = i + 1; j < nonmax.size(); j++) {
      if (thresh > EucliDist(nonmax[i], nonmax[j])) {
        nonmax.erase(nonmax.begin() + j);
        j--;
      }
    }
}

void sort_nonmax(CImg<float>& hough, TVectorOfPairs& nonmax) {

  for (int i = 0; i < nonmax.size(); i++) {
    float max = hough(nonmax[i].first, nonmax[i].second);
    int idx = i;
    for (int j = i + 1; j < nonmax.size(); j++) {
      if (hough(nonmax[j].first, nonmax[j].second) > max) {
        idx = j;
        max = hough(nonmax[j].first, nonmax[j].second);
      }
    }
    pair<int, int> temp = nonmax[i];
    nonmax[i] = nonmax[idx];
    nonmax[idx] = temp;
  }
}

void sort_nonmax3d(CImg<float>& hough, TVectorOf3D& nonmax) {

  for (int i = 0; i < nonmax.size(); i++) {
    float max = hough(nonmax[i].x, nonmax[i].y, nonmax[i].z);
    int idx = i;
    for (int j = i + 1; j < nonmax.size(); j++) {
      if (hough(nonmax[j].x, nonmax[j].y, nonmax[j].z) > max) {
        idx = j;
        max = hough(nonmax[j].x, nonmax[j].y, nonmax[j].z);
      }
    }
    nonmax_3d temp = nonmax[i];
    nonmax[i] = nonmax[idx];
    nonmax[idx] = temp;
  }
}


void HoughCircleTest(string path, float lt, float ht, float gk, float r_low, float r_high) {
  CImg<float> img(path.c_str());
  CImg<float> binImg(img.get_channel(0));
  img.display();
  CImg<unsigned char> unsignedimg(binImg * 255.0f);
  time_t start = time(NULL);
  unsigned char* edge = canny(unsignedimg, img._width, img._height, lt, ht, gk);
  CImg<float> edgeCimg(edge, img._width, img._height);
  edgeCimg.display("edge");
  CImg<float> hough, result;
  HoughCircle(img, edgeCimg, hough, result, 200.0f, 0.5f, r_low, r_high);
  //HoughLine(img, edgeCimg, hough, result, 200.0f, 0.3f);
  time_t end = time(NULL);
  cout << "Time: " << end - start << "s\n";
}