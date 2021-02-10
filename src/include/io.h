#ifndef SCSS_IO_H
#define SCSS_IO_H

char* scss_read_file(const char* filename);
void scss_write_file(const char* filename, char* outbuffer);
char* sh(const char* cmd);
unsigned int file_exists(char* filepath);

#endif
