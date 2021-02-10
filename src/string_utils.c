#include "include/string_utils.h"
#include <stdlib.h>
#include <string.h>

char *str_append(char **source, const char *piece) {
  char *src = *source;

  if (!piece)
    return src;

  if (!src) {
    src = calloc((strlen(piece) + 1), sizeof(char));
  } else {
    src = realloc(src, (strlen(src) + strlen(piece) + 1) * sizeof(char));
  }

  strcat(src, piece);

  return src;
}
