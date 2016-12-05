
/*
*    File: mymake.c
*  Author: Jacob Knorr
*
* Purpose: Make a dependency graph via adjacency list of the contents of a 
*		   makefile. Two command line arguments specify a makefile and target,
*		   for which a dependency graph will be created to organize the file 
*		   names and their repective dependencies. The contents of the graph
*		   will be printed to stdout using a postorder traversal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mymake.h"

void valid_target(char *);
void p();

/*
* process() -- reads and checks the specified makefile for formatting errors. Error message and program
* exit occur when error is found. Also, identifies targets in makefiles and calls function to add them
* to linked list of targets.
*/
void process(FILE *fp, char *filename) {
	int status;
	char word[65], target[65], cmd[65], *ptr;

	while (fscanf(fp, "%s", word) != EOF) {
		for (ptr = word; *ptr != '\0'; ptr++) {
			if (!isgraph(*ptr)) {
				fprintf(stderr, "Error: Non-graphical character found in file content.\n");
				exit(1);
			}
		}
		if (*word == '@') {
			if (strcmp(word, "@target") == 0) {
				status = fscanf(fp, "%s", target);
				if (status == EOF) {
					fprintf(stderr, "Error: improper format for '@target'\n");
					exit(1);
				} else {
					file_targets(target);
					fscanf(fp, "%s", word);   //checking for ':'
					if (strcmp(word, ":") != 0) {
						fprintf(stderr, "Error: improper format for ':' after @target.\n");
						exit(1);
					}
				}
			}
			else if (strcmp(word, "@cmd") == 0) {
				if ((status = fscanf(fp, "%s", cmd)) == EOF) {
					fprintf(stderr, "Error: no argument(s) for '@cmd'\n");
					exit(1);
				}
				else if (strcmp(cmd, "@target") == 0) {
					fprintf(stderr, "Error: no argument(s) for '@cmd'\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: '@' cannot be first character of word.\n");
				exit(1);
			}
		}
	}
}

/*
* identify_words() -- parses the input into words seperated as targets
* and dependencies. Adds word to list with target value of true or false.
*/
void identify_words(FILE *fp, char *filename) {
	char word[65], tar[65];
	rewind(fp);
	while (fscanf(fp, "%s", word) != EOF) {
		if (strcmp(word, "@target") == 0) {
			fscanf(fp, "%s", word);
			strcpy(tar, word);
			add_target(word, true);
			do {
				fscanf(fp, "%s", word);
				if (strcmp(word, "@cmd") == 0) {
					break;
				}
				else if (strcmp(word, ":") == 0) {
					continue;
				}
				else {
					add_target(word, false);
				}
			}
			while (strcmp(word, "@cmd") != 0);
		}
		else {
			if (*word == '@') {
				fprintf(stderr, "Error: cmd arguments cannot begin with '@'.\n");
				exit(1);
			}
			add_command(tar, word);
		}
	}
}

/*
* main() -- acts as driver function; reads in command line arguments, calls functions
* for further processing. Exit status of 0 if no program srs occured.
*/
int main (int argc, char *argv[]) {
	node *t;
	char *filename = NULL;
	char *input_target = NULL;
	FILE *fp;

	if (argc != 3) {
		fprintf(stderr, "Error: Input file or target not specified.\n");
		exit(1);
	}
	filename = argv[1]; 
	input_target = argv[2];

	if ((fp = fopen(filename, "r")) == NULL) {
		printf("Error: input file could not be opened.\n");			
		exit(1);
	}

	process(fp, filename);
	valid_target(input_target);
	identify_words(fp, filename);
	fclose(fp);
	find_edges();
	//p();
	t = locate(input_target);
	traverse(t);
	free_mem();

	return 0;
}

