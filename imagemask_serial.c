#include <stdio.h>
#include <stdlib.h>
#include "bmpo.h"
#include <time.h>
int main(){
	int n=-1;
	struct timespec start,end;
	while(n<0 || n>2){
	    printf("PRESS 0 FOR NO CONVERTION.\n");
	    printf("PRESS 1 FOR HORIZONTAL FLIP.\n");
	    printf("PRESS 2 FOR VERTICAL FLIP.\n");
	    printf("CHOICE:");
	    scanf("%d", &n);
	   }
	BMPImage_t* bmp=ReadBMP("s1_EARTH.bmp");
	BMPImage_t* copybmp=ReadBMP("s1_EARTH.bmp");
	int x,y;
	clock_gettime(CLOCK_MONOTONIC, &start);
	switch (n){
		case 1:
			for(y=0;y<bmp->header.height_px;++y){
				int x1=0;
				for(x=bmp->header.width_px-1;x>=(bmp->header.width_px)/2;--x){
					RGB_t colour=GetPixel(bmp,x,y);
					RGB_t grayscale;
					float rgbis = colour.r * 0.2126 + colour.g * 0.7152 + colour.b * 0.0722;
					grayscale.r=(rgbis);
					grayscale.g=(rgbis);
					grayscale.b=(rgbis);
					SetPixel(bmp,x1,y,grayscale);
					++x1;
				}
				x1=bmp->header.width_px-1;
				for(x=0;x<(bmp->header.width_px)/2;++x){
					RGB_t colour=GetPixel(copybmp,x,y);
					RGB_t grayscale;
					float rgbis = colour.r * 0.2126 + colour.g * 0.7152 + colour.b * 0.0722;
					grayscale.r=(rgbis);
					grayscale.g=(rgbis);
					grayscale.b=(rgbis);
					SetPixel(bmp,x1,y,grayscale);
					--x1;
				}
			}
			break;
		case 2:
			for(x=0;x<bmp->header.width_px;++x){
				int y1=0;
				for(y=bmp->header.height_px-1;y>=(bmp->header.height_px)/2;--y){
					RGB_t colour=GetPixel(bmp,x,y);
					RGB_t grayscale;
					float rgbis = colour.r * 0.2126 + colour.g * 0.7152 + colour.b * 0.0722;
					grayscale.r=(rgbis);
					grayscale.g=(rgbis);
					grayscale.b=(rgbis);
					SetPixel(bmp,x,y1,grayscale);
					++y1;
				}
				y1=bmp->header.height_px-1;
				for(y=0;y<((bmp->header.width_px)/2)-1;++y){
					RGB_t colour=GetPixel(copybmp,x,y);
					RGB_t grayscale;
					float rgbis = colour.r * 0.2126 + colour.g * 0.7152 + colour.b * 0.0722;
					grayscale.r=(rgbis);
					grayscale.g=(rgbis);
					grayscale.b=(rgbis);
					SetPixel(bmp,x,y1,grayscale);
					--y1;
				}
			}
			break;
		default:
			for(y=0;y<bmp->header.height_px;++y){
				for(x=0;x<bmp->header.width_px;++x){
					RGB_t colour=GetPixel(bmp,x,y);
					RGB_t grayscale;
					float rgbis = colour.r * 0.2126 + colour.g * 0.7152 + colour.b * 0.0722;
					grayscale.r=(rgbis);
					grayscale.g=(rgbis);
					grayscale.b=(rgbis);
					SetPixel(bmp,x,y,grayscale);
				}
			}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	const int DAS_NANO_SECONDS_IN_SEC = 1000000000;
	long timeElapsed_s = end.tv_sec - start.tv_sec;
	long timeElapsed_n = end.tv_nsec - start.tv_nsec;
	//If we have a negative number in timeElapsed_n , borrow a carry from seconds
	if ( timeElapsed_n < 0 ) 
	{timeElapsed_n =DAS_NANO_SECONDS_IN_SEC + timeElapsed_n; timeElapsed_s--;}
	printf("Time: %ld.%09ld secs \n",timeElapsed_s,timeElapsed_n);		
	SaveBMP(bmp,"output_serial.bmp");
	return 0;
}
