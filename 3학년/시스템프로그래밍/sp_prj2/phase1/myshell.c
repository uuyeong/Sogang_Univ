//PHASE1

#include "myshell.h"
#include<errno.h>

//명령어를 반복적으로 입력받고 처리
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

//입력된 명령어를 실행
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL) return; // 빈 입력 무시

    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) { // 자식 프로세스 생성
            if (execvp(argv[0], argv) < 0) {
                fprintf(stderr, "%s: Command not found.\n", argv[0]);                
                exit(0);
            }
        }
        // 부모 프로세스는 foreground 작업일 경우 기다림
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            fflush(stdout);
        } else { // background 실행인 경우 pid 출력
            printf("%d %s", pid, cmdline);
            printf("\n");
            fflush(stdout);
        }
    }
    return;
}

// 내부 명령어 처리 (cd, exit 등)
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "exit"))
        exit(0); // exit 명령어 처리

    if (!strcmp(argv[0], "cd")) { // cd 명령어 처리
        if (argv[1] == NULL) {
            fprintf(stderr, "cd: missing operand\n");
        } else {
            if (chdir(argv[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    }

    if (!strcmp(argv[0], "&")) // 단독 & 무시
        return 1;

    return 0;
}

// 인자를 분리하고 background 여부 확인 (명령어 파싱)
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc;
    int bg;

    buf[strlen(buf) - 1] = ' '; // 개행 문자를 공백으로 대체
    while (*buf && (*buf == ' ')) buf++; // 앞쪽 공백 제거
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) buf++;
    }
    argv[argc] = NULL;

    if (argc == 0) return 1;
    
    // 명령어 끝이 & 인 경우 background 처리
    if ((bg = (*argv[argc - 1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}


/* $end parseline */
