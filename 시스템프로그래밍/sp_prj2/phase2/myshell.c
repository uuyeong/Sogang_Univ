//PHASE2

#include "myshell.h"
#include<errno.h>

int main() 
{
    char cmdline[MAXLINE];

    while (1) {
	printf("> ");
    fflush(stdout);
          
	fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);
        
        eval(cmdline);
    }
}

// 따옴표 제거
void clean_quotes(char **argv) {
    for (int i = 0; argv[i] != NULL; i++) {
        char *arg = argv[i];
        int len = strlen(arg);

        // 쌍따옴표로 감싸져 있으면, 문자열 내에서 직접 제거
        if (len >= 2 && arg[0] == '"' && arg[len - 1] == '"') {
            arg[len - 1] = '\0';
            for (int j = 0; j < len - 1; j++) {
                arg[j] = arg[j + 1];]
            }
        }
    }
}


// 명령어 라인을 파이프 기준으로 나눠서 각 명령어의 argv 배열을 만들어줌
int parse_pipeline(char *cmdline, char ***cmds[]) {
    char *cmds_buf[MAXCMDS];
    int ncmds = 0;

    cmdline[strcspn(cmdline, "\n")] = '\0';

    // 파이프만 입력된 경우 방지
    if (cmdline[0] == '|') {
        fprintf(stderr, "syntax error near unexpected token `|'\n");
        *cmds = NULL;
        return -1;
    }

    // 파이프 단위로 나눔
    char *token = strtok(cmdline, "|");
    while (token != NULL && ncmds < MAXCMDS) {
        while (*token == ' ') token++;
        if (*token == '\0') {
            fprintf(stderr, "syntax error near unexpected token `|'\n");
            *cmds = NULL;
            return -1;
        }

        cmds_buf[ncmds++] = strdup(token);
        token = strtok(NULL, "|");
    }

    if (cmdline[strlen(cmdline) - 1] == '|') {
        fprintf(stderr, "syntax error: missing command after `|'\n");
        *cmds = NULL;
        return -1;
    }

    // 명령어를 공백 기준으로 나눠서 argv로 분리
    *cmds = malloc(sizeof(char **) * ncmds);
    for (int i = 0; i < ncmds; i++) {
        char *buf = cmds_buf[i];
        while (*buf == ' ') buf++;

        char **argv = malloc(sizeof(char *) * MAXARGS);
        int argc = 0;
        char *arg = strtok(buf, " ");
        while (arg != NULL) {
            argv[argc++] = strdup(arg);
            arg = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        clean_quotes(argv);
        (*cmds)[i] = argv;

        free(cmds_buf[i]);
    }

    return ncmds;
}


// 명령어 파이프라인 실행
void execute_pipeline(char ***cmds, int ncmds) {
    int i;
    int pipefd[2], prevfd = -1;
    pid_t pid;

    for (i = 0; i < ncmds; i++) {
        if (i < ncmds - 1)
            pipe(pipefd);

        if ((pid = fork()) == 0) {
            // 입력 리디렉션
            if (i > 0) {
                dup2(prevfd, STDIN_FILENO);
                close(prevfd);
            }
            // 출력 리디렉션
            if (i < ncmds - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            if (execvp(cmds[i][0], cmds[i]) < 0) {
                fprintf(stderr, "%s: Command not found.\n", cmds[i][0]);
                exit(1);
            }
        }

        if (i > 0) close(prevfd);
        if (i < ncmds - 1) {
            close(pipefd[1]);
            prevfd = pipefd[0];
        }
    }
    for (i = 0; i < ncmds; i++) wait(NULL);
}

// 명령어 배열 해제
void free_pipeline(char ***cmds, int ncmds) {
    for (int i = 0; i < ncmds; i++) {
        char **argv = cmds[i];
        for (int j = 0; argv[j] != NULL; j++)
            free(argv[j]);
        free(argv);
    }
    free(cmds);
}

//파이프 명령인지 확인 후 실행
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    
    
    // 파이프가 포함된 명령어 처리
    if (strchr(cmdline, '|')) {
        char ***cmds;
        int ncmds = parse_pipeline(cmdline, &cmds);
        if (ncmds == -1) return; 
        execute_pipeline(cmds, ncmds);
        free_pipeline(cmds, ncmds);
        return;
    }

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    clean_quotes(argv);
    if (argv[0] == NULL) return;

    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) {
            if (execvp(argv[0], argv) < 0) {
                fprintf(stderr, "%s: Command not found.\n", argv[0]);
                exit(1);
            }
        }

        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            fflush(stdout);
        } else {
            printf("%d %s", pid, cmdline);
            //printf("\n");
            fflush(stdout);
        }
    }
}


// 내부 명령어 처리
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "exit"))
        exit(0);

    if (!strcmp(argv[0], "cd")) {
        if (argv[1] == NULL)
            fprintf(stderr, "cd: missing operand\n");
        else if (chdir(argv[1]) != 0)
            perror("cd");
        return 1;
    }

    if (!strcmp(argv[0], "&"))
        return 1;

    return 0;
}


// 입력 명령어 파싱
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc = 0;
    int bg;

    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' ')) buf++;

    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) buf++;
    }
    argv[argc] = NULL;

    if (argc == 0) return 1;

    if ((bg = (*argv[argc - 1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}

/* $end parseline */
