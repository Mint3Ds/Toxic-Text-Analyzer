#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "textinput.h"

//10mb is set as the maximum file size.
#define maxFileSize 10000000L

//some functions are static so that other files can't call them. also prevent name clashing if another function got same name.
//const char* is used instead of just char* since const makes it read-only. 
//so prevent any accidental editing. aka for safety.
static long getFileSize(const char* filename) {
    //open the file as a binary file and in read-only mode.
    //read-only mode so we don't accidentally change value.
    //binary file because apparently using just r sometimes automatically convert \r\n into just \n.
    //so that'll make our reading inaccurate because the 2 bytes will become 1 instead if we use just "r".
    FILE *file = fopen(filename, "rb");
    //checks if file is NULL aka doesn't exist.
    //if yes, get out of this function with a -1 to show we got an error.
    //we chose -1 instead of 0 since 0 could be a legit file size.
    if (file == NULL) {
        return -1;
    }
    //move the file pointer to the end of the file.
    //SEEK_END is apparently a flag that tells fseek to move to the end.
    fseek(file, 0, SEEK_END);
    //get the position of the pointer. 
    //since the pointer is at the end, this means the pointer position is the file size.
    long size = ftell(file);
    //close the file to prevent memory leak.
    fclose(file);
    //return the file size.
    return size;
}

int countTotalWord(const char *line) {
    //keeps track of how many words there are/
    int words = 0;
    //flag to check if we're currently inside a word.
    int inWord = 0;

    //loops until the current character is \0, aka the end of the line.
    for (int i = 0; line[i] != '\0'; i++) {
        //check if the character is not a space/tab/newline and we're currently not inside a word.
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && !inWord) {
            //if yes, then it means we found the start of a word.
            //set inWord to true and add one to words (aka our word counter).
            inWord = 1;
            words++;
        }
        //check if the character is either a space, tab, or newline. And that we're currently inside a word.
        else if ((line[i] == ' ' || line[i] == '\t' || line[i] == '\n') && inWord) {
            //if yes then it means we got to the end of the word. Set inWord to false.
            inWord = 0;
        }
    }
    //return the total word count.
    return words;
}

int isLineEmpty(const char *line) {
    //iterates through each character of the line until the end of the string.
    for (int i = 0; line[i] != '\0'; i++) {
        //checks if the character is not an empty space or a comma.
        //specifically comma because apparently an empty row will show up as "," when printed out.
        //source is i tested it.
        if (!isspace(line[i]) && line[i] != ',') {
            //a character is found, so this line isn't empty.
            //so return false.
            return 0;
        }
    }
    //return true if it's empty.
    return 1;
}

//this function is used to just print an error message for memory issues and then exit program.
static void printExitError() {
    printf("Error! Memory cannot be allocated.\n");
    printf("The program will be terminated to prevent any further issues.");
    //using exit instead of return because exit terminates the whole program instead of just returning to whoever called this function.
    //we're exiting because memory error will probably make the program unreliable and also something is wrong with your OS.
    exit(1);
}

static int loadCsvLines(int currentFileNum, FILE *file, char*** totalLines, int* totalLinesNum) {
    //currentLine will contain the current line from the file.
    //the max size it can store is 15000 bytes.
    char currentLine[15000];
    //userCol is which column the user wants to load.
    //user is the user's input on yes or no.
    //maxCol contains the number of columns in the .csv file.
    int userCol = 0, user = -1, maxCol = 1;
    //extractLine will contain whatever the line in the column the user chose has.
    char **extractLine = NULL;
    //lineCtr will keep track of how many lines is stored inside extractLine.
    //maxSize is the max size of extractLine.
    int lineCtr = 0, maxSize = 10;

    //headerName will contain the first row of the .csv file.
    char headerName[15000];
    //get the column headers. also check and see if the file is empty.
    //we're checking again since unlike .txt files, .csv file is not 0kb when empty.
    //so the check we did in fileProcess() is useless here.
    if (fgets(headerName, sizeof(headerName), file) == NULL || isLineEmpty(headerName)) {
        printf("Error! File number %d appears to be empty. This file will be skipped.\n", currentFileNum);
        printf ("Please ensure the first row of your file is not empty.\n");
        return -1;
    }

    //gets the number of columns.
    //loops until it reaches the end of the string.
    for (int i = 0; headerName[i] != '\0'; i++) {
        //check if the current character is a comma. if yes, it means the start of an additional column is found.
        if (headerName[i] == ',') {
            maxCol++;
        }
    }

    //print out the first row.
    //we're assuming that the first row contains the header row.
    printf("List of columns found in order: %s", headerName);
    //ask the user to confirm if it's a header row.
    printf("\nIs this row a header row? Enter 1 for yes, 0 for no: ");
    //loops until a valid value is given.
    while (1) {
        scanf("%d", &user);
        //check if the user entered 1. if yes, it's a valid input so break this loop.
        if (user == 1) {
            while (getchar() != '\n');
            break;
        //check if the user entered 0.
        } else if (user == 0) {
            //print error since currently we only support .csv file with a header row.
            printf("\nSorry! Unfortunately, only .csv files with the first row as the header is accepted.");
            printf("\nThis file will be skipped. Please ensure the first row is the header row.\n");
            while (getchar() != '\n');
            return -1;
        //checks if user entered a number other than 0 or 1. if yes, print error.
        } else {
             printf("Error! Only either 0 or 1 is accepted. Please try again: ");
             while (getchar() != '\n');
        }
    }

    printf("Please enter the column number to analyze: ");
    //loops until the user enters a valid column number to process.
    while (1) {
        //checks if user entered an integer and that value is more than 0 and it's not more than the current columns.
        //if yes, exit the loop.
        if (scanf("%d", &userCol) == 1 && userCol >= 1  && userCol <= maxCol) {
            while (getchar() != '\n');
            break;
        //else print error message.
        } else {
            printf("\nError! Invalid column number entered.\nThe leftmost column starts with 1 and the number of columns this file has is %d.\n", maxCol);
            printf("\nPlease enter a valid number (e.g. 2): ");
            while (getchar() != '\n');
        }
    }

    //allocate memory for extractLine.
    //extractLine currently will be able to store 10 (taken from maxSize) char pointers (done by sizeof(char*)).
    extractLine = malloc(maxSize * sizeof(char*));
    //checks if the allocation was unsuccessful.
    if (!extractLine) { printExitError(); }

    //loops until the end of the file.
    while (fgets(currentLine, sizeof(currentLine), file)) {
        //splitCol will contain one column (aka cell) from the current line.
        char *splitCol;
        //currentColumn keeps track of what column we're currently on.
        int currentColumn = 1;
        //create tempLine as a copy of currentLine.
        //we create a copy because strtok will directly change the value of tempLine.
        //so if we used currentLine instead, it will cause issues as we need the original line for the while loop.
        char *tempLine = strdup(currentLine);
        //check if the duplication was unsuccessful.
        //if yes, then end the whole program.
        if (!tempLine) { printExitError(); }

        //tokenise/split the line by looking for the start of a new column (aka the comma) or a new line (aka \n).
        splitCol = strtok(tempLine, ",\n");
        //loops as long as there's still an existing column in splitCol to check.
        while (splitCol != NULL) {
            //check if the current column we're looking at is the same column the user wants to process.
            if (currentColumn == userCol) {
                //if yes, check if the array aka extractLine is full.
                if (lineCtr == maxSize) {
                    //if yes, double maxSize.
                    //we double it because apparently it's more memory efficient to double it than just increasing by one each time.
                    //because it takes less cost (e.g. instead of increasing 100 times, by doubling, you only need to increase 10 times).
                    maxSize *= 2;
                    //then increase the size of the array.
                    extractLine = realloc(extractLine, maxSize * sizeof(char*));
                    //checks if the reallocation was unsuccessful.
                    if(!extractLine) { printExitError(); }
                }

                //add the correct cell/data into extractLine.
                extractLine[lineCtr] = strdup(splitCol);
                //increae lineCtr so we can add a new line next loop.
                lineCtr++;
                //break because we found the correct column, so no need to check the other columns.
                break;
            }
            //not the right column, so get the line of the next column.
            splitCol = strtok(NULL, ",\n");
            //increase to check the next column.
            currentColumn++;
        }
        //we reached the end of the current row/line, so free tempLine since we don't need the data inside anymore.
        free(tempLine);
    }
    
    //pass back the extracted lines and total number of extracted lines to the function that called this.
    *totalLines = extractLine;
    *totalLinesNum = lineCtr;
    return 0;
}

FileData* loadFiles(const char *filePath[], int fileNum) {
    //calloc since we need loadSuccess to be initalised as 0.
    FileData* allFiles = calloc(fileNum, sizeof(FileData));
    //check if there was any memory issue.
    if (!allFiles) { printExitError(); }
    //loops until we reached the end of how many files the user uploaded.
    for (int i = 0; i < fileNum; i++) {
        //print the file number and the name.
        printf("\nLoading file number %d. File name is %s.\n", i + 1, filePath[i]);
        
        //initalise the struct with the current file.
        allFiles[i].filename = strdup(filePath[i]);

        //get the size of the current file.
        long fileSize = getFileSize(filePath[i]);
        //checks if the file can't be accessed.
        if (fileSize == -1) {
            //if yes, print an error and skip this file.
            printf("Error! The file could not be accessed to check the size.\n");
            printf("Please ensure the file exists and is readable. This file will be skipped.\n");
            continue;
        //checks if the file is over the max file size limit.
        } else if (fileSize > maxFileSize) {
            printf("Error! The file is too large.\n");
            printf("Please ensure the file isn't bigger than 10MB. This file will be skipped.\n");
            continue;
        //checks if the file size is 0, aka an empty text file.
        } else if (fileSize == 0) {
            printf("File is empty. This file will be skipped.\n");
            continue;
        }

        //open the file in read-only mode.
        FILE *file = fopen(filePath[i], "r");
        //checks if the file is unsuccessfully opened. if yes, print error and skip this file.
        if (!file) {
            printf("Error! The file cannot be opened.\n");
            printf("Please ensure the file exists and is readable. This file will be skipped.\n");
            continue;
        }
        
        //fileLines contains all the file lines.
        //fileLineCount contains the total line count. 
        //status is a flag to show whether the reading was successful. 0 for yes, 1 for no.
        char **fileLines = NULL;
        int fileLineCount = 0;
        int status = 0;

        //find which position in the filename has the last period (.) and put it into extension.
        const char *extension = strrchr(filePath[i], '.');
        //checks if a file extension is found, and that extension is a .csv.
        if (extension && strcmp(extension, ".csv") == 0) {
            //if yes, call the loadCsvLines() function.
            //the result of whether the loading is successful or not is stored into status.
            status = loadCsvLines(i + 1, file, &fileLines, &fileLineCount);
        } else { 
            //else read it as a text file.
            char currentLine[15000];
            //loops until the end of the file.
            while(fgets(currentLine, sizeof(currentLine), file)) {
                //since fileLineCount starts with 0, add 1 first before reallocating the array size.
                fileLineCount++;
                fileLines = realloc(fileLines, fileLineCount * sizeof(char*));
                //find the position of the newline aka \n.
                //once found, 
                currentLine[strcspn(currentLine, "\n")] = 0;
                fileLines[fileLineCount - 1] = strdup(currentLine);
            }
        }
        //cloe the file to prevent memory leak.
        fclose(file);

        //checks if no errors occured.
        if (status == 0) {
            //if yes, then store the data into the struct.
            allFiles[i].lines = fileLines;
            allFiles[i].lineCount = fileLineCount;
            //1 means success file processing for loadSuccess.
            allFiles[i].loadSuccess = 1;
            printf("Successfully loaded %d lines from '%s'!\n", fileLineCount, filePath[i]);
        } else {
            //else since the file has issues, we won't be processing it.
            //so free the memory for this file with issue.
             free(fileLines);
        }
    }
    return allFiles;
}

//counts and prints out the total words since it goes under task 1 for some reason.
long long printTotalWord(const FileData* allFiles, int numFiles) {
    //this contains the total word count of all the files.
    long long allTotalWords = 0;
    //loops as long as there's still a file for us to count.
    for (int i = 0; i < numFiles; i++) {
        //check if the file was loaded successfully.
        if (allFiles[i].loadSuccess) {
            long long totalWords = 0;
            //loops until the end of the file.
            for (int j = 0; j < allFiles[i].lineCount; j++) {
                //counts the number of words in the current line and add it to the file total.
                totalWords += countTotalWord(allFiles[i].lines[j]);
            }
            //print out the total word count for this file.
            printf("File '%s' has %lld words in total.\n", allFiles[i].filename, totalWords);
            //add the total word to the sum of all the files.
            allTotalWords += totalWords;
        }
    }
    //return the sum of all the files' words to the function that called it.
    return allTotalWords;
}

void freeMemory(FileData* allFiles, int numFiles) {
    //if there's no file aka no memory to free then just get out of this function.
    if (allFiles == NULL) { return; }

    //loops until all the files successfully loaded are freed.
    for (int i = 0; i < numFiles; i++) {
        //checks if memory was allocated for the array lines.
        if (allFiles[i].lines != NULL) {
            //if yes, then do a loop to free every line individually.
            for (int j = 0; j < allFiles[i].lineCount; j++) {
                free(allFiles[i].lines[j]);
            }
            //all the individual lines are freed so free the array itself now.
            free(allFiles[i].lines);
        }
        //frees the filename space.
        free(allFiles[i].filename);
    }
    //everything is freed so free the whole struct now.
    free(allFiles);
}