#define FAKE_TYPE_NULL 0
#define FAKE_TYPE_OFFSET 1
#define FAKE_TYPE_ABSOLUTE 2

#define FAKE_EPOCH_THRESHOLD 1300000000.0
#define USEC_MULTIPLIER 1000000
#define NSEC_MULTIPLIER 1000000000

#define FAKE_TIME_DOMESTIC_CONFIG ".faketime"
#define FAKE_TIME_GLOBAL_CONFIG "/tmp/.faketime" /* in regards of mod_perl/php */
#define FAKE_TIME_CONFIG_CACHE_LIFE 10
#define FAKE_TIME_CONFIG_STRING_LENGTH 32

#define FAKE_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

static struct {
    char type;
    struct timeval *offset;
    struct timeval *absolute;
} _fake_time;

static struct {
    time_t mtime;
    char *value;
} _config;

static time_t (*real_time)(time_t *);
static int (*real_gettimeofday)(struct timeval *, struct timezone *);

void _check_config_file();
void _read_config_file(char *, struct stat *);
void _parse_fake_time();
void _parse_relative_time(char *, char *);
void _parse_absolute_time(char *, char *);
char *_get_fake_time_string();
char *_get_fake_time_format();

#ifdef WARN
  #define warn(fmt, ...) fprintf(stderr, "[FT-WARN] " fmt "\n", ##__VA_ARGS__)
#else
  #define warn(...)
#endif

#ifdef INFO
  #define info(fmt, ...) printf("[FT-INFO] " fmt "\n", ##__VA_ARGS__)
#else
  #define info(...)
#endif

#ifdef DEBUG
  #define debug(fmt, ...) printf("[FT-DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
  #define debug(...)
#endif
