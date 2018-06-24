#include "blending.h"


CImg<float> blendTwoImages(const CImg<float>& a, const CImg<float>& b, int offset_x, int offset_y, int min_x, int min_y) {
  if (offset_x > 0) {
    int nwidth = b._width + abs(offset_x);
    int nheight = b._height + abs(offset_y);

    CImg<float> result(nwidth, nheight, 1, b.spectrum(), 0);

    cimg_forXY(a, i, j) {
      if (i > min_x)
        continue;
      for (int k = 0; k < a.spectrum(); k++)
        result(i, j, 0, k) = a(i, j, 0, k);
    }

    cimg_forXY(b, x, y) {
      for (int k = 0; k < b.spectrum(); k++)
        result(x + offset_x, y + offset_y, 0, k) = b(x, y, 0, k);
    }

    return result;
  } else {
    int nwidth = a._width + abs(offset_x);
    int nheight = a._height + abs(offset_y);

    CImg<float> result(nwidth, nheight, 1, a.spectrum(), 0);

    cimg_forXY(b, i, j) {
      for (int k = 0; k < b.spectrum(); k++)
        result(i, j, 0, k) = b(i, j, 0, k);
    }

    cimg_forXY(a, i, j) {
      if (i < min_x)
        continue;
      for (int k = 0; k < a.spectrum(); k++)
        result(i - offset_x, j - offset_y, 0, k) = a(i, j, 0, k);
    }

    return result;
  }
}