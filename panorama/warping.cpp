#include "bilinear_interpolation.h"
#include "warping.h"


const double PI = std::atan(1.0) * 4;

CImg<float> CylinderProjection(CImg<float>& img) {
  int width = img._width;
  int height = img._height;
  int depth = img._depth;
  CImg<float> result;
  result.assign(width, height, depth, 3);
  result.fill(0.0f);

  float centerX = width / 2;
  float centerY = height / 2;
  float f = width / (2 * tan(PI / 4 / 2));
  cimg_forXY(img, i, j) {
    float theta = asin((i - centerX) / f);
    int pointX = int(f * tan((i - centerX) / f) + centerX);
    int pointY = int((j - centerY) / cos(theta) + centerY);
    for (int k = 0; k < depth; k++) {
      if (pointX >= 0 && pointX < width && pointY >= 0 && pointY < height) {
        result(i, j, k, 0) = img(pointX, pointY, k, 0);
        result(i, j, k, 1) = img(pointX, pointY, k, 1);
        result(i, j, k, 2) = img(pointX, pointY, k, 2);
      }
      else {
        result(i, j, k, 0) = 0;
        result(i, j, k, 1) = 0;
        result(i, j, k, 2) = 0;
      }
    }
  }
  return result;
}

// CImg<float> resize(CImg<float>& img) {
//   CImg<float> res(img._width / 2, img._height / 2, 1, img.spectrum(), 0);
// }


int getXAfterWarping(float x, float y, MatrixXf& H) {
  return (H(0, 0) * x + H(1, 0) * y + H(2, 0)) / (H(6, 0) * x + H(7, 0) * y + 1);
}

int getYAfterWarping(float x, float y, MatrixXf& H) {
  return (H(3, 0) * x + H(4, 0) * y + H(5, 0)) / (H(6, 0) * x + H(7, 0) * y + 1);
}