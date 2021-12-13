#include "textRead.h"

#pragma warning(disable: 4996)

int TR_Init(textRead_t* tr, const char* filename) {
  FILE *F;

  if ((F = fopen(filename, "rt")) == NULL)
    return 0;

  fseek(F, 0, SEEK_END);
  int bufLen = ftell(F);
  fseek(F, 0, SEEK_SET);

  if ((tr->buf = (char *)malloc(sizeof(char) * bufLen + 1)) == NULL)
    return 0;

  tr->bufLng = fread(tr->buf, 1, bufLen, F);
  tr->buf[tr->bufLng] = 0;

  fclose(F);
  return 1;
}

void TR_Clear(textRead_t* tr) {
    if (tr->buf != NULL)
        free(tr->buf);
    tr->bufLng = 0;
}
