#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "bmp.h"


int main(int argc, char *argv[])
{
	if (argc!=2){
		printf("Too few / Too many parameters. Give 1 parameter\n");
		exit(5);
	}
	int number=atoi(argv[1]);
	if (number<0 || number>2){
		printf("Invalid input! Number must be either 0, 1 or 2\n");
		exit(6);
	}
	
	BMPImage_t* bmp=ReadBMP("s1_EARTH.bmp");
	int width=bmp->header.width_px;
	int height=bmp->header.height_px;
	int x, y;
	double start, end;
	switch (number){
		case 1:
			start = omp_get_wtime();
			#pragma omp parallel for private (x,y)
				for(y = 0; y < height; y++)
				{
    				for(x = 0; x < width / 2 + 1; x++)
    				{
        				RGB_t color1 = GetPixel(bmp, x, y);
        				RGB_t color2 = GetPixel(bmp, width - 1 - x, y);

						int avg_color1 =  0.2126*color1.r+ 0.7152*color1.g+0.0722*color1.b;
						int avg_color2 =  0.2126*color2.r+ 0.7152*color2.g+0.0722*color2.b;
						
        				SetPixel(bmp, x, y, avg_color2);
        				SetPixel(bmp, width - 1 - x, y, avg_color1);
    				}
				}					
			end = omp_get_wtime();
			SaveBMP(bmp,"output_omp.bmp");
			printf("Processing took %f seconds\n",end-start);
			break;
		case 2:
			start = omp_get_wtime();
			#pragma omp parallel for private (x,y)
				for(x = 0; x < width; x++)
				{
    				for(y = 0; y < height / 2 + 1; y++)
    				{
        				RGB_t color1 = GetPixel(bmp, x, y);
        				RGB_t color2 = GetPixel(bmp, x, height - 1 - y);

						int avg_color1 =  0.2126*color1.r+ 0.7152*color1.g+0.0722*color1.b;
						int avg_color2 =  0.2126*color2.r+ 0.7152*color2.g+0.0722*color2.b;
						
        				SetPixel(bmp, x, y, avg_color2);
        				SetPixel(bmp, x, height - 1 - y, avg_color1);
    				}
				}		
			end = omp_get_wtime();
			SaveBMP(bmp,"output_omp.bmp");
			printf("Processing took %f seconds\n",end-start);
			break;
		default:
			start = omp_get_wtime();
			#pragma omp parallel for private(x,y)
			    for(y=0;y<height;y++)
			    {
			        for(x=0;x<width;x++)
			        {
			            RGB_t colour = GetPixel(bmp,x,y);
			            int avg_colour = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.b;
			            SetPixel(bmp, x,y,avg_colour);
			        }
			            
			    }
			end = omp_get_wtime();
			SaveBMP(bmp,"output_omp.bmp");
			printf("Processing took %f seconds\n",end-start);
			break;
	}
	return 0;
}