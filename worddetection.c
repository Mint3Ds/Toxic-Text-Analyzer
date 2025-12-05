#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "worddetection.h"
//for isLineEmpty() function.
#include "textinput.h"

//this function is used to just print an error message for memory issues and then exit program.
static void printExitError() {
    printf("Error! Memory cannot be allocated.\n");
    printf("The program will be terminated to prevent any further issues.");
    //using exit instead of return because exit terminates the whole program instead of just returning to whoever called this function.
    //we're exiting because memory error will probably make the program unreliable and also something is wrong with your OS.
    exit(1);
}

static int getSeverityNum(Category severityType) {
    //returns how severe the word is to use for the toxicity score.
    //if the severity is something other than mild or moderate or severe then return 0, aka skip this word.
    switch (severityType) {
        case MILD:     return 1;
        case MODERATE: return 2;
        case SEVERE:   return 3;
        default:       return 0;
    }
}

static const char* getSeverityWord(Category severityType) {
    //returns what each constant represents.
    //if the severity is something other than mild or moderate or severe then return unknown.
    switch (severityType) {
        case MILD: return "Mild";
        case MODERATE: return "Moderate";
        case SEVERE: return "Severe";
        default: return "Unknown";
    }
}

static Category pickSeverity(const char* severityInput) {
    //lowerUser will store the user's input.
    char lowerUser[50];
    //the pointer is initalised here instead of the for loop since we need it later on to end the string with \0.
    int i = 0;
    //loops as long as there's still characters to check AND we haven't reached the max size of 50 yet.
    for (i = 0; severityInput[i] && i < 49; i++) {
        //make the current character lowercase.
        lowerUser[i] = tolower(severityInput[i]);
    }
    //end the string at the end.
    lowerUser[i] = '\0';
    //check which severity category the user chose and return that value.
    if (strcmp(lowerUser, "mild") == 0) return MILD;
    if (strcmp(lowerUser, "moderate") == 0) return MODERATE;
    if (strcmp(lowerUser, "severe") == 0) return SEVERE;
    //if what the user chose isn't mild or moderate or severe, then return unknown value.
    return UNKNOWN;
}

static char* strToLower(const char* severityInput) {
    //just converts a string to lowercase. to avoid having to copy-paste the code.
    //copy the input into a writable string, since const is read-only.
    char* lowercase = strdup(severityInput);
    //checks if the copying is unsuccessful.
    if (!lowercase) { printExitError(); }
    //loops until the end of the string.
    //lowercase[i] is the \0 character got the integer value of 0.
    //so that would be !lowercase[i].
    for (int i = 0; lowercase[i]; i++) {
        //make the character to the lowercase version.
        lowercase[i] = tolower(lowercase[i]);
    }
    //return the input as all lowercase.
    return lowercase;
}

static WordListTerm* loadWordList(const char* filename, int* currentSize) {
    //open the toxic/benign word list in read-only mode.
    FILE* file = fopen(filename, "r");
    //checks if the file had issues being opened.
    if (!file) { return NULL; }
    //list will contain the list of all the words in the word list.
    WordListTerm* list = NULL;
    //maxSize is the max size of list.
    int maxSize = 10;
    //currentSize is the current number of lines/words in the list.
    *currentSize = 0;
    //allocate memory for list.
    //list currently will be able to store 10 elements of WordListTerm.
    list = malloc(maxSize * sizeof(WordListTerm));
    //check if the memory allocation had error.
    if (!list) { printExitError(); }

    //stores the current line from the file.
    char line[256];
    //loops until end of the file.
    while (fgets(line, sizeof(line), file)) {
        //skips this line if it's empty.
        if (isLineEmpty(line)) { continue; }
        //finds the position in the line that has \n and replace it with a \0 to end the string.
        line[strcspn(line, "\n")] = 0;
        //phrase stores the word/phrase (aka like ngram).
        char* phrase = strtok(line, "|");
        //category stores the severity.
        char* category = strtok(NULL, "|");
        //check if we found a phrase/word and its severity.
        if (phrase && category) {
            //if yes, then check if our array is currently full.
            if (*currentSize >= maxSize) {
                //if yes, then double maxSize.
                maxSize *= 2;
                //then increase the size of the array with it.
                list = realloc(list, maxSize * sizeof(WordListTerm));
                //check if the reallocation was unsuccessful.
                if (!list) { printExitError(); }
            }
            //add the word/phrase and severity into the list.
            list[*currentSize].phrase = strToLower(strdup(phrase));
            list[*currentSize].category = pickSeverity(category);
            //sets a flag to tell whether the phrase has space or not.
            list[*currentSize].gotSpace = (strchr(list[*currentSize].phrase, ' ') != NULL);
            //increae currentSize so we can add the next word to the next element in the next loop.
            (*currentSize)++;
        }
    }
    //close the file and return the list of toxic/benign words.
    fclose(file);
    return list;
}

static void updateWord(detectedWordData* detectedWord, const char* word, Category severity, int newFreq) {
    //detectedWord->uniqueDetectedCount is the same as (*detectedWord).uniqueDetectedCount
    //loops until all the previous detected words are checked.
    for (int i = 0; i < detectedWord->uniqueDetectedCount; i++) {
        //check if the current word is the same as one of the previously detected words.
        if (strcmp(detectedWord->foundWord[i].word, word) == 0) {
            //if yes, then replace the old frequency with the new one.
            detectedWord->foundWord[i].frequency += newFreq;
            //calculate the toxicity score by doing (frequency*severity) and save it.
            detectedWord->foundWord[i].toxicityScore = detectedWord->foundWord[i].frequency * getSeverityNum(detectedWord->foundWord[i].severity);
            //the word was found, so exit the function.
            return;
        }
    }

    //the current word did not match with any of the previously detected words, so it's a new unique word.
    //increase the counter for the number of unique toxic/benign words.
    detectedWord->uniqueDetectedCount++;
    //reallocate foundWord to make space for the one more unique word.
    detectedWord->foundWord = realloc(detectedWord->foundWord, detectedWord->uniqueDetectedCount * sizeof(FoundWord));
    //check if the reallocation was unsuccessful.
    if (!detectedWord->foundWord) { printExitError(); }
    
    //newPos is the index of where the current word will be added.
    //-1 since in arrays, the first index is 0 instead of 1.
    int newPos = detectedWord->uniqueDetectedCount - 1;
    //save the word, its frequency, severity, and toxicity score into the struct. 
    detectedWord->foundWord[newPos].word = strdup(word);
    detectedWord->foundWord[newPos].frequency = newFreq;
    detectedWord->foundWord[newPos].severity = severity;
    //calculated by doing (frequency*severity).
    //the more severe the severity, the higher the score.
    detectedWord->foundWord[newPos].toxicityScore = newFreq * getSeverityNum(severity);
}

detectedWordData* wordDetection(const char* wordlistName, const FileData* allFilesData, AnalysisData* stage2Data, int numFiles) {
    //the number of elements allDetectedData can store is up to numFiles aka the number of files user processed.
    //sizeof(detectedWordData) is the size each element can be.
    //calloc is used instead of malloc since even though calloc is slower, it initalises everything as 0.
    //so we don't need to manually initalise it.
    detectedWordData* allDetectedData = calloc(numFiles, sizeof(detectedWordData));
    //check if the allocation was unsuccessful.
    if (!allDetectedData) { printExitError(); }

    //wordlistCount will contain the number of lines in the toxic/benign word list.
    int wordlistCount = 0;
    //load the toxic/benign word list.
    WordListTerm* wordlist = loadWordList(wordlistName, &wordlistCount);
    //check if any error occurred while loading and reading the word list.
    if (!wordlist) {
        //if yes, print an error and get out of this function.
        printf("\nError! The word list file '%s' could not be loaded.\n", wordlistName);
        printf("Please ensure a file with the same name exists in the same folder as this program and is readable.\n");
        //free the allocated memory since we don't need it as we got an error.
        free(allDetectedData);
        return NULL;
    }

    //loops until all the files user processed are checked.
    for (int i = 0; i < numFiles; i++) {
        //checks if this file had any errors while it was being processed by textinput.h 
        if (!allFilesData[i].loadSuccess) {
            //if yes, print an error and skip this file.
            printf("\nError! File '%s' has previously encountered an error while it was being processed in option 1.\n", allFilesData[i].filename);
            printf("This file will be skipped.\n");
            continue;
        }

        //copy over the path of the current file.
        allDetectedData[i].filename = strdup(allFilesData[i].filename);

        //this for loop is check for phrases aka words with spaces in them like "son of a bitch".
        //it uses the original untokenised lines as we can't use the tokenised ones from tokenisation.c
        //as that had already separated all the words with spaces.
        //loops until the end of the file user uploaded.
        for (int j = 0; j < allFilesData[i].lineCount; j++) {
            //convert the line to lowercase.
            char* lowerLine = strToLower(allFilesData[i].lines[j]);
            //check if the conversion was unsuccessful.
            if (!lowerLine) { printExitError(); }

            //loops until all the lines in the word list are checked.
            for (int k = 0; k < wordlistCount; k++) {
                //check if this toxic/benign word is a phrase aka has a space in the line.
                if (wordlist[k].gotSpace) {
                    //linePos will later contain the position of the toxic/benign phrase.
                    char* linePos = lowerLine;
                    //find the first occurrence of the matching toxic/benign phrase.
                    //loops until a matching toxic/benign phrase is no longer found.
                    while ((linePos = strstr(linePos, wordlist[k].phrase)) != NULL) {
                        //a match is found, so increase the total of detected toxic/benign by 1.
                        //only 1 since this phrase technically only counts as one toxic/benign score.
                        allDetectedData[i].totaldetectedWord++;
                        //check the severity of this phrase and add the corresponding severity frequency by 1.
                        if (wordlist[k].category == MILD) {
                            allDetectedData[i].mildFreq++;
                        } else if (wordlist[k].category == MODERATE) {
                            allDetectedData[i].moderateFreq++;
                        } else if (wordlist[k].category == SEVERE) {
                            allDetectedData[i].severeFreq++;
                        }
                        
                        //save this phrase into the allDetectedData struct.
                        //frequency is set to 1 since this is the first time we found this phrase.
                        updateWord(&allDetectedData[i], wordlist[k].phrase, wordlist[k].category, 1);

                        //we need to remove the frequency of any other detected word inside this phrase.
                        //otherwise the program will detect that word as well.
                        //e.g. the program will also detect the word "bitch" from "son of a bitch" and count it separately.
                        //so create tempPhrase as a copy of the current phrase.
                        //we create a copy because strtok will directly change the value.
                        char* tempPhrase = strdup(wordlist[k].phrase);
                        //check if the duplication was unsuccessful.
                        if (!tempPhrase) { printExitError(); }
                        //split the line by looking for spaces.
                        char* innerWord = strtok(tempPhrase, " ");
                        //loops as long as there's still another split word to check.
                        while (innerWord != NULL) {
                            //loops until all the lines in the word list are checked.
                            for (int l = 0; l < stage2Data[i].uniqueWordCount; l++) {
                                //check if the current word matches with this toxic/benign word.
                                if (strcmp(stage2Data[i].uniqueWord[l].word, innerWord) == 0) {
                                    //if yes, check if the frequency is more than 0.
                                    if (stage2Data[i].uniqueWord[l].updatedFrequency > 0) {
                                        //if yes, then decrease the frequency by 1.
                                        //this is needed since we use this frequency later on during the single word detection.
                                        //to know how many times this word occured.
                                        stage2Data[i].uniqueWord[l].updatedFrequency--;
                                    }
                                    break; // Stop searching for this component word
                                }
                            }
                            //move onto the next word.
                            innerWord = strtok(NULL, " ");
                        }
                        //we reached the end of the line, so free tempPhrase since we don't need it anymore.
                        free(tempPhrase);

                        //move the pointer to position after the found phrase.
                        //so we don't count the same phrase again.
                        linePos += strlen(wordlist[k].phrase);
                    }
                }
            }
            //we reached the end of the untokenised line, so free lowerLine since we don't need it anymore.
            free(lowerLine);
        }

        //this loops checks for the single words (or words with hypens like "son-of-a-bitch").
        //used the list of unique words obtained from tokenisation.c
        //loops until all the unique words of this file are checked.
        for (int j = 0; j < stage2Data[i].uniqueWordCount; j++) {
            //skips if the word frequency is less than or equal to 0.
            //due to the word actually being a part of a phrase. E.g. "son of a bitch".
            if (stage2Data[i].uniqueWord[j].updatedFrequency <= 0) { continue; }
            //loops until all the lines in the word list are checked.
            for (int k = 0; k < wordlistCount; k++) {
                //checks if this toxic/benign word is a single word, aka does not have a space somewhere.
                //AND if the current word matches with this toxic/benign word. 
                if (!wordlist[k].gotSpace && strcmp(stage2Data[i].uniqueWord[j].word, wordlist[k].phrase) == 0) {
                    //if yes, set frequency to the frequency found from the stage2Data[i].uniqueWord[j] struct.
                    //since tokenisation.c already did the counting of the word frequency.
                    int frequency = stage2Data[i].uniqueWord[j].updatedFrequency;
                    //add this frequency to the total frequency of toxic/benign words.
                    allDetectedData[i].totaldetectedWord += frequency;
                    //check the severity of this word and add the corresponding severity frequency by 1.
                    if (wordlist[k].category == MILD) {
                        allDetectedData[i].mildFreq += frequency;
                    } else if (wordlist[k].category == MODERATE) {
                        allDetectedData[i].moderateFreq += frequency;
                    } else if (wordlist[k].category == SEVERE) {
                        allDetectedData[i].severeFreq += frequency;
                    }

                    //save this phrase into the allDetectedData struct.
                    updateWord(&allDetectedData[i], wordlist[k].phrase, wordlist[k].category, frequency);
                }
            }
        }
        
        //checks if the total word count is more than 0.
        if (stage2Data[i].totalWordCount > 0) {
            //if yes, get the percentage of toxic/benign words compared to the overall file.
            //by doing (total detected word count/total word count)*100.
            allDetectedData[i].wordPercent = ((double)allDetectedData[i].totaldetectedWord / stage2Data[i].totalWordCount) * 100.0;
        }

        //currentTotalScore will contain the sum of each word/phrase's toxicity score.
        long long currentTotalScore = 0;
        //loops until all the unique toxic/benign word/phrases are checked.
        for (int j = 0; j < allDetectedData[i].uniqueDetectedCount; j++) {
            //Add the current sum with this word/phrase's toxicity score.
            currentTotalScore += allDetectedData[i].foundWord[j].toxicityScore;
        }
        //save the sum of all the toxicity scores.
        allDetectedData[i].totalToxicityScore = currentTotalScore;

        //checks if the sum of all the toxicity scores is more than 0.
        if (allDetectedData[i].totalToxicityScore > 0) {
            //loops until all the unique toxic/benign word/phrases are checked.
            for (int j = 0; j < allDetectedData[i].uniqueDetectedCount; j++) {
                //get the percentage of how much each word/phrase contributed to the total toxicity score.
                //done by doing (current word's toxicity score/total toxicity score)*100.
                allDetectedData[i].foundWord[j].toxicityScorePercent =  ((double)allDetectedData[i].foundWord[j].toxicityScore / allDetectedData[i].totalToxicityScore) * 100.0;
            }
        }
    }

    //free all the words/phrases from the toxic/benign word list since we don't need them anymore.
    for (int i = 0; i < wordlistCount; i++) { free(wordlist[i].phrase); }
    //and then free the struct itself.
    free(wordlist);

    //return all the obtained data.
    return allDetectedData;
}

void printdetectedWordData(const detectedWordData* wordData, int numFiles, const char* title) {
    //loops until all the files processed have been checked.
    for(int i = 0; i < numFiles; i++){
        //skip the file if the filename doesn't exist in wordData (due to errors).
        if (!wordData[i].filename) { continue; }
        //title is either "Toxic Words" for toxicwords.txt or "Benign Words" for benignwords.txt
        printf("\n%s Detection Stats:", title);

        printf("\n%s detection stats for file '%s': \n", title, wordData[i].filename);
        printf("Total matching words or phrase found: %d\n", wordData[i].totaldetectedWord);
        printf("Percentage of text matching the list: %.2f%%\n", wordData[i].wordPercent);
        printf("Total toxicity or benign score: %d\n", wordData[i].totalToxicityScore);

        printf("\nFrequency of mild words/phrases: %d\n", wordData[i].mildFreq);
        printf("Frequency of moderate words/phrases: %d\n", wordData[i].moderateFreq);
        printf("Frequency of severe words/phrases: %d\n", wordData[i].severeFreq);

        /////////NOTE!!!! remark this part if you don't want to see the entire detected word list here.
        //this is mainly for me to check.
        printf("\nList of all detected words/phrases:\n");
        //check if there's no detected words.
        if (wordData[i].uniqueDetectedCount == 0) {
            printf("None was found!\n");
        } else {
            //if there is at least one detected word.
            //loops until all the unique detected words are printed.
            for (int j = 0; j < wordData[i].uniqueDetectedCount; j++) {
                //print the individual word/phrase details.
                printf("- '%s' (Percentage to the total toxicity score: %.2f%%, Frequency: %d, Severity: %s)\n", 
                    wordData[i].foundWord[j].word, 
                    wordData[i].foundWord[j].toxicityScorePercent, 
                    wordData[i].foundWord[j].frequency,
                    getSeverityWord(wordData[i].foundWord[j].severity));
            }
        }
    }
}

void freeStage3memory(detectedWordData* wordData, int numFiles) {
    //if there's no data to free then just get out of this function.
    if (!wordData) { return; }

    //loops until all the files loaded are freed.
    for (int i = 0; i < numFiles; i++) {
        //free the filename.
        free(wordData[i].filename);
        //loops until every unique detected words are freed.
        for (int j = 0; j < wordData[i].uniqueDetectedCount; j++) {
            free(wordData[i].foundWord[j].word);
        }
        //all the words inside the array foundWord is freed, so free the array now.
        free(wordData[i].foundWord);
    }
    //free the struct.
    free(wordData);
}

void updateWordList(const char* wordlistName) {
    //userPhrase will contain the phrase/word to save.
    //userSeverity will contain the severity of the word.
    char userPhrase[256], userSeverity[50];
    //set the current severity to unknown first, in case an invalid severity is typed.
    Category category = UNKNOWN;

    printf("Enter the new word or phrase: ");
    //get the new word/phrase to save.
    //using fgets in case the user enters a phrase with space.
    fgets(userPhrase, sizeof(userPhrase), stdin);
    //replace the \n with \0 since fgets doesn't do that automatically.
    userPhrase[strcspn(userPhrase, "\n")] = 0;
    //loops as long as the severity inputted is not mild or moderate or severe.
    while (category == UNKNOWN) {
        printf("Enter the severity (mild, moderate, severe): ");
        //get the severity to save.
        fgets(userSeverity, sizeof(userSeverity), stdin);
        userSeverity[strcspn(userSeverity, "\n")] = 0;
        //choose a constant from the enum based on what the user inputted.
        category = pickSeverity(userSeverity);
        //check if the severity is valid.
        if (category == UNKNOWN) { printf("Error! The severity can only either be mild/moderate/severe.\nPlease try again.\n"); }
    }

    //open the toxicwords.txt or benignwords.txt in append mode.
    FILE* file = fopen(wordlistName, "a");
    //check if the file is unsuccessfully opened.
    if (!file) { 
        //if yes, print an error and exit this function.
        printf("Error! The file %s could not be opened for appending.\n This function will be exited.", wordlistName); 
        return; 
    }
    //write to the file in the format of "[word/phrase]|[severity]".
    fprintf(file, "\n%s|%s", userPhrase, userSeverity);
    //close the file.
    fclose(file);
    printf("Success! The word list '%s' has been updated.\n", wordlistName);
}