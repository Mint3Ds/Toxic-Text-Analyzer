#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "output.h"
#include "tokenisation.h"
#include "worddetection.h"
#include "sortingAlgos.h"

void reloadList(const char* reportName) {
    //open the report file in read-only mode.
    FILE* reportFile = fopen(reportName, "r");
    //give error if the file can't be opened.
    if (!reportFile) {
        printf("\nError! The report file '%s' could not be opened.\n", reportName);
        printf("Please make sure the file exists in the location. This function will be exited.\n");
        return;
    }

    //open the three word lists in write mode.
    FILE* stopFile = fopen("stopwords.txt", "w");
    FILE* toxicFile = fopen("toxicwords.txt", "w");
    FILE* benignFile = fopen("benignwords.txt", "w");
    //print error if any of them couldn't be opened.
    if (!stopFile || !toxicFile || !benignFile) {
        printf("Error! Could not open one or more word list files for writing.\n");
        printf("This function will be exited.\n");
        //close whichever files were successfully opened.
        if (stopFile) fclose(stopFile);
        if (toxicFile) fclose(toxicFile);
        if (benignFile) fclose(benignFile);
        fclose(reportFile);
        //exit the function.
        return;
    }

    //will contain a row from the report csv file.
    char csvLine[2500];
    //keeps track of the current row number.
    int rowNum = 0;

    printf("Reading from '%s' and reloading dictionaries...\n", reportName);

    while (fgets(csvLine, sizeof(csvLine), reportFile)) {
        rowNum++;
        //skip the header row.
        if (rowNum == 1) { continue; }
        //replace \r\n with \0.
        csvLine[strcspn(csvLine, "\r\n")] = 0;

        //keeps track of the current column number.
        int colNum = 0;
        //we'll change tempLine so csvLine won't be affected.
        char* tempLine = csvLine;

        //loop to manually get each column.
        //apparently strtok ignores empty cells, so we can't use that because we have said empty cells.
        while (1) {
            //get the position of the next comma.
            //as .csv file use comma to separate columns.
            char* nextComma = strchr(tempLine, ',');
            //if the comma is found, then replace the comma with \0 to end the string.
            if (nextComma) { *nextComma = '\0'; }

            //checks if the column we're in is either stopwords/toxicwords/benignwords.txt
            //and also check if the cell is not empty.
            //if yes, then write to the corresponding file.
            if (colNum == 14 && strlen(tempLine) > 0) {
                //14 is column O.
                fprintf(stopFile, "%s\n", tempLine);
            } else if (colNum == 16 && strlen(tempLine) > 0) {
                //16 is column Q.
                fprintf(toxicFile, "%s\n", tempLine);
            } else if (colNum == 18 && strlen(tempLine) > 0) {
                //18 is column S.
                fprintf(benignFile, "%s\n", tempLine);
            }

            //checks if a comma is found.
            if (nextComma) {
                //if yes then move the pointer of tempLine to the character after the comma (or technically now \0).
                //so next loop will look at a new cell.
                tempLine = nextComma + 1;
                colNum++;
            } else {
                //if no more commas are found then exit this loop as it's the end of this row.
                break;
            }
        }
    }

    //close all the files.
    fclose(reportFile);
    fclose(stopFile);
    fclose(toxicFile);
    fclose(benignFile);
    //print success message
    printf("\nSuccessfully reloaded all the dictionaries/word lists from '%s'!\n", reportName);
    printf("stopwords.txt, toxicwords.txt, and benignwords.txt have been overwritten.\n");
}

static int findMax(int a, int b) {
    //returns a if a is bigger, else returns b.
    if (a > b) { 
        return a; 
    } else { 
        return b; 
    }
}

static void printExitError() {
    printf("Error! Memory cannot be allocated.\n");
    printf("The program will be terminated to prevent any further issues.");
    //using exit instead of return because exit terminates the whole program instead of just returning to whoever called this function.
    //we're exiting because memory error will probably make the program unreliable and also something is wrong with your OS.
    exit(1);
}

static char** loadList(const char* filename, int* size) {
    *size = 0;
    FILE* file = fopen(filename, "r");
    //checks if the stopword/toxic/benign word list was unsuccessfully opened.
    if (!file) {
        //if yes, print an error and set size to 0 since we couldn't open the file to get the size.
        printf("Error! The list file '%s' could not be opened.\n", filename);
        *size = 0;
        return NULL;
    }

    int maxSize = 10;
    //allocate memory for list.
    char** list = malloc(maxSize * sizeof(char*));
    //check if allocation was unsuccessful.
    if (!list) { printExitError(); }

    char line[256];
    //loops until the end of the list.
    while (fgets(line, sizeof(line), file)) {
        //checks if the list array don't have anymore space.
        if (*size >= maxSize) {
            //if yes then double maxSize and reallocate the list size.
            maxSize *= 2;
            list = realloc(list, maxSize * sizeof(char*));
            //check if the reallocation was unsuccessful.
            if (!list) { printExitError(); }
        }
        //replace the \n with \0 to end string.
        line[strcspn(line, "\n")] = 0;
        //save the line into list.
        list[*size] = strdup(line);
        (*size)++;
    }
    fclose(file);
    return list;
}

static void freeListmemory(char** list, int size) {
    //exit if the list is empty.
    if (!list) { return; }
    //free all the lines inside the array.
    for (int i = 0; i < size; i++) {
        free(list[i]);
    }
    //free the array itself now.
    free(list);
}

//copied from static void saveFilteredWords(const AnalysisData* data) in splitColisation.c
char* filenameFromPath(const FileData* allFilesData, int index) {
    //checks if there's no file successfully loaded.
    //if yes, exit this function.
    if (!allFilesData[index].filename) { return NULL; }
    //technically you can also just use data->filename instead of putting it into a variable.
    //but like it's confusing to read, even i also get confused reading it.
    //also because this is slightly safer since it's a const and we can't change the original value by accident.
    const char* originalFilename = allFilesData[index].filename;

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

    //trimmedName will contain just the name of the file without the extension.
    char* trimmedName = malloc(300);
    if (!trimmedName) return NULL;
    
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
    return strcat(trimmedName, "AnalysisReport.csv");
}

void writeWordAnalysis(const FileData* allFilesData, AnalysisData* analysisData, detectedWordData* toxicResults, detectedWordData* benignResults, int numFiles) {
    //check if there's no analysed data we can print.
    if (!analysisData || !toxicResults || !benignResults) {
        printf("Error! There are missing analysis data!\n");
        return;
    }

    //loop through all the inputted files.
    for (int index = 0; index < numFiles; index++) {
        //skip the file if the filename or unique word doesn't exist (due to errors).
        if (!allFilesData[index].filename || !analysisData[index].uniqueWord) {
            printf("\nError! The file '%s' does not exist or is empty.\nThis file will be skipped.\n\n", allFilesData[index].filename);
            continue;
        }

        //creates a new string with the fileName and "Analysisreport.txt" appeneded to the end of it
        char* summaryFileName = filenameFromPath(allFilesData, index);
        //skips the file if there was issues in creating.
        if (!summaryFileName) { continue; }
        //open the file in write mode.
        FILE* file = fopen(summaryFileName, "w");
        if (!file) { 
            //file cannot be opened, return some errors and close it
            printf("There was an error accessing filename \"%s\". No summary was generated.\n", analysisData[index].filename );
            free(summaryFileName);
            fclose(file);
            return; 
        }

        //flag to see if the file gets any error. 1 for success, 0 for failure.
        int success = 1;
        //contains the number of lines in stopword/toxic/benign word list respectively.
        int stopwordCount = 0, toxicListCount = 0, benignListCount = 0;
        //loads the words from stopword.txt, toxicwords.txt, benignwords.txt
        //if anyone got an error, then set success to 0.
        char** stopwords = loadList("stopwords.txt", &stopwordCount);
        if (!stopwords) { success = 0; }
        char** toxicList = loadList("toxicwords.txt", &toxicListCount);
        if (!toxicList) { success = 0; }
        char** benignList = loadList("benignwords.txt", &benignListCount);
        if (!benignList) { success = 0; }

        //checks if there's no errors.
        if (success) {
            //if yes then get the size of all unique, unique toxic, and unique benign word lists.
            int freqCount = analysisData[index].uniqueWordCount;
            int toxicCount = toxicResults[index].uniqueDetectedCount;
            int benignCount = benignResults[index].uniqueDetectedCount;

            //sort the lists by ascending frequency.
            uniqueWordsMergeSort(analysisData[index].uniqueWord, 0, freqCount - 1);
            uniqueWordsReverseArray(analysisData[index].uniqueWord, freqCount);
            frequencyMergeSort(toxicResults[index].foundWord, 0, toxicCount - 1);
            //sort for descending.
            reverseArray(toxicResults[index].foundWord, toxicCount);
            frequencyMergeSort(benignResults[index].foundWord, 0, benignCount - 1);
            reverseArray(benignResults[index].foundWord, benignCount);

            //ok so because fprintf writes one row each time, it's kinda hard to separate into columns.
            //since even if one list has less lines than another, we still need to continue writing rows.
            //so we need to find which list has the most lines to know when to stop writing.
            int maxRows = 0;
            //statsCount is the number of stats we got from analysisData.
            int statsCount = 10;
            //compare each word list to find the longest list.
            maxRows = findMax(statsCount, freqCount);
            maxRows = findMax(maxRows, toxicCount);
            maxRows = findMax(maxRows, benignCount);
            maxRows = findMax(maxRows, stopwordCount);
            maxRows = findMax(maxRows, toxicListCount);
            maxRows = findMax(maxRows, benignListCount);

            //write the headers.
            //in .csv, the columns are separated with a comma.
            fprintf(file, "File Stat,Value,,Unique Word,Frequency,,Toxic Word,Frequency,Severity,,Benign Word,Frequency,Severity,,stopwords.txt,,toxicwords.txt,,benignwords.txt\n");

            //checks if there's still any line needing to be written.
            for (int i = 0; i < maxRows; i++) {
                //writes the all the stats from analysisData based on which row it is currently.
                //if all the stats are already written then write an empty tempLine.
                switch(i) {
                    case 0:
                        fprintf(file, "Analysis data for file:,%s", analysisData[index].filename);
                        break;
                    case 1:
                        fprintf(file, "Total word count (including stopwords):,%lld", analysisData[index].totalWordCount);
                        break;
                    case 2:
                        fprintf(file, "Unique word count (excluding stopwords):,%d", analysisData[index].uniqueWordCount);
                        break;
                    case 3:
                        fprintf(file, "Character count (excluding stopwords and special characters):,%lld",analysisData[index].totalCharCount);
                        break;
                    case 4:
                        fprintf(file, "Average word length:,%.2f", analysisData[index].avgWordLength);
                        break;
                    case 5:
                        fprintf(file, "Lexical diversity:,%.3f", analysisData[index].lexicalDiv);
                        break;
                    case 6:
                        fprintf(file, "Sentence count:,%d", analysisData[index].sentenceCount);
                        break;
                    case 7:
                        fprintf(file, "Average sentence length:,%.2f", analysisData[index].avgSentenceLength);
                        break;
                    case 8:
                        fprintf(file, "Total toxicity score:,%d", toxicResults[index].totalToxicityScore);
                        break;
                    case 9:
                        fprintf(file, "Total benign score:,%d", benignResults[index].totalToxicityScore);
                        break;
                    default:
                        //all the stats are written so just write an empty tempLine into this column.
                        fprintf(file, ",");
                        break;
                }

                //write an empty tempLine for the column after the word analysis stats to leave an empty column.
                //so it's easier to differentiate between categories.
                fprintf(file, ",,");

                //check if there's still unique words to write.
                if (i < freqCount) {
                    //if yes then write the unique word and its frequency.
                    fprintf(file, "%s,%d", analysisData[index].uniqueWord[i].word, analysisData[index].uniqueWord[i].frequency);
                } else {
                    //else write an empty tempLine.
                    fprintf(file, ",");
                }
                //leave an empty column again so it's easier to differentiate.
                fprintf(file, ",,");

                //check if there's still unique toxic words to write.
                if (i < toxicCount) {
                    //if yes then get the severity of the word.
                    const char* severity = "unknown";
                    switch(toxicResults[index].foundWord[i].severity) {
                        case MILD: 
                            severity = "mild"; 
                            break;
                        case MODERATE: 
                            severity = "moderate"; 
                            break;
                        case SEVERE: 
                            severity = "severe"; 
                            break;
                    }
                    //and then write to the file.
                    fprintf(file, "%s,%d,%s", toxicResults[index].foundWord[i].word, toxicResults[index].foundWord[i].frequency, severity);
                } else {
                    //else write an empty tempLine.
                    fprintf(file, ",,");
                }
                fprintf(file, ",,");

                //check if there's still unique benign words to write.
                if (i < benignCount) {
                    //if yes then get the severity of the word.
                    const char* severity = "unknown";
                    switch(benignResults[index].foundWord[i].severity) {
                        case MILD: 
                            severity = "mild"; 
                            break;
                        case MODERATE: 
                            severity = "moderate"; 
                            break;
                        case SEVERE: 
                            severity = "severe"; 
                            break;
                    }
                    //and then write to the file.
                    fprintf(file, "%s,%d,%s", benignResults[index].foundWord[i].word, benignResults[index].foundWord[i].frequency, severity);
                } else {
                    //else write an empty tempLine.
                    fprintf(file, ",,");
                }
                fprintf(file, ",,");
                
                //write a line from stopwords.txt if there's still any line that's not written yet.
                if (i < stopwordCount) { fprintf(file, "%s", stopwords[i]); }
                fprintf(file, ",,"); 

                //write a line from toxicwords.txt if there's still any line that's not written yet.
                if (i < toxicListCount) { fprintf(file, "%s", toxicList[i]); }
                fprintf(file, ",,");

                //write a line from benignwords.txt if there's still any line that's not written yet.
                if (i < benignListCount) { fprintf(file, "%s", benignList[i]); }

                //end the row.
                fprintf(file, "\n"); 
            }
        }
        //close the file.
        fclose(file);

        //checks if success is true.
        if (success) {
            //if yes then print success message.
            printf("Successfully finished writing to %s!\n", summaryFileName);
        } else {
            //if not then tell the user about the error and delete the file since it's incomplete.
            printf("Due to the errors, the report '%s' has been deleted.\n", summaryFileName);
            remove(summaryFileName);
        }
        
        //free all the memories allocated.
        free(summaryFileName);
        freeListmemory(stopwords, stopwordCount);
        freeListmemory(toxicList, toxicListCount);
        freeListmemory(benignList, benignListCount);
    }
}

