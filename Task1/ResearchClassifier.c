/*
 ============================================================================
 Name        : ResearchClassifier.c
 Author      : ron
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ResearchClassifier.h"
#include <string.h>

int main(void) {
	int* array = (int*)calloc(200000,sizeof(int));
	char* filePath="/home/ise/Downloads/classifiers_for_ron/calssifier_sets_random_9.txt149.txt";
	FILE* file;
	int counter = 0;
	int total = 0;
	file=(FILE*)fopen(filePath,"r");//only read file
	if (file==NULL)
		printf("Failed to open file.\n");
	char* temp;//saves line in each iteration
	char c;// eatch char in the text
	while((c=fgetc(file))!=EOF){// run until there is notheing else to read
		if (counter==0){
			temp=(char*)malloc(sizeof(char));
		}
		else {
			temp=realloc(temp,sizeof(char)*(counter+1));
		}
		if(c == '\b' || c == ' ' || c=='r' || c=='n'){
			int tempAsInt = (atoi(temp));
			array[total] = tempAsInt;
			free(temp);
			counter = 0;
			//printf("%d\n",tempAsInt);
			total++;
		}
		else {
			*(temp+counter)=c;//add the new string
			counter++;//increase
		}
	}
	fclose(file);
	//printf("total: %d\n",total);
	int* result = rankSet(array);
	printf("res1: %d, res2: %d\n",result[0],result[1]);
}

int* rankSet(int* setScanArray){
	// consists of 3 steps
	// step1: change measures of '-1' to arbitrary number, for example 11
	int* result;
	result = (int*)calloc(2, sizeof(int));
	int i;
	int setArrLength = 200000;
	for(i = 0; i<setArrLength;i = i + 1){
		if (setScanArray[i] == -1){
			setScanArray[i] = 11;
		}
	}
	// step2: normalize scan to binary values - 0 or 1
	int intervals = 100; // the size of each interval
	int trueIntervalsLength = setArrLength / intervals;
	int height = 1;
	int* true_intervals = (int*)calloc(trueIntervalsLength,sizeof(int)); // the normalized scan array
	// now all the cells of true_intervals are initiated to 0
	int zeroCounter = 0; // saves the number of zero's appeared in the current interval
	int trueIntervalsIndex = 0; // saves our current index to true_intervals array
	int numOfHeight = 0; // will save the number of times height appears in true_intervals - for future use
	for (i = 0; i < setArrLength; i = i + 1){
		if (i > 0 && i % intervals == 0){
			if (zeroCounter <= (int)(((float)intervals) * 0.2)){
				true_intervals[trueIntervalsIndex] = height;
				numOfHeight = numOfHeight + 1;
			}
			zeroCounter = 0;
			trueIntervalsIndex = trueIntervalsIndex + 1;
		}
		if (setScanArray[i] == 0){
			zeroCounter = zeroCounter + 1;
		}
	}
	// update the info about the last interval
	if (zeroCounter <= (int)(((float)intervals) * 0.2)){
		true_intervals[trueIntervalsIndex] = height;
		numOfHeight = numOfHeight + 1;
	}
	/*for(int h = 0;h<trueIntervalsLength;h++){
		if(true_intervals[h] != 0)
			printf("cell%d: %d\n",h,true_intervals[h]);
	}*/
	// step3: check the correlation of the set to second type and third type
	if (numOfHeight > 400 || numOfHeight == 0){ // no correlation
		 return result;
	}
	else if (numOfHeight < 5){ // correlation only the second type
		result[1] = 0;
	}
	int index = 0;
	int* corrects = (int*)calloc(2,sizeof(int));
	int* errors = (int*)calloc(2,sizeof(int));
	int min_correct = 2;
	int max_correct = 6;
	while(index < trueIntervalsLength){
		// advance to next cell that has value = height
		while(index < trueIntervalsLength && true_intervals[index]==0){
			index = index + 1;
		}
		// right now, true_intervals[index] = height
		// now we have to check if the pattern works
		int stepsCounter = 0; // save the length of the pattern 1,0,1,0,1,0,....,(can be more than 5! even 100!)
		int secTypeCounter = 0; // saves how much second type patterns we discovered
		int thirdTypeCounter = 0; // saves how much third type patterns we discovered
		int j = 0; // for deciding wether we use zero or height value in each loop
		int expectedVal;
		while(index < trueIntervalsLength){
			if(j % 2 == 0){
				expectedVal = height;
			}
			else {
				expectedVal = 0;
			}
			int status = detectStep(true_intervals, &index, expectedVal, trueIntervalsLength);
			if(status == 1){
				stepsCounter++;
				j++;
			}
			else{
				break;
			}
		}
		//printf("stepCount: %d\n",stepsCounter);
		int t; // iterate on stepsCounter
		int thresh2 = 0, thresh3 = 0; // thresholds for incrementing  (sec/third)TypeCounter
		for(t = 1; t <= stepsCounter; t++){
			thresh2++;
			thresh3++;
			if(thresh2 != 0 && thresh2 % 5 == 0){
				secTypeCounter++;
				thresh2 = -1;
			}
			if(thresh3 != 0 && thresh3 % 3 == 0){
				thirdTypeCounter++;
				thresh3 = -1;
			}
		}
		if(secTypeCounter == 0){
			errors[0] = errors[0] + 1;
		}
		else {
			corrects[0] = corrects[0] + secTypeCounter;
		}
		if(thirdTypeCounter == 0){
			errors[1] = errors[1] + 1;
		}
		else {
			corrects[1] = corrects[1] + thirdTypeCounter;
		}
	}
	//printf("corrects: %d %d\n",corrects[0],corrects[1]);
	//printf("errors: %d %d\n", errors[0], errors[1]);
	int thresh2 = (float)(errors[0] / (errors[0] + corrects[0]));
	int thresh3 = (float)(errors[1] / (errors[1] + corrects[1]));
	if (!(thresh2 > 0.5 || (corrects[0] < min_correct) || (corrects[0] >= max_correct))){
	    result[0] = (float)(corrects[0] / (corrects[0] + errors[0]));
	}
	if (!(thresh3 > 0.5 || (corrects[1] <= min_correct) || (corrects[1] >= max_correct))){
	    result[1] = (float)(corrects[1] / (corrects[1] + errors[1]));
	}
	//printf("%d %d \n",result[0], result[1]);
	return result;
}

int detectStep(int* trueIntervals, int* index, int expectedVal, int arrLength){
	/* args:
	 * trueIntervals - pointer to int array of the intervals
	 * index - pointer to the index to the array
	 * expectedVal - the value need to be consistent, can be zero or height.
	 * arrLength - the length of trueIntervals array
	 returns:
	 * 1 if the expectedVal appeared consistenly, otherwise 0.
	 */
	int counter = 0;
	while((*index < arrLength) && (trueIntervals[*index] == expectedVal)){
		counter = counter + 1;
		*index = *index + 1;
	}
	if (counter >= 2 && counter <= 6){
		return 1;
	}
	return 0;
}



