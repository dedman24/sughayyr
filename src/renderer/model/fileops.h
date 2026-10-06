#ifndef MODEL_FILEOPS_H_INCLUDED
#define MODEL_FILEOPS_H_INCLUDED

// stdlib includes.
#include "stdio.h"        // FILE & file ops.

// POSIX includes.
#include "fcntl.h"        // open.
#include "unistd.h"       // close.
#include "sys/mman.h"     // mmap.
#include "sys/stat.h"     // fstat.

typedef struct{
  char* data;
  size_t size;
} model__fileT;

#define MODEL__FILE_NULL ((model__fileT){ NULL, 0 })

static model__fileT model_load__map(const char* const restrict path){
// use fstat to get file size.
// load file using mmap.
  const int fd = open(path, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "ERROR: could not open model file '%s'.\n", path);
    return MODEL__FILE_NULL;
  }

  struct stat st;
  if(fstat(fd, &st) < 0){
    close(fd);
    fprintf(stderr, "ERROR: could not fstat model file '%s'.\n", path);
    return MODEL__FILE_NULL;
  }

  char* const restrict m_file = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(!m_file){
    close(fd);
    fprintf(stderr, "ERROR: could not mmap model file '%s'.\n", path);
    return MODEL__FILE_NULL;
  }
  close(fd);

  return (model__fileT){ .data = m_file, .size = st.st_size };
}

static void model_load__unmap(const model__fileT data){
  munmap(data.data, data.size);
}

#endif
