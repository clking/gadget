#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>

#include "faketime.h"


void
_init() {

    info("initializing %s", "faketime.so");
    real_time = dlsym(RTLD_NEXT, "time");
    real_gettimeofday = dlsym(RTLD_NEXT, "gettimeofday");

    _check_config_file((time_t)0);
    _parse_fake_time();
}

void
_check_config_file(time_t now) {
    char *path, *home;
    struct stat sb;

    if (now - _config.mtime <= FAKE_TIME_CONFIG_CACHE_LIFE) {
        debug("%ld vs. %ld still within config life, not reloading", now, _config.mtime);
        return;
    }
    debug("empty config or expired, reloading");
    _config.mtime = now;

    home = getenv("HOME");
    path = getenv("FAKE_TIME_FILE");
    if (!path)
        path = FAKE_TIME_GLOBAL_CONFIG;
    info("config file is %s", path);

    if (!stat(path, &sb)) {
        /* file exists */
        info("config file %s found", path);
        _read_config_file(path, &sb);
    } else {
        info("env or global config %s does not exist", path);

        path = (char *)malloc(BUFSIZ);
        sprintf(path, "%s/%s", home, FAKE_TIME_DOMESTIC_CONFIG);
        if (!stat(path, &sb)) {
            info("but found a domestic one %s", path);
            _read_config_file(path, &sb);
        } else {
            /* reset after config file gone */
            warn("config file %s does not exist, resetting", path);
            if (_config.value) {
                bzero(_config.value, sizeof(_config.value));
                _parse_fake_time();
            }
        }
    }
}

void
_read_config_file(char *path, struct stat *sb) {
    size_t buffer_size, value_size;
    FILE *fp;
    char *buf;

    debug("reading config file");

    buffer_size = 1 + sb->st_size;
    buf = (char *)malloc(buffer_size);
    bzero(buf, buffer_size);

    fp = fopen(path, "r");
    fread(buf, buffer_size, 1, fp);
    fclose(fp);

    if (!_config.value || (buf && strcmp(buf, _config.value))) {
        info("config changed, applying");
        if (!_config.value || sizeof(_config.value) < buffer_size) {
            debug("allocating %ld bytes", buffer_size);
            _config.value = (char *)malloc(buffer_size);
            bzero(_config.value, buffer_size);
        }
        debug("copying data: %d(%d) bytes to %d bytes", sizeof(buf), buffer_size, sizeof(_config.value));
        strcpy(_config.value, buf);
        _parse_fake_time();
        debug("config value is |%s|, eof? %d, ferror? %d", _config.value, feof(fp), ferror(fp));
    } else
        debug("config unchanged");
}

void
_parse_fake_time() {
    char *fake_time, *fake_time_format;

    fake_time = _get_fake_time_string();
    fake_time_format = _get_fake_time_format();

    if (fake_time) {
        switch (*fake_time) {
            case '@':
            case '+':
            case '-':
                debug("parsing %s relatively", fake_time);
                _parse_relative_time(fake_time, fake_time_format);
                break;

            default:
                debug("parsing %s absolutely", fake_time);
                _parse_absolute_time(fake_time, fake_time_format);
                break;
        }
    } else
        _fake_time.type = FAKE_TYPE_NULL;

    info("fake type %d, %s %ld", _fake_time.type, FAKE_TYPE_OFFSET == _fake_time.type ? "offset" : FAKE_TYPE_ABSOLUTE == _fake_time.type ? "absolute" : "null?", FAKE_TYPE_OFFSET == _fake_time.type ? _fake_time.offset->tv_sec : FAKE_TYPE_ABSOLUTE == _fake_time.type ? _fake_time.absolute->tv_sec : 0);
}

char *
_get_fake_time_string() {
    char *ft;

    ft = getenv("FAKE_TIME");
    info("fake time string from env |%s|", ft);
    if (!ft) {
        ft = _config.value;
        info("fake time string from config |%s|", ft);
    }

    return ft;
}

char *
_get_fake_time_format() {
    char *format;

    format = getenv("FAKE_TIME_FORMAT");
    if (!format)
        format = FAKE_TIME_FORMAT;

    return format;
}

void
_parse_relative_time(char *fake_time, char *fake_time_format) {
    int sign;
    double offset_f;
    char *endptr;
    char prefix;
    struct tm tp;
    struct timeval f;
    time_t now, offset;

    now = real_time(NULL);
    prefix = *fake_time;
    ++fake_time;

    _fake_time.type = FAKE_TYPE_OFFSET;
    if (!_fake_time.offset)
        _fake_time.offset = (struct timeval *)malloc(sizeof(struct timeval));

    info("fake time prefixed with %c, parsing", prefix);
    if ('@' == prefix) {
        debug("parsing start-at time %s formatted as %s", fake_time, fake_time_format);
        endptr = strptime(fake_time, fake_time_format, &tp);
        tp.tm_isdst = -1;
        debug("strptime stops at |%s|", endptr);
        if (endptr) {
            f.tv_sec = mktime(&tp);
            _fake_time.offset->tv_sec = (time_t)difftime(f.tv_sec, now);
            info("parsed time in epoch %ld now %ld diff %ld", f.tv_sec, now, _fake_time.offset->tv_sec);
        } else
            _fake_time.type = FAKE_TYPE_NULL;
    } else {
        sign = '-' == prefix ? -1 : 1;
        offset_f = strtod(fake_time, &endptr);
        offset = floor(offset_f);
        _fake_time.offset->tv_sec = sign * offset;
        _fake_time.offset->tv_usec = (suseconds_t)(USEC_MULTIPLIER * (offset_f - offset));
    }
}

void
_parse_absolute_time(char *fake_time, char *fake_time_format) {
    char *endptr;
    double epoch_f;
    int epoch;
    struct timeval f;
    struct timezone z;
    struct tm tp;

    debug("parsing absolute time %s formatted in %s", fake_time, fake_time_format);
    real_gettimeofday(&f, &z);
    bzero(&tp, sizeof(struct tm));
    endptr = strptime(fake_time, fake_time_format, &tp);
    if (endptr) {
        int f_sec;
        debug("parse successful at |%s|", endptr);
        tp.tm_isdst = -1;
        f_sec = mktime(&tp);
        if (f_sec > 0) {
            debug("and got a valid epoch time %d", f_sec);
            _fake_time.type = FAKE_TYPE_ABSOLUTE;
            f.tv_sec = f_sec;
            if (!_fake_time.absolute)
                _fake_time.absolute = (struct timeval *)malloc(sizeof(struct timeval));
            _fake_time.absolute->tv_sec = f.tv_sec;
            _fake_time.absolute->tv_usec = f.tv_usec;
        } else
            _fake_time.type = FAKE_TYPE_NULL;
    } else {
        debug("%s failed, should be an epoch", "strptime");
        epoch_f = strtod(fake_time, &endptr);
        debug("to be %f while remaining part is |%s|", epoch_f, (endptr ? endptr : ""));
        epoch = floor(epoch_f);
        debug("%f vs. %f", epoch_f, FAKE_EPOCH_THRESHOLD);
        if (epoch_f > FAKE_EPOCH_THRESHOLD) {
            info("%f considered to be a valid epoch time.", epoch_f);
            _fake_time.type = FAKE_TYPE_ABSOLUTE;
            f.tv_sec = epoch;
            f.tv_usec = (suseconds_t)(USEC_MULTIPLIER * (epoch_f - epoch));
            if (!_fake_time.absolute)
                _fake_time.absolute = (struct timeval *)malloc(sizeof(struct timeval));
            _fake_time.absolute->tv_sec = f.tv_sec;
            _fake_time.absolute->tv_usec = f.tv_usec;
        } else {
            info("%f is not big enough, make it offset.", epoch_f);
            _fake_time.type = FAKE_TYPE_OFFSET;
            f.tv_sec = epoch;
            f.tv_usec = (suseconds_t)(USEC_MULTIPLIER * (epoch_f - epoch));
            if (!_fake_time.offset)
                _fake_time.offset = (struct timeval *)malloc(sizeof(struct timeval));
            _fake_time.offset->tv_sec = f.tv_sec;
            _fake_time.offset->tv_usec = f.tv_usec;
            info("offset time set to %ld.%03ld", _fake_time.offset->tv_sec, _fake_time.offset->tv_usec);
        }
    }
}

time_t
_get_fake_time(time_t realistic_time) {
    _check_config_file(realistic_time);

    switch (_fake_time.type) {
        case FAKE_TYPE_OFFSET:
            debug("real time %ld + offset %ld = %ld", realistic_time, _fake_time.offset->tv_sec, _fake_time.offset->tv_sec + realistic_time);
            return _fake_time.offset->tv_sec + realistic_time;

        case FAKE_TYPE_ABSOLUTE:
            debug("real time %ld but absolute to %ld", realistic_time, _fake_time.absolute->tv_sec);
            return _fake_time.absolute->tv_sec;
    };

    return realistic_time;
}

struct timeval
_get_fake_timeval(struct timeval *realistic_timeval) {
    struct timeval f;

    _check_config_file(realistic_timeval->tv_sec);

    switch (_fake_time.type) {
        case FAKE_TYPE_OFFSET:
            f.tv_sec = _fake_time.offset->tv_sec + realistic_timeval->tv_sec;
            f.tv_usec = _fake_time.offset->tv_usec + realistic_timeval->tv_usec;
            return f;

        case FAKE_TYPE_ABSOLUTE:
            return *_fake_time.absolute;
    };

    return *realistic_timeval;
}

time_t
time(time_t *tloc) {
    time_t t, f;
    t = real_time(NULL);
    f = _get_fake_time(t);
    debug("real time is %ld, faken to %ld", t, f);

    if (tloc)
        *tloc = f;

    return f;
}

int
gettimeofday(struct timeval *tp, struct timezone *tzp) {
    int r = real_gettimeofday(tp, tzp);
    if (!r) {
        struct timeval fp;
        fp = _get_fake_timeval(tp);

        debug("got real timeval.tv_sec %ld.%03ld, faken to %ld.%03ld", tp->tv_sec, tp->tv_usec, fp.tv_sec, fp.tv_usec);
        tp->tv_sec = fp.tv_sec;
    }
    return 0;
}
