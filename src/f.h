#include <stdint.h>

/* useful operating system interfaces */
extern int _write(int, const char[], int);
extern int _read(int, char*, int);
extern int _open(const char *pathname, int flags);
