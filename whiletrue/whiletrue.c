#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHELL "/bin/sh"

#ifdef DEBUG
  #define debug(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
  #define debug(...)
#endif

extern char **environ;
pid_t child_pid;
time_t last_interrupted;

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
    if (child_pid) {
        debug("terminating child %d", child_pid);
        kill(child_pid, signal);
    }
}

void
usage(char* process) {
    printf("Usage: %s program\n", process);
}

void
_on_exit() {
    debug("executing normal termination process");
    if (child_pid) {
        debug("terminating child on exit");
        kill(child_pid, SIGTERM);
    }
}

void
register_event_hooks() {
    signal(SIGINT, interrupted);
    signal(SIGHUP, terminate_child);
    atexit(_on_exit);
}

void
display_time(time_t t, char *ts, size_t len) {
    struct tm *tp;

    if (!t)
        t = time(NULL);
    tp = localtime(&t);

    strftime(ts, len, "%F %T", tp);
}

void
build_shell_command(char **args, int argc, char **argv) {
    args[0] = SHELL;
    args[1] = "-c";
    args[2] = (char *)malloc(BUFSIZ);
    args[3] = NULL;
    bzero(args[2], BUFSIZ);
    for (int i = 1 ; i < argc ; ++i) {
        args[2][strlen(args[2])] = ' ';
        strcat(args[2], argv[i]);
    }
    ++args[2];
}

int
main(int argc, char **argv) {
    char ts[32];
    char *args[4];
    int child_status;

    if (argc < 2) {
        usage(argv[0]);
        return EX_DATAERR;
    }

    register_event_hooks();

    build_shell_command(args, argc, argv);
    debug("running: %s %s %s", args[0], args[1], args[2]);

    for ( ; ; ) {
        display_time(time(NULL), ts, sizeof(ts));
        printf("%s > %s\n", ts, args[2]);

        child_pid = fork();
        if (child_pid) {
            wait(&child_status);
        } else {
            execvp(args[0], args);
            debug("child never done %d", errno);
            exit(errno); /* should never be reached */
        }

        child_pid = 0;
        debug("sleeping 1 sec.");
        sleep(1);
    }
    free(ts);

    return 0;
}
