#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenisation.h"

//this function is used to just print an error message for memory issues and then exit program.
static void printExitError() {
    printf("Error! Memory cannot be allocated.\n");
    printf("The program will be terminated to prevent any further issues.");
    //using exit instead of return because exit terminates the whole program instead of just returning to whoever called this function.
    //we're exiting because memory error will probably make the program unreliable and also something is wrong with your OS.
    exit(1);
}

static char** loadStopwords(int* currentSize) {
    //open stopwords.txt in read-only mode.
    FILE* file = fopen("stopwords.txt", "r");
    //checks if the stopwords.txt is unsuccessfully opened. if yes, exit this function.
    //the NULL will be used to tell the program we got an error.
    if (!file) { return NULL; }
    //list will contain the stopwords.
    char** list = NULL;
    //stopword will contain a single line of maximum 100 characters.
    char stopword[100];
    //maxSize is the max size of list.
    int maxSize = 10;
    //set currentSize to 0. 
    //the * is needed so the program knows to change the original variable, aka the one being passed into currentSize.
    *currentSize = 0;
    //allocate memory for list.
    //list currently will be able to store 10 (taken from maxSize) char pointers (done by sizeof(char*)).
    list = malloc(maxSize * sizeof(char*));
    //checks if the allocation was unsuccessful.
    if (!list) { printExitError(); }

    //loops until the end of the file.
    while (fgets(stopword, sizeof(stopword), file)) {
        //finds the position in stopword that has \n and replace it with a \0 to end the string.
        stopword[strcspn(stopword, "\n")] = 0;
        //check if the current stopword line is not empty.
        if (strlen(stopword) > 0) {
            //if yes, then check if list aka the array is currently full.
            if (*currentSize >= maxSize) {
                //if yes, double maxSize.
                maxSize *= 2;
                //then increase the size of the array with it.
                list = realloc(list, maxSize * sizeof(char*));
                //checks if the reallocation was unsuccessful.
                if (!list) { printExitError(); }
            }
            //add the stopword to the list.
            list[*currentSize] = strdup(stopword);
            //increae currentSize so we can add the next stopword to the next element in the next loop.
            (*currentSize)++;
        }
    }
    //close the file and return the list of stopwords.
    fclose(file);
    return list;
}

static int isStopword(const char* word, char** stopwords, int stopwordCount) {
    //check if stopwords is NULL, aka if the program ran into an error while loading stopwords.txt.
    //if yes then exit this function.
    if (!stopwords) { return 0; }
    //loops until the end of the stopword list.
    for (int i = 0; i < stopwordCount; i++) {
        //checks if the word is a stopword. if yes then return 1.
        //the return 1 here is to show that the word being checked is a stopword.
        if (strcmp(word, stopwords[i]) == 0) return 1;
    }
    //return 0 here is to show the word being checked is NOT a stopword.
    return 0;
}

static void saveFilteredWords(const AnalysisData* data) {
    //the -> is the equivalent of (*data).uniqueWordCount
    //checks if there's no unique words or there's no file successfully loaded.
    //if yes, exit this function.
    if (data->uniqueWordCount == 0 || !data->filename) { return; }
    //trimmedName will contain just the name of the file without the extension.
    char trimmedName[300];
    //technically you can also just use data->filename instead of putting it into a variable.
    //but like it's confusing to read, even i also get confused reading it.
    //also because this is slightly safer since it's a const and we can't change the original value by accident.
    const char* originalFilename = data->filename;

    //macSlash finds the last position of the / character while winSlash finds the \ character.
    //since currently, originalFilename could contain a filepath like "folder1\folder2\word.txt".
    //so we need to trim out the path.
    const char* macSlash = strrchr(originalFilename, '/');
    const char* winSlash = strrchr(originalFilename, '\\');
    //check if a slash exist somewhere in the filename. 
    //if yes, then set the originalFilename pointer to the character after the slash.
    if (macSlash) { originalFilename = macSlash + 1; }
    //the "xSlash > originalFilename" part checks if the \ is found after the /.
    //so like if the filepath is "folder1\folder2/word.txt", we don't set the pointer to "folder2/word.txt" instead.
    if (winSlash && winSlash > originalFilename) { originalFilename = winSlash + 1; }

    //find which position in the filename has the last period (.) and put it into extension.
    char* extension = strrchr(originalFilename, '.');
    //checks if an extension is found.
    if (extension) {
        //if yes, then trim out the extension with "extension - originalFilename".
        //basically by not including the character space used for the extension.
        //and then put the result into trimmedName.
        strncpy(trimmedName, originalFilename, extension - originalFilename);
        //add the \0 at the end to end the string.
        //because otherwise we'll get some funny errors.
        trimmedName[extension - originalFilename] = '\0';
    } else {
        //else trimmedName is the same as originalFilename.
        strcpy(trimmedName, originalFilename);
    }

    //add the text "_filtered.txt" to trimmedName.
    strcat(trimmedName, "_filtered.txt");
    //open the file with the same name as trimmedName's under write mode.
    //the file will be created if it doesn't exist, or overwritten if it exists.
    FILE* outFile = fopen(trimmedName, "w");
    //checks if the file is unsuccessfully opened. prints an error if yes.
    if (!outFile) { printf("Error! The file %s could not be opened for writing.\nThis file will be skipped.\n", trimmedName); return; }
    //loops until all the unique words are checked.
    for (int i = 0; i < data->uniqueWordCount; i++) {
        //write the filtered word to the output file.
        fprintf(outFile, "%s\n", data->uniqueWord[i].word);
    }
    //close the file to prevent memory leak
    fclose(outFile);
    printf("Success! The filtered word list is saved to the file '%s'\n", trimmedName);
}

AnalysisData* wordAnalysis(const FileData* allFilesData, int numFiles) {
    //the number of elements allAnalysisData can store is up to NumFiles aka the number of files user processed.
    //sizeof(AnalysisData) is the size each element can be.
    //calloc is used instead of malloc since even though calloc is slower, it initalises everything as 0.
    //so we don't need to manually initalise it.
    AnalysisData* allAnalysisData = calloc(numFiles, sizeof(AnalysisData));
    //checks if there was any memory issue. if yes, exit the program.
    if (!allAnalysisData) { printExitError(); }

    //stopwordCount will be used to keep track of how many stopwords there are.
    int stopwordCount = 0;
    //get the list of stopwords.
    //stopwordCount is passed so it'll be updated with the count after the function is done.
    char** stopwords = loadStopwords(&stopwordCount);
    //check if an error was encountered inside the function.
    if (!stopwords) {
        //if yes, print a warning.
        printf("\nWarning! stopwords.txt cannot be found or it is empty.\n");
        printf("The file will still be analysed, but no words will be filtered!\n");
    }

    //loops until all the processed files are analysed.
    for (int i = 0; i < numFiles; i++) {
        //checks if this file had any errors while it was being processed by textinput.h 
        if (!allFilesData[i].loadSuccess) {
            //if yes, print an error and skip this file.
            printf("\nError! File '%s' has previously encountered an error while it was being processed in option 1.\n", allFilesData[i].filename);
            printf("This file will be skipped.\n");
            continue;
        }

        //copy over the filename from allFilesData to allAnalysisData struct.
        allAnalysisData[i].filename = strdup(allFilesData[i].filename);

        //tell the user that the program did not freeze.
        printf("\nAnalysing file '%s'.\n", allFilesData[i].filename);
        printf("Depending on the file size, this can take up to a minute. Please be patient!\n");

        //loops until the end of the current file.
        for (int j = 0; j < allFilesData[i].lineCount; j++) {
            //transfer over the line content. mostly to just make it more readable.
            char* line = allFilesData[i].lines[j];
            //create a temporary copy of line. 
            //this is for later when strtok is used, since that'll change the content.
            char* tempLine = strdup(line);
            //checks if the line is unsuccessfully copied. if yes, print error and terminate the program.
            if (!tempLine) { printExitError(); }
            
            //loops until the end of the line/string.
            for (int k = 0; line[k] != '\0'; k++) {
                //checks if the current character is either a . or ? or !
                //basically checks if it means the current character is the end of a sentence.
                if (line[k] == '.' || line[k] == '?' || line[k] == '!') {
                    //if yes, add one to the sentence count.
                    allAnalysisData[i].sentenceCount++;
                }
            }

            //split the line by looking for space or tabs or newlines or carriage returns.
            char* splitLine = strtok(tempLine, " \t\n\r");
            //loops as long as there's still a part of the line still needing to be checked.
            while (splitLine != NULL) {
                //cleanedWord is the word with only numbers or letters or asterisk or hyphen.
                char cleanedWord[256] = "";
                //cleanedWordSize is the size of the cleaned word.
                int cleanedWordSize = 0;
                //loops as long as it's not the end of the split string
                //AND also that we haven't reached the max capacity of cleanedWordSize yet.
                for (int k = 0; splitLine[k] != '\0' && cleanedWordSize < 255; k++) {
                    //character contains the current character we're checking.
                    char character = splitLine[k];
                    //check if the character is either a letter or number or hyphen or asterisk.
                    if (isalnum(character) || character == '-' || character == '*') {
                        //if yes, make the character lowercase and then place it into cleanedWord.
                        //and then increment cleanedWordSize by one to store the next character in the next loop.
                        cleanedWord[cleanedWordSize++] = tolower(character);
                    }
                }
                //finished checking the string, so end the string.
                cleanedWord[cleanedWordSize] = '\0';

                //check if we found any allowed character.
                if (strlen(cleanedWord) > 0) {
                    //if yes, add one to the total word count.
                    allAnalysisData[i].totalWordCount++;
                    //check if the word is not a stopword.
                    if (!isStopword(cleanedWord, stopwords, stopwordCount)) {
                        //if yes, then add the number of characters in the word into the total character count.
                        allAnalysisData[i].totalCharCount += strlen(cleanedWord);
                        //a flag to see if the word is a unique word.
                        int found = 0;
                        //loops until all the previous found unique words are checked.
                        for (int k = 0; k < allAnalysisData[i].uniqueWordCount; k++) {
                            //check if the current word is the same as an existing unique word. 
                            if (strcmp(allAnalysisData[i].uniqueWord[k].word, cleanedWord) == 0) {
                                //if yes then add one to the frequency of that word.
                                allAnalysisData[i].uniqueWord[k].frequency++;
                                //update updatedFrequency as well.
                                //updatedFrequency will be the frequency that'll be changed in worddetection.c 
                                allAnalysisData[i].uniqueWord[k].updatedFrequency = allAnalysisData[i].uniqueWord[k].frequency;
                                //set the found flag to 1 to show a matching word was found.
                                found = 1;
                                //get out of this for loop since we already found a match.
                                break;
                            }
                        }
                        //checks if the current word was not found in the unique word list.
                        if (!found) {
                            //if yes, then add one to the unique word count.
                            allAnalysisData[i].uniqueWordCount++;
                            //reallocate to make space for the one more unique word.
                            allAnalysisData[i].uniqueWord = realloc(allAnalysisData[i].uniqueWord, allAnalysisData[i].uniqueWordCount * sizeof(UniqueWords));
                            //add the word to the unique word list.
                            //the -1 is since the uniqueWordCount starts at 1 minimum, but the first element index is 0.
                            allAnalysisData[i].uniqueWord[allAnalysisData[i].uniqueWordCount - 1].word = strdup(cleanedWord);
                            //set the frequency to 1 since this is the first time we found this word.
                            allAnalysisData[i].uniqueWord[allAnalysisData[i].uniqueWordCount - 1].frequency = 1;
                            allAnalysisData[i].uniqueWord[allAnalysisData[i].uniqueWordCount - 1].updatedFrequency = 1;
                        }
                    }
                }
                //get the next split string.
                splitLine = strtok(NULL, " \t\n\r");
            }
            //we reached the end of this line, so free tempLine since we don't need the data inside anymore.
            free(tempLine);
        }

        //checks if the sentence count is 0 AND the total word count is more than 0.
        if (allAnalysisData[i].sentenceCount == 0 && allAnalysisData[i].totalWordCount > 0) {
            //if yes, this means there exists a sentence.
            //so set this line's sentenceCount to 1.
            allAnalysisData[i].sentenceCount = 1;
        }

        //checks if the total word count is more than 0.
        if (allAnalysisData[i].totalWordCount > 0) {
            //if yes, then get the lexical diversity by doing (unique word count/total word count)*100.
            allAnalysisData[i].lexicalDiv = ((double)allAnalysisData[i].uniqueWordCount / allAnalysisData[i].totalWordCount)*100.0;
        }
        //checks if the unique word count is more than 0.
        //if yes, get the average word length by doing (total character count/sentence count).
        if (allAnalysisData[i].uniqueWordCount > 0) {
            allAnalysisData[i].avgWordLength = (double)allAnalysisData[i].totalCharCount / allAnalysisData[i].uniqueWordCount;
        }
        //checks if the sentence count is more than 0.
        //if yes, get the average sentence length by doing (total word count/sentence count).
        if (allAnalysisData[i].sentenceCount > 0) {
            allAnalysisData[i].avgSentenceLength = (double)allAnalysisData[i].totalWordCount / allAnalysisData[i].sentenceCount;
        //else, check if the total word count is more than 0.
        } else if (allAnalysisData[i].totalWordCount > 0) {
            //if yes, then we're assuming there's at least one sentence since there's at least one word.
            allAnalysisData[i].sentenceCount = 1;
            allAnalysisData[i].avgSentenceLength = (double)allAnalysisData[i].totalWordCount;
        }
        //save the filtered words into a .txt file.
        saveFilteredWords(&allAnalysisData[i]);
    }

    //check if there's anything inside stopwords.
    if (stopwords) {
        //if yes, then free every word inside the array.
        for (int i = 0; i < stopwordCount; i++) { 
            free(stopwords[i]); 
        }
        //and then free the array.
        free(stopwords);
    }
    //return all the word analysis.
    return allAnalysisData;
}

void printWordAnalysis(const AnalysisData* analysisData, int numFiles) {
    //check if there's no analysed data we can print.
    if (!analysisData) { printf("There's no analysis data available.\n"); return; }
    //loops until all the files processed have been checked.
    for (int i = 0; i < numFiles; i++) {
        //skip the file if the filename doesn't exist in analysisData (due to errors).
        if (!analysisData[i].filename) { continue; }
        //print the analysis for the file.
        printf("\nWord analysis stats for file '%s': \n", analysisData[i].filename);
        printf("Total word count (including stopwords): %lld\n", analysisData[i].totalWordCount);
        printf("Unique word count (excluding stopwords): %d\n", analysisData[i].uniqueWordCount);
        printf("Character count (excluding stopwords and special characters): %lld\n", analysisData[i].totalCharCount);
        printf("Average word length (excluding stopwords and special characters): %.2f characters\n", analysisData[i].avgWordLength);
        printf("Lexical diversity: %.3f (unique/total count)\n", analysisData[i].lexicalDiv);
        printf("Sentence count (found by the punctuations .?!): %d\n", analysisData[i].sentenceCount);
        printf("Average sentence length (including stopwords): %.2f words\n", analysisData[i].avgSentenceLength);
    }
}

void freeStage2memory(AnalysisData* analysisData, int numFiles) {
    //if there's no data to free then just get out of this function.
    if (!analysisData) { return;}

    //loops until all the files loaded are freed.
    for (int i = 0; i < numFiles; i++) {
        //free the filename.
        free(analysisData[i].filename);
        //loops until all the unique words are freed.
        for (int j = 0; j < analysisData[i].uniqueWordCount; j++) {
            //free the memory allocated for this unique word.
            free(analysisData[i].uniqueWord[j].word);
        }
        //all the words inside the array uniqueWord is freed, so free the array now.
        free(analysisData[i].uniqueWord);
    }
    //free the struct.
    free(analysisData);
}