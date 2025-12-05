//the infndef is to prevent double declaration.
//basically it checks if TEXTINPUT_H is already defined.
#ifndef TEXTINPUT_H
//if TEXTINPUT_H isn't defined, then define it along with functions and stuff inside it.
#define TEXTINPUT_H
#include <stdio.h>

//some functions and struct are declared here so whatever other code that includes this .h file will be able to see these functions and call them.

//contains all the uploaded files and the lines inside.
typedef struct {
    //create a string for the name of file. needed since we're going to upload multiple files.
    char* filename;
    //create a string array. each element is one line from the file.
    char** lines;
    //the total number of lines in the array.
    //needed to loop through lines.
    int lineCount;
    //flag to check if the file is loaded without errors.
    //is used throughout textinput.c, tokenisation.c, wordanalysis.c to see if any file has issues.
    //1 is true, 0 is false.
    int loadSuccess;
} FileData;

//used to load the files and then return whatever result is gotten into the struct FileData.
//filePaths is the path to the files, fileNum is the number of files user specified.
FileData* loadFiles(const char* filePaths[], int fileNum);
//used to check if the line is empty.
//specifically for .csv files since empty .csv files are not 0kb.
int isLineEmpty(const char *line);
//used to count the total words since apparently that goes under stage 1 in the checklist.
//numFiles is the number of files successfully loaded, not the number user specified.
long long printTotalWord(const FileData* allFiles, int numFiles);

//used to free the memory allocated for FileData.
//to avoid memory leak or unneeded allocation.
void freeMemory(FileData* allFiles, int numFiles);

#endif