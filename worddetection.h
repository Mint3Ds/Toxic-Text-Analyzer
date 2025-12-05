#ifndef WORDDETECTION_H
#define WORDDETECTION_H
//for FileData struct.
#include "textinput.h"
//for analysisData struct.
#include "tokenisation.h"

//to calculate the toxicity score, mild represents 1, moderate is 2, severe is 3 and unknown is 0.
//used for the toxicity score and also to keep track of each word's severity.
typedef enum { MILD, MODERATE, SEVERE, UNKNOWN } Category;

//stores words/phrases from either toxicwords.txt or benignwords.txt.
//yes i couldn't think of a better name.
typedef struct {
    //the word or phrase.
    char* phrase;
    //the severity of the word or phrase.
    Category category;
    //flag to see if this word or space has spaces.
    //1 for yes, 0 for no.
    int gotSpace;
} WordListTerm;

//stores data about the found/detected words from the files user wanted to process.
typedef struct {
    char* word;
    int frequency;
    Category severity;
    //going by the assumption that the toxicity score is how toxic the word is.
    //will be calculated as (severity * frequency)
    int toxicityScore;
    //the percentage of how much the word contributes to the total toxicity, compared to other words.
    double toxicityScorePercent;
} FoundWord;

//stores the word detection results.
typedef struct {
    char* filename;
    //total number of words that's toxic/benign.
    int totaldetectedWord;
    //pecentage of word that's toxic/benign compared to the whole text.
    double wordPercent;
    //the sum of all the words' toxicity/benign score.
    //not sure what neutral term to use instead of toxicity.
    int totalToxicityScore;
    
    //frequency of each severity.
    int mildFreq;
    int moderateFreq;
    int severeFreq;

    //information about the detected word.
    FoundWord* foundWord;
    //total number of unique toxic/benign words.
    int uniqueDetectedCount;

} detectedWordData;

//wordlistName is either toxicwords.txt or benignwords.txt
//allFilesData are the files user uploaded from textinput.h aka stage 1.
//stage2Data is the tokenised data user got from tokenisation.h aka stage 2.
//numFiles is the number of files user uploaded.
detectedWordData* wordDetection(const char* wordlistName, const FileData* allFilesData, AnalysisData* stage2Data, int numFiles);
void printdetectedWordData(const detectedWordData* wordData, int numFiles, const char* title); 
void freeStage3memory(detectedWordData* wordData, int numFiles);
void updateWordList(const char* wordlistName);

#endif