#ifndef perspectiveTransform_h
#define perspectiveTransform_h
#include "Cimg.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace cimg_library;


void perspectiveTransform(CImg<float>& ori, CImg<float>& result, vector<pair<int, int> >& src);

void _perspectiveTransform(CImg<float>& ori, CImg<float>& result, vector<pair<int, int> >& src, vector<pair<int, int> >& dst);

void sortPoint(int height, int width, vector<pair<int, int> >& point);

bool myComp(pair<double, int>& p1, pair<double, int>& p2);

void sortPoint(vector<pair<int, int> >& point);
double calDist(pair<int, int>& p1, pair<int, int> p2);

#endif