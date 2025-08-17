//PHASE2

#ifndef MYSHELL_H
#define MYSHELL_H

#include "csapp.h"

#define MAXARGS   128
#define MAXCMDS 16 


void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int parse_pipeline(char *cmdline, char ***cmds[]);
void execute_pipeline(char ***cmds, int ncmds);
void free_pipeline(char ***cmds, int ncmds);
void clean_quotes(char **argv);

#endif
