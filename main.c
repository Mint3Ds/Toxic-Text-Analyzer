#include <stdio.h>
#include <string.h>
//used for free and exit. aka malloc stuff.
#include <stdlib.h>
#include "textinput.h"
#include "tokenisation.h"
#include "worddetection.h"
#include "sortingAlgos.h"
#include "output.h"

//sets the maximum number of files that can uploaded at a time to 5 files.
#define maxFile 5
//sets the maximum filename to 255 characters.
//255 because the 256th is the /0.
#define maxFileLen 256

//for the bar chart
#define MAX_BAR_WIDTH 100 

void ascdscDisplayList (FoundWord* list, char* filename, int size);
void printSortingAlgosMenuActual (void);
void sortingAlgos (FileData* allFiles, int fileNum, AnalysisData* tokenised, detectedWordData* toxicResults);
char* getFileName(const char* filepath);
void ASCIIbar(FoundWord arr[], int totaldata, int size);
void uniqueASCIIbar(UniqueWords arr[], int totaldata, int size);

void printMenu() {
    printf("\n1. Load files\n");
    printf("2. Tokenize and analyse files\n");
    printf("3. Do toxic word detection\n");
    printf("4. Do benign word detection\n");
    printf("5. Update toxic word list\n");
    printf("6. Update benign word list\n");
    printf("7. Save analysis results into output file\n");
    printf("8. Load toxic/benign/stop words list from output file\n");
    printf("9. Sorting algorithms /display top N Words / Bar Chart\n");
    printf("10. Compare Two Files\n");
    printf("11. Exit\n");
    printf("Enter choice: ");
}

int main() {
    //initalise all needed variables.
    FileData* allFiles = NULL;
    AnalysisData* tokenised = NULL;
    detectedWordData* toxicResults = NULL;
    detectedWordData* benignResults = NULL;
    int user = 0, fileNum = 0, confirm = 0;

    //flags to check if the stage was already done.
    int doneStage1 = 0;
    int doneStage2 = 0;
    int doneToxicDetect = 0;
    int doneBenignDetect = 0;

    while (1) {
        printMenu();
        if (scanf("%d", &user) != 1) {
            printf("\nError! Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        //exit the program if the user chooses to.
        if (user == 11) {
            printf("Exiting program...\n");
            break;
        }

        //chooses the case based on the number user entered.
        switch (user) {
            case 1: {
                //checks if the user has previously loaded files before.
                if (doneStage1) {
                    //if yes, then free and empty all the previous data since we don't need them anymore.
                    freeMemory(allFiles, fileNum);
                    freeStage2memory(tokenised, fileNum);
                    freeStage3memory(toxicResults, fileNum);
                    freeStage3memory(benignResults, fileNum);
                    allFiles = NULL; 
                    tokenised = NULL; 
                    toxicResults = NULL; 
                    benignResults = NULL;
                }

                printf("\nEnter how many files to analyse (up to %d files can be analysed at a time): ", maxFile);
                //loops until the user inputs the correct format.
                while(1) {
                    //checks if user entered the wrong format or negative/zero file number or a file number that's over the max limit.
                    //if yes, ask the user to try again.
                    if (scanf("%d", &fileNum) != 1 || fileNum <= 0 || fileNum > maxFile) {
                        printf("Invalid input. Please enter a number between 1 and %d: ", maxFile);
                        //clear the scanf input so we don't get infinite loop.
                        while (getchar() != '\n');
                    } else {
                        //else clear the scanf input and exit the while loop.
                        while (getchar() != '\n');
                        break;
                    }
                }

                char filePath[maxFile][maxFileLen];
                for (int i = 0; i < fileNum; i++) {
                    //+2 as we need to include the additional /n/0 if it's an overflow.
                    char tempPath[maxFileLen + 2];
                    printf("\nEnter path for file %d: ", i + 1);

                    //make sure filepath isn't too long.
                    while(1) {
                        fgets(tempPath, sizeof(tempPath), stdin);
                        //needed because fgets includes the \n at the end, so we need to replace that with \0 to end the string. 
                        //otherwise, when opening the file later on, it will open "file.txt\n" instead.
                        //strcspn(tempPath, "\n") looks for the position in tempPath where the character is "\n".
                        //then that position's value will be replaced by \0.
                        tempPath[strcspn(tempPath, "\n")] = '\0';
                        if (strlen(tempPath) >= maxFileLen) {
                            //-1 because the last (aka 256th) character must be used for the /0.
                            //so the user cannot replace the 256th character.
                            printf("\nFile path is too long! Max length is %d characters. Please try again: ", maxFileLen - 1);
                        } else {
                            //else, copy over the filename.
                            strcpy(filePath[i], tempPath);
                            break; 
                        }
                    }
                }
                
                //creates pointers for the each element of the array.
                const char *pathPointer[maxFile];
                for (int i = 0; i < fileNum; i++) {
                    pathPointer[i] = filePath[i];
                }
                
                //load the files.
                allFiles = loadFiles(pathPointer, fileNum);
                
                //set the flag for doneStage1 to true.
                doneStage1 = 1;
                //set the flags for all the other stages to false since new files are loaded, so the old ones can't be used.
                doneStage2 = 0;
                doneToxicDetect = 0;
                doneBenignDetect = 0;

                //print the total word count along with a disclaimer.
                printf("\nNote: This word count may be unreliable!\n");
                printf("As cases such as ' ! ' where a punctuation is separated by spaces would also count as a word here.");
                printf("\nFor a more accurate word count, please run option 2 to tokenise and analyse the file(s)!\n");
                printTotalWord(allFiles, fileNum);
                break;
            }

            case 2:
                //checks if the file has not been previously loaded.
                if (!doneStage1) {
                    //if yes, print error and tell the user to load it first since we need the variables.
                    printf("\nError! You must load the files first (option 1) to use this function.\n");
                } else {
                    //if no, free the memory first in case the files were tokenised before.
                    freeStage2memory(tokenised, fileNum);
                    //do the tokenisation and word analysis. 
                    tokenised = wordAnalysis(allFiles, fileNum);
                    //print the results.
                    printWordAnalysis(tokenised, fileNum);
                    
                    //set the doneStage2 flag to true.
                    doneStage2 = 1;
                    //set the later stages' flags to false just in case.
                    //since stage 2 was rerun, so some information may have changed.
                    doneToxicDetect = 0;
                    doneBenignDetect = 0;
                }
                break;

            case 3:
                //checks if the file has not been previously tokenised.
                if (!doneStage2) {
                    //if yes, print error and tell the user to tokenise it first since we need the variables.
                    printf("\nError! You must tokenise the files first (option 2) to use this function.\n");
                } else {
                    //if no, free the memory first in case the files had been detected before.
                    freeStage3memory(toxicResults, fileNum);
                    printf("\nDetecting the toxic words... Please be patient! It can take up to a minute.\n");
                    //do the toxic word detection.
                    toxicResults = wordDetection("toxicwords.txt", allFiles, tokenised, fileNum);
                    //check if the detection was successful.
                    if (toxicResults) {
                        //if yes, then print out the detection information.
                        printdetectedWordData(toxicResults, fileNum, "Toxic Words");
                        //set the doneToxicDetect flag to true.
                        doneToxicDetect = 1;
                    }
                }
                break;

            case 4:
                //literally just case 3 but the toxic parts are swapped to benign.
                //checks if the file has not been previously tokenised.
                if (!doneStage2) {
                    //if yes, print error and tell the user to tokenise it first since we need the variables.
                    printf("\nError! You must tokenise the files first (option 2) to use this function.\n");
                } else {
                    //if no, free the memory first in case the files had been detected before.
                    freeStage3memory(benignResults, fileNum);
                    printf("\nDetecting the benign words... Please be patient! It can take up to a minute.\n");
                    //do the benign word detection.
                    benignResults = wordDetection("benignwords.txt", allFiles, tokenised, fileNum);
                    //check if the detection was successful.
                    if (benignResults) {
                        //if yes, then print out the detection information and set the benign flag to true.
                        printdetectedWordData(benignResults, fileNum, "Benign Words");
                        doneBenignDetect = 1;
                    }
                }
                break;

            case 5:
                //update the toxic word list.
                updateWordList("toxicwords.txt");
                break;

            case 6:
                //update the benign word list.
                updateWordList("benignwords.txt");
                break;

            case 7:
                //basically checks if all the previous options were already run first since we need those values.
                if (!doneStage1) {
                    printf("\nError! You must first load the files first (option 1) to use this function");
                    break;
                } else if (!doneStage2) {
                    printf("\nError! You must tokenise the files first (option 2) to use this function.\n");
                    break;
                } else if (!doneToxicDetect || !doneBenignDetect) {
                    printf("\nError! You must run toxic (option 3) and benign (option 4) word detections before saving.\n");
                    break;
                } else {
                    printf("\nCurrently writing... (this may take a minute for larger datasets):\n");
                    writeWordAnalysis(allFiles, tokenised, toxicResults, benignResults, fileNum);
                }
                break;

            case 8:
                //print warning.
                printf("\nWARNING! This option will overwrite your current stopwords.txt, benignwords.txt, and toxicwords.txt\n");
                printf("\nAdditionally, if the required information isn't in the file, then your word list will become empty.\n");
                printf("If you wish to proceed, type 1: ");
                //exits if user doesn't enter an integer.
                if (scanf("%d", &confirm) != 1) {
                    printf("\nUser did not enter 1, so this option will be exited.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');
                //checks if user entered 1.
                if (confirm == 1) {
                    //if yes then get the report file name.
                    char reportName[256];
                    printf("\nEnter the name of the analysis report file to load from (e.g. ngram.csv): ");
                    fgets(reportName, sizeof(reportName), stdin);
                    //clear the \r\n.
                    reportName[strcspn(reportName, "\r\n")] = 0;
                    reloadList(reportName);
                } else {
                    //else exit.
                    printf("\nUser did not enter 1, so this option will be exited.\n");
                }
                break;
            case 9:
                //checks if all the required stuff is done.
                if (!doneStage1) {
                    printf("\nError! You must first load the files first (option 1) to use this function");
                    break;
                }
                if (!doneStage2) {
                    printf("\nError! You must tokenise the files first (option 2) to use this function.\n");
                    break;
                } else if (!doneToxicDetect || !doneBenignDetect) {
                    printf("\nError! You must run toxic (option 3) and benign (option 4) before sorting.\n");
                    break;
                } 
                sortingAlgos(allFiles, fileNum, tokenised, toxicResults); //asks user for what type of sort (alphabetical, severity, frequency, toxicityRating) and algo (bubble, merge, quick)
                break;

            case 10:
                if (!doneStage1) {
                    printf("\nError! You must first load the files first (option 1) to use this function");
                    break;
                }
                if (!doneStage2) {
                    printf("\nError! You must tokenise the files first (option 2) to use this function.\n");
                    break;
                } else if (!doneToxicDetect || !doneBenignDetect) {
                    printf("\nError! You must run toxic (option 3) and benign (option 4) before sorting.\n");
                    break;
                } 
                if (fileNum < 2) {
                    printf("Error: You must load at least two files in order to compare them: \n");
                    break;
                }
                printf("You must have the files you want to compare loaded via Option 1 (Load files)\n");
                printf("List of Files Currently Loaded:\n");
                for (int i = 0 ; i < fileNum; ++i) {
                    printf("%i. %s\n", (i+1), allFiles[i].filename);
                }
                int compareInput1;
                int compareInput2;
                while (1) {
                    printf("Select the first file you want to compare from: ");
                    if (scanf("%d", &compareInput1) != 1 || compareInput1 < 1 || compareInput1 > fileNum) { //first input failure
                        printf("Invalid input. Please enter a number from the List of Files Currently Loaded:\n");
                        //clear the scanf input so we don't get infinite loop.
                        while (getchar() != '\n');
                    } else { //first input success
                        while (getchar() != '\n');
                        printf("Select the second file you want to compare to: ");
                        if (scanf("%d", &compareInput2) != 1 || compareInput2 < 1 || compareInput2 > fileNum) { //second input failure
                            printf("Invalid input. Please enter a number from the List of Files Currently Loaded:\n");
                            //clear the scanf input so we don't get infinite loop.
                            while (getchar() != '\n');
                        } else { //second input success
                            while (getchar() != '\n');
                            if (compareInput1 == compareInput2) { // same file error
                                printf("You cannot compare a file with itself. Enter two different numbers from the list to compare to eachother: ");
                            } else { //final success
                                printf("Input sucessful. Starting comparison now...");
                                char* fileName1 = NULL;
                                char* fileName2 = NULL;
                                fileName1 = getFileName(allFiles[compareInput1-1].filename);
                                fileName2 = getFileName(allFiles[compareInput2-1].filename);
                                compareTwoFiles(fileName1, fileName2, toxicResults[compareInput1-1], benignResults[compareInput1-1], toxicResults[compareInput2-1], benignResults[compareInput2-1]);
                                break;
                            }
                          
                        }
                    }
                }
                break;
            case 11:
                //user chose to exit so free up all the memory before exiting to prevent memory leak.
                printf("Cleaning up all allocated resources...\n");
                freeMemory(allFiles, fileNum);
                freeStage2memory(tokenised, fileNum);
                freeStage3memory(toxicResults, fileNum);
                freeStage3memory(benignResults, fileNum);
                printf("Done!\n");
                break;

            default:
                printf("\nError! Please enter a valid number from one of the available options.\n");
                break;
        }
    }

    //user chose to exit so free up all the memory before exiting to prevent memory leak.
    printf("Cleaning up all allocated resources...\n");
    freeMemory(allFiles, fileNum);
    freeStage2memory(tokenised, fileNum);
    freeStage3memory(toxicResults, fileNum);
    freeStage3memory(benignResults, fileNum);
    printf("Done!\n");

    return 0; 
}

void sortingAlgos (FileData* allFiles, int fileNum, AnalysisData* tokenised, detectedWordData* toxicResults) {
    printf("\nSorting Algorithmns Menu\n");
    printf("1. Display N most Toxic words / Bar Chart\n");
    printf("2. Display N most frequent words / Bar Chart\n");
    printf("3. Frequency sort\n");
    printf("4. Alphabetical sort\n");
    printf("5. Severity Sort\n");
    printf("6. Severity Sort with Frequency\n");
    printf("7. Severity Sort with Alphabetical Order\n");
    printf("8. Toxicity Score Sort\n");
    printf("9. Exit\n");
    printf("Insert which sort you want here: ");
    int sortingInput;
    while (1) {
        if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 9) {
            printf("Invalid input. Please enter a number between 1 and 9: ");
            //clear the scanf input so we don't get infinite loop.
            while (getchar() != '\n');
        } else {
            //else clear the scanf input and exit the while loop.
            while (getchar() != '\n');
            break;
        }
    }
    switch (sortingInput) {
        int nMostInput;
        case 1:
            printf("\nSelect N for Top N Most Toxic Words: ");
            while (1) {
                if (scanf("%d", &nMostInput) != 1 || sortingInput < 0) {
                    //clear the scanf input so we don't get infinite loop.
                    printf("Invalid input. Please enter a positive number: ");
                    while (getchar() != '\n');
                } else {
                //else clear the scanf input and exit the while loop.
                while (getchar() != '\n');
                break;
                }
            }
            for (int i = 0; i <fileNum; ++i) {
                if (!allFiles[i].loadSuccess || !toxicResults[i].foundWord) {
                    printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                    continue;
                }
                printf("\nFilename: %s\n", allFiles[i].filename);
                nMostToxicWords(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount, nMostInput);
                printf("\nTop %i Most Toxic Words Bar Chart\n", nMostInput);
                ASCIIbar(toxicResults[i].foundWord, nMostInput, toxicResults[i].uniqueDetectedCount);
            }
            break;
        case 2:
            printf("\nSelect N for Top N Most Frequent Words: ");
            scanf("%d", &nMostInput);
            for (int i = 0; i <fileNum; ++i) {
                if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                    printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                    continue;
                }
                printf("Filename: %s", allFiles[i].filename);
                nMostWords(tokenised[i].uniqueWord, tokenised[i].uniqueWordCount, nMostInput);
                printf("\nTop %i Most Frequent Words Bar Chart\n", nMostInput);
                uniqueASCIIbar(tokenised[i].uniqueWord, nMostInput, tokenised[i].uniqueWordCount);
            }
            break;
        case 3:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for frequency: ");
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencyBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencyMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencyQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;
        case 4:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for alphabetical order: ");
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;
        case 5:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for severity: ");
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        severityBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        severityMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        severityQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;
        case 6:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for frequency and severity: ");
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencySeverityBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencySeverityMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        frequencySeverityQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;
        case 7:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for alphabetical order and severity: ");
                //checks for valid input.
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            //calls the sorting function based on what the user chooses.
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaSeverityBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaSeverityMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        alphaSeverityQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;
        case 8:
            while (1) {
                printSortingAlgosMenuActual();
                printf("Select which sorting algorithmn to sort for toxicity score: ");
                //checks for valid input.
                if (scanf("%d", &sortingInput) != 1 || sortingInput < 1 || sortingInput > 3) {
                    printf("Invalid input. Please enter a number between 1 and 9: ");
                    //clear the scanf input so we don't get infinite loop.
                    while (getchar() != '\n');
                } else {
                    //else clear the scanf input and exit the while loop.
                    while (getchar() != '\n');
                    break;
                }
            }
            //calls function based on what user chose.
            switch(sortingInput) {
                case 1:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        toxicityScoreBubbleSort(toxicResults[i].foundWord, toxicResults[i].uniqueDetectedCount);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 2:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        toxicityScoreMergeSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
                case 3:
                    for (int i = 0; i <fileNum; ++i) {
                        if (!allFiles[i].loadSuccess || !tokenised[i].uniqueWordCount) {
                            printf("\nError! The file %s has previously encountered errors.\nThis file will be skipped.\n", allFiles[i].filename);
                            continue;
                        }
                        toxicityScoreQuickSort(toxicResults[i].foundWord, 0, toxicResults[i].uniqueDetectedCount-1);
                        ascdscDisplayList(toxicResults[i].foundWord, allFiles[i].filename, toxicResults[i].uniqueDetectedCount);
                    }
                    break;
            }
            break;

        case 9:
        break;
        default:
        break;
    }
    return;
}

//function to print the sorting order.
void ascdscDisplayList (FoundWord* list, char* filename, int size) {
    int displayInput;
    //loops until the user gives a valid input.
    while (1) {
        printf("Enter 0 for Ascending Order, and 1 for Descending Order: ");
        if (scanf("%d", &displayInput) != 1 || displayInput < 0 || displayInput > 1) {
            printf("Invalid input. Please enter a 0 for ascending order, 1 for descending order: ");
            //clear the scanf input so we don't get infinite loop.
            while (getchar() != '\n');
        } else {
            //else clear the scanf input and exit the while loop.
            while (getchar() != '\n');
            break;
        }
    }
        printf("Filename: %s\n", filename);
        switch (displayInput) {
        case 0:
            displayList(list, size);
            break;
        case 1:
            reverseArray(list, size);
            displayList(list, size);
            break;
        default:
            break;
        }
    return;
}

char* getFileName(const char* filepath) {
    if (!filepath) return NULL;

    // Find start of filename (after last slash)
    const char* fnameStart = strrchr(filepath, '/');
    if (!fnameStart) fnameStart = strrchr(filepath, '\\');
    if (!fnameStart) fnameStart = filepath;
    else fnameStart++;

    // Duplicate filename portion
    char* name = strdup(fnameStart);
    if (!name) return NULL;

    return name;
}


void ASCIIbar(FoundWord arr[], int totaldata, int size) {
    if (!arr || totaldata <= 0) return;

    if (size > 0 && size < totaldata) totaldata = size; // clamp to array bounds

    int maxLen = 0;
    int maxFreq = 0;

    // Find longest word and max frequency safely (manual strlen)
    for (int i = 0; i < totaldata; i++) {
        if (!arr[i].word) continue;

        int len = 0;
        while (arr[i].word[len] != '\0') len++;  // safe, no strlen
        if (len > maxLen) maxLen = len;

        if (arr[i].frequency > maxFreq) maxFreq = arr[i].frequency;
    }

    if (maxFreq == 0) return;

    // Print bars
    for (int i = 0; i < totaldata; i++) {
        if (!arr[i].word) continue;

        // Print word
        int len = 0;
        while (arr[i].word[len] != '\0') len++;
        printf("%-*s | ", maxLen, arr[i].word);

        // Scale the bar
        int barLength = (arr[i].frequency * MAX_BAR_WIDTH) / maxFreq;
        if (barLength == 0 && arr[i].frequency > 0) barLength = 1;

        for (int j = 0; j < barLength; j++) putchar('#');

        printf(" (%d)\n", arr[i].frequency);
    }
}

void printSortingAlgosMenuActual (void) {
    printf("\nSorting Algos\n");
    printf("1. Bubblesort (very bad performance with large datasets, but good memory usage)\n");
    printf("2. Mergesort (decent performance and decent memory usage with larger datasets\n");
    printf("3. Quicksort (decent performance and good memory usage with larger datasets\n");
}

void uniqueASCIIbar(UniqueWords arr[], int totaldata, int size) {
    if (!arr || totaldata <= 0) return;

    if (size > 0 && size < totaldata) totaldata = size; // clamp to array bounds

    int maxLen = 0;
    int maxFreq = 0;

    // Find longest word and max frequency safely (manual strlen)
    for (int i = 0; i < totaldata; i++) {
        if (!arr[i].word) continue;

        int len = 0;
        while (arr[i].word[len] != '\0') len++;
        if (len > maxLen) maxLen = len;

        if (arr[i].frequency > maxFreq) maxFreq = arr[i].frequency;
    }

    if (maxFreq == 0) return;

    // Print bars
    for (int i = 0; i < totaldata; i++) {
        if (!arr[i].word) continue;

        // Print word
        int len = 0;
        while (arr[i].word[len] != '\0') len++;
        printf("%-*s | ", maxLen, arr[i].word);

        // Scale the bar
        int barLength = (arr[i].frequency * MAX_BAR_WIDTH) / maxFreq;
        if (barLength == 0 && arr[i].frequency > 0) barLength = 1;

        for (int j = 0; j < barLength; j++) putchar('#');

        printf(" (%d)\n", arr[i].frequency);
    }
}

