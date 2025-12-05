#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "output.h"
#include "textinput.h"
#include "tokenisation.h"
#include "worddetection.h"
#include "sortingAlgos.h"

//get the filename from the path
char* filenameFromPath(const FileData* allFilesData, int index);

//write literally everything out to the csv
void writeWordAnalysis(const FileData* allFilesData, AnalysisData* analysisData, detectedWordData* toxicResults, detectedWordData* benignResults, int numFiles);

//reload all the word lists.
void reloadList(const char* reportName);
#endif