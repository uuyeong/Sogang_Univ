//PHASE1

#ifndef MYSHELL_H
#define MYSHELL_H

#include "csapp.h"

#define MAXARGS   128

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

#endif
