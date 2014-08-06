#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define DATETIME_LENGTH_MAX 64

void
time_test() {
    time_t t;
    char *buf;
    struct tm *tp;

    t = time(NULL);
    tp = localtime(&t);
    buf = (char *)malloc(DATETIME_LENGTH_MAX);
    strftime(buf, DATETIME_LENGTH_MAX, "%Y-%m-%d %H:%M:%S", tp);
    printf("time() => %s(%ld) ", buf, t);
}

void
gettimeofday_test() {
    int r;
    char *buf;
    struct tm *tp;
    struct timeval tv;
    struct timezone tz;

    r = gettimeofday(&tv, &tz);
    tp = localtime(&tv.tv_sec);
    buf = (char *)malloc(DATETIME_LENGTH_MAX);
    strftime(buf, DATETIME_LENGTH_MAX, "%Y-%m-%d %H:%M:%S", tp);
    printf("gettimeofday() => %s.%ld(%ld.%ld)", buf, tv.tv_usec, tv.tv_sec, tv.tv_usec);
}

int
main(int argc, char** argv) {
    int i, times;

    times = argc > 1 ? (int)strtol(argv[1], NULL, 10) : 1;
    if (times > 1)
        printf("testing %d loops\n", times);

    for (i = 0 ; i < times ; ++i) {
        if (times > 1)
            printf("testing loop #%d ", 1 + i);

        time_test();
        gettimeofday_test();

        printf("\n");

        if (times > 1)
            sleep(1);
    }

    return 0;
}
