#ifndef TOKENISATION_H
#define TOKENISATION_H
//needed for the FileData struct.
#include "textinput.h" 

//used to contain the unique words and their frequency.
typedef struct {
    char* word;
    int frequency;
    //so basically, later in worddetection.c the code will remove 1 from frequency if there's a phrase that contains that word.
    //like if the code detects "son of a bitch", it will deduct 1 from the frequency of the word since it's part of a phrase.
    //but that'll make output.c save the wrong frequency into the report.csv as here, we don't care about phrases.
    //so we need a backup containing the changed frequency.
    int updatedFrequency;
} UniqueWords;

//used to hold all the word analysis information.
//the names i think are self-explanatory.
typedef struct {
    char* filename;
    long long totalWordCount;
    UniqueWords* uniqueWord;
    int uniqueWordCount;
    long long totalCharCount;
    double avgWordLength;
    int sentenceCount;
    double avgSentenceLength;
    double lexicalDiv;
} AnalysisData;

//make these functions visible to whoever included this .h file.
AnalysisData* wordAnalysis(const FileData* allFilesData, int numFiles);
void printWordAnalysis(const AnalysisData* analysisData, int numFiles);
void freeStage2memory(AnalysisData* analysisData, int numFiles);

#endif