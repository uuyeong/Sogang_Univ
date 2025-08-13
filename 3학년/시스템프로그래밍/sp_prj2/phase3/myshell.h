//PHASE3

#ifndef MYSHELL_H
#define MYSHELL_H

#include "csapp.h"

#define MAXARGS   128
#define MAXCMDS 16 
#define MAXJOBS 128



void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int parse_pipeline(char *cmdline, char ****cmds, int *bg);
void execute_pipeline(char ***cmds, int ncmds, int bg, char *cmdline);
void free_pipeline(char ***cmds, int ncmds);
void clean_quotes(char **argv);


typedef struct {
    pid_t pid;
    char cmdline[MAXLINE];
    int job_id;
    int running; // 1: running, 0: stopped
    int done;
    int terminated;
    int is_background;
    int exit_code;
} Job;


extern Job jobs[MAXJOBS];
int add_job(pid_t pid, char *cmdline, int running, int is_background);
void delete_job(pid_t pid);
int get_job_id(pid_t pid);
void print_jobs();
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void do_bg(char **argv);
void do_fg(char **argv);
void do_kill(char **argv);




#endif
