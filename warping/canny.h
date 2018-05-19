#ifndef canny_h
#define canny_h

unsigned char *canny(unsigned char *grey, int width, int height, float lt, float ht, float gk);
unsigned char *cannyparam(unsigned char *grey, int width, int height,
						  float lowThreshold, float highthreshold,
						  float gaussiankernelradius, int gaussiankernelwidth,
						  int contrastnormalised);
#endif