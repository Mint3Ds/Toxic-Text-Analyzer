#include <stdio.h>      // For file I/O (fopen, fgets, printf)
#include <string.h>
#define MAX_LEN 255
#define MAX_WORDS 100
#include "sortingAlgos.h"

static void swap(FoundWord* a, FoundWord* b) {
    FoundWord temp;
    FoundWord t = *a;
    *a = *b;
    *b = t;
}

//bubble sort based on frequency
void frequencyBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j].frequency > arr[j + 1].frequency) {
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

//bubble sort alphabetically
void alphaBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (strcmp(arr[j].word, arr[j+1].word) > 0) { //if the first string has a higher ascii value than the second (higher in alphabet)
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }
        //input should be normalised so dont care about capitals)

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

//bubble sort based on severity
void severityBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    int severity = 1;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j].severity > arr[j + 1].severity) {
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

void alphaSeverityBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j].severity > arr[j + 1].severity ||
                (strcmp(arr[j].word, arr[j+1].word) > 0 && arr[j].severity == arr[j + 1].severity)) {
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

void frequencySeverityBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j].severity > arr[j + 1].severity ||
                (arr[j].frequency > arr[j + 1].frequency && arr[j].severity == arr[j + 1].severity)) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

void toxicityScoreBubbleSort(FoundWord arr[], int size){
    int i, j;
    FoundWord temp;
    int swapped=0;
    for (i = 0; i < size - 1; i++) { 
        swapped = 0;
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j].toxicityScore > arr[j+1].toxicityScore) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == 0)
            break;
    }
}

static void frequencyMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { //based on frequency
        if (leftArr[i].frequency < rightArr[j].frequency) {
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

// The subarray to be sorted is in the index range [left-right]
void frequencyMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        frequencyMergeSort(arr, left, mid);
        frequencyMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        frequencyMerge(arr, left, mid, right);
    }
}

static void alphaMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { 
        if (strcmp(leftArr[i].word, rightArr[j].word) < 0) { //based on alphabetical order, if left array has lower ascii value (lower on alphabet)
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void alphaMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        alphaMergeSort(arr, left, mid);
        alphaMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        alphaMerge(arr, left, mid, right);
    }
}

static void severityMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { 
        if (leftArr[i].severity < rightArr[j].severity) { //based on severity
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void severityMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        severityMergeSort(arr, left, mid);
        severityMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        severityMerge(arr, left, mid, right);
    }
}

static void frequencySeverityMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { 
        if (leftArr[i].severity < rightArr[j].severity ||
            (leftArr[i].frequency < rightArr[j].frequency && leftArr[i].severity == rightArr[j].severity)) { //based on severity
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void frequencySeverityMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        frequencySeverityMergeSort(arr, left, mid);
        frequencySeverityMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        frequencySeverityMerge(arr, left, mid, right);
    }
}

static void alphaSeverityMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { 
        if (leftArr[i].severity < rightArr[j].severity  ||
            (strcmp(leftArr[i].word, rightArr[j].word) < 0 && leftArr[i].severity == rightArr[j].severity)) { //based on severity
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void alphaSeverityMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        alphaSeverityMergeSort(arr, left, mid);
        alphaSeverityMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        alphaSeverityMerge(arr, left, mid, right);
    }
}

static void toxicityScoreMerge(FoundWord arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    FoundWord leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { 
        if (leftArr[i].toxicityScore < rightArr[j].toxicityScore) { //based on toxicity score
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void toxicityScoreMergeSort(FoundWord arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        toxicityScoreMergeSort(arr, left, mid);
        toxicityScoreMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        toxicityScoreMerge(arr, left, mid, right);
    }
}

static int frequencyPartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    int pivot = arr[high].frequency;
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].frequency < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void frequencyQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = frequencyPartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        frequencyQuickSort(arr, low, pi - 1);
        frequencyQuickSort(arr, pi + 1, high);
    }
}

static int alphaPartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    char pivot[MAX_LEN] = {};
    strcpy(pivot, arr[high].word);
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (strcmp(arr[j].word, pivot) < 0) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void alphaQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = alphaPartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        alphaQuickSort(arr, low, pi - 1);
        alphaQuickSort(arr, pi + 1, high);
    }
}

static int severityPartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    int pivot = arr[high].severity;
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].severity < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void severityQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = severityPartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        severityQuickSort(arr, low, pi - 1);
        severityQuickSort(arr, pi + 1, high);
    }
}

static int alphaSeverityPartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    char wordPivot[MAX_LEN] = {};
    strcpy(wordPivot, arr[high].word);
    int severityPivot = arr[high].severity;
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].severity < severityPivot ||
            (strcmp(arr[j].word, wordPivot) < 0 && arr[j].severity == severityPivot)) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void alphaSeverityQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = alphaSeverityPartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        alphaSeverityQuickSort(arr, low, pi - 1);
        alphaSeverityQuickSort(arr, pi + 1, high);
    }
}

static int frequencySeverityPartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    int frequencyPivot = arr[high].frequency;
    int severityPivot = arr[high].severity;
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].severity < severityPivot ||
           (arr[j].severity == severityPivot && arr[j].frequency < frequencyPivot)) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void frequencySeverityQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = frequencySeverityPartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        frequencySeverityQuickSort(arr, low, pi - 1);
        frequencySeverityQuickSort(arr, pi + 1, high);
    }
}

static int toxicityScorePartition(FoundWord arr[], int low, int high) {
    
    // Choose the pivot
    int pivot = arr[high].toxicityScore;
    
    // Index of smaller element and indicates 
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to 
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].toxicityScore < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
            
        }
    }
    
    // Move pivot after smaller elements and
    // return its position
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void toxicityScoreQuickSort(FoundWord arr[], int low, int high) {
    if (low < high) {
        
        // pi is the partition return index of pivot
        int pi = toxicityScorePartition(arr, low, high);

        // recursion calls for smaller elements
        // and greater or equals elements
        toxicityScoreQuickSort(arr, low, pi - 1);
        toxicityScoreQuickSort(arr, pi + 1, high);
    }
}

void reverseArray(FoundWord arr[], int size) {
    if (!arr || size <= 1) return;   // nothing to do

    for (int i = 0, j = size - 1; i < j; ++i, --j) {
        FoundWord tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void displayList(FoundWord arr[], int size) {
    printf("%-6s  %-20s %-20s %-10s %-15s\n", "Number", "Word", "Frequency", "Severity", "Toxicity Score");
    for (int i = 0; i < size; i++) {
    printf("%6i. %-20s %-20d ", (i+1), arr[i].word, arr[i].frequency); //print current number, word and frequency
    switch (arr[i].severity) //switch for printing the severity
    {
    case UNKNOWN:
        printf("%-10s %-10d\n", "Unknown", arr[i].toxicityScore);
        break;
    case MILD:
        printf("%-10s %-10d\n", "Mild", arr[i].toxicityScore);
        break;
    case MODERATE:
        printf("%-10s %-10d\n", "Moderate", arr[i].toxicityScore);
        break;
    case SEVERE:
        printf("%-10s %-10d\n", "Severe", arr[i].toxicityScore);
        break;
    default:
        printf("Not enough words remaining, or error\n");
        break;
    }
    }
}

static void uniqueWordsMerge(UniqueWords arr[], int left, int mid, int right) {
    int i, j, k;
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Create temporary arrays
    UniqueWords leftArr[leftSize], rightArr[rightSize];

    // Copy data to temporary arrays
    for (i = 0; i < leftSize; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < rightSize; j++)
        rightArr[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = left;
    while (i < leftSize && j < rightSize) { //based on frequency
        if (leftArr[i].frequency < rightArr[j].frequency) {
            arr[k] = leftArr[i];
            i++;
        }
        else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

// The subarray to be sorted is in the index range [left-right]
void uniqueWordsMergeSort(UniqueWords arr[], int left, int right) {
    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        uniqueWordsMergeSort(arr, left, mid);
        uniqueWordsMergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        uniqueWordsMerge(arr, left, mid, right);
    }
}

void uniqueWordsReverseArray(UniqueWords arr[], int size) {
    if (!arr || size <= 1) return;   // nothing to do

    for (int i = 0, j = size - 1; i < j; ++i, --j) {
        UniqueWords tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void nMostWords(UniqueWords arr[], int size, int n) {
    if (!arr) {
        printf("nMostToxicWords: NULL array\n");
        return;
    }
    if (size <= 0) {
        printf("nMostToxicWords: no words (size=%d)\n", size);
        return;
    }
    if (n <= 0) {
        printf("nMostToxicWords: invalid n (%d)\n", n);
        return;
    }

    if (n > size) n = size;  // clamp n to array size

    uniqueWordsMergeSort(arr, 0, size-1); // sort ascending
    uniqueWordsReverseArray(arr, size);   // descending

    printf("\nTop %i Words:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%6i. %-50s %-50d\n", (i+1), arr[i].word ? arr[i].word : "(null)", arr[i].frequency);
    }
}


void nMostToxicWords(FoundWord arr[], int size, int n) {
    if (!arr) {
        printf("nMostToxicWords: NULL array\n");
        return;
    }
    if (size <= 0) {
        printf("nMostToxicWords: no words (size=%d)\n", size);
        return;
    }
    if (n <= 0) {
        printf("nMostToxicWords: invalid n (%d)\n", n);
        return;
    }

    // Sort descending by frequency (assume frequencyMergeSort sorts ascending) 
    frequencyMergeSort(arr, 0, size - 1);
    reverseArray(arr, size);

    printf("\nTop %d Toxic Words:\n", n);

    int printed = 0;
    for (int i = 0; i < size && printed < n; ++i) {
        if (!arr[i].word) continue;               
        if (arr[i].severity == UNKNOWN) continue; 

        const char *sevstr;
        switch (arr[i].severity) {
            case MILD:     sevstr = "Mild"; break;
            case MODERATE: sevstr = "Moderate"; break;
            case SEVERE:   sevstr = "Severe"; break;
            default:       sevstr = "Unknown"; break;
        }

        /* Use printed+1 to rank only printed toxic words (1..n) */
        printf("%6d. %-50s %-30d %-s\n", printed + 1, arr[i].word, arr[i].frequency, sevstr);

        printed++;
    }

    if (printed == 0) {
        printf("No toxic words found.\n");
    }
}


static void compareIntMetric(const char* fileName1, const char* fileName2, const char* label, int val1, int val2) {
    printf("%s:\n", label);
    printf("File %s: %i vs File %s: %i\n", fileName1, val1, fileName2, val2);

    if (val1 > val2) {
        float percent = val2 != 0 ? ((float)(val1 - val2) / val2) * 100 : 0.0f;
        printf("File %s has a higher %s, with %i. It is %i or %.2f%% higher than %s\n\n", 
               fileName1, label, val1, val1 - val2, percent, fileName2);
    } else if (val1 < val2) {
        float percent = val1 != 0 ? ((float)(val2 - val1) / val1) * 100 : 0.0f;
        printf("File %s has a higher %s, with %i. It is %i or %.2f%% higher than %s\n\n", 
               fileName2, label, val2, val2 - val1, percent, fileName1);
    } else {
        printf("They have equal %s.\n\n", label);
    }
}

// Helper function for percentage comparisons
static void compareDoubleMetric(const char* fileName1, const char* fileName2, const char* label, double val1, double val2) {
    printf("%s:\n", label);
    printf("File %s: %.2f%% vs File %s: %.2f%%\n", fileName1, val1, fileName2, val2);

    if (val1 > val2) {
        printf("File %s is %.2f percentage points higher than %s\n\n", fileName1, val1 - val2, fileName2);
    } else if (val1 < val2) {
        printf("File %s is %.2f percentage points higher than %s\n\n", fileName2, val2 - val1, fileName1);
    } else {
        printf("Their %s percentages are equal.\n\n", label);
    }
}

void compareTwoFiles(char* fileName1, char* fileName2, detectedWordData toxicResults1, detectedWordData benignResults1, detectedWordData toxicResults2, detectedWordData benignResults2) {

    printf("Comparing %s with %s\n\n", fileName1, fileName2);

    // ------------------ TOXICITY ------------------
    printf("Comparing Toxicity\n");

    compareIntMetric(fileName1, fileName2, "Toxicity Score", toxicResults1.totalToxicityScore, toxicResults2.totalToxicityScore);
    compareIntMetric(fileName1, fileName2, "Unique Toxic Words", toxicResults1.uniqueDetectedCount, toxicResults2.uniqueDetectedCount);
    compareDoubleMetric(fileName1, fileName2, "Toxic Percentage", toxicResults1.wordPercent, toxicResults2.wordPercent);
    compareIntMetric(fileName1, fileName2, "Total Detected (non-unique) Toxic Words", toxicResults1.totaldetectedWord, toxicResults2.totaldetectedWord);
    compareIntMetric(fileName1, fileName2, "Total Mild Toxic Words", toxicResults1.mildFreq, toxicResults2.mildFreq);
    compareIntMetric(fileName1, fileName2, "Total Moderate Toxic Words", toxicResults1.moderateFreq, toxicResults2.moderateFreq);
    compareIntMetric(fileName1, fileName2, "Total Severe Toxic Words", toxicResults1.severeFreq, toxicResults2.severeFreq);

    // ------------------ BENIGNITY ------------------
    printf("Comparing Benignity\n");

    compareIntMetric(fileName1, fileName2, "Benignity Score", benignResults1.totalToxicityScore, benignResults2.totalToxicityScore);
    compareIntMetric(fileName1, fileName2, "Unique Benign Words", benignResults1.uniqueDetectedCount, benignResults2.uniqueDetectedCount);
    compareDoubleMetric(fileName1, fileName2, "Benign Percentage", benignResults1.wordPercent, benignResults2.wordPercent);
    compareIntMetric(fileName1, fileName2, "Total Detected (non-unique) Benign Words", benignResults1.totaldetectedWord, benignResults2.totaldetectedWord);
    compareIntMetric(fileName1, fileName2, "Total Mild Benign Words", benignResults1.mildFreq, benignResults2.mildFreq);
    compareIntMetric(fileName1, fileName2, "Total Moderate Benign Words", benignResults1.moderateFreq, benignResults2.moderateFreq);
    compareIntMetric(fileName1, fileName2, "Total Severe Benign Words", benignResults1.severeFreq, benignResults2.severeFreq);
}
