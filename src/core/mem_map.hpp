#ifndef MYMEM_MAP_H
#define MYMEM_MAP_H

#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

void handle_error(const char* msg) ;
const char* map_file(const char* fname, size_t& length);

#endif /* ifndef MYMEM_MAP_H */
