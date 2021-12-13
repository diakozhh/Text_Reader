#ifndef __TEXT_READ_H__
#define __TEXT_READ_H__

#include <stdio.h>
#include <stdlib.h>
/**
* Text structure
*/
typedef struct textRead textRead_t;
struct textRead {
  char* buf;                  ///< file chars in string
  unsigned int bufLng;       ///< buffer length
};

/**
* Text view initialization
* @param[out] textRead_t *tr is text view structure
* @param[in] const char* filename is file name for read
* @return 1 if inited, 0 if fail
*/
int TR_Init(textRead_t* tr, const char* filename);

/**
* Text view destroy
* @param[in] textRead_t *tr is text view to destroy
*/
void TR_Clear(textRead_t* tr);

#endif // __TEXT_READ_H__

