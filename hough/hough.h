#ifndef hough_h
#define hough_h
#include "CImg.h"
#include <vector>
#include <string>

using namespace cimg_library;

typedef std::vector<std::pair<int, int> > TVectorOfPairs;
struct nonmax_3d{
  int x;
  int y;
  int z;
  nonmax_3d(int i, int j, int k) {
    x = i;
    y = j;
    z = k;
  }
  nonmax_3d& operator=(const nonmax_3d& nonmax) {
    x = nonmax.x;
    y = nonmax.y;
    z = nonmax.z;
  }
} ;
typedef std::vector<nonmax_3d> TVectorOf3D;
void HoughLine(CImg<float>& img, CImg<float>& edge, CImg<float>& HoughSpace, CImg<float>& result,
  float in_thresh, float out_thresh);
void non_maximum_suppression (CImg<float>& img, TVectorOfPairs& nonmax,
                float thresh, int halfwidth);
void non_maximum_suppression3d(CImg<float>& img, TVectorOf3D& nonmax,
                float thresh, int halfwidth);
void HoughCircle(CImg<float>& img, CImg<float>& edge, CImg<float>& HoughSpace, CImg<float>& result,
  float in_thresh, float out_thresh, float r_low, float r_high);
void sort_nonmax(CImg<float>& hough, TVectorOfPairs& nonmax);
void sort_nonmax3d(CImg<float>& hough, TVectorOf3D& nonmax);
void remove_neigh(CImg<float>& hough, TVectorOf3D& nonmax, float r_low);
void HoughCircleTest(std::string path, float lt, float ht, float gk, float r_low, float r_high);

#endif