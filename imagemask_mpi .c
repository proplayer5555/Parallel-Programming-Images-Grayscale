#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include <mpi.h>
#include <time.h>
int main(int argc, char *argv[]){
	int rank,size;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc!=2){
		printf("Too few / Too many parameters. Give 1 parameter\n");
		exit(5);
	}
	int number=atoi(argv[1]);
	if (number<0 || number>2){
		printf("Invalid input! Number must be either 0, 1 or 2\n");
		exit(6);
	}
	/*
	if (size<2){
	printf("At least two processes required! \n");
	MPI_Finalize();
	exit(100110);
	}*/
	struct timespec start,end;

	BMPImage_t* bmp=ReadBMP("s1_EARTH.bmp");
	//BMPImage_t* copybmp=ReadBMP("s1_EARTH.bmp");
	int width=bmp->header.width_px;
	int height=bmp->header.height_px;
	int totalpixel=bmp->header.height_px * bmp->header.width_px;
	int* grayscalearray = (int*) malloc (sizeof(int)*totalpixel);
	int i=0;
	int local_index = 0;
	int chunksize = totalpixel/size;
	int start_index = rank * chunksize;
	int end_index = (rank + 1) * chunksize;
	int *localgrayscalearray = (int*) malloc (sizeof(int)*chunksize);
	int x, y;
	
	switch (number){
		case 1:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
			
			
			for ( i=start_index ;i<end_index && i < totalpixel; i++) {
				x = i % width;
				y = i / width;
				RGB_t colour = GetPixel(bmp, x, y);
				localgrayscalearray[local_index]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.b;
				local_index++;
			}

			
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
			
			
			if (rank==0){
				int x,y;
				int index = 0;
				int lastrow = totalpixel - width + 1;
				for (i=lastrow; i<totalpixel; i++){
					x = i % width;
					y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}	
				for (y = 0; y < height; y++) {
					for ( x = bmp->header.width_px-1; x >= 0; x--) {
						SetPixel(bmp,x,y,grayscalearray[index]);
						index++;
					}
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_mpi.bmp");

			}
			break;
		case 2:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
					
			for ( i=start_index ;i<end_index && i < totalpixel; i++) {
				x = i % width;
				y = i / width;
				RGB_t colour = GetPixel(bmp, x, y);
				localgrayscalearray[local_index]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.b;
				local_index++;
			}

				
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
			
			
			if (rank==0){
				int x,y;
				int index = 0;
				int lastrow = totalpixel - width + 1;
				for (i=lastrow; i<totalpixel; i++){
					x = i % width;
					y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}	
				for (y = height - 1; y >= 0; y--) {
					for ( x = 0; x < width; x++) {
						SetPixel(bmp,x,y,grayscalearray[index]);
						index++;
					}
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_mpi.bmp");
			}
			break;
		default:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
			for ( i=start_index ;i<end_index && i < totalpixel; i++) {
				int x = i % width;
				int y = i / width;
				RGB_t colour = GetPixel(bmp, x, y);
				localgrayscalearray[local_index]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				local_index++;
			}
			
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);

						if (rank==0){
				int lastrow = totalpixel - width + 1;
				int x,y;
				for (i=lastrow; i<totalpixel; i++){
					x = i % width;
					y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}	
				
				int index = 0;
				for (y = 0; y < height; y++) {
					for ( x = 0; x < width; x++) {
						SetPixel(bmp,x,y,grayscalearray[index]);
						index++;
					}
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_mpi.bmp");			}
			
			break;
	}
	if (rank == 0){
		const int DAS_NANO_SECONDS_IN_SEC = 1000000000;
		long timeElapsed_s = end.tv_sec - start.tv_sec;
		long timeElapsed_n = end.tv_nsec - start.tv_nsec;
		//If we have a negative number in timeElapsed_n , borrow a carry from seconds
		if ( timeElapsed_n < 0 ) 
		{timeElapsed_n =DAS_NANO_SECONDS_IN_SEC + timeElapsed_n; timeElapsed_s--;}
		printf("Time: %ld.%09ld secs \n",timeElapsed_s,timeElapsed_n);
	}
	
	MPI_Finalize();
	return 0;
}