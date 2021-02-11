#include "include/io.h"
#include "include/scss.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  init_scss();
  const char *filepath = argv[1];
  char *contents = scss_read_file(filepath);
  char *out = scss(contents, filepath);

  if (contents)
    free(contents);

  if (out) {
    printf("%s\n", out);

    free(out);
  }

  return 0;
}
