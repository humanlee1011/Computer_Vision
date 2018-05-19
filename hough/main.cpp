#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include "CImg.h"
#include "hough.h"
#include "canny.h"

using namespace std;
using namespace cimg_library;


int main() {
  // 在Cimg中，读取jpg是调用了ImageMagick库，并标明库的位置
  // cimg::imagemagick_path("D:\\ImageMagick-6.9.2-Q16\\convert.exe");
  // 但直接转成bmp格式也ok
  int mode = 0;

  //cin >> mode;
  string lineroot = "./Dataset1/bmp/re";

  for (int i = 1; i <= 6; i++) {
    string path = lineroot + to_string(i) + ".bmp";
    CImg<float> img(path.c_str());
    CImg<float> binImg(img.get_channel(0));
    img.display();
    CImg<unsigned char> unsignedimg(binImg * 255.0f);
    time_t start = time(NULL);
    unsigned char* edge = canny(unsignedimg, img._width, img._height, 4.0f, 7.0f, 5.0f);
    CImg<float> edgeCimg(edge, img._width, img._height);
    edgeCimg.display("edge");
    CImg<float> hough, result;
    HoughLine(img, edgeCimg, hough, result, 200.0f, 0.55f);
    time_t end = time(NULL);
    cout << "Time: " << end - start << "s\n";
  }

  // detect circle
  string circleroot = "./Dataset2/bmp/re";
  //cin >> mode;
  // if (mode == 1)
  //   circleroot += "re";
  // while (true) {
  //   int i;
  //   cin >> i;
  //   string path = circleroot + to_string(i) + ".bmp";
  //   CImg<float> img(path.c_str());
  //   CImg<float> binImg(img.get_channel(0));
  //   img.display();
  //   CImg<unsigned char> unsignedimg(binImg * 255.0f);
  //   time_t start = time(NULL);
  //   float lt, ht, gk;
  //   cin >> lt >> ht >> gk;
  //   unsigned char* edge = canny(unsignedimg, img._width, img._height, lt, ht, gk);
  //   CImg<float> edgeCimg(edge, img._width, img._height);
  //   edgeCimg.display("edge");
  //   CImg<float> hough, result;

  //   cout << "r_low r_high";
  //   float r_low, r_high;
  //   cin >> r_low >> r_high;
  //   HoughCircle(img, edgeCimg, hough, result, 200.0f, 0.5f, r_low, r_high);
  //   //HoughLine(img, edgeCimg, hough, result, 200.0f, 0.3f);
  //   time_t end = time(NULL);
  //   cout << "Time: " << end - start << "s\n";
  // }
  // HoughCircleTest(circleroot + to_string(1) + ".bmp", 4.0f, 7.0f, 4.0f, 148, 153);
  // HoughCircleTest(circleroot + to_string(2) + ".bmp", 4.0f, 7.0f, 4.0f, 88, 110);
  // HoughCircleTest(circleroot + to_string(3) + ".bmp", 1.0f, 4.0f, 7.5f, 33, 36);
  // HoughCircleTest(circleroot + to_string(4) + ".bmp", 2.5f, 7.5f, 2.0f, 85, 100);
  // HoughCircleTest(circleroot + to_string(5) + ".bmp", 4.0f, 7.0f, 7.0f, 100, 130);
  // HoughCircleTest(circleroot + to_string(6) + ".bmp", 1.5f, 7.5f, 4.0f, 35, 60);

}