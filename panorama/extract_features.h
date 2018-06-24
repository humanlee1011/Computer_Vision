#ifndef extract_features_h
#define extract_features_h

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "CImg.h"

#define RESIZE_SIZE 500.0
using namespace std;
using namespace cimg_library;



extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

//extract SIFT features
map<vector<float>, VlSiftKeypoint> extractFeatures(CImg<float>& img);
#endif
