To compile using minGW in the command prompt, open the command prompt in the same folder as all the source files and enter “gcc main.c textinput.c tokenisation.c worddetection.c sortingAlgos.c output.c -o main -std=c99”. 

You can then run the program with “.\main.exe” or just clicking on the EXE file in the file browser.
	
The required input files are stopwords.txt, toxicwords.txt, and benignwords.txt. 

To test the program, you can also use normal.csv, normaltxt.txt, ngram.csv, ngramtxt.txt. To test larger files, you can use max.csv or maxtxt.txt, which are 7 to 8MB. To test for errors, you can use empty.csv, empty, or empty.txt to test empty files.