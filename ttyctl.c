#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static char const * TTY_ERROR = "Cannot open TTY ";
static char const * TTY_PREFIX = "/dev/";

void strapnd(char ** ptr, char const * str) {
    strcpy(*ptr, str);
    *ptr += strlen(str);
    **ptr = 0;
}

int cmd_blank(int tty) {
    char cmd[] = {14};
    return ioctl(tty, TIOCLINUX, cmd);
}

int cmd_wake(int tty) {
    char cmd[] = {4};
    return ioctl(tty, TIOCLINUX, cmd);
}

typedef int (*cmd_t)(int);

int open_and_run(char const * tty, cmd_t cmd) {
    size_t size = strlen(TTY_ERROR) + strlen(tty) + 1;
    if (!strchr(tty, '/')) {
        size += strlen(TTY_PREFIX);
    }

    char * buffer = calloc(size, sizeof(char));
    if (buffer == NULL) {
        perror("Cannot allocate memory");
        return 1;
    }

    char * cur = buffer;
    strapnd(&cur, TTY_ERROR);
    if (strchr(tty, '/')) {
        strapnd(&cur, tty);
    } else {
        strapnd(&cur, TTY_PREFIX);
        strapnd(&cur, tty);
    }

    int fd = open(buffer + strlen(TTY_ERROR), O_RDWR);
    if (fd == -1) {
        perror(buffer);
        free(buffer);
        return 1;
    }

    free(buffer);
    return cmd(fd);
}

struct {
    char const * cmd_string;
    cmd_t cmd;
    char const * cmd_help;
} COMMANDS[] = {
    { "blank", cmd_blank, "Blanks the TTY screen" },
    { "wake",  cmd_wake,  "Wakes up the TTY screen"  }
};

int usage(char const * cmd) {
    printf("Usage: %s command tty\n\nWhere command is one of:\n", cmd);

    for (int i = 0; i < sizeof(COMMANDS) / sizeof(*COMMANDS); ++i) {
        printf("\t% 5s %s\n", COMMANDS[i].cmd_string, COMMANDS[i].cmd_help);
    }

    return 0;
}

int main(int argc, char const ** argv) {
    if (argc != 3) {
        return 1 + usage(*argv);
    }

    for (int i = 0; i < sizeof(COMMANDS) / sizeof(*COMMANDS); ++i) {
        if (strcmp(COMMANDS[i].cmd_string, argv[1]) == 0) {
            return open_and_run(argv[2], COMMANDS[i].cmd);
        }
    }

    fprintf(stderr, "Unknown command %s\n", argv[1]);
    return 1 + usage(*argv);
}
