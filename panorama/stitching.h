#ifndef stitching_h
#define stitching_h

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <Eigen/Dense>
#include "CImg.h"
#include "align_features.h"

#define THRESHOLD 50.0

using namespace std;
using namespace Eigen;
using namespace cimg_library;

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

CImg<float> get_gray_image(CImg<float>& img);

void ReplacePairs(vector<POINT_PAIR>& bigger, vector<POINT_PAIR>& smaller);

vector<int> getAvgOffset(const vector<POINT_PAIR>& pairs, vector<int>& idxs);

// int getAvgOffsetX(const vector<POINT_PAIR>& pairs, vector<int>& idxs);

// int getMiddleIndex(vector<vector<int> > matching_index);
CImg<float> stitching(vector<CImg<float> >& src_imgs);


#endif