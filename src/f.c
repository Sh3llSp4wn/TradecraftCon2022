#include "f.h"

static const char fname[] = "/etc/passwd";

// shellcode entry point
int _start(){
        int fd = _open(fname, 0);  // O_RDONLY

        // create a buffer for us to read and write from
        int bytes_read = 0;
        char buf[16];

        // read and then write the file 16 bytes at a time
        while((bytes_read = _read(fd, buf, 16)) != 0){
            _write(1, buf, bytes_read);
        }

        // leave
        //__asm__("int3");
        return 0;
}
