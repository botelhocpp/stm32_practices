#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "usart.h"

#define STDIN   (0)
#define STDOUT  (1)
#define STDERR  (2)

int _write(int file, char *ptr, int len) {
    if (file == STDOUT || file == STDERR) {
        for (int i = 0; i < len; i++) {
            usart_write_blocking(USART1, ptr[i]);
        }
        return len;
    }
    
    errno = EBADF;

    return -1;
}

extern char end;
extern char _heap_limit;

caddr_t _sbrk(int incr) {
    static char *heap_end = 0;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &end;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &_heap_limit) {
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _close(int file) { return -1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _read(int file, char *ptr, int len) { return 0; }

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) { return 1; }

void _init(void) { return; }

void _fini(void) { return; }
