#ifndef sortingAlgos_h
#define sortingAlgos_h
#include "tokenisation.h"
#include "worddetection.h"
#include <stdio.h>      // For file I/O (fopen, fgets, printf)
#include <string.h>
#define MAX_LEN 255
#define MAX_WORDS 100

//sorts through the array of structures base on the frequency using bubble sort by ascending order. takes the array and its size as arguments.
void frequencyBubbleSort(FoundWord arr[], int size);

//sorts through the array of structures alphabetically using bubble sort by ascending order. takes the array and its size as arguments. 
void alphaBubbleSort(FoundWord arr[], int size);

//sorts through the array of structures based on severity using bubble sort by ascending order. takes the array and its size as arguments. 
void severityBubbleSort(FoundWord arr[], int size);

//sorts through the array of structures based on severity and alphabetically using bubble sort by ascending order. takes the array and its size as arguments. 
void alphaSeverityBubbleSort(FoundWord arr[], int size);

//sorts through the array of structures based on severity and frequency using bubble sort by ascending order. takes the array and its size as arguments. 
void frequencySeverityBubbleSort(FoundWord arr[], int size);

void toxicityScoreBubbleSort(FoundWord arr[], int size);

//sorts through an array of structures based on frequency using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void frequencyMergeSort(FoundWord arr[], int left, int right);

//sorts through an array of structures based on alpha using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void alphaMergeSort(FoundWord arr[], int left, int right);

//sorts through an array of structures based on severity using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void severityMergeSort(FoundWord arr[], int left, int right);

//sorts through an array of structures based on frequency and severity using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void frequencySeverityMergeSort(FoundWord arr[], int left, int right);

//sorts through an array of structures based on alphabetical order and severity using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void alphaSeverityMergeSort(FoundWord arr[], int left, int right);

//sorts through an array of structures based on toxicity score using merge sort by ascending order. takes the left-most index and right-most index as parameters. (0 and size-1)
void toxicityScoreMergeSort(FoundWord arr[], int left, int right);

//quicksorts the array based on frequency. takes the array, and the lowest and highest index numbers (0 and size-1)
void frequencyQuickSort(FoundWord arr[], int low, int high);

//quicksorts the array based on frequency. takes the array, and the lowest and highest index numbers (0 and size-1)
void alphaQuickSort(FoundWord arr[], int low, int high);

//quicksorts the array based on alphabetical order. takes the array, and the lowest and highest index numbers (0 and size-1)
void severityQuickSort(FoundWord arr[], int low, int high);

//quicksorts the array based on severity and alphabet. takes the array, and the lowest and highest index numbers (0 and size-1)
void alphaSeverityQuickSort(FoundWord arr[], int low, int high);

//quicksorts the array based on severity and frequency. takes the array, and the lowest and highest index numbers (0 and size-1)
void frequencySeverityQuickSort(FoundWord arr[], int low, int high);
 
//quicksort based on toxicity score. takes the array, and the lowest and highest index numbers (0 and size-1)
void toxicityScoreQuickSort(FoundWord arr[], int low, int high);

//reverses the order of an array. for descending order. takes in the array of structures and size.
void reverseArray(FoundWord arr[], int size);

//displays the top N most frequent words. takes in the uniqueWords struct. takes the array, size, and N words as arguments
void nMostWords(UniqueWords arr[], int size, int n);
//displays the top N most toxic words. takes the array, size, and N words as arguments
void nMostToxicWords(FoundWord arr[], int size, int n);

//displays the current array. takes the array and size of the array as argument.
void displayList(FoundWord arr[], int size);

void uniqueWordsMergeSort(UniqueWords arr[], int left, int right);
void uniqueWordsReverseArray(UniqueWords arr[], int size);
void compareTwoFiles(char* fileName1, char* fileName2, detectedWordData toxicResults1, detectedWordData benignResults1, detectedWordData toxicResults2, detectedWordData benignResults2);

#endif