//PHASE3

#include "myshell.h"
#include<errno.h>

// 전역 Job 목록과 인덱스 관리 변수
Job jobs[MAXJOBS];
int next_job_id = 1;
int fg_job_index = -1; // 포그라운드 Job 인덱스
int last_plus = -1; // 최근 Job(+)
int last_minus = -1; // 이전 Job(-)


// 시그널 등록: Ctrl+C, Ctrl+Z, 자식 종료
int main() 
{
    Signal(SIGCHLD, sigchld_handler);  // 자식 종료 처리
    Signal(SIGINT, sigint_handler);    // Ctrl+C
    Signal(SIGTSTP, sigtstp_handler);  // Ctrl+Z

    char cmdline[MAXLINE]; /* Command line */

    while (1) {
            printf("\x1b[32m> \x1b[0m");
            fflush(stdout);

            if (fgets(cmdline, MAXLINE, stdin) == NULL) exit(0);

            eval(cmdline);  

            for (int i = 0; i < MAXJOBS; i++) {
                if (jobs[i].pid > 0 && jobs[i].done && jobs[i].is_background) {
                    char mark = (i == last_plus) ? '+' : (i == last_minus ? '-' : ' ');
                    
                    char status_str[32];
                    if (jobs[i].terminated) {
                        strcpy(status_str, "Terminated");
                    } else if (jobs[i].exit_code != 0) {
                        sprintf(status_str, "Exit %d", jobs[i].exit_code); 
                    } else {
                        strcpy(status_str, "Done");
                    }

                    printf("[%d]%c  %-22s %s\n", jobs[i].job_id, mark, status_str, jobs[i].cmdline);
                    fflush(stdout);
                    jobs[i].pid = 0;
                }
            }

        }

}

// Job 리스트에 새 백그라운드 Job 추가
int add_job(pid_t pid, char *cmdline, int running, int is_background) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid;
            strcpy(jobs[i].cmdline, cmdline);
            jobs[i].job_id = next_job_id++;
            jobs[i].running = running;
            jobs[i].done = 0;
            jobs[i].terminated = 0;
            jobs[i].is_background = is_background;
            last_minus = last_plus;
            last_plus = i;
            return i;
        }
    }
    return -1;
}



// Job 종료 처리 (done 상태로)
void delete_job(pid_t pid) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            jobs[i].done = 1;
            jobs[i].running = 0;
            break;
        }
    }
}



int get_job_id(pid_t pid) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) return jobs[i].job_id;
    }
    return -1;
}


// Job 리스트 출력 (jobs 명령어)
void print_jobs() {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == 0) continue;

        // 포그라운드로 실행된 job은 아예 제외
        if (!jobs[i].running && jobs[i].done) {
            // background에서 완료된 job이면 한번 출력 후 삭제
            char mark = (i == last_plus) ? '+' : (i == last_minus ? '-' : ' ');
            const char *status = jobs[i].terminated ? "Terminated" : "Done";
            printf("[%d]%c  %-22s %s\n", jobs[i].job_id, mark, status, jobs[i].cmdline);
            fflush(stdout);
            jobs[i].pid = 0;
        }
        else if (!jobs[i].done && (jobs[i].running == 1 || jobs[i].running == 0)) {
            // 아직 실행 중이거나 Stopped 상태인 background job
            char mark = (i == last_plus) ? '+' : (i == last_minus ? '-' : ' ');
            const char *status = jobs[i].running ? "Running" : "Stopped";
            printf("[%d]%c  %-22s %s\n", jobs[i].job_id, mark, status, jobs[i].cmdline);
            fflush(stdout);
        }
    }
}


void clean_quotes(char **argv) {
    for (int i = 0; argv[i] != NULL; i++) {
        char *arg = argv[i];
        int len = strlen(arg);

        if (len >= 2 && arg[0] == '"' && arg[len - 1] == '"') {
            arg[len - 1] = '\0';      // 끝 " 제거
            for (int j = 0; j < len - 1; j++) {
                arg[j] = arg[j + 1];
            }
        }
    }
}

// 자식 종료/중단 시 상태 반영
void sigchld_handler(int sig) {
    int old_errno = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        for (int i = 0; i < MAXJOBS; i++) {
            if (jobs[i].pid != pid) continue;

            char mark = (i == last_plus) ? '+' : (i == last_minus ? '-' : ' ');

            if (WIFEXITED(status)) {
                jobs[i].done = 1;
                jobs[i].terminated = 0;
                int exit_status = WEXITSTATUS(status);
                jobs[i].exit_code = exit_status;
            }
            else if (WIFSIGNALED(status)) {
                jobs[i].done = 1;
                jobs[i].terminated = 1;
            }
            else if (WIFSTOPPED(status)) {
                jobs[i].running = 0;
                printf("\n[%d]%c  Stopped                 %s\n",
                    jobs[i].job_id, mark, jobs[i].cmdline);
            }

            fflush(stdout);
            break;
        }
    }

    errno = old_errno;
}

// Ctrl+C 입력 시 포그라운드 Job에 시그널 전달
void sigint_handler(int sig) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid > 0 && jobs[i].running && !jobs[i].done && !jobs[i].terminated) {
            kill(-jobs[i].pid, SIGINT);  // 프로세스 그룹 전체에 보내기
            break;
        }
    }
    write(STDOUT_FILENO, "\n", 1);
}

// Ctrl+Z 입력 시 포그라운드 Job 일시정지
void sigtstp_handler(int sig) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid > 0 && jobs[i].running && !jobs[i].done && !jobs[i].terminated) {
            kill(-jobs[i].pid, SIGTSTP);
            break;
        }
    }
}


// fg 명령어: Job을 포그라운드에서 실행
void do_fg(char **argv) {
    if (!argv[1]) {
        printf("fg: job id required\n");
        return;
    }
    int jid = atoi(argv[1]);
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].job_id == jid && jobs[i].pid != 0) {
            jobs[i].running = 1;
            jobs[i].is_background = 0;     
            fg_job_index = i;
            printf("%s\n", jobs[i].cmdline);
            fflush(stdout);
            kill(-jobs[i].pid, SIGCONT);   // 재개
            sigset_t mask, prev;
            Sigemptyset(&mask);
            Sigaddset(&mask, SIGCHLD);
            Sigprocmask(SIG_BLOCK, &mask, &prev);

            while (1) {
                if (jobs[fg_job_index].done || !jobs[fg_job_index].running)
                    break;
                Sigsuspend(&prev);
            }

            Sigprocmask(SIG_SETMASK, &prev, NULL);
            fg_job_index = -1;
            return;
        }
    }
    printf("fg: %d: no such job\n", jid);
}

// bg 명령어: 정지된 Job을 백그라운드에서 재개
void do_bg(char **argv) {
    if (!argv[1]) {
        printf("bg: job id required\n");
        return;
    }
    int jid = atoi(argv[1]);
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].job_id == jid && jobs[i].pid != 0) {
            if (jobs[i].running) {
                printf("bg: job %d already in background\n", jid);
                return;
            }
            jobs[i].running = 1;
            jobs[i].is_background = 1;
            kill(-jobs[i].pid, SIGCONT);
            printf("[%d]+ %s &\n", jobs[i].job_id, jobs[i].cmdline);
            fflush(stdout);
            return;
        }
    }
    printf("bg: %d: no such job\n", jid);
}

// kill 명령어: Job 종료
void do_kill(char **argv) {
    if (!argv[1]) {
        printf("kill: usage: killd [job id required] \n"); return;
    }
    int jid;
    if (argv[1][0] == '%') {
        jid = atoi(&argv[1][1]);
        for (int i = 0; i < MAXJOBS; i++) {
            if (jobs[i].job_id == jid && jobs[i].pid != 0) {
                kill(jobs[i].pid, SIGKILL);
                return;
            }
        }
        printf("kill: %s: no such job\n", argv[1]);
    } else {
        // Assume it's a PID — behave like bash
        pid_t pid = atoi(argv[1]);
        if (kill(pid, SIGKILL) < 0) {
            perror("kill");
        }
    }
}


// 파이프라인 명령어 파싱 시 '&' 백그라운드 여부도 함께 처리
int parse_pipeline(char *cmdline, char ****cmds, int *bg) {
    char *cmds_buf[MAXCMDS];
    int ncmds = 0;
    *bg = 0;

    cmdline[strcspn(cmdline, "\n")] = '\0';

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

    // 백그라운드 여부 확인
    char *amp = strrchr(cmdline, '&');
    if (amp && *(amp + 1) == '\0') {
        *bg = 1;
        *amp = '\0';
    }

    // 파이프 단위로 나누기
    while (token != NULL && ncmds < MAXCMDS) {
        cmds_buf[ncmds++] = strdup(token);
        token = strtok(NULL, "|");
    }

    // 명령어들을 파싱하여 argv 리스트로 만들기
    *cmds = malloc(sizeof(char **) * ncmds);
    for (int i = 0; i < ncmds; i++) {
        char *buf = cmds_buf[i];
        while (*buf == ' ') buf++;

        char **argv = malloc(sizeof(char *) * MAXARGS);
        int argc = 0;

        char *p = buf;
        while (*p) {
            while (isspace(*p)) p++;
            if (*p == '\0') break;

            char *start;
            int quoted = 0;

            if (*p == '"') {
                quoted = 1;
                start = ++p;
                while (*p && *p != '"') p++;
            } else {
                start = p;
                while (*p && !isspace(*p)) p++;
            }

            int len = p - start;
            char *arg = malloc(len + 1);
            strncpy(arg, start, len);
            arg[len] = '\0';

            argv[argc++] = arg;

            if (quoted && *p == '"') p++;
            if (!quoted && *p) p++;
        }
        argv[argc] = NULL;
        (*cmds)[i] = argv;
        free(cmds_buf[i]);
    }

    return ncmds;
}


// 명령어 파이프라인 실행 + 백그라운드 처리
void execute_pipeline(char ***cmds, int ncmds, int bg, char *cmdline) {
    int i, pipefd[2], prevfd = -1;
    pid_t pid;
    pid_t last_pid = -1;

    for (i = 0; i < ncmds; i++) {
        if (i < ncmds - 1) pipe(pipefd);

        if ((pid = fork()) == 0) {
            // 자식 프로세스
            setpgid(0, 0); // 프로세스 그룹 설정

            // 파이프 입력
            if (i > 0) {
                dup2(prevfd, STDIN_FILENO);
                close(prevfd);
            }

            // 파이프 출력
            if (i < ncmds - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // exec
            if (execvp(cmds[i][0], cmds[i]) < 0) {
                fprintf(stderr, "%s: Command not found\n", cmds[i][0]);
                exit(1);
            }
        }

        // 부모 프로세스
        if (i > 0) close(prevfd);
        if (i < ncmds - 1) {
            close(pipefd[1]);
            prevfd = pipefd[0];
        }

        last_pid = pid;
    }

    if (!bg) {
        for (i = 0; i < ncmds; i++) wait(NULL);
    } else {
        cmdline[strcspn(cmdline, "\n")] = '\0';
        add_job(last_pid, cmdline, 1, 1);        
        printf("[%d] %d\n", get_job_id(last_pid), last_pid);
        fflush(stdout);
    }
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

// 명령어 평가 eval 함수
void eval(char *cmdline) {
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    
    // 파이프 포함 여부 확인
    if (strchr(cmdline, '|')) {
        char ***cmds;
        int is_bg;
        int ncmds = parse_pipeline(cmdline, &cmds, &is_bg);
        execute_pipeline(cmds, ncmds, is_bg, cmdline);
        free_pipeline(cmds, ncmds);
        return;
    }

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL) return;
    // 내장 명령어 처리
    if (!strcmp(argv[0], "jobs")) { print_jobs(); return; }
    if (!strcmp(argv[0], "fg")) { do_fg(argv); return; }
    if (!strcmp(argv[0], "bg")) { do_bg(argv); return; }
    if (!strcmp(argv[0], "kill")) { do_kill(argv); return; }

    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) {
            setpgid(0, 0);  // 자식은 새로운 프로세스 그룹으로 묶음
            if (execvp(argv[0], argv) < 0) {
                fprintf(stderr, "%s: Command not found\n", argv[0]);
                exit(1);
            }
        }

        cmdline[strcspn(cmdline, "\n")] = '\0';
        char cleaned[MAXLINE];
        strcpy(cleaned, cmdline);
        int len = strlen(cleaned);
        if (len > 0 && cleaned[len - 1] == '&') {
            cleaned[len - 1] = '\0';
            while (len > 1 && cleaned[len - 2] == ' ') {
                cleaned[len - 2] = '\0';
                len--;
            }
        }
        // 일반 명령어 실행
        if (!bg) {
            // 포그라운드 작업으로 등록
            fg_job_index = add_job(pid, cleaned, 1, 0);
            sigset_t mask, prev;
            Sigemptyset(&mask);
            Sigaddset(&mask, SIGCHLD);
            Sigprocmask(SIG_BLOCK, &mask, &prev);

            while (1) {
                if (jobs[fg_job_index].done || !jobs[fg_job_index].running)
                    break;
                Sigsuspend(&prev);
            }

            Sigprocmask(SIG_SETMASK, &prev, NULL);
            fg_job_index = -1;
        } else {
            int job_index = add_job(pid, cleaned, 1, 1);
            printf("[%d] %d\n", jobs[job_index].job_id, pid);
            fflush(stdout);
        }
    }
}




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
    if (!strcmp(argv[0], "&")) return 1;
    if (!strcmp(argv[0], "jobs")) {
        print_jobs();
        return 1;
    }

    if (!strcmp(argv[0], "fg")) {
        do_fg(argv);
        return 1;
    }

    if (!strcmp(argv[0], "bg")) {
        do_bg(argv);
        return 1;
    }

    if (!strcmp(argv[0], "kill")) {
        do_kill(argv);
        return 1;
    }


    return 0;
}


// 명령어 인자 분리 및 '&' 확인
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc = 0;
    int bg = 0;

    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' ')) buf++;

    while ((delim = strchr(buf, ' '))) {
        *delim = '\0';
        if (*buf != '\0') {
            int len = strlen(buf);
            if (buf[len - 1] == '&' && len > 1) {
                buf[len - 1] = '\0';
                argv[argc++] = buf;
                argv[argc++] = "&";
            } else {
                argv[argc++] = buf;
            }
        }
        buf = delim + 1;
        while (*buf && (*buf == ' ')) buf++;
    }
    argv[argc] = NULL;

    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
        bg = 1;
        argv[--argc] = NULL;
    }

    return bg;
}

/* $end parseline */
