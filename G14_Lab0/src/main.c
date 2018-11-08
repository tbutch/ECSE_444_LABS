#include <stdio.h>
#include <add.h>

void MovingAverageOG(float * dataPointer, float * filteredData, double numberOfSamples, double filterDepth);
	
int main() {
	float samples[12] = {33.3, 44.7, 35.74, 11.2, 27.88, 11.4, 6.9, 44, 10.11, 21, 35.4, 33};
	float filteredSamples[12];
	float* testSample = &samples[5];
	float* firstFilteredData = &filteredSamples[0];

	//MovingAverageOG(testSample, firstFilteredData, 7, 2);
	
	return 0;
}

void MovingAverageOG(float * dataPointer, float * filteredData, double numberOfSamples, double filterDepth){
	
	// if number is odd, even is 0.
	int even = 0;
	if ( ((int) numberOfSamples) % 2 == 0){
		// if number is even, even = 1
		even = 1;
	}
	
	

}
