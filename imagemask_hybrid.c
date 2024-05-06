#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include <mpi.h>
#include <time.h>
#include <omp.h>
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
	int width=bmp->header.width_px;
	int height=bmp->header.height_px;
	int totalpixel=bmp->header.height_px * bmp->header.width_px;
	int* grayscalearray = (int*) malloc (sizeof(int)*totalpixel);
	int i=0;
	int chunksize = totalpixel/size;
	int start_index = rank * chunksize;
	int end_index = (rank + 1) * chunksize;
	int *localgrayscalearray = (int*) malloc (sizeof(int)*chunksize);
	
	switch (number){
		case 1:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
			#pragma omp parallel for private (i)
				for ( i=start_index ;i<end_index; i++) {
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					localgrayscalearray[ y * width + x - (rank * chunksize) ]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}
			
			
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
			
			
			if (rank==0){
			
				int lastrow = totalpixel - width + 1;
				for (i=lastrow; i<totalpixel; i++){
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}			
				#pragma omp parallel for private (i)
					for ( i=0 ;i<totalpixel; i++) {
						int x = i % width;
						int y = i / width;
						SetPixel(bmp,width - 1 - x,y,grayscalearray[i]);
					}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_hybrid.bmp");
			}
			break;
		case 2:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
			#pragma omp parallel for private (i)
				for ( i=start_index ;i<end_index; i++) {
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					localgrayscalearray[ y * width + x - (rank * chunksize) ]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}
			
			
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
		
			
			if (rank==0){
			
				int lastrow = totalpixel - width + 1;
				for (i=lastrow; i<totalpixel; i++){
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}			
				#pragma omp parallel for private (i)
					for ( i=0 ;i<totalpixel; i++) {
						int x = i % width;
						int y = i / width;
						SetPixel(bmp,x,height - 1 - y,grayscalearray[i]);
					}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_hybrid.bmp");
			}
			break;
		default:
			if (rank==0){
				clock_gettime(CLOCK_MONOTONIC, &start);
			}
			#pragma omp parallel for private (i)
				for ( i=start_index ;i<end_index; i++) {
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					localgrayscalearray[ y * width + x - (rank * chunksize) ]= 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}
			
		
			MPI_Gather(localgrayscalearray, chunksize, MPI_INT, grayscalearray, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
		
			
			if (rank==0){
			
				int lastrow = totalpixel - width + 1;
				for (i=lastrow; i<totalpixel; i++){
					int x = i % width;
					int y = i / width;
					RGB_t colour = GetPixel(bmp, x, y);
					grayscalearray[i] = 0.2126*colour.r+ 0.7152*colour.g+0.0722*colour.g;
				}			
				#pragma omp parallel for private (i)
					for ( i=0 ;i<totalpixel; i++) {
						int x = i % width;
						int y = i / width;
						SetPixel(bmp,x,y,grayscalearray[i]);
					}
				clock_gettime(CLOCK_MONOTONIC, &end);
				SaveBMP(bmp,"output_hybrid.bmp");
			}
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
