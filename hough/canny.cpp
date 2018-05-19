#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include "CImg.h"
/*
  C version. Based on Java code by
* Tom Gibara
*/

#define ffabs(x) ( (x) >= 0 ? (x) : -(x) )
#define GAUSSIAN_CUT_OFF 0.005f
#define MAGNITUDE_SCALE 100.0f
#define MAGNITUDE_LIMIT 1000.0f
#define MAGNITUDE_MAX ((int) (MAGNITUDE_SCALE * MAGNITUDE_LIMIT))

using namespace cimg_library;
using namespace std;

class CANNY {
private:
  unsigned char *data; /* input image */
  int width;
  int height;
  int *idata;          /* output for edges */
  int *magnitude;      /* edge magnitude as detected by Gaussians */
  float *xConv;        /* temporary for convolution in x direction */
  float *yConv;        /* temporary for convolution in y direction */
  float *xGradient;    /* gradients in x direction, as detected by Gaussians */
  float *yGradient;    /* gradients in x direction,a s detected by Gaussians */
public:

  CANNY(unsigned char *grey, int width, int height);
  ~CANNY();
  int computeGradients(float kernelRadius, int kernelWidth);
  void performHysteresis(int low, int high);
  void follow(int x1, int y1, int i1, int threshold);
  unsigned char *getData();
  int* getiData();
};

unsigned char *canny(unsigned char *grey, int width, int height, float lt, float ht, float gk);
unsigned char *cannyparam(unsigned char *grey, int width, int height,
                float lowThreshold, float highthreshold,
                float gaussiankernelradius, int gaussiankernelwidth,
                int contrastnormalised);

/*
  Helper
*/
class Helper {
public:
  static void normalizeContrast(unsigned char *data, int width, int height);
  static float hypotenuse(float x, float y);
  static float gaussian(float x, float sigma);
};
/*
  Canny edge detection with default parameters
    Params: grey - the greyscale image
          width, height - image width and height
    Returns: binary image with edges as set pixels
*/
unsigned char *canny(unsigned char *grey, int width, int height, float lt, float ht, float gk)
{
  // float lt, ht, gk;
  // cout << "lowthreshold, highthreshold, gaussiankernelradius\n";
  // cin >> lt >> ht >> gk;
  // Line 4 7 5
  return cannyparam(grey, width, height, lt, ht, gk, 16, 0);
}

/*
 Canny edge detection with parameters passed in by user
   Params: grey - the greyscale image
           width, height - image dimensions
       lowthreshold - default 2.5
       highthreshold - default 7.5
       gaussiankernelradius - radius of edge detection Gaussian, in standard deviations
         (default 2.0)
       gaussiankernelwidth - width of Gaussian kernel, in pixels (default 16)
       contrastnormalised - flag to normalise image before edge detection (defualt 0)
   Returns: binary image with set pixels as edges

*/
unsigned char *cannyparam(unsigned char *grey, int width, int height,
              float lowthreshold, float highthreshold,
              float gaussiankernelradius, int gaussiankernelwidth,
              int contrastnormalised)
{
  CANNY *can = 0;
  unsigned char *answer = 0;
    int low, high;
  int err;
  int i;

    answer = (unsigned char*)malloc(width * height);
  // if(!answer)
  //   goto error_exit;
  can = new CANNY(grey, width, height);
  // if(!can)
  //   goto error_exit;
  //get canny's data
  unsigned char* data = can->getData();
  if (contrastnormalised)
    Helper::normalizeContrast(data, width, height);

  err = can->computeGradients(gaussiankernelradius, gaussiankernelwidth);
  // if(err < 0)
  //   goto error_exit;
  low = (int) (lowthreshold * MAGNITUDE_SCALE + 0.5f);
  high = (int) ( highthreshold * MAGNITUDE_SCALE + 0.5f);
  can->performHysteresis(low, high);
  int* idata = can->getiData();
  //temp
  int cnt = 0;
  for(i=0;i<width*height;i++) {
    answer[i] = idata[i] > 0 ? 255 : 0;
    if (answer[i])
      cnt++;
  }
  cout << cnt << endl;

  return answer;
// error_exit:
//   free(this);
//   killbuffers(this);
//   return 0;
}


/*
  buffer allocation
*/
CANNY::CANNY(unsigned char *grey, int width, int height)
{
  // this = malloc(sizeof(CANNY));
  // if(!this)
  //   goto error_exit;
  this->data = (unsigned char*)malloc(width * height);
  this->idata = (int*)malloc(width * height * sizeof(int));
  this->magnitude = (int*)malloc(width * height * sizeof(int));
  this->xConv = (float*)malloc(width * height * sizeof(float));
  this->yConv = (float*)malloc(width * height * sizeof(float));
  this->xGradient = (float*)malloc(width * height * sizeof(float));
  this->yGradient = (float*)malloc(width * height * sizeof(float));
  // if(!this->data || !this->idata || !this->magnitude ||
  //   !this->xConv || !this->yConv ||
  //   !this->xGradient || !this->yGradient)
  //    goto error_exit;

  memcpy(this->data, grey, width * height);
  this->width = width;
  this->height = height;

// error_exit:
//   killbuffers(this);
//   return 0;
}

/*
  buffers destructor
*/
CANNY::~CANNY()
{
  if(this)
  {
    free(this->data);
    free(this->idata);
    free(this->magnitude);
    free(this->xConv);
    free(this->yConv);
    free(this->xGradient);
    free(this->yGradient);
  }
}

// NOTE: API to get CANNY's input image data
unsigned char *CANNY::getData() {
  return this->data;
}

// NOTE: API to get CANNY's idata
int *CANNY::getiData() {
  return this->idata;
}

/* NOTE: The elements of the method below (specifically the technique for
  non-maximal suppression and the technique for gradient computation)
  are derived from an implementation posted in the following forum (with the
  clear intent of others using the code):
    http://forum.java.sun.com/thread.jspa?threadID=546211&start=45&tstart=0
  My code effectively mimics the algorithm exhibited above.
  Since I don't know the providence of the code that was posted it is a
  possibility (though I think a very remote one) that this code violates
  someone's intellectual property rights. If this concerns you feel free to
  contact me for an alternative, though less efficient, implementation.
  */

int CANNY::computeGradients(float kernelRadius, int kernelWidth)
{
    float *kernel;
    float *diffKernel;
    int kwidth;

    int width, height;

    int initX;
    int maxX;
    int initY;
    int maxY;

    int x, y;
    int i;
    int flag;

    width = this->width;
        height = this->height;

    kernel = (float*)malloc(kernelWidth * sizeof(float));
    diffKernel = (float*)malloc(kernelWidth * sizeof(float));
    if(!kernel || !diffKernel)
      goto error_exit;

    /* initialise the Gaussian kernel */
    for (kwidth = 0; kwidth < kernelWidth; kwidth++)
    {
      float g1, g2, g3;
      g1 = Helper::gaussian((float) kwidth, kernelRadius);
      if (g1 <= GAUSSIAN_CUT_OFF && kwidth >= 2)
        break;
      g2 = Helper::gaussian(kwidth - 0.5f, kernelRadius);
      g3 = Helper::gaussian(kwidth + 0.5f, kernelRadius);
      kernel[kwidth] = (g1 + g2 + g3) / 3.0f / (2.0f * (float) 3.14 * kernelRadius * kernelRadius);
      diffKernel[kwidth] = g3 - g2;
    }

    initX = kwidth - 1;
    maxX = width - (kwidth - 1);
    initY = width * (kwidth - 1);
    maxY = width * (height - (kwidth - 1));

    /* perform convolution in x and y directions */
    for(x = initX; x < maxX; x++)
    {
      for(y = initY; y < maxY; y += width)
      {
        int index = x + y;
        float sumX = this->data[index] * kernel[0];
        float sumY = sumX;
        int xOffset = 1;
        int yOffset = width;
        while(xOffset < kwidth)
        {
          sumY += kernel[xOffset] * (this->data[index - yOffset] + this->data[index + yOffset]);
          sumX += kernel[xOffset] * (this->data[index - xOffset] + this->data[index + xOffset]);
          yOffset += width;
          xOffset++;
        }

        this->yConv[index] = sumY;
        this->xConv[index] = sumX;
      }

    }

    for (x = initX; x < maxX; x++)
    {
      for (y = initY; y < maxY; y += width)
      {
        float sum = 0.0f;
        int index = x + y;
        for (i = 1; i < kwidth; i++)
          sum += diffKernel[i] * (this->yConv[index - i] - this->yConv[index + i]);

        this->xGradient[index] = sum;
      }

    }

    for(x = kwidth; x < width - kwidth; x++)
    {
      for (y = initY; y < maxY; y += width)
      {
        float sum = 0.0f;
        int index = x + y;
        int yOffset = width;
        for (i = 1; i < kwidth; i++)
        {
          sum += diffKernel[i] * (this->xConv[index - yOffset] - this->xConv[index + yOffset]);
          yOffset += width;
        }

        this->yGradient[index] = sum;
      }

    }

    initX = kwidth;
    maxX = width - kwidth;
    initY = width * kwidth;
    maxY = width * (height - kwidth);
    for(x = initX; x < maxX; x++)
    {
      for(y = initY; y < maxY; y += width)
      {
        int index = x + y;
        int indexN = index - width;
        int indexS = index + width;
        int indexW = index - 1;
        int indexE = index + 1;
        int indexNW = indexN - 1;
        int indexNE = indexN + 1;
        int indexSW = indexS - 1;
        int indexSE = indexS + 1;

        float xGrad = this->xGradient[index];
        float yGrad = this->yGradient[index];
        float gradMag = Helper::hypotenuse(xGrad, yGrad);

        /* perform non-maximal supression */
        float nMag = Helper::hypotenuse(this->xGradient[indexN], this->yGradient[indexN]);
        float sMag = Helper::hypotenuse(this->xGradient[indexS], this->yGradient[indexS]);
        float wMag = Helper::hypotenuse(this->xGradient[indexW], this->yGradient[indexW]);
        float eMag = Helper::hypotenuse(this->xGradient[indexE], this->yGradient[indexE]);
        float neMag = Helper::hypotenuse(this->xGradient[indexNE], this->yGradient[indexNE]);
        float seMag = Helper::hypotenuse(this->xGradient[indexSE], this->yGradient[indexSE]);
        float swMag = Helper::hypotenuse(this->xGradient[indexSW], this->yGradient[indexSW]);
        float nwMag = Helper::hypotenuse(this->xGradient[indexNW], this->yGradient[indexNW]);
        float tmp;
        /*
         * An explanation of what's happening here, for those who want
         * to understand the source: This performs the "non-maximal
         * supression" phase of the Canny edge detection in which we
         * need to compare the gradient magnitude to that in the
         * direction of the gradient; only if the value is a local
         * maximum do we consider the point as an edge candidate.
         *
         * We need to break the comparison into a number of different
         * cases depending on the gradient direction so that the
         * appropriate values can be used. To avoid computing the
         * gradient direction, we use two simple comparisons: first we
         * check that the partial derivatives have the same sign (1)
         * and then we check which is larger (2). As a consequence, we
         * have reduced the problem to one of four identical cases that
         * each test the central gradient magnitude against the values at
         * two points with 'identical support'; what this means is that
         * the geometry required to accurately interpolate the magnitude
         * of gradient function at those points has an identical
         * geometry (upto right-angled-rotation/reflection).
         *
         * When comparing the central gradient to the two interpolated
         * values, we avoid performing any divisions by multiplying both
         * sides of each inequality by the greater of the two partial
         * derivatives. The common comparand is stored in a temporary
         * variable (3) and reused in the mirror case (4).
         *
         */


        flag = ( (xGrad * yGrad <= 0.0f) /*(1)*/
          ? ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
            ? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * neMag - (xGrad + yGrad) * eMag) /*(3)*/
              && tmp > fabs(yGrad * swMag - (xGrad + yGrad) * wMag) /*(4)*/
            : (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * neMag - (yGrad + xGrad) * nMag) /*(3)*/
              && tmp > ffabs(xGrad * swMag - (yGrad + xGrad) * sMag) /*(4)*/
          : ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
            ? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * seMag + (xGrad - yGrad) * eMag) /*(3)*/
              && tmp > ffabs(yGrad * nwMag + (xGrad - yGrad) * wMag) /*(4)*/
            : (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * seMag + (yGrad - xGrad) * sMag) /*(3)*/
              && tmp > ffabs(xGrad * nwMag + (yGrad - xGrad) * nMag) /*(4)*/
          );
        if(flag)
        {
          this->magnitude[index] = (gradMag >= MAGNITUDE_LIMIT) ? MAGNITUDE_MAX : (int) (MAGNITUDE_SCALE * gradMag);
          /*NOTE: The orientation of the edge is not employed by this
           implementation. It is a simple matter to compute it at
          this point as: Math.atan2(yGrad, xGrad); */
        }
        else
        {
          this->magnitude[index] = 0;
        }
      }
    }
    free(kernel);
    free(diffKernel);
    return 0;
error_exit:
    free(kernel);
    free(diffKernel);
    return -1;
}

  /*
    we follow edges. high gives the parameter for starting an edge,
    how the parameter for continuing it.
  */
void CANNY::performHysteresis(int low, int high)
{
  int offset = 0;
  int x, y;

  memset(this->idata, 0, this->width * this->height * sizeof(int));

  for(y = 0; y < this->height; y++)
  {
    for(x = 0; x < this->width; x++)
  {
      if(this->idata[offset] == 0 && this->magnitude[offset] >= high)
      follow(x, y, offset, low);
    offset++;
    }
  }
}

  /*
    recursive portion of edge follower
  */

void CANNY::follow(int x1, int y1, int i1, int threshold)
{
  int x, y;
  int x0 = x1 == 0 ? x1 : x1 - 1;
  int x2 = x1 == this->width - 1 ? x1 : x1 + 1;
  int y0 = y1 == 0 ? y1 : y1 - 1;
  int y2 = y1 == this->height -1 ? y1 : y1 + 1;

  this->idata[i1] = this->magnitude[i1];
  for (x = x0; x <= x2; x++)
  {
    for (y = y0; y <= y2; y++)
  {
      int i2 = x + y * this->width;
    if ((y != y1 || x != x1) && this->idata[i2] == 0 && this->magnitude[i2] >= threshold)
      follow(x, y, i2, threshold);
    }
  }
}

void Helper::normalizeContrast(unsigned char *data, int width, int height)
{
  int histogram[256] = {0};
    int remap[256];
  int sum = 0;
    int j = 0;
  int k;
    int target;
    int i;

  for (i = 0; i < width * height; i++)
      histogram[data[i]]++;


    for (i = 0; i < 256; i++)
  {
      sum += histogram[i];
      target = (sum*255)/(width * height);
      for (k = j+1; k <= target; k++)
        remap[k] = i;
      j = target;
   }

    for (i = 0; i < width * height; i++)
      data[i] = remap[data[i]];
}


float Helper::hypotenuse(float x, float y)
{
  return (float) sqrt(x*x +y*y);
}

float Helper::gaussian(float x, float sigma)
{
  return (float) exp(-(x * x) / (2.0f * sigma * sigma));
}

