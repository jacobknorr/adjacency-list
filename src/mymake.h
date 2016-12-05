/*
* File : mymake.h
* Author : Jacob Knorr
* Purpose: header file -- struct/function definitions and prototypes
*/
#ifndef _MYMAKE_H_
#define _MYMAKE_H_
#include <stdio.h>
#include <stdbool.h>

typedef struct vert_node {
	char *name, *cmd;
	bool t;
	bool visited;
	struct vert_node *next;
	struct node_edges *next_edge;
} node;

typedef struct node_edges {
	struct vert_node *dp_ptr;
	struct node_edges *nxt;
} edge;

struct targets {
	char *tar_name;
	struct targets *next;
};


void process(FILE *, char *);
void identify_words(FILE *, char *);

void add_command(char *, char *);
void valid_targets(char *);
void file_targets(char *);
node *add_target(char *, bool);
void add_edge(node *, node *);
node *locate(char *);
void find_edges();
void traverse(node *);
void print_cmd(node *);
void free_mem();

#endif
