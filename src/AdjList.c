/*
*   File : AdjList.c
*  Author: Jacob Knorr
*
* Purpose: graph-related functions. Adds and searches for targets, edges,
* 		   and does a postorder traversal of the graph.
*/

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mymake.h"

node *head = NULL;
node *tail = NULL;
struct targets *top = NULL;

/*
* add_command() -- concatenates the piece of the command given (cmd) to the end of the
* targets command string, cmd.
*/
void add_command(char *tar, char *cmd) {
	node *n;
	char *tmp;
	int len, clen, tl;
	
	n = locate(tar);
	if (!n->t) {
		fprintf(stderr, "Error: Target not found.\n");
		exit(1);
	}

	if (n->cmd == NULL) {
		strcat(cmd, " ");
		n->cmd = strdup(cmd);
	}
	else {
		len = strlen(n->cmd);
		clen = strlen(cmd);
		tl = len + clen + 1;
		tmp = malloc(tl*sizeof(char));
		strcat(cmd, " ");
		strcat(tmp, cmd);
		strcat(n->cmd, tmp);
	}
}

/*
* valid_target() -- checks if command line target is a target in the makefile. Reports
* an error and exits program if target is not found.
*/
void valid_target(char *in_tar) {
	int count = 0;
	struct targets *ptr;
	for (ptr = top; ptr; ptr = ptr->next) {
		if (strcmp(ptr->tar_name, in_tar) == 0) {
			count++;
		}
	}
	if (count == 0) {
		fprintf(stderr, "Error: Specified target not found in makefile.\n");
		exit(1);
	}
}

/*
* file_targets() -- creates a list just for targets in the makefile. Also, checks for
* target duplicates, in which case an error is displayed and the program is exited.
*/
void file_targets(char *target) {
	struct targets *node;
	struct targets *p;

	node = malloc(sizeof(struct targets));
	if (node == NULL) {
		fprintf(stderr, "Error: Out of memory.\n");
		exit(1);
	}
	if (top != NULL) {
		for (p = top; p; p = p->next) {
			if (strcmp(p->tar_name, target) == 0) {
				fprintf(stderr, "Error: cannot have same target for more than one rule.\n");
				exit(1);
			}
		}
	}
	node->tar_name = strdup(target);
	node->next = top;
	top = node;
}

/*
* add_target() -- takes name and value (1 or 0) of target and adds it to the list
* of nodes. Returns the newly added node.
*/
node *add_target(char *tar, bool tVal) {
	node *n, *tail;

	n = malloc(sizeof(node));
	if (n == NULL) {
		fprintf(stderr, "Error: out of memory.\n");
		exit(1);
	}
	n->name = strdup(tar);
	n->t = tVal;
	n->visited = false;
	n->cmd = NULL;
	n->next = NULL;
	n->next_edge = NULL;

	if (head == NULL) {
		head = n;
	}
	else {
		tail = head;
		while (tail->next != NULL) {
			tail = tail->next;
		}
		tail->next = n;
	}
	return n;
}

/*
* add_edge() -- creates an edge with a pointer to the name of its
* target. Each edge is added to the end of the dependency list.
*/
void add_edge(node *p1, node *p2) {
	edge *e, *nEdge;
	nEdge = malloc(sizeof(edge));
	if (nEdge == NULL) {
		fprintf(stderr, "Error: Out of memory.\n");
		exit(1);
	}
	nEdge->dp_ptr = p2;
	nEdge->nxt = NULL;

	if (p1->next_edge == NULL) {
		p1->next_edge = nEdge;
	}
	else {
		for (e = p1->next_edge; e->nxt != NULL; e = e->nxt);
		e->nxt = nEdge;
	}
}

void p() {
	node *n;
	for (n = head; n != NULL; n = n->next) {
		printf("%s\n", n->cmd);
	}
}

/*
* findnode() -- searched the node list for the given name and
* returns the node if found, NULL otherwise.
*/
node *locate(char *t) {
  node *n;
  for (n = head; n != NULL; n = n->next) {
  	if (strcmp(n->name, t) == 0) {
  		return n;
  	}
  }
  return NULL;
}

/*
* find_edges() -- finds the targets' dependencies and adds the edge
* to the graph.
*/
void find_edges() {
	node *p1, *p2, *dep;
	for (p1 = head; p1->next != NULL; p1 = p1->next) {
		if (p1->t == true) {
			for (p2 = p1->next; p2->t != true; p2 = p2->next) {
				dep = locate(p2->name);
				if (dep == NULL) {
					dep = add_target(p2->name, false);
				}
				add_edge(p1, dep);
				if (p2->next == NULL) {
					break;
				}
			}
		}
	}
}

/*
* traverse() -- takes the target name (if valid) from input and
* begins a postorder traversal of the graph starting at that node.
*/
void traverse(node *t) {
	edge *e;
	t->visited = true;
	for (e = t->next_edge; e != NULL; e = e->nxt) {
		if (e->dp_ptr->visited == 0) {
			traverse(e->dp_ptr);
		}
	}
	if (t->t) {
		print_cmd(t);
	}

}

void print_cmd(node *n) {
	edge *e;
	int b;
	struct stat stat1, stat2;

	if (stat(n->name, &stat1) < 0)
		b = 1;

	for (e = n->next_edge; e != NULL; e = e->nxt) {
		if (stat(e->dp_ptr->name, &stat2) < 0) {
			fprintf(stderr, "Error: Missing command.\n");
			exit(1);
		}
		if (!b && stat1.st_mtime < stat2.st_mtime) {
			b = 1;
		}
	}
	if (b) {
		printf("%s\n", n->cmd);
		if (system(n->cmd)) {
			fprintf(stderr, "Error: Command did not execute.\n");
			exit(1);
		}
	}
	return;
}

/*
* free_mem() -- frees the program of any dynamically allocated memory (nodes and edges)
*/
void free_mem() {
	struct targets *t0, *t1;
    node *p0, *p1;
    edge *e0, *e1;

    t0 = top;
    while (t0 != NULL) {
    	t1 = t0->next;
    	free(t0->tar_name);
    	free(t0);
    	t0 = t1;
    }
    p0 = head;
    while (p0 != NULL) {
		p1 = p0->next;
    	free(p0->name);
        e0 = p0->next_edge;
        while (e0 != NULL) {
            e1 = e0->nxt;
            free(e0);
            e0 = e1;
        }
        free(p0);
        p0 = p1;
    }
}


