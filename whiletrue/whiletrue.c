#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG

#ifdef DEBUG
  #define debug(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
  #define debug(...)
#endif

extern char **environ;
pid_t child_pid;
time_t last_interrupted;

void
usage(char* process) {
    printf("Usage: %s program\n", process);
}

void
interrupted(int signal) {
    time_t now;

    if (child_pid)
        kill(child_pid, signal);

    now = time(NULL);

    if (!last_interrupted || now - last_interrupted > 1) {
        last_interrupted = now;
        return;
    }

    debug("interrupted");
    exit(EX_OK);
}

void
terminate_child(int signal) {
    debug("terminating child");
    if (child_pid)
        kill(child_pid, SIGINT);
}

void
display_time(time_t t, char *ts) {
    struct tm *tp;

    if (!t)
        t = time(NULL);
    tp = localtime(&t);

    strftime(ts, 512, "%F %T", tp);
}

void
on_exit() {
    debug("terminating child on exit");
    if (child_pid)
        kill(child_pid, SIGINT);
}

int
main(int argc, char **argv) {
    char ts[512];
    char **args;
    int child_status;

    if (argc < 2) {
        usage(argv[0]);
        return EX_DATAERR;
    }

    signal(SIGINT, interrupted);
    signal(SIGTERM, terminate_child);
    atexit(on_exit);

    args = (char **)malloc(argc - 1);
    for (int i = 1 ; i < argc ; ++i)
        args[i - 1] = argv[i];

    for (int i = 0 ; i < argc - 1 ; ++i)
        debug("%s", args[i]);

    while (1) {
        display_time(time(NULL), ts);
        printf("%s > %s\n", ts, args[0]);

        child_pid = fork();
        if (child_pid) {
            wait(&child_status);
            debug("child status %d", child_status);
        } else {
            execvp(args[0], args);
            debug("child never done");
        }

        child_pid = 0;
        debug("sleeping 1 sec.");
        sleep(1);
    }
    free(ts);

    return 0;
}
