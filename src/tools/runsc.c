#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

extern int errno;

void print(){
  printf("called !!!");
}

int main(int ac, char* av[]){
        struct stat f_stats;
        int rv = stat(av[1], &f_stats);
        if(rv != 0){
                printf("ERROR [%d] on stat\n", rv);
                return 1;
        }
        int fd = open(av[1], O_RDONLY);

        // collect the filesize to allocate
        size_t file_size = f_stats.st_size;

        unsigned char *allocated_mem = mmap(
                NULL, 
                file_size, 
                7, MAP_PRIVATE | MAP_ANONYMOUS, 
                -1, 0);
        //allocated_mem[0] = 0xcc; // breakpoint
        rv = read(fd, allocated_mem, file_size);
        if (rv == 0){
                printf("ERROR on read\n");
                return 2;
        }
        rv = mprotect(allocated_mem, file_size, 7);
        if(rv != 0){
                printf("ERROR [%d] on mprotect\n", errno);
                perror("mprotect");
                return 3;
        }

        void (*fun_ptr)(void*) = allocated_mem;
        (*fun_ptr)(&print);
        return 0;
}
