#include <iostream>
#include <string>
#include <vector>
#include <cmath>
// #include <Eigen/Eigen>
// #define cimg_convert_path  "C:/Program Files/ImageMagick-7.0.8-Q16/convert.exe"
#include "CImg.h"
#include "align_features.h"
#include "blending.h"
#include "extract_features.h"
#include "stitching.h"
#include "warping.h"


using namespace std;
using namespace cimg_library;
// using namespace Eigen;

int main() {
  // string root = "../TEST-ImageData(2)/100NIKON-DSCN00";

  // for (int i = 10; i < 26; i++) {
  //   CImg<float> img((root + to_string(i) + "_DSCN00" + to_string(i) + ".JPG").c_str());
  //   // img.save(("../TEST-ImageData(2)/" + to_string(i) + ".bmp").c_str());
  //   img.display();
  // }
  // ii.display();
  cout << "Input Dataset: ";
  int mode = 0;
  cin >> mode;
  if (mode == 1) {
    string root = "../TEST-ImageData(1)/pano1_000";
    vector<CImg<float> > imgs;
    for (int i = 1; i < 5; i++) {
      CImg<float> img((root + to_string(i) + ".bmp").c_str());
      imgs.push_back(img);
    }
    CImg<float> result = stitching(imgs);
    result.display("result");

    result.save("../result/res1.bmp");
  } else if (mode == 2) {
    string root = "../TEST-ImageData(2)/";
    vector<CImg<float> > imgs;
    for (int i = 8; i < 26; i++) {
      CImg<float> img((root + to_string(i) + ".bmp").c_str());
      imgs.push_back(img);
    }
    CImg<float> result = stitching(imgs);
    result.display("result");
    result.save("../result/res2.bmp");
  }
}