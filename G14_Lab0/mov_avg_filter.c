#include <stdio.h>

int MovingAverage(int * dataPointer, int * filteredData, double numberOfSamples, double filterDepth){
	
	int even = 1;
	if ( (int) numberOfSamples % 2 == 0){
		even = 0;
	}
	return even;
}
