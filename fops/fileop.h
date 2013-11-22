#include<fcntl.h>
#include<stdio.h>

enum{
  Read,
  Write,
  ReadWrite,
  Create
};

int get_file_descriptor(char *fname, int mode);
char rdt_send(int fp);
int output_to(int file, char *buffer, int buf_len);
